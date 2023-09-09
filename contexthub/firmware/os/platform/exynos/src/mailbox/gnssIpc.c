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

#include "gnssApiCHub.h"
#include "gnssIpc.h"
#include "gnssShmipc.h"

#include <csp_common.h>
#include <csp_printf.h>
#include <string.h>
#include <string.h>
#include <mailboxGnssDrv.h>
#include <stddef.h>
#include <stdlib.h>

#define GNSS_LOCATION_ID (0)
#define GNSS_UNAVAILABLE_ID (1)

tGnssStatus gnssStatus;
tSENSOR_MEASUREMENTS * memsdata;

#define gnssCHub_callback(cb_name, ...) \
   do \
   { \
      if (cb_name) \
      { \
         cb_name(__VA_ARGS__); \
      } \
   } while (0)

//for Type Conversion
#define PI                                         (3.1415926535898)
#define DEG_PER_RAD                                (180.0 / PI)
#define LATITUDE_SCALING                           (DEG_PER_RAD/0.00000004190951585770)
#define LONGITUDE_SCALING                          (DEG_PER_RAD/0.00000008381903171539)
#define SIRF_MSG_SSB_PS_POS_RSP_LAT_TO_DOUBLE(lat) (lat * (double)0.00000004190951585770)
#define SIRF_MSG_SSB_PS_POS_RSP_LON_TO_DOUBLE(lon) (lon * (double)0.00000008381903171539)
/* Unix time epoch (01 Jan 1970) and GPS time epoch (06 Jan 1980) is 315964800 */
#define GPS_TIME_CONSTANT                          (315964800)
#define SECONDS_IN_WEEK                            (604800)
#define SECONDS_TO_NANOSECONDS                     (1000000000)
#define MILISECONDS_TO_NANOSECONDS                 (1000000)
#define SECONDS_TO_MILISECONDS                     (1000)

/** This is called by the CHUB Mailbox interrupt handler */
void gnssIpcMsgCallback(void)
{
   int msgid = 0;
   uint32_t length = GNSS_BETP_MSG_MAX_LENGTH;
   char message[GNSS_BETP_MSG_MAX_LENGTH] = {0, };

   while (0 < (msgid = betpUnwrapper(message, length)))
   {
      switch (msgid)
      {
         case BETP_SIRF_MSG_SSB_PS_POS_RSP:
            rxmsg_position(&gnssStatus, msgid, message, length);
            break;
#ifdef GNSS_ORG_SV_MSG
         case BETP_TARGET_GNSS_SV_STATUS_REPORT:
            rxmsg_svstatus(&gnssStatus, msgid, message, length);
            break;
#endif
         case BETP_TARGET_GNSS_SV_STATUS_REPORT_LIGHT:
            rxmsg_svstatus_light(&gnssStatus, msgid, message, length);
            break;
         case OSP_GNSS_UNAVAILABLE:
            rxmsg_unavailable(&gnssStatus, msgid, message, length);
            break;
         case BETP_SIRF_MSG_SSB_GEODETIC_NAVIGATION:
            rxmsg_geonav(&gnssStatus, msgid, message, length);
            break;
         case BETP_TARGET_POS_REQ_ACK:
            CSP_PRINTF_INFO("[nanohub]GNSS POS REQ ACK!!\n");
            break;
         case BETP_SIRF_MSG_SSB_PLATFORM_TEST:  /* GNSS-CHUB Autotest : TEST Purpose Only */
            rxmsg_autotest(&gnssStatus, msgid, message, length);
            break;   
         default:
            //record unknown msgids
            break;
      }
   }
   return;
}

uint64_t convertToGPSTime(uint16_t gps_week, uint32_t gps_time_of_week_ms)
{
   /*Convert to GPS time since Jan 6 1980*/
   uint64_t nanoGPSTime = 0;
   nanoGPSTime = ((uint64_t)GPS_TIME_CONSTANT) * SECONDS_TO_NANOSECONDS;
   nanoGPSTime += ((uint64_t) gps_week) * SECONDS_IN_WEEK * SECONDS_TO_NANOSECONDS;
   nanoGPSTime += ((uint64_t) gps_time_of_week_ms) * MILISECONDS_TO_NANOSECONDS;
   return nanoGPSTime;
}

