#include <stdio.h>
#include <string.h>
#include <math.h>
#include <getopt.h>
#include <stdlib.h>
#include <signal.h>
#include <stdint.h>
#include <wchar.h>

#include "awl_data.h"

#include "awl.h"
#include "awlfile.h"
#include "awl_distance.h"

#include "convert_raw.h"

/*! \mainpage awld, the AWL daemon
 *
 * \section intro_sec Introduction
 *
 * The \a awld daemon is the user-space process which is responsible
 * for:
 * \li receiving data from the \a awl.ko kernel module
 * \li receiving commands from the CAN bus
 * \li compute distance of obstacles in channels
 * \li send messages over the CAN bus
 *
 * The \a awld daemon can also optionnally:
 * \li receive data from a GPIB device
 * \li receive commands from UDP networking
 * \li send raw frames over UDP networking
 * \li record raw frames to a file
 * \li playback raw frames from a file
 *
 */

typedef struct obstacle_index {
	int index;
	uint16_t track_distance;
} awl_obstacle_index;

/* ================================================================ */

int obstacle_compare(const void* a, const void* b)
{
	return ((((awl_obstacle_index*)a)->track_distance<((awl_obstacle_index*)b)->track_distance)) ? -1 : (
	((awl_obstacle_index*)a)->track_distance > ((awl_obstacle_index*)b)->track_distance ? 1 : 0);
}

/* ================================================================ */

int process_obstacles(awl_data *awl)
{
	int i, j;
	awl_obstacle_index index[awl->maxObstacles];
	int num_obstacle_per_channel[AWL_MAX_CHANNELS];

	memset(num_obstacle_per_channel, 0, sizeof(int) * AWL_MAX_CHANNELS);

	for (i = 0, j = 0; i < awl->maxObstacles; i++){
		if (awl->obstacles[i].track_id) {
			num_obstacle_per_channel[awl->obstacles[i].channel] ++;
			if (!(awl->can_transmit_cooked_event_limit 
			    && num_obstacle_per_channel[awl->obstacles[i].channel] > awl->can_transmit_cooked_event_limit)) {
				//printf("OBS %d %d %d\n", awl->obstacles[i].channel, awl->obstacles[i].track_id, awl->obstacles[i].track_distance); 
				index[j].index = i;
				index[j].track_distance = awl->obstacles[i].track_distance;
				j++;
			}
		}
	}
	qsort(awl->obstacles, j, sizeof(awl_obstacle),
	      obstacle_compare);
	if (j) {
		// printf("OBS %d obstacles: ", j);
		for (i = 0; i < j; i++){
			send_obstacle(awl, &(awl->obstacles[index[i].index]));
			/*
			printf("%d %d %d %d, ", index[i].index,
			       awl->obstacles[index[i].index].track_id,
			       awl->obstacles[index[i].index].track_distance,
			       awl->obstacles[index[i].index].track_speed);
			*/
		}
		// printf("\n");
	}
	send_frame_done(awl);
	return 0;
}

/* ================================================================ */

