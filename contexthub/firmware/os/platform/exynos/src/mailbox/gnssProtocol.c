#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "gnssProtocol.h"
#include "gnssShmipc.h"
#include "csp_printf.h"
#include <gnssIpc.h>

uint8_t unpackUint8(char **p);
uint16_t unpackUint16(char **p);
uint32_t unpackUint32(char **p);
int16_t unpackInt16(char **p);
int32_t unpackInt32(char **p);
float unpackFloat(char **p);

int packUint8(char **p, uint8_t d);
int packUint16(char **p, uint16_t d);
int packUint32(char **p, uint32_t d);
int packInt32(char **p, int32_t d);
int packFloat(char **p, float d);

void *systemAllocation(int length);
void systemDeallocation(void *p);

int packLoggingReq(char *p, tSIRF_MSG_SSB_SIRF_INTERNAL *msg);
int packPosReq(char *p, tSIRF_MSG_POS_REQ *msg);

int packPauseGeofenseOperation(char *p, tOSP_PAUSE_GEOFENCE_OPERATION *msg);
int packContextAiding(char *p, tOSP_CONTEXT_AIDING *msg);
int packGnssUnavailable(char *p, tOSP_GNSS_UNAVAILABLE *msg);

int unpackGnssPosReqACK(tTARGET_POS_REQ_ACK *msg, char *p);
int unpackAutotest(tSIRF_MSG_SSB_PLATFORM_TEST *msg, char *p);

int unpackPosReq(tSIRF_MSG_SSB_PS_POS_RSP *msg, char *p);
#ifdef GNSS_ORG_SV_MSG
int unpackSvStatus(tTARGET_GNSS_SV_STATUS_REPORT_GNSS_SV_LIST *msg, char *p);
int unpackSvReport(tTARGET_GNSS_SV_STATUS_REPORT *msg, char *p);
#endif
int unpackSvReportLight(tTARGET_GNSS_SV_STATUS_REPORT_LIGHT *msg, char *p);
int unpackSvStatusLight(tTARGET_GNSS_SV_STATUS_REPORT_LIGHT_GNSS_SV_LIST *msg, char *p);
int unpackGeoNav(tSIRF_MSG_SSB_GEODETIC_NAVIGATION *msg, char *p);


int unpackPauseGeofenseOperation(tOSP_PAUSE_GEOFENCE_OPERATION *msg, char *p);
int unpackContextAiding(tOSP_CONTEXT_AIDING *msg, char *p);
int unpackGnssUnavailable(tOSP_GNSS_UNAVAILABLE *msg, char *p);

void print_bytes(char *buffer, int length);

