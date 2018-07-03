#ifndef AWL_FDIST_H
#define AWL_FDIST_H

int pow_array(float a[], int asize, float power);
int square_array(float a[], int asize);
float sum_array(float a[], int asize);
float findmax(float a[], int size);
int32_t findmax_and_pos(float result[], float array[], int size);
float findmin(float a[], int size);
float findmean(float a[], int size);
int32_t findmax_int(int32_t a[], int size);
int32_t findmin_int(int32_t a[], int size);
int32_t findmean_int(int32_t a[], int size);
int derivative3int(int32_t y[], int num_elements, int32_t  der[]);
int derivative3double(float y[], int num_elements, float  der[]);
int second_deri_double(float y[], int num_elements, float  der[]);
int derivative5double(float y[], int num_elements, float  der[]);
float MyVariance(float _XMean, float* _X, int _buffLength);
float MyCovariance(float _Xmean, float _Ymean, float* _X, float* _Y, int _numData);
int convolution(float x[], int nb_x, float y[], float kernel[], int ksize );
int convolutionU(int32_t x[], int nb_x, float y[], float kernel[], int ksize );
int FIRFilter( float in[], float out[], float kernel[], int ksize, int n );
float average( float a[], int start, int stop );
float standard_deviation(float array[], int size, float array_mean);
float  lognpdf(float x, float mu, float sigma);
float rms(float array[], int asize);
int cmpfunc (const void * a, const void * b);
int compare_distance (const void * a, const void * b);
float compute_SNR_from_detection(float input[], int detection, int sourcewidth, float noise);
float confidance(float input[], int nb_point, float margin, float pcent );
int derivate_kernel(int order, float a[]);
int derivate_kernel(int order, float a[]);
int denoise(int32_t signal[], int32_t noise[], int size );
float compute_SNR_noise(float input[], int dead_zone); 

#endif
