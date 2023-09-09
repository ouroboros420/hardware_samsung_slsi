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

// CON register
#define     GPIO_CON_INPUT              0x0     //input function setting
#define     GPIO_CON_OUTPUT             0x1     //output function setting

// DAT register
#define     GPIO_DAT_LOW                0x0     //low level ouput
#define     GPIO_DAT_HIGH               0x1     //high lvel output

// PUD register
#define     GPIO_PUD_DISABLE            0x0     //pull-up & pull-down disable
#define     GPIO_PUD_PULLDOWN_ENABLE    0x1     //pull-down enable
#define     GPIO_PUD_RESERVED           0x2     //Reserved
#define     GPIO_PUD_PULLUP_ENABLE      0x3     //pull-up enable

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
    Low_Level = (uint32_t) 0x0,
    High_Level = (uint32_t) 0x1,
    Falling_Edge = (uint32_t) 0x2,
    Rising_Edge = (uint32_t) 0x3,
    Both_Edge = (uint32_t) 0x4
} IntTriggerType;

typedef enum {
    eDisFLT = (uint32_t) 0x0,
    eEnFLT = (uint32_t) 0x1,
} IntFilterType;

// Driver APIs
void gpioInit(void);
void gpioConfig(IN GpioPinNumType gpioPinNum, IN uint32_t gpioFunction);
void gpioSetData(IN GpioPinNumType gpioPinNum, IN uint32_t gpioDataValue);
uint32_t gpioGetData(IN GpioPinNumType gpioPinNum);
void gpioSetPud(IN GpioPinNumType gpioPinNum, IN uint32_t gpioPudValue);
void gpioSetDrvStrength(IN GpioPinNumType gpioPinNum, IN uint32_t gpioDrvValue);
void gpioConfigPdn(IN GpioPinNumType gpioPinNum, IN uint32_t gpioFunction);
void gpioSetPudPdn(IN GpioPinNumType gpioPinNum, IN uint32_t gpioPudValue);
void gpioSetExtInterrupt(IN GpioEintNumType gpioEintNum,
                         IN IntTriggerType intTrigger,
                         IN IntFilterType intFilter, IN uint32_t intFilterWidth,
                         void (*callbackFunction) (uint32_t));
void gpioUnsetExtInterrupt(IN GpioEintNumType gpioEintNum);
uint32_t gpioGetDataEINT(GpioEintNumType gpioEintNum);
#if defined(SFR_SAVE_RESTORE)
void gpioSaveState(void);
void gpioRestoreState(void);
#endif
void gpioDeinit(void);
bool gpioIsPending(IN GpioEintNumType gpioEintNum);
void gpioClearPending(IN GpioEintNumType gpioEintNum);

// Wrapper layer for nanohub kernel
#if defined(SEOS)

#include <gpio.h>

#endif

#endif
