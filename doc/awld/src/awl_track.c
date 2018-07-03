#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>

#include "awl_track.h"
#include "awlmsg.h"
#include "awlcmd.h"


static int iMaxNumberofTempTracks = DEFAULT_MAX_NUMBER_OF_TEMP_TRACKS;
static int iTempTrackFound = 0;
static float fMaxDistanceToPair = 1;
static float delta_time = 1;
static awl_distance tempDetBuff[DEFAULT_MAX_NUMBER_OF_TEMP_TRACKS][DEFAULT_HISTO_SIZE]; //scratch pad for detection pairing, contains all the possible tracks in accordance with
static float tempConf[DEFAULT_MAX_NUMBER_OF_TEMP_TRACKS]; // For each possible tracks in the tempDetBuff, calculate a confidence level using intensity

static inline void resetTempBuff();
static inline int doesDetectionMatch(awl_distance _reference, awl_distance _candidate, float fMaxSNRToPair);
static inline void selectPtfprTracking(awl_data *awl, int _CurrentChannel, float fMaxSNRToPair);
static inline void setTempConfLevel(awl_data *awl);
static inline int findStrongest();
static inline void findPosAndVel(int _tempDet, awl_data *awl, int _CurrentChannel, int _indexTrack);
static inline int makeTrackResult(int _tempDet, awl_data *awl, int _CurrentChannel, int _indexTrack, float fMinSNRToTrack);
static inline void deleteTempData(int _index);
static inline void insertTracks(awl_data *awl, int _CurrentChannel, float fMinSNRToTrack);
static void clearOldTracks(awl_data *awl, int _CurrentChannel);
void alpha_beta_filter(float x_measured, float dt, awl_data *awl, awl_track  *tab);
void alpha_beta_filterV(float speed, float dt, awl_data *awl, awl_track  *tab);
void validateTrackingParameters(awl_data *awl);
int track_distance_AB(awl_data *awl,  int _CurrentChannel); // Call AB filter
int track_distance_LR(awl_data *awl,  int _CurrentChannel); // Call LR filter
int track_distance_NoTrack(awl_data *awl, int _CurrentChannel); // Call No filter


// ****************************************************************************
// LINEAR REGRESSION FUNCTIONS
// ****************************************************************************

static inline void resetTempBuff() {
        int i,j;
	iTempTrackFound = 0;
	for (i = 0; i < DEFAULT_MAX_NUMBER_OF_TEMP_TRACKS; i++) {
		tempConf[i] = 0;
		for (j = 0; j < DEFAULT_HISTO_SIZE; j++) {
			tempDetBuff[i][j].meters = 0;
			tempDetBuff[i][j].intensity = 0;
		}
	}
}

static inline int doesDetectionMatch(awl_distance _reference, awl_distance _candidate, float fMaxSNRToPair) {
	int retval = 0;
	float absDeltaD = fabs(_candidate.meters - _reference.meters);
	float absDeltalI = fabs(_candidate.intensity - _reference.intensity);

	if (absDeltaD < fMaxDistanceToPair
			&& absDeltalI < fMaxSNRToPair) {
		retval = 1;
	}
	return (retval);
}

