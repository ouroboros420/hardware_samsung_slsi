#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "betpManager.h"
#include "shmipc.h"
#include "csp_printf.h"
#if defined(SEOS)
   #include <heap.h>
#endif

uint8_t toUint8(char * v);
uint16_t toUint16(char * v);
uint32_t toUint32(char * v);
int32_t toInt32(char * v);
float toFloat(char * v);
void * systemAllocation(int length);
void systemDeallocation(void * p);
int packLoggingReq(char * p, tSIRF_MSG_SSB_SIRF_INTERNAL * msg);
int packPosReq(char * p, tSIRF_MSG_POS_REQ * msg);
int unpackPosReq(tSIRF_MSG_SSB_PS_POS_RSP * msg, char * p);
int unpackSvStatus(tTARGET_GNSS_SV_STATUS_REPORT_GNSS_SV_LIST * msg, char * p);
int unpackSvReport( tTARGET_GNSS_SV_STATUS_REPORT * msg, char * p);

uint8_t toUint8(char * v)
{
  return (uint8_t)(*v);
}

uint16_t toUint16(char * v)
{
   return ((uint16_t)(v[0] | v[1]<<8));
}

uint32_t toUint32(char * v)
{
   return ((uint32_t)(v[0] | v[1]<<8 | v[2]<<16 | v[3]<<24));
}

int32_t toInt32(char * v)
{
   return ((int32_t)(v[0] | v[1]<<8 | v[2]<<16 | v[3]<<24));
}

float toFloat(char * v)
{
   union Data { int i; float f; } data;
   data.i = (v[0] | v[1]<<8 | v[2]<<16 | v[3]<<24);
   return data.f;
}

void * systemAllocation(int length)
{
   int i = length;
   char * payload;
#if defined(SEOS)
   payload = (char *)heapAlloc(length);
#endif

   if (payload)
      while (i--)
         payload[i] = 0;

   return payload;
}

void systemDeallocation(void * payload)
{
#if defined(SEOS)
   heapFree(payload);
#endif
}

int packLoggingReq(char * p, tSIRF_MSG_SSB_SIRF_INTERNAL * msg)
{
   int offset = 0;
   p[offset++] = msg->Level_Change;
   p[offset++] = msg->DSP;
   p[offset++] = msg->INTC;
   p[offset++] = msg->KA;
   p[offset++] = msg->HWTIMER;
   p[offset++] = msg->UART;
   p[offset++] = msg->MEMORY;
   p[offset++] = msg->PWRCLK;
   p[offset++] = msg->MISC;
   p[offset++] = msg->SPI;
   p[offset++] = msg->CBHANDLER;
   p[offset++] = msg->CHDEVICE;
   p[offset++] = msg->INSAMPLE;
   p[offset++] = msg->AGC;
   p[offset++] = msg->CLKGEN;
   p[offset++] = msg->ATXCTRL;
   p[offset++] = msg->ACQ;
   p[offset++] = msg->TRACK;
   p[offset++] = msg->XCOR;
   p[offset++] = msg->CW;
   p[offset++] = msg->DGPSTRK;
   p[offset++] = msg->DRM;
   p[offset++] = msg->OS;
   p[offset++] = msg->RESET;
   p[offset++] = msg->PWRMGRCTRL;
   p[offset++] = msg->CTRLPLATDATA;
   p[offset++] = msg->NVM;
   p[offset++] = msg->HOSTIO;
   p[offset++] = msg->MITASK;
   p[offset++] = msg->UIDBG;
   p[offset++] = msg->PWRMGRLOGIC;
   p[offset++] = msg->AIDING;
   p[offset++] = msg->CTRLDATA;
   p[offset++] = msg->GPSRXCTRL;
   p[offset++] = msg->ATXCTRLMGR;
   p[offset++] = msg->VISLIST;
   p[offset++] = msg->MIGPS;
   p[offset++] = msg->NAV;
   p[offset++] = msg->DGPSAPP;
   p[offset++] = msg->BEP;
   p[offset++] = msg->SL;
   p[offset++] = msg->UIGPS;
   p[offset++] = msg->SSB;
   p[offset++] = msg->NMEA;
   p[offset++] = msg->QOS;
   p[offset++] = msg->SVDATA;
   p[offset++] = msg->TASKCTRL;
   p[offset++] = msg->SPISLV;
   p[offset++] = msg->MEI;
   p[offset++] = msg->SIF;
   p[offset++] = msg->ACT;
   p[offset++] = msg->DUMMY1;
   return offset;
}

