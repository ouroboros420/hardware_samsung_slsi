/*----------------------------------------------------------------------------
 *      Exynos SoC  -  CMU
 *----------------------------------------------------------------------------
 *      Name:    cmuDrv.c
 *      Purpose: To implement CMU driver functions for 9110
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

#ifndef __CMU_DRIVER_9110_H__
#define __CMU_DRIVER_9110_H__

/* CMU_CHUB */

#define    REG_CMU_PLL_CON0_MUX_CLKCMU_CHUB_BUS_USER                                                 (CMU_BASE_ADDRESS + 0x100)
#define    REG_CMU_PLL_CON1_MUX_CLKCMU_CHUB_BUS_USER                                                 (CMU_BASE_ADDRESS + 0x104)
#define    REG_CMU_PLL_CON2_MUX_CLKCMU_CHUB_BUS_USER                                                 (CMU_BASE_ADDRESS + 0x108)

#define    REG_CMU_PLL_CON0_MUX_CLKCMU_RCO_CHUB_BUS_USER                                             (CMU_BASE_ADDRESS + 0x120)
#define    REG_CMU_PLL_CON1_MUX_CLKCMU_RCO_CHUB_BUS_USER                                             (CMU_BASE_ADDRESS + 0x124)
#define    REG_CMU_PLL_CON2_MUX_CLKCMU_RCO_CHUB_BUS_USER                                             (CMU_BASE_ADDRESS + 0x128)

#define    REG_CMU_CHUB_CMU_CHUB_CONTROLLER_OPTION                                                   (CMU_BASE_ADDRESS + 0x800)

#define    REG_CMU_CLKOUT_CON_BLK_CHUB_CMU_CHUB_CLKOUT0                                              (CMU_BASE_ADDRESS + 0x810)

#define    REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_BUS                                                      (CMU_BASE_ADDRESS + 0x1000)

#define    REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_BUS                                                      (CMU_BASE_ADDRESS + 0x1800)

#define    REG_CMU_CLK_CON_GAT_CLK_BLK_CHUB_UID_CHUB_CMU_CHUB_IPCLKPORT_PCLK                         (CMU_BASE_ADDRESS + 0x2000)
#define    REG_CMU_CLK_CON_GAT_CLK_BLK_CHUB_UID_RSTNSYNC_CLK_CHUB_OSCCLK_RCO_IPCLKPORT_CLK           (CMU_BASE_ADDRESS + 0x2004)
#define    REG_CMU_CLK_CON_GAT_CLK_BLK_CHUB_UID_RSTNSYNC_CLK_CHUB_RTCCLK_IPCLKPORT_CLK               (CMU_BASE_ADDRESS + 0x2008)
#define    REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_AHB_BUSMATRIX_CHUB_IPCLKPORT_HCLK                   (CMU_BASE_ADDRESS + 0x200C)
#define    REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_BAAW_D_CHUB_IPCLKPORT_I_PCLK                        (CMU_BASE_ADDRESS + 0x2010)
#define    REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_BAAW_P_APM_CHUB_IPCLKPORT_I_PCLK                    (CMU_BASE_ADDRESS + 0x2014)
#define    REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_BPS_AXI_LP_CHUB_IPCLKPORT_I_CLK                     (CMU_BASE_ADDRESS + 0x2018)
#define    REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_BPS_AXI_P_CHUB_IPCLKPORT_I_CLK                      (CMU_BASE_ADDRESS + 0x201C)
#define    REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_CM4_CHUB_IPCLKPORT_FCLK                             (CMU_BASE_ADDRESS + 0x2020)
#define    REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_D_TZPC_CHUB_IPCLKPORT_PCLK                          (CMU_BASE_ADDRESS + 0x2024)
#define    REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_LHM_AXI_LP_CHUB_IPCLKPORT_I_CLK                     (CMU_BASE_ADDRESS + 0x2028)
#define    REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_LHM_AXI_P_CHUB_IPCLKPORT_I_CLK                      (CMU_BASE_ADDRESS + 0x202C)
#define    REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_LHS_AXI_C_CHUB_IPCLKPORT_I_CLK                      (CMU_BASE_ADDRESS + 0x2030)
#define    REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_LHS_AXI_D_CHUB_IPCLKPORT_I_CLK                      (CMU_BASE_ADDRESS + 0x2034)
#define    REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_PDMA_CHUB_IPCLKPORT_ACLK                            (CMU_BASE_ADDRESS + 0x2038)
#define    REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_PWM_CHUB_IPCLKPORT_I_PCLK_S0                        (CMU_BASE_ADDRESS + 0x203C)
#define    REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_RSTNSYNC_CLK_CHUB_BUS_IPCLKPORT_CLK                 (CMU_BASE_ADDRESS + 0x2040)
#define    REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_SWEEPER_D_CHUB_IPCLKPORT_ACLK                       (CMU_BASE_ADDRESS + 0x2044)
#define    REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_SWEEPER_P_APM_CHUB_IPCLKPORT_ACLK                   (CMU_BASE_ADDRESS + 0x2048)
#define    REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_SYSREG_CHUB_IPCLKPORT_PCLK                          (CMU_BASE_ADDRESS + 0x204C)
#define    REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_TIMER_CHUB_IPCLKPORT_PCLK                           (CMU_BASE_ADDRESS + 0x2050)
#define    REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_WDT_CHUB_IPCLKPORT_PCLK                             (CMU_BASE_ADDRESS + 0x2054)