static inline void selectPtfprTracking(awl_data *awl, int _CurrentChannel, float fMaxSNRToPair)
{
	int d, p, T;
	int tempIndex, currentHistoryIndex, lastHistoryIndex;

	lastHistoryIndex = awl->history_index[_CurrentChannel]; // Current history index
	currentHistoryIndex = lastHistoryIndex + 1;
	if(currentHistoryIndex >= awl->historicSize) {
		currentHistoryIndex = 0;
	}

	for (T = awl->historicSize-2; T >= 0; T--)
	{ // verification starting pt
		//printf("currentHistoryIndex: %d\n", currentHistoryIndex);
		for (d = 0; d < awl->maxNumberOfDetections && awl->detected_history[_CurrentChannel][(currentHistoryIndex*awl->maxNumberOfDetections) + d].meters; d++) 
		{
			if (T < awl->historicSize - 2)
			 { // add to previous
				for (tempIndex = 0; tempIndex < iTempTrackFound; tempIndex++) 
				{
					if (doesDetectionMatch(tempDetBuff[tempIndex][awl->historicSize-2],
							awl->detected_history[_CurrentChannel][(currentHistoryIndex*awl->maxNumberOfDetections) + d], fMaxSNRToPair)) {
						//printf("Match Init! %f vs %f\n", tempDetBuff[tempIndex][awl->historicSize-2].meters, awl->detected_history[_CurrentChannel][(currentHistoryIndex*awl->maxNumberOfDetections) + d].meters);
						tempDetBuff[tempIndex][T].meters =
								awl->detected_history[_CurrentChannel][(currentHistoryIndex*awl->maxNumberOfDetections) + d].meters;
						tempDetBuff[tempIndex][T].intensity =
								awl->detected_history[_CurrentChannel][(currentHistoryIndex*awl->maxNumberOfDetections) + d].intensity;
					}
				}
			} 
			else // T = awl->historicSize - 2
			{ // start new at beginning (still no points in tempDetBuff)
				for (p = 0; p < awl->maxNumberOfDetections && awl->detected_history[_CurrentChannel][((lastHistoryIndex)*awl->maxNumberOfDetections) + p].meters; p++) {
					if (doesDetectionMatch(awl->detected_history[_CurrentChannel][(currentHistoryIndex*awl->maxNumberOfDetections) + d], awl->detected_history[_CurrentChannel][((lastHistoryIndex)*awl->maxNumberOfDetections) + p], fMaxSNRToPair) && (iTempTrackFound < iMaxNumberofTempTracks- 1)) // Compare end of last history with beginning of new history and limit the number of tracks
					 {
						//printf("Match Init! %f vs %f\n", awl->detected_history[_CurrentChannel][(currentHistoryIndex*awl->maxNumberOfDetections) + d].meters, awl->detected_history[_CurrentChannel][((lastHistoryIndex)*awl->maxNumberOfDetections) + p].meters);
						//printf("Match Init! %d vs %d\n", awl->detected_history[_CurrentChannel][(currentHistoryIndex*awl->maxNumberOfDetections) + d].intensity, awl->detected_history[_CurrentChannel][((lastHistoryIndex)*awl->maxNumberOfDetections) + p].intensity);
						tempDetBuff[iTempTrackFound][T].meters =
								awl->detected_history[_CurrentChannel][(currentHistoryIndex*awl->maxNumberOfDetections) + d].meters;
						tempDetBuff[iTempTrackFound][T].intensity =
								awl->detected_history[_CurrentChannel][(currentHistoryIndex*awl->maxNumberOfDetections) + d].intensity;
						tempDetBuff[iTempTrackFound][T + 1].meters =
								awl->detected_history[_CurrentChannel][(lastHistoryIndex*awl->maxNumberOfDetections) + p].meters;
						tempDetBuff[iTempTrackFound][T + 1].intensity =
								awl->detected_history[_CurrentChannel][(lastHistoryIndex*awl->maxNumberOfDetections) + p].intensity;
						iTempTrackFound++;
					}
				}
			}
		}
		
		currentHistoryIndex++;
		if(currentHistoryIndex >= awl->historicSize) {
			currentHistoryIndex = 0;
		}
	}
	//printf("Scratch pad:\n");
	//for(int i = 0; i < awl->historicSize; i++) {
	//	for(int j = 0; j < MAX_NUMBER_OF_TEMP_TRACKS; j++) {
	//		if(tempDetBuff[j][i].meters){
	//		printf("History %d detection %d: %f\n", i, j, tempDetBuff[j][i].meters);
	//		}
	//	}
	//}
}

static inline void setTempConfLevel(awl_data *awl) { // Calculate confidence level for each possible track in tempDetBuff
	int i,h;
	for (i = 0; i < iTempTrackFound; i++) {
		for (h = 0; h < awl->historicSize; h++) {
			if (tempDetBuff[i][h].meters) {
				tempConf[i] += (1 / (float)awl->historicSize)* (tempDetBuff[i][h].intensity / 50.00);
			}
		}
	}
}

static inline int findStrongest() {
	int i;
        int strongest = iTempTrackFound;
	float strongestVal = 0;
	for (i = 0; i < iTempTrackFound; i++) {
		if (tempConf[i] > strongestVal) {
			strongestVal = tempConf[i];
			strongest = i;
		}
	}
	return (strongest);
}

