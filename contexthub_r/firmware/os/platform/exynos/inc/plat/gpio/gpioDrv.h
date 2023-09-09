/*----------------------------------------------------------------------------
 *      Exynos SoC  -  GPIO
 *----------------------------------------------------------------------------
 *      Name:    gpioDrv.h
 *      Purpose: To expose GPIO driver functions
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

#ifndef __GPIO_DRV_H__
#define __GPIO_DRV_H__

#include <csp_common.h>
#include <plat/gpio/gpio.h>
#include CSP_HEADER(gpio)

void gpioDrvInit(void);
void gpioDrvConfig(IN GpioPinNumType gpioPinNum, IN uint32_t gpioFunction);
uint32_t gpioDrvGetConfig(IN GpioPinNumType gpioPinNum);
void gpioDrvSetData(IN GpioPinNumType gpioPinNum, IN uint32_t gpioDataValue);
uint32_t gpioDrvGetData(IN GpioPinNumType gpioPinNum);
void gpioDrvSetPud(IN GpioPinNumType gpioPinNum, IN uint32_t gpioPudValue);
void gpioDrvSetDrvStrength(IN GpioPinNumType gpioPinNum, IN uint32_t gpioDrvValue);
void gpioDrvConfigPdn(IN GpioPinNumType gpioPinNum, IN uint32_t gpioFunction);
void gpioDrvSetPudPdn(IN GpioPinNumType gpioPinNum, IN uint32_t gpioPudValue);
void gpioDrvSetExtInterrupt(IN GpioEintNumType gpioEintNum, IN IntTriggerType intTrigger, IN IntFilterType intFilter, IN uint32_t intFilterWidth, IN void (*callbackFunction)(uint32_t));
void gpioDrvUnsetExtInterrupt(IN GpioEintNumType gpioEintNum);
void gpioDrvSetExtInterruptFilterSel(IN GpioEintNumType gpioEintNum, IN uint32_t intFilterSel);
uint32_t gpioDrvGetDataEINT(IN GpioEintNumType gpioEintNum);
uint32_t gpioDrvCheckEINT(IN GpioEintNumType gpioEintNum);
void gpioDrvDeinit(void);
void gpioDrvClearPend(IN GpioEintNumType gpioEintNum);
uint32_t gpioDrvIsPending(IN GpioEintNumType gpioEintNum);

#endif

