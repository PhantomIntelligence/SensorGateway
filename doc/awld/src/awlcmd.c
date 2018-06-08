#include <stdio.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>

#include "awl_data.h"
#include "awldev.h"
#include "awlfile.h"
#include "awlcmd.h"
#include "awlmsg.h"
#include "awludp.h"
#include "awlcan.h"

/* ================================================================ */

int process_transmit_raw(awl_data *awl, uint8_t *buf, size_t size)
{
	uint16_t channel_mask;
	uint8_t frame_rate;
	uint16_t payload_size;

	channel_mask = *((uint16_t *)(buf+1));
	frame_rate   = buf[3];
	payload_size = *((uint16_t *)(buf+4)) * sizeof(int32_t);
//printf("fr %d\n", awl->frame_rate);
	if (frame_rate) {
		if (frame_rate != awl->frame_rate) {
			int q;
			q = awl->frame_rate / frame_rate;
			frame_rate = awl->frame_rate / q;
		}
		if (frame_rate > awl->frame_rate) {
			printf ("CMD invalid raw frame rate %d\n", frame_rate);
			awl->udp_transmit_raw_frame_rate = awl->frame_rate;
		} else {
			awl->udp_transmit_raw_frame_rate = frame_rate;
		}
	} else {
		awl->udp_transmit_raw_frame_rate = awl->frame_rate;
	}
	printf ("CMD raw frame rate set to %d\n", awl->udp_transmit_raw_frame_rate);

	awl->udp_transmit_raw_channel_mask = channel_mask;
	printf("CMD channel mask is now 0x%x\n", awl->udp_transmit_raw_channel_mask);

	if (payload_size && payload_size <= awl->payload_size) {
		awl->raw_payload_size = payload_size;
	} else {
		awl->raw_payload_size = awl->payload_size;
	}
	set_udp_payload_size(awl);
	printf ("CMD raw payload size set to %zd samples\n", awl->raw_payload_size / sizeof(uint32_t));

	memcpy (&(awl->stream_cli_addr), &(awl->cli_addr), sizeof(awl->stream_cli_addr));
	if (channel_mask) {
		printf ("\nCMD serving raw to %s port %d\n", inet_ntoa(awl->cli_addr.sin_addr), ntohs(awl->cli_addr.sin_port));
	} else {
		printf ("\nCMD stopped serving raw\n");
	}

	return 0;
}

/* ================================================================ */

int process_transmit_cooked(awl_data *awl, uint32_t context, uint8_t *buf, size_t size)
{
	uint16_t channel_mask;
	uint8_t frame_rate;
	uint8_t event_mask;
	uint8_t event_limit;

	channel_mask = *((uint16_t*)(buf+1));
	frame_rate   = buf[3];
	event_mask   = buf[4];
	event_limit  = buf[6];

	if (frame_rate) {
		if (frame_rate != awl->frame_rate) {
			int q;
			q = awl->frame_rate / frame_rate;
			if (q) {
				frame_rate = awl->frame_rate / q;
			}
		}
		if (frame_rate > awl->frame_rate) {
			printf ("CMD invalid cooked frame rate %d\n", frame_rate);
			frame_rate = awl->frame_rate;
		}
	} else {
		frame_rate = awl->frame_rate;
	}
	printf ("\nCMD stopped serving cooked\n");

	switch (context) {
	default:

		awl->can_transmit_cooked_frame_rate = frame_rate;
		awl->can_transmit_cooked_channel_mask = channel_mask;
		awl->can_transmit_cooked_event_mask = event_mask;
		awl->can_transmit_cooked_event_limit = event_limit;
		printf("CMD CAN cooked frame rate set to %d\n",     awl->can_transmit_cooked_frame_rate);
		printf("CMD CAN cooked channel mask is now 0x%x\n", awl->can_transmit_cooked_channel_mask);
		printf("CMD CAN cooked  event  mask is now 0x%x\n", awl->can_transmit_cooked_event_mask);
		printf("CMD CAN cooked  event limit is now 0x%x\n", awl->can_transmit_cooked_event_limit);
		if (channel_mask && event_mask) {
			printf ("\nCMD serving cooked to CAN\n");
		} else {
			printf ("\nCMD stopped serving cooked to CAN\n");
		}
		break;

	case MSG_FROM_UDP:
		memcpy (&(awl->stream_cli_addr), &(awl->cli_addr), sizeof(awl->stream_cli_addr));

		awl->udp_transmit_cooked_frame_rate = frame_rate;
		awl->udp_transmit_cooked_channel_mask = channel_mask;
		awl->udp_transmit_cooked_event_mask = event_mask;
		awl->udp_transmit_cooked_event_limit = event_limit;
		printf("CMD UDP cooked frame rate set to %d\n",     awl->udp_transmit_cooked_frame_rate);
		printf("CMD UDP cooked channel mask is now 0x%x\n", awl->udp_transmit_cooked_channel_mask);
		printf("CMD UDP cooked  event  mask is now 0x%x\n", awl->udp_transmit_cooked_event_mask);
		printf("CMD UDP cooked  event limit is now 0x%x\n", awl->udp_transmit_cooked_event_limit);
		if (channel_mask && event_mask) {
			printf ("\nCMD UDP serving cooked to %s port %d\n", inet_ntoa(awl->cli_addr.sin_addr), ntohs(awl->cli_addr.sin_port));
		} else {
			printf ("\nCMD UDP stop serving cooked to %s port %d\n", inet_ntoa(awl->cli_addr.sin_addr), ntohs(awl->cli_addr.sin_port));
		}
		break;
	}

	return 0;
}

