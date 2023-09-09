/*----------------------------------------------------------------------------
 *      Exynos SoC  -  GPIO
 *----------------------------------------------------------------------------
 *      Name:    gpio9815.h
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

#ifndef __GPIO9815_H__
#define __GPIO9815_H__

#include <csp_common.h>
#include <csp_assert.h>
#include <csp_printf.h>

typedef enum {
    GPIO_CHUB_START_PIN_NUM = 0,
    GPH0_0 = GPIO_CHUB_START_PIN_NUM,
#define     GPH0_0_USI00_CHUB_USI_RXD_SPICLK_SCL    0x2
#define     GPH0_0_NWEINT                           0xF
    GPH0_1,
#define     GPH0_1_USI00_CHUB_USI_TXD_SPIDO_SDA     0x2
#define     GPH0_1_NWEINT                           0xF
    GPH0_2,
#define     GPH0_2_USI00_CHUB_USI_RTS_SPIDI_NA      0x2
#define     GPH0_2_I2C_CHUB00_SCL                   0x3
#define     GPH0_2_NWEINT                           0xF
    GPH0_3,
#define     GPH0_3_USI00_CHUB_USI_CTS_SPICS_NA      0x2
#define     GPH0_3_I2C_CHUB00_SDA                   0x3
#define     GPH0_3_NWEINT                           0xF
    GPH1_0,
#define     GPH1_0_USI01_CHUB_USI_RXD_SPICLK_SCL    0x2
#define     GPH1_0_NWEINT                           0xF
    GPH1_1,
#define     GPH1_1_USI01_CHUB_USI_TXD_SPIDO_SDA     0x2
#define     GPH1_1_NWEINT                           0xF
    GPH1_2,
#define     GPH1_2_USI01_CHUB_USI_RTS_SPIDI_NA      0x2
#define     GPH1_2_I2C_CHUB01_SCL                   0x3
#define     GPH1_2_NWEINT                           0xF
    GPH1_3,
#define     GPH1_3_USI01_CHUB_USI_CTS_SPICS_NA      0x2
#define     GPH1_3_I2C_CHUB01_SDA                   0x3
#define     GPH1_3_NWEINT                           0xF

    GPH2_0,
#define     GPH2_0_USI02_CHUB_USI_RXD_SPICLK_SCL    0x2
#define     GPH2_0_NWEINT                           0xF
    GPH2_1,
#define     GPH2_1_USI02_CHUB_USI_TXD_SPIDO_SDA     0x2
#define     GPH2_1_NWEINT                           0xF
    GPH2_2,
#define     GPH2_2_USI02_CHUB_USI_RTS_SPIDI_NA      0x2
#define     GPH2_2_I2C_CHUB02_SCL                   0x3
#define     GPH2_2_NWEINT                           0xF
    GPH2_3,
#define     GPH2_3_USI02_CHUB_USI_CTS_SPICS_NA      0x2
#define     GPH2_3_I2C_CHUB02_SDA                   0x3
#define     GPH2_3_NWEINT                           0xF

    GPH3_0,
#define     GPH3_0_NWEINT                           0xF
    GPH3_1,
#define     GPH3_1_NWEINT                           0xF
    GPH3_2,
#define     GPH3_2_NWEINT                           0xF
    GPH3_3,
#define     GPH3_3_NWEINT                           0xF
    GPH3_4,
#define     GPH3_4_NWEINT                           0xF
    GPIO_CHUB_MAX_PIN_NUM,
    GPIO_MAX_PIN_NUM = GPIO_CHUB_MAX_PIN_NUM
} GpioPinNumType;

#define GPIO_IRQ_START_NUM    EXT_INTH0_0
#define GPIO_IRQ_END_NUM      EXT_INTH3_4

#define GPIO_SUPPORT_FILTERSEL         1
#define GPIO_SUPPORT_PUD_RESISTOR_SEL  1
#define GPIO_SUPPORT_PDN_CONTROL       0

#endif

