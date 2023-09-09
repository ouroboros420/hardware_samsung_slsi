#ifndef __GNSS_PROTOCOL_H__
#define __GNSS_PROTOCOL_H__

/*
 * Copyright (C) 2018 Samsung Electronics Co., Ltd.
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

#include <stdint.h>
#if defined(MBGNSS_REQUIRED)

//Make BETP msgID
#define BETP_ENC_SUB_ID(message_sub_id) (((uint16_t)(message_sub_id)  & 0xFF) << 8)
#define BETP_ENC_MID(message_id)         ((uint16_t)(message_id)      & 0xFF)
#define BETP_MAKE_MSG_ID(mid, sid) (BETP_ENC_SUB_ID(sid) | BETP_ENC_MID(mid))

#define BETP_SIRF_MSG_SSB_SIRF_INTERNAL BETP_MAKE_MSG_ID(228, 0)
#define BETP_SIRF_MSG_POS_REQ BETP_MAKE_MSG_ID(101, 4)
#define BETP_SIRF_MSG_SSB_PS_POS_RSP BETP_MAKE_MSG_ID(69, 3)
#define BETP_TARGET_GNSS_SV_STATUS_REPORT BETP_MAKE_MSG_ID(10, 113)
#define BETP_TARGET_GNSS_SV_STATUS_REPORT_LIGHT BETP_MAKE_MSG_ID(10, 132)
#define BETP_SIRF_MSG_SSB_GEODETIC_NAVIGATION BETP_MAKE_MSG_ID(41, 255)
#define BETP_TARGET_POS_REQ_ACK BETP_MAKE_MSG_ID(10, 119)
#define BETP_SIRF_MSG_SSB_PLATFORM_TEST BETP_MAKE_MSG_ID(151, 255)


#define OSP_PAUSE_GEOFENCE_OPERATION BETP_MAKE_MSG_ID(16, 5)
#define OSP_CONTEXT_AIDING BETP_MAKE_MSG_ID(215, 6)
#define OSP_GNSS_UNAVAILABLE BETP_MAKE_MSG_ID(69, 4)

#define BETP_FRAME_START_INDICATOR0 (0xA0)
#define BETP_FRAME_START_INDICATOR1 (0xA3)
#define BETP_FRAME_END_INDICATOR0 (0xB0)
#define BETP_FRAME_END_INDICATOR1 (0xB3)
#define BETP_FRAME_PREAMBLE_SIZE (2)
#define BETP_FRAME_POSTAMBLE_SIZE (2)
#define BETP_FRAME_CHECKSUM_SIZE (4)
#define BETP_FRAME_HEADER_SIZE (6)

#define GNSS_STATUS_MAX_SVS 64

typedef enum
{
   PARSER_COMMON = 0,
   PARSER_BOOT,
   PARSER_OSP,
   PARSER_TGT,
   PARSER_RAW,
   PARSER_MEI,
   NUM_SIRFPROTO_PARSERS
} BetpParserTypes_t;

typedef struct
{
   uint16_t length;
   uint16_t flags;
   uint16_t msgid;
} BetpHeader_t;

typedef struct tTARGET_POS_REQ_ACK
{
   uint8_t requesting_ID; 
} tTARGET_POS_REQ_ACK;


typedef struct tSIRF_MSG_SSB_SIRF_INTERNAL
{
   uint8_t Level_Change;
   uint8_t DSP;
   uint8_t INTC;
   uint8_t KA;
   uint8_t HWTIMER;
   uint8_t UART;
   uint8_t MEMORY;
   uint8_t PWRCLK;
   uint8_t MISC;
   uint8_t SPI;
   uint8_t CBHANDLER;
   uint8_t CHDEVICE;
   uint8_t INSAMPLE;
   uint8_t AGC;
   uint8_t CLKGEN;
   uint8_t ATXCTRL;
   uint8_t ACQ;
   uint8_t TRACK;
   uint8_t XCOR;
   uint8_t CW;
   uint8_t DGPSTRK;
   uint8_t DRM;
   uint8_t OS;
   uint8_t RESET;
   uint8_t PWRMGRCTRL;
   uint8_t CTRLPLATDATA;
   uint8_t NVM;
   uint8_t HOSTIO;
   uint8_t MITASK;
   uint8_t UIDBG;
   uint8_t PWRMGRLOGIC;
   uint8_t AIDING;
   uint8_t CTRLDATA;
   uint8_t GPSRXCTRL;
   uint8_t ATXCTRLMGR;
   uint8_t VISLIST;
   uint8_t MIGPS;
   uint8_t NAV;
   uint8_t DGPSAPP;
   uint8_t BEP;
   uint8_t SL;
   uint8_t UIGPS;
   uint8_t SSB;
   uint8_t NMEA;
   uint8_t QOS;
   uint8_t SVDATA;
   uint8_t TASKCTRL;
   uint8_t SPISLV;
   uint8_t MEI;
   uint8_t SIF;
   uint8_t ACT;
   uint8_t DUMMY1;
} tSIRF_MSG_SSB_SIRF_INTERNAL;

typedef struct sSIRF_MSG_POS_REQ
{
   uint32_t time_btw_fixes_ms;
   uint8_t output_settings;
   uint16_t status_output;
   uint16_t meas_output;
   uint8_t id;
   uint8_t flags;
   uint8_t src;
   uint8_t reserved;
} tSIRF_MSG_POS_REQ;

typedef struct sSIRF_MSG_SSB_PS_POS_RSP
{
   uint8_t id;
   uint8_t reserved_1;
   uint8_t src;
   uint8_t reserved_2;
   uint8_t pos_result_flags;
   uint8_t clk_cor_utc_off;
   uint16_t gps_week;
   uint32_t gps_time_of_week_ms;
   int32_t lat;
   int32_t lon;
   int32_t altitude;
   uint16_t accuracy;
   uint16_t velocity_horizontal;
   uint16_t velocity_heading;
   uint8_t sources_used;
   uint16_t num_remaining;
   uint16_t alt_acc;
   uint16_t hor_vel_acc;
   uint16_t head_acc;
} tSIRF_MSG_SSB_PS_POS_RSP;

#ifdef GNSS_ORG_SV_MSG
typedef struct sTARGET_GNSS_SV_STATUS_REPORT_GNSS_SV_LIST
{
   uint8_t svid;
   uint8_t constellation;
   uint8_t flags;
   uint8_t reserved_1;
   float c_n0_dbhz;
   float elevation;
   float azimuth;
   float carrier_frequency_hz;
   char reserved1[8];
} tTARGET_GNSS_SV_STATUS_REPORT_GNSS_SV_LIST;

typedef struct sTARGET_GNSS_SV_STATUS_REPORT
{
   uint8_t id;
   uint8_t reserved_1;
   uint8_t src;
   uint8_t reserved_2;
   char reserved1[32];
   uint8_t num_svs;
   tTARGET_GNSS_SV_STATUS_REPORT_GNSS_SV_LIST gnss_sv_list[GNSS_STATUS_MAX_SVS];
} tTARGET_GNSS_SV_STATUS_REPORT;
#endif
typedef struct sTARGET_GNSS_SV_STATUS_REPORT_LIGHT_GNSS_SV_LIST
{
   uint8_t svid;
   uint8_t constellation;
   uint8_t flags;
   uint8_t c_n0_dbhz;
} tTARGET_GNSS_SV_STATUS_REPORT_LIGHT_GNSS_SV_LIST;

typedef struct sTARGET_GNSS_SV_STATUS_REPORT_LIGHT
{
   uint8_t id;
   uint8_t src;
   uint8_t num_svs;
   tTARGET_GNSS_SV_STATUS_REPORT_LIGHT_GNSS_SV_LIST gnss_sv_list[GNSS_STATUS_MAX_SVS];
} tTARGET_GNSS_SV_STATUS_REPORT_LIGHT;

typedef struct sSIRF_MSG_SSB_GEODETIC_NAVIGATION
{
    uint16_t nav_valid;
    char reserved1[4];
    uint32_t gps_tow;
    char reserved2[24];
    int32_t alt_msl;
    char reserved3[52];
} tSIRF_MSG_SSB_GEODETIC_NAVIGATION;

/* OSP_GNSS_UNAVAILABLE (69,4)
 *
 * Notification that the device is in a GNSS denied environment. This message
 * is routed as part of a position request and can therefore be routed to the
 * non-standard output (ie. context hub).
 *
 * unavailable:
 * 0 - To inicate that we consider it avalible again (only after it first has gone unavalible).
 * 1 - GNSS is now unavalible.
 *
 */
