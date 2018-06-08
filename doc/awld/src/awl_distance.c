#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#include "awl_distance.h"
#include "awl_data.h"
#include "awl_track.h"
#include "awl_fdist.h"
#include "awlcan.h"
#include "awlmsg.h"
#include "awlcmd.h"
#include "awldev.h"
#include "awlfile.h"
#include "awl_music.h"

static float matched_filter_models[NUMBER_MATCHED_FILTER_MODELS][MATCHED_FILTER_LENGTH] = 
{
	{0.018308, 0.1690, 0.6411, 1, 0, 0, 0, 0, 0, 0}, // 3
	{0.0135, 0.0889, 0.3411, 0.7642, 1, 0, 0, 0, 0, 0}, // 4
	{0.0111, 0.0561, 0.1979, 0.4868, 0.8353, 1, 0, 0, 0, 0}, // 5
	{0.0096, 0.0400, 0.1272, 0.3135, 0.5972, 0.8791, 1, 0, 0, 0}, // 6
	{0.0087, 0.0307, 0.0889, 0.2125, 0.4184, 0.6790, 0.9077, 1, 0, 0}, // 7
	{0.0080, 0.0249, 0.0663, 0.1520, 0.2994, 0.5075, 0.7397, 0.9274, 1, 0}, // 8
	{0.0075, 0.0210, 0.0520, 0.1140, 0.2213, 0.3809, 0.5810, 0.7856, 0.9415, 1} // 9
};


/* ================================================================== */

int printa(float pary[], int size) 
{
    int i;
    for (i = 0; i < size; i++) {
            printf("\t(%d) %f", i,pary[i]);
    }
        printf("\n");
        return 0;
}

/* ================================================================== */
int vplot(FILE * gnuplotPipe, float  varray[], int arraysize) {
        int i;
	char window[20];
	sprintf(window, "set title \"W:%d\"\n",arraysize);
        fprintf(gnuplotPipe, window );
        fprintf(gnuplotPipe, "set xrange [0:%d.4]\n",arraysize+5);
        fprintf(gnuplotPipe, "f(x)=0\n");
        fprintf(gnuplotPipe, "plot f(x)\n");
        fprintf(gnuplotPipe, " replot '-' with line \n");
        for (i = 0; i < arraysize; i++) {
                fprintf(gnuplotPipe, "%d %f\n", i, varray[i]);
        }
        fprintf(gnuplotPipe, "e");

        return 0;
}

/* ================================================================== */
int vplotInt(FILE * gnuplotPipe, int32_t  varray[], int arraysize) {
        int i;

        fprintf(gnuplotPipe, "set xrange [0:%d.4]\n",arraysize+5);
        fprintf(gnuplotPipe, "f(x)=0\n");
        fprintf(gnuplotPipe, "plot f(x)\n");
        fprintf(gnuplotPipe, " replot '-' with line \n");
        for (i = 0; i < arraysize; i++) {
                fprintf(gnuplotPipe, "%d %d\n", i, varray[i]);
        }
        fprintf(gnuplotPipe, "e");

        return 0;
}



/* ================================================================== */
float interpol(float y_1, float y, float y1, float y2 ) {
	float offsets[5], result=0.0;

	offsets[0] = y / ( y_1 - y );
	offsets[1] = y / ( y - y1 ); 
	offsets[2] = 1 - ( y1/(y2-y1) );
	offsets[3] = 2*y / ( y_1 - y1 );
	offsets[4] = 1 - ( 2*y1 / ( y2-y ));

	if ( offsets[1] < 0.5 ) {	// use 1 2 4
                        result = ( offsets[0] + offsets[1] + offsets[3] ) / 3.0;
        } else {			// use 2 3 5
                        result = ( offsets[1] + offsets[2] + offsets[4] ) / 3.0;
	}
	
	//printf("offsets %f %f %f %f %f  124-235 %f\n",  offsets[0],  offsets[1],  offsets[2],  offsets[3],  offsets[4], result );

return result;
}


/* ================================================================== */

int double_equal(float a, float b, float precision) {
	return fabs(a-b) <  precision;
}

int32_t saturation_test(int32_t Y[], int nb_point, awl_data *awl) {
	return findmax_int(Y, nb_point);

}


/* ================================================================= */
// DETECTION ALGORITHMS
/* ================================================================= */
// ALGO1

int max_array(int32_t a[], int nb_point, int algo, int acq_mode, awl_data *awl)
{
//printf("max_array, ");
//printf("offset %f threshold %f\n",offset, threshold );

	int i, first, second, channel;
	float div = 0.599;
	int32_t first_max = -214000000, second_max = -214000000;
	int noise;
	int begin = 0;
	int end;
	int second_found = 0;
	int32_t threshold_U = 0;
	float m, d, v, s;
        int intensity[MAX_TRACK];
	float offset=0.0;

	if (awl->dead_zone <= ACQUISITION_DELAY ) { 
		begin = ACQUISITION_DELAY - awl->dead_zone;
	}
	end = nb_point;


	channel = GET_CURRENT_AWL_CHANNEL;
	div = div * acq_mode;

	offset =  SYSTEM_OFFSET + ALGO_PARAM(0);

        for (i=0;i<nb_point;i++) {
		if ( ( a[i] / 8000 ) > 512 ) {
			intensity[i] = 1024;
		} else {
			 intensity[i] = ( a[i] / 8000 );
		}
        }
	noise =  findmin_int( intensity, 1000 ); 
        for (i=0;i<nb_point;i++) {
			intensity[i] = intensity[i]  - noise;
	}

	/*
        FILE * gnuplotPipe = popen ("gnuplot -persistent", "w");
        i = vplot(gnuplotPipe,  a , 150);
	pclose(gnuplotPipe );
	printf("Press enter to continue...\n");
	getchar();
	*/
	
//printf("threshold %lu\n", threshold_U );

	// first max
	first = 0;
	m = 0;
	for (i = begin; i < end; i++) {
		m += a[i];
		if (a[i] > first_max) {
			first_max = a[i];
			first = i;
		}
	}
	//if ( channel == 6 ) {	
	//	printf("CH6  %d %d  %f  %d \n ", channel, i, x[first], first_max);
	//}
	//printf("Press enter to continue...\n");
	//getchar();
       
//printf("m, ");
	m /= (begin - end + 1);
	v = 0;
	for (i = begin; i < end; i++) {
		d = (a[i] - m);
		v += d * d;
	}
//printf("v, ");
	v /= (end - begin);
	s = sqrt(v);
	threshold_U = 0.4 * first_max;
//printf("first_max, ");
	// second max
	second = 0;
	i = first;
	while ((a[i] - m) > s && i < end) {
		i++;
		//printf ("m %f, s %f, v %f\n", m, s, a[i] - m);
	}
	//printf ("%d %d ", first, i);

	for (; i < end; i++) {
		if (a[i] > second_max) {
			second_max = a[i];
			second = i;
		}
	}

	for ( i=0; i< awl->maxNumberOfDetections; i++ ) {
		awl->detected[channel][i].meters = 0;
	}
	
	if ( second_max > threshold_U) {
		second_found = 1;
		awl->detected[channel][1].meters = offset + awl->x_distance[second];
		awl->detected[channel][1].intensity = intensity[second];
	}
	awl->detected[channel][0].meters =  offset + awl->x_distance[first];
	awl->detected[channel][0].intensity = intensity[first];

	return 1 + second_found;
}

