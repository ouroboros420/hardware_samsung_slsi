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

#include <string.h>
#include <csp_os.h>
#include <csp_common.h>
#include <csp_printf.h>
#include <ipc_chub.h>
#include <plat/rtc.h>
#include <debug.h>
#include <platform.h>
#include <mailbox.h>
#include <mailboxDrv.h>
#include <mailboxOS.h>
#include <pwm.h>
#include <cmsis.h>

struct RxCBInfo {
    mailboxRxCB callback;
    void *buf;
    void *cookie;
} mRxCB;

struct rxbuf {
    u8 pad;
    u8 pre_preamble;
    u8 buf[PACKET_SIZE_MAX];
    u8 post_preamble;
};

struct __attribute__ ((__packed__)) nanohub_packet {
    uint8_t sync;
    uint32_t seq;
    uint32_t reason;
    uint8_t len;
    uint8_t data[];
};

/*
struct mailboxInfo {
    enum ipc_owner owner;
    uint32_t base;
    enum ipc_direction  dir;
};

struct mailboxInfo mMailboxInfo[] =  {
    {AP, MAILBOX_AP_BASE_ADDRESS, IPC_DST},
    {APM, MAILBOX_APM_BASE_ADDRESS, IPC_DST},
    {CP, MAILBOX_CP_BASE_ADDRESS, IPC_DST},
    {GNSS, MAILBOX_GNSS_BASE_ADDRESS, IPC_DST},
    {VTS, MAILBOX_VTS_BASE_ADDRESS, IPC_DST},
    {WLBT, MAILBOX_WLBT_BASE_ADDRESS, IPC_DST},
  #if (MAILBOX_ABOX_SUPPORT)
    {ABOX, MAILBOX_ABOX_BASE_ADDRESS, IPC_DST},
  #endif
  #if (MAILBOX_NPU_SUPPORT)
    {NPU, MAILBOX_NPU_BASE_ADDRESS, IPC_DST},
  #endif
  #if (MAILBOX_DSP_SUPPORT)
    {DSP, MAILBOX_DSP_BASE_ADDRESS, IPC_DST},
  #endif
};
*/

/* this function is to check the interval between int_wakeup and ap sleep */
u64 mLastWakeup_time;
static void UpdateWakeupInterruptCnt(void *buf)
{
    struct rxbuf *rxstruct = (struct rxbuf *)buf;
    struct nanohub_packet *packet;
    struct HostIntfDataBuffer *hostBuf;

    packet = (void *)&rxstruct->pre_preamble;
    hostBuf = (void *)packet->data;

    if (hostBuf->firstSample.interrupt == NANOHUB_INT_WAKEUP) {
        mLastWakeup_time = getTimeStampUS();
        CSP_PRINTF_DEBUG("%s, update wu:%lld\n", MAILBOX_DRV, mLastWakeup_time);
    }
}

#if defined(DEF_IPC_TIMESTAMP)
#ifdef IPC_DUMP
static void DumpBuf(struct nanohub_packet *packet)
{
    char *tmpbuf;

    tmpbuf = (char *)packet->data;
    osLog(LOG_INFO, "D:%x-%x, d:%x,%x,%x,%x,%x,%x,%x,%x\n", (int)packet->reason,
          (int)packet->len, tmpbuf[0], tmpbuf[1], tmpbuf[2], tmpbuf[3],
          tmpbuf[4], tmpbuf[5], tmpbuf[6], tmpbuf[7]);
}
#endif

static void debugSetIpcInfo(uint8_t * buf, bool read)
{
    struct rxbuf *rxstruct = (struct rxbuf *)buf;
    struct nanohub_packet *packet;
#ifdef DEF_IPC_TIMESTAMP
    struct cipc_data *ipc_data;
    struct cipc_evt *ipc_evt;
    u64 *utime = NULL;
#endif

#ifdef DEF_IPC_TIMESTAMP
    if (read == 0)
        packet = (void *)&rxstruct->pre_preamble;
    else
        packet = (void *)rxstruct;

    if (packet->reason == NANOHUB_REASON_READ_EVENT) {
        if (read)               /* update last A2C */
            ipc_data = cipc_get_base(CIPC_REG_DATA_AP2CHUB);
        else                    /* update last C2A */
            ipc_data = cipc_get_base(CIPC_REG_DATA_CHUB2AP);

        utime = (u64 *) & ipc_data->ctrl.qctrl.time;
    } else {                    /*  if (packet->reason == NANOHUB_REASON_WRITE_EVENT)  */
        if (read)               /* update last A2C */
            ipc_evt = cipc_get_base(CIPC_REG_EVT_AP2CHUB);
        else                    /* update last C2A */
            ipc_evt = cipc_get_base(CIPC_REG_EVT_CHUB2AP);

        utime = (u64 *) & ipc_evt->ctrl.qctrl.time;
    }

    *utime = getTimeStampUS();
#endif

#ifdef IPC_DUMP
    osLog(LOG_INFO, "(((%p, %d, r:0x%x)\n", buf, read, (u32) packet->reason);
    DumpBuf(buf);
#endif

}
#else
#define debugSetIpcInfo(a, b) ((void)0)
#endif