int main_job(awl_data *awl)
{
	uint8_t ch;
	uint16_t irq_top_page_counter;
	uint16_t irq_bottom_page_counter;
	uint32_t ts;
	static uint32_t last_ts = 0;

	ch = GET_CURRENT_AWL_CHANNEL;
	irq_top_page_counter = GET_CURRENT_AWL_TAIL_IRQ_TOP_PAGE_COUNTER;
	irq_bottom_page_counter = GET_CURRENT_AWL_TAIL_IRQ_BOTTOM_PAGE_COUNTER;
	ts = CURRENT_AWL_TIMESTAMP;

	if (!awl->calibration_mode) {

		if (awl->process_channel_mask & (1<<ch)) {
			analyze_it(awl);
		}
		//printf("TS %d %d %d %d\n", ch, ts, last_ts, awl->last_ts[ch]);
		if (last_ts != ts) {
			process_obstacles(awl);
			last_ts = ts;
		}
		if (awl->udp_transmit_raw_channel_mask & (1<<ch)) {
			send_raw_frame(awl);
		}
		if (!awl->heartbeat && !awl->first_ts[ch] && ts && ts != awl->last_ts[ch]) {
			if (ts && 
			    ts != awl->last_ts[ch] + 1) {
				/*printf("TS timestamp jumped from %d to %d\n",
				       awl->last_ts[ch], 
				       ts);*/
			}

			if (irq_top_page_counter && 
			    irq_top_page_counter != awl->last_irq_top_page_counters[ch] + 1) {
				/*printf("IRQ top page counter jumped from %d to %d\n",
				       awl->last_irq_top_page_counters[ch], 
				       irq_top_page_counter);*/
			}

			if (irq_bottom_page_counter && 
			    irq_bottom_page_counter != awl->last_irq_bottom_page_counters[ch] + 1) {
				/*printf("IRQ bottom page counter jumped from %d to %d\n",
			    	       awl->last_irq_bottom_page_counters[ch], 
			    	       irq_bottom_page_counter);*/
			}
		}
	}
/*
	awl->record_channel_mask = 1<<ch;
	awl->record_num_channels = 1;
*/
	if (awl->record_channel_mask & (1<<ch)) {
		addto_raw_record(awl);
	}

	awl->last_irq_top_page_counters[ch] = irq_top_page_counter;
	awl->last_irq_bottom_page_counters[ch] = irq_bottom_page_counter;
	awl->last_ts[ch] = ts;
	awl->last_st[ch] = CURRENT_AWL_STATUS;
	awl->last_rcv[ch] = GET_CURRENT_AWL_TAIL_CHANNEL_RECEIVE_COUNTER;
	awl->last_rcn[ch] = GET_CURRENT_AWL_TAIL_FILE_RECORD_COUNTER;
	awl->first_ts[ch] = 0;

// calibration will move from distance to main job, use awl.auto_calib_counter % awl.auto_calib
// dont't forget to check for non-zero awl.auto_calib
/*
        awl->auto_calib_counter++;
	//awl->auto_calib=500;
	if (awl->auto_calib) {
		if (!(awl->auto_calib_counter % awl->auto_calib)) {
			calibrate( awl, awl->default_beta );
			save_calibration_data(awl);
			//printf("calib %d %d..\n ", awl->auto_calib_counter, awl->auto_calib);
			//getchar();
		}
	}
*/
	if (awl->calibration_mode && (awl->calibration_count[ch] < awl->calibration_max[ch])) {
		printf ("CAL calibrating channel %d, %d < %d\n",
			ch, awl->calibration_count[ch], awl->calibration_max[ch]);
		awl->calibration_count[ch] ++;
		switch (awl->calibration_mode) {
		case CALIBRATION_MODE_START:
			addto_raw_record(awl);
			break;
		case CALIBRATION_MODE_LIVE:
			update_calibration(awl);
		}
	}

	return 0;
}

/* ================================================================ */

void awl_client_help(awl_data *awl)
{
	printf(" h              Print this help\n");
	printf(" r MASK         Start recording\n");
	printf(" p MASK         playback\n");
	printf(" z MASK         calibration\n");
	printf(" Z MASK         Clear calibration\n");
	printf(" d              Dump debug information\n");
	printf(" v MODE         Set verbose mode\n");
	printf(" f              Force refresh\n");
	printf(" R ADDR=VALUE   Set AWL register value\n");
	printf(" A ADDR=VALUE   Set ADC register value\n");
	printf(" l ALGO         Select algo\n");
	printf(" L PARAM=FVALUE Set algo parameter\n");
	printf(" t AlGO         Select tracking algo\n");
	printf(" T PARAM=FVALUE Set tracking algo parameter\n");
}

/* ================================================================ */

