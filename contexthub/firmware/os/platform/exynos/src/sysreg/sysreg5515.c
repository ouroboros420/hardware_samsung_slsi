/*----------------------------------------------------------------------------
 * Exynos SoC  -  CSP
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

#include <csp_printf.h>
#include <sysreg.h>
#include <ipc_lpd.h>
#include <pwrDrv.h>
#include <pwrDrvPwrGating.h>

// CHUB
#define    REG_SYSREG_BUS_COMPONENET_DRCG_EN       (SYSREG_BASE_ADDRESS + 0x104)
#define    REG_SYSREG_MEMCLK                       (SYSREG_BASE_ADDRESS + 0x108)
#define    REG_SYSREG_MEMCTRL_D                    (SYSREG_BASE_ADDRESS + 0x10C)
#define    REG_SYSREG_MEMCTRL_I                    (SYSREG_BASE_ADDRESS + 0x110)
#define    REG_SYSREG_MIF_REQ                      (SYSREG_BASE_ADDRESS + 0x200)
#define    REG_SYSREG_MIF_REQ_ACK                  (SYSREG_BASE_ADDRESS + 0x204)
#define    REG_SYSREG_PD_REQ                       (SYSREG_BASE_ADDRESS + 0x220)
#define    REG_SYSREG_EARLY_WAKEUP_WINDOW_REG      (SYSREG_BASE_ADDRESS + 0x230)
#define    REG_SYSREG_APM_UP_STATUS                (SYSREG_BASE_ADDRESS + 0x240)
#define    REG_SYSREG_CLEAR_VVALID                 (SYSREG_BASE_ADDRESS + 0x250)
#define    REG_SYSREG_LPP_MCS                      (SYSREG_BASE_ADDRESS + 0x300)
#define    REG_SYSREG_LPP_MCSW                     (SYSREG_BASE_ADDRESS + 0x304)
#define    REG_SYSREG_LPP_MCSRD                    (SYSREG_BASE_ADDRESS + 0x308)
#define    REG_SYSREG_LPP_MCSWR                    (SYSREG_BASE_ADDRESS + 0x30C)
#define    REG_SYSREG_LPP_KCS                      (SYSREG_BASE_ADDRESS + 0x310)
#define    REG_SYSREG_LPP_ADME                     (SYSREG_BASE_ADDRESS + 0x314)
#define    REG_SYSREG_LPP_WRME                     (SYSREG_BASE_ADDRESS + 0x318)
#define    REG_SYSREG_EMA_STATUS                   (SYSREG_BASE_ADDRESS + 0x31C)
#define    REG_SYSREG_CMGP_REQ_OUT                 (SYSREG_BASE_ADDRESS + 0x320)
#define    REG_SYSREG_CMGP_REQ_ACK_IN              (SYSREG_BASE_ADDRESS + 0x324)
#define    REG_SYSREG_DEBUG                        (SYSREG_BASE_ADDRESS + 0x404)
#define    REG_SYSREG_HWACG_CM4_SATUS              (SYSREG_BASE_ADDRESS + 0x420)
#define    REG_SYSREG_HWACG_CM4_CONFIG             (SYSREG_BASE_ADDRESS + 0x424)
#define    REG_SYSREG_AXQOS                        (SYSREG_BASE_ADDRESS + 0x434)
#define    REG_SYSREG_HWACG_CM4_INT0               (SYSREG_BASE_ADDRESS + 0x500)
#define    REG_SYSREG_HWACG_CM4_INT1               (SYSREG_BASE_ADDRESS + 0x504)
#define    REG_SYSREG_HWACG_CM4_INT2               (SYSREG_BASE_ADDRESS + 0x508)
#define    REG_SYSREG_CHUB_VVALID_INTR             (SYSREG_BASE_ADDRESS + 0x600)
#define    REG_SYSREG_CHUB_VVALID_COUNTER          (SYSREG_BASE_ADDRESS + 0x604)
#define    REG_SYSREG_CHUB_SCALABLE_CTRL0          (SYSREG_BASE_ADDRESS + 0x700)
#define    REG_SYSREG_CHUB_SCALABLE_CTRL1          (SYSREG_BASE_ADDRESS + 0x704)
#define    REG_SYSREG_SRAM_CON_I_CHUB              (SYSREG_BASE_ADDRESS + 0x800)
#define    REG_SYSREG_SRAM_CON_D_CHUB              (SYSREG_BASE_ADDRESS + 0x804)
#define    REG_SYSREG_BPS_SEL_RESP                 (SYSREG_BASE_ADDRESS + 0x900)
#define    REG_SYSREG_YAMIN_SECURE                 (SYSREG_BASE_ADDRESS + 0x1000)
#define    REG_SYSREG_YAMIN_NONSECURE              (SYSREG_BASE_ADDRESS + 0x1004)
#define    REG_SYSREG_YAMIN_INST_CTRL              (SYSREG_BASE_ADDRESS + 0x1008)
#define    REG_SYSREG_YAMIN_INST_FAULT0            (SYSREG_BASE_ADDRESS + 0x100C)
#define    REG_SYSREG_YAMIN_INST_FAULT1            (SYSREG_BASE_ADDRESS + 0x1010)
#define    REG_SYSREG_INTREQ_CM4_TO                (SYSREG_BASE_ADDRESS + 0x1014)
#define    REG_SYSREG_INTREQ_YAMIN_TO              (SYSREG_BASE_ADDRESS + 0x1018)
#define    REG_SYSREG_HWACG_YAMIN_INT0             (SYSREG_BASE_ADDRESS + 0x1100)
#define    REG_SYSREG_HWACG_YAMIN_INT1             (SYSREG_BASE_ADDRESS + 0x1104)
#define    REG_SYSREG_YAMIN_INST_CUR               (SYSREG_BASE_ADDRESS + 0x1200)
#define    REG_SYSREG_USI_CHUB0_SW_CONF            (SYSREG_BASE_ADDRESS + 0x3000)
#define    REG_SYSREG_I2C_CHUB0_SW_CONF            (SYSREG_BASE_ADDRESS + 0x3004)
#define    REG_SYSREG_I2C_CHUB1_SW_CONF            (SYSREG_BASE_ADDRESS + 0x3008)
#define    REG_SYSREG_USI_CHUB0_IPCLK              (SYSREG_BASE_ADDRESS + 0x3000)
#define    REG_SYSREG_I2C_CHUB0_IPCLK              (SYSREG_BASE_ADDRESS + 0x3004)
#define    REG_SYSREG_I2C_CHUB1_IPCLK              (SYSREG_BASE_ADDRESS + 0x3008)

// CMGP -- need to check base address
#define    REG_SYSREG_USI_CMGP0_SW_CONF            (SYSREG_CMGP_BASE_ADDRESS + 0x2000)
#define    REG_SYSREG_I2C_CMGP0_SW_CONF            (SYSREG_CMGP_BASE_ADDRESS + 0x2004)
#define    REG_SYSREG_USI_CMGP1_SW_CONF            (SYSREG_CMGP_BASE_ADDRESS + 0x2010)
#define    REG_SYSREG_I2C_CMGP1_SW_CONF            (SYSREG_CMGP_BASE_ADDRESS + 0x2014)
#define    REG_SYSREG_USI_CMGP2_SW_CONF            (SYSREG_CMGP_BASE_ADDRESS + 0x2020)
#define    REG_SYSREG_I2C_CMGP2_SW_CONF            (SYSREG_CMGP_BASE_ADDRESS + 0x2024)
#define    REG_SYSREG_USI_CMGP3_SW_CONF            (SYSREG_CMGP_BASE_ADDRESS + 0x2030)
#define    REG_SYSREG_I2C_CMGP3_SW_CONF            (SYSREG_CMGP_BASE_ADDRESS + 0x2034)
#define    REG_SYSREG_I2C_CMGP4_SW_CONF            (SYSREG_CMGP_BASE_ADDRESS + 0x2040)
#define    REG_SYSREG_I2C_CMGP5_SW_CONF            (SYSREG_CMGP_BASE_ADDRESS + 0x2050)
#define    REG_SYSREG_I2C_CMGP6_SW_CONF            (SYSREG_CMGP_BASE_ADDRESS + 0x2060)

#define    REG_SYSREG_USI_CMGP0_IPCLK              (SYSREG_CMGP_BASE_ADDRESS + 0x3000)
#define    REG_SYSREG_I2C_CMGP0_IPCLK              (SYSREG_CMGP_BASE_ADDRESS + 0x3004)
#define    REG_SYSREG_USI_CMGP1_IPCLK              (SYSREG_CMGP_BASE_ADDRESS + 0x3010)
#define    REG_SYSREG_I2C_CMGP1_IPCLK              (SYSREG_CMGP_BASE_ADDRESS + 0x3014)
#define    REG_SYSREG_USI_CMGP2_IPCLK              (SYSREG_CMGP_BASE_ADDRESS + 0x3020)
#define    REG_SYSREG_I2C_CMGP2_IPCLK              (SYSREG_CMGP_BASE_ADDRESS + 0x3024)
#define    REG_SYSREG_USI_CMGP3_IPCLK              (SYSREG_CMGP_BASE_ADDRESS + 0x3030)
#define    REG_SYSREG_I2C_CMGP3_IPCLK              (SYSREG_CMGP_BASE_ADDRESS + 0x3034)
#define    REG_SYSREG_I2C_CMGP4_IPCLK              (SYSREG_CMGP_BASE_ADDRESS + 0x3040)
#define    REG_SYSREG_I2C_CMGP5_IPCLK              (SYSREG_CMGP_BASE_ADDRESS + 0x3050)
#define    REG_SYSREG_I2C_CMGP6_IPCLK              (SYSREG_CMGP_BASE_ADDRESS + 0x3060)

#define    REG_SYSREG_ADC_AP_FLAG                  (SYSREG_CMGP_BASE_ADDRESS + 0x0008)
#define    REG_SYSREG_ADC_CHUB_FLAG                (SYSREG_CMGP_BASE_ADDRESS + 0x000C)
#define    REG_SYSREG_ADC_INIT_TURN                (SYSREG_CMGP_BASE_ADDRESS + 0x0010)

/* REG_SYSREG_MIF_REQ */
#define    MIF_REQ                                 (1<<0)

