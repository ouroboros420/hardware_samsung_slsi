/*----------------------------------------------------------------------------
 *      Exynos SoC  -  PWM
 *----------------------------------------------------------------------------
 *      Name:    pwm.c
 *      Purpose: To implement PWM APIs
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

#include <csp_common.h>
#include <cpu/cpu.h>
#include <cpu.h>
#if defined(SEOS)
    #include <errno.h>
    #include <cmsis.h>
#endif
#include <pwm.h>

#define REG_PWM_TCFG0           (PWM_BASE_ADDRESS + 0x00)
#define REG_PWM_TCFG1           (PWM_BASE_ADDRESS + 0x04)
#define REG_PWM_TCON            (PWM_BASE_ADDRESS + 0x08)
#define REG_PWM_TCNTB0          (PWM_BASE_ADDRESS + 0x0C)
#define REG_PWM_TCMPB0          (PWM_BASE_ADDRESS + 0x10)
#define REG_PWM_TCNTO0          (PWM_BASE_ADDRESS + 0x14)
#define REG_PWM_TCNTB1          (PWM_BASE_ADDRESS + 0x18)
#define REG_PWM_TCMPB1          (PWM_BASE_ADDRESS + 0x1C)
#define REG_PWM_TCNTO1          (PWM_BASE_ADDRESS + 0x20)
#define REG_PWM_TCNTB2          (PWM_BASE_ADDRESS + 0x24)
#define REG_PWM_TCMPB2          (PWM_BASE_ADDRESS + 0x28)
#define REG_PWM_TCNTO2          (PWM_BASE_ADDRESS + 0x2C)
#define REG_PWM_TCNTB3          (PWM_BASE_ADDRESS + 0x30)
#define REG_PWM_TCMPB3          (PWM_BASE_ADDRESS + 0x34)
#define REG_PWM_TCNTO3          (PWM_BASE_ADDRESS + 0x38)
#define REG_PWM_TCNTB4          (PWM_BASE_ADDRESS + 0x3C)
#define REG_PWM_TCMPB4          (0)
#define REG_PWM_TCNTO4          (PWM_BASE_ADDRESS + 0x40)
#define REG_PWM_TINT_CSTAT      (PWM_BASE_ADDRESS + 0x44)

#define PWM_OSC_CLOCK               OSC_CLOCK
#define PWM_PREDIVIDER              2
#define PWM_TIMER_CLOCK             (PWM_OSC_CLOCK / PWM_PREDIVIDER)  /* 24576000 / 2 */
#define PWM_COUNT_MS                (PWM_TIMER_CLOCK/1000)
#define PWM_COUNT_MAX               0xFFFFFFFF
#define PWM_MAX_MS                  (PWM_COUNT_MAX/PWM_COUNT_MS)

#define PWM_TIMER_CLOSE             0
#define PWM_TIMER_OPEN              1

#define PWM_DEFAULT_TOUT_LEVEL      LOW
#define PWM_DEFAULT_DUTY            (50)

#define PWM_TIMER0_CON_BIT_OFFSET   0
#define PWM_TIMER1_CON_BIT_OFFSET   8
#define PWM_TIMER2_CON_BIT_OFFSET   12
#define PWM_TIMER3_CON_BIT_OFFSET   16
#define PWM_TIMER4_CON_BIT_OFFSET   20

#define PWM_START_BIT               0
#define PWM_UPDATE_BIT              1
#define PWM_INVERT_BIT              2
#define PWM_RELOAD_BIT              3

#define PWM_INT_STATUS_BIT_OFFSET   5
#define PWM_INT_STATUS_MASK         0x1F

#define PWM_REG_ADDR(id) \
    REG_PWM_TCNTB##id, \
    REG_PWM_TCMPB##id, \
    REG_PWM_TCNTO##id, \
    PWM_TIMER##id##_CON_BIT_OFFSET

#define PWM_INVALID_ID      0
#define PWM_INVALID_MODE    1
#define PWM_INVALID_DUTY    2

