/*----------------------------------------------------------------------------
 * Exynos SoC  -  SYSREG
 *
 * Copyright (C) 2020 Samsung Electronics Co., Ltd.
 *
 * Shinkyu Park <shinkyu.park@samsung.com>
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

#ifndef __SYSREG_5515_H__
#define __SYSREG_5515_H__

void sysregPDRequest(void);
void sysregPDRelease(void);
void sysregSetMaskIrq(uint64_t irq);
void sysregClearMaskIrq(uint64_t irq);
uint64_t sysregGetMaskIrq(void);
void sysregSetDRCG(IN uint32_t enable);
void sysregSetEarlyWakeupWindowReq(bool en);
uint32_t sysregGetEarlyWakeupWindowReq(void);
void sysregSetVvalidIntr(uint32_t intMode, uint32_t intMask);
void sysregSetMIFReq(bool enable);
uint32_t sysregGetMIFReqAck(void);
void sysregClrMIFReqAck(void);
bool sysregMIFRequest(void);
bool sysregMIFRelease(void);
void sysregSetCmgpReq(bool enable);
uint32_t sysregGetCmgpReqAck(void);
int32_t sysregAdcGetTurn(uint32_t timeout);
void sysregAdcReleaseTurn(void);
void sysregCmgpRequest(void);
void sysregCmgpRelease(void);
void sysregSetCM55Base(uint32_t baseAddr);
void sysregSendIRQtoCM55(void);
void sysregClearIRQfromCM55(void);
void sysregSetOscEn(void);
void sysregSaveState(void);
void sysregRestoreState(void);

#endif