typedef struct sOSP_GNSS_UNAVAILABLE
{
   uint8_t unavailable;
} tOSP_GNSS_UNAVAILABLE;

/* OSP_PAUSE_GEOFENCE_OPERATION (16,5)
 *
 * Pause/Resume all geofence operations.
 * This is an additional pause status applied on top of the already existing
 * PAUSE status for each indidual fence. This means that if a fence is paused,
 * then pausing and unpausing geofence operations as a whole will not affect
 * the induvidual fence pause status (it will still remain paused when this is
 * resumed again).
 *
 * pause:
 * 0 - (default) geofencing operations are not paused (however induvidual fences can still be paused).
 * 1 - pauses all geofencing operations (note, induvidual fences might be deliberitly unpaused but this will override that).
*/
typedef struct sOSP_PAUSE_GEOFENCE_OPERATION
{
   uint8_t pause;
} tOSP_PAUSE_GEOFENCE_OPERATION;

/* OSP_CONTEXT_AIDING (215,6)
 *
 * Context:
 * CONTEXT_STATUS_UNKNOWN     = 0,
 * CONTEXT_STATUS_NOT_MOVING  = 1,
 * CONTEXT_STATUS_MOVING      = 2,
 * CONTEXT_STATUS_WALKING     = 3,
 * CONTEXT_STATUS_DRIVING     = 4,
 * CONTEXT_STATUS_RUNNING     = 5,
 * CONTEXT_STATUS_FROZEN      = 6,
 * CONTEXT_STATUS_BIKING      = 7,
 * CONTEXT_STATUS_SWIMMING    = 8
 *
 * Confidence:
 * Level of confidence, value is integer in range 0 to 100 % .
 *
 */
