/*----------------------------------------------------------------------------
 *      Exynos SoC  -  CMU
 *----------------------------------------------------------------------------
 *      Name:    cmuDrv9815.c
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

static uint32_t tChubMainClk = CHUB_MAIN_CLK;

static void cmuDrvSetMuxClkCmuChubRcoUser(int);
static void cmuDrvSetMuxClkCmuChubBusUser(int);
static void cmuDrvSetMuxClkCmuChubPeriUser(int);
static void cmuDrvSetMuxClkChubBus(int);
static void cmuDrvSetMuxClkChubI2C(int);
static void cmuDrvSetMuxClkChubUSI0(int);
static void cmuDrvSetMuxClkChubUSI1(int);
static void cmuDrvSetMuxClkChubUSI2(int);
static void cmuDrvSetMuxClkChubTimer(int);
static void cmuDrvSetMuxClkChubDmailbox(int);
static void cmuDrvSetMuxClkChubHpmbus(int);
static uint32_t cmuDrvGetMuxClkCmuChubRcoUser(void);
static uint32_t cmuDrvGetMuxClkCmuChubBusUser(void);
static uint32_t cmuDrvGetMuxClkCmuChubPeriUser(void);
static uint32_t cmuDrvGetMuxClkChubBus(void);
static uint32_t cmuDrvGetMuxClkChubI2C(void);
static uint32_t cmuDrvGetMuxClkChubUSI0(void);
static uint32_t cmuDrvGetMuxClkChubUSI1(void);
static uint32_t cmuDrvGetMuxClkChubUSI2(void);
static uint32_t cmuDrvGetMuxClkChubTimer(void);
static uint32_t cmuDrvGetMuxClkChubDmailbox(void);
static uint32_t cmuDrvGetMuxClkChubHpmbus(void);
static void cmuDrvSetDivChubBus(IN uint32_t divider);
static void cmuDrvSetDivChubI2C(IN uint32_t divider);
static void cmuDrvSetDivChubUSI0(IN uint32_t divider);
static void cmuDrvSetDivChubUSI1(IN uint32_t divider);
static void cmuDrvSetDivChubUSI2(IN uint32_t divider);
static void cmuDrvSetDivChubDmailbox(IN uint32_t divider);
static void cmuDrvSetDivChubDmailboxCore(IN uint32_t divider);
static void cmuDrvSetDivChubHpmbus(IN uint32_t divider);
static void cmuDrvSetDivChubSerialLifBclk(IN uint32_t divider);
static void cmuDrvSetDivChubSerialLifCore(IN uint32_t divider);
static uint32_t cmuDrvGetDivChubBus(void);
static uint32_t cmuDrvGetDivChubI2C(void);
static uint32_t cmuDrvGetDivChubUSI0(void);
static uint32_t cmuDrvGetDivChubUSI1(void);
static uint32_t cmuDrvGetDivChubUSI2(void);
static uint32_t cmuDrvGetDivChubDmailbox(void);
static uint32_t cmuDrvGetDivChubDmailboxCore(void);
static uint32_t cmuDrvGetDivChubHpmbus(void);
static uint32_t cmuDrvGetDivChubSerialLifBclk(void);
static uint32_t cmuDrvGetDivChubSerialLifCore(void);
void cmuDrvCMGPWarning(const char* func);

#define DEFAULT_BUS_DIV                2
#define DEFAULT_I2C_DIV                2
#define DEFAULT_USI_DIV                1
#define DEFAULT_SERIAL_LIF_BCLK_DIV    16
#define DEFAULT_SERIAL_LIF_CORE_DIV    1
#define DEFAULT_DMAILBOX_DIV           7
#define DEFAULT_DMAILBOX_CORE_DIV      2
#define DEFAULT_HPMBUS_DIV             8

void cmuDrvInit(uint32_t mainclk)
{
    uint32_t val = 0;

    //if (mainclk)
    //    tChubMainClk = mainclk;

    // Initializes mux and divider
    // !!! Make sure that clock source for CHUB, CLKCMU_CHUB_BUS_USER is 393Mhz and provided at this initializing time

    // MUX_CLKCMU_CHUB_RCO_USER
    // Select CLK_RCO_CHUB : 49.152 MHz
    cmuDrvSetMuxClkCmuChubRcoUser(CLK_RCO_CHUB);

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
    cmuDrvSetMuxClkCmuChubBusUser(CLKCMU_CHUB_BUS);

    // MUX_CLKCMU_CHUB_PERI_USER
    // Select CLKCMU_CHUB_PERI : 393.216 MHz
    cmuDrvSetMuxClkCmuChubPeriUser(CLKCMU_CHUB_PERI);

    // MUX_CLK_CHUB_BUS
    cmuDrvSetMuxClkChubBus(MUX_CLK_CHUB_BUS_USER);

    // DIV_CLK_CHUB_BUS
    cmuDrvSetDivChubBus(DEFAULT_BUS_DIV);

    // MUX_CLK_CHUB_TIMER_FCLK
    cmuDrvSetMuxClkChubTimer(RTCCLK_CHUB);

    // MUX_CLK_CHUB_I2C
    cmuDrvSetMuxClkChubI2C(MUX_CLK_CHUB_RCO_USER);
    // DIV_CLK_CHUB_I2C
    cmuDrvSetDivChubI2C(DEFAULT_I2C_DIV);

    // MUX_CLK_CHUB_USI0
    cmuDrvSetMuxClkChubUSI0(MUX_CLK_CHUB_RCO_USER);
    // DIV_CLK_CHUB_USI0
    cmuDrvSetDivChubUSI0(DEFAULT_USI_DIV);

    // MUX_CLK_CHUB_USI1
    cmuDrvSetMuxClkChubUSI1(MUX_CLK_CHUB_RCO_USER);
    // DIV_CLK_CHUB_USI1
    cmuDrvSetDivChubUSI1(DEFAULT_USI_DIV);

    // MUX_CLK_CHUB_USI2
    cmuDrvSetMuxClkChubUSI2(MUX_CLK_CHUB_RCO_USER);
    // DIV_CLK_CHUB_USI2
    cmuDrvSetDivChubUSI2(DEFAULT_USI_DIV);

    // MUX_CLK_CHUB_DMAILBOX
    cmuDrvSetMuxClkChubDmailbox(MUX_CLK_CHUB_RCO_USER);
    cmuDrvSetDivChubDmailbox(DEFAULT_DMAILBOX_DIV);
    cmuDrvSetDivChubDmailboxCore(DEFAULT_DMAILBOX_CORE_DIV);

    cmuDrvSetMuxClkChubHpmbus(MUX_CLK_CHUB_RCO_USER);
    cmuDrvSetDivChubHpmbus(DEFAULT_HPMBUS_DIV);

    cmuDrvSetDivChubSerialLifBclk(DEFAULT_SERIAL_LIF_BCLK_DIV);
    cmuDrvSetDivChubSerialLifCore(DEFAULT_SERIAL_LIF_CORE_DIV);

    CSP_PRINTF_INFO("CMU: RcoU( %dMHz ), BusU( %dMHz )\n",
                    (int)(cmuDrvGetMuxClkCmuChubRcoUser()/1000000),
                    (int)(cmuDrvGetMuxClkCmuChubBusUser()/1000000));
    CSP_PRINTF_INFO("CMU: B(%dM), D(%dM), H(%dM), T(%dK),\n",
                    (int)(cmuDrvGetMuxClkChubBus()/1000000),
                    (int)(cmuDrvGetMuxClkChubDmailbox()/1000000),
                    (int)(cmuDrvGetMuxClkChubHpmbus()/1000000),
                    (int)(cmuDrvGetMuxClkChubTimer()/1000));
    CSP_PRINTF_INFO("CMU: I(%dM), U0(%dM), U1(%dM), U2(%dM)\n",
                    (int)(cmuDrvGetMuxClkChubI2C()/1000000),
                    (int)(cmuDrvGetMuxClkChubUSI0()/1000000),
                    (int)(cmuDrvGetMuxClkChubUSI1()/1000000),
                    (int)(cmuDrvGetMuxClkChubUSI2()/1000000));
}

static void cmuDrvSetMuxClkCmuChubRcoUser(int select)
{
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_PLL_CON0_MUX_CLK_CHUB_RCO_USER);

    if (select)
        regValue |= MUX_USER_SEL;
    else
        regValue &= ~MUX_USER_SEL;

    __raw_write32(regValue, REG_CMU_PLL_CON0_MUX_CLK_CHUB_RCO_USER);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_PLL_CON0_MUX_CLK_CHUB_RCO_USER) & MUX_BUSY);
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

static void cmuDrvSetMuxClkCmuChubPeriUser(int select)
{
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_PLL_CON0_MUX_CLK_CHUB_PERI_USER);

    if (select)
        regValue |= MUX_USER_SEL;
    else
        regValue &= ~MUX_USER_SEL;

    __raw_write32(regValue, REG_CMU_PLL_CON0_MUX_CLK_CHUB_PERI_USER);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_PLL_CON0_MUX_CLK_CHUB_PERI_USER) & MUX_BUSY);
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

static void cmuDrvSetMuxClkChubDmailbox(int select)
{
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_DMAILBOX);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_DMAILBOX);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_DMAILBOX) & MUX_BUSY);
}

static void cmuDrvSetMuxClkChubHpmbus(int select)
{
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_HPMBUS);

    if (select)
        regValue |= MUX_SEL;
    else
        regValue &= ~MUX_SEL;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_HPMBUS);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_HPMBUS) & MUX_BUSY);
}

static uint32_t cmuDrvGetMuxClkCmuChubRcoUser(void)
{
    return (__raw_read32(REG_CMU_PLL_CON0_MUX_CLK_CHUB_RCO_USER) & MUX_USER_SEL)? CLK_RCO_FREQ : OSCCLK_RCO_FREQ;
}

static uint32_t cmuDrvGetMuxClkCmuChubBusUser(void)
{
    return (__raw_read32(REG_CMU_PLL_CON0_MUX_CLK_CHUB_BUS_USER) & MUX_USER_SEL)? tChubMainClk : OSCCLK_RCO_FREQ;
}

static uint32_t cmuDrvGetMuxClkCmuChubPeriUser(void)
{
    return (__raw_read32(REG_CMU_PLL_CON0_MUX_CLK_CHUB_BUS_USER) & MUX_USER_SEL)? CHUB_PERI_CLK : OSCCLK_RCO_FREQ;
}

static uint32_t cmuDrvGetMuxClkChubBus(void)
{
    uint32_t speed = 0;

    if (__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_BUS) & MUX_SEL)
        speed = cmuDrvGetMuxClkCmuChubBusUser();
    else
        speed = cmuDrvGetMuxClkCmuChubRcoUser();

    return speed;
}

static uint32_t cmuDrvGetMuxClkChubI2C(void)
{
    uint32_t speed = 0;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_I2C) & MUX_SEL)
        speed = cmuDrvGetMuxClkCmuChubPeriUser();
    else
        speed = cmuDrvGetMuxClkCmuChubRcoUser();

    return speed;
}

static uint32_t cmuDrvGetMuxClkChubTimer(void)
{
    uint32_t speed = 0;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_TIMER) & MUX_SEL)
        speed= RTC_FREQ;
    else
        speed = cmuDrvGetMuxClkCmuChubRcoUser();

    return speed;
}

static uint32_t cmuDrvGetMuxClkChubUSI0(void)
{
    uint32_t speed = 0;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI0) & MUX_SEL)
        speed = cmuDrvGetMuxClkCmuChubPeriUser();
    else
        speed = cmuDrvGetMuxClkCmuChubRcoUser();

    return speed;
}

static uint32_t cmuDrvGetMuxClkChubUSI1(void)
{
    uint32_t speed = 0;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI1) & MUX_SEL)
        speed = cmuDrvGetMuxClkCmuChubPeriUser();
    else
        speed = cmuDrvGetMuxClkCmuChubRcoUser();

    return speed;
}

static uint32_t cmuDrvGetMuxClkChubUSI2(void)
{
    uint32_t speed = 0;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI2) & MUX_SEL)
        speed= cmuDrvGetMuxClkCmuChubPeriUser();
    else
        speed = cmuDrvGetMuxClkCmuChubRcoUser();

    return speed;
}

static uint32_t cmuDrvGetMuxClkChubDmailbox(void)
{
    uint32_t speed = 0;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_DMAILBOX) & MUX_SEL)
        speed= cmuDrvGetMuxClkCmuChubPeriUser();
    else
        speed = cmuDrvGetMuxClkCmuChubRcoUser();

    return speed;
}

static uint32_t cmuDrvGetMuxClkChubHpmbus(void)
{
    uint32_t speed = 0;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_HPMBUS) & MUX_SEL)
        speed= cmuDrvGetMuxClkCmuChubPeriUser();
    else
        speed = cmuDrvGetMuxClkCmuChubRcoUser();

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

static void cmuDrvSetDivChubI2C(IN uint32_t divider)
{
    if (divider == 0)
        return;
    // divider value is clear and set to given divider value
    divider = (divider - 1) & DIV_RATIO_MASK_4BIT;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_I2C) & ~DIV_RATIO_MASK_4BIT) | divider), REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_I2C);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_I2C) & DIV_BUSY);
}

static void cmuDrvSetDivChubUSI0(IN uint32_t divider)
{
    if (divider == 0)
        return;
    // divider value is clear and set to given divider value
    divider = (divider - 1) & DIV_RATIO_MASK_4BIT;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI0) & ~DIV_RATIO_MASK_4BIT) | divider), REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI0);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI0) & DIV_BUSY);
}

static void cmuDrvSetDivChubUSI1(IN uint32_t divider)
{
    if (divider == 0)
        return;
    // divider value is clear and set to given divider value
    divider = (divider - 1) & DIV_RATIO_MASK_4BIT;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI1) & ~DIV_RATIO_MASK_4BIT) | divider), REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI1);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI1) & DIV_BUSY);
}

static void cmuDrvSetDivChubUSI2(IN uint32_t divider)
{
    if (divider == 0)
        return;
    // divider value is clear and set to given divider value
    divider = (divider - 1) & DIV_RATIO_MASK_4BIT;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI2) & ~DIV_RATIO_MASK_4BIT) | divider), REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI2);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI2) & DIV_BUSY);
}

static void cmuDrvSetDivChubDmailbox(IN uint32_t divider)
{
    if (divider == 0)
        return;
    // divider value is clear and set to given divider value
    divider = (divider - 1) & DIV_RATIO_MASK_2BIT;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_DMAILBOX) & ~DIV_RATIO_MASK_2BIT)| divider), REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_DMAILBOX);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_DMAILBOX) & DIV_BUSY);
}

static void cmuDrvSetDivChubDmailboxCore(IN uint32_t divider)
{
    if (divider == 0)
        return;
    // divider value is clear and set to given divider value
    divider = (divider - 1) & DIV_RATIO_MASK_4BIT;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_DMAILBOX_CORE) & ~DIV_RATIO_MASK_4BIT)| divider), REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_DMAILBOX_CORE);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_DMAILBOX_CORE) & DIV_BUSY);
}

static void cmuDrvSetDivChubHpmbus(IN uint32_t divider)
{
    if (divider == 0)
        return;
    // divider value is clear and set to given divider value
    divider = (divider - 1) & DIV_RATIO_MASK_4BIT;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_HPMBUS) & ~DIV_RATIO_MASK_4BIT)| divider), REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_HPMBUS);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_HPMBUS) & DIV_BUSY);
}

static void cmuDrvSetDivChubSerialLifBclk(IN uint32_t divider)
{
    if (divider == 0)
        return;
    // divider value is clear and set to given divider value
    divider = (divider - 1) & DIV_RATIO_MASK_5BIT;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SERIAL_LIF_BCLK) & ~DIV_RATIO_MASK_5BIT)| divider), REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SERIAL_LIF_BCLK);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SERIAL_LIF_BCLK) & DIV_BUSY);
}

static void cmuDrvSetDivChubSerialLifCore(IN uint32_t divider)
{
    if (divider == 0)
        return;
    // divider value is clear and set to given divider value
    divider = (divider - 1) & DIV_RATIO_MASK_4BIT;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SERIAL_LIF_CORE) & ~DIV_RATIO_MASK_4BIT)| divider), REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SERIAL_LIF_CORE);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SERIAL_LIF_CORE) & DIV_BUSY);
}


static uint32_t cmuDrvGetDivChubBus(void)
{
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_BUS) & DIV_RATIO_MASK_3BIT) + 1;

    return divider;
}
static uint32_t cmuDrvGetDivChubI2C(void)
{
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_I2C) & DIV_RATIO_MASK_4BIT) + 1;

    return divider;
}
static uint32_t cmuDrvGetDivChubUSI0(void)
{
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI0) & DIV_RATIO_MASK_4BIT) + 1;

    return divider;
}
static uint32_t cmuDrvGetDivChubUSI1(void)
{
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI1) & DIV_RATIO_MASK_4BIT) + 1;

    return divider;
}
static uint32_t cmuDrvGetDivChubUSI2(void)
{
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI2) & DIV_RATIO_MASK_4BIT) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivChubDmailbox(void)
{
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_DMAILBOX) & DIV_RATIO_MASK_2BIT) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivChubDmailboxCore(void)
{
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_DMAILBOX_CORE) & DIV_RATIO_MASK_4BIT) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivChubHpmbus(void)
{
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_HPMBUS) & DIV_RATIO_MASK_3BIT) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivChubSerialLifBclk(void)
{
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SERIAL_LIF_BCLK) & DIV_RATIO_MASK_5BIT) + 1;

    return divider;
}

static uint32_t cmuDrvGetDivChubSerialLifCore(void)
{
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_SERIAL_LIF_CORE) & DIV_RATIO_MASK_4BIT) + 1;

    return divider;
}

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

uint32_t cmuDrvSetCpuMux(uint32_t mux)
{
    uint32_t prev;

    prev = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_BUS) & MUX_SEL;
    if (mux == CPU_MUX_LOW)
        cmuDrvSetMuxClkChubBus(MUX_CLK_CHUB_RCO_USER);
    else
        cmuDrvSetMuxClkChubBus(MUX_CLK_CHUB_BUS_USER);

    return prev;
}

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
    cmuDrvSetDivChubBus(div);

    return 0;
}

//
uint32_t cmuDrvGetSpeed(IN CmuIpType ip)
{
    switch(ip) {
        case CMU_CLK_OSCCLK_RCO:
            return OSCCLK_RCO_FREQ;
        break;

        case CMU_CLK_CLK_RCO:
            return CLK_RCO_FREQ;
        break;

        case CMU_CLK_RTCCLK:
            return RTC_FREQ;
        break;

        case CMU_CLK_OUTPUT_CMUAPM:
            return tChubMainClk;
        break;

        case CMU_CLK_OUTPUT_CPU:
            return cmuDrvGetMuxClkChubBus() / cmuDrvGetDivChubBus();
        break;

        case CMU_CLK_CHUB_TIMER:
            return cmuDrvGetMuxClkChubTimer();
        break;

        case CMU_CLK_CHUB_BUS:
            return cmuDrvGetMuxClkChubBus();
        break;

        case CMU_CLK_CHUB_I2C:
        case CMU_CLK_I2C01:
        case CMU_CLK_I2C03:
        case CMU_CLK_I2C05:
            return cmuDrvGetMuxClkChubI2C() / cmuDrvGetDivChubI2C();
        break;

        case CMU_CLK_CHUB_USI00:
        case CMU_CLK_I2C00:
            return cmuDrvGetMuxClkChubUSI0() / cmuDrvGetDivChubUSI0();
        break;

        case CMU_CLK_CHUB_USI01:
        case CMU_CLK_I2C02:
            return cmuDrvGetMuxClkChubUSI1() / cmuDrvGetDivChubUSI1();
        break;

        case CMU_CLK_CHUB_USI02:
        case CMU_CLK_I2C04:
            return cmuDrvGetMuxClkChubUSI2() / cmuDrvGetDivChubUSI2();
        break;

        case CMU_CLK_CHUB_SERIAL_LIF_BCLK:
            return cmuDrvGetMuxClkCmuChubRcoUser() / cmuDrvGetDivChubSerialLifBclk();
        break;

        case CMU_CLK_CHUB_SERIAL_LIF_CORE:
            return cmuDrvGetMuxClkCmuChubRcoUser() / cmuDrvGetDivChubSerialLifCore();
        break;

        case CMU_CLK_CHUB_DMAILBOX:
            return cmuDrvGetMuxClkChubDmailbox() / cmuDrvGetDivChubDmailboxCore() / cmuDrvGetDivChubDmailbox();
        break;

        case CMU_CLK_CHUB_DMAILBOX_CORE:
            return cmuDrvGetMuxClkChubDmailbox() / cmuDrvGetDivChubDmailboxCore();
        break;

        case CMU_CLK_CHUB_HPMBUS:
            return cmuDrvGetMuxClkChubHpmbus() / cmuDrvGetDivChubHpmbus();
        break;

        case CMU_CLK_CMGP_I2C:
        case CMU_CLK_I2C07:
        case CMU_CLK_I2C09:
        case CMU_CLK_I2C11:
            return cmuDrvGetMuxClkCmgpI2C();
        break;

        case CMU_CLK_CMGP_USI00:
        case CMU_CLK_I2C06:
            return cmuDrvGetMuxClkCmgpUSI00();
        break;

        case CMU_CLK_CMGP_USI01:
        case CMU_CLK_I2C08:
            return cmuDrvGetMuxClkCmgpUSI01();
        break;

        case CMU_CLK_CMGP_USI02:
        case CMU_CLK_I2C10:
            return cmuDrvGetMuxClkCmgpUSI02();
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
            cmuDrvSetDivChubBus(divider);
        break;

        case CMU_CLK_CHUB_I2C:
            cmuDrvSetDivChubI2C(divider);
        break;

        case CMU_CLK_CHUB_USI00:
            cmuDrvSetDivChubUSI0(divider);
        break;

        case CMU_CLK_CHUB_USI01:
            cmuDrvSetDivChubUSI1(divider);
        break;

        case CMU_CLK_CHUB_USI02:
            cmuDrvSetDivChubUSI2(divider);
        break;

        case CMU_CLK_CHUB_SERIAL_LIF_BCLK:
            cmuDrvSetDivChubSerialLifBclk(divider);
        break;

        case CMU_CLK_CHUB_SERIAL_LIF_CORE:
            cmuDrvSetDivChubSerialLifCore(divider);
        break;

        case CMU_CLK_CHUB_DMAILBOX:
            cmuDrvSetDivChubDmailbox(divider);
        break;

        case CMU_CLK_CHUB_DMAILBOX_CORE:
            cmuDrvSetDivChubDmailboxCore(divider);
        break;

        case CMU_CLK_CHUB_HPMBUS:
            cmuDrvSetDivChubHpmbus(divider);
        break;

        case CMU_CLK_CMGP_I2C:
            cmuDrvSetDivCmgpI2C(divider);
        break;

        case CMU_CLK_CMGP_USI00:
            cmuDrvSetDivCmgpUSI00(divider);
        break;

        case CMU_CLK_CMGP_USI01:
            cmuDrvSetDivCmgpUSI01(divider);
        break;

        case CMU_CLK_CMGP_USI02:
            cmuDrvSetDivCmgpUSI02(divider);
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
    CSP_PRINTF_INFO("[CMU] cmuDrvSaveState NOT ready! \n");

    /***************************************************************
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

    ***************************************************************/
}

void cmuDrvRestoreState(void)
{
    CSP_PRINTF_INFO("[CMU] cmuDrvRestoreState NOT ready! \n");

    /***************************************************************
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

    ***************************************************************/
}