/* REG_SYSREG_MIF_REQ_ACK */
#define    INTREQ_MIF_REQ_ACK                      (1<<8)
#define    MIF_REQ_ACK_STATUS                      (1<<4)
#define    MIF_REQ_INT_CLEAR                       (1<<0)

/* REG_SYSREG_CMGP_REQ */
#define    CMGP_REQ                                 (1<<0)

/* REG_SYSREG_CMGP_REQ_ACK */
#define    INTREQ_CMGP_REQ_ACK                      (1<<8)
#define    CMGP_REQ_ACK_STATUS                      (1<<4)
#define    CMGP_REQ_INT_CLEAR                       (1<<0)

/* REG_SYSREG_PD_REQ */
#define    PD_REQ                                  (1<<0)

/* REG_SYSREG_EARLY_WAKEUP_WINDOW_REG */
#define    EARLY_WAKEUP_WINDOW_REQ                 (1<<0)

/* REG_SYSREG_APM_UP_STATUS */
#define    APM_UP_STATUS                           (1<<0)


#define SYSREG_CHUB_DRCG_ENABLE	    0xFF
#define SYSREG_CHUB_DRCG_DISABLE    0x0
#define SYSREQ_CHUB_HWACG_CM4_CLKREQ_MASK_IRQ      0x3ffffff

