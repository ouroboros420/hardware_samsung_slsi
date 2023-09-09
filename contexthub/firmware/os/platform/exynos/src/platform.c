/*----------------------------------------------------------------------------
 *      Exynos SoC  -  Platfor
 *----------------------------------------------------------------------------
 *      Name:    platform.c
 *      Purpose: To implement platform-specific
 *      Rev.:    V1.00
 *----------------------------------------------------------------------------
 *
 * Copyright (C) 2017 Samsung Electronics Co., Ltd.
 *
 * Contact: Boojin Kim <boojin.kim@samsung.com>
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

#include <variant.h>
#include <cpu/cpuMath.h>
#include <csp_common.h>
#include <cmu.h>
#if defined(UART_REQUIRED)
#include <uart.h>
#endif
#if defined(DMAILBOX_REQUIRED)
#include <dmailbox.h>
#endif
#include <usi.h>
#include <wdt.h>
#include <cmsis.h>
#include <plat/i2c/i2c.h>
#include <plat/i3c/i3c.h>
#include <rtc.h>
#include <plat.h>
#include <mailbox.h>
#include <typedef.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <printf.h>
#include <platform.h>
#include <seos.h>
#include <heap.h>
#include <timer.h>
#include <plat/gpio/gpio.h>
#include <mpu.h>
#include <cpu.h>
#include <pwm.h>
#include <hostIntf.h>
#include <atomic.h>
#include <hostIntf.h>
#include <nanohubPacket.h>
#include <sensType.h>
#include <pwrDrv.h>
#include <boardInit.h>
#include <plat/mailbox/ipc_chub.h>
#include <sysreg.h>
#if defined(EXYNOS_VSYNC)
#include <csisOS.h>
#endif
#include <cmgp.h>
#include <mct.h>
#include <dvfs/dvfs.h>

#ifdef DEBUG_LOG_EVT
#define EARLY_LOG_BUF_SIZE      1024
#define HOSTINTF_HEADER_SIZE    4
uint8_t *mEarlyLogBuffer;
uint16_t mEarlyLogBufferCnt;
uint16_t mEarlyLogBufferOffset;
bool mLateBoot;
#endif

// The elapsed time in nano-seconds since system powered up
static uint64_t mTimeAccumulated = 0;
static uint32_t mReloadVal = 0;

// Expiration time of the very next software timer
static uint64_t mWakeupTime = 0;

// Calculate the nano-second time of SysTick current value register
uint64_t platSystickTicksToNs(uint32_t systickTicks)
{
    return (((uint64_t) mReloadVal -
             systickTicks) * TIME_OF_SYSTICK_IN_MS / mReloadVal);
}

// Return the accumulative elapsed time in nano-second since system starts
uint64_t platGetTicks(void)
{
#if defined (MCT_SUPPORT)
    return mctGetTimeStampNS();
#else
    return rtcGetTimeStampNS();
#endif
}

uint64_t getTimeStampUS(void)
{
#if defined (MCT_SUPPORT)
    return mctGetTimeStampUS();
#elif (USE_PWM_AS_TIMESTAMP)
    return pwmGetTimeStampUS();
#else
    return rtcGetTimeStampUS();
#endif
}

// Return the elapsed time in nano-second since the last systick
uint64_t platGetTicksSinceTheLastTick(void)
{
    return (uint64_t) (((uint64_t) mReloadVal -
                        SysTick->VAL) * TIME_OF_SYSTICK_IN_MS / mReloadVal);
}

void *platLogAllocUserData(bool force)
{
#if defined(DEBUG_LOG_EVT)
    struct HostIntfDataBuffer *userData = NULL;

    if (mLateBoot) {
        userData = heapAlloc(sizeof(struct HostIntfDataBuffer));
    } else if (mEarlyLogBufferOffset <
               EARLY_LOG_BUF_SIZE - HOSTINTF_HEADER_SIZE) {
        userData =
            (struct HostIntfDataBuffer *)(mEarlyLogBuffer +
                                          mEarlyLogBufferOffset);
        mEarlyLogBufferOffset += HOSTINTF_HEADER_SIZE;
    }
    if (userData) {
        userData->sensType = SENS_TYPE_INVALID;
        userData->length = 0;
        userData->dataType = HOSTINTF_DATA_TYPE_LOG;
        userData->interrupt = NANOHUB_INT_NONWAKEUP;
    }
    return userData;
#endif
#ifdef IPC_PRINT
    return ipc_logbuf_inbase(force);
#else
    return NULL;
#endif
}

#if defined(DEBUG_LOG_EVT)
static void platEarlyLogFree(void *buf)
{
    struct HostIntfDataBuffer *userData = (struct HostIntfDataBuffer *)buf;
    mEarlyLogBufferCnt += userData->length + HOSTINTF_HEADER_SIZE;
    if (mEarlyLogBufferCnt >= mEarlyLogBufferOffset) {
        heapFree(mEarlyLogBuffer);
    }
}
#endif

void platEarlyLogFlush(void)
{
#if defined(DEBUG_LOG_EVT)
    uint16_t i = 0;
    struct HostIntfDataBuffer *userData;

    mLateBoot = true;

    while (i < mEarlyLogBufferOffset) {
        userData = (struct HostIntfDataBuffer *)(mEarlyLogBuffer + i);
#ifdef IPC_PRINT
        platEarlyLogFree(userData);
#else
        osEnqueueEvt(EVENT_TYPE_BIT_DISCARDABLE | EVT_DEBUG_LOG, userData,
                     platEarlyLogFree);
#endif
        i += HOSTINTF_HEADER_SIZE + userData->length;
    }
#endif
}

void platLogFlush(void *userData)
{
#if defined(DEBUG_LOG_EVT)
    if (userData && mLateBoot) {
        if (ipc_get_ap_wake() == AP_SLEEP)
            heapFree(userData);
        else if (!osEnqueueEvt
            (EVENT_TYPE_BIT_DISCARDABLE | EVT_DEBUG_LOG, userData, heapFree))
            heapFree(userData);
    }
#endif
#if defined IPC_PRINT
    if (userData)
        ipc_logbuf_req_flush((struct logbuf_content *)userData);
#endif
}

bool platLogPutcharF(void *userData, char ch)
{
#if defined(DEBUG_LOG_EVT)
    struct HostIntfDataBuffer *buffer;

    if (userData) {
        buffer = userData;
        if (buffer->length == sizeof(uint64_t) + HOSTINTF_SENSOR_DATA_MAX) {
            buffer->buffer[buffer->length - 1] = '\n';
        } else if (!mLateBoot) {
#ifndef IPC_PRINT
            if (mEarlyLogBufferOffset == EARLY_LOG_BUF_SIZE) {
                buffer->buffer[buffer->length - 1] = '\n';
            } else {
                buffer->buffer[buffer->length++] = ch;
                mEarlyLogBufferOffset++;
            }
#endif
        } else {
            buffer->buffer[buffer->length++] = ch;
        }
    }
#endif
#ifdef IPC_PRINT
    /* runtime print */
    if (userData) {
        struct logbuf_content *log = userData;
        char *buf;

        if (log->newline && log->nextaddr)
            log = ipc_logbuf_get_curlogbuf(log);

        buf = &log->buf[0];
        if (log->size < LOGBUF_DATA_SIZE - 2) {
            buf[log->size] = ch;
            log->size++;
            if (log->size == (LOGBUF_DATA_SIZE - 2)) {
                buf[log->size] = '\n';
                buf[log->size + 1] = '\0';
                ipc_logbuf_set_req_num(log);
                if (log->newline) {
                    struct logbuf_content *newline = platLogAllocUserData((bool)log->error);

                    if (newline) {
                        newline->newline = log->newline;
                        newline->newline++;
                        newline->error = log->error;
                        log->nextaddr = (u32)newline;
                    } else {
                        log->newline = 0;
                        log->nextaddr = 0;
                    }
                }
            }
        }
    }
