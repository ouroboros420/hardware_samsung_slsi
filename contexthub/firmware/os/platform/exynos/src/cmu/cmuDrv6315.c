/*----------------------------------------------------------------------------
 *      Exynos SoC  -  CMU
 *----------------------------------------------------------------------------
 *      Name:    cmuDrv6315.c
 *      Purpose: To implement CMU driver functions
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

#include <cmu.h>
#include <cmuDrv.h>
#include <sysreg.h>
#include <csp_common.h>

#define DEFAULT_NOC_DIV                 1
#define DEFAULT_I2C_DIV                 2
#define DEFAULT_SPI_I2C0_DIV            2
#define DEFAULT_SPI_I2C1_DIV            2
#define DEFAULT_SPI_MS_CTRL_DIV         2
#define DEFAULT_USI0_DIV                2
#define DEFAULT_USI1_DIV                2
#define DEFAULT_USI2_DIV                2
#define DEFAULT_USI3_DIV                2

static uint32_t tChubMainClk = CHUB_MAIN_CLK;

static inline void cmuDrvSetMuxClkChubNocUser(uint32_t sel) {
    uint32_t regValue;

    regValue = __raw_read32(REG_CMU_PLL_CON0_MUX_CLKALIVE_CHUB_NOC_USER);
    if (sel)
        regValue |= MUX_USER_SEL;
    else
        regValue &= ~MUX_USER_SEL;
    __raw_write32(regValue, REG_CMU_PLL_CON0_MUX_CLKALIVE_CHUB_NOC_USER);

    while(__raw_read32(REG_CMU_PLL_CON0_MUX_CLKALIVE_CHUB_NOC_USER) & MUX_BUSY);
}

static inline void cmuDrvSetMuxClkChubPeriUser(uint32_t sel) {
    uint32_t regValue;

    regValue = __raw_read32(REG_CMU_PLL_CON0_MUX_CLKALIVE_CHUB_PERI_USER);
    if (sel)
        regValue |= MUX_USER_SEL;
    else
        regValue &= ~MUX_USER_SEL;
    __raw_write32(regValue, REG_CMU_PLL_CON0_MUX_CLKALIVE_CHUB_PERI_USER);

    while(__raw_read32(REG_CMU_PLL_CON0_MUX_CLKALIVE_CHUB_PERI_USER) & MUX_BUSY);
}

static inline void cmuDrvSetMuxClkChubRcoUser(uint32_t sel) {
    uint32_t regValue;

    regValue = __raw_read32(REG_CMU_PLL_CON0_MUX_CLKALIVE_CHUB_RCO_USER);
    if (sel)
        regValue |= MUX_USER_SEL;
    else
        regValue &= ~MUX_USER_SEL;
    __raw_write32(regValue, REG_CMU_PLL_CON0_MUX_CLKALIVE_CHUB_RCO_USER);

    while(__raw_read32(REG_CMU_PLL_CON0_MUX_CLKALIVE_CHUB_RCO_USER) & MUX_BUSY);
}

static inline void cmuDrvSetMuxClkChubTimer(uint32_t sel) {
    uint32_t regValue;

    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CHUB_TIMER);
    if (sel)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;
    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CHUB_TIMER);

    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CHUB_TIMER) & MUX_BUSY);
}

static inline void cmuDrvSetMuxClkChubI2c(uint32_t sel) {
    uint32_t regValue;

    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_I2C);
    if (sel)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;
    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_I2C);

    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_I2C) & MUX_BUSY);
}

static inline void cmuDrvSetMuxClkChubSpiI2c0(uint32_t sel) {
    uint32_t regValue;

    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_SPI_I2C0);
    if (sel)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;
    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_SPI_I2C0);

    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_SPI_I2C0) & MUX_BUSY);
}

static inline void cmuDrvSetMuxClkChubSpiI2c1(uint32_t sel) {
    uint32_t regValue;

    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_SPI_I2C1);
    if (sel)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;
    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_SPI_I2C1);

    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_SPI_I2C1) & MUX_BUSY);
}

static inline void cmuDrvSetMuxClkChubSpiMsCtrl(uint32_t sel) {
    uint32_t regValue;

    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_SPI_MS_CTRL);
    if (sel)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;
    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_SPI_MS_CTRL);

    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_SPI_MS_CTRL) & MUX_BUSY);
}

static inline void cmuDrvSetMuxClkChubUsi0(uint32_t sel) {
    uint32_t regValue;

    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI0);
    if (sel)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;
    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI0);

    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI0) & MUX_BUSY);
}

static inline void cmuDrvSetMuxClkChubUsi1(uint32_t sel) {
    uint32_t regValue;

    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI1);
    if (sel)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;
    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI1);

    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI1) & MUX_BUSY);
}

static inline void cmuDrvSetMuxClkChubUsi2(uint32_t sel) {
    uint32_t regValue;

    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI2);
    if (sel)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;
    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI2);

    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI2) & MUX_BUSY);
}

static inline void cmuDrvSetMuxClkChubUsi3(uint32_t sel) {
    uint32_t regValue;

    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI3);
    if (sel)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;
    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI3);

    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI3) & MUX_BUSY);
}

static inline uint32_t cmuDrvGetMuxClkChubNocUser(void) {
    return (__raw_read32(REG_CMU_PLL_CON0_MUX_CLKALIVE_CHUB_NOC_USER) & MUX_USER_SEL)?
        CLKALIVE_CHUB_NOC : OSCCLK_CHUB;
}

static inline uint32_t cmuDrvGetMuxClkChubPeriUser(void) {
    return (__raw_read32(REG_CMU_PLL_CON0_MUX_CLKALIVE_CHUB_PERI_USER) & MUX_USER_SEL)?
        CLKALIVE_CHUB_PERI : OSCCLK_CHUB;
}

static inline uint32_t cmuDrvGetMuxClkChubRcoUser(void) {
    return (__raw_read32(REG_CMU_PLL_CON0_MUX_CLKALIVE_CHUB_RCO_USER) & MUX_USER_SEL)?
        CLKALIVE_CHUB_RCO : OSCCLK_CHUB;
}

static inline uint32_t cmuDrvGetMuxClkChubTimer(void) {
    return (__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CHUB_TIMER) & MUX_SEL)?
        RTCCLK_CHUB : OSCCLK_CHUB;
}

static inline uint32_t cmuDrvGetMuxClkChubI2c(void) {
    return (__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_I2C) & MUX_SEL)?
        cmuDrvGetMuxClkChubPeriUser() : cmuDrvGetMuxClkChubRcoUser();
}

static inline uint32_t cmuDrvGetMuxClkChubSpiI2c0(void) {
    return (__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_SPI_I2C0) & MUX_SEL)?
        cmuDrvGetMuxClkChubPeriUser() : cmuDrvGetMuxClkChubRcoUser();
}

static inline uint32_t cmuDrvGetMuxClkChubSpiI2c1(void) {
    return (__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_SPI_I2C1) & MUX_SEL)?
        cmuDrvGetMuxClkChubPeriUser() : cmuDrvGetMuxClkChubRcoUser();
}

static inline uint32_t cmuDrvGetMuxClkChubSpiMsCtrl(void) {
    return (__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_SPI_MS_CTRL) & MUX_SEL)?
        cmuDrvGetMuxClkChubPeriUser() : cmuDrvGetMuxClkChubRcoUser();
}

static inline uint32_t cmuDrvGetMuxClkChubUsi0(void) {
    return (__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI0) & MUX_SEL)?
        cmuDrvGetMuxClkChubPeriUser() : cmuDrvGetMuxClkChubRcoUser();
}

static inline uint32_t cmuDrvGetMuxClkChubUsi1(void) {
    return (__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI1) & MUX_SEL)?
        cmuDrvGetMuxClkChubPeriUser() : cmuDrvGetMuxClkChubRcoUser();
}

static inline uint32_t cmuDrvGetMuxClkChubUsi2(void) {
    return (__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI2) & MUX_SEL)?
        cmuDrvGetMuxClkChubPeriUser() : cmuDrvGetMuxClkChubRcoUser();
}

static inline uint32_t cmuDrvGetMuxClkChubUsi3(void) {
    return (__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI3) & MUX_SEL)?
        cmuDrvGetMuxClkChubPeriUser() : cmuDrvGetMuxClkChubRcoUser();
}

static inline void cmuDrvSetDivChubI2c(uint32_t div) {
    uint32_t regValue;

    if (div == 0)
        return;
    
    regValue = __raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_I2C);
    regValue &= ~DIV_RATIO_MASK_4BIT;
    regValue |= (div -1) & DIV_RATIO_MASK_4BIT;
    __raw_write32(regValue, REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_I2C);

    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_I2C) & DIV_BUSY);
}

static inline void cmuDrvSetDivChubNoc (uint32_t div) {
    uint32_t regValue;

    if (div == 0)
        return;
    
    regValue = __raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_NOC);
    regValue &= ~DIV_RATIO_MASK_3BIT;
    regValue |= (div -1) & DIV_RATIO_MASK_3BIT;
    __raw_write32(regValue, REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_NOC);

    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_NOC) & DIV_BUSY);
}

static inline void cmuDrvSetDivChubSpiI2c0(uint32_t div) {
    uint32_t regValue;

    if (div == 0)
        return;
    
    regValue = __raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SPI_I2C0);
    regValue &= ~DIV_RATIO_MASK_4BIT;
    regValue |= (div -1) & DIV_RATIO_MASK_4BIT;
    __raw_write32(regValue, REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SPI_I2C0);

    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SPI_I2C0) & DIV_BUSY);
}

static inline void cmuDrvSetDivChubSpiI2c1(uint32_t div) {
    uint32_t regValue;

    if (div == 0)
        return;
    
    regValue = __raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SPI_I2C1);
    regValue &= ~DIV_RATIO_MASK_4BIT;
    regValue |= (div -1) & DIV_RATIO_MASK_4BIT;
    __raw_write32(regValue, REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SPI_I2C1);

    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SPI_I2C1) & DIV_BUSY);
}

static inline void cmuDrvSetDivChubSpiMsCtrl(uint32_t div) {
    uint32_t regValue;

    if (div == 0)
        return;
    
    regValue = __raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SPI_MS_CTRL);
    regValue &= ~DIV_RATIO_MASK_4BIT;
    regValue |= (div -1) & DIV_RATIO_MASK_4BIT;
    __raw_write32(regValue, REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SPI_MS_CTRL);

    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SPI_MS_CTRL) & DIV_BUSY);
}

static inline void cmuDrvSetDivChubUsi0(uint32_t div) {
    uint32_t regValue;

    if (div == 0)
        return;
    
    regValue = __raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI0);
    regValue &= ~DIV_RATIO_MASK_4BIT;
    regValue |= (div -1) & DIV_RATIO_MASK_4BIT;
    __raw_write32(regValue, REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI0);

    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI0) & DIV_BUSY);
}

static inline void cmuDrvSetDivChubUsi1(uint32_t div) {
    uint32_t regValue;

    if (div == 0)
        return;
    
    regValue = __raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI1);
    regValue &= ~DIV_RATIO_MASK_4BIT;
    regValue |= (div -1) & DIV_RATIO_MASK_4BIT;
    __raw_write32(regValue, REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI1);

    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI1) & DIV_BUSY);
}

static inline void cmuDrvSetDivChubUsi2(uint32_t div) {
    uint32_t regValue;

    if (div == 0)
        return;
    
    regValue = __raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI2);
    regValue &= ~DIV_RATIO_MASK_4BIT;
    regValue |= (div -1) & DIV_RATIO_MASK_4BIT;
    __raw_write32(regValue, REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI2);

    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI2) & DIV_BUSY);
}

static inline void cmuDrvSetDivChubUsi3(uint32_t div) {
    uint32_t regValue;

    if (div == 0)
        return;
    
    regValue = __raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI3);
    regValue &= ~DIV_RATIO_MASK_4BIT;
    regValue |= (div -1) & DIV_RATIO_MASK_4BIT;
    __raw_write32(regValue, REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI3);

    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI3) & DIV_BUSY);
}

static inline uint32_t cmuDrvGetDivChubI2c(void) {
    uint32_t regValue;

    regValue = __raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_I2C);
    regValue &= DIV_RATIO_MASK_4BIT;
    
    return regValue + 1;
}

static inline uint32_t cmuDrvGetDivChubNoc(void) {
    uint32_t regValue;

    regValue = __raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_NOC);
    regValue &= DIV_RATIO_MASK_3BIT;
    
    return regValue + 1;
}

static inline uint32_t cmuDrvGetDivChubSpiI2c0(void) {
    uint32_t regValue;

    regValue = __raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SPI_I2C0);
    regValue &= DIV_RATIO_MASK_4BIT;
    
    return regValue + 1;
}

static inline uint32_t cmuDrvGetDivChubSpiI2c1(void) {
    uint32_t regValue;

    regValue = __raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SPI_I2C1);
    regValue &= DIV_RATIO_MASK_4BIT;
    
    return regValue + 1;
}

static inline uint32_t cmuDrvGetDivChubSpiMsCtrl(void) {
    uint32_t regValue;

    regValue = __raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SPI_MS_CTRL);
    regValue &= DIV_RATIO_MASK_4BIT;
    
    return regValue + 1;
}

static inline uint32_t cmuDrvGetDivChubUsi0(void) {
    uint32_t regValue;

    regValue = __raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI0);
    regValue &= DIV_RATIO_MASK_4BIT;
    
    return regValue + 1;
}

static inline uint32_t cmuDrvGetDivChubUsi1(void) {
    uint32_t regValue;

    regValue = __raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI1);
    regValue &= DIV_RATIO_MASK_4BIT;
    
    return regValue + 1;
}

static inline uint32_t cmuDrvGetDivChubUsi2(void) {
    uint32_t regValue;

    regValue = __raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI2);
    regValue &= DIV_RATIO_MASK_4BIT;
    
    return regValue + 1;
}

static inline uint32_t cmuDrvGetDivChubUsi3(void) {
    uint32_t regValue;

    regValue = __raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI3);
    regValue &= DIV_RATIO_MASK_4BIT;
    
    return regValue + 1;
}


void cmuDrvInit(uint32_t mainclk)
{
    uint32_t val = 0;

    if (mainclk)
        tChubMainClk = mainclk;

    // Initializes mux and divider
    // !!! Make sure that clock source for CHUB, CLKCMU_CHUB_BUS_USER is 393Mhz and provided at this initializing time

    // PLL_CON1_MUX_CLKALIVE_CHUB_NOC_USER
    // Select CLK_RCO_CHUB : 49.152 MHz
    cmuDrvSetMuxClkChubNocUser(0);
    // CLK_CON_DIV_DIV_CLK_CHUB_NOC
    cmuDrvSetDivChubNoc(DEFAULT_NOC_DIV);

    // CHUB_CONTROLLER_OPTION
    val = 0;
    val |= (CHUB_CONTROLLER_OPTION_DEBUG_ENABLE |
            CHUB_CONTROLLER_OPTION_LAYER2_CTRL_ENABLE |
            CHUB_CONTROLLER_OPTION_PM_ENABLE |
            CHUB_CONTROLLER_OPTION_HWACG_ENABLE |
            CHUB_CONTROLLER_OPTION_MEMPG_ENABLE);
    __raw_write32(val, REG_CMU_CHUB_CMU_CHUB_CONTROLLER_OPTION);

    // MUX_CLKCMU_CHUB_BUS_USER
    // Select CLKCMU_CHUB_BUS : 393.216 MHz
    cmuDrvSetMuxClkChubPeriUser(0);

    // MUX_CLKCMU_CHUB_PERI_USER
    // Select CLKCMU_CHUB_PERI : 393.216 MHz
    cmuDrvSetMuxClkChubRcoUser(0);

    // CLK_CON_MUX_MUX_CHUB_TIMER
    cmuDrvSetMuxClkChubTimer(0);

    // CLK_CON_MUX_MUX_CLK_CHUB_I2C
    cmuDrvSetMuxClkChubI2c(0);
    // CLK_CON_DIV_DIV_CLK_CHUB_I2C
    cmuDrvSetDivChubI2c(DEFAULT_I2C_DIV);

    // CLK_CON_MUX_MUX_CLK_CHUB_SPI_I2C0
    cmuDrvSetMuxClkChubSpiI2c0(0);
    // CLK_CON_DIV_DIV_CLK_CHUB_SPI_I2C0
    cmuDrvSetDivChubSpiI2c0(DEFAULT_SPI_I2C0_DIV);

    // CLK_CON_MUX_MUX_CLK_CHUB_SPI_I2C1
    cmuDrvSetMuxClkChubSpiI2c1(0);
    // CLK_CON_DIV_DIV_CLK_CHUB_SPI_I2C1
    cmuDrvSetDivChubSpiI2c1(DEFAULT_SPI_I2C1_DIV);

    // CLK_CON_MUX_MUX_CLK_CHUB_SPI_MS_CTRL
    cmuDrvSetMuxClkChubSpiMsCtrl(0);
    // CLK_CON_DIV_DIV_CLK_CHUB_SPI_MS_CTRL
    cmuDrvSetDivChubSpiMsCtrl(DEFAULT_SPI_MS_CTRL_DIV);

    // CLK_CON_MUX_MUX_CLK_CHUB_USI0
    cmuDrvSetMuxClkChubUsi0(0);
    // CLK_CON_DIV_DIV_CLK_CHUB_USI0
    cmuDrvSetDivChubUsi0(DEFAULT_USI0_DIV);

    // CLK_CON_MUX_MUX_CLK_CHUB_USI1
    cmuDrvSetMuxClkChubUsi1(0);
    // CLK_CON_DIV_DIV_CLK_CHUB_USI1
    cmuDrvSetDivChubUsi1(DEFAULT_USI1_DIV);

    // CLK_CON_MUX_MUX_CLK_CHUB_USI2
    cmuDrvSetMuxClkChubUsi2(0);
    // CLK_CON_DIV_DIV_CLK_CHUB_USI2
    cmuDrvSetDivChubUsi2(DEFAULT_USI2_DIV);

    // CLK_CON_MUX_MUX_CLK_CHUB_USI3
    cmuDrvSetMuxClkChubUsi3(0);
    // CLK_CON_DIV_DIV_CLK_CHUB_USI3
    cmuDrvSetDivChubUsi3(DEFAULT_USI3_DIV);

    CSP_PRINTF_INFO("CMU: NocU(%dM), RcoU(%dM), PeriU(%dM)\n",
                    (int)(cmuDrvGetMuxClkChubNocUser()/1000000),
                    (int)(cmuDrvGetMuxClkChubRcoUser()/1000000),
                    (int)(cmuDrvGetMuxClkChubPeriUser()/1000000));
    CSP_PRINTF_INFO("CMU: I(%dM), SI0(%dM), SI1(%dM), T(%dM)\n",
                    (int)(cmuDrvGetMuxClkChubI2c()/1000000),
                    (int)(cmuDrvGetMuxClkChubSpiI2c0()/1000000),
                    (int)(cmuDrvGetMuxClkChubSpiI2c1()/1000000),
                    (int)(cmuDrvGetMuxClkChubTimer()/1000000));
    CSP_PRINTF_INFO("CMU: U0(%dM), U1(%dM), U2(%dM), U3(%dM)\n",
                    (int)(cmuDrvGetMuxClkChubUsi0()/1000000),
                    (int)(cmuDrvGetMuxClkChubUsi1()/1000000),
                    (int)(cmuDrvGetMuxClkChubUsi2()/1000000),
                    (int)(cmuDrvGetMuxClkChubUsi3()/1000000));
}

/*
void cmuDrvCMGPWarning(const char* func)
{
    CSP_PRINTF_ERROR("%s : not recommand to use CMGP\n", func);
}

static uint32_t cmuDrvGetMuxClkCmgpI2C(void)
{
    cmuDrvCMGPWarning(__func__);
    return 0;
}

static uint32_t cmuDrvGetMuxClkCmgpUSI00(void)
{
    cmuDrvCMGPWarning(__func__);
    return 0;
}

static uint32_t cmuDrvGetMuxClkCmgpUSI01(void)
{
    cmuDrvCMGPWarning(__func__);
    return 0;
}

static uint32_t cmuDrvGetMuxClkCmgpUSI02(void)
{
    cmuDrvCMGPWarning(__func__);
    return 0;
}

static void cmuDrvSetDivCmgpI2C(IN uint32_t divider)
{
    cmuDrvCMGPWarning(__func__);
}

static void cmuDrvSetDivCmgpUSI00(IN uint32_t divider)
{
    cmuDrvCMGPWarning(__func__);
}

static void cmuDrvSetDivCmgpUSI01(IN uint32_t divider)
{
    cmuDrvCMGPWarning(__func__);
}

static void cmuDrvSetDivCmgpUSI02(IN uint32_t divider)
{
    cmuDrvCMGPWarning(__func__);
}
*/

