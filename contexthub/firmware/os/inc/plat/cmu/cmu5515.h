/*----------------------------------------------------------------------------
 * Exynos SoC  -  CMU
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

#ifndef __CMU5515_H__
#define __CMU5515_H__

typedef enum {
    CMU_CLK_OSCCLK_RCO_CHUB = 0,
    CMU_CLK_CLK_RCO_CHUB,
    CMU_CLK_RTCCLK,
    CMU_CLK_OUTPUT_CPU,
    CMU_CLK_CHUB_BUS,
    CMU_CLK_CHUB_CM4,
    CMU_CLK_CHUB_CM55,
    CMU_CLK_CHUB_USI0,
    CMU_CLK_CHUB_SPI0,
    CMU_CLK_CHUB_I2C0,
    CMU_CLK_CHUB_I2C1,
    CMU_CLK_CHUB_TIMER,

    CMU_CLK_OSCCLK_RCO_CMGP,
    CMU_CLK_CLK_RCO_CMGP,
    CMU_CLK_CMGP_USI0,
    CMU_CLK_CMGP_USI1,
    CMU_CLK_CMGP_USI2,
    CMU_CLK_CMGP_USI3,

    CMU_CLK_CMGP_SPI0,
    CMU_CLK_CMGP_SPI1,
    CMU_CLK_CMGP_SPI2,
    CMU_CLK_CMGP_SPI3,

    CMU_CLK_CMGP_I2C0,
    CMU_CLK_CMGP_I2C1,
    CMU_CLK_CMGP_I2C2,
    CMU_CLK_CMGP_I2C3,
    CMU_CLK_CMGP_I2C4,
    CMU_CLK_CMGP_I2C5,
    CMU_CLK_CMGP_I2C6,

    CMU_CLK_CMGP_ADC,

    CMU_CLK_I2C,
    CMU_CLK_I2C0 = CMU_CLK_I2C,
    CMU_CLK_I2C1,
    CMU_CLK_I2C2,
    CMU_CLK_I2C3,
    CMU_CLK_I2C4,
    CMU_CLK_I2C5,
    CMU_CLK_I2C6,
    CMU_CLK_I2C7,
    CMU_CLK_I2C8,
    CMU_CLK_I2C9,
    CMU_CLK_I2C10,
    CMU_CLK_I2C11,
    CMU_CLK_I2C12,
    CMU_CLK_I2C13,

    CMU_MAX_IP_TYPE
} CmuIpType;

typedef enum {
    CMU_HWACG_IP_CM4 = 0,
    CMU_HWACG_IP_CM55,
    CMU_HWACG_IP_GPIO,
    CMU_HWACG_IP_I2C_CHUB00,
    CMU_HWACG_IP_PDMA,
    CMU_HWACG_IP_PWM,
    CMU_HWACG_IP_TIMER,
    CMU_HWACG_IP_USI0,
    CMU_HWACG_IP_WDT0,
    CMU_HWACG_IP_WDT1,
    CMU_HWACG_IP_I2C0,
    CMU_HWACG_IP_I2C1,
    CMU_HWACG_IP_SYSREG,

    CMU_HWACG_MAX_IP_TYPE
} CmuHwacgIpType;

// Constant values of clocks =============
#define RTC_FREQ                32768      // 32.768Khz
#define XTAL_FREQ               26000000   // 26Mhz
#define OSCCLK_RCO_FREQ         24576000   // 24.576Mhz
#define CLK_RCO_FREQ            49152000   // 49.152Mhz
#define HFRCO_FREQ              393216000  // 393.216Mhz
#define CMGP_PLL_FREQ           400000000  // 393.216Mhz

#define CHUB_MAIN_CLK           HFRCO_FREQ

void cmuDrvDisableCM55Qch(void);
#endif  // #ifndef __CMU5515_H__
