/*----------------------------------------------------------------------------
 *      Exynos SoC  -  GPIO
 *----------------------------------------------------------------------------
 *      Name:    gpioDrv.c
 *      Purpose: To implement GPIO driver functions
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
#if defined(SEOS)
#include <cmsis.h>
#endif
#include <gpioDrv.h>

static uint32_t mGpioBitMask[] = {
    0,
    0x1,
    0x3,
    0x7,
    0xF
};

#if defined(s5e9610)
typedef struct {
    uint32_t order:4;

    /* Number of Selection Bits */
    uint32_t num_con_bits;    // if not valid, 0
    uint32_t num_dat_bits;    // if not valid, 0
    uint32_t num_pud_bits;    // if not valid, 0
    uint32_t num_drv_bits;    // if not valid, 0
    uint32_t num_conpdn_bits; // if not valid, 0
    uint32_t num_pudpdn_bits; // if not valid, 0

    /* Address of CON, DAT, PUD, CONPDN, PUDPDN register */
    uint32_t addr_con;
    uint32_t addr_dat; //valid(0~5), if not valid, mark as 0x7
    uint32_t addr_pud; //valid(0~5), if not valid, mark as 0x7
    uint32_t addr_drv; //valid(0~5), if not valid, mark as 0x7
    uint32_t addr_conpdn;      //valid(0~5), if not valid, mark as 0x7
    uint32_t addr_pudpdn;      //valid(0~5), if not valid, mark as 0x7

    /* CON or PUD is base address */
} GpioPinInfoType;
#elif defined (s5e9630) || defined (s5e3830)
typedef struct {
    uint32_t order:4;

    /* Number of Selection Bits */
    uint32_t num_con_bits;    // if not valid, 0
    uint32_t num_dat_bits;    // if not valid, 0
    uint32_t num_pud_bits;    // if not valid, 0
    uint32_t num_drv_bits;    // if not valid, 0

    /* Address of CON, DAT, PUD, CONPDN register */
    uint32_t addr_con;
    uint32_t addr_dat; //valid(0~5), if not valid, mark as 0x7
    uint32_t addr_pud; //valid(0~5), if not valid, mark as 0x7
    uint32_t addr_drv; //valid(0~5), if not valid, mark as 0x7
} GpioPinInfoType;
#endif
typedef struct {
    /* Number of Selection Bits */
    uint32_t con_order;
    uint32_t flt_order;
    uint32_t mask_order;
    uint32_t pend_order;

    /* GPIO Port, Pin Number */
    GpioPinNumType gpioPinNum;

    /* Address */
    uint32_t con_address;
    uint32_t flt_address;
    uint32_t mask_address;
    uint32_t pend_address;
    //uint32_t  *fixpri_address;
} GpioEintInfoType;

#include CSP_SOURCE(gpio)

void gpioDrvInit(void)
{
    GpioEintNumType eint, end_eint;

    end_eint = GPIO_CHUB_MAX_EINT_NUM;
#ifdef TEST_PRINT
	CSP_PRINTF_ERROR("%s: end_eint(%d) \n", __FUNCTION__, end_eint);
#endif
    // Disable CHUB EINTs before enabling GPIO_CHUB_IRQn
    for(eint = (GpioEintNumType)0 ; eint < end_eint ; eint++)
        gpioDrvUnsetExtInterrupt(eint);

	/* TBD: without powergating */
#if defined(s5e9610)
    NVIC_ClearPendingIRQ(GPIO_CHUB_IRQn);
    NVIC_EnableIRQ(GPIO_CHUB_IRQn);
#elif defined(s5e9630)
    for(int i=0; i<19; i++)
    {
        if(i!=8){
            NVIC_ClearPendingIRQ(EXT_INTH0_0 + i);
            NVIC_EnableIRQ(EXT_INTH0_0 + i);
		}
    }
#endif
}

