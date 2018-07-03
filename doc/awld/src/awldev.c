#include <stdio.h>
#include <string.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/if_ether.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "awl.h"
#include "awl_data.h"
#include "awlcan.h"
#include "awludp.h"
#include "awlcmd.h"
#include "awlmsg.h"
#include "awl_distance.h"
#include "awldev.h"

/* ================================================================ */

int write_proc(char *proc_path, char *cmd)
{
#ifdef __arm__
	int fd, ret;

	ret = open(proc_path, O_RDWR);
	if (ret < 0) {
		fprintf(stderr, "PROC ");
		perror(proc_path);
		return ret;
	}
	fd = ret;
	ret = write(fd, cmd, strlen(cmd));
	if (ret < 0) {
		fprintf(stderr, "PROC ");
		perror(cmd);
		return ret;
	}
	close(fd);
#endif
	return 0;
}

/* ================================================================ */

int read_bias(uint32_t *value)
{
#ifdef __arm__
	int fd, ret;
	char cmd[32];

	if (!value) return 1;

	ret = open(AWL_BIAS_PROC_PATH, O_RDWR);
	if (ret < 0) {
		fprintf(stderr, "BIAS ");
		perror(AWL_BIAS_PROC_PATH);
		return ret;
	}
	fd = ret;
	ret = read(fd, cmd, 32);
	if (ret < 0) {
		fprintf(stderr, "BIAS ");
		perror(cmd);
		return ret;
	}
	cmd[ret] = '\0';
	printf("BIAS %s\n", cmd);
	ret = sscanf(cmd, "%i", value);
	if (ret != 1) {
		fprintf(stderr, "AWL invalid return %s\n", cmd);
		return 2;
	}

	close(fd);
#endif
	return 0;
}

/* ================================================================ */

int read_gpio(uint16_t address, uint32_t *value)
{
#ifdef __arm__
	int fd, ret;
	char cmd[32];
	char path[PATH_MAX];

	if (!value) return 1;

	sprintf(path, "/sys/class/gpio/gpio%d/value", address);
	ret = open(path, O_RDWR);
	if (ret < 0) {
		fprintf(stderr, "GPIO ");
		perror(path);
		return ret;
	}
	fd = ret;
	ret = read(fd, cmd, 32);
	if (ret < 0) {
		fprintf(stderr, "GPIO ");
		perror(cmd);
		return ret;
	}
	cmd[ret] = '\0';
	printf("GPIO %s\n", cmd);
	ret = sscanf(cmd, "%i", value);
	if (ret != 1) {
		fprintf(stderr, "GPIO invalid return %s\n", cmd);
		return 2;
	}

	close(fd);
#endif
	return 0;
}

/* ================================================================ */

int read_register(char *proc_path, uint16_t address, uint32_t *value)
{
#ifdef __arm__
	int fd, ret;
	char cmd[32];

	if (!value) return 1;

	ret = open(proc_path, O_RDWR);
	if (ret < 0) {
		fprintf(stderr, "REG ");
		perror(proc_path);
		return ret;
	}
	fd = ret;
	sprintf(cmd, "? 0x%x\n", address);
	ret = write(fd, cmd, strlen(cmd));
	if (ret < 0) {
		fprintf(stderr, "REG ");
		perror(cmd);
		return ret;
	}
	ret = read(fd, cmd, 32);
	if (ret < 0) {
		fprintf(stderr, "REG ");
		perror(cmd);
		return ret;
	}
	cmd[ret] = '\0';
	printf("REG %s\n", cmd);
	ret = sscanf(cmd, "%hi %i", &address, value);
	if (ret != 2) {
		fprintf(stderr, "REG invalid return %s\n", cmd);
		return 2;
	}

	close(fd);
#endif
	return 0;
}

/* ================================================================ */

int dump_registers(uint32_t *buffer, size_t num_reg)
{
	return 0;
}

/* ================================================================ */

int write_bias(uint32_t value)
{
	char cmd[32];

	sprintf(cmd, "0x%x", value);
	return write_proc(AWL_BIAS_PROC_PATH, cmd);
}

/* ================================================================ */

int read_adc_register(uint16_t address, uint32_t *value)
{
	return read_register(AWL_ADC_PROC_PATH, address, value);
}

/* ================================================================ */

int write_gpio(uint16_t address, uint32_t value)
{
#ifdef __arm__
	char cmd[32];
	char path[PATH_MAX];

	sprintf(path, "/sys/class/gpio/gpio%d/value", address);
	sprintf(cmd, "%d", value);
	return write_proc(path, cmd);
#else
	return 0;
#endif
}

/* ================================================================ */

int write_adc_register(uint16_t address, uint32_t value)
{
	char cmd[32];

	sprintf(cmd, "0x%x 0x%x", address, value);
	return write_proc(AWL_ADC_PROC_PATH, cmd);
}

/* ================================================================ */