typedef struct sOSP_CONTEXT_AIDING
{
   uint16_t msg_ver;
   uint8_t context;
   uint8_t confidence;
   uint16_t gps_week;
   uint32_t gps_tow;
   uint32_t length_of_sleep;
   uint8_t source;
} tOSP_CONTEXT_AIDING;

/**
 * Raw Sensor Measurements
 *
 * Timeoffset is time since the timestamp found in the RAW_SENSOR_SAMPLES
 * container.
 *
 * Maximum supported sample set is 6. This allows a 3 axis sensor to relay
 * measurements and error estimates.
 */
typedef struct sSENSOR_MEASUREMENTS
{
   uint8_t timeoffset;
   uint8_t count;
   int32_t *msrmnt;
} tSENSOR_MEASUREMENTS;

/**
 * Raw Sensor Readings
 *
 * timestamp in nanoseconds
 *
 * timeref { 0=SYSTEM_CLOCK }
 *
 * sensor naming convension: [ACL|MAG|GYRO|BARO|TEMP]_[R:raw|C:calibrated]<data elements per measurement reading>
 * sensor { 0=ACL_R3, 1=MAG_R3, 2=GYRO_R3, 4=BARO_R1, 5=TEMP_R1 }
 *
 * Maximum supported sample set is 32.
 */

/**
 * GNSS Autotest
 *
 * mode = PLATFORM_TEST_CMD_CHUB (41)
 *
 * option = 0 => high-power position request
 *        = 1 => low-power position request
 *        = 2 => low-power position request with time-only responses
 *        = 3 => cancel position request
 * datrnum    => NULL
 *
 * option = 4 => send context message
 * datrnum[0] => GnssContextStatus
 *        [1] => GnssContextConfidence
 */
 
#define PLATFORM_TEST_CMD_CHUB (41)

typedef enum{
  GNSS_AUTOTEST_CMD_HIGH_PWR_POS_REQ            = 0,
  GNSS_AUTOTEST_CMD_LOW_PWR_POS_REQ             = 1,
  GNSS_AUTOTEST_CMD_LOW_PWR_POS_REQ_TIME_ONLY   = 2,
  GNSS_AUTOTEST_CMD_CANCEL_POS_REQ              = 3,
  GNSS_AUTOTEST_CMD_CONTEXT                     = 4,
} GNSS_AUTOTEST_COMMAND;

typedef struct sSIRF_MSG_SSB_PLATFORM_TEST
{
   uint16_t mode;
   uint16_t option;
   uint32_t datrnum[4];
} tSIRF_MSG_SSB_PLATFORM_TEST;

int betpWrapper(uint32_t msgid, void* message, int length);
int betpUnwrapper(void* message, int length);

#endif
#endif /* __GNSS_PROTOCOL_H__ */
