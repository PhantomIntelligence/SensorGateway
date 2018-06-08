//********** 2012-09 *********//

#ifdef __WIN32__
#include <winsock2.h>
#include <windows.h>
#include <ws2def.h>
#include <ws2tcpip.h>
#endif
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
//#include <sstream>  
#include <unistd.h>

#ifdef __linux__
#define SOCKET int
#define SOCKET_ERROR -1
#define INVALID_SOCKET -1
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#endif
#include "awl_data.h"
//#include "awl_distance.h"
#include "awl_fdist.h"

#define DISPLAY_X 1366
#define DISPLAY_Y 43//87//100
#define DISPLAY_Y_OFF 20
#define DISPLAY_X_OFF 4//7//20
#define NB_TRACES 16
#define NB_CHANNEL 16
#define CHANNEL_MASK 0xFFFF
#define NB_DET_MAX 4
#define TRACES_SIZE 1024
#define OFF0 100
#define MAXVAL 0xFFFFFFFF
#define GAIN 1
#define ATTN 1 
#define WAIT 10

#define PORT_NUM 23456
//#define AWLI_IP "192.168.22.10"
#define AWLI_IP "192.168.1.10"

//#define IMAGE CvMat
#define IMAGE IplImage
#define IMAGE_WIDTH(x) (x->width)
#define IMAGE_HEIGHT(x) (x->height)
#define IMAGE_M_PIX(mat, col, row, value) (*((mat)->data.ptr + (size_t)(mat)->step*(row) + (CV_ELEM_SIZE((mat)->type))*(col)) = value)

static awl_distance distances[NB_CHANNEL][NB_DET_MAX];
static awl_distance persist_distances[NB_CHANNEL][NB_DET_MAX];
int mapped_channels[NB_CHANNEL] = {0, 1, 2, 3, 4, 5, 6, 7 ,8 ,9 ,10, 11, 12, 13, 14, 15};

#define MAX_OBJECTS 128
uint16_t awl_object_id[MAX_OBJECTS];
uint8_t  awl_object_sens[MAX_OBJECTS];
uint8_t  awl_object_prob[MAX_OBJECTS];
uint16_t awl_object_ttc[MAX_OBJECTS];
float awl_object_dist[MAX_OBJECTS];
float awl_object_vel[MAX_OBJECTS];
float awl_object_acc[MAX_OBJECTS];
float awl_object_snr[MAX_OBJECTS];

CvFont font100;
CvFont font50;
CvFont font75;

#define NUM_ALGO_PARAMS 16
float algo_parameters[NUM_ALGO_PARAMS] = {0};

uint32_t fixed_scaling = 0;
int nb_channels;

int no_transmit_raw = 0;
uint16_t raw_payload_size = 0;
uint16_t record_payload_size = 0;
uint16_t raw_channel_mask = CHANNEL_MASK;
uint8_t raw_frame_rate = 0;
uint16_t cooked_channel_mask = CHANNEL_MASK;
uint8_t cooked_frame_rate = 0;
uint8_t cooked_event_mask = 0x42; // 0x42 | 0x04 | 0x08;
uint8_t cooked_event_limit = 0;
uint16_t wanted_record_channel_mask = 0;
uint16_t active_record_channel_mask = 0;
uint8_t record_frame_rate = 0;
uint16_t wanted_playback_channel_mask = 0;
uint16_t active_playback_channel_mask = 0;
uint8_t playback_frame_rate = 0;

#define DATA32 int32_t
#define FINDMAX(a,b) findmax_int(a,b)
#define FINDMIN(a,b) findmin_int(a,b)
#define FINDMEAN(a,b) findmean_int(a,b)

/* ================================================================== */

void trace_display(IMAGE * grf)
{
	/* Define data display */
	int i;
	char String[8];
	for (i = 0; i < NB_TRACES; i++) {
		cvRectangle(grf,	/* the dest image */
			    cvPoint(DISPLAY_X_OFF + 30, (i * 100)+20),	/* top left point */
			    cvPoint(DISPLAY_X_OFF + TRACES_SIZE, (i + 1) * 100),	/* bottom right point */
			    cvScalar(255, 255, 0, 0),	/* the color  */
			    1, 8, 0);	/* thickness, line type, shift */

		sprintf(String, "Ch%d", i);
		cvRectangle(grf,
			  cvPoint(TRACES_SIZE + 30, (i * 100) + 40),
			  cvPoint(TRACES_SIZE + 120, (i * 100) + 48),
			cvScalar(0, 0, 0, 0),
			CV_FILLED, 8, 0);

		cvPutText(grf, String,
			  cvPoint(TRACES_SIZE + 30, (i * 100) + 45), &font100,
			  cvScalar(255, 255, 255, 0));

		if (mapped_channels[i] != i) {
			sprintf(String, "(%d)", mapped_channels[i]);
			cvPutText(grf, String,
				  cvPoint(TRACES_SIZE + 30, (i * 100) + 65), &font50,
				  cvScalar(255, 255, 255, 0));
		}

		cvRectangle(grf,        /* the dest image */
			cvPoint(TRACES_SIZE  + 30, (i * 100) + 70),     /* top left point */
			cvPoint(TRACES_SIZE + 120, (i + 1) * 100),        /* bottom right point */
			cvScalar(0, 0, 0, 0),       /* the color  */
			CV_FILLED, 8, 0);   /* thickness, line type, shift */
		if (active_record_channel_mask & (1 << mapped_channels[i] )) {
			sprintf(String, "REC");
			cvPutText(grf, String,
				  cvPoint(TRACES_SIZE + 30, (i * 100) + 95), &font75,
				  cvScalar(0, 0, 255, 0));
		} else if (active_playback_channel_mask & (1 << mapped_channels[i] )) {
			sprintf(String, "PBK");
			cvPutText(grf, String,
				  cvPoint(TRACES_SIZE + 30, (i * 100) + 95), &font75,
				  cvScalar(0, 255, 0, 0));
		}
	}
}

/* ================================================================== */

void trace_display_x(IMAGE * grf, int channel_mask)
{
	/* Define data display */
	int i, t;
	char String[8];
	int height;
	if (nb_channels) height = DISPLAY_Y * NB_TRACES / nb_channels;
	else height=100;
	for (i=0, t = 0; t < NB_TRACES; t++) {
		if (!(channel_mask & (1<<t))) continue;
		cvRectangle(grf,	/* the dest image */
			    cvPoint(DISPLAY_X_OFF, (i * height)+20),	/* top left point */
			    cvPoint(DISPLAY_X_OFF + TRACES_SIZE, (i + 1) * height),	/* bottom right point */
			    cvScalar(255, 255, 0, 0),	/* the color  */
			    1, 8, 0);	/* thickness, line type, shift */

		sprintf(String, "Ch%d", t);
		cvRectangle(grf,
			  cvPoint(TRACES_SIZE + 30, (i * height) + height * 0.40),
			  cvPoint(TRACES_SIZE + 120, (i * height) + height * 0.48),
			cvScalar(0, 0, 0, 0),
			CV_FILLED, 8, 0);

		cvPutText(grf, String,
			  cvPoint(TRACES_SIZE + 30, (i * height) + height * 0.45), &font100,
			  cvScalar(255, 255, 255, 0));

		if (mapped_channels[t] != t) {
			sprintf(String, "(%d)", mapped_channels[t]);
			cvPutText(grf, String,
				  cvPoint(TRACES_SIZE + 30, (i * height) + height * 0.65), &font50,
				  cvScalar(255, 255, 255, 0));
		}

		cvRectangle(grf,        /* the dest image */
			cvPoint(TRACES_SIZE  + 30, (i * height) + height*.70),     /* top left point */
			cvPoint(TRACES_SIZE + 120, (i + 1) * height),        /* bottom right point */
			cvScalar(0, 0, 0, 0),       /* the color  */
			CV_FILLED, 8, 0);   /* thickness, line type, shift */
		if (active_record_channel_mask & (1 << mapped_channels[t] )) {
			sprintf(String, "REC");
			cvPutText(grf, String,
				  cvPoint(TRACES_SIZE + 30, (i * height) + height*.95), &font75,
				  cvScalar(0, 0, 255, 0));
		} else if (active_playback_channel_mask & (1 << mapped_channels[t] )) {
			sprintf(String, "PBK");
			cvPutText(grf, String,
				  cvPoint(TRACES_SIZE + 30, (i * height) + height*.95), &font75,
				  cvScalar(0, 255, 0, 0));
		}
		i++;
	}
}

/* ================================================================== */

int get_trace_size(uint8_t *buffer)
{
	int mode;
	static int trace_size;

	if (buffer) {
		mode = (buffer[5] &  0x30 ) >> 4;
		switch(mode) {
		default:
			trace_size = 128;
		case 1:
			trace_size = 256;
		case 2:
			trace_size = 512;
		case 3:
			//trace_size = 1024;
			trace_size = 1024;
		}
	}
	if (!raw_payload_size) return trace_size;

	if (raw_payload_size <= trace_size) trace_size = raw_payload_size;
	return trace_size;
}