bool sendLocationReq(uint8_t id, int32_t rate, bool passive, bool availability, bool power, bool timeonly)
{
   int result = 0;
   tSIRF_MSG_POS_REQ stopLocUpdate;
   stopLocUpdate.reserved = 0;
   stopLocUpdate.src = 1;
   stopLocUpdate.meas_output = 0x0;
   stopLocUpdate.flags = 0x01;

   if (availability)
   {
      stopLocUpdate.status_output = 0x18;
      stopLocUpdate.output_settings = 0x0;
   }
   else
   {
      //watch : 0x202 for full SV status, mobile : 0xA00 for Light SV status
      stopLocUpdate.status_output = 0xA00;
      stopLocUpdate.output_settings = 0x1;
   }

   stopLocUpdate.time_btw_fixes_ms = rate * SECONDS_TO_MILISECONDS;
   stopLocUpdate.id = id;

   stopLocUpdate.flags = (timeonly << 0x2) | (passive << 0x1) | !power;
   result = betpWrapper( BETP_SIRF_MSG_POS_REQ, &stopLocUpdate, sizeof(tSIRF_MSG_POS_REQ) );

   CSP_PRINTF_INFO("[ %s ] id:%u rate:%li passive:%u avail:%u power:%u timeonly:%u\n", __func__, id, rate, passive, availability, power, timeonly);
   return (result > 0);
}

uint32_t gnssChubIcdVersion(void)
{
   return GNSS_CHUB_ICD_VERSION;
}

void gnssRegisterLocationCallback(GNSSLocationCallback cb)
{
   gnssStatus.locationCb = cb;
}

void gnssRegisterStatusCallback(GNSSStatusCallback cb)
{
   gnssStatus.svStatusCb = cb;
}

void gnssRegisterStatusLightCallback(GNSSStatusLightCallback cb)
{
   gnssStatus.svStatusLightCb = cb;
}


void gnssRegisterPassiveCallback(GNSSLocationCallback cb)
{
   gnssStatus.passiveCb = cb;
}

void gnssRegisterUnavailableCallback(GNSSUnavailCallback cb)
{
   gnssStatus.unavailableCb = cb;
}

void gnssClearLocationCallback(void)
{
   gnssStatus.locationCb = NULL;
}

void gnssClearStatusCallback(void)
{
   gnssStatus.svStatusCb = NULL;
}

void gnssClearStatusLightCallback(void)
{
   gnssStatus.svStatusLightCb = NULL;
}

void gnssClearPassiveCallback(void)
{
   gnssStatus.passiveCb = NULL;
}

void gnssClearUnavailableCallback(void)
{
   gnssStatus.unavailableCb = NULL;
}

bool gnssRequestLocationUpdates(int32_t rate, bool power)
{
   bool result = false;

   if (gnssStatus.locationCb == NULL)
      return false;

   if (rate < 0)
      return false;

   gnssStatus.singleFix = (rate == 0);
   gnssStatus.active = true;
   gnssStatus.rate = rate;
   gnssStatus.altMsl = 0;

   if (gnssStatus.singleFix)
      result = sendLocationReq(GNSS_LOCATION_ID, 1, false, false, power, gnssStatus.enableTimeOnly);
   else
      result = sendLocationReq(GNSS_LOCATION_ID, gnssStatus.rate, false, false, power, gnssStatus.enableTimeOnly);

   return result;
}

bool gnssStopLocationUpdates(void)
{
   bool result = false;

   gnssStatus.singleFix = false;
   gnssStatus.active = false;
   gnssStatus.rate = 0;
   gnssStatus.altMsl = 0;

   result = sendLocationReq(GNSS_LOCATION_ID, gnssStatus.passive, gnssStatus.passive, false, false, gnssStatus.enableTimeOnly);

   return result;
}

bool gnssSetPassiveLocation(bool enable)
{
   bool result = false;

   if (gnssStatus.passiveCb == NULL)
      return false;

   gnssStatus.passive = enable;

   if (!gnssStatus.active)
      result = sendLocationReq(GNSS_LOCATION_ID, gnssStatus.passive, gnssStatus.passive, false, false, gnssStatus.enableTimeOnly);

   return result;
}