//
void gpioDrvConfig(IN GpioPinNumType gpioPinNum, IN uint32_t gpioFunction)
{
    uint32_t order;
    uint32_t num_con_bits;
    volatile uint32_t sfr_address;
    uint32_t reg_value;
    uint32_t reg_mask = 0x0;

#ifdef TEST_PRINT
	CSP_PRINTF_ERROR(" %s: gpioPinNum(%d), gpioFunction(%d) \n", __FUNCTION__, gpioPinNum, (int)gpioFunction);
	CSP_PRINTF_ERROR(" %s: gpioPinNum(%d), gpioFunction(%d) \n", __FUNCTION__, gpioPinNum, (int)gpioFunction);
#endif
    // Check if given GPIO can be configured
    num_con_bits = mGpioPinInfo[gpioPinNum].num_con_bits;
    if (num_con_bits == 0) {
        return;
    }

    order = mGpioPinInfo[gpioPinNum].order;
    sfr_address = mGpioPinInfo[gpioPinNum].addr_con;
#ifdef TEST_PRINT
	CSP_PRINTF_ERROR("%s: sfr_address(0x%x), order(%d), num_con_bits(%d)\n", __FUNCTION__, (int)sfr_address,(int)order,(int)num_con_bits);
#endif

    reg_mask = mGpioBitMask[num_con_bits];
    reg_mask <<= (order * num_con_bits);

    reg_value = __raw_readl( sfr_address );
    reg_value &= ~reg_mask;
    reg_value |= (gpioFunction << (order * num_con_bits));
    __raw_writel( reg_value, sfr_address );
#ifdef TEST_PRINT
	CSP_PRINTF_ERROR("%s: sfr_address(0x%x), reg_value(0x%x) \n", __FUNCTION__, (int)sfr_address, (int)reg_value);
#endif
}

uint32_t gpioDrvGetConfig(IN GpioPinNumType gpioPinNum)
{
    uint32_t order;
    uint32_t num_con_bits;
    volatile uint32_t sfr_address;
    uint32_t reg_value;
    uint32_t reg_mask = 0x0;

    // Check if given GPIO can be configured
    num_con_bits = mGpioPinInfo[gpioPinNum].num_con_bits;
    if (num_con_bits == 0) {
        return -1;
    }

    order = mGpioPinInfo[gpioPinNum].order;
    sfr_address = mGpioPinInfo[gpioPinNum].addr_con;

    reg_mask = mGpioBitMask[num_con_bits];

    reg_value = __raw_readl( sfr_address );
    reg_value = reg_value >> (order * num_con_bits);
    reg_value &= reg_mask;

    return reg_value;
}

//
void gpioDrvSetData(IN GpioPinNumType gpioPinNum, IN uint32_t gpioDataValue)
{
    uint32_t order;
    uint32_t num_dat_bits;
    volatile uint32_t sfr_address;
    uint32_t reg_value;
    uint32_t reg_mask = 0x0;

    // Check if given GPIO can be set
    num_dat_bits = mGpioPinInfo[gpioPinNum].num_dat_bits;
    if (num_dat_bits == 0) {
        return;
    }
#ifdef TEST_PRINT
	//CSP_PRINTF_ERROR("%s: gpioPinNum(%d), gpioDataValue(%d) \n", __FUNCTION__, gpioPinNum, (int)gpioDataValue);
#endif

    order = mGpioPinInfo[gpioPinNum].order;
    sfr_address = mGpioPinInfo[gpioPinNum].addr_dat;
#ifdef TEST_PRINT
	//CSP_PRINTF_ERROR("%s: sfr_address(0x%x) \n", __FUNCTION__, (int)sfr_address);
#endif

    reg_mask = mGpioBitMask[num_dat_bits];
    reg_mask <<= (order * num_dat_bits);

    reg_value = __raw_readl( sfr_address );
    reg_value &= ~reg_mask;
    reg_value |= (gpioDataValue << (order * num_dat_bits));
    __raw_writel( reg_value, sfr_address );
#ifdef TEST_PRINT
	CSP_PRINTF_ERROR("%s:  gpioPinNum(%d), reg_value(0x%x) \n", __FUNCTION__,gpioPinNum, (int)reg_value);
#endif
}

