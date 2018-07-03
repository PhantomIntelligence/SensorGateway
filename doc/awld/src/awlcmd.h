#ifndef AWL_CMD_H
#define AWL_CMD_H

#define CMD_RAW_0 0x80
#define CMD_RAW_1 0x81
#define CMD_RAW_2 0x82
#define CMD_RAW_3 0x83
#define CMD_RAW_4 0x84

#define CMD_SET_PARAMETER      0xC0
#define CMD_QUERY_PARAMETER    0xC1
#define CMD_RESPONSE_PARAMETER 0xC2
#define CMD_ERROR_PARAMETER    0xC3

#define CMD_RECORD_RAW         0xD0
#define CMD_PLAYBACK_RAW       0xD1
#define CMD_RECORD_CALIBRATION 0xDA
#define CMD_CLEAR_CALIBRATION  0xDB

#define CMD_TRANSMIT_RAW    0xE0
#define CMD_TRANSMIT_COOKED 0xE1

#define CMD_DEBUG_DUMP          0xF0
#define CMD_DEBUG_VERBOSE       0xF1
#define CMD_DEBUG_FORCE_REFRESH 0xF2

#define PTYPE_ALGO_SELECTED        0x01
#define PTYPE_ALGO_PARAMETER       0x02
#define PTYPE_AWL_REGISTER         0x03
#define PTYPE_BIAS                 0x04
#define PTYPE_ADC_REGISTER         0x05
#define PTYPE_PRESET               0x06
#define PTYPE_GLOBAL_PARAMETER     0x07
#define PTYPE_GPIO                 0x08
#define PTYPE_TRACK_ALGO_SELECTED  0x11
#define PTYPE_TRACK_ALGO_PARAMETER 0x12
#define PTYPE_DATE_TIME            0x20
#define PTYPE_OLD_TIME		   0x21 // deprecated
#define PTYPE_RECORD               0xD0
#define PTYPE_PLAYBACK             0xD1

#define PADDR_NULL 0
#define PADDR_DATE 1
#define PADDR_TIME 2
#define PADDR_TIMEZONE 3
#define PADDR_ONGOING 1
#define PADDR_NOT_ONGOING 2

#define PADDR_WINDOW_WIDTH     0x0011
#define PADDR_WINDOW_QTY       0x0012
#define PADDR_ALGO_SMA         0x0001
#define PADDR_ALGO_PERSISTANCE 0x0002

#define PADDR_AB_ALPHA_POS        0x0101
#define PADDR_AB_BETA_POS         0x0102
#define PADDR_AB_ALPHA_SPEED      0x0111 //obsolete
#define PADDR_AB_BETA_SPEED       0x0112 //obsolete
#define PADDR_AB_MAX_OBJECT_SPEED 0x0121

#define EVENT_MASK_OBSTACLE                  0x02 // deprecated, use 0x80 except for AWL7
#define EVENT_MASK_SENSOR_DISTANCE1_4        0x04 // deprecated, use 0x40
#define EVENT_MASK_SENSOR_DISTANCE5_8        0x08 // deprecated, use 0x40
#define EVENT_MASK_SENSOR_INTENSITY1_4       0x10 // deprecated, use 0x40
#define EVENT_MASK_SENSOR_INTENSITY5_8       0x20 // deprecated, use 0x40
#define EVENT_MASK_SENSOR_DISTANCE_INTENSITY 0x40
#define EVENT_MASK_OBSTACLE_COMPACT          0x80 // use 0x02 on AWL7

int process_cmd(agitwl_data *awl, int32_t id, uint8_t *buf, size_t size);
int send_msg(awl_data *awl, int32_t id, uint8_t *buf, size_t size);
int send_record(awl_data *awl);
int send_playback(awl_data *awl);
int send_record_calibration(awl_data *awl);
int send_clear_calibration(awl_data *awl);
int send_set_parameter(awl_data *awl);
int send_query_parameter(awl_data *awl);
int send_distance(awl_data *awl, int channel, uint16_t *distance_buffer);
int send_intensity(awl_data *awl, int channel, uint16_t *intensity_buffer);
int send_distance_intensity(awl_data *awl, int channel, uint16_t *distance_buffer, uint16_t *intensity_buffer);
int send_obstacle(awl_data *awl, awl_obstacle *obstacle);
int send_frame_done(awl_data *awl);
int send_debug_dump(awl_data *awl);
int send_debug_verbose(awl_data *awl);
int send_debug_force_refresh(awl_data *awl);

#endif
