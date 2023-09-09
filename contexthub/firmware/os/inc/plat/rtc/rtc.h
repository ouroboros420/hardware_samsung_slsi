/*----------------------------------------------------------------------------
 *      Exynos SoC  -  RTC
 *----------------------------------------------------------------------------
 *      Name:    rtc.h
 *      Purpose: To expose RTC APIs
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

#ifndef __RTC_H__
#define __RTC_H__

#include <csp_common.h>
#include <csp_assert.h>
#include <csp_printf.h>

union time64_t {
    struct {
        uint64_t tm_sec  : 6;
        uint64_t tm_min  : 6;
        uint64_t tm_hour : 5;
        uint64_t tm_mday : 5;
        uint64_t tm_mon  : 4;
        uint64_t tm_year : 8;
        uint64_t tm_wday : 3;
        uint64_t tm_yday : 9;
    };
    uint64_t time64;
};

#define RTC_MIN_YEAR        2000
#define RTC_MAX_YEAR        2999
#define RTC_IGNORE_VALUE    0xFF
#define RTC_REPEAT_OFF      0
#define RTC_REPEAT_ON       1

#define RTC_TICK_INTERRUPT      (1<<0)
#define RTC_TICK0_INTERRUPT     (1<<0)
#define RTC_ALARM_INTERRUPT     (1<<1)
#define RTC_TICK1_INTERRUPT     (1<<2)

// Driver APIs
void rtcInit(void);
void rtcSetInitTime(int64_t time);
uint64_t rtcGetTimeStampNS(void);
uint64_t rtcGetTimeStampUS(void);
void rtcSetTickTime(IN uint32_t sec, IN uint32_t mSec, IN uint32_t uSec,
                     OUT void (*callback) (void), uint8_t repeat);
uint64_t rtcGetCurrentTickTime(void);
uint32_t rtcGetTSTick(void);
void rtcSetDateTime(uint32_t year, uint32_t month, uint32_t date,
                    uint32_t hour, uint32_t min, uint32_t sec);
void rtcSetDate(uint32_t year, uint32_t month, uint32_t date);
void rtcSetTime(uint32_t hour, uint32_t min, uint32_t sec);
void rtcSetTime64(union time64_t time);
void rtcGetTime64(union time64_t *time);
void rtcGetDateTime(uint32_t* YYYY, uint32_t* MM, uint32_t* DD,
                    uint32_t* hh, uint32_t* mm, uint32_t* ss);
void rtcGetDate(uint32_t* YYYY, uint32_t* MM, uint32_t* DD);
void rtcGetBCDTime(uint32_t* hh, uint32_t* mm, uint32_t* ss);
void rtcSetAlarmDateTime(uint32_t year, uint32_t month, uint32_t date,
                         uint32_t hour, uint32_t min, uint32_t sec);
void rtcSetAlarmDate(uint32_t year, uint32_t month, uint32_t date);
void rtcSetAlarmTime(uint32_t hour, uint32_t min, uint32_t sec);
void rtcSetAlarmInterrupt(uint32_t en_year, uint32_t en_mon, uint32_t en_date,
                          uint32_t en_hour, uint32_t en_min, uint32_t en_sec);
void rtcEnableAlarmInterrupt(void);
void rtcDisableAlarmInterrupt(void);
void rtcStartTick(void);
void rtcStopTick(void);
void rtcStartTSTick(void);
void rtcStopTSTick(void);
int rtcGetTickRepeat(void);
uint64_t rtcGetTickTime(void);
void rtcSetTickHandler(void (*callback)(void));
void rtcSetAlarmHandler(void (*callback)(void));
uint64_t rtcGetTime(void);
void rtc_IRQHandler(void);

#endif  /* __RTC_H__ */

