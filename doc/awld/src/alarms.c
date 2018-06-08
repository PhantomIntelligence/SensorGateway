/****************************************************************
**
**  declaration for alarms 
**
**  GUARDIAN_ALARM_Project: Guardian
**  GUARDIAN_ALARM_Authors: ETR/EMO
**  creation: 2016-02-06
**
**  GUARDIAN_ALARM_Revision:
**  date     |  author   |  comment
**  ---------|-----------|-------------------------------------
**
****************************************************************/
#include "alarms.h"

struct AlarmSetting alarmConditions;
struct AlarmData detections[GUARDIAN_ALARM_NUMCHANNEL][GUARDIAN_ALARM_NUMDETECTION];

//global variables
struct AlarmSetting alarmCondition;
int iTriggers, iAlarms = 0;


// GUARDIAN_ALARM_Init_Alarm: put default values in the alarm condition
// il devrait y avoir un element optionnel pour passer une struct pour ne pas avoir les param par defaut
void GUARDIAN_ALARM_Init_Alarm()
{
	int a,c,t = 0;
	for (a=0;a<GUARDIAN_ALARM_NUMALARMS;a++){
		for (c=0;c<GUARDIAN_ALARM_NUMCHANNEL;c++){
			alarmConditions.alarms[a].dist_min[c] = GUARDIAN_ALARM_MINDIST;
			alarmConditions.alarms[a].dist_max[c] = GUARDIAN_ALARM_MINDIST;
			alarmConditions.alarms[a].vel_min[c]  = GUARDIAN_ALARM_MINVEL;
			alarmConditions.alarms[a].vel_max[c]  = GUARDIAN_ALARM_MAXVEL;
		}
	}
	for (t=0;t<GUARDIAN_ALARM_NUMTRIGGER;t++)
		alarmConditions.triggers[t].triggerMask=0x0000000; // no condition
	alarmConditions.trigger.triggerMode = 0x0000000; // all set to GUARDIAN_ALARM_OR
} 

