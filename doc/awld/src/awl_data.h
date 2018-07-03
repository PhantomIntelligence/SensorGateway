#ifndef AWL_DATA_H
#define AWL_DATA_H

#include <stdint.h>
#include <stdio.h>
#include <limits.h>
#ifdef __linux__
#include <netinet/in.h>
#include <linux/can.h>
#include <linux/if_packet.h>
#endif

#define SENSOR_VERSION_MAJOR 0
#define SENSOR_VERSION_MINOR 1

#define LIGHTSPEED    6.66928e-9  // s/m round trip
#define SAMPLING_PERIOD 2.5e-9    // s per sample
#define MAX_TRACK 1024            // sample per frame

#define AWL_MEM_BASE 0x1FF00000
#define AWL_MEM_SIZE 0x80000

#define AWL_MAX_CHANNELS 16
#define AWL_MAX_FRAMES (AWL_MAX_CHANNELS * 2)
#define SADDR_SIZE 128

#define AWLD_DEBUG
#define AWLD_DEBUG_CAN
#define AWLD_DEBUG_AWL

#define AWLD_DEFAULT_UDP_PORT 23456
#define MINIMUM_SAMPLE_FOR_NOISE 30 // Minimum sample

#define FILE_RECORD_HEADER_SIZE 8
#define FILE_CHANNEL_HEADER_SIZE 8
#define ACQUISITION_DELAY  100
#define SYSTEM_OFFSET -50.7

#define AWL_PAYLOAD_SIZE_MAX 4096
#define AWL_PAYLOAD_HEADER_SIZE_32 4
#define AWL_PAYLOAD_TRAILER_SIZE_32 5
#define AWL_PAYLOAD_HEADER_SIZE (AWL_PAYLOAD_HEADER_SIZE_32 * sizeof(uint32_t))
#define AWL_PAYLOAD_TRAILER_SIZE (AWL_PAYLOAD_TRAILER_SIZE_32 * sizeof(uint32_t))

#define AWL_PAYLOAD_SIZE_CURRENT (awl->payload_size)
#define AWL_PAYLOAD_SIZE_RECORD (awl->record_payload_size)
#define AWL_PAYLOAD_SIZE_PLAYBACK (awl->playback_payload_size)
#define AWL_PAYLOAD_SIZE_32 (AWL_PAYLOAD_SIZE_CURRENT / sizeof(uint32_t)) 
#define AWL_PAYLOAD_SIZE_RECORD_32 (AWL_PAYLOAD_SIZE_RECORD / sizeof(uint32_t)) 
#define AWL_PAYLOAD_SIZE_PLAYBACK_32 (AWL_PAYLOAD_SIZE_PLAYBACK / sizeof(uint32_t)) 
#define AWL_PAYLOAD_SIZE_S32 (AWL_PAYLOAD_SIZE_CURRENT / sizeof(int32_t))

#define AWL_FRAME_HEADER_SIZE 8
#define AWL_FRAME_SIZE (AWL_FRAME_HEADER_SIZE + AWL_PAYLOAD_SIZE_CURRENT)
#define AWL_FRAME_SIZE_MAX (AWL_FRAME_HEADER_SIZE + AWL_PAYLOAD_SIZE_MAX)

#define UDP_HEADER_SIZE 16
#define UDP_PAYLOAD_SIZE_MAX AWL_PAYLOAD_SIZE_CURRENT
#define UDP_FRAME_SIZE_MAX (UDP_HEADER_SIZE + UDP_PAYLOAD_SIZE_MAX)
#define UDP_DEFAULT_NUM_DATAGRAM 4
#define UDP_DEFAULT_PAYLOAD_SIZE (AWL_PAYLOAD_SIZE_MAX / UDP_DEFAULT_NUM_DATAGRAM)
#define UDP_PAYLOAD_SIZE (awl->udp_payload_size)
#define UDP_FRAME_SIZE (UDP_HEADER_SIZE + UDP_PAYLOAD_SIZE)

