/*----------------------------------------------------------------------------
 * Exynos SoC  -  MAILBOX2CP
 *
 * Copyright (C) 2021 Samsung Electronics Co., Ltd.
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

#ifndef __MAILBOX_CP_H__
#define __MAILBOX_CP_H__

#include <csp_common.h>

void mailboxCpWriteData(uint32_t num, uint32_t data);
uint32_t mailboxCpReadData(uint32_t num);
void mailboxCpSendInterrupt(uint32_t int_num);
void mailboxCpSendMessage(uint32_t int_num, uint32_t sr, uint32_t msg);
void mailboxCpSetCallback(uint32_t num, void (*callback)(void));
void mailboxCP_IRQHandler(void);
void mailboxCpInit(void);

#endif