/* ================================================================ */

int process_recording(awl_data *awl, uint8_t *buf, size_t size)
{
	uint16_t channel_mask;
	uint16_t frame_rate;
	uint16_t payload_size;
	int i;

	channel_mask = *((uint16_t*)(buf+1));
	frame_rate   = buf[3];
	payload_size = *((uint16_t *)(buf+4)) * sizeof(int32_t);
	if (frame_rate) {
		if (frame_rate != awl->frame_rate) {
			int q;
			q = awl->frame_rate / frame_rate;
			frame_rate = awl->frame_rate / q;
		}
		if (frame_rate > awl->frame_rate) {
			printf ("CMD invalid record frame rate %d\n", frame_rate);
			awl->record_frame_rate = awl->frame_rate;
		} else {
			awl->record_frame_rate = frame_rate;
		}
	} else {
		awl->record_frame_rate = awl->frame_rate;
	}
	printf ("CMD record frame rate set to %d\n", awl->record_frame_rate);

	awl->record_channel_mask = channel_mask;
	awl->record_num_channels = 0;
	for (i = 0; i < 16; i++) {
		if (awl->record_channel_mask & 1 << i) {	
			awl->record_num_channels ++;
		}
	}

	if (payload_size && payload_size <= awl->payload_size) {
		awl->record_payload_size = payload_size;
	} else {
		awl->record_payload_size = awl->payload_size;
	}
	printf ("CMD record payload size set to %zd samples\n", awl->record_payload_size / sizeof(uint32_t));

	if (channel_mask) {
		start_recording(awl);
		printf ("CMD recording to %s (0x%02x)\n", awl->record_filename,
		        awl->record_channel_mask);

	} else {
		stop_recording(awl);
		printf ("CMD stopped recording\n");
		//printf ("stopped recording to %s\n", awl->record_filename);
	}
	
	return 0;
}

/* ================================================================ */

int process_playback(awl_data *awl, uint8_t *buf, size_t size)
{
	uint16_t channel_mask;

	channel_mask = *((uint16_t*)(buf+1));

	awl->playback_channel_mask = channel_mask;
	if (channel_mask) {
		start_playback(awl);
		printf ("CMD playback of %s (0x%02x)\n", awl->playback_filename,
		        awl->playback_channel_mask);

	} else {
		stop_playback(awl);
		printf ("CMD stopped playback of %s\n", awl->playback_filename);
	}
	
	return 0;
}

/* ================================================================ */

int process_record_calibration(awl_data *awl, uint8_t *buf, size_t size)
{
	uint16_t channel_mask;
	int16_t num_frames;
	uint16_t i;

	if (awl->playback_channel_mask) {
		printf("CMD can't calibrate in playback mode\n");
		return -1;
	}

	if (awl->record_channel_mask) {
		printf("CMD can't calibrate in record mode\n");
		return -1;
	}

	channel_mask = buf[1];
	if (channel_mask == 0xff) channel_mask = 0xffff;
	num_frames   = buf[2];
	if (!num_frames) {
		printf ("CMD invalid number of frames %d\n", num_frames);
		return -1;
	}

	stop_recording(awl);

	awl->record_channel_mask = channel_mask;
	for (i = 0; i < 16; i++) {
		if (channel_mask & 1 << i) {	
			awl->calibration_max[i] = num_frames;
			awl->calibration_count[i] = 0;
			awl->calibration_beta[i] = *((float*)(buf+4));
		}
	}

	if (channel_mask) {
		awl->calibration_mode = CALIBRATION_MODE_LIVE;
	} else {
		awl->calibration_mode = CALIBRATION_MODE_NONE;
	}
	awl->record_payload_size = awl->payload_size;
	
	return 0;
}

/* ================================================================ */

int process_clear_calibration(awl_data *awl, uint8_t *buf, size_t size)
{
	uint16_t channel_mask;
       	uint8_t	flags;
	int ret;

	// JUST ONE BYTE FOR CHANNEL MASK !
	channel_mask = buf[1];
	if (channel_mask == 0xff) channel_mask = 0xffff;
	flags = buf[2];
	ret = clear_calibration_data(awl, channel_mask, flags);
	printf ("CMD calibration cleared\n");
	return ret;
}

