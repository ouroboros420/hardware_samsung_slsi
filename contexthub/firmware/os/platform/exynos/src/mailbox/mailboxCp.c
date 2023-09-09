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

#include "csp_common.h"
#include "ipc_hw.h"
#include "plat/cpu/cpu.h"
#include "cmsis.h"
#include "mailboxCp.h"

#define MAX_MAILBOX_CP_SR_NUM  4
#define MAX_MAILBOX_INT_NUM    16

static void (*mMailboxCpCallback[16])(void);

void mailboxCpWriteData(uint32_t num, uint32_t data)
{
    if (num >= MAX_MAILBOX_CP_SR_NUM)
        return;
    ipc_hw_write_shared_reg((void *)MAILBOX_CP_BASE_ADDRESS, data, num);
}

uint32_t mailboxCpReadData(uint32_t num)
{
    if (num >= MAX_MAILBOX_CP_SR_NUM)
        return 0;

    return ipc_hw_read_shared_reg((void *)MAILBOX_CP_BASE_ADDRESS, num);
}

void mailboxCpSendInterrupt(uint32_t int_num)
{
    if (int_num >= MAX_MAILBOX_INT_NUM)
        return;
    ipc_hw_gen_interrupt((void *)MAILBOX_CP_BASE_ADDRESS, IPC_SRC_MB0 , int_num);
}

void mailboxCpSendMessage(uint32_t int_num, uint32_t sr, uint32_t msg)
{
    mailboxCpWriteData(sr, msg);
    mailboxCpSendInterrupt(int_num);
}

void mailboxCpSetCallback(uint32_t num, void (*callback)(void))
{
    if (num >= MAX_MAILBOX_INT_NUM)
        return;

    mMailboxCpCallback[num] = callback;
}

void mailboxCP_IRQHandler(void)
{
    uint32_t intState;

    intState = ipc_hw_read_int_status_reg_all((void *)MAILBOX_CP_BASE_ADDRESS, IPC_DST_MB1);
    ipc_hw_clear_all_int_pend_reg((void *)MAILBOX_CP_BASE_ADDRESS, IPC_DST_MB1);

    for (int i = 0 ; i < 16 ; i++) {
        if (intState & (0x1 << i) && mMailboxCpCallback[i] != NULL) {
            mMailboxCpCallback[i]();
        }
    }
}

void mailboxCpInit(void)
{
    ipc_hw_clear_all_int_pend_reg((void *)MAILBOX_CP_BASE_ADDRESS, IPC_DST_MB1);
    cpuRegisterHandler(MB_CP_IRQn, mailboxCP_IRQHandler);
    NVIC_EnableIRQ(MB_CP_IRQn);
}

