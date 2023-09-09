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

#ifndef __GPIO9935_H__
#define __GPIO9935_H__

#include <csp_common.h>
#include <csp_assert.h>
#include <csp_printf.h>

#define GPIO_INPUT  0x0
#define GPIO_OUTPUT 0x1
#define GPIO_NWEINT 0xF

typedef enum {
    GPIO_CHUB_START_PIN_NUM = 0,
    GPH0_0 = GPIO_CHUB_START_PIN_NUM,
#define USI_CHUB0__RXD_SPICLK_SCL   0x2
#define SPI_CHUB_MULTI_SLV__SPICLK  0x4
    GPH0_1,
#define USI_CHUB0__TXD_SPIDo_SDA    0x2
#define SPI_CHUB_MULTI_SLV__SPIDo   0x4
    GPH0_2,
#define USI_CHUB0__RTSn_SPIDi_NA    0x2
#define SPI_I2C_CHUB0__SCL          0x3
#define SPI_CHUB_MULTI_SLV__SPIDi   0x4
    GPH0_3,
#define USI_CHUB0__CTSn_SPInCS_NA   0x2
#define SPI_I2C_CHUB0__SDA          0x3
#define SPI_CHUB_MULTI_SLV__SPInCS0 0x4
    GPH1_0,
#define USI_CHUB1__RXD_SPICLK_SCL   0x2
#define SPI_CHUB_MULTI_SLV__SPInCS1 0x4
    GPH1_1,
#define USI_CHUB1__TXD_SPIDo_SDA    0x2
#define SPI_CHUB_MULTI_SLV__SPInCS2 0x4
    GPH1_2,
#define USI_CHUB1__RTSn_SPIDi_NA    0x2
#define SPI_I2C_CHUB1__SCL          0x3
#define SPI_CHUB_MULTI_SLV__SPInCS3 0x4
    GPH1_3,
#define USI_CHUB1__CTSn_SPInCS_NA   0x2
#define SPI_I2C_CHUB1__SDA          0x3
    GPH2_0,
#define USI_CHUB2__RXD_SPICLK_SCL   0x2
    GPH2_1,
#define USI_CHUB2__TXD_SPIDo_SDA    0x2
    GPH2_2,
#define USI_CHUB2__RTSn_SPIDi_NA    0x2
    GPH2_3,
#define USI_CHUB2__CTSn_SPInCS_NA   0x2
    GPH3_0,
#define USI_CHUB3__RXD_SPICLK_SCL   0x2
    GPH3_1,
#define USI_CHUB3__TXD_SPIDo_SDA    0x2
    GPH3_2,
#define USI_CHUB3__RTSn_SPIDi_NA    0x2
    GPH3_3,
#define USI_CHUB3__CTSn_SPInCS_NA   0x2
    GPH4_0,
#define I3C_CHUB__SCL               0x2
    GPH4_1,
#define I3C_CHUB__SDA               0x2
    GPH4_2,
#define I2C_CHUB__SCL               0x2
    GPH4_3,
#define I2C_CHUB__SDA               0x2
    GPH5_0,
#define PWM_CHUB_TOUT_0             0x2
    GPH5_1,
#define PWM_CHUB_TOUT_1             0x2
    GPH5_2,
#define PWM_CHUB_TOUT_2             0x2
    GPH5_3,
#define PWM_CHUB_TOUT_3             0x2
    GPH5_4,
    GPH5_5,
    GPH5_6,
    GPB5_0,
#define I2S3_BCLK__AUD__ALV         0x2
#define I2S3_BCLK__CHUBVTS__ALV     0x3
    GPB5_1,
#define I2S3_WS__AUD__ALV           0x2
#define I2S3_WSO__CHUBVTS__ALV      0x3
    GPB5_2,
#define I2S3_SDO__AUD__ALV          0x2
#define I2S3_SDO__CHUBVTS__ALV      0x3
    GPB5_3,
#define I2S3_SDI__AUD__ALV          0x2
    GPIO_CHUB_MAX_PIN_NUM,
    GPM00_0 = GPIO_CHUB_MAX_PIN_NUM,
#define USI_CMGP0__RXD_SPICLK_SCL   0x2
#define SPI_CMGP_MULTI_SLV__SPICLK  0x4
    GPM00_1,
#define USI_CMGP0__TXD_SPIDo_SDA    0x2
#define SPI_CMGP_MULTI_SLV__SPIDo   0x4
    GPM01_0,
#define USI_CMGP0__RTSn_SPIDi_NA    0x2
#define SPI_I2C_CMGP0__SCL          0x3
#define SPI_CMGP_MULTI_SLV__SPIDi   0x4
    GPM01_1,
#define USI_CMGP0__CTSn_SPInCS_NA   0x2
#define SPI_I2C_CMGP0__SDA          0x3
#define SPI_CMGP_MULTI_SLV__SPInCS0 0x4
    GPM02_0,
#define USI_CMGP1__RXD_SPICLK_SCL   0x2
    GPM02_1,
#define USI_CMGP1__TXD_SPIDo_SDA    0x2
#define SPI_CMGP_MULTI_SLV__SPInCS2 0x4
    GPM03_0,
#define USI_CMGP1__RTSn_SPIDi_NA    0x2
#define SPI_I2C_CMGP1__SCL          0x3
#define SPI_CMGP_MULTI_SLV__SPInCS3 0x4
    GPM03_1,
#define USI_CMGP1__CTSn_SPInCS_NA   0x2
#define SPI_I2C_CMGP1__SDA          0x3
    GPM04_0,
#define USI_CMGP2__RXD_SPICLK_SCL   0x2
    GPM04_1,
#define USI_CMGP2__TXD_SPIDo_SDA    0x2
    GPM05_0,
#define USI_CMGP2__RTSn_SPIDi_NA    0x2
#define I2C_CMGP2__SCL              0x3
    GPM05_1,
#define USI_CMGP2__CTSn_SPInCS_NA   0x2
#define I2C_CMGP2__SDA              0x3
    GPM06_0,
#define USI_CMGP3__RXD_SPICLK_SCL   0x2
#define OSCCLK_MCLK__ALV            0x5
    GPM06_1,
#define USI_CMGP3__TXD_SPIDo_SDA    0x2
    GPM07_0,
#define USI_CMGP3__RTSn_SPIDi_NA    0x2
#define I2C_CMGP3__SCL              0x3
    GPM07_1,
#define USI_CMGP3__CTSn_SPInCS_NA   0x2
#define I2C_CMGP3__SDA              0x3
    GPM08_0,
#define USI_CMGP4__RXD_SPICLK_SCL   0x2
    GPM08_1,
#define USI_CMGP4__TXD_SPIDo_SDA    0x2
    GPM09_0,
#define USI_CMGP4__RTSn_SPIDi_NA    0x2
#define I2C_CMGP4__SCL              0x3
    GPM09_1,
#define USI_CMGP4__CTSn_SPInCS_NA   0x2
#define I2C_CMGP4__SDA              0x3
    GPM10_0,
#define USI_CMGP5__RXD_SPICLK_SCL   0x2
    GPM10_1,
#define USI_CMGP5__TXD_SPIDo_SDA    0x2
    GPM11_0,
#define USI_CMGP5__RTSn_SPIDi_NA    0x2
#define I2C_CMGP5__SCL              0x3
#define UART_DBGCORE_RXD__ALV       0x4
    GPM11_1,
#define USI_CMGP5__CTSn_SPInCS_NA   0x2
#define I2C_CMGP5__SDA              0x3
#define UART_DBGCORE_TXD__ALV       0x4
    GPM12_0,
#define USI_CMGP6__RXD_SPICLK_SCL   0x2
#define CP_UART1_RXD                0x4
    GPM12_1,
#define USI_CMGP6__TXD_SPIDo_SDA    0x2
#define CP_UART1_TXD                0x4
    GPM13_0,
#define USI_CMGP6__RTSn_SPIDi_NA    0x2
#define I2C_CMGP6__SCL              0x3
#define UART_GNSS_RXD__ALV          0x4
    GPM13_1,
#define USI_CMGP6__CTSn_SPInCS_NA   0x2
#define I2C_CMGP6__SDA              0x3
#define UART_GNSS_TXD__ALV          0x4
    GPM15_0,
    GPM16_0,
#define I3C_UFD__SCL__ALV           0x2
#define I2C_UFD__SCL__ALV           0x3
    GPM17_0,
#define I3C_UFD__SDA__ALV           0x2
#define I2C_UFD__SDA__ALV           0x3
    GPQ0_0,
#define UART_BT_RXD                 0x2
    GPQ0_1,
#define UART_BT_TXD                 0x2
    GPQ0_2,
#define UART_BT_RTSn                0x2
    GPQ0_3,
#define UART_BT_CTSn                0x2
    GPIO_MAX_PIN_NUM
} GpioPinNumType;

#define GPIO_SUPPORT_FILTERSEL         1
#define GPIO_SUPPORT_PUD_RESISTOR_SEL  1
#define GPIO_SUPPORT_PDN_CONTROL       0

#endif  // #ifndef __GPIO9935_H__
