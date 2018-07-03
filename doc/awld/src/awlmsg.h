#ifndef AWL_MSG_H
#define AWL_MSG_H

#define MSG_SENSOR_MONITOR_STATUS 1
#define MSG_SENSOR_BOOT           2
#define MSG_GAIN_CH1_4            3
#define MSG_GAIN_CH5_8            4
#define MSG_FRAME_DONE            9
#define MSG_OBSTACLE_TRAK        10
#define MSG_OBSTACLE_VELOCITY    11
#define MSG_OBSTACLE_SIZE        12
#define MSG_OBSTACLE_POSITION    13
#define MSG_CH1_DISTANCE1_4  20
#define MSG_CH2_DISTANCE1_4  21
#define MSG_CH3_DISTANCE1_4  22
#define MSG_CH4_DISTANCE1_4  23
#define MSG_CH5_DISTANCE1_4  24
#define MSG_CH6_DISTANCE1_4  25
#define MSG_CH7_DISTANCE1_4  26
#define MSG_CH1_DISTANCE5_8  30
#define MSG_CH2_DISTANCE5_8  31
#define MSG_CH3_DISTANCE5_8  32
#define MSG_CH4_DISTANCE5_8  33
#define MSG_CH5_DISTANCE5_8  34
#define MSG_CH6_DISTANCE5_8  35
#define MSG_CH7_DISTANCE5_8  36
#define MSG_CH1_INTENSITY1_4 40
#define MSG_CH2_INTENSITY1_4 41
#define MSG_CH3_INTENSITY1_4 42
#define MSG_CH4_INTENSITY1_4 43
#define MSG_CH5_INTENSITY1_4 44
#define MSG_CH6_INTENSITY1_4 45
#define MSG_CH7_INTENSITY1_4 46
#define MSG_CH1_INTENSITY5_8 50
#define MSG_CH2_INTENSITY5_8 51
#define MSG_CH3_INTENSITY5_8 52
#define MSG_CH4_INTENSITY5_8 53
#define MSG_CH5_INTENSITY5_8 54
#define MSG_CH6_INTENSITY5_8 55
#define MSG_CH7_INTENSITY5_8 56
#define MSG_CH_DISTANCE_INTENSITY 60
#define MSG_CONTROL_GROUP    80
// 0x60 = 96
#define MSG_FROM_IPC         0x61
#define MSG_FROM_UDP         0x62

#define HW_ERR_TX1 (1<<0)
#define HW_ERR_TX2 (1<<1)
#define HW_ERR_RX  (1<<2)
#define HW_ERR_DSP (1<<3)
#define HW_ERR_MEM (1<<4)

#define RX_ERR_CH1 (1<<0)
#define RX_ERR_CH2 (1<<1)
#define RX_ERR_CH3 (1<<2)
#define RX_ERR_CH4 (1<<3)
#define RX_ERR_CH5 (1<<4)
#define RX_ERR_CH6 (1<<5)
#define RX_ERR_CH7 (1<<6)
#define RX_ERR_MON (1<<7)

#define RE_ERR_BIST           (1<<0)
#define RE_ERR_SHUTDOWN       (1<<1)
#define RE_ERR_SENSOR_BLOCKED (1<<2)
#define RE_ERR_REDUCED_PERF   (1<<3)
#define RE_ERR_SATURATION     (1<<4)

#endif
