#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include "awl_data.h"

/* ================================================================== */
int denoise(int32_t signal[], int32_t noise[], int size ) {
        int i;
        for( i = 0; i < size; ++i ) {
                 signal[i] =  signal[i] - noise[i];
        }
	return 0;
}
/* ================================================================== */
int pow_array(float a[], int asize, float power)
{
   int i;
   for (i=0; i < asize; i++)
   {
         a[i] = pow(a[i], power);
   }
   return 0;
}
/* ================================================================== */
int square_array(float a[], int asize)
{
   int i;
   for (i=0; i < asize; i++)
   {
         a[i] = a[i] * a[i];
   }
   return 0;
}
/* ================================================================== */
float sum_array(float a[], int asize)
{
   int i;
   float sum=0;
   for (i=0; i < asize; i++)
   {
         sum = sum + a[i];
   }
   return(sum);
}
/* ================================================================== */
float findmax(float array[], int size){
    int i;
    float maximum;
    maximum = array[0];

    for(i = 0; i < size; i++){
        if(maximum < array[i])
            maximum = array[i];
    }
    return maximum;
}
/* ================================================================== */
int32_t findmax_and_pos(float result[], float array[], int size)
{
        // result[0] = max value, result[1] = max value index
        int i;
        int maxpos = 0;
        float maximum;
        maximum = array[0];

        for(i = 0; i < size; i++){
                if(maximum < array[i]){
                   maximum = array[i];
                   maxpos = i;
                }
        }
        result[0] = maximum;
        result[1] = maxpos;
        return 0;
}
/* ================================================================== */
int32_t findmean_int(int32_t array[], int size){
    int i;
    int64_t sum = 0;
    int32_t mean;

    for(i = 0; i < size; i++){
	sum += array[i];
    }
    mean = sum / i;
    return mean;
}
/* ================================================================== */
float findmean(float array[], int size){
    int i;
    float sum = 0;
    float mean;

    for(i = 0; i < size; i++){
	sum += array[i];
    }
    mean = sum / i;
    return mean;
}
/* ================================================================== */
int32_t findmax_int(int32_t array[], int size){
    int i;
    int32_t maximum;
    maximum = array[0];

    for(i = 0; i < size; i++){
        if(maximum < array[i])
            maximum = array[i];
    }
    return maximum;
}
/* ================================================================== */

float findmin(float array[], int size){
    int i;
    float minimum;
    minimum = array[0];

    for(i = 0; i < size; i++){
        if(minimum > array[i])
            minimum = array[i];
    }
    //printf("min %f \n",  minimum );
    return minimum;
}
/* ================================================================== */
int32_t findmin_int(int32_t array[], int size){
    int i;
    int32_t minimum;
    minimum = array[0];

    for(i = 0; i < size; i++){
        if(minimum > array[i])
            minimum = array[i];
    }
    return minimum;
}

/* ================================================================== */
int derivative3int(int32_t y[], int num_elements, int32_t  der[])
{
        int i,h=2;
        for ( i=1; i<(num_elements-1) ; i++) {
                der[i] = ( ( y[i+1] - y[i-1] )/h );
                //printf("der %ld\n", der[i]);
        }
        //assuming the slope of y0 = y1 and ymax = ymax -1
        der[0] = der[1];
        der[num_elements-1] = der[num_elements-2];
	return 0;
}
/* ================================================================== */
int derivative3float(float y[], int num_elements, float  der[])
{
        int i,h=2;
        for ( i=1; i<(num_elements-1) ; i++) {
                der[i] = ( ( y[i+1] - y[i-1] )/h );
                //printf("der %ld\n", der[i]);
        }
        //assuming the slope of y0 = y1 and ymax = ymax -1
        der[0] = der[1];
        der[num_elements-1] = der[num_elements-2];
	return 0;
}
/* ================================================================== */
int second_deri_float(float y[], int num_elements, float  der[]) {
        int i;
	float h=0.01;
        for ( i=1; i<(num_elements-1) ; i++) {
                der[i] = ( ( y[i+1] - 2*y[i] + y[i-1] )/h );
                //printf("der %ld\n", der[i]);
        }
        der[0] = der[1];
        der[num_elements-1] = der[num_elements-2];
	return 0;
}
/* ================================================================== */
int derivative5float(float y[], int num_elements, float  der[])
{
        int i,h=12;
        for ( i=2; i<(num_elements-2) ; i++) {
                der[i] = ( ( ( 8*y[i+1] ) - ( 8*y[i-1] ) + y[i-2] - y[i+2] ) /h );
                //printf("der %ld\n", der[i]);
        }
        //assuming the slope of y0 = y1 = y2 and ymax = ymax -1 = ymax -2
        der[0] = der[2];
        der[1] = der[2];
        der[num_elements-1] = der[num_elements-3];
        der[num_elements-2] = der[num_elements-3];
	return 0;
}
/* ================================================================== */
//int convolution(float x[], int nb_x, float y[], float kernel[], int ksize ) {
int convolution(float * __restrict x, int nb_x, float * __restrict y, float * __restrict kernel, int ksize ) {
	int i,j;

	for ( i = 0; i < nb_x; i++ ) {
    		y[i] = 0;                       // set to zero before sum
    		for ( j = 0; j < ksize; j++ ) {	
			if ( i >= j ) {
        			y[i] += x[i - j] * kernel[j];    // convolve: multiply and accumulate
			}
    		}
	}
	return 0;
}
/* ================================================================== */
int convolutionU(int32_t x[], int nb_x, float y[], float kernel[], int ksize ) {
	int i,j;

	for ( i = 0; i < nb_x; i++ ) {
    		y[i] = 0;                       // set to zero before sum
    		for ( j = 0; j < ksize; j++ ) {	
			if ( i >= j ) {
        			y[i] += x[i - j] * kernel[j];    // convolve: multiply and accumulate
			}
    		}
	}
	return 0;
}
/* ================================================================== */
int FIRFilter( float in[], float out[], float kernel[], int ksize, int n )
{
	unsigned int i, j;
	for( i = 0; i < n - ksize + 1; ++i ) {
		out[ i ] = 0.0;
		for( j = 0; j < ksize; ++j )
		out[ i ] += in[ i + j ] * kernel[ j ];
	}
	return 0;
}

