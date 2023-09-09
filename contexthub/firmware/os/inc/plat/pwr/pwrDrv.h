/*----------------------------------------------------------------------------
 *      Exynos SoC  -  PWR
 *----------------------------------------------------------------------------
 *      Name:    pwrDrv.h
 *      Purpose: To expose PWR driver functions
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

#ifndef __PWR_DRV_H__
#define __PWR_DRV_H__

#include <csp_common.h>

#define POWER_MODE_NORMAL   0
#define POWER_MODE_SLEEP    1

#if defined(LOCAL_POWERGATE)
void pwrDrvPrepareSleepPwrGating(uint32_t data);
#endif

void pwrDrvEnableGnssWakeLock(void);
void pwrDrvDisableGnssWakeLock(void);
bool pwrDrvGetGnssWakeLock(void);

void pwrDrvEnableApWakeLock(void);
void pwrDrvDisableApWakeLock(void);
bool pwrDrvGetApWakeLock(void);

void pwrDrvEnableCM55WakeLock(void);
void pwrDrvDisableCM55WakeLock(void);
bool pwrDrvGetCM55WakeLock(void);
void pwrDrvEnableCM55BootLock(void);
void pwrDrvDisableCM55BootLock(void);

bool pwrDrvGetWakeLock(void);

void pwrDrvSleep(void);
void pwrDrvSetPowerMode(uint32_t mode);
uint32_t pwrDrvGetPowerMode(void);

#endif