#define    REG_CMU_PCH_CON_LHM_AXI_LP_CHUB_PCH                                                       (CMU_BASE_ADDRESS + 0x3000)
#define    REG_CMU_PCH_CON_LHM_AXI_P_CHUB_PCH                                                        (CMU_BASE_ADDRESS + 0x3004)
#define    PCH_CON_LHS_AXI_C_CHUB_PCH                                                                (CMU_BASE_ADDRESS + 0x3008)
#define    PCH_CON_LHS_AXI_D_CHUB_PCH                                                                (CMU_BASE_ADDRESS + 0x300C)
#define    REG_CMU_QCH_CON_BAAW_D_CHUB_QCH                                                           (CMU_BASE_ADDRESS + 0x3010)
#define    REG_CMU_QCH_CON_BAAW_P_APM_CHUB_QCH                                                       (CMU_BASE_ADDRESS + 0x3014)
#define    REG_CMU_QCH_CON_CHUB_CMU_CHUB_QCH                                                         (CMU_BASE_ADDRESS + 0x3018)
#define    REG_CMU_QCH_CON_CM4_CHUB_QCH                                                              (CMU_BASE_ADDRESS + 0x301C)
#define    REG_CMU_QCH_CON_D_TZPC_CHUB_QCH                                                           (CMU_BASE_ADDRESS + 0x3020)
#define    REG_CMU_QCH_CON_LHM_AXI_LP_CHUB_QCH                                                       (CMU_BASE_ADDRESS + 0x3024)
#define    REG_CMU_QCH_CON_LHM_AXI_P_CHUB_QCH                                                        (CMU_BASE_ADDRESS + 0x3028)
#define    REG_CMU_QCH_CON_LHS_AXI_C_CHUB_QCH                                                        (CMU_BASE_ADDRESS + 0x302C)
#define    REG_CMU_QCH_CON_LHS_AXI_D_CHUB_QCH                                                        (CMU_BASE_ADDRESS + 0x3030)
#define    REG_CMU_QCH_CON_PDMA_CHUB_QCH                                                             (CMU_BASE_ADDRESS + 0x3034)
#define    REG_CMU_QCH_CON_PWM_CHUB_QCH                                                              (CMU_BASE_ADDRESS + 0x3038)
#define    REG_CMU_QCH_CON_SWEEPER_D_CHUB_QCH                                                        (CMU_BASE_ADDRESS + 0x303C)
#define    REG_CMU_QCH_CON_SWEEPER_P_APM_CHUB_QCH                                                    (CMU_BASE_ADDRESS + 0x3040)
#define    REG_CMU_QCH_CON_SYSREG_CHUB_QCH                                                           (CMU_BASE_ADDRESS + 0x3044)
#define    REG_CMU_QCH_CON_TIMER_CHUB_QCH                                                            (CMU_BASE_ADDRESS + 0x3048)
#define    REG_CMU_QCH_CON_WDT_CHUB_QCH                                                              (CMU_BASE_ADDRESS + 0x304C)