/*
int min_array(int32_t a[], int num_elements, awl_distance **distance)
{
	int offset = -140;
	float div = 17.0;
	int i, index = 0;
	float meters;
	int32_t min = INT32_MAX;
	for (i = 0; i < num_elements; i++) {
		if (a[i] < min) {
			min = a[i];
			index = i;
		}
	}
	meters = ((index) * div) + offset;
	if ((*distance)) {
		(*distance)->meters = meters;
		(*distance)->value = min;
		(*distance)->index = index;
	}
	return 1;
}
*/

/* ================================================================== */
// ALGO2

int double_wave(int32_t Y[], int nb_point, awl_data *awl)
{

        int i, nb_detected, channel, found=0;
        int firstDerivateOrder = 2;  // noise immunity
        int secondDerivateOrder = 6; // curve matching
        int list[MAX_TRACK];
	float thres;
        float first[50] = { 0 };
        float second[100] = { 0 };
        float yscale[MAX_TRACK], yprime[MAX_TRACK], ysecond[MAX_TRACK];
	int  intensity[MAX_TRACK];
	int start = 0;
	int end;
	int  noise = 0;
	float offset=0.0;
	float threshold=0.0;
	float dx, y_offset;
	//int intensity_filter = 0;
        //int32_t max;
        //float offset = -3.0521;
	float detected[DEFAULT_AWL_MAX_DETECTED] = {0};
	//float scale;
	//float filter[5]= {8.0,4.0,2.0,1.0,1.0};
	float detRMS, sigRMS, snrmin, snr;

	
	channel = GET_CURRENT_AWL_CHANNEL;
	if (awl->dead_zone <= ACQUISITION_DELAY ) { 
		start = ACQUISITION_DELAY - awl->dead_zone;
	}
	end = nb_point;

	/*
        FILE * gnuplotPipe = popen ("gnuplot -persistent", "w");
        FILE * gnuplotPipe2 = popen ("gnuplot -persistent", "w");
        FILE * gnuplotPipe3 = popen ("gnuplot -persistent", "w");
        */

	snrmin = ALGO_PARAM(7); 

       //threshold = 0.35; 
       //firstDerivateOrder = 2;  
       //secondDerivateOrder = 7; 

	
        //intensity_filter = ALGO_PARAM(4); 
       
        i = derivate_kernel(firstDerivateOrder, first);
        i = derivate_kernel(secondDerivateOrder, second);


	offset =  SYSTEM_OFFSET + ALGO_PARAM(0);

	/*
        printf(" O:%f Th:%f F:%d S:%d snr %f \n", ALGO_PARAM(0), threshold, firstDerivateOrder, secondDerivateOrder, snrmin );
	printf("Press enter to continue...\n");
	getchar();
	*/

        // X = 0:sampling/lightspeed:(length-1)*sampling/lightspeed;
		
	//max = findmax_int(Y, nb_point);
	//scale = max/32;
	//
	//
	//
 
        for (i=0;i<nb_point;i++) {
		yscale[i] =  ( float ) ( Y[i] );
		if ( ( Y[i] / 8000 ) > 512 ) {
			intensity[i] = 1024;
		} else {
			 intensity[i] = ( Y[i] / 8000 );
		}
        }
	noise =  findmin_int( intensity, 1000 ); 
        for (i=0;i<nb_point;i++) {
			intensity[i] = intensity[i]  - noise;
	}

	sigRMS = rms(yscale+( nb_point-60), 50);	
	
	i = derivate_kernel(firstDerivateOrder, first);
        i = derivate_kernel(secondDerivateOrder, second);

	

        i = convolution(yscale, nb_point, yprime, first, (2*firstDerivateOrder)+1 );
        for ( i=0; i< (nb_point-firstDerivateOrder); i++ ) {
                yprime[i] = yprime[i+firstDerivateOrder];
        }

	for (i=0;i<nb_point-firstDerivateOrder; i++ ) {
		yprime[i] = yprime[i+firstDerivateOrder];
	}

        i = convolution(yprime, nb_point, ysecond, second, (2*secondDerivateOrder)+1  );
        for ( i=0; i< (nb_point-secondDerivateOrder); i++ ) {
                ysecond[i] = ysecond[i+secondDerivateOrder];
        } 
         
	/* 
        i = vplot(gnuplotPipe, yscale , 351);
	pclose(gnuplotPipe );

        i = vplot(gnuplotPipe2, yprime,  352);
        i = vplot(gnuplotPipe3, ysecond,  353);

	pclose(gnuplotPipe2 );
	pclose(gnuplotPipe3 );

	printf("Press enter to continue...\n");
	getchar();
	*/

	thres =  threshold * findmin(ysecond, nb_point);
	//printf("thres %f %f \n", findmin(ysecond, nb_point), thres   );

	// Object may be there
	for  ( i=start; i< end; i++ )  {
                if ( ysecond[i] < thres ) {
                        list[found] = i;
                        found++;
                }
        }
	//for ( i=0; i<found; i++ ) {
	//        printf("ind  %d   %f ", list[i], ysecond[i]);
	//}
	//printf("\n");
	

        nb_detected =0;
	for ( i=0; i< awl->maxNumberOfDetections; i++ ) {
		awl->detected[channel][i].meters = 0;
	}
         for ( i=0; i< found; i++ ) {
                if ( list[i] == ( list[i+1] -1 ) ) {
		    detRMS = rms(yscale+list[i+1], 1 + ( 2 * secondDerivateOrder)  );
		    snr = 20*log10(detRMS/sigRMS);		
		    if  ( snr > snrmin  ) {
		    	//printf(" rms Det %f  rms Signal %f  snr %f snrmin %f   \n ", detRMS, sigRMS, snr, snrmin );
                        if ( ( yprime[list[i]]<0 && yprime[list[i+1]]>=0 ) && ( nb_detected < awl->maxNumberOfDetections ) ) { 
				dx =  awl->x_distance[list[i+1]]-awl->x_distance[list[i]];
				//y_offset = yprime[list[i]] / (yprime[list[i]]-yprime[list[i+1]]);
				y_offset = interpol( yprime[list[i]-1], yprime[list[i]], yprime[list[i]+1], yprime[list[i]+2] );
                                detected[nb_detected] = offset +  awl->x_distance[list[i]]+ ( ( dx ) * ( y_offset )) ;
				awl->detected[channel][nb_detected].meters = detected[nb_detected];
				awl->detected[channel][nb_detected].intensity =    ( snr + 21 )*2;
				//printf("Intensity %d \n", awl->detected[channel][nb_detected].intensity);

                                nb_detected++;
                        }
		    }
               }
	}

	return nb_detected;
}

