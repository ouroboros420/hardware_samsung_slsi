/*
 * Copyright (C) 2021 Samsung Electronics Co., Ltd.
 *
 * Contact: Shinkyu Park <shinkyu.park@samsung.com>
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

#include <csp_common.h>
#include <sysreg.h>
#include <ipc_lpd.h>
#include <cmsis.h>
#include <plat/cpu/cpu.h>
#include <ipc_common.h>
#include <ipc_hw.h>
#include <pwrDrv.h>
#include <cmu.h>
#include <cpu.h>
#include <rtc.h>
#include <mailboxApm.h>
#if defined(SEOS)
  #include <platform.h>
#elif defined(EMBOS)
  #include <rtos.h>
#endif

#if !defined(CM55_BASE_ADDRESS)
#error "CM55_BASE_ADDRESS is not defined"
#endif
#if !defined(CM55_IPC_OFFSET)
#error "CM55_IPC_OFFSET is not defined"
#endif


#define LPD_FW_MAGIC_SIZE 16
#define LPD_FW_INFO_MAGIC "LPD Firmware"
#define LPD_FW_IPC_MAGIC "LPD FW IPC"

enum lpd_ipc_command_dir {
        IPC_COMMAND_DIR_TO,        // to CM55
        IPC_COMMAND_DIR_FROM,      // from CM55
        IPC_COMMAND_DIR_MAX
};

char cm55Response[8][16] = {
  "",
  "INVALID",
  "BOOT_DONE",
  "START_DRAW",
  "START_DRAW_ACK",
  "STOP_DRAW",
  "STOP_DRAW_ACK",
  "END_DRAW",
};

enum lpd_ipc_command {
        IPC_COMMAND_INVALID   = 1,   // CM55 <-> CM4
        IPC_COMMAND_BOOT_DONE,       // CM55 -> CM4
        IPC_COMMAND_START_DRAW,      // CM4 -> CM55
        IPC_COMMAND_START_DRAW_ACK,  // CM55 -> CM4
        IPC_COMMAND_STOP_DRAW,       // CM4 -> CM55
        IPC_COMMAND_STOP_DRAW_ACK,   // CM55 -> CM4
        IPC_COMMAND_END_DRAW,        // CM55 -> CM4
        IPC_COMMAND_MAX
};

struct lpd_fw_memory_map {
    struct lpd_fw_dram_map {
        uint32_t        base;
        uint32_t        size;
    } rmem; // reserved DRAM
    struct lpd_fw_sram_map {
        uint32_t        code_start;
        uint32_t        code_end;
        uint32_t        ipc_start;
        uint32_t        ipc_end;
        uint32_t        logbuf_start;
        uint32_t        logbuf_end;
        uint32_t        ram_start;
        uint32_t        ram_end;
        uint32_t        desc_start;
        uint32_t        desc_end;
        uint32_t        img_start;
        uint32_t        img_end;
        uint32_t        dump_start;
        uint32_t        dump_end;
    } sram;
};

struct lpd_fw_info {
    char    magic[16];
    uint8_t    soc[8];
    uint32_t    version;
    uint32_t    meta_ver;
    uint8_t    time[21];
    uint8_t    name[19];
    uint8_t    commit[200];
    struct lpd_fw_memory_map map;
};

struct lpd_fw_ipc_data {
    union time64_t time;
    uint8_t res[9];
};

struct lpd_fw_ipc{
    char magic[LPD_FW_MAGIC_SIZE];
    uint32_t cmd[IPC_COMMAND_DIR_MAX];
    uint8_t busy;
    struct lpd_fw_ipc_data data;
};

enum LpdState {
    LPD_STATE_OFF,
    LPD_STATE_IDLE,
    LPD_STATE_MIF_ON,
    LPD_STATE_DPU_ON,
    LPD_STATE_GPIO_ON,
    LPD_STATE_RUN,
};

char lpdStateString[][8] = {
    "OFF",
    "IDLE",
    "MIF_ON",
    "DPU_ON",
    "GPIO_ON",
    "RUN",
};

enum Cm55State {
    CM55_STATE_OFF,
    CM55_STATE_ON,
    CM55_STATE_IDLE,
    CM55_STATE_RUN,
};

enum Cm55BootMode {
    CM55_BOOT_BL,
    CM55_BOOT_OS,
};

char cm55StateString[][8] = {
    "OFF",
    "ON",
    "IDLE",
    "RUN",
};

enum lpdEvent {
    EVT_LPD_CM55_BOOT,
    EVT_LPD_CM55_DONE,
    EVT_LPD_CM55_IRQ,
};

static enum LpdState mLPDState = LPD_STATE_OFF;
static enum Cm55State mCm55State = CM55_STATE_OFF;
static bool mLPDMode = false;
//static bool mWaitForStart = false;
static uint32_t mLPDPeriod = LPD_PERIOD_1M;
struct lpd_fw_info *lpd_info = (struct lpd_fw_info *)(CM55_BASE_ADDRESS + CM55_IPC_OFFSET);
struct lpd_fw_ipc *lpd_ipc;
static uint32_t mLastMin, mLastSec;
static uint32_t mStoredEntry, mStoredInst;
static uint32_t mDPURetryCnt;
static enum Cm55BootMode  mCM55BootMode = CM55_BOOT_BL;
static union time64_t mTempTime;
static uint32_t mTimeValid;

#if defined(SEOS)
  uint32_t mTid;
#else
  OS_TASK *mTCB;
#endif

#define REG_CHUB_CPU_CM55_CONFIGURATION 0x52863500
#define REG_CHUB_CPU_CM55_STATUS        0x52863504
#define REG_PMU_DPU_STATUS              0x52861D04
#define CM55_TEMP_ENTRYPOINT            (CM55_BASE_ADDRESS + 0x180)
#define CM55_TEMP_STACKPOINT            (CM55_BASE_ADDRESS + 0x1FC)
#define CM55_INSTRUCTION_WFI            0xE7FDBF30

#define LPD_DEBUG_ENABLE  1

#if (LPD_DEBUG_ENABLE)
  #define LPD_IPC_DEBUG(fmt, ...) do { \
        CSP_PRINTF_INFO("[LPD]" fmt, ##__VA_ARGS__); \
    } while (0);
#else
  #define LPD_IPC_DEBUG(fmt, ...)
#endif

static void ipc_lpd_DPU_on(void);
static void ipc_lpd_DPU_off(void);
static void ipc_lpd_DRAM_on(void);
static void ipc_lpd_DRAM_off(void);

static bool ipc_lpd_check_magic(void)
{
    if (strncmp(lpd_info->magic, LPD_FW_INFO_MAGIC, sizeof(LPD_FW_INFO_MAGIC)))
        return false;
    return true;
}

int ipc_lpd_reset_release(void)
{
    if (mCM55BootMode == CM55_BOOT_BL) {
        if (ipc_lpd_check_magic()) {
                mStoredEntry = *(unsigned int*)(CM55_BASE_ADDRESS + 4);
                mStoredInst = *(unsigned int*)(CM55_TEMP_ENTRYPOINT);
        }
        *(unsigned int*)(CM55_BASE_ADDRESS + 4) = CM55_TEMP_ENTRYPOINT + 1;
        *(unsigned int*)(CM55_TEMP_ENTRYPOINT) = CM55_INSTRUCTION_WFI;
    } else {
        if (ipc_lpd_check_magic() == true && mStoredEntry != 0) {
              *(unsigned int*)(CM55_BASE_ADDRESS + 4) = mStoredEntry;
              *(unsigned int*)(CM55_TEMP_ENTRYPOINT) = mStoredInst;
        } else {  // If CM55 image is not loaded
            return -1;
        }
    }

    LPD_IPC_DEBUG("%s\n", __func__);

    pwrDrvEnableCM55BootLock();
    mailboxApmCM55ResetRelease();

    return 0;
}

static void ipc_lpd_handle_reset_release(void)
{
    LPD_IPC_DEBUG("%s\n", __func__);
    if (mCM55BootMode == CM55_BOOT_BL && mStoredEntry != 0) {
       *(unsigned int*)(CM55_BASE_ADDRESS + 4) = mStoredEntry;
       *(unsigned int*)(CM55_TEMP_ENTRYPOINT) = mStoredInst;
    }
    mCm55State = CM55_STATE_ON;
    pwrDrvDisableCM55BootLock();
}

static void ipc_lpd_send_to_CM55(enum lpd_ipc_command cmd)
{
    LPD_IPC_DEBUG("%s : %d\n", __func__, cmd);
    lpd_ipc->cmd[IPC_COMMAND_DIR_TO] = cmd;
    sysregSendIRQtoCM55();
}

void ipc_lpd_start(uint32_t period)
{
    LPD_IPC_DEBUG("ipc_lpd_start\n");

    mLPDPeriod = period;

    if (mLPDState != LPD_STATE_OFF) {
        LPD_IPC_DEBUG("%s : LPD is already started(%s)\n", __func__, lpdStateString[mLPDState]);
        mLPDMode = true;
        return;
    }

    if (ipc_lpd_check_magic() == false) {
        CSP_PRINTF_INFO("%s : check magic failed !!!\n", __func__);
        return;
    }

    /*
    if (mCm55State == CM55_STATE_OFF) {
        ipc_lpd_reset_release();
    }
    */

    mLPDMode = true;
    mLPDState = LPD_STATE_IDLE;
}

