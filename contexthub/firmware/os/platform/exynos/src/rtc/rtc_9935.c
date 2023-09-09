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
#elif defined(EMBOS)
    #include <Device.h>
#endif


#define RTC_INTP_OFFSET             0x0030 //Specifies the Interrupt Pending Register 
#define RTC_CLK_COUNT_OFFSET        0x0034 //Specifies the Clock Count Register 
#define RTC_TICCON0_OFFSET          0x0038 //Specifies the Tick Control Register 
#define RTC_TICCON1_OFFSET          0x003C //Specifies the Tick Control Register
#define RTC_RTCCON_OFFSET           0x0040 //Specifies the RTC Control Register 
#define RTC_TICCNT0_OFFSET          0x0044 //Specifies the Tick Time Count Register 
#define RTC_TICCNT1_OFFSET          0x0048 //Specifies the Tick Time Count Register
#define RTC_RTCALM_OFFSET           0x0050 //Specifies the RTC Alarm Control Register 
#define RTC_ALMSEC_OFFSET           0x0054 //Specifies the Alarm Second Data Register 
#define RTC_ALMMIN_OFFSET           0x0058 //Specifies the Alarm Minute Data Register 
#define RTC_ALMHOUR_OFFSET          0x005C //Specifies the Alarm Hour Data Register 
#define RTC_ALMDAY_OFFSET           0x0060 //Specifies the Alarm Day Data Register 
#define RTC_ALMMON_OFFSET           0x0064 //Specifies the Alarm Month Data Register 
#define RTC_ALMYEAR_OFFSET          0x0068 //Specifies the Alarm Year Data Register 
#define RTC_BCDSEC_OFFSET           0x0070 //Specifies the BCD Second Register 
#define RTC_BCDMIN_OFFSET           0x0074 //Specifies the BCD Minute Register 
#define RTC_BCDHOUR_OFFSET          0x0078 //Specifies the BCD Hour Register 
#define RTC_BCDDAY_OFFSET           0x007C //Specifies the BCD Day Register 
#define RTC_BCDDAYWEEK_OFFSET       0x0080 //Specifies the BCD Day of the Week Register 
#define RTC_BCDMON_OFFSET           0x0084 //Specifies the BCD Month Register 
#define RTC_BCDYEAR_OFFSET          0x0088 //Specifies the BCD Year Register 
#define RTC_CURTICCNT0_OFFSET       0x0090 //Specifies the Current Tick Time Counter Register 
#define RTC_CURTICCNT1_OFFSET       0x0094 //Specifies the Current Tick Time Counter Register 

#define RTC_FREQ                    32768
#define RTC_BASE_YEAR               1900
#define NS_PER_S                    1000000000ULL
#define RTC_TICK_CYCLE              ((RTC_TICK_COUNT + 1) * NS_PER_S / RTC_FREQ)
#define RTC_MAX_TICK_TIME           86400000000ULL //24hr

//RTC_INTP
#define INTP_TIC_1                  (1 << 2)
#define INTP_ALARM                  (1 << 1)
#define INTP_TIC_0                  (1 << 0)
//RTC_TICCON
#define TICCON_TICEN                (1 << 0)
//RTC_RTCCON
#define RTCCON_LATCH_EN             (1 << 8)
#define RTCCON_CLKRST               (1 << 3)
#define RTCCON_CNTSEL               (1 << 2)
#define RTCCON_CLKSEL               (1 << 1)
#define RTCCON_CTLEN                (1 << 0)
//RTC_RTCALM
#define RTCALM_ALMEN                (1 << 6)
#define RTCALM_YEAREN               (1 << 5)
#define RTCALM_MONEN                (1 << 4)
#define RTCALM_DAYEN                (1 << 3)
#define RTCALM_HOUREN               (1 << 2)
#define RTCALM_MINEN                (1 << 1)
#define RTCALM_SECEN                (1 << 0)

#if RTC_TIMESTAMP_TICK
    #define RTC_TS_TICCON           RTC_TICCON1_OFFSET
    #define RTC_TS_TICCNT           RTC_TICCNT1_OFFSET
    #define RTC_TS_CURTICCNT        RTC_CURTICCNT1_OFFSET
#else
    #define RTC_TS_TICCON           RTC_TICCON0_OFFSET
    #define RTC_TS_TICCNT           RTC_TICCNT0_OFFSET
    #define RTC_TS_CURTICCNT        RTC_CURTICCNT0_OFFSET
#endif

#if RTC_INTERRUPT_TICK
    #define RTC_INT_TICCON          RTC_TICCON1_OFFSET
    #define RTC_INT_TICCNT          RTC_TICCNT1_OFFSET
    #define RTC_INT_CURTICCNT       RTC_CURTICCNT1_OFFSET
    #define INTP_INT_TIC            INTP_TIC_1
#else
    #define RTC_INT_TICCON          RTC_TICCON0_OFFSET
    #define RTC_INT_TICCNT          RTC_TICCNT0_OFFSET
    #define RTC_INT_CURTICCNT       RTC_CURTICCNT0_OFFSET
    #define INTP_INT_TIC            INTP_TIC_0
#endif

#define PRINTF_ERROR                CSP_PRINTF_ERROR
#define PRINTF_INFO                 CSP_PRINTF_INFO

enum rtcTicClk {
    RTC_TIC_CLK_32768 = 0,
    RTC_TIC_CLK_16384,
    RTC_TIC_CLK_8192,
    RTC_TIC_CLK_4096,
    RTC_TIC_CLK_2048,
    RTC_TIC_CLK_1024,
    RTC_TIC_CLK_512,
    RTC_TIC_CLK_256,
    RTC_TIC_CLK_128,
    RTC_TIC_CLK_64,
    RTC_TIC_CLK_32,
    RTC_TIC_CLK_16,
    RTC_TIC_CLK_8,
    RTC_TIC_CLK_4,
    RTC_TIC_CLK_2,
    RTC_TIC_CLK_1,
};