#define CHUB_VVALID_INTR_FLTEN                     (1<<5)
#define CHUB_VVALID_INTR_LLMODE                    (1<<4)
#define CHUB_VVALID_INTR_HLMODE                    (1<<3)
#define CHUB_VVALID_INTR_REMODE                    (1<<2)
#define CHUB_VVALID_INTR_FEMODE                    (1<<1)
#define CHUB_VVALID_INTR_IMODE                     (1<<0)

#define AP_TURN                                    0
#define CHUB_TURN                                  1

#if !defined(CM55_BASE_ADDRESS)
#error "CM55_BASE_ADDRESS is not defined"
#endif

struct sfrData {
    uint32_t addr;
    uint32_t val;
};

static struct sfrData mSysregSfrData[] = {
    {REG_SYSREG_BUS_COMPONENET_DRCG_EN, 0},
    {REG_SYSREG_YAMIN_SECURE,           0},
};

void sysregPDRequest(void)
{
    __raw_writel(PD_REQ, (volatile uint32_t *)REG_SYSREG_PD_REQ);
}

void sysregPDRelease(void)
{
    __raw_writel(!PD_REQ, (volatile uint32_t *)REG_SYSREG_PD_REQ);
}

void sysregSetMaskIrq(uint64_t irq)
{
    uint32_t regValue;

    regValue = __raw_readl(REG_SYSREG_HWACG_CM4_INT1);
    regValue |= (uint32_t)(irq & 0xFFFFFFFF);
    __raw_writel(regValue, REG_SYSREG_HWACG_CM4_INT1);

    regValue = __raw_readl(REG_SYSREG_HWACG_CM4_INT2);
    regValue |= (uint32_t)(irq >> 32);
    __raw_writel(regValue, REG_SYSREG_HWACG_CM4_INT2);
}