int cmuDrvSetCpuClk(uint32_t freq)
{
    uint32_t div;

    if (freq > tChubMainClk)
        return -1;

    div = 1;
    while ((tChubMainClk / div) > freq) {
        div++;
        if ( div > 0x8)
            return -1;
    }
    CSP_PRINTF_INFO("cmuDrvSetCpuClk div%lu\n", div);
    cmuDrvSetDivider(CMU_CLK_CHUB_BUS, div);

    return 0;
}

//
uint32_t cmuDrvGetSpeed(IN CmuIpType ip)
{
    switch(ip) {
        case CMU_CLK_NOC:
            return CLKALIVE_CHUB_NOC;
        break;

        case CMU_CLK_PERI:
            return CLKALIVE_CHUB_PERI;
        break;

        case CMU_CLK_RCO:
            return CLKALIVE_CHUB_RCO;
        break;

        case CMU_CLK_OSCCLK:
            return OSCCLK_CHUB;
        break;

        case CMU_CLK_RTCCLK:
            return RTCCLK_CHUB;
        break;

        case CMU_CLK_CHUB_BUS:
        case CMU_CLK_OUTPUT_CPU:
            return cmuDrvGetMuxClkChubNocUser() / cmuDrvGetDivChubNoc();
        break;

        case CMU_CLK_TIMER:
            return cmuDrvGetMuxClkChubTimer();
        break;

        case CMU_CLK_CHUB_SPI_MS_CTRL:
            return cmuDrvGetMuxClkChubSpiMsCtrl() / cmuDrvGetDivChubSpiMsCtrl();
            break;

        case CMU_CLK_CHUB_I2C:
        case CMU_CLK_I2C04:
        case CMU_CLK_I2C07:
            return cmuDrvGetMuxClkChubI2c() / cmuDrvGetDivChubI2c();
        break;

        case CMU_CLK_CHUB_SPI_I2C0:
        case CMU_CLK_I2C05:
            return cmuDrvGetMuxClkChubSpiI2c0() / cmuDrvGetDivChubSpiI2c0();
        break;

        case CMU_CLK_CHUB_SPI_I2C1:
        case CMU_CLK_I2C03:
            return cmuDrvGetMuxClkChubSpiI2c1() / cmuDrvGetDivChubSpiI2c1();
        break;

        case CMU_CLK_CHUB_USI00:
        case CMU_CLK_I2C00:
            return cmuDrvGetMuxClkChubUsi0() / cmuDrvGetDivChubUsi0();
        break;

        case CMU_CLK_CHUB_USI01:
        case CMU_CLK_I2C01:
            return cmuDrvGetMuxClkChubUsi1() / cmuDrvGetDivChubUsi1();
        break;

        case CMU_CLK_CHUB_USI02:
        case CMU_CLK_I2C02:
            return cmuDrvGetMuxClkChubUsi2() / cmuDrvGetDivChubUsi2();
        break;

        case CMU_CLK_CHUB_USI03:
        case CMU_CLK_I2C06:
            return cmuDrvGetMuxClkChubUsi3() / cmuDrvGetDivChubUsi3();
        break;

        default:
            CSP_ASSERT(0);
        break;
    }

    return 0;
}