/* ================================================================ */

int process_debug_dump (awl_data *awl, uint8_t *buf, size_t size)
{
	int i;
	buf[size-1] = '\0';
	printf("DBG DUMP %s\n", buf);
	printf("DBG current frame rate is %d frames/second\n", awl->frame_rate);
	printf("DBG current dead zone is %d samples\n", awl->dead_zone);
	printf("DBG payload size is %zd samples\n", awl->payload_size / sizeof(uint32_t));
	printf("DBG process channel mask is 0x%04x\n", awl->process_channel_mask);
	printf("DBG current algo is %d\n", awl->current_algo);
	for(i=0; i < awl->num_params[awl->current_algo];  i ++) {
		if (ALGO_HAS_PARAM(i)) {
			printf("DBG algo %d parameter %d value is %f\n",
			       awl->current_algo, i, ALGO_PARAM(i));
		}
	}
	printf("DBG current track algo is %d\n", awl->current_track_algo);
	for(i=0; i < awl->num_params[awl->current_track_algo];  i ++) {
		if (TRACKALGO_HAS_PARAM(i)) {
			printf("DBG track algo %d parameter %d value is %f\n",
			       awl->current_track_algo, i, TRACKALGO_PARAM(i));
		}
	}
	printf("DBG CAN transmit cooked channel mask is 0x%04x\n", awl->can_transmit_cooked_channel_mask);
	printf("DBG CAN transmit cooked event   mask is 0x%04x\n", awl->can_transmit_cooked_event_mask);
	printf("DBG UDP transmit cooked channel mask is 0x%04x\n", awl->udp_transmit_cooked_channel_mask);
	printf("DBG UDP transmit raw    channel mask is 0x%04x\n", awl->udp_transmit_raw_channel_mask);
	return 0;
}

/* ================================================================ */

int process_debug_verbose (awl_data *awl, uint8_t *buf, size_t size)
{
	buf[size-1] = '\0';
	printf("DBG VERBOSE 0x%02x\n", buf[1]);
	awl->verbose_distance = buf[1] & 0x01;
	awl->verbose_awl      = buf[1] & 0x08;
	awl->verbose_can      = buf[1] & 0x10;
	awl->verbose_playback = buf[1] & 0x20;
	awl->verbose_udp      = buf[1] & 0x40;
	awl->verbose_gpib     = buf[1] & 0x80;
	return 0;
}

/* ================================================================ */

int process_debug_force_refresh (awl_data *awl, uint8_t *buf, size_t size)
{
	get_frame_rate(awl);
	printf ("DBG current frame rate is %d\n", awl->frame_rate);
	get_dead_zone(awl);
	printf ("DBG current dead zone is %d\n", awl->dead_zone);
	return 0;
}

/* ================================================================ */

