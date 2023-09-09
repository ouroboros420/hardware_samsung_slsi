/*----------------------------------------------------------------------------
 *      Exynos SoC  -  SYSREG
 *----------------------------------------------------------------------------
 *      Name:    sysreg6315.c
 *      Purpose: To implement SYSREG driver functions
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

#include <sysreg.h>

#define REG_SYSREG_USER_REG0                (SYSREG_BASE_ADDRESS + 0x0)
#define REG_SYSREG_USER_REG1                (SYSREG_BASE_ADDRESS + 0x4)
#define REG_SYSREG_USER_REG2                (SYSREG_BASE_ADDRESS + 0x8)
#define REG_SYSREG_USER_REG3                (SYSREG_BASE_ADDRESS + 0xc)
#define REG_SYSREG_USER_REG4                (SYSREG_BASE_ADDRESS + 0x10)

#define REG_SYSREG_SFR_APB                  (SYSREG_BASE_ADDRESS + 0x100)
#define REG_SYSREG_BUS_COMPONENT_DRCG_EN	(SYSREG_BASE_ADDRESS + 0x104)
#define REG_SYSREG_MEMCLK	                (SYSREG_BASE_ADDRESS + 0x108)
#define REG_SYSREG_MEMCTRL_D	            (SYSREG_BASE_ADDRESS + 0x10c)
#define REG_SYSREG_MEMCTRL_I	            (SYSREG_BASE_ADDRESS + 0x110)

#define REG_SYSREG_MIF_REQ_OUT	            (SYSREG_BASE_ADDRESS + 0x200)
#define REG_SYSREG_MIF_REQ_ACK_IN	        (SYSREG_BASE_ADDRESS + 0x204)
#define REG_SYSREG_PD_REQ	                (SYSREG_BASE_ADDRESS + 0x220)
#define REG_SYSREG_EARLY_WAKEUP_WINDOW_REQ	(SYSREG_BASE_ADDRESS + 0x230)
#define REG_SYSREG_APM_UP_STATUS	        (SYSREG_BASE_ADDRESS + 0x240)
#define REG_SYSREG_CLEAR_VVALID	            (SYSREG_BASE_ADDRESS + 0x250)
#define REG_SYSREG_CMGP_REQ_OUT	            (SYSREG_BASE_ADDRESS + 0x260)
#define REG_SYSREG_CMGP_REQ_ACK_IN	        (SYSREG_BASE_ADDRESS + 0x264)
#define REG_SYSREG_OSCCLK_ENABLE	        (SYSREG_BASE_ADDRESS + 0x270)

#define REG_SYSREG_LPC_EMA	                (SYSREG_BASE_ADDRESS + 0x300)
#define REG_SYSREG_LPC_EMAW	                (SYSREG_BASE_ADDRESS + 0x304)
#define REG_SYSREG_LPC_EMAA	                (SYSREG_BASE_ADDRESS + 0x308)
#define REG_SYSREG_LPC_EMAB	                (SYSREG_BASE_ADDRESS + 0x30c)
#define REG_SYSREG_LPC_EMAS	                (SYSREG_BASE_ADDRESS + 0x310)
#define REG_SYSREG_LPC_KEN	                (SYSREG_BASE_ADDRESS + 0x314)
#define REG_SYSREG_EMA_STATUS	            (SYSREG_BASE_ADDRESS + 0x318)

#define REG_SYSREG_DEBUG	                (SYSREG_BASE_ADDRESS + 0x404)

#define REG_SYSREG_HWACG_CM4_STATUS	        (SYSREG_BASE_ADDRESS + 0x420)
#define REG_SYSREG_HWACG_CM4_CONFIG	        (SYSREG_BASE_ADDRESS + 0x424)
#define REG_SYSREG_HWACG_CM4_CLKREQ_0	    (SYSREG_BASE_ADDRESS + 0x428)
#define REG_SYSREG_HWACG_CM4_CLKREQ_1	    (SYSREG_BASE_ADDRESS + 0x42c)
#define REG_SYSREG_HWACG_CM4_CLKREQ_2	    (SYSREG_BASE_ADDRESS + 0x430)
#define REG_SYSREG_HWACG_CM4_CLKREQ_3	    (SYSREG_BASE_ADDRESS + 0x434)

#define REG_SYSREG_SRAM_CON_I_CHUB	        (SYSREG_BASE_ADDRESS + 0x800)
#define REG_SYSREG_SRAM_CON_D_CHUB	        (SYSREG_BASE_ADDRESS + 0x804)

#define REG_SYSREG_BPS_SEL_RESP	            (SYSREG_BASE_ADDRESS + 0x900)
#define REG_SYSREG_AxCACHE_VALUE_C	        (SYSREG_BASE_ADDRESS + 0x910)
#define REG_SYSREG_AxCACHE_MUX_SEL_C	    (SYSREG_BASE_ADDRESS + 0x914)

#define REG_SYSREG_SPI_MULTI_SLV_Q_CTRL	    (SYSREG_BASE_ADDRESS + 0x1000)
#define REG_SYSREG_SPI_I2C_CHUB1_SPI_MULTI_SLV_MODE\
                                            (SYSREG_BASE_ADDRESS + 0x1010)
#define REG_SYSREG_USI_CHUB1_SPI_MULTI_SLV_MODE\
                                            (SYSREG_BASE_ADDRESS + 0x1014)
#define REG_SYSREG_SPI_I2C_CHUB0_SPI_MULTI_SLV_MODE\
                                            (SYSREG_BASE_ADDRESS + 0x1018)
#define REG_SYSREG_USI_CHUB0_SPI_MULTI_SLV_MODE\
                                            (SYSREG_BASE_ADDRESS + 0x101c)

#define REG_SYSREG_USI_CHUB0_SW_CONF	    (SYSREG_BASE_ADDRESS + 0x2000)
#define REG_SYSREG_SPI_I2C_CHUB0_SW_CONF	(SYSREG_BASE_ADDRESS + 0x2004)
#define REG_SYSREG_USI_CHUB1_SW_CONF	    (SYSREG_BASE_ADDRESS + 0x2010)
#define REG_SYSREG_SPI_I2C_CHUB1_SW_CONF	(SYSREG_BASE_ADDRESS + 0x2014)
#define REG_SYSREG_USI_CHUB2_SW_CONF	    (SYSREG_BASE_ADDRESS + 0x2020)
#define REG_SYSREG_USI_CHUB3_SW_CONF	    (SYSREG_BASE_ADDRESS + 0x2030)
#define REG_SYSREG_I2C_CHUB0_SW_CONF	    (SYSREG_BASE_ADDRESS + 0x2040)
#define REG_SYSREG_I2C_CHUB1_SW_CONF	    (SYSREG_BASE_ADDRESS + 0x2050)

#define REG_SYSREG_USI_CHUB0_IPCLK	        (SYSREG_BASE_ADDRESS + 0x3000)
#define REG_SYSREG_SPI_I2C_CHUB0_IPCLK	    (SYSREG_BASE_ADDRESS + 0x3004)
#define REG_SYSREG_USI_CHUB1_IPCLK	        (SYSREG_BASE_ADDRESS + 0x3010)
#define REG_SYSREG_SPI_I2C_CHUB1_IPCLK	    (SYSREG_BASE_ADDRESS + 0x3014)
#define REG_SYSREG_USI_CHUB2_IPCLK	        (SYSREG_BASE_ADDRESS + 0x3020)
#define REG_SYSREG_USI_CHUB3_IPCLK	        (SYSREG_BASE_ADDRESS + 0x3030)
#define REG_SYSREG_I2C_CHUB0_IPCLK	        (SYSREG_BASE_ADDRESS + 0x3040)
#define REG_SYSREG_I2C_CHUB1_IPCLK	        (SYSREG_BASE_ADDRESS + 0x3050)


typedef enum {
    SWCONF_USI_CHUB00_SW_CONF,
    SWCONF_SPI_I2C_CHUB00_SW_CONF,
    SWCONF_USI_CHUB01_SW_CONF,
    SWCONF_SPI_I2C_CHUB01_SW_CONF,
    SWCONF_USI_CHUB02_SW_CONF,
    SWCONF_USI_CHUB03_SW_CONF,
    SWCONF_I2C_CHUB00_SW_CONF,
    SWCONF_I2C_CHUB01_SW_CONF,

    SYSREG_SWCONF_PORT_MAX
} SysregSwConfPortType;

typedef enum {
    SWCONF_USI_UART = 0x1,
    SWCONF_USI_SPI = 0x2,
    SWCONF_USI_I2C = 0x4,

    SYSREG_SWCONF_PROTOCOL_MAX
} SysregSwConfProtocolType;
#define SYSREG_CHUB_DRCG_ENABLE	    0xF
#define SYSREG_CHUB_DRCG_DISABLE    0x0
#define SYSREQ_CHUB_HWACG_CM4_CLKREQ_MASK_IRQ   0x3ffffff


//Public API to mask IRQs for CHUB CM4 clock request
void sysregSetMaskIrq(IN uint64_t irq)
{
    uint32_t regValue;

    regValue = (uint32_t)irq;
    __raw_writel(regValue, REG_SYSREG_HWACG_CM4_CLKREQ_0);

    regValue |= (uint32_t)(irq>>32);
    __raw_writel(regValue, REG_SYSREG_HWACG_CM4_CLKREQ_2);
}

uint64_t sysregGetMaskIrq(void)
{
    uint64_t regValue;

    regValue = __raw_readl(REG_SYSREG_HWACG_CM4_CLKREQ_2);
    regValue = regValue << 32;
    regValue |= __raw_readl(REG_SYSREG_HWACG_CM4_CLKREQ_0);

    return regValue;
}

//
void sysregSetDRCG(IN uint32_t enable)
{
    if(enable) {
        __raw_writel(SYSREG_CHUB_DRCG_ENABLE, REG_SYSREG_BUS_COMPONENT_DRCG_EN);
    }
    else {
        __raw_writel(SYSREG_CHUB_DRCG_DISABLE, REG_SYSREG_BUS_COMPONENT_DRCG_EN);
    }
}

void sysregSetOscEn(void)
{
    uint32_t regValue;

    regValue = 0x1;
    __raw_writel(regValue, REG_SYSREG_OSCCLK_ENABLE);
}

void sysregSetOscDis(void)
{
    uint32_t regValue;

    regValue = 0x0;
    __raw_writel(regValue, REG_SYSREG_OSCCLK_ENABLE);
}

void sysregSetCmgpReq(void)
{
    uint32_t regValue;

    regValue = 0x1;
    __raw_writel(regValue, REG_SYSREG_CMGP_REQ_OUT);
}

void sysregClearCmgpReq(void)
{
    uint32_t regValue;

    regValue = 0x0;
    __raw_writel(regValue, REG_SYSREG_CMGP_REQ_OUT);
}

void sysregInit(void)
{
}

void sysregSaveState(void)
{
}

void sysregRestoreState(void)
{
}

