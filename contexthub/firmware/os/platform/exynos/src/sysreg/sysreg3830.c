/*----------------------------------------------------------------------------
 * Exynos SoC  -  SYSREG
 *
 * Copyright (C) 2021 Samsung Electronics Co., Ltd.
 *
 * Huiling wu <huiling.wu@samsung.com>
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
//p-2040
#define    REG_SYSREG_BUS_COMPONENET_DRCG_EN       (SYSREG_BASE_ADDRESS + 0x104)
#define    REG_SYSREG_MEMCLK                       (SYSREG_BASE_ADDRESS + 0x108)
#define    REG_SYSREG_MEMCTRL_D                    (SYSREG_BASE_ADDRESS + 0x10C)
#define    REG_SYSREG_MEMCTRL_I                    (SYSREG_BASE_ADDRESS + 0x110)
#define    REG_SYSREG_MIF_REQ_OUT                  (SYSREG_BASE_ADDRESS + 0x200)
#define    REG_SYSREG_MIF_REQ_ACK_IN               (SYSREG_BASE_ADDRESS + 0x204)
#define    REG_SYSREG_HWACG_CM4_STATUS             (SYSREG_BASE_ADDRESS + 0x420)
#define    REG_SYSREG_HWACG_CM4_CONFIG             (SYSREG_BASE_ADDRESS + 0x424)
#define    REG_SYSREG_HWACG_CM4_CLKREQ             (SYSREG_BASE_ADDRESS + 0x428)

#define    REG_SYSREG_USI_CMGP00_SW_CONF           (SYSREG_CMGP_BASE_ADDRESS + 0x2000)
#define    REG_SYSREG_USI_CMGP01_SW_CONF           (SYSREG_CMGP_BASE_ADDRESS + 0x2010)
#define    REG_SYSREG_USI_CMGP00_IPCLK             (SYSREG_CMGP_BASE_ADDRESS + 0x3000)
#define    REG_SYSREG_USI_CMGP01_IPCLK             (SYSREG_CMGP_BASE_ADDRESS + 0x3010)

typedef enum {
    SWCONF_USI_CHUB00_SW_CONF,
    SWCONF_USI_CHUB01_SW_CONF,
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

void sysregSetWakeupInterruptAll(void) {
    //__raw_writel(0xFFFFFFFF, REG_SYSREG_INT_EN_SET);
}

void sysregClrWakeupInterruptAll(void) {
    //__raw_writel(0xFFFFFFFF, REG_SYSREG_INT_EN_CLR);
}

void sysregPDRequest(void) {
    //__raw_writel(PD_REQ, (volatile uint32_t *)REG_SYSREG_PD_REQ);
}

void sysregPDRelease(void) {
    //__raw_writel(!PD_REQ, (volatile uint32_t *)REG_SYSREG_PD_REQ);
}

void sysregSetMaskIrq(IN uint64_t irq) {
    uint32_t regValue;

    regValue = (uint32_t)irq;
    __raw_writel(regValue, REG_SYSREG_HWACG_CM4_CLKREQ);
}

uint64_t sysregGetMaskIrq(void) {
    uint64_t regValue;

    regValue = __raw_readl(REG_SYSREG_HWACG_CM4_CLKREQ);

    return regValue;
}

void sysregClearMaskIrq(uint64_t irq) {
    uint32_t regValue;

    regValue = __raw_readl(REG_SYSREG_HWACG_CM4_CLKREQ);
    regValue &= ~((uint32_t)(irq & 0xFFFFFFFF));
    __raw_writel(regValue, REG_SYSREG_HWACG_CM4_CLKREQ);
}

//
void sysregSetDRCG(IN uint32_t enable) {
    if(enable) {
        __raw_writel(SYSREG_CHUB_DRCG_ENABLE, REG_SYSREG_BUS_COMPONENET_DRCG_EN);
    }
    else {
        __raw_writel(SYSREG_CHUB_DRCG_DISABLE, REG_SYSREG_BUS_COMPONENET_DRCG_EN);
    }
}

void sysregSetEarlyWakeupWindowReq(bool en) {
}

uint32_t sysregGetEarlyWakeupWindowReq(void) {
    return 0;
}

void sysregSetVvalidIntr(uint32_t intMode, uint32_t intMask) {
}

void sysregSetOscEn(void) {
}

void sysregSetOscDis(void) {
}

void sysregSetCmgpReq(void) {
}

void sysregClearCmgpReq(void) {
}

void sysregInit(void) {
}

void sysregSaveState(void) {
}

void sysregRestoreState(void) {
}
