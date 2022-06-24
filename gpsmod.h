
#ifndef _GPS_H_
#define _GPS_H_

#include "gpsd/gpsd-dev/include/libgps.h"
#include "bluetooth.h"

#define MAX_GPS_WAIT_RETRIES 60 // 60 tries at 0.5 seconds a try is a 30 second timeout
#define MAX_GPS_READ_RETRIES 5
#define GPS_WAIT_TIME_MICROSECS 500000 // 1/2 second

static struct gps_data_t gpsdata;
static struct fixsource_t source;

int init_gps();
void process_gps_data(struct ODID_UAS_Data *uasData);

#endif