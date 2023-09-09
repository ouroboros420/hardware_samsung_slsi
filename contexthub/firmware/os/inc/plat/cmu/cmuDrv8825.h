/*----------------------------------------------------------------------------
 * Exynos SoC  -  CMU
 *
 * Copyright (C) 2021 Samsung Electronics Co., Ltd.
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

#ifndef __CMU_DRIVER_8825_H__
#define __CMU_DRIVER_8825_H__

/*******************************************
 * CMU CHUB
 ******************************************/
#define REG_CMU_PLL_CON0_MUX_CLKCMU_CHUB_PERI_USER          (CMU_BASE_ADDRESS + 0x600)
#define REG_CMU_PLL_CON1_MUX_CLKCMU_CHUB_PERI_USER          (CMU_BASE_ADDRESS + 0x604)

#define REG_CMU_PLL_CON0_MUX_CLKCMU_CHUB_RCO_USER           (CMU_BASE_ADDRESS + 0x610)
#define REG_CMU_PLL_CON1_MUX_CLKCMU_CHUB_RCO_USER           (CMU_BASE_ADDRESS + 0x614)

#define REG_CMU_PLL_CON0_MUX_CLKCMU_CHUB_BUS_USER           (CMU_BASE_ADDRESS + 0x620)
#define REG_CMU_PLL_CON1_MUX_CLKCMU_CHUB_BUS_USER           (CMU_BASE_ADDRESS + 0x624)

#define REG_CMU_CHUB_CMU_CHUB_CONTROLLER_OPTION             (CMU_BASE_ADDRESS + 0x800)

#define REG_CMU_CLKOUT_CON_BLK_CHUB_CMU_CHUB_CLKOUT         (CMU_BASE_ADDRESS + 0x810)

#define REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_BUS                (CMU_BASE_ADDRESS + 0x1000)
#define REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_I2C                (CMU_BASE_ADDRESS + 0x1004)
#define REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_TIMER              (CMU_BASE_ADDRESS + 0x1008)
#define REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI0               (CMU_BASE_ADDRESS + 0x100C)
#define REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI1               (CMU_BASE_ADDRESS + 0x1010)
#define REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI2               (CMU_BASE_ADDRESS + 0x1014)
#define REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI3               (CMU_BASE_ADDRESS + 0x1018)

#define REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_BUS                (CMU_BASE_ADDRESS + 0x1800)
#define REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_I2C                (CMU_BASE_ADDRESS + 0x1804)
#define REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI0               (CMU_BASE_ADDRESS + 0x1808)
#define REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI1               (CMU_BASE_ADDRESS + 0x180C)
#define REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI2               (CMU_BASE_ADDRESS + 0x1810)
#define REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI3               (CMU_BASE_ADDRESS + 0x1814)

/*******************************************
 * CMU CHUBVTS
 ******************************************/
#define REG_CMU_PLL_CON0_MUX_CLKCMU_CHUBVTS_BUS_USER        (CMU_CHUBVTS_BASE_ADDRESS + 0x600)
#define REG_CMU_PLL_CON1_MUX_CLKCMU_CHUBVTS_BUS_USER        (CMU_CHUBVTS_BASE_ADDRESS + 0x604)

#define REG_CMU_PLL_CON0_MUX_CLKCMU_CHUBVTS_RCO_USER        (CMU_CHUBVTS_BASE_ADDRESS + 0x610)
#define REG_CMU_PLL_CON1_MUX_CLKCMU_CHUBVTS_RCO_USER        (CMU_CHUBVTS_BASE_ADDRESS + 0x614)

#define REG_CMU_CHUB_CMU_CHUBVTS_CONTROLLER_OPTION          (CMU_CHUBVTS_BASE_ADDRESS + 0x800)

#define REG_CMU_CLK_CON_MUX_MUX_CLK_CHUBVTS_BUS             (CMU_CHUBVTS_BASE_ADDRESS + 0x1000)

#define REG_CMU_CLK_CON_DIV_DIV_CLK_CHUBVTS_BUS             (CMU_CHUBVTS_BASE_ADDRESS + 0x1800)

/*******************************************
 * CMU CMGP
 ******************************************/
#define REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_I2C                (CMU_CMGP_BASE_ADDRESS + 0x1004)
#define REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_I3C                (CMU_CMGP_BASE_ADDRESS + 0x1008)
#define REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI0               (CMU_CMGP_BASE_ADDRESS + 0x100C)
#define REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI1               (CMU_CMGP_BASE_ADDRESS + 0x1010)
#define REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI2               (CMU_CMGP_BASE_ADDRESS + 0x1014)
#define REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI3               (CMU_CMGP_BASE_ADDRESS + 0x1018)
#define REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI4               (CMU_CMGP_BASE_ADDRESS + 0x101C)

#define REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C                (CMU_CMGP_BASE_ADDRESS + 0x1800)
#define REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I3C                (CMU_CMGP_BASE_ADDRESS + 0x1804)
#define REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI0               (CMU_CMGP_BASE_ADDRESS + 0x1808)
#define REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI1               (CMU_CMGP_BASE_ADDRESS + 0x180C)
#define REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI2               (CMU_CMGP_BASE_ADDRESS + 0x1810)
#define REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI3               (CMU_CMGP_BASE_ADDRESS + 0x1814)
#define REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI4               (CMU_CMGP_BASE_ADDRESS + 0x1818)


// --Selection for Mux
#define OSCCLK_RCO_CHUB                                     0
#define CLKCMU_CHUB_BUS                                     1
#define CLK_RCO_CHUB                                        1
#define CLK_CHUB_OSCCLK_RCO                                 0
#define CLK_CHUB_RTCCLK                                     1
#define MUX_CLK_RCO_CHUB_USER                               0
#define MUX_CLKCMU_CHUB_BUS_USER                            1
#define MUX_CLKCMU_CHUB_PERI_USER                           1
#define MUX_CLKCMU_CMGP_RCO_USER                            0
#define MUX_CLKCMU_CMGP_PERI_USER                           1

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
#define DIV_RATIO_MASK_2BIT                                 (0x3)
#define DIV_RATIO_MASK_3BIT                                 (0x7)
#define DIV_RATIO_MASK_4BIT                                 (0xF)
#define DIV_RATIO_MASK_5BIT                                 (0x1F)
#define DIV_BUSY                                            (0x1 << 16)
#define DIV_HWACG                                           (0x1 << 28)

#endif // __CMU_DRIVER_8825_H__

