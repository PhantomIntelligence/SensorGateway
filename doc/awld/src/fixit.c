#include <stdio.h>
#include <stdint.h>

int main (int argc, char**argv)
{

	unsigned char buf[16*4096];
	int i, n;
	int implied_channel_mask = 0xff;
	int real_channel_mask = 0;
	FILE *fr, *fw;

	if (argc < 0) {
		printf("%s IN OUT\n", argv[0]);
		return 1;
	}
		
	fr = fopen(argv[1], "r+");
	if (!fr) {
		perror("open read");
		return 1;
	}
	fw = fopen(argv[2], "w+");
	if (!fw) {
		perror("open write");
		return 1;
	}

	n = fread(buf, 0x30, 1, fr);
	if (n != 1) {
		perror("read file header");
		return 1;
	}
	n = fwrite(buf, 0x30, 1, fw);
	if (n != 1) {
		perror("write file header");
		return 1;
	}

	i = 0;
	real_channel_mask = 0;

	while (n == 1) {
		n = fread(buf + i, 4, 1, fr);
		if (n == 1) {
			//printf ("%x %x %x %x\n", buf[i], buf[i+1], buf[1+2], buf[i+3]);
			if (i && (buf[i] == 0x90 && buf[i+1] == 0x00 && buf[i+2] == implied_channel_mask)) {
printf("%d\n", i);
				if (i-8 > 4096*0) real_channel_mask = 0x1;
				if (i-8 > 4096*1) real_channel_mask = 0x3;
				if (i-8 > 4096*2) real_channel_mask = 0x7;
				if (i-8 > 4096*3) real_channel_mask = 0xf;
				if (i-8 > 4096*4) real_channel_mask = 0x1f;
				if (i-8 > 4096*5) real_channel_mask = 0x3f;
				if (i-8 > 4096*6) real_channel_mask = 0x7f;
				if (i-8 > 4096*7) real_channel_mask = 0xff;
				buf[2] = real_channel_mask;
				*((uint32_t*)(buf+4)) = i-8;
				real_channel_mask = 0;
				n = fwrite(buf, i, 1, fw);
				buf[0] = buf[i];
				buf[1] = buf[i+1];
				buf[2] = buf[i+2];
				buf[3] = buf[i+3];
				i = 0;
			}
			i += 4;
		}
	}
	fclose(fw);
	fclose(fr);
	return 0;
}
