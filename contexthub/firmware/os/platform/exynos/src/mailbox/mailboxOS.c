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
#include <plat/rtc/rtc.h>
#if defined(LPD_SUPPORT)
  #include <ipc_lpd.h>
#endif

#if defined(SEOS)
  #include <platform.h>
  #include <nanohubCommand.h>
  #include <hostIntf.h>
  #include <cmsis.h>
  #include <timer.h>
  #include <algos/ap_hub_sync.h>
  #include <seos.h>
  #if defined(HEAP_DEBUG)
    #include <heapDebug.h>
  #endif
#elif defined(EMBOS)
  #include <RTOS.h>
#endif
#include <dvfs/dvfs.h>

#define SUPPORT_LOOPBACKTEST
extern bool mLoopbackTest;

void mailboxAPHandleIRQ(int evt, void *priv)
{
    perfMonitorSetEvt(chub_prof_ipc_rd_in);

    switch (evt) {
    case IRQ_EVT_A2C_WAKEUP:
#if defined(SEOS)
        hostIntfRxPacket(true);
#endif
        break;
    case IRQ_EVT_A2C_WAKEUP_CLR:
#if defined(SEOS)
        hostIntfRxPacket(false);
#endif
        break;
    case IRQ_EVT_A2C_DEBUG:
        debugHandler();
        break;
#if defined(LPD_SUPPORT)
    case IRQ_EVT_A2C_LPD_START:
    {
        union time64_t time;
        time.time64 = ipc_read_hw_value(IPC_VAL_HW_DEBUG);
        CSP_PRINTF_INFO("LPD_START_REQ : %d/%d/%d %d:%d:%d\n", 1900+time.tm_year,
                         time.tm_mon + 1, time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec);
        rtcSetTime64(time);
  #if defined(SEOS)
        ipc_lpd_send_event(EVT_LPD_START);
  #elif defined(EMBOS)
        ipc_lpd_send_event(EVENT_LPD_START);
  #endif
        break;
    }
    case IRQ_EVT_A2C_LPD_END:
  #if defined(SEOS)
        ipc_lpd_send_event(EVT_LPD_STOP);
  #elif defined(EMBOS)
        ipc_lpd_send_event(EVENT_LPD_STOP);
  #endif
        break;
#endif
        break;
    case IRQ_EVT_A2C_AON_START:
#if defined(AONCAM_SUPPORT)
#endif
	break;
    case IRQ_EVT_A2C_AON_STOP:
#if defined(AONCAM_SUPPORT)
#endif
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
static u32 mWakeupOnAPSleepCnt;
#define MAX_IPC_HANDLE_THRES_HOLD (600000) /* The maximum timeout on AP is 512(timeout_reply) ms */
static struct cipc_info *cipc;

void mailboxAP_IRQHandler(void)
{
    enum ipc_mb_id my_mb_id = (enum ipc_mb_id)cipc->user_info[CIPC_USER_AP2CHUB].map_info.dst.mb_id;
    enum ipc_mb_id opp_mb_id = (enum ipc_mb_id)cipc->user_info[CIPC_USER_AP2CHUB].map_info.src.mb_id;
    void *mb_base = cipc->user_info[CIPC_USER_AP2CHUB].mb_base;
    unsigned int status = ipc_hw_read_int_status_reg_all(mb_base, my_mb_id);
    int start_index = ipc_hw_start_bit(my_mb_id);
    int irq_num = IRQ_NUM_CHUB_ALIVE + start_index;
    u64 diffTime = 0;
    int log;

#if defined(EMBOS)
	OS_EnterNestableInterrupt();// os_error_fix_1
#endif
    /* chub alive interrupt handle */
    if (status & (1 << irq_num)) {
        status &= ~(1 << irq_num);
        ipc_hw_clear_int_pend_reg(mb_base, my_mb_id, irq_num);

        log = (u32)ipc_read_hw_value(IPC_VAL_HW_AP_STATUS);

        if (log == AP_SLEEP) {
            ipc_set_ap_wake(log);
#if defined(SEOS)
            apHubSyncReset(NULL);
            hostIntfSetInterruptMask(NANOHUB_INT_NONWAKEUP);
#endif
            diffTime = getTimeStampUS() - mLastWakeup_time;

            if (diffTime < MAX_IPC_HANDLE_THRES_HOLD) {
                mWakeupOnAPSleepCnt++;
                /* Don't let AP sleep */
                osLog(LOG_INFO, "%s: ap enter sleep: Wakeup(%d):%lld,%lld\n",
                        __func__, mWakeupOnAPSleepCnt, (u64) mLastWakeup_time,
                        diffTime);
                /* Try it onetime to avoid frequenct AP early wakeup */
                mLastWakeup_time = 0;
                //ipc_hw_gen_interrupt(mb_base, opp_mb_id, IRQ_NUM_CHUB_ALIVE);
            }
        }
        else if (log == CHUB_PG_IN) {
            pwrDrvDisableApWakeLock();
#if defined(ALIVE_DVS_SUPPORT)
            dvfsRequestFreq(100000000);
#endif

        }
        else if (log == CHUB_PG_OUT) {
            pwrDrvEnableApWakeLock();
#if defined(ALIVE_DVS_SUPPORT)
            dvfsRequestFreq(400000000);
#endif
            /* give ack for AP_WAKE */
            ipc_hw_gen_interrupt(mb_base, opp_mb_id, IRQ_NUM_CHUB_ALIVE);
        }
        else if (log == AP_WAKE) {
            ipc_set_ap_wake(log);
#if defined(SEOS)
            hostIntfClearInterruptMask(NANOHUB_INT_NONWAKEUP);
#endif
            /* give ack for AP_WAKE */
            ipc_hw_gen_interrupt(mb_base, opp_mb_id, IRQ_NUM_CHUB_ALIVE);
        }
        else {
            CSP_PRINTF_ERROR("%s: wrong ap wake val:%d\n", __func__, log);
        }

        ipc_write_hw_value(IPC_VAL_HW_AP_STATUS, 0);

        osLog(LOG_INFO, "%s:ap enter:%d, %s,LastW:(%lld,%lld)\n",
                MAILBOX_DRV, (int)log, (ipc_get_ap_wake() == AP_WAKE) ? "wake" : "sleep",
                (u64) mLastWakeup_time / 1000, diffTime / 1000);
    }

    if (status)
        cipc_irqhandler(CIPC_USER_AP2CHUB, status);
#if defined(EMBOS)
    OS_LeaveNestableInterrupt();// os_error_fix_1
#endif
}

#if defined(MBGNSS_REQUIRED)
void MailboxGnss2Chub_IRQHandler(void)
{
    uint32_t num, i;
#if defined(EMBOS)
    OS_EnterNestableInterrupt();// os_error_fix_1
#endif
    //CSP_PRINTF_INFO("[nanohub] MBGnss2Chub_IRQHandler I\n");
    num = ipc_hw_read_int_status_reg_all((void *)MAILBOX_GNSS_BASE_ADDRESS, IPC_DST_MB1);
    int start_index = ipc_hw_start_bit(IPC_DST_MB1);

    for (i = start_index; i < start_index + IPC_HW_IRQ_MAX; i++) {
        if (num & (1 << i)) {
            //CSP_PRINTF_INFO("[nanohub] MBGnss2Chub Loop i:%u\n", (unsigned int)i);
            mailboxGnssDrvhandleIrq((i - start_index));
        }
    }

#if defined(EMBOS)
    OS_LeaveNestableInterrupt();// os_error_fix_1
#endif
}
#endif

void mailboxOsEnable(void)
{
#ifndef EXYNOS_CONTEXTHUB_EXT
    NVIC_ClearPendingIRQ(MB_AP_IRQn);
    NVIC_EnableIRQ(MB_AP_IRQn);
#endif
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