void sysregClearMaskIrq(uint64_t irq)
{
    uint32_t regValue;

    regValue = __raw_readl(REG_SYSREG_HWACG_CM4_INT1);
    regValue &= ~((uint32_t)(irq & 0xFFFFFFFF));
    __raw_writel(regValue, REG_SYSREG_HWACG_CM4_INT1);

    regValue = __raw_readl(REG_SYSREG_HWACG_CM4_INT2);
    regValue &= ~((uint32_t)(irq >> 32));
    __raw_writel(regValue, REG_SYSREG_HWACG_CM4_INT2);
}

uint64_t sysregGetMaskIrq(void)
{
    uint64_t regValue;

    regValue = __raw_readl(REG_SYSREG_HWACG_CM4_INT2);
    regValue = regValue << 32;
    regValue |= __raw_readl(REG_SYSREG_HWACG_CM4_INT1);

    return regValue;
}

void sysregSetDRCG(uint32_t enable)
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

    regValue = __raw_readl((volatile uint32_t *)REG_SYSREG_EARLY_WAKEUP_WINDOW_REG);
    regValue &= ~EARLY_WAKEUP_WINDOW_REQ;
    if(en)
        regValue |= EARLY_WAKEUP_WINDOW_REQ;

    __raw_writel((uint32_t)regValue, (volatile uint32_t *)REG_SYSREG_EARLY_WAKEUP_WINDOW_REG);
}

uint32_t sysregGetEarlyWakeupWindowReq(void)
{
    uint32_t regValue;

    regValue = __raw_readl((volatile uint32_t *)REG_SYSREG_EARLY_WAKEUP_WINDOW_REG) & EARLY_WAKEUP_WINDOW_REQ;

    return regValue;
}

void sysregSetVvalidIntr(uint32_t intMode, uint32_t intMask)
{
    uint32_t regValue;

    regValue = CHUB_VVALID_INTR_FLTEN | ((intMode & 0xF) << 1) | (intMask & 0x1);
    __raw_writel(regValue, (volatile uint32_t *)REG_SYSREG_CHUB_VVALID_INTR);
}

void sysregSetMIFReq(bool enable)
{
    uint32_t regValue;

    regValue = enable ? 1:0;

    __raw_writel((uint32_t)regValue, (volatile uint32_t *)REG_SYSREG_MIF_REQ);
}

uint32_t sysregGetMIFReqAck(void)
{
    uint32_t regValue;

    regValue = __raw_readl((volatile uint32_t *)REG_SYSREG_MIF_REQ_ACK);

    return regValue;
}

