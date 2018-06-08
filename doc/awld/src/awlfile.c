#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <errno.h>

#include "awl.h"
#include "awl_data.h"
#include "awludp.h"
#include "awl_distance.h"
#include "awlcmd.h"
#include "convert_raw.h"

#define FILE_SIGNATURE_OFFSET    0x00	//: Signature
#define MAX_MAJOR_REVISION 1
#define MAX_MINOR_REVISION 9

#define MIN_MAJOR_REV_DEAD_ZONE 1
#define MIN_MINOR_REV_DEAD_ZONE 7

#define FILE_MAJOR_REV_OFFSET    0x08	//:
#define FILE_MINOR_REV_OFFSET    0x09	//:
#define FILE_LENGTH_OFFSET       0x0A	//: Header length, in bytes, U16_LE (includes signature, major, minor and header length)
#define FILE_NB_REC_OFFSET       0x0C	//: Number of records, U32_LE (0x00000000 = unknown)
#define FILE_DATE_OFFSET         0x10	//: Start of Recording Date, ISO-8601 compact form ASCII (YYYYMMDDHHMMSS)
#define FILE_MODEL_OFFSET        0x20	//: Unit Model, 2 bytes
#define FILE_SERIAL_OFFSET       0x22	//: Unit Serial, 6 bytes
#define FILE_FRAME_RATE_OFFSET   0x28	//: GLobal frame rate
#define FILE_CHANNEL_MASK_OFFSET 0x29	//: Channel mask for 16 channels, use extension for more
#define FILE_CHANNEL_MAP_OFFSET  0x30	//: Channel map
#define FILE_CHANNEL_MAP_NUM_CHANNELS  0x08	//: Channel map is for 8 channels, use extension for more
#define FILE_DEAD_ZONE_OFFSET    0x38   //: Dead zone value
#define FILE_HEADER_EXT_OFFSET   0x40	//: Optional header extensions and padding

#define REC_TYPE_OFFSET         0x00	//: Data format (0x90 = I32_LE)
#define REC_FRAME_RATE_OFFSET   0x01	//: Frame rate (0x00 = use global)
#define REC_CHANNEL_MASK_OFFSET 0x02	//: Channel mask, U16_LE
#define REC_SIZE_OFFSET         0x04	//: Payload Lenght, in bytes, U32_LE
#define REC_DATA_OFFSET         0x08	//: Payload Data

#define REC_HEADER_SIZE  0x08
#define CH_HEADER_SIZE  0x08
#define HEADER_EXTENSION_HEADER_SIZE 0x04

#define HEADER_EXT_FPGA_REG_DUMP 0x0a04;
#define HEADER_EXT_ADC_REG_DUMP  0x0b02;
#define HEADER_EXT_CHANNEL_MASK  0x0101; //: Extension of channel mask for more than 16 channels
#define HEADER_EXT_CHANNEL_MAP   0x0201; //: Extension of channel mapping for more than 8 channels
#define FPGA_DUMP_NUM_REG 32
#define FILE_FPGA_DUMP_SIZE (HEADER_EXTENSION_HEADER_SIZE + FPGA_DUMP_NUM_REG * sizeof(uint32_t))

#define FILE_HEADER_SIZE FILE_HEADER_EXT_OFFSET
//#define FILE_HEADER_SIZE (FILE_HEADER_EXT_OFFSET + FILE_FPGA_DUMP_SIZE)

#define CH_TS_OFFSET         0x00	//: Timestamp U32_LE
#define CH_STATUS_OFFSET     0x04	//: Status U32_LE

#define REC_TYPE_MULTI_RAW 0x90

//      log_data_file( format, 0,  ts,  size, filename); open filename
//      log_data_file( format, ch,  ts,  size, data);
//      log_data_file( format, 0,  ts,  size, NULL);  close filename

static awl_data *static_awl = NULL;

static uint32_t last_tail[AWL_MAX_CHANNELS][4];

/* ================================================================== */

