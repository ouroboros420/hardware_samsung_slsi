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
    GPH0_0 = GPIO_CHUB_START_PIN_NUM,
#define     GPH0_0_USI00_CHUB_USI_RXD_SPICLK_SCL    0x2
#define     GPH0_0_SPI_CHUB_MULTI_SLV_SPICLK        0x4
#define     GPH0_0_NWEINT                           0xF
    GPH0_1,
#define     GPH0_1_USI00_CHUB_USI_TXD_SPIDO_SDA     0x2
#define     GPH0_1_SPI_CHUB_MULTI_SLV_SPIDO         0x4
#define     GPH0_1_NWEINT                           0xF
    GPH0_2,
#define     GPH0_2_USI00_CHUB_USI_RTS_SPIDI_NA      0x2
#define     GPH0_2_SPI_I2C_CHUB00_SCL               0x3
#define     GPH0_2_SPI_CHUB_MULTI_SLV_SPIDI         0x4
#define     GPH0_2_NWEINT                           0xF
    GPH0_3,
#define     GPH0_3_USI00_CHUB_USI_CTS_SPICS_NA      0x2
#define     GPH0_3_SPI_I2C_CHUB00_SDA               0x3
#define     GPH0_3_SPI_CHUB_MULTI_SLV_SPINCS0       0x4
#define     GPH0_3_NWEINT                           0xF
    GPH1_0,
#define     GPH1_0_USI01_CHUB_USI_RXD_SPICLK_SCL    0x2
#define     GPH1_0_SPI_CHUB_MULTI_SLV_SPINCS1       0x4
#define     GPH1_0_NWEINT                           0xF
    GPH1_1,
#define     GPH1_1_USI01_CHUB_USI_TXD_SPIDO_SDA     0x2
#define     GPH1_1_SPI_CHUB_MULTI_SLV_SPINCS2       0x4
#define     GPH1_1_NWEINT                           0xF
    GPH1_2,
#define     GPH1_2_USI01_CHUB_USI_RTS_SPIDI_NA      0x2
#define     GPH1_2_SPI_I2C_CHUB01_SCL               0x3
#define     GPH1_2_SPI_CHUB_MULTI_SLV_SPINCS3       0x4
#define     GPH1_2_NWEINT                           0xF
    GPH1_3,
#define     GPH1_3_USI01_CHUB_USI_CTS_SPICS_NA      0x2
#define     GPH1_3_SPI_I2C_CHUB01_SDA               0x3
#define     GPH1_3_NWEINT                           0xF

    GPH2_0,
#define     GPH2_0_USI02_CHUB_USI_RXD_SPICLK_SCL    0x2
#define     GPH2_0_NWEINT                           0xF
    GPH2_1,
#define     GPH2_1_USI02_CHUB_USI_TXD_SPIDO_SDA     0x2
#define     GPH2_1_NWEINT                           0xF
    GPH2_2,
#define     GPH2_2_USI02_CHUB_USI_RTS_SPIDI_NA      0x2
#define     GPH2_2_NWEINT                           0xF
    GPH2_3,
#define     GPH2_3_USI02_CHUB_USI_CTS_SPICS_NA      0x2
#define     GPH2_3_NWEINT                           0xF

    GPH3_0,
#define     GPH3_0_USI03_CHUB_USI_RXD_SPICLK_SCL    0x2
#define     GPH3_0_NWEINT                           0xF
    GPH3_1,
#define     GPH3_1_USI03_CHUB_USI_TXD_SPIDO_SDA     0x2
#define     GPH3_1_NWEINT                           0xF
    GPH3_2,
#define     GPH3_2_USI03_CHUB_USI_RTS_SPIDI_NA      0x2
#define     GPH3_2_NWEINT                           0xF
    GPH3_3,
#define     GPH3_3_USI03_CHUB_USI_CTS_SPICS_NA      0x2
#define     GPH3_3_NWEINT                           0xF

    GPH4_0,