#define DEC_PWM(ch, dutyratio, irq) { \
    .duty = dutyratio,    \
    .irqNum = irq,   \
    .tcntb_addr = REG_PWM_TCNTB##ch, \
    .tcmpb_addr = REG_PWM_TCMPB##ch, \
    .tcnto_addr = REG_PWM_TCNTO##ch, \
    .con_offset = PWM_TIMER##ch##_CON_BIT_OFFSET, }

typedef struct {
    uint32_t period; /* usecond period */
    uint32_t count;
    uint32_t duty;
    uint32_t start;
    uint32_t tcntb_addr;
    uint32_t tcmpb_addr;
    uint32_t tcnto_addr;
    uint32_t con_offset;
    uint32_t irqNum;
    void (*handler)(void);
}PwmTimerInfoType;

PwmTimerInfoType mPwmTimerInfo[PWM_TIMER_NUM_MAX] = {
    DEC_PWM(0, PWM_DEFAULT_DUTY, PWM0_CHUB_IRQn),
    DEC_PWM(1, PWM_DEFAULT_DUTY, PWM1_CHUB_IRQn),
    DEC_PWM(2, PWM_DEFAULT_DUTY, PWM2_CHUB_IRQn),
    DEC_PWM(3, PWM_DEFAULT_DUTY, PWM3_CHUB_IRQn),
#if (SUPPORT_PWM_TIMER4)
    DEC_PWM(4, PWM_DEFAULT_DUTY, PWM4_CHUB_IRQn),
#endif
};

#define PRINTF_ERROR CSP_PRINTF_ERROR
#define PRINTF_INFO  CSP_PRINTF_INFO

static uint32_t pwmCalcDutyCycle(PwmTimerNumType timer);
static void pwmClearPend(PwmTimerNumType timer);
static int pwmCheckTimer(PwmTimerNumType timer, int fid);

static int pwmCheckTimer(PwmTimerNumType timer, int fid)
{
    if( timer >= PWM_TIMER_NUM_MAX )
    {
        PRINTF_ERROR("pwm(%d) : Invalid timer(%d)\n", fid, timer);
        CSP_ASSERT(0);
        return -1;
    }

    return 0;
}

static uint32_t pwmCalcDutyCycle(PwmTimerNumType timer)
{
    uint32_t count;

    count = (mPwmTimerInfo[timer].count * mPwmTimerInfo[timer].duty) / 100;

    return count;
}

int pwmStartTimer(PwmTimerNumType timer)
{
    uint32_t regValue;

    if (pwmCheckTimer(timer, 0))
      return -1;

    if (mPwmTimerInfo[timer].start == 1)
        pwmStopTimer(timer);

    regValue = __raw_readl(REG_PWM_TCON);
    regValue |= (0x1 << (mPwmTimerInfo[timer].con_offset + PWM_START_BIT));
    __raw_writel(regValue, REG_PWM_TCON);

    mPwmTimerInfo[timer].start = 1;

    return 0;
}

int pwmStopTimer(PwmTimerNumType timer)
{
    uint32_t regValue;

    if (pwmCheckTimer(timer, 1))
      return -1;

    regValue = __raw_readl(REG_PWM_TCON);
    regValue &= ~(0x1 << (mPwmTimerInfo[timer].con_offset + PWM_START_BIT));
    __raw_writel(regValue, REG_PWM_TCON);

    mPwmTimerInfo[timer].start = 0;

    return 0;
}