bool gnssSetUnavailableUpdates(bool enable)
{
   bool result = false;

   if (gnssStatus.unavailableCb == NULL)
      return false;

   result = sendLocationReq(GNSS_UNAVAILABLE_ID, enable, enable, enable, false, gnssStatus.enableTimeOnly);

   if (result)
   {
      gnssStatus.enableUnavailable = enable;
   }

   return result;
}

bool gnssSetGeofence(bool enable)
{
   int result = 0;
   tOSP_PAUSE_GEOFENCE_OPERATION msg;

   if (enable && gnssStatus.enableUnavailable)
   {
      gnssSetUnavailableUpdates(true);
   }

   msg.pause = !enable;
   result = betpWrapper(OSP_PAUSE_GEOFENCE_OPERATION, &msg, sizeof(tOSP_PAUSE_GEOFENCE_OPERATION));

   if (result > 0)
   {
      gnssStatus.enableGeofence = enable;
   }

   return (result > 0);
}

bool gnssSetLogging(bool enable)
{
   bool result = false;
   tSIRF_MSG_SSB_SIRF_INTERNAL msg = {
      0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //16
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //32
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //48
      1, 1, 1, 1 }; //52

   msg.Level_Change = (enable) ? 0 : 2;

   result = (betpWrapper(BETP_SIRF_MSG_SSB_SIRF_INTERNAL, &msg, sizeof(tSIRF_MSG_SSB_SIRF_INTERNAL)) > 0);

   return result;
}

bool gnssSetContextStatus(GnssContextStatus context, GnssContextConfidence confidence)
{
   bool result = 0;
   tOSP_CONTEXT_AIDING msg;
   msg.context = context;

   if (context >= GNSS_CTX_STATUS_COUNT)
      return false;

   switch (confidence)
   {
      case GNSS_CTX_CONF_UNKNOWN:
         msg.confidence = 0;
         break;
      case GNSS_CTX_CONF_LOW:
         msg.confidence = 25;
         break;
      case GNSS_CTX_CONF_MED:
         msg.confidence = 50;
         break;
      case GNSS_CTX_CONF_HIGH:
         msg.confidence = 95;
         break;
      default:
//         CSP_PRINTF_INFO("[ %s ] Invalid GnssContextConfidence.\n", __func__);
         return false;
         break;
   }

   result = (betpWrapper(OSP_CONTEXT_AIDING, &msg, sizeof(tOSP_CONTEXT_AIDING)) > 0);

   return result;
}

bool gnssUpdateRawSensorMeasurement(GnssMemsType sensorId, uint64_t time, int32_t *data)
{
   return false;
}

void gnssApiCHubInit(void)
{
   gnssStatus.passiveCb = NULL;
   gnssStatus.locationCb = NULL;
   gnssStatus.svStatusCb = NULL;
   gnssStatus.svStatusLightCb = NULL;
   gnssStatus.unavailableCb = NULL;
   gnssStatus.singleFix = false;
   gnssStatus.passive = false;
   gnssStatus.enableGeofence = true;
   gnssStatus.enableUnavailable = false;
   gnssStatus.enableTimeOnly = true;

   //memsdata = gnssAlloc((sizeof() + (sizeof() + sizeof(int32_t) * 6) * 32) * 5);
   shmipc_init();
   
   return;
}

void gnssApiCHubDeinit(void)
{
   return;
}