int double_wavelet(int32_t Y[], int nb_point, awl_data *awl) {

        //int anb_detected[10] = { 0 };
        int nb_detected =  0;
	float first_damping = 0.15; // noise immunity
	//int i,j,k,l, max_detected, second_width = 6, found; // curve matching
	int i,j,k,l,z, second_width = 6, first_width = 11, found; // curve matching
	float thres;
	//float margin = 0.05;
        int list[MAX_TRACK];
        int intensity[MAX_TRACK];
	float candidate[32][3] = {{0}};
	int numwidth=2;
	int channel, start = 0, val_repeat = 0;
	int end;      	
	//int confidence = 2*numwidth+1;
        float  sum = 0, first[11];
        float  second[50];
	int width;  // , second_lenght;
        float yscale[MAX_TRACK], yprime[MAX_TRACK], ysecond[MAX_TRACK];
	//float  tresh;
	float new_candidate =0;
	float dx, y_offset;
	int  noise = 0;      	
	float offset=0.0;
	float threshold=0.0;
	float sigRMS, snrmin, snr;
	//char c;
	
	//static float ch_threshold[8];
	//char c;

	if (awl->dead_zone <= ACQUISITION_DELAY ) { 
		start = ACQUISITION_DELAY - awl->dead_zone;
	}
	end = nb_point;
	
/*
        FILE * gnuplotPipe = popen ("gnuplot -persistent", "w");
        FILE * gnuplotPipe2 = popen ("gnuplot -persistent", "w");
        FILE * gnuplotPipe3 = popen ("gnuplot -persistent", "w");
*/
        //FILE * gnuplotPipe3 = popen ("gnuplot -persistent", "w");

        //int32_t mean;
        //float offset = -3.0521;
	//float detected[32] = {0};
	//float scale;

	//memset(candidate,0,sizeof(float*10*8));	
	channel = GET_CURRENT_AWL_CHANNEL;


	//offset = offset -36.6;
	offset =  SYSTEM_OFFSET  + ALGO_PARAM(0);

	//mean = findmean_int(Y, nb_point);
	//scale = mean;
	
	threshold = ALGO_PARAM(1); 
	first_damping  = ALGO_PARAM(2);
	numwidth = ALGO_PARAM(3);
	second_width = ALGO_PARAM(5); 
	snrmin = ALGO_PARAM(7); 

	/*
	printf("offset  %f\n", offset);
	printf("channe  %d\n", channel);
	printf("threshold  %f\n", threshold);
	printf("first_damping  %f\n", first_damping);
	printf("numwidth  %d\n", numwidth);
	printf("intensity_filter  %d\n", intensity_filter);
	printf("second_width  %d\n", second_width);
	 printf("Press enter to continue...\n");
	 getchar();
	*/

        for (i=0;i<nb_point;i++) {
		yscale[i] =  ( float ) ( Y[i] );
		if ( ( Y[i] / 6000 ) > 512 ) {
			intensity[i] = 1024;
		} else {
			 intensity[i] = ( Y[i] / 6000 );
		}
        }
	noise =  findmin_int( intensity, 1000 ); 
        for (i=0;i<nb_point;i++) {
			intensity[i] = intensity[i]  - noise;
	}
	
	sigRMS = rms(yscale+( nb_point-60), 50);	

	// wavelet 1
	// float FIRST[11]  =  {0, 0, 0, 0.00003,  2.65962,  0,  -2.65962,   -0.00003,  0, 0, 0 };
	// FIRST = cat(2,lognpdf(5:-1:1,0,FirstDamping),0,-lognpdf(1:5,0,FirstDamping));
        for ( i=0; i<5; i++ ) {
		first[i] = lognpdf( ( 5.0 - i ), 0, first_damping );
		first[(5*2)-i] = -1.0 * first[i];
	//	first[i+5] = -1.0 * lognpdf(i,0, first_damping );
	}
	first[5] = 0;

	//  sum(lognpdf([1:5],0,FirstDamping))
	for ( i=0; i<5; i++ ) {
		sum = sum + first[i];
	}
	for ( i=0; i<11; i++ ) {
		first[i] = first[i]/sum;
	}
	/*
	for ( i=0; i<=11; i++ ) {
		printf("first %lf\n", first[i]);
	}
	i = getchar();
	*/
	
    	i = convolution(yscale, nb_point, yprime, first, first_width ); 
	// resize
	for (j=0;j<nb_point-((first_width -1)/2); j++ ) {
		ysecond[j] = ysecond[j+((first_width -1)/2)];
	}
	
	// wave 2
	// Detected = []
	// numfound = []
	//  -numwidth to numwidth
	l=0;	
	nb_detected = 0;
	for (i=(-numwidth); i <= numwidth; i++) {
		width = second_width+i;
		//printf("wave no  %d\n  ", width  );
		for ( j=-(width); j<= width; j++) {
			second[j+width] = sin(PI*j/width);
			//printf(" %f \n ", second[j+width]);
		}
		//printf("----\n");
		j= convolution(yprime, nb_point, ysecond, second, ((2*width)+1) );
		
		for (j=0;j<nb_point-width; j++ ) {
			ysecond[j] = ysecond[j+width];
		}
                //j= pow_array(ysecond, nb_point,2);
                //j= square_array(ysecond, nb_point);

		// test d optimisation 
                // j= pow_array(ysecond, nb_point,2);
		// thres =  threshold  * rms(ysecond+10, nb_point-10);
		thres =   threshold / 100  * findmax(ysecond, nb_point);
		
		/*	
        	if ( i >= 0 ) {	
			int graph;
			graph = vplot(gnuplotPipe3, ysecond,  352);
			pclose(gnuplotPipe3 );
		}
	
		getchar();
		*/

		// printf("thres %f \n", thres);
 		

		// Object may be there
		found = 0; 
	        for ( j=start; j< end; j++ ) {
                	//if ( ysecond[j] > thres ) {
                	if ( ysecond[j] <  thres ) {
                        	list[found] = j;
                        	found++;
		      		//printf("ind  %d  %d  %f   %f \n", j, found,  thres,  ysecond[j]);
			}
                }	
		snr = 0;
	 	for ( j=0; j< found; j++ ) {
			if ( list[j] == ( list[j+1] -1 ) ) { // two consecutive value
			     //printf("%f   %f \n ", yprime[list[i]],  yprime[list[i+1]] );	
			    snr = compute_SNR_from_detection(yscale, list[j+1], width, sigRMS);
			//     detRMS = rms(yscale+list[j+1], 1 + (  width)  );	
			//     snr = 20*log10(detRMS/sigRMS);
						
			    if  (  snr  > snrmin  ) {
			    	//printf(" rms Det %f  rms Signal %f  snr %f  n %d  \n ", detRMS, sigRMS, snr, i );

				if (  yprime[list[j]]<0 && yprime[list[j+1]]>=0 )   {
					dx =  awl->x_distance[list[j+1]]-awl->x_distance[list[j]];
					y_offset = interpol( yprime[list[j]-1], yprime[list[j]], yprime[list[j]+1], yprime[list[j]+2] );
					new_candidate = offset + awl->x_distance[list[j]] + ( ( dx ) * ( y_offset  ) );
					//printf("detec  %d  n %d  new val  %f \n", i, nb_detected, new_candidate );
					// it's the first one
					if ( nb_detected ) { 
					 	val_repeat = 0;	
						for ( z=0; z<nb_detected; z++ ) {
							if ( double_equal( new_candidate, candidate[z][0], 0.01 )) { 
								candidate[z][1]++;
								val_repeat=1;
								//printf("repeat\n");
							}
							//printf("bilan  %f  %f \n", candidate[z][0], candidate[z][1] );
						} //  end for z	
						//printf("\n");
						if (!val_repeat) {
							candidate[nb_detected][0] = new_candidate;
							candidate[nb_detected][1]++;
							candidate[nb_detected][2] =   (snr + 21)*2;
							//printf("new  %f  %f \n", candidate[nb_detected][0], candidate[nb_detected][1] );
							nb_detected++;
						}
					} else {   // if nb_detected
						candidate[nb_detected][0] = new_candidate;
						candidate[nb_detected][1]++;
						candidate[nb_detected][2] =   (snr + 21)*2;
						//printf("first  %f  %f \n", candidate[nb_detected][0], candidate[nb_detected][1] );
						nb_detected++;
					}   // if nb_detected
				} // if yprime pass zero
			    }  // SNR valid
			}
		}
		l++;
	}
	
	
        
	for ( i=0; i< awl->maxNumberOfDetections; i++ ) {
		awl->detected[channel][i].meters = 0;
		awl->detected[channel][i].intensity = 0;
	}

	k=0;
	for ( i=0; i< nb_detected; i++ ) {	
		if (( candidate[i][1] == 5 ) && ( k <   8)){ 
			awl->detected[channel][k].meters = candidate[i][0];
			awl->detected[channel][k].intensity = candidate[i][2];
			//printf("Valeur  %f Int %f \n", candidate[i][0], candidate[i][2] );
			k++;
		}   
	}

		//		if ( candidate[][1] = 5 )
                //for ( i=0; i<5; i++ ) {
                 //       temp[i] =   candidate[i][k];
			//printf(" d%d  %f ",i,  temp[i] );
		//}
	 	//printf(" \n ");
		//temp[0] = 1.1;  temp[1] = 2.2; temp[2] = 1.01; temp[3] = 0.5; temp[4] = 2;
   		//qsort(temp, 5 , sizeof(float), cmpfunc);
                //for ( i=0; i<5; i++ ) {
                //        printf(" t%d  %f ",i,  temp[i] );
                //}
	 	//printf(" SSS  \n ");
		//awl->detected[channel][k].meters =  confidance(temp, 5, 0.05, .59 );
   		//detected[k] = (float) candidate[0][k];
	 	//printf(" \n ");
	 	
	//printf(" detected %d ", nb_detected );
	//printf("Press enter to continue...\n");
	//getchar();

/*
	 printf("Press y to display...\n");
	 c = getchar();
	if ( c == 'y' ) {

        	i = vplot(gnuplotPipe,  yscale+20 , 250);
	 	pclose(gnuplotPipe );
		
        	i = vplot(gnuplotPipe2, yprime+20,  251);
		pclose(gnuplotPipe2 );
        	i = vplot(gnuplotPipe3, ysecond+20,  252);
		pclose(gnuplotPipe3 );

	 	printf("Press enter to continue...\n");
	 	getchar();
	}
	
*/

	return(0);

}

