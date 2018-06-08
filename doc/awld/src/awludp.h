#ifndef AWL_UDP_H
#define AWL_UDP_H

int init_udp(awl_data *awl);
int process_udp(awl_data *awl);
int send_raw_frame(awl_data *awl);
int send_udp(awl_data *awl, int32_t id, size_t size, uint8_t *buf);
int set_udp_payload_size(awl_data *awl);

#endif
