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

#ifndef __GPIO8535_H__
#define __GPIO8535_H__

#include <csp_common.h>
#include <csp_assert.h>
#include <csp_printf.h>
//p-239
typedef enum {
    GPIO_CHUB_START_PIN_NUM = 0,
    GPH00_0 = GPIO_CHUB_START_PIN_NUM,
#define     USI_CHUB0_RXD_SPICLK_SCL       0x2
    GPH00_1,
#define     USI_CHUB0_TXD_SPIDO_SDA        0x2
    GPH00_2,
#define     USI_CHUB0_RTS_SPIDI            0x2
#define     I3C_CHUB_SCL                   0x3
    GPH00_3,
#define     USI_CHUB0_CTS_SPICS            0x2
#define     I3C_CHUB_SDA                   0x3
    GPH00_4,
#define     USI_CHUB1_RXD_SPICLK_SCL       0x2
    GPH00_5,
#define     USI_CHUB1_TXD_SPIDO_SDA        0x2
    GPH00_6,
#define     USI_CHUB1_RTS_SPIDI            0x2
#define     I2C_CHUB1_SCL                  0x3
    GPH00_7,
#define     USI_CHUB1_CTS_SPICS            0x2
#define     I2C_CHUB1_SDA                  0x3
//p-248
    GPE00_0,
#define     NWEINT_GPE0                    0xF
    GPE01_0,
#define     NWEINT_GPE1                    0xF

    GPIO_CHUB_MAX_PIN_NUM,
//p-254
    GPM00_0 = GPIO_CHUB_MAX_PIN_NUM,
#define     USI_CMGP0_RXD_SPICLK_SCL       0x2
#define     USI_CHUB2_RXD_SPICLK_SCL       0x5
    GPM01_0,
#define     USI_CMGP0_TXD_SPIDO_SDA        0x2
#define     USI_CHUB2_TXD_SPIDO_SDA        0x5
    GPM02_0,
#define     USI_CMGP0_RTS_SPIDI            0x2
#define     I2C_CMGP0_SCL                  0x3
#define     I3C_CMGP0_SCL                  0x4
#define     USI_CHUB2_RTS_SPIDI            0x5
#define     I2C_CHUB2_SCL                  0x6
    GPM03_0,
#define     USI_CMGP0_CTS_SPICS            0x2
#define     I2C_CMGP0_SDA                  0x3
#define     I3C_CMGP0_SDA                  0x4
#define     USI_CHUB2_CTS_SPICS            0x5
#define     I2C_CHUB2_SDA                  0x6
    GPM04_0,
#define     USI_CMGP1_RXD_SPICLK_SCL       0x2
    GPM05_0,
#define     USI_CMGP1_TXD_SPIDO_SDA        0x2
    GPM06_0,
#define     USI_CMGP1_RTS_SPIDI            0x2
#define     I2C_CMGP1_SCL                  0x3
    GPM07_0,
#define     USI_CMGP1_CTS_SPICS            0x2
#define     I2C_CMGP1_SDA                  0x3
    GPM08_0,
#define     USI_CMGP2_RXD_SPICLK_SCL       0x2
    GPM09_0,
#define     USI_CMGP2_TXD_SPIDO_SDA        0x2
    GPM10_0,
#define     USI_CMGP2_RTS_SPIDI            0x2
#define     I2C_CMGP2_SCL                  0x3
    GPM11_0,
#define     USI_CMGP2_CTS_SPICS            0x2
#define     I2C_CMGP2_SDA                  0x3
//p-265
    GPM12_0,
#define     EINT_CMGP2CHUB0                0x2
    GPM13_0,
#define     USI_CMGP3_RXD_SPICLK_SCL       0x2
#define     EINT_CMGP2CHUB1                0x4
    GPM14_0,
#define     USI_CMGP3_TXD_SPIDO_SDA        0x2
    GPM15_0,
#define     USI_CMGP3_RTS_SPIDI            0x2
#define     I2C_CMGP3_SCL                  0x3
    GPM16_0,
#define     USI_CMGP3_CTS_SPICS            0x2
#define     I2C_CMGP3_SDA                  0x3
    GPM17_0,
#define     USI_CMGP4_RXD_SPICLK_SCL       0x2
    GPM18_0,
#define     USI_CMGP4_TXD_SPIDO_SDA        0x2
    GPM19_0,
#define     USI_CMGP4_RTS_SPIDI            0x2
#define     I2C_CMGP4_SCL                  0x3
    GPM20_0,
#define     USI_CMGP4_CTS_SPICS            0x2
#define     I2C_CMGP4_SDA                  0x3
    GPIO_MAX_PIN_NUM
} GpioPinNumType;

#define GPIO_SUPPORT_FILTERSEL         1
#define GPIO_SUPPORT_PUD_RESISTOR_SEL  1
#define GPIO_SUPPORT_PDN_CONTROL       0

#endif  // #ifndef __GPIO8535_H__