int set_parameter(awl_data *awl, uint8_t *buf, size_t size)
{
	uint8_t type, addr8, algo;
	uint16_t address;
	uint32_t uvalue;
	float fvalue;
	int ret = 0;
	time_t now;
	struct tm t;
	struct timeval tv;
	char append[16];
	size_t len;
	int eos, i;

	type = buf[1];
	addr8 = buf[2];
	algo = buf[3];
	address = *((uint16_t*)(buf+2));
	uvalue = *((uint32_t*)(buf+4));
	fvalue = *((float*)(buf+4));

	printf ("CMD S 0x%02x 0x%04x 0x%08x %f\n", type, address, uvalue, fvalue);

	now = time(NULL);
	localtime_r(&now, &t);

	switch (type) {
	case PTYPE_ALGO_SELECTED:
		if (uvalue < awl->num_algos) {
			awl->current_algo = uvalue;
			printf("CMD current algo %d\n", awl->current_algo);
		} else {
			printf("CMD num algos %d\n", awl->num_algos);
			ret = -1;
		}
		break;
	case PTYPE_ALGO_PARAMETER:
		if (algo) {
			if (ALGO_N_HAS_PARAM(algo, addr8)) {
				ALGO_N_PARAM(algo, addr8) = fvalue;
				printf ("CMD algo %d parameter %d=%f\n", algo, addr8, fvalue);
			}
		} else {
			if (ALGO_HAS_PARAM(address)) {
				ALGO_PARAM(address) = fvalue;
				printf ("CMD algo %d parameter %d=%f\n", awl->current_algo, addr8, fvalue);
			} else ret = -1;
		}
		break;
	case PTYPE_TRACK_ALGO_SELECTED:
		if (uvalue < awl->num_algos) {
			awl->current_track_algo = uvalue;
			printf("CMD current track algo %d\n", awl->current_track_algo);
		} else {
			printf("CMD num track algos %d\n", awl->num_algos);
			ret = -1;
		}
		break;
	case PTYPE_TRACK_ALGO_PARAMETER:
		if (algo) {
			if (TRACKALGO_N_HAS_PARAM(algo, addr8)) {
				TRACKALGO_N_PARAM(algo, addr8) = fvalue;
				printf ("CMD track algo %d parameter %d=%f\n", algo, addr8, fvalue);
			}
		} else {
			if (TRACKALGO_HAS_PARAM(address)) {
				TRACKALGO_PARAM(address) = fvalue;
				printf ("CMD track algo %d parameter %d=%f\n", awl->current_track_algo, addr8, fvalue);
			} else ret = -1;
		}
		break;
	case PTYPE_AWL_REGISTER:
		ret = write_awl_register(address, uvalue);
		if (!ret) {
			switch (address) {
			case 0x10:
			case 0x14:
				get_frame_rate(awl);
				break;
			case 0x24:
			case 0x1c:
				get_dead_zone(awl);
				break;
			}
		}
		break;
	case PTYPE_BIAS:
		ret = write_bias(uvalue);
		break;
	case PTYPE_ADC_REGISTER:
		ret = write_adc_register(address, uvalue);
		break;
	case PTYPE_PRESET:
		ret = -1;
		break;
	case PTYPE_GLOBAL_PARAMETER:
		switch(address) {
		default:
		case PADDR_NULL:
			ret = -1;
			break;
		case PADDR_WINDOW_WIDTH:
			awl->window_width = uvalue;
			break;
		case PADDR_WINDOW_QTY:
			awl->window_qty = uvalue;
			break;
		case PADDR_ALGO_SMA:
			awl->algo_sma = uvalue;
			break;
		case PADDR_ALGO_PERSISTANCE:
			awl->algo_persistence = uvalue;
			break;
		case PADDR_AB_ALPHA_POS:
			awl->ab_alpha_pos = fvalue;
			break;
		case PADDR_AB_BETA_POS:
			awl->ab_beta_pos = fvalue;
			break;
		case PADDR_AB_ALPHA_SPEED:
			awl->ab_alpha_speed = fvalue;
			break;
		case PADDR_AB_BETA_SPEED:
			awl->ab_beta_speed = fvalue;
			break;
		case PADDR_AB_MAX_OBJECT_SPEED:
			awl->track_max_object_speed = fvalue;
			break;
		}
		break;
	case PTYPE_GPIO:
		ret = write_gpio(address, uvalue);
		break;
	case PTYPE_RECORD:
	case PTYPE_PLAYBACK:
		switch(address) {
		default:
		case PADDR_NULL:
			strncpy(append, (char*)buf+2, 6);
			append[6] = '\0';
			eos = 0;
			for (i = 0; i < 6; i++) {
				if (append[i] == '\0') {
					eos = 1;
				}
			}
			switch(type) {
			case PTYPE_RECORD:
				len = strlen(awl->record_filename);
				if (!awl->receiving_record_filename) {
					awl->receiving_record_filename = 1;
					awl->record_filename[0] = '\0';
				}
				strncat(awl->record_filename, append, PATH_MAX - len - 1);
				if (eos) {
					awl->receiving_record_filename = 0;
				}
				break;
			case PTYPE_PLAYBACK:
				len = strlen(awl->playback_filename);
				if (!awl->receiving_playback_filename) {
					awl->receiving_playback_filename = 1;
					awl->playback_filename[0] = '\0';
				}
				if (eos) {
					awl->receiving_playback_filename = 0;
				}
				strncat(awl->playback_filename, append, PATH_MAX - len - 1);
				break;
			}
			break;
		case PADDR_ONGOING:
			ret = -1;
			break;
		}
		break;
	case PTYPE_DATE_TIME:
		switch(address) {
		default:
			break;
		case 0: // deprecated
			t.tm_year = *((uint16_t*)(buf+4));
			t.tm_mon = buf[6];
			t.tm_mday = buf[7];
			tv.tv_sec = mktime(&t);
			tv.tv_usec = 0;
			settimeofday(&tv, NULL);
			break;
		case PADDR_DATE:
			t.tm_year = *((uint16_t*)(buf+4)) - 1900;
			t.tm_mon = buf[6] - 1;
			t.tm_mday = buf[7];
			tv.tv_sec = mktime(&t);
			tv.tv_usec = 0;
			settimeofday(&tv, NULL);
			break;

		case PADDR_TIME:
			t.tm_hour = buf[4];
			t.tm_min = buf[5];
			t.tm_sec = buf[6];
			tv.tv_sec = mktime(&t);
			tv.tv_usec = 0;
			settimeofday(&tv, NULL);
			break;
		case PADDR_TIMEZONE:
			break;
		}
		break;
	case PTYPE_OLD_TIME: // deprecated;
		t.tm_hour = buf[4];
		t.tm_min = buf[5];
		t.tm_sec = buf[6];
		tv.tv_sec = mktime(&t);
		tv.tv_usec = 0;
		settimeofday(&tv, NULL);
		break;
	}

	return ret;
}