#define CURRENT_AWL_FRAME (awl->frames + AWL_FRAME_SIZE_MAX * awl->current_frame)
#define CURRENT_AWL_PAYLOAD (CURRENT_AWL_FRAME + AWL_FRAME_HEADER_SIZE)
#define CURRENT_AWL_FRAME_32 ((uint32_t*)(CURRENT_AWL_FRAME)) 
#define CURRENT_AWL_PAYLOAD_32 ((uint32_t*)(CURRENT_AWL_PAYLOAD))
#define CURRENT_AWL_FRAME_S32 ((int32_t*)(CURRENT_AWL_FRAME)) 
#define CURRENT_AWL_PAYLOAD_S32 ((int32_t*)(CURRENT_AWL_PAYLOAD))

#define     CURRENT_AWL_CMD         (CURRENT_AWL_FRAME[0])
#define GET_CURRENT_AWL_CHANNEL    ((CURRENT_AWL_PAYLOAD[4])>>4)
#define SET_CURRENT_AWL_CHANNEL(x)  {CURRENT_AWL_PAYLOAD[4]&=0x0f;\
                                     CURRENT_AWL_PAYLOAD[4]|=(x<<4)&0xf0;}
#define GET_CURRENT_AWL_ACQ_MODE   ((CURRENT_AWL_PAYLOAD[5]&0x30)>>4)
#define GET_CURRENT_AWL_SIGNEDNESS ((CURRENT_AWL_PAYLOAD[5]&0x08))
#define GET_CURRENT_AWL_LZ_ENA     ((CURRENT_AWL_PAYLOAD[5]&0x07))
#define     CURRENT_AWL_ACCUM       (CURRENT_AWL_PAYLOAD[6])
#define     CURRENT_AWL_CHKSM       (CURRENT_AWL_PAYLOAD[7])
#define     CURRENT_AWL_TIMESTAMP   (CURRENT_AWL_PAYLOAD_32[0])
#define     CURRENT_AWL_STATUS      (CURRENT_AWL_PAYLOAD_32[1])

#define     CURRENT_AWL_PAYLOAD_TRAILER     (CURRENT_AWL_PAYLOAD + AWL_PAYLOAD_SIZE_CURRENT - AWL_PAYLOAD_TRAILER_SIZE)
#define     CURRENT_AWL_PAYLOAD_TRAILER_MAX (CURRENT_AWL_PAYLOAD + AWL_PAYLOAD_SIZE_MAX - AWL_PAYLOAD_TRAILER_SIZE)
#define     CURRENT_AWL_PAYLOAD_TRAILER_32  ((int32_t*)(CURRENT_AWL_PAYLOAD_TRAILER))

#define     CURRENT_AWL_TAIL_CHANNEL         (CURRENT_AWL_PAYLOAD[AWL_PAYLOAD_SIZE_CURRENT-4])
#define     CURRENT_AWL_TAIL_PAGE            (CURRENT_AWL_PAYLOAD[AWL_PAYLOAD_SIZE_CURRENT-3])
#define     CURRENT_AWL_TAIL_ISR             (CURRENT_AWL_PAYLOAD[AWL_PAYLOAD_SIZE_CURRENT-1])
#define     CURRENT_AWL_TAIL_PAGE_COUNTERS   (CURRENT_AWL_PAYLOAD_32[AWL_PAYLOAD_SIZE_32 - 3])
#define     CURRENT_AWL_TAIL_RECORD_COUNTERS (CURRENT_AWL_PAYLOAD_32[AWL_PAYLOAD_SIZE_32 - 4])

#define GET_CURRENT_AWL_TAIL_IRQ_TOP_PAGE_COUNTER       (CURRENT_AWL_TAIL_PAGE_COUNTERS & 0xffff)
#define GET_CURRENT_AWL_TAIL_IRQ_BOTTOM_PAGE_COUNTER    (CURRENT_AWL_TAIL_PAGE_COUNTERS >> 16)
#define GET_CURRENT_AWL_TAIL_CHANNEL_RECEIVE_COUNTER    (CURRENT_AWL_TAIL_RECORD_COUNTERS & 0xffff)
#define SET_CURRENT_AWL_TAIL_CHANNEL_RECEIVE_COUNTER(x) {CURRENT_AWL_TAIL_RECORD_COUNTERS &= 0xffff;\
                                                         CURRENT_AWL_TAIL_RECORD_COUNTERS |= x & 0xffff;}
