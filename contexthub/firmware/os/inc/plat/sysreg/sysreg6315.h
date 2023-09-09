/*----------------------------------------------------------------------------
 *      Exynos SoC  -  SYSREG
 *----------------------------------------------------------------------------
 *      Name:    sysregDrv6315.h
 *      Purpose: To implement SYSREG driver functions for 6315
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

#ifndef __SYSREG__6315_H__
#define __SYSREG__6315_H__

// CHUB
void sysregSetMaskIrq(IN uint64_t irq);
uint64_t sysregGetMaskIrq(void);
void sysregSetDRCG(IN uint32_t enable);
void sysregSetOscEn(void);
void sysregSetOscDis(void);
void sysregSetCmgpReq(void);
void sysregClearCmgpReq(void);
void sysregSaveState(void);
void sysregRestoreState(void);
#endif