/* ================================================================ */

int query_parameter(awl_data *awl, uint8_t *buf, size_t size)
{
	uint8_t type;
	uint16_t address;
	uint32_t *uvalue;
	float *fvalue;
	int ret = 0;
	time_t now;
	struct tm t;
	char *s;
	uint16_t channel_mask = 0xffff;
	size_t l;

	type = buf[1];
	address = *((uint16_t*)(buf+2));
	uvalue = ((uint32_t*)(buf+4));
	fvalue = ((float*)(buf+4));

	buf[0] = CMD_RESPONSE_PARAMETER;

	printf ("CMD Q 0x%02x 0x%04x 0x%08x %f\n", type, address, *uvalue, *fvalue);

	now = time(NULL);
	localtime_r(&now, &t);

	s = NULL;

	switch (type) {
	case PTYPE_ALGO_SELECTED:
		*uvalue = awl->current_algo;
		send_msg(awl, MSG_CONTROL_GROUP, buf, 8);
		break;
	case PTYPE_ALGO_PARAMETER:
		if (ALGO_HAS_PARAM(address)) {
			*fvalue = ALGO_PARAM(address);
//printf("Sending parameter %d=%f\n", address, *fvalue);
//{int i; for (i = 0; i <<8; i++) { printf("%02x ", buf[i]); } printf("\n");}
			send_msg(awl, MSG_CONTROL_GROUP, buf, 8);
		} else ret = -1;
		break;
	case PTYPE_TRACK_ALGO_SELECTED:
		*uvalue = awl->current_track_algo;
		send_msg(awl, MSG_CONTROL_GROUP, buf, 8);
		break;
	case PTYPE_TRACK_ALGO_PARAMETER:
		if (TRACKALGO_HAS_PARAM(address)) {
			*fvalue = TRACKALGO_PARAM(address);
//printf("Sending parameter %d=%f\n", address, *fvalue);
//{int i; for (i = 0; i <<8; i++) { printf("%02x ", buf[i]); } printf("\n");}
			send_msg(awl, MSG_CONTROL_GROUP, buf, 8);
		} else ret = -1;
		break;
	case PTYPE_AWL_REGISTER:
		if (!read_awl_register(address, uvalue)) {
			send_msg(awl, MSG_CONTROL_GROUP, buf, 8);
		} else ret = -1;
		break;
	case PTYPE_BIAS:
		if (!read_bias(uvalue)) {
			send_msg(awl, MSG_CONTROL_GROUP, buf, 8);
		} else ret = -1;
		break;
	case PTYPE_ADC_REGISTER:
		if (!read_adc_register(address, uvalue)) {
			send_msg(awl, MSG_CONTROL_GROUP, buf, 8);
		} else ret = -1;
		break;
	case PTYPE_PRESET:
		ret = -1;
		break;
	case PTYPE_GLOBAL_PARAMETER:
		switch(address) {
		default:
		case PADDR_NULL:
			ret = -1;
			break;
		case PADDR_WINDOW_WIDTH:
			*uvalue = awl->window_width;
			break;
		case PADDR_WINDOW_QTY:
			*uvalue = awl->window_qty;
			break;
		case PADDR_ALGO_SMA:
			*uvalue = awl->algo_sma;
			break;
		case PADDR_ALGO_PERSISTANCE:
			*uvalue = awl->algo_persistence;
			break;
		}
		if (!ret) {
			send_msg(awl, MSG_CONTROL_GROUP, buf, 8);
		}
		break;
	case PTYPE_GPIO:
		if (!read_gpio(address, uvalue)) {
			send_msg(awl, MSG_CONTROL_GROUP, buf, 8);
		} else ret = -1;
		break;
	case PTYPE_RECORD:
		s = awl->record_filename;
		channel_mask = awl->record_channel_mask;
	case PTYPE_PLAYBACK:
		if (!s) {
			s = awl->playback_filename;
			channel_mask = awl->playback_channel_mask;
		}
		switch(address) {
		default:
			ret = -1;
			break;
		case PADDR_NULL:
			l = strlen(s);
			while (l > 0) {
				strncpy((char*)buf+2, s, 6);
				send_msg(awl, MSG_CONTROL_GROUP, buf, 8);
				if (l > 6) {
					s+=6;
				}
				l-=6;
			};
			if(!l) {
				memset(buf+2, 0, 6);
				send_msg(awl, MSG_CONTROL_GROUP, buf, 8);
			}
			break;
		case PADDR_ONGOING:
			buf[3] = 1;
			*((uint16_t*)(buf+4)) = channel_mask;
			send_msg(awl, MSG_CONTROL_GROUP, buf, 8);
		}
		
		break;
	case PTYPE_DATE_TIME:
		switch (address) {
		default: //deprecated
		case PADDR_DATE:
			*((uint16_t*)(buf+4)) = t.tm_year + 1900;
			buf[6] = t.tm_mon + 1;
			buf[7] = t.tm_mday;
			send_msg(awl, MSG_CONTROL_GROUP, buf, 8);
			break;
		case PADDR_TIME:
			buf[4] = t.tm_hour;
			buf[5] = t.tm_min;
			buf[6] = t.tm_sec;
			buf[7] = 0x00;
			send_msg(awl, MSG_CONTROL_GROUP, buf, 8);
			break;
		case PADDR_TIMEZONE:
			ret = -1;
			break;
		}
		break;
	}

	return ret;
}