//
uint32_t gpioDrvGetData(IN GpioPinNumType gpioPinNum)
{
    uint32_t order;
    uint32_t num_dat_bits;
    volatile uint32_t sfr_address;
    uint32_t reg_value;
    uint32_t reg_mask = 0x0;

    // Check if given GPIO can be read
    num_dat_bits = mGpioPinInfo[gpioPinNum].num_dat_bits;
    if (num_dat_bits == 0) {
        return 0xffffffff;
    }

    order = mGpioPinInfo[gpioPinNum].order;
    sfr_address = mGpioPinInfo[gpioPinNum].addr_dat;

    reg_mask = mGpioBitMask[num_dat_bits];
    reg_mask <<= (order * num_dat_bits);

    // Just reading value, therefore it does not need to be thread-safe
    reg_value = __raw_readl( sfr_address );
    reg_value &= reg_mask;
    reg_value >>= (order * num_dat_bits);

    return reg_value;
}

//
void gpioDrvSetPud(IN GpioPinNumType gpioPinNum, IN uint32_t gpioPudValue)
{
    uint32_t order;
    uint32_t num_pud_bits;
    volatile uint32_t sfr_address;
    uint32_t reg_value;
    uint32_t reg_mask = 0x0;

    // Check if given GPIO can be set
    num_pud_bits = mGpioPinInfo[gpioPinNum].num_pud_bits;
    if (num_pud_bits == 0) {
        return;
    }

    order = mGpioPinInfo[gpioPinNum].order;
    sfr_address = mGpioPinInfo[gpioPinNum].addr_pud;

    reg_mask = mGpioBitMask[num_pud_bits];
    reg_mask <<= (order * num_pud_bits);

    reg_value = __raw_readl(sfr_address);
    reg_value &= ~reg_mask;
    reg_value |= (gpioPudValue << (order * num_pud_bits));
    __raw_writel( reg_value, sfr_address );
}

//
void gpioDrvSetDrvStrength(IN GpioPinNumType gpioPinNum,
                           IN uint32_t gpioDrvValue)
{
    uint32_t order;
    uint32_t num_drv_bits;
    volatile uint32_t sfr_address;
    uint32_t reg_value;
    uint32_t reg_mask = 0x0;

    // Check if given GPIO can be set
    num_drv_bits = mGpioPinInfo[gpioPinNum].num_drv_bits;
    if (num_drv_bits == 0) {
        return;
    }

    order = mGpioPinInfo[gpioPinNum].order;
    sfr_address = mGpioPinInfo[gpioPinNum].addr_drv;

    reg_mask = mGpioBitMask[num_drv_bits];
    reg_mask <<= (order * num_drv_bits);

    reg_value = __raw_readl( sfr_address );
    reg_value &= ~reg_mask;
    reg_value |= (gpioDrvValue << (order * num_drv_bits));
    __raw_writel( reg_value, sfr_address );
}

//
void gpioDrvConfigPdn(IN GpioPinNumType gpioPinNum, IN uint32_t gpioFunction)
{
#if defined(s5e9610)
    uint32_t order;
    uint32_t num_conpdn_bits;

    volatile uint32_t sfr_address;
    uint32_t reg_value;
    uint32_t reg_mask = 0x0;

    // Check if given GPIO can be set
    num_conpdn_bits = mGpioPinInfo[gpioPinNum].num_conpdn_bits;
    if (num_conpdn_bits == 0) {
        return;
    }

    order = mGpioPinInfo[gpioPinNum].order;
    sfr_address = mGpioPinInfo[gpioPinNum].addr_conpdn;

    reg_mask = mGpioBitMask[num_conpdn_bits];
    reg_mask <<= (order * num_conpdn_bits);

    reg_value = __raw_readl( sfr_address );
    reg_value &= ~reg_mask;
    reg_value |= (gpioFunction << (order * num_conpdn_bits));
    __raw_writel( reg_value, sfr_address );
#endif
}

