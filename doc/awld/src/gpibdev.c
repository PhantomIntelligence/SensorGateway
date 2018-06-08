#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "awl_data.h"

/* ================================================================ */

int init_gpibdev(awl_data *awl)
{
	int ret;

	ret = open(awl->gpib_devname, O_RDWR);
	if (awl->gpib_fd < 0) {
		perror("GPIB fd");
		return ret;
	} else awl->gpib_fd = ret;

	return 0;
}

/* ================================================================ */

int send_gpibdev(awl_data *awl, char *cmd)
{
	int ret;

	if (awl->gpib_fd < 0) return 0;
	ret = write(awl->gpib_fd, cmd, strlen(cmd));
	if (ret < 0) {
		perror("GPIB write");
		return ret;
	}
	return 0;
}

/* =============================================================== */

int recv_gpibdev(awl_data *awl, uint8_t *buffer, size_t *count)
{
	if (awl->gpib_fd < 0) return 0;
	*count = read(awl->gpib_fd, buffer, *count);
	if (*count < 0) {
		perror("GPIB read");
		return *count;
	}
	return 0;
}

/* ================================================================ */
