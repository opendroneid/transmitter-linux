
#include "gpsmod.h"
#include <math.h>

int init_gps(struct fixsource_t* source, struct gps_data_t* gpsdata) {
    unsigned int flags = WATCH_ENABLE;

    gpsd_source_spec(NULL, source);

    if (0 != gps_open(source->server, source->port, gpsdata)) {
        return 1;
    }

    if (NULL != source->device) {
        flags |= WATCH_DEVICE;
    }
    
    gps_stream(gpsdata, flags, source->device);

    return 0;
}

void process_gps_data(struct gps_data_t* gpsdata, struct ODID_UAS_Data *uasData) {
    if(gpsdata->fix.mode >= MODE_2D) {
        uasData->Location.Latitude = gpsdata->fix.latitude;
        uasData->Location.Longitude = gpsdata->fix.longitude;

        if(isfinite(gpsdata->fix.track)) {
            uasData->Location.Direction = gpsdata->fix.track;
        }
    }

    if(gpsdata->fix.mode >= MODE_3D) {
        if(isfinite(gpsdata->fix.altMSL)) {
            uasData->Location.AltitudeGeo = gpsdata->fix.altMSL;
            uasData->Location.AltitudeBaro = gpsdata->fix.altMSL;
            uasData->Location.Height = \
                gpsdata->fix.altMSL - uasData->System.OperatorAltitudeGeo;
        } else if(isfinite(gpsdata->fix.altitude)) {
            uasData->Location.AltitudeGeo = gpsdata->fix.altitude;
            uasData->Location.AltitudeBaro = gpsdata->fix.altitude;
            uasData->Location.Height = \
                gpsdata->fix.altitude - uasData->System.OperatorAltitudeGeo;
        }

        if(isfinite(gpsdata->fix.speed)) {
            uasData->Location.SpeedHorizontal = gpsdata->fix.speed;
        }

        if(isfinite(gpsdata->fix.climb)) {
            uasData->Location.SpeedVertical = gpsdata->fix.climb;
        }

        if(isfinite(gpsdata->fix.eph)) {
            uasData->Location.HorizAccuracy = createEnumHorizontalAccuracy(gpsdata->fix.eph);
        }

        if(isfinite(gpsdata->fix.epy)) {
            uasData->Location.VertAccuracy = createEnumVerticalAccuracy(gpsdata->fix.epy);
        }

        if(isfinite(gpsdata->fix.eps)) {
            uasData->Location.SpeedAccuracy = createEnumSpeedAccuracy(gpsdata->fix.eps);
        }

    }
}