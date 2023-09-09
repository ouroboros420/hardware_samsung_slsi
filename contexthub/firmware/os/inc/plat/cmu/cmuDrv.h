/*----------------------------------------------------------------------------
 *      Exynos SoC  -  CMU
 *----------------------------------------------------------------------------
 *      Name:    cmuDrv.h
 *      Purpose: To expose CMU driver functions
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

#ifndef __CMU_DRIVER_H__
#define __CMU_DRIVER_H__

#include <csp_common.h>
#include <cmu.h>

#include CSP_HEADER(cmuDrv)

void cmuDrvInit(uint32_t mainclk);
int cmuDrvSetCpuClk(uint32_t freq);
uint32_t cmuDrvGetSpeed(IN CmuIpType ip);
bool cmuDrvSetDivider(IN CmuIpType ip, IN uint32_t divider);
bool cmuDrvControlHwacg(IN CmuHwacgControlType enable);
bool cmuDrvControlHwacgIP(IN CmuHwacgIpType ip, IN CmuHwacgControlType enable);
void cmuDrvDeinit(void);
void cmuDrvSaveState(void);
void cmuDrvRestoreState(void);
uint32_t cmuDrvSetCpuMux(uint32_t mux);
#endif