static inline void findPosAndVel(int _tempDet, awl_data *awl, int _CurrentChannel, int _indexTrack) {
	int i;
	float fXmean, fYmean;
	//float fYstd;
	float Distances[awl->historicSize];
	float Times[awl->historicSize];
	//const float fTscan = sensorParameter.system.timerTrigger[0] * 0.001; // timer in ms... // Guardian parameter
	const float fTscan = (float) 1/awl->frame_rate;

	for (i = 0; i < awl->historicSize; i++) {
		if (tempDetBuff[_tempDet][i].meters == 0) { // If value has to be estimated
			if (i == 0) {
				tempDetBuff[_tempDet][i].meters =
						tempDetBuff[_tempDet][i + 1].meters;
			} else if (i == (awl->historicSize - 1)) {
				tempDetBuff[_tempDet][i].meters =
						tempDetBuff[_tempDet][i - 1].meters;
			} else {
				tempDetBuff[_tempDet][i].meters = 0.5
						* (tempDetBuff[_tempDet][i + 1].meters
								+ tempDetBuff[_tempDet][i - 1].meters);
			}
		}
		Distances[i] = tempDetBuff[_tempDet][i].meters;
		Times[i] = (float) (i + 1);
	}

	//printf("Selected track:\n");
	//for(int i = 0; i < awl->historicSize; i++) {
	//	printf("%f\n", Distances[i]);
	//}

	//means
	fXmean = findmean(Times, awl->historicSize); //0.5 * (awl->historicSize + 1);
	fYmean = findmean(Distances, awl->historicSize);
	//fYstd = standard_deviation(Distances, awl->historicSize, fYmean);
	//speed
	awl->all_channel_track[_CurrentChannel][_indexTrack].vk_1 = (-1)*MyCovariance(fXmean, fYmean, Times, Distances,
			awl->historicSize)/(MyVariance(fXmean, Times, awl->historicSize)*fTscan);
	//awl->all_channel_track[_CurrentChannel][_indexTrack].vk_1 = fYstd/(MyVariance(fXmean, Times, awl->historicSize)*fTscan);
	//distance
	awl->all_channel_track[_CurrentChannel][_indexTrack].xk_1 = fYmean + awl->all_channel_track[_CurrentChannel][_indexTrack].vk_1 * (awl->historicSize - fXmean)*fTscan;
	//awl->all_channel_track[_CurrentChannel][_indexTrack].xk_1 = fYmean + fYstd;

}

static inline int makeTrackResult(int _tempDet, awl_data *awl, int _CurrentChannel, int _indexTrack, float fMinSNRToTrack) {
	int success = 0;
	float conf, Int;
	int i, numDetInTrack = 0;

	if (_tempDet >= 0 && _tempDet < iMaxNumberofTempTracks) {
		conf = tempConf[_tempDet]*100;
		Int = 0.00;
		for (i = 0; i < awl->historicSize; i++) {
			if (tempDetBuff[_tempDet][i].intensity) {
				Int += tempDetBuff[_tempDet][i].intensity;
				numDetInTrack++;
			}
		}
		Int /= numDetInTrack; // This allow to verify the MEAN SNR for each detection in a track

		if (Int >= fMinSNRToTrack) { //	got track! (if MEAN SNR is above threshold)

			awl->all_channel_track[_CurrentChannel][_indexTrack].track_id = (_CurrentChannel*awl->maxNumberOfTrackPerChannel) + _indexTrack + 1;
			awl->all_channel_track[_CurrentChannel][_indexTrack].channel = _CurrentChannel;
			awl->all_channel_track[_CurrentChannel][_indexTrack].intensity = (int) Int;
			awl->all_channel_track[_CurrentChannel][_indexTrack].prob_track = (int) conf;
			awl->track_ttl_index[_CurrentChannel][_indexTrack] = 1; // Must be > 0
			if (awl->all_channel_track[_CurrentChannel][_indexTrack].prob_track > 100)
				awl->all_channel_track[_CurrentChannel][_indexTrack].prob_track = 100;

			findPosAndVel(_tempDet, awl, _CurrentChannel, _indexTrack);

			success = 1;
		}
	}
	return (success);
}

