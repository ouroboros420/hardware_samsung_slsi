/*
 * Copyright (C) 2018 Samsung Electronics Co., Ltd.
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

#include <csp_printf.h>
#include <mailboxGnssDrv.h>
#include <ipc_chub.h>
#include <gnssIpc.h>
#include <csp_os.h>
#include <csp_common.h>
#include <pwrDrv.h>

#if defined(MBGNSS_REQUIRED)
enum evt_chub_gnss {
    IRQ_EVT_GNSS_CH0,
    IRQ_EVT_GNSS_CH1,
    IRQ_EVT_GNSS_CH2,
    IRQ_EVT_GNSS_CH3,
    IRQ_EVT_GNSS_CH4,
    IRQ_EVT_GNSS_CH5,
    IRQ_EVT_GNSS_CH6,
    IRQ_EVT_GNSS_CH7,
    IRQ_EVT_GNSS_CH8,
    IRQ_EVT_GNSS_CH9,
    IRQ_EVT_GNSS_CH10,
    IRQ_EVT_GNSS_CH11,
    IRQ_EVT_GNSS_CH12,
    IRQ_EVT_GNSS_CH13,
    IRQ_EVT_GNSS_CH14,
    IRQ_EVT_GNSS_CH15,
    IRQ_EVT_GNSS_CH_MAX,
};

struct mailboxGnssIsr {
    void (*func)(void);
}GnssIsrList[IRQ_EVT_GNSS_CH_MAX];

void mailboxGnssDrvSetIsr(int event, void *isr)
{
    GnssIsrList[event].func = (void (*)(void))isr;
}

void mailboxGnssDrvClrIsr(int event)
{
    GnssIsrList[event].func = NULL;
}
void mailboxGnssDrvGenInterupt(unsigned int event)
{
    ipc_hw_gen_interrupt((void *)MAILBOX_GNSS_BASE_ADDRESS, IPC_SRC_MB0, event);
}

void mailboxGnssDrvClrInterupt(unsigned int event)
{
    ipc_hw_clear_int_pend_reg((void *)MAILBOX_GNSS_BASE_ADDRESS, IPC_DST_MB1, event);
}

void mailboxGnssDrvMaskIrq(int irq)
{
    //ipc_hw_mask_irq(GNSS, irq);
}

void mailboxGnssDrvUnmaskIrq(int irq)
{
    //ipc_hw_unmask_irq(GNSS, irq);
}

void mailboxGnssDrvhandleIrq(unsigned int event)
{
    mailboxGnssDrvClrInterupt(event);
    if (GnssIsrList[event].func == NULL)
    {
        CSP_PRINTF_ERROR("%s: Wrong interrupt happened(event: %d)\n",__func__, event);
        return;
    }
    else
    {
        GnssIsrList[event].func();
        if (event == IRQ_EVT_GNSS_CH0 || event == IRQ_EVT_GNSS_CH1)
        {
            /* Send Message( Memory Lock ACK ) since the Message( Memory Lock ) received */
            mailboxGnssDrvGenInterupt((unsigned int)event);
        }
    }
}

void mailboxGnssDrvEnable(void)
{
    //CHUB shall do nothing for initialization of mailbox since it is handled by Master.
    //ipc_set_owner(GNSS, (void *)MAILBOX_GNSS_BASE_ADDRESS, IPC_DST);

    gnssApiCHubInit();
    //CSP_PRINTF_ERROR("[nanohub] mailboxGnssDrvEnable called\n");
    /* Register Callback */
    mailboxGnssDrvSetIsr((int)IRQ_EVT_GNSS_CH0, (void *)pwrDrvEnableGnssWakeLock);
    mailboxGnssDrvSetIsr((int)IRQ_EVT_GNSS_CH1, (void *)pwrDrvDisableGnssWakeLock);
    mailboxGnssDrvSetIsr((int)IRQ_EVT_GNSS_CH2, (void *)gnssIpcMsgCallback);
}

#endif

