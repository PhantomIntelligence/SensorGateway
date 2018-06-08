/****************************************************************
**
**  definition for alarms 
**
**  Project: Guardian
**  Authors: ETR/EMO
**  creation: 2016-02-06
**
**  Revision:
**  date     |  author   |  comment
**  ---------|-----------|-------------------------------------
**
****************************************************************/

#ifndef GUARDIAN_ALARMS
#define GUARDIAN_ALARMS

#define GUARDIAN_ALARM_NUMCHANNEL 16  // number of channel in sensor... should be defined elsewhere....
#define GUARDIAN_ALARM_NUMDETECTION 4 // number of maximum detection per channel 
#define GUARDIAN_ALARM_NUMALARMS  8   // number of available alarm on system (cannot exceed the length of and integer)
#define GUARDIAN_ALARM_NUMTRIGGER 2   // number of trigger on the sensor (cannot exceed the length of and integer)

#define GUARDIAN_ALARM_WAIT_TIME 10   // number of frame to hold the trigger after the conditions are not met anymore.


//distances between 0m and 500m
#define GUARDIAN_ALARM_MINDIST  0
#define GUARDIAN_ALARM_MAXDIST 500
//velocity range between -100 m/s and 100 m/s (360km/h)
#define GUARDIAN_ALARM_MINVEL  -100
#define GUARDIAN_ALARM_MAXVEL   100

#define GUARDIAN_ALARM_TRIGGER_MODE_OR  0
#define GUARDIAN_ALARM_TRIGGER_MODE_AND 1


struct AlarmConditions {
	float dist_min[GUARDIAN_ALARM_NUMCHANNEL];
	float dist_max[GUARDIAN_ALARM_NUMCHANNEL];
	float vel_min[GUARDIAN_ALARM_NUMCHANNEL];
	float vel_max[GUARDIAN_ALARM_NUMCHANNEL];
};
struct TriggerCondition{
	int triggerMask;
	int triggerMode;
};
struct AlarmSetting{
	struct AlarmConditions alarms[GUARDIAN_ALARM_NUMALARMS];
	struct TriggerCondition triggers[GUARDIAN_ALARM_NUMTRIGGER];
	int triggerMask[GUARDIAN_ALARM_NUMTRIGGER];
	int triggerMode;
};

struct AlarmData{
	float distance;
	float velocity;
	float intensity;
	int   ID;
};

//extern struct AlarmSetting alarmConditions;
//extern struct AlarmData detections[GUARDIAN_ALARM_NUMCHANNEL][GUARDIAN_ALARM_NUMDETECTION];

// Init_Alarm: put default values in the alarm condition
// il devrait y avoir un element optionnel pour passer une struct pour ne pas avoir les param par defaut
void Init_Alarm(); 

// CheckAlarms: validate if the alarm conditions are met.
//              receives the detection data from all sensors
//              returns a bitfield with the active alarms
int CheckAlarms(struct AlarmData* pData);

// CheckTriggers: validate if trigger conditions are met
//                returns the bitfield of active triggers
int CheckTriggers();


#endif