/* ================================================================== */
// ALGO3

int double_derivate(int32_t Y[], int nb_point, awl_data *awl)
{

        int i, nb_detected, channel, found=0;
        int firstDerivateOrder = 2;  // noise immunity
        int secondDerivateOrder = 6; // curve matching
        int list[MAX_TRACK];
	float thres;
        float first[50] = { 0 };
        float second[100] = { 0 };
        float yscale[MAX_TRACK], yprime[MAX_TRACK], ysecond[MAX_TRACK];
	int start = 0;
	int end;
	float offset=0.0;
	float threshold=0.0;
	float dx, y_offset;
	//int intensity_filter = 0;
        //int32_t max;
        //float offset = -3.0521;
	float detected[DEFAULT_AWL_MAX_DETECTED] = {0};
	//float scale;
	//float filter[5]= {8.0,4.0,2.0,1.0,1.0};
	float snrmin, snr;
	float signal_std = 0;

	
	channel = GET_CURRENT_AWL_CHANNEL;
	if (awl->dead_zone <= ACQUISITION_DELAY ) { 
		start = ACQUISITION_DELAY - awl->dead_zone;
	}
	end = nb_point;

	/*
        FILE * gnuplotPipe = popen ("gnuplot -persistent", "w");
        FILE * gnuplotPipe2 = popen ("gnuplot -persistent", "w");
        FILE * gnuplotPipe3 = popen ("gnuplot -persistent", "w");
        */
	threshold  = ALGO_PARAM(1); 
	firstDerivateOrder = ALGO_PARAM(2);  
        secondDerivateOrder = ALGO_PARAM(3);
	snrmin = ALGO_PARAM(7); 

       //threshold = 0.35; 
       //firstDerivateOrder = 2;  
       //secondDerivateOrder = 7; 

	
        //intensity_filter = ALGO_PARAM(4); 
       
        i = derivate_kernel(firstDerivateOrder, first);
        i = derivate_kernel(secondDerivateOrder, second);


	offset = SYSTEM_OFFSET  + ALGO_PARAM(0);

	/*
        printf(" O:%f Th:%f F:%d S:%d snr %f \n", ALGO_PARAM(0), threshold, firstDerivateOrder, secondDerivateOrder, snrmin );
	printf("Press enter to continue...\n");
	getchar();
	*/

        // X = 0:sampling/lightspeed:(length-1)*sampling/lightspeed;
		
	//max = findmax_int(Y, nb_point);
	//scale = max/32;
	//
	//
	//
 
        for (i=0;i<nb_point;i++) {
		yscale[i] =  ( float ) ( Y[i] );
        }

	signal_std = compute_SNR_noise(yscale, awl->dead_zone);
	
	i = derivate_kernel(firstDerivateOrder, first);
        i = derivate_kernel(secondDerivateOrder, second);

	

        i = convolution(yscale, nb_point, yprime, first, (2*firstDerivateOrder)+1 );
        for ( i=0; i< (nb_point-firstDerivateOrder); i++ ) {
                yprime[i] = yprime[i+firstDerivateOrder];
        }

	for (i=0;i<nb_point-firstDerivateOrder; i++ ) {
		yprime[i] = yprime[i+firstDerivateOrder];
	}

        i = convolution(yprime, nb_point, ysecond, second, (2*secondDerivateOrder)+1  );
        for ( i=0; i< (nb_point-secondDerivateOrder); i++ ) {
                ysecond[i] = ysecond[i+secondDerivateOrder];
        } 
         
	/* 
        i = vplot(gnuplotPipe, yscale , 351);
	pclose(gnuplotPipe );

        i = vplot(gnuplotPipe2, yprime,  352);
        i = vplot(gnuplotPipe3, ysecond,  353);

	pclose(gnuplotPipe2 );
	pclose(gnuplotPipe3 );

	printf("Press enter to continue...\n");
	getchar();
	*/

	thres =  threshold * findmin(ysecond, nb_point);

	// if(awl->verbose_distance) {
	// 	printf("Signal threshold: %f\n", thres);
	// 	printf("%f %f %f %f %f\n", yscale[100], yscale[200], yscale[300], yscale[400], yscale[500]);
	// 	printf("%f %f %f %f %f\n", ysecond[100], ysecond[200], ysecond[300], ysecond[400], ysecond[500]);
	// }
	//printf("thres %f %f \n", findmin(ysecond, nb_point), thres   );

	// Object may be there
	for  ( i=start; i< end; i++ )  {
                if ( ysecond[i] < thres ) {
                        list[found] = i;
                        found++;
                }
        }
	//for ( i=0; i<found; i++ ) {
	//        printf("ind  %d   %f ", list[i], ysecond[i]);
	//}
	//printf("\n");
	

        nb_detected =0;
	for ( i=0; i< awl->maxNumberOfDetections; i++ ) {
		awl->detected[channel][i].meters = 0;
		awl->detected[channel][i].intensity = 0;
	}
         for ( i=0; i< found; i++ ) {
                if ( list[i] == ( list[i+1] -1 ) ) {
		    snr = compute_SNR_from_detection(yscale, list[i+1], secondDerivateOrder, signal_std);	
		    if  ( snr > snrmin  ) {
		 	//printf(" rms Det %f  rms Signal %f  snr %f snrmin %f   \n ", detRMS, sigRMS, snr, snrmin );
                        if ( ( yprime[list[i]]<0 && yprime[list[i+1]]>=0 ) && ( nb_detected < awl->maxNumberOfDetections ) ) { 
				dx =  awl->x_distance[list[i+1]]-awl->x_distance[list[i]];
				//y_offset = yprime[list[i]] / (yprime[list[i]]-yprime[list[i+1]]);
				y_offset = interpol( yprime[list[i]-1], yprime[list[i]], yprime[list[i]+1], yprime[list[i]+2] );
                                detected[nb_detected] = offset +  awl->x_distance[list[i]]+ ( ( dx ) * ( y_offset )) ;
				awl->detected[channel][nb_detected].meters = detected[nb_detected];
				awl->detected[channel][nb_detected].intensity =    ( snr + 21 )*2;
				//printf("Intensity %d \n", awl->detected[channel][nb_detected].intensity);
				//getchar();
                                nb_detected++;
                        }
		    }
               }
	}

	return nb_detected;
}