void awl_client(awl_data *awl)
{
	printf("ALWD client mode\n");
	switch(awl->client_command)
	{
	case 'h':
		awl_client_help(awl);
		return;
	case 'r':
		send_record(awl);
		return;
	case 'p':
		send_playback(awl);
		return;
	case 'z':
		send_record_calibration(awl);
		break;
	case 'Z':
		send_clear_calibration(awl);
		break;
	case 'd':
		send_debug_dump(awl);
		break;
	case 'v':
		send_debug_verbose(awl);
		break;
	case 'f':
		send_debug_force_refresh(awl);
		break;
	case 'R':
		awl->client_ptype = PTYPE_AWL_REGISTER;
		break;
	case 'A':
		awl->client_ptype = PTYPE_ADC_REGISTER;
		break;
	case 'l':
		awl->client_ptype = PTYPE_ALGO_SELECTED;
		break;
	case 'L':
		awl->client_ptype = PTYPE_ALGO_PARAMETER;
		break;
	case 't':
		awl->client_ptype = PTYPE_TRACK_ALGO_SELECTED;
		break;
	case 'T':
		awl->client_ptype = PTYPE_TRACK_ALGO_PARAMETER;
		break;
	default:
		printf ("AWLD unknown client command '%c' (0x%02x)\n", awl->client_command, awl->client_command);
		return;
	}
	send_set_parameter(awl);
	send_query_parameter(awl);
}

/* ================================================================ */

void awl_sa_handler(int signal)
{
	static int record_mask = 0;
	printf("AWLD caught signal 0x%02x\n", signal);
	switch(signal) {
	case SIGINT:
        	control_global_recording(0);
        	exit(0);
		break;
	case SIGUSR1:
		if (record_mask) record_mask = 0;
		else record_mask = 0xffff;
		control_global_recording(record_mask);
		break;
	}
}

/* ================================================================ */


int heartbeat(awl_data *awl)
{
	static uint32_t ts = 0;
	//int k=0;
	int ret = 0;
	int channel;
	int break_channel;
	int calib = 0;

	process_file(awl);
//printf ("apcm %02X\n", awl->playback_channel_mask);
	if (awl->playback_channel_mask) return 0;

	if (!awl->heartbeat) return 0;
	//if (awl->calibration_mode) return 0;

	ts ++;
	break_channel = 16;

	for (channel = 0; channel < awl->num_channels; channel++) {
		//printf("%x, %x, %x\n", awl->process_channel_mask, (1<<channel), awl->process_channel_mask & (1<<channel));
		if (1) {
			unsigned int i, d;
			float f;
/* CAVEAT EMPTOR: there is no direct relation between
 *                the channel and the current frame,
 *                but since one should select a frame
 *                to put the data in, might as well
 *                use the channel number as an index
 */
			awl->current_frame = channel;
			CURRENT_AWL_CMD = CMD_RAW_0;
			CURRENT_AWL_TIMESTAMP = ts;
			CURRENT_AWL_STATUS = 0x00013100;
			SET_CURRENT_AWL_CHANNEL(channel);
			if (channel >= 0 && channel < awl->num_channels) {
				//printf("map %d %d\n", channel, awl->mapped_channels[channel]);
				SET_CURRENT_AWL_CHANNEL(awl->mapped_channels[channel]);
			}
			//printf("ch %d\n", GET_CURRENT_AWL_CHANNEL);
			CURRENT_AWL_PAYLOAD_32[2] = 0x7fffffff;
			CURRENT_AWL_PAYLOAD_32[3] = 0x80000000;
			switch (awl->heartbeat) {
			default:
				break;
			case 1:
				// Moving pattern
				//break_channel = 4;
				memset(CURRENT_AWL_PAYLOAD+4*sizeof(uint32_t), 0, AWL_PAYLOAD_SIZE_CURRENT-4*sizeof(uint32_t));
				d = (ts+(channel * 10) )%300;
//			d = (ts)%300;
				f = d;
				for (i = 0; i < 16; i++) {
					*(CURRENT_AWL_FRAME_S32 + 20 + i + d) = (i << 27) * (1 - (f/250));
					*(CURRENT_AWL_FRAME_S32 + 40 + i + d) = (i << 26) * (1 - (f/250));
				}
				for (i = 16; i < 32; i++) {
					*(CURRENT_AWL_FRAME_S32 + 20 + i + d) = ((32 - i) << 27) * (1 - (f/250));
					*(CURRENT_AWL_FRAME_S32 + 40 + i + d) = ((32 - i) << 26) * (1 - (f/250));
				}
				break;
			case 3:
				break_channel = 3;
				// NO BREAK HERE
			case 2:
				wmemset((wchar_t *)CURRENT_AWL_PAYLOAD_S32+4, (channel + 1) * 0x10000, AWL_PAYLOAD_SIZE_S32-4);
				break;
			case 6:
				calib = 1;
				// NO BREAK HERE
			case 4:
				// Still standing pattern
				memset(CURRENT_AWL_PAYLOAD+4*sizeof(uint32_t), 0, AWL_PAYLOAD_SIZE_CURRENT-4*sizeof(uint32_t));
				d = ((channel * 10) )%300;
//			d = (ts)%300;
				f = d;
				for (i = 0; i < 16; i++) {
					*(CURRENT_AWL_FRAME_S32 + 140 + i + d) = ((i << 27) * (1 - (f/250)))/100;
					*(CURRENT_AWL_FRAME_S32 + 160 + i + d) = ((i << 26) * (1 - (f/250)))/100;
				}
				for (i = 16; i < 32; i++) {
					*(CURRENT_AWL_FRAME_S32 + 140 + i + d) = (((32 - i) << 27) * (1 - (f/250)))/100;
					*(CURRENT_AWL_FRAME_S32 + 160 + i + d) = (((32 - i) << 26) * (1 - (f/250)))/100;
				}
				break;
			case 5:
				memcpy(CURRENT_AWL_PAYLOAD+4*sizeof(uint32_t),
					CALIBRATION_DATA(channel)+4*sizeof(uint32_t),
					AWL_PAYLOAD_SIZE_CURRENT-4*sizeof(uint32_t));
				break;
			}

			if (calib) calibrate(awl, -.01);
	
			if (channel > break_channel) break;
			main_job(awl);
				
			//PDEBUG("%d %d\n", channel, ts);
		}
	}
	//PDEBUG("%d\r", ret);
	//PDEBUG("%d\r", k);
	return ret;
}

