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
#if defined(SEOS)
  #include <timer.h>
#elif defined(EMBOS)
  #include <core_cm4.h>
#endif
#include <cpu.h>
#include <pwrDrvPwrGating.h>
#include <ipc_chub.h>


static MPU_Type mMPU[8];

void pwrDrvCmgpSaveState(void)
{
    // Currently there is no SFRs to be saved & restored in CMGP
}

void pwrDrvCmgpRestoreState(void)
{
    // Currently there is no SFRs to be saved & restored in CMGP
}

/* System Control Block */
static uint32_t VTOR; // Vector Table Offset Register set by platInitialize
static uint32_t AIRCR; // NVIC_SetPriorityGrouping
static uint8_t  SHP[12]; // NVIC_SetPriority
static uint32_t SHCSR;

/* Nested Vectored Interrupt Controller */
static uint8_t  IP[240]; // NVIC_SetPriority
//static uint32_t ICER[8]; // NVIC_DisableIRQ
static uint32_t ISER[8]; // NVIC_EnableIRQ

/* VFP Unit */
static uint32_t FPCCR;
static uint32_t CPACR;

#define CPACRd         (*(volatile uint32_t*) (0xE000ED88uL))
#define FPCCRd         (*(volatile uint32_t*) (0xE000EF34uL))

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
    /* No Need to Save ICER */
    /*
    j = sizeof(NVIC->ICER) / sizeof(NVIC->ICER[0]);
    for(i = 0; i < j; i++)
        ICER[i] = NVIC->ICER[i];
    */
    j = sizeof(NVIC->ISER) / sizeof(NVIC->ISER[0]);
    for(i = 0; i < j; i++)
        ISER[i] = NVIC->ISER[i];

    /* Save MPU */
    mMPU[0].CTRL = MPU->CTRL;
    for (i = 0 ; i < 8 ; i++) {
        MPU->RNR = i;
        mMPU[i].RBAR = MPU->RBAR;
        mMPU[i].RASR = MPU->RASR;
        mMPU[i].RBAR_A1 = MPU->RBAR_A1;
        mMPU[i].RASR_A1 = MPU->RASR_A1;
        mMPU[i].RBAR_A2 = MPU->RBAR_A2;
        mMPU[i].RASR_A2 = MPU->RASR_A2;
        mMPU[i].RBAR_A3 = MPU->RBAR_A3;
        mMPU[i].RASR_A3 = MPU->RASR_A3;
    }

     /* Save VFP */
    CPACR = CPACRd;
    FPCCR = FPCCRd;
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
    /* No need to restore ICER */
    /*
    j = sizeof(NVIC->ICER) / sizeof(NVIC->ICER[0]);
    for(i = 0; i < j; i++)
        NVIC->ICER[i] = ICER[i];
    */
    j = sizeof(NVIC->ISER) / sizeof(NVIC->ISER[0]);
    for(i = 0; i < j; i++)
        NVIC->ISER[i] = ISER[i];

    /* Restore MPU */
    for (i = 0 ; i < 8 ; i++) {
        MPU->RNR = i;
        MPU->RBAR = mMPU[i].RBAR;
        MPU->RASR = mMPU[i].RASR = MPU->RASR;
        MPU->RBAR_A1 = mMPU[i].RBAR_A1;
        MPU->RASR_A1 = mMPU[i].RASR_A1;
        MPU->RBAR_A2 = mMPU[i].RBAR_A2;
        MPU->RASR_A2 = mMPU[i].RASR_A2;
        MPU->RBAR_A3 = mMPU[i].RBAR_A3;
        MPU->RASR_A3 = mMPU[i].RASR_A3;
    }
    MPU->RNR = 0;
    MPU->CTRL = mMPU[0].CTRL;

     /* Restore VFP */
    CPACRd = CPACR;
    FPCCRd = FPCCR;
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
