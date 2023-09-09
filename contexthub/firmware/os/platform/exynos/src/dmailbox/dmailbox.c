/*----------------------------------------------------------------------------
 *      Exynos SoC  -  Dmailbox
 *----------------------------------------------------------------------------
 *      Name:    dmailbox.c
 *      Purpose: To implement Dmailbox APIs
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

#include "dmailbox.h"
#include "csp_common.h"

/*V1.1.0*/
#define REG_DMAILBOX_SFR_APB                (DMAILBOX_BASE_ADDRESS + 0x30)
#define REG_DMAILBOX_INT_EN_SET             (DMAILBOX_BASE_ADDRESS + 0x40)
#define REG_DMAILBOX_INT_EN_CLR             (DMAILBOX_BASE_ADDRESS + 0x44)
#define REG_DMAILBOX_INT_PEND               (DMAILBOX_BASE_ADDRESS + 0x48)
#define REG_DMAILBOX_INT_CLR                (DMAILBOX_BASE_ADDRESS + 0x4c)
#define REG_DMAILBOX_CTRL                   (DMAILBOX_BASE_ADDRESS + 0x100)
#define REG_DMAILBOX_CONFIG                 (DMAILBOX_BASE_ADDRESS + 0x104)
#define REG_DMAILBOX_CONF_DONE              (DMAILBOX_BASE_ADDRESS + 0x108)
#define REG_DMAILBOX_STATUS                 (DMAILBOX_BASE_ADDRESS + 0x10c)
#define REG_DMAILBOX_READ_DONE_SIZE         (DMAILBOX_BASE_ADDRESS + 0x110)
#define REG_DMAILBOX_WRITE_DONE_SIZE        (DMAILBOX_BASE_ADDRESS + 0x114)
#define REG_DMAILBOX_RX_WD_TH               (DMAILBOX_BASE_ADDRESS + 0x118)
#define REG_DMAILBOX_TX_WD_TH               (DMAILBOX_BASE_ADDRESS + 0x11c)
#define REG_DMAILBOX_RX_WD_CNT              (DMAILBOX_BASE_ADDRESS + 0x120)
#define REG_DMAILBOX_TX_WD_CNT              (DMAILBOX_BASE_ADDRESS + 0x124)
#define REG_DMAILBOX_AXI_RMASTER0_STARTADDR (DMAILBOX_BASE_ADDRESS + 0x200)
#define REG_DMAILBOX_AXI_RMASTER0_ENDADDR   (DMAILBOX_BASE_ADDRESS + 0x204)
#define REG_DMAILBOX_AXI_RMASTER1_STARTADDR (DMAILBOX_BASE_ADDRESS + 0x208)
#define REG_DMAILBOX_AXI_RMASTER1_ENDADDR   (DMAILBOX_BASE_ADDRESS + 0x20c)
#define REG_DMAILBOX_AXI_RMASTER2_STARTADDR (DMAILBOX_BASE_ADDRESS + 0x210)
#define REG_DMAILBOX_AXI_RMASTER2_ENDADDR   (DMAILBOX_BASE_ADDRESS + 0x214)
#define REG_DMAILBOX_AXI_RMASTER3_STARTADDR (DMAILBOX_BASE_ADDRESS + 0x218)
#define REG_DMAILBOX_AXI_RMASTER3_ENDADDR   (DMAILBOX_BASE_ADDRESS + 0x21c)
#define REG_DMAILBOX_AXI_WMASTER0_STARTADDR (DMAILBOX_BASE_ADDRESS + 0x220)
#define REG_DMAILBOX_AXI_WMASTER1_STARTADDR (DMAILBOX_BASE_ADDRESS + 0x224)
#define REG_DMAILBOX_AXI_WMASTER2_STARTADDR (DMAILBOX_BASE_ADDRESS + 0x228)
#define REG_DMAILBOX_AXI_WMASTER3_STARTADDR (DMAILBOX_BASE_ADDRESS + 0x22c)
#define REG_DMAILBOX_DMAILBOX_IF0           (DMAILBOX_BASE_ADDRESS + 0x300)
#define REG_DMAILBOX_DMAILBOX_IF1           (DMAILBOX_BASE_ADDRESS + 0x304)
#define REG_DMAILBOX_DMAILBOX_IF2           (DMAILBOX_BASE_ADDRESS + 0x308)
#define REG_DMAILBOX_DMAILBOX_IF3           (DMAILBOX_BASE_ADDRESS + 0x30c)

#define REG_DMAILBOX_APBSEMA_REL            (DMAILBOX_BASE_ADDRESS + 0x1000)

// REG_DMAILBOX_CTRL
#define CTRL_AXI_QCH_DISABLE        (1 << 16)
#define CTRL_APB_QCH_DISABLE        (1 << 12)
#define CTRL_LOOPBACK_TEST_EN       (1 << 8)
#define CTRL_TX_EN                  (1 << 4)
#define CTRL_RX_EN                  (1 << 0)

#define DMAILBOX_INFO(fmt, ...) CSP_PRINTF_INFO("[DMAIL]" fmt, ##__VA_ARGS__)
#define DMAILBOX_ERROR(fmt, ...) CSP_PRINTF_ERROR("[DMAIL]" fmt, ##__VA_ARGS__)

static inline int dmailboxRequestSema(void) {
    uint32_t regValue;
    int ret = 0;

    regValue = __raw_readl(REG_DMAILBOX_CTRL);
    if (regValue == 0x0BAD0BAD) {
        DMAILBOX_ERROR("fail to request semaphore");
        ret = -1;
    }

    return ret;
}

static inline void dmailboxReleaseSema(void) {
    uint32_t regValue;
    regValue = __raw_readl(REG_DMAILBOX_APBSEMA_REL);
}

static inline void dmailboxEnableTx(void) {
    uint32_t regValue;

    regValue = __raw_readl(REG_DMAILBOX_CTRL);
    regValue |= CTRL_TX_EN;
    __raw_writel(regValue, REG_DMAILBOX_CTRL);
}

static inline void dmailboxEnableRx(void) {
    uint32_t regValue;

    regValue = __raw_readl(REG_DMAILBOX_CTRL);
    regValue |= CTRL_RX_EN;
    __raw_writel(regValue, REG_DMAILBOX_CTRL);
}

static inline void dmailboxDisableTx(void) {
    uint32_t regValue;

    regValue = __raw_readl(REG_DMAILBOX_CTRL);
    regValue &= ~CTRL_TX_EN;
    __raw_writel(regValue, REG_DMAILBOX_CTRL);
}

static inline void dmailboxDisableRx(void) {
    uint32_t regValue;

    regValue = __raw_readl(REG_DMAILBOX_CTRL);
    regValue &= ~CTRL_RX_EN;
    __raw_writel(regValue, REG_DMAILBOX_CTRL);
}

void dmailboxInit(void) {
#if DMAILBOX_USE_SEMA
    if (dmailboxRequestSema())
        return;
#endif

    dmailboxDisableTx();
    dmailboxDisableRx();

#if DMAILBOX_USE_SEMA
    dmailboxReleaseSema();
#endif
}