/* ================================================================ */

int process_parameter(awl_data *awl, uint8_t *buf, size_t size)
{

	// Procdess response to a query parameter message.
	// Nothing to do for now.

	uint8_t type;
	uint16_t address;
	uint32_t uvalue;
	float fvalue;

	type = buf[1];
	address = *((uint16_t*)(buf+2));
	uvalue = *((uint32_t*)(buf+4));
	fvalue = *((float*)(buf+4));

	printf ("CMD R 0x%02x 0x%04x 0x%08x %f\n", type, address, uvalue, fvalue);

	switch (type) {
	case PTYPE_ALGO_SELECTED:
		break;
	case PTYPE_ALGO_PARAMETER:
		break;
	case PTYPE_TRACK_ALGO_SELECTED:
		break;
	case PTYPE_TRACK_ALGO_PARAMETER:
		break;
	case PTYPE_AWL_REGISTER:
		break;
	case PTYPE_BIAS:
		break;
	case PTYPE_ADC_REGISTER:
		break;
	case PTYPE_PRESET:
		break;
	case PTYPE_GLOBAL_PARAMETER:
		break;
	case PTYPE_GPIO:
		break;
	case PTYPE_RECORD:
		break;
	case PTYPE_PLAYBACK:
		break;
	}
	return 0;
}

/* ================================================================ */

int process_cmd(awl_data *awl, int32_t cmd_id, uint8_t *buf, size_t size)
{
	uint8_t command;
	int feedback = 1;
	int ret = -1;

	command = buf[0];
printf("CMD 0x%02x\n", command);
	switch(command) {
	default:
		break;
	case CMD_TRANSMIT_RAW:
		ret = process_transmit_raw(awl, buf, size);
		break;
	case CMD_TRANSMIT_COOKED:
		ret = process_transmit_cooked(awl, cmd_id, buf, size);
		break;
	case CMD_RECORD_RAW:
		ret = process_recording(awl, buf, size);
		break;
	case CMD_PLAYBACK_RAW:
		ret = process_playback(awl, buf, size);
		break;
	case CMD_RECORD_CALIBRATION:
		ret = process_record_calibration(awl, buf, size);
		break;
	case CMD_CLEAR_CALIBRATION:
		ret = process_clear_calibration(awl, buf, size);
		break;
	case CMD_SET_PARAMETER:
		ret = set_parameter(awl, buf, size);
		break;
	case CMD_QUERY_PARAMETER:
		ret = query_parameter(awl, buf, size);
		break;
	case CMD_RESPONSE_PARAMETER:
		ret = process_parameter(awl, buf, size);
		feedback = 0;
		break;
	case CMD_ERROR_PARAMETER:
		printf("CMD ERROR PARAMETER\n");
		feedback = 0;
		break;
	case CMD_DEBUG_DUMP:
		ret = process_debug_dump(awl, buf, size);
		feedback = 0;
		break;
	case CMD_DEBUG_VERBOSE:
		ret = process_debug_verbose(awl, buf, size);
		feedback = 0;
		break;
	case CMD_DEBUG_FORCE_REFRESH:
		ret = process_debug_force_refresh(awl, buf, size);
		feedback = 0;
		break;
	}

	if (ret) {
printf("CMD ERROR 0x%02x\n", buf[1]);	
		buf[0] = CMD_ERROR_PARAMETER;
		send_msg(awl, MSG_CONTROL_GROUP, buf, 8);
	} else {
		if (feedback) {
			buf[0] = CMD_RESPONSE_PARAMETER;
			send_msg(awl, MSG_CONTROL_GROUP, buf, 8);
		}
	}
	return 0;
}

/* ================================================================ */

int send_msg(awl_data *awl, int32_t msg_id, uint8_t *buf, size_t size)
{
	send_udp(awl, msg_id, size, buf);
	send_can(awl, msg_id, size, buf);
	return 0;
}

/* ================================================================ */