/* ================================================================== */

void trace_curve3(IMAGE * grf, DATA32 * y, int channel, int packet, 
		 int r, int g, int b, int show_scale_info)
{
	int j, x;
	static float scale[8];
	static DATA32 maximum[NB_CHANNEL], minimum[NB_CHANNEL], top[NB_CHANNEL], bottom[NB_CHANNEL], delta[NB_CHANNEL], mean[NB_CHANNEL];
	float y1, y2;
	//static int x_scale[8];
	char s[8];
	CvScalar color;
	int ch;
	static int trace_size[8];
	int start, end;
//printf("packet %d\n",packet);
#if 0
	cvRectangle(grf,        /* the dest image */
		cvPoint(TRACES_SIZE + 100, 0),
		cvPoint(DISPLAY_X, DISPLAY_Y * NB_TRACES),        /* bottom right point */
		cvScalar(0, 0, 0, 0),       /* the color  */
		CV_FILLED, 8, 0);   /* thickness, line type, shift */
#endif
		cvRectangle(grf,        /* the dest image */
			cvPoint(DISPLAY_X_OFF + packet * 256, (channel * 100) + 20),     /* top left point */
			cvPoint(DISPLAY_X_OFF + packet * 256 + 256, (channel + 1) * 100),        /* bottom right point */
			cvScalar(0, 0, 0, 0),       /* the color  */
			CV_FILLED, 8, 0);   /* thickness, line type, shift */
		ch = mapped_channels[channel];
		//trace_size = 512;
		if (!packet) {
			trace_size[channel] = get_trace_size((uint8_t*)y);
			//x_scale[channel] = 1024/trace_size[channel];
			maximum[channel] = FINDMAX((y+4), trace_size[channel] - 18);
			minimum[channel] = FINDMIN((y+4), trace_size[channel] - 18);
			//mean = findmean_uint((y+2), trace_size[channel] - 8);
			mean[channel] = FINDMEAN((y+8) + trace_size[channel]/2, 16);
			if (maximum[channel] > mean[channel]) top[channel] = maximum[channel] - mean[channel];
			else top[channel] = mean[channel] - maximum[channel];
			if (minimum[channel] > mean[channel]) bottom[channel] = minimum[channel] - mean[channel];
			else bottom[channel] = mean[channel] - minimum[channel];
			delta[channel] = top[channel] > bottom[channel] ? top[channel] : bottom[channel];
			if (delta[channel] == 0.0) delta[channel] = 1.0;
			scale[channel] = 35.0/delta[channel];
		}

		if (r+g+b) color = cvScalar(255, 255, 255, 0);
		else color = cvScalar(0, 0, 0, 0);
		switch (show_scale_info) {
		default:
			sprintf(s, "scaling: %5.3g", scale[channel]);
			break;
		case 1:
			sprintf(s, "[%08X:%08X:%08X]", minimum[channel], mean[channel], maximum[channel]);
			break;
		}
		cvPutText(grf, s, cvPoint(DISPLAY_X_OFF + TRACES_SIZE - 300, (channel+1)*100 -10), &font50,
		          color);

		switch(packet) {
		default:
			start = 0;
			end = 256;
			break;
		case 0:
			start = 16;
			end = 256;
			break;
		case 3:
			start = 0;
			end = 240;
			break;	
		}

		for (x = start; x < end; x++) {
			y1 = y[x];
			y1 -= mean[channel];
			y2 = y[x + 1];
			y2 -= mean[channel];
			/*
			printf ("%d %d %d %d %d %d ", maximum[channel], minimum[channel], top[channel], bottom[channel], delta[channel], mean[channel]);
			printf("%f ", scale[channel]);
			printf("%d ", y[x]);
			printf ("%f ", y1);
			printf ("%f\n", y1 * scale[channel]);
			*/
			cvLine(grf,
			       cvPoint(DISPLAY_X_OFF -0 + 4 + (x + packet * 256), ((channel + 1) * OFF0) - 35 - GAIN * scale[channel] * y1 / ATTN),
			       cvPoint(DISPLAY_X_OFF -0 + 4 + (x + 1 + packet * 256), ((channel + 1) * OFF0) - 35 - GAIN * scale[channel] * y2 / ATTN),
			       CV_RGB(r, g, b),	/* the color; */
			       1, 8, 0);	/* thickness, line type, shift */
		}
		for (j = 0; j < NB_DET_MAX; j++) {
			if (distances[ch][j].meters) {
			 	x = distances[ch][j].index;
				if (r+g+b) {
					if (j) {
						if (j == 7) color = cvScalar(106, 150, 46, 0);
						else color = cvScalar((j&1)*255, (j&2)*255, (j&4)*255, 0);
					} else color = cvScalar(13, 131, 241, 0);
				} else color = cvScalar(0, 0, 0, 0);
				cvLine(grf,
				       cvPoint(DISPLAY_X_OFF + x, (channel * 100) + 22),
				       cvPoint(DISPLAY_X_OFF + x, (channel * 100) + 25),
					       color, 2, 8, 0);
	
				sprintf(s, "%4.1fm", distances[ch][j].meters);
				cvPutText(grf, s, cvPoint(DISPLAY_X_OFF + 20 + (j*75), (channel+1)*100 - 10), &font50, color);
			}
		}
/*
		for (j = 0; j < 1; j++) {
			color = cvScalar(255, 255, 255, 0);
			sprintf(s, "%4.1fm", persist_distances[ch+1][j].meters);
			cvPutText(grf, s, cvPoint(TRACES_SIZE+100, (channel+1)*100 + 60), &font100, color);
		}
*/
}

/* ================================================================== */

void trace_curve(IMAGE * grf, DATA32 * y, int channel,
		 int r, int g, int b, int show_scale_info, int debut_index)
{
	int j, x;
	float scale;
	char s[8];
	DATA32 maximum, minimum, top, bottom, delta, mean;
	float y1, y2;
	int x_scale;
	CvScalar color;
	int ch;
	int trace_size;
#if 0
	cvRectangle(grf,        /* the dest image */
		cvPoint(TRACES_SIZE + 100, 0),
		cvPoint(DISPLAY_X, DISPLAY_Y * NB_TRACES),        /* bottom right point */
		cvScalar(0, 0, 0, 0),       /* the color  */
		CV_FILLED, 8, 0);   /* thickness, line type, shift */
#endif
		cvRectangle(grf,        /* the dest image */
			cvPoint(DISPLAY_X_OFF, (channel * 100) + 20),     /* top left point */
			cvPoint(DISPLAY_X_OFF + TRACES_SIZE, (channel + 1) * 100),        /* bottom right point */
			cvScalar(0, 0, 0, 0),       /* the color  */
			CV_FILLED, 8, 0);   /* thickness, line type, shift */
		ch = mapped_channels[channel];
		trace_size = get_trace_size((uint8_t*)y) - debut_index;
		//trace_size = 512;
		x_scale = 1024/trace_size;
		maximum = FINDMAX((y+4), trace_size - 18);
		minimum = FINDMIN((y+4), trace_size - 18);
		//mean = findmean_uint((y+2), trace_size - 8);
		mean = FINDMEAN((y+8) + trace_size/2, 16);
		if (maximum > mean) top = maximum - mean;
		else top = mean - maximum;
		if (minimum > mean) bottom = minimum - mean;
		else bottom = mean - minimum;
		delta = top > bottom ? top : bottom;
		if (delta == 0.0) delta = 1.0;
		scale = 35.0/delta;

		if (r+g+b) color = cvScalar(255, 255, 255, 0);
		else color = cvScalar(0, 0, 0, 0);
		switch (show_scale_info) {
		default:
			sprintf(s, "scaling: %5.3g", scale);
			break;
		case 1:
			sprintf(s, "[%08X:%08X:%08X]", minimum, mean, maximum);
			break;
		}
		cvPutText(grf, s, cvPoint(DISPLAY_X_OFF + TRACES_SIZE - 300, (channel+1)*100 -10), &font50,
		          color);

		for (x = 0; x < (trace_size - 16); x++) {
			y1 = y[4 + x];
			y1 -= mean;
			y2 = y[4 + x + 1];
			y2 -= mean;
			/*
			printf ("%d %d %d %d %d %d ", maximum, minimum, top, bottom, delta, mean);
			printf("%f ", scale);
			printf("%d ", y[4 + x]);
			printf ("%f ", y1);
			printf ("%f\n", y1 * scale);
			*/
			cvLine(grf,
			       cvPoint(DISPLAY_X_OFF -0 + 4 + x * x_scale, ((channel + 1) * OFF0) - 35 - GAIN * scale * y1 / ATTN),
			       cvPoint(DISPLAY_X_OFF -0 + 4 + (x+1) * x_scale, ((channel + 1) * OFF0) - 35 - GAIN * scale * y2 / ATTN),
			       CV_RGB(r, g, b),	/* the color; */
			       1, 8, 0);	/* thickness, line type, shift */
		}
		for (j = 0; j < NB_DET_MAX; j++) {
			if (distances[ch][j].meters) {
			 	x = distances[ch][j].index;
				if (r+g+b) {
					if (j) {
						if (j == 7) color = cvScalar(106, 150, 46, 0);
						else color = cvScalar((j&1)*255, (j&2)*255, (j&4)*255, 0);
					} else color = cvScalar(13, 131, 241, 0);
				} else color = cvScalar(0, 0, 0, 0);
				cvLine(grf,
				       cvPoint(DISPLAY_X_OFF + x, (channel * 100) + 22),
				       cvPoint(DISPLAY_X_OFF + x, (channel * 100) + 25),
					       color, 2, 8, 0);
	
				sprintf(s, "%4.1fm", distances[ch][j].meters);
				cvPutText(grf, s, cvPoint(DISPLAY_X_OFF + 20 + (j*75), (channel+1)*100 - 10), &font50, color);
			}
		}
/*
		for (j = 0; j < 1; j++) {
			color = cvScalar(255, 255, 255, 0);
			sprintf(s, "%4.1fm", persist_distances[ch+1][j].meters);
			cvPutText(grf, s, cvPoint(TRACES_SIZE+100, (channel+1)*100 + 60), &font100, color);
		}
*/
}

