/*----------------------------------------------------------------------------
 *      Exynos SoC  -  WDT
 *----------------------------------------------------------------------------
 *      Name:    wdt.h
 *      Purpose: To expose WDT APIs and define macros
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

#ifndef __WDT_H__
#define __WDT_H__

#include <csp_common.h>

#define WDT_DEFAULT_TIMEOUT     15000 // 15000ms

void wdtInit(void);
void wdtEnableClk(void);
void wdtDisableClk(void);
void wdtEnable(void);
void wdtDisable(void);
void wdtSetTime(uint32_t time_msec);
void wdtPing(void);
void wdtEnableInterrupt(void);
void wdtDisableInterrupt(void);
void wdtEnableReset(void);
void wdtDisableReset(void);
void wdtSaveState(void);
void wdtRestoreState(void);
void wdtDeinit(void);

#endif