bool GNSS_PROTOCOL_DEBUG = true;
char alpha[16] = {'0','1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

void print_bytes(char *buffer, int length)
{
   int i;
   int index = 0;
   char printbuffer[65];
   char *b = printbuffer;
   while (index < length)
   {
      int max = (32 < length - index)? 32 : length - index;
      for (i = 0; i < max; i++)
      {
         *b = alpha[buffer[index] >> 4];
         b++;
         *b = alpha[buffer[index] & 0xF];
         index++;
         b++;
         *b = 0;
      }
      //CSP_PRINTF_INFO("[ gnssProtocol ] %s\n", printbuffer);
   }
}

uint8_t unpackUint8(char **p)
{
   char *v = *p;
   *p = *p + 1;
   return (uint8_t)(*v);
}

uint16_t unpackUint16(char **p)
{
   char *v = *p;
   *p = *p + 2;
   return ((uint16_t)(v[0] | v[1]<<8));
}

uint32_t unpackUint32(char **p)
{
   char *v = *p;
   *p = *p + 4;
   return ((uint32_t)(v[0] | v[1]<<8 | v[2]<<16 | v[3]<<24));
}

int16_t unpackInt16(char **p)
{
   char *v = *p;
   *p = *p + 2;
   return ((int16_t)(v[0] | v[1]<<8));
}

int32_t unpackInt32(char **p)
{
   char *v = *p;
   *p = *p + 4;
   return ((int32_t)(v[0] | v[1]<<8 | v[2]<<16 | v[3]<<24));
}

float unpackFloat(char **p)
{
   union Data { int i; float f; } data;
   char *v = *p;
   *p = *p + 4;
   data.i = (v[0] | v[1]<<8 | v[2]<<16 | v[3]<<24);
   return data.f;
}

int packUint8(char **p, uint8_t d)
{
   int i = 1;
   memcpy(*p, &d, i);
   *p = *p + i;
   return i;
}

int packUint16(char **p, uint16_t d)
{
   int i = 2;
   memcpy(*p, &d, i);
   *p = *p + i;
   return i;
}

int packUint32(char **p, uint32_t d)
{
   int i = 4;
   memcpy(*p, &d, i);
   *p = *p + i;
   return i;
}

int packInt32(char **p, int32_t d)
{
   int i = 4;
   memcpy(*p, &d, i);
   *p = *p + i;
   return i;
}

int packFloat(char **p, float d)
{
   int i = 4;
   memcpy(*p, &d, i);
   *p = *p + i;
   return i;
}

/*
void *systemAllocation(int length)
{
   int i = length;
   char *payload = (char*) gnssAlloc(length);
   if (payload)
      while (i--)
         payload[i] = 0;

   return payload;
}

void systemDeallocation(void *payload)
{
   gnssFree(payload);
}
*/

int packLoggingReq(char *p, tSIRF_MSG_SSB_SIRF_INTERNAL *msg)
{
   char *v = p;
   packUint8(&v, msg->Level_Change);
   packUint8(&v, msg->DSP);
   packUint8(&v, msg->INTC);
   packUint8(&v, msg->KA);
   packUint8(&v, msg->HWTIMER);
   packUint8(&v, msg->UART);
   packUint8(&v, msg->MEMORY);
   packUint8(&v, msg->PWRCLK);
   packUint8(&v, msg->MISC);
   packUint8(&v, msg->SPI);
   packUint8(&v, msg->CBHANDLER);
   packUint8(&v, msg->CHDEVICE);
   packUint8(&v, msg->INSAMPLE);
   packUint8(&v, msg->AGC);
   packUint8(&v, msg->CLKGEN);
   packUint8(&v, msg->ATXCTRL);
   packUint8(&v, msg->ACQ);
   packUint8(&v, msg->TRACK);
   packUint8(&v, msg->XCOR);
   packUint8(&v, msg->CW);
   packUint8(&v, msg->DGPSTRK);
   packUint8(&v, msg->DRM);
   packUint8(&v, msg->OS);
   packUint8(&v, msg->RESET);
   packUint8(&v, msg->PWRMGRCTRL);
   packUint8(&v, msg->CTRLPLATDATA);
   packUint8(&v, msg->NVM);
   packUint8(&v, msg->HOSTIO);
   packUint8(&v, msg->MITASK);
   packUint8(&v, msg->UIDBG);
   packUint8(&v, msg->PWRMGRLOGIC);
   packUint8(&v, msg->AIDING);
   packUint8(&v, msg->CTRLDATA);
   packUint8(&v, msg->GPSRXCTRL);
   packUint8(&v, msg->ATXCTRLMGR);
   packUint8(&v, msg->VISLIST);
   packUint8(&v, msg->MIGPS);
   packUint8(&v, msg->NAV);
   packUint8(&v, msg->DGPSAPP);
   packUint8(&v, msg->BEP);
   packUint8(&v, msg->SL);
   packUint8(&v, msg->UIGPS);
   packUint8(&v, msg->SSB);
   packUint8(&v, msg->NMEA);
   packUint8(&v, msg->QOS);
   packUint8(&v, msg->SVDATA);
   packUint8(&v, msg->TASKCTRL);
   packUint8(&v, msg->SPISLV);
   packUint8(&v, msg->MEI);
   packUint8(&v, msg->SIF);
   packUint8(&v, msg->ACT);
   packUint8(&v, msg->DUMMY1);
   return v - p;
}

int packPosReq(char *p, tSIRF_MSG_POS_REQ *msg)
{
   char *v = p;
   packUint32(&v, msg->time_btw_fixes_ms);
   packUint8(&v, msg->output_settings);
   packUint16(&v, msg->status_output);
   packUint16(&v, msg->meas_output);
   packUint8(&v, msg->id);
   packUint8(&v, msg->flags);
   packUint8(&v, msg->src);
   packUint8(&v, msg->reserved);
   return v - p;
}

int packPauseGeofenseOperation(char *p, tOSP_PAUSE_GEOFENCE_OPERATION *msg)
{
   char *v = p;
   packUint8(&v, msg->pause);
   return v - p;
}

int packContextAiding(char *p, tOSP_CONTEXT_AIDING *msg)
{
   char *v = p;
   packUint8(&v, msg->context);
   packUint8(&v, msg->confidence);
   return v - p;
}

int packGnssUnavailable(char *p, tOSP_GNSS_UNAVAILABLE *msg)
{
   char *v = p;
   packUint8(&v, msg->unavailable);
   return v - p;
}

int unpackPosReq(tSIRF_MSG_SSB_PS_POS_RSP *msg, char *p)
{
   char *v = p;
   msg->id = unpackUint8(&v);
   msg->reserved_1 = unpackUint8(&v);
   msg->src = unpackUint8(&v);
   msg->reserved_2 = unpackUint8(&v);
   msg->pos_result_flags = unpackUint8(&v);
   msg->clk_cor_utc_off = unpackUint8(&v);
   msg->gps_week = unpackUint16(&v);
   msg->gps_time_of_week_ms = unpackUint32(&v);
   msg->lat = unpackUint32(&v);
   msg->lon = unpackUint32(&v);
   msg->altitude = unpackUint32(&v);
   msg->accuracy = unpackUint16(&v);
   msg->velocity_horizontal = unpackUint16(&v);
   msg->velocity_heading = unpackUint16(&v);
   msg->sources_used = unpackUint8(&v);
   msg->num_remaining = unpackUint16(&v);
   msg->alt_acc = unpackUint16(&v);
   msg->hor_vel_acc = unpackUint16(&v);
   msg->head_acc = unpackUint16(&v);
   return v - p;
}

#ifdef GNSS_ORG_SV_MSG
int unpackSvStatus(tTARGET_GNSS_SV_STATUS_REPORT_GNSS_SV_LIST *msg, char *p)
{
   char *v = p;
   msg->svid = unpackUint8(&v);
   msg->constellation = unpackUint8(&v);
   msg->flags = unpackUint8(&v);
   v += 1;
   msg->c_n0_dbhz = unpackFloat(&v);
   msg->elevation = unpackFloat(&v);
   msg->azimuth = unpackFloat(&v);
   msg->carrier_frequency_hz = unpackFloat(&v);
   v += 8;
   return v - p;
}

int unpackSvReport(tTARGET_GNSS_SV_STATUS_REPORT *msg, char *p)
{
   int i = 0;
   int remain = 0;
   char *v = p;

   msg->id = unpackUint8(&v);
   msg->reserved_1 = unpackUint8(&v);
   msg->src = unpackUint8(&v);
   msg->reserved_2 = unpackUint8(&v);
   v += 32;
   msg->num_svs = unpackUint8(&v);

   if (msg->num_svs > GNSS_STATUS_MAX_SVS)
   {
     remain = msg->num_svs - GNSS_STATUS_MAX_SVS;
     msg->num_svs = GNSS_STATUS_MAX_SVS;
   }

   for (i = 0; i < msg->num_svs; i++)
   {
      v += unpackSvStatus( &(msg->gnss_sv_list[i]), v);
   }

   if (remain)
   {
     v += remain *16; /*sv status packed size*/
   }

   return v - p;
}
#endif
int unpackSvStatusLight(tTARGET_GNSS_SV_STATUS_REPORT_LIGHT_GNSS_SV_LIST *msg, char *p)
{
   char *v = p;
   msg->svid = unpackUint8(&v);
   msg->constellation = unpackUint8(&v);
   msg->flags = unpackUint8(&v);
   msg->c_n0_dbhz = unpackUint8(&v);
   return v - p;
}

int unpackSvReportLight(tTARGET_GNSS_SV_STATUS_REPORT_LIGHT *msg, char *p)
{
   int i = 0;
   int remain = 0;
   char *v = p;

   msg->id = unpackUint8(&v);
   msg->src = unpackUint8(&v);
   msg->num_svs = unpackUint8(&v);

   if (msg->num_svs > GNSS_STATUS_MAX_SVS)
   {
     remain = msg->num_svs - GNSS_STATUS_MAX_SVS;
     msg->num_svs = GNSS_STATUS_MAX_SVS;
   }

   for (i = 0; i < msg->num_svs; i++)
   {
      v += unpackSvStatusLight( &(msg->gnss_sv_list[i]), v);
   }

   if (remain)
   {
     v += remain *16; /*sv status packed size*/
   }

   return v - p;
}

int unpackPauseGeofenseOperation(tOSP_PAUSE_GEOFENCE_OPERATION *msg, char *p)
{
   char *v = p;
   msg->pause = unpackUint8(&v);
   return v - p;
}

int unpackContextAiding(tOSP_CONTEXT_AIDING *msg, char *p)
{
   char *v = p;
   msg->context = unpackUint8(&v);
   msg->confidence = unpackUint8(&v);
   return v - p;
}

int unpackGnssUnavailable(tOSP_GNSS_UNAVAILABLE *msg, char *p)
{
   char *v = p;
   msg->unavailable = unpackUint8(&v);
   return v - p;
}

int unpackGeoNav(tSIRF_MSG_SSB_GEODETIC_NAVIGATION *msg, char *p)
{
   char *v = p;
   msg->nav_valid = unpackUint16(&v);
   v += 4;
   msg->gps_tow = unpackUint32(&v);
   v += 24;
   msg->alt_msl = unpackInt32(&v);
   v += 56;
   return v - p;
}

int unpackGnssPosReqACK(tTARGET_POS_REQ_ACK *msg, char *p)
{
   char *v = p;
   msg->requesting_ID = unpackUint8(&v);
   return v - p;
}

int unpackAutotest(tSIRF_MSG_SSB_PLATFORM_TEST *msg, char *p)
{
   char *v = p;
   msg->mode = unpackUint16(&v);
   msg->option = unpackUint16(&v);
   for(int i = 0; i < 4; i++)
   {
      msg->datrnum[i] = unpackUint32(&v);
   }
   return v - p;
}

int betpWrapper(uint32_t msgid, void *message, int length)
{
   int result = 0;
   /*worst case size requirements */
   //int maxlength = length + BETP_FRAME_PREAMBLE_SIZE + BETP_FRAME_POSTAMBLE_SIZE + BETP_FRAME_CHECKSUM_SIZE + BETP_FRAME_HEADER_SIZE;
   /*Is a static swap space a better idea? Is this code ever going to be multi-threaded? */
   //char *payload = systemAllocation(maxlength);
   char payload[100] = {0, };

//   if (payload)
//   {
      int offset = 0;
      BetpHeader_t hdr = { 0, PARSER_OSP, msgid };
      char *base = payload + BETP_FRAME_PREAMBLE_SIZE + BETP_FRAME_HEADER_SIZE;
      switch(msgid)
      {
         case BETP_SIRF_MSG_SSB_SIRF_INTERNAL:
            offset += packLoggingReq(base, (tSIRF_MSG_SSB_SIRF_INTERNAL *)message);
            break;
         case BETP_SIRF_MSG_POS_REQ:
            offset += packPosReq(base, (tSIRF_MSG_POS_REQ *)message);
            break;
         case OSP_PAUSE_GEOFENCE_OPERATION:
            offset += packPauseGeofenseOperation(base, (tOSP_PAUSE_GEOFENCE_OPERATION *)message);
            break;
         case OSP_CONTEXT_AIDING:
            offset += packContextAiding(base, (tOSP_CONTEXT_AIDING *)message);
            break;
         case OSP_GNSS_UNAVAILABLE:
            offset += packGnssUnavailable(base, (tOSP_GNSS_UNAVAILABLE *)message);
            break;
         default:
            CSP_PRINTF_INFO("[ %s ] Unhandled message %lu\n", __func__, msgid);
            break;
      }

      hdr.length = offset;

      payload[0] = BETP_FRAME_START_INDICATOR0;
      payload[1] = BETP_FRAME_START_INDICATOR1;
      offset += BETP_FRAME_PREAMBLE_SIZE;

      memcpy(payload + BETP_FRAME_PREAMBLE_SIZE, &hdr, BETP_FRAME_HEADER_SIZE);
      offset += BETP_FRAME_HEADER_SIZE;

      /*struct already loaded */
      /*we don't do checksums */

      payload[offset] = BETP_FRAME_END_INDICATOR0;
      payload[offset+1] = BETP_FRAME_END_INDICATOR1;
      offset += BETP_FRAME_POSTAMBLE_SIZE;
      if (GNSS_PROTOCOL_DEBUG)
         print_bytes(payload, offset);
      result = shmipc_write(payload, offset);
//      systemDeallocation(payload);
//   }

   return result;
}

int betpUnwrapper(void *message, int length)
{
   int id = 0;
   int maxlength = length + BETP_FRAME_PREAMBLE_SIZE + BETP_FRAME_POSTAMBLE_SIZE + BETP_FRAME_CHECKSUM_SIZE + BETP_FRAME_HEADER_SIZE + 4; /*the 4 is just in case padding...*/
   char payload[GNSS_IPC_MSG_MAX_LENGTH] = {0, };

//   if (payload)
//   {
      int offset = BETP_FRAME_PREAMBLE_SIZE + BETP_FRAME_HEADER_SIZE;
      int bytesRead = shmipc_read(payload, maxlength);

      if (bytesRead >= offset)
      {
         if (payload[0] == BETP_FRAME_START_INDICATOR0 && payload[1] == BETP_FRAME_START_INDICATOR1)
         {
            BetpHeader_t *hdr = (BetpHeader_t*)(payload + BETP_FRAME_PREAMBLE_SIZE);
            offset += hdr->length + ((hdr->flags & 0x8000) ? BETP_FRAME_CHECKSUM_SIZE : 0);
            /*we are ignoring checksums */
            id = hdr->msgid;

            if (payload[offset] == BETP_FRAME_END_INDICATOR0 && payload[offset + 1] == BETP_FRAME_END_INDICATOR1)
            {
               char *base = payload + BETP_FRAME_PREAMBLE_SIZE + BETP_FRAME_HEADER_SIZE;
               switch (id)
               {
                  case BETP_SIRF_MSG_SSB_PS_POS_RSP:
                     unpackPosReq((tSIRF_MSG_SSB_PS_POS_RSP *)message, base);
                  break;
#ifdef GNSS_ORG_SV_MSG
                  case BETP_TARGET_GNSS_SV_STATUS_REPORT:
                     unpackSvReport((tTARGET_GNSS_SV_STATUS_REPORT *)message, base);
                  break;
#endif
				  case BETP_TARGET_GNSS_SV_STATUS_REPORT_LIGHT:
                     unpackSvReportLight((tTARGET_GNSS_SV_STATUS_REPORT_LIGHT *)message, base);
                  break;
                  case BETP_SIRF_MSG_SSB_GEODETIC_NAVIGATION:
                     unpackGeoNav((tSIRF_MSG_SSB_GEODETIC_NAVIGATION *)message, base);
                     break;
                  case OSP_PAUSE_GEOFENCE_OPERATION:
                     unpackPauseGeofenseOperation((tOSP_PAUSE_GEOFENCE_OPERATION *)message, base);
                     break;
                  case OSP_CONTEXT_AIDING:
                     unpackContextAiding((tOSP_CONTEXT_AIDING *)message, base);
                     break;
                  case OSP_GNSS_UNAVAILABLE:
                     unpackGnssUnavailable((tOSP_GNSS_UNAVAILABLE *)message, base);
                     break;
                  case BETP_TARGET_POS_REQ_ACK:
                     unpackGnssPosReqACK((tTARGET_POS_REQ_ACK *)message, base);
                     break;
                  case BETP_SIRF_MSG_SSB_PLATFORM_TEST:
                     unpackAutotest((tSIRF_MSG_SSB_PLATFORM_TEST*)message, base);
                     break;
                  default:
                     CSP_PRINTF_INFO("[ %s ] Unhandled message %i\n", __func__, id);
                     break;
               }
            }
         }
      }
      else
      {
         /*pass on the error code */
         id = bytesRead;
      }
     // systemDeallocation(payload);
//   }
   return id;
}
