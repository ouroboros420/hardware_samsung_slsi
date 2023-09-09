/*----------------------------------------------------------------------------
 * Exynos SoC  -  CMU
 *
 * Copyright (C) 2022 Samsung Electronics Co., Ltd.
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

static uint32_t cmuDrvGetDivValue(uint32_t inFreq, uint32_t target, uint32_t maxDiv);

static uint32_t cmuDrvGetSpeedMuxChubNocUser(void);
static uint32_t cmuDrvGetSpeedMuxChubPeriUser(void);
static uint32_t cmuDrvGetSpeedMuxChubRcoUser(void);
static uint32_t cmuDrvGetSpeedMuxChubNoc(void);
static uint32_t cmuDrvGetSpeedMuxChubUSI0(void);
static uint32_t cmuDrvGetSpeedMuxChubUSI1(void);
static uint32_t cmuDrvGetSpeedMuxChubUSI2(void);
static uint32_t cmuDrvGetSpeedMuxChubUSI3(void);
static uint32_t cmuDrvGetSpeedMuxChubI2C(void);
static uint32_t cmuDrvGetSpeedMuxChubTimer(void);
static void cmuDrvSetMuxClkCmuChubRcoUser(int);
static void cmuDrvSetMuxClkCmuChubNocUser(int);
static void cmuDrvSetMuxClkCmuChubPeriUser(int);
static void cmuDrvSetMuxClkChubNoc(int);
static void cmuDrvSetMuxClkChubI2C(int);
static void cmuDrvSetMuxClkChubTimer(int);
static void cmuDrvSetMuxClkChubUSI0(int);
static void cmuDrvSetMuxClkChubUSI1(int);
static void cmuDrvSetMuxClkChubUSI2(int);
static void cmuDrvSetMuxClkChubUSI3(int);
static void cmuDrvSetDivChubNoc(IN uint32_t divider);
static void cmuDrvSetDivChubI2C(IN uint32_t divider);
static void cmuDrvSetDivChubUSI0(IN uint32_t divider);
static void cmuDrvSetDivChubUSI1(IN uint32_t divider);
static void cmuDrvSetDivChubUSI2(IN uint32_t divider);
static void cmuDrvSetDivChubUSI3(IN uint32_t divider);
static uint32_t cmuDrvGetDivChubBus(void);
static uint32_t cmuDrvGetDivChubI2C(void);
static uint32_t cmuDrvGetDivChubUSI0(void);
static uint32_t cmuDrvGetDivChubUSI1(void);
static uint32_t cmuDrvGetDivChubUSI2(void);
static uint32_t cmuDrvGetDivChubUSI3(void);
static void cmuDrvSetMuxClkCmgpI2C(int);
static void cmuDrvSetMuxClkCmgpUSI0(int);
static void cmuDrvSetMuxClkCmgpUSI1(int);
static void cmuDrvSetMuxClkCmgpUSI2(int);
static void cmuDrvSetMuxClkCmgpUSI3(int);
static void cmuDrvSetMuxClkCmgpUSI4(int);
static void cmuDrvSetMuxClkCmgpUSI5(int);
static void cmuDrvSetMuxClkCmgpUSI6(int);
static void cmuDrvSetDivCmgpI2C(IN uint32_t divider);
static void cmuDrvSetDivCmgpUSI0(IN uint32_t divider);
static void cmuDrvSetDivCmgpUSI1(IN uint32_t divider);
static void cmuDrvSetDivCmgpUSI2(IN uint32_t divider);
static void cmuDrvSetDivCmgpUSI3(IN uint32_t divider);
static void cmuDrvSetDivCmgpUSI4(IN uint32_t divider);
static void cmuDrvSetDivCmgpUSI5(IN uint32_t divider);
static void cmuDrvSetDivCmgpUSI6(IN uint32_t divider);
static uint32_t cmuDrvGetDivCmgpI2C(void);
static uint32_t cmuDrvGetDivCmgpUSI0(void);
static uint32_t cmuDrvGetDivCmgpUSI1(void);
static uint32_t cmuDrvGetDivCmgpUSI2(void);
static uint32_t cmuDrvGetDivCmgpUSI3(void);
static uint32_t cmuDrvGetDivCmgpUSI4(void);
static uint32_t cmuDrvGetDivCmgpUSI5(void);
static uint32_t cmuDrvGetDivCmgpUSI6(void);
static uint32_t cmuDrvGetSpeedMuxCmgpI2C(void);
static uint32_t cmuDrvGetSpeedMuxCmgpUSI0(void);
static uint32_t cmuDrvGetSpeedMuxCmgpUSI1(void);
static uint32_t cmuDrvGetSpeedMuxCmgpUSI2(void);
static uint32_t cmuDrvGetSpeedMuxCmgpUSI3(void);
static uint32_t cmuDrvGetSpeedMuxCmgpUSI4(void);
static uint32_t cmuDrvGetSpeedMuxCmgpUSI5(void);
static uint32_t cmuDrvGetSpeedMuxCmgpUSI6(void);

#define DEFAULT_I2C_DIV                2
#define DEFAULT_USI_DIV                2

struct sfrData {
    uint32_t addr;
    uint32_t val;
};

static uint32_t tChubMainClk = CHUB_MAIN_CLK;

static struct sfrData mCmuSfrData[] = {
    {REG_CMU_PLL_CON0_MUX_CLKCMU_CHUB_RCO_USER,  0},
    {REG_CMU_PLL_CON0_MUX_CLKCMU_CHUB_NOC_USER,  0},
    {REG_CMU_PLL_CON0_MUX_CLKCMU_CHUB_PERI_USER, 0},
    {REG_CMU_CLK_CON_MUX_CLK_CHUB_USI0,          0},
    {REG_CMU_CLK_CON_MUX_CLK_CHUB_USI1,          0},
    {REG_CMU_CLK_CON_MUX_CLK_CHUB_USI2,          0},
    {REG_CMU_CLK_CON_MUX_CLK_CHUB_USI3,          0},
    {REG_CMU_CLK_CON_MUX_CLK_CHUB_I2C,           0},
    {REG_CMU_CLK_CON_MUX_CLK_CHUB_NOC,           0},
    {REG_CMU_CLK_CON_MUX_CLK_CHUB_TIMER,         0},

    {REG_CMU_CLK_CON_DIV_CLK_CHUB_USI0,          0},
    {REG_CMU_CLK_CON_DIV_CLK_CHUB_USI1,          0},
    {REG_CMU_CLK_CON_DIV_CLK_CHUB_USI2,          0},
    {REG_CMU_CLK_CON_DIV_CLK_CHUB_USI3,          0},
    {REG_CMU_CLK_CON_DIV_CLK_CHUB_I2C,           0},
    {REG_CMU_CLK_CON_DIV_CLK_CHUB_NOC,           0},
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

void cmuDrvInit(uint32_t mainClk) {
    uint32_t regValue = 0;

    // Set MUX for CHUB NOC and CPU
    cmuDrvSetMuxClkCmuChubRcoUser(OSCCLK_RCO_CHUB);
    cmuDrvSetMuxClkCmuChubNocUser(CLKCMU_CHUB_NOC);
    cmuDrvSetMuxClkCmuChubPeriUser(CLKCMU_CHUB_PERI);

    (void)mainClk;
    cmuDrvSetCpuClk(CHUB_MAIN_CLK);

    // CHUB_CONTROLLER_OPTION
    regValue = 0;
    regValue |= (CHUB_CONTROLLER_OPTION_DEBUG_ENABLE |
                 CHUB_CONTROLLER_OPTION_LAYER2_CTRL_ENABLE |
                 CHUB_CONTROLLER_OPTION_PM_ENABLE |
                 CHUB_CONTROLLER_OPTION_HWACG_ENABLE |
                 CHUB_CONTROLLER_OPTION_MEMPG_ENABLE);

    __raw_write32(regValue, REG_CMU_CHUB_CMU_CHUB_CONTROLLER_OPTION);

    // MUX_CLK_CHUB_TIMER_FCLK
    cmuDrvSetMuxClkChubTimer(RTCCLK_CHUB);

    // Set MUX & DIV for CHUB_I2C
    cmuDrvSetMuxClkChubI2C(MUX_CLKCMU_CHUB_PERI_USER);
    cmuDrvSetDivChubI2C(DEFAULT_I2C_DIV);

    // Set MUX & DIV for CHUB_USI
    cmuDrvSetMuxClkChubUSI0(MUX_CLKCMU_CHUB_PERI_USER);
    cmuDrvSetDivChubUSI0(DEFAULT_USI_DIV);
    cmuDrvSetMuxClkChubUSI1(MUX_CLKCMU_CHUB_PERI_USER);
    cmuDrvSetDivChubUSI1(DEFAULT_USI_DIV);
    cmuDrvSetMuxClkChubUSI2(MUX_CLKCMU_CHUB_PERI_USER);
    cmuDrvSetDivChubUSI2(DEFAULT_USI_DIV);
    cmuDrvSetMuxClkChubUSI3(MUX_CLKCMU_CHUB_PERI_USER);
    cmuDrvSetDivChubUSI3(DEFAULT_USI_DIV);

    CSP_PRINTF_INFO("CMU: RcoU(%d), BusU(%d), PeriU(%d), Bus(%d)\n",
                    (int)(cmuDrvGetSpeedMuxChubRcoUser()),
                    (int)(cmuDrvGetSpeedMuxChubNocUser()),
                    (int)(cmuDrvGetSpeedMuxChubPeriUser()),
                    (int)(cmuDrvGetSpeedMuxChubNoc()));
    CSP_PRINTF_INFO("CMU: I(%d), U0(%d), U1(%d), U2(%d), U3(%d)\n",
                    (int)(cmuDrvGetSpeedMuxChubI2C()),
                    (int)(cmuDrvGetSpeedMuxChubUSI0()),
                    (int)(cmuDrvGetSpeedMuxChubUSI1()),
                    (int)(cmuDrvGetSpeedMuxChubUSI2()),
                    (int)(cmuDrvGetSpeedMuxChubUSI3()));
}

static uint32_t cmuDrvGetSpeedMuxChubNocUser(void)
{
    return (__raw_read32(REG_CMU_PLL_CON0_MUX_CLKCMU_CHUB_NOC_USER) & MUX_USER_SEL)? RCO_400_FREQ : CLK_RCO_FREQ;
}

static uint32_t cmuDrvGetSpeedMuxChubPeriUser(void)
{
    return (__raw_read32(REG_CMU_PLL_CON0_MUX_CLKCMU_CHUB_PERI_USER) & MUX_USER_SEL)? RCO_400_FREQ : CLK_RCO_FREQ;
}

static uint32_t cmuDrvGetSpeedMuxChubRcoUser(void)
{
    return (__raw_read32(REG_CMU_PLL_CON0_MUX_CLKCMU_CHUB_RCO_USER) & MUX_USER_SEL)? OSCCLK_RCO_FREQ : CLK_RCO_FREQ;
}

static uint32_t cmuDrvGetSpeedMuxChubNoc(void)
{
    uint32_t speed = 0;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_CLK_CHUB_NOC) & MUX_SEL)
        speed = cmuDrvGetSpeedMuxChubNocUser();
    else
        speed = cmuDrvGetSpeedMuxChubRcoUser();

    return speed;
}

static uint32_t cmuDrvGetSpeedMuxChubUSI0(void)
{
    uint32_t speed = 0;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_CLK_CHUB_USI0) & MUX_SEL)
        speed = cmuDrvGetSpeedMuxChubPeriUser();
    else
        speed = cmuDrvGetSpeedMuxChubRcoUser();

    return speed;
}

static uint32_t cmuDrvGetSpeedMuxChubUSI1(void)
{
    uint32_t speed = 0;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_CLK_CHUB_USI1) & MUX_SEL)
        speed = cmuDrvGetSpeedMuxChubPeriUser();
    else
        speed = cmuDrvGetSpeedMuxChubRcoUser();

    return speed;
}

static uint32_t cmuDrvGetSpeedMuxChubUSI2(void)
{
    uint32_t speed = 0;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_CLK_CHUB_USI2) & MUX_SEL)
        speed = cmuDrvGetSpeedMuxChubPeriUser();
    else
        speed = cmuDrvGetSpeedMuxChubRcoUser();

    return speed;
}

static uint32_t cmuDrvGetSpeedMuxChubUSI3(void)
{
    uint32_t speed = 0;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_CLK_CHUB_USI3) & MUX_SEL)
        speed = cmuDrvGetSpeedMuxChubPeriUser();
    else
        speed = cmuDrvGetSpeedMuxChubRcoUser();

    return speed;
}

static uint32_t cmuDrvGetSpeedMuxChubI2C(void)
{
    uint32_t speed = 0;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_CLK_CHUB_I2C) & MUX_SEL)
        speed = cmuDrvGetSpeedMuxChubPeriUser();
    else
        speed = cmuDrvGetSpeedMuxChubRcoUser();

    return speed;
}

static uint32_t cmuDrvGetSpeedMuxChubTimer(void)
{
    uint32_t speed = 0;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_CLK_CHUB_TIMER) & MUX_SEL)
        speed = RTC_FREQ;
    else
        speed = OSCCLK_RCO_FREQ;

    return speed;
}

#if defined(SEOS)
static uint32_t __attribute__((unused)) cmuDrvGetSpeedMuxCmgpI2C(void) {
#else
static uint32_t cmuDrvGetSpeedMuxCmgpI2C(void) {
#endif
    uint32_t speed;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_CLK_CMGP_I2C) & MUX_SEL)
        speed = RCO_400_FREQ;
    else
        speed = CLK_RCO_FREQ;

    return speed;
}

#if defined(SEOS)
static uint32_t __attribute__((unused)) cmuDrvGetSpeedMuxCmgpUSI0(void) {
#else
static uint32_t cmuDrvGetSpeedMuxCmgpUSI0(void) {
#endif
    uint32_t speed;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_CLK_CMGP_USI0) & MUX_SEL)
        speed = RCO_400_FREQ;
    else
        speed = CLK_RCO_FREQ;

    return speed;
}

#if defined(SEOS)
static uint32_t __attribute__((unused)) cmuDrvGetSpeedMuxCmgpUSI1(void) {
#else
static uint32_t cmuDrvGetSpeedMuxCmgpUSI1(void) {
#endif
    uint32_t speed;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_CLK_CMGP_USI1) & MUX_SEL)
        speed = RCO_400_FREQ;
    else
        speed = CLK_RCO_FREQ;

    return speed;
}

#if defined(SEOS)
static uint32_t __attribute__((unused)) cmuDrvGetSpeedMuxCmgpUSI2(void) {
#else
static uint32_t cmuDrvGetSpeedMuxCmgpUSI2(void) {
#endif
    uint32_t speed;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_CLK_CMGP_USI2) & MUX_SEL)
        speed = RCO_400_FREQ;
    else
        speed = CLK_RCO_FREQ;

    return speed;
}

#if defined(SEOS)
static uint32_t __attribute__((unused)) cmuDrvGetSpeedMuxCmgpUSI3(void) {
#else
static uint32_t cmuDrvGetSpeedMuxCmgpUSI3(void) {
#endif
    uint32_t speed;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_CLK_CMGP_USI3) & MUX_SEL)
        speed = RCO_400_FREQ;
    else
        speed = CLK_RCO_FREQ;

    return speed;
}

#if defined(SEOS)
static uint32_t __attribute__((unused)) cmuDrvGetSpeedMuxCmgpUSI4(void) {
#else
static uint32_t cmuDrvGetSpeedMuxCmgpUSI4(void) {
#endif
    uint32_t speed;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_CLK_CMGP_USI4) & MUX_SEL)
        speed = RCO_400_FREQ;
    else
        speed = CLK_RCO_FREQ;

    return speed;
}

#if defined(SEOS)
static uint32_t __attribute__((unused)) cmuDrvGetSpeedMuxCmgpUSI5(void) {
#else
static uint32_t cmuDrvGetSpeedMuxCmgpUSI5(void) {
#endif
    uint32_t speed;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_CLK_CMGP_USI5) & MUX_SEL)
        speed = RCO_400_FREQ;
    else
        speed = CLK_RCO_FREQ;

    return speed;
}

#if defined(SEOS)
static uint32_t __attribute__((unused)) cmuDrvGetSpeedMuxCmgpUSI6(void) {
#else
static uint32_t cmuDrvGetSpeedMuxCmgpUSI6(void) {
#endif
    uint32_t speed;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_CLK_CMGP_USI6) & MUX_SEL)
        speed = RCO_400_FREQ;
    else
        speed = CLK_RCO_FREQ;

    return speed;
}

static void cmuDrvSetMuxClkCmuChubRcoUser(int select) {
    uint32_t regValue;

    regValue = __raw_read32(REG_CMU_PLL_CON0_MUX_CLKCMU_CHUB_RCO_USER);

    if (select)
        regValue |= MUX_USER_SEL;
    else
        regValue &= ~MUX_USER_SEL;

    __raw_write32(regValue, REG_CMU_PLL_CON0_MUX_CLKCMU_CHUB_RCO_USER);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_PLL_CON0_MUX_CLKCMU_CHUB_RCO_USER) & MUX_BUSY);
}

static void cmuDrvSetMuxClkCmuChubNocUser(int select) {
    uint32_t regValue;

    regValue = __raw_read32(REG_CMU_PLL_CON0_MUX_CLKCMU_CHUB_NOC_USER);

    if (select)
        regValue |= MUX_USER_SEL;
    else
        regValue &= ~MUX_USER_SEL;

    __raw_write32(regValue, REG_CMU_PLL_CON0_MUX_CLKCMU_CHUB_NOC_USER);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_PLL_CON0_MUX_CLKCMU_CHUB_NOC_USER) & MUX_BUSY);
}

static void cmuDrvSetMuxClkCmuChubPeriUser(int select) {
    uint32_t regValue;

    regValue = __raw_read32(REG_CMU_PLL_CON0_MUX_CLKCMU_CHUB_PERI_USER);

    if (select)
        regValue |= MUX_USER_SEL;
    else
        regValue &= ~MUX_USER_SEL;

    __raw_write32(regValue, REG_CMU_PLL_CON0_MUX_CLKCMU_CHUB_PERI_USER);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_PLL_CON0_MUX_CLKCMU_CHUB_PERI_USER) & MUX_BUSY);
}

static void cmuDrvSetMuxClkChubNoc(int select) {
    uint32_t regValue;

    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_CLK_CHUB_NOC);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_CLK_CHUB_NOC);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_CLK_CHUB_NOC) & MUX_BUSY);
}

static void cmuDrvSetMuxClkChubI2C(int select) {
    uint32_t regValue;

    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_CLK_CHUB_I2C);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_CLK_CHUB_I2C);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_CLK_CHUB_I2C) & MUX_BUSY);
}

static void cmuDrvSetMuxClkChubTimer(int select) {
    uint32_t regValue;

    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_CLK_CHUB_TIMER);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_CLK_CHUB_TIMER);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_CLK_CHUB_TIMER) & MUX_BUSY);
}

static void cmuDrvSetMuxClkChubUSI0(int select) {
    uint32_t regValue;

    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_CLK_CHUB_USI0);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_CLK_CHUB_USI0);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_CLK_CHUB_USI0) & MUX_BUSY);
}

static void cmuDrvSetMuxClkChubUSI1(int select) {
    uint32_t regValue;

    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_CLK_CHUB_USI1);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_CLK_CHUB_USI1);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_CLK_CHUB_USI1) & MUX_BUSY);
}

static void cmuDrvSetMuxClkChubUSI2(int select) {
    uint32_t regValue;

    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_CLK_CHUB_USI2);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_CLK_CHUB_USI2);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_CLK_CHUB_USI2) & MUX_BUSY);
}

static void cmuDrvSetMuxClkChubUSI3(int select) {
    uint32_t regValue;

    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_CLK_CHUB_USI3);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_CLK_CHUB_USI3);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_CLK_CHUB_USI3) & MUX_BUSY);
}

static void cmuDrvSetDivChubNoc(IN uint32_t divider) {
    uint32_t regValue;

    if (divider == 0)
        return;

    divider = (divider - 1) & DIV_RATIO_MASK_2BIT;

    regValue = (__raw_read32(REG_CMU_CLK_CON_DIV_CLK_CHUB_NOC) & ~DIV_RATIO_MASK_2BIT) | divider;
    __raw_write32(regValue, REG_CMU_CLK_CON_DIV_CLK_CHUB_NOC);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_CLK_CHUB_NOC) & DIV_BUSY);
}

static void cmuDrvSetDivChubI2C(IN uint32_t divider) {
    uint32_t regValue;

    if (divider == 0)
        return;

    divider = (divider - 1) & DIV_RATIO_MASK_4BIT;

    regValue = (__raw_read32(REG_CMU_CLK_CON_DIV_CLK_CHUB_I2C) & ~DIV_RATIO_MASK_4BIT) | divider;
    __raw_write32(regValue, REG_CMU_CLK_CON_DIV_CLK_CHUB_I2C);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_CLK_CHUB_I2C) & DIV_BUSY);
}

static void cmuDrvSetDivChubUSI0(IN uint32_t divider) {
    uint32_t regValue;

    if (divider == 0)
        return;

    divider = (divider - 1) & DIV_RATIO_MASK_4BIT;

    regValue = (__raw_read32(REG_CMU_CLK_CON_DIV_CLK_CHUB_USI0) & ~DIV_RATIO_MASK_4BIT) | divider;
    __raw_write32(regValue, REG_CMU_CLK_CON_DIV_CLK_CHUB_USI0);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_CLK_CHUB_USI0) & DIV_BUSY);
}

static void cmuDrvSetDivChubUSI1(IN uint32_t divider) {
    uint32_t regValue;

    if (divider == 0)
        return;

    divider = (divider - 1) & DIV_RATIO_MASK_4BIT;

    regValue = (__raw_read32(REG_CMU_CLK_CON_DIV_CLK_CHUB_USI1) & ~DIV_RATIO_MASK_4BIT) | divider;
    __raw_write32(regValue, REG_CMU_CLK_CON_DIV_CLK_CHUB_USI1);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_CLK_CHUB_USI1) & DIV_BUSY);
}

static void cmuDrvSetDivChubUSI2(IN uint32_t divider) {
    uint32_t regValue;

    if (divider == 0)
        return;

    divider = (divider - 1) & DIV_RATIO_MASK_4BIT;

    regValue = (__raw_read32(REG_CMU_CLK_CON_DIV_CLK_CHUB_USI2) & ~DIV_RATIO_MASK_4BIT) | divider;
    __raw_write32(regValue, REG_CMU_CLK_CON_DIV_CLK_CHUB_USI2);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_CLK_CHUB_USI2) & DIV_BUSY);
}

static void cmuDrvSetDivChubUSI3(IN uint32_t divider) {
    uint32_t regValue;

    if (divider == 0)
        return;

    divider = (divider - 1) & DIV_RATIO_MASK_4BIT;

    regValue = (__raw_read32(REG_CMU_CLK_CON_DIV_CLK_CHUB_USI3) & ~DIV_RATIO_MASK_4BIT) | divider;
    __raw_write32(regValue, REG_CMU_CLK_CON_DIV_CLK_CHUB_USI3);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_CLK_CHUB_USI3) & DIV_BUSY);
}

static uint32_t cmuDrvGetDivChubBus(void) {
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_CLK_CHUB_NOC) & DIV_RATIO_MASK_2BIT) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivChubI2C(void) {
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_CLK_CHUB_I2C) & DIV_RATIO_MASK_4BIT) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivChubUSI0(void) {
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_CLK_CHUB_USI0) & DIV_RATIO_MASK_4BIT) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivChubUSI1(void) {
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_CLK_CHUB_USI1) & DIV_RATIO_MASK_4BIT) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivChubUSI2(void) {
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_CLK_CHUB_USI2) & DIV_RATIO_MASK_4BIT) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivChubUSI3(void) {
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_CLK_CHUB_USI3) & DIV_RATIO_MASK_4BIT) + 1;

    return divider;
}

#if defined(SEOS)
static void __attribute__((unused)) cmuDrvSetMuxClkCmgpI2C(int select) {
#else
static void cmuDrvSetMuxClkCmgpI2C(int select) {
#endif
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_CLK_CMGP_I2C);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_CLK_CMGP_I2C);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_CLK_CMGP_I2C) & MUX_BUSY);
}

#if defined(SEOS)
static void __attribute__((unused)) cmuDrvSetMuxClkCmgpUSI0(int select) {
#else
static void cmuDrvSetMuxClkCmgpUSI0(int select) {
#endif
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_CLK_CMGP_USI0);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_CLK_CMGP_USI0);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_CLK_CMGP_USI0) & MUX_BUSY);
}

#if defined(SEOS)
static void __attribute__((unused)) cmuDrvSetMuxClkCmgpUSI1(int select) {
#else
static void cmuDrvSetMuxClkCmgpUSI1(int select) {
#endif
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_CLK_CMGP_USI1);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_CLK_CMGP_USI1);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_CLK_CMGP_USI1) & MUX_BUSY);
}

#if defined(SEOS)
static void __attribute__((unused)) cmuDrvSetMuxClkCmgpUSI2(int select) {
#else
static void cmuDrvSetMuxClkCmgpUSI2(int select) {
#endif
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_CLK_CMGP_USI2);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_CLK_CMGP_USI2);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_CLK_CMGP_USI2) & MUX_BUSY);
}

#if defined(SEOS)
static void __attribute__((unused)) cmuDrvSetMuxClkCmgpUSI3(int select) {
#else
static void cmuDrvSetMuxClkCmgpUSI3(int select) {
#endif
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_CLK_CMGP_USI3);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_CLK_CMGP_USI3);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_CLK_CMGP_USI3) & MUX_BUSY);
}

#if defined(SEOS)
static void __attribute__((unused)) cmuDrvSetMuxClkCmgpUSI4(int select) {
#else
static void cmuDrvSetMuxClkCmgpUSI4(int select) {
#endif
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_CLK_CMGP_USI4);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_CLK_CMGP_USI4);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_CLK_CMGP_USI4) & MUX_BUSY);
}

#if defined(SEOS)
static void __attribute__((unused)) cmuDrvSetMuxClkCmgpUSI5(int select) {
#else
static void cmuDrvSetMuxClkCmgpUSI5(int select) {
#endif
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_CLK_CMGP_USI5);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_CLK_CMGP_USI5);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_CLK_CMGP_USI5) & MUX_BUSY);
}

#if defined(SEOS)
static void __attribute__((unused)) cmuDrvSetMuxClkCmgpUSI6(int select) {
#else
static void cmuDrvSetMuxClkCmgpUSI6(int select) {
#endif
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_CLK_CMGP_USI6);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_CLK_CMGP_USI6);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_CLK_CMGP_USI6) & MUX_BUSY);
}

static void cmuDrvSetDivCmgpI2C(IN uint32_t divider) {
    if (divider == 0)
        return;

    // divider value is clear and set to given divider value
    divider = (divider - 1) & DIV_RATIO_MASK_4BIT;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_MUX_CLK_CMGP_I2C) & ~DIV_RATIO_MASK_4BIT) | divider),
                  REG_CMU_CLK_CON_MUX_CLK_CMGP_I2C);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_CLK_CMGP_I2C) & DIV_BUSY);
}

static void cmuDrvSetDivCmgpUSI0(IN uint32_t divider) {
    if (divider == 0)
        return;

    // divider value is clear and set to given divider value
    divider = (divider - 1) & DIV_RATIO_MASK_4BIT;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_CLK_CMGP_USI00_USI) & ~DIV_RATIO_MASK_4BIT) | divider),
                  REG_CMU_CLK_CON_DIV_CLK_CMGP_USI00_USI);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_CLK_CMGP_USI00_USI) & DIV_BUSY);
}

static void cmuDrvSetDivCmgpUSI1(IN uint32_t divider) {
    if (divider == 0)
        return;

    // divider value is clear and set to given divider value
    divider = (divider - 1) & DIV_RATIO_MASK_4BIT;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_CLK_CMGP_USI01_USI) & ~DIV_RATIO_MASK_4BIT) | divider),
                  REG_CMU_CLK_CON_DIV_CLK_CMGP_USI01_USI);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_CLK_CMGP_USI01_USI) & DIV_BUSY);
}

static void cmuDrvSetDivCmgpUSI2(IN uint32_t divider) {
    if (divider == 0)
        return;

    // divider value is clear and set to given divider value
    divider = (divider - 1) & DIV_RATIO_MASK_4BIT;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_CLK_CMGP_USI02_USI) & ~DIV_RATIO_MASK_4BIT) | divider),
                  REG_CMU_CLK_CON_DIV_CLK_CMGP_USI02_USI);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_CLK_CMGP_USI02_USI) & DIV_BUSY);
}

static void cmuDrvSetDivCmgpUSI3(IN uint32_t divider) {
    if (divider == 0)
        return;

    // divider value is clear and set to given divider value
    divider = (divider - 1) & DIV_RATIO_MASK_4BIT;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_CLK_CMGP_USI03_USI) & ~DIV_RATIO_MASK_4BIT) | divider),
                  REG_CMU_CLK_CON_DIV_CLK_CMGP_USI03_USI);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_CLK_CMGP_USI03_USI) & DIV_BUSY);
}

static void cmuDrvSetDivCmgpUSI4(IN uint32_t divider) {
    if (divider == 0)
        return;

    // divider value is clear and set to given divider value
    divider = (divider - 1) & DIV_RATIO_MASK_4BIT;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_CLK_CMGP_USI04_USI) & ~DIV_RATIO_MASK_4BIT) | divider),
                  REG_CMU_CLK_CON_DIV_CLK_CMGP_USI04_USI);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_CLK_CMGP_USI04_USI) & DIV_BUSY);
}

static void cmuDrvSetDivCmgpUSI5(IN uint32_t divider) {
    if (divider == 0)
        return;

    // divider value is clear and set to given divider value
    divider = (divider - 1) & DIV_RATIO_MASK_4BIT;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_CLK_CMGP_USI05_USI) & ~DIV_RATIO_MASK_4BIT) | divider),
                  REG_CMU_CLK_CON_DIV_CLK_CMGP_USI05_USI);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_CLK_CMGP_USI05_USI) & DIV_BUSY);
}

static void cmuDrvSetDivCmgpUSI6(IN uint32_t divider) {
    if (divider == 0)
        return;

    // divider value is clear and set to given divider value
    divider = (divider - 1) & DIV_RATIO_MASK_4BIT;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_CLK_CMGP_USI06_USI) & ~DIV_RATIO_MASK_4BIT) | divider),
                  REG_CMU_CLK_CON_DIV_CLK_CMGP_USI06_USI);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_CLK_CMGP_USI06_USI) & DIV_BUSY);
}

static uint32_t cmuDrvGetDivCmgpI2C(void) {
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_MUX_CLK_CMGP_I2C) & DIV_RATIO_MASK_4BIT) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivCmgpUSI0(void) {
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_CLK_CMGP_USI00_USI) & DIV_RATIO_MASK_4BIT) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivCmgpUSI1(void) {
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_CLK_CMGP_USI01_USI) & DIV_RATIO_MASK_4BIT) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivCmgpUSI2(void) {
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_CLK_CMGP_USI02_USI) & DIV_RATIO_MASK_4BIT) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivCmgpUSI3(void) {
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_CLK_CMGP_USI03_USI) & DIV_RATIO_MASK_4BIT) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivCmgpUSI4(void) {
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_CLK_CMGP_USI04_USI) & DIV_RATIO_MASK_4BIT) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivCmgpUSI5(void) {
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_CLK_CMGP_USI05_USI) & DIV_RATIO_MASK_4BIT) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivCmgpUSI6(void) {
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_CLK_CMGP_USI06_USI) & DIV_RATIO_MASK_4BIT) + 1;

    return divider;
}

uint32_t cmuDrvSetCpuMux(uint32_t mux)
{
    uint32_t prev;

    prev = __raw_read32(REG_CMU_CLK_CON_MUX_CLK_CHUB_NOC) & MUX_SEL;
    if (mux == CPU_MUX_LOW)
        cmuDrvSetMuxClkChubNoc(MUX_CLKCMU_CHUB_RCO_USER);
    else
        cmuDrvSetMuxClkChubNoc(MUX_CLKCMU_CHUB_NOC_USER);

    return prev;
}

int cmuDrvSetCpuClk(uint32_t freq)
{
    uint32_t div;


    if (freq <= CLK_RCO_FREQ) {
        // Set Input Clock : CLK_RCO_CHUB (49152000)
        cmuDrvSetMuxClkChubNoc(MUX_CLKCMU_CHUB_RCO_USER);
        div = cmuDrvGetDivValue(CLK_RCO_FREQ, freq, 4);
    } else {
        // Set Input Clock : HFRCO_CHUB (393216000)
        cmuDrvSetMuxClkChubNoc(MUX_CLKCMU_CHUB_NOC_USER);
        div = cmuDrvGetDivValue(HFRCO_FREQ, freq, 4);
    }
    cmuDrvSetDivChubNoc(div);
    tChubMainClk = cmuDrvGetSpeed(CMU_CLK_CHUB_BUS);

    //CSP_PRINTF_INFO("cmuDrvSetCpuClk div%lu, freq:%lu\n", div, tChubMainClk);
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
            return cmuDrvGetSpeedMuxChubI2C();
        case CMU_MUX_CHUB_USI0:
        case CMU_CLK_CHUB_SPI0:
            return cmuDrvGetSpeedMuxChubUSI0();
        case CMU_MUX_CHUB_USI1:
        case CMU_CLK_CHUB_SPI1:
            return cmuDrvGetSpeedMuxChubUSI1();
        case CMU_MUX_CHUB_USI2:
        case CMU_CLK_CHUB_SPI2:
            return cmuDrvGetSpeedMuxChubUSI2();
        case CMU_MUX_CHUB_USI3:
        case CMU_CLK_CHUB_SPI3:
            return cmuDrvGetSpeedMuxChubUSI3();

        case CMU_DIV_CHUB_NOC:
        case CMU_CLK_OUTPUT_CPU:
            return cmuDrvGetSpeedMuxChubNoc() / cmuDrvGetDivChubBus();
        case CMU_DIV_CHUB_USI0:
        case CMU_CLK_CHUB_I2C0:
            return cmuDrvGetSpeedMuxChubUSI0() / cmuDrvGetDivChubUSI0();
        case CMU_DIV_CHUB_USI1:
        case CMU_CLK_CHUB_I2C1:
            return cmuDrvGetSpeedMuxChubUSI1() / cmuDrvGetDivChubUSI1();
        case CMU_DIV_CHUB_USI2:
        case CMU_CLK_CHUB_I2C3:
            return cmuDrvGetSpeedMuxChubUSI2() / cmuDrvGetDivChubUSI2();
        case CMU_DIV_CHUB_USI3:
        case CMU_CLK_CHUB_I2C4:
            return cmuDrvGetSpeedMuxChubUSI3() / cmuDrvGetDivChubUSI3();
        case CMU_DIV_CHUB_I2C:
        case CMU_CLK_CHUB_I2C2:
        case CMU_CLK_CHUB_I2C5:
            return cmuDrvGetSpeedMuxChubI2C() / cmuDrvGetDivChubI2C();

        case CMU_MUX_CMGP_I2C:
            return cmuDrvGetSpeedMuxCmgpI2C();
        case CMU_MUX_CMGP_USI0:
        case CMU_CLK_CMGP_SPI0:
            return cmuDrvGetSpeedMuxCmgpUSI0();
        case CMU_MUX_CMGP_USI1:
        case CMU_CLK_CMGP_SPI1:
            return cmuDrvGetSpeedMuxCmgpUSI1();
        case CMU_MUX_CMGP_USI2:
        case CMU_CLK_CMGP_SPI2:
            return cmuDrvGetSpeedMuxCmgpUSI2();
        case CMU_MUX_CMGP_USI3:
        case CMU_CLK_CMGP_SPI3:
            return cmuDrvGetSpeedMuxCmgpUSI3();
        case CMU_MUX_CMGP_USI4:
        case CMU_CLK_CMGP_SPI4:
            return cmuDrvGetSpeedMuxCmgpUSI4();
        case CMU_MUX_CMGP_USI5:
        case CMU_CLK_CMGP_SPI5:
            return cmuDrvGetSpeedMuxCmgpUSI5();
        case CMU_MUX_CMGP_USI6:
        case CMU_CLK_CMGP_SPI6:
            return cmuDrvGetSpeedMuxCmgpUSI6();

        case CMU_DIV_CMGP_I2C:
        case CMU_CLK_CMGP_I2C1:
        case CMU_CLK_CMGP_I2C3:
        case CMU_CLK_CMGP_I2C5:
        case CMU_CLK_CMGP_I2C7:
        case CMU_CLK_CMGP_I2C9:
        case CMU_CLK_CMGP_I2C11:
        case CMU_CLK_CMGP_I2C13:
            return cmuDrvGetSpeedMuxCmgpI2C() / cmuDrvGetDivCmgpI2C();

        case CMU_DIV_CMGP_USI0:
        case CMU_CLK_CMGP_I2C0:
            return cmuDrvGetSpeedMuxCmgpUSI0() / cmuDrvGetDivCmgpUSI0();
        case CMU_DIV_CMGP_USI1:
        case CMU_CLK_CMGP_I2C2:
            return cmuDrvGetSpeedMuxCmgpUSI1() / cmuDrvGetDivCmgpUSI1();
        case CMU_DIV_CMGP_USI2:
        case CMU_CLK_CMGP_I2C4:
            return cmuDrvGetSpeedMuxCmgpUSI2() / cmuDrvGetDivCmgpUSI2();
        case CMU_DIV_CMGP_USI3:
        case CMU_CLK_CMGP_I2C6:
            return cmuDrvGetSpeedMuxCmgpUSI3() / cmuDrvGetDivCmgpUSI3();
        case CMU_DIV_CMGP_USI4:
        case CMU_CLK_CMGP_I2C8:
            return cmuDrvGetSpeedMuxCmgpUSI4() / cmuDrvGetDivCmgpUSI4();
        case CMU_DIV_CMGP_USI5:
        case CMU_CLK_CMGP_I2C10:
            return cmuDrvGetSpeedMuxCmgpUSI5() / cmuDrvGetDivCmgpUSI5();
        case CMU_DIV_CMGP_USI6:
        case CMU_CLK_CMGP_I2C12:
            return cmuDrvGetSpeedMuxCmgpUSI6() / cmuDrvGetDivCmgpUSI6();

        default:
            break;
    }

    return 0;
}

bool cmuDrvSetDivider(IN CmuIpType ip, IN uint32_t divider)
{
    switch(ip) {
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
                cmuDrvSetDivChubI2C(divider);
            break;

        case CMU_DIV_CMGP_USI0:
        case CMU_CLK_CMGP_SPI0:
                cmuDrvSetDivCmgpUSI0(divider);
            break;

        case CMU_DIV_CMGP_USI1:
        case CMU_CLK_CMGP_SPI1:
                cmuDrvSetDivCmgpUSI1(divider);
            break;

        case CMU_DIV_CMGP_USI2:
        case CMU_CLK_CMGP_SPI2:
                cmuDrvSetDivCmgpUSI2(divider);
            break;

        case CMU_DIV_CMGP_USI3:
        case CMU_CLK_CMGP_SPI3:
                cmuDrvSetDivCmgpUSI3(divider);
            break;

        case CMU_DIV_CMGP_USI4:
        case CMU_CLK_CMGP_SPI4:
                cmuDrvSetDivCmgpUSI4(divider);
            break;

        case CMU_DIV_CMGP_USI5:
        case CMU_CLK_CMGP_SPI5:
                cmuDrvSetDivCmgpUSI5(divider);
            break;

        case CMU_DIV_CMGP_USI6:
        case CMU_CLK_CMGP_SPI6:
                cmuDrvSetDivCmgpUSI6(divider);
            break;

        case CMU_DIV_CMGP_I2C:
                cmuDrvSetDivCmgpI2C(divider);
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