/* ================================================================== */

void trace_curves(IMAGE * grf, DATA32 * y, int channel_mask,
		  int r, int g, int b, int sparse, int show_scale_info, int debut_index)
{
	int i, j, x;
	float scale;
	int t = 0;
	DATA32 maximum, minimum, top, bottom, delta, mean;
	float y1, y2;
	int x_scale;
	char s[8];
	CvScalar color;
	int ch;
	int trace_size;

	cvRectangle(grf,        /* the dest image */
		cvPoint(TRACES_SIZE + 100, 0),
		cvPoint(DISPLAY_X, DISPLAY_Y * NB_TRACES),        /* bottom right point */
		cvScalar(0, 0, 0, 0),       /* the color  */
		CV_FILLED, 8, 0);   /* thickness, line type, shift */
	for (i = 0; i < NB_TRACES; i++) {
		ch = mapped_channels[i];
		if (!(channel_mask & (1<<ch))) continue;
		if (sparse) t = ch;
		else t++;
		cvRectangle(grf,        /* the dest image */
			cvPoint(DISPLAY_X_OFF, (i * 100) + 20),     /* top left point */
			cvPoint(DISPLAY_X_OFF + TRACES_SIZE, (i + 1) * 100),        /* bottom right point */
			cvScalar(0, 0, 0, 0),       /* the color  */
			CV_FILLED, 8, 0);   /* thickness, line type, shift */
		trace_size = get_trace_size((uint8_t*)(y+t*1024))  -  debut_index;
		//trace_size = 512;
		//printf("%d %08x %08x\n", t, y[t*1024], y[t*1024+1]);
		x_scale = 1024/trace_size;
		maximum = FINDMAX((y+4) + (1024*ch), trace_size - 18);
		minimum = FINDMIN((y+4) + (1024*ch), trace_size - 18);
		//mean = findmean_uint((y+2) + (1024*ch), trace_size - 8);
		mean = FINDMEAN((y+8) + (1024*ch) + trace_size/2, 16);
		if (maximum > mean) top = maximum - mean;
		else top = mean - maximum;
		if (minimum > mean) bottom = minimum - mean;
		else bottom = mean - minimum;
		delta = top > bottom ? top : bottom;
		if (delta == 0.0) delta = 1.0;
		scale = 35.0/delta;

		if (r+g+b) color = cvScalar(255, 255, 255, 0);
		else color = cvScalar(0, 0, 0, 0);
		switch (show_scale_info) {
		default:
			sprintf(s, "scaling: %5.3g", scale);
			break;
		case 1:
			sprintf(s, "[%08X:%08X:%08X]", minimum, mean, maximum);
			break;
		}
		cvPutText(grf, s, cvPoint(DISPLAY_X_OFF + TRACES_SIZE - 300, (i+1)*100 -10), &font50,
		          color);

		for (x = 0; x < (trace_size - 16); x++) {
			y1 = y[4 + t * 1024 + x];
			y1 -= mean;
			y2 = y[4 + t * 1024 + x + 1];
			y2 -= mean;
			/*
			printf ("%d %d %d %d %d %d ", maximum, minimum, top, bottom, delta, mean);
			printf("%f ", scale);
			printf("%d ", y[4 + t * 1024 + x]);
			printf ("%f ", y1);
			printf ("%f\n", y1 * scale);
			*/
			cvLine(grf,
			       cvPoint(DISPLAY_X_OFF -0 + 4 + x * x_scale, ((i + 1) * OFF0) - 35 - GAIN * scale * y1 / ATTN),
			       cvPoint(DISPLAY_X_OFF -0 + 4 + (x+1) * x_scale, ((i + 1) * OFF0) - 35 - GAIN * scale * y2 / ATTN),
			       CV_RGB(r, g, b),	/* the color; */
			       1, 8, 0);	/* thickness, line type, shift */
		}
		for (j = 0; j < NB_DET_MAX; j++) {
			if (distances[ch][j].meters) {
			 	x = distances[ch][j].index;
				if (r+g+b) {
					if (j) {
						if (j == 7) color = cvScalar(106, 150, 46, 0);
						else color = cvScalar((j&1)*255, (j&2)*255, (j&4)*255, 0);
					} else color = cvScalar(13, 131, 241, 0);
				} else color = cvScalar(0, 0, 0, 0);
				cvLine(grf,
				       cvPoint(DISPLAY_X_OFF + x, (i * 100) + 22),
				       cvPoint(DISPLAY_X_OFF + x, (i * 100) + 25),
					       color, 2, 8, 0);
	
				sprintf(s, "%4.1fm", distances[ch][j].meters);
				cvPutText(grf, s, cvPoint(DISPLAY_X_OFF + 20 + (j*75), (i+1)*100 - 10), &font50, color);
			}
		}
/*
		for (j = 0; j < 1; j++) {
			color = cvScalar(255, 255, 255, 0);
			sprintf(s, "%4.1fm", persist_distances[ch+1][j].meters);
			cvPutText(grf, s, cvPoint(TRACES_SIZE+100, (i+1)*100 + 60), &font100, color);
		}
*/
	}
}

/* ================================================================== */

