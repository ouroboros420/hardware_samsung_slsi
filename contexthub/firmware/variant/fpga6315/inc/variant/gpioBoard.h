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
    /*
    // AK09918 MAGNETIC Sensor:: I2C_CHUB0 @ USI_CHUB0 as I2C
    {GPH0_0, GPH0_0_USI00_CHUB_USI_RXD_SPICLK_SCL, GPIO_DAT_HIGH, GPIO_PUD_PULLUP, GPIO_CONPDN_OUTPUT_HIGH, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    {GPH0_1, GPH0_1_USI00_CHUB_USI_TXD_SPIDO_SDA, GPIO_DAT_HIGH, GPIO_PUD_PULLUP, GPIO_CONPDN_OUTPUT_HIGH, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},

    // RPR-0521 ALS/P Sensor:: I2C_CHUB1 @ USI_I2C_CHUB0
    {GPH0_2, GPH0_2_I2C_CHUB00_SCL, GPIO_DAT_HIGH, GPIO_PUD_PULLUP, GPIO_CONPDN_OUTPUT_HIGH, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    {GPH0_3, GPH0_3_I2C_CHUB00_SDA, GPIO_DAT_HIGH, GPIO_PUD_PULLUP, GPIO_CONPDN_OUTPUT_HIGH, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    
    // BMI160 Accelation-Gyro Sensor:: SPI_CHUB1 @ USI_CHUB1 as SPI
    {GPH1_0, GPH1_0_USI01_CHUB_USI_RXD_SPICLK_SCL, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_HIGH, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    {GPH1_1, GPH1_1_USI01_CHUB_USI_TXD_SPIDO_SDA, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_HIGH, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    {GPH1_2, GPH1_2_USI01_CHUB_USI_RTS_SPIDI_NA, GPIO_DAT_HIGH, GPIO_PUD_PULLDOWN, GPIO_CONPDN_OUTPUT_HIGH, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    {GPH1_3, GPH1_3_USI01_CHUB_USI_CTS_SPICS_NA, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_HIGH, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},

    // BMI380 PRESSURE:: I2C_CHUB4 @ USI_CHUB2 as I2C
    {GPH2_2, GPH2_2_I2C_CHUB02_SCL, GPIO_DAT_HIGH, GPIO_PUD_PULLUP, GPIO_CONPDN_OUTPUT_HIGH, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    {GPH2_3, GPH2_3_I2C_CHUB02_SDA, GPIO_DAT_HIGH, GPIO_PUD_PULLUP, GPIO_CONPDN_OUTPUT_HIGH, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},

    //Interrupt A_INT_1 @ CHUB
    {GPH3_0, GPIO_CON_EINT, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    //Interrupt A_INT_2 @ CHUB
    {GPH3_1, GPIO_CON_EINT, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    //Interrupt Proximity
    {GPH3_2, GPIO_CON_EINT, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    //Interrupt Barometer
    {GPH3_3, GPIO_CON_EINT, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},

    */
};
#endif

