/*----------------------------------------------------------------------------
 *      Exynos SoC  - CMGP
 *----------------------------------------------------------------------------
 *      Name:    cmgpDrv.h
 *      Purpose: To expose CMGP driver functions
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

#ifndef __CMGP_DRIVER_H__
#define __CMGP_DRIVER_H__

#include <csp_common.h>
#include <cmgp.h>

#include CSP_HEADER(cmgpDrv)

void cmgpDrvInit(void);
void cmgpDrvEnableInterrupt(uint32_t intNum);
void cmgpDrvDisableInterrupt(uint32_t intNum);
void cmgpDrvSetIrqPriority(uint32_t intNum, uint32_t priority);
uint32_t cmgpDrvGetIrqNum(void);

#endif
