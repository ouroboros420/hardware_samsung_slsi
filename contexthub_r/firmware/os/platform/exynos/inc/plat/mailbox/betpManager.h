#ifndef _BETP_MANAGER_H
#define _BETP_MANGER_H

#include <stdint.h>
#if defined(MBGNSS_REQUIRED)

//Make BETP msgID
#define SIRF_ENC_SUB_ID(message_sub_id) (((uint16_t)(message_sub_id)  & 0xFF) << 8)
#define SIRF_ENC_MID(message_id)         ((uint16_t)(message_id)      & 0xFF)
#define SIRF_MAKE_MSG_ID(mid, sid) (SIRF_ENC_SUB_ID(sid) | SIRF_ENC_MID(mid))

#define BETP_SIRF_MSG_SSB_SIRF_INTERNAL SIRF_MAKE_MSG_ID(228, 0)
#define BETP_SIRF_MSG_POS_REQ SIRF_MAKE_MSG_ID(101, 4)
#define BETP_SIRF_MSG_SSB_PS_POS_RSP SIRF_MAKE_MSG_ID(69, 3)
#define BETP_TARGET_GNSS_SV_STATUS_REPORT SIRF_MAKE_MSG_ID(10, 113)

#define BETP_FRAME_START_INDICATOR0 (0xA0)
#define BETP_FRAME_START_INDICATOR1 (0xA3)
#define BETP_FRAME_END_INDICATOR0 (0xB0)
#define BETP_FRAME_END_INDICATOR1 (0xB3)
#define BETP_FRAME_PREAMBLE_SIZE (2)
#define BETP_FRAME_POSTAMBLE_SIZE (2)
#define BETP_FRAME_CHECKSUM_SIZE (4)
#define BETP_FRAME_HEADER_SIZE (6)

#define GNSS_STATUS_MAX_SVS 32

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

typedef struct tSIRF_MSG_POS_REQ
{
   uint32_t time_btw_fixes_ms;
   uint8_t output_settings;
   uint16_t status_output;
   uint16_t meas_output;
   uint32_t ID;
} tSIRF_MSG_POS_REQ;

typedef struct tSIRF_MSG_SSB_PS_POS_RSP
{
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
   uint8_t num_remaining;
} tSIRF_MSG_SSB_PS_POS_RSP;

typedef struct tTARGET_GNSS_SV_STATUS_REPORT_GNSS_SV_LIST
{
   int16_t svid;
   uint8_t constellation;
   float c_n0_dbhz;
   float elevation;
   float azimuth;
   uint8_t flags;
} tTARGET_GNSS_SV_STATUS_REPORT_GNSS_SV_LIST;

typedef struct tTARGET_GNSS_SV_STATUS_REPORT
{
   int32_t num_svs;
   tTARGET_GNSS_SV_STATUS_REPORT_GNSS_SV_LIST gnss_sv_list[GNSS_STATUS_MAX_SVS];
} tTARGET_GNSS_SV_STATUS_REPORT;

int betpWrapper ( uint32_t betpMsgID, void* betpMsgData, int betpMsgLength);
int betpUnwrapper ( void* msgData, int length );

#endif
#endif
