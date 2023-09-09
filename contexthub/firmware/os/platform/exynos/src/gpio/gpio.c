/*----------------------------------------------------------------------------
 *      Exynos SoC  -  GPIO
 *----------------------------------------------------------------------------
 *      Name:    gpio.c
 *      Purpose: To implement GPIO APIs
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

#include <cmsis.h>
#include <csp_common.h>
#include <gpio.h>
#include <plat/gpio/gpio.h>
#include <plat/cpu/cpu.h>
#include <plat/cpu/mpu.h>
#include <plat/cmgp/cmgp.h>
#if defined(EMBOS)
  #include <Device.h>
#endif
#include CSP_SOURCE(gpio)

#define GPIO_DEBUG 0

#define PRINTF_ERROR CSP_PRINTF_ERROR
#if GPIO_DEBUG
    #define PRINTF_DEBUG CSP_PRINTF_DEBUG
#else
    #define PRINTF_DEBUG(fmt, ...) {}
#endif

// Public API to initialize GPIO. This should be called when OS starts
void gpioInit(void)
{
    GpioPinNumType pin;

    for (pin = GPIO_CHUB_START_PIN_NUM ; pin < GPIO_CHUB_MAX_PIN_NUM ; pin++) {
        gpioUnsetExtInterrupt(pin);
        NVIC_DisableIRQ((IRQn_Type)mGpioPinInfo[pin].irqNum);
        NVIC_ClearPendingIRQ((IRQn_Type)mGpioPinInfo[pin].irqNum);
    }
}

static int gpioCheck(uint32_t gpioNum)
{
    unsigned int lr;
    asm volatile ("mov %0, lr" : "=r" (lr));

    if (gpioNum >= GPIO_MAX_PIN_NUM) {
        PRINTF_ERROR("%s fail.(%d) 0x%X\n", __func__, (int)gpioNum, lr);
        return -1;
    }
    return 0;
}

static int gpioCheckHandler(const struct Gpio* gpio, unsigned int lr) 
{
    if (gpio == NULL) {
        PRINTF_ERROR("Invalid GPIO Handler (LR:%x)\n", lr);
        return -1;
    }
    return 0;
}

// Public API to configure GPIO function
void gpioSetFunction(GpioPinNumType gpioPinNum, uint32_t gpioFunction)
{
    uint32_t address;
    uint32_t regValue;
    uint32_t regMask=0;

    if (gpioCheck(gpioPinNum))
        return;

    address = mGpioPinInfo[gpioPinNum].conBase;
    regMask = (0x1 << REG_CON_BITFIELD) - 1;
    regMask = regMask << (REG_CON_BITFIELD * mGpioPinInfo[gpioPinNum].index);

    regValue = __raw_readl(address);
    regValue &= ~regMask;
    regValue |= regMask & (gpioFunction << (REG_CON_BITFIELD * mGpioPinInfo[gpioPinNum].index));
    PRINTF_DEBUG("%s(%d) : %x > %x\n", __func__, gpioPinNum, (unsigned int)regValue, (unsigned int)address);
    __raw_writel(regValue, address);
}

// Public API to set pull up, down setting
void gpioSetPud(GpioPinNumType gpioPinNum, uint32_t gpioPudValue)
{
    uint32_t address;
    uint32_t regValue;
    uint32_t regMask=0;

    if (gpioCheck(gpioPinNum))
        return;

    address = mGpioPinInfo[gpioPinNum].conBase + REG_PUD_OFFSET;
    regMask = (0x1 << REG_PUD_BITFIELD) - 1;
    regMask = regMask << (REG_PUD_BITFIELD * mGpioPinInfo[gpioPinNum].index);

    regValue = __raw_readl(address);
    regValue &= ~regMask;
    regValue |= regMask & (gpioPudValue << (REG_PUD_BITFIELD * mGpioPinInfo[gpioPinNum].index));
    __raw_writel(regValue, address);
}

// Public API to set drive-strength
void gpioSetDrvStrength(GpioPinNumType gpioPinNum, uint32_t gpioDrvValue)
{
    uint32_t address;
    uint32_t regValue;
    uint32_t regMask=0;

    if (gpioCheck(gpioPinNum))
        return;

    address = mGpioPinInfo[gpioPinNum].conBase + REG_DRV_OFFSET;
    regMask = (0x1 << REG_DRV_BITFIELD) - 1;
    regMask = regMask << (REG_DRV_BITFIELD * mGpioPinInfo[gpioPinNum].index);

    regValue = __raw_readl(address);
    regValue &= ~regMask;
    regValue |= regMask & (gpioDrvValue << (REG_DRV_BITFIELD * mGpioPinInfo[gpioPinNum].index));
    __raw_writel(regValue, address);
}

// Public API to set the level of GPIO Pad
void gpioSetData(GpioPinNumType gpioPinNum, uint32_t gpioDataValue)
{
    uint32_t address;
    uint32_t regValue;
    uint32_t regMask=0;

    if (gpioCheck(gpioPinNum))
        return;

    address = mGpioPinInfo[gpioPinNum].conBase + REG_DAT_OFFSET;
    regMask = (0x1 << REG_DAT_BITFIELD) - 1;
    regMask = regMask << (REG_DAT_BITFIELD * mGpioPinInfo[gpioPinNum].index);

    regValue = __raw_readl(address);
    regValue &= ~regMask;
    regValue |= regMask & (gpioDataValue << (REG_DAT_BITFIELD * mGpioPinInfo[gpioPinNum].index));
    __raw_writel(regValue, address);
}

// Public API to read the level of GPIO
uint32_t gpioGetData(GpioPinNumType gpioPinNum)
{
    uint32_t address;
    uint32_t regValue;
    uint32_t regMask=0;

    if (gpioCheck(gpioPinNum))
        return 0xFFFFFFFF;

    address = mGpioPinInfo[gpioPinNum].conBase + REG_DAT_OFFSET;
    regMask = (0x1 << REG_DAT_BITFIELD) - 1;
    regMask = regMask << (REG_DAT_BITFIELD * mGpioPinInfo[gpioPinNum].index);

    regValue = __raw_readl(address);
    if (regValue &= regMask)
        return 1;
    else
        return 0;
}

uint32_t gpioGetConfig(GpioPinNumType gpioPinNum)
{
    uint32_t address;
    uint32_t regValue;
    uint32_t regMask=0;

    if (gpioCheck(gpioPinNum))
        return 0xFFFFFFFF;

    address = mGpioPinInfo[gpioPinNum].conBase;
    regMask = (0x1 << REG_CON_BITFIELD) - 1;

    regValue = __raw_readl(address);
    regValue = regValue >> (REG_CON_BITFIELD * mGpioPinInfo[gpioPinNum].index);
    regValue = regValue & regMask;

    return regValue;
}

// Public API to configure GPIO function for system power mode
void gpioConfigPdn(GpioPinNumType gpioPinNum, uint32_t gpioFunction)
{
#if (GPIO_SUPPORT_PDN_CONTROL)
    uint32_t address;
    uint32_t regValue;
    uint32_t regMask=0;

    if (gpioCheck(gpioPinNum))
        return ;

    address = mGpioPinInfo[gpioPinNum].conBase + REG_PDN_CON_OFFSET;
    regMask = (0x1 << REG_CON_BITFIELD) - 1;
    regMask = regMask << (REG_CON_BITFIELD * mGpioPinInfo[gpioPinNum].index);

    regValue = __raw_readl(address);
    regValue &= ~regMask;
    regValue |= regMask & (gpioDataValue << (REG_CON_BITFIELD * mGpioPinInfo[gpioPinNum].index));
    __raw_writel(regValue, address);
#else
    return;
#endif
}


// Public API to configure GPIO function for system power mode
void gpioSetPudPdn(GpioPinNumType gpioPinNum, uint32_t gpioPudValue)
{
#if (GPIO_SUPPORT_PDN_CONTROL)
    uint32_t address;
    uint32_t regValue;
    uint32_t regMask=0;

    if (gpioCheck(gpioPinNum))
        return ;

    address = mGpioPinInfo[gpioPinNum].conBase + REG_PDN_PUD_OFFSET;
    regMask = (0x1 << REG_PUD_BITFIELD) - 1;
    regMask = regMask << (REG_PUD_BITFIELD * mGpioPinInfo[gpioPinNum].index);

    regValue = __raw_readl(address);
    regValue &= ~regMask;
    regValue |= regMask & (gpioDataValue << (REG_PUD_BITFIELD * mGpioPinInfo[gpioPinNum].index));
    __raw_writel(regValue, address);
#else
    return;
#endif
}

void gpioSetIntFilter(GpioPinNumType gpioPinNum,
                      IntFilterSel filterType,
                      uint32_t filterWidth)
{
    uint32_t address;
    uint32_t regValue;
    uint32_t regMask;
    uint32_t filter=0;

    if (gpioCheck(gpioPinNum))
        return ;

    if (filterType != eNoFLT)
      filter = (0x1 << REG_FILTER_EN_SHIFT);  // Enable Interrupt

#if (GPIO_SUPPORT_FILTERSEL)
    if (filter != eNoFLT)
        filter |= ((filterType - 1) << REG_FILTER_SEL_SHIFT);
#endif

    filterWidth &= (0x1 << REG_FILTER_WIDTH_BITFIELD) - 1;
    filter |= filterWidth;

    address = mGpioPinInfo[gpioPinNum].eintFltcon;
    regMask = (0x1 << REG_NWEINT_FLTCON_BITFIELD) - 1;
    if (REG_NWEINT_FLTCON_BITFIELD * mGpioPinInfo[gpioPinNum].index >= 32) {
        regMask = regMask << (REG_NWEINT_FLTCON_BITFIELD *
               (mGpioPinInfo[gpioPinNum].index - (32/REG_NWEINT_FLTCON_BITFIELD)));
    } else {
        regMask = regMask << (REG_NWEINT_FLTCON_BITFIELD * mGpioPinInfo[gpioPinNum].index);
    }

    regValue = __raw_readl(address);
    regValue &= ~regMask;
    if (REG_NWEINT_FLTCON_BITFIELD * mGpioPinInfo[gpioPinNum].index >= 32) {
      regValue |= regMask & (filter << (REG_NWEINT_FLTCON_BITFIELD *
                (mGpioPinInfo[gpioPinNum].index - (32/REG_NWEINT_FLTCON_BITFIELD))));
    } else {
      regValue |= regMask & (filter << (REG_NWEINT_FLTCON_BITFIELD * mGpioPinInfo[gpioPinNum].index));
    }

    PRINTF_DEBUG("%s(%d) : %x > %x\n", __func__, gpioPinNum, (unsigned int)regValue, (unsigned int)address);
    __raw_writel(regValue, address);
}

void gpioEnableInterrupt(GpioPinNumType gpioPinNum, uint32_t en)
{
    uint32_t address;
    uint32_t regValue;
    uint32_t regMask;

    if (gpioCheck(gpioPinNum))
        return ;

    address = mGpioPinInfo[gpioPinNum].eintBase + REG_NWEINT_MASK_OFFSET;
    regMask = (0x1 << REG_NWEINT_MASK_BITFIELD) - 1;
    regMask = regMask << (mGpioPinInfo[gpioPinNum].index * REG_NWEINT_MASK_BITFIELD);

    regValue = __raw_readl(address);
    if (en) {
        regValue &= ~regMask;
        if (gpioPinNum < GPIO_CHUB_MAX_PIN_NUM) {
            NVIC_ClearPendingIRQ((IRQn_Type)mGpioPinInfo[gpioPinNum].irqNum);
            NVIC_EnableIRQ((IRQn_Type)mGpioPinInfo[gpioPinNum].irqNum);
        } else {  // CMGP GPIO Interrupt
            cmgpEnableInterrupt(mGpioPinInfo[gpioPinNum].irqNum);
        }
    } else {
        regValue |= regMask;
        if (gpioPinNum < GPIO_CHUB_MAX_PIN_NUM) {
            NVIC_DisableIRQ((IRQn_Type)mGpioPinInfo[gpioPinNum].irqNum);
            NVIC_ClearPendingIRQ((IRQn_Type)mGpioPinInfo[gpioPinNum].irqNum);
        } else {  // CMGP GPIO Interrupt
            cmgpDisableInterrupt(mGpioPinInfo[gpioPinNum].irqNum);
        }
    }

    PRINTF_DEBUG("%s(%d) : %x > %x\n", __func__, gpioPinNum, (unsigned int)regValue, (unsigned int)address);
    __raw_writel(regValue, address);
}

void gpioClearPending(GpioPinNumType gpioPinNum)
{
    uint32_t address;
    uint32_t regMask;

    if (gpioCheck(gpioPinNum))
        return ;

    address = mGpioPinInfo[gpioPinNum].eintBase + REG_NWEINT_PEND_OFFSET;
    regMask = (0x1 << REG_NWEINT_PEND_BITFIELD) - 1;
    regMask = regMask << (mGpioPinInfo[gpioPinNum].index * REG_NWEINT_PEND_BITFIELD);

    PRINTF_DEBUG("%s : %x > %x\n", __func__, (unsigned int)regMask, (unsigned int)address);
    __raw_writel(regMask, address);
}

uint32_t gpioIsPending(GpioPinNumType gpioPinNum)
{
    uint32_t address;
    uint32_t regValue;
    uint32_t regMask;

    if (gpioCheck(gpioPinNum))
        return 0;

    address = mGpioPinInfo[gpioPinNum].eintBase + REG_NWEINT_PEND_OFFSET;
    regMask = (0x1 << REG_NWEINT_PEND_BITFIELD) - 1;
    regMask = regMask << (mGpioPinInfo[gpioPinNum].index * REG_NWEINT_PEND_BITFIELD);

    regValue = __raw_readl(address);

    return regValue & regMask;
}

void gpioSetExtInterrupt(GpioPinNumType gpioPinNum,
                         IntTriggerType intTrigger,
                         IntFilterSel   intFilter,
                         uint32_t       filterWidth,
                         void (*callbackFunction) (void))
{
    uint32_t address;
    uint32_t regValue;
    uint32_t regMask;
    uint32_t vectorBase = SCB->VTOR;

    if (gpioCheck(gpioPinNum))
        return ;

    if (intTrigger > Both_Edge) {
        CSP_ASSERT(0);
        return;
    }

    if (callbackFunction == NULL) {
        PRINTF_ERROR("%s fail : no callback function\n", __func__);
        return;
    }

    address = mGpioPinInfo[gpioPinNum].eintBase;
    regMask = (0x1 << REG_NWEINT_CON_BITFIELD) - 1;
    regMask = regMask << (mGpioPinInfo[gpioPinNum].index * REG_NWEINT_CON_BITFIELD);

    regValue = __raw_readl(address);
    regValue &= ~regMask;
    regValue |= intTrigger << (mGpioPinInfo[gpioPinNum].index * REG_NWEINT_CON_BITFIELD);
    PRINTF_DEBUG("%s(%d) : %x > %x\n", __func__, gpioPinNum, (unsigned int)regValue, (unsigned int)address);
    __raw_writel(regValue, address);

    gpioSetIntFilter(gpioPinNum, intFilter, filterWidth);

    PRINTF_DEBUG("%s(%d) : Set Callback %x\n", __func__, gpioPinNum, (unsigned int)callbackFunction);
    mGpioPinInfo[gpioPinNum].callback = callbackFunction;
  #if !defined (SUPPORT_ONE_GPIO_INTERRUPT)
    if (gpioPinNum < GPIO_CHUB_MAX_PIN_NUM) {
        cpuRegisterHandler(mGpioPinInfo[gpioPinNum].irqNum, callbackFunction);
    }
  #endif
    mGpioPinInfo[gpioPinNum].eintEn = 1;

    gpioClearPending(gpioPinNum);
    gpioEnableInterrupt(gpioPinNum, 1);
}

//
void gpioUnsetExtInterrupt(GpioPinNumType gpioPinNum)
{
    if (gpioCheck(gpioPinNum))
        return ;

    gpioEnableInterrupt(gpioPinNum, 0);
    gpioClearPending(gpioPinNum);

    mGpioPinInfo[gpioPinNum].callback = NULL;
    mGpioPinInfo[gpioPinNum].eintEn = 0;
}

void gpioDeinit(void)
{
    GpioPinNumType pin;
    for (pin = GPIO_CHUB_START_PIN_NUM ; pin < GPIO_CHUB_MAX_PIN_NUM ; pin++) {
        gpioUnsetExtInterrupt(pin);
    }
}

#if defined(SFR_SAVE_RESTORE)
void gpioSaveState(void)
{
    // To save GPIO SFRs before entering system power mode
}

void gpioRestoreState(void)
{
    // To restore GPIO SFRs after exiting system power mode
}
#endif

void gpio_IRQHandler(void)
{
    GpioPinNumType pin;
    int found=0;

    for (pin = GPIO_CHUB_START_PIN_NUM ; pin < GPIO_CHUB_MAX_PIN_NUM ; pin++) {
        if (gpioIsPending(pin)) {
            found = 1;
            if (mGpioPinInfo[pin].eintEn) {
                if (mGpioPinInfo[pin].callback == NULL) {
                    PRINTF_ERROR("%s : NULL callback(%u)\n", __func__, pin);
                } else {
                    mGpioPinInfo[pin].callback();
                }
            } else {
                PRINTF_ERROR("%s : supurious EINT(%u)\n", __func__, pin);
            }
            gpioClearPending(pin);
            return;
        }
    }

    if (found == 0) {
        PRINTF_ERROR("%s : No pending EINT\n", __func__);
    }
}

void gpioCmgpIRQHandler(GpioPinNumType gpioPinNum)
{
    if (gpioIsPending((GpioPinNumType)mGpioPinInfo[gpioPinNum].pin)) {
        if (mGpioPinInfo[gpioPinNum].eintEn) {
            if (mGpioPinInfo[gpioPinNum].callback == NULL) {
                PRINTF_ERROR("%s : NULL callback(%u)\n", __func__, gpioPinNum);
            } else {
                mGpioPinInfo[gpioPinNum].callback();
            }
        } else {
            PRINTF_ERROR("%s : supurious EINT(%u)\n", __func__, gpioPinNum);
        }
        gpioClearPending(gpioPinNum);
    } else {
        PRINTF_ERROR("%s : No pending EINT(%u)\n", __func__, gpioPinNum);
    }
}


struct Gpio *gpioRequest(uint32_t gpioNum)
{
    if (gpioCheck(gpioNum))
        return NULL;

    return &mGpioPinInfo[gpioNum];
}

void gpioRelease(struct Gpio* gpio)
{
    unsigned int lr;
    asm volatile ("mov %0, lr" : "=r" (lr));

    if (gpioCheckHandler(gpio, lr)) {
        return;
    }

    gpioUnsetExtInterrupt((GpioPinNumType)gpio->pin);

    if (gpio->pin < GPIO_CHUB_MAX_PIN_NUM) {
        NVIC_ClearPendingIRQ((IRQn_Type)gpio->irqNum);
        NVIC_DisableIRQ((IRQn_Type)gpio->irqNum);
    }
}

void gpioConfigInput(const struct Gpio* gpio, int32_t drvStrength, enum GpioPullMode pull)
{
    uint32_t pud = GPIO_PUD_PULLDOWN;
    unsigned int lr;
    asm volatile ("mov %0, lr" : "=r" (lr));

    if (gpioCheckHandler(gpio, lr)) {
        return;
    }

    switch(pull)
    {
        case GPIO_PULL_NONE:
            pud = GPIO_PUD_DISABLE;
            break;
        case GPIO_PULL_UP:
            pud = GPIO_PUD_PULLUP;
            break;
        case GPIO_PULL_DOWN:
            pud = GPIO_PUD_PULLDOWN;
            break;
    }
    gpioSetFunction((GpioPinNumType)gpio->pin, GPIO_CON_INPUT);
    gpioSetPud((GpioPinNumType)gpio->pin, pud);
    gpioSetDrvStrength((GpioPinNumType)gpio->pin, drvStrength);
}

void gpioConfigOutput(const struct Gpio* gpio, int32_t drvStrength, enum GpioPullMode pull, enum GpioOpenDrainMode odrMode, bool value)
{
    uint32_t pud = GPIO_PUD_PULLDOWN;
    unsigned int lr;
    asm volatile ("mov %0, lr" : "=r" (lr));

    if (gpioCheckHandler(gpio, lr)) {
        return;
    }

    switch(pull)
    {
        case GPIO_PULL_NONE:
            pud = GPIO_PUD_DISABLE;
            break;
        case GPIO_PULL_UP:
            pud = GPIO_PUD_PULLUP;
            break;
        case GPIO_PULL_DOWN:
            pud = GPIO_PUD_PULLDOWN;
            break;
    }
    gpioSetPud((GpioPinNumType)gpio->pin, pud);
    gpioSetDrvStrength((GpioPinNumType)gpio->pin, drvStrength);
    gpioSetData((GpioPinNumType)gpio->pin, value);
    gpioSetFunction((GpioPinNumType)gpio->pin, GPIO_CON_OUTPUT);
}

void gpioConfigAlt(const struct Gpio* gpio, int32_t drvStrength, enum GpioPullMode pull, enum GpioOpenDrainMode odrMode, uint32_t altFunc)
{
    uint32_t pud = GPIO_PUD_PULLDOWN;
    unsigned int lr;
    asm volatile ("mov %0, lr" : "=r" (lr));


    (void)odrMode;

    if (gpioCheckHandler(gpio, lr)) {
        return;
    }

    switch(pull)
    {
        case GPIO_PULL_NONE:
            pud = GPIO_PUD_DISABLE;
            break;
        case GPIO_PULL_UP:
            pud = GPIO_PUD_PULLUP;
            break;
        case GPIO_PULL_DOWN:
            pud = GPIO_PUD_PULLDOWN;
            break;
    }
    gpioSetPud((GpioPinNumType)gpio->pin, pud);
    gpioSetDrvStrength((GpioPinNumType)gpio->pin, drvStrength);
    gpioSetFunction((GpioPinNumType)gpio->pin, altFunc);
}

void gpioSetInterrupt(struct Gpio * gpio,
                         IntTriggerType intTrigger,
                         IntFilterSel   intFilter,
                         uint32_t       filterWidth,
                         void (*callbackFunction) (void))
{
    unsigned int lr;
    asm volatile ("mov %0, lr" : "=r" (lr));

    if (gpioCheckHandler(gpio, lr)) {
        return;
    }

    gpioSetExtInterrupt((GpioPinNumType)gpio->pin, intTrigger, intFilter, filterWidth, callbackFunction);
}

void gpioSet(const struct Gpio* gpio, bool value)
{
    unsigned int lr;
    asm volatile ("mov %0, lr" : "=r" (lr));

    if (gpioCheckHandler(gpio, lr)) {
        return;
    }

    gpioSetData((GpioPinNumType)gpio->pin, value);
}

bool gpioGet(const struct Gpio *gpio)
{
    unsigned int lr;
    asm volatile ("mov %0, lr" : "=r" (lr));

    if (gpioCheckHandler(gpio, lr)) {
        return false;
    }

    return gpioGetData((GpioPinNumType)gpio->pin);
}
