
#ifndef _GPSMOD_H_
#define _GPSMOD_H_

#include <stdlib.h>

#include "gpsd/gpsd-dev/include/libgps.h"
#include "bluetooth.h"

#define MAX_GPS_WAIT_RETRIES 60 // 60 tries at 0.5 seconds a try is a 30 second timeout
#define MAX_GPS_READ_RETRIES 5
#define GPS_WAIT_TIME_MICROSECS 500000 // 1/2 second

int init_gps(struct fixsource_t* source, struct gps_data_t* gpsdata);
void process_gps_data(struct gps_data_t* gpsdata, struct ODID_UAS_Data *uasData);

#endif