#define GET_CURRENT_AWL_TAIL_FILE_RECORD_COUNTER        (CURRENT_AWL_TAIL_RECORD_COUNTERS >> 16)
#define SET_CURRENT_AWL_TAIL_FILE_RECORD_COUNTER(x)     {CURRENT_AWL_TAIL_RECORD_COUNTERS &= 0xffff0000;\
                                                         CURRENT_AWL_TAIL_RECORD_COUNTERS |= x << 16;}

#define CURRENT_UDP_FRAME(frame) (CURRENT_AWL_FRAME + frame * UDP_PAYLOAD_SIZE)
#define CURRENT_UDP_FRAME_32(frame) ((uint32_t*)(CURRENT_UDP_FRAME(frame)))
#define CURRENT_UDP_FRAME_S32(frame) ((int32_t*)(CURRENT_UDP_FRAME(frame)))

#define CURRENT_UDP_CMD(frame) (CURRENT_UDP_FRAME(frame)[0])
#define CURRENT_UDP_FRAMERATE(frame) (CURRENT_UDP_FRAME(frame)[1])
#define CURRENT_UDP_CHANNEL(frame) (CURRENT_UDP_FRAME(frame)[2])
#define CURRENT_UDP_TIMESTAMP(frame) (CURRENT_UDP_FRAME_32(frame)[1])

#define DEFAULT_AWL_MAX_DETECTED 		4
#define DEFAULT_HISTO_SIZE 				16
#define DEFAULT_AWL_MAX_DETECTED_HISTO 	( DEFAULT_AWL_MAX_DETECTED * DEFAULT_HISTO_SIZE ) 
#define ALGO2_PARAM0_OFFSET				5.0
#define ALGO2_PARAM1_THRESHOLD			0.35
#define ALGO2_PARAM2_FIRSTDAMPING		1.3
#define ALGO2_PARAM3_NUMWIDTH			2.0
#define ALGO2_PARAM4_INTENSITYFILTER	8.0
#define ALGO2_PARAM5_SECONDWIDTH		8.0
#define ALGO2_PARAM6_UNDEFINED			0.0
#define ALGO2_PARAM7_SNRMIN				16.0
#define ALGO3_PARAM0_OFFSET				0.0
#define ALGO3_PARAM1_THRESHOLD			0.24
#define ALGO3_PARAM2_1STDERIVATEORDER	2.0
#define ALGO3_PARAM3_2NDDERIVATEORDER	8.0
#define ALGO3_PARAM4_INTENSITYFILTER	0.0
#define ALGO3_PARAM5_UNDEFINED			0.0
#define ALGO3_PARAM6_FIRSTINDEX			28.0
#define ALGO3_PARAM7_SNRMIN				24.0
#define ALGO4_PARAM0_OFFSET				0.0
#define ALGO4_PARAM1_UNDEFINED			0.0
#define ALGO4_PARAM2_DAMPING			1.3
#define ALGO4_PARAM3_MEANWIDTH			3.0
#define ALGO4_PARAM4_SOURCEWIDTH		7.0
#define ALGO4_PARAM5_DERIVATEWIDTH		5.0
#define ALGO4_PARAM6_PROCESS			1.0
#define ALGO4_PARAM7_SNRMIN				18.0
#define ALGO5_PARAM0_OFFSET				0.0
#define ALGO5_PARAM1_SOURCEWIDTH		7.0
#define ALGO5_PARAM2_NBDIAGS			4.0
#define ALGO5_PARAM3_MAXCOUNTEVD		10.0
#define ALGO5_PARAM4_HISTORYLENGTH		4.0
#define ALGO5_PARAM5_MAXNUMBEROFSOURCES 0.0
#define ALGO5_PARAM6_UNDEFINED			0.0
#define ALGO5_PARAM7_SNRMIN				10.0
#define ALGO6_PARAM0_OFFSET				0.0
#define ALGO6_PARAM1_STDFACTOR			0.4
#define ALGO6_PARAM2_DAMPING			1.3
#define ALGO6_PARAM3_SOURCEWIDTH		7.0
#define ALGO6_PARAM4_DERIVATEWIDTH		5.0
#define ALGO6_PARAM5_SNRMIN				16.0
#define DEFAULT_AWL_MAX_TRACK_PER_CHANNEL 2
#define DEFAULT_AWL_MAX_OBSTACLES 		(AWL_MAX_CHANNELS * DEFAULT_AWL_MAX_TRACK_PER_CHANNEL)
#define TRACKERS_MAX_OBJECT_SPEED		30 // cm/s
#define ABFILTER_PARAM0_ALPHAPOS		0.2
#define ABFILTER_PARAM1_BETAPOS			0.015
#define ABFILTER_PARAM2_ALPHASPEED		0.6
#define ABFILTER_PARAM3_BETASPEED		0.5
#define ABFILTER_PARAM4_PERSISTANCE		3
#define LRFILTER_PARAM0_MAX_SNR_TO_PAIR	20
#define LRFILTER_PARAM1_MIN_SNR_TO_TRACK 25



