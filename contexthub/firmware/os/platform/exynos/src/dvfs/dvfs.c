/*
 * Copyright (C) 2021 The Android Open Source Project
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
#if defined(SEOS)
    #include <seos.h>
    #include <errno.h>
#endif
#include <dvfs/dvfs.h>
#include <cmu.h>
#include <mailboxApm.h>

#define APM_DVFS_ID_FVP             0x2
#define APM_DVFS_DOMAIN_ID_CHUB     0x19
#define APM_DVFS_DOMAIN_ID_uFD      0x1C

#define APM_PLUGIN_UFD              0x0
#define APM_PLUGIN_DVFS             0x1

#define DVFS_POOL                   8
#define DVFS_LEVEL                  2

enum DvfsStatus {
    DVFS_RUNNING,
    DVFS_IDLE,
    DVFS_DONE,
    DVFS_REQEST_L0,
    DVFS_REQEST_L1,
};

#define DVFS_DEBUG_ENABLE  1

#if (DVFS_DEBUG_ENABLE)
  #define DVFS_DEBUG(fmt, ...) do { \
        CSP_PRINTF_INFO("[DVFS]" fmt, ##__VA_ARGS__); \
    } while (0);
#else
  #define DVFS_DEBUG(fmt, ...)
#endif

#define DVFS_INFO(fmt, ...) CSP_PRINTF_INFO("[DVFS]" fmt, ##__VA_ARGS__)
#define DVFS_ERROR(fmt, ...) CSP_PRINTF_ERROR("[DVFS]" fmt, ##__VA_ARGS__)

static uint32_t mDvfsStatus;
static uint32_t mDvfsCurLvl;
static uint32_t mDvfsTargetFreq;
static uint32_t mDvfsCurFreq;
static uint32_t mDvfsTable[DVFS_LEVEL] = {400000000, 200000000};

static int checkClkValid(uint32_t clk) {
    if (clk > mDvfsTable[0]) {
        DVFS_ERROR("Invalid clk requested %lu\n", clk);
        return -1;
    }

    return 0;
}

static uint32_t getDvfsLvl(uint32_t clk) {
    uint32_t i;

    for (i = 1; i < DVFS_LEVEL; i++) {
        if (clk > mDvfsTable[i])
            break;
    }

    return i - 1;
}

int dvfsRequestFreq(uint32_t clk) {
    uint32_t targetLvl;

    if (checkClkValid(clk))
        return -EINVAL;

    if (mDvfsStatus != DVFS_IDLE && mDvfsStatus != DVFS_DONE) {
        DVFS_INFO("DVFS is running %lu %lu\n", mDvfsTargetFreq, clk);
        mDvfsTargetFreq = clk;
        return -EBUSY;
    }

    if (mDvfsCurFreq == clk) {
        mDvfsStatus = DVFS_IDLE;
        return 0;
    }

    mDvfsStatus = DVFS_RUNNING;
    targetLvl = getDvfsLvl(clk);

    if (mDvfsCurLvl == targetLvl) {
        cmuSetCpuClk(clk);
        mDvfsCurFreq = clk;
        mDvfsStatus = DVFS_IDLE;
    } else if (mDvfsCurLvl < targetLvl) {
        cmuSetCpuClk(clk);
        mDvfsCurFreq = clk;
        mDvfsTargetFreq = clk;
        mDvfsStatus = DVFS_REQEST_L1;

        //send dvfs request L1
        mailboxApmReqDvsFreq(mDvfsTable[targetLvl]);
    } else {
        cmuSetCpuClk(mDvfsTable[mDvfsCurLvl]);
        mDvfsCurFreq = mDvfsTable[mDvfsCurLvl];
        mDvfsTargetFreq = clk;
        mDvfsStatus = DVFS_REQEST_L0;

        //send dvfs request L0
        mailboxApmReqDvsFreq(mDvfsTable[targetLvl]);
    }

    return 0;
}

void dvfs_IRQHandler(void) {
    mDvfsCurLvl = mDvfsStatus - DVFS_REQEST_L0;
    mDvfsStatus = DVFS_DONE;
    dvfsRequestFreq(mDvfsTargetFreq);
}

void dvfsInit(void) {
    mDvfsStatus = DVFS_IDLE;
    mDvfsCurLvl = 0;
    mDvfsCurFreq = 0;
}
