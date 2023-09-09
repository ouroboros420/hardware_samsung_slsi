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

#include <cmu.h>
#include <cmuDrv.h>
#include <sysreg.h>
#include <csp_common.h>
#include <dvfs/dvfs.h>
#include <util.h>

static uint32_t cmuDrvGetDivValue(uint32_t inFreq, uint32_t target, uint32_t maxDiv);

static uint32_t cmuDrvGetSpeedMuxChubNocUser(void);
static uint32_t cmuDrvGetSpeedMuxChubPeriUser(void);
static uint32_t cmuDrvGetSpeedMuxChubRcoUser(void);
static uint32_t cmuDrvGetSpeedMuxChubUsi0(void);
static uint32_t cmuDrvGetSpeedMuxChubUsi1(void);
static uint32_t cmuDrvGetSpeedMuxChubUsi2(void);
static uint32_t cmuDrvGetSpeedMuxChubUsi3(void);
static uint32_t cmuDrvGetSpeedMuxChubI2c(void);
static uint32_t cmuDrvGetSpeedMuxChubSpiMsCtrl(void);
static uint32_t cmuDrvGetSpeedMuxChubSpiI2c0(void);
static uint32_t cmuDrvGetSpeedMuxChubSpiI2c1(void);
static uint32_t cmuDrvGetSpeedMuxChubTimer(void);

static void cmuDrvSetMuxClkAliveChubNocUser(int);
static void cmuDrvSetMuxClkAliveChubPeriUser(int);
static void cmuDrvSetMuxClkAliveChubRcoUser(int);
static void cmuDrvSetMuxClkChubNoc(int);
static void cmuDrvSetMuxClkChubUSI0(int);
static void cmuDrvSetMuxClkChubUSI1(int);
static void cmuDrvSetMuxClkChubUSI2(int);
static void cmuDrvSetMuxClkChubUSI3(int);
static void cmuDrvSetMuxClkChubI2C(int);
static void cmuDrvSetMuxClkChubSPI_MS_CTRL(int);
static void cmuDrvSetMuxClkChubSPII2C0(int);
static void cmuDrvSetMuxClkChubSPII2C1(int);
static void cmuDrvSetMuxClkChubTimer(int);

static void cmuDrvSetDivChubNoc(IN uint32_t divider);
static void cmuDrvSetDivChubUSI0(IN uint32_t divider);
static void cmuDrvSetDivChubUSI1(IN uint32_t divider);
static void cmuDrvSetDivChubUSI2(IN uint32_t divider);
static void cmuDrvSetDivChubUSI3(IN uint32_t divider);
static void cmuDrvSetDivChubI2C(IN uint32_t divider);
static void cmuDrvSetDivChubSPI_MS_CTRL(IN uint32_t divider);
static void cmuDrvSetDivChubSPII2C0(IN uint32_t divider);
static void cmuDrvSetDivChubSPII2C1(IN uint32_t divider);
static void cmuDrvSetDivChubSlifUsProx(IN uint32_t divider);
static void cmuDrvSetDivChubSlifUsProxCore(IN uint32_t divider);

static uint32_t cmuDrvGetDivChubNoc(void);
static uint32_t cmuDrvGetDivChubUsi0(void);
static uint32_t cmuDrvGetDivChubUsi1(void);
static uint32_t cmuDrvGetDivChubUsi2(void);
static uint32_t cmuDrvGetDivChubUsi3(void);
static uint32_t cmuDrvGetDivChubI2c(void);
static uint32_t cmuDrvGetDivChubSpiMsCtrl(void);
static uint32_t cmuDrvGetDivChubSpiI2c0(void);
static uint32_t cmuDrvGetDivChubSpiI2c1(void);
static uint32_t cmuDrvGetDivChubSlifUsProx(void);
static uint32_t cmuDrvGetDivChubSlifUsProxCore(void);

static uint32_t cmuDrvGetSpeedMuxCmgpI2c(void);
static uint32_t cmuDrvGetSpeedMuxCmgpSpiI2c0(void);
static uint32_t cmuDrvGetSpeedMuxCmgpSpiI2c1(void);
static uint32_t cmuDrvGetSpeedMuxCmgpSpiMsCtrl(void);
static uint32_t cmuDrvGetSpeedMuxCmgpUsi0(void);
static uint32_t cmuDrvGetSpeedMuxCmgpUsi1(void);
static uint32_t cmuDrvGetSpeedMuxCmgpUsi2(void);
static uint32_t cmuDrvGetSpeedMuxCmgpUsi3(void);
static uint32_t cmuDrvGetSpeedMuxCmgpUsi4(void);
static uint32_t cmuDrvGetSpeedMuxCmgpUsi5(void);
static uint32_t cmuDrvGetSpeedMuxCmgpUsi6(void);
static void cmuDrvSetMuxClkCmgpI2C(int);
static void cmuDrvSetMuxClkCmgpSPI_I2C0(int);
static void cmuDrvSetMuxClkCmgpSPI_I2C1(int);
static void cmuDrvSetMuxClkCmgpSPI_MS_CTRL(int);
static void cmuDrvSetMuxClkCmgpUSI0(int);
static void cmuDrvSetMuxClkCmgpUSI1(int);
static void cmuDrvSetMuxClkCmgpUSI2(int);
static void cmuDrvSetMuxClkCmgpUSI3(int);
static void cmuDrvSetMuxClkCmgpUSI4(int);
static void cmuDrvSetMuxClkCmgpUSI5(int);
static void cmuDrvSetMuxClkCmgpUSI6(int);
static void cmuDrvSetDivCmgpI2C(IN uint32_t divider);
static void cmuDrvSetDivCmgpSPI_I2C0(IN uint32_t divider);
static void cmuDrvSetDivCmgpSPI_I2C1(IN uint32_t divider);
static void cmuDrvSetDivCmgpSPI_MS_CTRL(IN uint32_t divider);
static void cmuDrvSetDivCmgpUSI0(IN uint32_t divider);
static void cmuDrvSetDivCmgpUSI1(IN uint32_t divider);
static void cmuDrvSetDivCmgpUSI2(IN uint32_t divider);
static void cmuDrvSetDivCmgpUSI3(IN uint32_t divider);
static void cmuDrvSetDivCmgpUSI4(IN uint32_t divider);
static void cmuDrvSetDivCmgpUSI5(IN uint32_t divider);
static void cmuDrvSetDivCmgpUSI6(IN uint32_t divider);
static uint32_t cmuDrvGetDivCmgpI2c(void);
static uint32_t cmuDrvGetDivCmgpSpiI2c0(void);
static uint32_t cmuDrvGetDivCmgpSpiI2c1(void);
static uint32_t cmuDrvGetDivCmgpSpiMsCtrl(void);
static uint32_t cmuDrvGetDivCmgpUsi0(void);
static uint32_t cmuDrvGetDivCmgpUsi1(void);
static uint32_t cmuDrvGetDivCmgpUsi2(void);
static uint32_t cmuDrvGetDivCmgpUsi3(void);
static uint32_t cmuDrvGetDivCmgpUsi4(void);
static uint32_t cmuDrvGetDivCmgpUsi5(void);
static uint32_t cmuDrvGetDivCmgpUsi6(void);