bool ipc_lpd_isactive(void)
{
    return mLPDMode;
}

static void ipc_lpd_off(void)
{
    mLPDState = LPD_STATE_OFF;

    LPD_IPC_DEBUG("ipc_lpd_off\n");

    cipc_add_evt(CIPC_REG_EVT_CHUB2AP, IRQ_EVT_A2C_LPD_END);
}

static void ipc_lpd_DRAM_on(void)
{
    LPD_IPC_DEBUG("%s\n", __func__);
    if (mLPDMode == false) {
        mailboxApmDPUEnable();
        return;
    }

    if (sysregGetMIFReqAck() & 0x10) { // If MIF is already On
        mLPDState = LPD_STATE_MIF_ON;
        ipc_lpd_DPU_on();
    } else {
        mailboxApmMIFEnable(true);
    }
}

static void ipc_lpd_DRAM_off(void)
{
    LPD_IPC_DEBUG("%s\n", __func__);

    if (sysregGetMIFReqAck() & 0x10) {
        mailboxApmMIFDisable(true);
    } else { // IF MIF is already off
        mLPDState = LPD_STATE_IDLE;
        if (mLPDMode == false) {
            ipc_lpd_off();
        }
        pwrDrvDisableCM55WakeLock();
    }
}

static void ipc_lpd_DPU_on(void)
{
    LPD_IPC_DEBUG("%s\n", __func__);
    if (mLPDMode == false) {
        ipc_lpd_DRAM_off();
    } else {
        mailboxApmDPUEnable();
    }
}