#else
    uartPutCharDbg(ch);
#endif

    return true;
}

void platLogv(char clevel, uint32_t flags, const char *str, va_list vl)
{
#ifdef EXYNOS_CONTEXTHUB
    u8 force = (clevel == 'E') ? 1 : 0;
    u8 newline = ((clevel == 'E') || (clevel == 'W') || (clevel == 'C') || (clevel == 'I') || (clevel == 'A')) ? 1 : 0;
    struct logbuf_content *userData = platLogAllocUserData(force);

    if (userData == NULL)
        return;

    userData->timestamp = getTimeStampUS();
    userData->level = clevel;
    userData->error = force;
    userData->newline = newline;
#else
    void *userData = platLogAllocUserData();
#endif

    cvprintf(platLogPutcharF, flags, userData, str, vl);

    platLogFlush(userData);
}

void platSleep(void)
{
    pwrDrvSleep();
}

bool platSleepClockRequest(uint64_t wakeupTime, uint32_t maxJitterPpm,
                           uint32_t maxDriftPpm, uint32_t maxErrTotalPpm)
{
    uint64_t intState, curTime = timGetTime();

    // Software timer exists and current time passed the expiration time of it
    if (wakeupTime && curTime >= wakeupTime)
        return false;

    intState = cpuIntsOff();

    mWakeupTime = wakeupTime;

    cpuIntsRestore(intState);

    return true;
}

