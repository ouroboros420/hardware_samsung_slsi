
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
#include <cmu.h>
#include <dvfs/dvfs.h>
#if defined(SEOS)
  #include <seos.h>
#endif

#define APM_DVFS_ID_FVP             0x2

#define APM_DVFS_DOMAIN_ID_CHUB     0x19
#define APM_DVFS_DOMAIN_ID_uFD      0x1C

#define APM_PLUGIN_UFD              0x0
#define APM_PLUGIN_DVFS             0x1

#define APM_DEBUG_ENABLE  1

#if (APM_DEBUG_ENABLE)
  #define APM_DEBUG(fmt, ...) do { \
        CSP_PRINTF_INFO("[APM]" fmt, ##__VA_ARGS__); \
    } while (0);
#else
  #define APM_DEBUG(fmt, ...)
#endif

#define APM_INFO(fmt, ...) CSP_PRINTF_INFO("[APM]" fmt, ##__VA_ARGS__)
#define APM_ERROR(fmt, ...) CSP_PRINTF_ERROR("[APM]" fmt, ##__VA_ARGS__)

int32_t mailboxApmGetSleepReady(void)
{
    return 1;
}

void mailboxApmWriteEvent(enum mailbox_event event)
{
    (void)event;
}

void mailboxApmReqDvsFreq(uint32_t targetFreq)
{
    uint32_t sr0, sr1;
    static uint8_t seq;

    seq = ((seq + 1) & 0x3F);

    sr0 = APM_DVFS_DOMAIN_ID_CHUB | (seq << 16) | (APM_DVFS_ID_FVP << 26);
    sr1 = targetFreq / 1000;

    APM_DEBUG("DVFS request freq %lu\n", targetFreq);
    ipc_hw_write_shared_reg((void *)MAILBOX_APM_BASE_ADDRESS, sr0, 0);
    ipc_hw_write_shared_reg((void *)MAILBOX_APM_BASE_ADDRESS, sr1, 1);
    ipc_hw_gen_interrupt((void *)MAILBOX_APM_BASE_ADDRESS, IPC_SRC_MB0 , APM_PLUGIN_DVFS);
}

void mailboxApmEnable(void)
{
    cpuRegisterHandler(MB_APM_IRQn, mailboxAPM_IRQHandler);
    NVIC_EnableIRQ(MB_APM_IRQn);
}

void mailboxAPM_IRQHandler(void)
{
    uint32_t sr2, sr3;
    uint32_t intState, seq;
    int err;

    sr2 = ipc_hw_read_shared_reg((void *)MAILBOX_APM_BASE_ADDRESS, 2);
    sr3 = ipc_hw_read_shared_reg((void *)MAILBOX_APM_BASE_ADDRESS, 3);
    intState = ipc_hw_read_int_status_reg_all((void *)MAILBOX_APM_BASE_ADDRESS, IPC_DST_MB1);
    ipc_hw_clear_all_int_pend_reg((void *)MAILBOX_APM_BASE_ADDRESS, IPC_DST_MB1);

    if (intState & (1 << 0)) {

    } else if (intState & (1 << 1)) {
        seq = (sr2 >> 16) & 0x3F;
        err = sr3 >> 24;
        if (err) {
            APM_ERROR("DVFS request fail\n");
            return;
        }

        APM_DEBUG("DVFS requeset success!\n");
#if defined(ALIVE_DVS_SUPPORT)
        dvfs_IRQHandler();
#endif
    } else {
        APM_ERROR("mailboxAPM_IRQHandler: abnormal interrupt 0x%X\n", (unsigned int)intState);
    }
}

#endif
