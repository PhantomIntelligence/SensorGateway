#ifndef AWL_CAN_H
#define AWL_CAN_H

int init_can(awl_data *awl);
int process_can(awl_data *awl);
int send_can(awl_data *awl, int32_t id, size_t dlc, uint8_t *buffer);


#endif