/* ================================================================== */
// ALGO4

int rms_convolution(int32_t Y[], int nb_point, awl_data *awl)
{

	int i,j, channel;
	int sourcewidth = 7; 	//  7  halfwidth of the source pulse
	int meanwidth = 3;   	//  3    number of sample on each side for the average (DC calibration)
	int derivate_width = 5; 	// halfsize for the lognormal derviate
	float derivate_damping = 1.3;  //log normal parameter
	float snrmin = 20.0;     	//  SNR for thresholding
	float offset = 0.0;    	//  Offset to add to X if not already included
	float signal_mean = 0, signal_std = 0;
	// float initial_rms =0;

	float threshold=0.0;
        float  sum=0.0, derivate[30];
        float yscale[MAX_TRACK], yprime[MAX_TRACK], ysecond[MAX_TRACK];
	int start = 0;
	int end;
        int nb_detected =0;
	int process = 1;
	float snr;
	float dx, y_offset;
	float detected[DEFAULT_AWL_MAX_DETECTED] = {0};

	offset =  SYSTEM_OFFSET + ALGO_PARAM(0) - 0.6; // AWL16 Distance patch included

	derivate_damping  = ALGO_PARAM(2);  	//1.3
	meanwidth = ALGO_PARAM(3);     		//3 
	sourcewidth = ALGO_PARAM(4);		//7
	derivate_width = ALGO_PARAM(5); 	//5
	process = ALGO_PARAM(6); 		//1
	snrmin = ALGO_PARAM(7); 		//18.0

	/*
        FILE * gnuplotPipe = popen ("gnuplot -persistent", "w");
        FILE * gnuplotPipe2 = popen ("gnuplot -persistent", "w");
        FILE * gnuplotPipe3 = popen ("gnuplot -persistent", "w");
	*/

	channel = GET_CURRENT_AWL_CHANNEL;
	if (awl->dead_zone <= ACQUISITION_DELAY ) { 
		start = ACQUISITION_DELAY - awl->dead_zone;
	}
	end = nb_point;

        // derivative kernel
        // float FIRST[11]  =  {0, 0, 0, 0.00003,  2.65962,  0,  -2.65962,   -0.00003,  0, 0, 0 };
        // FIRST = cat(2,lognpdf(5:-1:1,0,FirstDamping),0,-lognpdf(1:5,0,FirstDamping));
        // DERIVATE = cat(2,lognpdf(derivatewidth:-1:1,0,derivatedamping),...
        //                  0,...
        //                                   -lognpdf(1:derivatewidth,0,derivatedamping));
        //
        for ( i=0; i<derivate_width; i++ ) {
        	derivate[i] = lognpdf( ( derivate_width - i ), 0, derivate_damping );
        	//derivate[i+derivate_width] = -1.0 * lognpdf(i,0, derivate_damping );
		derivate[(derivate_width*2)-i] = -1.0 * derivate[i];
        }
	derivate[derivate_width] = 0;
        //  sum(lognpdf([1:5],0,FirstDamping))
        for ( i=0; i<derivate_width; i++ ) {
        	sum = sum + derivate[i];
        }
        for ( i=0; i<( 2*derivate_width+1 ); i++ ) {
        	derivate[i] = derivate[i]/sum;
        }

	/*	
        for ( i=0; i<2*derivate_width+1; i++ ) {
              printf("derivate %lf\n", derivate[i]);
        }
        i = getchar();
	*/
	
	//%threshold for detection
	//aveB=mean(Y(10:T0index-10));
	//rmsB=sqrt(sum((Y(10:T0index-10)-aveB).^2)/(T0index-20));
	
        for (i=0;i<nb_point;i++) {
		yscale[i] =  ( float ) ( Y[i] );
        }
	
	/*	
	signal_mean = findmean(yscale);
	sum_2 = 0;
        for ( i=0; i<nb_sample; i++ ) {
		sum_2 =  sum_2 + ( yscale[i] - signal_mean ) * ( yscale[i] - signal_mean );
              	//printf("signal %f  sum2 %f\n", yscale[i], sum_2);
		
        }

	*/
	signal_std = compute_SNR_noise(yscale, awl->dead_zone);

        //printf("rmsb %f\n", signal_std);
        //i = getchar();

        //threshold= (2*sourcewidth+1)*(rmsB*(10.^(SNRmin/20))).^2;
        threshold = (2*sourcewidth+1) * ( signal_std * pow(10.0, (snrmin/20.0))) * ( signal_std * pow(10.0, (snrmin/20.0))); 
        
        //printf("process %d\n", process );
        //i = getchar();
        
	//%condtionning
	//%first conv (signal RMS)
	//YPRIME(1:length)=zeros(1,length);
	//for n=sourcewidth+meanwidth+1:length-sourcewidth-meanwidth
	//    YPRIME(n)=sum((Y(n-sourcewidth:n+sourcewidth)-aveB).^2);
	//end

        for ( i=sourcewidth+meanwidth+1; i<nb_point-sourcewidth-meanwidth ; i++ ) {
		yprime[i] = 0;
		// signal_mean=(mean(Y(n-sourcewidth-meanwidth:n-sourcewidth))+mean(Y(n+sourcewidth:n+sourcewidth+meanwidth)))/2;	
		if ( process ) {
			signal_mean = ( ( findmean(yscale+i-sourcewidth-meanwidth, meanwidth ) + 
				findmean(yscale+i+sourcewidth,  meanwidth ) ) /2 );
		
		}
        	for ( j=-sourcewidth; j<sourcewidth ; j++ ) {
			yprime[i] =  yprime[i] + ( yscale[i+j] - signal_mean ) * ( yscale[i+j] - signal_mean );	

		}
	}




	
	// Compute initial value
	/*
        for ( i=start-sourcewidth-1; i<=start+sourcewidth-1 ; i++ ) {
		initial_rms = initial_rms + ( yscale[i] - signal_mean ) * ( yscale[i] - signal_mean );	
	}  
        for ( i=start; i< nb_point; i++ ) {
		yprime[i] = initial_rms -( ( yscale[i-sourcewidth] - signal_mean ) * ( yscale[i-sourcewidth] - signal_mean ) )
		+ ( ( yscale[i+sourcewidth] - signal_mean ) * ( yscale[i+sourcewidth] - signal_mean ) );
	}
        */

        //printf("initial rms %f\n", initial_rms );
        //i = getchar();
	


	//YSECOND = conv(Y,DERIVATE);
	//YSECOND = YSECOND(1+derivatewidth:length+derivatewidth);%size adjustment

        i = convolution(yscale, nb_point, ysecond, derivate, (derivate_width*2)+1 );
        for ( i=0; i< (nb_point-derivate_width); i++ ) {
                ysecond[i] = ysecond[i+derivate_width];
        }

	/*
        i = vplot(gnuplotPipe, yprime , 351);
        i = vplot(gnuplotPipe2, yscale,  352);
        i = vplot(gnuplotPipe3, ysecond,  353);
	pclose(gnuplotPipe );
	pclose(gnuplotPipe2 );
	pclose(gnuplotPipe3 );
        i = getchar();
	*/

        nb_detected =0;
	for ( i=0; i< awl->maxNumberOfDetections; i++ ) {
		awl->detected[channel][i].meters = 0;
		awl->detected[channel][i].intensity = 0;
	}

	dx =  awl->x_distance[start+1]-awl->x_distance[start];
	for  ( i=start; i< end; i++ )  {
                if (( yprime[i] >=  threshold ) &&  ( ( ysecond[i-1]>0 && ysecond[i]<=0 ) && ( nb_detected < awl->maxNumberOfDetections )) ) { 
				y_offset = interpol( ysecond[i-1], ysecond[i], ysecond[i+1], ysecond[i+2] );
                                detected[nb_detected] = offset +  awl->x_distance[i]+ ( ( dx ) * ( y_offset )) ;
				awl->detected[channel][nb_detected].meters = detected[nb_detected];
				snr = compute_SNR_from_detection(yscale, i, sourcewidth, signal_std);
		    		//detRMS = rms(yscale+i-derivate_width, 1 + ( 2* derivate_width)  );
		    		//snr = 20*log10(detRMS/signal_std);		
				awl->detected[channel][nb_detected].intensity =    ( snr + 21 )*2;
				// printf("detRMS %f , sigRMS %f    %d  ", detRMS, sigRMS, nb_detected);
        			// getchar();
                                nb_detected++;
                }
        }

	return nb_detected;
}