#define AWL_DEV_PROC_PATH  "/proc/awl/0/registers"
#define AWL_ADC_PROC_PATH  "/proc/awl/0/adc"
#define AWL_BIAS_PROC_PATH "/proc/awl/0/bias"

#define AWL_NUM_ALGOS 11
#define AWL_NUM_PARAMS 20
#define AWL_NUM_TRACK_ALGOS 3

#define ALGO_N_PARAM(m,n) ((awl->algo_params[m])[n])
#define ALGO_PARAM(n) ((awl->algo_params[awl->current_algo])[n])
#define ALGO_HAS_PARAM(n) (awl->num_params[awl->current_algo] > n)
#define ALGO_N_HAS_PARAM(n,m) (awl->num_params[n] > m)
#define ALGO_PARAM_X(n) ((awl.algo_params[awl.current_algo])[n])
#define ALGO_N_PARAM_X(m,n) ((awl.algo_params[m])[n])
#define ALGO_HAS_PARAM_X(n) (awl.num_params[awl.current_algo] > n)
#define ALGO_N_HAS_PARAM_X(m,n) (awl.num_params[m] > n)

#define TRACKALGO_N_PARAM(m,n) ((awl->track_algo_params[m])[n])
#define TRACKALGO_PARAM(n) ((awl->track_algo_params[awl->current_track_algo])[n])
#define TRACKALGO_HAS_PARAM(n) (awl->num_paramstrack[awl->current_track_algo] > n)
#define TRACKALGO_N_HAS_PARAM(n,m) (awl->num_paramstrack[n] > m)
#define TRACKALGO_PARAM_X(n) ((awl.track_algo_params[awl.current_track_algo])[n])
#define TRACKALGO_N_PARAM_X(m,n) ((awl.track_algo_params[m])[n])
#define TRACKALGO_HAS_PARAM_X(n) (awl.num_paramstrack[awl.current_track_algo] > n)
#define TRACKALGO_N_HAS_PARAM_X(m,n) (awl.num_paramstrack[m] > n)

#define CALIBRATION_DATA(x) (awl->calibration_data + x * AWL_PAYLOAD_SIZE_CURRENT)
#define CALIBRATION_DATA_32(x) ((uint32_t*)CALIBRATION_DATA(x))
#define CALIBRATION_DATA_S32(x) ((int32_t*)CALIBRATION_DATA(x))

#define CALIBRATION_MODE_NONE 0
#define CALIBRATION_MODE_START 1
#define CALIBRATION_MODE_USE 2
#define CALIBRATION_MODE_FINISH 3
#define CALIBRATION_MODE_LIVE 4

#define MAX_LINE_LEN 256
#define MAX_NB_VAR 126 
#define VAR_NAME_MAX 64
#define VALUE_STRING_MAX  4096 
#define AWL_CONFIG_FILENAME "/etc/aerostar/awld.conf"