static void init_file_header(awl_data * awl, char buffer[])
{
	memset(buffer, 0, FILE_HEADER_SIZE);
	strcpy(buffer, "PHANTOM01");	//Signature
	buffer[FILE_MAJOR_REV_OFFSET] = MAX_MAJOR_REVISION;	//Version
	buffer[FILE_MINOR_REV_OFFSET] = MAX_MINOR_REVISION;	//Revision
	buffer[FILE_LENGTH_OFFSET] = FILE_HEADER_SIZE;	//Head Lenght
	time_t now = time(NULL);	//Time Stamp
	strftime(buffer + FILE_DATE_OFFSET, 20, "%Y%m%d%H%M%S",
		 localtime(&now));
	buffer[FILE_DATE_OFFSET + 14] = '\0';
	buffer[FILE_FRAME_RATE_OFFSET] = awl->record_frame_rate & 0xff;
	*((uint16_t *) (buffer + FILE_DEAD_ZONE_OFFSET)) = awl->dead_zone & 0xffff;
	*((uint8_t *) (buffer + FILE_CHANNEL_MASK_OFFSET)) =
	    awl->record_channel_mask & 0xff;
	if (FILE_HEADER_SIZE < FILE_CHANNEL_MAP_OFFSET + FILE_CHANNEL_MAP_NUM_CHANNELS) {
		fprintf(stderr, "REC ERROR FILE HEADER SIZE\n");
		exit(0);
	}
	//((uint16_t*)(buffer + FILE_HEADER_EXTENSIONS))[0] = HEADER_EXT_FPGA_REG_DUMP;
	//((uint16_t*)(buffer + FILE_HEADER_EXTENSIONS))[1] = FPGA_DUMP_NUM_REG * sizeof(uint32_t);
	//dump_awl_registers((uint32_t*)(buffer + FILE_HEADER_EXTENSIONS), FPGA_DUMP_NUM_REG);
	memcpy(buffer + FILE_CHANNEL_MAP_OFFSET, awl->mapped_channels,
	       FILE_CHANNEL_MAP_NUM_CHANNELS);
	
}

/* ================================================================== */

int start_recording(awl_data * awl)
{
	char headbuff[FILE_HEADER_SIZE];
	char date[SADDR_SIZE];
	time_t now;
	int n;
	char *filename;
	char path[PATH_MAX];
	char cwd[PATH_MAX];
	size_t len;

	now = time(NULL);
	strftime(date, 256, "%Y%m%d%H%M%S", localtime(&now));

	//if (awl->record_fp) printf ("REC %s RFP %x\n", __PRETTY_FUNCTION__, awl->record_fp);

	if (!(strlen(awl->record_filename))) {
		snprintf(awl->record_filename, PATH_MAX, "awl%s.bin", date);
		awl->record_filename[PATH_MAX - 1] = '\0';
	}
	awl->record_num_records = 0;

	if (awl->record_fp) {
		fprintf(stderr, "REC stray record_fp\n");
		fclose(awl->record_fp);
		awl->record_fp = NULL;
	}
	switch(awl->calibration_mode) {
	default:
		filename = awl->calibration_filename;
		break;
	case CALIBRATION_MODE_NONE:
	case CALIBRATION_MODE_USE:
		filename = awl->record_filename;
		break;
	}
	strcpy(path, awl->record_path_prefix);
	len = strlen(path);
	strncat(path, filename, PATH_MAX - len - 1);
	awl->record_fp = fopen(path, "w+");
	if (!(awl->record_fp)) {
		fprintf(stderr, "REC %s fopen, ", __PRETTY_FUNCTION__);
		perror(path);
		return errno;
	}
	init_file_header(awl, headbuff);
	n = fwrite(headbuff, FILE_HEADER_SIZE, 1, awl->record_fp);
	if (n != 1) {
		fprintf(stderr, "REC %s fwrite, ", __PRETTY_FUNCTION__);
		perror("");
		fclose(awl->record_fp);
		awl->record_fp = NULL;
		return errno;
	}
	fflush(awl->record_fp);
	awl->record_pos = ftell(awl->record_fp);
	getcwd(cwd, PATH_MAX);
	printf("REC working directory %s\n", cwd);
	printf("REC open filename %s, fileno %d\n", path, fileno(awl->record_fp));

	return 0;
}

/* ================================================================== */

