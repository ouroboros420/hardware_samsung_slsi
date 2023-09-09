/*----------------------------------------------------------------------------
 * Exynos SoC  -  GPIO
 *
 * Copyright (C) 2020 Samsung Electronics Co., Ltd.
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

#ifndef __GPIO5515_H__
#define __GPIO5515_H__

#include <csp_common.h>
#include <csp_assert.h>
#include <csp_printf.h>

typedef enum {
    GPIO_CHUB_START_PIN_NUM = 0,
    GPM00_0 = GPIO_CHUB_START_PIN_NUM,
#define     GPM00_0_USI_CMGP00_RXD_SPICLK_SCL       0x2
#define     GPM00_0_USI_CHUB00_RXD_SPICLK_SCL       0x4
    GPM01_0,
#define     GPM01_0_USI_CMGP00_TXD_SPIDO_SDA        0x2
#define     GPM01_0_USI_CHUB00_TXD_SPIDO_SDA        0x4
    GPM02_0,
#define     GPM02_0_USI_CMGP00_RTS_SPIDI            0x2
#define     GPM02_0_I2C_CMGP00_SCL                  0x3
#define     GPM02_0_USI_CHUB00_RTS_SPIDI            0x4
#define     GPM02_0_I2C_CHUB00_SCL                  0x5
    GPM03_0,
#define     GPM03_0_USI_CMGP00_CTS_SPICS            0x2
#define     GPM03_0_I2C_CMGP00_SDA                  0x3
#define     GPM03_0_USI_CHUB00_CTS_SPICS            0x4
#define     GPM03_0_I2C_CHUB00_SDA                  0x5
    GPM04_0,
#define     GPM04_0_USI_CMGP01_RXD_SPICLK_SCL       0x2
#define     GPM04_0_I2C_CHUB01_SCL                  0x4
    GPM05_0,
#define     GPM05_0_USI_CMGP01_TXD_SPIDO_SDA        0x2
#define     GPM05_0_I2C_CHUB01_SDA                  0x4
    GPM06_0,
#define     GPM06_0_USI_CMGP01_RTS_SPIDI            0x2
#define     GPM06_0_I2C_CMGP01_SCL                  0x3
    GPM07_0,
#define     GPM07_0_USI_CMGP01_CTS_SPICS            0x2
#define     GPM07_0_I2C_CMGP01_SDA                  0x3
    GPM08_0,
#define     GPM08_0_USI_CMGP02_RXD_SPICLK_SCL       0x2
    GPM09_0,
#define     GPM09_0_USI_CMGP02_TXD_SPIDO_SDA        0x2
    GPM10_0,
#define     GPM10_0_USI_CMGP02_RTS_SPIDI            0x2
#define     GPM10_0_I2C_CMGP02_SCL                  0x3
    GPM11_0,
#define     GPM11_0_USI_CMGP02_CTS_SPICS            0x2
#define     GPM11_0_I2C_CMGP02_SDA                  0x3
    GPM12_0,
#define     GPM12_0_USI_CMGP03_RXD_SPICLK_SCL       0x2
    GPM13_0,
#define     GPM13_0_USI_CMGP03_TXD_SPIDO_SDA        0x2
    GPM14_0,
#define     GPM14_0_USI_CMGP03_RTS_SPIDI            0x2
#define     GPM14_0_I2C_CMGP03_SCL                  0x3
    GPM15_0,
#define     GPM15_0_USI_CMGP03_CTS_SPICS            0x2
#define     GPM15_0_I2C_CMGP03_SDA                  0x3
    GPM16_0,
#define     GPM16_0_I2C_CMGP04_SCL                  0x2
    GPM17_0,
#define     GPM17_0_I2C_CMGP04_SDA                  0x2
    GPM18_0,
#define     GPM18_0_I2C_CMGP05_SCL                  0x2
    GPM19_0,
#define     GPM19_0_I2C_CMGP05_SDA                  0x2
    GPM20_0,
#define     GPM20_0_I2C_CMGP06_SCL                  0x2
    GPM21_0,
#define     GPM21_0_I2C_CMGP06_SDA                  0x2
    GPM22_0,
    GPM23_0,
    GPM24_0,
    GPM25_0,
    GPIO_CHUB_MAX_PIN_NUM,
    GPIO_MAX_PIN_NUM = GPIO_CHUB_MAX_PIN_NUM
} GpioPinNumType;

#define GPIO_SUPPORT_FILTERSEL         1
#define GPIO_SUPPORT_PUD_RESISTOR_SEL  1
#define GPIO_SUPPORT_PDN_CONTROL       0

#endif  // #ifndef __GPIO5515_H__
