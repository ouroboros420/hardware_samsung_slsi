/*----------------------------------------------------------------------------
 *      Exynos SoC  -  GNSS
 *----------------------------------------------------------------------------
 *      Name:    gnss.h
 *      Purpose: To expose GNSS APIs and define macros
 *      Rev.:    V1.00
 *----------------------------------------------------------------------------
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _GNSS_API_CHUB_H_INCLUDED_
#define _GNSS_API_CHUB_H_INCLUDED_

/* This file will be shared. */

#include <stdint.h>
#include <stdbool.h>

#define GNSS_TIME_FLAGS_HAS_UTC_OFFSET (1 << 0)
typedef struct GNSSTimeStruct
{
   uint64_t gps_time;   /* Number of nanoseconds since Jan 6 1980 */
   int32_t  utc_offset; /* Offset between GNSS time and UTC time (including the UTC leap second count since Jan 6 1980) */
   uint8_t  flags;      /* Time flags; see above definitions */
} GNSSTime;
/*sensor naming convension: [ACL|MAG|GYRO|BARO|TEMP]_[R:raw|C:calibrated]<data elements per measurement reading>*/
typedef enum
{
   ACL_R3 = 0,
   MAG_R3 = 1,
   GYRO_R3 = 2,
   BARO_R1 = 3,
   TEMP_R1 = 4,
} GnssMemsType;

typedef enum
{
   GNSS_CTX_STATUS_UNKNOWN = 0,
   GNSS_CTX_STATUS_NOT_MOVING = 1,
   GNSS_CTX_STATUS_MOVING = 2,
   GNSS_CTX_STATUS_WALKING = 3,
   GNSS_CTX_STATUS_DRIVING = 4,
   GNSS_CTX_STATUS_RUNNING = 5,
   GNSS_CTX_STATUS_FROZEN = 6,
   GNSS_CTX_STATUS_BIKING = 7,
   GNSS_CTX_STATUS_SWIMMING = 8,
   GNSS_CTX_STATUS_COUNT = 9,
} GnssContextStatus;

typedef enum
{
   GNSS_CTX_CONF_UNKNOWN = 0, /* External sensor fusion cannot determine the confidence. */
   GNSS_CTX_CONF_LOW = 1,     /* Confidence is greater than 25%. */
   GNSS_CTX_CONF_MED = 2,     /* Confidence is greater than 50%. */
   GNSS_CTX_CONF_HIGH = 3,    /* Confidence is greater than 95%. */
   GNSS_CTX_CONF_COUNT = 4,   /* Confidence is greater than 95%. */
} GnssContextConfidence;

/** Represents a Location. */
typedef struct GNSSLocationStruct
{
   double lat;                 /** Latitude in degrees */
   double lon;                 /** Longitude in degrees */
   double altitude;            /** Altitude, WGS84 in meters. */
   double velocity_horizontal; /** Velocity 2D. in m/s. */
   double velocity_heading;    /** <Heading> : Heading, in degrees */
   uint32_t accuracy;          /** 95 percentile 2D error in meters */
   GNSSTime time;              /** Time of the GPS fix */
   uint8_t precise_accuracy;   /** Set to 1 means velocity is precise */
   float velocity_accuracy;    /** Velocity accuracy. Range from 0 to 5.10 m/s */
   double alt_msl;            /** Altitude from Mean Sea Level, in meters. */
}GNSSLocation;

/* Types of constellation */
#define GNSS_CONSTELLATION_UNKNOWN   0
#define GNSS_CONSTELLATION_GPS       1
#define GNSS_CONSTELLATION_SBAS      2
#define GNSS_CONSTELLATION_GLONASS   3
#define GNSS_CONSTELLATION_QZSS      4
#define GNSS_CONSTELLATION_BEIDOU    5
#define GNSS_CONSTELLATION_GALILEO   6

/* Types of flags */
#define GNSS_SV_FLAGS_HAS_EPHEMERIS_DATA  (1 << 0)
#define GNSS_SV_FLAGS_HAS_ALMANAC_DATA    (1 << 1)
#define GNSS_SV_FLAGS_USED_IN_FIX         (1 << 2)

#define GNSS_STATUS_MAX_SVS 64

typedef struct GNSSSatStatusStruct
{
   uint8_t svid;
   uint8_t constellation;
   uint8_t flags;
   float   cno_dbhz;
   float azimuth;
   float elevation;
}GNSSSatStatus;

typedef struct GNSSStatusStruct
{
   int32_t num_svs;
   GNSSSatStatus sat[GNSS_STATUS_MAX_SVS];
}GNSSStatus;

typedef struct GNSSSatStatusLightStruct
{
   uint8_t svid;
   uint8_t constellation;
   uint8_t flags;
   uint8_t cno_dbhz;
}GNSSSatStatusLight;

typedef struct GNSSStatusLightStruct
{
   int32_t num_svs;
   GNSSSatStatusLight sat[GNSS_STATUS_MAX_SVS];
}GNSSStatusLight;

/**
 * Location callback, will be called with NULL of no location is available when
 * the callback is made.
 * Struct 'location' is only valid until function returns.
 * Please copy it to your own memory for using it somewhere else.
 **/
typedef void (*GNSSLocationCallback) (GNSSLocation* location);

