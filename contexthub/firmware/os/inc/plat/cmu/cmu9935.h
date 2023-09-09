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

#ifndef __CMU9935_H__
#define __CMU9935_H__

typedef enum {

    CMU_MUX_CHUB_NOC_USER,
    CMU_MUX_CHUB_PERI_USER,
    CMU_MUX_CHUB_RCO_USER,
    CMU_MUX_CHUB_NOC,
    CMU_MUX_CHUB_TIMER,
    CMU_MUX_CHUB_I2C,
    CMU_MUX_CHUB_SPI_I2C0,
    CMU_MUX_CHUB_SPI_I2C1,
    CMU_MUX_CHUB_SPI_MS_CTRL,
    CMU_MUX_CHUB_USI0,
    CMU_MUX_CHUB_USI1,
    CMU_MUX_CHUB_USI2,
    CMU_MUX_CHUB_USI3,

    CMU_DIV_CHUB_I2C,
    CMU_DIV_CHUB_NOC,
    CMU_DIV_CHUB_SLIF_PROX,
    CMU_DIV_CHUB_SLIF_PROX_CORE,
    CMU_DIV_CHUB_SPI_I2C0,
    CMU_DIV_CHUB_SPI_I2C1,
    CMU_DIV_CHUB_SPI_MS_CTRL,
    CMU_DIV_CHUB_USI0,
    CMU_DIV_CHUB_USI1,
    CMU_DIV_CHUB_USI2,
    CMU_DIV_CHUB_USI3,

    CMU_MUX_CMGP_NOC_USER,
    CMU_MUX_CMGP_PERI_USER,
    CMU_MUX_CMGP_I2C,
    CMU_MUX_CMGP_SPI_I2C0,
    CMU_MUX_CMGP_SPI_I2C1,
    CMU_MUX_CMGP_SPI_MS_CTRL,
    CMU_MUX_CMGP_USI0,
    CMU_MUX_CMGP_USI1,
    CMU_MUX_CMGP_USI2,
    CMU_MUX_CMGP_USI3,
    CMU_MUX_CMGP_USI4,
    CMU_MUX_CMGP_USI5,
    CMU_MUX_CMGP_USI6,

    CMU_DIV_CMGP_I2C,
    CMU_DIV_CMGP_SPI_I2C0,
    CMU_DIV_CMGP_SPI_I2C1,
    CMU_DIV_CMGP_SPI_MS_CTRL,
    CMU_DIV_CMGP_USI0,
    CMU_DIV_CMGP_USI1,
    CMU_DIV_CMGP_USI2,
    CMU_DIV_CMGP_USI3,
    CMU_DIV_CMGP_USI4,
    CMU_DIV_CMGP_USI5,
    CMU_DIV_CMGP_USI6,

    CMU_CLK_OUTPUT_CPU,
    CMU_CLK_CHUB_BUS,

    CMU_CLK_SPI,
    CMU_CLK_CHUB_SPI0 = CMU_CLK_SPI,
    CMU_CLK_CHUB_SPI1,
    CMU_CLK_CHUB_SPI2,
    CMU_CLK_CHUB_SPI3,
    CMU_CLK_CMGP_SPI0,
    CMU_CLK_CMGP_SPI1,
    CMU_CLK_CMGP_SPI2,
    CMU_CLK_CMGP_SPI3,
    CMU_CLK_CMGP_SPI4,
    CMU_CLK_CMGP_SPI5,
    CMU_CLK_CMGP_SPI6,

    CMU_CLK_I2C,
    CMU_CLK_CHUB_I2C0 = CMU_CLK_I2C,
    CMU_CLK_CHUB_I2C1,
    CMU_CLK_CHUB_I2C2,
    CMU_CLK_CHUB_I2C3,
    CMU_CLK_CHUB_I2C4,
    CMU_CLK_CHUB_I2C5,
    CMU_CLK_CHUB_I2C6,
    CMU_CLK_CMGP_I2C0,
    CMU_CLK_CMGP_I2C1,
    CMU_CLK_CMGP_I2C2,
    CMU_CLK_CMGP_I2C3,
    CMU_CLK_CMGP_I2C4,
    CMU_CLK_CMGP_I2C5,
    CMU_CLK_CMGP_I2C6,
    CMU_CLK_CMGP_I2C7,
    CMU_CLK_CMGP_I2C8,
    CMU_CLK_CMGP_I2C9,
    CMU_CLK_CMGP_I2C10,
    CMU_CLK_CMGP_I2C11,
    CMU_CLK_CMGP_I2C12,
    CMU_CLK_CMGP_I2C13,

    CMU_CLK_I3C,
    CMU_CLK_CHUB_I3C0 = CMU_CLK_I3C,
    CMU_CLK_CMGP_I3C0,

    CMU_MAX_IP_TYPE
} CmuIpType;

typedef enum {
    CMU_HWACG_IP_CM4 = 0,
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
#define RTC_FREQ                 32768 //  32.768KHz
#define OSCCLK_RCO_FREQ       24576000 //  24.576MHz
#define OSCCLK_FREQ
#define CLK_RCO_FREQ          49152000 //  49.152MHz
#define RCO_PMU_FREQ          49152000 //  49.152MHz
#define RCO_400_FREQ         393216000 // 393.216MHz

#define CHUB_MAIN_CLK           (RCO_400_FREQ / 2)

#endif  // #ifndef __CMU9935_H__