int send_record(awl_data *awl)
{
	uint8_t buffer[8];
	memset(buffer, 0, 8);
	buffer[0] = CMD_RECORD_RAW;
	buffer[1] = awl->client_value;
	return send_msg(awl, MSG_CONTROL_GROUP, buffer, 8);
}

/* ================================================================ */

int send_playback(awl_data *awl)
{
	uint8_t buffer[8];
	memset(buffer, 0, 8);
	buffer[0] = CMD_PLAYBACK_RAW;
	buffer[1] = awl->client_value;
	return send_msg(awl, MSG_CONTROL_GROUP, buffer, 8);
}

/* ================================================================ */

int send_record_calibration(awl_data *awl)
{
	uint8_t buffer[8];
	memset(buffer, 0, 8);
	buffer[0] = CMD_RECORD_CALIBRATION;
	buffer[1] = 0xff;
	buffer[2] = awl->client_value;
	*((float *)(buffer+4)) = 0.1;  // beta
	return send_msg(awl, MSG_CONTROL_GROUP, buffer, 8);
}

/* ================================================================ */

int send_clear_calibration(awl_data *awl)
{
	uint8_t buffer[8];
	memset(buffer, 0, 8);
	buffer[0] = CMD_CLEAR_CALIBRATION;
	buffer[1] = 0xff;
	buffer[2] = awl->client_value;
	return send_msg(awl, MSG_CONTROL_GROUP, buffer, 8);
}

/* ================================================================ */

int send_debug_dump(awl_data *awl)
{
	uint8_t buffer[8];
	memset(buffer, 0, 8);
	buffer[0] = CMD_DEBUG_DUMP;
	return send_msg(awl, MSG_CONTROL_GROUP, buffer, 8);
}

/* ================================================================ */

int send_debug_verbose(awl_data *awl)
{
	uint8_t buffer[8];
	memset(buffer, 0, 8);
	buffer[0] = CMD_DEBUG_VERBOSE;
	buffer[1] = awl->client_value;
	return send_msg(awl, MSG_CONTROL_GROUP, buffer, 8);
}

/* ================================================================ */

int send_debug_force_refresh(awl_data *awl)
{
	uint8_t buffer[8];
	memset(buffer, 0, 8);
	buffer[0] = CMD_DEBUG_FORCE_REFRESH;
	return send_msg(awl, MSG_CONTROL_GROUP, buffer, 8);
}

/* ================================================================ */

int send_set_parameter(awl_data *awl)
{
	uint8_t buffer[8];
	memset(buffer, 0, 8);
	buffer[0] = CMD_SET_PARAMETER;
	buffer[1] = awl->client_ptype;
	*((uint16_t*)(buffer+2)) = awl->client_address;
	*((uint32_t*)(buffer+4)) = awl->client_value;
	return send_msg(awl, MSG_CONTROL_GROUP, buffer, 8);
}

/* ================================================================ */

int send_query_parameter(awl_data *awl)
{
	uint8_t buffer[8];
	memset(buffer, 0, 8);
	buffer[0] = CMD_QUERY_PARAMETER;
	buffer[1] = awl->client_ptype;
	*((uint16_t*)(buffer+2)) = awl->client_address;
	*((uint32_t*)(buffer+4)) = awl->client_value;
	return send_msg(awl, MSG_CONTROL_GROUP, buffer, 8);
}

/* ================================================================ */

int send_distance(awl_data *awl, int channel, uint16_t *distance_buffer)
{
	int ret;
	if (channel > 7) return 0; // OLD MESSAGE, ONLY VALID FOR CHANNEL 0-7
	if (!(awl->can_transmit_cooked_channel_mask & (1 << channel))) return 0;
	if (awl->can_transmit_cooked_frame_rate != awl->frame_rate) {
		if (CURRENT_AWL_TIMESTAMP % (awl->frame_rate/awl->can_transmit_cooked_frame_rate)) return 0;
	}
	if (awl->can_transmit_cooked_event_mask & EVENT_MASK_SENSOR_DISTANCE1_4) {
		ret = send_msg(awl, MSG_CH1_DISTANCE1_4 + channel, (uint8_t *) distance_buffer, 8);
	}
	if (awl->can_transmit_cooked_event_mask & EVENT_MASK_SENSOR_DISTANCE5_8) {
		ret = send_msg(awl, MSG_CH1_DISTANCE5_8 + channel, (uint8_t *) (distance_buffer + 4), 8);
	}
	return ret;
}

/* ================================================================ */