void mailboxDrvHandleRxMessage(int irq_num)
{
    void *rxbuf;
    u32 ret;

    if (mRxCB.buf) {
        rxbuf = cipc_read_data(CIPC_REG_DATA_AP2CHUB, &ret);
        if (rxbuf) {
            memcpy(mRxCB.buf, rxbuf, ret);
            ret = 0;
        } else {
            ret = -1;
        }
        debugSetIpcInfo(rxbuf, 1);
        if (mRxCB.callback)
            mRxCB.callback(mRxCB.cookie, mRxCB.buf, ret);
        else
            CSP_PRINTF_ERROR("%s: no CB: %p, %p\n", MAILBOX_DRV, mRxCB.buf, mRxCB.callback);
    }
}

int32_t mailboxDrvRead(void *buf, uint32_t size, mailboxRxCB callback, void *cookie)
{
    if ((buf == NULL) || (size == 0) || (callback == NULL)) {
        CSP_PRINTF_ERROR("%s err:%p, %d, %p\n", __func__, buf, (int)size, callback);
        return -1;
    }

    (void)size;
    mRxCB.callback = callback;
    mRxCB.cookie = cookie;
    mRxCB.buf = buf;

    return 0;
}

void mailboxDrvWriteEvent(enum mailbox_event event, uint32_t val)
{
    enum irq_evt_chub evt;
    struct ipc_info *ipc = ipc_get_info();

    perfMonitorSetEvt(chub_prof_ipc_wt_evt_in);

    switch (event) {
    case MAILBOX_EVT_WAKEUP:
        evt = val ? IRQ_EVT_C2A_INT : IRQ_EVT_C2A_INTCLR;
		cipc_add_evt(CIPC_REG_EVT_CHUB2AP, evt);
        break;
    case MAILBOX_EVT_DEBUG:
        ipc_write_value(IPC_VAL_C2A_DEBUG, (enum ipc_debug_event)val);
        evt = IRQ_EVT_C2A_DEBUG;
		cipc_add_evt(CIPC_REG_EVT_CHUB2AP, evt);
        break;
    case MAILBOX_EVT_REBOOT:
        ipc_write_hw_value(IPC_VAL_HW_AP_STATUS, CHUB_REBOOT_REQ);
        ipc_hw_gen_interrupt(ipc->mb_base, ipc->opp_mb_id, IRQ_NUM_CHUB_LOG);
        break;
    default:
        CSP_PRINTF_DEBUG("%s: invalid event %d, val %u\n", __func__, event, (u32) val);
        break;
    };

    perfMonitorSetEvt(chub_prof_ipc_wt_evt_out);
}

#ifdef DEF_IPC_MONITOR
u64 mNumIpcOut;
#endif

int32_t mailboxDrvWrite(const void *buf, uint32_t size)
{
    int32_t ret;
    if ((buf == NULL) || (size == 0)) {
        CSP_PRINTF_ERROR("%s err:%p, %d\n", __func__, buf, (int)size);
        return -1;
    }
    perfMonitorSetEvt(chub_prof_ipc_wt_in);
#ifdef DEF_IPC_MONITOR
    mNumIpcOut++;
#endif
    debugSetIpcInfo((void *)buf, 0);
    UpdateWakeupInterruptCnt((void *)buf);
    ret = cipc_write_data(CIPC_REG_DATA_CHUB2AP, (void *)buf, size);
    perfMonitorSetEvt(chub_prof_ipc_wt_out);

    return ret;
}

void mailboxDrvEnable(void)
{
    if (!ipc_init(IPC_OWN_CHUB, IPC_DST_MB1, 0, (void *)MAILBOX_AP_BASE_ADDRESS, NULL)) {
        CSP_PRINTF_ERROR("%s: ipc_init failed\n", __func__);
        return;
    }
}

void mailboxDrvDisable(void)
{
    ipc_hw_set_mcuctrl((void *)MAILBOX_AP_BASE_ADDRESS, 0x1);
    ipc_hw_clear_all_int_pend_reg((void *)MAILBOX_AP_BASE_ADDRESS, IPC_SRC_MB0);
    ipc_hw_clear_all_int_pend_reg((void *)MAILBOX_AP_BASE_ADDRESS, IPC_DST_MB1);
}