#ifdef GNSS_ORG_SV_MSG
void sortByCN0(tTARGET_GNSS_SV_STATUS_REPORT *pStatRsp, int Idx[])
{
   int i = 0;
   int j = 0;
   float max = 0.0;
   int maxIndex = 0;
   int temp = 0;
   int svNum = pStatRsp->num_svs;

   for (i = 0; i < svNum; i++)
   {
      Idx[i] = i;
   }

   if(svNum>=1)
   {
      for (i = 0; i< (pStatRsp->num_svs); i++)
      {
         for (j = i; j<pStatRsp->num_svs; j++)
         {
            if (max < pStatRsp->gnss_sv_list[Idx[j]].c_n0_dbhz)
            {
               max = pStatRsp->gnss_sv_list[Idx[j]].c_n0_dbhz;
               maxIndex = j;
            }
         }
         temp = Idx[i];
         Idx[i] = Idx[maxIndex];
         Idx[maxIndex] = temp;
         max = 0.0;
      }
   }
}
#endif
void sortByCN0_uint8(tTARGET_GNSS_SV_STATUS_REPORT_LIGHT *pStatRsp, int Idx[])
{
   int i = 0;
   int j = 0;
   uint8_t max = 0;
   int maxIndex = 0;
   int temp = 0;
   int svNum = pStatRsp->num_svs;

   for (i = 0; i < svNum; i++)
   {
      Idx[i] = i;
   }

   if(svNum>=1)
   {
      for (i = 0; i< (pStatRsp->num_svs); i++)
      {
         for (j = i; j<pStatRsp->num_svs; j++)
         {
            if (max < pStatRsp->gnss_sv_list[Idx[j]].c_n0_dbhz)
            {
               max = pStatRsp->gnss_sv_list[Idx[j]].c_n0_dbhz;
               maxIndex = j;
            }
         }
         temp = Idx[i];
         Idx[i] = Idx[maxIndex];
         Idx[maxIndex] = temp;
         max = 0;
      }
   }
}

/*
void report_gnss_error(const char *err)
{
   return;
}
*/

void rxmsg_position(tGnssStatus *info, int32_t msgid, void *message, int length)
{
   if (info != NULL && message != NULL)
   {
      GNSSLocation pPos;
      bool bUpdate = false;
      tSIRF_MSG_SSB_PS_POS_RSP *pPosRsp = (tSIRF_MSG_SSB_PS_POS_RSP *) message;

      if ((pPosRsp->pos_result_flags & 0x2F) == 0x2F)
      {
         bUpdate = true;
         pPos.lat = SIRF_MSG_SSB_PS_POS_RSP_LAT_TO_DOUBLE((double)(pPosRsp->lat));
         pPos.lon = SIRF_MSG_SSB_PS_POS_RSP_LON_TO_DOUBLE((double)(pPosRsp->lon));
         pPos.altitude = (double) (pPosRsp->altitude) / (double) 10.0;
         pPos.velocity_horizontal = (double) (pPosRsp->velocity_horizontal) / (double) 100.0;
         pPos.velocity_heading = (double) (pPosRsp->velocity_heading) / (double) 10.0;
         pPos.accuracy = pPosRsp->accuracy;
         pPos.velocity_accuracy = (float) (pPosRsp->sources_used) / 50.0f;
         
         if(pPosRsp->gps_time_of_week_ms == info->mslGpsTowSync){
            pPos.alt_msl = info->altMsl;
         } else{
           pPos.alt_msl = 0;
         }

         if(pPosRsp->pos_result_flags & 0x10)
         {
            pPos.precise_accuracy = 1;
         } else
         {
            pPos.precise_accuracy = 0;
         }
         CSP_PRINTF_INFO("[ %s ] pPosRsp->pos_result_flags[%d], pPos.alt_msl [%lf]\n", __func__, pPosRsp->pos_result_flags, pPos.alt_msl);

         if (info->singleFix)
         {
            gnssStopLocationUpdates();
         }
      }
      else if ((pPosRsp->pos_result_flags & 0x01) == 0x01)
      {
         bUpdate = true;
         pPos.lat = (double) 0xFF;
         pPos.lon = (double) 0xFF;
      }

      if(bUpdate)
      {
         pPos.time.gps_time = convertToGPSTime(pPosRsp->gps_week, pPosRsp->gps_time_of_week_ms);
         if ((pPosRsp->pos_result_flags & 0x40) == 0x40)
         {     
            pPos.time.flags = GNSS_TIME_FLAGS_HAS_UTC_OFFSET;
            pPos.time.utc_offset = pPosRsp->clk_cor_utc_off;
         } else
         {
            pPos.time.flags = 0;
            pPos.time.utc_offset = 0;
         }

         if (info->active)
         {
            gnssCHub_callback(info->locationCb, &pPos);
         }
         else if (info->passive)
         {
            gnssCHub_callback(info->passiveCb, &pPos);
         }
         CSP_PRINTF_INFO("[ %s ] flag[%d] gps[%lld], utc[%d]\n", __func__, pPosRsp->pos_result_flags, pPos.time.gps_time, (int) pPos.time.utc_offset);
      }
   }
}

