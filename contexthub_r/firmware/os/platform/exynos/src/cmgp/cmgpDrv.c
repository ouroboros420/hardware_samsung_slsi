/*----------------------------------------------------------------------------
 *      Exynos SoC  -  CMGP
 *----------------------------------------------------------------------------
 *      Name:    cmgpDrv.c
 *      Purpose: To implement CMGP driver functions
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

#include <csp_common.h>
#include <cmgp.h>
#include <cmgpDrv.h>
#include <sysreg.h>
#if defined(SEOS)
	#include <cmsis.h>
	#include <errno.h>
#endif

void cmgpDrvInit(void)
{
    // Disable all CMGP interrupts before enable NVIC
    __raw_writel( 0xFFFFFFFF , REG_SYSREG_INTC0_IEN_CLR );
    __raw_writel( 0xFFFFFFFF , REG_SYSREG_INTC1_IEN_CLR );
    // C-M core can process non-secure interrupt only
#if defined(s5e3830)
    NVIC_ClearPendingIRQ(SYSREG0_CMGP_IRQn);
#endif
    NVIC_ClearPendingIRQ(SYSREG1_CMGP_IRQn);

    // Enable NVIC
    // C-M core can process non-secure interrupt only
#if defined(s5e3830)
    NVIC_EnableIRQ(SYSREG0_CMGP_IRQn); // secure
#endif
    NVIC_EnableIRQ(SYSREG1_CMGP_IRQn); // non-secure
}

void cmgpDrvEnableInterrupt(uint32_t intNum)
{
    uint32_t regValue;

    if( intNum < 32 ){
        regValue = __raw_readl(REG_SYSREG_INTC0_IEN_SET);
        regValue |= (0x1 << intNum );
        __raw_writel( regValue , REG_SYSREG_INTC0_IEN_SET );
    }
    else{
        regValue = __raw_readl(REG_SYSREG_INTC1_IEN_SET);
        regValue |= (0x1 << (intNum -32));
        __raw_writel( regValue, REG_SYSREG_INTC1_IEN_SET );
    }

    return;
}

void cmgpDrvDisableInterrupt(uint32_t intNum)
{
    uint32_t regValue;

    if( intNum < 32 ){
        regValue = (0x1 << intNum );
        __raw_writel( regValue , REG_SYSREG_INTC0_IEN_CLR );
    }
    else{
        regValue = (0x1 << (intNum -32));
        __raw_writel( regValue, REG_SYSREG_INTC1_IEN_CLR );
    }

    return;
}

void cmgpDrvSetIrqPriority(uint32_t intNum, uint32_t priority)
{
    uint32_t reg_addr;
    uint32_t shift;

    if( intNum < 32 ){
        reg_addr = REG_SYSREG_INTC0_IPRIORITY0;
        reg_addr = reg_addr + ((intNum/8)*4);
    }
    else{
        intNum = intNum - 32;
        reg_addr = REG_SYSREG_INTC1_IPRIORITY0;
        reg_addr = reg_addr + ((intNum/8)*4);
    }

    shift = (intNum%8) * 4;
    priority = priority & 0xF;

    __raw_writel( priority << shift, reg_addr);

    return;
}

uint32_t cmgpDrvGetIrqNum(void)
{
    uint32_t regValue;
    uint32_t flag = 0;
    uint32_t intNum = 0;
    uint32_t i;

    regValue = __raw_readl(REG_SYSREG_INTC0_IPRIO_NONSECURE_PEND);

    for( i = 0 ; i < 32 ; i++ ) {
        if( regValue & (0x1<<i)){
            intNum = i;
            flag = 1;
        }
    }

    if( flag == 0 ) {
        regValue = __raw_readl(REG_SYSREG_INTC1_IPRIO_NONSECURE_PEND);

        for( i = 0 ; i < 32 ; i++ ) {
            if( regValue & (0x1<<i)){
                intNum = i+32;
            }
        }
    }

    return intNum;
}
