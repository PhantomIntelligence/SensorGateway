#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "awl_data.h"
#include "awlcmd.h"

static int numparams[AWL_NUM_ALGOS] = { AWL_NUM_PARAMS, AWL_NUM_PARAMS, AWL_NUM_PARAMS, AWL_NUM_PARAMS, AWL_NUM_PARAMS, AWL_NUM_PARAMS, AWL_NUM_PARAMS };
static float params0[AWL_NUM_PARAMS] = { 0 }; // Algo is obsolete
static float params1[AWL_NUM_PARAMS] = { 0 }; // Algo is obsolete
static float params2[AWL_NUM_PARAMS] =
    { ALGO2_PARAM0_OFFSET, ALGO2_PARAM1_THRESHOLD, ALGO2_PARAM2_FIRSTDAMPING, ALGO2_PARAM3_NUMWIDTH, ALGO2_PARAM4_INTENSITYFILTER, ALGO2_PARAM5_SECONDWIDTH, ALGO2_PARAM6_UNDEFINED , ALGO2_PARAM7_SNRMIN, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static float params3[AWL_NUM_PARAMS] =
    { ALGO3_PARAM0_OFFSET, ALGO3_PARAM1_THRESHOLD, ALGO3_PARAM2_1STDERIVATEORDER, ALGO3_PARAM3_2NDDERIVATEORDER, ALGO3_PARAM4_INTENSITYFILTER, ALGO3_PARAM5_UNDEFINED, ALGO3_PARAM6_FIRSTINDEX, ALGO3_PARAM7_SNRMIN, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static float params4[AWL_NUM_PARAMS] = 
	{ ALGO4_PARAM0_OFFSET, ALGO4_PARAM1_UNDEFINED, ALGO4_PARAM2_DAMPING, ALGO4_PARAM3_MEANWIDTH, ALGO4_PARAM4_SOURCEWIDTH, ALGO4_PARAM5_DERIVATEWIDTH, ALGO4_PARAM6_PROCESS, ALGO4_PARAM7_SNRMIN, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static float params5[AWL_NUM_PARAMS] = 
	{ ALGO5_PARAM0_OFFSET, ALGO5_PARAM1_SOURCEWIDTH, ALGO5_PARAM2_NBDIAGS, ALGO5_PARAM3_MAXCOUNTEVD, ALGO5_PARAM4_HISTORYLENGTH, ALGO5_PARAM5_MAXNUMBEROFSOURCES, ALGO5_PARAM6_UNDEFINED, ALGO5_PARAM7_SNRMIN, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static float params6[AWL_NUM_PARAMS] = 
	{ ALGO6_PARAM0_OFFSET, ALGO6_PARAM1_STDFACTOR, ALGO6_PARAM2_DAMPING, ALGO6_PARAM3_SOURCEWIDTH, ALGO6_PARAM4_DERIVATEWIDTH, ALGO6_PARAM5_SNRMIN, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static float *params[AWL_NUM_ALGOS] = { params0, params1, params2, params3, params4, params5, params6 };

static int numparamstrack[AWL_NUM_TRACK_ALGOS] = { AWL_NUM_PARAMS, AWL_NUM_PARAMS, AWL_NUM_PARAMS };
static float trackparams0[AWL_NUM_PARAMS] = { 0 }; // Disabled tracker
static float trackparams1[AWL_NUM_PARAMS] = 
	{ ABFILTER_PARAM0_ALPHAPOS, ABFILTER_PARAM1_BETAPOS, ABFILTER_PARAM2_ALPHASPEED, ABFILTER_PARAM3_BETASPEED, ABFILTER_PARAM4_PERSISTANCE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // AB filter
static float trackparams2[AWL_NUM_PARAMS] =
	 { LRFILTER_PARAM0_MAX_SNR_TO_PAIR, LRFILTER_PARAM1_MIN_SNR_TO_TRACK, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // LR tracker
static float *trackparams[AWL_NUM_TRACK_ALGOS] = { trackparams0, trackparams1, trackparams2 };


int parse_config_file(awl_data * awl)
{
	FILE *config_fp;
	char line[MAX_LINE_LEN + 1];
	char *token;
	char current_param[VAR_NAME_MAX];
	int strindex = 0, intindex = 0, floatindex = 0;
	char *first_dquote, *last_dquote;
	char *save_ptr;

	config_fp = fopen(awl->config_filename, "r");
	if (!config_fp) {
		fprintf (stderr, "INIT ");
		perror(awl->config_filename);
		return errno;
	}

	while (fgets(line, MAX_LINE_LEN, config_fp) != NULL) {
		token = strtok_r(line, "\t =\n", &save_ptr);
		if (token != NULL && token[0] != '#') {
			strcpy(current_param, token);
	//		token = strtok(NULL, "\t =\n");
			token = save_ptr;
			first_dquote = strchr(token, '"');
			last_dquote = strrchr(token, '"');

/*
printf("token %s\n", token);
if (first_dquote) printf("first dquote %s\n", first_dquote);
if (last_dquote) printf("last dquote %s\n", last_dquote);
*/
			if (first_dquote && last_dquote && first_dquote != last_dquote) {
				*last_dquote = '\0';
				strcpy(awl->strparm[strindex].value, first_dquote + 1);
				strcpy(awl->strparm[strindex++].parameter,
				       current_param);
				/*
				printf("%s string %s\n",
				       awl->strparm[strindex - 1].parameter,
				       awl->strparm[strindex - 1].value);
				*/
			} else {
				if (strchr(token, '.')) {
					sscanf(token, "%f", &(awl->floatparm[floatindex].value));
					strcpy(awl->floatparm[floatindex++].
					       parameter, current_param);
					/*
					printf("%s float:%f\n",
					       awl->floatparm[floatindex -
							      1].parameter,
					       awl->floatparm[floatindex -
							      1].value);
					*/
				} else {
					sscanf(token, "%i", &(awl->intparm[intindex].value));
					strcpy(awl->intparm[intindex++].
					       parameter, current_param);
					/*
					printf("%s int:%d\n",
					       awl->intparm[intindex -
							    1].parameter,
					       awl->intparm[intindex -
							    1].value);
					*/
				}
			}
		}
	}

	return 0;
}

int apply_config_int(awl_data *awl, char *name, int *value)
{
	int i, hit = -1;
	if (awl->config_dump) {
		printf("%s=%d\n", name, *value);
		return -1;
	}
	for(i=0; i<MAX_NB_VAR; i++) {
		if (!strcmp(name, awl->intparm[i].parameter)) {
			hit = i;
			break;
		}
	}
	if (hit > -1) {
		*value = awl->intparm[hit].value;
	}
	return hit;
}

int apply_config_float(awl_data *awl, char *name, float *value)
{
	int i, hit = -1;
	if (awl->config_dump) {
		printf("%s=%f\n", name, *value);
		return -1;
	}
	for(i=0; i<MAX_NB_VAR; i++) {
		if (!strcmp(name, awl->floatparm[i].parameter)) {
			hit = i;
			break;
		}
	}
	if (hit > -1) {
		*value = awl->floatparm[hit].value;
	}
	return hit;
}

int apply_config_str(awl_data *awl, char *name, char *value, size_t size)
{
	int i, hit = -1;
	if (awl->config_dump) {
		printf("%s=\"%s\"\n", name, value);
		return -1;
	}
	for(i=0; i<MAX_NB_VAR; i++) {
		if (!strcmp(name, awl->strparm[i].parameter)) {
			hit = i;
			break;
		}
	}
	if (hit > -1) {
		strncpy(value, awl->strparm[hit].value, size);
	}
	return hit;
}

int apply_config_file(awl_data *awl)
{
	int i;
	int algo, parameter;
	char name[64];

	apply_config_str(awl, "awl-ifname", (awl->awl_ifname), PATH_MAX);
	apply_config_str(awl, "can-ifname", (awl->can_ifname), PATH_MAX);
	apply_config_str(awl, "gpib-devname", (awl->gpib_devname), PATH_MAX);
	apply_config_str(awl, "post-calibration-script", (awl->post_calibration_script), PATH_MAX);
	apply_config_str(awl, "post-recording-script", (awl->post_recording_script), PATH_MAX);
	apply_config_str(awl, "playback-path-prefix", (awl->playback_path_prefix), PATH_MAX);
	apply_config_str(awl, "record-path-prefix", (awl->record_path_prefix), PATH_MAX);
	apply_config_float(awl, "frequency", &(awl->frequency));
	apply_config_str(awl, "srv-saddr", (awl->srv_saddr), SADDR_SIZE);
	apply_config_int(awl, "srv-port", &(awl->srv_port));
	apply_config_str(awl, "cli-saddr", (awl->cli_saddr), SADDR_SIZE);
	apply_config_int(awl, "cli-port", &(awl->cli_port));
	/* to be remove */
	apply_config_int(awl, "window-width", &(awl->window_width));
	apply_config_int(awl, "window-qty",   &(awl->window_qty));
	apply_config_int(awl, "algo-sma",   &(awl->algo_sma));
	/* to be remove */
	/* ab filter */
        //apply_config_float(awl, "ab-alpha-pos", &(awl->ab_alpha_pos));
        //apply_config_float(awl, "ab-beta-pos", &(awl->ab_beta_pos));
        //apply_config_float(awl, "ab-alpha-speed", &(awl->ab_alpha_speed));
        //apply_config_float(awl, "ab-beta-speed", &(awl->ab_beta_speed));
	//apply_config_int(awl, "algo-persistence",   &(awl->algo_persistence));
	apply_config_float(awl, "track-max-object-speed", &(awl->track_max_object_speed));
	
        apply_config_int(awl, "auto-calib-counter", &(awl->auto_calib_counter));
        apply_config_int(awl, "auto-calib", &(awl->auto_calib));
        apply_config_float(awl, "default-beta", &(awl->default_beta));

/*
		case 'g':
			sscanf(optarg, "%i", &arg1);
			&(awl->client_address = arg1;
			break;
		case 'o':
			sscanf(optarg, "%f", &argf);
			if (ALGO_HAS_PARAM_X(&(awl->client_address)) {
				ALGO_PARAM_X(&(awl->client_address) = argf;
			} else {
				printf ("algo %d does not have parameter %d\n", &(awl->current_algo, &(awl->client_address);
				exit (1);
			}
			break;
*/
	apply_config_int(awl, "verbose-udp", &(awl->verbose_udp));
	apply_config_int(awl, "verbose-can", &(awl->verbose_can));
	apply_config_int(awl, "verbose-awl", &(awl->verbose_awl));
	apply_config_int(awl, "verbose-gpib", &(awl->verbose_gpib));
	apply_config_int(awl, "verbose-distance", &(awl->verbose_distance));
	apply_config_int(awl, "verbose-playback", &(awl->verbose_playback));
	i = awl->process_channel_mask;
	if (apply_config_int(awl, "channel-mask", &i) >= 0) awl->process_channel_mask = i;
	i = awl->record_channel_mask;
	if (apply_config_int(awl, "record-channel-mask", &i) >= 0) awl->record_channel_mask = i;
	apply_config_int(awl, "playback-loop-max", &(awl->playback_loop_max));
	i = awl->heartbeat;
	if (apply_config_int(awl, "heartbeat", &i) >= 0) awl->heartbeat = i;
	i = awl->payload_size / sizeof(uint32_t);
	if (apply_config_int(awl, "payload-size", &i) >= 0) {
		i *= sizeof(uint32_t);
		if (i <= AWL_PAYLOAD_SIZE_MAX) {
			awl->payload_size = i;
		}
	}
/*
			l = strlen(optarg);
			for (i = 0; i < l && i < AWL_NUM_CHANNELS; i++) {
				if (optarg[i] >= '0' && optarg[i] < '0' + AWL_NUM_CHANNELS) {
					&(awl->mapped_channels[i] = optarg[i] - '0';
					printf ("mapping physical channel %d to logical %d\n", i, &(awl->mapped_channels[i]);
				}
			}
			break;
*/
	apply_config_str(awl, "calibration-filename", (awl->calibration_filename), PATH_MAX);

	i = awl->can_transmit_cooked_event_mask;
	if (apply_config_int(awl, "cooked-event-mask", &i) >= 0) awl->can_transmit_cooked_event_mask = i;

	for (algo = 0; algo < AWL_NUM_ALGOS; algo++) { // Fill in detection algorithms params
		for (parameter = 0; parameter < AWL_NUM_PARAMS; parameter ++) {
			sprintf(name, "algo%d-param%d", algo, parameter);
			apply_config_float(awl, name, &(params[algo][parameter]));
		}
	}
	for (algo = 0; algo < AWL_NUM_TRACK_ALGOS; algo++) { // Fill in tracking algorithms params
		for(parameter = 0; parameter < AWL_NUM_PARAMS; parameter ++) {
			sprintf(name, "trackalgo%d-param%d", algo, parameter);
			apply_config_float(awl, name, &(trackparams[algo][parameter]));
		}
	}
	apply_config_int(awl, "algo", &(awl->current_algo));
	apply_config_int(awl, "trackalgo", &(awl->current_track_algo));
	apply_config_int(awl, "maxRawDetections", &(awl->maxNumberOfDetections));
	apply_config_int(awl, "historicSize", &(awl->historicSize));
	apply_config_int(awl, "maxTracks", &(awl->maxNumberOfTrackPerChannel));

	i = awl->record_frame_rate;
	if (apply_config_int(awl, "record-frame-rate", &i) >= 0) awl->record_frame_rate = i;
	i = awl->udp_transmit_raw_frame_rate;
	if (apply_config_int(awl, "transmit-raw-frame-rate", &i) >= 0) awl->udp_transmit_raw_frame_rate = i;
	i = awl->can_transmit_cooked_frame_rate;
	if (apply_config_int(awl, "transmit-cooked-frame-rate", &i) >= 0) awl->can_transmit_cooked_frame_rate = i;
	return 0;

}

int init_awl_data(awl_data * awl)
{
	int i,j;
 	//float sampling = 2.5e-9;


	//memset(awl, 0, sizeof(awl_data));

	parse_config_file(awl);
	strcpy(awl->awl_ifname, "awl0");
	strcpy(awl->can_ifname, "can0");
	strcpy(awl->gpib_devname, "/dev/usbtmc0");
	strcpy(awl->calibration_filename, "awld-calibration.bin");
	awl->frequency = 30.0;
	strcpy(awl->srv_saddr, "0.0.0.0");
	awl->srv_port = AWLD_DEFAULT_UDP_PORT;
	strcpy(awl->cli_saddr, "0.0.0.0");
	awl->num_channels = AWL_MAX_CHANNELS;
	awl->sensor_version_major = SENSOR_VERSION_MAJOR;
	awl->sensor_version_minor = SENSOR_VERSION_MINOR;
	awl->checksum_ok = 1;
	awl->bist_ok = 1;
	awl->num_frames = AWL_MAX_FRAMES;
	awl->payload_size = AWL_PAYLOAD_SIZE_MAX;
	awl->current_frame = 0;
	if (awl->mmap_mode) {
		awl->mmap_fd = open("/dev/mem", O_RDWR);
		if (!awl->mmap_fd) {
			perror("INIT /dev/mem");
			close(awl->awl_fd);
			return -1;
		}
		awl->mmap = mmap(NULL, AWL_MEM_SIZE, PROT_READ|PROT_WRITE,
		                 MAP_SHARED|MAP_FIXED, awl->mmap_fd, AWL_MEM_BASE);
		if (!awl->mmap) {
			perror("INIT mmap");
			close(awl->mmap_fd);
			close(awl->awl_fd);
			return -1;
		}
	} else {
		awl->frames =
		    malloc(sizeof(void *) * awl->num_frames * AWL_FRAME_SIZE_MAX +
			   FILE_RECORD_HEADER_SIZE);
		if (!awl->frames) {
			perror("INIT frames");
			close(awl->awl_fd);
			return -1;
		}
		memset(awl->frames, 0,
		       awl->num_frames * AWL_FRAME_SIZE_MAX + FILE_RECORD_HEADER_SIZE);
	}
	awl->maxNumberOfDetections = DEFAULT_AWL_MAX_DETECTED;
	awl->historicSize = DEFAULT_HISTO_SIZE;
	awl->maxDetectionsInHistoric = DEFAULT_AWL_MAX_DETECTED_HISTO;
	awl->maxNumberOfTrackPerChannel = DEFAULT_AWL_MAX_TRACK_PER_CHANNEL;
	awl->maxObstacles = DEFAULT_AWL_MAX_OBSTACLES;
	awl->current_algo = 3;
	awl->current_track_algo = 1;
	awl->num_algos = AWL_NUM_ALGOS;
	awl->num_params = numparams;
	awl->num_paramstrack = numparamstrack;
	awl->algo_params = params;
	awl->track_algo_params = trackparams;
	awl->process_channel_mask = 0xffff;
	/* to be remove */
	awl->window_width=3;
	awl->window_qty=3;
	awl->algo_sma=0;
	awl->algo_persistence=0;
	awl->ab_alpha_pos=0; /*  act on position x */
	awl->ab_beta_pos=0;  /*  act on velocity v */
	awl->ab_alpha_speed=0; /*  act on position x */
	awl->ab_beta_speed=0;  /*  act on velocity v */
	/* to be remove */
	awl->track_max_object_speed=30; /*  validate an entry on ab filter m/s  */
	awl->can_transmit_cooked_event_mask = EVENT_MASK_OBSTACLE_COMPACT;
	//awl->can_transmit_cooked_event_mask = EVENT_MASK_OBSTACLE;
	awl->udp_transmit_cooked_event_mask = 0;

	awl->auto_calib_counter=1;
	awl->auto_calib=1000000;
	awl->default_beta=0.001;

	awl->dead_zone_override = -1;

	for (i = 0; i < AWL_MAX_CHANNELS; i++) {
		awl->mapped_channels[i] = i;
		awl->calibration_max[i] = 1;
		/* to be remove */
		awl->history_index[i] = 0;
		awl->sma_index[i] = 0;
		/* to be remove */
		awl->persist_index[i] = 0;
		awl->first_ts[i] = 1;
	}

	
	//  maybe a memset on this
    //  awl_track all_channel_track[AWL_NUM_CHANNELS][AWL_MAX_TRACK_PER_CHANNEL];
    for (i = 0; i < AWL_MAX_CHANNELS; i++) {
        	for (j = 0; j < DEFAULT_AWL_MAX_TRACK_PER_CHANNEL; j++) {
			awl->all_channel_track[i][j].xk_1 = 0;
			awl->all_channel_track[i][j].vk_1 = 0;
			awl->track_ttl_index[i][j] = -2;
		}
	}


	if (!awl->calibration_data) {
		awl->calibration_data =
		    malloc(AWL_MAX_CHANNELS * AWL_PAYLOAD_SIZE_MAX);
		if (awl->calibration_data) {
			memset(awl->calibration_data, 0,
			       AWL_MAX_CHANNELS * AWL_PAYLOAD_SIZE_MAX);
		} else {
			perror("INIT calibration_data");
			return -1;
		}
	}
	
	for (i = 0; i < MAX_TRACK; i++) {
 	      		awl->x_distance[i] =  (  i - awl->dead_zone )   *  SAMPLING_PERIOD/LIGHTSPEED;
 	}

	apply_config_file(awl);

	// Validate config parameters
	if(awl->maxNumberOfDetections > DEFAULT_AWL_MAX_DETECTED || awl->maxNumberOfDetections <= 0) {
		awl->maxNumberOfDetections = DEFAULT_AWL_MAX_DETECTED;
	}
	if(awl->historicSize > DEFAULT_HISTO_SIZE || awl->historicSize <= 0) {
		awl->historicSize = DEFAULT_HISTO_SIZE;
	}
	if(awl->maxNumberOfTrackPerChannel > DEFAULT_AWL_MAX_TRACK_PER_CHANNEL || awl->maxNumberOfTrackPerChannel <= 0) {
		awl->maxNumberOfTrackPerChannel = DEFAULT_AWL_MAX_TRACK_PER_CHANNEL;
	}

	awl->maxDetectionsInHistoric = awl->maxNumberOfDetections * awl->historicSize;
	awl->maxObstacles = AWL_MAX_CHANNELS * awl->maxNumberOfTrackPerChannel;

	if(awl->maxDetectionsInHistoric > DEFAULT_AWL_MAX_DETECTED_HISTO || awl->maxDetectionsInHistoric <= 0) {
		awl->maxDetectionsInHistoric = DEFAULT_AWL_MAX_DETECTED_HISTO;
	}
	if(awl->maxObstacles > DEFAULT_AWL_MAX_OBSTACLES || awl->maxObstacles <= 0) {
		awl->maxObstacles = DEFAULT_AWL_MAX_OBSTACLES;
	}

	return 0;
}