// GUARDIAN_ALARM_CheckAlarms: validate if the alarm conditions are met.
//              receives the detection data from all sensors
//              returns a bitfield with the active alarms
int GUARDIAN_ALARM_CheckAlarms(struct AlarmData* pData)
{
	int a,c,d; // alarm, channel and detection index
	float mind,maxd,minv,maxv,dist,vel = 0;
		
	for (a=0;a<GUARDIAN_ALARM_NUMALARMS;a++)
	{
		for (c=0;c<GUARDIAN_ALARM_NUMCHANNEL;c++)
		{
			for (d=0;d<GUARDIAN_ALARM_NUMDETECTION;d++)
			{
				dist = detections[c][d].distance;
				if(dist)  //valid detection
				{
					// put data into registers (temps variables)
					mind = alarmConditions.alarms.dist_min[c];
					maxd = alarmConditions.alarms.dist_max[c];
					minv = alarmConditions.alarms.vel_min[c];
					maxv = alarmConditions.alarms.vel_max[c];
					vel  = detections[c][d].velocity;
					
					if ((mind && maxd) && (mind < maxd)) && ((mind<dist) && (maxd>dist)) // inclusive
						iAlarms |= 0x0001<<a;
					else if ((mind && maxd) && (mind > maxd)) && ((mind<dist) || (maxd>dist)) // exclusive
						iAlarms |= 0x0001<<a;
					else if (mind && mind < dist) || (maxd && maxd > dist)// one or the other
						iAlarms |= 0x0001<<a;						
					else
						iAlarms &= ~(0x0001<<a);
						
					if(minv && minv>vel) || (maxv && maxv<vel) //check if alarm was set on distance, clear if velocity does not match
						iAlarms &= ~(0x0001<<a);
				}
				else 
					break;
			}
		}
	}
		
}
// GUARDIAN_ALARM_CheckTriggers: validate if trigger conditions are met
//                returns the bitfield of active triggers
int GUARDIAN_ALARM_CheckTriggers()
{
	static enum {GUARDIAN_ALARM_OFF,GUARDIAN_ALARM_LAT,GUARDIAN_ALARM_ON,GUARDIAN_ALARM_WAIT} state[GUARDIAN_ALARM_NUMTRIGGER];
	static int timer[GUARDIAN_ALARM_NUMTRIGGER];
	int t;
	
	for (t=0;t<GUARDIAN_ALARM_NUMTRIGGER;t++)
	{
		switch(state[t])
		{
			case GUARDIAN_ALARM_LAT: // wait to confirm that it is not a rogue alarm
				iTriggers &= ~(0x0001<<t); // sets trigger to 0.
				state[t]=GUARDIAN_ALARM_OFF;// default next state
				if(alarmConditions.triggerMask[t] && iAlarms)
				{
					if(((alarmConditions.triggerMode&(0x0001<<t)) == GUARDIAN_ALARM_TRIGGER_MODE_OR<<t) && (iAlarms&alarmConditions.triggerMask[t]))
						state[t]=GUARDIAN_ALARM_ON;
					else if(((alarmConditions.triggerMode&(0x0001<<t)) == GUARDIAN_ALARM_TRIGGER_MODE_AND<<t) && (iAlarms&alarmConditions.triggerMask[t])==alarmConditions.triggerMask[t])	
						state[t]=GUARDIAN_ALARM_ON;
				}
				break;
			case GUARDIAN_ALARM_ON: //activate the trigger
				iTriggers |= (0x0001<<t); // sets trigger to 1.
				state[t]=GUARDIAN_ALARM_WAIT;// default next state
				timer[t]=GUARDIAN_ALARM_WAIT_TIME;
				if(alarmConditions.triggerMask[t] && iAlarms)
				{
					if(((alarmConditions.triggerMode&(0x0001<<t)) == GUARDIAN_ALARM_TRIGGER_MODE_OR<<t) && (iAlarms&alarmConditions.triggerMask[t]))
						state[t]=GUARDIAN_ALARM_ON;
					else if(((alarmConditions.triggerMode&(0x0001<<t)) == GUARDIAN_ALARM_TRIGGER_MODE_AND<<t) && (iAlarms&alarmConditions.triggerMask[t])==alarmConditions.triggerMask[t])	
						state[t]=GUARDIAN_ALARM_ON;
				}
				break;
			case GUARDIAN_ALARM_WAIT: // hold the trigger for certain time to prenvent glitches
				if(--timer[t])
				{
					iTriggers |= (0x0001<<t); // sets trigger to 1.
					state[t]=GUARDIAN_ALARM_WAIT;
					if(alarmConditions.triggerMask[t] && iAlarms)
					{
						if(((alarmConditions.triggerMode&(0x0001<<t)) == GUARDIAN_ALARM_TRIGGER_MODE_OR<<t) && (iAlarms&alarmConditions.triggerMask[t]))
							state[t]=GUARDIAN_ALARM_ON;
						else if(((alarmConditions.triggerMode&(0x0001<<t)) == GUARDIAN_ALARM_TRIGGER_MODE_AND<<t) && (iAlarms&alarmConditions.triggerMask[t])==alarmConditions.triggerMask[t])	
							state[t]=GUARDIAN_ALARM_ON;
					}
				}
				else
					state[t]=GUARDIAN_ALARM_OFF;
				break;
			default: // all other values considered as GUARDIAN_ALARM_OFF state
				iTriggers &= ~(0x0001<<t); // sets trigger to 0.
				state[t]=GUARDIAN_ALARM_OFF;// default next state
				if(alarmConditions.triggerMask[t] && iAlarms)
				{
					if(((alarmConditions.triggerMode&(0x0001<<t)) == GUARDIAN_ALARM_TRIGGER_MODE_OR<<t) && (iAlarms&alarmConditions.triggerMask[t]))
						state[t]=GUARDIAN_ALARM_LAT;
					else if(((alarmConditions.triggerMode&(0x0001<<t)) == GUARDIAN_ALARM_TRIGGER_MODE_AND<<t) && (iAlarms&alarmConditions.triggerMask[t])==alarmConditions.triggerMask[t])	
						state[t]=GUARDIAN_ALARM_LAT;
				}
				break;
		}
	}
	return(iTriggers)
}
