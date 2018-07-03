#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "awl.h"
#include "awl_data.h"
#include "awludp.h"
#include "gpibdev.h"
#include "awl_distance.h"
#include "awlcmd.h"


/* ================================================================ */

int init_gpib(awl_data *awl)
{
	int ret;
	const size_t buffer_size = 4096;
	size_t count;
	uint8_t buffer[buffer_size];

	ret = init_gpibdev(awl);
	if (ret) return ret;

	ret = send_gpibdev(awl, "DAT:SOU MATH");
	if (ret) return ret;
	ret = send_gpibdev(awl, "DAT:ENC SRP");
	if (ret) return ret;
	ret = send_gpibdev(awl, "WFMO:BYT_N 2");
	if (ret) return ret;
	ret = send_gpibdev(awl, "DAT:STAR 1");
	if (ret) return ret;
	ret = send_gpibdev(awl, "DAT:STOP 1000");
	if (ret) return ret;
	ret = send_gpibdev(awl, "*OPC");
	if (ret) return ret;
	ret = send_gpibdev(awl, "WFMO?");
	if (ret) return ret;
	count = buffer_size;
	ret = recv_gpibdev(awl, buffer, &count);
	if (ret) return ret;
	return 0;
}

/* ================================================================ */

int process_gpib(awl_data *awl)
{
	int ret;
	static int first = 1;
	static uint32_t ts = 0;
	const size_t buffer_size = 4096;
	size_t count;
	uint8_t buffer[buffer_size];
	int i;
	int len_size, len;
	char len_string[16];

	if (!first) {
		awl->current_frame = (awl->current_frame + 1) % AWL_MAX_FRAMES;
	}
	first = 0;

	ret = send_gpibdev(awl, "CURV?");
	if (ret) return ret;
	count = buffer_size;
	ret = recv_gpibdev(awl, buffer, &count);
	if (ret) return ret;
	//printf ("GPIB c: %d\n", (int)count);
	if (count < 1000) return -1;
	len_size = buffer[1] - '0';
	//printf ("GPIB ls: %d\n", len_size);
	if (len_size > 16) return -1;
	strncpy(len_string, (char*)(buffer+2), len_size);
	len = atoi(len_string) / 2;
	//printf("GPIB l: %d\n", len);
	for (i = 0; i < len; i ++) {
		CURRENT_AWL_PAYLOAD[i] = (((uint16_t*)buffer)[i])<<16;
	}
	memset(CURRENT_AWL_FRAME, 0, AWL_FRAME_HEADER_SIZE);
	CURRENT_AWL_CMD = CMD_RAW_0;
	CURRENT_AWL_TIMESTAMP = ts;
	if (awl->verbose_gpib) {
		int i;
		printf("GPIB %d < ", len);
		for (i = 0; i < 32 && i < len; i ++) {
			printf("%02x ", CURRENT_AWL_FRAME[i]);
		}
		printf("\n");
		if (len > 32) {
			printf("GPIB *\nGPIB ");
			for (i = len -32; i < len; i ++) {
				printf("%02x ", CURRENT_AWL_FRAME[i]);
			}
			printf("\n");
		}
	}
	if (!awl->playback_channel_mask) {
		main_job(awl);
	}
	ts ++;
	return 0;
}

/* ================================================================ */