/* ================================================================ */

int main_loop(awl_data *awl)
	
{
	static int count = 0;
	struct sigaction sa;
	struct timeval tv;
	int ret;
	float period = 0.0;
	long sec, usec;

	memset(&sa, 0, sizeof(struct sigaction));
	sa.sa_handler = awl_sa_handler;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGUSR1, &sa, NULL);

	if (awl->frequency != 0.0) {
		period = 1.0/awl->frequency;
	}
	sec = roundf(period);
	usec = (period - roundf(period)) * 1000000;
	printf("ALWD timeout frequency %f Hz\n", awl->frequency);
	PDEBUG("AWLD T %f s\n", period);
	PDEBUG("AWLD tv: %ld s %ld us\n", sec, usec);

	if (!awl->client_mode && !awl->calibration_mode) {
//		send_sensor_boot(awl);
//		send_sensor_monitor_status(awl);
	}

	while(1) {
		fd_set rfds;
		int nfds = 0;

		FD_ZERO(&rfds);
		if (awl->udp_fd >= 0) {
			if (awl->udp_fd > nfds) nfds = awl->udp_fd;
			FD_SET(awl->udp_fd, &rfds);
		}
		if (awl->can_fd >= 0) {
			if (awl->can_fd > nfds) nfds = awl->can_fd;
			FD_SET(awl->can_fd, &rfds);
		}
		if ((!awl->client_mode) && awl->awl_fd >= 0) {
			if (awl->awl_fd > nfds) nfds = awl->awl_fd;
			FD_SET(awl->awl_fd, &rfds);
		}
		nfds ++;
		tv.tv_sec = sec;
		tv.tv_usec = usec;

		ret = select(nfds, &rfds, NULL, NULL, &tv);
		if (ret == -1) {
			//break;
		} else if (ret) {
			if (awl->udp_fd > 0 && FD_ISSET(awl->udp_fd, &rfds)) {
				if ((ret = process_udp(awl))) {
					//return ret;
				}
			}
			if (awl->can_fd > 0 && FD_ISSET(awl->can_fd, &rfds)) {
				if ((ret = process_can(awl))) {
					//return ret;
				}
			}
			if (awl->awl_fd > 0 && FD_ISSET(awl->awl_fd, &rfds)) {
				if ((ret = process_awl(awl))) {
					//return ret;
				}
			}
		} else {
			if (awl->gpib_fd > 0) {
				process_gpib(awl);
			} else ret = heartbeat(awl);
		}
		if (awl->client_mode) {
			count ++;
			if (count > (awl->frequency * 2)) break;
		}
		if (awl->calibration_mode) {
			int i;
			int finished = 1;
			for (i = 0; i < awl->num_channels; i++) {
				if (awl->process_channel_mask & (1<<i)) {
					if (awl->calibration_count[i] < awl->calibration_max[i]) {
						finished = 0;
					}
				}
			}
			if (finished) {
				switch (awl->calibration_mode) {
				case CALIBRATION_MODE_START:
					return finish_calibration(awl);
					break;
				case CALIBRATION_MODE_LIVE:
					save_calibration_data(awl);
					awl->calibration_mode = CALIBRATION_MODE_NONE;
					awl->record_channel_mask = 0;
				}
			}
		}
	}
	return ret;
}