#define DEFAULT_BUS_DIV                1
#define DEFAULT_I2C_DIV                2
#define DEFAULT_USI_DIV                2

struct sfrData {
    uint32_t addr;
    uint32_t val;
};

static uint32_t tChubMainClk = CHUB_MAIN_CLK;

static struct sfrData mCmuSfrData[] = {
    {REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_NOC, 0},
    {REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_I2C, 0},
    {REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SERIAL_LIF_US_PROX, 0},
    {REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SERIAL_LIF_US_PROX_CORE, 0},
    {REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SPI_I2C0, 0},
    {REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SPI_I2C1, 0},
    {REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SPI_MS_CTRL, 0},
    {REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI0, 0},
    {REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI1, 0},
    {REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI2, 0},
    {REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI3, 0},

    {REG_CMU_PLL_CON0_MUX_CLKALIVE_CHUB_NOC_USER, 0},
    {REG_CMU_PLL_CON0_MUX_CLKALIVE_CHUB_PERI_USER, 0},
    {REG_CMU_PLL_CON0_MUX_CLKALIVE_CHUB_RCO_USER, 0},
    {REG_CMU_CHUB_CMU_CHUB_CONTROLLER_OPTION, 0},

    {REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_TIMER, 0},
    {REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_I2C, 0},
    {REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_NOC, 0},
    {REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_SPI_I2C0, 0},
    {REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_SPI_I2C1, 0},
    {REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_SPI_MS_CTRL, 0},
    {REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI0, 0},
    {REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI1, 0},
    {REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI2, 0},
    {REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI3, 0},
};

static uint32_t cmuDrvGetDivValue(uint32_t inFreq, uint32_t target, uint32_t maxDiv)
{
    uint32_t div;

    for (div = 1 ; div <= maxDiv ; div++)
    {
        if ((inFreq / div) <= target)
            break;
    }

    return div;
}

void cmuDrvInit(uint32_t mainClk)
{
    uint32_t val = 0;

    //  Set MUX for CHUB BUS and CPU
    // Select CLKCMU_CHUB_BUS : 393.216 MHz
    cmuDrvSetMuxClkAliveChubNocUser(CLKALIVE_CHUBVTS_NOC);
    cmuDrvSetMuxClkAliveChubPeriUser(CLKALIVE_CHUB_PERI);
    cmuDrvSetMuxClkAliveChubRcoUser(CLKALIVE_CHUBVTS_RCO);
    cmuDrvSetMuxClkChubNoc(MUX_CLKALIVE_CHUB_NOC_USER);

#if defined(ALIVE_DVS_SUPPORT)
    dvfsRequestFreq(CHUB_MAIN_CLK);
#else
    cmuDrvSetCpuClk(CHUB_MAIN_CLK);
#endif

    // CHUB_CONTROLLER_OPTION
    val = 0;
    val |= (CHUB_CONTROLLER_OPTION_DEBUG_ENABLE |
            CHUB_CONTROLLER_OPTION_LAYER2_CTRL_ENABLE |
            CHUB_CONTROLLER_OPTION_PM_ENABLE |
            CHUB_CONTROLLER_OPTION_HWACG_ENABLE |
            CHUB_CONTROLLER_OPTION_MEMPG_ENABLE);
    __raw_write32(val, REG_CMU_CHUB_CMU_CHUB_CONTROLLER_OPTION);

    // MUX_CLK_CHUB_TIMER_FCLK
    cmuDrvSetMuxClkChubTimer(RTCCLK_CHUB);

    // Set MUX & DIV for CHUB_USI0~3
    cmuDrvSetMuxClkChubUSI0(MUX_CLKALIVE_CHUB_PERI_USER);
    cmuDrvSetDivChubUSI0(DEFAULT_USI_DIV);

    cmuDrvSetMuxClkChubUSI1(MUX_CLKALIVE_CHUB_PERI_USER);
    cmuDrvSetDivChubUSI1(DEFAULT_USI_DIV);

    cmuDrvSetMuxClkChubUSI2(MUX_CLKALIVE_CHUB_PERI_USER);
    cmuDrvSetDivChubUSI2(DEFAULT_USI_DIV);

    cmuDrvSetMuxClkChubUSI3(MUX_CLKALIVE_CHUB_PERI_USER);
    cmuDrvSetDivChubUSI3(DEFAULT_USI_DIV);

    // Set MUX & DIV for CHUB_I2C
    cmuDrvSetMuxClkChubI2C(MUX_CLKALIVE_CHUB_PERI_USER);
    cmuDrvSetDivChubI2C(DEFAULT_I2C_DIV);

    // Set MUX & DIV for CHUB_SPI_I2C
    cmuDrvSetMuxClkChubSPII2C0(MUX_CLKALIVE_CHUB_PERI_USER);
    cmuDrvSetDivChubSPII2C0(DEFAULT_I2C_DIV);

    cmuDrvSetMuxClkChubSPII2C1(MUX_CLKALIVE_CHUB_PERI_USER);
    cmuDrvSetDivChubSPII2C1(DEFAULT_I2C_DIV);

    cmuDrvSetDivChubSlifUsProx(DEFAULT_BUS_DIV);
    cmuDrvSetDivChubSlifUsProxCore(DEFAULT_BUS_DIV);
}

static void cmuDrvSetMuxClkAliveChubNocUser(int select)
{
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_PLL_CON0_MUX_CLKALIVE_CHUB_NOC_USER);

    if (select)
        regValue |= MUX_USER_SEL;
    else
        regValue &= ~MUX_USER_SEL;

    __raw_write32(regValue, REG_CMU_PLL_CON0_MUX_CLKALIVE_CHUB_NOC_USER);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_PLL_CON0_MUX_CLKALIVE_CHUB_NOC_USER) & MUX_BUSY);
}