//
void gpioDrvSetPudPdn(IN GpioPinNumType gpioPinNum, IN uint32_t gpioPudValue)
{
#if defined(s5e9610)
    uint32_t order;
    uint32_t num_pudpdn_bits;
    volatile uint32_t sfr_address;
    uint32_t reg_value;
    uint32_t reg_mask = 0x0;

    // Check if given GPIO can be set
    num_pudpdn_bits = mGpioPinInfo[gpioPinNum].num_pudpdn_bits;
    if (num_pudpdn_bits == 0) {
        return;
    }

    order = mGpioPinInfo[gpioPinNum].order;
    sfr_address = mGpioPinInfo[gpioPinNum].addr_pudpdn;

    reg_mask = mGpioBitMask[num_pudpdn_bits];
    reg_mask <<= (order * num_pudpdn_bits);

    reg_value = __raw_readl( sfr_address );
    reg_value &= ~reg_mask;
    reg_value |= (gpioPudValue << (order * num_pudpdn_bits));
    __raw_writel( reg_value, sfr_address );
#endif
}

//
void gpioDrvSetExtInterrupt(IN GpioEintNumType gpioEintNum,
                            IN IntTriggerType intTrigger,
                            IN IntFilterType intFilter,
                            IN uint32_t intFilterWidth,
                            IN void (*callbackFunction) (uint32_t))
{
    uint32_t order;
    volatile uint32_t sfr_address;
    uint32_t reg_value;
    const uint32_t con_mask = 0x7;
    const uint32_t flt_mask = 0xFF;
    GpioPinNumType gpioPinNum;

    (void)callbackFunction; // to remove warning

#ifdef TEST_PRINT
		CSP_PRINTF_ERROR("%s: gpioEintNum(0x%x), intTrigger(%d) \n", __FUNCTION__, (int)gpioEintNum, (int)intTrigger);
#endif
    gpioPinNum = mGpioEintInfo[gpioEintNum].gpioPinNum;

    // Just in case user did not configure the GPIO as external interrupt function
    gpioDrvConfig(gpioPinNum, 0x0F);

    // External Interrupt Configuration Register
    order = mGpioEintInfo[gpioEintNum].con_order;
    sfr_address = mGpioEintInfo[gpioEintNum].con_address;

    reg_value = __raw_readl( sfr_address );
    reg_value &= ~(con_mask << order);
    reg_value |= (intTrigger << order);

    __raw_writel( reg_value, sfr_address );
#ifdef TEST_PRINT
	CSP_PRINTF_ERROR("%s: gpioEintNum(0x%x), sfr_address(0x%x), reg_value(0x%x) \n", __FUNCTION__, (int)gpioEintNum, (int)sfr_address, (int)reg_value);
#endif

    // External Interrupt Filter Control Register
    order = mGpioEintInfo[gpioEintNum].flt_order;
    sfr_address = mGpioEintInfo[gpioEintNum].flt_address;

    reg_value = __raw_readl( sfr_address );
    reg_value &= ~(flt_mask << order);

    if ((intFilter == eEnFLT) && (intFilterWidth > 0x7F)) {
        CSP_ASSERT(0);
    }

    switch (intFilter) {
    case eDisFLT:
        reg_value |= (0x00 << order);
        break;
    case eEnFLT:
        reg_value |= ((0x80 | intFilterWidth) << order);
        break;
    default:
        CSP_ASSERT(0);
        break;
    }

    __raw_writel( reg_value, sfr_address );

    // External Interrupt Pend Register
    order = mGpioEintInfo[gpioEintNum].pend_order;
    sfr_address = mGpioEintInfo[gpioEintNum].pend_address;
    reg_value = (0x1 << order);
    __raw_writel( reg_value, sfr_address );

    // External Interrupt Mask Register
    sfr_address = mGpioEintInfo[gpioEintNum].mask_address;
    reg_value = __raw_readl( sfr_address );
    order = mGpioEintInfo[gpioEintNum].mask_order;
    reg_value &= ~(0x1 << order);
    __raw_writel( reg_value, sfr_address );
}

