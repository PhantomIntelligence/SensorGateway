#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>

#include "awl_data.h"
#include "awlcmd.h"
#include "awlcan.h"
#include "awlfile.h"
#include "awl_data.h"
#include "awl_distance.h"
#include "awl.h"


/* ================================================================ */

int set_udp_payload_size(awl_data *awl)
{
	if (awl->raw_payload_size > AWL_PAYLOAD_SIZE_MAX) {
		return -1;
	}

	if (awl->raw_payload_size == AWL_PAYLOAD_SIZE_MAX) {
		awl->udp_payload_size = UDP_DEFAULT_PAYLOAD_SIZE;
		awl->udp_num_datagram = UDP_DEFAULT_NUM_DATAGRAM;
		return 0;
	}

	if (awl->raw_payload_size <= UDP_DEFAULT_PAYLOAD_SIZE) {
		awl->udp_payload_size = awl->raw_payload_size;
		awl->udp_num_datagram = 1;
		return 0;
	}
	awl->udp_payload_size = UDP_DEFAULT_PAYLOAD_SIZE;
	awl->udp_num_datagram = awl->raw_payload_size / UDP_DEFAULT_PAYLOAD_SIZE;
	if (awl->udp_num_datagram * UDP_DEFAULT_PAYLOAD_SIZE < awl->udp_payload_size) {
		awl->udp_num_datagram ++;
	}

	return 0;
}

/* ================================================================ */

int init_udp(awl_data *awl)
{
	int ret;

	awl->udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (awl->udp_fd < 0) {
		perror("UDP fd");
		return awl->udp_fd;
	}

	ret = fcntl(awl->udp_fd, F_GETFL, 0);
	if (ret >= 0) {
		ret = fcntl(awl->udp_fd, F_SETFL, ret | O_NONBLOCK);
	}

	printf("UDP Server binding to IP address %s, UDP port %d\n", awl->srv_saddr, awl->srv_port);
	memset(&(awl->srv_addr), 0, sizeof(awl->srv_addr));
	awl->srv_addr.sin_family = AF_INET;
	awl->srv_addr.sin_addr.s_addr = inet_addr(awl->srv_saddr);
	awl->srv_addr.sin_port = htons(awl->srv_port);

	printf("UDP Default client as IP address %s, UDP port %d\n", awl->cli_saddr, awl->cli_port);
	memset(&(awl->cli_addr), 0, sizeof(awl->cli_addr));
	awl->cli_addr.sin_family = AF_INET;
	awl->cli_addr.sin_addr.s_addr = inet_addr(awl->cli_saddr);
	awl->cli_addr.sin_port = htons(awl->cli_port);

	printf("UDP Default stream client as IP address %s, UDP port %d\n", awl->cli_saddr, awl->cli_port);
	memset(&(awl->stream_cli_addr), 0, sizeof(awl->stream_cli_addr));
	awl->stream_cli_addr.sin_family = AF_INET;
	awl->stream_cli_addr.sin_addr.s_addr = inet_addr(awl->cli_saddr);
	awl->stream_cli_addr.sin_port = htons(awl->cli_port);

	awl->raw_payload_size = awl->payload_size;
	set_udp_payload_size(awl);

	printf("UDP payload size is %d bytes, number of datagrams will be %d\n",
		awl->udp_payload_size, awl->udp_num_datagram);

	ret = bind(awl->udp_fd, (struct sockaddr*)&awl->srv_addr, sizeof(awl->srv_addr));
	if (ret < 0) {
		perror("UDP bind");
		close(awl->udp_fd);
		awl->udp_fd = -1;
		return ret;
	}
	return 0;
}

/* ================================================================ */

int process_udp(awl_data *awl)
{
	int ret;
	uint8_t buf[UDP_FRAME_SIZE_MAX];
	socklen_t size;

	size = sizeof(awl->cli_addr);
	ret = recvfrom(awl->udp_fd, buf, UDP_FRAME_SIZE_MAX, 0, (struct sockaddr*)&(awl->cli_addr), &size);
	if (ret < 0) {
		perror("UDP recvfrom");
	} else {
		if (awl->verbose_udp) {
			int i;
			printf("UDP %d < ", ret);
			for (i = 0; i < 32 && i < ret; i ++) {
				printf("%02x ", buf[i]);
			}
			printf("\n");
			if (ret > 32) { 
				printf("UDP *\nUDP ");
				for (i = ret -32; i < ret; i ++) {
					printf("%02x ", buf[i]);
				}
				printf("\n");
			}
		}
		process_cmd(awl, MSG_FROM_UDP, buf, ret);
	}
	return 0;
}

/* ================================================================ */