#define    REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_ADC                                                      (CMU_CMGP_BASE_ADDRESS + 0x1000)
#define    REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_I2C0                                                     (CMU_CMGP_BASE_ADDRESS + 0x1004)
#define    REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_I2C1                                                     (CMU_CMGP_BASE_ADDRESS + 0x1008)
#define    REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_I2C2                                                     (CMU_CMGP_BASE_ADDRESS + 0x100C)
#define    REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_I2C3                                                     (CMU_CMGP_BASE_ADDRESS + 0x1010)
#define    REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI0                                                     (CMU_CMGP_BASE_ADDRESS + 0x1020)
#define    REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI1                                                     (CMU_CMGP_BASE_ADDRESS + 0x1024)
#define    REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI2                                                     (CMU_CMGP_BASE_ADDRESS + 0x1028)
#define    REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI3                                                     (CMU_CMGP_BASE_ADDRESS + 0x102C)

#define    REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_ADC                                                      (CMU_CMGP_BASE_ADDRESS + 0x1800)
#define    REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C0                                                     (CMU_CMGP_BASE_ADDRESS + 0x1804)
#define    REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C1                                                     (CMU_CMGP_BASE_ADDRESS + 0x1808)
#define    REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C2                                                     (CMU_CMGP_BASE_ADDRESS + 0x180C)
#define    REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C3                                                     (CMU_CMGP_BASE_ADDRESS + 0x1810)
#define    REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI0                                                     (CMU_CMGP_BASE_ADDRESS + 0x1820)
#define    REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI1                                                     (CMU_CMGP_BASE_ADDRESS + 0x1824)
#define    REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI2                                                     (CMU_CMGP_BASE_ADDRESS + 0x1828)
#define    REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI3                                                     (CMU_CMGP_BASE_ADDRESS + 0x182C)


// PLL_CON0_MUX_CLKCMU_CHUB_BUS_USER

// -- Select
#define MUX_SEL_OFFSET                                      (4)
#define MUX_SEL_BUSY_OFFSET                                 (7)
#define SELECT_OFFSET                                       (0)
#define SELECT_BUSY_OFFSET                                  (1 << 16)
#define SELECT_OSCCLK_RCO                                   (0 << 4)
#define SELECT_CLKCMU_CHUB_BUS                              (1 << 4)
#define MUX_CLKCMU_CHUB_BUS_USER_IS_BUSY                    0x80



// PLL_CON2_MUX_CLKCMU_CHUB_DLL_BUS_USER

// -- Enable or disable HWACG
#define MUX_CLKCMU_CHUB_BUS_USER_HWACG_DISABLE              (0 << 28)
#define MUX_CLKCMU_CHUB_BUS_USER_HWACG_ENABLE               (1 << 28)



// PLL_CON0_MUX_CLKCMU_CHUB_DLL_BUS_USER

// -- Select
//#define SELECT_OSCCLK_RCO                                   (0 << 4)
#define SELECT_CLKCMU_APM_DLL                               (1 << 4)
#define MUX_CLKCMU_CHUB_DLL_USER_IS_BUSY                    0x80



// PLL_CON2_MUX_CLKCMU_CHUB_DLL_BUS_USER

// -- Enable or disable HWACG
#define MUX_CLKCMU_CHUB_DLL_BUS_USER_HWACG_DISABLE          (0 << 28)
#define MUX_CLKCMU_CHUB_DLL_BUS_USER_HWACG_ENABLE           (1 << 28)



