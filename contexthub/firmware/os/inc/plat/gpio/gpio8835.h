/*----------------------------------------------------------------------------
 * Exynos SoC  -  GPIO
 *
 * Copyright (C) 2022 Samsung Electronics Co., Ltd.
 *
 * Shinkyu Park <shinkyu.park@samsung.com>
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

#ifndef __GPIO8835_H__
#define __GPIO8835_H__

#include <csp_common.h>
#include <csp_assert.h>
#include <csp_printf.h>

#define GPIO_INPUT  0x0
#define GPIO_OUTPUT 0x1
#define GPIO_NWEINT 0xF

typedef enum {
    GPIO_CHUB_START_PIN_NUM = 0,
    GPH0_0 = GPIO_CHUB_START_PIN_NUM,
#define USI_CHUB0__RXD_SPICLK_SCL       0x2
    GPH0_1,
#define USI_CHUB0__TXD_SPIDo_SDA        0x2
    GPH0_2,
#define USI_CHUB0__RTSn_SPIDi_NA        0x2
    GPH0_3,
#define USI_CHUB0__CTSn_SPInCS_NA       0x2
    GPH0_4,
#define USI_CHUB1__RXD_SPICLK_SCL       0x2
    GPH0_5,
#define USI_CHUB1__TXD_SPIDo_SDA        0x2
    GPH0_6,
#define USI_CHUB1__RTSn_SPIDi_NA        0x2
#define I2C_CHUB1__SCL                  0x3
    GPH0_7,
#define USI_CHUB1__CTSn_SPInCS_NA       0x2
#define I2C_CHUB1__SDA                  0x3
    GPH1_0,
    GPH1_1,
    GPH1_2,
    GPH1_3,
    GPE0_0,
    GPE1_0,
    GPIO_CHUB_MAX_PIN_NUM,
    GPM00_0 = GPIO_CHUB_MAX_PIN_NUM,
    GPM01_0,
    GPM02_0,
    GPM03_0,
    GPM04_0,
    GPM05_0,
    GPM06_0,
    GPM07_0,
    GPM08_0,
    GPM09_0,
    GPM10_0,
    GPM11_0,
    GPM12_0,
#define EINT_CMGP2CHUB0__ALV            0x2
    GPM13_0,
#define USI_CMGP3__RXD_SPICLK_SCL       0x2
#define EINT_CMGP2CHUB1__ALV            0x4
    GPM14_0,
    GPM15_0,
    GPM16_0,
    GPM17_0,
    GPM18_0,
    GPM19_0,
    GPM20_0,
    GPM21_0,
    GPM22_0,
    GPM23_0,
    GPM24_0,
    GPM25_0,
    GPM26_0,
    GPM27_0,
    GPM28_0,
    GPC08_0,
    GPC09_0,
    GPC10_0,
    GPC11_0,
    GPC12_0,
    GPC13_0,
    GPC14_0,
    GPIO_MAX_PIN_NUM
} GpioPinNumType;

#define GPIO_SUPPORT_FILTERSEL         1
#define GPIO_SUPPORT_PUD_RESISTOR_SEL  1
#define GPIO_SUPPORT_PDN_CONTROL       0

#endif  // #ifndef __GPIO8835_H__
