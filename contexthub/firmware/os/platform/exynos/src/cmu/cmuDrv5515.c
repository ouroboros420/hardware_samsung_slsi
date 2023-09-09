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

static uint32_t cmuDrvGetDivValue(uint32_t inFreq, uint32_t target, uint32_t maxDiv);
static void cmuDrvSetMuxClkCmuChubRcoUser(int);
static void cmuDrvSetMuxClkCmuChubBusUser(int);
static void cmuDrvSetMuxClkChubBus(int);
static void cmuDrvSetMuxClkChubUSI0(int);
static void cmuDrvSetMuxClkChubI2C0(int);
static void cmuDrvSetMuxClkChubI2C1(int);
static void cmuDrvSetMuxClkChubTimer(int);
static uint32_t cmuDrvGetMuxClkCmuChubRcoUser(void);
static uint32_t cmuDrvGetMuxClkCmuChubBusUser(void);
static uint32_t cmuDrvGetMuxClkChubBus(void);
static uint32_t cmuDrvGetMuxClkChubUSI0(void);
static uint32_t cmuDrvGetMuxClkChubSPI0(void);
static uint32_t cmuDrvGetMuxClkChubI2C0(void);
static uint32_t cmuDrvGetMuxClkChubI2C1(void);
static uint32_t cmuDrvGetMuxClkChubTimer(void);
static void cmuDrvSetDivChubBus(IN uint32_t divider);
static void cmuDrvSetDivChubUSI0(IN uint32_t divider);
static void cmuDrvSetDivChubI2C0(IN uint32_t divider);
static void cmuDrvSetDivChubI2C1(IN uint32_t divider);
static uint32_t cmuDrvGetDivChubBus(void);
static uint32_t cmuDrvGetDivChubUSI0(void);
static uint32_t cmuDrvGetDivChubI2C0(void);
static uint32_t cmuDrvGetDivChubI2C1(void);
static void cmuDrvSetMuxClkCmgpI2C0(int);
static void cmuDrvSetMuxClkCmgpI2C1(int);
static void cmuDrvSetMuxClkCmgpI2C2(int);
static void cmuDrvSetMuxClkCmgpI2C3(int);
static void cmuDrvSetMuxClkCmgpI2C4(int);
static void cmuDrvSetMuxClkCmgpI2C5(int);
static void cmuDrvSetMuxClkCmgpI2C6(int);
static void cmuDrvSetMuxClkCmgpUSI0(int);
static void cmuDrvSetMuxClkCmgpUSI1(int);
static void cmuDrvSetMuxClkCmgpUSI2(int);
static void cmuDrvSetMuxClkCmgpUSI3(int);
static uint32_t cmuDrvGetMuxClkCmgpI2C0(void);
static uint32_t cmuDrvGetMuxClkCmgpI2C1(void);
static uint32_t cmuDrvGetMuxClkCmgpI2C2(void);
static uint32_t cmuDrvGetMuxClkCmgpI2C3(void);
static uint32_t cmuDrvGetMuxClkCmgpI2C4(void);
static uint32_t cmuDrvGetMuxClkCmgpI2C5(void);
static uint32_t cmuDrvGetMuxClkCmgpI2C6(void);
static uint32_t cmuDrvGetMuxClkCmgpUSI0(void);
static uint32_t cmuDrvGetMuxClkCmgpUSI1(void);
static uint32_t cmuDrvGetMuxClkCmgpUSI2(void);
static uint32_t cmuDrvGetMuxClkCmgpUSI3(void);
static uint32_t cmuDrvGetMuxClkCmgpSPI0(void);
static uint32_t cmuDrvGetMuxClkCmgpSPI1(void);
static uint32_t cmuDrvGetMuxClkCmgpSPI2(void);
static uint32_t cmuDrvGetMuxClkCmgpSPI3(void);
static void cmuDrvSetDivCmgpI2C0(IN uint32_t divider);
static void cmuDrvSetDivCmgpI2C1(IN uint32_t divider);
static void cmuDrvSetDivCmgpI2C2(IN uint32_t divider);
static void cmuDrvSetDivCmgpI2C3(IN uint32_t divider);
static void cmuDrvSetDivCmgpI2C4(IN uint32_t divider);
static void cmuDrvSetDivCmgpI2C5(IN uint32_t divider);
static void cmuDrvSetDivCmgpI2C6(IN uint32_t divider);
static void cmuDrvSetDivCmgpUSI0(IN uint32_t divider);
static void cmuDrvSetDivCmgpUSI1(IN uint32_t divider);
static void cmuDrvSetDivCmgpUSI2(IN uint32_t divider);
static void cmuDrvSetDivCmgpUSI3(IN uint32_t divider);
static uint32_t cmuDrvGetDivCmgpI2C0(void);
static uint32_t cmuDrvGetDivCmgpI2C1(void);
static uint32_t cmuDrvGetDivCmgpI2C2(void);
static uint32_t cmuDrvGetDivCmgpI2C3(void);
static uint32_t cmuDrvGetDivCmgpI2C4(void);
static uint32_t cmuDrvGetDivCmgpI2C5(void);
static uint32_t cmuDrvGetDivCmgpI2C6(void);
static uint32_t cmuDrvGetDivCmgpUSI0(void);
static uint32_t cmuDrvGetDivCmgpUSI1(void);
static uint32_t cmuDrvGetDivCmgpUSI2(void);
static uint32_t cmuDrvGetDivCmgpUSI3(void);