int close_raw_record(awl_data * awl)
{
	char buf[REC_HEADER_SIZE];
	int pos;
	int n;

	//if (awl->record_fp) printf ("REC %s RFP %x\n", __PRETTY_FUNCTION__, awl->record_fp);
	if (awl->record_real_channel_mask == awl->record_channel_mask)
		return 0;
	if (awl->record_fp) {
		fflush(awl->record_fp);
		pos = ftell(awl->record_fp);
		if (pos < 0) {
			fprintf(stderr, "REC %s ftell, ", __PRETTY_FUNCTION__);
			perror("");
			fclose(awl->record_fp);
			awl->record_fp = NULL;
			return errno;
		}
		n = fseek(awl->record_fp, awl->record_pos, SEEK_SET);
		if (n) {
			fprintf(stderr, "REC %s fseek header, ", __PRETTY_FUNCTION__);
			perror("");
			fclose(awl->record_fp);
			awl->record_fp = NULL;
			return errno;
		}
		buf[REC_TYPE_OFFSET] = REC_TYPE_MULTI_RAW;
		buf[REC_FRAME_RATE_OFFSET] = awl->record_frame_rate;
		*((uint16_t *) (buf + REC_CHANNEL_MASK_OFFSET)) =
		    awl->record_real_channel_mask;
		*((uint32_t *) (buf + REC_SIZE_OFFSET)) =
		    awl->record_real_num_channels * AWL_PAYLOAD_SIZE_RECORD;
		n = fwrite(buf, REC_HEADER_SIZE, 1, awl->record_fp);
		if (n != 1) {
			fprintf(stderr, "REC %s fwrite, ", __PRETTY_FUNCTION__);
			perror("");
			fclose(awl->record_fp);
			awl->record_fp = NULL;
			return errno;
		}
		fflush(awl->record_fp);
		awl->record_pos = pos;
		n = fseek(awl->record_fp, awl->record_pos, SEEK_SET);
		if (n) {
			fprintf(stderr, "REC %s fseek end, ", __PRETTY_FUNCTION__);
			perror("");
			fclose(awl->record_fp);
			awl->record_fp = NULL;
			return errno;
		}
	}
	return 0;
}

/* ================================================================== */

int stop_recording(awl_data * awl)
{
	int n;
	char cmd[PATH_MAX * 2];

	cmd[0] = '\0';
//printf ("REC calibration mode is %d\n", awl->calibration_mode);
	switch(awl->calibration_mode) {
	default:
		if (strlen(awl->post_calibration_script)) {
			snprintf(cmd, PATH_MAX * 2, "%s %s",
			        awl->post_calibration_script,
			        awl->calibration_filename);
			cmd[PATH_MAX * 2 - 1] = '\0';
		}
		break;
	case CALIBRATION_MODE_NONE:
	case CALIBRATION_MODE_USE:
		if (strlen(awl->post_recording_script)) {
			snprintf(cmd, PATH_MAX * 2, "%s %s",
			        awl->post_recording_script,
			        awl->record_filename);
			cmd[PATH_MAX * 2 - 1] = '\0';
		}
		break;
	}

	//if (awl->record_fp) printf ("REC %s RFP %x\n", __PRETTY_FUNCTION__, awl->record_fp);
	awl->record_filename[0] = '\0';
	if (awl->record_fp) {
		if (awl->record_num_records)
			close_raw_record(awl);

		n = fseek(awl->record_fp, FILE_NB_REC_OFFSET, SEEK_SET);
		if (n) {
			fprintf(stderr, "REC %s fseek header, ", __PRETTY_FUNCTION__);
			perror("");
			fclose(awl->record_fp);
			awl->record_fp = NULL;
			return errno;
		}
		n = fwrite(&awl->record_num_records, sizeof(uint32_t), 1,
			   awl->record_fp);
		if (n != 1) {
			fprintf(stderr, "REC %s fwrite, ", __PRETTY_FUNCTION__);
			perror("");
			fclose(awl->record_fp);
			awl->record_fp = NULL;
			return errno;
		}
		fflush(awl->record_fp);
		fclose(awl->record_fp);
		awl->record_fp = NULL;
		awl->record_channel_mask = 0;
		printf("REC close  wrote %d track\n", awl->record_num_records);
		if (strlen(cmd)){
			printf("REC executing %s\n", cmd);
			system(cmd);
		}
	}
	return 0;
}