void sysregClrMIFReqAck(void)
{
    __raw_writel(0x1, (volatile uint32_t *)REG_SYSREG_MIF_REQ_ACK);
}

#define Wait_Count 6 //6ms for MIF On
bool sysregMIFRequest(void) {
    uint32_t cnt=0;

    sysregSetMIFReq(true);
    while(!(sysregGetMIFReqAck() & MIF_REQ_ACK_STATUS) && (cnt < Wait_Count)) {
        mSleep(1);
        cnt++;
    }

    if(cnt >= Wait_Count)
        return false;

    return true;
}

bool sysregMIFRelease(void) {
    uint32_t cnt=0;

    sysregSetMIFReq(false);
    while((sysregGetMIFReqAck() & MIF_REQ_ACK_STATUS) && (cnt < Wait_Count)) {
        mSleep(1);
        cnt++;
    }

    if(cnt >= Wait_Count)
        return false;

    return true;
}

int32_t sysregAdcGetTurn(uint32_t timeout)
{
    uint32_t cnt=0;

    //  Set CHUB Flag
    __raw_writel(0x1, REG_SYSREG_ADC_CHUB_FLAG);
    //  Give turn to AP
    __raw_writel(AP_TURN, REG_SYSREG_ADC_INIT_TURN);

    //  Wait while release AP Flag
    while (__raw_readl(REG_SYSREG_ADC_AP_FLAG) &&
           __raw_readl(REG_SYSREG_ADC_INIT_TURN) == AP_TURN) {
        cnt++;
        if (timeout >= timeout) {
            __raw_writel(0x0, REG_SYSREG_ADC_CHUB_FLAG);
            CSP_PRINTF_ERROR("Time Out! ADC is being used by AP.\n");
            return -1;
        }
    }
    return 0;

}

void sysregAdcReleaseTurn(void)
{
    __raw_writel(0x0, REG_SYSREG_ADC_CHUB_FLAG);
}

uint32_t sysregGetCmgpReqAck(void) {
    uint32_t regValue;

    regValue = __raw_readl((volatile uint32_t *)REG_SYSREG_CMGP_REQ_ACK_IN);

    return regValue;
}

void sysregCmgpRequest(void) {
    __raw_writel(CMGP_REQ, (volatile uint32_t *)REG_SYSREG_CMGP_REQ_OUT);
}

void sysregCmgpRelease(void) {
    __raw_writel(~CMGP_REQ, (volatile uint32_t *)REG_SYSREG_CMGP_REQ_OUT);
}

void sysregSetCM55Base(uint32_t baseAddr)
{
    __raw_writel(baseAddr >> 7, (volatile uint32_t*)REG_SYSREG_YAMIN_SECURE);
}

void sysregSendIRQtoCM55(void)
{
    __raw_writel(0x1, (volatile uint32_t*)REG_SYSREG_INTREQ_CM4_TO);
}

void sysregClearIRQfromCM55(void)
{
    __raw_writel(0x0, (volatile uint32_t*)REG_SYSREG_INTREQ_YAMIN_TO);
}

void sysregSetOscEn(void)
{
    // 5515 does not support
}

void sysregInit(void)
{
    sysregCmgpRequest();
    sysregSetCM55Base(CM55_BASE_ADDRESS);
    ipc_lpd_reset_release();
}

void sysregSaveState(void)
{
    uint32_t idx = 0;
    uint32_t size;

    size = sizeof(mSysregSfrData) / sizeof(struct sfrData);

    for (idx = 0 ; idx < size ; idx++) {
        mSysregSfrData[idx].val = __raw_readl(mSysregSfrData[idx].addr);
    }
}

void sysregRestoreState(void)
{
    uint32_t idx = 0;
    uint32_t size;

    size = sizeof(mSysregSfrData) / sizeof(struct sfrData);

    for (idx = 0 ; idx < size ; idx++) {
        __raw_writel(mSysregSfrData[idx].val, mSysregSfrData[idx].addr);
    }
}