#define     GPH4_0_I3C_CHUB_SCL                     0x2
#define     GPH4_0_NWEINT                           0xF
    GPH4_1,
#define     GPH4_1_I3C_CHUB_SDA                     0x2
#define     GPH4_1_NWEINT                           0xF
    GPH4_2,
#define     GPH4_2_I2C_CHUB_SCL                     0x2
#define     GPH4_2_NWEINT                           0xF
    GPH4_3,
#define     GPH4_3_I2C_CHUB_SDA                     0x2
#define     GPH4_3_NWEINT                           0xF
    GPH5_0,
#define     GPH5_0_NWEINT                           0xF
    GPH5_1,
#define     GPH5_1_NWEINT                           0xF
    GPH5_2,
#define     GPH5_2_NWEINT                           0xF
    GPH5_3,
#define     GPH5_3_NWEINT                           0xF
    GPH5_4,
#define     GPH5_4_NWEINT                           0xF

    GPIO_CHUB_MAX_PIN_NUM,
    GPM00_0 = GPIO_CHUB_MAX_PIN_NUM,
#define     GPM00_0_USI_CMGP0_RXD_SPICLK_SCL        0x2
#define     GPM00_0_SPI_CMGP_MULTI_SLV_SPICLK       0x4
    GPM00_1,
#define     GPM00_1_USI_CMGP0_TXD_SPIDO_SDA         0x2
#define     GPM00_1_SPI_CMGP_MULTI_SLV_SPIDO        0x4
    GPM01_0,
#define     GPM01_0_USI_CMGP0_RTS_SPIDI             0x2
#define     GPM01_0_I2C_CMGP0_SCL                   0x3
#define     GPM01_0_SPI_CMGP_MULTI_SLV_SPIDI        0x4
    GPM01_1,
#define     GPM01_1_USI_CMGP0_CTS_SPICS             0x2
#define     GPM01_1_I2C_CMGP0_SDA                   0x3
#define     GPM01_1_SPI_CMGP_MULTI_SLV_SPICS0       0x4
    GPM02_0,
#define     GPM02_0_USI_CMGP1_RXD_SPICLK_SCL        0x2
#define     GPM02_0_SPI_CMGP_MULTI_SLV_SPICS1       0x4
    GPM02_1,
#define     GPM02_1_USI_CMGP1_TXD_SPIDO_SDA         0x2
#define     GPM02_1_SPI_CMGP_MULTI_SLV_SPICS2       0x4
    GPM03_0,
#define     GPM03_0_USI_CMGP1_RTS_SPIDI             0x2
#define     GPM03_0_I2C_CMGP1_SCL                   0x3
#define     GPM03_0_SPI_CMGP_MULTI_SLV_SPICS3       0x4
    GPM03_1,
#define     GPM03_1_USI_CMGP1_CTS_SPICS             0x2
#define     GPM03_1_I2C_CMGP1_SDA                   0x3
#define     GPM03_1_SPI_CMGP_MULTI_SLV_SPICS4       0x4
    GPM04_0,
#define     GPM04_0_USI_CMGP2_RXD_SPICLK_SCL        0x2
    GPM04_1,
#define     GPM04_1_USI_CMGP2_TXD_SPIDO_SDA         0x2
    GPM05_0,
#define     GPM05_0_USI_CMGP2_RTS_SPIDI             0x2
#define     GPM05_0_I2C_CMGP2_SCL                   0x3
#define     GPM05_0_I3C_CMGP_SCL                    0x4
#define     GPM05_0_I3C_UFD_SCL                     0x5
    GPM05_1,
#define     GPM05_1_USI_CMGP2_CTS_SPICS             0x2
#define     GPM05_1_I2C_CMGP2_SDA                   0x3
#define     GPM05_1_I3C_CMGP_SDA                    0x4
#define     GPM05_1_I3C_UFD_SDA                     0x5
    GPM06_0,