uint64_t platGetNextWakeupTime(void)
{
    uint64_t t;

    t = platGetTicks();

    if (mWakeupTime > t)
        return mWakeupTime - t;
    else if (mWakeupTime == 0)
        return 0;
    else // if mWakeupTime =< t
        return 1;
}

uint64_t platGetOSTickVariable(void)
{
    return mTimeAccumulated;
}

void platSetOSTickVariable(uint64_t elapsedTime)
{
    mTimeAccumulated += elapsedTime;
}

void platSetOSTickVariableAfterSleep(void)
{
    uint64_t t = (uint64_t) (pwmGetCurrentTime(PWM_TIMER0)) * 1000;

    //CSP_PRINTF_INFO("platSetOSTickVariableAfterSleep (%lld)\n", t);
    platSetOSTickVariable(t);
}

void platSetOSTickVariableAfterPwrGating(void)
{
    platSetOSTickVariable(rtcGetCurrentTickTime() * 1000);
}

bool platRequestDevInSleepMode(uint32_t sleepDevID, uint32_t maxWakeupTime)
{
    return true;
}

bool platReleaseDevInSleepMode(uint32_t sleepDevID)
{
    return true;
}

// In nanhub kernel, IRQ handler is called after exiting from platSleep in evtQueueDequeue since C-M4 PRIMASK is off when entering wfi.
// timIntHandler calls platGetTicks() to check the elapsed time since wfi, hence later updating systick(mTimeAccumulated) by SysTick_Handler is no problem.
// updating systick(mTimeAccumulated) in pwrDrv right after exiting from wfi may break timestamp.
void SysTick_Handler(void)
{
    if(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) {
        mTimeAccumulated += (uint64_t) TIME_OF_SYSTICK_IN_MS;
    }
}

#if defined(UART_REQUIRED)
static void serialInit(void)
{
    usiSetUartDbg();
#ifndef IPC_PRINT
    uartOpenDbg(UART_BAUDRATE_115200, UART_DATA_BITS_8, UART_STOP_BITS_1,
                UART_PARITY_NONE);

    uartPutCharDbg('\n');
#endif
}
#endif

void platStartSysTick(void)
{
    /* set reload register */
    SysTick->LOAD = mReloadVal - 1;

    /* set Priority for Systick Interrupt */
    NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1);

    /* Load the SysTick Counter Value */
    SysTick->VAL = 0;

    /* Set control */
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
        SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
}

void platStopSysTick(void)
{
    // Disable SysTick
    SysTick->CTRL = 0;
}

extern uint32_t __code_start[];