#define DEFAULT_BUS_DIV                1
#define DEFAULT_I2C_DIV                2
#define DEFAULT_USI_DIV                2

struct sfrData {
    uint32_t addr;
    uint32_t val;
};

static uint32_t tChubMainClk = CHUB_MAIN_CLK;

static struct sfrData mCmuSfrData[] = {
    {REG_CMU_PLL_CON0_MUX_CLK_CHUB_BUS_USER,  0},
    {REG_CMU_PLL_CON0_MUX_CLK_RCO_CHUB_USER,  0},
    {REG_CMU_CHUB_CMU_CHUB_CONTROLLER_OPTION, 0},
    {REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_BUS,    0},
    {REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_I2C0,   0},
    {REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_I2C1,   0},
    {REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_TIMER , 0},
    {REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI0,   0},
    {REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_BUS,    0},
    {REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_I2C0,   0},
    {REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_I2C1,   0},
    {REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI0,   0},
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

void cmuDrvDisableCM55Qch(void)
{
    __raw_writel(0x4, REG_CMU_QCH_CON_YAMIN_MCU_CHUB_QCH_CLKIN);
    __raw_writel(0x4, REG_CMU_QCH_CON_YAMIN_MCU_CHUB_QCH_DBGCLK);
    __raw_writel(0x4, REG_CMU_QCH_CON_YAMIN_MCU_CHUB_QCH_IWICCLK);
    __raw_writel(0x4, REG_CMU_QCH_CON_YAMIN_MCU_CHUB_QCH_PWRDBG);
}

void cmuDrvInit(uint32_t mainClk)
{
    uint32_t val = 0;

    //  Set MUX for CHUB BUS and CPU
    // Select CLKCMU_CHUB_BUS : 393.216 MHz
    cmuDrvSetMuxClkCmuChubBusUser(CLKCMU_CHUB_BUS);
    cmuDrvSetMuxClkCmuChubRcoUser(CLK_RCO_CHUB);

    if (mainClk == 0)
        cmuDrvSetCpuClk(CHUB_MAIN_CLK);
    else
        cmuDrvSetCpuClk(mainClk);

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

    // Set MUX & DIV for CHUB_USI0
    cmuDrvSetMuxClkChubUSI0(MUX_CLK_RCO_CHUB_USER);
    cmuDrvSetDivChubUSI0(DEFAULT_USI_DIV);

    // Set MUX & DIV for CHUB_I2C0
    cmuDrvSetMuxClkChubI2C0(MUX_CLK_RCO_CHUB_USER);
    cmuDrvSetDivChubI2C0(DEFAULT_I2C_DIV);

    // Set MUX & DIV for CHUB_I2C1
    cmuDrvSetMuxClkChubI2C1(MUX_CLK_RCO_CHUB_USER);
    cmuDrvSetDivChubI2C1(DEFAULT_I2C_DIV);

    CSP_PRINTF_INFO("CMU: RcoU(%dMHz), BusU(%dMHz), Bus(%d)\n",
                    (int)(cmuDrvGetMuxClkCmuChubRcoUser()),
                    (int)(cmuDrvGetMuxClkCmuChubBusUser()),
                    (int)(cmuDrvGetMuxClkChubBus()));
    CSP_PRINTF_INFO("CMU: U0(%d), I0(%d), I1(%d), T(%d)\n",
                    (int)(cmuDrvGetMuxClkChubUSI0()),
                    (int)(cmuDrvGetMuxClkChubI2C0()),
                    (int)(cmuDrvGetMuxClkChubI2C1()),
                    (int)(cmuDrvGetMuxClkChubTimer()));
}

static void cmuDrvSetMuxClkCmuChubRcoUser(int select)
{
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_PLL_CON0_MUX_CLK_RCO_CHUB_USER);

