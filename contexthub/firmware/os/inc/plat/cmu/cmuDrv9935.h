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

#ifndef __CMU_DRIVER_9935_H__
#define __CMU_DRIVER_9935_H__

// CMU_CHUB
#define REG_CMU_PLL_CON0_MUX_CLKALIVE_CHUB_NOC_USER         (CMU_BASE_ADDRESS + 0x600)
#define REG_CMU_PLL_CON1_MUX_CLKALIVE_CHUB_NOC_USER         (CMU_BASE_ADDRESS + 0x604)
#define REG_CMU_PLL_CON0_MUX_CLKALIVE_CHUB_PERI_USER        (CMU_BASE_ADDRESS + 0x610)
#define REG_CMU_PLL_CON1_MUX_CLKALIVE_CHUB_PERI_USER        (CMU_BASE_ADDRESS + 0x614)
#define REG_CMU_PLL_CON0_MUX_CLKALIVE_CHUB_RCO_USER         (CMU_BASE_ADDRESS + 0x620)
#define REG_CMU_PLL_CON1_MUX_CLKALIVE_CHUB_RCO_USER         (CMU_BASE_ADDRESS + 0x624)
#define REG_CMU_CHUB_CMU_CHUB_CONTROLLER_OPTION             (CMU_BASE_ADDRESS + 0x800)
#define REG_CMU_CLKOUT_CON_BLK_CHUB_CMU_CHUB_CLKOUT         (CMU_BASE_ADDRESS + 0x810)

#define REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_TIMER              (CMU_BASE_ADDRESS + 0x1000)
#define REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_I2C                (CMU_BASE_ADDRESS + 0x1004)
#define REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_NOC                (CMU_BASE_ADDRESS + 0x1008)
#define REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_SPI_I2C0           (CMU_BASE_ADDRESS + 0x100C)
#define REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_SPI_I2C1           (CMU_BASE_ADDRESS + 0x1010)
#define REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_SPI_MS_CTRL        (CMU_BASE_ADDRESS + 0x1014)
#define REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI0               (CMU_BASE_ADDRESS + 0x1018)
#define REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI1               (CMU_BASE_ADDRESS + 0x101C)
#define REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI2               (CMU_BASE_ADDRESS + 0x1020)
#define REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI3               (CMU_BASE_ADDRESS + 0x1024)

#define REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_I2C                (CMU_BASE_ADDRESS + 0x1800)
#define REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_NOC                (CMU_BASE_ADDRESS + 0x1804)
#define REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SERIAL_LIF_US_PROX (CMU_BASE_ADDRESS + 0x1808)
#define REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SERIAL_LIF_US_PROX_CORE    (CMU_BASE_ADDRESS + 0x180C)
#define REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SPI_I2C0           (CMU_BASE_ADDRESS + 0x1810)
#define REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SPI_I2C1           (CMU_BASE_ADDRESS + 0x1814)
#define REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SPI_MS_CTRL        (CMU_BASE_ADDRESS + 0x1818)
#define REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI0               (CMU_BASE_ADDRESS + 0x181C)
#define REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI1               (CMU_BASE_ADDRESS + 0x1820)
#define REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI2               (CMU_BASE_ADDRESS + 0x1824)
#define REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI3               (CMU_BASE_ADDRESS + 0x1828)

// CMU_CHUBVTS
#define REG_CMU_PLL_CON0_MUX_CLKALIVE_CHUBVTS_NOC_USER      (CMU_CHUBVTS_BASE_ADDRESS + 0x600)
#define REG_CMU_PLL_CON1_MUX_CLKALIVE_CHUBVTS_NOC_USER      (CMU_CHUBVTS_BASE_ADDRESS + 0x604)
#define REG_CMU_PLL_CON0_MUX_CLKALIVE_CHUBVTS_RCO_USER      (CMU_CHUBVTS_BASE_ADDRESS + 0x610)
#define REG_CMU_PLL_CON1_MUX_CLKALIVE_CHUBVTS_RCO_USER      (CMU_CHUBVTS_BASE_ADDRESS + 0x614)
#define REG_CMU_CHUBVTS_CMU_CHUBVTS_CONTROLLER_OPTION       (CMU_CHUBVTS_BASE_ADDRESS + 0x800)
#define REG_CMU_CLKOUT_CON_BLK_CHUBVTS_CMU_CHUBVTS_CLKOUT   (CMU_CHUBVTS_BASE_ADDRESS + 0x810)
#define REG_CMU_CLK_CON_MUX_MUX_CLK_CHUBVTS_DMAILBOX_CCLK   (CMU_CHUBVTS_BASE_ADDRESS + 0x1000)
#define REG_CMU_CLK_CON_MUX_MUX_CLK_CHUBVTS_NOC             (CMU_CHUBVTS_BASE_ADDRESS + 0x1004)
#define REG_CMU_CLK_CON_DIV_DIV_CLK_CHUBVTS_DMAILBOX_CCLK   (CMU_CHUBVTS_BASE_ADDRESS + 0x1800)
#define REG_CMU_CLK_CON_DIV_DIV_CLK_CHUBVTS_ECU             (CMU_CHUBVTS_BASE_ADDRESS + 0x1804)
#define REG_CMU_CLK_CON_DIV_DIV_CLK_CHUBVTS_NOC             (CMU_CHUBVTS_BASE_ADDRESS + 0x1808)

