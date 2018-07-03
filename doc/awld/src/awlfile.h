#ifndef AWL_FILE_H
#define AWL_FILE_H

int process_file(awl_data *awl);

int start_recording(awl_data *awl);
int start_raw_record(awl_data *awl);
int addto_raw_record(awl_data *awl);
int stop_recording(awl_data *awl);

int start_playback(awl_data *awl);
int stop_playback(awl_data *awl);
int read_raw_record(awl_data *awl);

void control_global_recording(int record_mask);

int read_calibration(awl_data *awl);
int finish_calibration(awl_data *awl);
int calibrate(awl_data * awl, float beta);
int update_calibration(awl_data * awl);
int save_calibration_data(awl_data *awl);
int clear_calibration_data(awl_data *awl, int channet_mask, int flags);

#endif
