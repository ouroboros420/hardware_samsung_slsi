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
#endif
#include <cpu.h>
#include <pwrDrv.h>
#include <mailbox.h>
#include <pwm.h>
#include <timer.h>
#include <rtc.h>
#include <wdt.h>
#include <sysreg.h>
#include <debug.h>
#if defined(LOCAL_POWERGATE)
#include <pwrDrvPwrGating.h>
#endif

static void pwrDrvPrepareSleepSysTickEnabled(uint32_t data)
{
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
}

static void pwrDrvPrepareSleepSysTickDisabled(uint32_t data)
{
    uint64_t intState;
    intState = cpuIntsOff();

    if(data < 10)
        return;

    // Need to update the elapsed time of SysTick before disable it !!!!
    platSetOSTickVariable(platGetTicksSinceTheLastTick());

#if defined(SEOS)
    // Disable SysTick
    //platStopSysTick();
#endif

    // Allow processor to enter deep sleep
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

    // Start PWM Timer for wakeup source to serve next software timer.
    // !!! To Do -> need to check side effect on 64bit -> 32bit down scaling of remainingTime
    // CSP_PRINTF_INFO("pwrDrvPrepareSleepSysTickDisabled (%d)\n", (int)data);
    pwmSetTime(PWM_TIMER0, data);
    pwmStartTimer(PWM_TIMER0);

    perfMonitorSetEvt(chub_prof_wfi_in);
    saveNvicRegister();
    asm volatile ("wfi\n" "nop":::"memory");
    perfMonitorSetEvt(chub_prof_wfi_out);

    // Update systick(mTimeAccumulated) and restart SysTick
#if defined(SEOS)
    // Update OS Tick variable
    platSetOSTickVariableAfterSleep();

    // Re-start SysTick
    //platStartSysTick();
#endif

    cpuIntsRestore(intState);
}

#if defined(LOCAL_POWERGATE)
void pwrDrvPrepareSleepPwrGating(uint32_t data)
{
    uint64_t intState;
    uint32_t i;
    uint32_t intMask;

#if defined(SEOS)
    intState = cpuIntsOff();
#endif

    // Need to update the elapsed time of SysTick before entering into pwr-gating !!!!
    platSetOSTickVariable(platGetTicksSinceTheLastTick());

    // Disable SysTick
#if defined(SEOS)
    platStopSysTick();
#endif

    // Allow processor to enter deep sleep
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

#if defined(MBAPM_REQUIRED)
    mailboxSetPowerMode(1);
#endif


#if defined(RTC_REQUIRED)
    // Start RTC Tick1 for wakeup source to serve next software timer.
    rtcSetTickTime(0, 0, data, NULL, 0);
#endif

    // Save SFRs before they are lost
    pwrDrvSaveState();

    // Save SCB, NVIC, MPU
    pwrDrvSaveCorePeri();

    // To Do -> need to optimize to reduce the execution time for codes below ...
    for (i = 0; i < NUM_INTERRUPTS; i++) {
        NVIC_DisableIRQ(i);
        //NVIC_ClearPendingIRQ(i);
    }

    /* Set boot mode power gating */
    ipc_set_chub_bootmode(BOOTMODE_PWRGATING, ipc_get_chub_rtlogmode());

    // Save CPU GPRs and call wfi
    pwrDrvPwrGating();
    //asm volatile ("wfi\n" "nop":::"memory");

    // Restore SCB, NVIC, MPU
    pwrDrvRestoreCorePeri();

    // Restore SFRs
    pwrDrvRestoreState();

    sysregSetMaskIrq(intMask);

    // Will get back here from bl by APM to power-up CHUB

    // Update OS Tick variable
    platSetOSTickVariableAfterPwrGating();

    // Re-start SysTick
#if defined(SEOS)
    platStartSysTick();

    cpuIntsRestore(intState);
#endif
}
#endif

typedef enum {
    PWR_SLEEP_MODE_CORE_CLOCK_GATING = 0,
    PWR_SLEEP_MODE_CORE_CLOCK_GATING_VARIABLE_SYSTICK,
#if defined(LOCAL_POWERGATE)
    PWR_SLEEP_MODE_PMU_PWR_GATING,
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
#endif

    // Neither pending software timer nor event in queue exists, we enter into pwr gating
    if(nextTimeToWakeup == 0)
    {
        // Checking SleepReadyReq. APM may or may not allow us to enter power-gating
#if defined(LOCAL_POWERGATE)
#if defined(MBAPM_REQUIRED)
        if (mailboxGetPowerMode()) { // <= Will test in EVT1
#else
        if(!pwrDrvGetWakeLock()){
#endif
            mPwrSleepPrepareFncs[PWR_SLEEP_MODE_PMU_PWR_GATING].data = PWM_MAX_TIME_TO_EXPIRE_IN_MICROSECOND; // 300 seconds
            return PWR_SLEEP_MODE_PMU_PWR_GATING;
        }
        else
#endif
        {
            mPwrSleepPrepareFncs[PWR_SLEEP_MODE_CORE_CLOCK_GATING_VARIABLE_SYSTICK].data = PWM_MAX_TIME_TO_EXPIRE_IN_MICROSECOND; // 300 seconds
            return PWR_SLEEP_MODE_CORE_CLOCK_GATING_VARIABLE_SYSTICK;
        }
    }
    // We enter into clock gating with sys tick disabled
    else
    {
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