void trace_curves_x(IMAGE * grf, DATA32 * y, int channel_mask, int display_channel_mask,
		  int r, int g, int b, int sparse, int show_scale_info, int debut_index)
{
	int i, ii, j, x;
	float scale;
	int t = 0;
	char s[8];
	DATA32 maximum, minimum, top, bottom, delta, mean;
	float y1, y2;
	int x_scale;
	CvScalar color;
	int ch;
	int trace_size;
	int height;
	int k;

	if (nb_channels) height = DISPLAY_Y * NB_TRACES / nb_channels;
	else height=100;

	cvRectangle(grf,        /* the dest image */
		cvPoint(TRACES_SIZE + 100, 0),
		cvPoint(DISPLAY_X, DISPLAY_Y * NB_TRACES),        /* bottom right point */
		cvScalar(0, 0, 0, 0),       /* the color  */
		CV_FILLED, 8, 0);   /* thickness, line type, shift */
	for (ii=0, i=0; i < NB_TRACES; i++) {
		if (!(display_channel_mask & (1<<i))) continue;
		ch = mapped_channels[i];
		if ((channel_mask & (1<<ch))) {
		if (sparse) t = ch;
		else t++;
		cvRectangle(grf,        /* the dest image */
			cvPoint(DISPLAY_X_OFF, (ii * height) + 20),     /* top left point */
			cvPoint(DISPLAY_X_OFF + TRACES_SIZE, (ii + 1) * height),        /* bottom right point */
			cvScalar(0, 0, 0, 0),       /* the color  */
			CV_FILLED, 8, 0);   /* thickness, line type, shift */
		trace_size = get_trace_size((uint8_t*)(y+t*1024))  -  debut_index;
		//trace_size = 512;
		//printf("%d %08x %08x\n", t, y[t*1024], y[t*1024+1]);
		x_scale = 1024/trace_size;
		if (fixed_scaling) {
			scale = height * 0.35 / fixed_scaling;
		} else {
			maximum = FINDMAX((y+4) + (1024*ch), trace_size - 18);
			minimum = FINDMIN((y+4) + (1024*ch), trace_size - 18);
			//mean = findmean_uint((y+2) + (1024*ch), trace_size - 8);
			mean = FINDMEAN((y+8) + (1024*ch) + trace_size/2, 16);
			if (maximum > mean) top = maximum - mean;
			else top = mean - maximum;
			if (minimum > mean) bottom = minimum - mean;
			else bottom = mean - minimum;
			delta = top > bottom ? top : bottom;
			if (delta == 0.0) delta = 1.0;
			scale = height*0.35/delta;
		}

		if (r+g+b) color = cvScalar(255, 255, 255, 0);
		else color = cvScalar(0, 0, 0, 0);
		switch (show_scale_info) {
		default:
			sprintf(s, "scaling: %5.3g", scale);
			break;
		case 1:
			sprintf(s, "[%08X:%08X:%08X]", minimum, mean, maximum);
			break;
		}
		cvPutText(grf, s, cvPoint(DISPLAY_X_OFF + TRACES_SIZE - 300, (ii+1)*height -10), &font50,
		          color);

		for (x = 0; x < (trace_size - 16); x++) {
			y1 = y[4 + t * 1024 + x];
			y1 -= mean;
			y2 = y[4 + t * 1024 + x + 1];
			y2 -= mean;
			/*
			printf ("%d %d %d %d %d %d ", maximum, minimum, top, bottom, delta, mean);
			printf("%f ", scale);
			printf("%d ", y[4 + t * 1024 + x]);
			printf ("%f ", y1);
			printf ("%f\n", y1 * scale);
			*/
			cvLine(grf,
			       cvPoint(DISPLAY_X_OFF -0 + 4 + x * x_scale, ((ii + 1) * height) - height*0.35 - GAIN * scale * y1 / ATTN),
			       cvPoint(DISPLAY_X_OFF -0 + 4 + (x+1) * x_scale, ((ii + 1) * height) - height*0.35 - GAIN * scale * y2 / ATTN),
			       CV_RGB(r, g, b),	/* the color; */
			       1, 8, 0);	/* thickness, line type, shift */
		}
		for (j = 0; j < NB_DET_MAX; j++) {
			if (distances[ch][j].meters) {
			 	x = distances[ch][j].index;
				if (r+g+b) {
					if (j) {
						if (j == 7) color = cvScalar(106, 150, 46, 0);
						else color = cvScalar((j&1)*255, (j&2)*255, (j&4)*255, 0);
					} else color = cvScalar(13, 131, 241, 0);
				} else color = cvScalar(0, 0, 0, 0);
				cvLine(grf,
				       cvPoint(DISPLAY_X_OFF + x, (ii * height) + 22),
				       cvPoint(DISPLAY_X_OFF + x, (ii * height) + 25),
					       color, 2, 8, 0);
	
				//printf("%d %d %5.2fm\n", ch, j, distances[ch][j].meters);
				sprintf(s, "%5.2fm", distances[ch][j].meters);
				cvPutText(grf, s, cvPoint(DISPLAY_X_OFF + 20 + (j*75), (ii+1)*height - 10), &font50, color);
			}
		}
		for (j = 0, k = 0; j < MAX_OBJECTS; j++) {
/*
int awl_object_id[MAX_OBJECTS];
int awl_object_sens[MAX_OBJECTS];
int awl_object_prob[MAX_OBJECTS];
int awl_object_ttc[MAX_OBJECTS];
int awl_object_dist[MAX_OBJECTS];
int awl_object_vel[MAX_OBJECTS];
int awl_object_acc[MAX_OBJECTS];
*/
//printf("id %d\n", awl_object_id[j]);
			if (awl_object_id[j] && (awl_object_sens[j] & (1<<ch))) {
//printf("mask 0x%x\n", awl_object_sens[j]);
/*
				if (r+g+b) {
					if (j) {
						if (j == 7) color = cvScalar(106, 150, 46, 0);
						else color = cvScalar((j&1)*255, (j&2)*255, (j&4)*255, 0);
					} else color = cvScalar(13, 131, 241, 0);
				} else color = cvScalar(0, 0, 0, 0);
*/
				color = cvScalar(255,255,0,0);
	
				//printf("%d %5.2fm\n", j, awl_object_dist[j]);
				sprintf(s, "%5.2fm", awl_object_dist[j]);
//printf("dist %d\n", awl_object_dist[j]);
				cvPutText(grf, s, cvPoint(DISPLAY_X_OFF + 20 + (k*150), (ii+1)*height - 30), &font50, color);
				sprintf(s, "%5.2fm/s", awl_object_vel[j]);
				cvPutText(grf, s, cvPoint(DISPLAY_X_OFF + 20 + (k*150), (ii+1)*height - 50), &font50, color);
				sprintf(s, "0x%04x %4.1fdB", awl_object_id[j], awl_object_snr[j]);
				cvPutText(grf, s, cvPoint(DISPLAY_X_OFF + 20 + (k*150), (ii+1)*height - 70), &font50, color);
				k++;
			}
		}
/*
		for (j = 0; j < 1; j++) {
			color = cvScalar(255, 255, 255, 0);
			sprintf(s, "%4.1fm", persist_distances[ch+1][j].meters);
			cvPutText(grf, s, cvPoint(TRACES_SIZE+100, (ii+1)*100 + 60), &font100, color);
		}
*/
		//printf ("ch %d i %d ii %d\n", ch, i, ii);
		}
		ii++;
	}
}

/* ================================================================== */

void circle(IMAGE * img, int x, int y, int r, CvScalar color)
{
	cvCircle(img, cvPoint(x, y), r, cvScalar(255, 255, 255, 0), 2, 8, 0);
	cvCircle(img, cvPoint(x - 2, y + 2), r, color, 2, 8, 0);
}

/* ================================================================== */

void headup_text(IMAGE *frame, CvFont * font, float meters, int x, int y)
{
	char s[6];

	sprintf(s, "% 4.1f m", meters);
	cvPutText(frame, s, cvPoint(x, y), font, cvScalar(255, 255, 255, 0));
	cvPutText(frame, s, cvPoint(x - 2, y + 2), font, cvScalar(0, 0, 0, 0));
}

/* ================================================================== */

void headup_display(IMAGE * frame, CvVideoWriter *writer1, CvVideoWriter *writer2)
{
	int w, h, lr, sr;
	CvScalar red;

	awl_distance *distance = NULL;

	w = IMAGE_WIDTH(frame);
	h = IMAGE_HEIGHT(frame);
	lr = h / 16;
	sr = h / 8;
	red = cvScalar(0, 0, 255, 0);

	
/*
	if (distance) {
		headup_text(frame, &font75, distance->meters, w / 2 - lr, h / 3 - lr);
		if (distance->next) {
			headup_text(frame, &font50, distance->next->meters, w / 2 - lr, h / 3 - 2 * lr);
		}
	}
*/
	if (writer1) {
		cvWriteFrame(writer1, frame);
	}

	circle(frame, w / 2, h / 3 - lr / 4, lr, red);
	circle(frame, w / 2 - lr * 2, h / 3 - lr / 4, lr, red);
	circle(frame, w / 2 + lr * 2, h / 3 - lr / 4, lr, red);
	circle(frame, w / 2 - sr, h / 3 + sr, sr, red);
	circle(frame, w / 2 + sr, h / 3 + sr, sr, red);
	circle(frame, w / 2 + sr * 3, h / 3 + sr, sr, red);
	circle(frame, w / 2 - sr * 3, h / 3 + sr, sr, red);

	if (distance) {
		headup_text(frame, &font75, distance->meters, w / 2 - lr, h / 3 - lr);
/*
		if (distance->next) {
			headup_text(frame, &font50, distance->next->meters, w / 2 - lr, h / 3 - 2 * lr);
		}
*/
	}
	if (writer2) {
		cvWriteFrame(writer2, frame);
	}
}

/* ================================================================== */

int send_msg(SOCKET receivefd, struct sockaddr_in *serv_addr, uint8_t *msg, size_t size)
{
	int ret;
	int i;
	printf("sending msg");
	for (i = 0; i < size; i ++) printf (" %02x", msg[i]);
	printf("\n");
	if ((ret =
	     sendto(receivefd, (char*)msg, size, 0,
		    (struct sockaddr *)serv_addr, sizeof(*serv_addr)) < 0))
		perror("sendto");
	return ret;
}

/* ================================================================== */

int send_transmit_raw(SOCKET receivefd, struct sockaddr_in *serv_addr)
{
	uint8_t msg[8];
	memset(msg, 0, 8);
	msg[0] = 0xe0;		// TRANSMIT_RAW
	if (no_transmit_raw ) {
		*((uint16_t *)(msg+1)) = 0;
	} else {
		*((uint16_t *)(msg+1)) = raw_channel_mask & CHANNEL_MASK;
		msg[3] = raw_frame_rate & 0xff;
	}
//msg[3] = 10;
	*((uint16_t*)(msg+4)) = raw_payload_size; // number of samples
	return send_msg(receivefd, serv_addr, msg, 8);
}

/* ================================================================== */

int send_transmit_cooked(SOCKET receivefd, struct sockaddr_in *serv_addr)
{
	uint8_t msg[8];
	memset(msg, 0, 8);
	msg[0] = 0xe1;		// TRANSMIT_RAW
	*((uint16_t*)(msg+1)) = cooked_channel_mask & CHANNEL_MASK;
	msg[3] = cooked_frame_rate &  0xFF;
//msg[3] = 15;
	msg[4] = cooked_event_mask & 0xFF;
	msg[6] = cooked_event_limit & 0xFF;
	return send_msg(receivefd, serv_addr, msg, 8);
}

/* ================================================================== */

