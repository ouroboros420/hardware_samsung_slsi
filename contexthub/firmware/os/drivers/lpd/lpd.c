/*----------------------------------------------------------------------------
 * Exynos SoC  -  LPD
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

#include <atomic.h>
#include <hostIntf.h>
#include <timer.h>
#include <seos.h>
#include <eventnums.h>
#include <sysreg.h>
#include <cmu.h>
#include <rtc.h>
#include <ipc_lpd.h>
#include <platform.h>
#include <mailboxApm.h>

#define LPD_APP_ID      APP_ID_MAKE(NANOHUB_VENDOR_EXYNOS, 8)
#define LPD_APP_VERSION 1

#define LPD_UPDATE_1S   1000000000ULL
#define LPD_UPDATE_1M   60000000000ULL

#define GET_STATE() (atomicReadByte(&mLpdTask.state))

#define SET_STATE(s) do {\
      CSP_PRINTF_INFO("LPD state: %s -> %s\n", states[(int)GET_STATE()], states[(int)s]);\
      atomicWriteByte(&mLpdTask.state, (s));\
  }while(0)

char states[][10] = {
  "OFF",
  "IDLE",
  "CM55_BOOT",
  "MIF_ON",
  "DPU_ON",
  "RUN",
};

static struct lpd_task
{
    uint32_t id;
    uint32_t on;
    uint32_t timerHandle;
    uint32_t period;
    volatile uint8_t state;
} mLpdTask;

static void lpdTimerCallback(uint32_t timerId, void *cookie)
{
    union time64_t time;
    //osEnqueuePrivateEvt(EVT_LPD_TIMER, cookie, NULL, mLpdTask.id);
    //enableMIF(lpdStart);
    rtcGetTime64(&time);
    CSP_PRINTF_INFO("LPD Run @ %d/%d/%d %02d:%02d:%02d\n", 1900+time.tm_year, time.tm_mon + 1, time.tm_mday,
                time.tm_hour, time.tm_min, time.tm_sec);
    ipc_lpd_set_time(time);
    ipc_lpd_run();
}

static void lpdHandleEvent(uint32_t evtType, const void* evtData)
{
    union time64_t time;

    switch (evtType) {
        case EVT_APP_START:
            mLpdTask.on = 1;
            break;
        case EVT_LPD_START:
            CSP_PRINTF_INFO("Received LPD START Request!!!\n");
            ipc_lpd_start(LPD_PERIOD_1S);
            mLpdTask.timerHandle = timTimerSet(LPD_UPDATE_1M, 0, 50, lpdTimerCallback, NULL, false);
            rtcGetTime64(&time);
            ipc_lpd_set_time(time);
            CSP_PRINTF_INFO("LPD Run @ %d/%d/%d %02d:%02d:%02d\n", 1900+time.tm_year, time.tm_mon+1, time.tm_mday,
                time.tm_hour, time.tm_min, time.tm_sec);
            ipc_lpd_run();
            mLpdTask.on = 1;
            break;
        case EVT_LPD_STOP:
            CSP_PRINTF_INFO("Recieved LPD STOP Request!!!\n");
            timTimerCancel(mLpdTask.timerHandle);
            ipc_lpd_stop();
            break;
    }
}

static bool lpdStartTask(uint32_t taskId)
{
    CSP_PRINTF_INFO("LPD Task starts\n");

    mLpdTask.id = taskId;
    mLpdTask.on = 0;
    osEventSubscribe(taskId, EVT_LPD_START);
    osEventSubscribe(taskId, EVT_LPD_STOP);
    osEventSubscribe(taskId, EVT_APP_START);
    ipc_lpd_set_tid((void *)taskId);
    return true;
}

static void lpdEndTask(void)
{
}

INTERNAL_APP_INIT(LPD_APP_ID, LPD_APP_VERSION, lpdStartTask, lpdEndTask, lpdHandleEvent);