static void ipc_lpd_DPU_off(void)
{
    LPD_IPC_DEBUG("%s\n", __func__);
    mailboxApmDPUDisable();
}

#if defined(LPD_GPIO_RETENTION_CONTROL)
static void ipc_lpd_GPIO_on(void)
{
    LPD_IPC_DEBUG("%s\n", __func__);
    if (mLPDMode == false) {
        ipc_lpd_DPU_off();
    } else {
        mailboxApmGPIORetentionDisable();
    }
}

static void ipc_lpd_GPIO_off(void)
{
    LPD_IPC_DEBUG("%s\n", __func__);
    mailboxApmGPIORetentionEnable();
}
#endif

static uint32_t ipc_lpd_check_DPU_on(void)
{
    return __raw_readl(REG_PMU_DPU_STATUS);
}

static void ipc_lpd_start_draw(void)
{
    uint64_t intState;

    CSP_PRINTF_INFO("%s\n", __func__);
    intState = cpuIntsOff();

    if (mLPDMode == false) {
        ipc_lpd_DPU_off();
    } else {
        if (ipc_lpd_check_DPU_on() == 0) {
            if (mDPURetryCnt >= 3) {
                ipc_lpd_DPU_off();
            } else {
                mDPURetryCnt++;
                ipc_lpd_DPU_on();
            }
            return;
        }
        ipc_lpd_send_to_CM55(IPC_COMMAND_START_DRAW);
        mLPDState = LPD_STATE_RUN;
    }

    cpuIntsRestore(intState);
}

static void ipc_lpd_handle_DRAM_on(void)
{
    LPD_IPC_DEBUG("%s : %d\n", __func__, mCm55State);

    mLPDState = LPD_STATE_MIF_ON;
    if (mLPDMode == true) {
        if (mCm55State != CM55_STATE_IDLE) {
            mCM55BootMode = CM55_BOOT_OS;
            ipc_lpd_reset_release();
        } else {
            ipc_lpd_DPU_on();
        }
    } else {
        ipc_lpd_DRAM_off();
    }
}

static void ipc_lpd_handle_DRAM_off(void)
{
    LPD_IPC_DEBUG("%s\n", __func__);
    mLPDState = LPD_STATE_IDLE;
    if (mLPDMode == false) {
        // Enable DPU block when stop LPD mode
        mailboxApmDPUEnable();
    }
    pwrDrvDisableCM55WakeLock();
}