bool cmuDrvSetDivider(IN CmuIpType ip, IN uint32_t divider)
{
    switch(ip) {
        case CMU_CLK_CHUB_BUS:
            cmuDrvSetDivChubNoc(divider);
        break;

        case CMU_CLK_CHUB_I2C:
            cmuDrvSetDivChubI2c(divider);
        break;

        case CMU_CLK_CHUB_USI00:
            cmuDrvSetDivChubUsi0(divider);
        break;

        case CMU_CLK_CHUB_USI01:
            cmuDrvSetDivChubUsi1(divider);
        break;

        case CMU_CLK_CHUB_USI02:
            cmuDrvSetDivChubUsi2(divider);
        break;

        case CMU_CLK_CHUB_USI03:
            cmuDrvSetDivChubUsi3(divider);
        break;

        case CMU_CLK_CHUB_SPI_I2C0:
            cmuDrvSetDivChubSpiI2c0(divider);
        break;

        case CMU_CLK_CHUB_SPI_I2C1:
            cmuDrvSetDivChubSpiI2c1(divider);
        break;

        case CMU_CLK_CHUB_SPI_MS_CTRL:
            cmuDrvSetDivChubSpiMsCtrl(divider);
        break;

        default:
            CSP_ASSERT(0);
            return false;
        break;
    }

    return true;
}

