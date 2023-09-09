/*----------------------------------------------------------------------------
 *      Exynos SoC  -  PWM
 *----------------------------------------------------------------------------
 *      Name:    pwmDrv.c
 *      Purpose: To implement PWM driver functions
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
#if defined(SEOS)
    #include <errno.h>
#endif

#include <pwm.h>
#include <pwmDrv.h>


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
#define PWM_DEFAULT_DUTY            50

#define PWM_TIMER0_CON_BIT_OFFSET   0
#define PWM_TIMER1_CON_BIT_OFFSET   8
#define PWM_TIMER2_CON_BIT_OFFSET   12
#define PWM_TIMER3_CON_BIT_OFFSET   16

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

typedef struct {
    uint32_t period; /* usecond period */
    uint32_t count;
    uint32_t duty;
    uint32_t start;
    uint32_t tcntb_addr;
    uint32_t tcmpb_addr;
    uint32_t tcnto_addr;
    uint32_t con_offset;
}PwmTimerInfoType;

PwmTimerInfoType mPwmTimerInfo[PWM_TIMER_NUM_MAX] = {
    { 1000, PWM_COUNT_MS, PWM_DEFAULT_DUTY, 0, PWM_REG_ADDR(0)},
#if defined(PWM_REQUIRED)
    { 1000, PWM_COUNT_MS, PWM_DEFAULT_DUTY, 0, PWM_REG_ADDR(1)},
    { 1000, PWM_COUNT_MS, PWM_DEFAULT_DUTY, 0, PWM_REG_ADDR(2)},
    { 1000, PWM_COUNT_MS, PWM_DEFAULT_DUTY, 0, PWM_REG_ADDR(3)},
#endif
};

static uint32_t pwmDrvCalcCount(uint32_t uSecond)
{
    uint32_t mSec, uSec;
    uint32_t count;

    mSec = uSecond / 1000;
    uSec = uSecond % 1000;

    if( mSec > PWM_MAX_MS )
        return 0;

    count = mSec * PWM_COUNT_MS ;
    count = count + (PWM_COUNT_MS * uSec) / 1000;

    return count;
}

static uint32_t pwmDrvCalcDutyCycle(PwmTimerNumType timer)
{
    uint32_t count;

    count = (mPwmTimerInfo[timer].count * mPwmTimerInfo[timer].duty) / 100;

    return count;
}

int pwmDrvOpen( PwmRequestType *request)
{
    int ret;

    if( request->timer >= PWM_TIMER_NUM_MAX )
        return -EINVAL;

    ret = pwmDrvSetTime( request->timer, request->period);
    if( ret < 0 )
        return ret;

    pwmDrvSetDuty( request->timer, PWM_DEFAULT_DUTY );
    pwmDrvSetMode( request->timer, request->mode );
    pwmDrvSetDefaultLevel( request->timer, PWM_DEFAULT_TOUT_LEVEL );

    if( request->interruptEn)
        pwmDrvEnableInterrupt( request->timer );
    else
        pwmDrvDisableInterrupt( request->timer);

    return 0;
}

int pwmDrvClearPend( PwmTimerNumType timer)
{
    uint32_t regValue;

    if( timer >= PWM_TIMER_NUM_MAX )
        return -EINVAL;

    regValue = __raw_readl( REG_PWM_TINT_CSTAT );

    regValue &= PWM_INT_STATUS_MASK;
    regValue |= (0x1<< (PWM_INT_STATUS_BIT_OFFSET + timer));

    __raw_writel( regValue, REG_PWM_TINT_CSTAT );

    return 0;
}

int pwmDrvEnableInterrupt( PwmTimerNumType timer )
{
    uint32_t regValue;

    if( timer >= PWM_TIMER_NUM_MAX )
        return -EINVAL;

    pwmDrvClearPend( timer );

    regValue = __raw_readl( REG_PWM_TINT_CSTAT );

    regValue &= PWM_INT_STATUS_MASK;
    regValue |= (0x1 << timer );

    __raw_writel( regValue, REG_PWM_TINT_CSTAT );

    return 0;
}

int pwmDrvDisableInterrupt( PwmTimerNumType timer )
{
    uint32_t regValue;

    if( timer >= PWM_TIMER_NUM_MAX )
        return -EINVAL;

    regValue = __raw_readl( REG_PWM_TINT_CSTAT );

    regValue &= PWM_INT_STATUS_MASK;
    regValue &= ~(0x1 << timer );

    __raw_writel( regValue, REG_PWM_TINT_CSTAT );

    return 0;
}

int pwmDrvStartTimer( PwmTimerNumType timer )
{
    uint32_t regValue;

    if( timer >= PWM_TIMER_NUM_MAX )
        return -EINVAL;

    if( mPwmTimerInfo[timer].count == 0 )
        return -EINVAL;

    regValue = __raw_readl( REG_PWM_TCON);

    // Stop timer if enabled
    if( mPwmTimerInfo[timer].start == 1 )
           pwmDrvStopTimer( timer );

    regValue |= (0x1 << (mPwmTimerInfo[timer].con_offset + PWM_START_BIT));

    __raw_writel( regValue, REG_PWM_TCON );

    mPwmTimerInfo[timer].start = 1;

    return 0;
}

