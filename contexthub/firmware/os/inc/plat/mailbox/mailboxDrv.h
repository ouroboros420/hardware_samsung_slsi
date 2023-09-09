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

#ifndef __MAILBOX_DRV_H__
#define __MAILBOX_DRV_H__

#include <csp_common.h>
#include <ipc_chub.h>

#define MAILBOX_DRV "MB"

#define MAILBOX_EVT_AP_OWN 0x100
#define MAILBOX_EVT_APM_OWN 0x200
#define MAILBOX_EVT_CP_OWN 0x400
#define MAILBOX_EVT_GNSS_OWN 0x800

enum mailbox_event {
    MAILBOX_EVT_WAKEUP = MAILBOX_EVT_AP_OWN,
    MAILBOX_EVT_WAKEUP_CLEAR,
    MAILBOX_EVT_DEBUG,
    MAILBOX_EVT_REBOOT,
    MAILBOX_EVT_SLEEP = MAILBOX_EVT_APM_OWN,
    MAILBOX_EVT_MAX,
};

typedef void (*mailboxRxCB) (void *cookie, void *Rxbuf, int err);

void mailboxDrvEnable(void);
void mailboxDrvDisable(void);
int32_t mailboxDrvWrite(const void *buf, uint32_t size);
int32_t mailboxDrvRead(void *buf, uint32_t size, mailboxRxCB callback,
                       void *cookie);
void mailboxDrvWriteEvent(enum mailbox_event event, uint32_t val);
void mailboxDrvHandleRxMessage(int irq_num);
#endif