/* ================================================================== */
// ALGO4.5

int matched_filter_algo45(int32_t Y[], int nb_point, awl_data *awl)
{
	// Algo parameters
	int sourcewidth = 4;	       // Halfwidth of the source pulse (Default: 7)
	int derivate_width = 5;        // Halfsize for the lognormal derivate (Default: 5)
	float std_factor = 0.4; 	       // Multiply the standard deviation for thresholding (Default: 6)
	float derivate_damping = 1.3;  // Log normal parameter (Default: 1.3)
	float snrmin = 20.0;     	//  SNR for thresholding
	float offset = 5.0;   	       //  Offset to add to X if not already included

	// Other variables
	int i,j, channel, found = 0;
	int list[MAX_TRACK];
	float matched_signal_max = 0;
	float matched_noise_mean = 0;
	float signal_std = 0.0;
	float threshold= 0.0;
        float  sum=0.0,  derivate[30];
        float yscale[MAX_TRACK], ymatched[MAX_TRACK], yprime[MAX_TRACK];
	int start = 0;
	int end;
        int nb_detected =0;
	float snr;
	float dx, y_offset;
	float detected[DEFAULT_AWL_MAX_DETECTED] = {0};
	
	// Read parameter configuration from config file
	offset = SYSTEM_OFFSET + ALGO_PARAM(0) - 0.6; // AWL16 Distance patch included
	std_factor = ALGO_PARAM(1);
	derivate_damping  = ALGO_PARAM(2);  	
	sourcewidth = ALGO_PARAM(3);		
	derivate_width = ALGO_PARAM(4);
	snrmin = ALGO_PARAM(5);

	// Parameter validation
	if(sourcewidth < 3 || sourcewidth > 9){
		printf("!Warning! Algo45: sourcewidth at value %d is out of bound.\n", sourcewidth);
		printf("Setting sourcewidth value to default: 7.\n");
		sourcewidth = 7;
	}

	channel = GET_CURRENT_AWL_CHANNEL;
	if (awl->dead_zone <= ACQUISITION_DELAY ) { 
		start = ACQUISITION_DELAY - awl->dead_zone;
	}
	end = nb_point;

	// Create derivate waveform
        for ( i=0; i<derivate_width; i++ ) {
        	derivate[i] = lognpdf( ( derivate_width - i ), 0, derivate_damping );
		derivate[(derivate_width*2)-i] = -1.0 * derivate[i];
        }

	derivate[derivate_width] = 0;

        for ( i=0; i<derivate_width; i++ ) {
        	sum = sum + derivate[i];
        }
        for ( i=0; i<( 2*derivate_width+1 ); i++ ) {
        	derivate[i] = derivate[i]/sum;
        }
	
	// Scale signal & calculate RMS
        for (i=0;i<nb_point;i++) {
		yscale[i] =  ( float ) ( Y[i] );
        }

	signal_std = compute_SNR_noise(yscale, awl->dead_zone);
	
	// Matched filter operation
        for ( i=10+sourcewidth+1; i<nb_point-sourcewidth-1 ; i++) 
	{
		ymatched[i] = matched_filter_models[sourcewidth-3][sourcewidth+1]*yscale[i]; // Center point
		for(j = 0; j < sourcewidth ; j++) {
                	ymatched[i] += matched_filter_models[sourcewidth-3][j]*yscale[i - sourcewidth + j]; // Left arm
			ymatched[i] += matched_filter_models[sourcewidth-3][j]*yscale[i + sourcewidth - j]; // Right arm
		}
	}


	// First stage threshold
	matched_signal_max = findmax(ymatched+10+sourcewidth+1, nb_point-sourcewidth-1);
	matched_noise_mean = findmean(ymatched+10+sourcewidth+1, 30);
	threshold = matched_noise_mean + std_factor*(matched_signal_max-matched_noise_mean);
	
	// New SNR min controlled threshold: (the 3.7 factor is sigma_matched_source/sigma_source)
	// threshold =  3.7*( signal_std * pow(10.0, (snrmin/20.0)));

	// if(awl->verbose_distance) {
	// 	printf("Signal threshold: %f\n", threshold);
	// 	printf("Signal mean: %f\n", signal_mean); 
	// 	printf("%f %f %f %f %f\n", yscale[100], yscale[200], yscale[300], yscale[400], yscale[500]);
	// 	printf("%f %f %f %f %f\n", ymatched[100], ymatched[200], ymatched[300], ymatched[400], ymatched[500]);
	// }

	// Create first derivative signal
        i = convolution(yscale, nb_point, yprime, derivate, (derivate_width*2)+1 );
        for ( i=0; i< (nb_point-derivate_width); i++ ) {
                yprime[i] = yprime[i+derivate_width];
        }

        nb_detected =0;
	for ( i=0; i< awl->maxNumberOfDetections; i++ ) {
		awl->detected[channel][i].meters = 0;
		awl->detected[channel][i].intensity = 0;
	}

	for  ( i=start+10+sourcewidth+1; i< end-sourcewidth-1; i++ )  {
                if ( ymatched[i] >= threshold ) {
                        list[found] = i;
                        found++;
                }
        }

         for ( i=0; i < found; i++ ) {
                if ( list[i] == ( list[i+1] -1 ) ) {
		    snr = compute_SNR_from_detection(yscale, list[i+1], sourcewidth, signal_std);
		    //detRMS = rms( yscale+list[i+1]-sourcewidth, 1 + (2*sourcewidth) );
		    //snr = 20*log10(detRMS/signal_std);
		    //printf("SNR: %f, SNR MIN: %f\n", snr, snrmin);
		    if  ( snr > snrmin  ) {
                        if ( ( yprime[list[i]]>=0 && yprime[list[i+1]]<0 ) && ( nb_detected < awl->maxNumberOfDetections ) ) {
				dx =  awl->x_distance[list[i+1]]-awl->x_distance[list[i]];
				y_offset = interpol( yprime[list[i]-1], yprime[list[i]], yprime[list[i]+1], yprime[list[i]+2] );
                                detected[nb_detected] = offset +  awl->x_distance[list[i]]+ ( ( dx ) * ( y_offset )) ;
				awl->detected[channel][nb_detected].meters = detected[nb_detected];
				awl->detected[channel][nb_detected].intensity =    ( snr + 21 )*2;
                                nb_detected++;
                        }
		    }
               }
	}


	return nb_detected;
}

