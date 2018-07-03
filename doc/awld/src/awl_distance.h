#ifndef AWL_DISTANCE_H
#define AWL_DISTANCE_H
#include "awl_data.h"
#define PI 3.14159
#define NUMBER_MATCHED_FILTER_MODELS 7
#define MATCHED_FILTER_LENGTH 10
#define SATURATION_VALUE 0xFFF0
#define MIN_SATURATED_VALUES 4

int get_distance(int32_t a[], int num_elements, awl_distance **distance, awl_data *awl);

int min_array(int32_t a[], int num_elements, awl_distance **distance);
int max_array(int32_t a[], int num_elements, int algo, int mul, awl_data *awl);
int double_derivate(int32_t a[], int num_elements, awl_data *awl);
int double_wavelet(int32_t a[], int num_elements,  awl_data *awl);
int double_wave(int32_t a[], int num_elements,  awl_data *awl);
int rms_convolution(int32_t a[], int num_elements,  awl_data *awl);
int matched_filter_algo45(int32_t Y[], int nb_point, awl_data *awl);
int analyze_it(awl_data *awl);
int double_equal(float a, float b, float precision );
int test_msg(int32_t a[], int num_elements, awl_data *awl);
float interpol(float y_1, float y, float y1, float y2 );
int32_t saturation_test(int32_t Y[], int nb_point, awl_data *awl);

#endif