void platInitialize(void)
{
    int i;
    u32 bootmode;
    uint32_t appBase = ((uint32_t) & __code_start) & ~1;

    SCB->VTOR = appBase;

    // No group priority is used. All IRQs are just simply prioritized by Interrupt Priority Registers
    // 0 is the highest priority and 0xff is the lowest one
    NVIC_SetPriorityGrouping(7);
    for (i = 0; i < NUM_INTERRUPTS; i++) {
        NVIC_SetPriority(i, i);
        NVIC_DisableIRQ(i);

        /* Wakeup Interrupt shall be kept the status */
#if  defined(s5e9610)
        if ((i != MB_AP_IRQn) && (i != MB_CP_IRQn) && (i != MB_APM_IRQn)
        	&& (i != MB_GNSS_IRQn) && (i != RTC_IRQn) && (i != MB_WLBT_IRQn))
            NVIC_ClearPendingIRQ(i);
#endif
    }

    // PWM0 Timer is used as a wakeup source during SysTick disabled sleep mode.
    // OS Tick variable needs be updated right away after wakeup from SysTick disabled sleep.
    NVIC_SetPriority(PWM0_CHUB_IRQn, 0);

#ifndef EXYNOS_CONTEXTHUB_EXT
    mailboxEnable();
#endif

#if defined (DEF_MPU_SUPPORT)
    mpuStart();
#endif


#if defined (ALIVE_DVS_SUPPORT)
    dvfsInit();
#endif

    cmuInit(ipc_get_chub_clk());

    /* Initialize GPIO */
    gpioInit();

    /* Initialize CMGP */
    cmgpInit();

    /* Initialize Board */
    boardInit();

    /* Initialize RTC */
    rtcInit();

#if defined(EXYNOS_VSYNC)
    /* Enable CSIS interrupt */
    csisOsInit();
#endif

#if defined(DMAILBOX_REQUIRED)
    dmailboxInit();
#endif

    i2cInit();
#if defined(I3C_REQUIRED)
    i3cInit();
#endif

    /* Initialize PWM Timer for wakeup source from core wfi */
    pwmInit(PWM_TIMER0, 1000, PWM_TIMER_MODE_ONESHOT, 1);
#if (USE_PWM_AS_TIMESTAMP)
    pwmInit(TS_PWM_ID, 100000000, PWM_TIMER_MODE_INTERVAL, 0);
    pwmStartTimer(TS_PWM_ID);
#endif

    /* bring up SysTick */
    mReloadVal = cmuGetReloadValForOSTick();
    //platStartSysTick();
#ifdef DEBUG_LOG_EVT
    /* allocate buffer for early boot log message */
    mEarlyLogBuffer = heapAlloc(EARLY_LOG_BUF_SIZE);
#endif

    bootmode = ipc_get_chub_bootmode();
    sysregInit();
#ifdef LOCAL_POWERGATE
#if defined(s5e9110)
    sysregSetAPMReq(TRUE);
#endif
#endif

#if !defined(s5e9110)
    sysregSetOscEn();
#endif

#ifdef HEAP_DEBUG
    heapShowStatus();
#endif

#if defined (MCT_SUPPORT)
    mctInit();
#endif

    if(bootmode == BOOTMODE_COLD)
        CSP_PRINTF_INFO("System boots up by cold-boot!\n");
    else
        CSP_PRINTF_ERROR("System boots up by unknown boot mode(%d)!\n", bootmode);
}

extern uint8_t __ipc_start[];
void *platGetPersistentRamStore(uint32_t * bytes)
{
    unsigned int size = getPersistedDatasize();
    void *buffer = (void *)__ipc_start;

    if (size > CHUB_PERSISTBUF_SIZE)
        CSP_PRINTF_ERROR("%s: fails to invalid persist size:%d\n", __func__, size);

    if (buffer)
        *bytes = size;

    return buffer;
}

uint32_t platFreeResources(uint32_t tid)
{
    osLog(LOG_DEBUG, "%s needs more\n", __func__);
    return true;
}

void platReset(int systemReset)
{
    int i;

    osLog(LOG_DEBUG, "%s(%d)\n", __func__, systemReset);

    wdtDisable();
    ipc_write_hw_value(IPC_VAL_HW_BOOTMODE, 1);

    if (systemReset) {
        NVIC_SystemReset();
    } else {
        gpioDeinit();
        mailboxDrvDisable();

        for (i = 0; i < NUM_INTERRUPTS; i++) {
            NVIC_DisableIRQ(i);
            NVIC_ClearPendingIRQ(i);
        }
	asm volatile ("wfi\n" "nop":::"memory");
    }
}

void platPeriodic()
{
    wdtPing();
}

void logbuf_callback(void)
{
    CSP_PRINTF_INFO("%s logbuf set level 1\n", __func__);
    ipc_logbuf_loglevel(CHUB_RT_LOG_DUMP, 1);
}