/* ================================================================== */

int start_raw_record(awl_data * awl)
{
	char buf[REC_HEADER_SIZE];
	static_awl = awl;
	int n;

	//if (awl->record_fp) printf ("REC %s RFP %x\n", __PRETTY_FUNCTION__, awl->record_fp);
	if (awl->record_num_records)
		close_raw_record(awl);

	if (awl->record_fp) {
		awl->record_real_num_channels = 0;
		awl->record_real_channel_mask = 0;
		awl->record_num_records++;
		awl->record_pos = ftell(awl->record_fp);
		buf[REC_TYPE_OFFSET] = REC_TYPE_MULTI_RAW;
		buf[REC_FRAME_RATE_OFFSET] = awl->record_frame_rate;
		*((uint16_t *) (buf + REC_CHANNEL_MASK_OFFSET)) =
		    awl->record_channel_mask;
		*((uint32_t *) (buf + REC_SIZE_OFFSET)) =
		    awl->record_num_channels * AWL_PAYLOAD_SIZE_RECORD;
		n = fwrite(buf, REC_HEADER_SIZE, 1, awl->record_fp);
		if (n != 1) {
			fprintf(stderr, "REC %s fwrite, ", __PRETTY_FUNCTION__);
			perror("");
			fclose(awl->record_fp);
			awl->record_fp = NULL;
			return errno;
		}
		//fflush(awl->record_fp);
	}
	return 0;
}

/* ================================================================== */

int addto_raw_record(awl_data * awl)
{
	int n;
	int ch;

	ch = GET_CURRENT_AWL_CHANNEL;
	//if (awl->record_fp) printf ("REC %s RFP %x\n", __PRETTY_FUNCTION__, awl->record_fp);
	if ((!awl->last_ts[ch])||(awl->last_ts[ch] != CURRENT_AWL_TIMESTAMP)) {
		start_raw_record(awl);
	}
	SET_CURRENT_AWL_TAIL_FILE_RECORD_COUNTER(awl->record_num_records);
	if (awl->record_fp) {
		n = fwrite(CURRENT_AWL_PAYLOAD, AWL_PAYLOAD_SIZE_RECORD, 1,
			   awl->record_fp);
		if (n != 1) {
			fprintf(stderr, "REC %s fwrite returns %d, ftell %ld, fileno %d, eof %d, error %d, ",
			       __PRETTY_FUNCTION__,
			       n,
			       ftell(awl->record_fp),
			       fileno(awl->record_fp),
			       feof(awl->record_fp),
			       ferror(awl->record_fp));
			perror("");
			fclose(awl->record_fp);
			awl->record_fp = NULL;
			return errno;
		}
		awl->record_real_num_channels++;
		awl->record_real_channel_mask |= (1 << ch);
		//fflush(awl->record_fp);
		//printf("REC write %x\n", AWL_PAYLOAD_SIZE_RECORD);
		//printf("REC ftell %lx\n", ftell(awl->record_fp));
	}
	return 0;
}

/* ================================================================== */

int stop_playback(awl_data * awl)
{
	if (awl->playback_fp) {
		fclose(awl->playback_fp);
		awl->playback_fp = NULL;
		get_frame_rate(awl);
		get_dead_zone(awl);
		//if (awl->verbose_playback)
			printf("PBK played back %d records\n",
			       awl->playback_num_records);
	}
	return 0;
}

/* ================================================================== */