//
void gpioDrvUnsetExtInterrupt(IN GpioEintNumType gpioEintNum)
{
    uint32_t order;
    volatile uint32_t sfr_address;
    uint32_t reg_value;
    //GpioPinNumType gpioPinNum;

    // Make the pad input
    //gpioPinNum = mGpioEintInfo[gpioEintNum].gpioPinNum;
	//No need to change the configuration when removing pending interrupts
    //gpioDrvConfig(gpioPinNum, 0);

    // External Interrupt Mask Register
    order = mGpioEintInfo[gpioEintNum].mask_order;
    sfr_address = mGpioEintInfo[gpioEintNum].mask_address;
    reg_value = __raw_readl( sfr_address );
    reg_value |= (0x1 << order);
    __raw_writel( reg_value, sfr_address );

    // External Interrupt Pend Register
    order = mGpioEintInfo[gpioEintNum].pend_order;
    sfr_address = mGpioEintInfo[gpioEintNum].pend_address;
    reg_value = (0x1 << order);
    __raw_writel( reg_value, sfr_address );
}

//
void gpioDrvSetExtInterruptFilterSel(IN GpioEintNumType gpioEintNum, IN uint32_t intFilterSel)
{
#if defined(s5e9630)
    uint32_t order;
    volatile uint32_t sfr_address;
    uint32_t reg_value;
    const uint32_t flt_mask = 0x40;
    GpioPinNumType gpioPinNum;

    gpioPinNum = mGpioEintInfo[gpioEintNum].gpioPinNum;

    // Just in case user did not configure the GPIO as external interrupt function
    reg_value = gpioDrvGetConfig(gpioPinNum);
    if(reg_value != 0x0F){
	    CSP_PRINTF_ERROR("%s: gpio(0x%x) is not interrupt mode(current: 0x%x) \n", __FUNCTION__, (int)gpioEintNum, (int)reg_value);
        return;
    }

    // External Interrupt Filter Control Register
    order = mGpioEintInfo[gpioEintNum].flt_order;
    sfr_address = mGpioEintInfo[gpioEintNum].flt_address;

    reg_value = __raw_readl( sfr_address );
    reg_value &= ~(flt_mask << order);

    switch ((IntFilterSel)intFilterSel) {
    case eAnalogFLT:
        reg_value |= (0x00 << order);
        break;
    case eDigitalFLT:
        reg_value |= (0x40 << order);
        break;
    default:
        CSP_ASSERT(0);
        break;
    }

    __raw_writel( reg_value, sfr_address );

#ifdef TEST_PRINT
	CSP_PRINTF_ERROR("%s: gpioEintNum(0x%x), sfr_address(0x%x), reg_value(0x%x) \n", __FUNCTION__, (int)gpioEintNum, (int)sfr_address, (int)reg_value);
#endif
#endif
}

uint32_t gpioDrvGetDataEINT(IN GpioEintNumType gpioEintNum)
{
    GpioPinNumType gpioPinNum;

    gpioPinNum = mGpioEintInfo[gpioEintNum].gpioPinNum;

    return gpioDrvGetData(gpioPinNum);
}

void gpioDrvClearPend(IN GpioEintNumType gpioEintNum)
{
    uint32_t order;
    volatile uint32_t sfr_address;
    uint32_t reg_value;

    order = mGpioEintInfo[gpioEintNum].pend_order;
    sfr_address = mGpioEintInfo[gpioEintNum].pend_address;
    reg_value = __raw_readl( sfr_address );

    if( reg_value & (0x1 << order)) {
        reg_value = (0x1 << order);
        __raw_writel( reg_value, sfr_address );
    }
}

uint32_t gpioDrvIsPending(IN GpioEintNumType gpioEintNum)
{
    uint32_t order;
    volatile uint32_t sfr_address;
    uint32_t reg_value;

    order = mGpioEintInfo[gpioEintNum].pend_order;
    sfr_address = mGpioEintInfo[gpioEintNum].pend_address;
    reg_value = __raw_readl( sfr_address );

    if( reg_value & (0x1 << order))
        return 1;
    else
        return 0;
}

void gpioDrvDeinit(void)
{
#if defined(s5e9610)
    NVIC_DisableIRQ(GPIO_CHUB_IRQn);
#elif defined(s5e9630)
    for(int i=0; i<21; i++)
    {
        NVIC_DisableIRQ(EXT_INTH0_0 + i);
    }
#else
#endif
}

