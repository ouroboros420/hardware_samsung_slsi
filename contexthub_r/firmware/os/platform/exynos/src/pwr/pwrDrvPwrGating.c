/*----------------------------------------------------------------------------
 *      Exynos SoC  -  PWR
 *----------------------------------------------------------------------------
 *      Name:    pwrDrvPwrGating.c
 *      Purpose: To implement save and restore SFRs & GPRs for power-gating
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
#include <cpu.h>
#include <pwrDrvPwrGating.h>
#include <cmuDrv.h>
#include <timer.h>
#include <rtc.h>
#include <wdt.h>
#include <plat/i2c/i2c.h>
#include <plat/spi/spi.h>
#include <usi.h>
#include <ipc_chub.h>

typedef struct _PwrDrv_SaveRestoreFunc
{
    void (*save_func)(void);
    void (*restore_func)(void);
} PwrDrv_SaveRestoreFunc;

static PwrDrv_SaveRestoreFunc pwrDrv_SaveRestoreFunc_Table[] =
{
    { cmuDrvSaveState, cmuDrvRestoreState },
    { usiSaveState, usiRestoreState },
    { sysregSaveState, sysregRestoreState },
    { i2cSaveState, i2cRestoreState },
    { spiSaveState, spiRestoreState },
};

static PwrDrv_SaveRestoreFunc pwrDrv_SaveRestoreCmgpFunc_Table[] =
{
    { cmuDrvCmgpSaveState, cmuDrvCmgpRestoreState },
    { sysregSaveState, sysregRestoreState },
    { usiCmgpSaveState, usiCmgpRestoreState },
    { i2cCmgpSaveState, i2cCmgpRestoreState },
    { spiSaveState, spiRestoreState },
};

static uint32_t table_cnt = sizeof(pwrDrv_SaveRestoreFunc_Table) / sizeof(PwrDrv_SaveRestoreFunc) ;

void pwrDrvSaveState(void)
{
    uint32_t i;

    for( i = 0; i < table_cnt; i++ )
    {
        pwrDrv_SaveRestoreFunc_Table[i].save_func();
    }
}

void pwrDrvRestoreState(void)
{
    uint32_t i;

    for( i = 0; i < table_cnt; i++ )
    {
        pwrDrv_SaveRestoreFunc_Table[i].restore_func();
    }
}

void pwrDrvCmgpSaveState(void)
{
    uint32_t i = sizeof(pwrDrv_SaveRestoreCmgpFunc_Table) / sizeof(PwrDrv_SaveRestoreFunc) ;

    for( i = 0; i < table_cnt; i++ )
    {
        pwrDrv_SaveRestoreCmgpFunc_Table[i].save_func();
    }
}

void pwrDrvCmgpRestoreState(void)
{
    uint32_t i = sizeof(pwrDrv_SaveRestoreCmgpFunc_Table) / sizeof(PwrDrv_SaveRestoreFunc) ;

    for( i = 0; i < table_cnt; i++ )
    {
        pwrDrv_SaveRestoreCmgpFunc_Table[i].restore_func();
    }
}

/* System Control Block */
uint32_t VTOR; // Vector Table Offset Register set by platInitialize
uint32_t AIRCR; // NVIC_SetPriorityGrouping
uint8_t  SHP[12]; // NVIC_SetPriority
uint32_t SHCSR;

/* Nested Vectored Interrupt Controller */
uint8_t  IP[240]; // NVIC_SetPriority
uint32_t ICER[8]; // NVIC_DisableIRQ
uint32_t ISER[8]; // NVIC_EnableIRQ

/* Memory Protection Unit */
uint32_t CTRL;
uint32_t RNR;
uint32_t RBAR;
uint32_t RASR;

void pwrDrvSaveCorePeri(void)
{
    u16 i, j;

    /* Save SCB */
    VTOR = SCB->VTOR;
    AIRCR = SCB->AIRCR;
    j = sizeof(SCB->SHP) / sizeof(SCB->SHP[0]);
    for(i = 0; i < j; i++)
        SHP[i] = SCB->SHP[i];
    SHCSR = SCB->SHCSR;

    /* Save NVIC */
    j = sizeof(NVIC->IP) / sizeof(NVIC->IP[0]);
    for(i = 0; i < j; i++)
        IP[i] = NVIC->IP[i];
    j = sizeof(NVIC->ICER) / sizeof(NVIC->ICER[0]);
    for(i = 0; i < j; i++)
        ICER[i] = NVIC->ICER[i];
    j = sizeof(NVIC->ISER) / sizeof(NVIC->ISER[0]);
    for(i = 0; i < j; i++)
        ISER[i] = NVIC->ISER[i];

    /* Save MPU */
    CTRL = MPU->CTRL;
    RNR = MPU->RNR;
    RBAR = MPU->RBAR;
    RASR = MPU->RASR;
}

void pwrDrvRestoreCorePeri(void)
{
    u16 i, j;

    /* Save SCB */
    SCB->VTOR = VTOR;
    SCB->AIRCR = AIRCR;
    j = sizeof(SCB->SHP) / sizeof(SCB->SHP[0]);
    for(i = 0; i < j; i++)
        SCB->SHP[i] = SHP[i];
    SCB->SHCSR = SHCSR;

    /* Save NVIC */
    j = sizeof(NVIC->IP) / sizeof(NVIC->IP[0]);
    for(i = 0; i < j; i++)
        NVIC->IP[i] = IP[i] ;
    j = sizeof(NVIC->ICER) / sizeof(NVIC->ICER[0]);
    for(i = 0; i < j; i++)
        NVIC->ICER[i] = ICER[i];
    j = sizeof(NVIC->ISER) / sizeof(NVIC->ISER[0]);
    for(i = 0; i < j; i++)
        NVIC->ISER[i] = ISER[i];

    /* Save MPU */
    MPU->CTRL = CTRL;
    MPU->RNR = RNR;
    MPU->RBAR = RBAR;
    MPU->RASR = RASR;

}

#if defined(LOCAL_POWERGATE)
void pwrDrvPwrGating(void)
{
    u32* msp;
    u32* psp;

    /* Save CPU Registers */
    msp = ipc_get_chub_msp();
    psp = ipc_get_chub_psp();
    pwrDrvPwrGatingSaveCPU(msp, psp);
}
#endif