int start_playback(awl_data * awl)
{
	char head[FILE_HEADER_SIZE];
	uint16_t head_len;
	int major_rev, minor_rev;
	int n;
	char path[PATH_MAX];
	size_t len;

	if (awl->calibration_mode == CALIBRATION_MODE_USE) {
		strcpy(path, awl->calibration_filename);
	} else {
		//printf("PBK Gonna play %s\n", awl->playback_filename);
		awl->playback_loop_count++;
		if (awl->playback_loop_max
		    && (awl->playback_loop_count > awl->playback_loop_max))
			exit(0);

		if (!awl->playback_channel_mask)
			return 0;

		if (!strlen(awl->playback_filename)) {
			strcpy(awl->playback_filename, awl->record_filename);
		}
		if (!strlen(awl->playback_filename)) {
			return -1;
		}
		strcpy(path, awl->playback_path_prefix);
		len = strlen(path);
		strncat(path, awl->playback_filename, PATH_MAX - len - 1);
	}

	awl->playback_fp = fopen(path, "r");
	if (!(awl->playback_fp)) {
		fprintf(stderr, "PBK %s fopen, ", __PRETTY_FUNCTION__);
		perror(path);
		return errno;
	}

	n = fread(head, FILE_HEADER_SIZE, 1, awl->playback_fp);
	if (n != 1) {
		fprintf(stderr, "PBK %s fread, ", __PRETTY_FUNCTION__);
		perror(path);
		stop_playback(awl);
		return errno;
	}

	if (strncmp("PHANTOM01", head, 8)) {
	if (strncmp("AEROSTAR1", head, 8)) {
		fprintf(stderr, "PBK %s: unsuported file format\n",
			awl->playback_filename);
		stop_playback(awl);
		return -2;
	}
	}

	major_rev = head[FILE_MAJOR_REV_OFFSET];
	minor_rev = head[FILE_MINOR_REV_OFFSET];

	if (major_rev > MAX_MAJOR_REVISION
	    || minor_rev > MAX_MINOR_REVISION) {
		fprintf(stderr,
			"PBK Unsupported file version %d.%d (supporting up to %d.%d)\n",
			major_rev, minor_rev,
			MAX_MAJOR_REVISION, MAX_MINOR_REVISION);
		stop_playback(awl);
		return -2;
	}
	if (awl->verbose_playback)
		printf("BPK revision %d.%d\n", major_rev, minor_rev);

	head_len = *((uint16_t *) (head + FILE_LENGTH_OFFSET));
	if (head_len > FILE_HEADER_SIZE) {
		fprintf(stderr, "PBK File Format Error %s, header size %d > %d\n",
			awl->playback_filename, head_len, FILE_HEADER_SIZE);
		exit(1);
	}
	if (head[FILE_FRAME_RATE_OFFSET]) {
		set_frame_rate(awl, head[FILE_FRAME_RATE_OFFSET]);
	}
	if ((major_rev == MIN_MAJOR_REV_DEAD_ZONE && minor_rev >= MIN_MINOR_REV_DEAD_ZONE)
	    || major_rev > MIN_MAJOR_REV_DEAD_ZONE) {
		awl->dead_zone = *((uint16_t *)(head + FILE_DEAD_ZONE_OFFSET));
	}
	if (awl->verbose_playback) {
		printf("PBK %d (0x%x) records in file\n",
		       *((uint32_t *) (head + FILE_NB_REC_OFFSET)),
		       *((uint32_t *) (head + FILE_NB_REC_OFFSET)));
		printf("PBK global frame rate is %d Hz\n", awl->frame_rate);
		printf("PBK dead zone is %d\n", awl->dead_zone);
		printf("PBK global channel mask is 0x%x\n", *((uint8_t *) (head + FILE_CHANNEL_MASK_OFFSET)));
		printf("PBK seeking to %d bytes\n", head_len);
	}
	if (head_len >= FILE_CHANNEL_MAP_OFFSET + FILE_CHANNEL_MAP_NUM_CHANNELS) {
		//memcpy(awl->mapped_channels, head+FILE_CHANNEL_MAP_OFFSET, FILE_CHANNEL_MAP_NUM_CHANNELS);
		if (awl->verbose_playback) {
			int i;
			printf("PBK channels mapped as");
			for (i = 0; i < AWL_MAX_CHANNELS; i++) {
				printf(" %d", awl->mapped_channels[i]);
			}
			printf("\n");
		}
	}
	n = fseek(awl->playback_fp, head_len, SEEK_SET);
	if (n) {
		fprintf(stderr, "PBK %s fseek, ", __PRETTY_FUNCTION__);
		perror("");
		stop_playback(awl);
		return errno;
	}
	awl->playback_num_records = 0;

//printf("open %d\n", ftell(awl->playback_fp));
	return 0;
}

/* ================================================================== */

