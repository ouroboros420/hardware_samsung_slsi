/*----------------------------------------------------------------------------
 *      EXYNOS  -  GPIO
 *----------------------------------------------------------------------------
 *      Name:    gpioBoard.h
 *      Purpose: To expose GPIO Board APIs
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

#ifndef __GPIO_BOARD_H__
#define __GPIO_BOARD_H__

#include <csp_common.h>
#include <plat/gpio/gpio.h>
#include <plat/cmgp/cmgp.h>

void gpioBoardInit(void);

// Upon POR reset, initial settings of GPIO Pads are defined here.
static const uint8_t mGpioPortInitValue[][7] = {
    // I2C : LIGHT, MAG
    {GPM04_0, GPM04_0_USI_CMGP01_RXD_SPICLK_SCL, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_HIGH, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    {GPM05_0, GPM05_0_USI_CMGP01_TXD_SPIDO_SDA, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_HIGH, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},

    // I2C : HRM
    {GPM06_0, GPM06_0_I2C_CMGP01_SCL, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_HIGH, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    {GPM07_0, GPM07_0_I2C_CMGP01_SDA, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_HIGH, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},

    // SPI : 6-AXIS
    {GPM08_0, GPM08_0_USI_CMGP02_RXD_SPICLK_SCL, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_HIGH, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    {GPM09_0, GPM09_0_USI_CMGP02_TXD_SPIDO_SDA, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_HIGH, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    {GPM10_0, GPM10_0_USI_CMGP02_RTS_SPIDI, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_HIGH, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    {GPM11_0, GPM11_0_USI_CMGP02_CTS_SPICS, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_HIGH, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},

    // EINT - HRM_INT
    {GPM17_0, GPIO_CON_EINT, GPIO_DAT_HIGH, GPIO_PUD_PULLUP, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},

    // OUTPUT : SENSOR_POWER_EN
    {GPM18_0, GPIO_CON_OUTPUT, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    // OUTPUT : HRM_EN
    {GPM19_0, GPIO_CON_OUTPUT, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},

    /*  Interrupt : */
    // 6-Axis Sensor Int
    {GPM22_0, GPIO_CON_EINT, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    {GPM23_0, GPIO_CON_EINT, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
};
#endif