/* ================================================================ */

int usage(awl_data *awl, char *argv0)
{
	printf("awld, compiled " __DATE__ ", " __TIME__ "\n");
	printf("Usage %s\n", argv0);
	printf("      -a AWL-INTERFACE-NAME\n");
	printf("      -c CAN-INTERFACE-NAME\n");
	printf("      -C h | d | v VERBOSE-MASK | f | [(r|p)CHANNEL-MASK] | [(A|R|L)ADDRESS=VALUE] | [lALGO]\n");
	printf("      -d DESTINATION-UDP-PORT\n");
	printf("      -D DESTINATION-IP-ADDRESS\n");
	printf("      -f CONFIG-FILENAME\n");
	printf("      -u (dumps default config)\n");
	printf("      -y FREQUENCY-IN-HERTZ\n");
	printf("      -l ALGORITHM-NUMBER\n");
	printf("      -g ALGO-PARAMETER-NUMBER\n");
	printf("      -o ALGO-PARMETER-VALUE\n");
	printf("      -G GPIB-DEVICE-NAME\n");
	printf("      -H HEARTBEAT-TYPE\n");
	printf("      -S SERVER-IP_ADDRESS\n");
	printf("      -s SERVER-UDP-PORT\n");
	printf("      -m AWL-CHANNEL_MASK\n");
	printf("      -r RECORD-CHANNEL-MASK\n");
	printf("      -p PLAYBACK-FILENAME\n");
	printf("      -n PLAYBACK-LOOP-LIMIT\n");
	printf("      -M CHANNEL-MAP\n");
	printf("      -v [ucagdp]*\n");
	printf("      -w WINDOW-WIDTH\n");
	printf("      -W WINDOW-QTY\n");
	printf("      -x NUMBER-OF-SAMPLES\n");
	printf("      -e DEADZONE-OVERRIDE\n"); 
	printf("      -z [CALIBRATION-COUNT] (calibration file defaults to %s)\n", awl->calibration_filename);
	printf("      -Z ALTERNATE-CALIBRATION_FILE\n");
	printf("use -v to verbose UDP, CAN, GPIB, AWL, DISTANCE and/or PLAYBACK\n");
	printf("use -H to send fake data on heartbeat\n");
	printf("use -C to send a command to a running %s (client mode)\n", argv0);
	printf("use -Ch to get help on client mode\n");
	printf("use -z to record a new calibration file (calibration mode)\n");
	return 0;

}

/* ================================================================ */