static inline void deleteTempData(int _index){
	tempConf[_index] = 0.00;
}

static inline void insertTracks(awl_data *awl, int _CurrentChannel, float fMinSNRToTrack) {
	int strongestIndex;
        int i;
	for (i = 0; i < awl->maxNumberOfTrackPerChannel; i++) { // Find at most awl->maxNumberOfTrackPerChannel tracks
		strongestIndex = findStrongest(); // Find the next possible track with the highest confidence level
		if (strongestIndex < iTempTrackFound) { // Limit max number of tracks to the number of possible tracks found
			if (!makeTrackResult(strongestIndex, awl, _CurrentChannel, i, fMinSNRToTrack)) {
				i--; // Could not make track because MEAN TRACK SNR is not above threshold
			}
		} else {
			break;
		}
		deleteTempData(strongestIndex); // Erase track from confidence scratch pad so that it cannot be selected twice
	}
}

static void clearOldTracks(awl_data *awl, int _CurrentChannel) {
	int tr;
	for (tr=0;tr<DEFAULT_AWL_MAX_TRACK_PER_CHANNEL; tr++) {
		awl->all_channel_track[_CurrentChannel][tr].prob_track = 0;
		awl->all_channel_track[_CurrentChannel][tr].xk_1 = 0;
		awl->all_channel_track[_CurrentChannel][tr].intensity = 0;
		awl->all_channel_track[_CurrentChannel][tr].vk_1 = 0;
		awl->track_ttl_index[_CurrentChannel][tr] = -2;
	}
}

// ****************************************************************************
// ALPHA-BETA FUNCTION
// ****************************************************************************

void alpha_beta_filterV(float speed, float dt, awl_data *awl, awl_track  *tab) {
	float xk_2 = tab->xk_2;
	float vk_2 = tab->vk_2;
	float alpha = TRACKALGO_PARAM(2);
	float beta = TRACKALGO_PARAM(3);

	float xk; // (speed unfiltered)
	float vk; // derivative of speed 
	float rk; // error 

	xk = xk_2 + ( vk_2 * dt );
 	//update (estimated) velocity  
	vk = vk_2;
 	//what is our residual error (mesured - estimated) 
	rk = speed - xk;
 	//update our estimates given the residual error. 
	xk = xk + alpha * rk;
	vk = vk + ( beta * rk ) / dt;
 	//finished!
 	//now all our "currents" become our "olds" for next time 
	tab->vk_2 = vk;
	tab->xk_2 = xk;
        //printf( "%f \t %f\n", x_measured, tab->xk_1 );
        //printf( "%f \t %f\n", vk, tab->vk_1 );

}

void alpha_beta_filter(float x_measured, float dt, awl_data *awl, awl_track  *tab) {
	float xk_1 = tab->xk_1;
	float vk_1 = tab->vk_1;
	float alpha = TRACKALGO_PARAM(0);
	float beta = TRACKALGO_PARAM(1);

	float xk; // (position)
	float vk; // derivative of position (ie: velocity)
	float rk; // error 

 	// (estimated 9Yposition )  ( pos = pos + vel (last).dt)
	xk = xk_1 + ( vk_1 * dt );
 	//update (estimated) velocity  
	vk = vk_1;
 	//what is our residual error (mesured - estimated) 
	rk = x_measured - xk;
 	//update our estimates given the residual error. 
	xk = xk + alpha * rk;
	vk = vk + ( beta * rk ) / dt;
 	//finished!
 	//now all our "currents" become our "olds" for next time 
	tab->vk_1 = vk;
	tab->xk_1 = xk;
	// tab->xk_2 = vk;  // for speed filtering
        //printf( "%f \t %f\n", x_measured, tab->xk_1 );
        //printf( "%f \t %f\n", vk, tab->vk_1 );
        //sleep( 1 );

}

// ****************************************************************************
// MAIN FUNCTIONS
// ****************************************************************************

