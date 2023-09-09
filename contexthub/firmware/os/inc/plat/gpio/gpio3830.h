/*----------------------------------------------------------------------------
 * Exynos SoC  -  GPIO
 *
 * Copyright (C) 2021 Samsung Electronics Co., Ltd.
 *
 * Huiling wu <huiling.wu@samsung.com>
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

#ifndef __GPIO3830_H__
#define __GPIO3830_H__

#include <csp_common.h>
#include <csp_assert.h>
#include <csp_printf.h>

typedef enum {
    GPIO_CHUB_START_PIN_NUM = 0,
    GPIO_CHUB_MAX_PIN_NUM = GPIO_CHUB_START_PIN_NUM,

    GPM00_0 = GPIO_CHUB_MAX_PIN_NUM,
#define USI_CMGP0__RXD_SPICLK_SCL   2
    GPM01_0,
#define USI_CMGP0__TXD_SPIDo_SDA    2
    GPM02_0,
#define USI_CMGP0__RTSn_SPIDi_NA    2
    GPM03_0,
#define USI_CMGP0__CTSn_SPInCS_NA   2
    GPM04_0,
#define USI_CMGP1__RXD_SPICLK_SCL   2
    GPM05_0,
#define USI_CMGP1__TXD_SPIDo_SDA    2
    GPM06_0,
#define USI_CMGP1__RTSn_SPIDi_NA    2
    GPM07_0,
#define USI_CMGP1__CTSn_SPInCS_NA   2
    GPIO_MAX_PIN_NUM
} GpioPinNumType;

#define GPIO_SUPPORT_FILTERSEL         1
#define GPIO_SUPPORT_PUD_RESISTOR_SEL  1
#define GPIO_SUPPORT_PDN_CONTROL       0

#endif  // #ifndef __GPIO3830_H__