int read_raw_record(awl_data * awl)
{
	char head[REC_HEADER_SIZE];
	int n;
	int channel;
	uint16_t channel_mask;
	int num_channels;
	uint8_t record_type;
	uint32_t remain_len, read_len;
	uint32_t ts, st;
	uint16_t rcv, rcn;

	if (!(awl->playback_fp)) {
		printf("PBK No playback file\n");
		return -1;
	}

	if (awl->verbose_playback) {
		printf("PBK at %ld (0x%lx)\n", ftell(awl->playback_fp),
		       ftell(awl->playback_fp));
	}
	n = fread(head, REC_HEADER_SIZE, 1, awl->playback_fp);
	if (n != 1) {
		int eof;
		int error;
		eof = feof(awl->playback_fp);
		error = ferror(awl->playback_fp);
		if (!eof) {
			fprintf(stderr, "PBK %s fread header, ", __PRETTY_FUNCTION__);
			perror("");
		}
		stop_playback(awl);
		if (eof) return -1;
		return error;
	}
	record_type = head[REC_TYPE_OFFSET];

	switch (record_type) {
	default:
		break;
	case REC_TYPE_MULTI_RAW:
		awl->playback_num_records++;
		channel_mask =
		    (*((uint16_t *) (head + REC_CHANNEL_MASK_OFFSET)));
		num_channels = 0;
		for (channel = 0; channel < AWL_MAX_CHANNELS; channel++) {
			if (channel_mask & 1 << channel) num_channels++;
		}
		remain_len = *((uint32_t *) (head + REC_SIZE_OFFSET));
		if (num_channels) {
			awl->playback_payload_size = remain_len / num_channels;
		} else {
			awl->playback_payload_size = 0;
		}
		if (awl->verbose_playback) {
			printf("PBK record channel mask 0x%x\n", channel_mask);
			printf("PBK record size %u (0x%x)\n",
			       remain_len, remain_len);
		}
		for (channel = 0; channel < awl->num_channels; channel++) {
			read_len =
			    AWL_PAYLOAD_SIZE_PLAYBACK <
			    remain_len ? AWL_PAYLOAD_SIZE_PLAYBACK : remain_len;
			if (channel_mask & (1 << channel)) {
				/* CAVEAT EMPTOR: there is no direct relation between
				 *                the channel and the current frame,
				 *                but since one should select a frame
				 *                to put the data in, might as well
				 *                use the channel number as an index
				 */
				awl->current_frame = channel;
				n = fread(CURRENT_AWL_PAYLOAD, read_len,
					  1, awl->playback_fp);
				if (n != 1) {
					if (!feof(awl->playback_fp)) {
						fprintf(stderr,
							"PBK %s fread %d, %d, %d, %d, %d, ",
						       	__PRETTY_FUNCTION__,
							read_len, remain_len,
							AWL_PAYLOAD_SIZE_PLAYBACK, AWL_PAYLOAD_SIZE_RECORD, awl->payload_size);
						perror("");
					}
					stop_playback(awl);
					return errno;
				}
				if (awl->calibration_mode) {
					SET_CURRENT_AWL_CHANNEL(channel);
				} else {
					SET_CURRENT_AWL_CHANNEL
						    (awl->mapped_channels[channel]);
				}
				ts = CURRENT_AWL_TIMESTAMP;
				st = CURRENT_AWL_STATUS;
				rcv = GET_CURRENT_AWL_TAIL_CHANNEL_RECEIVE_COUNTER;
				rcn = GET_CURRENT_AWL_TAIL_FILE_RECORD_COUNTER;
				if (CURRENT_AWL_PAYLOAD_32[2] !=
				    0x7fffffff
				    || CURRENT_AWL_PAYLOAD_32[3] !=
				    0x80000000) {
					printf("PBK missed sync\n");
				}
				if (awl->verbose_playback) {
					int fault = 0;
					if (CURRENT_AWL_PAYLOAD_32[2] !=
					    0x7fffffff
					    || CURRENT_AWL_PAYLOAD_32[3]
					    != 0x80000000) {
						fault = 1;
					}
					if (!awl->first_ts[channel]) {
						if (awl->last_ts[channel] + 1 != ts) {
							printf
							    ("PBK channel %d ts jumping from 0x%08x to 0x%08x\n",
							     channel,
							     awl->last_ts[channel], ts);
							fault = 1;
						}
						if (awl->last_rcv[channel] && awl->last_rcv[channel] + 1 != rcv) {
							printf("PBK channel %d rcv jumping from 0x%04x to 0x%04x\n",
							       channel, awl->last_rcv[channel], rcv); 
						}
						if (awl->last_rcn[channel] && awl->last_rcn[channel] + 1 != rcn) {
							printf("PBK channel %d rcn jumping from 0x%04x to 0x%04x\n",
							       channel, awl->last_rcn[channel], rcn); 
						}
					}
					if (fault) {
						printf
						    ("PBK %08x %08x ... 0x%08x 0x%08x 0x%08x 0x%08x\n"
						     "PBK %08x %08x ... 0x%08x 0x%08x 0x%08x 0x%08x\n",
						     awl->last_ts[channel],
						     awl->last_st[channel],
						     last_tail[channel]
						     [0], last_tail[channel]
						     [1], last_tail[channel]
						     [2], last_tail[channel]
						     [3], ts, st,
						     CURRENT_AWL_PAYLOAD_32
						     [AWL_PAYLOAD_SIZE_PLAYBACK_32
						      - 4],
						     CURRENT_AWL_PAYLOAD_32
						     [AWL_PAYLOAD_SIZE_PLAYBACK_32
						      - 3],
						     CURRENT_AWL_PAYLOAD_32
						     [AWL_PAYLOAD_SIZE_PLAYBACK_32
						      - 2],
						     CURRENT_AWL_PAYLOAD_32
						     [AWL_PAYLOAD_SIZE_PLAYBACK_32 - 1]);
					}
				}

				memcpy(last_tail[channel],
				       CURRENT_AWL_PAYLOAD +
				       AWL_PAYLOAD_SIZE_PLAYBACK -
				       (4 * sizeof(uint32_t)),
				        4 * sizeof(uint32_t));
				remain_len -= read_len;
				CURRENT_AWL_CMD = CMD_RAW_0;
				if (!awl->calibration_mode) { 
					main_job(awl);
				}
				if (awl->verbose_playback) {
					int i;
					printf("PBK %d %d < ",
					       GET_CURRENT_AWL_CHANNEL,
					       read_len);
					for (i = 0; i < 32 && i < read_len; i++) {
						printf("%02x ",
						       CURRENT_AWL_PAYLOAD[i]);
					}
					printf("\n");
					if (read_len > 32) {
						printf("PBK *\nPBK ");
						for (i = read_len - 32;
						     i < read_len; i++) {
							printf("%02x ",
							       CURRENT_AWL_PAYLOAD
							       [i]);
						}
						printf("\n");
					}
				}
			}
		}
		if (remain_len) {
			n = fseek(awl->playback_fp, remain_len, SEEK_CUR);
			if (n) {
				fprintf(stderr, "PBK %s fseek, ", __PRETTY_FUNCTION__);
				perror("");
				stop_playback(awl);
				return errno;
			}
		}
		break;
	}

	return 0;
}