int track_distance_AB(awl_data *awl, int _CurrentChannel) {

	/*
 	010 Obstacle 
	MSG_OBSTACLE_TRAK
		Track Tracking ID	Word
		Obstacle Sensors	Bit pattern: 0-8
		Track type		TBD
		Track Probability	0-100
		Time to collision	Unsigned integer In milliseconds
	011 Obstacle Velocity
	MSG_OBSTACLE_VELOCITY
		Tracking ID		
		Track Distance		Integer in the range (1-20000)
		Track Speed		Integer in the range -27778 to 27778 in cm/s
		Track Acceleration	Integer in the range -27778 to 27778 in cm/s^2
	012 Obstacle Size 
	MSG_OBSTACLE_SIZE
		Tracking ID		
		Object Height Max	0: unknown
		Object Width Max	0: unknown
		Object Intensity	0-4096
	013 Obstacle Angular Position	Tracking ID
	MSG_OBSTACLE_POSITION
		Object start angle 	Integer -900 to + 900 in tenth of a degree
		Object End Angle	Integer -900 to + 900 in tenth of a degree
		Object Angular Velocity	Integer -900 to + 900 in tenth of a degree/sec

        	#define MSG_OBSTACLE_TRAK        10
        	#define MSG_OBSTACLE_VELOCITY    11
        	#define MSG_OBSTACLE_SIZE        12
        	#define MSG_OBSTACLE_POSITION    13 
 
	*/

	
	int i,j, channel, found=0;
	float valid_tunel = fMaxDistanceToPair;
	float xm=0, xm_right, xm_left;
	int snr;

	// Testing  initial tracks
	
	channel = _CurrentChannel;
	/*
	if ( channel == 6 ) {
		printf("Testing initial values of alphabeta internal track components\n");
		printf("AB init: %f %f %f %f \n",
			awl->ab_alpha,
        		awl->ab_beta,
        		awl->all_channel_track[0][0].xk_1,
       	 		awl->all_channel_track[0][0].vk_1);
	}
	*/
	
        // Real Hall position 35m 
        // x = 35;

	// Testing abfile parameters
	//printf("AB file parameters: %f %f %f \n", awl->ab_alpha, awl->ab_beta, awl->ab_max_object_speed);	
	//getchar();
       
        /*  
        ab_x = awl->all_channel_track[0][0].ab_data; 
        alpha_beta_filter(xm, delta_time , &awl->all_channel_track[0][0].ab_data );
        printf("Ideal     position: %6.3f \n",x);
        printf("Mesaured  position: %6.3f  [diff:%.3f]\n",xm, x-xm );
        printf("AlphaBeta position: %6.3f  [diff:%.3f]\n",ab_x.xk_1, x-ab_x.xk_1 );
       */ 

	//	initialize_alpha_beta(0.85, 0.005, &ab_x); //x position
	

	
	/* every index minus -1 for this track but already available set to -2 */


	
	for (i=0;i<awl->maxNumberOfTrackPerChannel;i++) {
		if ( awl->track_ttl_index[channel][i] > -2 ) {
        		awl->track_ttl_index[channel][i]--;
			//printf("TTL %d", awl->track_ttl_index[channel][i]);
		}
		//printf("\n");
	}

	/* for all distances in the current channel */
	//  awl->all_channel_track[channel][4].xk_1 = 4.0;
	for (i=0;i<awl->maxNumberOfDetections;i++) {
		xm = awl->detected[channel][i].meters;
		snr = awl->detected[channel][i].intensity;
		found=0;
		 // xm = 4.0;
		 // awl->track_ttl_index[channel][4] = 4;
		if ( xm > 0 ) {
			/* try to find an existing track */
			j=0;
			while ( !found &&  ( j<awl->maxNumberOfTrackPerChannel) ) {
				if ( awl->track_ttl_index[channel][j] > -2 ) {
					xm_left  = awl->all_channel_track[channel][j].xk_1 - valid_tunel;	
					xm_right = awl->all_channel_track[channel][j].xk_1 + valid_tunel;	
					if ( ( xm >= xm_left ) &&  ( xm <= xm_right ) )  {
						// printf("Found value %d \n",j );
						found = j+1;	  
						/* update the value in the track system default  persistence = 3   */
						awl->track_ttl_index[channel][found-1] = TRACKALGO_PARAM(4);
        				alpha_beta_filter(xm, delta_time , awl, &awl->all_channel_track[channel][found-1] );
						awl->all_channel_track[channel][found-1].intensity = snr;
					}
				}
				j++;
			}
			/* no value close create a new one */
			j=0;
			while ( !found &&  ( j<awl->maxNumberOfTrackPerChannel) ) {
				if ( awl->track_ttl_index[channel][j] == -2 ) {
					// printf("New value inserted %d \n", ++awl->awl_track_counter );
					found = j+1;	  
					awl->all_channel_track[channel][found-1].track_id = ++awl->awl_track_counter;	
					awl->all_channel_track[channel][found-1].channel = channel;
					awl->track_ttl_index[channel][found-1] = 0;
					// initial condition
					awl->all_channel_track[channel][found-1].xk_1 = xm;
					awl->all_channel_track[channel][found-1].intensity = snr;
				}
				j++;
			}
			
		}
		
	} /* for all distances in the current channel */



	if (awl->verbose_distance) {
		printf("TRK		   AB_FILTER %f %f %f %f %f\n", TRACKALGO_PARAM(0), TRACKALGO_PARAM(1),TRACKALGO_PARAM(2), TRACKALGO_PARAM(3), TRACKALGO_PARAM(4));
		printf("TRK CH%d   AB_FILTER : ", channel);
		for (i=0;i<8;i++) {
			 printf(" -   %f ", awl->detected[channel][i].meters);

		}
	 	printf("\n");

		printf("TRK CH%d ", channel);
		for (i=0;i< awl->maxNumberOfTrackPerChannel; i++) {
			printf(" - %d ",  awl->track_ttl_index[channel][i]);
		}
	 	printf("\n");
		
		
        	printf("TRK CH%d  AlphaBeta position: %6.3f  [diff:%.3f]\n",channel,
        	awl->all_channel_track[channel][found-1].xk_1,
		xm - awl->all_channel_track[channel][found-1].xk_1);
        	printf("TRK CH%d  Speed: %6.3f \n",channel, awl->all_channel_track[channel][found-1].vk_1);
        	// alpha_beta_filterV(awl->all_channel_track[channel][found-1].vk_1
		//	, delta_time , awl, &awl->all_channel_track[channel][found-1] );
        	// printf("TRK CH%d  Speed 22: %6.3f \n",channel, awl->all_channel_track[channel][found-1].xk_2);
		printf("\n");

		// getchar();
	
	}

	for (i=0;i< awl->maxNumberOfTrackPerChannel; i++) {
		if ( awl->track_ttl_index[channel][i] > 0 ) {

			awl->obstacles[channel * awl->maxNumberOfTrackPerChannel +i].track_id = 	
				awl->all_channel_track[channel][i].track_id;
			awl->obstacles[channel * awl->maxNumberOfTrackPerChannel +i].channel = 
				awl->all_channel_track[channel][i].channel;
			awl->obstacles[channel * awl->maxNumberOfTrackPerChannel +i].track_distance = 	
				   awl->all_channel_track[channel][i].xk_1 * 100.0;
			awl->obstacles[channel * awl->maxNumberOfTrackPerChannel +i].track_speed = 	
			  	   awl->all_channel_track[channel][i].vk_1 * 100.0;
			awl->obstacles[channel * awl->maxNumberOfTrackPerChannel +i].object_intensity = 
					awl->all_channel_track[channel][i].intensity;
			// printf("Intensity %d \n", awl->obstacles[channel * awl->maxNumberOfTrackPerChannel +i].object_intensity    );
			// getchar();
			//send_obstacle(awl,  object_buffer);

			if (awl->verbose_distance) {
				/* Send the can message   */	
        			//printf("TRK CH%d  TRANSMIT",channel);
        			//printf(" x  %6.3f  v %6.3f  id %d", awl->all_channel_track[channel][i].xk_1,
        			//awl->all_channel_track[channel][i].vk_1,  awl->obstacles[channel*8+i].track_id) ;
			}
		} else {
			awl->obstacles[channel* awl->maxNumberOfTrackPerChannel +i].track_id = 0;	
		}	
	}
	if (awl->verbose_distance) {
		 printf("\n");
	}

	
	// Fake data
	/*
	for (i=0;i< 7; i++) {

			awl->obstacles[i * awl->maxNumberOfTrackPerChannel +i].track_id = 30000+i;
			awl->obstacles[i * awl->maxNumberOfTrackPerChannel +i].channel = 1 << ( i ); 
			awl->obstacles[i * awl->maxNumberOfTrackPerChannel +i].track_distance =  5000+ 100*i;	
			awl->obstacles[i * awl->maxNumberOfTrackPerChannel +i].track_speed =   5000+100*i; 	

	}
	*/
		
	// Fake data
	/*
	for (i=0;i< 7; i++) {

			awl->obstacles[i * awl->maxNumberOfTrackPerChannel +i].track_id = 30000+i;
			awl->obstacles[i * awl->maxNumberOfTrackPerChannel +i].channel = 1 << ( i ); 
			awl->obstacles[i * awl->maxNumberOfTrackPerChannel +i].track_distance =  5000+i;	
			awl->obstacles[i * awl->maxNumberOfTrackPerChannel +i].track_speed =   9000+i; 	
	}
	*/
		
	// Fake data
	/*
	for (i=0;i< 7; i++) {

			awl->obstacles[i * awl->maxNumberOfTrackPerChannel +i].track_id = 30000+i;
			awl->obstacles[i * awl->maxNumberOfTrackPerChannel +i].channel = 1 << ( i ); 
			awl->obstacles[i * awl->maxNumberOfTrackPerChannel +i].track_distance =  5000+i;	
			awl->obstacles[i * awl->maxNumberOfTrackPerChannel +i].track_speed =   9000+i; 	
	}
	*/
		
	return 0;
	
}


