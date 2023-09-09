/*----------------------------------------------------------------------------
 *      Exynos SoC  -  CSIS
 *----------------------------------------------------------------------------
 *      Name:    csisOS.c
 *      Purpose: OS-dependent part for CSIS_VVALID interrupts
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

#if defined(EXYNOS_VSYNC)

#include <csp_common.h>
#include <csisOS.h>
#if defined(SEOS)
#include <cmsis.h>
#endif

#define CSIS0 0
#define CSIS1 1
#define CSIS2 2
#define CSIS3 3

#if !defined(WORKAROUND_FOR_EXYNOS_VSYNC)

// from vsync.c
extern void vsyncIsr(uint32_t csis_no);

void csisOsInterruptControl(bool on)
{
    if(on) {
#if defined(s5e9610)
        NVIC_ClearPendingIRQ(CSIS0_VVALID_IRQn);
        NVIC_ClearPendingIRQ(CSIS1_VVALID_IRQn);
        NVIC_ClearPendingIRQ(CSIS2_VVALID_IRQn);
        NVIC_ClearPendingIRQ(CSIS3_VVALID_IRQn);

        NVIC_EnableIRQ(CSIS0_VVALID_IRQn);
        NVIC_EnableIRQ(CSIS1_VVALID_IRQn);
        NVIC_EnableIRQ(CSIS2_VVALID_IRQn);
        NVIC_EnableIRQ(CSIS3_VVALID_IRQn);
#endif
    }
    else {
#if defined(s5e9610)
        NVIC_DisableIRQ(CSIS0_VVALID_IRQn);
        NVIC_DisableIRQ(CSIS1_VVALID_IRQn);
        NVIC_DisableIRQ(CSIS2_VVALID_IRQn);
        NVIC_DisableIRQ(CSIS3_VVALID_IRQn);
#endif
    }
}

void csis0_IRQHandler(void)
{
    vsyncIsr(CSIS0);
}

void csis1_IRQHandler(void)
{
    vsyncIsr(CSIS1);
}

void csis2_IRQHandler(void)
{
    vsyncIsr(CSIS2);
}

void csis3_IRQHandler(void)
{
    vsyncIsr(CSIS3);
}
#endif

void csisOsInit(void)
{
#if !defined(WORKAROUND_FOR_EXYNOS_VSYNC)
    csisOsInterruptControl(true);
#endif
}
#endif