int pwmDrvStopTimer( PwmTimerNumType timer )
{
    uint32_t regValue;

    if( timer >= PWM_TIMER_NUM_MAX )
        return -EINVAL;

    regValue = __raw_readl( REG_PWM_TCON);

    regValue &= ~(0x1 << (mPwmTimerInfo[timer].con_offset + PWM_START_BIT));

    __raw_writel( regValue, REG_PWM_TCON );

    mPwmTimerInfo[timer].start = 0;

    return 0;
}

void pwmDrvStopAllTimer( void )
{
    PwmTimerNumType i;

    for(i = PWM_TIMER_NUM_MIN ; i < PWM_TIMER_NUM_MAX ; i++ )
        pwmDrvStopTimer( i );
}

int pwmDrvSetTime( PwmTimerNumType timer, uint32_t uSecond )
{
    uint32_t count;
    uint32_t dutycycle;
    uint32_t regValue;

    if( timer >= PWM_TIMER_NUM_MAX )
        return -EINVAL;

    count = pwmDrvCalcCount(uSecond);

    if( count == 0 )
        return -EINVAL;

    mPwmTimerInfo[timer].period = uSecond;
    mPwmTimerInfo[timer].count = count;

    dutycycle = pwmDrvCalcDutyCycle( timer );

    if( mPwmTimerInfo[timer].start )
        pwmDrvStopTimer(timer);

    __raw_writel( count, mPwmTimerInfo[timer].tcntb_addr );
    __raw_writel( dutycycle, mPwmTimerInfo[timer].tcmpb_addr);

    regValue = __raw_readl( REG_PWM_TCON );
    regValue |= (0x1 << (mPwmTimerInfo[timer].con_offset + PWM_UPDATE_BIT));
    __raw_writel( regValue, REG_PWM_TCON ); /* Manual Update */
    regValue &= ~(0x1 << (mPwmTimerInfo[timer].con_offset + PWM_UPDATE_BIT));
    __raw_writel( regValue, REG_PWM_TCON ); /* Manual Update clear*/

    if( mPwmTimerInfo[timer].start )
        pwmDrvStartTimer(timer);

    return 0;
}

int pwmDrvSetDuty( PwmTimerNumType timer, uint32_t duty )
{
    uint32_t dutycycle;

    if( timer >= PWM_TIMER_NUM_MAX )
        return -EINVAL;

    if( duty == 0 || duty >= 100 )
        return -EINVAL;

    mPwmTimerInfo[timer].duty = duty;

    dutycycle = pwmDrvCalcDutyCycle( timer );
    __raw_writel( dutycycle, mPwmTimerInfo[timer].tcmpb_addr );

    return 0;
}

int pwmDrvSetMode( PwmTimerNumType timer, PwmTimerModeType mode )
{
    uint32_t regValue;

    if( timer >= PWM_TIMER_NUM_MAX )
        return -EINVAL;

    regValue = __raw_readl( REG_PWM_TCON );

    if( mode == PWM_TIMER_MODE_INTERVAL )
        regValue |= (0x1 << (mPwmTimerInfo[timer].con_offset + PWM_RELOAD_BIT));
    else
        regValue &= ~(0x1 << (mPwmTimerInfo[timer].con_offset + PWM_RELOAD_BIT));

    __raw_writel( regValue, REG_PWM_TCON );

    return 0;
}

int pwmDrvSetDefaultLevel( PwmTimerNumType timer, uint32_t level )
{
    uint32_t regValue;

    if( timer >= PWM_TIMER_NUM_MAX )
        return -EINVAL;

    regValue = __raw_readl( REG_PWM_TCON );

    if( level == HIGH )
        regValue &= ~(0x1 << (mPwmTimerInfo[timer].con_offset + PWM_INVERT_BIT));
    else
        regValue |= (0x1 << (mPwmTimerInfo[timer].con_offset + PWM_INVERT_BIT));

    __raw_writel( regValue, REG_PWM_TCON );

    return 0;
}

uint32_t pwmDrvGetCount( PwmTimerNumType timer)
{
    uint32_t regValue;

    if( timer >= PWM_TIMER_NUM_MAX )
        return 0;

    regValue = __raw_readl( mPwmTimerInfo[timer].tcnto_addr );

    return mPwmTimerInfo[timer].count - regValue;
}

uint32_t pwmDrvGetCurrentTime( PwmTimerNumType timer)
{
    uint64_t count;
    uint64_t uSec;

    if( timer >= PWM_TIMER_NUM_MAX )
        return 0;

    count = mPwmTimerInfo[timer].count - __raw_readl( mPwmTimerInfo[timer].tcnto_addr );
    uSec = (count * 1000) / PWM_COUNT_MS; // * 100 instead of *1000 to avoid 32bit overflow

    return (uint32_t)uSec;
}

uint32_t pwmDrvGetRemainTime( PwmTimerNumType timer)
{
    uint64_t count;
    uint64_t uSec;

    if( timer >= PWM_TIMER_NUM_MAX )
        return 0;

    count = __raw_readl( mPwmTimerInfo[timer].tcnto_addr );
    uSec = (count * 1000) / PWM_COUNT_MS;

    return (uint32_t)uSec;
}

int pwmDrvClose ( PwmTimerNumType timer )
{
    if( timer >= PWM_TIMER_NUM_MAX )
        return -EINVAL;

    pwmDrvStopTimer(timer);

    mPwmTimerInfo[timer].period = 0;
    mPwmTimerInfo[timer].count = 0;
    mPwmTimerInfo[timer].start = 0;

    return 0;
}

