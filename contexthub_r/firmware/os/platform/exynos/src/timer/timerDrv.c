/*----------------------------------------------------------------------------
 *      Exynos SoC  -  TIMER
 *----------------------------------------------------------------------------
 *      Name:    timerDrv.c
 *      Purpose: To implement TIMER driver functions
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
#include <cmsis.h>
#endif
#include <plat/timer/timer.h>
#include <timerDrv.h>

#define TIMER_TCSR (TIMER_BASE_ADDRESS + 0x0)
#define TIMER_TRVR (TIMER_BASE_ADDRESS + 0x4)
#define TIMER_TCVR (TIMER_BASE_ADDRESS + 0x8)

#define TMEN_BIT (0)
#define INTEN_BIT (1)
#define PSFSEL_BIT (4)
#define CNTFLAG_BIT (16)

#define PERI_TIMER_1US (3)

void timerDrvInit(void)
{
    /* 1 = Divides the SCLK(24.576Mhz, CLK_CHUB_OSCCLK_RCO) frequency by 8 */
    __raw_writel((7 << PSFSEL_BIT) | (0 << INTEN_BIT) | (0 << TMEN_BIT),
            TIMER_TCSR);

    /* peri timer current value register clear */
    __raw_writel(PERI_TIMER_1US, TIMER_TCVR);

    /* 3.072 count: 1 us */
    __raw_writel(PERI_TIMER_1US, TIMER_TRVR);

    NVIC_EnableIRQ(TIMER_CHUB_IRQn);
}

void timerDrvSet(uint32_t usec)
{
    /* peri timer current value register clear */
    __raw_writel(PERI_TIMER_1US, TIMER_TCVR);

    /* 1 = Divides the SCLK(24.576Mhz, CLK_CHUB_OSCCLK_RCO) frequency by 8 */
    __raw_writel(PERI_TIMER_1US * usec, TIMER_TRVR);

    /* peri timer enable */
    __raw_writel((1 << PSFSEL_BIT) | (1 << INTEN_BIT) | (1 << TMEN_BIT),
            TIMER_TCSR);
}

void timerDrvUnset(void)
{
    /* peri timer disable */
    __raw_writel((1 << PSFSEL_BIT) | (0 << INTEN_BIT) | (0 << TMEN_BIT),
            TIMER_TCSR);

    /* peri timer current value register clear */
    __raw_writel(PERI_TIMER_1US, TIMER_TCVR);
    __raw_writel(PERI_TIMER_1US, TIMER_TRVR);
}