static bool isDrcgHwacgSet = 0;

//
bool cmuDrvControlHwacg(IN CmuHwacgControlType enable)
{
    // Dynamic Root Clock Gating can be set once at boot time
    if(isDrcgHwacgSet) {
        CSP_ASSERT(0);
        return false;
    }

    uint32_t val = 0;

    // CHUB_CMU_CHUB_CONTROLLER_OPTION in CMU_CHUB
    if(enable) {
        val = __raw_read32(REG_CMU_CHUB_CMU_CHUB_CONTROLLER_OPTION);
        val |= CHUB_CONTROLLER_OPTION_PM_ENABLE;
        val |= CHUB_CONTROLLER_OPTION_HWACG_ENABLE;
        val |= CHUB_CONTROLLER_OPTION_MEMPG_ENABLE;
        __raw_write32(val, REG_CMU_CHUB_CMU_CHUB_CONTROLLER_OPTION);
    }
    else {
        // Disable HWACG and power management
        val = __raw_read32(REG_CMU_CHUB_CMU_CHUB_CONTROLLER_OPTION);;
        val &= ~CHUB_CONTROLLER_OPTION_PM_ENABLE;
        val &= ~CHUB_CONTROLLER_OPTION_HWACG_ENABLE;
        val &= ~CHUB_CONTROLLER_OPTION_MEMPG_ENABLE;
        __raw_write32(val, REG_CMU_CHUB_CMU_CHUB_CONTROLLER_OPTION);
    }

    // BUS_COMPONENT_DRCG_EN in SYSREG_CHUB
    sysregSetDRCG(enable);

    isDrcgHwacgSet = 1;

    return true;
}

