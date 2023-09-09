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

#include <csp_os.h>
#include <csp_common.h>
#include <cmsis.h>
#include <mailbox.h>
#include <mailboxDrv.h>
#include <mailboxApm.h>
#include <ipc_hw.h>
#include <plat/cpu/cpu.h>
#include <sysreg.h>
#if defined(SEOS)
  #include <seos.h>
#endif

#define APM_INT_CM55_RESET      1
#define APM_INT_DPU_ONOFF       2
#define APM_INT_MIF_ONOFF       3
#define APM_INT_GPIO_RETENTION  4

enum MAILBOX_APM_EVENT {
    MAILBOX_WAKEUP_REQ,
    MAILBOX_WAKEUP_REQ_ACK,
    MAILBOX_SLEEPREADY_REQ,
    MAILBOX_SLEEPREADY_REQ_ACK,
    MAILBOX_SLEEPREADY_REQ_EINT,
    MAILBOX_SLEEPREADY_REQ_EINT_ACK,
    MAILBOX_POWER_MODE_MAX,
    EVT_APM2CHUB_MAX
};

static int mEvtMifRequest;
static int mEvtDpuRequest;
static uint32_t cmdCount;
static struct APMOps *ops;
void (*mCmgpTimerHandler)(void) = NULL;

int32_t mailboxApmGetSleepReady(void)
{
    return 1;
}

void mailboxApmWriteEvent(enum mailbox_event event)
{
    if (event == MAILBOX_EVT_SLEEP) {
    }
}

static void mailboxApmSendCommand(uint32_t intNum, uint32_t cmd)
{
    ipc_hw_write_shared_reg((void *)MAILBOX_APM_BASE_ADDRESS, cmd, 0);
    ipc_hw_gen_interrupt((void *)MAILBOX_APM_BASE_ADDRESS, IPC_SRC_MB0 , intNum);
}

void mailboxApmMIFEnable(bool enableDRAM)
{
    if (enableDRAM) {
        sysregSetMIFReq(true);
    } else {
        mailboxApmSendCommand(APM_INT_MIF_ONOFF, APM_MSG_MIF_ONLY_ON);
    }
}

void mailboxApmMIFDisable(bool enableDRAM)
{
    if (enableDRAM) {
        sysregSetMIFReq(false);
    } else {
        mailboxApmSendCommand(APM_INT_MIF_ONOFF, APM_MSG_MIF_ONLY_OFF);
    }
}

static void MIFReq_IRQHandler(void)
{
    uint32_t state;

    state = sysregGetMIFReqAck() & 0x10;
    sysregClrMIFReqAck();

    CSP_PRINTF_INFO("MIF_REQ IRQ : %lu\n", (state >> 4));
    if (state) {
        ops->dramOn();
    } else {
        ops->dramOff();
    }
}

void mailboxApmCM55ResetRelease(void)
{
    mailboxApmSendCommand(APM_INT_CM55_RESET, APM_MSG_CM55_RESET_RELEASE);
}

void mailboxApmDPUEnable(void)
{
    mailboxApmSendCommand(APM_INT_DPU_ONOFF, APM_MSG_DPU_ON);
}

void mailboxApmDPUDisable(void)
{
    mailboxApmSendCommand(APM_INT_DPU_ONOFF, APM_MSG_DPU_OFF);
}

void mailboxApmGPIORetentionEnable(void)
{
    mailboxApmSendCommand(APM_INT_GPIO_RETENTION, APM_MSG_GPIO_RETENTION_ON);
}

void mailboxApmGPIORetentionDisable(void)
{
    mailboxApmSendCommand(APM_INT_GPIO_RETENTION, APM_MSG_GPIO_RETENTION_OFF);
}

void mailboxApmSetOps(struct APMOps *p)
{
    ops = p;
}

void mailboxApmEnable(void)
{
    cpuRegisterHandler(MB_APM_IRQn, mailboxAPM_IRQHandler);
    cpuRegisterHandler(MIF_REQ_ACK_IRQn, MIFReq_IRQHandler);
    NVIC_EnableIRQ(MB_APM_IRQn);
    NVIC_EnableIRQ(MIF_REQ_ACK_IRQn);
}

void mailboxAPMSetTimerHandler(void (*handler)(void))
{
    mCmgpTimerHandler = handler;
}

void mailboxAPM_IRQHandler(void)
{
    uint32_t response;
    uint32_t intState;

    response = ipc_hw_read_shared_reg((void *)MAILBOX_APM_BASE_ADDRESS, 2);
    intState = ipc_hw_read_int_status_reg_all((void *)MAILBOX_APM_BASE_ADDRESS, IPC_DST_MB1);
    ipc_hw_clear_all_int_pend_reg((void *)MAILBOX_APM_BASE_ADDRESS, IPC_DST_MB1);

    //CSP_PRINTF_ERROR("APM_IRQHandler : 0x%x - 0x%x\n", (unsigned int)intState, (unsigned int)response);
    if ((response & 0xFFFF0000) != 0x0) {
        CSP_PRINTF_ERROR("APM_IRQHandler : abnormal RESP (0x%08x)\n", (unsigned int)response);
    } else {
        if (intState & 0x1) {
        } else if (intState & 0x2) {
            if (ops->resetRelease != NULL)
                ops->resetRelease();
        } else if (intState & 0x4) {
            if (response & APM_MSG_DPU_ON) {
                if (ops->dpuOn != NULL)
                    ops->dpuOn();
            } else {
                if (ops->dpuOff != NULL)
                    ops->dpuOff();
            }
        }
    }
}


#endif