int send_record(SOCKET receivefd, struct sockaddr_in *serv_addr)
{
	uint8_t msg[8];
	memset(msg, 0, 8);
	msg[0] = 0xd0;		// RECORDING
	*((uint16_t *)(msg+1)) = active_record_channel_mask & 0xffff;
	msg[3] = record_frame_rate & 0xff;
	*((uint16_t*)(msg+4)) = record_payload_size; // number of samples
	return send_msg(receivefd, serv_addr, msg, 8);
}

/* ================================================================== */

int send_playback(SOCKET receivefd, struct sockaddr_in *serv_addr)
{
	uint8_t msg[8];
	memset(msg, 0, 8);
	msg[0] = 0xd1;		// PLAYBACK
	*((uint16_t *)(msg+1)) = active_playback_channel_mask & 0xffff;
	msg[3] = playback_frame_rate & 0xff;
	return send_msg(receivefd, serv_addr, msg, 8);
}

/* ================================================================== */

int send_record_calibrate(SOCKET receivefd, struct sockaddr_in *serv_addr)
{
	uint8_t msg[8];
	memset(msg, 0, 8);
	msg[0] = 0xda;		// RECORD_CALIBRATION
	msg[1] = 0xff;
	msg[2] = 10;            // num of frames
	*((float *)(msg+4)) = 0.1;  // beta
	return send_msg(receivefd, serv_addr, msg, 8);
}

/* ================================================================== */

int send_clear_calibrate(SOCKET receivefd, struct sockaddr_in *serv_addr)
{
	uint8_t msg[8];
	memset(msg, 0, 8);
	msg[0] = 0xdb;		// CLEAR_CALIBRATION
	msg[1] = 0xff;
	msg[2] = 1;            // erase file
	return send_msg(receivefd, serv_addr, msg, 8);
}

/* ================================================================== */

int send_date(SOCKET receivefd, struct sockaddr_in *serv_addr)
{
	uint8_t msg[8];
	time_t now;
	struct tm t;

	now = time(NULL);
	localtime_r(&now, &t);

	memset(msg, 0, 8);
	
	msg[0] = 0xc0;		// SET_PARAMETER
	msg[1] = 0x20;		// DATE
	msg[2] = 1;
	*((uint16_t*)(msg+4)) = t.tm_year + 1900;
	msg[6] = t.tm_mon + 1;
	msg[7] = t.tm_mday;

	return send_msg(receivefd, serv_addr, msg, 8);
}

/* ================================================================== */

int send_time(SOCKET receivefd, struct sockaddr_in *serv_addr)
{
	uint8_t msg[8];
	time_t now;
	struct tm t;

	now = time(NULL);
	localtime_r(&now, &t);

	memset(msg, 0, 8);
	
	msg[0] = 0xc0;		// SET_PARAMETER
	msg[1] = 0x20;		// DATE
	msg[2] = 2;
	msg[4] = t.tm_hour;
	msg[5] = t.tm_min;
	msg[6] = t.tm_sec;

	return send_msg(receivefd, serv_addr, msg, 8);
}

/* ================================================================== */

int send_set_algo(SOCKET receivefd, struct sockaddr_in *serv_addr, int algo)
{
	uint8_t msg[8];
	memset(msg, 0, 8);
	msg[0] = 0xc0;		// SET_PARAMETER
	msg[1] = 0x02;          // ALGO PARAMETER
	msg[2] = algo & 0xff;
	msg[3] = (algo >> 8) & 0xff;
	*((uint32_t*)(msg+4)) = algo;
	return send_msg(receivefd, serv_addr, msg, 8);
}

/* ================================================================== */

int send_set_parameter(SOCKET receivefd, struct sockaddr_in *serv_addr, uint16_t parameter, float value)
{
	uint8_t msg[8];
	memset(msg, 0, 8);
	msg[0] = 0xc0;		// SET_PARAMETER
	msg[1] = 0x02;          // ALGO PARAMETER
	msg[2] = parameter & 0xff;
	msg[3] = (parameter >> 8) & 0xff;
	*((float*)(msg+4)) = value;
	return send_msg(receivefd, serv_addr, msg, 8);
}

/* ================================================================== */

int send_get_parameter(SOCKET receivefd, struct sockaddr_in *serv_addr, uint16_t parameter)
{
	uint8_t msg[8];
	memset(msg, 0, 8);
	msg[0] = 0xc1;		// QUERY_PARAMETER
	msg[1] = 0x02;          // ALGO PARAMETER
	msg[2] = parameter & 0xff;
	msg[3] = (parameter >> 8) & 0xff;
	return send_msg(receivefd, serv_addr, msg, 8);
}

/* ================================================================== */

void init_socket(char *server_ip, int server_udp_port, SOCKET *receivefd,
		 struct sockaddr_in *send_data, struct sockaddr_in *serv_addr)
{

#ifdef __linux__
	int flags;
	int optval;
	socklen_t optlen;
#endif
#ifdef __WIN32__
//	char optval;
//	int optlen;
	unsigned long int mode;
	unsigned long int res;
#endif

	printf("Connecting to server %s on UDP port %d\n", server_ip,
	       server_udp_port);