    if (select)
        regValue |= MUX_USER_SEL;
    else
        regValue &= ~MUX_USER_SEL;

    __raw_write32(regValue, REG_CMU_PLL_CON0_MUX_CLK_RCO_CHUB_USER);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_PLL_CON0_MUX_CLK_RCO_CHUB_USER) & MUX_BUSY);
}

static void cmuDrvSetMuxClkCmuChubBusUser(int select)
{
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_PLL_CON0_MUX_CLK_CHUB_BUS_USER);

    if (select)
        regValue |= MUX_USER_SEL;
    else
        regValue &= ~MUX_USER_SEL;

    __raw_write32(regValue, REG_CMU_PLL_CON0_MUX_CLK_CHUB_BUS_USER);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_PLL_CON0_MUX_CLK_CHUB_BUS_USER) & MUX_BUSY);
}

static void cmuDrvSetMuxClkChubBus(int select)
{
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_BUS);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_BUS);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_BUS) & MUX_BUSY);
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

static void cmuDrvSetMuxClkChubI2C0(int select)
{
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_I2C0);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_I2C0);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_I2C0) & MUX_BUSY);
}

static void cmuDrvSetMuxClkChubI2C1(int select)
{
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_I2C1);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_I2C1);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_I2C1) & MUX_BUSY);
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

static uint32_t cmuDrvGetMuxClkCmuChubRcoUser(void)
{
    return (__raw_read32(REG_CMU_PLL_CON0_MUX_CLK_RCO_CHUB_USER) & MUX_USER_SEL)? CLK_RCO_FREQ : OSCCLK_RCO_FREQ;
}

static uint32_t cmuDrvGetMuxClkCmuChubBusUser(void)
{
    return (__raw_read32(REG_CMU_PLL_CON0_MUX_CLK_CHUB_BUS_USER) & MUX_USER_SEL)? HFRCO_FREQ : OSCCLK_RCO_FREQ;
}

static uint32_t cmuDrvGetMuxClkChubBus(void)
{
    uint32_t speed = 0;

    if (__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_BUS) & MUX_SEL)
        speed = cmuDrvGetMuxClkCmuChubBusUser() / cmuDrvGetDivChubBus();
    else
        speed = cmuDrvGetMuxClkCmuChubRcoUser() / cmuDrvGetDivChubBus();

    return speed;
}

static uint32_t cmuDrvGetMuxClkChubTimer(void)
{
    uint32_t speed = 0;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_TIMER) & MUX_SEL)
        speed= RTC_FREQ;
    else
        speed = OSCCLK_RCO_FREQ;

    return speed;
}

static uint32_t cmuDrvGetMuxClkChubUSI0(void)
{
    uint32_t speed = 0;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI0) & MUX_SEL)
        speed = cmuDrvGetMuxClkCmuChubBusUser() / cmuDrvGetDivChubUSI0();
    else
        speed = cmuDrvGetMuxClkCmuChubRcoUser() / cmuDrvGetDivChubUSI0();

    return speed;
}

static uint32_t cmuDrvGetMuxClkChubSPI0(void)
{
    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI0) & MUX_SEL)
        return cmuDrvGetMuxClkCmuChubBusUser();
    else
        return cmuDrvGetMuxClkCmuChubRcoUser();
}

static uint32_t cmuDrvGetMuxClkChubI2C0(void)
{
    uint32_t speed = 0;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_I2C0) & MUX_SEL)
        speed = cmuDrvGetMuxClkCmuChubBusUser() / cmuDrvGetDivChubI2C0();
    else
        speed = cmuDrvGetMuxClkCmuChubRcoUser() / cmuDrvGetDivChubI2C0();

    return speed;
}

