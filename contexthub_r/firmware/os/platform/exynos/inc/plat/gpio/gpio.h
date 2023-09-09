/*----------------------------------------------------------------------------
 *      Exynos SoC  -  GPIO
 *----------------------------------------------------------------------------
 *      Name:    gpio.h
 *      Purpose: To expose GPIO APIs and define macros
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

#ifndef __GPIO_H__
#define __GPIO_H__

#include <csp_common.h>
#include <csp_assert.h>
#include <csp_printf.h>
#if defined(SEOS)
#include <gpio.h>
#endif

// CON register
#define     GPIO_CON_INPUT              0x0     //input function setting
#define     GPIO_CON_OUTPUT             0x1     //output function setting
#define     GPIO_CON_FUNC_1             0x2     //function 1
#define     GPIO_CON_FUNC_2             0x3     //function 2
#define     GPIO_CON_FUNC_3             0x4     //function 3
#define     GPIO_CON_FUNC_4             0x5     //function 4
#define     GPIO_CON_FUNC_5             0x6     //function 5
#define     GPIO_CON_FUNC_6             0x7     //function 6
#define     GPIO_CON_EINT               0xF     //function 7

#define     PIN_0                       0
#define     PIN_1                       1
#define     PIN_2                       2
#define     PIN_3                       3
#define     PIN_4                       4
#define     PIN_5                       5
#define     PIN_6                       6
#define     PIN_7                       7
#define     PIN_8                       8

#define     CON_0                       0
#define     CON_1                       1

// DAT register
#define     GPIO_DAT_LOW                0x0     //low level ouput
#define     GPIO_DAT_HIGH               0x1     //high lvel output

// PUD register
#define     GPIO_PUD_DISABLE            0x0     //pull-up & pull-down disable
#define     GPIO_PUD_PULLDOWN           0x1     //pull-down enable
#define     GPIO_PUD_RESERVED           0x2     //Reserved
#define     GPIO_PUD_PULLUP             0x3     //pull-up enable

// DRV register
#define     GPIO_DRV_1X                 0x0     //drive strength(noraml)
#define     GPIO_DRV_2X                 0x1     //drive strength(2 times)
#define     GPIO_DRV_3X                 0x2     //drive strength(3 times)
#define     GPIO_DRV_4X                 0x3     //drive strength(4 times)
#if defined(SEOS)
#define     GPIO_SPEED_LOW              GPIO_DRV_1X
#define     GPIO_SPEED_MEDIUM           GPIO_DRV_2X
#define     GPIO_SPEED_FAST             GPIO_DRV_3X
#define     GPIO_SPEED_HIGH             GPIO_DRV_4X
#endif

// CONPDN register
#define     GPIO_CONPDN_OUTPUT_LOW      0x0     //output low level in sleep mode
#define     GPIO_CONPDN_OUTPUT_HIGH     0x1     //output high level in sleep mode
#define     GPIO_CONPDN_INPUT           0x2     //inout configuration in sleep mode
#define     GPIO_CONPDN_PREVIOUS_STATE  0x3     //keep previous status of the GPIO

// PUDPDN register
#define     GPIO_PUDPDN_DISABLE         0x0     //pull-up & pull-down disable in sleep mode
#define     GPIO_PUDPDN_PULLDOWN_ENABLE 0x1     //pull-down enable in sleep mode
#define     GPIO_PUDPDN_RESERVED        0x2     //Reserved
#define     GPIO_PUDPDN_PULLUP_ENABLE   0x3     //pull-up enable in sleep mode

#include CSP_HEADER(gpio)

typedef enum {
    Low_Level    = (uint32_t) 0x0,
    High_Level   = (uint32_t) 0x1,
    Falling_Edge = (uint32_t) 0x2,
    Rising_Edge  = (uint32_t) 0x3,
    Both_Edge    = (uint32_t) 0x4
} IntTriggerType;

typedef enum {
    eDisFLT      = (uint32_t) 0x0,
    eEnFLT       = (uint32_t) 0x1,
} IntFilterEnType;

typedef enum {
    eNoFLT       = (uint32_t) 0x0,
    eDelayFLT    = (uint32_t) 0x1,
    eDigitalFLT  = (uint32_t) 0x2,
} IntFilterSel;

struct Gpio {
    uint8_t  pin;
    uint8_t  index;
    uint8_t  eintEn;
    uint8_t  irqNum;
    uint32_t conBase;
    uint32_t eintBase;
    uint32_t eintFltcon;
    void (*callback)(void);
};

// Driver APIs
void gpioInit(void);
void gpioSetFunction(GpioPinNumType gpioPinNum, uint32_t gpioFunction);
void gpioSetPud(GpioPinNumType gpioPinNum, uint32_t gpioPudValue);
void gpioSetDrvStrength(GpioPinNumType gpioPinNum, uint32_t gpioDrvValue);
void gpioSetData(GpioPinNumType gpioPinNum, uint32_t gpioDataValue);
uint32_t gpioGetData(GpioPinNumType gpioPinNum);
uint32_t gpioGetConfig(GpioPinNumType gpioPinNum);
void gpioConfigPdn(GpioPinNumType gpioPinNum, uint32_t gpioFunction);
void gpioSetPudPdn(GpioPinNumType gpioPinNum, uint32_t gpioPudValue);
void gpioSetIntFilter(GpioPinNumType gpioPinNum, IntFilterSel filterType, uint32_t filterWidth);
void gpioEnableInterrupt(GpioPinNumType gpioPinNum, uint32_t en);
void gpioClearPending(GpioPinNumType gpioPinNum);
uint32_t gpioIsPending(GpioPinNumType gpioPinNum);
void gpioSetExtInterrupt(GpioPinNumType gpioPinNum, IntTriggerType intTrigger, IntFilterSel   intFilter,
                         uint32_t       filterWidth, void (*callbackFunction) (void));
void gpioUnsetExtInterrupt(GpioPinNumType gpioPinNum);
void gpioDeinit(void);

#if defined(SFR_SAVE_RESTORE)
void gpioSaveState(void);
void gpioRestoreState(void);
#endif
#if defined(SEOS)
void gpioSetInterrupt(struct Gpio* __restrict gpio, IntTriggerType intTrigger, IntFilterSel   intFilter,
                         uint32_t       filterWidth, void (*callbackFunction) (void));
#endif

void gpio_IRQHandler(void);
void gpioCmgpIRQHandler(GpioPinNumType gpioPinNum);

#endif
