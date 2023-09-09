/*----------------------------------------------------------------------------
 *      Exynos SoC  -  PWM
 *----------------------------------------------------------------------------
 *      Name:    pwmOS.c
 *      Purpose: OS-dependent part for PWM driver
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
#include <csp_assert.h>
#include <csp_printf.h>
#include <pwm.h>
#include <pwmOS.h>
#include <pwmDrv.h>

#if defined(SEOS)
#include <platform.h>
#endif

#if defined(PWM_REQUIRED)
static struct PwmIsr PwmIsrList[PwmIdMax];
#endif

// In nanhub kerne, IRQ handler is called after exiting from platSleep in evtQueueDequeue.
// Therefore, code to update os time is moved to pwrDrv
void pwm0_IRQHandler_Process(void)
{
    //pwmDrvClearPend(PWM_TIMER0);

#if defined(SEOS)
#if defined(SYSTICK_DLL)
    // Update OS Tick variable
    platSetOSTickVariableAfterSleep();
#endif

    // Re-start SysTick
    platStartSysTick();
#endif
}

// In nanhub kernel, IRQ handler is called after exiting from platSleep in evtQueueDequeue since C-M4 PRIMASK is off when entering wfi.
// timIntHandler calls platGetTicks() to check the elapsed time since wfi, hence later updating systick(mTimeAccumulated) by SysTick_Handler is no problem.
// platGetTicks() does not check pwm0, therefore updating systick(mTimeAccumulated) should be done right after exiting from wfi in pwrDrv.
void pwm0_IRQHandler(void)
{
    pwmDrvClearPend(PWM_TIMER0);
}

#if defined(PWM_REQUIRED)
void pwm_SetHandler(uint8_t id, void *isr)
{
    if((id <PwmId1) || (id>PwmId3) || (isr == NULL)){
        CSP_PRINTF_ERROR("%s: bad parameter",__FUNCTION__);
        return;
     }

    PwmIsrList[id].func = (void(*)(void))isr;
}
void pwm_UnsetHandler(uint8_t id)
{
    if((id <PwmId1) || (id>PwmId3)){
        CSP_PRINTF_ERROR("%s: bad parameter",__FUNCTION__);
        return;
     }

    PwmIsrList[id].func = NULL;
}

void pwm1_IRQHandler(void)
{
    pwmDrvClearPend(PWM_TIMER1);

    // Do Something
    if(PwmIsrList[PwmId1].func) (*PwmIsrList[PwmId1].func)();
}

void pwm2_IRQHandler(void)
{
    pwmDrvClearPend(PWM_TIMER2);

    // Do Something
    if(PwmIsrList[PwmId2].func) (*PwmIsrList[PwmId2].func)();
}

void pwm3_IRQHandler(void)
{
    pwmDrvClearPend(PWM_TIMER3);

    // Do Something
    if(PwmIsrList[PwmId3].func) (*PwmIsrList[PwmId3].func)();
}
#endif

