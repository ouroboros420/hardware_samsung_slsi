/*----------------------------------------------------------------------------
 *      Exynos SoC  -  CMU
 *----------------------------------------------------------------------------
 *      Name:    cmuDrv.c
 *      Purpose: To implement CMU driver functions for 9610
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

#ifndef __CMU_DRIVER_9630_H__
#define __CMU_DRIVER_9630_H__

#define    REG_CMU_PLL_CON0_MUX_CLKCMU_CHUB_RCO_USER                                                 (CMU_BASE_ADDRESS + 0x600)
#define    REG_CMU_PLL_CON1_MUX_CLKCMU_CHUB_RCO_USER                                                 (CMU_BASE_ADDRESS + 0x604)

#define    REG_CMU_PLL_CON0_MUX_CLK_CHUB_BUS_USER                                                    (CMU_BASE_ADDRESS + 0x610)
#define    REG_CMU_PLL_CON1_MUX_CLK_CHUB_BUS_USER                                                    (CMU_BASE_ADDRESS + 0x614)

#define    REG_CMU_CHUB_CMU_CHUB_CONTROLLER_OPTION                                                   (CMU_BASE_ADDRESS + 0x800)

#define    REG_CMU_CLKOUT_CON_BLK_CHUB_CMU_CHUB_CLKOUT0                                              (CMU_BASE_ADDRESS + 0x810)

#define    REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_BUS                                                      (CMU_BASE_ADDRESS + 0x1000)
#define    REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_I2C                                                      (CMU_BASE_ADDRESS + 0x1004)
#define    REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_TIMER_FCLK                                               (CMU_BASE_ADDRESS + 0x1008)
#define    REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI0                                                     (CMU_BASE_ADDRESS + 0x100C)
#define    REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI1                                                     (CMU_BASE_ADDRESS + 0x1010)
#define    REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI2                                                     (CMU_BASE_ADDRESS + 0x1014)

#define    REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_BUS                                                      (CMU_BASE_ADDRESS + 0x1800)
#define    REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_I2C                                                      (CMU_BASE_ADDRESS + 0x1804)
#define    REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI0                                                     (CMU_BASE_ADDRESS + 0x1808)
#define    REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI1                                                     (CMU_BASE_ADDRESS + 0x180C)
#define    REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI2                                                     (CMU_BASE_ADDRESS + 0x1810)

// --Selection for Mux
#define OSCCLK_RCO_CHUB                                     0
#define RCO_CHUB                                            1
#define CLKCMU_CHUB_BUS                                     1
#define MUX_CLKCMU_CHUB_RCO_USER                            0
#define MUX_CLK_CHUB_BUS_USER                               1
#define RTCCLK_CHUB                                         1

// PLL_CON0_MUX_CLKCMU_CHUB_RCO_USER
// -- Select
#define SELECT_MUX_CLKCMU_CHUB_RCO_USER                     (1 << 4)
#define MUX_CLKCMU_CHUB_RCO_USER_IS_BUSY                    (1 << 16)

// PLL_CON1_MUX_CLKCMU_CHUB_RCO_USER
// -- Enable or disable HWACG
#define MUX_CLKCMU_CHUB_RCO_USER_HWACG_ENABLE               (1 << 28)

// PLL_CON0_MUX_CLK_CHUB_BUS_USER
// -- Select
#define SELECT_MUX_CLK_CHUB_BUS_USER                        (1 << 4)
#define MUX_CLK_CHUB_BUS_USER_BUSY                          (1 << 16)

// PLL_CON1_MUX_CLK_CHUB_BUS_USER
// -- Enable or disable HWACG
#define MUX_CLK_CHUB_BUS_USER_HWACG_ENABLE                  (1 << 28)

// CHUB_CMU_CHUB_CONTROLLER_OPTION
#define CHUB_CONTROLLER_OPTION_DEBUG_ENABLE                 (1 << 31)
#define CHUB_CONTROLLER_OPTION_LAYER2_CTRL_ENABLE           (1 << 30)
#define CHUB_CONTROLLER_OPTION_PM_ENABLE                    (1 << 29)
#define CHUB_CONTROLLER_OPTION_HWACG_ENABLE                 (1 << 28)
#define CHUB_CONTROLLER_OPTION_MEMPG_ENABLE                 (1 << 24)

// CLKOUT_CON_BLK_CHUB_CMU_CHUB_CLKOUT0
#define CHUB_CLKOUT0_ENABLE                                 (1 << 29)
#define CHUB_CLKOUT0_ENABLE_AUTOMATIC_CLKGATING             (1 << 28)
#define CHUB_CLKOUT0_VALIDATE_CLK_REQ                       (1 << 27)
#define CHUB_CLKOUT0_BUSY                                   (1 << 16)

// CLK_CON_MUX_MUX_CLK_CHUB_BUS
// -- Select
#define SELECT_MUX_CLK_CHUB_BUS                             (1 << 0)
#define MUX_CLK_CHUB_BUS_BUSY                               (1 << 16)
// -- Enable or disable HWACG
#define MUX_CLK_CHUB_BUS_HWACG_ENABLE                       (1 << 28)

// CLK_CON_MUX_MUX_CLK_CHUB_I2C
// -- Select
#define SELECT_MUX_CLK_CHUB_I2C                             (1 << 0)
#define MUX_CLK_CHUB_I2C_BUSY                               (1 << 16)
// -- Enable or disable HWACG
#define MUX_CLK_CHUB_I2C_HWACG_ENABLE                       (1 << 28)

// CLK_CON_MUX_MUX_CLK_CHUB_TIMER_FCLK
// -- Select
#define SELECT_MUX_CLK_CHUB_TIMER_FCLK                      (1 << 0)
#define MUX_CLK_CHUB_TIMER_FCLK_BUSY                        (1 << 16)
// -- Enable or disable HWACG
#define MUX_CLK_CHUB_TIMER_FCLK_HWACG_ENABLE                (1 << 28)

// CLK_CON_MUX_MUX_CLK_CHUB_USI0
// -- Select
#define SELECT_MUX_CLK_CHUB_USI0                            (1 << 0)
#define MUX_CLK_CHUB_USI0_BUSY                              (1 << 16)
// -- Enable or disable HWACG
#define MUX_CLK_CHUB_USI0_HWACG_ENABLE                      (1 << 28)

// CLK_CON_MUX_MUX_CLK_CHUB_USI1
// -- Select
#define SELECT_MUX_CLK_CHUB_USI1                            (1 << 0)
#define MUX_CLK_CHUB_USI1_BUSY                              (1 << 16)
// -- Enable or disable HWACG
#define MUX_CLK_CHUB_USI1_HWACG_ENABLE                      (1 << 28)

// CLK_CON_MUX_MUX_CLK_CHUB_USI2
// -- Select
#define SELECT_MUX_CLK_CHUB_USI2                            (1 << 0)
#define MUX_CLK_CHUB_USI2_BUSY                              (1 << 16)
// -- Enable or disable HWACG
#define MUX_CLK_CHUB_USI2_HWACG_ENABLE                      (1 << 28)

// CLK_CON_DIV_DIV_CLK_CHUB_BUS
// -- DIVRATIO
#define DIV_CLK_CHUB_BUS_DIVRATIO_BIT                       0x7
#define DIV_CLK_CHUB_BUS_BUSY                               (1 << 16)
// -- Enable or disable HWACG
#define DIV_CLK_CHUB_BUS_HWACG_ENABLE                       (1 << 28)

// CLK_CON_DIV_DIV_CLK_CHUB_I2C
// -- DIVRATIO
#define DIV_CLK_CHUB_I2C_DIVRATIO_BIT                       0xF
#define DIV_CLK_CHUB_I2C_BUSY                               (1 << 16)
// -- Enable or disable HWACG
#define DIV_CLK_CHUB_I2C_HWACG_ENABLE                       (1 << 28)

// CLK_CON_DIV_DIV_CLK_CHUB_USI0
// -- DIVRATIO
#define DIV_CLK_CHUB_USI0_DIVRATIO_BIT                       0xF
#define DIV_CLK_CHUB_USI0_BUSY                               (1 << 16)
// -- Enable or disable HWACG
#define DIV_CLK_CHUB_USI0_HWACG_ENABLE                       (1 << 28)

// CLK_CON_DIV_DIV_CLK_CHUB_USI1
// -- DIVRATIO
#define DIV_CLK_CHUB_USI1_DIVRATIO_BIT                       0xF
#define DIV_CLK_CHUB_USI1_BUSY                               (1 << 16)
// -- Enable or disable HWACG
#define DIV_CLK_CHUB_USI1_HWACG_ENABLE                       (1 << 28)

// CLK_CON_DIV_DIV_CLK_CHUB_USI2
// -- DIVRATIO
#define DIV_CLK_CHUB_USI2_DIVRATIO_BIT                       0xF
#define DIV_CLK_CHUB_USI2_BUSY                               (1 << 16)
// -- Enable or disable HWACG
#define DIV_CLK_CHUB_USI2_HWACG_ENABLE                       (1 << 28)

#endif

