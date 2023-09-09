/*----------------------------------------------------------------------------
 *      Exynos SoC  -  CMGP
 *----------------------------------------------------------------------------
 *      Name:    cmgp.c
 *      Purpose: To implement CMGP APIs
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

#include <errno.h>
#include <cmgp.h>
#include <plat/gpio/gpio.h>
#include <plat/i2c/i2c.h>
#include <plat/usi/usi.h>

#include CSP_SOURCE(cmgp)


void cmgpEnableInterrupt(uint32_t intNum)
{
    uint32_t address;
    uint32_t regValue;

    if (intNum >= CMGP_MAX_INT_NUM)
        return;
  #if (CMGP_MAX_INT_NUM > 32)
    if (intNum >= 64) {
      CSP_PRINTF_ERROR("%s : invalid irqNum (%d)\n", __func__, (int)intNum);
      return;
    }
    if (intNum < 32) {
        address = GET_INTC_REG(0, SET);
        regValue = __raw_readl(address);
        regValue |= (0x1 << intNum);
    } else {
        address = GET_INTC_REG(1, SET);
        regValue = __raw_readl(address);
        regValue |= (0x1 << (intNum - 32));
    }
    __raw_writel( regValue, address);
  #else
    if (intNum >= 32) {
        CSP_PRINTF_ERROR("%s : invalid irqNum (%d)\n", __func__, (int)intNum);
        return;
    }
    address = GET_INTC_REG(CMGP_DEFAULT_IRQ_GROUP, SET);
    regValue = __raw_readl(address);
    regValue |= (0x1 << intNum);
    __raw_writel( regValue, address);
  #endif
}

void cmgpDisableInterrupt(uint32_t intNum)
{
    uint32_t address;
    uint32_t regValue = 0;
  #if (CMGP_MAX_INT_NUM > 32)
    if (intNum >= 64) {
      CSP_PRINTF_ERROR("%s : invalid irqNum (%d)\n", __func__, (int)intNum);
      return;
    }
    if (intNum < 32) {
        address = GET_INTC_REG(0, CLR);
        regValue |= (0x1 << intNum);
    } else {
        address = GET_INTC_REG(1, CLR);
        regValue |= (0x1 << (intNum - 32));
    }
    __raw_writel( regValue, address);
  #else
    if (intNum >= 32) {
        CSP_PRINTF_ERROR("%s : invalid irqNum (%d)\n", __func__, (int)intNum);
        return;
    }
    address = GET_INTC_REG(CMGP_DEFAULT_IRQ_GROUP, CLR);
    regValue |= (0x1 << intNum);
    __raw_writel( regValue, address);
  #endif
}

void cmgpSetIrqPriority(uint32_t intNum, uint32_t priority)
{
    uint32_t address;
    uint32_t regValue = 0;
    uint32_t regMask = (0x1 << SYSREG_INTC_PRIORITY_BITFIELD) - 1;

  #if (CMGP_MAX_INT_NUM > 32)
    if (intNum >= 64) {
      CSP_PRINTF_ERROR("%s : invalid irqNum (%d)\n", __func__, (int)intNum);
      return;
    }
    if (intNum < 32) {
        address = GET_INTC_REG(0, PRIORITY) + ((intNum / 8) * 4);
    } else {
        intNum = intNum - 32;
        address = GET_INTC_REG(1, PRIORITY) + ((intNum / 8) * 4);
    }
    regValue = __raw_readl(address);
    regMask = regMask << ((intNum % 8) * 4);
    regValue &= ~regMask;
    regValue |= (regMask & (priority << ((intNum % 8) * 4)));
    __raw_writel( regValue, address);
  #else
    if (intNum >= 32) {
        //CSP_PRINTF_ERROR("%s : invalid irqNum (%d)\n", __func__, (int)intNum);
        return;
    }
    address = GET_INTC_REG(CMGP_DEFAULT_IRQ_GROUP, PRIORITY) + ((intNum / 8) * 4);
    regValue = __raw_readl(address);
    regMask = regMask << ((intNum % 8) * 4);
    regValue &= ~regMask;
    regValue |= (regMask & (priority << ((intNum % 8) * 4)));
    __raw_writel( regValue, address);
  #endif
}

int cmgpGetIrqNum(void)
{
    uint32_t address;
    uint32_t regValue = 0;
    uint32_t group = 0;
    uint32_t i,j;
    int      intNum = -1;

    #if (CMGP_MAX_INT_NUM <= 32)
        group = CMGP_DEFAULT_IRQ_GROUP;
    #endif

    if (group)
        address = GET_INTC_REG(1, NSEC_PEND);
    else
        address = GET_INTC_REG(0, NSEC_PEND);

    regValue = __raw_readl(address);

    for (j = group ; j < 2 ; j++) {
        for (i = 0 ; i < 32 ; i++) {
            if( regValue & (0x1 << i)){
              #if (CMGP_MAX_INT_NUM > 32)
                intNum = (group *32) + i;
              #else
                intNum = i;
              #endif
                return intNum;
            }
        }
      #if (CMGP_MAX_INT_NUM <= 32)
        break;
      #endif
    }

    return intNum;
}

void cmgpUSISetFunction(int usiChannel, uint32_t function)
{
    uint32_t address;

    //if (usiChannel < USI_CHUB_CHANNEL_MAX || usiChannel >= USI_CHANNEL_MAX) {
    if (usiChannel >= USI_CHANNEL_MAX) {
        CSP_PRINTF_ERROR("%s : invalid channel(%d)\n", __func__, usiChannel);
        return;
    }

    if ((function != 1) && (function != 2) && (function != 4)) {
        CSP_PRINTF_ERROR("%s : invalid function(%x)\n", __func__, (unsigned int)function);
        return;
    }

    usiChannel = usiChannel - USI_CHUB_CHANNEL_MAX;

    if (usiChannel % 2)
        address = REG_I2C_CMGP_SW_CONF_BASE + (usiChannel / 2) * REG_USI_CMGP_SW_CONF_OFFSET;
    else
        address = REG_USI_CMGP_SW_CONF_BASE + (usiChannel / 2) * REG_USI_CMGP_SW_CONF_OFFSET;

    __raw_writel(function & 0x7 , address);
}

void cmgp_IRQHandler(void)
{
    unsigned int intNum;
    intNum = cmgpGetIrqNum();

    if (intNum >= ((int)sizeof(mCmgpIrqInfo)/(int)sizeof(mCmgpIrqInfo[0])))
        return;

    switch( mCmgpIrqInfo[intNum].irqGroup ) {
        case CMGP_IRQ_GROUP_GPIO:
            gpioCmgpIRQHandler((GpioPinNumType)mCmgpIrqInfo[intNum].irqSource);
            break;
        case CMGP_IRQ_GROUP_USI:
            usiCmgpIrqHandler(mCmgpIrqInfo[intNum].irqSource);
            break;
        case CMGP_IRQ_GROUP_I2C:
            i2c_IRQHandler(mCmgpIrqInfo[intNum].irqSource);
            break;
        case CMGP_IRQ_GROUP_ADC:
            //adcCmgpIrqHandler(irqInfo.irqSource);
            break;
        default:
            CSP_ASSERT(0);
            break;
    }
}