typedef struct
{
    char parameter[VAR_NAME_MAX+1];
    char value[VALUE_STRING_MAX+1];
}strparameter;

typedef struct
{
    char parameter[VAR_NAME_MAX+1];
    float value;
}floatparameter;

typedef struct
{
    char parameter[VAR_NAME_MAX+1];
    int value;
}intparameter;

/* to be remove */
typedef struct histogram {
	int qty;
        float bin;  	
	float meters;
	int intensity;
} histogram;
/* to be remove */

typedef struct awl_track {
	int track_id;            
	int index_no;              /* -2, -1  not reported,  first report 1 to  n report*/
	int position;  		/* cm */
	int velocity;  		/* cm/s */
	int acceleration; 	/* cm/s^2  */
	int intensity;
	int prob_track;		/* 0-100 */
	int collision;		/* ms */
	int ang_start;		/* -900 to +900 1/10 degree */
	int ang_end;		/* -900 to +900 1/10 degree */
	int ang_speed;		/* 1/10 degree/s */
	uint16_t  channel;
    	float xk_1;  /*  x estimate  */
    	float vk_1;  /*  v estimate  */
    	float xk_2;  /*  x estimate velocity */
    	float vk_2;  /*  v estimate velocity */

} awl_track;

typedef struct awl_distance {
	float meters;
	int intensity;
	int index;
	uint32_t value;
} awl_distance;

typedef struct awl_obstacle {
	uint16_t track_id;
	uint16_t  channel;
	uint16_t  track_type;
	uint8_t  track_probability;
	uint16_t time_to_collision_estimate;
	uint16_t track_distance;
	int16_t  track_speed;
	int16_t  track_acceleration;
	uint16_t object_height_max;
	uint16_t object_width_max;
	uint16_t object_intensity;
	int16_t  object_start_angle;
	int16_t  object_end_angle;
	int16_t  object_angular_velocity;
} awl_obstacle;

