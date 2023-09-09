/*----------------------------------------------------------------------------
 *      Exynos SoC  -  CMU
 *----------------------------------------------------------------------------
 *      Name:    cmuDrv9110.c
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


static uint32_t tChubMainClk = CHUB_MAIN_CLK;

static void cmuDrvSetMuxClkCmuChubBusUser(ChubClkSrcType clk)
{
    uint32_t reg;
    uint32_t addr;
    uint32_t offset;

    addr = REG_CMU_PLL_CON0_MUX_CLKCMU_CHUB_BUS_USER;
    offset = MUX_SEL_OFFSET;
    if(clk == CMU_OSC_CLK || clk == CMU_CHUB_BUS_CLK){
        reg = __raw_read32(addr);
        if(clk == CMU_OSC_CLK)
            reg &= ~(0x1 << offset);
        else
            reg |= (0x1 << offset);

        __raw_write32(reg,addr);

        // Wait til mux is changed
        while(__raw_read32(addr) & MUX_SEL_BUSY_OFFSET);
    }
    else{
    }
}

static uint32_t cmuDrvGetMuxClkCmuChubBusUser(void)
{
    return (__raw_read32(REG_CMU_PLL_CON0_MUX_CLKCMU_CHUB_BUS_USER) & MUX_SEL_OFFSET)? tChubMainClk : CLKCMU_RCO_ALV;
}

static void cmuDrvSetMuxClkCmuChubUser(ChubClkSrcType clk)
{
    uint32_t reg;
    uint32_t addr;
    uint32_t offset;

    addr = REG_CMU_PLL_CON0_MUX_CLKCMU_RCO_CHUB_BUS_USER;
    offset = MUX_SEL_OFFSET;
    if(clk != CMU_OSC_CLK && clk != CMU_RCO_CLK){
        CSP_PRINTF_ERROR("%s: invalid input(clk: %d)\n",__func__, clk);
        return;
    }

    reg = __raw_read32(addr);
    if(clk == CMU_OSC_CLK)
        reg &= ~(0x1 << offset);
    else
        reg |= (0x1 << offset);

    __raw_write32(reg,addr);

    // Wait til mux is changed
    while(__raw_read32(addr) & MUX_SEL_BUSY_OFFSET);
}

static uint32_t cmuDrvGetMuxClkCmuChubUser(void)
{
    return (__raw_read32(REG_CMU_PLL_CON0_MUX_CLKCMU_RCO_CHUB_BUS_USER) & MUX_SEL_OFFSET) ? tChubMainClk : CLKCMU_XTAL;
}

static void cmuDrvSetMuxClkChubBus(bool sel)
{
    uint32_t reg;
    uint32_t addr;
    uint32_t offset;

    addr = REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_BUS;
    offset = SELECT_OFFSET;

    reg = __raw_read32(addr);
    if(sel)
        reg |= (0x1<<offset);
    else
        reg &= ~(0x1<<offset);

    __raw_write32(reg,addr);


    // Wait til mux is changed
    while(__raw_read32(addr) & SELECT_BUSY_OFFSET);
}

static uint32_t cmuDrvGetMuxClkChubBus(void)
{
    uint32_t speed = 0;

//
// Need to check!!!
//
    speed = (__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_BUS) & SELECT_OFFSET) ? cmuDrvGetMuxClkCmuChubBusUser() : cmuDrvGetMuxClkCmuChubUser();

    return speed;
}

static uint32_t cmuDrvGetCpuSpeed(void)
{
    uint32_t speed = 0;

//
// Need to check!!!
//
    speed = cmuDrvGetMuxClkChubBus();
    speed /= (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_BUS) & 0xF) + 1;

    return speed;
}

static void cmuDrvSetDivChubBus(IN uint32_t divider)
{
//
// Need to check!!!
//

    // divider value is clear and set to given divider value
    divider = divider & 0xF;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_BUS) & 0xFFFFFFF0) | divider), REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_BUS);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_BUS) & DIV_CLK_CHUB_BUS_IS_BUSY);
}

static uint32_t cmuDrvGetMuxClkCmgpI2C00(void)
{
    uint32_t speed = 0;

//
// Need to check!!!
//
    speed = (__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_I2C0) & 0x1) ? CLKCMU_CMGP_BUS : CLKCMU_RCO_ALV;
    speed /= (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C0) & 0xF) + 1;

    return speed;
}

static uint32_t cmuDrvGetMuxClkCmgpI2C01(void)
{
    uint32_t speed = 0;

//
// Need to check!!!
//
    speed = (__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_I2C1) & 0x1) ? CLKCMU_CMGP_BUS : CLKCMU_RCO_ALV;
    speed /= (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C1) & 0xF) + 1;

    return speed;
}

static uint32_t cmuDrvGetMuxClkCmgpI2C02(void)
{
    uint32_t speed = 0;

//
// Need to check!!!
//
    speed = (__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_I2C2) & 0x1) ? CLKCMU_CMGP_BUS : CLKCMU_RCO_ALV;
    speed /= (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C2) & 0xF) + 1;

    return speed;
}

static uint32_t cmuDrvGetMuxClkCmgpI2C03(void)
{
    uint32_t speed = 0;

//
// Need to check!!!
//
    speed = (__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_I2C3) & 0x1) ? CLKCMU_CMGP_BUS : CLKCMU_RCO_ALV;
    speed /= (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C3) & 0xF) + 1;

    return speed;
}

static uint32_t cmuDrvGetMuxClkCmgpUSI00(void)
{
    uint32_t speed = 0;

    speed = (__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI0) & 0x1) ? CLKCMU_CMGP_BUS : CLKCMU_RCO_ALV;
    speed /= (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI0) & 0xF) + 1;

    return speed;
}

static uint32_t cmuDrvGetMuxClkCmgpUSI01(void)
{
    uint32_t speed = 0;

    speed = (__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI1) & 0x1) ? CLKCMU_CMGP_BUS : CLKCMU_RCO_ALV;
    speed /= (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI1) & 0xF) + 1;

    return speed;
}

static uint32_t cmuDrvGetMuxClkCmgpUSI02(void)
{
    uint32_t speed = 0;

    speed = (__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI2) & 0x1) ? CLKCMU_CMGP_BUS : CLKCMU_RCO_ALV;
    speed /= (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI2) & 0xF) + 1;

    return speed;
}

static uint32_t cmuDrvGetMuxClkCmgpUSI03(void)
{
    uint32_t speed = 0;

    speed = (__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CMGP_USI3) & 0x1) ? CLKCMU_CMGP_BUS : CLKCMU_RCO_ALV;
    speed /= (__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI3) & 0xF) + 1;

    return speed;
}

static void cmuDrvSetDivCmgpI2C00(IN uint32_t divider)
{
    // divider value is clear and set to given divider value
    divider = divider & 0xF;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C0) & 0xFFFFFFF0) | divider), REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C0);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C0) & DIV_CLK_I2C_CMGP_IS_BUSY);
}

static void cmuDrvSetDivCmgpI2C01(IN uint32_t divider)
{
    // divider value is clear and set to given divider value
    divider = divider & 0xF;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C1) & 0xFFFFFFF0) | divider), REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C1);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C1) & DIV_CLK_I2C_CMGP_IS_BUSY);
}

static void cmuDrvSetDivCmgpI2C02(IN uint32_t divider)
{
    // divider value is clear and set to given divider value
    divider = divider & 0xF;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C2) & 0xFFFFFFF0) | divider), REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C2);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C2) & DIV_CLK_I2C_CMGP_IS_BUSY);
}

static void cmuDrvSetDivCmgpI2C03(IN uint32_t divider)
{
    // divider value is clear and set to given divider value
    divider = divider & 0xF;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C3) & 0xFFFFFFF0) | divider), REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C3);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C3) & DIV_CLK_I2C_CMGP_IS_BUSY);
}

static void cmuDrvSetDivCmgpUSI00(IN uint32_t divider)
{
    // divider value is clear and set to given divider value
    divider = divider & 0xF;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI0) & 0xFFFFFFF0) | divider), REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI0);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI0) & DIV_CLK_USI_CMGP00_IS_BUSY);
}

static void cmuDrvSetDivCmgpUSI01(IN uint32_t divider)
{
    // divider value is clear and set to given divider value
    divider = divider & 0xF;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI1) & 0xFFFFFFF0) | divider), REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI1);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI1) & DIV_CLK_USI_CMGP01_IS_BUSY);
}

static void cmuDrvSetDivCmgpUSI02(IN uint32_t divider)
{
    // divider value is clear and set to given divider value
    divider = divider & 0xF;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI2) & 0xFFFFFFF0) | divider), REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI2);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI2) & DIV_CLK_USI_CMGP02_IS_BUSY);
}

static void cmuDrvSetDivCmgpUSI03(IN uint32_t divider)
{
    // divider value is clear and set to given divider value
    divider = divider & 0xF;
    __raw_write32(((__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI3) & 0xFFFFFFF0) | divider), REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI3);

    // Wait while divider is changing
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI3) & DIV_CLK_USI_CMGP03_IS_BUSY);
}


void cmuDrvInit(uint32_t mainclk)
{
    uint32_t div;

    tChubMainClk = mainclk;


    if(mainclk==CLKCMU_XTAL){
        cmuDrvSetMuxClkCmuChubUser(CMU_OSC_CLK);
        cmuDrvSetMuxClkCmuChubBusUser(CMU_OSC_CLK);
        cmuDrvSetMuxClkChubBus(true);
    }else if(mainclk==CLKCMU_RCO_ALV){
        cmuDrvSetMuxClkCmuChubUser(CMU_RCO_CLK);
        cmuDrvSetMuxClkCmuChubBusUser(CMU_OSC_CLK);
        cmuDrvSetMuxClkChubBus(false);
    }else if(mainclk==CLKCMU_CHUB_DLL_USER){
        cmuDrvSetMuxClkCmuChubUser(CMU_OSC_CLK);
        cmuDrvSetMuxClkCmuChubBusUser(CMU_CHUB_BUS_CLK);
        cmuDrvSetMuxClkChubBus(true);
    }else if(mainclk<CLKCMU_CHUB_DLL_USER){
        cmuDrvSetMuxClkCmuChubUser(CMU_OSC_CLK);
        cmuDrvSetMuxClkCmuChubBusUser(CMU_CHUB_BUS_CLK);
        cmuDrvSetMuxClkChubBus(true);

        div = (CLKCMU_CHUB_DLL_USER / mainclk)-1;
        cmuDrvSetDivChubBus(div);
    }

#if 0
    if((mainclk ==CLKCMU_XTAL)||(mainclk ==CLKCMU_RCO_ALV)||(mainclk ==CLKCMU_CHUB_DLL_USER)){
        switch(mainclk)
        {
            case CMU_OSC_CLK:
                cmuDrvSetMuxClkCmuChubUser(CMU_OSC_CLK);
                cmuDrvSetMuxClkCmuChubBusUser(CMU_OSC_CLK);
                cmuDrvSetMuxClkChubBus(true);
                break;
            case CMU_RCO_CLK:
                cmuDrvSetMuxClkCmuChubUser(CMU_RCO_CLK);
                cmuDrvSetMuxClkCmuChubBusUser(CMU_OSC_CLK);
                cmuDrvSetMuxClkChubBus(false);
                break;
            case CMU_CHUB_BUS_CLK:
                cmuDrvSetMuxClkCmuChubUser(CMU_OSC_CLK);
                cmuDrvSetMuxClkCmuChubBusUser(CMU_CHUB_BUS_CLK);
                cmuDrvSetMuxClkChubBus(true);
                break;
            default:
                cmuDrvSetMuxClkCmuChubUser(CMU_OSC_CLK);
                cmuDrvSetMuxClkCmuChubBusUser(CMU_CHUB_BUS_CLK);
                cmuDrvSetMuxClkChubBus(true);
                break;
        }
    }
    else if(mainclk<CLKCMU_CHUB_DLL_USER){
        cmuDrvSetMuxClkCmuChubUser(CMU_OSC_CLK);
        cmuDrvSetMuxClkCmuChubBusUser(CMU_CHUB_BUS_CLK);
        cmuDrvSetMuxClkChubBus(true);

        div = (CLKCMU_CHUB_DLL_USER / mainclk)-1;
        cmuDrvSetDivChubBus(div);
    }
#endif
}

//
uint32_t cmuDrvGetSpeed(IN CmuIpType ip)
{
    switch(ip) {
        case CMU_CLK_OSCCLK_RCO:
            return CLKCMU_RCO_ALV;
        break;

        case CMU_CLK_RTCCLK:
            return CLKCMU_RTC;
        break;

        case CMU_CLK_OUTPUT_CMUCMU:
            return cmuDrvGetCpuSpeed();
        break;

        case CMU_CLK_OUTPUT_CMUAPM:
            return tChubMainClk;
        break;

        case CMU_CLK_OUTPUT_CPU:
            return cmuDrvGetCpuSpeed();
        break;

        case CMU_CLK_CHUB_TIMER:
            return cmuDrvGetCpuSpeed();
        break;

        case CMU_CLK_CHUB_BUS:
            return cmuDrvGetMuxClkChubBus();
        break;

        case CMU_CLK_CHUB_WDT:
            return CLKCMU_RCO_ALV;
        break;

        case CMU_CLK_CMGP_I2C00:
            return cmuDrvGetMuxClkCmgpI2C00();
        break;

        case CMU_CLK_CMGP_I2C01:
            return cmuDrvGetMuxClkCmgpI2C01();
        break;

        case CMU_CLK_CMGP_I2C02:
            return cmuDrvGetMuxClkCmgpI2C02();
        break;

        case CMU_CLK_CMGP_I2C03:
            return cmuDrvGetMuxClkCmgpI2C03();
        break;

        case CMU_CLK_CMGP_USI00:
            return cmuDrvGetMuxClkCmgpUSI00();
        break;

        case CMU_CLK_CMGP_USI01:
            return cmuDrvGetMuxClkCmgpUSI01();
        break;

        case CMU_CLK_CMGP_USI02:
            return cmuDrvGetMuxClkCmgpUSI02();
        break;

        case CMU_CLK_CMGP_USI03:
            return cmuDrvGetMuxClkCmgpUSI03();
        break;

        default:
            CSP_ASSERT(0);
        break;
    }

    return 0;
}

//
bool cmuDrvSetDivider(IN CmuIpType ip, IN uint32_t divider)
{
    switch(ip) {

        case CMU_CLK_CHUB_BUS:
            cmuDrvSetDivChubBus(divider);
        break;

        case CMU_CLK_CMGP_I2C00:
            cmuDrvSetDivCmgpI2C00(divider);
        break;

        case CMU_CLK_CMGP_I2C01:
            cmuDrvSetDivCmgpI2C01(divider);
        break;

        case CMU_CLK_CMGP_I2C02:
            cmuDrvSetDivCmgpI2C02(divider);
        break;

        case CMU_CLK_CMGP_I2C03:
            cmuDrvSetDivCmgpI2C03(divider);
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
        val = 0;
        val |= (uint32_t)CHUB_CONTROLLER_ENABLE_DEBUG;
        val |= CHUB_CONTROLLER_OPTION_PM_ENABLE;
        val |= CHUB_CONTROLLER_OPTION_HWACG_ENABLE;
        __raw_write32(val, REG_CMU_CHUB_CMU_CHUB_CONTROLLER_OPTION);
    }
    else {
        // Disable HWACG and power management
        val = 0;
        val |= CHUB_CONTROLLER_OPTION_PM_DISABLE;
        val |= CHUB_CONTROLLER_OPTION_HWACG_DISABLE;
        __raw_write32(val, REG_CMU_CHUB_CMU_CHUB_CONTROLLER_OPTION);
    }

    // BUS_COMPONENT_DRCG_EN in SYSREG_CHUB
    sysregDrvSetHWACG(enable);

    isDrcgHwacgSet = 1;

    return true;
}

bool cmuDrvControlHwacgIP(IN CmuHwacgIpType ip, IN CmuHwacgControlType enable)
{
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

#if 0
            // CLK_CON_xxx
            val = __raw_read32(REG_CMU_CLK_CON_MUX_CLK_CHUB_TIMER_FCLK);
            if(enable)
                val |= CLK_CON_xxx_ENABLE_AUTOMATIC_CLKGATING;
            else
                val &= (uint32_t)~CLK_CON_xxx_ENABLE_AUTOMATIC_CLKGATING;
            __raw_write32(val, REG_CMU_CLK_CON_MUX_CLK_CHUB_TIMER_FCLK);
#endif

            val = __raw_read32(REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_TIMER_CHUB_IPCLKPORT_PCLK);
            if(enable)
                val |= CLK_CON_xxx_ENABLE_AUTOMATIC_CLKGATING;
            else
                val &= (uint32_t)~CLK_CON_xxx_ENABLE_AUTOMATIC_CLKGATING;
            __raw_write32(val, REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_TIMER_CHUB_IPCLKPORT_PCLK);
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

    return true;
}

// Need to increase if more SFRs need to be kept. Currently it is 23 SFRs of CMU
#define E9110_CMUSFR_NUM 25
#define E9110_CMUCHUB_SFR_NUM 15
static uint32_t buf_CMUSFR[E9110_CMUSFR_NUM];
void cmuDrvSaveState(void)
{
    uint32_t idx = 0;

    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_PLL_CON0_MUX_CLKCMU_CHUB_BUS_USER);
    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_PLL_CON0_MUX_CLKCMU_RCO_CHUB_BUS_USER);
    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_BUS);
    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_BUS);

    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_CHUB_CMU_CHUB_CONTROLLER_OPTION);
    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_QCH_CON_CM4_CHUB_QCH);
    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_CM4_CHUB_IPCLKPORT_FCLK);
    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_QCH_CON_PDMA_CHUB_QCH);
    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_PDMA_CHUB_IPCLKPORT_ACLK);
    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_QCH_CON_PWM_CHUB_QCH);
    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_PWM_CHUB_IPCLKPORT_I_PCLK_S0);
    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_QCH_CON_TIMER_CHUB_QCH);
    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_TIMER_CHUB_IPCLKPORT_PCLK);
    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_QCH_CON_WDT_CHUB_QCH);
    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_WDT_CHUB_IPCLKPORT_PCLK);

    //CSP_PRINTF_INFO("[CMU] %d SFRs saved\n", (int)idx);
}

void cmuDrvRestoreState(void)
{
    uint32_t idx = 0;

    __raw_write32(buf_CMUSFR[idx++], REG_CMU_PLL_CON0_MUX_CLKCMU_CHUB_BUS_USER);
    while(__raw_read32(REG_CMU_PLL_CON0_MUX_CLKCMU_CHUB_BUS_USER) & MUX_SEL_BUSY_OFFSET);

    __raw_write32(buf_CMUSFR[idx++], REG_CMU_PLL_CON0_MUX_CLKCMU_RCO_CHUB_BUS_USER);
    while(__raw_read32(REG_CMU_PLL_CON0_MUX_CLKCMU_RCO_CHUB_BUS_USER) & MUX_SEL_BUSY_OFFSET);

    __raw_write32(buf_CMUSFR[idx++], REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_BUS);
    while(__raw_read32(REG_CMU_CLK_CON_MUX_MUX_CLK_CHUB_BUS) & SELECT_BUSY_OFFSET);

    __raw_write32(buf_CMUSFR[idx++], REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_BUS);
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CHUB_BUS) & DIV_CLK_CHUB_BUS_IS_BUSY);

    __raw_write32(buf_CMUSFR[idx++], REG_CMU_CHUB_CMU_CHUB_CONTROLLER_OPTION);
    __raw_write32(buf_CMUSFR[idx++], REG_CMU_QCH_CON_CM4_CHUB_QCH);
    __raw_write32(buf_CMUSFR[idx++], REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_CM4_CHUB_IPCLKPORT_FCLK);
    __raw_write32(buf_CMUSFR[idx++], REG_CMU_QCH_CON_PDMA_CHUB_QCH);
    __raw_write32(buf_CMUSFR[idx++], REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_PDMA_CHUB_IPCLKPORT_ACLK);
    __raw_write32(buf_CMUSFR[idx++], REG_CMU_QCH_CON_PWM_CHUB_QCH);
    __raw_write32(buf_CMUSFR[idx++], REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_PWM_CHUB_IPCLKPORT_I_PCLK_S0);
    __raw_write32(buf_CMUSFR[idx++], REG_CMU_QCH_CON_TIMER_CHUB_QCH);
    __raw_write32(buf_CMUSFR[idx++], REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_TIMER_CHUB_IPCLKPORT_PCLK);
    __raw_write32(buf_CMUSFR[idx++], REG_CMU_QCH_CON_WDT_CHUB_QCH);
    __raw_write32(buf_CMUSFR[idx++], REG_CMU_CLK_CON_GAT_GOUT_BLK_CHUB_UID_WDT_CHUB_IPCLKPORT_PCLK);

    //CSP_PRINTF_INFO("[CMU] %d SFRs are restored\n", (int)idx);

}

void cmuDrvCmgpSaveState(void)
{
    uint32_t idx = E9110_CMUCHUB_SFR_NUM;

    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C0);
    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C1);
    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C2);
    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C3);
    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI0);
    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI1);
    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI2);
    buf_CMUSFR[idx++] = __raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI3);

    CSP_PRINTF_INFO("[CMU_CMGP] %d SFRs saved\n", (int)idx - E9110_CMUCHUB_SFR_NUM);
}

void cmuDrvCmgpRestoreState(void)
{
    uint32_t idx =  E9110_CMUCHUB_SFR_NUM;

    __raw_write32(buf_CMUSFR[idx++], REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C0);
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C0) & DIV_CLK_I2C_CMGP_IS_BUSY);

    __raw_write32(buf_CMUSFR[idx++], REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C1);
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C1) & DIV_CLK_I2C_CMGP_IS_BUSY);

    __raw_write32(buf_CMUSFR[idx++], REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C2);
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C2) & DIV_CLK_I2C_CMGP_IS_BUSY);

    __raw_write32(buf_CMUSFR[idx++], REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C3);
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_I2C3) & DIV_CLK_I2C_CMGP_IS_BUSY);

    __raw_write32(buf_CMUSFR[idx++], REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI0);
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI0) & DIV_CLK_USI_CMGP00_IS_BUSY);

    __raw_write32(buf_CMUSFR[idx++], REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI1);
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI1) & DIV_CLK_USI_CMGP01_IS_BUSY);

    __raw_write32(buf_CMUSFR[idx++], REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI2);
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI2) & DIV_CLK_USI_CMGP02_IS_BUSY);

    __raw_write32(buf_CMUSFR[idx++], REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI3);
    while(__raw_read32(REG_CMU_CLK_CON_DIV_DIV_CLK_CMGP_USI3) & DIV_CLK_USI_CMGP03_IS_BUSY);

    CSP_PRINTF_INFO("[CMU_CMGP] %d SFRs are restored\n", (int)idx - E9110_CMUCHUB_SFR_NUM);

}