#ifdef GNSS_ORG_SV_MSG
void rxmsg_svstatus(tGnssStatus *info, int32_t msgid, void *message, int length)
{
   if (info->svStatusCb != NULL && message != NULL)
   {
      int i = 0;
      int sortedIdx[GNSS_STATUS_MAX_SVS];
      GNSSStatus pStat;
      tTARGET_GNSS_SV_STATUS_REPORT *pStatRsp = (tTARGET_GNSS_SV_STATUS_REPORT *) message;
      sortByCN0(pStatRsp, sortedIdx);
      pStat.num_svs = 0;
      for (i = 0; i < pStatRsp->num_svs; i++)
      {
         if (pStatRsp->gnss_sv_list[sortedIdx[i]].c_n0_dbhz > 0)
         {
            pStat.sat[pStat.num_svs].cno_dbhz= pStatRsp->gnss_sv_list[sortedIdx[i]].c_n0_dbhz;
            pStat.sat[pStat.num_svs].svid = pStatRsp->gnss_sv_list[sortedIdx[i]].svid;
            pStat.sat[pStat.num_svs].constellation = pStatRsp->gnss_sv_list[sortedIdx[i]].constellation;
            pStat.sat[pStat.num_svs].elevation = pStatRsp->gnss_sv_list[sortedIdx[i]].elevation;
            pStat.sat[pStat.num_svs].azimuth = pStatRsp->gnss_sv_list[sortedIdx[i]].azimuth;
            pStat.sat[pStat.num_svs].flags = pStatRsp->gnss_sv_list[sortedIdx[i]].flags;
            pStat.num_svs++;
         }
      }
      gnssCHub_callback(info->svStatusCb, &pStat);
   }
}
#endif
void rxmsg_svstatus_light(tGnssStatus *info, int32_t msgid, void *message, int length)
{
   if (info->svStatusLightCb != NULL && message != NULL)
   {
      int i = 0;
      int sortedIdx[GNSS_STATUS_MAX_SVS];
      GNSSStatusLight pStat;
      tTARGET_GNSS_SV_STATUS_REPORT_LIGHT *pStatRsp = (tTARGET_GNSS_SV_STATUS_REPORT_LIGHT *) message;
      sortByCN0_uint8(pStatRsp, sortedIdx);
      pStat.num_svs = 0;
      for (i = 0; i < pStatRsp->num_svs; i++)
      {
         if (pStatRsp->gnss_sv_list[sortedIdx[i]].c_n0_dbhz > 0)
         {
            pStat.sat[pStat.num_svs].cno_dbhz= pStatRsp->gnss_sv_list[sortedIdx[i]].c_n0_dbhz;
            pStat.sat[pStat.num_svs].svid = pStatRsp->gnss_sv_list[sortedIdx[i]].svid;
            pStat.sat[pStat.num_svs].constellation = pStatRsp->gnss_sv_list[sortedIdx[i]].constellation;
            pStat.sat[pStat.num_svs].flags = pStatRsp->gnss_sv_list[sortedIdx[i]].flags;
            pStat.num_svs++;
         }
      }
      gnssCHub_callback(info->svStatusLightCb, &pStat);
   }
}

void rxmsg_unavailable(tGnssStatus *info, int32_t msgid, void *message, int length)
{
   if (info->unavailableCb != NULL && message != NULL)
   {
      tOSP_GNSS_UNAVAILABLE *msg = (tOSP_GNSS_UNAVAILABLE *) message;
      gnssCHub_callback(info->unavailableCb, msg->unavailable);
   }
}