static void cmuDrvSetMuxClkAliveChubPeriUser(int select)
{
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_PLL_CON0_MUX_CLKALIVE_CHUB_PERI_USER);

    if (select)
        regValue |= MUX_USER_SEL;
    else
        regValue &= ~MUX_USER_SEL;

    __raw_write32(regValue, REG_CMU_PLL_CON0_MUX_CLKALIVE_CHUB_PERI_USER);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_PLL_CON0_MUX_CLKALIVE_CHUB_PERI_USER) & MUX_BUSY);
}

static void cmuDrvSetMuxClkAliveChubRcoUser(int select)
{
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_PLL_CON0_MUX_CLKALIVE_CHUB_RCO_USER);

    if (select)
        regValue |= MUX_USER_SEL;
    else
        regValue &= ~MUX_USER_SEL;

    __raw_write32(regValue, REG_CMU_PLL_CON0_MUX_CLKALIVE_CHUB_RCO_USER);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_PLL_CON0_MUX_CLKALIVE_CHUB_RCO_USER) & MUX_BUSY);
}

static void cmuDrvSetMuxClkChubNoc(int select)
{
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_NOC);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_NOC);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_NOC) & MUX_BUSY);
}

static void cmuDrvSetMuxClkChubUSI0(int select)
{
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI0);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI0);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI0) & MUX_BUSY);
}

static void cmuDrvSetMuxClkChubUSI1(int select)
{
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI1);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI1);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI1) & MUX_BUSY);
}

static void cmuDrvSetMuxClkChubUSI2(int select)
{
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI2);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI2);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI2) & MUX_BUSY);
}

static void cmuDrvSetMuxClkChubUSI3(int select)
{
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI3);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI3);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI3) & MUX_BUSY);
}

static void cmuDrvSetMuxClkChubI2C(int select)
{
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_I2C);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_I2C);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_I2C) & MUX_BUSY);
}

static void cmuDrvSetMuxClkChubSPII2C0(int select)
{
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_SPI_I2C0);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_SPI_I2C0);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_SPI_I2C0) & MUX_BUSY);
}

static void cmuDrvSetMuxClkChubSPII2C1(int select)
{
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_SPI_I2C1);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_SPI_I2C1);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_SPI_I2C1) & MUX_BUSY);
}

static void cmuDrvSetMuxClkChubTimer(int select)
{
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_TIMER);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_TIMER);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_TIMER) & MUX_BUSY);
}

#if defined(SEOS)
static void __attribute__((unused)) cmuDrvSetMuxClkChubSPI_MS_CTRL(int select) {
#else
static void cmuDrvSetMuxClkChubSPI_MS_CTRL(int select) {
#endif
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_SPI_MS_CTRL);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_SPI_MS_CTRL);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_SPI_MS_CTRL) & MUX_BUSY);
}

static uint32_t cmuDrvGetSpeedMuxChubNocUser(void)
{
    return (__raw_read32(REG_CMU_PLL_CON0_MUX_CLKALIVE_CHUB_NOC_USER) & MUX_USER_SEL)? RCO_400_FREQ : CLK_RCO_FREQ;
}

static uint32_t cmuDrvGetSpeedMuxChubPeriUser(void)
{
    return (__raw_read32(REG_CMU_PLL_CON0_MUX_CLKALIVE_CHUB_PERI_USER) & MUX_USER_SEL)? RCO_400_FREQ : CLK_RCO_FREQ;
}

static uint32_t cmuDrvGetSpeedMuxChubRcoUser(void)
{
    return (__raw_read32(REG_CMU_PLL_CON0_MUX_CLKALIVE_CHUB_RCO_USER) & MUX_USER_SEL)? OSCCLK_RCO_FREQ : CLK_RCO_FREQ;
}

static uint32_t cmuDrvGetSpeedMuxChubNoc(void)
{
    uint32_t speed = 0;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_NOC) & MUX_SEL)
        speed = cmuDrvGetSpeedMuxChubRcoUser();
    else
        speed = cmuDrvGetSpeedMuxChubNocUser();

    return speed;
}

static uint32_t cmuDrvGetSpeedMuxChubUsi0(void)
{
    uint32_t speed = 0;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI0) & MUX_SEL)
        speed = cmuDrvGetSpeedMuxChubPeriUser();
    else
        speed = cmuDrvGetSpeedMuxChubRcoUser();

    return speed;
}

static uint32_t cmuDrvGetSpeedMuxChubUsi1(void)
{
    uint32_t speed = 0;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI1) & MUX_SEL)
        speed = cmuDrvGetSpeedMuxChubPeriUser();
    else
        speed = cmuDrvGetSpeedMuxChubRcoUser();

    return speed;
}

static uint32_t cmuDrvGetSpeedMuxChubUsi2(void)
{
    uint32_t speed = 0;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI2) & MUX_SEL)
        speed = cmuDrvGetSpeedMuxChubPeriUser();
    else
        speed = cmuDrvGetSpeedMuxChubRcoUser();

    return speed;
}

static uint32_t cmuDrvGetSpeedMuxChubUsi3(void)
{
    uint32_t speed = 0;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI3) & MUX_SEL)
        speed = cmuDrvGetSpeedMuxChubPeriUser();
    else
        speed = cmuDrvGetSpeedMuxChubRcoUser();

    return speed;
}

static uint32_t cmuDrvGetSpeedMuxChubI2c(void)
{
    uint32_t speed = 0;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_I2C) & MUX_SEL)
        speed = cmuDrvGetSpeedMuxChubPeriUser();
    else
        speed = cmuDrvGetSpeedMuxChubRcoUser();

    return speed;
}

/*
static uint32_t cmuDrvGetSpeedMuxChubVtsNocUser(void)
{
    return (__raw_read32(REG_CMU_PLL_CON0_MUX_CLKALIVE_CHUBVTS_NOC_USER) & MUX_USER_SEL)? RCO_400_FREQ : CLK_RCO_FREQ;
}

static uint32_t cmuDrvGetSpeedMuxChubVtsRcoUser(void)
{
    return (__raw_read32(REG_CMU_PLL_CON0_MUX_CLKALIVE_CHUBVTS_RCO_USER) & MUX_USER_SEL)? RCO_400_FREQ : CLK_RCO_FREQ;
}
*/

static uint32_t cmuDrvGetSpeedMuxChubSpiMsCtrl(void)
{
    uint32_t speed = 0;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_SPI_MS_CTRL) & MUX_SEL)
        speed = cmuDrvGetSpeedMuxChubPeriUser();
    else
        speed = cmuDrvGetSpeedMuxChubRcoUser();

    return speed;
}

static uint32_t cmuDrvGetSpeedMuxChubSpiI2c0(void)
{
    uint32_t speed = 0;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_SPI_I2C0) & MUX_SEL)
        speed = cmuDrvGetSpeedMuxChubPeriUser();
    else
        speed = cmuDrvGetSpeedMuxChubRcoUser();

    return speed;
}

