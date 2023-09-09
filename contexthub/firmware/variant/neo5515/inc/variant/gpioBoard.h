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
    // SPI : ACCEL/GYRP
    {GPM00_0, GPM00_0_USI_CMGP00_RXD_SPICLK_SCL, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_HIGH, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    {GPM01_0, GPM01_0_USI_CMGP00_TXD_SPIDO_SDA, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_HIGH, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    {GPM02_0, GPM02_0_USI_CMGP00_RTS_SPIDI, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_HIGH, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    {GPM03_0, GPM03_0_USI_CMGP00_CTS_SPICS, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_HIGH, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},

    // I2C : BARO
    {GPM04_0, GPM04_0_USI_CMGP01_RXD_SPICLK_SCL, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_HIGH, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    {GPM05_0, GPM05_0_USI_CMGP01_TXD_SPIDO_SDA, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_HIGH, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},

    // I2C : HRM
    {GPM06_0, GPM06_0_I2C_CMGP01_SCL, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_HIGH, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    {GPM07_0, GPM07_0_I2C_CMGP01_SDA, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_HIGH, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},

    // I2C : LIGHT, MAG
    {GPM14_0, GPM14_0_I2C_CMGP03_SCL, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_HIGH, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    {GPM15_0, GPM15_0_I2C_CMGP03_SDA, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_HIGH, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},

    // EINT - BARO_INT
    {GPM20_0, GPIO_CON_EINT, GPIO_DAT_HIGH, GPIO_PUD_PULLUP, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    // EINT - HRM_INT
    {GPM21_0, GPIO_CON_EINT, GPIO_DAT_HIGH, GPIO_PUD_PULLUP, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},

    /*  Interrupt : */
    // 6-Axis Sensor Int
    {GPM22_0, GPIO_CON_EINT, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    {GPM23_0, GPIO_CON_EINT, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},

    // OUTPUT : SENSOR_POWER_EN
    {GPM24_0, GPIO_CON_OUTPUT, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    // OUTPUT : HRM_EN
};
#endif