/* ================================================================== */

void control_global_recording(int record_mask)
{
	if (static_awl) {
		if (record_mask) {
			static_awl->record_channel_mask = record_mask;
			start_recording(static_awl);
		} else {
			stop_recording(static_awl);
		}
	}
}

/* ================================================================== */

int process_file(awl_data * awl)
{
	if (!awl->playback_channel_mask)
		return 1;
	if (strlen(awl->playback_filename)) {
		if (awl->playback_fp) {
			read_raw_record(awl);
			return 0;
		} else {
			start_playback(awl);
			return 0;
		}
	}
	return 1;
}

/* ================================================================== */

int process_calibration_record(awl_data * awl)
{
	uint8_t channel;
	int i, j;
printf("proc cal\n");

	if (!awl->calibration_data) return -1;
	channel = GET_CURRENT_AWL_CHANNEL;
	j = awl->calibration_count[channel];
	if (awl->verbose_playback) {
		printf("CAL Got calibration data for channel %d, sample %d:", channel, j);
		for (i = 0; i < 64; i ++) {
			printf(" %02x", CURRENT_AWL_PAYLOAD[i]);
		}
		printf ("\n");
	}
	for (i = 0; i < AWL_PAYLOAD_SIZE_S32; i++) {
		CALIBRATION_DATA_S32(channel)[i] = (j * CALIBRATION_DATA_S32(channel)[i] + CURRENT_AWL_PAYLOAD_S32[i])/(j+1);
	}
	if (awl->verbose_playback) {
		printf("PBK Calibration data for channel %d is now:", channel);
		for (i = 0; i < 64; i ++) {
			printf(" %02x", CALIBRATION_DATA_S32(channel)[i]);
		}
		printf ("\n");
	}
	awl->calibration_count[channel] ++;
	return 0;
}

