/*----------------------------------------------------------------------------
 *      Exynos  -  GPIO
 *----------------------------------------------------------------------------
 *      Name:    gpio.h
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

#include <plat/gpio/gpio.h>
#include <plat/cmgp/cmgp.h>

#define VDD_EN

// Upon POR reset, initial settings of GPIO Pads are defined here.
static const uint8_t mGpioPortInitValue[][7] = {
    // BMI160 Accelation-Gyro Sensor:: SPI_CHUB0 @ XUSI_CHUB0 as SPI
    /* SPI CLK, CS, MISO, MOSI*/
    {GPH0_0, GPH0_0_USI00_CHUB_USI_RXD_SPICLK_SCL, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    {GPH0_1, GPH0_1_USI00_CHUB_USI_TXD_SPIDO_SDA, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    {GPH0_2, GPH0_2_USI00_CHUB_USI_RTS_SPIDI_NA, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    {GPH0_3, GPH0_3_USI00_CHUB_USI_CTS_SPICS_NA, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    /*  Interrupt : */
    //Interrupt A_INT_1 @ CHUB
    {GPH2_0, GPIO_CON_EINT, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    //Interrupt A_INT_2 @ CHUB
    {GPH2_1, GPIO_CON_EINT, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},

    // AK09918 MAGNETIC Sensor:: I2C2_CHUB @ XUSI_CHUB1 as I2C
    // I2C SCL / SDA
    {GPH0_4, GPH0_4_USI01_CHUB_USI_RXD_SPICLK_SCL, GPIO_DAT_HIGH, GPIO_PUD_PULLUP, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    {GPH0_5, GPH0_5_USI01_CHUB_USI_TXD_SPIDO_SDA, GPIO_DAT_HIGH, GPIO_PUD_PULLUP, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},

    // BMI280 PRESSURE:: I2C3_CHUB @ XUSI_CHUB1 as I2C
    /* I2C SCL / SDA */
    {GPH0_6, GPH0_6_I2C_CHUB01_SCL, GPIO_DAT_HIGH, GPIO_PUD_PULLUP, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    {GPH0_7, GPH0_7_I2C_CHUB01_SDA, GPIO_DAT_HIGH, GPIO_PUD_PULLUP, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    /*  Interrupt : */

    // RPR-0521RS Light Sensor:: I2C4_CHUB @ XUSI_CHUB2 as I2C
	/* I2C SCL / SDA */
	{GPH1_0, GPH1_0_USI02_CHUB_USI_RXD_SPICLK_SCL, GPIO_DAT_HIGH, GPIO_PUD_PULLUP, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
	{GPH1_1, GPH1_1_USI02_CHUB_USI_TXD_SPIDO_SDA, GPIO_DAT_HIGH, GPIO_PUD_PULLUP, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
	/*	Interrupt : */
	{GPH2_2, GPIO_CON_EINT, GPIO_DAT_HIGH, GPIO_PUD_PULLUP, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},

#ifdef VDD_EN
  /* VDD_Sensor_EN */
	{GPH2_3, GPIO_CON_OUTPUT, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
#endif
};

#define USE_GPIO_PDN

void gpioBoardInit(void);

#endif

