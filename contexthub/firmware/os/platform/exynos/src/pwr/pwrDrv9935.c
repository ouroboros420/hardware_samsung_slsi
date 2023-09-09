/*----------------------------------------------------------------------------
 *  Exynos SoC  -  PwrDRV
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

#include <cmuDrv.h>
#include <rtc.h>
#include <wdt.h>
#include <plat/i2c/i2c.h>
#include <plat/spi/spi.h>
#include <plat/i3c/i3c.h>
#include <usi.h>
#include <pwm.h>
#include <ipc_chub.h>
#include <mpu.h>

typedef struct _PwrDrv_SaveRestoreFunc
{
    void (*save_func)(void);
    void (*restore_func)(void);
} PwrDrv_SaveRestoreFunc;

static PwrDrv_SaveRestoreFunc pwrDrv_SaveRestoreFunc_Table[] =
{
    { cmuDrvSaveState,  cmuDrvRestoreState },
    { pwmSaveState,     pwmRestoreState },
    { sysregSaveState,  sysregRestoreState },
    { usiSaveState,     usiRestoreState },
    { i2cSaveState,     i2cRestoreState },
    { spiSaveState,     spiRestoreState },
    { i3cSaveState,     i3cRestoreState },
};

static uint32_t table_cnt = sizeof(pwrDrv_SaveRestoreFunc_Table) / sizeof(PwrDrv_SaveRestoreFunc) ;

static void pwrDrvSaveState(void)
{
    uint32_t i;

    for( i = 0; i < table_cnt; i++ )
    {
        if (pwrDrv_SaveRestoreFunc_Table[i].save_func != NULL)
            pwrDrv_SaveRestoreFunc_Table[i].save_func();
    }
}

static void pwrDrvRestoreState(void)
{
    uint32_t i;

    for( i = 0; i < table_cnt; i++ )
    {
        if (pwrDrv_SaveRestoreFunc_Table[i].restore_func != NULL)
            pwrDrv_SaveRestoreFunc_Table[i].restore_func();
    }
}

#if defined(LOCAL_POWERGATE)
void pwrDrvPrepareSleepPwrGating(uint32_t data)
{
    uint64_t intState;
    uint64_t intEn;
    uint32_t pend0, pend1;
    uint64_t intMask;

    intState = cpuIntsOff();
    //OS_INTERRUPT_MaskGlobal();
    intEn = NVIC->ISER[1];
    intEn = intEn << 32;
    intEn |= NVIC->ISER[0];

    /* all interrupt mask */
    intMask = sysregGetMaskIrq();
    // Mask IRQ excpet for RTC & MAILBOX
    sysregSetMaskIrq(0xFFFFFFFFFFFFFFFFULL);

    // Disable All Interrupts
    NVIC->ICER[0] = 0xFFFFFFFF;
    NVIC->ICER[1] = 0xFFFFFFFF;

    pend0 = NVIC->ISPR[0];
    pend1 = NVIC->ISPR[1];
    //if (intEn & NVIC->ISPR[0] || (intEn >> 32) & NVIC->ISPR[1]) {
    if (((uint32_t)intEn & pend0) || ((uint32_t)(intEn >> 32) & pend1)) {
	    sysregClearMaskIrq(~intMask);
	    NVIC->ISER[0] = (uint32_t)intEn;
	    NVIC->ISER[1] = (uint32_t)(intEn >> 32);
	    return;
    }

    rtcSetTickTime(0, 0, data, NULL, 0);

    // Clear Pending Bit
    NVIC->ICPR[0] = 0xFFFFFFFF;
    NVIC->ICPR[1] = 0xFFFFFFFF;

    // Disable SysTick
#if defined(SEOS)
    platStopSysTick();
#else
    SysTick->CTRL = 0;
#endif

    if (SysTick->CTRL & 0x10000) {
        SCB->ICSR = (0x1 <<25);
    }

    // Allow processor to enter deep sleep
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

    sysregSetEarlyWakeupWindowReq(true);

    // Save SFRs before they are lost
    pwrDrvSaveState();
    // Save SCB, NVIC, MPU
    pwrDrvSaveCorePeri();


#if defined(EMBOS)
    // Disable MPU
#endif
    /* Set boot mode power gating */
#if defined (DEF_MPU_SUPPORT)
    mpuAllowRomWrite(true);
#endif
    ipc_set_chub_bootmode(BOOTMODE_PWRGATING, ipc_get_chub_rtlogmode());

    PWR_DEBUG_PRINT("Power OFF\n");
    sysregSetWakeupInterruptAll();
    sysregPDRequest();
    // Save CPU GPRs and call wfi
    pwrDrvPwrGating();
    //asm volatile ("wfi\n" "nop":::"memory");
    sysregClrWakeupInterruptAll();

    rtcStopTick();

    // Restore SCB, NVIC, MPU
    pwrDrvRestoreCorePeri();

#if defined(EMBOS)
    __iar_init_vfp();
#endif
    PWR_DEBUG_PRINT("Power ON\n");

    // Restore SFRs
    pwrDrvRestoreState();
#if defined (DEF_MPU_SUPPORT)
    mpuStart();
#endif


    sysregClearMaskIrq(~intMask);

#if defined(SEOS)
    // Update OS Tick variable
    //platSetOSTickVariableAfterPwrGating();
#endif

    NVIC->ISER[0] = (uint32_t)intEn;
    NVIC->ISER[1] = (uint32_t)(intEn >> 32);

    //platStartSysTick();
    cpuIntsRestore(intState);
}
#endif