/* ================================================================== */

int read_calibration(awl_data * awl)
{
	int ret, i;
	if (!awl->calibration_data) return -1;
	//awl->record_payload_size = awl->payload_size;
	//printf("read cal path%s\n", awl->calibration_filename);
	switch(awl->calibration_mode) {
	default:
		return awl->calibration_mode;
	case CALIBRATION_MODE_FINISH:
		break;
	case CALIBRATION_MODE_NONE:
		awl->calibration_mode = CALIBRATION_MODE_USE;
		break;
	}
	//printf("read cal mode %d\n", awl->calibration_mode);
	for (i = 0; i < AWL_MAX_CHANNELS; i++) awl->calibration_count[i] = 0;
	start_playback(awl);
	while (!(ret = read_raw_record(awl))) {
		process_calibration_record(awl);
	}
	//printf("read cal ret %d\n", ret);
	stop_playback(awl);
	awl->calibration_mode = 0;
	return 0;
}

/* ================================================================== */

int save_calibration_data(awl_data * awl)
{
	int channel;

	awl->record_channel_mask = 0xffff;
	start_recording(awl);
	for (channel = 0; channel < AWL_MAX_CHANNELS; channel++) {
		memcpy(CURRENT_AWL_PAYLOAD, CALIBRATION_DATA(channel), AWL_PAYLOAD_SIZE_MAX);
		CURRENT_AWL_PAYLOAD_32[0] = 0;
		CURRENT_AWL_PAYLOAD_32[1] = 0;
		SET_CURRENT_AWL_CHANNEL(channel);
		CURRENT_AWL_PAYLOAD_32[2] = 0x7fffffff;
		CURRENT_AWL_PAYLOAD_32[3] = 0x80000000;
		addto_raw_record(awl);
	}
	stop_recording(awl);

	return 0;
}

/* ================================================================== */

int finish_calibration(awl_data * awl)
{
	awl->playback_channel_mask = 0xffff;
	stop_recording(awl);
	awl->calibration_mode = CALIBRATION_MODE_FINISH;	
	read_calibration(awl);
	save_calibration_data(awl);
	return 0;
}

/* ================================================================== */

int update_calibration(awl_data *awl)
{
	int channel;
	float alpha, beta;
	int i;

	channel = GET_CURRENT_AWL_CHANNEL;
	beta = awl->calibration_beta[channel];
	alpha = 1 - beta;
	for (i = 0; i < AWL_PAYLOAD_SIZE_S32; i++) {
		CALIBRATION_DATA_S32(channel)[i] = alpha * CALIBRATION_DATA_S32(channel)[i] + beta * CURRENT_AWL_PAYLOAD_S32[i];
	}
	return 0;
}

/* ================================================================== */

int clear_calibration_data(awl_data *awl, int channel_mask, int flags)
{
	int channel;

	for (channel = 0; channel < AWL_MAX_CHANNELS; channel ++) {
		if (channel_mask & 1 << channel) {
			memset(CALIBRATION_DATA(channel), 0, AWL_PAYLOAD_SIZE_MAX);
		}
	}
	if (flags == 1) unlink(awl->calibration_filename);
	return 0;
}

/* ================================================================== */

int calibrate(awl_data * awl, float beta, int count)
{
	int channel;

	channel = GET_CURRENT_AWL_CHANNEL;
	awl->calibration_beta[channel] = beta;
	awl->calibration_count[channel] = 0;
	awl->calibration_max[channel] = count;
	awl->calibration_mode = CALIBRATION_MODE_LIVE;
	return 0;
}

/* ================================================================== */
