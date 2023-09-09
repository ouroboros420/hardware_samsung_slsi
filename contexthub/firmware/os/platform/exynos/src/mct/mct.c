/*----------------------------------------------------------------------------
 *      Exynos SoC  -  MCT
 *----------------------------------------------------------------------------
 *      Name:    mct.c
 *      Purpose: To implement platform-specific
 *      Rev.:    V1.00
 *----------------------------------------------------------------------------
 *
 * Copyright (C) 2021 Samsung Electronics Co., Ltd.
 *
 * Contact: Sukmin Kang <sukmin.kang@samsung.com>
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

#include <mct.h>
#include <rtc.h>
#include <cpu.h>
#include <csp_common.h>
#include <plat/mailbox/ipc_chub.h>

#define REG_MCT_INCR_RTCCLK         (MCT_ALIVE_BASE_ADDRESS + 0x004)
#define REG_MCT_CNT_L               (MCT_ALIVE_BASE_ADDRESS + 0x110)
#define REG_MCT_CNT_U               (MCT_ALIVE_BASE_ADDRESS + 0x114)
#define REG_MCT_COMPENSATE_VALUE    (MCT_ALIVE_BASE_ADDRESS + 0x124)
#define NANOSECOND                  1000000000
#define MCT_MAX_SEC                 3600

#define COMPENSATE_VALUE_MASK       0x7F
#define COMPENSATE_MODE_MASK        0x80000000

static uint64_t mMctOffset = 0;
static uint64_t mIncrRtcclk;
static uint32_t mCompensateValue;
static uint64_t mMctCarry;
#if (MCT_USE_RTC)
static bool mUseRtc = false;
static uint64_t mBaseTick;
static uint64_t mRtcTick;
static uint64_t mRtcBaseTick;
static uint32_t mLastRtcTick;
#endif

static uint64_t mctGetTickCount(void)
{
    uint32_t count_u = 0, count_l = 0;
    uint64_t count;
    uint32_t retry = 5;

    while (retry--) {
        count_u = __raw_readl(REG_MCT_CNT_U);
        count_l = __raw_readl(REG_MCT_CNT_L);
        if (count_u == __raw_readl(REG_MCT_CNT_U))
            break;
   }
   count = ((uint64_t)count_u << 32) | count_l;

   return count;
}

void mctSetClockState(bool en)
{
#if (MCT_USE_RTC)
    if (en) {
        mUseRtc = false;
    } else {
        mUseRtc = true;
        mRtcTick = 0;
        rtcStopTSTick();
        mRtcBaseTick = rtcGetTSTick();
        mLastRtcTick = rtcGetTSTick();
        mBaseTick = mctGetTickCount();
        rtcStartTSTick();
    }
#endif
}

static inline uint32_t do_div(uint64_t *n, uint32_t base)
{
    uint32_t remainder = *n % base;
    *n = *n / base;
    return remainder;
}

static void clocks_calc_mult_shift(uint32_t *mult, uint32_t *shift, uint32_t from, uint32_t to, uint32_t maxsec)
{
    uint64_t tmp;
    uint32_t sft, sftacc= 32;

    /*
     * Calculate the shift factor which is limiting the conversion
     * range:
     */
    tmp = ((u64)maxsec * from) >> 32;
    while (tmp) {
        tmp >>=1;
        sftacc--;
    }

    /*
     * Find the conversion shift/mult pair which has the best
     * accuracy and fits the maxsec conversion range:
     */
    for (sft = 32; sft > 0; sft--) {
        tmp = (u64) to << sft;
        tmp += from / 2;
        do_div(&tmp, from);
        if ((tmp >> sftacc) == 0)
            break;
    }
    *mult = tmp;
    *shift = sft;
}


uint64_t mctGetTimeStampNS()
{
    uint64_t time, carry;
    uint32_t irqState;
    uint32_t mult,shift;

    irqState = cpuIntsOff();

#if (MCT_USE_RTC)
    if (mUseRtc) {
        uint32_t currRtcTick = rtcGetTSTick();
        uint64_t passRtcTick = currRtcTick;
        if (currRtcTick < mLastRtcTick) {
            mRtcTick += (0x100000000 - mRtcBaseTick);
            mRtcBaseTick = 0;
        } else {
            passRtcTick -= mRtcBaseTick;
        }
        passRtcTick += mRtcTick;

        time = mBaseTick + (passRtcTick * mIncrRtcclk);
        if (mCompensateValue & COMPENSATE_MODE_MASK) {
            time += (passRtcTick / MCT_COMPENSATE_CNT ) * (mCompensateValue & COMPENSATE_VALUE_MASK);
        } else {
            time -= (passRtcTick / MCT_COMPENSATE_CNT ) * (mCompensateValue & COMPENSATE_VALUE_MASK);
        }
        mLastRtcTick = currRtcTick;
    } else {
        time = mctGetTickCount();
    }
#else
    time = mctGetTickCount();
#endif

    if (time > mMctOffset)
        time -= mMctOffset;
    else {
        osLog(LOG_INFO, "time(%llu) < MCT offset(%llu)\n", time, mMctOffset);
        time = 0;
    }

    time -= (mMctCarry * MCT_FREQ * MCT_MAX_SEC);

    if(time >= (uint64_t)MCT_FREQ * MCT_MAX_SEC) {
        mMctCarry++;
        time -= (uint64_t)MCT_FREQ * MCT_MAX_SEC;
    }

    clocks_calc_mult_shift(&mult, &shift, MCT_FREQ, NANOSECOND, MCT_MAX_SEC);

    time = (time * mult) >> shift;
    time += mMctCarry * MCT_MAX_SEC * NANOSECOND;

    cpuIntsRestore(irqState);

    return time;
}

uint64_t mctGetTimeStampUS()
{
    return mctGetTimeStampNS() / 1000;
}

void mctInit()
{
    mMctCarry = 0;
    mMctOffset = ipc_get_chub_boottime();
    mIncrRtcclk = __raw_readl(REG_MCT_INCR_RTCCLK);
    mCompensateValue = (__raw_readl(REG_MCT_COMPENSATE_VALUE));
    osLog(LOG_INFO, "MCT offset cnt %llu\n", mMctOffset);
}