static uint32_t cmuDrvGetMuxClkChubI2C1(void)
{
    uint32_t speed = 0;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_I2C1) & MUX_SEL)
        speed= cmuDrvGetMuxClkCmuChubBusUser() / cmuDrvGetDivChubI2C1();
    else
        speed = cmuDrvGetMuxClkCmuChubRcoUser() / cmuDrvGetDivChubI2C1();

    return speed;
}

static void cmuDrvSetDivChubBus(IN uint32_t divider)
{
    if (divider == 0)
        return;

    // divider value is clear and set to given divider value
    divider = (divider - 1) & DIV_RATIO_MASK_3BIT;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_BUS) & ~DIV_RATIO_MASK_3BIT) | divider), REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_BUS);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_BUS) & DIV_BUSY);
}

static void cmuDrvSetDivChubUSI0(IN uint32_t divider)
{
    if (divider == 0)
        return;
    // divider value is clear and set to given divider value
    divider = (divider - 1) & DIV_RATIO_MASK_5BIT;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI0) & ~DIV_RATIO_MASK_4BIT) | divider), REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI0);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI0) & DIV_BUSY);
}

static void cmuDrvSetDivChubI2C0(IN uint32_t divider)
{
    if (divider == 0)
        return;
    // divider value is clear and set to given divider value
    divider = (divider - 1) & DIV_RATIO_MASK_4BIT;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_I2C0) & ~DIV_RATIO_MASK_4BIT) | divider), REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_I2C0);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_I2C0) & DIV_BUSY);
}

static void cmuDrvSetDivChubI2C1(IN uint32_t divider)
{
    if (divider == 0)
        return;
    // divider value is clear and set to given divider value
    divider = (divider - 1) & DIV_RATIO_MASK_4BIT;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_I2C1) & ~DIV_RATIO_MASK_4BIT) | divider), REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_I2C1);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_I2C1) & DIV_BUSY);
}

static uint32_t cmuDrvGetDivChubBus(void)
{
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_BUS) & DIV_RATIO_MASK_3BIT) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivChubUSI0(void)
{
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI0) & DIV_RATIO_MASK_5BIT) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivChubI2C0(void)
{
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_I2C0) & DIV_RATIO_MASK_4BIT) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivChubI2C1(void)
{
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_I2C1) & DIV_RATIO_MASK_4BIT) + 1;

    return divider;
}

#if defined(SEOS)
static void __attribute__((unused)) cmuDrvSetMuxClkCmgpI2C0(int select) {
#else
static void cmuDrvSetMuxClkCmgpI2C0(int select) {
#endif
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_I2C0);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_I2C0);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_I2C0) & MUX_BUSY);
}

#if defined(SEOS)
static void __attribute__((unused)) cmuDrvSetMuxClkCmgpI2C1(int select) {
#else
static void cmuDrvSetMuxClkCmgpI2C1(int select) {
#endif
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_I2C1);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_I2C1);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_I2C1) & MUX_BUSY);
}

#if defined(SEOS)
static void __attribute__((unused)) cmuDrvSetMuxClkCmgpI2C2(int select) {
#else
static void cmuDrvSetMuxClkCmgpI2C2(int select) {
#endif
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_I2C2);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_I2C2);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_I2C2) & MUX_BUSY);
}

#if defined(SEOS)
static void __attribute__((unused)) cmuDrvSetMuxClkCmgpI2C3(int select) {
#else
static void cmuDrvSetMuxClkCmgpI2C3(int select) {
#endif
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_I2C3);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_I2C3);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_I2C3) & MUX_BUSY);
}

#if defined(SEOS)
static void __attribute__((unused)) cmuDrvSetMuxClkCmgpI2C4(int select) {
#else
static void cmuDrvSetMuxClkCmgpI2C4(int select) {
#endif
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_I2C4);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_I2C4);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_I2C4) & MUX_BUSY);
}

#if defined(SEOS)
static void __attribute__((unused)) cmuDrvSetMuxClkCmgpI2C5(int select) {
#else
static void cmuDrvSetMuxClkCmgpI2C5(int select) {
#endif
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_I2C5);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_I2C5);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_I2C5) & MUX_BUSY);
}

#if defined(SEOS)
static void __attribute__((unused)) cmuDrvSetMuxClkCmgpI2C6(int select) {
#else
static void cmuDrvSetMuxClkCmgpI2C6(int select) {
#endif
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_I2C6);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_I2C6);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_I2C6) & MUX_BUSY);
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