// CMU_CMGP
#define REG_CMU_PLL_CON0_MUX_CLKALIVE_CMGP_NOC_USER         (CMU_CMGP_BASE_ADDRESS + 0x600)
#define REG_CMU_PLL_CON1_MUX_CLKALIVE_CMGP_NOC_USER         (CMU_CMGP_BASE_ADDRESS + 0x604)
#define REG_CMU_PLL_CON0_MUX_CLKALIVE_CMGP_PERI_USER        (CMU_CMGP_BASE_ADDRESS + 0x610)
#define REG_CMU_PLL_CON1_MUX_CLKALIVE_CMGP_PERI_USER        (CMU_CMGP_BASE_ADDRESS + 0x614)
#define REG_CMU_CMGP_CMU_CMGP_CONTROLLER_OPTION             (CMU_CMGP_BASE_ADDRESS + 0x800)
#define REG_CMU_CLKOUT_CON_BLK_CMGP_CMU_CMGP_CLKOUT         (CMU_CMGP_BASE_ADDRESS + 0x810)

#define REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_I2C                (CMU_CMGP_BASE_ADDRESS + 0x1000)
#define REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_SPI_I2C0           (CMU_CMGP_BASE_ADDRESS + 0x1004)
#define REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_SPI_I2C1           (CMU_CMGP_BASE_ADDRESS + 0x1008)
#define REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_SPI_MS_CTRL        (CMU_CMGP_BASE_ADDRESS + 0x100C)
#define REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI0               (CMU_CMGP_BASE_ADDRESS + 0x1010)
#define REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI1               (CMU_CMGP_BASE_ADDRESS + 0x1014)
#define REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI2               (CMU_CMGP_BASE_ADDRESS + 0x1018)
#define REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI3               (CMU_CMGP_BASE_ADDRESS + 0x101C)
#define REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI4               (CMU_CMGP_BASE_ADDRESS + 0x1020)
#define REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI5               (CMU_CMGP_BASE_ADDRESS + 0x1024)
#define REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI6               (CMU_CMGP_BASE_ADDRESS + 0x1028)

#define REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C                (CMU_CMGP_BASE_ADDRESS + 0x1800)
#define REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_SPI_I2C0           (CMU_CMGP_BASE_ADDRESS + 0x1804)
#define REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_SPI_I2C1           (CMU_CMGP_BASE_ADDRESS + 0x1808)
#define REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_SPI_MS_CTRL        (CMU_CMGP_BASE_ADDRESS + 0x180C)
#define REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI0               (CMU_CMGP_BASE_ADDRESS + 0x1810)
#define REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI1               (CMU_CMGP_BASE_ADDRESS + 0x1814)
#define REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI2               (CMU_CMGP_BASE_ADDRESS + 0x1818)
#define REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI3               (CMU_CMGP_BASE_ADDRESS + 0x181C)
#define REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI4               (CMU_CMGP_BASE_ADDRESS + 0x1820)
#define REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI5               (CMU_CMGP_BASE_ADDRESS + 0x1824)
#define REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI6               (CMU_CMGP_BASE_ADDRESS + 0x1828)

// --Selection for Mux
#define FREE_OSCCLK_CHUB                                    0
#define CLKALIVE_CHUBVTS_NOC                                1
#define CLKALIVE_CHUB_PERI                                  1
#define CLKALIVE_CHUBVTS_RCO                                1
#define RTCCLK_CHUB                                         1
#define MUX_CLKALIVE_CHUB_RCO_USER                          0
#define MUX_CLKALIVE_CHUB_PERI_USER                         1
#define MUX_CLKALIVE_CHUB_NOC_USER                          0

#define MUX_SEL                                             (0x1 << 0)
#define MUX_USER_SEL                                        (0x1 << 4)
#define MUX_BUSY                                            (0x1 << 16)
#define MUX_HWACG                                           (0x1 << 28)

// CHUB_CMU_CHUB_CONTROLLER_OPTION
#define CHUB_CONTROLLER_OPTION_DEBUG_ENABLE                 (1 << 31)
#define CHUB_CONTROLLER_OPTION_LAYER2_CTRL_ENABLE           (1 << 30)
#define CHUB_CONTROLLER_OPTION_PM_ENABLE                    (1 << 29)
#define CHUB_CONTROLLER_OPTION_HWACG_ENABLE                 (1 << 28)
#define CHUB_CONTROLLER_OPTION_MEMPG_ENABLE                 (1 << 24)

// CLKOUT_CON_BLK_CHUB_CMU_CHUB_CLKOUT
#define CHUB_CLKOUT_ENABLE                                  (1 << 29)
#define CHUB_CLKOUT_ENABLE_AUTOMATIC_CLKGATING              (1 << 28)
#define CHUB_CLKOUT_VALIDATE_CLK_REQ                        (1 << 20)
#define CHUB_CLKOUT_BUSY                                    (1 << 16)

#define CHUB_CLKOUT_SELECT_BIT                              8
#define CHUB_CLKOUT_SELECT_MASK                             0x1F

#define DIV_RATIO_BIT                                       0
#define DIV_BUSY                                            (0x1 << 16)
#define DIV_HWACG                                           (0x1 << 28)
#endif // __CMU_DRIVER_9935_H__