	if ((*receivefd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
		perror("datagram socket");
#ifdef __WIN32__
	res = 1;
	res = ioctlsocket(*receivefd, FIONBIO, &mode);
	if (res != NO_ERROR) {
		fprintf(stderr, "ioctlsocket failed with error: %ld\n", res);
	}
#else
	if (-1 == (flags = fcntl(*receivefd, F_GETFL, 0))) {
		perror("fcntl 1");
		flags = 0;
	}
	if (-1 == (fcntl(*receivefd, F_SETFL, flags | O_NONBLOCK))) {
		perror("fcntl 2");
	}

	if (-1 ==
	    getsockopt(*receivefd, SOL_SOCKET, SO_RCVBUF, &optval, &optlen)) {
		perror("getsockopt 1");
	} else {
		printf("RECVBUF = %d\n", optval);
		optval *= 2;
		if (-1 ==
		    setsockopt(*receivefd, SOL_SOCKET, SO_RCVBUF, &optval,
			       optlen)) {
			perror("setsockopt");
		} else {
			if (-1 ==
			    getsockopt(*receivefd, SOL_SOCKET, SO_RCVBUF,
				       &optval, &optlen)) {
				perror("getsockopt 2");
			} else {
				printf("RECVBUF = %d\n", optval);
			}
		}
	}
#endif
	memset(serv_addr, 0, sizeof(*serv_addr));
	serv_addr->sin_family = AF_INET;
	serv_addr->sin_addr.s_addr = inet_addr(server_ip);
	serv_addr->sin_port = htons(PORT_NUM);

	memset(send_data, 0, sizeof(*send_data));
	send_data->sin_family = AF_INET;
	send_data->sin_addr.s_addr = htonl(INADDR_ANY);
	send_data->sin_port = htons(0);

	if (bind(*receivefd, (struct sockaddr *)send_data, sizeof(*send_data)) == SOCKET_ERROR)
		perror("bind");
}

/* ================================================================== */

void process_distances(uint8_t *buffer)
{
	int channel;
	float div = 0.599;
	float offset;
	int trace_size;
	int start = 0, i;

	//printf("Process distance\n");

	offset = algo_parameters[0] -81;

	trace_size = get_trace_size(NULL);
	if (!trace_size) trace_size = 1024;
	div = div * 1024 / trace_size;

	if (buffer[0] >= 30) {
		channel = buffer[0] - 30;
		start = 4;
	} else if (buffer[0] >= 20) {
		channel = buffer[0] - 20;
		start = 0;
	}
	if (channel >= NB_CHANNEL) return;
	if (!start && no_transmit_raw) {
		printf("Ch %d", channel);
	}
	for (i = 0; i < 4; i++) {
		distances[channel][start + i].meters = (((uint16_t *)(buffer+4))[i])/100.0;
		if (distances[channel][start + i].meters > 0.0
		    && distances[channel][start + i].meters != persist_distances[channel][start + i].meters ) {
			persist_distances[channel][start + i].meters = distances[channel][start + i].meters;
		}
		distances[channel][start + i].index = 5 + (2*distances[channel][i].meters - offset) / div;
		if (no_transmit_raw) {
			printf(" %f m", distances[channel][start + i].meters);
		}
	}
	if (start && no_transmit_raw) {
		printf("\n");
	}
}

/* ================================================================== */

void process_distances_intensities(uint8_t *buffer8)
{
	int channel;
	float div = 0.599;
	float offset;
	int trace_size;
	int i;

	uint16_t *buffer = (uint16_t*)buffer8;

	//printf("Process distances intensities\n");

	offset = algo_parameters[0] -81;

	trace_size = get_trace_size(NULL);
	if (!trace_size) trace_size = 1024;
	div = div * 1024 / trace_size;

	channel = buffer[2];
	i = buffer[5];

	if (channel >= NB_CHANNEL) return;
	if (i >= NB_DET_MAX) return;

	//printf ("msg %d %d %d %d %d %d\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5]);
	//printf ("ch %d i %d CH %d I %d\n", channel, i, NB_CHANNEL, NB_DET_MAX);

	if (no_transmit_raw) {
		printf("Ch %d", channel);
	}
	distances[channel][i].meters = buffer[3]/100.0;
	if (distances[channel][i].meters > 0.0
	    && distances[channel][i].meters != persist_distances[channel][i].meters ) {
		persist_distances[channel][i].meters = distances[channel][i].meters;
	}
	distances[channel][i].index = 5 + (2*distances[channel][i].meters - offset) / div;
	if (no_transmit_raw) {
		printf(" %f m", distances[channel][i].meters);
	}
	if (no_transmit_raw) {
		printf("\n");
	}
	//intensities[channel][i] = something (buffer[4]);
}

/* ================================================================== */

void process_objects(uint8_t *buffer)
{
	int16_t *s;
	uint16_t *u;
	int i, id;

	s = (int16_t*)buffer;
	u = (uint16_t*)buffer;
	id = u[2];

	for (i = 0; awl_object_id[i] != id && i < MAX_OBJECTS; i++);
	if (i == MAX_OBJECTS)
		for (i = 0; awl_object_id[i] && i < MAX_OBJECTS; i++);

	if (i == MAX_OBJECTS) return;

//printf("id %d, slot %d\n", id, i);
	awl_object_id[i] = id;
	switch (buffer[0]) {
	default:
		break;
	case 10:
		awl_object_sens[i] = buffer[6];
//printf("mask 0x%x, slot %d\n", awl_object_sens[i], i);
		awl_object_prob[i] = buffer[11];
		awl_object_ttc[i]  = u[5];
		break;
	case 11:
		awl_object_dist[i] = s[3] / 100.0;
		awl_object_vel[i]  = s[4] / 100.0;
		awl_object_acc[i]  = s[5] / 100.0;
		break;
	case 12:
		awl_object_snr[i] = (u[5]/2.0) - 21.0;
		break;
	case 13:
		break;
	}
}

/* ================================================================== */

void clear_objects(void)
{
	int i;

	for (i = 0; i < MAX_OBJECTS; i++) {
		awl_object_id[i] = 0;
	}
}

/* ================================================================== */

int process_key( SOCKET receivefd, struct sockaddr_in *serv_addr, int *show_scale_info)
{
	int c, carry;
	c = cvWaitKey(1);
	c = c % 256;
	switch(c) {
	case 'r':
	case 'R':
		if (wanted_record_channel_mask) {
			active_record_channel_mask = wanted_record_channel_mask;
		} else {
			active_record_channel_mask = raw_channel_mask;
		}
		send_record(receivefd, serv_addr);
		break;
	case 'p':
	case 'P':
		//playback_mask = 0xff;
		//send_playback(receivefd, serv_addr, playback_mask);
		break;
	case 's':
	case 'S':
		if (active_record_channel_mask) {
			active_record_channel_mask = 0;
			send_record(receivefd, serv_addr);
		}
		if (active_playback_channel_mask) {
			active_playback_channel_mask = 0;
			send_playback(receivefd, serv_addr);
		}
		break;
	case 'c':
		send_record_calibrate(receivefd, serv_addr);
		break;
	case 'C':
		send_clear_calibrate(receivefd, serv_addr);
		break;
	case ' ':
		(*show_scale_info) ++;
		*show_scale_info %= 2;
		break;
	case 'm':
	case 'M':
		carry = raw_channel_mask & 0x1;
		raw_channel_mask >>= 1;
		if (carry) raw_channel_mask |= 0x01<<(NB_CHANNEL-1);
		raw_channel_mask &= CHANNEL_MASK;
		send_transmit_raw(receivefd, serv_addr);
		break;
	case 'n':
	case 'N':
		carry = raw_channel_mask & 0x01<<(NB_CHANNEL-1);
		raw_channel_mask <<= 1;
		if (carry) raw_channel_mask |= 0x1;
		raw_channel_mask &= CHANNEL_MASK;
		send_transmit_raw(receivefd, serv_addr);
		break;
	case -57: // F10
		send_set_algo(receivefd, serv_addr, 0);
		break;
	case 27:
		cvWaitKey(1);
		return -1;
		break;
	}

	if (c >= -66 && c <= -58) { // F1 to F9
		send_set_algo(receivefd, serv_addr, c+67);
	}
	return 0;
}

/* ================================================================== */

void process_udp2(char *server_ip, int server_udp_port, IMAGE * grf,
		 CvCapture * capture,
                 CvVideoWriter *writer1,
                 CvVideoWriter *writer2,
                 int display_modulo,
		 int debut_index)
{
	struct sockaddr_in send_data;
	struct sockaddr_in serv_addr;
	SOCKET receivefd;
	int n;
	uint8_t *header;
	uint8_t tail[16];
	uint8_t channel, packet;
	uint8_t *buffer;
	char title[1024];
	DATA32 *y;
	IMAGE *frame;
	struct timeval tv;
	fd_set rfds;
	int nfds;
	int ret;
	//awl_distance *awl_dist = NULL;
	int show_scale_info = 1;
	int last_errno = 0;
	static int valid = 0;
	static uint32_t last_ts = 0;
	static int last_channel = -1;
	static int last_packet = 0x84;
	uint32_t ts;
#ifdef __WIN32__
	unsigned long error_number;
#define EINTR WSAEINTR
#define EAGAIN WSAEWOULDBLOCK
#define EMSGSIZE WSAEMSGSIZE
#define EWOULDBLOCK WSAEWOULDBLOCK
#define ERRNO error_number
//#define GETERRNO {error_number = WSAGetLastError(); if (error_number) fprintf(stderr, "Error %ld\n", error_number);}
#define GETERRNO error_number = WSAGetLastError()
	WORD wVersionRequested;
	WSADATA wsaData;

	wVersionRequested = MAKEWORD( 2, 2 );
	if( WSAStartup( wVersionRequested, &wsaData )!= 0 )
	{
		GETERRNO;
		perror("WSA Startup");
	}
#else
#define ERRNO errno
#define GETERRNO
#endif

#define BUFFER_SIZE (TRACES_SIZE * sizeof(int32_t) * 16 + 1024)
	init_socket(server_ip, server_udp_port, &receivefd, &send_data,
		    &serv_addr);

	buffer = (uint8_t *) malloc(BUFFER_SIZE);
	if (!buffer) {
		perror("y_buffers");
		exit(-1);
	}
	memset(buffer, 0, BUFFER_SIZE);
	printf("streaming...\n");
	send_date(receivefd, &serv_addr);
	send_time(receivefd, &serv_addr);
	send_transmit_raw(receivefd, &serv_addr);
	send_get_parameter(receivefd, &serv_addr, 0);
	while (1) {

		if (process_key(receivefd, &serv_addr, &show_scale_info)) return;

		FD_ZERO(&rfds);
		FD_SET(receivefd, &rfds);
		tv.tv_sec = 0;
		tv.tv_usec = 33333;
		nfds = receivefd + 1;
		ret = select(nfds, &rfds, NULL, NULL, &tv);
		if (ret == SOCKET_ERROR) {
			GETERRNO;
			if (ERRNO == EINTR || ERRNO == EAGAIN
			    || ERRNO == EWOULDBLOCK)
				continue;
			else
				if (last_errno != ERRNO) {
					last_errno = ERRNO;
					perror("select");
				}
			continue;
		}
		if (ret && FD_ISSET(receivefd, &rfds)) {
			if (last_channel == -1 || last_packet == 0x84) {
				//printf("Resync\n");
				header = buffer;
			} else {
				header += 1024;
				if (header > buffer + BUFFER_SIZE) {
					last_channel = -1;
					last_packet = 0x84;
				} else {	
					memcpy(tail, header, 16);
				}
			}
			n = recvfrom(receivefd, (void *)header, 4096 + 16,
				     0, 0, 0);
			if (n == SOCKET_ERROR) {
				GETERRNO;
				if (ERRNO == EINTR || ERRNO == EAGAIN
				    || ERRNO == EWOULDBLOCK) {
					continue;
				} else {
 					if (ERRNO != EMSGSIZE) {
						if (last_errno != ERRNO) {
							last_errno = ERRNO;
							perror("recvfrom");
						}
						continue;
					}
				}
			}
			packet = header[0];
			channel = header[2];
			ts = *((uint32_t *) (header + 4));
			if (packet < 0x81 || packet > 0x84) {
				if (packet >= 20 && packet <= 36) {
					// MSG_CH1_DISTANCE1_4 .. MSG_CH7_DISTANCE5_8
					process_distances(header);
				}
				if (packet >= 10 && packet <= 13) {
					// MSG_OBSTACLE_TRAK, MSG_OBSTACLE_VELOCITY,
					// MSG_OBSTACLE_SIZE, MSG_OBSTACLE_POSITION
					process_objects(header);
				}
				if (packet == 60) {
					// MSG_CH_DISTANCE_INTENSITY
					process_distances_intensities(header);
				}
				if (packet == 0xc2) {
					uint16_t address = *((uint16_t *)(header+2));
					float value = *((float *)(header+4));
					if (address < NUM_ALGO_PARAMS) {
						algo_parameters[address] = value;
						printf("Getting %d=%f\n", address, value);
					}
				}
				//printf ("%02X:%d\n", header[0], n);
				continue;
			}
			//printf("0x%08x 0x%08x 0x%x %d %d 0x%x %d\n", buffer, header, last_packet, channel, ts, packet, channel);
			if (channel < 0 || channel > 7) {
				//printf("Invalid channel number %d\n", channel);
				valid = 0;
				last_channel = -1;
				last_packet = 0x84;
				continue;
			}
			if (channel == last_channel && ts == last_ts) {
				if (last_packet != packet - 1) {
					//printf("Invalid packet sequence\n");
					valid = 0;
					last_channel = -1;
					last_packet = 0x84;
					continue;
				} else {
					if (packet == 0x84) {
						valid = 1;
					}
				}
			} else {
				if (packet != 0x81) {
					//printf("Missed first packet\n");
					valid = 0;
					last_channel = -1;
					last_packet = 0x84;
					continue;
				}
			}
			if (packet) memcpy(header, tail, 16);
			last_ts = ts;
			if (valid != -1) {
				last_packet = packet;
				last_channel = channel;
			}
			if (valid == 1) {
				//printf("Display\n");
				valid = 0;
				last_channel = -1;
				// UDP
				y = (int32_t*) (header - 3 * 1024 + 16);
				trace_curve(grf, y+debut_index, channel, 255, 255, 255, show_scale_info, debut_index);
				trace_display(grf);
				snprintf(title, 1024, "Data Display from %s", server_ip);
				cvShowImage(title, grf);
				// UDP
				if (capture) {
					frame = cvQueryFrame(capture);
					if (frame) {
						headup_display(frame, writer1, writer2);
						cvShowImage("Video", frame);
					}
				}
			}
		} else {
			//send_msg(receivefd, &serv_addr);
		}
	}

}

/* ================================================================== */

void process_udp(char *server_ip, int server_udp_port, IMAGE * grf,
		 CvCapture * capture,
                 CvVideoWriter *writer1,
                 CvVideoWriter *writer2,
                 int display_modulo,
		 int debut_index)
{
	struct sockaddr_in send_data;
	struct sockaddr_in serv_addr;
	SOCKET receivefd;
	int n;
	uint8_t header[16];
	uint8_t tail[16];
	uint8_t msg[1024];
	int packet;
	uint8_t ch, fr;
	uint32_t ts, last_ts;
	const int nb_buffers = 2;
	uint8_t *y_buffers[nb_buffers];
	int current_buffer;
	DATA32 *y;
	uint8_t *u;
	IMAGE *frame;
	struct timeval tv;
	fd_set rfds;
	int nfds;
	int ret;
	int i;
	//awl_distance *awl_dist = NULL;
	int show_scale_info = 1;
	int last_errno = 0;
	uint16_t trace_channel_mask;
#ifdef __WIN32__
	unsigned long error_number;
#define EINTR WSAEINTR
#define EAGAIN WSAEWOULDBLOCK
#define EMSGSIZE WSAEMSGSIZE
#define EWOULDBLOCK WSAEWOULDBLOCK
#define ERRNO error_number
//#define GETERRNO {error_number = WSAGetLastError(); if (error_number) fprintf(stderr, "Error %ld\n", error_number);}
#define GETERRNO error_number = WSAGetLastError()
	WORD wVersionRequested;
	WSADATA wsaData;

	wVersionRequested = MAKEWORD( 2, 2 );
	if( WSAStartup( wVersionRequested, &wsaData )!= 0 )
	{
		GETERRNO;
		perror("WSA Startup");
	}
#else
#define ERRNO errno
#define GETERRNO
#endif

	nb_channels = 0;
	for(i = 0; i < NB_TRACES; i++) {
		if (!(raw_channel_mask & (1<<i))) continue;
		nb_channels ++;
	}

	init_socket(server_ip, server_udp_port, &receivefd, &send_data,
		    &serv_addr);

	for (current_buffer = 0; current_buffer < nb_buffers; current_buffer++) {
		y_buffers[current_buffer] =
		    (uint8_t *) malloc(NB_TRACES * TRACES_SIZE *
				       sizeof(int32_t) + 4096);
		if (!y_buffers[current_buffer]) {
			perror("y_buffers");
			exit(-1);
		}
		memset(y_buffers[current_buffer], 0,
		       NB_TRACES * TRACES_SIZE * sizeof(int32_t) + 4096);
	}
	current_buffer = 0;
	u = y_buffers[current_buffer];
	y = (DATA32 *) u;
	printf("streaming...\n");
	send_date(receivefd, &serv_addr);
	send_time(receivefd, &serv_addr);
	send_transmit_raw(receivefd, &serv_addr);
	send_transmit_cooked(receivefd, &serv_addr);
	send_get_parameter(receivefd, &serv_addr, 0);
	while (1) {

		if (process_key(receivefd, &serv_addr, &show_scale_info)) return;

		FD_ZERO(&rfds);
		FD_SET(receivefd, &rfds);
		tv.tv_sec = 0;
		tv.tv_usec = 33333;
		nfds = receivefd + 1;
		ret = select(nfds, &rfds, NULL, NULL, &tv);
		if (ret == SOCKET_ERROR) {
			GETERRNO;
			if (ERRNO == EINTR || ERRNO == EAGAIN
			    || ERRNO == EWOULDBLOCK)
				continue;
			else
				if (last_errno != ERRNO) {
					last_errno = ERRNO;
					perror("select");
				}
			continue;
		}
		if (ret && FD_ISSET(receivefd, &rfds)) {
			n = recvfrom(receivefd, (char *)header, 16, MSG_PEEK, 0, 0);
			if (n == SOCKET_ERROR) {
				GETERRNO;
				if (ERRNO == EINTR || ERRNO == EAGAIN
				    || ERRNO == EWOULDBLOCK) {
					continue;
				} else {
 					if (ERRNO != EMSGSIZE) {
						if (last_errno != ERRNO) {
							last_errno = ERRNO;
							perror("recvfrom PEEK");
						}
						continue;
					}
				}
			}
			fr = header[0];
			if (fr == 0x80) fr = 0x81;
			ch = header[2];
			if (fr < 0x81 || fr > 0x84) {
				n = recvfrom(receivefd, (void*)msg, 1024,
					     0, 0, 0);
				if (n == SOCKET_ERROR) {
					GETERRNO;
					if (ERRNO == EINTR || ERRNO == EAGAIN
					    || ERRNO == EWOULDBLOCK)
						continue;
					else
						if (last_errno != ERRNO) {
							last_errno = ERRNO;
							perror("recvfrom");
						}
					continue;
				}
				if (fr >= 20 && fr <= 36) {
					// MSG_CH1_DISTANCE1_4 .. MSG_CH7_DISTANCE5_8
					process_distances(msg);
				}
				if (fr >= 10 && fr <= 13) {
					// MSG_OBSTACLE_TRAK, MSG_OBSTACLE_VELOCITY,
					// MSG_OBSTACLE_SIZE, MSG_OBSTACLE_POSITION
/*
if(fr == 10) {
int ii;
for (ii =  0; ii < 16 ; ii++) {
printf("%x ", msg[ii]);
}
printf("\n");
}
*/

					process_objects(msg);
				}
				if (fr == 60) {
					// MSG_CH_DISTANCE_INTENSITY
					process_distances_intensities(msg);
				}
				if (fr == 0xc2) {
					uint16_t address = *((uint16_t *)(header+2));
					float value = *((float *)(header+4));
					if (address < NUM_ALGO_PARAMS) {
						algo_parameters[address] = value;
						printf("Getting %d=%f\n", address, value);
					}
				}
				//printf ("%02X:%d\n", msg[0], n);
				continue;
			}
			//printf("Current channel number %d\n", ch);
			if (ch < 0 || ch >= NB_TRACES) {
				//printf("Invalid channel number %d\n", ch);
				continue;
			}
			ts = *((uint32_t *) (header + 4));
			//printf("ts %d\n", ts);
			if (ts != last_ts) {
				if (current_buffer) {
					y = (DATA32
					     *) (y_buffers[(current_buffer -
							    1) % nb_buffers]);
				} else {
					y = (DATA32
					     *) (y_buffers[nb_buffers - 1]);
				}
				// UDP
				//trace_curves(grf, y + 2, channel_mask, 1024, 0, 0, 0, 0, 1);
				//trace_display(grf);
				//cvShowImage("Data Display", grf);
				y = (DATA32 *)(y_buffers[current_buffer]);
				//get_distance( CURRENT_AWL_PAYLOAD_32  , AWL_PAYLOAD_SIZE_32, &0, awl);
				//trace_curves(grf, y + 2, trace_channel_mask, 255, 255, 255, 1, show_scale_info);
				//trace_curves(grf, y + 2+debut_index, trace_channel_mask, 255, 255, 255, 1, show_scale_info, debut_index);
				trace_curves_x(grf, y + 2+debut_index, trace_channel_mask, raw_channel_mask, 255, 255, 255, 1, show_scale_info, debut_index);
				clear_objects();
				//trace_display(grf);
				trace_display_x(grf, raw_channel_mask);
				cvShowImage("Data Display", grf);
				// UDP
				if (capture) {
					frame = cvQueryFrame(capture);
					if (frame) {
						headup_display(frame, writer1, writer2);
						cvShowImage("Video", frame);
					}
				}

				current_buffer =
				    (current_buffer + 1) % nb_buffers;;
				u = y_buffers[current_buffer];
				y = (DATA32 *) (y_buffers[current_buffer]);

				last_ts = ts;
				trace_channel_mask = 0;
			}
			trace_channel_mask |= 1 << ch;
			packet = ch * 4;
			packet += (fr - 0x81);
			if (packet) {
				memcpy(tail, u + (packet * 1024), 8);
			}
			//printf ("%x %d %d %d\n", fr, ch, ts, packet); 
			n = recvfrom(receivefd, (void *)u + (packet * 1024), 4096 + 16,
				     0, 0, 0);
			if (n < 0) {
				GETERRNO;
				if (ERRNO == EINTR || ERRNO == EAGAIN
				    || ERRNO == EWOULDBLOCK)
					continue;
				else
					perror("recvfrom");
				continue;
			}
			//printf ("%d %08x %08x", ch, u+
			if (packet) {
				memcpy(u + (packet * 1024), tail, 8);
			}
		} else {
			//send_msg(receivefd, &serv_addr);
		}
	}

}

/* ================================================================== */

int main(int argc, char **argv)
{

	int display_y;
	int c; 

	char capture_file[256];
	char server_ip[256];
	const int server_udp_default = 23456;
	int server_udp_port = server_udp_default;
	int display_modulo = 0;

	int camera_index = -1;
	int debut_index = 0;

	CvCapture* capture = NULL;
	CvVideoWriter *writer1 = NULL;
	CvVideoWriter *writer2 = NULL;
	char writer1_file[256];
	char writer2_file[256];
	IMAGE *frame = NULL;
	time_t now;
	int write_to_video = 0;
	fixed_scaling = 0;

	strcpy(capture_file, "");
	strcpy(server_ip, "127.0.0.1");

	cvInitFont(&font100, CV_FONT_HERSHEY_SIMPLEX, 1.0, 1.0, 0, 1, CV_AA);
	cvInitFont(&font75, CV_FONT_HERSHEY_SIMPLEX, 0.75, 0.75, 0, 1, CV_AA);
	cvInitFont(&font50, CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5, 0, 1, CV_AA);

	/*  Buffer write test */

	while ((c = getopt(argc, argv, "wf:s:S:C:m:c:d:T:M:F:T:X:Y:K:L:k:l:t:x:y:")) != -1) {
		int arg1;
		int i, l;
		//PDEBUG("%c %d %s ", c, c, optarg);
		switch (c) {
		case 'f':
			strncpy(capture_file, optarg, 256);
			break;
		case 'C':
		case 'S':
			strncpy(server_ip, optarg, 256);
			if (c == 'C') no_transmit_raw = 1;
			break;
		case 's':
			server_udp_port = atoi(optarg);
			break;
		case 'm':
			display_modulo = atoi(optarg);
			break;
		case 'c':
			camera_index = atoi(optarg);
			break;
		case 'd':
			debut_index = atoi(optarg);
			break;
		case 'w':
			write_to_video = 1;
			break;
		case 'M':
			l = strlen(optarg);
			for (i = 0; i < l && i < AWL_MAX_CHANNELS; i++) {
				if (optarg[i] >= '0' && optarg[i] < '0' + AWL_MAX_CHANNELS) {
					mapped_channels[i] = optarg[i] - '0';
				}
			}
			break;
		case 'F':
			sscanf(optarg, "%i", &arg1);
			fixed_scaling = arg1;
			break;
		case 'T':
			sscanf(optarg, "%i", &arg1);
			raw_channel_mask = arg1;
			break;
		case 'X':
			sscanf(optarg, "%i", &arg1);
			raw_payload_size = arg1;
			break;
		case 'Y':
			sscanf(optarg, "%i", &arg1);
			raw_frame_rate = arg1;
			break;
		case 'K':
			sscanf(optarg, "%i", &arg1);
			cooked_channel_mask = arg1;
			break;
		case 'L':
			sscanf(optarg, "%i", &arg1);
			cooked_frame_rate = arg1;
			break;
		case 'k':
			sscanf(optarg, "%i", &arg1);
			cooked_event_mask = arg1;
			break;
		case 'l':
			sscanf(optarg, "%i", &arg1);
			cooked_event_limit = arg1;
			break;
		case 't':
			sscanf(optarg, "%i", &arg1);
			wanted_record_channel_mask = arg1;
			break;
		case 'x':
			sscanf(optarg, "%i", &arg1);
			record_payload_size = arg1;
			break;
		case 'y':
			sscanf(optarg, "%i", &arg1);
			record_frame_rate = arg1;
			break;
		default:
			printf("Usage %s [OPTIONS]\n", argv[0]);
			printf(" where OPTIONS are:\n");
			printf("      -f CAPTURE-FILE    Select file to use as video input\n");
			printf("      -C SERVER-IP       Select IP address of sensor data server, no raw data\n");
			printf("      -S SERVER-IP       Select IP address of sensor data server\n");
			printf("      -s SERVER-UDP-PORT Select UDP port of sensor data server (defaults to %d)\n", server_udp_default);
			printf("      -m DISPLAY-MODULO  Select sensor frame skip modulo\n");
			printf("      -c CAMERA-INDEX    Select index of camera for video input (-1 = use default one)\n");
			printf("      -d DISPLAY-SKIP    Skip n point(s) in the beginning of the display ( 0 is the default )\n");
			printf("      -w                 Record video to a file name awl<DATE_AND_TIME>.avi\n");
			printf("      -M CHANNEL-MAP     Set channel remapping\n");
			printf("      -F FIXED-SCALING   Use fixed scaling\n");
			printf("      -T CHANNEL-MASK    Set transmit raw channel mask\n");
			printf("      -X PAYLOAD-SIZE    Set transmit raw payload size in samples\n");
			printf("      -Y FRAME-RATE      Set transmit raw frame rate\n");
			printf("      -K CHANNEL-MASK    Set transmit cooked channel mask\n");
			printf("      -L FRAME-RATE      Set transmit cooked frame rate\n");
			printf("      -k EVENT-MASK      Set transmit cooked event mask\n");
			printf("      -l EVENT-LIMIT     Set transmit cooked event limit\n");
			printf("      -t CHANNEL-MASK    Set recording channel mask\n");
			printf("      -x PAYLOAD-SIZE    Set recording payload size in samples\n");
			printf("      -y FRAME-RATE      Set recording frame rate\n");
			return 0;
		}
	}

	display_y = DISPLAY_Y * NB_TRACES + OFF0;
	IMAGE *grf =
	    cvCreateImage(cvSize(DISPLAY_X, display_y), IPL_DEPTH_8U, 3);
	//IMAGE *grf = cvCreateMat( display_y, DISPLAY_X, CV_8UC1 );

	cvNamedWindow("Data Display", CV_WINDOW_AUTOSIZE);

	if (strlen(capture_file)) {
		capture = cvCreateFileCapture( capture_file );
	} else {
		if (camera_index >= 0) {
			capture = cvCaptureFromCAM(camera_index);
		}
	}
	if (capture) {
		cvNamedWindow("Video", CV_WINDOW_AUTOSIZE);
		now = time(NULL);
		strftime(writer1_file, 128, "dst%Y%m%d%H%M%S.avi", localtime(&now));
		strftime(writer2_file, 128, "awl%Y%m%d%H%M%S.avi", localtime(&now));

		frame = cvQueryFrame(capture);
		if (write_to_video) {
			writer1 = cvCreateVideoWriter(writer1_file, CV_FOURCC('X','V','I','D'), 10.0, cvSize(IMAGE_WIDTH(frame), IMAGE_HEIGHT(frame)), 1);
			writer2 = cvCreateVideoWriter(writer2_file, CV_FOURCC('X','V','I','D'), 10.0, cvSize(IMAGE_WIDTH(frame), IMAGE_HEIGHT(frame)), 1);
		}
	}

	//trace_display_x(grf, channel_mask);
	cvShowImage("Data Display", grf);

	/*  DATA out of RANGE */
#ifdef __WIN32__
	process_udp2(server_ip, server_udp_port, grf, capture, writer1, writer2, display_modulo);
#else
	process_udp(server_ip, server_udp_port, grf, capture, writer1, writer2, display_modulo, debut_index);
#endif
	cvWaitKey(1);

	cvDestroyWindow("Data Display");
	cvReleaseImage(&grf);

	if (capture) {
		cvReleaseCapture(&capture);
	}
	if (writer1) {
		cvReleaseVideoWriter(&writer1);
	}
	if (writer2) {
		cvReleaseVideoWriter(&writer2);
	}
	cvDestroyWindow("Video");

	return 0;
}
