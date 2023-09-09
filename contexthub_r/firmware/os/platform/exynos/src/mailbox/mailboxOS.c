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

#include <mailboxOS.h>
#include <mailboxDrv.h>
#include <mailboxApmDrv.h>
#if defined(MBGNSS_REQUIRED)
#include <mailboxGnssDrv.h>
#endif
#include <csp_common.h>
#include <csp_os.h>
#include <csp_printf.h>
#include <csp_assert.h>
#include <ipc_chub.h>
#include <pwrDrv.h>
#include <sysreg.h>
#include <mailboxOS.h>
#include <debug.h>

#if defined(SEOS)
#include <platform.h>
#include <nanohubCommand.h>
#include <hostIntf.h>
#include <cmsis.h>
#include <plat/rtc/rtc.h>
#include <timer.h>
#include <algos/ap_hub_sync.h>
#if defined(HEAP_DEBUG)
#include <heapDebug.h>
#endif
#endif

#define SUPPORT_LOOPBACKTEST
extern bool mLoopbackTest;

void mailboxAPHandleIRQ(int evt, void *priv)
{
    perfMonitorSetEvt(chub_prof_ipc_rd_in);

    switch (evt) {
    case IRQ_EVT_A2C_WAKEUP:
#if defined(LOCAL_POWERGATE)
        pwrDrvEnableApWakeLock();
#endif
#if defined(SEOS)
        hostIntfRxPacket(true);
#endif
        break;
    case IRQ_EVT_A2C_WAKEUP_CLR:
#if defined(LOCAL_POWERGATE)
        pwrDrvDisableApWakeLock();
#endif
#if defined(SEOS)
        hostIntfRxPacket(false);
#endif
        break;
    case IRQ_EVT_A2C_DEBUG:
        debugHandler();
        break;
	case CIPC_REG_DATA_AP2CHUB:
#ifdef SUPPORT_LOOPBACKTEST
		if (mLoopbackTest) {
			cipc_loopback_test(CIPC_REG_DATA_AP2CHUB, 0);
			mLoopbackTest = 0;
			return;
		}
#endif
		mailboxDrvHandleRxMessage(evt);
		break;
	default:
        CSP_PRINTF_ERROR("%s: invalid evt:%d\n", __func__, evt);
        break;
    }
    perfMonitorSetEvt(chub_prof_ipc_rd_out);
}

extern u64 mLastWakeup_time;
u32 mWakeupOnAPSleepCnt;
#define MAX_IPC_HANDLE_THRES_HOLD (600000) /* The maximum timeout on AP is 512(timeout_reply) ms */
static struct cipc_info *cipc;

void mailboxAP_IRQHandler(void)
{
    enum ipc_mb_id my_mb_id = cipc->user_info[CIPC_USER_AP2CHUB].map_info.dst.mb_id;
    enum ipc_mb_id opp_mb_id = cipc->user_info[CIPC_USER_AP2CHUB].map_info.src.mb_id;
    void *mb_base = cipc->user_info[CIPC_USER_AP2CHUB].mb_base;
    unsigned int status = ipc_hw_read_int_status_reg_all(mb_base, my_mb_id);
    int start_index = ipc_hw_start_bit(my_mb_id);
    int irq_num = IRQ_NUM_CHUB_ALIVE + start_index;
    u64 diffTime = 0;
    int log;

    /* chub alive interrupt handle */
    if (status & (1 << irq_num)) {
        status &= ~(1 << irq_num);
        ipc_hw_clear_int_pend_reg(mb_base, my_mb_id, irq_num);

        log = (u32)ipc_read_hw_value(IPC_VAL_HW_AP_STATUS);
        if (log) {
            ipc_set_ap_wake(log);
            if (log == AP_SLEEP)
                apHubSyncReset(NULL);
        }
        ipc_write_hw_value(IPC_VAL_HW_AP_STATUS, 0);

        /* fix the wakeup interrupt is not handled during ap sleep */
        if (log == AP_SLEEP) {
            diffTime = getTimeStampUS() - mLastWakeup_time;

            if (diffTime < MAX_IPC_HANDLE_THRES_HOLD) {
                mWakeupOnAPSleepCnt++;
                /* Don't let AP sleep */
                osLog(LOG_INFO, "%s: ap enter sleep: Wakeup(%d):%lld,%lld\n",
                      __func__, mWakeupOnAPSleepCnt, (u64) mLastWakeup_time,
                      diffTime);
                /* Try it onetime to avoid frequenct AP early wakeup */
                mLastWakeup_time = 0;
                ipc_hw_gen_interrupt(mb_base, opp_mb_id, IRQ_NUM_CHUB_ALIVE);
            }
            hostIntfSetInterruptMask(NANOHUB_INT_NONWAKEUP);
        } else {
		    /* give ack for AP_WAKE */
            ipc_hw_gen_interrupt(mb_base, opp_mb_id, IRQ_NUM_CHUB_ALIVE);
            hostIntfClearInterruptMask(NANOHUB_INT_NONWAKEUP);
        }
        osLog(LOG_INFO, "%s:ap enter:%d, %s,LastW:(%lld,%lld)\n",
              MAILBOX_DRV, (int)log,
              (ipc_get_ap_wake() == AP_WAKE) ? "wake" : "sleep",
              (u64) mLastWakeup_time / 1000, diffTime / 1000);
    }

    if (status)
        cipc_irqhandler(CIPC_USER_AP2CHUB, status);
}

void mailboxOsEnable(void)
{
    NVIC_ClearPendingIRQ(MB_AP_IRQn);
    NVIC_EnableIRQ(MB_AP_IRQn);

#ifdef CIPC_DEF_USER_ABOX
    NVIC_ClearPendingIRQ(MB_ABOX_IRQn);
    NVIC_EnableIRQ(MB_ABOX_IRQn);
#endif
#if defined(MBGNSS_REQUIRED)
    NVIC_ClearPendingIRQ(MB_GNSS_IRQn);
    NVIC_EnableIRQ(MB_GNSS_IRQn);
#endif
#if defined(APM_HANDSHAKING)
    NVIC_EnableIRQ(MB_APM_IRQn);
#endif
	cipc = cipc_get_info();
}
