/*
 * Copyright (C) 2017 Samsung Electronics Co., Ltd.
 *
 * Contact: Boyoung Kim <Eric.by.kim@samsung.com>
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

#if defined(MBAPM_REQUIRED)

#include <mailbox.h>
#include <mailboxApm.h>
#include <ipc_chub.h>
#include <csp_os.h>
#include <csp_common.h>

/*  mailbox REG  */

#define GetMasterlock(a)    ((a>>28)&0xff)
#define GetEintInfo(a)      (a&0xffff)
#define GetSleepReadyReq(a) (a&0xf)

enum MAILBOX_APM_EVENT {
    MAILBOX_WAKEUP_REQ,
    MAILBOX_WAKEUP_REQ_ACK,
    MAILBOX_SLEEPREADY_REQ,
    MAILBOX_SLEEPREADY_REQ_ACK,
    MAILBOX_SLEEPREADY_REQ_EINT,
    MAILBOX_SLEEPREADY_REQ_EINT_ACK,
    MAILBOX_POWER_MODE_MAX,
};

static uint32_t SleepReadyReq;
static uint32_t MasterWakelock;
static uint32_t MailboxEintInfo;

static void mailboxApmSetSleepReady(uint32_t val)
{
    SleepReadyReq = GetSleepReadyReq(val);
}

int32_t mailboxApmGetSleepReady(void)
{
    return SleepReadyReq;
}

static void mailboxApmSetMasterlock(uint32_t val)
{
    MasterWakelock = GetMasterlock(val);
}

static int32_t mailboxApmGetMasterlock(void)
{
    return MasterWakelock;
}

static void mailboxApmSetEintInfo(uint32_t val)
{
    MailboxEintInfo = GetEintInfo(val);
}

static int32_t mailboxApmGetEintInfo(void)
{
    return MailboxEintInfo;
}

void handleApmRxMessage(int irq_num)
{
    uint32_t sr2, sr3;

    if (irq_num != IRQ_CH2)
        return;

    sr2 = ipc_hw_read_shared_reg(APM, 2);
    sr3 = ipc_hw_read_shared_reg(APM, 3);

    mailboxApmSetSleepReady(sr2);
    mailboxApmSetMasterlock(sr3);
    mailboxApmSetEintInfo(sr3);

    ipc_hw_write_shared_reg(APM, 2, 0);
    ipc_hw_write_shared_reg(APM, 3, 0);
}

void mailboxApmWriteEvent(enum mailbox_event event)
{
    uint32_t sr0;

    if (event == MAILBOX_EVT_SLEEP) {
        /* MAILBOX_SLEEPREADY_REQ_ACK */
        sr0 = 1;
        ipc_hw_write_shared_reg(APM, 0, sr0);
        ipc_hw_gen_interrupt(APM, IRQ_CH2);
    }
}

void mailboxApmEnable(void)
{
#if defined(APM_HANDSHAKING)
    NVIC_EnableIRQ(MB_APM_IRQn);
#endif

    //CHUB shall do nothing for initialization of mailbox since it is handled by Master.
    ipc_set_owner(APM, (void *)MAILBOX_APM_BASE_ADDRESS, IPC_DST);
}

void mailboxApm2Chub_IRQHandler(void)
{
    uint32_t num, i;

    num = ipc_hw_read_int_status_reg(APM);
    int start_index = ipc_hw_read_int_start_index(APM);

    for (i = start_index; i < start_index + IRQ_EVT_CHUB_MAX; i++) {
        if (num & (1 << i)) {
            if ((i - start_index) == IRQ_EVT_CH2) {
                ipc_hw_clear_int_pend_reg(APM, IRQ_EVT_CH2);
                handleApmRxMessage(IRQ_EVT_CH2);
            }
        }
    }
}

#endif