static uint32_t cmuDrvGetMuxClkCmgpI2C0(void) {
    uint32_t speed;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_I2C0) & MUX_SEL)
        speed = HFRCO_FREQ / cmuDrvGetDivCmgpI2C0();
    else
        speed = CLK_RCO_FREQ / cmuDrvGetDivCmgpI2C0();

    return speed;
}

static uint32_t cmuDrvGetMuxClkCmgpI2C1(void) {
    uint32_t speed;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_I2C1) & MUX_SEL)
        speed = HFRCO_FREQ / cmuDrvGetDivCmgpI2C1();
    else
        speed = CLK_RCO_FREQ / cmuDrvGetDivCmgpI2C1();

    return speed;
}

static uint32_t cmuDrvGetMuxClkCmgpI2C2(void) {
    uint32_t speed;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_I2C2) & MUX_SEL)
        speed = HFRCO_FREQ / cmuDrvGetDivCmgpI2C2();
    else
        speed = CLK_RCO_FREQ / cmuDrvGetDivCmgpI2C2();

    return speed;
}

static uint32_t cmuDrvGetMuxClkCmgpI2C3(void) {
    uint32_t speed;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_I2C3) & MUX_SEL)
        speed = HFRCO_FREQ / cmuDrvGetDivCmgpI2C3();
    else
        speed = CLK_RCO_FREQ / cmuDrvGetDivCmgpI2C3();

    return speed;
}

static uint32_t cmuDrvGetMuxClkCmgpI2C4(void) {
    uint32_t speed;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_I2C4) & MUX_SEL)
        speed = HFRCO_FREQ / cmuDrvGetDivCmgpI2C4();
    else
        speed = CLK_RCO_FREQ / cmuDrvGetDivCmgpI2C4();

    return speed;
}

static uint32_t cmuDrvGetMuxClkCmgpI2C5(void) {
    uint32_t speed;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_I2C5) & MUX_SEL)
        speed = HFRCO_FREQ / cmuDrvGetDivCmgpI2C5();
    else
        speed = CLK_RCO_FREQ / cmuDrvGetDivCmgpI2C5();

    return speed;
}

static uint32_t cmuDrvGetMuxClkCmgpI2C6(void) {
    uint32_t speed;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_I2C6) & MUX_SEL)
        speed = HFRCO_FREQ / cmuDrvGetDivCmgpI2C6();
    else
        speed = CLK_RCO_FREQ / cmuDrvGetDivCmgpI2C6();

    return speed;
}

static uint32_t cmuDrvGetMuxClkCmgpUSI0(void) {
    uint32_t speed;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI0) & MUX_SEL)
        speed = HFRCO_FREQ / cmuDrvGetDivCmgpUSI0();
    else
        speed = CLK_RCO_FREQ / cmuDrvGetDivCmgpUSI0();

    return speed;
}

static uint32_t cmuDrvGetMuxClkCmgpUSI1(void) {
    uint32_t speed;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI1) & MUX_SEL)
        speed = HFRCO_FREQ / cmuDrvGetDivCmgpUSI1();
    else
        speed = CLK_RCO_FREQ / cmuDrvGetDivCmgpUSI1();

    return speed;
}

static uint32_t cmuDrvGetMuxClkCmgpUSI2(void) {
    uint32_t speed;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI2) & MUX_SEL)
        speed = HFRCO_FREQ / cmuDrvGetDivCmgpUSI2();
    else
        speed = CLK_RCO_FREQ / cmuDrvGetDivCmgpUSI2();

    return speed;
}

static uint32_t cmuDrvGetMuxClkCmgpUSI3(void) {
    uint32_t speed;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI3) & MUX_SEL)
        speed = HFRCO_FREQ / cmuDrvGetDivCmgpUSI3();
    else
        speed = CLK_RCO_FREQ / cmuDrvGetDivCmgpUSI3();

    return speed;
}

static uint32_t cmuDrvGetMuxClkCmgpSPI0(void) {
    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI0) & MUX_SEL)
        return HFRCO_FREQ;
    else
        return CLK_RCO_FREQ;
}