int track_distance_LR(awl_data *awl, int _CurrentChannel) 
{
    int i;
	float fMaxSNRToPair = 10, fMinSNRToTrack = 15;

	fMaxSNRToPair = TRACKALGO_PARAM(0);
	fMinSNRToTrack = TRACKALGO_PARAM(1);
	iMaxNumberofTempTracks = awl->maxNumberOfDetections * awl->maxNumberOfDetections;

	resetTempBuff();
	clearOldTracks(awl, _CurrentChannel);
	selectPtfprTracking(awl, _CurrentChannel, fMaxSNRToPair);
	setTempConfLevel(awl);
	insertTracks(awl, _CurrentChannel, fMinSNRToTrack);

	for (i=0;i< awl->maxNumberOfTrackPerChannel; i++) {
		if ( awl->track_ttl_index[_CurrentChannel][i] > 0 ) {

			awl->obstacles[_CurrentChannel * awl->maxNumberOfTrackPerChannel +i].track_id = 	
				awl->all_channel_track[_CurrentChannel][i].track_id;
			awl->obstacles[_CurrentChannel * awl->maxNumberOfTrackPerChannel +i].channel = 
				awl->all_channel_track[_CurrentChannel][i].channel;
			awl->obstacles[_CurrentChannel * awl->maxNumberOfTrackPerChannel +i].track_distance = 	
				   awl->all_channel_track[_CurrentChannel][i].xk_1 * 100.0;
			awl->obstacles[_CurrentChannel * awl->maxNumberOfTrackPerChannel +i].track_speed = 	
			  	   awl->all_channel_track[_CurrentChannel][i].vk_1 * 100.0;
			awl->obstacles[_CurrentChannel * awl->maxNumberOfTrackPerChannel +i].object_intensity = 
					awl->all_channel_track[_CurrentChannel][i].intensity;
			//printf("Intensity %d \n", awl->obstacles[_CurrentChannel * awl->maxNumberOfTrackPerChannel +i].object_intensity    );
			// getchar();
			//send_obstacle(awl,  object_buffer);
		} else {
			awl->obstacles[_CurrentChannel* awl->maxNumberOfTrackPerChannel +i].track_id = 0;	
		}	
	}

	if (awl->verbose_distance) {
		int i;
		printf("TRK CH%d   LR_FILTER : ", _CurrentChannel);
		for (i=0;i<awl->maxNumberOfTrackPerChannel;i++) {
			printf(" -   %f ", awl->all_channel_track[_CurrentChannel][i].xk_1);
		}
	 	printf("\n");

		printf("TRK CH%d ", _CurrentChannel);
		
		for (i=0;i< awl->maxNumberOfTrackPerChannel; i++) {
			printf(" %d ",  awl->track_ttl_index[_CurrentChannel][i]);
		}
	 	printf("\n");
		printf("\n");
		printf("\n");
	
	}

	return 0;
}

