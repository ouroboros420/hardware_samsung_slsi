/*----------------------------------------------------------------------------
 *      Exynos SoC  -  SYSREG
 *----------------------------------------------------------------------------
 *      Name:    sysregDrv9935.h
 *      Purpose: To implement SYSREG driver functions for 9935
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

#ifndef __SYSREG__9935_H__
#define __SYSREG__9935_H__

// CHUB
void sysregSetWakeupInterruptAll(void);
void sysregClrWakeupInterruptAll(void);
void sysregPDRequest(void);
void sysregPDRelease(void);
void sysregSetMaskIrq(IN uint64_t irq);
uint64_t sysregGetMaskIrq(void);
void sysregClearMaskIrq(uint64_t irq);
void sysregSetDRCG(IN uint32_t enable);
void sysregSetEarlyWakeupWindowReq(bool en);
uint32_t sysregGetEarlyWakeupWindowReq(void);
void sysregSetVvalidIntr(uint32_t intMode, uint32_t intMask);
void sysregSetOscEn(void);
void sysregSetOscDis(void);
void sysregSetCmgpReq(void);
void sysregClearCmgpReq(void);
void sysregSaveState(void);
void sysregRestoreState(void);
#endif

