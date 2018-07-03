#include <stdio.h>
#include <string.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>

#include "awl_data.h"
#include "awlcan.h"
#include "awlmsg.h"
#include "awlcmd.h"

/* ================================================================ */

int init_can(awl_data *awl)
{
	struct ifreq ifr;
	int ret;

	awl->can_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	if (awl->can_fd < 0) {
		perror("CAN fd");
		return awl->can_fd;
	}

	ret = fcntl(awl->can_fd, F_GETFL, 0);
	if (ret >= 0) {
		ret = fcntl(awl->can_fd, F_SETFL, ret | O_NONBLOCK);
	}

	awl->can_addr.can_family = AF_CAN;

	strcpy(ifr.ifr_name, awl->can_ifname);
	ret = ioctl(awl->can_fd, SIOCGIFINDEX, &ifr);
	if (ret < 0) {
		perror("CAN SIOCGIFINDEX");
		close(awl->can_fd);
		awl->can_fd = -1;
		return ret;
	}
	awl->can_addr.can_ifindex = ifr.ifr_ifindex;
	if (!awl->can_addr.can_ifindex) {
		perror("CAN invalid interface");
		close(awl->can_fd);
		awl->can_fd = -1;
		return -3;
	}

	printf("CAN Using interface %s\n", awl->can_ifname);

	ret = bind(awl->can_fd, (struct sockaddr*)&awl->can_addr, sizeof(awl->can_addr));
	if (ret < 0) {
		perror("CAN bind");
		close(awl->can_fd);
		awl->can_fd = -1;
		return ret;
	}
	return 0;
}

/* ================================================================ */

int process_can(awl_data *awl)
{
	struct can_frame cf;
	size_t size = sizeof(cf);
	int ret;

	ret = read(awl->can_fd, &cf, size);
	if (ret < 0) {
		perror("CAN read");
	} else {
		if (cf.can_dlc && awl->verbose_can) {
			int i;
			uint8_t *buf = (uint8_t*)&cf;
			printf ("CAN 0x%02x %d < ", cf.can_id, cf.can_dlc);
			for (i =0; i<sizeof(cf); i++) {
				printf ("%02x ", buf[i]);
			}
			printf ("\n");
		}
		if (cf.can_id == MSG_CONTROL_GROUP) {
			process_cmd(awl, cf.can_id, cf.data, cf.can_dlc);
		}
	}
	return 0;
}

/* ================================================================ */

int send_can(awl_data *awl, int32_t id, size_t dlc, uint8_t *buffer)
{
	int ret;
	struct can_frame cf;

	if (awl->can_fd < 0) return 0;

	memset(&cf, 0, sizeof(cf));
	if (dlc > 8) dlc = 8;

	cf.can_id = id;
	cf.can_dlc = dlc;
	memcpy(cf.data, buffer, dlc);

	ret = write(awl->can_fd, &cf, sizeof(cf));
#ifndef __arm__
	ret = 1;
#endif
	if (ret > 0 && awl->verbose_can) {
		int i;
		uint8_t *buf = (uint8_t*)&cf;
		printf ("CAN %2x %d > ", cf.can_id, cf.can_dlc);
		for (i =0; i<sizeof(cf); i++) {
			printf ("%02x ", buf[i]);
		}
		printf ("\n");
	}
	return ret;
}

/* ================================================================ */
