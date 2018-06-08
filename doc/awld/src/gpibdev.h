#ifndef GPIBDEV_H
#define GPIBDEV_H

int init_gpibdev(awl_data *awl);
int send_gpibdev(awl_data *awl, char *cmd);
int recv_gpibdev(awl_data *awl, uint8_t *buffer, size_t *count);

#endif