int packPosReq(char * p, tSIRF_MSG_POS_REQ * msg)
{
   int offset = 0;
   memcpy(p, &(msg->time_btw_fixes_ms), 4);
   offset += 4;
   memcpy(p + offset, &(msg->output_settings), 1);
   offset += 1;
   memcpy(p + offset, &(msg->status_output), 2);
   offset += 2;
   memcpy(p + offset, &(msg->meas_output), 2);
   offset += 2;
   memcpy(p + offset, &(msg->ID), 4);
   offset += 4;
   return offset;
}

int unpackPosReq(tSIRF_MSG_SSB_PS_POS_RSP * msg, char * p)
{
   int offset = 0;
   msg->pos_result_flags = toUint8(p + offset);
   offset += 1;
   msg->clk_cor_utc_off = toUint8(p + offset);
   offset += 1;
   msg->gps_week = toUint16(p + offset);
   offset += 2;
   msg->gps_time_of_week_ms = toUint32(p + offset);
   offset += 4;
   msg->lat = toUint32(p + offset);
   offset += 4;
   msg->lon = toUint32(p + offset);
   offset += 4;
   msg->altitude = toUint32(p + offset);
   offset += 4;
   msg->accuracy = toUint16(p + offset);
   offset += 2;
   msg->velocity_horizontal = toUint16(p + offset);
   offset += 2;
   msg->velocity_heading = toUint16(p + offset);
   offset += 2;
   msg->sources_used = toUint8(p + offset);
   offset += 1;
   msg->num_remaining = toUint8(p + offset);
   offset += 1;

   return offset;
}

int unpackSvStatus(tTARGET_GNSS_SV_STATUS_REPORT_GNSS_SV_LIST * msg, char * p)
{
   int offset = 0;
   msg->svid = toUint16(p + offset);
   offset += 2;
   msg->constellation = toUint8(p + offset);
   offset += 1;
   msg->c_n0_dbhz = toFloat(p + offset);
   offset += 4;
   msg->elevation = toFloat(p + offset);
   offset += 4;
   msg->azimuth = toFloat(p + offset);
   offset += 4;
   msg->flags = toUint8(p + offset);
   offset += 1;
   return offset;
}

int unpackSvReport(tTARGET_GNSS_SV_STATUS_REPORT * msg, char * p)
{
   int i = 0;
   int remain = 0;
   int offset = 0;
   msg->num_svs = toInt32(p + offset);

   if (msg->num_svs > GNSS_STATUS_MAX_SVS)
   {
     remain = msg->num_svs - GNSS_STATUS_MAX_SVS;
     msg->num_svs = GNSS_STATUS_MAX_SVS;
   }

   offset += 4;
   for (i = 0; i < msg->num_svs; i++)
   {
      offset += unpackSvStatus( &(msg->gnss_sv_list[i]), p + offset);
   }

   if (remain)
   {
     offset += remain * 16; /*sv status packed size*/
   }
   return offset;
}