int send_raw_frame(awl_data *awl)
{
	int ret = 0;
	uint8_t tail[UDP_HEADER_SIZE];
	int i;
	uint8_t ch, fr;
	uint32_t ts;

	fr = awl->frame_rate;
	ch = GET_CURRENT_AWL_CHANNEL;
	ts = CURRENT_AWL_TIMESTAMP;
	//printf("UDP channel %d, transmit_raw_channel_mask %x\n", ch, awl->transmit_raw_channel_mask);
	if (!(awl->udp_transmit_raw_channel_mask & (1<<ch))) return 0;
	if (awl->udp_transmit_raw_frame_rate != awl->frame_rate) {
		if (CURRENT_AWL_TIMESTAMP % (awl->frame_rate/awl->udp_transmit_raw_frame_rate)) return 0;
	}

	//printf("UDP stream\n");
	for (i = 0; i < awl->udp_num_datagram; i++) {
		memcpy(tail, CURRENT_UDP_FRAME(i), UDP_HEADER_SIZE);
		if (awl->udp_num_datagram > 1) CURRENT_UDP_CMD(i) = CMD_RAW_1 + i;
		else CURRENT_UDP_CMD(i) = CMD_RAW_0;
		CURRENT_UDP_FRAMERATE(i) = fr;
		CURRENT_UDP_CHANNEL(i) = ch;
		CURRENT_UDP_TIMESTAMP(i) = ts;
		//printf("UDP sending 0x%02x %d %d %d\n", CURRENT_UDP_CMD(i), CURRENT_UDP_FRAMERATE(i), CURRENT_UDP_CHANNEL(i), CURRENT_UDP_TIMESTAMP(i));

		//printf ("UDP %s\n %d", inet_ntoa(awl->stream_cli_addr.sin_addr), ntohs(awl->stream_cli_addr.sin_port));
		ret = sendto(awl->udp_fd, CURRENT_UDP_FRAME(i), UDP_FRAME_SIZE, 0, (struct sockaddr*)&(awl->stream_cli_addr), sizeof(awl->stream_cli_addr));
		if (ret < 0) {
			//perror("UDP sendto");
		} else if (awl->verbose_udp) {
			int j;
			printf("UDP %d > ", ret);
			for (j = 0; j < 32 && j < ret; j ++) {
				printf("%02x ", CURRENT_UDP_FRAME(i)[j]);
			}
			printf("\n");
			if (ret > 32) { 
				printf("UDP *\nUDP ");
				for (j = ret -32; j < ret; j ++) {
					printf("%02x ", CURRENT_UDP_FRAME(i)[j]);
				}
				printf("\n");
			}
		}
		memcpy(CURRENT_UDP_FRAME(i), tail, UDP_HEADER_SIZE);
	}
	return ret;
}

/* ================================================================ */

int send_udp(awl_data *awl, int32_t id, size_t size, uint8_t *buf)
{
	const size_t max_size = 256;
	uint8_t buffer[max_size];
	uint32_t *buf32;
	size_t size2;
	int ret;

	buf32 = (uint32_t*)buffer;

/* If id < 0, don't append id to message */
	if (id < 0 || id == MSG_CONTROL_GROUP) {
		if (size > max_size) {
			size = max_size;
		}
		memcpy(buffer, buf, size);
		size2 = size;
	} else {
		if (size > (max_size - sizeof(id))) {
			size = max_size - sizeof(id);
		}
		memcpy(buffer + sizeof(id), buf, size);
		*buf32 = id;
		size2 = size + sizeof(id);
	}

	ret = 0;

	/* We don't want to redirect raw transmission to client mode transmitter */
	if (buffer[0] < MSG_CONTROL_GROUP) {
		if (awl->udp_transmit_cooked_frame_rate
			&& awl->udp_transmit_cooked_channel_mask
			&& awl->udp_transmit_cooked_event_mask) {
			ret = sendto(awl->udp_fd, buffer, size2, 0, (struct sockaddr*)&(awl->stream_cli_addr), sizeof(awl->stream_cli_addr));
		}
	} else {
		ret = sendto(awl->udp_fd, buffer, size2, 0, (struct sockaddr*)&(awl->cli_addr), sizeof(awl->cli_addr));
	}
	if (ret < 0) {
		//perror("UDP sendto");
	} else if (awl->verbose_udp) {
		int i;
		printf("UDP %d > ", ret);
		for (i = 0; i < 32 && i < ret; i ++) {
			printf("%02x ", buffer[i]);
		}
		printf("\n");
		if (ret > 32) { 
			printf("UDP *\nUDP ");
			for (i = ret -32; i < ret; i ++) {
				printf("%02x ", buffer[i]);
			}
			printf("\n");
		}
	}
	return ret;
}

/* ================================================================ */