int track_distance_NoTrack(awl_data *awl, int _CurrentChannel) {

	int i, maxDetections;

	if(awl->maxNumberOfDetections > awl->maxNumberOfTrackPerChannel) { // Saturation
		maxDetections = awl->maxNumberOfTrackPerChannel;
	} else {
		maxDetections = awl->maxNumberOfDetections;
	}

	for (i=0;i < maxDetections; i++) 
	{
		if(awl->detected[_CurrentChannel][i].meters) 
		{
			awl->obstacles[_CurrentChannel * awl->maxNumberOfTrackPerChannel + i].track_id = awl->maxNumberOfTrackPerChannel*_CurrentChannel + i;
			awl->obstacles[_CurrentChannel * awl->maxNumberOfTrackPerChannel + i].channel = _CurrentChannel;
			awl->obstacles[_CurrentChannel * awl->maxNumberOfTrackPerChannel + i].track_distance = 	
				awl->detected[_CurrentChannel][i].meters * 100.0;
			awl->obstacles[_CurrentChannel * awl->maxNumberOfTrackPerChannel + i].track_speed = 0;
			awl->obstacles[_CurrentChannel * awl->maxNumberOfTrackPerChannel + i].object_intensity = 
				awl->detected[_CurrentChannel][i].intensity;
		} else {
			awl->obstacles[_CurrentChannel * awl->maxNumberOfTrackPerChannel + i].track_id = 0;
		}
	}

	if (awl->verbose_distance) {
		int i;
		printf("TRK CH%d   NO_FILTER : ", _CurrentChannel);
		for (i=0;i<maxDetections;i++) {
			printf(" -   %d ", awl->obstacles[_CurrentChannel * awl->maxNumberOfTrackPerChannel +i].track_distance);
		}

	 	printf("\n");
		printf("\n");
		printf("\n");
	
	}

	return 0;
}

