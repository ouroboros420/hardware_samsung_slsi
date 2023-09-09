/*----------------------------------------------------------------------------
 *      Exynos SoC  -  RTC
 *----------------------------------------------------------------------------
 *      Name:    rtc_v1.c
 *      Purpose: To implement RTC driver functions
 *      Rev.:    V1.00
 *----------------------------------------------------------------------------
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

#if defined(RTC_REQUIRED)

#include <csp_common.h>
#include <cpu.h>
#include <cpu/cpu.h>
#include <rtc/rtc.h>
#include <cmsis.h>
#if defined(SEOS)
    #include <platform.h>
    #include <atomic.h>
#endif

#define REG_RTC_INTP            (RTC_BASE_ADDRESS + 0x30)
#define REG_RTC_TICCON0         (RTC_BASE_ADDRESS + 0x38)
#define REG_RTC_TICCON1         (RTC_BASE_ADDRESS + 0x3C)
#define REG_RTC_RTCCON          (RTC_BASE_ADDRESS + 0x40)
#define REG_RTC_TICCNT0         (RTC_BASE_ADDRESS + 0x44)
#define REG_RTC_TICCNT1         (RTC_BASE_ADDRESS + 0x48)
#define REG_RTC_PRETICK         (RTC_BASE_ADDRESS + 0x4C)
#define REG_RTC_RTCALM          (RTC_BASE_ADDRESS + 0x50)
#define REG_RTC_ALMSEC          (RTC_BASE_ADDRESS + 0x54)
#define REG_RTC_ALMMIN          (RTC_BASE_ADDRESS + 0x58)
#define REG_RTC_ALMHOUR         (RTC_BASE_ADDRESS + 0x5C)
#define REG_RTC_ALMDAY          (RTC_BASE_ADDRESS + 0x60)
#define REG_RTC_ALMMON          (RTC_BASE_ADDRESS + 0x64)
#define REG_RTC_ALMYEAR         (RTC_BASE_ADDRESS + 0x68)
#define REG_RTC_BCDSEC          (RTC_BASE_ADDRESS + 0x70)
#define REG_RTC_BCDMIN          (RTC_BASE_ADDRESS + 0x74)
#define REG_RTC_BCDHOUR         (RTC_BASE_ADDRESS + 0x78)
#define REG_RTC_BCDDAY          (RTC_BASE_ADDRESS + 0x7C)
#define REG_RTC_BCDDAYWEEK      (RTC_BASE_ADDRESS + 0x80)
#define REG_RTC_BCDMON          (RTC_BASE_ADDRESS + 0x84)
#define REG_RTC_BCDYEAR         (RTC_BASE_ADDRESS + 0x88)
#define REG_RTC_CURTICCNT0      (RTC_BASE_ADDRESS + 0x90)
#define REG_RTC_CURTICCNT1      (RTC_BASE_ADDRESS + 0x94)

#define TICEN_BIT1      (0)
#define TICEN_BIT0      (0)
#define TICCKSEL_BIT1   (1)
#define TICCKSEL_BIT0   (1)
#define CLKOUTEN_BIT    (9)
#define CLKRST_BIT      (3)
#define CNTSEL_BIT      (2)
#define CLKSEL_BIT      (1)
#define CTLEN_BIT       (0)

#define ALM_INT_MASK    (0x1<<1)
#define TICK_INT_MASK   (0x1<<2)
#define TICK0_INT_MASK  (0x1<<0)
#define TICK1_INT_MASK  (0x1<<2)

#define ALMEN_BIT       (6)
#define YEAREN_BIT      (5)
#define MONEN_BIT       (4)
#define DAYEN_BIT       (3)
#define HOUREN_BIT      (2)
#define MINEN_BIT       (1)
#define SECEN_BIT       (0)

#if (RTC_TIMESTAMP_TICK == 1)
  #define REG_RTC_TS_TICCON    REG_RTC_TICCON1
  #define REG_RTC_TS_TICCNT    REG_RTC_TICCNT1
  #define REG_RTC_TS_CURTICCNT REG_RTC_CURTICCNT1
  #define REG_RTC_TICCON       REG_RTC_TICCON0
  #define REG_RTC_TICCNT       REG_RTC_TICCNT0
  #define REG_RTC_CURTICCNT    REG_RTC_CURTICCNT0
  #define TS_TICKEN_BIT        TICEN_BIT1
  #define TS_TICK_INT_BIT      TICK1_INT_MASK
  #define TICKEN_BIT           TICEN_BIT0
  #define TICKCKSEL_BIT        TICCKSEL_BIT0
  #define TICK_INT_BIT         TICK0_INT_MASK
  #define INT_MASK             (TICK0_INT_MASK | ALM_INT_MASK)
#else
  #define REG_RTC_TS_TICCON    REG_RTC_TICCON0
  #define REG_RTC_TS_TICCNT    REG_RTC_TICCNT0
  #define REG_RTC_TS_CURTICCNT REG_RTC_CURTICCNT0
  #define REG_RTC_TICCON       REG_RTC_TICCON1
  #define REG_RTC_TICCNT       REG_RTC_TICCNT1
  #define REG_RTC_CURTICCNT    REG_RTC_CURTICCNT1
  #define TS_TICKEN_BIT        TICEN_BIT0
  #define TS_TICK_INT_BIT      TICK0_INT_MASK
  #define TICKEN_BIT           TICEN_BIT1
  #define TICKCKSEL_BIT        TICCKSEL_BIT1
  #define TICK_INT_BIT         TICK1_INT_MASK
  #define INT_MASK             (TICK1_INT_MASK | ALM_INT_MASK)
#endif

#define RTC_32768HZ            32768

#define NS_PER_S               1000000000ULL

#define RTC_TICK_COUNT         0xFFFFFFFFULL
#define RTC_TICK_CYCLE         ( (RTC_TICK_COUNT+1) * NS_PER_S / RTC_32768HZ )
#define RTC_MAX_TICK_TIME      86400000000ULL //24hr

#define PRINTF_ERROR           CSP_PRINTF_ERROR
#define PRINTF_INFO            CSP_PRINTF_INFO
#define PRINTF_DEBUG           CSP_PRINTF_INFO//CSP_PRINTF_DEBUG

static int64_t initial_total_sec_ns;
static uint64_t last_system_time_nsec;
static uint64_t adjustTime;

typedef struct
{
    uint8_t   repeat;
    uint8_t   tickEn;
    uint32_t  count;
    void (*callback)(void);
} RtcTickInfoType;

RtcTickInfoType mRtcTickInfo;

// Driver API to initialize RTC
void rtcInit(void)
{
    uint32_t regValue;

    mRtcTickInfo.repeat = 0;
    mRtcTickInfo.tickEn = 0;
    mRtcTickInfo.callback = NULL;

    // Set tick count with maximum value.
    //  It it up-counter and lasts 36hr before wrap down to 0
    __raw_writel(RTC_TICK_COUNT, REG_RTC_TS_TICCNT);

    // Control Enable & Reset
    regValue = (0x1 << CTLEN_BIT) | (1 << CLKRST_BIT);
    __raw_writel(regValue, REG_RTC_RTCCON);

    // Reset Tick Count
    regValue = 0x0;
    __raw_writel(regValue, REG_RTC_TS_TICCON);

    // Tick Enable for timestamp
    regValue = (1 << TS_TICKEN_BIT);
    __raw_writel(regValue, REG_RTC_TS_TICCON);

    // Exit from RTC control mode
    regValue = __raw_readl(REG_RTC_RTCCON);
    __raw_writel( regValue & (uint32_t)(~(0x1 << CTLEN_BIT)) , REG_RTC_RTCCON);

    PRINTF_INFO("RTC Initialized\n");
}

void rtcSetInitTime(int64_t time)
{
    initial_total_sec_ns += time;
}

uint64_t rtcGetTimeStampNS(void)
{
    uint32_t expired = 0;
    uint32_t curTick;
    uint64_t timestamp;
    uint32_t irqState;


    irqState = cpuIntsOff();

    curTick = __raw_readl(REG_RTC_TS_CURTICCNT);

    expired = __raw_readl(REG_RTC_INTP) & TS_TICK_INT_BIT;

    if (expired) {
        curTick = __raw_readl(REG_RTC_TS_CURTICCNT);
        adjustTime += RTC_TICK_CYCLE;
        __raw_writel(TS_TICK_INT_BIT, REG_RTC_INTP);
    }

    timestamp = ((uint64_t)curTick * NS_PER_S) / RTC_32768HZ + adjustTime;
    timestamp += initial_total_sec_ns;

    if (last_system_time_nsec > timestamp) {
        PRINTF_ERROR("Oops! last_system_time(%llu) < curr_system_time(%llu)\n",
            last_system_time_nsec, timestamp);
        CSP_ASSERT(0);
    }

    cpuIntsRestore(irqState);

    return timestamp;
}

uint64_t rtcGetTimeStampUS(void)
{
    uint64_t timestamp;

    timestamp = rtcGetTimeStampNS() / 1000;

    return timestamp;
}

// Driver API to set tick time
void rtcSetTickTime(IN uint32_t sec, IN uint32_t mSec, IN uint32_t uSec,
                    OUT void (*callback) (void), IN uint8_t repeat)
{
    uint32_t tickCountValue;
    uint64_t tickTime;

    rtcStopTick();

    if ((sec == 0) && (mSec == 0) && (uSec == 0)) {
        PRINTF_ERROR("%s values are invalid\n", __func__);
        CSP_ASSERT(0);
        return;
    }

    tickTime = uSec + (mSec * 1000);
    tickTime += sec * 1000000ULL;

    if (tickTime > RTC_MAX_TICK_TIME) {
        PRINTF_ERROR("%s values is too big :  %llu\n", __func__, tickTime);
        CSP_ASSERT(0);
        return;
    }

    tickCountValue = (uint32_t)((tickTime * RTC_32768HZ) / 1000000);

    __raw_writel(tickCountValue, REG_RTC_TICCNT);

    mRtcTickInfo.repeat = repeat;
    mRtcTickInfo.count = tickCountValue;
    mRtcTickInfo.callback = callback;

    rtcStartTick();
}

uint64_t rtcGetCurrentTickTime(void)
{
    uint64_t currentTime;

    // calculate elapsed time(usec)
    currentTime = (uint64_t)(__raw_readl(REG_RTC_CURTICCNT)) * 1000000;

    return currentTime;
}

void rtcStartTick(void)
{
    uint32_t regValue;

    if (mRtcTickInfo.tickEn) {
        PRINTF_INFO("%s: Already enabled\n", __func__);
        return;
    }

    if (mRtcTickInfo.count == 0) {
        PRINTF_ERROR("%s: Tick value is 0\n", __func__);
        CSP_ASSERT(0);
        return;
    }

    if (mRtcTickInfo.callback == NULL) {
        PRINTF_ERROR("%s: Tick callback is NULL\n", __func__);
        CSP_ASSERT(0);
        return;
    }

    //__raw_writel(counter, REG_RTC_TICCNT);
    regValue = __raw_readl(REG_RTC_TICCON);
    regValue |= (0x1 << TICKEN_BIT);
    __raw_writel(regValue, REG_RTC_TICCON);

    mRtcTickInfo.tickEn = 1;

    NVIC_EnableIRQ(RTC_INTERRUPT_NO);
}

void rtcStopTick(void)
{
    uint32_t regValue;

    if (mRtcTickInfo.tickEn == 0) {
        PRINTF_INFO("%s: Already Disabled\n", __func__);
        return;
    }

    regValue = __raw_readl(REG_RTC_TICCON);
    regValue &= ~(0x1 << TICKEN_BIT);
    __raw_writel(regValue, REG_RTC_TICCON);

    mRtcTickInfo.tickEn = 0;

    NVIC_DisableIRQ(RTC_INTERRUPT_NO);
}

void rtcSetTickHandler(void (*callback)(void))
{
    if (callback == NULL) {
        PRINTF_ERROR("%s: callback is NULL\n", __func__);
        CSP_ASSERT(0);
        return;
    }
}

uint64_t rtcGetTime(void)
{
  #if defined(SEOS)
    return platGetTicks();
  #else
    return rtcGetTimeStampNS();
  #endif
}

void rtc_IRQHandler(void)
{
    rtcStopTick();

    if (mRtcTickInfo.callback != NULL)
        mRtcTickInfo.callback();

    // Pending Clear
    __raw_writel(TICK_INT_BIT, REG_RTC_INTP);

    if (mRtcTickInfo.repeat)
        rtcStartTick();
}

#endif
