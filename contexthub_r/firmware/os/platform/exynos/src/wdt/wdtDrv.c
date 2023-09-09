/*----------------------------------------------------------------------------
 *      Exynos SoC  -  WDT
 *----------------------------------------------------------------------------
 *      Name:    wdtDrv.c
 *      Purpose: To implement WDT driver functions
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
#include <wdt.h>
#include <wdtDrv.h>

#define REG_WDT_WTCON               (WDT_BASE_ADDRESS + 0x0)
#define REG_WDT_WTDAT               (WDT_BASE_ADDRESS + 0x4)
#define REG_WDT_WTCNT               (WDT_BASE_ADDRESS + 0x8)
#define REG_WDT_WTCLRINT            (WDT_BASE_ADDRESS + 0xC)

#define WDT_PRESCALER_BIT           (8)
#define WDT_EN_BIT                  (5)
#define WDT_CLK_DIV_BIT             (3)
#define WDT_INT_ENABLE_BIT          (2)
#define WDT_RESET_ENABLE_BIT        (0)

#define WDT_OSC_CLOCK               OSC_CLOCK
#define WDT_PRESCALER               (0x5F) // OSC_CLOCK / 96(0x5F+1)
#define WDT_DIVIDER                 (2) // 0=1/16, 1=1/32, 2=1/64, 3=1/128

#if (WDT_DIVIDER==0)
    #define WDT_DIV_VALUE           16
#elif (WDT_DIVIDER==1)
    #define WDT_DIV_VALUE           32
#elif (WDT_DIVIDER==2)
    #define WDT_DIV_VALUE           64
#elif (WDT_DIVIDER==3)
    #define WDT_DIV_VALUE           128
#endif

#define WDT_TIMER_CLOCK             (WDT_OSC_CLOCK / WDT_PRESCALER / WDT_DIV_VALUE)
#define WDT_COUNT_MS                (WDT_TIMER_CLOCK / 1000)

#define WDT_START                   1
#define WDT_STOP                    0

uint32_t mWdtState;

void wdtDrvInit(uint32_t mSec, uint32_t intEn, uint32_t resetEn)
{
    uint32_t regValue = 0;

    wdtDrvDisable();

    regValue |= ( WDT_PRESCALER << WDT_PRESCALER_BIT );
    regValue |= ( WDT_DIVIDER << WDT_CLK_DIV_BIT );

    if( intEn )
        regValue |= ( 0x1 << WDT_INT_ENABLE_BIT );

    if( resetEn )
        regValue |= ( 0x1 << WDT_RESET_ENABLE_BIT );

    __raw_writel( regValue, REG_WDT_WTCON );

    regValue = mSec * WDT_COUNT_MS;

    __raw_writel( regValue, REG_WDT_WTDAT );
    __raw_writel( regValue, REG_WDT_WTCNT );

    wdtDrvEnable();
}

void wdtDrvEnable(void)
{
    uint32_t regValue;

    regValue = __raw_readl( REG_WDT_WTCON );
    regValue |= ( 0x1 << WDT_EN_BIT );

    __raw_writel( regValue, REG_WDT_WTCON );

    mWdtState = WDT_START;
}

void wdtDrvDisable(void)
{
    uint32_t regValue;

    regValue = __raw_readl( REG_WDT_WTCON );
    regValue &= (uint32_t)(~( 0x1 << WDT_EN_BIT ));

    __raw_writel( regValue, REG_WDT_WTCON );

    mWdtState = WDT_STOP;
}

void wdtDrvSetTime(uint32_t mSec)
{
    uint32_t regValue;
    uint32_t state = mWdtState;


    if( state == WDT_START)
        wdtDrvDisable();

    regValue = mSec * WDT_COUNT_MS;

    __raw_writel( regValue, REG_WDT_WTDAT );
    __raw_writel( regValue, REG_WDT_WTCNT );

    if( state == WDT_START)
        wdtDrvEnable();
}

void wdtDrvEnableInterrupt(void)
{
    uint32_t regValue;

    regValue = __raw_readl( REG_WDT_WTCON );
    regValue |= ( 0x1 << WDT_INT_ENABLE_BIT );

    __raw_writel( regValue, REG_WDT_WTCON );
}

void wdtDrvDisableInterrupt(void)
{
    uint32_t regValue;

    regValue = __raw_readl( REG_WDT_WTCON );
    regValue &= (uint32_t)(~( 0x1 << WDT_INT_ENABLE_BIT ));

    __raw_writel( regValue, REG_WDT_WTCON );
}

void wdtDrvEnableReset(void)
{
    uint32_t regValue;

    regValue = __raw_readl(REG_WDT_WTCON);
    regValue |= (0x1 << WDT_RESET_ENABLE_BIT);

    __raw_writel( regValue, REG_WDT_WTCON );
}

void wdtDrvDisableReset(void)
{
    uint32_t regValue;

    regValue = __raw_readl(REG_WDT_WTCON);
    regValue &= (uint32_t)(~(0x1 << WDT_RESET_ENABLE_BIT));

    __raw_writel( regValue, REG_WDT_WTCON );
}

void wdtDrvClose(void)
{
    wdtDrvDisable();
    wdtDisableInterrupt();
    wdtDisableReset();
}

void wdtDrvPendClear(void)
{
    __raw_writel( 0x1, REG_WDT_WTCLRINT );
}