static void ipc_lpd_handle_DPU_on(void)
{
    uint64_t intState;
    LPD_IPC_DEBUG("%s\n", __func__);
    mLPDState = LPD_STATE_DPU_ON;
    if (mLPDMode == true) {
        intState = cpuIntsOff();
        if (mCm55State != CM55_STATE_IDLE) {
            //mWaitForStart = true;
        } else {
          #if defined(LPD_GPIO_RETENTION_CONTROL)
            ipc_lpd_GPIO_on();
          #else
            ipc_lpd_start_draw();
          #endif
        }
        cpuIntsRestore(intState);
    } else {
        ipc_lpd_off();
    }
}

static void ipc_lpd_handle_DPU_off(void)
{
    LPD_IPC_DEBUG("%s\n", __func__);
    mLPDState = LPD_STATE_MIF_ON;
    ipc_lpd_DRAM_off();
}

#if defined(LPD_GPIO_RETENTION_CONTROL)
static void ipc_lpd_handle_GPIO_on(void)
{
    LPD_IPC_DEBUG("%s\n", __func__);
    mLPDState = LPD_STATE_GPIO_ON;
    if (mLPDMode == true) {
        ipc_lpd_start_draw();
    } else {
        ipc_lpd_GPIO_off();
    }
}

static void ipc_lpd_handle_GPIO_off(void)
{
    LPD_IPC_DEBUG("%s\n", __func__);
    ipc_lpd_DPU_off();
}
#endif

void ipc_lpd_run(void)
{
    uint64_t start = getTimeStampUS();
    uint64_t intState;

    CSP_PRINTF_INFO("%s : %s/%s =====================\n", __func__, lpdStateString[mLPDState], cm55StateString[mCm55State]);

    if (ipc_get_ap_wake() == AP_WAKE) {
        //CSP_PRINTF_INFO("%s : AP is not in SLEEP state\n", __func__);
        //return;
    }

    if (ipc_lpd_check_magic() == false) {
        CSP_PRINTF_INFO("%s : check magic failed !!!\n", __func__);
        return;
    }

    if (mLPDState != LPD_STATE_IDLE) {
        CSP_PRINTF_INFO("%s: LPD state is not IDLE(%s)\n", __func__, lpdStateString[mLPDState]);
        return;
    }

    /*
    if (mLPDMode == false) {
        CSP_PRINTF_ERROR("%s: LPD is not enabled\n", __func__);
        return;
    }
    */

    /* Set WakeLock not to enter P/G while operating LPD */
    pwrDrvEnableCM55WakeLock();

    if (mCm55State == CM55_STATE_OFF) {
        LPD_IPC_DEBUG("%s: CM55 is OFF, Waiting for boot\n", __func__);
        intState = cpuIntsOn();
        while(mCm55State == CM55_STATE_OFF) {
            if (getTimeStampUS() - start > 50000) {
                CSP_PRINTF_INFO("%s: CM55 boot timeout\n", __func__);
                pwrDrvDisableCM55WakeLock();
                cpuIntsRestore(intState);
                return;
            }
        }
        cpuIntsRestore(intState);
        LPD_IPC_DEBUG("%s: CM55 is ON", __func__);
    }

    if (sysregGetMIFReqAck() & 0x10) {
        mLPDState = LPD_STATE_MIF_ON;
        if (mCm55State == CM55_STATE_IDLE) {
            ipc_lpd_DPU_on();
        } else {
            mCM55BootMode = CM55_BOOT_OS;
            ipc_lpd_reset_release();
        }
    } else {
        ipc_lpd_DRAM_on();
    }
}

void ipc_lpd_stop(void)
{
    LPD_IPC_DEBUG("%s : %d\n", __func__, mLPDState);
    mLPDMode = false;
    if (mLPDState == LPD_STATE_RUN) {
        ipc_lpd_send_to_CM55(IPC_COMMAND_STOP_DRAW);
    } else if (mLPDState == LPD_STATE_IDLE) {
        // Enable DPU Block when stop LPD mode
        mailboxApmDPUEnable();
    }
}

static void ipc_lpd_cm55_boot_done(void)
{
    uint64_t intState;

    CSP_PRINTF_INFO("CM55 BOOT Done\n");

    intState = cpuIntsOff();
    mCm55State = CM55_STATE_IDLE;

    if (mTimeValid) {
        lpd_ipc->data.time.time64 = mTempTime.time64;
        mTimeValid = 0;
    }

    cpuIntsRestore(intState);

    if (mLPDMode) {
        ipc_lpd_DPU_on();
    } else {
        ipc_lpd_DRAM_off();
    }
}