typedef struct {
	int udp_fd;
	int can_fd;
	int awl_fd;
	int gpib_fd;
	int mmap_fd;
	FILE *record_fp;
	FILE *playback_fp;
	struct sockaddr_in srv_addr;
	struct sockaddr_in cli_addr;
	struct sockaddr_in stream_cli_addr;
#ifdef __linux__
	struct sockaddr_can can_addr;
	struct sockaddr_ll awl_addr;
#endif
	float frequency;
	int srv_port;
	int cli_port;
	char cli_saddr[SADDR_SIZE];
	char srv_saddr[SADDR_SIZE];
	char can_ifname[PATH_MAX];
	char awl_ifname[PATH_MAX];
	char gpib_devname[PATH_MAX];
	char record_filename[PATH_MAX];
	char playback_filename[PATH_MAX];
	char calibration_filename[PATH_MAX];
	char config_filename[PATH_MAX];
	char post_calibration_script[PATH_MAX];
	char post_recording_script[PATH_MAX];
	char playback_path_prefix[PATH_MAX];
	char record_path_prefix[PATH_MAX];
	int config_dump;
	int verbose_udp;
	int verbose_can;
	int verbose_awl;
	int verbose_gpib;
	int verbose_distance;
	int verbose_playback;
	int num_channels;
	int num_frames;
	int payload_size;
	int raw_payload_size;
	int udp_payload_size;
	int record_payload_size;
	int playback_payload_size;
	int udp_num_datagram;
	int current_frame;
	int sensor_version_major;
	int sensor_version_minor;
	int checksum_ok;
	int bist_ok;
	int hardware_error;
	int receiver_error;
	int reporting_status;
	uint8_t receiving_playback_filename;
	uint8_t receiving_record_filename;
	uint8_t heartbeat;
	uint8_t calibration_mode;
	uint8_t mmap_mode;
	uint8_t client_mode;
	uint8_t client_command;
	uint8_t client_ptype;
	uint8_t pad_pad;
	uint16_t client_address;
	uint32_t client_value;
	uint16_t process_channel_mask;
	uint16_t udp_transmit_raw_channel_mask;
	uint16_t udp_transmit_cooked_channel_mask;
	uint16_t udp_transmit_cooked_event_mask;
	uint16_t udp_transmit_cooked_event_limit;
	uint16_t can_transmit_cooked_channel_mask;
	uint16_t can_transmit_cooked_event_mask;
	uint16_t can_transmit_cooked_event_limit;
	uint16_t record_channel_mask;
	uint16_t record_real_channel_mask;
	uint16_t playback_channel_mask;
	uint16_t frame_rate;
	uint16_t record_frame_rate;
	uint16_t udp_transmit_raw_frame_rate;
	uint16_t udp_transmit_cooked_frame_rate;
	uint16_t can_transmit_cooked_frame_rate;
	uint16_t can_transmit_cooked_track_limit;
	uint16_t awl_frame_counter;
	uint16_t awl_track_counter;
	int dead_zone;
	int dead_zone_override;
	int record_num_channels;
	int record_real_num_channels;
	int record_num_records;
	int record_pos;
	uint32_t record_last_ts;
	int playback_num_records;
	int playback_loop_count;
	int playback_loop_max;
	uint8_t *frames;
	uint8_t *mmap;
	int current_algo;
	int current_track_algo;
	int maxNumberOfDetections;
	int historicSize;
	int maxNumberOfTrackPerChannel;
	int maxDetectionsInHistoric;
	int maxObstacles;
	int num_algos;
	int *num_params;
	int *num_paramstrack;
	float **algo_params;
	float **track_algo_params;
	uint8_t mapped_channels[AWL_MAX_CHANNELS];
	awl_distance detected[AWL_MAX_CHANNELS][DEFAULT_AWL_MAX_DETECTED];
	awl_distance detected_history[AWL_MAX_CHANNELS][DEFAULT_AWL_MAX_DETECTED_HISTO];
	/* to be remove */
	//histogram  detected_sma[AWL_MAX_CHANNELS][AWL_MAX_DETECTED_HISTO];
	//histogram  detected_persist[AWL_MAX_CHANNELS][AWL_MAX_DETECTED_HISTO];
	int history_index[AWL_MAX_CHANNELS];
	int sma_index[AWL_MAX_CHANNELS];
	/* to be remove */
	awl_track all_channel_track[AWL_MAX_CHANNELS][DEFAULT_AWL_MAX_TRACK_PER_CHANNEL];
	int track_ttl_index[AWL_MAX_CHANNELS][DEFAULT_AWL_MAX_TRACK_PER_CHANNEL];
	int persist_index[AWL_MAX_CHANNELS];
	int calibration_count[AWL_MAX_CHANNELS];
	int calibration_max[AWL_MAX_CHANNELS];
	float calibration_beta[AWL_MAX_CHANNELS];
	uint8_t *calibration_data;
	float x_distance[MAX_TRACK];
	strparameter  strparm[MAX_NB_VAR];
	intparameter  intparm[MAX_NB_VAR];
	floatparameter  floatparm[MAX_NB_VAR];
	/* to be remove */
	int window_width;
	int window_qty;
	int algo_sma;
	int algo_persistence;
	/* alpha beta parameters */
    float ab_alpha_pos; /*  act on position x */
    float ab_beta_pos;  /*  act on velocity v */
    float ab_alpha_speed; /*  act on position x */
    float ab_beta_speed;  /*  act on velocity v */
	/* to be remove */
	float track_max_object_speed; /*  validate an entry on ab filter cm/s  */
	int auto_calib_counter;
	int auto_calib;
	float default_beta;
	uint16_t last_irq_top_page_counters[AWL_MAX_CHANNELS];
	uint16_t last_irq_bottom_page_counters[AWL_MAX_CHANNELS];
	uint32_t last_ts[AWL_MAX_CHANNELS];
	uint32_t last_st[AWL_MAX_CHANNELS];
	uint32_t first_ts[AWL_MAX_CHANNELS];
	uint16_t last_rcv[AWL_MAX_CHANNELS];
	uint16_t last_rcn[AWL_MAX_CHANNELS];
	awl_obstacle obstacles[DEFAULT_AWL_MAX_OBSTACLES];
} awl_data;

int init_awl_data(awl_data *awl);

#endif