int betpWrapper(uint32_t betpMsgID, void* betpMsgData, int betpMsgLength)
{
   int result = 0;
   /* worst case size requirements */
   int maxlength = betpMsgLength + BETP_FRAME_PREAMBLE_SIZE + BETP_FRAME_POSTAMBLE_SIZE + BETP_FRAME_CHECKSUM_SIZE + BETP_FRAME_HEADER_SIZE;
   /* Is a static swap space a better idea? Is this code ever going to be multi-threaded? */
   char * payload = systemAllocation(maxlength);

   if (payload)
   {
      int offset = 0;
      BetpHeader_t hdr = { 0, PARSER_OSP, betpMsgID };
      switch(betpMsgID)
      {
         case BETP_SIRF_MSG_SSB_SIRF_INTERNAL:
         {
            offset += packLoggingReq(payload + BETP_FRAME_PREAMBLE_SIZE + BETP_FRAME_HEADER_SIZE, (tSIRF_MSG_SSB_SIRF_INTERNAL *)betpMsgData);
            hdr.length = offset;
         }
         break;
         case BETP_SIRF_MSG_POS_REQ:
         {
            offset += packPosReq(payload + BETP_FRAME_PREAMBLE_SIZE + BETP_FRAME_HEADER_SIZE, (tSIRF_MSG_POS_REQ *)betpMsgData);
            hdr.length = offset;
         }
         break;
      }

      payload[0] = BETP_FRAME_START_INDICATOR0;
      payload[1] = BETP_FRAME_START_INDICATOR1;
      offset += BETP_FRAME_PREAMBLE_SIZE;

      memcpy(payload + BETP_FRAME_PREAMBLE_SIZE, &hdr, BETP_FRAME_HEADER_SIZE);
      offset += BETP_FRAME_HEADER_SIZE;

      /* struct already loaded */
      /* we don't do checksums */

      payload[offset] = BETP_FRAME_END_INDICATOR0;
      payload[offset+1] = BETP_FRAME_END_INDICATOR1;
      offset += BETP_FRAME_POSTAMBLE_SIZE;

      result = shmipc_write(payload, offset);
      systemDeallocation(payload);
   }

   return result;
}

int betpUnwrapper(void * msgData, int length)
{
   int id = 0;
   int maxlength = length + BETP_FRAME_PREAMBLE_SIZE + BETP_FRAME_POSTAMBLE_SIZE + BETP_FRAME_CHECKSUM_SIZE + BETP_FRAME_HEADER_SIZE + 4; /* the 4 is just in case padding...*/
   char * payload = systemAllocation(maxlength);

   if (payload)
   {
      int offset = BETP_FRAME_PREAMBLE_SIZE + BETP_FRAME_HEADER_SIZE;
      int bytesRead = shmipc_read(payload, maxlength);

      if (bytesRead >= offset)
      {
         if (payload[0] == BETP_FRAME_START_INDICATOR0 && payload[1] == BETP_FRAME_START_INDICATOR1)
         {
            BetpHeader_t * hdr = (BetpHeader_t*)(payload + BETP_FRAME_PREAMBLE_SIZE);
            offset += hdr->length + ((hdr->flags & 0x8000) ? BETP_FRAME_CHECKSUM_SIZE : 0);
            /* we are ignoring checksums */
            id = hdr->msgid;

            if (payload[offset] == BETP_FRAME_END_INDICATOR0 && payload[offset + 1] == BETP_FRAME_END_INDICATOR1)
            {
               switch (id)
               {
                  case BETP_SIRF_MSG_SSB_PS_POS_RSP:
                     unpackPosReq((tSIRF_MSG_SSB_PS_POS_RSP *)msgData, (payload + BETP_FRAME_PREAMBLE_SIZE + BETP_FRAME_HEADER_SIZE));
                  break;
                  case BETP_TARGET_GNSS_SV_STATUS_REPORT:
                     unpackSvReport((tTARGET_GNSS_SV_STATUS_REPORT *)msgData, (payload + BETP_FRAME_PREAMBLE_SIZE + BETP_FRAME_HEADER_SIZE));
                  break;
                  default:
//                   CSP_PRINTF_INFO(" betpUnwrapper : Unhandled message %i\n", id);
                     break;
               }
            }
         }
      }
      else
      {
         /* pass on the error code */
         id = bytesRead;
      }
      systemDeallocation(payload);
   }
   return id;
}