int read_awl_register(uint16_t address, uint32_t *value)
{
#ifdef __arm__
	return read_register(AWL_DEV_PROC_PATH, address, value);
#else
	switch(address) {
	default:
		*value = 0;
		break;
	case 0x1c:
		*value = 0;
		break;
	}
	return 0;
#endif
}

/* ================================================================ */

int dump_awl_registers(uint32_t *buffer, size_t num_reg)
{
	return dump_registers(buffer, num_reg);
}

/* ================================================================ */

int write_awl_register(uint16_t address, uint32_t value)
{
	char cmd[32];

	sprintf(cmd, "0x%x 0x%x", address, value);
	return write_proc(AWL_DEV_PROC_PATH, cmd);
}

/* ================================================================ */

int init_awl(awl_data *awl)
{
#ifdef __arm__
	struct ifreq ifr;
	int ret;

	awl->awl_fd = socket(PF_PACKET, SOCK_RAW, PF_UNSPEC);
	if (awl->awl_fd < 0) {
		perror("AWL fd");
		return awl->awl_fd;
	}
/*
	ret = fcntl(awl->can_fd, F_GETFL, 0);
	if (ret >= 0) {
		ret = fcntl(awl->can_fd, F_SETFL, ret | O_NONBLOCK);
	}
*/
	awl->awl_addr.sll_family = AF_PACKET;
	awl->awl_addr.sll_protocol = htons(ETH_P_ALL);

	strcpy(ifr.ifr_name, awl->awl_ifname);
	ret = ioctl(awl->awl_fd, SIOCGIFINDEX, &ifr);
	if (ret < 0) {
		perror("AWL SIOCGIFINDEX");
		close(awl->awl_fd);
		awl->awl_fd = -1;
		return ret;
	}
	awl->awl_addr.sll_ifindex = ifr.ifr_ifindex;
	if (!awl->awl_addr.sll_ifindex) {
		perror("AWL invalid interface");
		close(awl->awl_fd);
		awl->awl_fd = -1;
		return  -3;
	}

	printf("AWL using interface %s\n", awl->awl_ifname);
	ret = bind(awl->awl_fd, (struct sockaddr*)&awl->awl_addr, sizeof(awl->awl_addr));
	if (ret < 0) {
		perror("AWL bind");
		close(awl->awl_fd);
		awl->awl_fd = -1;
		return ret;
	}
#endif
	get_frame_rate(awl);
	printf ("AWL current frame rate is %d\n", awl->frame_rate);
	get_dead_zone(awl);
	printf ("AWL current dead zone is %d\n", awl->dead_zone);
	return 0;
}

/* ================================================================ */

int classic_process_awl(awl_data *awl)
{
	int ret, ch;
	static int first = 1;
	//uint8_t tail[AWL_FRAME_HEADER_SIZE];

	//PDEBUG("process awl\n");
	if (!first) {
		awl->current_frame = (awl->current_frame + 1) % AWL_MAX_FRAMES;
	}
	first = 0;
	ret = read(awl->awl_fd, CURRENT_AWL_FRAME, AWL_FRAME_SIZE_MAX);
	if (ret < 0) {
		perror("ALW read");
	} else {
		ch = GET_CURRENT_AWL_CHANNEL;
		if (ch >= 0 && ch < AWL_MAX_CHANNELS) {
			SET_CURRENT_AWL_CHANNEL(awl->mapped_channels[ch]);
		}
		memcpy(CURRENT_AWL_PAYLOAD_TRAILER, CURRENT_AWL_PAYLOAD_TRAILER_MAX, AWL_PAYLOAD_TRAILER_SIZE);
		//memcpy(tail, CURRENT_AWL_FRAME + AWL_PAYLOAD_SIZE, AWL_FRAME_HEADER_SIZE);
		SET_CURRENT_AWL_TAIL_CHANNEL_RECEIVE_COUNTER(awl->awl_frame_counter);
		awl->awl_frame_counter ++;
		if (awl->verbose_awl) {
			int i;
			printf("AWL %d %d < ", ch, ret);
			for (i = 0; i < 32 && i < ret; i ++) {
				printf("%02x ", CURRENT_AWL_FRAME[i]);
			}
			printf("\n");
			if (ret > 32) {
				printf("AWL *\nAWL ");
				for (i = ret -32; i < ret; i ++) {
					printf("%02x ", CURRENT_AWL_FRAME[i]);
				}
				printf("\n");
			}
		}
		if (!awl->playback_channel_mask) {
			main_job(awl);
		}
		//memcpy(CURRENT_AWL_FRAME + AWL_PAYLOAD_SIZE, tail, AWL_FRAME_HEADER_SIZE);
//		if (awl->save_to_file) write_data_log(CURRENT_FRAME - AWL_FRAME_HEADER_SIZE);
	}
	return 0;
}

/* ================================================================ */

