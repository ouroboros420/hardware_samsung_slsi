/*----------------------------------------------------------------------------
 *      Exynos SoC  -  CMU
 *----------------------------------------------------------------------------
 *      Name:    cmuDrv.c
 *      Purpose: To implement CMU driver functions for 6315
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

#ifndef __CMU_DRIVER_6315_H__
#define __CMU_DRIVER_6315_H__

#define REG_CMU_PLL_CON0_MUX_CLKALIVE_CHUB_NOC_USER	    (CMU_BASE_ADDRESS + 0x0600)
#define REG_CMU_PLL_CON1_MUX_CLKALIVE_CHUB_NOC_USER	    (CMU_BASE_ADDRESS + 0x0604)
#define REG_CMU_PLL_CON0_MUX_CLKALIVE_CHUB_PERI_USER	(CMU_BASE_ADDRESS + 0x0610)
#define REG_CMU_PLL_CON1_MUX_CLKALIVE_CHUB_PERI_USER	(CMU_BASE_ADDRESS + 0x0614)
#define REG_CMU_PLL_CON0_MUX_CLKALIVE_CHUB_RCO_USER	    (CMU_BASE_ADDRESS + 0x0620)
#define REG_CMU_PLL_CON1_MUX_CLKALIVE_CHUB_RCO_USER	    (CMU_BASE_ADDRESS + 0x0624)
#define REG_CMU_CHUB_CMU_CHUB_CONTROLLER_OPTION	        (CMU_BASE_ADDRESS + 0x0800)
#define REG_CMU_CLKOUT_CON_BLK_CHUB_CMU_CHUB_CLKOUT	    (CMU_BASE_ADDRESS + 0x0810)
#define REG_CMU_CHUB_SPARE0	                            (CMU_BASE_ADDRESS + 0x0a00)
#define REG_CMU_CHUB_SPARE1	                            (CMU_BASE_ADDRESS + 0x0a04)
#define REG_CMU_CLK_CON_MUX_MUX_CHUB_TIMER	            (CMU_BASE_ADDRESS + 0x1000)
#define REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_I2C	        (CMU_BASE_ADDRESS + 0x1004)
#define REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_SPI_I2C0	    (CMU_BASE_ADDRESS + 0x1010)
#define REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_SPI_I2C1	    (CMU_BASE_ADDRESS + 0x1014)
#define REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_SPI_MS_CTRL    (CMU_BASE_ADDRESS + 0x1018)
#define REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI0	        (CMU_BASE_ADDRESS + 0x101c)
#define REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI1	        (CMU_BASE_ADDRESS + 0x1020)
#define REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI2	        (CMU_BASE_ADDRESS + 0x1024)
#define REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI3	        (CMU_BASE_ADDRESS + 0x1028)
#define REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_I2C	        (CMU_BASE_ADDRESS + 0x1800)
#define REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_NOC	        (CMU_BASE_ADDRESS + 0x180c)
#define REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SPI_I2C0	    (CMU_BASE_ADDRESS + 0x1810)
#define REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SPI_I2C1	    (CMU_BASE_ADDRESS + 0x1814)
#define REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SPI_MS_CTRL	(CMU_BASE_ADDRESS + 0x1818)
#define REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI0	        (CMU_BASE_ADDRESS + 0x181c)
#define REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI1	        (CMU_BASE_ADDRESS + 0x1820)
#define REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI2	        (CMU_BASE_ADDRESS + 0x1824)
#define REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI3	        (CMU_BASE_ADDRESS + 0x1828)

#define OSCCLK_CHUB                                     0
#define CLKALIVE_CHUB_NOC                               1
#define CLKALIVE_CHUB_PERI                              1
#define CLKALIVE_CHUB_RCO                               1
#define RTCCLK_CHUB                                     1
#define MUX_CLKALIVE_CHUB_RCO_USER                      0
#define MUX_CLKALIVE_CHUB_PERI_USER                     1

// --Selection for Mux
#define MUX_SEL                                         (0x1 << 0)
#define MUX_USER_SEL                                    (0x1 << 4)
#define MUX_BUSY                                        (0x1 << 16)
#define MUX_HWACG                                       (0x1 << 28)

// --Div
#define DIV_RATIO_BIT                                   0
#define DIV_RATIO_MASK_2BIT                             (0x3)
#define DIV_RATIO_MASK_3BIT                             (0x7)
#define DIV_RATIO_MASK_4BIT                             (0xF)
#define DIV_RATIO_MASK_5BIT                             (0x1F)
#define DIV_BUSY                                        (0x1 << 16)
#define DIV_HWACG                                       (0x1 << 28)

// CHUB_CMU_CHUB_CONTROLLER_OPTION
#define CHUB_CONTROLLER_OPTION_DEBUG_ENABLE             (1 << 31)
#define CHUB_CONTROLLER_OPTION_LAYER2_CTRL_ENABLE       (1 << 30)
#define CHUB_CONTROLLER_OPTION_PM_ENABLE                (1 << 29)
#define CHUB_CONTROLLER_OPTION_HWACG_ENABLE             (1 << 28)
#define CHUB_CONTROLLER_OPTION_MEMPG_ENABLE             (1 << 24)

/*
// CLKOUT_CON_BLK_CHUB_CMU_CHUB_CLKOUT0
// CLKOUT_CON_BLK_CHUB_CMU_CHUB_CLKOUT1
#define CHUB_CLKOUT0_ENABLE                                 (1 << 29)
#define CHUB_CLKOUT0_ENABLE_AUTOMATIC_CLKGATING             (1 << 28)
#define CHUB_CLKOUT0_VALIDATE_CLK_REQ                       (1 << 27)
#define CHUB_CLKOUT0_BUSY                                   (1 << 16)

*/
#endif

