/*----------------------------------------------------------------------------
 *      Exynos SoC  -  CPU STIR
 *----------------------------------------------------------------------------
 *      Name:    swint.c
 *      Purpose: To implement APIs for Software Tirgger Interrupt Register to generate interrrupt by SW
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

#if defined(SWINT_REQUIRED)
#include <swint.h>
#include <cmsis.h>

void swintTrigger(IN IRQn_Type irq)
{
    volatile uint32_t reg;

    // Enable unprivilieged software access to the STIR
    reg = SCB->CCR;
    SCB->CCR |= SCB_CCR_USERSETMPEND_Msk;

    // Generate interrupt you want
    if(irq >= 0)
        NVIC->STIR = NVIC_STIR_INTID_Msk & irq;

    // Restore STIR
    SCB->CCR = reg;
}
#endif