int mmap_process_awl(awl_data *awl)
{
	int ret, ch;
	uint8_t tail[AWL_FRAME_HEADER_SIZE];
	uint8_t buffer[64];


	ret = read(awl->awl_fd, buffer, 64);
	if (ret < 0) {
		perror("AWL read");
	} else {
		ch = GET_CURRENT_AWL_CHANNEL;
		if (ch >= 0 && ch < AWL_MAX_CHANNELS) {
			SET_CURRENT_AWL_CHANNEL(awl->mapped_channels[ch]);
		}
		memcpy(tail, CURRENT_AWL_FRAME + AWL_PAYLOAD_SIZE_MAX, AWL_FRAME_HEADER_SIZE);
		SET_CURRENT_AWL_TAIL_CHANNEL_RECEIVE_COUNTER(awl->awl_frame_counter);
		awl->awl_frame_counter ++;
		if (awl->verbose_awl) {
			int i;
			printf("AWL %d %d < ", ch, ret);
			for (i = 0; i < 32 && i < ret; i ++) {
				printf("%02x ", CURRENT_AWL_FRAME[i]);
			}
			printf("\n");
			if (ret > 32) {
				printf("AWL *\nAWL ");
				for (i = ret -32; i < ret; i ++) {
					printf("%02x ", CURRENT_AWL_FRAME[i]);
				}
				printf("\n");
			}
		}
		if (!awl->playback_channel_mask) {
			main_job(awl);
		}
		memcpy(CURRENT_AWL_FRAME + AWL_PAYLOAD_SIZE_MAX, tail, AWL_FRAME_HEADER_SIZE);
//		if (awl->save_to_file) write_data_log(CURRENT_FRAME - AWL_FRAME_HEADER_SIZE);
	}
	return 0;
}

/* ================================================================ */

int process_awl(awl_data *awl)
{
	if (awl->mmap_mode) {
		return mmap_process_awl(awl);
	} else {
		return classic_process_awl(awl);
	}
}

/* ================================================================ */

int get_temperature(awl_data *awl)
{
	return 500;
}

/* ================================================================ */

int get_operating_voltage(awl_data *awl)
{
	return 180;
}

/* ================================================================ */

int get_dead_zone(awl_data *awl)
{
	int ret;
	uint32_t dead_zone;
	uint32_t interleave_reg;
	int interleave;

	if (awl->dead_zone_override >= 0) {
		awl->dead_zone = awl->dead_zone_override;
		return awl->dead_zone;
	}

	ret = read_awl_register(0x24, &interleave_reg);
	switch (interleave_reg & 0x00000030) {
	default:
	case 0:
		interleave = 1;
		break;
	case 0x10:
		interleave = 2;
		break;
	case 0x20:
		interleave = 4;
		break;
	case 0x30:
		interleave = 8;
		break;
	}

	ret = read_awl_register(0x1c, &dead_zone);
	if (!ret) {
		dead_zone *= interleave;
		awl->dead_zone = dead_zone;
		return awl->dead_zone;
	}
	return 0;
}

/* ================================================================ */

int get_frame_rate(awl_data *awl)
{
	int ret;
	uint32_t frame_rate;

#ifdef __arm__
	ret = read_awl_register(0x14, &frame_rate);
#else
	ret = 0;
	frame_rate = awl->frequency;
#endif
	if (!ret) {
		awl->frame_rate = frame_rate;
		awl->record_frame_rate = awl->frame_rate;
		awl->udp_transmit_raw_frame_rate = awl->frame_rate;
		awl->can_transmit_cooked_frame_rate = awl->frame_rate;
		return awl->frame_rate;
	}
	return 0;
}

/* ================================================================ */

int set_frame_rate(awl_data *awl, int frame_rate)
{
	if (frame_rate < 0 || frame_rate > 150) return -1;
	if (frame_rate) {
		awl->frame_rate = frame_rate;
		awl->record_frame_rate = awl->frame_rate;
		awl->udp_transmit_raw_frame_rate = awl->frame_rate;
		awl->can_transmit_cooked_frame_rate = awl->frame_rate;
	}
	return awl->frame_rate;
}

/* ================================================================ */

int send_sensor_monitor_status(awl_data *awl)
{
	int ret;
	uint8_t msg[8];
	int16_t *msg16;

	msg16 = (int16_t*)msg;

	msg16[0] = get_temperature(awl);
	msg16[1] = get_operating_voltage(awl);
	msg[3] = awl->frame_rate;
	msg[4] = awl->hardware_error;
	msg[6] = awl->receiver_error;
	msg[7] = awl->reporting_status;
	ret = send_msg(awl, MSG_SENSOR_MONITOR_STATUS, msg, 8);
	return ret;
}

/* ================================================================ */

int send_sensor_boot(awl_data *awl)
{
	int ret;
	uint8_t msg[8];

	memset(msg, 0, 8);
	msg[0] = awl->sensor_version_major;
	msg[1] = awl->sensor_version_minor;
	msg[2] = awl->checksum_ok;
	msg[3] = awl->bist_ok;
	ret = send_msg(awl, MSG_SENSOR_BOOT, msg, 8);
	return ret;
}

/* ================================================================ */
