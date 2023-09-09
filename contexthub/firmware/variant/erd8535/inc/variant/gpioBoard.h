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
    // ACC_SENSOR
    {GPH00_0, USI_CHUB0_RXD_SPICLK_SCL,  GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_HIGH, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    {GPH00_1, USI_CHUB0_TXD_SPIDO_SDA,   GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_HIGH, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    {GPH00_2, USI_CHUB0_RTS_SPIDI,    GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_HIGH, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    {GPH00_3, USI_CHUB0_CTS_SPICS,    GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_HIGH, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},

    // MAG_SENSOR
    {GPH00_4, USI_CHUB1_RXD_SPICLK_SCL, GPIO_DAT_HIGH, GPIO_PUD_PULLUP, GPIO_CONPDN_OUTPUT_HIGH, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    {GPH00_5, USI_CHUB1_TXD_SPIDO_SDA, GPIO_DAT_HIGH, GPIO_PUD_PULLUP, GPIO_CONPDN_OUTPUT_HIGH, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},

    // ALS/P_SENSOR
    {GPH00_6, I2C_CHUB1_SCL, GPIO_DAT_HIGH, GPIO_PUD_PULLUP, GPIO_CONPDN_OUTPUT_HIGH, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    {GPH00_7, I2C_CHUB1_SDA, GPIO_DAT_HIGH, GPIO_PUD_PULLUP, GPIO_CONPDN_OUTPUT_HIGH, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},

    // ACC_SENSOR1_INT1
    {GPM13_0, EINT_CMGP2CHUB1, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    // P-SENSOR_INT
    {GPM12_0, EINT_CMGP2CHUB0, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},

    // ACC_SENSOR1_INT1
    {GPE01_0, NWEINT_GPE1, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    // P-SENSOR_INT
    {GPE00_0, NWEINT_GPE0, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
};
#endif

