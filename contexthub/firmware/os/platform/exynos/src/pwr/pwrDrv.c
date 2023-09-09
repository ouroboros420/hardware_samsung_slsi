/*----------------------------------------------------------------------------
 *      Exynos SoC  -  PWR
 *----------------------------------------------------------------------------
 *      Name:    pwrDrv.c
 *      Purpose: To implement power modes
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
#include <cmsis.h>
#if defined(SEOS)
  #include <platform.h>
  #include <timer.h>
#elif defined(EMBOS)
  #include <core_cm4.h>
  #include <Device.h>
  #include <RTOS.h>
#endif
#include <cpu.h>
#include <cmu.h>
#include <pwrDrv.h>
#include <mailbox.h>
#include <pwm.h>
#include <rtc.h>
#include <wdt.h>
#include <sysreg.h>
#include <debug.h>
#include <mct.h>
#if defined(LOCAL_POWERGATE)
#include <pwrDrvPwrGating.h>
#endif

#if defined(EMBOS)
void __iar_init_vfp(void);
#endif

#define PWR_ENABLE_DEBUG   0

#define PWR_INFO_PRINT(fmt, ...) do { \
    CSP_PRINTF_INFO("[PWR]" fmt, ##__VA_ARGS__); \
} while(0)

#if (PWR_ENABLE_DEBUG)
  #define PWR_DEBUG_PRINT(fmt, ...) do { \
        CSP_PRINTF_INFO("[PWR]" fmt, ##__VA_ARGS__); \
    } while(0)
#else
  #define PWR_DEBUG_PRINT(fmt, ...)
#endif

static bool mGnssWakeLock = false;
static bool mApWakeLock = true;
static bool mCM55WakeLock = false;
static bool mCM55BootLock = false;

uint32_t pdCnt;

#if defined(LOCAL_POWERGATE)
  #include CSP_SOURCE(pwrDrv)
#endif

void pwrDrvEnableApWakeLock(void)
{
    mApWakeLock = true;
    PWR_DEBUG_PRINT("AP Lock (%d)\n", mApWakeLock);
#if defined(MCT_SUPPORT) && defined(MCT_USE_RTC)
    mctSetClockState(true);
#endif
}

void pwrDrvDisableApWakeLock(void)
{
    mApWakeLock = false;
    PWR_DEBUG_PRINT("AP Unlock (%d)\n", mApWakeLock);
#if defined(MCT_SUPPORT) && defined(MCT_USE_RTC)
    mctSetClockState(false);
#endif
}

bool pwrDrvGetApWakeLock(void)
{
    return mApWakeLock;
}

void pwrDrvEnableGnssWakeLock(void)
{
    mGnssWakeLock = true;
    PWR_DEBUG_PRINT("GNSS Lock (%d)\n", mGnssWakeLock);
}

bool pwrDrvGetGnssWakeLock(void)
{
    return mGnssWakeLock;
}

void pwrDrvDisableGnssWakeLock(void)
{
    mGnssWakeLock = false;
    PWR_DEBUG_PRINT("GNSS Unrlock (%d)\n", mGnssWakeLock);
}

void pwrDrvEnableCM55WakeLock(void)
{
    mCM55WakeLock = true;
    PWR_DEBUG_PRINT("CM55 Lock (%d)\n", mCM55WakeLock);
}

void pwrDrvDisableCM55WakeLock(void)
{
    mCM55WakeLock = false;
    PWR_DEBUG_PRINT("CM55 Unlock (%d)\n", mCM55WakeLock);
}

void pwrDrvEnableCM55BootLock(void)
{
    mCM55BootLock = true;
    PWR_DEBUG_PRINT("CM55 Boot Lock (%d)\n", mCM55BootLock);
}

void pwrDrvDisableCM55BootLock(void)
{
    mCM55BootLock = false;
    PWR_DEBUG_PRINT("CM55 Boot Unlock (%d)\n", mCM55BootLock);
}

bool pwrDrvGetCM55WakeLock(void)
{
    return mCM55WakeLock;
}

bool pwrDrvGetWakeLock(void)
{
    //PWR_DEBUG_PRINT("Get WakeLock : %d/%d/%d\n", mGnssWakeLock, mApWakeLock, mCM55WakeLock);
    return (mGnssWakeLock || mApWakeLock || mCM55WakeLock || mCM55BootLock);
}


static void pwrDrvPrepareSleepSysTickEnabled(uint32_t data)
{
#if defined(SEOS)
    uint64_t intState;
    intState = cpuIntsOff();

    // Prevent processor to enter deep sleep
    SCB->SCR &=~ SCB_SCR_SLEEPDEEP_Msk;

    (void)data;

    perfMonitorSetEvt(chub_prof_wfi2_in);
    asm volatile ("wfi\n" "nop":::"memory");
    perfMonitorSetEvt(chub_prof_wfi2_out);

    // SysTick counts down to 0, which means SysTick gets expired and core wakes up by systick interrupt
    // Right after wakeup up from platSleep, timIntHandler in evtQueueDequeue can process software timer properly with updated os tick (mTimeAccumulated)
    if(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) {
        platSetOSTickVariable(TIME_OF_SYSTICK_IN_MS);
    } else {
        // Other interrupt wakes up core, therefore os tick (mTimeAccumulated) will be updated later when SysTick expires.
    }

    cpuIntsRestore(intState);
#endif
}

static void pwrDrvPrepareSleepSysTickDisabled(uint32_t data)
{
#if defined(SEOS)
    uint64_t intState;
    intState = cpuIntsOff();
    uint32_t cpuClkMux;
    uint32_t clkChanged = 0;

    if(data < 10)
        return;

    // Need to update the elapsed time of SysTick before disable it !!!!
    platSetOSTickVariable(platGetTicksSinceTheLastTick());

    // Disable SysTick
    //platStopSysTick();

    // Allow processor to enter deep sleep
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

    // Start PWM Timer for wakeup source to serve next software timer.
    // !!! To Do -> need to check side effect on 64bit -> 32bit down scaling of remainingTime
    //PWR_DEBUG_PRINT("pwrDrvPrepareSleepSysTickDisabled (%d)\n", (int)data);
    pwmSetTime(PWM_TIMER0, data);
    pwmStartTimer(PWM_TIMER0);

    perfMonitorSetEvt(chub_prof_wfi_in);
    if (data > 1000 && mApWakeLock == true && mCM55WakeLock == false) {
        clkChanged = 1;
        cpuClkMux = cmuSetCpuMux(CPU_MUX_LOW);
    }
    asm volatile ("wfi\n" "nop":::"memory");
    if (clkChanged) {
        cmuSetCpuMux(cpuClkMux);
    }
    perfMonitorSetEvt(chub_prof_wfi_out);

    // Update systick(mTimeAccumulated) and restart SysTick
    // Update OS Tick variable
    platSetOSTickVariableAfterSleep();

    // Re-start SysTick
    //platStartSysTick();

    cpuIntsRestore(intState);
#endif
}

#if defined(SUPPORT_RCO_DIVIDING)
#if defined(SEOS)
static void pwrDrvPrepareSleepRcoDividing(uint32_t data)
#else
    void pwrDrvPrepareSleepRcoDividing(uint32_t data)
#endif
{
    uint64_t intState;
    uint32_t intStatReg, intMask;

    intState = cpuIntsOff();
    //CSP_PRINTF_INFO("intState:%llu\n", intState);

    intStatReg = NVIC->ISER[0];
    //CSP_PRINTF_INFO("intStatReg:%lu\n", intStatReg);

    //Disable All Interrupts
    NVIC->ICER[0] = 0xFFFFFFFF;

    //all interrupt mask
    intMask = sysregGetMaskIrq();

    //Mask Irq except APM
    sysregSetMaskIrq(0x3FDFFF);

    //if pending interrupt
    if(intStatReg & NVIC->ISPR[0]) {
        //CSP_PRINTF_INFO("pending interrupt!\n");
        sysregClearMaskIrq(0x3FFFFF);
        sysregSetMaskIrq(intMask);

        NVIC->ISER[0] = intStatReg;
        cpuIntsRestore(intState);

        return;
    }

    //Clear All pending interrupt
    NVIC->ICPR[0] = 0xFFFFFFFF;

    // Allow processor to enter deep sleep
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

    // Send RCO divide to APM
    mailboxSetPowerMode(1);

    NVIC_ClearPendingIRQ(MB_APM_IRQn);

#if defined(SEOS)
    rtcSetTickTime(0, 0, data, NULL, 0);
#endif

    // enable APM mailbox INT
    NVIC_EnableIRQ(MB_APM_IRQn);

    asm volatile ("wfi\n" "nop":::"memory");

    //SYSREQ_CHUB_HWACG_CM4_CLKREQ_MASK_IRQ
    sysregClearMaskIrq(0x3FFFFF);
    sysregSetMaskIrq(intMask);

    //CSP_PRINTF_INFO("pending bit 0x%x\n", (unsigned int)NVIC->ISPR[0]);

    NVIC->ISER[0] = intStatReg;
    //CSP_PRINTF_INFO("intStatReg:%lu\n", NVIC->ISER[0]);
    cpuIntsRestore(intState);
}
#endif

typedef enum {
    PWR_SLEEP_MODE_CORE_CLOCK_GATING = 0,
    PWR_SLEEP_MODE_CORE_CLOCK_GATING_VARIABLE_SYSTICK,
#if defined(LOCAL_POWERGATE)
    PWR_SLEEP_MODE_PMU_PWR_GATING,
#endif
#if defined(SUPPORT_RCO_DIVIDING)
    PWR_SLEEP_MODE_RCO_DIVIDING,
#endif
    PWR_SLEEP_MODE_MAX
} PwrSleepModeType;

struct PwrSleepPrepareFncs {
    void (*prepare)(uint32_t);
    uint32_t data;
} static mPwrSleepPrepareFncs[] = {
    /* PWR_SLEEP_MODE_CORE_CLOCK_GATING */
    {
        .prepare = pwrDrvPrepareSleepSysTickEnabled,
        .data = 0
    },
    /* PWR_SLEEP_MODE_CORE_CLOCK_GATING_VARIABLE_SYSTICK */
    {
        .prepare = pwrDrvPrepareSleepSysTickDisabled,
        .data = 0
    },
