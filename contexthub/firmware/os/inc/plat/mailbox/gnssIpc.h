#ifndef __GNSS_IPC_H__
#define __GNSS_IPC_H__

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

#if defined(MBGNSS_REQUIRED)

#include <stdint.h>
#include "gnssProtocol.h"
#include "gnssApiCHub.h"

#define GNSS_CHUB_ICD_VERSION (1)
#define GNSS_BETP_MSG_MAX_LENGTH  (320)
#define GNSS_IPC_MSG_MAX_LENGTH (GNSS_BETP_MSG_MAX_LENGTH + BETP_FRAME_PREAMBLE_SIZE + BETP_FRAME_POSTAMBLE_SIZE + BETP_FRAME_CHECKSUM_SIZE + BETP_FRAME_HEADER_SIZE + 4)


typedef struct sGnssStatus
{
   GNSSLocationCallback passiveCb;
   GNSSLocationCallback locationCb;
   GNSSStatusCallback svStatusCb;
   GNSSStatusLightCallback svStatusLightCb;
   GNSSUnavailCallback unavailableCb;
   int rate;
   double altMsl;
   uint32_t mslGpsTowSync;
   bool active;
   bool passive;
   bool singleFix;
   bool enableUnavailable;
   bool enableGeofence;
   bool enableTimeOnly;
} tGnssStatus;

extern tGnssStatus gnssStatus;

/* mailbox interrupt callback for shmipc updates */
void gnssIpcMsgCallback(void);

/* */
void gnssApiCHubInit(void);

/* */
void gnssApiCHubDeinit(void);

/* */
void rxmsg_position(tGnssStatus *info, int32_t msgid, void *message, int length);

#ifdef GNSS_ORG_SV_MSG
/* */
void rxmsg_svstatus(tGnssStatus *info, int32_t msgid, void *message, int length);
#endif

/* */
void rxmsg_svstatus_light(tGnssStatus *info, int32_t msgid, void *message, int length);

/* */
void rxmsg_unavailable(tGnssStatus *info, int32_t msgid, void *message, int length);

/* */
void rxmsg_geonav(tGnssStatus *info, int32_t msgid, void *message, int length);

/* */
void rxmsg_autotest(tGnssStatus *info, int32_t msgid, void *message, int length);

/* */
bool sendLocationReq(uint8_t id, int32_t rate, bool passive, bool availibility, bool power, bool timeonly);

#ifdef GNSS_ORG_SV_MSG
/* */
void sortByCN0(tTARGET_GNSS_SV_STATUS_REPORT *pStatRsp, int sortedIdx[]);
#endif

/* */
void sortByCN0_uint8(tTARGET_GNSS_SV_STATUS_REPORT_LIGHT *pStatRsp, int sortedIdx[]);

/* */
uint64_t convertToGPSTime(uint16_t gps_week, uint32_t gps_time_of_week_ms);

/* */
//void report_gnss_error(const char *err);

#endif
#endif /* __GNSS_IPC_H__ */