static uint32_t cmuDrvGetSpeedMuxChubSpiI2c1(void)
{
    uint32_t speed = 0;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_SPI_I2C1) & MUX_SEL)
        speed = cmuDrvGetSpeedMuxChubPeriUser();
    else
        speed = cmuDrvGetSpeedMuxChubRcoUser();

    return speed;
}

static uint32_t cmuDrvGetSpeedMuxChubTimer(void)
{
    uint32_t speed = 0;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_TIMER) & MUX_SEL)
        speed = RTC_FREQ;
    else
        speed = OSCCLK_RCO_FREQ;

    return speed;
}

static void cmuDrvSetDivChubNoc(IN uint32_t divider)
{
    if (divider == 0)
        return;

    // divider value is clear and set to given divider value
    divider = (divider - 1) & MASK_3BITS;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_NOC) & ~MASK_3BITS) | divider), REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_NOC);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_NOC) & DIV_BUSY);
}

static void cmuDrvSetDivChubUSI0(IN uint32_t divider)
{
    if (divider == 0)
        return;
    // divider value is clear and set to given divider value
    divider = (divider - 1) & MASK_4BITS;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI0) & ~MASK_4BITS) | divider), REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI0);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI0) & DIV_BUSY);
}

static void cmuDrvSetDivChubUSI1(IN uint32_t divider)
{
    if (divider == 0)
        return;
    // divider value is clear and set to given divider value
    divider = (divider - 1) & MASK_4BITS;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI1) & ~MASK_4BITS) | divider), REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI1);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI1) & DIV_BUSY);
}

static void cmuDrvSetDivChubUSI2(IN uint32_t divider)
{
    if (divider == 0)
        return;
    // divider value is clear and set to given divider value
    divider = (divider - 1) & MASK_4BITS;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI2) & ~MASK_4BITS) | divider), REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI2);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI2) & DIV_BUSY);
}

static void cmuDrvSetDivChubUSI3(IN uint32_t divider)
{
    if (divider == 0)
        return;
    // divider value is clear and set to given divider value
    divider = (divider - 1) & MASK_4BITS;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI3) & ~MASK_4BITS) | divider), REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI3);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI3) & DIV_BUSY);
}

static void cmuDrvSetDivChubI2C(IN uint32_t divider)
{
    if (divider == 0)
        return;
    // divider value is clear and set to given divider value
    divider = (divider - 1) & MASK_4BITS;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_I2C) & ~MASK_4BITS) | divider), REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_I2C);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_I2C) & DIV_BUSY);
}

static void cmuDrvSetDivChubSPI_MS_CTRL(IN uint32_t divider)
{
    if (divider == 0)
        return;
    // divider value is clear and set to given divider value
    divider = (divider - 1) & MASK_4BITS;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SPI_MS_CTRL) & ~MASK_4BITS) | divider), REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SPI_MS_CTRL);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SPI_MS_CTRL) & DIV_BUSY);
}

static void cmuDrvSetDivChubSPII2C0(IN uint32_t divider)
{
    if (divider == 0)
        return;
    // divider value is clear and set to given divider value
    divider = (divider - 1) & MASK_4BITS;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SPI_I2C0) & ~MASK_4BITS) | divider), REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SPI_I2C0);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SPI_I2C0) & DIV_BUSY);
}

static void cmuDrvSetDivChubSPII2C1(IN uint32_t divider)
{
    if (divider == 0)
        return;
    // divider value is clear and set to given divider value
    divider = (divider - 1) & MASK_4BITS;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SPI_I2C1) & ~MASK_4BITS) | divider), REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SPI_I2C1);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SPI_I2C1) & DIV_BUSY);
}

static void cmuDrvSetDivChubSlifUsProx(IN uint32_t divider)
{
    if (divider == 0)
        return;
    // divider value is clear and set to given divider value
    divider = (divider - 1) & MASK_7BITS;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SERIAL_LIF_US_PROX) & ~MASK_7BITS) | divider), REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SERIAL_LIF_US_PROX);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SERIAL_LIF_US_PROX) & DIV_BUSY);
}

static void cmuDrvSetDivChubSlifUsProxCore(IN uint32_t divider)
{
    if (divider == 0)
        return;
    // divider value is clear and set to given divider value
    divider = (divider - 1) & MASK_7BITS;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SERIAL_LIF_US_PROX_CORE) & ~MASK_7BITS) | divider), REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SERIAL_LIF_US_PROX_CORE);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SERIAL_LIF_US_PROX_CORE) & DIV_BUSY);
}

static uint32_t cmuDrvGetDivChubNoc(void)
{
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_NOC) & MASK_3BITS) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivChubUsi0(void)
{
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI0) & MASK_4BITS) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivChubUsi1(void)
{
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI1) & MASK_4BITS) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivChubUsi2(void)
{
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI2) & MASK_4BITS) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivChubUsi3(void)
{
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI3) & MASK_4BITS) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivChubI2c(void)
{
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_I2C) & MASK_4BITS) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivChubSpiMsCtrl(void)
{
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SPI_MS_CTRL) & MASK_4BITS) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivChubSpiI2c0(void)
{
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SPI_I2C0) & MASK_4BITS) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivChubSpiI2c1(void)
{
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SPI_I2C1) & MASK_4BITS) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivChubSlifUsProx(void)
{
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SERIAL_LIF_US_PROX) & MASK_7BITS) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivChubSlifUsProxCore(void)
{
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SERIAL_LIF_US_PROX_CORE) & MASK_7BITS) + 1;

    return divider;
}

#if defined(SEOS)
static void __attribute__((unused)) cmuDrvSetMuxClkCmgpI2C(int select) {
#else
static void cmuDrvSetMuxClkCmgpI2C(int select) {
#endif
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_I2C);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_I2C);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_I2C) & MUX_BUSY);
}

#if defined(SEOS)
static void __attribute__((unused)) cmuDrvSetMuxClkCmgpSPI_I2C0(int select) {
#else
static void cmuDrvSetMuxClkCmgpSPI_I2C0(int select) {
#endif
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_SPI_I2C0);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_SPI_I2C0);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_SPI_I2C0) & MUX_BUSY);
}

#if defined(SEOS)
static void __attribute__((unused)) cmuDrvSetMuxClkCmgpSPI_I2C1(int select) {
#else
static void cmuDrvSetMuxClkCmgpSPI_I2C1(int select) {
#endif
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_SPI_I2C1);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_SPI_I2C1);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_SPI_I2C1) & MUX_BUSY);
}

