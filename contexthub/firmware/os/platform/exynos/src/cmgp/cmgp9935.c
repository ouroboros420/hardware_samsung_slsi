/*----------------------------------------------------------------------------
 *      Exynos SoC  -  CMGP
 *----------------------------------------------------------------------------
 *      Name:    cmgp.c
 *      Purpose: To implement CMGP APIs for 9935 
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

#ifndef __CMGP9935_C__
#define __CMGP9935_C__

#include <errno.h>
#include <csp_common.h>
#include <cmgp.h>
#include <usi.h>
#include <i2c.h>
#include <cmsis.h>
#if defined(EMBOS)
  #include <Device.h>
#endif
#include CSP_HEADER(gpio)
#include CSP_HEADER(cmgp)

#define REG_USI_CMGP0_SW_CONF                   (SYSREG_CMGP_BASE_ADDRESS + 0x2000)
#define REG_I2C_CMGP0_SW_CONF                   (SYSREG_CMGP_BASE_ADDRESS + 0x2070)
#define REG_USI_CMGP1_SW_CONF                   (SYSREG_CMGP_BASE_ADDRESS + 0x2010)
#define REG_I2C_CMGP1_SW_CONF                   (SYSREG_CMGP_BASE_ADDRESS + 0x2074)
#define REG_USI_CMGP2_SW_CONF                   (SYSREG_CMGP_BASE_ADDRESS + 0x2020)
#define REG_I2C_CMGP2_SW_CONF                   (SYSREG_CMGP_BASE_ADDRESS + 0x2024)
#define REG_USI_CMGP3_SW_CONF                   (SYSREG_CMGP_BASE_ADDRESS + 0x2030)
#define REG_I2C_CMGP3_SW_CONF                   (SYSREG_CMGP_BASE_ADDRESS + 0x2034)
#define REG_USI_CMGP4_SW_CONF                   (SYSREG_CMGP_BASE_ADDRESS + 0x2040)
#define REG_I2C_CMGP4_SW_CONF                   (SYSREG_CMGP_BASE_ADDRESS + 0x2044)
#define REG_USI_CMGP5_SW_CONF                   (SYSREG_CMGP_BASE_ADDRESS + 0x2050)
#define REG_I2C_CMGP5_SW_CONF                   (SYSREG_CMGP_BASE_ADDRESS + 0x2054)
#define REG_USI_CMGP6_SW_CONF                   (SYSREG_CMGP_BASE_ADDRESS + 0x2060)
#define REG_I2C_CMGP6_SW_CONF                   (SYSREG_CMGP_BASE_ADDRESS + 0x2064)

#define SYSREG_CMGP_INTC_BASE                   (SYSREG_CMGP2CHUB_BASE_ADDRESS)//(CMGP_BASE + 0x70000)
#define REG_SYSREG_INTC_CONTROL                 (SYSREG_CMGP_INTC_BASE + 0x280)
#define REG_SYSREG_INTC0_IGROUP                 (SYSREG_CMGP_INTC_BASE + 0x284)
#define REG_SYSREG_INTC0_IEN_SET                (SYSREG_CMGP_INTC_BASE + 0x288)
#define REG_SYSREG_INTC0_IEN_CLR                (SYSREG_CMGP_INTC_BASE + 0x28C)
#define REG_SYSREG_INTC0_IPEND                  (SYSREG_CMGP_INTC_BASE + 0x290)
#define REG_SYSREG_INTC0_IPRIO_PEND             (SYSREG_CMGP_INTC_BASE + 0x294)
#define REG_SYSREG_INTC0_IPRIORITY0             (SYSREG_CMGP_INTC_BASE + 0x298)
#define REG_SYSREG_INTC0_IPRIORITY1             (SYSREG_CMGP_INTC_BASE + 0x29C)
#define REG_SYSREG_INTC0_IPRIORITY2             (SYSREG_CMGP_INTC_BASE + 0x2A0)
#define REG_SYSREG_INTC0_IPRIORITY3             (SYSREG_CMGP_INTC_BASE + 0x2A4)
#define REG_SYSREG_INTC0_IPRIO_SECURE_PEND      (SYSREG_CMGP_INTC_BASE + 0x2A8)
#define REG_SYSREG_INTC0_IPRIO_NONSECURE_PEND   (SYSREG_CMGP_INTC_BASE + 0x2AC)

#define REG_SYSREG_INTC1_IGROUP                 (SYSREG_CMGP_INTC_BASE + 0x2B4)
#define REG_SYSREG_INTC1_IEN_SET                (SYSREG_CMGP_INTC_BASE + 0x2B8)
#define REG_SYSREG_INTC1_IEN_CLR                (SYSREG_CMGP_INTC_BASE + 0x2BC)
#define REG_SYSREG_INTC1_IPEND                  (SYSREG_CMGP_INTC_BASE + 0x2C0)
#define REG_SYSREG_INTC1_IPRIO_PEND             (SYSREG_CMGP_INTC_BASE + 0x2C4)
#define REG_SYSREG_INTC1_IPRIORITY0             (SYSREG_CMGP_INTC_BASE + 0x2C8)
#define REG_SYSREG_INTC1_IPRIORITY1             (SYSREG_CMGP_INTC_BASE + 0x2CC)
#define REG_SYSREG_INTC1_IPRIORITY2             (SYSREG_CMGP_INTC_BASE + 0x2D0)
#define REG_SYSREG_INTC1_IPRIORITY3             (SYSREG_CMGP_INTC_BASE + 0x2D4)
#define REG_SYSREG_INTC1_IPRIO_SECURE_PEND      (SYSREG_CMGP_INTC_BASE + 0x2D8)
#define REG_SYSREG_INTC1_IPRIO_NONSECURE_PEND   (SYSREG_CMGP_INTC_BASE + 0x2DC)


#define REG_SYSREG_INTC_BASE                    REG_SYSREG_INTC0_IGROUP
#define REG_SYSREG_INTC_OFFSET                  0x30
#define REG_SYSREG_INTC_IGROUP_OFFSET           0x4
#define REG_SYSREG_INTC_SET_OFFSET              0x8
#define REG_SYSREG_INTC_CLR_OFFSET              0xC
#define REG_SYSREG_INTC_PEND_OFFSET             0x10
#define REG_SYSREG_INTC_PRIO_PEND_OFFSET        0x14
#define REG_SYSREG_INTC_PRIORITY_OFFSET         0x18
#define REG_SYSREG_INTC_SEC_PEND_OFFSET         0x28
#define REG_SYSREG_INTC_NSEC_PEND_OFFSET        0x2C

#define REG_USI_CMGP_SW_CONF_BASE               REG_USI_CMGP0_SW_CONF
#define REG_I2C_CMGP_SW_CONF_BASE               REG_I2C_CMGP2_SW_CONF
#define REG_USI_CMGP_SW_CONF_OFFSET             0x10

#define CMGP_MAX_INT_NUM                        54
#define CMGP_DEFAULT_IRQ_GROUP                  0
#define SYSREG_INTC_PRIORITY_BITFIELD           4

#define GET_INTC_REG(group, reg) \
  ((REG_SYSREG_INTC_BASE + group*REG_SYSREG_INTC_OFFSET) + \
  REG_SYSREG_INTC_##reg##_OFFSET )

#define DEC_CMGP_IRQ(index, source, group) \
  [index] = {.irqSource = source,  .irqGroup = group }

struct CmgpIrqInfoType mCmgpIrqInfo[]= {
    DEC_CMGP_IRQ( CMGP_GPM00_0_IRQ, GPM00_0, CMGP_IRQ_GROUP_GPIO ),
    DEC_CMGP_IRQ( CMGP_GPM00_1_IRQ, GPM00_1, CMGP_IRQ_GROUP_GPIO ),
    DEC_CMGP_IRQ( CMGP_GPM01_0_IRQ, GPM01_0, CMGP_IRQ_GROUP_GPIO ),
    DEC_CMGP_IRQ( CMGP_GPM01_1_IRQ, GPM01_1, CMGP_IRQ_GROUP_GPIO ),
    DEC_CMGP_IRQ( CMGP_GPM02_0_IRQ, GPM02_0, CMGP_IRQ_GROUP_GPIO ),
    DEC_CMGP_IRQ( CMGP_GPM02_1_IRQ, GPM02_1, CMGP_IRQ_GROUP_GPIO ),
    DEC_CMGP_IRQ( CMGP_GPM03_0_IRQ, GPM03_0, CMGP_IRQ_GROUP_GPIO ),
    DEC_CMGP_IRQ( CMGP_GPM03_1_IRQ, GPM03_1, CMGP_IRQ_GROUP_GPIO ),
    DEC_CMGP_IRQ( CMGP_GPM04_0_IRQ, GPM04_0, CMGP_IRQ_GROUP_GPIO ),
    DEC_CMGP_IRQ( CMGP_GPM04_1_IRQ, GPM04_1, CMGP_IRQ_GROUP_GPIO ),
    DEC_CMGP_IRQ( CMGP_GPM05_0_IRQ, GPM05_0, CMGP_IRQ_GROUP_GPIO ),
    DEC_CMGP_IRQ( CMGP_GPM05_1_IRQ, GPM05_1, CMGP_IRQ_GROUP_GPIO ),
    DEC_CMGP_IRQ( CMGP_GPM06_0_IRQ, GPM06_0, CMGP_IRQ_GROUP_GPIO ),
    DEC_CMGP_IRQ( CMGP_GPM06_1_IRQ, GPM06_1, CMGP_IRQ_GROUP_GPIO ),
    DEC_CMGP_IRQ( CMGP_GPM07_0_IRQ, GPM07_0, CMGP_IRQ_GROUP_GPIO ),
    DEC_CMGP_IRQ( CMGP_GPM07_1_IRQ, GPM07_1, CMGP_IRQ_GROUP_GPIO ),
    DEC_CMGP_IRQ( CMGP_GPM08_0_IRQ, GPM08_0, CMGP_IRQ_GROUP_GPIO ),
    DEC_CMGP_IRQ( CMGP_GPM08_1_IRQ, GPM08_1, CMGP_IRQ_GROUP_GPIO ),
    DEC_CMGP_IRQ( CMGP_GPM09_0_IRQ, GPM09_0, CMGP_IRQ_GROUP_GPIO ),
    DEC_CMGP_IRQ( CMGP_GPM09_1_IRQ, GPM09_1, CMGP_IRQ_GROUP_GPIO ),
    DEC_CMGP_IRQ( CMGP_GPM10_0_IRQ, GPM10_0, CMGP_IRQ_GROUP_GPIO ),
    DEC_CMGP_IRQ( CMGP_GPM10_1_IRQ, GPM10_1, CMGP_IRQ_GROUP_GPIO ),
    DEC_CMGP_IRQ( CMGP_GPM11_0_IRQ, GPM11_0, CMGP_IRQ_GROUP_GPIO ),
    DEC_CMGP_IRQ( CMGP_GPM11_1_IRQ, GPM11_1, CMGP_IRQ_GROUP_GPIO ),
    DEC_CMGP_IRQ( CMGP_GPM12_0_IRQ, GPM12_0, CMGP_IRQ_GROUP_GPIO ),
    DEC_CMGP_IRQ( CMGP_GPM12_1_IRQ, GPM12_1, CMGP_IRQ_GROUP_GPIO ),
    DEC_CMGP_IRQ( CMGP_GPM13_0_IRQ, GPM13_0, CMGP_IRQ_GROUP_GPIO ),
    DEC_CMGP_IRQ( CMGP_GPM13_1_IRQ, GPM13_1, CMGP_IRQ_GROUP_GPIO ),
    DEC_CMGP_IRQ( CMGP_GPM15_0_IRQ, GPM15_0, CMGP_IRQ_GROUP_GPIO ),
    DEC_CMGP_IRQ( CMGP_GPM16_0_IRQ, GPM16_0, CMGP_IRQ_GROUP_GPIO ),
    DEC_CMGP_IRQ( CMGP_GPM17_0_IRQ, GPM17_0, CMGP_IRQ_GROUP_GPIO ),
};

void cmgpInit(void)
{
    // Disable all CMGP interrupts before enable NVIC
    __raw_writel( 0xFFFFFFFF , GET_INTC_REG(0, CLR) );
    __raw_writel( 0xFFFFFFFF , GET_INTC_REG(1, CLR) );
    NVIC_ClearPendingIRQ(SYSREG1_CMGP_IRQn);
    NVIC_EnableIRQ(SYSREG1_CMGP_IRQn);

    return;
}

#endif