static uint32_t cmuDrvGetMuxClkCmgpSPI1(void) {
    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI1) & MUX_SEL)
        return HFRCO_FREQ;
    else
        return CLK_RCO_FREQ;
}

static uint32_t cmuDrvGetMuxClkCmgpSPI2(void) {
    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI2) & MUX_SEL)
        return HFRCO_FREQ;
    else
        return CLK_RCO_FREQ;
}

static uint32_t cmuDrvGetMuxClkCmgpSPI3(void) {
    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI3) & MUX_SEL)
        return HFRCO_FREQ;
    else
        return CLK_RCO_FREQ;
}

static void cmuDrvSetDivCmgpI2C0(IN uint32_t divider) {
    if (divider == 0)
        return;

    // divider value is clear and set to given divider value
    divider = (divider - 1) & DIV_RATIO_MASK_4BIT;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C0) & ~DIV_RATIO_MASK_4BIT) | divider), 
                  REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C0);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C0) & DIV_BUSY);
}

static void cmuDrvSetDivCmgpI2C1(IN uint32_t divider) {
    if (divider == 0)
        return;

    // divider value is clear and set to given divider value
    divider = (divider - 1) & DIV_RATIO_MASK_4BIT;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C1) & ~DIV_RATIO_MASK_4BIT) | divider), 
                  REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C1);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C1) & DIV_BUSY);
}

static void cmuDrvSetDivCmgpI2C2(IN uint32_t divider) {
    if (divider == 0)
        return;

    // divider value is clear and set to given divider value
    divider = (divider - 1) & DIV_RATIO_MASK_4BIT;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C2) & ~DIV_RATIO_MASK_4BIT) | divider), 
                  REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C2);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C2) & DIV_BUSY);
}

static void cmuDrvSetDivCmgpI2C3(IN uint32_t divider) {
    if (divider == 0)
        return;

    // divider value is clear and set to given divider value
    divider = (divider - 1) & DIV_RATIO_MASK_4BIT;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C3) & ~DIV_RATIO_MASK_4BIT) | divider), 
                  REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C3);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C3) & DIV_BUSY);
}

static void cmuDrvSetDivCmgpI2C4(IN uint32_t divider) {
    if (divider == 0)
        return;

    // divider value is clear and set to given divider value
    divider = (divider - 1) & DIV_RATIO_MASK_4BIT;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C4) & ~DIV_RATIO_MASK_4BIT) | divider), 
                  REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C4);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C4) & DIV_BUSY);
}

static void cmuDrvSetDivCmgpI2C5(IN uint32_t divider) {
    if (divider == 0)
        return;

    // divider value is clear and set to given divider value
    divider = (divider - 1) & DIV_RATIO_MASK_4BIT;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C5) & ~DIV_RATIO_MASK_4BIT) | divider), 
                  REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C5);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C5) & DIV_BUSY);
}

static void cmuDrvSetDivCmgpI2C6(IN uint32_t divider) {
    if (divider == 0)
        return;

    // divider value is clear and set to given divider value
    divider = (divider - 1) & DIV_RATIO_MASK_4BIT;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C6) & ~DIV_RATIO_MASK_4BIT) | divider), 
                  REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C6);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C6) & DIV_BUSY);
}

static void cmuDrvSetDivCmgpUSI0(IN uint32_t divider) {
    if (divider == 0)
        return;

    // divider value is clear and set to given divider value
    divider = (divider - 1) & DIV_RATIO_MASK_5BIT;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI0) & ~DIV_RATIO_MASK_4BIT) | divider), 
                  REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI0);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI0) & DIV_BUSY);
}

static void cmuDrvSetDivCmgpUSI1(IN uint32_t divider) {
    if (divider == 0)
        return;

    // divider value is clear and set to given divider value
    divider = (divider - 1) & DIV_RATIO_MASK_5BIT;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI1) & ~DIV_RATIO_MASK_4BIT) | divider), 
                  REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI1);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI1) & DIV_BUSY);
}

static void cmuDrvSetDivCmgpUSI2(IN uint32_t divider) {
    if (divider == 0)
        return;

    // divider value is clear and set to given divider value
    divider = (divider - 1) & DIV_RATIO_MASK_5BIT;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI2) & ~DIV_RATIO_MASK_4BIT) | divider), 
                  REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI2);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI2) & DIV_BUSY);
}

