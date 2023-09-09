/*
 * Exynos SOC - ADC
 *
 * Copyright (C) 2020 Samsung Electronics Co., Ltd.
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

#include <csp_common.h>
#include <csp_printf.h>
#include <sysreg.h>
#include <adc.h>


#define REG_ADC_CON1            (ADC_CMGP_BASE_ADDRESS + 0x00)
#define REG_ADC_CON2            (ADC_CMGP_BASE_ADDRESS + 0x04)
#define REG_ADC_DAT             (ADC_CMGP_BASE_ADDRESS + 0x08)
#define REG_ADC_SUM_DAT         (ADC_CMGP_BASE_ADDRESS + 0x0C)
#define REG_ADC_INT_EN          (ADC_CMGP_BASE_ADDRESS + 0x10)
#define REG_ADC_INT_STATUS      (ADC_CMGP_BASE_ADDRESS + 0x14)
#define REG_ADC_DEBUG_DATA      (ADC_CMGP_BASE_ADDRESS + 0x1C)

#define STC_EN                  1
#define SOFT_RESET_BIT          1
#define ADC_RESET_ASSERT        2
#define ADC_RESET_RELEASE       1

#define C_TIME_BIT              4
#define C_TIME_MASK             0x7
#define CH_SEL_MASK             0xF

#define CONV_END                (0x1 << 31)
#define ADCDAT_MASK             0xFFF
#define SUMDAT_MASK             0x3FFFF

#define TIMEOUT                 100000

uint32_t mConvMode[ADC_CHANNEL_MAX];

static int32_t adcCheckChannel(const char* func, IN AdcChType ch)
{
    if (ch >= ADC_CHANNEL_MAX) {
        CSP_PRINTF_ERROR("%s ADC Channel error : %d \n", func,  ch);
        return -1;
    }

    return 0;
}

int32_t adcInit(IN AdcChType ch, IN AdcConvModeType convMode)
{
    if (adcCheckChannel(__func__, ch)) {
        return -1;
    }

    mConvMode[ch] = (uint32_t)(convMode & C_TIME_MASK);

    return 0;
}

static int32_t adcGetTurn(void)
{
    return sysregAdcGetTurn(TIMEOUT);
}

static void adcReleaseTurn(void)
{
    sysregAdcReleaseTurn();
}

static void adcInitHw(IN AdcChType ch, IN AdcConvModeType ctime)
{
    uint32_t regValue;

    /* Reset */
    __raw_writel((ADC_RESET_ASSERT << SOFT_RESET_BIT), REG_ADC_CON1);
    __raw_writel((ADC_RESET_RELEASE << SOFT_RESET_BIT), REG_ADC_CON1);

    /* Set Conversion Mode & Channel*/
    regValue = __raw_readl(REG_ADC_CON2);
    regValue &= ~((C_TIME_MASK << C_TIME_BIT) | CH_SEL_MASK);
    regValue |= (ctime << C_TIME_BIT) | (ch);

    __raw_writel(regValue, REG_ADC_CON2);
}

static int32_t adcConversionStart(void)
{
    uint32_t regValue;
    int32_t  timeout = TIMEOUT;

    /* Start Conversion */
    regValue = __raw_readl(REG_ADC_CON1);
    regValue |= STC_EN;
    __raw_writel(regValue, REG_ADC_CON1);

    /* Wait until conversion is done */
    while(timeout-- > 0) {
        regValue = __raw_readl(REG_ADC_DAT);
        if (regValue & (unsigned int)CONV_END)
            return 0;
    }

    return -1;
}

int32_t adcRead(IN AdcChType ch)
{
    uint32_t regValue;

    if (adcCheckChannel(__func__, ch)) {
        return -1;
    }

    if (adcGetTurn())
        return -1;

    adcInitHw(ch, (AdcConvModeType)(mConvMode[ch] & C_TIME_MASK));

    if (adcConversionStart()) {
        CSP_PRINTF_ERROR("ADC Conversion Fail!!\n");
        return -1;
    }

    regValue = __raw_readl(REG_ADC_DAT) & ADCDAT_MASK;

    adcReleaseTurn();

    return regValue;
}
