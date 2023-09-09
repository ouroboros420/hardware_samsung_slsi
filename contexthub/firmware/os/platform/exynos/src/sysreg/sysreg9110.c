/*----------------------------------------------------------------------------
 *      Exynos SoC  -  SYSREG
 *----------------------------------------------------------------------------
 *      Name:    sysreg.c
 *      Purpose: To implement SYSREG APIs
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

#include <pwrDrv.h>
#include <pwrDrvPwrGating.h>

#define SYSREG_CHUB_DRCG_ENABLE	    0x7F
#define SYSREG_CHUB_DRCG_DISABLE    0x0
#define SYSREQ_CHUB_HWACG_CM4_CLKREQ_MASK_IRQ   0x3ffffff

#define CHUB_VVALID_INTR_PULSE_MASK                 (3<<1)
#define CHUB_VVALID_INTR_FALLING_EDGE_ENABLE        (1<<0)
#define CHUB_VVALID_INTR_RISING_EDGE_ENABLE         (1<<1)
#define CHUB_VVALID_INTR_IMODE_MASK                 (1)
#define CHUB_VVALID_INTR_MASK_ENABLE                (1<<0)
#define CHUB_VVALID_INTR_MASK_DISABLE               (0<<0)

#define E9110_SYSREGSFR_NUM 2
static uint32_t buf_SYSREGSFR[E9110_SYSREGSFR_NUM];

void sysregSetPdReq(IN bool en)
{
    uint32_t regValue;
    regValue = __raw_readl((volatile uint32_t *)REG_SYSREG_PD_REQ);
    regValue &= PD_REQ;
    if(en)
        regValue |= PD_REQ;

    __raw_writel((uint32_t)regValue, (volatile uint32_t *)REG_SYSREG_PD_REQ);
}

uint32_t sysregGetPdReq(void)
{
    uint32_t regValue;

    regValue = __raw_readl((volatile uint32_t *)REG_SYSREG_PD_REQ);

    return regValue;
}

void sysregSetMaskIrq(uint64_t irq)
{
    uint32_t regValue;

    regValue = (uint32_t)(irq & 0x1FFFFFFF);
    __raw_writel(regValue, REG_SYSREG_HWACG_CM4_CLKREQ);
}

uint64_t sysregGetMaskIrq(void)
{
    uint64_t regValue;

    regValue = __raw_readl(REG_SYSREG_HWACG_CM4_CLKREQ) & 0x1FFFFFFF;

    return regValue;
}

void sysregSetDRCG(IN uint32_t enable)
{
    if(enable) {
        __raw_writel(SYSREG_CHUB_DRCG_ENABLE, REG_SYSREG_BUS_COMPONENET_DRCG_EN);
    }
    else {
        __raw_writel(SYSREG_CHUB_DRCG_DISABLE, REG_SYSREG_BUS_COMPONENET_DRCG_EN);
    }
}

void sysregSetEarlyWakeupWindowReq(IN bool en)
{
    uint32_t regValue;

    regValue = __raw_readl((volatile uint32_t *)REG_SYSREG_EARLY_WAKEUP_WINDOW_REG);
    regValue &= EARLY_WAKEUP_WINDOW_REQ;
    if(en)
        regValue |= EARLY_WAKEUP_WINDOW_REQ;

    __raw_writel((uint32_t)regValue, (volatile uint32_t *)REG_SYSREG_EARLY_WAKEUP_WINDOW_REG);
}

uint32_t sysregGetEarlyWakeupWindowReq(void)
{
    uint32_t regValue;

    regValue = __raw_readl((volatile uint32_t *)REG_SYSREG_EARLY_WAKEUP_WINDOW_REG);

    return regValue;
}

void sysregSetVvalidIntr(IN bool negEdge, IN bool intMask)
{
    uint32_t reg;

    reg = __raw_readl((volatile uint32_t *)REG_SYSREG_CHUB_VVALID_INTR);
    /* Mask VVALID Interrupt */
    reg &= ~(CHUB_VVALID_INTR_IMODE_MASK);
    reg |= CHUB_VVALID_INTR_MASK_ENABLE;
    __raw_writel((uint32_t)reg, (volatile uint32_t *)REG_SYSREG_CHUB_VVALID_INTR);

    reg = __raw_readl((volatile uint32_t *)REG_SYSREG_CHUB_VVALID_INTR);
    reg &= ~(CHUB_VVALID_INTR_PULSE_MASK | CHUB_VVALID_INTR_IMODE_MASK);

    if(negEdge)
        reg |= ~(CHUB_VVALID_INTR_FALLING_EDGE_ENABLE);
    else
        reg |= ~(CHUB_VVALID_INTR_RISING_EDGE_ENABLE);

    if(intMask)
        reg |= ~(CHUB_VVALID_INTR_MASK_ENABLE);
    else
        reg |= ~(CHUB_VVALID_INTR_MASK_DISABLE);

    __raw_writel((uint32_t)reg, (volatile uint32_t *)REG_SYSREG_CHUB_VVALID_INTR);
}

