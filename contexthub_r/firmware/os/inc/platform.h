/*
 * Copyright (C) 2016 The Android Open Source Project
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

#ifndef _PLATFORM_H_
#define _PLATFORM_H_

//
//  platform.h
//  seos
//
//  Created by Simon Wilson on 10/2/14.
//

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <seos.h>

/* plat life cycle */
void platInitialize(void);
void platUninitialize(void);
void platPeriodic(void);

// free all platform-specific resources for TID, and return non-zero status if some cleanup was done
uint32_t platFreeResources(uint32_t tid);

/* Logging */
#ifdef EXYNOS_CONTEXTHUB
void *platLogAllocUserData(bool force);
#else
void *platLogAllocUserData();
#endif
void platLogFlush(void *userData);
bool platLogPutcharF(void *userData, char ch);
void platEarlyLogFlush(void);

/* fast timer */
#define TIME_OF_SYSTICK_IN_MS (TICK_INTERNAL_IN_MS * 1000 * 1000)
uint64_t platSystickTicksToNs(uint32_t systickTicks);
uint64_t platGetTicks(void); //in nanoseconds since an arbitrary starting point in the past
uint64_t platGetTicksSinceTheLastTick(void);


/* sleep/wake */
#define PLAT_MAX_SLEEP_DEVS    32

void platSleep(void);

//in platSleepClockRequest() code to set next timer of some variety will live
//note that maxErrTotalPpm != maxDriftPpm + maxJitterPpm is quite possible since it is possible to have:
// timer A allowing 300ppm of jitter and 10pp of drift and timer B allowing 20ppm of jitter and 500ppm of drift
// in that case we'd see maxJitterPpm = 200, maxDriftPpm = 500, maxErrTotalPpm = 520  (MAX of all timers' allowable error totals)
//return true if timer was set. false if you failed (you will be called right back though. so false is usually reserved for cases
// like "it is too soon to set a timer")
//a special case is when nextTimer == 0. this indicates no next timer, so configure system for that
//do not call this func if timer is already due - it will be delayed (potentially by a whole sleep-wke cycle), though this is unlikely
bool platSleepClockRequest(uint64_t wakeupTime, uint32_t maxJitterPpm, uint32_t maxDriftPpm, uint32_t maxErrTotalPpm); //"nextTime == 0" => "no wakeup needed"
uint64_t platGetNextWakeupTime(void);
uint64_t platGetOSTickVariable(void);
void platSetOSTickVariable(uint64_t elapsedTime);
void platSetOSTickVariableAfterSleep(void);
void platSetOSTickVariableAfterPwrGating(void);

/* 0 for any "max" value means "do not care" */
bool platRequestDevInSleepMode(uint32_t sleepDevID, uint32_t maxWakeupTime); //request that this device remain powered/clocked in sleep mode   (device lists are platform specific)
bool platAdjustDevInSleepMode(uint32_t sleepDevID, uint32_t maxWakeupTime); //adjust maxWakeupTime for this device
bool platReleaseDevInSleepMode(uint32_t sleepDevID); //unrequest that this device remain powered/clocked in sleep mode (device lists are platform specific)

void platStartSysTick(void);
void platStopSysTick(void);
#ifdef EXYNOS_CONTEXTHUB
void platReset(int systemReset);
#endif

uint64_t getTimeStampUS(void);

#ifdef __cplusplus
}
#endif

#endif