void validateTrackingParameters(awl_data *awl) {

	if(awl->current_track_algo < 0 || awl->current_track_algo > AWL_NUM_TRACK_ALGOS - 1) {
		printf("Warning: Incorrect parameter for sensor-level tracking algo selection.\n");
		printf("Correct values are from 0 (disabled) to %d. Tracker is now disabled.\n", AWL_NUM_TRACK_ALGOS-1);
		awl->current_track_algo = 0;
	}

	if(fMaxDistanceToPair < 0) {
		printf("Warning: Max distance to pair value %f is incorrect. Set to 1.\n", fMaxDistanceToPair);
		fMaxDistanceToPair = 1;
	}

	if(delta_time <= 0) {
		printf("Warning: Incorrect parameter for time between acquisition parameter (zero or negative value). Set to 1.\n");
		delta_time = 1;
	}
	
}

int track_distance(awl_data *awl) {

	if ( awl->frame_rate > 0 ) {
		delta_time = 1.0/(awl->frame_rate) * 1.0;
		fMaxDistanceToPair = awl->track_max_object_speed / awl->frame_rate;
	} else { // Playback mode
		delta_time = 1.0;  // Crash prevention
		fMaxDistanceToPair = 2.0;
	}

	validateTrackingParameters(awl);
	//awl->current_track_algo=1;

	switch(awl->current_track_algo)
	{
		case 0: // No Track
			return track_distance_NoTrack(awl, GET_CURRENT_AWL_CHANNEL);
			break;
		case 1: // AB filter
			return track_distance_AB(awl, GET_CURRENT_AWL_CHANNEL);
			break;
		case 2: // LR filter
			return track_distance_LR(awl, GET_CURRENT_AWL_CHANNEL);
			break;
		default:
			return track_distance_AB(awl, GET_CURRENT_AWL_CHANNEL);
			break;
	}

	return 0; // unreachable
}
