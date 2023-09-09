/*----------------------------------------------------------------------------
 *      Exynos SoC  -  SYSREG
 *----------------------------------------------------------------------------
 *      Name:    sysreg9925.c
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
#define    REG_SYSREG_MEMCLK                       (SYSREG_BASE_ADDRESS + 0x108)
#define    REG_SYSREG_MEMCTRL_D                    (SYSREG_BASE_ADDRESS + 0x10C)
#define    REG_SYSREG_MEMCTRL_I                    (SYSREG_BASE_ADDRESS + 0x110)
#define    REG_SYSREG_MIF_REQ_OUT                  (SYSREG_BASE_ADDRESS + 0x200)
#define    REG_SYSREG_MIF_REQ_ACK_IN               (SYSREG_BASE_ADDRESS + 0x204)
#define    REG_SYSREG_PD_REQ                       (SYSREG_BASE_ADDRESS + 0x220)
#define    REG_SYSREG_EARLY_WAKEUP_WINDOW_REQ       (SYSREG_BASE_ADDRESS + 0x230)
#define    REG_SYSREG_APM_UP_STATUS                (SYSREG_BASE_ADDRESS + 0x240)
#define    REG_SYSREG_CLEAR_VVALID                 (SYSREG_BASE_ADDRESS + 0x250)
#define    REG_SYSREG_CMGP_REQ_OUT                 (SYSREG_BASE_ADDRESS + 0x260)
#define    REG_SYSREG_CMGP_REQ_ACK_IN              (SYSREG_BASE_ADDRESS + 0x264)
#define    REG_SYSREG_OSCCLK_ENABLE                (SYSREG_BASE_ADDRESS + 0x270)
#define    REG_SYSREG_AON_CIS                      (SYSREG_BASE_ADDRESS + 0x280)
#define    REG_SYSREG_HWACG_CM4_STATUS             (SYSREG_BASE_ADDRESS + 0x420)
#define    REG_SYSREG_HWACG_CM4_CONFIG             (SYSREG_BASE_ADDRESS + 0x424)
#define    REG_SYSREG_HWACG_CM4_CLKREQ0            (SYSREG_BASE_ADDRESS + 0x428)
#define    REG_SYSREG_HWACG_CM4_CLKREQ             (REG_SYSREG_HWACG_CM4_CLKREQ0)
#define    REG_SYSREG_HWACG_CM4_CLKREQ1            (SYSREG_BASE_ADDRESS + 0x42C)
#define    REG_SYSREG_HWACG_CM4_CLKREQ2            (SYSREG_BASE_ADDRESS + 0x430)
#define    REG_SYSREG_HWACG_CM4_CLKREQ3            (SYSREG_BASE_ADDRESS + 0x434)

#define    REG_SYSREG_CHUB_VVALID_INTR             (SYSREG_BASE_ADDRESS + 0x600)
#define    REG_SYSREG_CHUB_VVALID_COUNTER          (SYSREG_BASE_ADDRESS + 0x604)

#define    REG_SYSREG_SPI_CHUB_MULTI_SLV_Q_CTRL         (SYSREG_BASE_ADDRESS + 0x1000)
#define    REG_SYSREG_SPI_I2C_CHUB1_SPI_MULTI_SLV_MODE (SYSREG_BASE_ADDRESS + 0x1010)
#define    USI_CHUB1_SPI_MULTI_SLV_MODE            (SYSREG_BASE_ADDRESS + 0x1014)
#define    SPI_I2C_CHUB0_SPI_MULTI_SLV_MODE        (SYSREG_BASE_ADDRESS + 0x1018)
#define    USI_CHUB0_SPI_MULTI_SLV_MODE            (SYSREG_BASE_ADDRESS + 0x101C)

#define    REG_SYSREG_USI_CHUB00_SW_CONF           (SYSREG_BASE_ADDRESS + 0x2000)
#define    REG_SYSREG_SPI_I2C_CHUB00_SW_CONF       (SYSREG_BASE_ADDRESS + 0x2004)
#define    REG_SYSREG_USI_CHUB01_SW_CONF           (SYSREG_BASE_ADDRESS + 0x2010)
#define    REG_SYSREG_SPI_I2C_CHUB01_SW_CONF       (SYSREG_BASE_ADDRESS + 0x2014)
#define    REG_SYSREG_USI_CHUB02_SW_CONF           (SYSREG_BASE_ADDRESS + 0x2020)
#define    REG_SYSREG_USI_CHUB03_SW_CONF           (SYSREG_BASE_ADDRESS + 0x2030)
#define    REG_SYSREG_I2C_CHUB_SW_CONF             (SYSREG_BASE_ADDRESS + 0x2040)

#define    REG_SYSREG_USI_CHUB00_IPCLK             (SYSREG_BASE_ADDRESS + 0x3000)
#define    REG_SYSREG_SPI_I2C_CHUB00_IPCLK         (SYSREG_BASE_ADDRESS + 0x3004)
#define    REG_SYSREG_USI_CHUB01_IPCLK             (SYSREG_BASE_ADDRESS + 0x3010)
#define    REG_SYSREG_SPI_I2C_CHUB01_IPCLK         (SYSREG_BASE_ADDRESS + 0x3014)
#define    REG_SYSREG_USI_CHUB02_IPCLK             (SYSREG_BASE_ADDRESS + 0x3020)
#define    REG_SYSREG_USI_CHUB03_IPCLK             (SYSREG_BASE_ADDRESS + 0x3030)
#define    REG_SYSREG_I2C_CHUB_IPCLK               (SYSREG_BASE_ADDRESS + 0x3040)

#define    REG_SYSREG_USI_CMGP00_SW_CONF           (SYSREG_CMGP_BASE_ADDRESS + 0x2000)
#define    REG_SYSREG_USI_CMGP01_SW_CONF           (SYSREG_CMGP_BASE_ADDRESS + 0x2010)
#define    REG_SYSREG_USI_CMGP02_SW_CONF           (SYSREG_CMGP_BASE_ADDRESS + 0x2020)
#define    REG_SYSREG_I2C_CMGP02_SW_CONF           (SYSREG_CMGP_BASE_ADDRESS + 0x2024)
#define    REG_SYSREG_USI_CMGP03_SW_CONF           (SYSREG_CMGP_BASE_ADDRESS + 0x2030)
#define    REG_SYSREG_I2C_CMGP03_SW_CONF           (SYSREG_CMGP_BASE_ADDRESS + 0x2034)
#define    REG_SYSREG_USI_CMGP04_SW_CONF           (SYSREG_CMGP_BASE_ADDRESS + 0x2040)
#define    REG_SYSREG_I2C_CMGP04_SW_CONF           (SYSREG_CMGP_BASE_ADDRESS + 0x2044)
#define    REG_SYSREG_USI_CMGP05_SW_CONF           (SYSREG_CMGP_BASE_ADDRESS + 0x2050)
#define    REG_SYSREG_I2C_CMGP05_SW_CONF           (SYSREG_CMGP_BASE_ADDRESS + 0x2054)
#define    REG_SYSREG_USI_CMGP06_SW_CONF           (SYSREG_CMGP_BASE_ADDRESS + 0x2060)
#define    REG_SYSREG_I2C_CMGP06_SW_CONF           (SYSREG_CMGP_BASE_ADDRESS + 0x2064)
#define    REG_SYSREG_SPI_I2C_CMGP00_SW_CONF       (SYSREG_CMGP_BASE_ADDRESS + 0x2070)
#define    REG_SYSREG_SPI_I2C_CMGP01_SW_CONF       (SYSREG_CMGP_BASE_ADDRESS + 0x2074)
#define    REG_SYSREG_CMGP_I2C_SW_CONF             (SYSREG_CMGP_BASE_ADDRESS + 0x2080)
#define    REG_SYSREG_CMGP_I2C_IPCLK               (SYSREG_CMGP_BASE_ADDRESS + 0x2084)

#define    REG_SYSREG_SPI_CMGP_MULTI_SLV_Q_CTRL         (SYSREG_CMGP_BASE_ADDRESS + 0x2200)
#define    REG_SYSREG_USI_CMGP0_SPI_MULTI_SLV_MODE (SYSREG_CMGP_BASE_ADDRESS + 0x2204)
#define    REG_SYSREG_SPI_I2C_CMGP0_SPI_MULTI_SLV_MODE (SYSREG_CMGP_BASE_ADDRESS + 0x2208)
#define    REG_SYSREG_USI_CMGP1_SPI_MULTI_SLV_MODE (SYSREG_CMGP_BASE_ADDRESS + 0x220C)
#define    REG_SYSREG_SPI_I2C_CMGP1_SPI_MULTI_SLV_MODE (SYSREG_CMGP_BASE_ADDRESS + 0x2210)

#define    REG_SYSREG_USI_CMGP00_IPCLK             (SYSREG_CMGP_BASE_ADDRESS + 0x3000)
#define    REG_SYSREG_USI_CMGP01_IPCLK             (SYSREG_CMGP_BASE_ADDRESS + 0x3010)
#define    REG_SYSREG_USI_CMGP02_IPCLK             (SYSREG_CMGP_BASE_ADDRESS + 0x3020)
#define    REG_SYSREG_I2C_CMGP02_IPCLK             (SYSREG_CMGP_BASE_ADDRESS + 0x3024)
#define    REG_SYSREG_USI_CMGP03_IPCLK             (SYSREG_CMGP_BASE_ADDRESS + 0x3030)
#define    REG_SYSREG_I2C_CMGP03_IPCLK             (SYSREG_CMGP_BASE_ADDRESS + 0x3034)
#define    REG_SYSREG_USI_CMGP04_IPCLK             (SYSREG_CMGP_BASE_ADDRESS + 0x3040)
#define    REG_SYSREG_I2C_CMGP04_IPCLK             (SYSREG_CMGP_BASE_ADDRESS + 0x3044)
#define    REG_SYSREG_USI_CMGP05_IPCLK             (SYSREG_CMGP_BASE_ADDRESS + 0x3050)
#define    REG_SYSREG_I2C_CMGP05_IPCLK             (SYSREG_CMGP_BASE_ADDRESS + 0x3054)
#define    REG_SYSREG_USI_CMGP06_IPCLK             (SYSREG_CMGP_BASE_ADDRESS + 0x3060)
#define    REG_SYSREG_I2C_CMGP06_IPCLK             (SYSREG_CMGP_BASE_ADDRESS + 0x3064)
#define    REG_SYSREG_SPI_I2C_CMGP00_IPCLK         (SYSREG_CMGP_BASE_ADDRESS + 0x3070)
#define    REG_SYSREG_SPI_I2C_CMGP01_IPCLK         (SYSREG_CMGP_BASE_ADDRESS + 0x3074)
#define    REG_SYSREG_I3C_CMGP_VGPIO_TX_DATA       (SYSREG_CMGP_BASE_ADDRESS + 0x3080)
#define    REG_SYSREG_I3C_CMGP_VGPIO_TX_CON        (SYSREG_CMGP_BASE_ADDRESS + 0x3084)

#define    REG_SYSREG_INT_EN_SET                   (COMB_WAKEUP_SRC_BASE_ADDRESS + 0x288)
#define    REG_SYSREG_INT_EN_CLR                   (COMB_WAKEUP_SRC_BASE_ADDRESS + 0x28C)

/* REG_SYSREG_PD_REQ */
#define    PD_REQ                                  (1<<0)