bool cmuDrvControlHwacgIP(IN CmuHwacgIpType ip, IN CmuHwacgControlType enable)
{
    return true;
}

// Need to increase if more SFRs need to be kept. Currently it is 27 SFRs of CMU
//static uint32_t buf_CMUSFR[E9815_CMUSFR_NUM];
void cmuDrvSaveState(void)
{
#if 1
    CSP_PRINTF_INFO("[CMU] cmuDrvSaveState NOT ready! \n");
#else
    uint32_t idx = 0;

    // SFRs set by cmdDrvInit
    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_PLL_CON0_MUX_CLKCMU_CHUB_BUS_USER);
    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_I2C);
    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI00);
    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_I2C);
    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI00);

    // SFR set by cmuDrvSetDivCmgpI2C
    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_I2C_CMGP);

    // SFR set by cmuDrvSetDivCmgpUSI00
    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_USI_CMGP00);

    // SFR set by cmuDrvSetDivCmgpUSI01
    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_USI_CMGP01);

    // SFR set by cmuDrvSetDivCmgpUSI02
    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_USI_CMGP02);

    // SFR set by cmuDrvSetDivCmgpUSI03
    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_USI_CMGP03);

    // SFR set by cmuDrvControlHwacg
    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_CHUB_CMU_CHUB_CONTROLLER_OPTION);

    // SFR set by sysregDrvSetHWACG
    //buf_CMUSFR[idx++] = __raw_read32(REG_SYSREG_BUS_COMPONENET_DRCG_EN);

    // SFR set by cmuDrvControlHwacgIP
    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_QCH_CON_CM4_CHUB_QCH);

    // SFR set by cmuDrvControlHwacgIP
    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_CM4_CHUB_IPCLKPORT_FCLK);

    // SFR set by cmuDrvControlHwacgIP
    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_QCH_CON_GPIO_CHUB_QCH);

    // SFR set by cmuDrvControlHwacgIP
    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_GPIO_CHUB_IPCLKPORT_PCLK);

    // SFR set by cmuDrvControlHwacgIP
    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_QCH_CON_I2C_CHUB00_QCH);

    // SFR set by cmuDrvControlHwacgIP
    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_I2C_CHUB00_IPCLKPORT_IPCLK);

    // SFR set by cmuDrvControlHwacgIP
    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_I2C_CHUB00_IPCLKPORT_PCLK);

    // SFR set by cmuDrvControlHwacgIP
    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_I2C_CHUB00_IPCLKPORT_PCLK);

    // SFR set by cmuDrvControlHwacgIP
    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_PDMA_CHUB_IPCLKPORT_ACLK);

    // SFR set by cmuDrvControlHwacgIP
    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_PWM_CHUB_IPCLKPORT_I_PCLK_S0);

    // SFR set by cmuDrvControlHwacgIP
    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_TIMER_CHUB_IPCLKPORT_PCLK);

    // SFR set by cmuDrvControlHwacgIP
    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_USI_CHUB00_IPCLKPORT_IPCLK);

    // SFR set by cmuDrvControlHwacgIP
    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_USI_CHUB00_IPCLKPORT_PCLK);

    // SFR set by cmuDrvControlHwacgIP
    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_QCH_CON_WDT_CHUB_QCH);

    // SFR set by cmuDrvControlHwacgIP
    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_WDT_CHUB_IPCLKPORT_PCLK);

    CSP_PRINTF_INFO("[CMU] %d SFRs saved\n", (int)idx);