// Public API to initialize PWM. This should be called when OS starts
int pwmInit( PwmTimerNumType timer, uint32_t uSecond,
             PwmTimerModeType mode, uint32_t intEn )
{
    uint32_t count, dutyCycle;
    uint32_t mSec, uSec;
    uint32_t regValue, start;

    if (pwmCheckTimer(timer, 2))
      return -1;

    if( mode >= PWM_TIMER_MODE_MAX )
    {
        PRINTF_ERROR("pwmInit : Invalid mode(%d)\n", mode);
        CSP_ASSERT(0);
        return -1;
    }

    if( uSecond == 0 )
    {
        PRINTF_ERROR("pwmInit : Invalid time(%lu)\n", uSecond);
        CSP_ASSERT(0);
        return -1;
    }

    mSec = uSecond / 1000;
    uSec = uSecond % 1000;

    if (mSec > PWM_MAX_MS) {
        PRINTF_ERROR("pwmInit : time is too big (%lu > %u)\n", mSec, PWM_MAX_MS);
        return -1;
    }
    count = mSec * PWM_COUNT_MS;
    count = count + (PWM_COUNT_MS * uSec) / 1000;

    mPwmTimerInfo[timer].period = uSecond;
    mPwmTimerInfo[timer].count = count;

    dutyCycle = pwmCalcDutyCycle(timer);

    start = mPwmTimerInfo[timer].start;
    if (start)
        pwmStopTimer(timer);

    __raw_writel(count, mPwmTimerInfo[timer].tcntb_addr);
    if (mPwmTimerInfo[timer].tcmpb_addr)
        __raw_writel(dutyCycle, mPwmTimerInfo[timer].tcmpb_addr);

    /* Manual Update */
    regValue = __raw_readl(REG_PWM_TCON);

    if (mode == PWM_TIMER_MODE_INTERVAL) {
#if (SUPPORT_PWM_TIMER4)
        if (timer == PWM_TIMER4)
            regValue |= (0x1 << (mPwmTimerInfo[timer].con_offset + PWM_INVERT_BIT));
        else
#endif
            regValue |= (0x1 << (mPwmTimerInfo[timer].con_offset + PWM_RELOAD_BIT));
    } else {
#if (SUPPORT_PWM_TIMER4)
        if (timer == PWM_TIMER4)
            regValue &= ~(0x1 << (mPwmTimerInfo[timer].con_offset + PWM_INVERT_BIT));
        else
#endif
            regValue &= ~(0x1 << (mPwmTimerInfo[timer].con_offset + PWM_RELOAD_BIT));
    }

    regValue |= (0x1 << (mPwmTimerInfo[timer].con_offset + PWM_UPDATE_BIT));
    __raw_writel(regValue, REG_PWM_TCON);
    /* Clear Manual Update */
    regValue &= ~(0x1 << (mPwmTimerInfo[timer].con_offset + PWM_UPDATE_BIT));
    __raw_writel(regValue, REG_PWM_TCON);

    if (start)
        pwmStartTimer(timer);

    if( intEn )
        pwmEnableInterrupt(timer);

    return 0;
}

static void pwmClearPend(PwmTimerNumType timer)
{
    uint32_t regValue;

    regValue = __raw_readl(REG_PWM_TINT_CSTAT);
    regValue &= PWM_INT_STATUS_MASK;
    regValue |= (0x1<< (PWM_INT_STATUS_BIT_OFFSET + timer));
    __raw_writel(regValue, REG_PWM_TINT_CSTAT);
}

int pwmEnableInterrupt(PwmTimerNumType timer )
{
    uint32_t regValue;

    if (pwmCheckTimer(timer, 3))
      return -1;

    pwmClearPend(timer);

    regValue = __raw_readl(REG_PWM_TINT_CSTAT);
    regValue &= PWM_INT_STATUS_MASK;
    regValue |= (0x1 << timer);
    __raw_writel(regValue, REG_PWM_TINT_CSTAT);

    NVIC_EnableIRQ(mPwmTimerInfo[timer].irqNum);

    return 0;
}


int pwmDisableInterrupt(PwmTimerNumType timer )
{
    uint32_t regValue;

    if (pwmCheckTimer(timer, 4))
      return -1;

    regValue = __raw_readl( REG_PWM_TINT_CSTAT );
    regValue &= PWM_INT_STATUS_MASK;
    regValue &= ~(0x1 << timer );
    __raw_writel( regValue, REG_PWM_TINT_CSTAT );

    NVIC_DisableIRQ(mPwmTimerInfo[timer].irqNum);
    return 0;
}