#if defined(SEOS)
static void __attribute__((unused)) cmuDrvSetMuxClkCmgpSPI_MS_CTRL(int select) {
#else
static void cmuDrvSetMuxClkCmgpSPI_MS_CTRL(int select) {
#endif
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_SPI_MS_CTRL);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_SPI_MS_CTRL);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_SPI_MS_CTRL) & MUX_BUSY);
}

#if defined(SEOS)
static void __attribute__((unused)) cmuDrvSetMuxClkCmgpUSI0(int select) {
#else
static void cmuDrvSetMuxClkCmgpUSI0(int select) {
#endif
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI0);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI0);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI0) & MUX_BUSY);
}

#if defined(SEOS)
static void __attribute__((unused)) cmuDrvSetMuxClkCmgpUSI1(int select) {
#else
static void cmuDrvSetMuxClkCmgpUSI1(int select) {
#endif
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI1);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI1);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI1) & MUX_BUSY);
}

#if defined(SEOS)
static void __attribute__((unused)) cmuDrvSetMuxClkCmgpUSI2(int select) {
#else
static void cmuDrvSetMuxClkCmgpUSI2(int select) {
#endif
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI2);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI2);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI2) & MUX_BUSY);
}

#if defined(SEOS)
static void __attribute__((unused)) cmuDrvSetMuxClkCmgpUSI3(int select) {
#else
static void cmuDrvSetMuxClkCmgpUSI3(int select) {
#endif
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI3);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI3);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI3) & MUX_BUSY);
}


#if defined(SEOS)
static void __attribute__((unused)) cmuDrvSetMuxClkCmgpUSI4(int select) {
#else
static void cmuDrvSetMuxClkCmgpUSI4(int select) {
#endif
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI4);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI4);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI4) & MUX_BUSY);
}


#if defined(SEOS)
static void __attribute__((unused)) cmuDrvSetMuxClkCmgpUSI5(int select) {
#else
static void cmuDrvSetMuxClkCmgpUSI5(int select) {
#endif
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI5);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI5);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI5) & MUX_BUSY);
}


#if defined(SEOS)
static void __attribute__((unused)) cmuDrvSetMuxClkCmgpUSI6(int select) {
#else
static void cmuDrvSetMuxClkCmgpUSI6(int select) {
#endif
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI6);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI6);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI6) & MUX_BUSY);
}

#if defined(SEOS)
static uint32_t __attribute__((unused)) cmuDrvGetSpeedMuxCmgpI2c(void) {
#else
static uint32_t cmuDrvGetSpeedMuxCmgpI2c(void) {
#endif
    uint32_t speed;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_I2C) & MUX_SEL)
        speed = RCO_400_FREQ;
    else
        speed = CLK_RCO_FREQ;

    return speed;
}

#if defined(SEOS)
static uint32_t __attribute__((unused)) cmuDrvGetSpeedMuxCmgpSpiI2c0(void) {
#else
static uint32_t cmuDrvGetSpeedMuxCmgpSpiI2c0(void) {
#endif
    uint32_t speed;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_SPI_I2C0) & MUX_SEL)
        speed = RCO_400_FREQ;
    else
        speed = CLK_RCO_FREQ;

    return speed;
}

#if defined(SEOS)
static uint32_t __attribute__((unused)) cmuDrvGetSpeedMuxCmgpSpiI2c1(void) {
#else
static uint32_t cmuDrvGetSpeedMuxCmgpSpiI2c1(void) {
#endif
    uint32_t speed;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_SPI_I2C1) & MUX_SEL)
        speed = RCO_400_FREQ;
    else
        speed = CLK_RCO_FREQ;

    return speed;
}

#if defined(SEOS)
static uint32_t __attribute__((unused)) cmuDrvGetSpeedMuxCmgpSpiMsCtrl(void) {
#else
static uint32_t cmuDrvGetSpeedMuxCmgpSpiMsCtrl(void) {
#endif
    uint32_t speed;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_SPI_MS_CTRL) & MUX_SEL)
        speed = RCO_400_FREQ;
    else
        speed = CLK_RCO_FREQ;

    return speed;
}

#if defined(SEOS)
static uint32_t __attribute__((unused)) cmuDrvGetSpeedMuxCmgpUsi0(void) {
#else
static uint32_t cmuDrvGetSpeedMuxCmgpUsi0(void) {
#endif
    uint32_t speed;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI0) & MUX_SEL)
        speed = RCO_400_FREQ;
    else
        speed = CLK_RCO_FREQ;

    return speed;
}

#if defined(SEOS)
static uint32_t __attribute__((unused)) cmuDrvGetSpeedMuxCmgpUsi1(void) {
#else
static uint32_t cmuDrvGetSpeedMuxCmgpUsi1(void) {
#endif
    uint32_t speed;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI1) & MUX_SEL)
        speed = RCO_400_FREQ;
    else
        speed = CLK_RCO_FREQ;

    return speed;
}

#if defined(SEOS)
static uint32_t __attribute__((unused)) cmuDrvGetSpeedMuxCmgpUsi2(void) {
#else
static uint32_t cmuDrvGetSpeedMuxCmgpUsi2(void) {
#endif
    uint32_t speed;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI2) & MUX_SEL)
        speed = RCO_400_FREQ;
    else
        speed = CLK_RCO_FREQ;

    return speed;
}

#if defined(SEOS)
static uint32_t __attribute__((unused)) cmuDrvGetSpeedMuxCmgpUsi3(void) {
#else
static uint32_t cmuDrvGetSpeedMuxCmgpUsi3(void) {
#endif
    uint32_t speed;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI3) & MUX_SEL)
        speed = RCO_400_FREQ;
    else
        speed = CLK_RCO_FREQ;

    return speed;
}

#if defined(SEOS)
static uint32_t __attribute__((unused)) cmuDrvGetSpeedMuxCmgpUsi4(void) {
#else
static uint32_t cmuDrvGetSpeedMuxCmgpUsi4(void) {
#endif
    uint32_t speed;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI4) & MUX_SEL)
        speed = RCO_400_FREQ;
    else
        speed = CLK_RCO_FREQ;

    return speed;
}

#if defined(SEOS)
static uint32_t __attribute__((unused)) cmuDrvGetSpeedMuxCmgpUsi5(void) {
#else
static uint32_t cmuDrvGetSpeedMuxCmgpUsi5(void) {
#endif
    uint32_t speed;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI5) & MUX_SEL)
        speed = RCO_400_FREQ;
    else
        speed = CLK_RCO_FREQ;

    return speed;
}

