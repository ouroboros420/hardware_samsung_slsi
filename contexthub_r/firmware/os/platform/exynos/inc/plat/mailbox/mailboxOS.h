/*
 * Copyright (C) 2017 Samsung Electronics Co., Ltd.
 *
 * Contact: Boojin Kim <boojin.kim@samsung.com>
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

#ifndef __MAILBOX_OS_H__
#define __MAILBOX_OS_H__

void mailboxAPHandleIRQ(int evt, void *priv);
void mailboxOsEnable(void);
void mailboxAP_IRQHandler(void);
#if defined(MBAPM_REQUIRED)
void mailboxApm2Chub_IRQHandler(void);
#endif
#if defined(MBGNSS_REQUIRED)
void MailboxGnss2Chub_IRQHandler(void);
#endif
#endif