typedef struct
{
    uint8_t  repeat;
    uint32_t count;
    void (*callback)(void);
} RtcTickInfoType;

static RtcTickInfoType mRtcTickInfo;

static inline uint32_t rtcGetIntp(void)
{
    return __raw_readl(RTC_INTERRUPT_BASE_ADDRESS + RTC_INTP_OFFSET);
}

static inline void rtcPendClear(uint32_t bit)
{
    __raw_writel(bit, RTC_INTERRUPT_BASE_ADDRESS + RTC_INTP_OFFSET);
}

static inline void rtcSetTicClkSel(uint32_t sel)
{
    uint32_t regValue;

    regValue = __raw_readl(RTC_INTERRUPT_BASE_ADDRESS + RTC_INT_TICCON);
    regValue &= ~(0xF << 1);
    regValue |= (sel << 1);
    __raw_writel(regValue, RTC_INTERRUPT_BASE_ADDRESS + RTC_INT_TICCON);
}

static inline void rtcDisableTick(void)
{
    uint32_t regValue;
    
    regValue = __raw_readl(RTC_INTERRUPT_BASE_ADDRESS + RTC_INT_TICCON);
    regValue &= ~TICCON_TICEN;
    __raw_writel(regValue, RTC_INTERRUPT_BASE_ADDRESS + RTC_INT_TICCON);
}

static inline void rtcEnableTick(void)
{
    uint32_t regValue;
    
    regValue = __raw_readl(RTC_INTERRUPT_BASE_ADDRESS + RTC_INT_TICCON);
    regValue |= TICCON_TICEN;
    __raw_writel(regValue, RTC_INTERRUPT_BASE_ADDRESS + RTC_INT_TICCON);
}

static inline void rtcSetIntTicCnt(uint32_t count)
{
    __raw_writel(count, RTC_INTERRUPT_BASE_ADDRESS + RTC_INT_TICCNT);
}

static inline uint32_t rtcGetCurTicCnt(void)
{
    uint32_t regValue;

    regValue = __raw_readl(RTC_TIMESTAMP_BASE_ADDRESS + RTC_TS_CURTICCNT);
    return regValue;
}

void rtcInit(void)
{
    rtcSetTicClkSel(RTC_TIC_CLK_32768);
}

uint32_t rtcGetTSTick(void)
{
    return rtcGetCurTicCnt();
}

uint64_t rtcGetTimeStampNS(void)
{
    uint32_t expired = 0;
    uint32_t curTick;
    uint64_t timestamp;
    uint32_t irqState;

    irqState = cpuIntsOff();

    curTick = rtcGetCurTicCnt(); 

    timestamp = ((uint64_t)curTick * NS_PER_S) / RTC_FREQ;

    cpuIntsRestore(irqState);

    return timestamp;
}

uint64_t rtcGetTimeStampUS(void)
{
    uint64_t timestamp;

    timestamp = rtcGetTimeStampNS() / 1000;

    return timestamp;
}


void rtcSetTickTime(IN uint32_t sec, IN uint32_t mSec, IN uint32_t uSec,
                    OUT void (*callback) (void), IN uint8_t repeat)
{
    uint32_t tickCountValue;
    uint64_t tickTime;

    rtcDisableTick();
    NVIC_DisableIRQ(RTC_INTERRUPT_NO);

    if ((sec == 0) && (mSec == 0) && (uSec == 0)) {
        PRINTF_ERROR("%s values are invalid\n", __func__);
        return;
    }

    tickTime = uSec + (mSec * 1000);
    tickTime += sec * 1000000ULL;

    if (tickTime > RTC_MAX_TICK_TIME) {
        PRINTF_ERROR("%s values is too big :  %llu\n", __func__, tickTime);
        return;
    }

    tickCountValue = (uint32_t)((tickTime * RTC_FREQ) / 1000000);

    rtcSetIntTicCnt(tickCountValue);

    mRtcTickInfo.repeat = repeat;
    mRtcTickInfo.count = tickCountValue;
    mRtcTickInfo.callback = callback;

    rtcEnableTick();
    NVIC_EnableIRQ(RTC_INTERRUPT_NO);
}

void rtcSetTickHandler(void (*callback)(void))
{
    if (callback == NULL) {
        PRINTF_ERROR("%s: callback is NULL\n", __func__);
        return;
    }

    mRtcTickInfo.callback = callback;
}

void rtcStopTick(void)
{
    rtcDisableTick();
    NVIC_DisableIRQ(RTC_INTERRUPT_NO);
}


//CHUB should not control TS tick. APM is in charge.
void rtcStopTSTick(void)
{
    (void)0;
    return;
}

void rtcStartTSTick(void)
{
    (void)0;
    return;
}

uint64_t rtcGetTime(void)
{
    return platGetTicks();
}

void rtc_IRQHandler(void)
{
    uint32_t status;

    status = rtcGetIntp();

    if (status & INTP_INT_TIC)
    {
        rtcDisableTick();
        NVIC_DisableIRQ(RTC_INTERRUPT_NO);

        if (mRtcTickInfo.callback != NULL)
            mRtcTickInfo.callback();

        rtcPendClear(INTP_INT_TIC);

        if (mRtcTickInfo.repeat)
        {
            rtcEnableTick();
            NVIC_EnableIRQ(RTC_INTERRUPT_NO);
        }
    }

}

#endif //RTC_REQUIRED
