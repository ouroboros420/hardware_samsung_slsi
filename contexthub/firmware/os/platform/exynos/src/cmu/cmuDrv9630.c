/*----------------------------------------------------------------------------
 *      Exynos SoC  -  CMU
 *----------------------------------------------------------------------------
 *      Name:    cmuDrv9610.c
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
static void cmuDrvSetMuxClkChubBus(int);
static void cmuDrvSetMuxClkChubI2C(int);
static void cmuDrvSetMuxClkChubUSI0(int);
static void cmuDrvSetMuxClkChubUSI1(int);
static void cmuDrvSetMuxClkChubUSI2(int);
static void cmuDrvSetMuxClkChubTimer(int);
static uint32_t cmuDrvGetMuxClkCmuChubRcoUser(void);
static uint32_t cmuDrvGetMuxClkCmuChubBusUser(void);
static uint32_t cmuDrvGetMuxClkChubBus(void);
static uint32_t cmuDrvGetMuxClkChubI2C(void);
static uint32_t cmuDrvGetMuxClkChubUSI0(void);
static uint32_t cmuDrvGetMuxClkChubUSI1(void);
static uint32_t cmuDrvGetMuxClkChubUSI2(void);
static uint32_t cmuDrvGetMuxClkChubTimer(void);
static void cmuDrvSetDivChubBus(IN uint32_t divider);
static void cmuDrvSetDivChubI2C(IN uint32_t divider);
static void cmuDrvSetDivChubUSI0(IN uint32_t divider);
static void cmuDrvSetDivChubUSI1(IN uint32_t divider);
static void cmuDrvSetDivChubUSI2(IN uint32_t divider);
static uint32_t cmuDrvGetDivChubBus(void);
static uint32_t cmuDrvGetDivChubI2C(void);
static uint32_t cmuDrvGetDivChubUSI0(void);
static uint32_t cmuDrvGetDivChubUSI1(void);
static uint32_t cmuDrvGetDivChubUSI2(void);

void cmuDrvInit(uint32_t mainclk)
{
    uint32_t val = 0;
    uint32_t divider;

    tChubMainClk = mainclk;

    // Initializes mux and divider
    // !!! Make sure that clock source for CHUB, CLKCMU_CHUB_BUS_USER is 360Mhz and provided at this initializing time
    // MUX_CLKCMU_CHUB_BUS_USER
    cmuDrvSetMuxClkCmuChubRcoUser(RCO_CHUB);

    // CHUB_CONTROLLER_OPTION
    val = 0;
    val |= (CHUB_CONTROLLER_OPTION_DEBUG_ENABLE |
            CHUB_CONTROLLER_OPTION_LAYER2_CTRL_ENABLE |
            CHUB_CONTROLLER_OPTION_PM_ENABLE |
            CHUB_CONTROLLER_OPTION_HWACG_ENABLE |
            CHUB_CONTROLLER_OPTION_MEMPG_ENABLE);
    __raw_write32(val, REG_CMU_CHUB_CMU_CHUB_CONTROLLER_OPTION);

    // MUX_CLKCMU_CHUB_BUS_USER
    cmuDrvSetMuxClkCmuChubBusUser(CLKCMU_CHUB_BUS);

    // MUX_CLK_CHUB_BUS
    cmuDrvSetMuxClkChubBus(MUX_CLK_CHUB_BUS_USER);
    // DIV_CLK_CHUB_BUS
    divider = 0;
    cmuDrvSetDivChubBus(divider);

    // MUX_CLK_CHUB_TIMER_FCLK
    cmuDrvSetMuxClkChubTimer(RTCCLK_CHUB);

    // MUX_CLK_CHUB_I2C
    cmuDrvSetMuxClkChubI2C(OSCCLK_RCO_CHUB);
    // DIV_CLK_CHUB_I2C
    divider = 1;
    cmuDrvSetDivChubI2C(divider);

    // MUX_CLK_CHUB_USI0
    cmuDrvSetMuxClkChubUSI0(OSCCLK_RCO_CHUB);
    // DIV_CLK_CHUB_USI0
    divider = 1;
    cmuDrvSetDivChubUSI0(divider);

    // MUX_CLK_CHUB_USI1
    cmuDrvSetMuxClkChubUSI1(OSCCLK_RCO_CHUB);
    // DIV_CLK_CHUB_USI1
    divider = 1;
    cmuDrvSetDivChubUSI1(divider);

    // MUX_CLK_CHUB_USI2
    cmuDrvSetMuxClkChubUSI2(OSCCLK_RCO_CHUB);
    // DIV_CLK_CHUB_USI2
    divider = 1;
    cmuDrvSetDivChubUSI2(divider);

    CSP_PRINTF_INFO("CMU: RcoU( %dMHz ), BusU( %dMHz ) \n", \
        (int)(cmuDrvGetMuxClkCmuChubRcoUser()/1000000), (int)(cmuDrvGetMuxClkCmuChubBusUser()/1000000));
    CSP_PRINTF_INFO("CMU: B(%d), I(%d), T(%d), U0(%d), U1(%d), U2(%d) \n", \
        (int)(cmuDrvGetMuxClkChubBus()/1000000), (int)(cmuDrvGetMuxClkChubI2C()/1000000), (int)(cmuDrvGetMuxClkChubTimer()/1000),\
        (int)(cmuDrvGetMuxClkChubUSI0()/1000000), (int)(cmuDrvGetMuxClkChubUSI1()/1000000), (int)(cmuDrvGetMuxClkChubUSI2()/1000000));
}

static void cmuDrvSetMuxClkCmuChubRcoUser(int select)
{
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_PLL_CON0_MUX_CLKCMU_CHUB_RCO_USER);

    if (select)
        regValue |= SELECT_MUX_CLKCMU_CHUB_RCO_USER;
    else
        regValue &= ~SELECT_MUX_CLKCMU_CHUB_RCO_USER;

    __raw_write32(regValue, REG_CMU_PLL_CON0_MUX_CLKCMU_CHUB_RCO_USER);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_PLL_CON0_MUX_CLKCMU_CHUB_RCO_USER) & MUX_CLKCMU_CHUB_RCO_USER_IS_BUSY);
}

static void cmuDrvSetMuxClkCmuChubBusUser(int select)
{
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_PLL_CON0_MUX_CLK_CHUB_BUS_USER);

    if (select)
        regValue |= SELECT_MUX_CLK_CHUB_BUS_USER;
    else
        regValue &= ~SELECT_MUX_CLK_CHUB_BUS_USER;

    __raw_write32(regValue, REG_CMU_PLL_CON0_MUX_CLK_CHUB_BUS_USER);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_PLL_CON0_MUX_CLK_CHUB_BUS_USER) & MUX_CLK_CHUB_BUS_USER_BUSY);
}

static void cmuDrvSetMuxClkChubBus(int select)
{
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_BUS);

    if (select)
        regValue |= SELECT_MUX_CLK_CHUB_BUS;
    else
        regValue &= ~SELECT_MUX_CLK_CHUB_BUS;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_BUS);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_BUS) & MUX_CLK_CHUB_BUS_BUSY);
}

static void cmuDrvSetMuxClkChubI2C(int select)
{
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_I2C);

    if (select)
        regValue |= SELECT_MUX_CLK_CHUB_I2C;
    else
        regValue &= ~SELECT_MUX_CLK_CHUB_I2C;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_I2C);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_I2C) & MUX_CLK_CHUB_I2C_BUSY);
}

static void cmuDrvSetMuxClkChubUSI0(int select)
{
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI0);

    if (select)
        regValue |= SELECT_MUX_CLK_CHUB_USI0;
    else
        regValue &= ~SELECT_MUX_CLK_CHUB_USI0;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI0);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI0) & MUX_CLK_CHUB_USI0_BUSY);
}

static void cmuDrvSetMuxClkChubUSI1(int select)
{
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI1);

    if (select)
        regValue |= SELECT_MUX_CLK_CHUB_USI1;
    else
        regValue &= ~SELECT_MUX_CLK_CHUB_USI1;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI1);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI1) & MUX_CLK_CHUB_USI1_BUSY);
}

static void cmuDrvSetMuxClkChubUSI2(int select)
{
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI2);

    if (select)
        regValue |= SELECT_MUX_CLK_CHUB_USI2;
    else
        regValue &= ~SELECT_MUX_CLK_CHUB_USI2;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI2);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI2) & MUX_CLK_CHUB_USI2_BUSY);
}

static void cmuDrvSetMuxClkChubTimer(int select)
{
    uint32_t regValue;
    regValue = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_TIMER_FCLK);

    if (select)
        regValue |= SELECT_MUX_CLK_CHUB_TIMER_FCLK;
    else
        regValue &= ~SELECT_MUX_CLK_CHUB_TIMER_FCLK;

    __raw_write32(regValue, REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_TIMER_FCLK);

    // Wait while mux is changing
    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_TIMER_FCLK) & MUX_CLK_CHUB_TIMER_FCLK_BUSY);
}

static uint32_t cmuDrvGetMuxClkCmuChubRcoUser(void)
{
    return (__raw_read32(REG_CMU_PLL_CON0_MUX_CLKCMU_CHUB_RCO_USER) & SELECT_MUX_CLKCMU_CHUB_RCO_USER)? CLKCMU_RCO : CLKCMU_OSC_RCO;
}

static uint32_t cmuDrvGetMuxClkCmuChubBusUser(void)
{
    return (__raw_read32(REG_CMU_PLL_CON0_MUX_CLK_CHUB_BUS_USER) & SELECT_MUX_CLK_CHUB_BUS_USER)? tChubMainClk : CLKCMU_OSC_RCO;
}

static uint32_t cmuDrvGetMuxClkChubBus(void)
{
    uint32_t speed = 0;

    if (__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_BUS) & SELECT_MUX_CLK_CHUB_BUS)
        speed = cmuDrvGetMuxClkCmuChubBusUser();
    else
        speed = cmuDrvGetMuxClkCmuChubRcoUser();

    return speed;
}

static uint32_t cmuDrvGetMuxClkChubI2C(void)
{
    uint32_t speed = 0;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_I2C) & SELECT_MUX_CLK_CHUB_I2C)
        speed = cmuDrvGetMuxClkCmuChubBusUser();
    else
        speed = cmuDrvGetMuxClkCmuChubRcoUser();

    return speed;
}

static uint32_t cmuDrvGetMuxClkChubTimer(void)
{
    uint32_t speed = 0;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_TIMER_FCLK) & SELECT_MUX_CLK_CHUB_TIMER_FCLK)
        speed= CLKCMU_RTC;
    else
        speed = cmuDrvGetMuxClkCmuChubRcoUser();

    return speed;
}

static uint32_t cmuDrvGetMuxClkChubUSI0(void)
{
    uint32_t speed = 0;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI0) & SELECT_MUX_CLK_CHUB_USI0)
        speed = cmuDrvGetMuxClkCmuChubBusUser();
    else
        speed = cmuDrvGetMuxClkCmuChubRcoUser();

    return speed;
}

static uint32_t cmuDrvGetMuxClkChubUSI1(void)
{
    uint32_t speed = 0;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI1) & SELECT_MUX_CLK_CHUB_USI1)
        speed = cmuDrvGetMuxClkCmuChubBusUser();
    else
        speed = cmuDrvGetMuxClkCmuChubRcoUser();

    return speed;
}

static uint32_t cmuDrvGetMuxClkChubUSI2(void)
{
    uint32_t speed = 0;

    if(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_USI2) & SELECT_MUX_CLK_CHUB_USI2)
        speed= cmuDrvGetMuxClkCmuChubBusUser();
    else
        speed = cmuDrvGetMuxClkCmuChubRcoUser();

    return speed;
}

static void cmuDrvSetDivChubBus(IN uint32_t divider)
{
    // divider value is clear and set to given divider value
    divider = divider & DIV_CLK_CHUB_BUS_DIVRATIO_BIT;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_BUS) & ~DIV_CLK_CHUB_BUS_DIVRATIO_BIT) | divider), REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_BUS);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_BUS) & DIV_CLK_CHUB_BUS_BUSY);
}

static void cmuDrvSetDivChubI2C(IN uint32_t divider)
{
    // divider value is clear and set to given divider value
    divider = divider & DIV_CLK_CHUB_I2C_DIVRATIO_BIT;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_I2C) & ~DIV_CLK_CHUB_I2C_DIVRATIO_BIT) | divider), REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_I2C);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_I2C) & DIV_CLK_CHUB_I2C_BUSY);
}

static void cmuDrvSetDivChubUSI0(IN uint32_t divider)
{
    // divider value is clear and set to given divider value
    divider = divider & DIV_CLK_CHUB_USI0_DIVRATIO_BIT;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI0) & ~DIV_CLK_CHUB_USI0_DIVRATIO_BIT) | divider), REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI0);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI0) & DIV_CLK_CHUB_USI0_BUSY);
}

static void cmuDrvSetDivChubUSI1(IN uint32_t divider)
{
    // divider value is clear and set to given divider value
    divider = divider & DIV_CLK_CHUB_USI1_DIVRATIO_BIT;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI1) & ~DIV_CLK_CHUB_USI1_DIVRATIO_BIT) | divider), REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI1);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI1) & DIV_CLK_CHUB_USI1_BUSY);
}

static void cmuDrvSetDivChubUSI2(IN uint32_t divider)
{
    // divider value is clear and set to given divider value
    divider = divider & DIV_CLK_CHUB_USI2_DIVRATIO_BIT;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI2) & ~DIV_CLK_CHUB_USI2_DIVRATIO_BIT) | divider), REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI2);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI2) & DIV_CLK_CHUB_USI2_BUSY);
}

static uint32_t cmuDrvGetDivChubBus(void)
{
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_BUS) & DIV_CLK_CHUB_BUS_DIVRATIO_BIT) + 1;

    return divider;
}
static uint32_t cmuDrvGetDivChubI2C(void)
{
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_I2C) & DIV_CLK_CHUB_I2C_DIVRATIO_BIT) + 1;

    return divider;
}
static uint32_t cmuDrvGetDivChubUSI0(void)
{
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI0) & DIV_CLK_CHUB_USI0_DIVRATIO_BIT) + 1;

    return divider;
}
static uint32_t cmuDrvGetDivChubUSI1(void)
{
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI1) & DIV_CLK_CHUB_USI1_DIVRATIO_BIT) + 1;

    return divider;
}
static uint32_t cmuDrvGetDivChubUSI2(void)
{
    uint32_t divider;
    divider = (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_USI2) & DIV_CLK_CHUB_USI2_DIVRATIO_BIT) + 1;

    return divider;
}

static uint32_t cmuDrvGetMuxClkCmgpI2C(void)
{
    CSP_PRINTF_ERROR("%s: CMGP not recommand to use! \n",__FUNCTION__);
    return false;
}

static uint32_t cmuDrvGetMuxClkCmgpUSI00(void)
{
    CSP_PRINTF_ERROR("%s: CMGP not recommand to use! \n",__FUNCTION__);
    return false;
}

static uint32_t cmuDrvGetMuxClkCmgpUSI01(void)
{
    CSP_PRINTF_ERROR("%s: CMGP not recommand to use! \n",__FUNCTION__);
    return false;
}

static uint32_t cmuDrvGetMuxClkCmgpUSI02(void)
{
    CSP_PRINTF_ERROR("%s: CMGP not recommand to use! \n",__FUNCTION__);
    return false;
}

static uint32_t cmuDrvGetMuxClkCmgpUSI03(void)
{
    CSP_PRINTF_ERROR("%s: CMGP not recommand to use! \n",__FUNCTION__);
    return false;
}

static void cmuDrvSetDivCmgpI2C(IN uint32_t divider)
{
    CSP_PRINTF_ERROR("%s: CMGP not recommand to use! \n",__FUNCTION__);
}

static void cmuDrvSetDivCmgpUSI00(IN uint32_t divider)
{
    CSP_PRINTF_ERROR("%s: CMGP not recommand to use! \n",__FUNCTION__);
}

static void cmuDrvSetDivCmgpUSI01(IN uint32_t divider)
{
    CSP_PRINTF_ERROR("%s: CMGP not recommand to use! \n",__FUNCTION__);
}

static void cmuDrvSetDivCmgpUSI02(IN uint32_t divider)
{
    CSP_PRINTF_ERROR("%s: CMGP not recommand to use! \n",__FUNCTION__);
}

static void cmuDrvSetDivCmgpUSI03(IN uint32_t divider)
{
    CSP_PRINTF_ERROR("%s: CMGP not recommand to use! \n",__FUNCTION__);
}

int cmuDrvSetCpuClk(uint32_t freq)
{
    uint32_t div;

    if (freq > tChubMainClk)
        return -1;

    div = 1;
    while ((tChubMainClk / div) > freq) {
        div++;
        if ( div > 0x7)
            return -1;
    }
    CSP_PRINTF_INFO("cmuDrvSetCpuClk div%lu\n", div);
    cmuDrvSetDivChubBus(div-1);

    return 0;
}

//
uint32_t cmuDrvGetSpeed(IN CmuIpType ip)
{
    switch(ip) {
        case CMU_CLK_OSCCLK_RCO:
            return CLKCMU_OSC_RCO;
        break;

        case CMU_CLK_CLK_RCO:
            return CLKCMU_RCO;
        break;

        case CMU_CLK_RTCCLK:
            return CLKCMU_RTC;
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

        case CMU_CLK_CHUB_WDT:
            return CLKCMU_RCO;
        break;

        case CMU_CLK_CMGP_I2C:
        case CMU_CLK_I2C07:
        case CMU_CLK_I2C09:
        case CMU_CLK_I2C11:
        case CMU_CLK_I2C13:
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

        case CMU_CLK_CMGP_USI03:
        case CMU_CLK_I2C12:
            return cmuDrvGetMuxClkCmgpUSI03();
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

        case CMU_CLK_CMGP_USI03:
            cmuDrvSetDivCmgpUSI03(divider);
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
#if 0
    uint32_t val = 0;

    // Dynamic Root Clock Gating must be set once at boot time before setting the hwacg enablement of each ip
    if(!isDrcgHwacgSet) {
        CSP_ASSERT(0);
        return false;
    }

    switch(ip) {
        case CMU_HWACG_IP_CM4:
            // QCH_CON_xxx
            val = __raw_read32(REG_CMU_QCH_CON_CM4_CHUB_QCH);
            val |= ((enable) ? QCH_CON_xxx_ENABLE : QCH_CON_xxx_IGNORE_FORCE_PM_EN);
            __raw_write32(val, REG_CMU_QCH_CON_CM4_CHUB_QCH);

            // CLK_CON_xxx
            val = __raw_read32(REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_CM4_CHUB_IPCLKPORT_FCLK);
            if(enable)
                val |= CLK_CON_xxx_ENABLE_AUTOMATIC_CLKGATING;
            else
                val &= (uint32_t)~CLK_CON_xxx_ENABLE_AUTOMATIC_CLKGATING;
            __raw_write32(val, REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_CM4_CHUB_IPCLKPORT_FCLK);
        break;

        case CMU_HWACG_IP_GPIO:
            // QCH_CON_xxx
            val = __raw_read32(REG_CMU_QCH_CON_GPIO_CHUB_QCH);
            val |= ((enable) ? QCH_CON_xxx_ENABLE : QCH_CON_xxx_IGNORE_FORCE_PM_EN);
            __raw_write32(val, REG_CMU_QCH_CON_GPIO_CHUB_QCH);

            // CLK_CON_xxx
            val = __raw_read32(REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_GPIO_CHUB_IPCLKPORT_PCLK);
            if(enable)
                val |= CLK_CON_xxx_ENABLE_AUTOMATIC_CLKGATING;
            else
                val &= (uint32_t)~CLK_CON_xxx_ENABLE_AUTOMATIC_CLKGATING;
            __raw_write32(val, REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_GPIO_CHUB_IPCLKPORT_PCLK);
        break;

        case CMU_HWACG_IP_I2C_CHUB00:
            // QCH_CON_xxx
            val = __raw_read32(REG_CMU_QCH_CON_I2C_CHUB00_QCH);
            val |= ((enable) ? QCH_CON_xxx_ENABLE : QCH_CON_xxx_IGNORE_FORCE_PM_EN);
            __raw_write32(val, REG_CMU_QCH_CON_I2C_CHUB00_QCH);

            // CLK_CON_xxx
            val = __raw_read32(REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_I2C_CHUB00_IPCLKPORT_IPCLK);
            if(enable)
                val |= CLK_CON_xxx_ENABLE_AUTOMATIC_CLKGATING;
            else
                val &= (uint32_t)~CLK_CON_xxx_ENABLE_AUTOMATIC_CLKGATING;
            __raw_write32(val, REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_I2C_CHUB00_IPCLKPORT_IPCLK);
            val = __raw_read32(REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_I2C_CHUB00_IPCLKPORT_PCLK);
            if(enable)
                val |= CLK_CON_xxx_ENABLE_AUTOMATIC_CLKGATING;
            else
                val &= (uint32_t)~CLK_CON_xxx_ENABLE_AUTOMATIC_CLKGATING;
            __raw_write32(val, REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_I2C_CHUB00_IPCLKPORT_PCLK);
        break;

        case CMU_HWACG_IP_PDMA:
            // QCH_CON_xxx
            val = __raw_read32(REG_CMU_QCH_CON_PDMA_CHUB_QCH);
            val |= ((enable) ? QCH_CON_xxx_ENABLE : QCH_CON_xxx_IGNORE_FORCE_PM_EN);
            __raw_write32(val, REG_CMU_QCH_CON_PDMA_CHUB_QCH);

            // CLK_CON_xxx
            val = __raw_read32(REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_PDMA_CHUB_IPCLKPORT_ACLK);
            if(enable)
                val |= CLK_CON_xxx_ENABLE_AUTOMATIC_CLKGATING;
            else
                val &= (uint32_t)~CLK_CON_xxx_ENABLE_AUTOMATIC_CLKGATING;
            __raw_write32(val, REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_PDMA_CHUB_IPCLKPORT_ACLK);
        break;

        case CMU_HWACG_IP_PWM:
            // QCH_CON_xxx
            val = __raw_read32(REG_CMU_QCH_CON_PWM_CHUB_QCH);
            val |= ((enable) ? QCH_CON_xxx_ENABLE : QCH_CON_xxx_IGNORE_FORCE_PM_EN);
            __raw_write32(val, REG_CMU_QCH_CON_PWM_CHUB_QCH);

            // CLK_CON_xxx
            val = __raw_read32(REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_PWM_CHUB_IPCLKPORT_I_PCLK_S0);
            if(enable)
                val |= CLK_CON_xxx_ENABLE_AUTOMATIC_CLKGATING;
            else
                val &= (uint32_t)~CLK_CON_xxx_ENABLE_AUTOMATIC_CLKGATING;
            __raw_write32(val, REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_PWM_CHUB_IPCLKPORT_I_PCLK_S0);
        break;

        case CMU_HWACG_IP_TIMER:
            // QCH_CON_xxx
            val = __raw_read32(REG_CMU_QCH_CON_TIMER_CHUB_QCH);
            val |= ((enable) ? QCH_CON_xxx_ENABLE : QCH_CON_xxx_IGNORE_FORCE_PM_EN);
            __raw_write32(val, REG_CMU_QCH_CON_TIMER_CHUB_QCH);

            val = __raw_read32(REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_TIMER_CHUB_IPCLKPORT_PCLK);
            if(enable)
                val |= CLK_CON_xxx_ENABLE_AUTOMATIC_CLKGATING;
            else
                val &= (uint32_t)~CLK_CON_xxx_ENABLE_AUTOMATIC_CLKGATING;
            __raw_write32(val, REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_TIMER_CHUB_IPCLKPORT_PCLK);
        break;

        case CMU_HWACG_IP_USI_CHUB00:
            // QCH_CON_xxx
            val = __raw_read32(REG_CMU_QCH_CON_USI_CHUB00_QCH);
            val |= ((enable) ? QCH_CON_xxx_ENABLE : QCH_CON_xxx_IGNORE_FORCE_PM_EN);
            __raw_write32(val, REG_CMU_QCH_CON_USI_CHUB00_QCH);

            // CLK_CON_xxx
            val = __raw_read32(REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_USI_CHUB00_IPCLKPORT_IPCLK);
            if(enable)
                val |= CLK_CON_xxx_ENABLE_AUTOMATIC_CLKGATING;
            else
                val &= (uint32_t)~CLK_CON_xxx_ENABLE_AUTOMATIC_CLKGATING;
            __raw_write32(val, REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_USI_CHUB00_IPCLKPORT_IPCLK);
            val = __raw_read32(REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_USI_CHUB00_IPCLKPORT_PCLK);
            if(enable)
                val |= CLK_CON_xxx_ENABLE_AUTOMATIC_CLKGATING;
            else
                val &= (uint32_t)~CLK_CON_xxx_ENABLE_AUTOMATIC_CLKGATING;
            __raw_write32(val, REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_USI_CHUB00_IPCLKPORT_PCLK);
        break;

        case CMU_HWACG_IP_WDT:
            // QCH_CON_xxx
            val = __raw_read32(REG_CMU_QCH_CON_WDT_CHUB_QCH);
            val |= ((enable) ? QCH_CON_xxx_ENABLE : QCH_CON_xxx_IGNORE_FORCE_PM_EN);
            __raw_write32(val, REG_CMU_QCH_CON_WDT_CHUB_QCH);

            // CLK_CON_xxx
            val = __raw_read32(REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_WDT_CHUB_IPCLKPORT_PCLK);
            if(enable)
                val |= CLK_CON_xxx_ENABLE_AUTOMATIC_CLKGATING;
            else
                val &= (uint32_t)~CLK_CON_xxx_ENABLE_AUTOMATIC_CLKGATING;
            __raw_write32(val, REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_WDT_CHUB_IPCLKPORT_PCLK);
        break;

        default:
            CSP_ASSERT(0);
            return false;
        break;
    }
#endif
    return true;
}

// Need to increase if more SFRs need to be kept. Currently it is 27 SFRs of CMU
//#define E9630_CMUSFR_NUM 30
//static uint32_t buf_CMUSFR[E9630_CMUSFR_NUM];
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

