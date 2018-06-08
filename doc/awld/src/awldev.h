#ifndef AWL_AWLDEV_H
#define AWL_AWLDEV_H

int init_awl(awl_data *awl);
int process_awl(awl_data *awl);
int get_dead_zone(awl_data *awl);
int get_frame_rate(awl_data *awl);
int set_frame_rate(awl_data *awl, int frame_rate);
int send_sensor_monitor_status(awl_data *awl);
int send_sensor_boot(awl_data *awl);
int read_bias(uint32_t *value);
int write_bias(uint32_t value);
int read_awl_register(uint16_t address, uint32_t *value);
int write_awl_register(uint16_t address, uint32_t value);
int dump_awl_registers(uint32_t *buffer, size_t num_reg);
int read_adc_register(uint16_t address, uint32_t *value);
int write_adc_register(uint16_t address, uint32_t value);
int read_gpio(uint16_t address, uint32_t *value);
int write_gpio(uint16_t address, uint32_t value);

#endif
