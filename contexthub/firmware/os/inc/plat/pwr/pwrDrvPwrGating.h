/*----------------------------------------------------------------------------
 *      Exynos SoC  -  PWR
 *----------------------------------------------------------------------------
 *      Name:    pwrDrvPwrGating.h
 *      Purpose: To implement save and restore SFRs & GPRs for power-gating
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

#ifndef __PWR_DRV_PWR_GATING_H__
#define __PWR_DRV_PWR_GATING_H__

#include <csp_common.h>

void pwrDrvCmgpSaveState(void);
void pwrDrvCmgpRestoreState(void);
void pwrDrvSaveCorePeri(void);
void pwrDrvRestoreCorePeri(void);
void pwrDrvPwrGating(void);
void pwrDrvPwrGatingSaveCPU(u32 *msp, u32 *psp);
void pwrDrvPwrGatingRestoreCPU(u32 msp, u32 psp);

#endif

