#ifndef __GPS_H__
#define __GPS_H__

#include "stdint.h"
#include "stdbool.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NMEA_MAX 90

typedef struct
{
	bool isValid;

    uint8_t hour;
    uint8_t minute;
    uint8_t second;

    uint8_t day;
    uint8_t month;
    uint8_t year;

    uint32_t speed_kmh10;
} GPS_Result_t;

// TOOD review
extern char gps_latest[NMEA_MAX];
extern bool gps_ready;

void gps_init();
GPS_Result_t ParseGNRMC(char* nmea);

#ifdef __cplusplus
}
#endif

#endif
