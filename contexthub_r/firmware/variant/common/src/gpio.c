/*----------------------------------------------------------------------------
 *      ERD  -  GPIO
 *----------------------------------------------------------------------------
 *      Name:    gpio.c
 *      Purpose: Initialize GPIO Pads for POR reset and wakeup from system power mode
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

#include <gpioBoard.h>
#include <plat/gpio/gpio.h>
#include <plat/cmgp/cmgp.h>
#include <csp_common.h>

#if defined(SEOS)
#include <cmsis.h>
#endif

// Initialize GPIO Pads to meet board requirement
void gpioBoardInit(void)
{
    uint32_t i, size;

    size = sizeof(mGpioPortInitValue) / sizeof(mGpioPortInitValue[0]);
	CSP_PRINTF_ERROR(" %s: size(%d) \n",__FUNCTION__,(int)size);

    for (i = 0; i < size; i++) {
        gpioSetFunction((GpioPinNumType) mGpioPortInitValue[i][0],
                   (uint32_t) mGpioPortInitValue[i][1]);
        gpioSetData((GpioPinNumType) mGpioPortInitValue[i][0],
                    (uint32_t) mGpioPortInitValue[i][2]);
        gpioSetPud((GpioPinNumType) mGpioPortInitValue[i][0],
                   (uint32_t) mGpioPortInitValue[i][3]);

#ifdef USE_GPIO_PDN
        gpioConfigPdn((GpioPinNumType) mGpioPortInitValue[i][0],
            (uint32_t) mGpioPortInitValue[i][4]);
#endif
        gpioSetPudPdn((GpioPinNumType) mGpioPortInitValue[i][0],
            (uint32_t) mGpioPortInitValue[i][5]);
        gpioSetDrvStrength((GpioPinNumType) mGpioPortInitValue[i][0],
                           (uint32_t) mGpioPortInitValue[i][6]);
    }
#ifdef VDD_EN
    msleep(100);
#endif
}

