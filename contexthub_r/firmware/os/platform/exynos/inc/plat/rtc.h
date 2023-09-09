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
void rtcSetWakeupTimer(IN uint64_t delay);
void rtcSetSystemTime(IN uint32_t year, IN uint32_t mon, IN uint32_t date,
                       IN uint32_t day, IN uint32_t hour, IN uint32_t min,
                       IN uint32_t sec);
void rtcGetSystemTime(OUT uint32_t * year, OUT uint32_t * mon,
                       OUT uint32_t * date, OUT uint32_t * day,
                       OUT uint32_t * hour, OUT uint32_t * min,
                       OUT uint32_t * sec);
uint64_t rtcGetSystemTimeSec(void);
void rtcSetTimeStampNS(uint64_t time);
uint64_t rtcGetTimeStampNS(void);
uint64_t rtcGetTimeStampUS(void);

#if (RTC_ALARM_SUPPORT)
void rtcSetAlarmTime(IN uint32_t year, IN uint32_t mon, IN uint32_t date,
                      IN uint32_t hour, IN uint32_t min, IN uint32_t sec,
                      void (*callback) (void), uint8_t repeat);
void rtcGetAlarmTime(OUT uint32_t * year, OUT uint32_t * mon,
                      OUT uint32_t * date, OUT uint32_t * hour,
                      OUT uint32_t * min, OUT uint32_t * sec);
void rtcStartAlarm(void);
void rtcStopAlarm(void);
#endif

void rtcSetTickTime(IN uint32_t sec, IN uint32_t mSec, IN uint32_t uSec,
                     OUT void (*callback) (void), uint8_t repeat);
uint64_t rtcGetCurrentTickTime(void);
void rtcStartTick(void);
void rtcStopTick(void);
void rtcDeinit(void);
void rtcIRQHandler(uint32_t irq);
int rtcGetTickRepeat(void);
uint64_t rtcGetTickTime(void);

// Same with rtcGetSystemTimeSec. Called by nanohub hostintf
uint64_t rtcGetTime(void);
void rtcClearInt(uint32_t interrupt);

#endif  /* __RTC_H__ */