/**
 * SV Status callback, will be called with NULL of no location is available when
 * the callback is made.
 * Struct 'status' is only valid until function returns.
 * Please copy it to your own memory for using it somewhere else.
 **/
typedef void (*GNSSStatusCallback) (GNSSStatus* status);

/**
 * Light version of SV Status callback for mobile, will be called with NULL of no location is available when
 * the callback is made.
 * Struct 'status' is only valid until function returns.
 * Please copy it to your own memory for using it somewhere else.
 **/
typedef void (*GNSSStatusLightCallback) (GNSSStatusLight* status);

/**
 * GNSS unavailable callback, will be called when GNSS signals are unavailable
 * Inside this callback, you may call setGnssGeofenceEnable(false) to disable
 * geofence processing
 **/
typedef void (*GNSSUnavailCallback) (bool unavailable);

/**
 * GNSS Chub ICD Version
 */
uint32_t gnssChubIcdVersion(void);

/**
 * Register location callback.
 *
 * @param cb Callback to receive location updates.
 *
 */
void gnssRegisterLocationCallback(GNSSLocationCallback cb);

/**
 * Register satellite status callback.
 *
 * @param cb Callback to receive satellite status updates.
 *
 */
void gnssRegisterStatusCallback(GNSSStatusCallback cb);

/**
 * Register light satellite status callback.
 *
 * @param cb Callback to receive light satellite status updates.
 *
 */
void gnssRegisterStatusLightCallback(GNSSStatusLightCallback cb);

/**
 * Register passive location callback.
 *
 * @param cb Callback to receive passive location updates.
 *
 */
void gnssRegisterPassiveCallback(GNSSLocationCallback cb);

/**
 * Register unavailable signal status callback.
 *
 * @param cb Callback to receive unavailable signal status updates.
 *
 */
void gnssRegisterUnavailableCallback(GNSSUnavailCallback cb);

/**
 * Clear location callback.
 */
void gnssClearLocationCallback(void);

/**
 * Clear satellite status callback.
 */
void gnssClearStatusCallback(void);

/**
 * Clear light satellite status callback.
 */
void gnssClearStatusLightCallback(void);


/**
 * Clear passive location callback.
 */
void gnssClearPassiveCallback(void);

/**
 * Clear unavailable signal status callback.
 */
void gnssClearUnavailableCallback(void);

/**
 * Start a location request.
 *
 * @param rate  = 0  Single Fix
 *              > 0  Time between location fixes in seconds.
 *
 * @Note - A location callback or status callback must be set or this will return false.
 *
 * @return false Error
 *         true  Success
 **/
bool gnssRequestLocationUpdates(int32_t rate, bool power);

/**
 * Stop a location request
 *
 * @return false Error
 *         true  Success
 **/
bool gnssStopLocationUpdates(void);

/**
 * Eable passive location updates.
 *
 * @param enable Enables passive location updates to be reported when active updates are stopped.
 *
 * @Note The callback for passive updates must be set or this will return false.
 *
 * @return false Error
 *         true  Success
 **/
bool gnssSetPassiveLocation(bool enable);

/**
 * Enables or disables geofence processing
 *
 * @return false Error
 *         true  Success
 **/
bool gnssSetGeofence(bool enable);

/**
 * Enable or disable unavailable signal updates
 *
 * @return false Error
 *         true  Success
 **/
bool gnssSetUnavailableUpdates(bool enable);

/**
 * Enables or disables gnss logging
 *
 * @return false Error
 *         true  Success
 **/
bool gnssSetLogging(bool enable);

/**
 * Passes in a context and confidence level to GNSS, allowing GNSS to optimize its solution accordingly
 *
 * @param status - An enum describing the context.
 *
 * @param confidence - An enum describing the confidence level.
 *
 * @return false Error
 *         true  Success
 **/
bool gnssSetContextStatus(GnssContextStatus status, GnssContextConfidence confidence);

/** */
bool gnssUpdateRawSensorMeasurement(GnssMemsType sensorId, uint64_t time, int32_t *data);

/*  ######### Location request USAGE EXAMPLE ###########
void locationCB(GNSSLocation *location)
{
   GNSSLocation *pLoc = (GNSSLocation*) location;
   printf("New location lon:%f lat:%f\n", pLoc->lon, pLoc->lat);
}

void statusCB(GNSSStatus *svStatus)
{
   GNSSStatus *pStat = (GNSSStatus*) svStatus;
   printf("Tracked SV Num : %d\n", pStat->num_svs);
   for(int i = 0; i<pStat->num_svs; i++)
   {
      printf("[ %d ]  SV_ID : %d, SV_CN0 : %f\n", i, pStat->sat[i].svid, pStat->sat[i].cno_dbhz);
   }
}

void gnssUnavailCB(void)
{
   printf("GNSS unavailable! disable for 10 seconds and retry\n")
   setGnssGeofenceEnable(false);
   sleep(10*1000);
   setGnssGeofenceEnable(true);
}

int main(void)
{
   requestGnssUnavailUpdates(gnssUnavailCB);

   requestLocationUpdates(1, locCB, statCB);
   printf("Location request is running\n");

   sleep(60*1000);

   printf("Stopping location request\n");
   stopLocationUpdates();

   requestPassiveLocationUpdates(locCB, statCB);
   sleep(60*1000);
   stopPassiveLocationUpdates();
}
*/

#endif /* _GnssApi_CHUB_H_INCLUDED_ */