static void ipc_lpd_handle_error(void)
{
}

static struct APMOps lpdOps = {
    .resetRelease = ipc_lpd_handle_reset_release,
    .dramOn = ipc_lpd_handle_DRAM_on,
    .dramOff = ipc_lpd_handle_DRAM_off,
    .dpuOn = ipc_lpd_handle_DPU_on,
    .dpuOff = ipc_lpd_handle_DPU_off,
#if defined(LPD_GPIO_RETENTION_CONTROL)
    .gpioRetentionOn = ipc_lpd_handle_GPIO_on,
    .gpioRetentionOff = ipc_lpd_handle_GPIO_off,
#endif
    .error  = ipc_lpd_handle_error,
    //.mifOn = ipc_lpd_handle_MIF_on,
    //.mifOff = ipc_lpd_handle_MIF_off,
};

void ipc_lpd_check_update(void)
{
    uint32_t hour, min, sec;

    if (mLPDMode == false)
        return;

    rtcGetBCDTime(&hour, &min, &sec);

    switch (mLPDPeriod)
    {
    case LPD_PERIOD_1S:
        if (sec == mLastSec)
            return;
        break;
    case LPD_PERIOD_1M:
        if (min == mLastMin)
            return;
        break;
    default:
        break;
    }

    mLastMin = min;
    mLastSec = sec;

#if defined(EMBOS)
    if (mTCB != NULL)
        OS_SignalEvent(EVENT_LPD_UPDATE, mTCB);
#endif
}

void ipc_lpd_set_tid(void *tid)
{
  #if defined(SEOS)
    mTid = (uint32_t)tid;
    CSP_PRINTF_INFO("%s : %lu\n", __func__, mTid);
  #elif defined(EMBOS)
    mTCB = (OS_TASK *)tid;
  #endif
}

void ipc_lpd_send_event(uint32_t event)
{
  #if defined(SEOS)
    if (mTid != 0)
        osEnqueueEvt(event, NULL, NULL);
        //osEnqueuePrivateEvt(event, NULL, NULL, mTid);
  #elif defined(EMBOS)
    if (mTCB != NULL)
        OS_SignalEvent(event, mTCB);
  #endif
}

void ipc_lpd_init(void)
{
    LPD_IPC_DEBUG("IPC LPD INIT\n");
    lpd_ipc = (struct lpd_fw_ipc *)lpd_info->map.sram.ipc_start;
    mailboxApmSetOps(&lpdOps);
    cpuRegisterHandler(CM55_IRQn, cm55_IRQHandler);
    NVIC_EnableIRQ(CM55_IRQn);
}

void ipc_lpd_powerdown(void)
{
    if (mCm55State != CM55_STATE_IDLE) {
        //LPD_IPC_DEBUG("CM55 is not idle. Disable Q-Ch\n");
        cmuDrvDisableCM55Qch();
    }

    mCm55State = CM55_STATE_OFF;
}

void ipc_lpd_set_time(union time64_t time)
{
    if (lpd_ipc == NULL)
        return;

    if (mCm55State == CM55_STATE_IDLE) {
        lpd_ipc->data.time.time64 = time.time64;
    } else {
        mTempTime.time64 = time.time64;
        mTimeValid = 1;
    }
}

void ipc_lpd_restore(void)
{
    if (mCm55State != CM55_STATE_OFF)
        return;
    mCM55BootMode = CM55_BOOT_BL;
    ipc_lpd_reset_release();
}

void cm55_IRQHandler(void)
{
    uint32_t resp = lpd_ipc->cmd[IPC_COMMAND_DIR_FROM];

    sysregClearIRQfromCM55();
    LPD_IPC_DEBUG("CM55 IRQ : %lu - %s\n", resp, cm55Response[resp]);

    switch (resp) {
    case IPC_COMMAND_BOOT_DONE:
        ipc_lpd_cm55_boot_done();
        break;
    case IPC_COMMAND_START_DRAW_ACK:
        break;
    case IPC_COMMAND_STOP_DRAW_ACK:
        break;
    case IPC_COMMAND_END_DRAW:
#if defined(LPD_GPIO_RETENTION_CONTROL)
        ipc_lpd_GPIO_off();
#else
        ipc_lpd_DPU_off();
#endif
        break;
    default:
        CSP_PRINTF_INFO("%s : invalid message(%lu)\n", __func__, lpd_ipc->cmd[IPC_COMMAND_DIR_FROM]);
        break;
    }
}
