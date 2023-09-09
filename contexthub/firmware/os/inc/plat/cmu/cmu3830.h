/*----------------------------------------------------------------------------
 * Exynos SoC  -  CMU
 *
 * Copyright (C) 2021 Samsung Electronics Co., Ltd.
 *
 * Huiling Wu <huiling.wu@samsung.com>
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

#ifndef __CMU3830_H__
#define __CMU3830_H__

typedef enum {
    CMU_MUX_CHUB_BUS_USER,
    CMU_MUX_CHUB_RCO_USER,
    CMU_MUX_CHUB_BUS,

    CMU_DIV_CHUB_BUS,

    CMU_MUX_CMGP_USI0,
    CMU_MUX_CMGP_USI1,

    CMU_DIV_CMGP_USI0,
    CMU_DIV_CMGP_USI1,

    CMU_CLK_SPI,
    CMU_CLK_SPI0 = CMU_CLK_SPI,
    CMU_CLK_SPI1,

    CMU_CLK_I2C,
    CMU_CLK_I2C0 = CMU_CLK_I2C,
    CMU_CLK_I2C1,

    CMU_CLK_OUTPUT_CPU,
    CMU_CLK_CHUB_BUS,

    CMU_MAX_IP_TYPE
} CmuIpType;

typedef enum {
    CMU_HWACG_IP_CM4 = 0,
    CMU_HWACG_IP_PWM,
    CMU_HWACG_IP_TIMER,
    CMU_HWACG_IP_WDT0,
    CMU_HWACG_IP_SYSREG,

    CMU_HWACG_MAX_IP_TYPE
} CmuHwacgIpType;

// Constant values of clocks =============
#define RTC_FREQ                32768      // 32.768Khz
#define XTAL_FREQ               26000000   // 26Mhz
#define OSCCLK_RCO_FREQ         24576000   // 24.576Mhz
#define CLK_RCO_FREQ            49152000   // 49.152Mhz
#define CLKCMU_CHUB_BUS_USER    360000000  // 360MHz
#define CLKCMU_CMGP_BUS_USER    400000000  // 400MHz

#define CHUB_MAIN_CLK           CLKCMU_CHUB_BUS_USER

#endif  // #ifndef __CMU3830_H__