int main(int argc, char *argv[])
{
	int ret;
	awl_data awl;
	int c;
	int i, l;
	char *options = "uhH:a:c:f:S:s:D:d:v:l:g:o:G:m:r:p:M:C:n:z::Z:y:w:W:x:e:";

	memset(&awl, 0, sizeof(awl_data));
	strncpy(awl.config_filename, AWL_CONFIG_FILENAME, PATH_MAX);

	while ((c = getopt (argc, argv, options)) != -1) {
		switch(c) {
		case 'f':
			strncpy(awl.config_filename, optarg, PATH_MAX);
			break;
		case 'u':
			awl.config_dump = 1;
		}
	}

	init_awl_data(&awl);

	if (awl.config_dump) return 1;

	optind = 1;
	while ((c = getopt (argc, argv, options)) != -1) {
		int arg1, arg2;
		float argf;
		//PDEBUG("%c %d %s ", c, c, optarg);
		switch (c) {
		case 'f':
		case 'u':
			break;
		case 'a':
			strncpy(awl.awl_ifname, optarg, PATH_MAX);
			//PDEBUG("a %s\n", awl.awl_ifname);
			break;
		case 'c':
			strncpy(awl.can_ifname, optarg, PATH_MAX);
			//PDEBUG("c %f Hz\n", awl.can_ifname);
			break;
		case 'G':
			strncpy(awl.gpib_devname, optarg, PATH_MAX);
			//PDEBUG("D %s\n", awl.gpib_devname);
			break;
		case 'y':
			awl.frequency = atof(optarg);
			//PDEBUG("f %f Hz\n", awl.frequency);
			break;
		case 'S':
			strncpy(awl.srv_saddr, optarg, SADDR_SIZE);
			//PDEBUG("S %s\n", awl.srv_saddr);
			break;
		case 's':
			awl.srv_port = atoi(optarg);
			//PDEBUG("s %d\n", awl.srv_port);
			break;
		case 'D':
			strncpy(awl.cli_saddr, optarg, SADDR_SIZE);
			//PDEBUG("D %s\n", awl.cli_saddr);
			break;
		case 'd':
			awl.cli_port = atoi(optarg);
			//PDEBUG("d %d\n", awl.cli_port);
			break;
		case 'l':
			i = atoi(optarg);
			if ( i >= 0 && i < awl.num_algos) {
				awl.current_algo = i;
			} else {
				printf ("AWLD algo %d does not exists\n", i);
				exit (1);
			}
			//PDEBUG("l %d\n", awl.algo);
			break;
		case 'g':
			sscanf(optarg, "%i", &arg1);
			awl.client_address = arg1;
			break;
		case 'o':
			sscanf(optarg, "%f", &argf);
			if (ALGO_HAS_PARAM_X(awl.client_address)) {
				ALGO_PARAM_X(awl.client_address) = argf;
			} else {
				printf ("AWLD algo %d does not have parameter %d\n", awl.current_algo, awl.client_address);
				exit (1);
			}
			break;
		case 'v':
			for(i = 0; i < strlen(optarg); i++) {
				if(optarg[i] == 'U') awl.verbose_udp = 0;
				if(optarg[i] == 'C') awl.verbose_can = 0;
				if(optarg[i] == 'A') awl.verbose_awl = 0;
				if(optarg[i] == 'G') awl.verbose_gpib = 0;
				if(optarg[i] == 'D') awl.verbose_distance = 0;
				if(optarg[i] == 'P') awl.verbose_playback = 0;
				if(optarg[i] == 'u') awl.verbose_udp = 1;
				if(optarg[i] == 'c') awl.verbose_can = 1;
				if(optarg[i] == 'a') awl.verbose_awl = 1;
				if(optarg[i] == 'g') awl.verbose_gpib = 1;
				if(optarg[i] == 'd') awl.verbose_distance = 1;
				if(optarg[i] == 'p') awl.verbose_playback = 1;
			}
			break;
		case 'm':
			sscanf(optarg, "%i", &arg1);
			awl.process_channel_mask = arg1;
			//PDEBUG("m %d\n", awl.channel_mask);
			break;
		case 'r':
			sscanf(optarg, "%i", &arg1);
			awl.record_channel_mask = arg1;
			//awl.record_channel_mask = 0x7F;
			start_recording(&awl);
			//PDEBUG("r %d\n", awl.record_channel_mask);
			break;
		case 'n':
			awl.playback_loop_max = atoi(optarg);
			break;
		case 'p':
			// KEEP after 'P'
			awl.playback_channel_mask = 0xffff;
			strncpy(awl.playback_filename, optarg, PATH_MAX);
			strncpy(awl.awl_ifname, "", PATH_MAX);
			break;
		case 'H':
			awl.heartbeat = atoi(optarg);
			break;
		case 'M':
			l = strlen(optarg);
			for (i = 0; i < l && i < awl.num_channels; i++) {
				if (   (optarg[i] >= '0' && optarg[i] <= '9')
				    || (optarg[i] >= 'A' && optarg[i] <= 'F')
				    || (optarg[i] >= 'a' && optarg[i] <= 'f')) {
					if (optarg[i] >= '0' && optarg[i] <= '9') {
						awl.mapped_channels[i] = optarg[i] - '0';
					}
					if (optarg[i] >= 'A' && optarg[i] <= 'F') {
						awl.mapped_channels[i] = optarg[i] - 'A' + 10;
					}
					if (optarg[i] >= 'a' && optarg[i] <= 'f') {
						awl.mapped_channels[i] = optarg[i] - 'a' + 10;
					}
					printf ("AWLD mapping physical channel %d to logical %d\n", i, awl.mapped_channels[i]);
				}
			}
			break;
		case 'C':
			awl.client_mode = 1;
			awl.srv_port = 0;
			awl.client_command = optarg[0];
			switch(awl.client_command) {
			case 'r':
			case 'p':
			case 'z':
			case 'Z':
			case 'l':
			case 't':
			case 'v':
				sscanf(optarg+1, "%i", &arg1);
				awl.client_address = arg1;
				awl.client_value = arg1;
				break;
			case 'R':
			case 'A':
				sscanf(optarg+1, "%i=%i", &arg1, &arg2);
				awl.client_address = arg1;
				awl.client_value = arg2;
				break;
			case 'L':
			case 'T':
				sscanf(optarg+1, "%i=%f", &arg1, &argf);
				awl.client_address = arg1;
				awl.client_value = *((uint32_t*)(&argf));
				break;
			}
			if (!strcmp(awl.cli_saddr, "0.0.0.0")) {
				strcpy(awl.cli_saddr, "127.0.0.1");
			}
			if (!awl.cli_port) awl.cli_port = AWLD_DEFAULT_UDP_PORT;
			break;
		case 'z':
			awl.process_channel_mask = 0xffff;
			awl.calibration_mode = CALIBRATION_MODE_START;
			if (optarg) {
				arg1 = atoi(optarg);
				for (i = 0; i < awl.num_channels; i++) {
					awl.calibration_max[i] = arg1;
				}
			}
			break;
		case 'Z':
			strncpy(awl.calibration_filename, optarg, PATH_MAX);
			break;
		case 'w':
			awl.window_width = atoi(optarg);
			break;
		case 'W':
			awl.window_qty = atoi(optarg);
			break;
		case 'x':
			arg1 = atoi(optarg);
			if (arg1 > 0 && arg1 <= AWL_PAYLOAD_SIZE_MAX) awl.payload_size = arg1 * sizeof(int32_t);
			break;
		case 'e':
			arg1 = atoi(optarg);
			awl.dead_zone_override = arg1;
			break;
		case 'h':
			// keep this one above default without break
		default:
			return usage(&awl, argv[0]);
		}
	}

	//PDEBUG("Init\n");

	if (awl.calibration_mode) {
		start_recording(&awl);
	} else {
		read_calibration(&awl);
	}

	ret = init_udp(&awl);
	if (ret) awl.bist_ok = 0;

	ret = init_can(&awl);
	if (ret) awl.bist_ok = 0;

	if (awl.client_mode) {
		awl_client(&awl);
	} else {
		ret = init_awl(&awl);
		if (ret) awl.receiver_error = 0xff;
		ret = init_gpib(&awl);
	}
	
	ret = main_loop(&awl);

	return ret;
}

/* ================================================================ */

