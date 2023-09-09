/*----------------------------------------------------------------------------
 *      Exynos SoC  -  ADC Driver
 *----------------------------------------------------------------------------
 *      Name:    adcDrv.c
 *      Purpose: To implement ADC APIs
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
#include <csp_printf.h>

#if defined(SEOS)
    #include <cmsis.h>
    #include <atomic.h>
    #include <platform.h>
#endif

#include <adc.h>
#include <adcDrv.h>
#include <sysreg.h>
#include <sysregDrv.h>

#define REG_ADC_CON1            (ADC_CMGP_BASE_ADDRESS + 0x00)
#define REG_ADC_CON2            (ADC_CMGP_BASE_ADDRESS + 0x04)
#define REG_ADC_DAT             (ADC_CMGP_BASE_ADDRESS + 0x08)
#define REG_ADC_SUM_DAT         (ADC_CMGP_BASE_ADDRESS + 0x0C)
#define REG_ADC_INT_EN          (ADC_CMGP_BASE_ADDRESS + 0x10)
#define REG_ADC_INT_STATUS      (ADC_CMGP_BASE_ADDRESS + 0x14)
#define REG_ADC_DEBUG_DATA      (ADC_CMGP_BASE_ADDRESS + 0x18)

#if defined(ADC_REQUIRED)

#define SOFT_RESET_BIT          (1)
#define SOFT_RESET_MASK         (0x3<<SOFT_RESET_BIT)
#define ADC_RESET_ASSERT        (2)
#define ADC_RESET_RELEASE       (1)

#define STC_EN_BIT              (0)
#define STC_EN                  (1)

#define C_TIME_BIT              (4)
#define C_TIME_MASK             (0x7<<C_TIME_BIT)

#define CH_SEL_BIT              (0)
#define CH_SEL_MASK             (0xF<<CH_SEL_BIT)

#define FLAG_BIT                (31)
#define DAT_MASK                (0xFFF)
#define SUMDAT_MASK             (0x3FFFF)

#define AP_TURN                 (0)
#define CHUB_TURN               (1)

#define TIMEOUT                 0x100000

uint32_t mConvMode[ADC_CHANNEL_MAX];

static int32_t adcDrvGetTurn()
{
    uint32_t timeout=0;

    __raw_writel(0x1, REG_SYSREG_ADC_CHUB_FLAG);
    __raw_writel(AP_TURN, REG_SYSREG_ADC_INIT_TURN);

    while (__raw_readl(REG_SYSREG_ADC_AP_FLAG) &&
           __raw_readl(REG_SYSREG_ADC_INIT_TURN == AP_TURN)) {
        timeout++;
        if (timeout >= TIMEOUT) {
            CSP_PRINTF_ERROR("Time Out! ADC is being used by AP.\n");
            return -1;
        }
    }
    return 0;
}

static void adcDrvReleaseTurn()
{
    __raw_writel(0x0, REG_SYSREG_ADC_CHUB_FLAG);
}

static void adcDrvInitHw(IN AdcChType ch)
{
    uint32_t regValue;

    /* Reset */
    __raw_writel((ADC_RESET_ASSERT << SOFT_RESET_BIT), REG_ADC_CON1);
    __raw_writel((ADC_RESET_RELEASE << SOFT_RESET_BIT), REG_ADC_CON1);

    /* Set Conversion Mode & Channel*/
    regValue = __raw_readl(REG_ADC_CON2);
    regValue &= ~(C_TIME_MASK | CH_SEL_MASK);
    regValue |= (mConvMode[ch] << C_TIME_BIT) | (ch << CH_SEL_BIT);

    __raw_writel(regValue, REG_ADC_CON2);
}

static void adcDrvConversionStart(void)
{
    uint32_t regValue;

    /* Start Conversion */
    regValue = __raw_readl(REG_ADC_CON1);
    regValue |= (STC_EN << STC_EN_BIT);
    __raw_writel(regValue, REG_ADC_CON1);

    /* Wait until conversion is done */
    while(1) {
        regValue = __raw_readl(REG_ADC_DAT);
        if (regValue & (0x1 << FLAG_BIT))
            break;
    }
}

int32_t adcDrvInit(IN AdcChType ch, IN AdcConvModeType convMode)
{
    mConvMode[ch] = (uint32_t)convMode;

    return 0;
}

int32_t adcDrvRead(IN AdcChType ch)
{
    uint32_t regValue;

    if(adcDrvGetTurn())
        return -1;

    adcDrvInitHw(ch);

    adcDrvConversionStart();

    regValue = __raw_readl(REG_ADC_DAT);
    regValue = regValue & DAT_MASK;

    adcDrvReleaseTurn();

    return regValue;
}

#endif