/* REG_SYSREG_EARLY_WAKEUP_WINDOW_REQ */
#define    EARLY_WAKEUP_WINDOW_REQ                 (1<<0)



typedef enum {
    SWCONF_USI_CHUB00_SW_CONF,
    SWCONF_SPI_I2C_CHUB00_SW_CONF,
    SWCONF_USI_CHUB01_SW_CONF,
    SWCONF_SPI_I2C_CHUB01_SW_CONF,
    SWCONF_USI_CHUB02_SW_CONF,
    SWCONF_USI_CHUB03_SW_CONF,
    SWCONF_I2C_CHUB_SW_CONF,
    SYSREG_SWCONF_PORT_MAX
} SysregSwConfPortType;

typedef enum {
    SWCONF_USI_UART = 0x1,
    SWCONF_USI_SPI = 0x2,
    SWCONF_USI_I2C = 0x4,

    SYSREG_SWCONF_PROTOCOL_MAX
} SysregSwConfProtocolType;
#define SYSREG_CHUB_DRCG_ENABLE	    0xFF
#define SYSREG_CHUB_DRCG_DISABLE    0x0
#define SYSREQ_CHUB_HWACG_CM4_CLKREQ_MASK_IRQ   0x3ffffff

#define CHUB_VVALID_INTR_FLTEN                     (1<<5)
#define CHUB_VVALID_INTR_LLMODE                    (1<<4)
#define CHUB_VVALID_INTR_HLMODE                    (1<<3)
#define CHUB_VVALID_INTR_REMODE                    (1<<2)
#define CHUB_VVALID_INTR_FEMODE                    (1<<1)
#define CHUB_VVALID_INTR_IMODE                     (1<<0)