#if defined(SEOS)
static uint32_t __attribute__((unused)) cmuDrvGetSpeedMuxCmgpUsi6(void) {
#else
static uint32_t cmuDrvGetSpeedMuxCmgpUsi6(void) {
#endif
    uint32_t speed;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI6) & MUX_SEL)
        speed = RCO_400_FREQ;
    else
        speed = CLK_RCO_FREQ;

    return speed;
}

#if defined(SEOS)
static void __attribute__((unused)) cmuDrvSetDivCmgpI2C(IN uint32_t divider) {
#else
static void cmuDrvSetDivCmgpI2C(IN uint32_t divider) {
#endif
    if (divider == 0)
        return;

    // divider value is clear and set to given divider value
    divider = (divider - 1) & MASK_4BITS;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C) & ~MASK_4BITS) | divider),
                  REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C) & DIV_BUSY);
}

#if defined(SEOS)
static void __attribute__((unused)) cmuDrvSetDivCmgpSPI_I2C0(IN uint32_t divider) {
#else
static void cmuDrvSetDivCmgpSPI_I2C0(IN uint32_t divider) {
#endif
    if (divider == 0)
        return;

    // divider value is clear and set to given divider value
    divider = (divider - 1) & MASK_4BITS;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_SPI_I2C0) & ~MASK_4BITS) | divider),
                  REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_SPI_I2C0);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_SPI_I2C0) & DIV_BUSY);
}

#if defined(SEOS)
static void __attribute__((unused)) cmuDrvSetDivCmgpSPI_I2C1(IN uint32_t divider) {
#else
static void cmuDrvSetDivCmgpSPI_I2C1(IN uint32_t divider) {
#endif
    if (divider == 0)
        return;

    // divider value is clear and set to given divider value
    divider = (divider - 1) & MASK_4BITS;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_SPI_I2C1) & ~MASK_4BITS) | divider),
                  REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_SPI_I2C1);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_SPI_I2C1) & DIV_BUSY);
}

#if defined(SEOS)
static void __attribute__((unused)) cmuDrvSetDivCmgpSPI_MS_CTRL(IN uint32_t divider) {
#else
static void cmuDrvSetDivCmgpSPI_MS_CTRL(IN uint32_t divider) {
#endif
    if (divider == 0)
        return;

    // divider value is clear and set to given divider value
    divider = (divider - 1) & MASK_4BITS;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_SPI_MS_CTRL) & ~MASK_4BITS) | divider),
                  REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_SPI_MS_CTRL);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_SPI_MS_CTRL) & DIV_BUSY);
}

#if defined(SEOS)
static void __attribute__((unused)) cmuDrvSetDivCmgpUSI0(IN uint32_t divider) {
#else
static void cmuDrvSetDivCmgpUSI0(IN uint32_t divider) {
#endif
    if (divider == 0)
        return;

    // divider value is clear and set to given divider value
    divider = (divider - 1) & MASK_5BITS;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI0) & ~MASK_4BITS) | divider),
                  REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI0);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI0) & DIV_BUSY);
}

#if defined(SEOS)
static void __attribute__((unused)) cmuDrvSetDivCmgpUSI1(IN uint32_t divider) {
#else
static void cmuDrvSetDivCmgpUSI1(IN uint32_t divider) {
#endif
    if (divider == 0)
        return;

    // divider value is clear and set to given divider value
    divider = (divider - 1) & MASK_5BITS;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI1) & ~MASK_4BITS) | divider),
                  REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI1);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI1) & DIV_BUSY);
}

#if defined(SEOS)
static void __attribute__((unused)) cmuDrvSetDivCmgpUSI2(IN uint32_t divider) {
#else
static void cmuDrvSetDivCmgpUSI2(IN uint32_t divider) {
#endif
    if (divider == 0)
        return;

    // divider value is clear and set to given divider value
    divider = (divider - 1) & MASK_5BITS;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI2) & ~MASK_4BITS) | divider),
                  REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI2);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI2) & DIV_BUSY);
}

#if defined(SEOS)
static void __attribute__((unused)) cmuDrvSetDivCmgpUSI3(IN uint32_t divider) {
#else
static void cmuDrvSetDivCmgpUSI3(IN uint32_t divider) {
#endif
    if (divider == 0)
        return;

    // divider value is clear and set to given divider value
    divider = (divider - 1) & MASK_5BITS;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI3) & ~MASK_4BITS) | divider),
                  REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI3);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI3) & DIV_BUSY);
}

#if defined(SEOS)
static void __attribute__((unused)) cmuDrvSetDivCmgpUSI4(IN uint32_t divider) {
#else
static void cmuDrvSetDivCmgpUSI4(IN uint32_t divider) {
#endif
    if (divider == 0)
        return;

    // divider value is clear and set to given divider value
    divider = (divider - 1) & MASK_5BITS;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI4) & ~MASK_4BITS) | divider),
                  REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI4);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI4) & DIV_BUSY);
}

#if defined(SEOS)
static void __attribute__((unused)) cmuDrvSetDivCmgpUSI5(IN uint32_t divider) {
#else
static void cmuDrvSetDivCmgpUSI5(IN uint32_t divider) {
#endif
    if (divider == 0)
        return;

    // divider value is clear and set to given divider value
    divider = (divider - 1) & MASK_5BITS;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI5) & ~MASK_4BITS) | divider),
                  REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI5);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI5) & DIV_BUSY);
}

#if defined(SEOS)
static void __attribute__((unused)) cmuDrvSetDivCmgpUSI6(IN uint32_t divider) {
#else
static void cmuDrvSetDivCmgpUSI6(IN uint32_t divider) {
#endif
    if (divider == 0)
        return;

    // divider value is clear and set to given divider value
    divider = (divider - 1) & MASK_5BITS;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI6) & ~MASK_4BITS) | divider),
                  REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI6);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI6) & DIV_BUSY);
}

static uint32_t cmuDrvGetDivCmgpI2c(void) {
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C) & MASK_4BITS) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivCmgpSpiI2c0(void) {
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_SPI_I2C0) & MASK_4BITS) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivCmgpSpiI2c1(void) {
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_SPI_I2C1) & MASK_4BITS) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivCmgpSpiMsCtrl(void) {
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_SPI_MS_CTRL) & MASK_4BITS) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivCmgpUsi0(void) {
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI0) & MASK_4BITS) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivCmgpUsi1(void) {
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI1) & MASK_4BITS) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivCmgpUsi2(void) {
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI2) & MASK_4BITS) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivCmgpUsi3(void) {
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI3) & MASK_4BITS) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivCmgpUsi4(void) {
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI4) & MASK_4BITS) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivCmgpUsi5(void) {
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI5) & MASK_4BITS) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivCmgpUsi6(void) {
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI6) & MASK_4BITS) + 1;

    return divider;
}