void pwmStopAllTimer( void )
{
    PwmTimerNumType timer;

    for(timer = PWM_TIMER_NUM_MIN ; timer < PWM_TIMER_NUM_MAX ; timer++ )
        pwmStopTimer( timer );
}

int pwmSetDuty(PwmTimerNumType timer, uint32_t duty )
{
    uint32_t dutyCycle;

    if (pwmCheckTimer(timer, 5))
      return -1;

    if( duty == 0 || duty >= 100 )
    {
        //PRINTF_ERROR("pwmSetDuty : Invalid duty(%lu)\n", duty);
        CSP_ASSERT(0);
        return -1;
    }

    mPwmTimerInfo[timer].duty = duty;

    dutyCycle = pwmCalcDutyCycle(timer);
    __raw_writel(dutyCycle, mPwmTimerInfo[timer].tcmpb_addr);

    return 0;
}

int pwmSetMode(PwmTimerNumType timer, PwmTimerModeType mode )
{
    uint32_t regValue;

    if (pwmCheckTimer(timer, 6))
      return -1;

    regValue = __raw_readl(REG_PWM_TCON);

    if (mode == PWM_TIMER_MODE_INTERVAL) {
#if (SUPPORT_PWM_TIMER4)
        if (timer == PWM_TIMER4)
            regValue |= (0x1 << (mPwmTimerInfo[timer].con_offset + PWM_INVERT_BIT));
        else
#endif
            regValue |= (0x1 << (mPwmTimerInfo[timer].con_offset + PWM_RELOAD_BIT));
    } else {
#if (SUPPORT_PWM_TIMER4)
        if (timer == PWM_TIMER4)
            regValue &= ~(0x1 << (mPwmTimerInfo[timer].con_offset + PWM_INVERT_BIT));
        else
#endif
            regValue &= ~(0x1 << (mPwmTimerInfo[timer].con_offset + PWM_RELOAD_BIT));
    }

    __raw_writel( regValue, REG_PWM_TCON );

    return 0;
}

int pwmSetDefaultLevel(PwmTimerNumType timer, uint32_t level )
{
    uint32_t regValue;

    if (pwmCheckTimer(timer, 7))
      return -1;

    regValue = __raw_readl( REG_PWM_TCON );

    if( level == HIGH )
        regValue &= ~(0x1 << (mPwmTimerInfo[timer].con_offset + PWM_INVERT_BIT));
    else
        regValue |= (0x1 << (mPwmTimerInfo[timer].con_offset + PWM_INVERT_BIT));

    __raw_writel( regValue, REG_PWM_TCON );

    return 0;
}

uint32_t pwmGetCount(PwmTimerNumType timer)
{
    uint32_t regValue;

    if (pwmCheckTimer(timer, 8))
      return -1;

    regValue = __raw_readl( mPwmTimerInfo[timer].tcnto_addr );

    return mPwmTimerInfo[timer].count - regValue;
}

#if defined(SFR_SAVE_RESTORE)
// Public API to save PWM SFRs before entering system power mode
void pwmSaveState(void)
{

}

// Public API to restore PWM SFRs after exiting system power mode
void pwmRestoreState(void)
{

}
#endif

// Public API to shutdown PWM
int pwmClose(PwmTimerNumType timer )
{
    if (pwmCheckTimer(timer, 9))
      return -1;

    pwmStopTimer(timer);

    mPwmTimerInfo[timer].period = 0;
    mPwmTimerInfo[timer].count = 0;
    mPwmTimerInfo[timer].start = 0;

    return 0;
}