// CHUB_CMU_CHUB_CONTROLLER_OPTION

// -- Enable or disable pwer management
#define CHUB_CONTROLLER_DISABLE_DEBUG                       (0 << 31)
#define CHUB_CONTROLLER_ENABLE_DEBUG                        (1 << 31)

// -- Enable or disable pwer management
#define CHUB_CONTROLLER_OPTION_PM_DISABLE                   (0 << 29)
#define CHUB_CONTROLLER_OPTION_PM_ENABLE                    (1 << 29)

// -- Enable or disable HWACG
#define CHUB_CONTROLLER_OPTION_HWACG_DISABLE                (0 << 28)
#define CHUB_CONTROLLER_OPTION_HWACG_ENABLE                 (1 << 28)



// CLK_CON_MUX_CLK_CHUB_TIMER_FCLK

// -- Select
#define SELECT_DFT_CLK_CHUB_OSCCLK_RCO                      (0 << 0)
#define SELECT_DFT_CLK_CHUB_RTCCLK                          (1 << 0)
#define CLK_CON_MUX_CLK_CHUB_TIMER_IS_BUSY                  0x10000

// -- Enable or disable HWACG
#define CLK_CON_MUX_CLK_CHUB_TIMER_HWACG_DISABLE            (0 << 28)
#define CLK_CON_MUX_CLK_CHUB_TIMER_HWACG_ENABLE             (1 << 28)



// CLK_CON_MUX_MUX_CLK_CHUB_BUS

// -- Select
#define SELECT_MUX_CLKCMU_CHUB_BUS_USER                     (0 << 0)
#define SELECT_MUX_CLKCMU_CHUB_DLL_BUS_USER                 (1 << 0)
#define MUX_CLK_CHUB_BUS_IS_BUSY                            0x10000

// -- Enable or disable HWACG
#define MUX_CLK_CHUB_BUS_HWACG_DISABLE                      (0 << 28)
#define MUX_CLK_CHUB_BUS_HWACG_ENABLE                       (1 << 28)



// CLK_CON_MUX_MUX_CLK_CHUB_I2C

// -- Select
#define SELECT_I2C_OSCCLK_RCO                               (0 << 0)
#define SELECT_I2C_MUX_CLK_CHUB_BUS                         (1 << 0)
#define MUX_CLK_CHUB_I2C_IS_BUSY                            0x10000

// -- Enable or disable HWACG
#define MUX_CLK_CHUB_I2C_HWACG_DISABLE                      (0 << 28)
#define MUX_CLK_CHUB_I2C_HWACG_ENABLE                       (1 << 28)



// CLK_CON_MUX_MUX_CLK_CHUB_USI00

// -- Select
#define SELECT_USI00_OSCCLK_RCO                             (0 << 0)
#define SELECT_USI00_MUX_CLK_CHUB_BUS                       (1 << 0)
#define MUX_CLK_CHUB_USI00_IS_BUSY                          0x10000

// -- Enable or disable HWACG
#define MUX_CLK_CHUB_USI00_HWACG_DISABLE                    (0 << 28)
#define MUX_CLK_CHUB_USI00_HWACG_ENABLE                     (1 << 28)



// CLK_CON_MUX_MUX_CLK_CHUB_USI01

// -- Select
#define SELECT_USI01_OSCCLK_RCO                             (0 << 0)
#define SELECT_USI01_MUX_CLK_CHUB_BUS                       (1 << 0)
#define MUX_CLK_CHUB_USI01_IS_BUSY                          0x10000

// -- Enable or disable HWACG
#define MUX_CLK_CHUB_USI01_HWACG_DISABLE                    (0 << 28)
#define MUX_CLK_CHUB_USI01_HWACG_ENABLE                     (1 << 28)



// CLK_CON_DIV_DIV_CLK_CHUB_BUS