#endif
}

void cmuDrvRestoreState(void)
{
#if 1
    CSP_PRINTF_INFO("[CMU] cmuDrvRestoreState NOT ready! \n");
#else
    uint32_t idx = 0;

    // The seuqence to restore should match with cmuDrvSaveState !!!

    __raw_write32(buf_CMUSFR[idx++], REG_CMU_PLL_CON0_MUX_CLKCMU_CHUB_BUS_USER);
    // Wait while mux is changing
    while(__raw_read32(REG_CMU_PLL_CON0_MUX_CLKCMU_CHUB_BUS_USER) & MUX_CLKCMU_CHUB_BUS_USER_IS_BUSY);

    __raw_write32(buf_CMUSFR[idx++], REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_BUS);
    // Wait while mux is changing
    while(__raw_read32(REG_CMU_PLL_CON0_MUX_CLKCMU_CHUB_BUS_USER) & MUX_CLKCMU_CHUB_BUS_USER_IS_BUSY);

    __raw_write32(buf_CMUSFR[idx++], REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_I2C);
    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_I2C) & MUX_CLK_CHUB_I2C_IS_BUSY);

    __raw_write32(buf_CMUSFR[idx++], REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_TIMER);
    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_I2C) & MUX_CLK_CHUB_I2C_IS_BUSY);

    __raw_write32(buf_CMUSFR[idx++], REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI00);
    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI00) & MUX_CLK_CHUB_USI00_IS_BUSY);

    __raw_write32(buf_CMUSFR[idx++], REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_I2C);
    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_I2C) & DIV_CLK_CHUB_I2C_IS_BUSY);

    __raw_write32(buf_CMUSFR[idx++], REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI00);
    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI00) & DIV_CLK_CHUB_USI00_IS_BUSY);

    // SFR set by cmuDrvSetDivCmgpI2C
    __raw_write32(buf_CMUSFR[idx++], REG_CMU_CLK_CON_DIV_DIV_CLK_I2C_CMGP);
    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_I2C_CMGP) & DIV_CLK_I2C_CMGP_IS_BUSY);

    // SFR set by cmuDrvSetDivCmgpUSI00
    __raw_write32(buf_CMUSFR[idx++], REG_CMU_CLK_CON_DIV_DIV_CLK_USI_CMGP00);
    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_USI_CMGP00) & DIV_CLK_USI_CMGP00_IS_BUSY);


    // SFR set by cmuDrvSetDivCmgpUSI01
    __raw_write32(buf_CMUSFR[idx++], REG_CMU_CLK_CON_DIV_DIV_CLK_USI_CMGP01);
    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_USI_CMGP01) & DIV_CLK_USI_CMGP01_IS_BUSY);

    // SFR set by cmuDrvSetDivCmgpUSI02
    __raw_write32(buf_CMUSFR[idx++], REG_CMU_CLK_CON_DIV_DIV_CLK_USI_CMGP02);
    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_USI_CMGP02) & DIV_CLK_USI_CMGP02_IS_BUSY);

    // SFR set by cmuDrvSetDivCmgpUSI03
    __raw_write32(buf_CMUSFR[idx++], REG_CMU_CLK_CON_DIV_DIV_CLK_USI_CMGP03);
    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_USI_CMGP03) & DIV_CLK_USI_CMGP03_IS_BUSY);

    // SFR set by cmuDrvControlHwacg
    __raw_write32(buf_CMUSFR[idx++], REG_CMU_CHUB_CMU_CHUB_CONTROLLER_OPTION);

    // SFR set by sysregDrvSetHWACG
    //buf_CMUSFR[idx++] = __raw_read32(REG_SYSREG_BUS_COMPONENET_DRCG_EN);

    // SFR set by cmuDrvControlHwacgIP
    __raw_write32(buf_CMUSFR[idx++], REG_CMU_QCH_CON_CM4_CHUB_QCH);

    // SFR set by cmuDrvControlHwacgIP
    __raw_write32(buf_CMUSFR[idx++], REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_CM4_CHUB_IPCLKPORT_FCLK);

    // SFR set by cmuDrvControlHwacgIP
    __raw_write32(buf_CMUSFR[idx++], REG_CMU_QCH_CON_GPIO_CHUB_QCH);

    // SFR set by cmuDrvControlHwacgIP
    __raw_write32(buf_CMUSFR[idx++], REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_GPIO_CHUB_IPCLKPORT_PCLK);

    // SFR set by cmuDrvControlHwacgIP
    __raw_write32(buf_CMUSFR[idx++], REG_CMU_QCH_CON_I2C_CHUB00_QCH);

    // SFR set by cmuDrvControlHwacgIP
    __raw_write32(buf_CMUSFR[idx++], REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_I2C_CHUB00_IPCLKPORT_IPCLK);

    // SFR set by cmuDrvControlHwacgIP
    __raw_write32(buf_CMUSFR[idx++], REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_I2C_CHUB00_IPCLKPORT_PCLK);

    // SFR set by cmuDrvControlHwacgIP
    __raw_write32(buf_CMUSFR[idx++], REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_I2C_CHUB00_IPCLKPORT_PCLK);

    // SFR set by cmuDrvControlHwacgIP
    __raw_write32(buf_CMUSFR[idx++], REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_PDMA_CHUB_IPCLKPORT_ACLK);

    // SFR set by cmuDrvControlHwacgIP
    __raw_write32(buf_CMUSFR[idx++], REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_PWM_CHUB_IPCLKPORT_I_PCLK_S0);

    // SFR set by cmuDrvControlHwacgIP
    __raw_write32(buf_CMUSFR[idx++], REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_TIMER_CHUB_IPCLKPORT_PCLK);

    // SFR set by cmuDrvControlHwacgIP
    __raw_write32(buf_CMUSFR[idx++], REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_USI_CHUB00_IPCLKPORT_IPCLK);

    // SFR set by cmuDrvControlHwacgIP
    __raw_write32(buf_CMUSFR[idx++], REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_USI_CHUB00_IPCLKPORT_PCLK);

    // SFR set by cmuDrvControlHwacgIP
    __raw_write32(buf_CMUSFR[idx++], REG_CMU_QCH_CON_WDT_CHUB_QCH);

    // SFR set by cmuDrvControlHwacgIP
    __raw_write32(buf_CMUSFR[idx++], REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_WDT_CHUB_IPCLKPORT_PCLK);

    CSP_PRINTF_INFO("[CMU] %d SFRs are restored\n", (int)idx);

#endif
}