int pwmSetTime(PwmTimerNumType timer, uint32_t uSecond )
{
    uint32_t count;
    uint32_t dutyCycle;
    uint32_t regValue;
    uint32_t start;
    uint32_t mSec, uSec;

    if (pwmCheckTimer(timer, 10))
      return -1;

    mSec = uSecond / 1000;
    uSec = uSecond % 1000;

    if( mSec > PWM_MAX_MS || uSecond == 0) {
        //PRINTF_ERROR("pwmSetTime: Invalid time(%lu us)\n", uSecond);
        CSP_ASSERT(0);
        return -1;
    }

    count = mSec * PWM_COUNT_MS ;
    count = count + (PWM_COUNT_MS * uSec) / 1000;

    mPwmTimerInfo[timer].period = uSecond;
    mPwmTimerInfo[timer].count = count;

    dutyCycle = pwmCalcDutyCycle(timer);

    start = mPwmTimerInfo[timer].start;
    if (start)
        pwmStopTimer(timer);

    __raw_writel( count, mPwmTimerInfo[timer].tcntb_addr );
    if (mPwmTimerInfo[timer].tcmpb_addr)
        __raw_writel( dutyCycle, mPwmTimerInfo[timer].tcmpb_addr);

    regValue = __raw_readl( REG_PWM_TCON );
    regValue |= (0x1 << (mPwmTimerInfo[timer].con_offset + PWM_UPDATE_BIT));
    __raw_writel( regValue, REG_PWM_TCON ); /* Manual Update */
    regValue &= ~(0x1 << (mPwmTimerInfo[timer].con_offset + PWM_UPDATE_BIT));
    __raw_writel( regValue, REG_PWM_TCON ); /* Manual Update clear*/

    if( start )
        pwmStartTimer(timer);

    return 0;
}

uint32_t pwmGetCurrentTime(PwmTimerNumType timer)
{
    uint64_t count;
    uint64_t uSec;

    if (pwmCheckTimer(timer, 11))
      return -1;

    count = mPwmTimerInfo[timer].count - __raw_readl( mPwmTimerInfo[timer].tcnto_addr );
    uSec = (count * 1000) / PWM_COUNT_MS; // * 100 instead of *1000 to avoid 32bit overflow

    return (uint32_t)uSec;
}

static uint64_t adjustTime;
uint64_t timeStamp;
uint64_t lastTime;

uint64_t pwmGetTimeStampUS(void)
{
    uint64_t count;
    uint64_t uSec;
    uint32_t irqState;

    irqState = cpuIntsOff();

    count = mPwmTimerInfo[TS_PWM_ID].count - __raw_readl( mPwmTimerInfo[TS_PWM_ID].tcnto_addr );
    uSec = (count * 1000) / PWM_COUNT_MS;
    if ( uSec < lastTime)
        adjustTime = adjustTime + mPwmTimerInfo[TS_PWM_ID].period;
    lastTime = uSec;
    uSec += adjustTime;

    cpuIntsRestore(irqState);

    return uSec;
}

void pwmSetHandler(PwmTimerNumType timer, void (*handler)(void))
{
    if (pwmCheckTimer(timer, 12))
      return ;

    mPwmTimerInfo[timer].handler = handler;
}

void pwm0_IRQHandler(void)
{
    if (mPwmTimerInfo[PWM_TIMER0].handler)
        mPwmTimerInfo[PWM_TIMER0].handler();
    pwmClearPend(PWM_TIMER0);
}

void pwm1_IRQHandler(void)
{
    if (mPwmTimerInfo[PWM_TIMER1].handler)
        mPwmTimerInfo[PWM_TIMER1].handler();
    pwmClearPend(PWM_TIMER1);
}

void pwm2_IRQHandler(void)
{
    if (mPwmTimerInfo[PWM_TIMER2].handler)
        mPwmTimerInfo[PWM_TIMER2].handler();
    pwmClearPend(PWM_TIMER2);
}

void pwm3_IRQHandler(void)
{
    if (mPwmTimerInfo[PWM_TIMER3].handler)
        mPwmTimerInfo[PWM_TIMER3].handler();
    pwmClearPend(PWM_TIMER3);
}

void pwm4_IRQHandler(void)
{
#if (SUPPORT_PWM_TIMER4)
    if (mPwmTimerInfo[PWM_TIMER4].handler)
        mPwmTimerInfo[PWM_TIMER4].handler();
    pwmClearPend(PWM_TIMER4);
#endif
}