void rxmsg_geonav(tGnssStatus *info, int32_t msgid, void *message, int length)
{
   tSIRF_MSG_SSB_GEODETIC_NAVIGATION *pGeoNav = (tSIRF_MSG_SSB_GEODETIC_NAVIGATION *) message;
   if(!(pGeoNav->nav_valid && 0x1)){
      info->altMsl = (double) pGeoNav->alt_msl / (double) 100.0;
      info->mslGpsTowSync = pGeoNav->gps_tow;
      CSP_PRINTF_INFO("[ %s ] alt_msl[%lf], gps_tow[%d]\n", __func__, info->altMsl, (int) info->mslGpsTowSync);
   } else{
      info->altMsl = 0;
      CSP_PRINTF_INFO("[ %s ] Position was not fixed yet. Invalid alt_msl\n", __func__);
   }
}

/* GNSS-CHUB Autotest : TEST Purpose Only */
static void locationCB(GNSSLocation* location)
{
   CSP_PRINTF_INFO("[GNSSAUTO][ %s ] lat[%lf], lon[%lf], time[%lld]\n", __func__, location->lat, location->lon, location->time.gps_time);
}

static void statusCB(GNSSStatus* status)
{
   CSP_PRINTF_INFO("[GNSSAUTO][ %s ] SV num [%d]\n", __func__, (int)status->num_svs);
}

static void statusLightCB(GNSSStatusLight* status)
{
   CSP_PRINTF_INFO("[GNSSAUTO][ %s ] SV num [%d]\n", __func__, (int)status->num_svs);
   for(int i = 0; i< (int)status->num_svs; i++)
   {
	   CSP_PRINTF_INFO("[GNSSAUTO][ %s ] SV CN0 [%d][%d]\n", __func__, i, (int)status->sat[i].cno_dbhz);
   }
}

static void autotest_registerCB(GNSSLocationCallback locCB, GNSSStatusCallback statCB, GNSSStatusLightCallback statLightCB)
{
   gnssRegisterLocationCallback(locCB);
   gnssRegisterStatusCallback(statCB);
   gnssRegisterStatusLightCallback(statLightCB);
}

static void autotest_unregisterCB()
{
   gnssClearLocationCallback();
   gnssClearStatusCallback();
   gnssClearStatusLightCallback();
}

void rxmsg_autotest(tGnssStatus *info, int32_t msgid, void *message, int length)
{
   tSIRF_MSG_SSB_PLATFORM_TEST *pAutoTest = (tSIRF_MSG_SSB_PLATFORM_TEST *) message;
   if(pAutoTest->mode != PLATFORM_TEST_CMD_CHUB) {
      CSP_PRINTF_INFO("[GNSSAUTO][ %s ] Not a CHUB TEST Mode [%d, %d]\n", __func__, pAutoTest->mode, pAutoTest->option);
      return;
   }
   CSP_PRINTF_INFO("[GNSSAUTO][ %s ] Test Option [ %d ]\n", __func__, pAutoTest->option);

   switch(pAutoTest->option)
   {
      case GNSS_AUTOTEST_CMD_HIGH_PWR_POS_REQ:
         autotest_registerCB(locationCB, statusCB, statusLightCB);
         gnssStatus.enableTimeOnly = false;
         gnssRequestLocationUpdates(1, true);
         break;
      case GNSS_AUTOTEST_CMD_LOW_PWR_POS_REQ:
         autotest_registerCB(locationCB, statusCB, statusLightCB);
         gnssStatus.enableTimeOnly = false;
         gnssRequestLocationUpdates(1, false);
         break;
      case GNSS_AUTOTEST_CMD_LOW_PWR_POS_REQ_TIME_ONLY:
         autotest_registerCB(locationCB, statusCB, statusLightCB);
         gnssStatus.enableTimeOnly = true;
         gnssRequestLocationUpdates(1, false);
         break;
      case GNSS_AUTOTEST_CMD_CANCEL_POS_REQ:
         autotest_unregisterCB();
         gnssStopLocationUpdates();
         break;
      case GNSS_AUTOTEST_CMD_CONTEXT:
         gnssSetContextStatus((GnssContextStatus)pAutoTest->datrnum[0], (GnssContextConfidence)pAutoTest->datrnum[1]);
         break;
      default:
         CSP_PRINTF_INFO("[GNSSAUTO][ %s ] Unknown test option\n", __func__);
         break;
   }
}
