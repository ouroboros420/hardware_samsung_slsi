/*----------------------------------------------------------------------------
 * Exynos SoC  -  CMU
 *
 * Copyright (C) 2020 Samsung Electronics Co., Ltd.
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

#include <cmu.h>
#include <cmuDrv.h>
#include <sysreg.h>
#include <csp_common.h>

static void cmuDrvSetMuxClkCmuChubRcoUser(int);
static void cmuDrvSetMuxClkCmuChubBusUser(int);
static void cmuDrvSetMuxClkChubBus(int);

static uint32_t cmuDrvGetFreqMuxClkCmuChubRcoUser(void);
static uint32_t cmuDrvGetFreqMuxClkCmuChubBusUser(void);
static uint32_t cmuDrvGetFreqMuxClkChubBus(void);

static void cmuDrvSetDivChubBus(IN uint32_t divider);

static uint32_t cmuDrvGetDivChubBus(void);

static void cmuDrvSetMuxClkCmgpUSI0(int);
static void cmuDrvSetMuxClkCmgpUSI1(int);

static uint32_t cmuDrvGetFreqMuxClkCmgpUSI0(void);
static uint32_t cmuDrvGetFreqMuxClkCmgpUSI1(void);

static void cmuDrvSetDivCmgpUSI0(IN uint32_t divider);
static void cmuDrvSetDivCmgpUSI1(IN uint32_t divider);

static uint32_t cmuDrvGetDivCmgpUSI0(void);
static uint32_t cmuDrvGetDivCmgpUSI1(void);

#define DEFAULT_BUS_DIV                0
#define DEFAULT_USI_DIV                0

struct sfrData {
    uint32_t addr;
    uint32_t val;
};

static uint32_t tChubMainClk = CHUB_MAIN_CLK;

static struct sfrData mCmuSfrData[] = {
    {PLL_CON0_MUX_CLKCMU_CHUB_BUS_USER, 0},
    {PLL_CON0_MUX_CLK_RCO_CHUB_USER,    0},
    {CLK_CON_MUX_MUX_CLK_CHUB_BUS,      0},
    {CLK_CON_DIV_DIV_CLK_CHUB_BUS,      0},
};

void cmuDrvInit(uint32_t mainClk) {
    uint32_t regValue = 0;

    // Set MUX for CHUB NOC and CPU
    cmuDrvSetMuxClkCmuChubBusUser(CLKCMU_CHUB_BUS);
    cmuDrvSetMuxClkCmuChubRcoUser(CLK_RCO_CHUB__ALV);
    (void)mainClk;
    cmuDrvSetMuxClkChubBus(MUX_CLKCMU_CHUB_BUS_USER);

    // Set MUX & DIV for CMGP_USI
    cmuDrvSetMuxClkCmgpUSI0(CLK_RCO_CMGP);
    cmuDrvSetMuxClkCmgpUSI1(CLK_RCO_CMGP);
}

uint32_t cmuDrvSetCpuMux(uint32_t mux) {
    return 0;
}

static void cmuDrvSetMuxClkCmuChubBusUser(int select) {
    uint32_t regValue;

    regValue = __raw_read32(PLL_CON0_MUX_CLKCMU_CHUB_BUS_USER);

    if (select)
        regValue |= MUX_USER_SEL;
    else
        regValue &= ~MUX_USER_SEL;

    __raw_write32(regValue, PLL_CON0_MUX_CLKCMU_CHUB_BUS_USER);

    // Wait while mux is changing
    while(__raw_read32(PLL_CON0_MUX_CLKCMU_CHUB_BUS_USER) & MUX_BUSY);
}

static void cmuDrvSetMuxClkCmuChubRcoUser(int select) {
    uint32_t regValue;

    regValue = __raw_read32(PLL_CON0_MUX_CLK_RCO_CHUB_USER);

    if (select)
        regValue |= MUX_USER_SEL;
    else
        regValue &= ~MUX_USER_SEL;

    __raw_write32(regValue, PLL_CON0_MUX_CLK_RCO_CHUB_USER);

    // Wait while mux is changing
    while(__raw_read32(PLL_CON0_MUX_CLK_RCO_CHUB_USER) & MUX_BUSY);
}

static void cmuDrvSetMuxClkChubBus(int select) {
    uint32_t regValue;

    regValue = __raw_read32(CLK_CON_MUX_MUX_CLK_CHUB_BUS);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, CLK_CON_MUX_MUX_CLK_CHUB_BUS);

    // Wait while mux is changing
    while(__raw_read32(CLK_CON_MUX_MUX_CLK_CHUB_BUS) & MUX_BUSY);
}

static uint32_t cmuDrvGetFreqMuxClkCmuChubBusUser(void) {
    return (__raw_read32(PLL_CON0_MUX_CLKCMU_CHUB_BUS_USER) & MUX_USER_SEL)? CLKCMU_CHUB_BUS_USER : OSCCLK_RCO_FREQ;
}

static uint32_t cmuDrvGetFreqMuxClkCmuChubRcoUser(void) {
    return (__raw_read32(PLL_CON0_MUX_CLK_RCO_CHUB_USER) & MUX_USER_SEL)? CLK_RCO_FREQ : OSCCLK_RCO_FREQ;
}

static uint32_t cmuDrvGetFreqMuxClkChubBus(void) {
    uint32_t speed = 0;

    if (__raw_read32(CLK_CON_MUX_MUX_CLK_CHUB_BUS) & MUX_SEL)
        speed = cmuDrvGetFreqMuxClkCmuChubBusUser() / cmuDrvGetDivChubBus();
    else
        speed = cmuDrvGetFreqMuxClkCmuChubRcoUser() / cmuDrvGetDivChubBus();

    return speed;
}

static void cmuDrvSetDivChubBus(IN uint32_t divider) {
    uint32_t regValue;

    if (divider == 0)
        return;

    divider = (divider - 1) & DIV_RATIO_MASK_3BIT;

    regValue = (__raw_read32(CLK_CON_DIV_DIV_CLK_CHUB_BUS) & ~DIV_RATIO_MASK_3BIT) | divider;
    __raw_write32(regValue, CLK_CON_DIV_DIV_CLK_CHUB_BUS);

    // Wait while divider is changing
    while(__raw_read32(CLK_CON_DIV_DIV_CLK_CHUB_BUS) & DIV_BUSY);
}

static uint32_t cmuDrvGetDivChubBus(void) {
    uint32_t divider;
    divider = (__raw_read32(CLK_CON_DIV_DIV_CLK_CHUB_BUS) & DIV_RATIO_MASK_3BIT) + 1;

    return divider;
}

#if defined(SEOS)
static void __attribute__((unused)) cmuDrvSetMuxClkCmgpUSI0(int select) {
#else
static void cmuDrvSetMuxClkCmgpUSI0(int select) {
#endif
    uint32_t regValue;
    regValue = __raw_read32(CLK_CON_MUX_CLK_CMGP_USI_CMGP0);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, CLK_CON_MUX_CLK_CMGP_USI_CMGP0);

    // Wait while mux is changing
    while(__raw_read32(CLK_CON_MUX_CLK_CMGP_USI_CMGP0) & MUX_BUSY);
}

#if defined(SEOS)
static void __attribute__((unused)) cmuDrvSetMuxClkCmgpUSI1(int select) {
#else
static void cmuDrvSetMuxClkCmgpUSI1(int select) {
#endif
    uint32_t regValue;
    regValue = __raw_read32(CLK_CON_MUX_CLK_CMGP_USI_CMGP1);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, CLK_CON_MUX_CLK_CMGP_USI_CMGP1);

    // Wait while mux is changing
    while(__raw_read32(CLK_CON_MUX_CLK_CMGP_USI_CMGP1) & MUX_BUSY);
}

static uint32_t cmuDrvGetFreqMuxClkCmgpUSI0(void) {
    uint32_t speed;

    if(__raw_read32(CLK_CON_MUX_CLK_CMGP_USI_CMGP0) & MUX_SEL)
        speed = CLKCMU_CMGP_BUS_USER / cmuDrvGetDivCmgpUSI0();
    else
        speed = CLK_RCO_FREQ / cmuDrvGetDivCmgpUSI0();

    return speed;
}

static uint32_t cmuDrvGetFreqMuxClkCmgpUSI1(void) {
    uint32_t speed;

    if(__raw_read32(CLK_CON_MUX_CLK_CMGP_USI_CMGP1) & MUX_SEL)
        speed = CLKCMU_CMGP_BUS_USER / cmuDrvGetDivCmgpUSI0();
    else
        speed = CLK_RCO_FREQ / cmuDrvGetDivCmgpUSI1();

    return speed;
}

static void cmuDrvSetDivCmgpUSI0(IN uint32_t divider) {
    if (divider == 0)
        return;

    // divider value is clear and set to given divider value
    divider = (divider - 1) & DIV_RATIO_MASK_5BIT;
    __raw_write32(((__raw_read32(CLK_CON_DIV_CLK_CMGP_USI_CMGP0) & ~DIV_RATIO_MASK_5BIT) | divider), 
                  CLK_CON_DIV_CLK_CMGP_USI_CMGP0);

    // Wait while divider is changing
    while(__raw_read32(CLK_CON_DIV_CLK_CMGP_USI_CMGP0) & DIV_BUSY);
}

static void cmuDrvSetDivCmgpUSI1(IN uint32_t divider) {
    if (divider == 0)
        return;

    // divider value is clear and set to given divider value
    divider = (divider - 1) & DIV_RATIO_MASK_5BIT;
    __raw_write32(((__raw_read32(CLK_CON_DIV_CLK_CMGP_USI_CMGP1) & ~DIV_RATIO_MASK_5BIT) | divider), 
                  CLK_CON_DIV_CLK_CMGP_USI_CMGP1);

    // Wait while divider is changing
    while(__raw_read32(CLK_CON_DIV_CLK_CMGP_USI_CMGP1) & DIV_BUSY);
}

static uint32_t cmuDrvGetDivCmgpUSI0(void) {
    uint32_t divider;
    divider = (__raw_read32(CLK_CON_DIV_CLK_CMGP_USI_CMGP0) & DIV_RATIO_MASK_5BIT) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivCmgpUSI1(void) {
    uint32_t divider;
    divider = (__raw_read32(CLK_CON_DIV_CLK_CMGP_USI_CMGP1) & DIV_RATIO_MASK_5BIT) + 1;

    return divider;
}

uint32_t cmuDrvGetSpeed(IN CmuIpType ip)
{
    switch(ip) {
        case CMU_CLK_OUTPUT_CPU:
        case CMU_CLK_CHUB_BUS:
        case CMU_MUX_CHUB_BUS:
            return cmuDrvGetFreqMuxClkChubBus();

        case CMU_MUX_CMGP_USI0:
        case CMU_CLK_SPI0:
            return cmuDrvGetFreqMuxClkCmgpUSI0();

        case CMU_MUX_CMGP_USI1:
        case CMU_CLK_SPI1:
            return cmuDrvGetFreqMuxClkCmgpUSI1();

        case CMU_DIV_CMGP_USI0:
        case CMU_CLK_I2C0:
            return cmuDrvGetFreqMuxClkCmgpUSI0() / cmuDrvGetDivCmgpUSI0();

        case CMU_DIV_CMGP_USI1:
        case CMU_CLK_I2C1:
            return cmuDrvGetFreqMuxClkCmgpUSI1() / cmuDrvGetDivCmgpUSI1();

        default:
            break;
    }

    return 0;
}

bool cmuDrvSetDivider(IN CmuIpType ip, IN uint32_t divider)
{
    switch(ip) {
        case CMU_DIV_CHUB_BUS:
        case CMU_CLK_OUTPUT_CPU:
        case CMU_CLK_CHUB_BUS:
            cmuDrvSetDivChubBus(divider);
            break;

        case CMU_DIV_CMGP_USI0:
        case CMU_CLK_SPI0:
            cmuDrvSetDivCmgpUSI0(divider);
            break;

        case CMU_DIV_CMGP_USI1:
        case CMU_CLK_SPI1:
            cmuDrvSetDivCmgpUSI1(divider);
            break;

        default:
            return false;
    }

    return true;
}

static bool isDrcgHwacgSet = 0;

//
bool cmuDrvControlHwacg(IN CmuHwacgControlType enable)
{
    // Dynamic Root Clock Gating can be set once at boot time
    if(isDrcgHwacgSet && enable) {
        return false;
    }

    uint32_t val = 0;

    // CHUB_CMU_CHUB_CONTROLLER_OPTION in CMU_CHUB
    if(enable) {
        val = __raw_read32(CHUB_CMU_CHUB_CONTROLLER_OPTION);
        val |= CHUB_CONTROLLER_OPTION_PM_ENABLE;
        val |= CHUB_CONTROLLER_OPTION_HWACG_ENABLE;
        val |= CHUB_CONTROLLER_OPTION_MEMPG_ENABLE;
        __raw_write32(val, CHUB_CMU_CHUB_CONTROLLER_OPTION);
    }
    else {
        // Disable HWACG and power management
        val = __raw_read32(CHUB_CMU_CHUB_CONTROLLER_OPTION);
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