void sysregSetMIFReq(IN bool enable)
{
    uint32_t regValue;

    regValue = __raw_readl((volatile uint32_t *)REG_SYSREG_MIF_REQ);
    if(enable)
        regValue |= MIF_REQ;
    else
        regValue &= ~(MIF_REQ);

    __raw_writel((uint32_t)regValue, (volatile uint32_t *)REG_SYSREG_MIF_REQ);
}

uint32_t sysregGetMIFAck(void)
{
    uint32_t regValue;

    regValue = __raw_readl((volatile uint32_t *)REG_SYSREG_MIF_REQ_ACK);

    return regValue;
}

void sysregClrMIFReqAck(void)
{
    uint32_t regValue;

    regValue = __raw_readl((volatile uint32_t *)REG_SYSREG_APM_REQ_ACK);
    regValue |= APM_REQ_INT_CLEAR;
    __raw_writel(regValue, (volatile uint32_t *)REG_SYSREG_APM_REQ_ACK);
}

#define Wait_Count 5 //5ms for MIF On
bool sysregEnableMIF(void)
{
    uint32_t cnt=0;

    sysregSetMIFReq(true);
    while(!(sysregGetMIFAck()&0x4) && (cnt < Wait_Count)){
        mSleep(1);
        cnt++;
    }

    sysregSetMIFReq(false);

    if(cnt > Wait_Count)
        return false;

    sysregClrMIFReqAck();

    return true;
}

void sysregSetAPMReq(IN bool enable)
{
    uint32_t regValue;

    regValue = __raw_readl((volatile uint32_t *)REG_SYSREG_APM_REQ);
    if(enable)
        regValue |= 0x1;
    else
        regValue &= ~(0x1);

    __raw_writel((uint32_t)regValue, (volatile uint32_t *)REG_SYSREG_APM_REQ);
}

uint32_t sysregGetAPMAck(void)
{
    uint32_t regValue;

    regValue = __raw_readl((volatile uint32_t *)REG_SYSREG_APM_REQ_ACK);

    return regValue;
}

void sysregClrAPMAck(void)
{
    uint32_t regValue;

    regValue = __raw_readl((volatile uint32_t *)REG_SYSREG_APM_REQ_ACK);
    regValue |= APM_REQ_INT_CLEAR;
    __raw_writel((uint32_t)regValue, (volatile uint32_t *)REG_SYSREG_APM_REQ_ACK);
}

#define APM_Wait_Count 10 //10ms for APM wakeup
bool sysregEnableAPM(void)
{
    uint32_t cnt=0;

    sysregSetAPMReq(true);
    while(!(sysregGetAPMAck()&0x10) && (cnt < APM_Wait_Count)){
        mSleep(1);
        cnt++;
    }

    sysregClrAPMAck();

    if(cnt > APM_Wait_Count)
        return false;

    return true;
}

uint32_t sysregGetAPMUpStatus(void)
{
    uint32_t regValue;

    regValue = __raw_readl((volatile uint32_t *)REG_SYSREG_APM_UP_STATUS);

    return regValue;
}

void sysregInit(void)
{
}

void sysregSaveState(void)
{
    uint32_t idx = 0;

    buf_SYSREGSFR[idx++] = __raw_read32(REG_SYSREG_BUS_COMPONENET_DRCG_EN);
    buf_SYSREGSFR[idx++] = __raw_read32(REG_SYSREG_HWACG_CM4_CLKREQ);
}

void sysregRestoreState(void)
{
    uint32_t idx = 0;

    __raw_write32(buf_SYSREGSFR[idx++], REG_SYSREG_BUS_COMPONENET_DRCG_EN);
    __raw_write32(buf_SYSREGSFR[idx++], REG_SYSREG_HWACG_CM4_CLKREQ);
}

void sysregApmReqAck_IRQHandler(void)
{
    /* Need to check if APM_REQ_ACK */
    sysregClrAPMAck();
    //pwrDrvSetPowerMode(POWER_MODE_NORMAL);
#if defined(s5e9110)
    pwrDrvCmgpRestoreState();
#endif
}