/* ================================================================== */
// TEST MESSAGES
/* ================================================================== */

int test_msg(int32_t Y[], int nb_point, awl_data *awl)
{

	int  channel;
	channel = GET_CURRENT_AWL_CHANNEL;

	awl->detected[channel][0].meters =  10 + channel;
	awl->detected[channel][0].intensity = 10 + channel;
	awl->detected[channel][1].meters =  20 + channel;
	awl->detected[channel][1].intensity = 20 + channel;

	return 2;


}

/* ================================================================== */

int test_msg2(int32_t Y[], int nb_point, awl_data *awl)
{

	int  channel;
	channel = GET_CURRENT_AWL_CHANNEL;

	awl->detected[channel][0].meters =  CURRENT_AWL_PAYLOAD_S32[8] / 100000;
	awl->detected[channel][0].intensity = CURRENT_AWL_PAYLOAD_S32[8] /10000;
	awl->detected[channel][1].meters =  CURRENT_AWL_PAYLOAD_S32[8] / 5000;
	awl->detected[channel][1].intensity = CURRENT_AWL_PAYLOAD_S32[8] / 5000;

	return 2;

}

/* ================================================================== */
// MAIN FUNCTIONS
/* ================================================================== */

int get_distance(int32_t a[], int num_elements, awl_distance **distance, awl_data *awl)
{

	//int acq_mode[4] = {8,4,2,1};
	int multiple;
	int nb_points;
	const int header = 24;
	//static int print_track = 0x00006100;
	// int i=0;
	//int chooser;

	//awl->dead_zone = -40;
	nb_points	= num_elements - ( ACQUISITION_DELAY - awl->dead_zone + header );
	//multiple =  acq_mode[GET_CURRENT_AWL_ACQ_MODE];
	multiple =1;
	//printf("Mul %d  :\n", multiple); 
	
	//chooser = awl->auto_calib_counter % 3;

	// Test for the RMS force to algo n
	//		printf("Saturation %u\n",saturation_test(a+AWL_PAYLOAD_HEADER_SIZE_32, nb_points ,  awl));
	//awl->current_algo = 3;
	
	switch (awl->current_algo) {
		case 1:
			return max_array( a+AWL_PAYLOAD_HEADER_SIZE_32, nb_points,  awl->current_algo,  multiple, awl);
			break;
		case 2:
			return double_wave( a+AWL_PAYLOAD_HEADER_SIZE_32, nb_points ,  awl);
			break;
		case 3:
			return double_derivate( a+AWL_PAYLOAD_HEADER_SIZE_32, nb_points,  awl);
			break;
		case 4:
			return rms_convolution( a+AWL_PAYLOAD_HEADER_SIZE_32, nb_points,  awl);
			break;
		case 5:
			return matched_filter_algo45( a+AWL_PAYLOAD_HEADER_SIZE_32, nb_points, awl);
			//return music( a+AWL_PAYLOAD_HEADER_SIZE_32, nb_points,  awl);
			break;
		case 6:
			return matched_filter_algo45( a+AWL_PAYLOAD_HEADER_SIZE_32, nb_points, awl);
			break;
		case 9:
			return test_msg( a+AWL_PAYLOAD_HEADER_SIZE_32, nb_points,  awl);
			break;
		case 10:
			return test_msg2( a+AWL_PAYLOAD_HEADER_SIZE_32, nb_points,  awl);
			break;
		default:
			return double_derivate( a+AWL_PAYLOAD_HEADER_SIZE_32, nb_points,  awl);
			break;
	}	

}

/* ================================================================== */