#if defined(LOCAL_POWERGATE)
    /* PWR_SLEEP_PMU_PWR_GATING */
    {
        .prepare = pwrDrvPrepareSleepPwrGating,
        .data = 0
    }
#endif
#if defined(SUPPORT_RCO_DIVIDING)
    /* PWR_SLEEP_PMU_PWR_GATING */
    {
        .prepare = pwrDrvPrepareSleepRcoDividing,
        .data = 0
    }
#endif
};

#define TIME_TO_ENTER_VARIABLE_SYSTICK 1000000 // 1ms
//#define TIME_TO_ENTER_VARIABLE_SYSTICK 1000000000 // 1s
static PwrSleepModeType pwrDrvDecideSleepMode(void)
{
    // Till CTS timestamp failures get fixed, Variable Tick and Power gating will be disabled !!!!
    //return PWR_SLEEP_MODE_CORE_CLOCK_GATING;

    uint64_t nextTimeToWakeup;

#if defined(SEOS)
    nextTimeToWakeup = platGetNextWakeupTime();
#elif defined(EMBOS)
    /* Need to Implement */
    nextTimeToWakeup=1;
#endif

    // Neither pending software timer nor event in queue exists, we enter into pwr gating
    if(nextTimeToWakeup == 0)
    {
        // Checking SleepReadyReq. APM may or may not allow us to enter power-gating
#if defined(LOCAL_POWERGATE)
//#if (defined(MBAPM_REQUIRED) && !defined(s5e9110) && )
//        if (mailboxGetPowerMode()) { // <= Will test in EVT1
//#else
        if(!pwrDrvGetWakeLock()){
//#endif
            PWR_DEBUG_PRINT("Enter Sleep for %dus\n", PWM_MAX_TIME_TO_EXPIRE_IN_MICROSECOND);
            mPwrSleepPrepareFncs[PWR_SLEEP_MODE_PMU_PWR_GATING].data = PWM_MAX_TIME_TO_EXPIRE_IN_MICROSECOND; // 300 seconds
            return PWR_SLEEP_MODE_PMU_PWR_GATING;
        }
        else
#endif
        {
#if defined(SUPPORT_RCO_DIVIDING)
            if (!pwrDrvGetWakeLock()){
                mPwrSleepPrepareFncs[PWR_SLEEP_MODE_RCO_DIVIDING].data = PWM_MAX_TIME_TO_EXPIRE_IN_MICROSECOND;
                return PWR_SLEEP_MODE_RCO_DIVIDING;
            }
#endif
            mPwrSleepPrepareFncs[PWR_SLEEP_MODE_CORE_CLOCK_GATING_VARIABLE_SYSTICK].data = PWM_MAX_TIME_TO_EXPIRE_IN_MICROSECOND; // 300 seconds
            return PWR_SLEEP_MODE_CORE_CLOCK_GATING_VARIABLE_SYSTICK;
        }
    }
#if defined(LOCAL_POWERGATE)
    else if ((nextTimeToWakeup / 1000000) > 2 && pwrDrvGetWakeLock() == 0) {
            mPwrSleepPrepareFncs[PWR_SLEEP_MODE_PMU_PWR_GATING].data = (uint32_t)(nextTimeToWakeup / 1000);
            return PWR_SLEEP_MODE_PMU_PWR_GATING;
    }
#endif
    // We enter into clock gating with sys tick disabled
    else
    {
#if defined(SUPPORT_RCO_DIVIDING)
        if(!pwrDrvGetWakeLock()){
            mPwrSleepPrepareFncs[PWR_SLEEP_MODE_RCO_DIVIDING].data = (uint32_t)nextTimeToWakeup / 1000;
            return PWR_SLEEP_MODE_RCO_DIVIDING;
        }
#endif
        mPwrSleepPrepareFncs[PWR_SLEEP_MODE_CORE_CLOCK_GATING_VARIABLE_SYSTICK].data = (uint32_t)nextTimeToWakeup / 1000;
        return PWR_SLEEP_MODE_CORE_CLOCK_GATING_VARIABLE_SYSTICK;
    }

    //return PWR_SLEEP_MODE_CORE_CLOCK_GATING;
}

static void pwrDrvPrepareSleep(PwrSleepModeType mode)
{
    mPwrSleepPrepareFncs[mode].prepare(mPwrSleepPrepareFncs[mode].data);
}

static void pwrDrvPostSleep(void)
{
    // Do something if needed
}

void pwrDrvSleep(void)
{
#ifdef EXYNOS_CONTEXTHUB
    wdtDisableClk();
#endif
    pwrDrvPrepareSleep(pwrDrvDecideSleepMode());
    pwrDrvPostSleep();
#ifdef EXYNOS_CONTEXTHUB
    wdtSetTime(WDT_DEFAULT_TIMEOUT);
    wdtEnableClk();
#endif

}
