#ifndef AWL_TRACK_H
#define AWL_TRACK_H
#include "awl_data.h"
#include "awl_distance.h"
#include "awl_fdist.h"

#define DEFAULT_MAX_NUMBER_OF_TEMP_TRACKS  		DEFAULT_AWL_MAX_DETECTED * DEFAULT_AWL_MAX_DETECTED

int track_distance(awl_data *awl);

#endif
