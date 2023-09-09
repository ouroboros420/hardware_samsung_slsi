/*----------------------------------------------------------------------------
 *      Exynos SoC  -  SYSREG
 *----------------------------------------------------------------------------
 *      Name:    sysreg9815.c
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

#define    REG_SYSREG_BUS_COMPONENET_DRCG_EN       (SYSREG_BASE_ADDRESS + 0x104)
#define    REG_SYSREG_PD_REQ                       (SYSREG_BASE_ADDRESS + 0x220)
#define    REG_SYSREG_EALY_WAKEUP_WINDOW_REQ       (SYSREG_BASE_ADDRESS + 0x230)
#define    REG_SYSREG_APM_UP_STATUS                (SYSREG_BASE_ADDRESS + 0x240)
#define    REG_SYSREG_CLEAR_VVALID                 (SYSREG_BASE_ADDRESS + 0x250)
#define    REG_SYSREG_CMGP_REQ_OUT                 (SYSREG_BASE_ADDRESS + 0x260)
#define    REG_SYSREG_CMGP_REQ_ACK_IN              (SYSREG_BASE_ADDRESS + 0x264)
#define    REG_SYSREG_OSCCLK_ENABLE                (SYSREG_BASE_ADDRESS + 0x270)
#define    REG_SYSREG_HWACG_CM4_CLKREQ             (SYSREG_BASE_ADDRESS + 0x428)
#define    REG_SYSREG_HWACG_CM4_CLKREQ1            (SYSREG_BASE_ADDRESS + 0x42C)
#define    REG_SYSREG_HWACG_CM4_CLKREQ2            (SYSREG_BASE_ADDRESS + 0x430)

#define    REG_SYSREG_USI_CHUB00_SW_CONF           (SYSREG_BASE_ADDRESS + 0x2000)
#define    REG_SYSREG_I2C_CHUB00_SW_CONF           (SYSREG_BASE_ADDRESS + 0x2004)
#define    REG_SYSREG_USI_CHUB01_SW_CONF           (SYSREG_BASE_ADDRESS + 0x2010)
#define    REG_SYSREG_I2C_CHUB01_SW_CONF           (SYSREG_BASE_ADDRESS + 0x2014)
#define    REG_SYSREG_USI_CHUB02_SW_CONF           (SYSREG_BASE_ADDRESS + 0x2020)
#define    REG_SYSREG_I2C_CHUB02_SW_CONF           (SYSREG_BASE_ADDRESS + 0x2024)

#define    REG_SYSREG_USI_CHUB00_IPCLK             (SYSREG_BASE_ADDRESS + 0x3000)
#define    REG_SYSREG_I2C_CHUB00_IPCLK             (SYSREG_BASE_ADDRESS + 0x3004)
#define    REG_SYSREG_USI_CHUB01_IPCLK             (SYSREG_BASE_ADDRESS + 0x3010)
#define    REG_SYSREG_I2C_CHUB01_IPCLK             (SYSREG_BASE_ADDRESS + 0x3014)
#define    REG_SYSREG_USI_CHUB02_IPCLK             (SYSREG_BASE_ADDRESS + 0x3020)
#define    REG_SYSREG_I2C_CHUB02_IPCLK             (SYSREG_BASE_ADDRESS + 0x3024)

#define    REG_SYSREG_USI_CMGP00_SW_CONF           (SYSREG_CMGP_BASE_ADDRESS + 0x2000)
#define    REG_SYSREG_I2C_CMGP00_SW_CONF           (SYSREG_CMGP_BASE_ADDRESS + 0x2004)
#define    REG_SYSREG_USI_CMGP01_SW_CONF           (SYSREG_CMGP_BASE_ADDRESS + 0x2010)
#define    REG_SYSREG_I2C_CMGP01_SW_CONF           (SYSREG_CMGP_BASE_ADDRESS + 0x2014)
#define    REG_SYSREG_USI_CMGP02_SW_CONF           (SYSREG_CMGP_BASE_ADDRESS + 0x2020)
#define    REG_SYSREG_I2C_CMGP02_SW_CONF           (SYSREG_CMGP_BASE_ADDRESS + 0x2024)
#define    REG_SYSREG_USI_CMGP03_SW_CONF           (SYSREG_CMGP_BASE_ADDRESS + 0x2030)
#define    REG_SYSREG_I2C_CMGP03_SW_CONF           (SYSREG_CMGP_BASE_ADDRESS + 0x2034)

#define    REG_SYSREG_USI_CMGP00_IPCLK             (SYSREG_CMGP_BASE_ADDRESS + 0x3000)
#define    REG_SYSREG_I2C_CMGP00_IPCLK             (SYSREG_CMGP_BASE_ADDRESS + 0x3004)
#define    REG_SYSREG_USI_CMGP01_IPCLK             (SYSREG_CMGP_BASE_ADDRESS + 0x3010)
#define    REG_SYSREG_I2C_CMGP01_IPCLK             (SYSREG_CMGP_BASE_ADDRESS + 0x3014)
#define    REG_SYSREG_USI_CMGP02_IPCLK             (SYSREG_CMGP_BASE_ADDRESS + 0x3020)
#define    REG_SYSREG_I2C_CMGP02_IPCLK             (SYSREG_CMGP_BASE_ADDRESS + 0x3024)
#define    REG_SYSREG_USI_CMGP03_IPCLK             (SYSREG_CMGP_BASE_ADDRESS + 0x3030)
#define    REG_SYSREG_I2C_CMGP03_IPCLK             (SYSREG_CMGP_BASE_ADDRESS + 0x3034)

typedef enum {
    SWCONF_USI_CHUB00_SW_CONF,
    SWCONF_I2C_CHUB00_SW_CONF,
    SWCONF_USI_CHUB01_SW_CONF,
    SWCONF_I2C_CHUB01_SW_CONF,
    SWCONF_USI_CHUB02_SW_CONF,
    SWCONF_I2C_CHUB02_SW_CONF,

    SYSREG_SWCONF_PORT_MAX
} SysregSwConfPortType;

typedef enum {
    SWCONF_USI_UART = 0x1,
    SWCONF_USI_SPI = 0x2,
    SWCONF_USI_I2C = 0x4,

    SYSREG_SWCONF_PROTOCOL_MAX
} SysregSwConfProtocolType;
#define SYSREG_CHUB_DRCG_ENABLE	    0x3FF
#define SYSREG_CHUB_DRCG_DISABLE    0x0
#define SYSREQ_CHUB_HWACG_CM4_CLKREQ_MASK_IRQ   0x3ffffff


//Public API to mask IRQs for CHUB CM4 clock request
void sysregSetMaskIrq(IN uint64_t irq)
{
    uint32_t regValue;

    regValue = (uint32_t)irq;
    __raw_writel(regValue, REG_SYSREG_HWACG_CM4_CLKREQ);

    regValue |= (uint32_t)(irq>>32);
    __raw_writel(regValue, REG_SYSREG_HWACG_CM4_CLKREQ2);
}

uint64_t sysregGetMaskIrq(void)
{
    uint64_t regValue;

    regValue = __raw_readl(REG_SYSREG_HWACG_CM4_CLKREQ2);
    regValue = regValue << 32;
    regValue |= __raw_readl(REG_SYSREG_HWACG_CM4_CLKREQ);

    return regValue;
}

//
void sysregSetDRCG(IN uint32_t enable)
{
    if(enable) {
        __raw_writel(SYSREG_CHUB_DRCG_ENABLE, REG_SYSREG_BUS_COMPONENET_DRCG_EN);
    }
    else {
        __raw_writel(SYSREG_CHUB_DRCG_DISABLE, REG_SYSREG_BUS_COMPONENET_DRCG_EN);
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