static void cmuDrvSetDivCmgpUSI3(IN uint32_t divider) {
    if (divider == 0)
        return;

    // divider value is clear and set to given divider value
    divider = (divider - 1) & DIV_RATIO_MASK_5BIT;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI3) & ~DIV_RATIO_MASK_4BIT) | divider), 
                  REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI3);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI3) & DIV_BUSY);
}

static uint32_t cmuDrvGetDivCmgpI2C0(void) {
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C0) & DIV_RATIO_MASK_4BIT) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivCmgpI2C1(void) {
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C1) & DIV_RATIO_MASK_4BIT) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivCmgpI2C2(void) {
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C2) & DIV_RATIO_MASK_4BIT) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivCmgpI2C3(void) {
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C3) & DIV_RATIO_MASK_4BIT) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivCmgpI2C4(void) {
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C4) & DIV_RATIO_MASK_4BIT) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivCmgpI2C5(void) {
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C5) & DIV_RATIO_MASK_4BIT) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivCmgpI2C6(void) {
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C6) & DIV_RATIO_MASK_4BIT) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivCmgpUSI0(void) {
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI0) & DIV_RATIO_MASK_5BIT) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivCmgpUSI1(void) {
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI1) & DIV_RATIO_MASK_5BIT) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivCmgpUSI2(void) {
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI2) & DIV_RATIO_MASK_5BIT) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivCmgpUSI3(void) {
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI3) & DIV_RATIO_MASK_5BIT) + 1;

    return divider;
}

uint32_t cmuDrvSetCpuMux(uint32_t mux)
{
    uint32_t prev;

    prev = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_BUS) & MUX_SEL;
    if (mux == CPU_MUX_LOW)
        cmuDrvSetMuxClkChubBus(MUX_CLK_RCO_CHUB_USER);
    else
        cmuDrvSetMuxClkChubBus(MUX_CLK_CHUB_BUS_USER);

    return prev;
}

int cmuDrvSetCpuClk(uint32_t freq)
{
    uint32_t div;


    if (freq <= CLK_RCO_FREQ) {
        // Set Input Clock : CLK_RCO_CHUB (49152000)
        cmuDrvSetMuxClkChubBus(MUX_CLK_RCO_CHUB_USER);
        div = cmuDrvGetDivValue(CLK_RCO_FREQ, freq, 8);
    } else {
        // Set Input Clock : HFRCO_CHUB (393216000)
        cmuDrvSetMuxClkChubBus(MUX_CLK_CHUB_BUS_USER);
        div = cmuDrvGetDivValue(HFRCO_FREQ, freq, 8);
    }
    cmuDrvSetDivChubBus(div);
    tChubMainClk = cmuDrvGetSpeed(CMU_CLK_CHUB_BUS);

    //CSP_PRINTF_INFO("cmuDrvSetCpuClk div%lu, freq:%lu\n", div, tChubMainClk);
    return 0;
}