uint32_t cmuDrvSetCpuMux(uint32_t mux)
{
    uint32_t prev;

    prev = __raw_read32(REG_CMU_PLL_CON0_MUX_CLKALIVE_CHUB_NOC_USER) & MUX_USER_SEL;
    if (mux == CPU_MUX_LOW)
        cmuDrvSetMuxClkAliveChubNocUser(FREE_OSCCLK_CHUB);
    else
        cmuDrvSetMuxClkAliveChubNocUser(CLKALIVE_CHUBVTS_NOC);

    return prev;
}

int cmuDrvSetCpuClk(uint32_t freq)
{
    uint32_t div;


    if (freq <= CLK_RCO_FREQ) {
        // Set Input Clock : CMU_FREE_OSCCLK_RCO_CHUB (49152000)
        cmuDrvSetMuxClkAliveChubNocUser(FREE_OSCCLK_CHUB);
        div = cmuDrvGetDivValue(CLK_RCO_FREQ, freq, 8);
    } else {
        // Set Input Clock : HFRCO_CHUB (393216000)
        cmuDrvSetMuxClkAliveChubNocUser(CLKALIVE_CHUBVTS_NOC);
        div = cmuDrvGetDivValue(RCO_400_FREQ, freq, 8);
    }
    cmuDrvSetDivChubNoc(div);
    tChubMainClk = cmuDrvGetSpeed(CMU_CLK_OUTPUT_CPU);

    CSP_PRINTF_INFO("cmuDrvSetCpuClk div%lu, freq:%lu\n", div, tChubMainClk);
    return 0;
}

//
uint32_t cmuDrvGetSpeed(IN CmuIpType ip)
{
    switch(ip) {
        case CMU_MUX_CHUB_NOC_USER:
            return cmuDrvGetSpeedMuxChubNocUser();
        case CMU_MUX_CHUB_PERI_USER:
            return cmuDrvGetSpeedMuxChubPeriUser();
        case CMU_MUX_CHUB_RCO_USER:
            return cmuDrvGetSpeedMuxChubRcoUser();
        case CMU_MUX_CHUB_TIMER:
            return cmuDrvGetSpeedMuxChubTimer();
        case CMU_MUX_CHUB_NOC:
            return cmuDrvGetSpeedMuxChubNoc();
        case CMU_MUX_CHUB_I2C:
            return cmuDrvGetSpeedMuxChubI2c();
        case CMU_MUX_CHUB_SPI_I2C0:
            return cmuDrvGetSpeedMuxChubSpiI2c0();
        case CMU_MUX_CHUB_SPI_I2C1:
            return cmuDrvGetSpeedMuxChubSpiI2c1();
        case CMU_MUX_CHUB_SPI_MS_CTRL:
            return cmuDrvGetSpeedMuxChubSpiMsCtrl();
        case CMU_MUX_CHUB_USI0:
        case CMU_CLK_CHUB_SPI0:
            return cmuDrvGetSpeedMuxChubUsi0();
        case CMU_MUX_CHUB_USI1:
        case CMU_CLK_CHUB_SPI1:
            return cmuDrvGetSpeedMuxChubUsi1();
        case CMU_MUX_CHUB_USI2:
        case CMU_CLK_CHUB_SPI2:
            return cmuDrvGetSpeedMuxChubUsi2();
        case CMU_MUX_CHUB_USI3:
        case CMU_CLK_CHUB_SPI3:
            return cmuDrvGetSpeedMuxChubUsi3();

        case CMU_DIV_CHUB_NOC:
        case CMU_CLK_OUTPUT_CPU:
            return cmuDrvGetSpeedMuxChubNoc() / cmuDrvGetDivChubNoc();
        case CMU_DIV_CHUB_SPI_MS_CTRL:
            return cmuDrvGetSpeedMuxChubSpiMsCtrl() / cmuDrvGetDivChubSpiMsCtrl();
        case CMU_DIV_CHUB_USI0:
        case CMU_CLK_CHUB_I2C0:
            return cmuDrvGetSpeedMuxChubUsi0() / cmuDrvGetDivChubUsi0();
        case CMU_DIV_CHUB_SPI_I2C0:
        case CMU_CLK_CHUB_I2C1:
            return cmuDrvGetSpeedMuxChubSpiI2c0() / cmuDrvGetDivChubSpiI2c0();
        case CMU_DIV_CHUB_USI1:
        case CMU_CLK_CHUB_I2C2:
            return cmuDrvGetSpeedMuxChubUsi1() / cmuDrvGetDivChubUsi1();
        case CMU_DIV_CHUB_SPI_I2C1:
        case CMU_CLK_CHUB_I2C3:
            return cmuDrvGetSpeedMuxChubSpiI2c1() / cmuDrvGetDivChubSpiI2c1();
        case CMU_DIV_CHUB_USI2:
        case CMU_CLK_CHUB_I2C4:
            return cmuDrvGetSpeedMuxChubUsi2() / cmuDrvGetDivChubUsi2();
        case CMU_DIV_CHUB_USI3:
        case CMU_CLK_CHUB_I2C5:
            return cmuDrvGetSpeedMuxChubUsi3() / cmuDrvGetDivChubUsi3();
        case CMU_DIV_CHUB_I2C:
        case CMU_CLK_CHUB_I2C6:
        case CMU_CLK_CHUB_I3C0:
            return cmuDrvGetSpeedMuxChubI2c() / cmuDrvGetDivChubI2c();
        case CMU_DIV_CHUB_SLIF_PROX:
            return cmuDrvGetSpeedMuxChubRcoUser() / cmuDrvGetDivChubSlifUsProx();
        case CMU_DIV_CHUB_SLIF_PROX_CORE:
            return cmuDrvGetSpeedMuxChubRcoUser() / cmuDrvGetDivChubSlifUsProxCore();

        case CMU_MUX_CMGP_I2C:
            return cmuDrvGetSpeedMuxCmgpI2c();
        case CMU_MUX_CMGP_SPI_I2C0:
            return cmuDrvGetSpeedMuxCmgpSpiI2c0();
        case CMU_MUX_CMGP_SPI_I2C1:
            return cmuDrvGetSpeedMuxCmgpSpiI2c1();
        case CMU_MUX_CMGP_SPI_MS_CTRL:
            return cmuDrvGetSpeedMuxCmgpSpiMsCtrl();
        case CMU_MUX_CMGP_USI0:
        case CMU_CLK_CMGP_SPI0:
            return cmuDrvGetSpeedMuxCmgpUsi0();
        case CMU_MUX_CMGP_USI1:
        case CMU_CLK_CMGP_SPI1:
            return cmuDrvGetSpeedMuxCmgpUsi1();
        case CMU_MUX_CMGP_USI2:
        case CMU_CLK_CMGP_SPI2:
            return cmuDrvGetSpeedMuxCmgpUsi2();
        case CMU_MUX_CMGP_USI3:
        case CMU_CLK_CMGP_SPI3:
            return cmuDrvGetSpeedMuxCmgpUsi3();
        case CMU_MUX_CMGP_USI4:
        case CMU_CLK_CMGP_SPI4:
            return cmuDrvGetSpeedMuxCmgpUsi4();
        case CMU_MUX_CMGP_USI5:
        case CMU_CLK_CMGP_SPI5:
            return cmuDrvGetSpeedMuxCmgpUsi5();
        case CMU_MUX_CMGP_USI6:
        case CMU_CLK_CMGP_SPI6:
            return cmuDrvGetSpeedMuxCmgpUsi6();

        case CMU_DIV_CMGP_I2C:
        case CMU_CLK_CMGP_I2C5:
        case CMU_CLK_CMGP_I2C7:
        case CMU_CLK_CMGP_I2C9:
        case CMU_CLK_CMGP_I2C11:
        case CMU_CLK_CMGP_I2C13:
            return cmuDrvGetSpeedMuxCmgpI2c() / cmuDrvGetDivCmgpI2c();
        case CMU_DIV_CMGP_SPI_I2C0:
        case CMU_CLK_CMGP_I2C1:
            return cmuDrvGetSpeedMuxCmgpSpiI2c0() / cmuDrvGetDivCmgpSpiI2c0();
        case CMU_DIV_CMGP_SPI_I2C1:
        case CMU_CLK_CMGP_I2C3:
            return cmuDrvGetSpeedMuxCmgpSpiI2c1() / cmuDrvGetDivCmgpSpiI2c1();
        case CMU_DIV_CMGP_SPI_MS_CTRL:
            return cmuDrvGetSpeedMuxCmgpSpiMsCtrl() / cmuDrvGetDivCmgpSpiMsCtrl();
        case CMU_DIV_CMGP_USI0:
        case CMU_CLK_CMGP_I2C0:
            return cmuDrvGetSpeedMuxCmgpUsi0() / cmuDrvGetDivCmgpUsi0();
        case CMU_DIV_CMGP_USI1:
        case CMU_CLK_CMGP_I2C2:
            return cmuDrvGetSpeedMuxCmgpUsi1() / cmuDrvGetDivCmgpUsi1();
        case CMU_DIV_CMGP_USI2:
        case CMU_CLK_CMGP_I2C4:
            return cmuDrvGetSpeedMuxCmgpUsi2() / cmuDrvGetDivCmgpUsi2();
        case CMU_DIV_CMGP_USI3:
        case CMU_CLK_CMGP_I2C6:
            return cmuDrvGetSpeedMuxCmgpUsi3() / cmuDrvGetDivCmgpUsi3();
        case CMU_DIV_CMGP_USI4:
        case CMU_CLK_CMGP_I2C8:
            return cmuDrvGetSpeedMuxCmgpUsi4() / cmuDrvGetDivCmgpUsi4();
        case CMU_DIV_CMGP_USI5:
        case CMU_CLK_CMGP_I2C10:
            return cmuDrvGetSpeedMuxCmgpUsi5() / cmuDrvGetDivCmgpUsi5();
        case CMU_DIV_CMGP_USI6:
        case CMU_CLK_CMGP_I2C12:
            return cmuDrvGetSpeedMuxCmgpUsi6() / cmuDrvGetDivCmgpUsi6();

        default:
            break;
    }

    return 0;
}