#define     GPM06_0_USI_CMGP3_RXD_SPICLK_SCL        0x2
#define     GPM06_0_OSCCLK_MCLK_ALV                 0x5
    GPM06_1,
#define     GPM06_1_USI_CMGP3_TXD_SPIDO_SDA         0x2
    GPM07_0,
#define     GPM07_0_USI_CMGP3_RTS_SPIDI             0x2
#define     GPM07_0_I2C_CMGP3_SCL                   0x3
    GPM07_1,
#define     GPM07_1_USI_CMGP3_CTS_SPICS             0x2
#define     GPM07_1_I2C_CMGP3_SDA                   0x3
    GPM08_0,
#define     GPM08_0_USI_CMGP4_RXD_SPICLK_SCL        0x2
    GPM08_1,
#define     GPM08_1_USI_CMGP4_TXD_SPIDO_SDA         0x2
    GPM09_0,
#define     GPM09_0_USI_CMGP4_RTS_SPIDI             0x2
#define     GPM09_0_I2C_CMGP4_SCL                   0x3
    GPM09_1,
#define     GPM09_1_USI_CMGP4_CTS_SPICS             0x2
#define     GPM09_1_I2C_CMGP4_SDA                   0x3
    GPM10_0,
#define     GPM10_0_USI_CMGP5_RXD_SPICLK_SCL        0x2
    GPM10_1,
#define     GPM10_1_USI_CMGP5_TXD_SPIDO_SDA         0x2
    GPM11_0,
#define     GPM11_0_USI_CMGP5_RTS_SPIDI             0x2
#define     GPM11_0_I2C_CMGP5_SCL                   0x3
#define     GPM11_0_UART_DBGCORE_RXD                0x4
    GPM11_1,
#define     GPM11_1_USI_CMGP5_CTS_SPICS             0x2
#define     GPM11_1_I2C_CMGP5_SDA                   0x3
#define     GPM11_1_UART_DBGCORE_TXD                0x4
    GPM12_0,
#define     GPM12_0_USI_CMGP6_RXD_SPICLK_SCL        0x2
#define     GPM12_0_CP_UART1_RXD                    0x4
    GPM12_1,
#define     GPM12_1_USI_CMGP6_TXD_SPIDO_SDA         0x2
#define     GPM12_1_CP_UART1_TXD                    0x4
    GPM13_0,
#define     GPM13_0_USI_CMGP6_RTS_SPIDI             0x2
#define     GPM13_0_I2C_CMGP6_SCL                   0x3
#define     GPM13_0_UART_GNSS_RXD                   0x4
    GPM13_1,
#define     GPM13_1_USI_CMGP6_CTS_SPICS             0x2
#define     GPM13_1_I2C_CMGP6_SDA                   0x3
#define     GPM13_1_UART_GNSS_TXD                   0x4
    GPM14_0,
    GPM15_0,
    GPM16_0,
#define    GPM16_0_SPI_UFD_CLK                      0x2
    GPM17_0,
#define    GPM17_0_SPI_UFD_DO                       0x2
    GPM18_0,
#define    GPM18_0_SPI_UFD_DI                       0x2
    GPM19_0,
#define    GPM19_0_SPI_UFD_CS                       0x2
    GPM20_0,
    GPM21_0,
    GPM22_0,
    GPM23_0,
#define    GPM23_0_I2C_CMGP_SCL                    0x2
    GPM24_0,
#define    GPM24_0_I2C_CMGP_SDA                    0x2
    GPIO_MAX_PIN_NUM
} GpioPinNumType;

#define GPIO_SUPPORT_FILTERSEL         1
#define GPIO_SUPPORT_PUD_RESISTOR_SEL  1
#define GPIO_SUPPORT_PDN_CONTROL       0

#endif  // #ifndef __GPIO5515_H__