int send_intensity(awl_data *awl, int channel, uint16_t *intensity_buffer)
{
	int ret;
	if (channel > 7) return 0; // OLD MESSAGE, ONLY VALID FOR CHANNEL 0-7
	if (!(awl->can_transmit_cooked_channel_mask & (1 << channel))) return 0;
	if (awl->can_transmit_cooked_frame_rate != awl->frame_rate) {
		if (CURRENT_AWL_TIMESTAMP % (awl->frame_rate/awl->can_transmit_cooked_frame_rate)) return 0;
	}
	if (awl->can_transmit_cooked_event_mask & EVENT_MASK_SENSOR_INTENSITY1_4) {
		ret = send_msg(awl, MSG_CH1_INTENSITY1_4 + channel, (uint8_t *) intensity_buffer, 8);
	}
	if (awl->can_transmit_cooked_event_mask & EVENT_MASK_SENSOR_INTENSITY5_8) {
		ret = send_msg(awl, MSG_CH1_INTENSITY5_8 + channel, (uint8_t *) (intensity_buffer + 4), 8);
	}
	return ret;
}

/* ================================================================ */

int send_distance_intensity(awl_data *awl, int channel, uint16_t *distance_buffer, uint16_t *intensity_buffer)
{
	int i;
	int ret;
	uint16_t buffer[4];
	//printf ("TCCM: %x %x\n", awl->can_transmit_cooked_channel_mask, (1 << channel));
	if (!(awl->can_transmit_cooked_channel_mask & (1 << channel))) return 0;
	if (awl->can_transmit_cooked_frame_rate != awl->frame_rate) {
		if (CURRENT_AWL_TIMESTAMP % (awl->frame_rate/awl->can_transmit_cooked_frame_rate)) return 0;
	}
	if (awl->can_transmit_cooked_event_mask & EVENT_MASK_SENSOR_DISTANCE_INTENSITY) {
		for (i = 0; i < 8; i ++) {
			buffer[0] = channel;
			buffer[1] = distance_buffer[i];
			buffer[2] = intensity_buffer[i];
			buffer[3] = i;
			if (distance_buffer[i]) {
				ret = send_msg(awl, MSG_CH_DISTANCE_INTENSITY, (uint8_t *) buffer, 8);
			}
			//printf("Dist: %d %d %d %d\n", buffer[0], buffer[1], buffer[2], buffer[3]);
		}
	}
	return ret;
}

/* ================================================================ */

int send_obstacle(awl_data *awl, awl_obstacle *obstacle)
{
	int ret;
	uint8_t buf8[8];
	uint16_t *buf16;
	buf16 = (uint16_t*)buf8;

	if (awl->can_transmit_cooked_frame_rate != awl->frame_rate) {
		if (CURRENT_AWL_TIMESTAMP % (awl->frame_rate/awl->can_transmit_cooked_frame_rate)) return 0;
	}
	if (awl->can_transmit_cooked_event_mask & EVENT_MASK_OBSTACLE
	    || awl->can_transmit_cooked_event_mask & EVENT_MASK_OBSTACLE_COMPACT) {
//int i;
		buf16[0] = obstacle->track_id;
		buf8[2]  = (1 << (obstacle->channel & 0xff)) & 0xff;
		buf8[3]  = obstacle->channel & 0xff;
		buf8[4]  = (obstacle->channel >> 8) & 0xff;
		buf8[5]  = obstacle->track_probability;
		//buf16[3] = obstacle->time_to_collision_estimate;
		buf16[3] = obstacle->object_intensity;
		ret = send_msg(awl, MSG_OBSTACLE_TRAK, buf8, 8);
/*
printf("\nsend ob ");
for (i = 0; i < 4; i ++) printf ("%04x ", buf16[i]);
*/

		buf16[1] = obstacle->track_distance;
		buf16[2] = obstacle->track_speed;
		buf16[3] = obstacle->track_acceleration;
		ret = send_msg(awl, MSG_OBSTACLE_VELOCITY, buf8, 8);
/*
printf("\nsend ob ");
for (i = 0; i < 4; i ++) printf ("%04x ", buf16[i]);
printf ("\n");
*/
	}
	if (awl->can_transmit_cooked_event_mask & EVENT_MASK_OBSTACLE) {

		buf16[1] = obstacle->object_height_max;
		buf16[2] = obstacle->object_width_max;
		buf16[3] = obstacle->object_intensity;
		ret = send_msg(awl, MSG_OBSTACLE_SIZE, buf8, 8);

		buf16[1] = obstacle->object_start_angle;
		buf16[2] = obstacle->object_end_angle;
		buf16[3] = obstacle->object_angular_velocity;
		ret = send_msg(awl, MSG_OBSTACLE_POSITION, buf8, 8);
	}
	return ret;
}

/* ================================================================ */

int send_frame_done(awl_data *awl)
{
	uint16_t buffer[4];
	uint32_t ts;

	memset(buffer, 0, 8);
	ts = awl->last_ts[GET_CURRENT_AWL_CHANNEL] & 0xffff;
	buffer[0] = ts;
	buffer[1] = 0x0010;
	return send_msg(awl, MSG_FRAME_DONE, (uint8_t*)buffer, 8);
}

/* ================================================================ */