//
uint32_t cmuDrvGetSpeed(IN CmuIpType ip)
{
    switch(ip) {
        case CMU_CLK_OSCCLK_RCO_CHUB:
            return OSCCLK_RCO_FREQ;

        case CMU_CLK_CLK_RCO_CHUB:
            return CLK_RCO_FREQ;

        case CMU_CLK_RTCCLK:
            return RTC_FREQ;

        case CMU_CLK_CHUB_CM4:
        case CMU_CLK_OUTPUT_CPU:
            return cmuDrvGetMuxClkChubBus() / cmuDrvGetDivChubBus();

        case CMU_CLK_CHUB_TIMER:
            return cmuDrvGetMuxClkChubTimer();

        case CMU_CLK_CHUB_BUS:
            return cmuDrvGetMuxClkChubBus();

        case CMU_CLK_CHUB_I2C0:
        case CMU_CLK_I2C1:
            return cmuDrvGetMuxClkChubI2C0();

        case CMU_CLK_CHUB_I2C1:
        case CMU_CLK_I2C2:
            return cmuDrvGetMuxClkChubI2C1();

        case CMU_CLK_CHUB_USI0:
        case CMU_CLK_I2C0:
            return cmuDrvGetMuxClkChubUSI0();

        case CMU_CLK_CMGP_I2C0:
        case CMU_CLK_I2C4:
            return cmuDrvGetMuxClkCmgpI2C0();

        case CMU_CLK_CMGP_I2C1:
        case CMU_CLK_I2C6:
            return cmuDrvGetMuxClkCmgpI2C1();

        case CMU_CLK_CMGP_I2C2:
        case CMU_CLK_I2C8:
            return cmuDrvGetMuxClkCmgpI2C2();

        case CMU_CLK_CMGP_I2C3:
        case CMU_CLK_I2C10:
            return cmuDrvGetMuxClkCmgpI2C3();

        case CMU_CLK_CMGP_I2C4:
        case CMU_CLK_I2C11:
            return cmuDrvGetMuxClkCmgpI2C4();

        case CMU_CLK_CMGP_I2C5:
        case CMU_CLK_I2C12:
            return cmuDrvGetMuxClkCmgpI2C5();

        case CMU_CLK_CMGP_I2C6:
        case CMU_CLK_I2C13:
            return cmuDrvGetMuxClkCmgpI2C6();

        case CMU_CLK_CMGP_USI0:
        case CMU_CLK_I2C3:
            return cmuDrvGetMuxClkCmgpUSI0();

        case CMU_CLK_CMGP_USI1:
        case CMU_CLK_I2C5:
            return cmuDrvGetMuxClkCmgpUSI1();

        case CMU_CLK_CMGP_USI2:
        case CMU_CLK_I2C7:
            return cmuDrvGetMuxClkCmgpUSI2();

        case CMU_CLK_CMGP_USI3:
        case CMU_CLK_I2C9:
            return cmuDrvGetMuxClkCmgpUSI3();

        case CMU_CLK_CHUB_SPI0:
            return cmuDrvGetMuxClkChubSPI0();
        case CMU_CLK_CMGP_SPI0:
            return cmuDrvGetMuxClkCmgpSPI0();
        case CMU_CLK_CMGP_SPI1:
            return cmuDrvGetMuxClkCmgpSPI1();
        case CMU_CLK_CMGP_SPI2:
            return cmuDrvGetMuxClkCmgpSPI2();
        case CMU_CLK_CMGP_SPI3:
            return cmuDrvGetMuxClkCmgpSPI3();

        default:
            break;
    }

    return 0;
}

bool cmuDrvSetDivider(IN CmuIpType ip, IN uint32_t divider)
{
    switch(ip) {
        case CMU_CLK_CHUB_BUS:
            cmuDrvSetDivChubBus(divider);
            break;

        case CMU_CLK_CHUB_I2C0:
            cmuDrvSetDivChubI2C0(divider);
            break;

        case CMU_CLK_CHUB_I2C1:
            cmuDrvSetDivChubI2C1(divider);
            break;

        case CMU_CLK_CHUB_USI0:
        case CMU_CLK_CHUB_SPI0:
            cmuDrvSetDivChubUSI0(divider);
            break;

        case CMU_CLK_CMGP_I2C0:
            cmuDrvSetDivCmgpI2C0(divider);
            break;

        case CMU_CLK_CMGP_I2C1:
            cmuDrvSetDivCmgpI2C1(divider);
            break;

        case CMU_CLK_CMGP_I2C2:
            cmuDrvSetDivCmgpI2C2(divider);
            break;

        case CMU_CLK_CMGP_I2C3:
            cmuDrvSetDivCmgpI2C3(divider);
            break;

        case CMU_CLK_CMGP_I2C4:
            cmuDrvSetDivCmgpI2C4(divider);
            break;

        case CMU_CLK_CMGP_I2C5:
            cmuDrvSetDivCmgpI2C5(divider);
            break;

        case CMU_CLK_CMGP_I2C6:
            cmuDrvSetDivCmgpI2C6(divider);
            break;

        case CMU_CLK_CMGP_USI0:
        case CMU_CLK_CMGP_SPI0:
            cmuDrvSetDivCmgpUSI0(divider);
            break;

        case CMU_CLK_CMGP_USI1:
        case CMU_CLK_CMGP_SPI1:
            cmuDrvSetDivCmgpUSI1(divider);
            break;

        case CMU_CLK_CMGP_USI2:
        case CMU_CLK_CMGP_SPI2:
            cmuDrvSetDivCmgpUSI2(divider);
            break;

        case CMU_CLK_CMGP_USI3:
        case CMU_CLK_CMGP_SPI3:
            cmuDrvSetDivCmgpUSI3(divider);
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