/* ================================================================== */
float average( float array[], int start, int stop ) {
	int i;
	float sum = 0;
	for( i = start; i <= stop; sum += array[ i++ ] );
	return sum / (1+stop-start);
}
/* ================================================================== */
float standard_deviation(float array[], int size, float array_mean) {
        float std_deviation = 0.0;
        int i;

        for(i = 0; i < size; i++)
        {
                std_deviation += pow((array[i] - array_mean), 2);
        }

        return sqrt(std_deviation/(size - 1));
}
/* ================================================================== */
float  lognpdf(float x, float mu, float sigma) {
        float y, c, lny;
        float sq2pi=2.506628274631000241612;

        if ( x<=0 )
        {
                y = 0;
        }
        else
        {
                c = (log(x) - mu) / sigma;
                lny = -0.5 * c * c - log(sigma*x*sq2pi);
                y = exp ( lny );
        }
        return y;

}
/* ================================================================== */
float MyVariance(float _XMean, float* _X, int _buffLength) {

	int i;
	float fNumerateur = 0;

	for (i = 0; i < _buffLength; i++) {
		fNumerateur += (_X[i] - _XMean) * (_X[i] - _XMean);
	}

	return fNumerateur / _buffLength;
}
/* ================================================================== */
float MyCovariance(float _Xmean, float _Ymean, float* _X, float* _Y,
		int _numData) {

	int i;
	float fNumerateur = 0;

	for (i = 0; i < _numData; i++) {
		fNumerateur += (_X[i] - _Xmean) * (_Y[i] - _Ymean);
	}

	return fNumerateur / _numData;
}
/* ================================================================== */
float rms(float array[], int asize) {
        int i;
	//float* work_array;
        float sum = 0.0, frms;

	//work_array = (float*)malloc(asize*sizeof(float));

        for( i = 0; i < asize; ++i ) {
                sum += array[i] * array[i];
        }
        frms = sqrt(( sum / asize ));
	//printf("From the function RMS %f \n", frms);
	//free(work_array);
	//work_array=NULL;
        return frms;
}

/* ================================================================== */
int cmpfunc (const void * a, const void * b)
{
   return ( *(float*)a - *(float*)b );
}
/* ================================================================== */

int derivate_kernel(int order, float kernel[]) {
        int i;
        for (i=0;i< 1+order*2; i++) {
                if (i<order) {
                        kernel[i] = -1;
                } else if (i>order) {
                        kernel[i] = 1;
                } else {
                        kernel[i] = 0;
                }
        }
	return 0;
}
/* ================================================================== */


int compare_distance(const void *p1, const void *p2) {
    const struct histogram *elem1 = p1;
    const struct histogram *elem2 = p2;

   if ( elem1->meters < elem2->meters)
      return -1;
   else if (elem1->meters > elem2->meters)
      return 1;
   else
      return 0;
}


/* ================================================================== */
float compute_SNR_noise(float input[], int dead_zone) {
	float signal_mean, sum_2, noise;
	int i;
	int nb_sample = MINIMUM_SAMPLE_FOR_NOISE; // Minimum sample
       		
	if (dead_zone <= ( ACQUISITION_DELAY - MINIMUM_SAMPLE_FOR_NOISE ) ) { 
		nb_sample  = ACQUISITION_DELAY - dead_zone - 1;
	}
        signal_mean = findmean(input, nb_sample);
        sum_2 = 0;
        for ( i=1; i<nb_sample; i++ ) {
                sum_2 =  sum_2 + ( input[i] - signal_mean ) * ( input[i] - signal_mean );
        }
        noise = sqrt(sum_2/nb_sample);
                
	return noise;
}

/* ================================================================== */

float compute_SNR_from_detection(float input[], int detection, int sourcewidth, float noise) {

        float detect = standard_deviation(input+detection-sourcewidth, (2*sourcewidth) + 1, findmean(input+detection-sourcewidth, (2*sourcewidth)+1)); // Windowed energy around signal center

        return 20*log10(detect/noise);
}

/* ================================================================== */

float confidance(float input[], int nb_point, float margin, float pcent ) {
	int i, count = 1;
	float distance=0.0;
	int max=0; 
	
	float prev = input[0];
	for (i = 1; i < nb_point; i++) {
		if (input[i] == prev) {
      			count++;
			if ( i == nb_point -1 ) {
                        	if ( max < count ) {
                                	max = count;
                                	distance = input[i];
                        	}
			}
		} else {
			if ( max < count ) {
				max = count;
				distance = input[i-1];
			}
      			prev = input[i];
      			count = 1;
    		}
	}
      	//printf("max %d  dist %f \n", max, distance);
	
	if ( ((float)  max/nb_point) <=  pcent ) {
		distance = 0.0;
	} 
	return distance;
}

/* ================================================================== */