void sysregSetWakeupInterruptAll(void)
{
    __raw_writel(0xFFFFFFFF, REG_SYSREG_INT_EN_SET);
}

void sysregClrWakeupInterruptAll(void)
{
    __raw_writel(0xFFFFFFFF, REG_SYSREG_INT_EN_CLR);
}

void sysregPDRequest(void)
{
    __raw_writel(PD_REQ, (volatile uint32_t *)REG_SYSREG_PD_REQ);
}

void sysregPDRelease(void)
{
    __raw_writel(!PD_REQ, (volatile uint32_t *)REG_SYSREG_PD_REQ);
}

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

void sysregClearMaskIrq(uint64_t irq)
{
    uint32_t regValue;

    regValue = __raw_readl(REG_SYSREG_HWACG_CM4_CLKREQ);
    regValue &= ~((uint32_t)(irq & 0xFFFFFFFF));
    __raw_writel(regValue, REG_SYSREG_HWACG_CM4_CLKREQ);

    regValue = __raw_readl(REG_SYSREG_HWACG_CM4_CLKREQ2);
    regValue &= ~((uint32_t)(irq >> 32));
    __raw_writel(regValue, REG_SYSREG_HWACG_CM4_CLKREQ2);
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

void sysregSetEarlyWakeupWindowReq(bool en)
{
    uint32_t regValue;

    regValue = __raw_readl((volatile uint32_t *)REG_SYSREG_EARLY_WAKEUP_WINDOW_REQ);
    regValue &= ~EARLY_WAKEUP_WINDOW_REQ;
    if(en)
        regValue |= EARLY_WAKEUP_WINDOW_REQ;

    __raw_writel((uint32_t)regValue, (volatile uint32_t *)REG_SYSREG_EARLY_WAKEUP_WINDOW_REQ);
}

uint32_t sysregGetEarlyWakeupWindowReq(void)
{
    uint32_t regValue;

    regValue = __raw_readl((volatile uint32_t *)REG_SYSREG_EARLY_WAKEUP_WINDOW_REQ) & EARLY_WAKEUP_WINDOW_REQ;

    return regValue;
}

void sysregSetVvalidIntr(uint32_t intMode, uint32_t intMask)
{
    uint32_t regValue;

    regValue = CHUB_VVALID_INTR_FLTEN | ((intMode & 0xF) << 1) | (intMask & 0x1);
    __raw_writel(regValue, (volatile uint32_t *)REG_SYSREG_CHUB_VVALID_INTR);
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