bool cmuDrvSetDivider(IN CmuIpType ip, IN uint32_t divider)
{
    switch(ip) {
        case CMU_DIV_CHUB_NOC:
        case CMU_CLK_OUTPUT_CPU:
        case CMU_CLK_CHUB_BUS:
                cmuDrvSetDivChubNoc(divider);
            break;

        case CMU_DIV_CHUB_USI0:
        case CMU_CLK_CHUB_SPI0:
                cmuDrvSetDivChubUSI0(divider);
            break;

        case CMU_DIV_CHUB_USI1:
        case CMU_CLK_CHUB_SPI1:
                cmuDrvSetDivChubUSI1(divider);
            break;

        case CMU_DIV_CHUB_USI2:
        case CMU_CLK_CHUB_SPI2:
                cmuDrvSetDivChubUSI2(divider);
            break;

        case CMU_DIV_CHUB_USI3:
        case CMU_CLK_CHUB_SPI3:
                cmuDrvSetDivChubUSI3(divider);
            break;

        case CMU_DIV_CHUB_I2C:
        case CMU_CLK_CHUB_I3C0:
                cmuDrvSetDivChubI2C(divider);
            break;

        case CMU_DIV_CHUB_SPI_MS_CTRL:
                cmuDrvSetDivChubSPI_MS_CTRL(divider);
            break;

        case CMU_DIV_CHUB_SPI_I2C0:
                cmuDrvSetDivChubSPII2C0(divider);
            break;

        case CMU_DIV_CHUB_SPI_I2C1:
                cmuDrvSetDivChubSPII2C1(divider);
            break;

        default:
            return false;
    }

    return true;
}

static bool isDrcgHwacgSet = 0;

//FIXME
bool cmuDrvControlHwacg(IN CmuHwacgControlType enable)
{
    // Dynamic Root Clock Gating can be set once at boot time
    if(isDrcgHwacgSet && enable) {
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

    isDrcgHwacgSet = enable;

    return true;
}

bool cmuDrvControlHwacgIP(IN CmuHwacgIpType ip, IN CmuHwacgControlType enable)
{
    return true;
}

void cmuDrvSaveState(void)
{
    uint32_t idx = 0;
    uint32_t size;

    size = sizeof(mCmuSfrData) / sizeof(struct sfrData);

    for (idx = 0 ; idx < size ; idx++) {
        mCmuSfrData[idx].val = __raw_readl(mCmuSfrData[idx].addr);
    }
}

void cmuDrvRestoreState(void)
{
    uint32_t idx;
    uint32_t size;

    size = sizeof(mCmuSfrData) / sizeof(struct sfrData);

    for (idx = 0 ; idx < size ; idx++) {
        __raw_writel(mCmuSfrData[idx].val, mCmuSfrData[idx].addr);
    }
}