int analyze_it(awl_data *awl) {

	int i;
	//int j,k;
	int channel;
	//int nb_bin=0;
	//int near;
	//int bin_group=0;
	//float  right_value=0;
	//float  left_value=0;
	//float nearest;
	channel = GET_CURRENT_AWL_CHANNEL;
	//printf ("dist ch %d\n", channel);
        awl_distance *awl_dist = NULL;
	uint16_t distance_buffer[DEFAULT_AWL_MAX_DETECTED] = {0};
	uint16_t intensity_buffer[DEFAULT_AWL_MAX_DETECTED] = {0};
	//histogram histo_sorted[AWL_MAX_DETECTED_HISTO];

	//memset(histo_sorted, 0, AWL_MAX_DETECTED_HISTO * sizeof(struct histogram));

	if (awl->current_algo > 255) {
		memset(CURRENT_AWL_PAYLOAD, 0, AWL_PAYLOAD_SIZE_MAX);
		awl->current_algo -= 255;
	}

	if (awl->current_algo != 255) {
	 	if ( !awl->playback_channel_mask ) {
			denoise( CURRENT_AWL_PAYLOAD_S32+4, CALIBRATION_DATA_S32(channel)+4 , AWL_PAYLOAD_SIZE_S32-8);
		} else {
			//printf("Playback mode\n");
		}

		get_distance( CURRENT_AWL_PAYLOAD_S32  , AWL_PAYLOAD_SIZE_S32, &awl_dist, awl);

		if (awl->verbose_distance) {	
			printf("DSI 0x%08x PG%d CH%d ISR%d\n",
			       CURRENT_AWL_TIMESTAMP, CURRENT_AWL_TAIL_PAGE, CURRENT_AWL_TAIL_CHANNEL, CURRENT_AWL_TAIL_ISR);
			printf("DSALGO %d\n",awl->current_algo);
			printf("DSPARAMS %d %d %d %d %d\n", awl->maxNumberOfDetections, awl->historicSize, awl->maxNumberOfTrackPerChannel, awl->maxDetectionsInHistoric, awl->maxObstacles);
			printf("DST CH%d", channel);
		}
		
		for (i=0;i<awl->maxNumberOfDetections;i++) // Fill ip the detected history
		{
			awl->detected_history[channel][ awl->history_index[channel]*awl->maxNumberOfDetections + i ].meters    = 
			awl->detected[channel][i].meters;
			awl->detected_history[channel][ awl->history_index[channel]*awl->maxNumberOfDetections + i ].intensity = 
			awl->detected[channel][i].intensity;
		}
	
#if 0
		if ( awl->window_width > 0 ) {
			// Insert last detected values in the history values  = 0 =>  no detection.
			//printf("Channel %d    Index  %d\n", channel, awl->history_index[channel]);
			for (i=0;i<AWL_MAX_DETECTED;i++) {
				awl->detected_history[channel][ awl->history_index[channel]*8 + i ].meters    = 
				awl->detected[channel][i].meters;
				awl->detected_history[channel][ awl->history_index[channel]*8 + i ].intensity = 
				awl->detected[channel][i].intensity;
			}
			// Create an histogram
			for (i=0;i<AWL_MAX_DETECTED_HISTO;i++) {
				histo_sorted[i].meters = awl->detected_history[channel][i].meters;
				//printf("\t%d - %f ",awl->detected[channel][i].intensity, awl->detected_history[channel][i].meters );

			}	
			qsort ( histo_sorted, AWL_MAX_DETECTED_HISTO , sizeof(histogram), compare_distance );
			//printf("\n Sorted  Histogram Win valid Bin \n");
               		for (i=0;i<AWL_MAX_DETECTED_HISTO;i++) {
				//nearest = floorf(histo_sorted[i].meters * 10 + 0.5) / 10;
				near = histo_sorted[i].meters * 10 + 0.5;
				nearest = 0.1 * round( near );

				if ( nearest > 0  ) {
					//printf("\n  %d %f \n", near, nearest);
					if ( nb_bin==0 ) {
						 //printf("\n first bin  %d %f \n", near, nearest);
						 histo_sorted[nb_bin].qty++;
						 histo_sorted[nb_bin].bin = nearest;
					 	histo_sorted[nb_bin].meters =   histo_sorted[i].meters;
					 	nb_bin++;
					} else {
						if ( histo_sorted[i].meters < histo_sorted[nb_bin-1].bin + 0.1 ) {
					 	//printf("\n fit first bin  %d %f \n", near, nearest);
					 	histo_sorted[nb_bin-1].qty++;
						histo_sorted[nb_bin-1].meters =  histo_sorted[i].meters;
						} else {
					 		//printf("\n go to next bin  %d %f \n", near, nearest);
					 		histo_sorted[nb_bin].qty++;
							histo_sorted[nb_bin].bin =  nearest;
							histo_sorted[nb_bin].meters =   histo_sorted[i].meters;
							nb_bin++;
						}
					}		
	
		        	}		
			   //printf("\t%f - %f ",histo_sorted[i].intensity, histo_sorted[i].meters );
               		}  // for all the history
	
			//printf("\n");
			//
			//Travelling the histogramme with window_width
			
	        	j=0;
			bin_group=0;
			for (i=0;i<AWL_MAX_DETECTED_HISTO;i++) {
				left_value = histo_sorted[i].bin;
				right_value = histo_sorted[i].bin + ( awl->window_width / 10.0 ) + 0.01;

                                bin_group = 0;
                                while (( histo_sorted[i].bin < right_value ) &&
                                        ( histo_sorted[i].bin != 0 ) && ( i < 40 )) {
                                        bin_group =  bin_group + histo_sorted[i].qty;
                                        i++;
                                }


                               if ( ( bin_group >= awl->window_qty ) && ( j < 8 ) )   {
                                        distance_buffer[j] =  100 *  left_value;
                                        intensity_buffer[j] = histo_sorted[i].intensity;
					/* populate  SMA and persist  */
					
					
	 				if ( awl->algo_sma != 0 ) {
			       			awl->detected_sma[channel][ awl->sma_index[channel]*8 + j ].bin    =
						distance_buffer[j];
                                		awl->detected_sma[channel][ awl->sma_index[channel]*8 + j ].intensity =
                                        	intensity_buffer[j];
						awl->sma_index[channel] =  ( awl->sma_index[channel] + 1 ) % awl->algo_sma;
					}

        				if ( awl->algo_persistence !=0 ) {
			       			awl->detected_persist[channel][ j ].bin    = left_value;
                                		awl->detected_persist[channel][ j ].intensity = histo_sorted[i].intensity;
                                		awl->detected_persist[channel][ j ].qty = awl->algo_persistence;
					//awl->persist_index[channel] =  ( awl->persist_index[channel] + 1 ) % awl->algo_persistence;
					}

					/* populate  SMA and persist  */
	
                                        if (awl->verbose_distance) {
                                                printf(", %6.2f m", distance_buffer[j]/100.0   );
                                        }
                                        j++;
                             }
				/*
				if ( histo_sorted[i].qty + bin_group >  0 ) {
					printf("\nHisto\t %f - %f - %d ",
					left_value, right_value,  bin_group );
				}
				*/
                        }

	 		if ( awl->algo_sma != 0 ) {
                        	for (i=0;i<AWL_MAX_DETECTED_HISTO;i++) {
			   		printf("\t%f - %d  ",awl->detected_sma[channel][i].bin,
								awl->detected_sma[channel][i].intensity );
				}
	 			//getchar();
			}
				
			int somme_distance_buffer = 0;	
        		if ( awl->algo_persistence !=0 ) {
				for (i=0;i<8;i++) {
					somme_distance_buffer = somme_distance_buffer + distance_buffer[i];
				}
			         //printf("somme buffer %d   %d \n", somme_distance_buffer, awl->detected_persist[channel][0].qty);
				if   ( somme_distance_buffer < 1 )  {
					if ( awl->detected_persist[channel][0].qty > 0 ) {

                        			for (k=0;k<8;k++) {
							distance_buffer[k] = 100 * awl->detected_persist[channel][k].bin;
			   				//printf("\t%f - %d -  %d ",awl->detected_persist[channel][k].bin,
							//		awl->detected_persist[channel][k].intensity,
							//		awl->detected_persist[channel][k].qty);
						}
						if  ( awl->detected_persist[channel][0].qty ==  1 ) {
							for (k=0;k<8;k++) {
								awl->detected_persist[channel][k].bin = 0;
							}
						}
					 	awl->detected_persist[channel][0].qty--;
					}
	 				//	getchar();
				}
			}
			

			
		} else {  // Case of histogram off  ie  histo_width = 0
			for (i=0;i<8;i++) {
				distance_buffer[i] =  100 *  awl->detected[channel][i].meters;
				intensity_buffer[i] =    awl->detected[channel][i].intensity;
				if (awl->verbose_distance) {
					printf(", %6.2f m",awl->detected[channel][i].meters  );
				}
			}

		}
#endif
		for (i=0;i<awl->maxNumberOfDetections;i++) {
			distance_buffer[i] =  100 *  awl->detected[channel][i].meters;
			intensity_buffer[i] =    awl->detected[channel][i].intensity;
			if (awl->verbose_distance) {
				printf(", %6.2f m",awl->detected[channel][i].meters  );
			}
		}

		awl->history_index[channel] =  ( awl->history_index[channel] + 1 ) % awl->historicSize;


		if (awl->verbose_distance) {
			printf("\n");
			printf("INT CH%d", channel);
			for (i=0;i<awl->maxNumberOfDetections;i++) {
				printf(", %6d  ", awl->detected[channel][i].intensity  );
			}
			printf("\n");
		}

	}  // if not algo debug can 255


	i = track_distance(awl);

	// if(awl->verbose_distance) {
	// 	int o;
	// 	for(o = 0; o < AWL_MAX_TRACK_PER_CHANNEL; o++)
	// 	{
	// 		printf("Channel: %d ; Track ID: %d ; Track pos: %d ; Track speed: %d ; Track int: %d\n", awl->obstacles[channel * AWL_MAX_TRACK_PER_CHANNEL +i].channel,
	// 		 awl->obstacles[channel * AWL_MAX_TRACK_PER_CHANNEL + o].track_id, 
	// 		 awl->obstacles[channel * AWL_MAX_TRACK_PER_CHANNEL + o].track_distance,
	// 		 awl->obstacles[channel * AWL_MAX_TRACK_PER_CHANNEL + o].track_speed,
	// 		 awl->obstacles[channel * AWL_MAX_TRACK_PER_CHANNEL +i].object_intensity);
	// 	}
	// }

	//printf ("analyzing channel %d\n", channel);
	send_distance(awl, channel, distance_buffer);
	send_intensity(awl, channel, intensity_buffer);
	send_distance_intensity(awl, channel, distance_buffer, intensity_buffer);
     
/*
       	printf(  "Object 1-4 %d  canbus %d  " , channel, MSG_CH1_DISTANCE1_4 + channel );
	printf( "0x%04x cm  0x%04x cm  0x%04x cm 0x%04x cm\n", distance_buffer[0], distance_buffer[1], 
							distance_buffer[2], distance_buffer[3]);
*/

/*
       	printf(  "Object 5-8 %d  canbus %d  " , channel, MSG_CH1_DISTANCE5_8 + channel );
	printf( "0x%04x cm  0x%04x cm  0x%04x cm 0x%04x cm\n", distance_buffer[4], distance_buffer[5], 
							distance_buffer[6], distance_buffer[7]);
*/
	return 0;	

}