// -- DIVRATIO
#define CLK_CON_DIV_DIV_CLK_CHUB_BUS_DEFAULT_DIVRATIO       (0 << 0)
#define DIV_CLK_CHUB_BUS_IS_BUSY                            0x10000

// -- Enable or disable HWACG
#define DIV_CLK_CHUB_BUS_HWACG_DISABLE                      (0 << 28)
#define DIV_CLK_CHUB_BUS_HWACG_ENABLE                       (1 << 28)



// CLK_CON_DIV_DIV_CLK_CHUB_I2C

// -- DIVRATIO
#define CLK_CON_DIV_DIV_CLK_CHUB_I2C_DEFAULT_DIVRATIO       (1 << 0)
#define DIV_CLK_CHUB_I2C_IS_BUSY                            0x10000

// -- Enable or disable HWACG
#define DIV_CLK_CHUB_I2C_HWACG_DISABLE                      (0 << 28)
#define DIV_CLK_CHUB_I2C_HWACG_ENABLE                       (1 << 28)



// CLK_CON_DIV_DIV_CLK_CHUB_USI00

// -- DIVRATIO
#define CLK_CON_DIV_DIV_CLK_CHUB_USI00_DEFAULT_DIVRATIO     (0 << 0)
#define DIV_CLK_CHUB_USI00_IS_BUSY                          0x10000

// -- Enable or disable HWACG
#define DIV_CLK_CHUB_USI00_HWACG_DISABLE                    (0 << 28)
#define DIV_CLK_CHUB_USI00_HWACG_ENABLE                     (1 << 28)



// CLK_CON_DIV_DIV_CLK_CHUB_USI01

// -- DIVRATIO
#define CLK_CON_DIV_DIV_CLK_CHUB_USI01_DEFAULT_DIVRATIO     (0 << 0)
#define DIV_CLK_CHUB_USI01_IS_BUSY                          0x10000

// -- Enable or disable HWACG
#define DIV_CLK_CHUB_USI01_HWACG_DISABLE                    (0 << 28)
#define DIV_CLK_CHUB_USI01_HWACG_ENABLE                     (1 << 28)



// QCH_CON_xxx

// -- ENABLE
#define QCH_CON_xxx_ENABLE                                  (1 << 0)

// -- IGNORE_FORCE_PM_EN
#define QCH_CON_xxx_IGNORE_FORCE_PM_EN                      (1 << 2)


// CLK_CON_xxx

// -- ENABLE_AUTOMATIC_CLKGATING
#define CLK_CON_xxx_ENABLE_AUTOMATIC_CLKGATING              (1 << 28)


// PLL_CON0_MUX_CLKCMU_CMGP_BUS_USER

// -- Select
#define SELECT_OSCCLK_RCO_CMGP                              (0 << 4)
#define SELECT_CLKCMU_CMGP_BUS                              (1 << 4)
#define MUX_CLKCMU_CMGP_BUS_USER_IS_BUSY                    0x80



// PLL_CON0_MUX_CLKCMU_CMGP_DLL_USER

// -- Select
#define SELECT_OSCCLK_RCO_CMGP                              (0 << 4)
#define SELECT_CLKCMU_APM_DLL_CMGP                          (1 << 4)
#define MUX_CLKCMU_CMGP_DLL_USER_IS_BUSY                    0x80


// CLK_CON_DIV_DIV_CLK_I2C_CMGP

#define DIV_CLK_I2C_CMGP_IS_BUSY                            0x10000


// CLK_CON_DIV_DIV_CLK_USI_CMGP00

#define DIV_CLK_USI_CMGP00_IS_BUSY                            0x10000

// CLK_CON_DIV_DIV_CLK_USI_CMGP01

#define DIV_CLK_USI_CMGP01_IS_BUSY                            0x10000

// CLK_CON_DIV_DIV_CLK_USI_CMGP02

#define DIV_CLK_USI_CMGP02_IS_BUSY                            0x10000

// CLK_CON_DIV_DIV_CLK_USI_CMGP03

#define DIV_CLK_USI_CMGP03_IS_BUSY                            0x10000


#endif

