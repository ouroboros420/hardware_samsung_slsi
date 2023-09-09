/*----------------------------------------------------------------------------
 *      NEO9815  -  GPIO
 *----------------------------------------------------------------------------
 *      Name:    gpioNEO9815.c
 *      Purpose: Initialize GPIO Pads for POR reset and wakeup from system power mode
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

#include <gpioNeo9815.h>
#include <plat/gpio/gpio.h>
#include <plat/cmgp/cmgp.h>
#include <csp_common.h>

#if defined(SEOS)
#include <cmsis.h>
#endif

#define VDD_EN

// Upon POR reset, initial settings of GPIO Pads are defined here.
static const uint8_t mGpioPortInitValue[][7] = {
    // BMI160 Accelation-Gyro Sensor:: SPI_CHUB0 @ XUSI_CHUB0 as SPI
    /* SPI CLK, CS, MISO, MOSI*/
    {GPIO_H0_0, GPH0_0_USI00_CHUB_USI_RXD_SPICLK_SCL, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    {GPIO_H0_1, GPH0_1_USI00_CHUB_USI_TXD_SPIDO_SDA, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    {GPIO_H0_2, GPH0_2_USI00_CHUB_USI_RTS_SPIDI_NA, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    {GPIO_H0_3, GPH0_3_USI00_CHUB_USI_CTS_SPICS_NA, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    /*  Interrupt : */
    //Interrupt A_INT_1 @ CHUB
    {GPIO_H2_0, GPH2_0_NWEINT, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    //Interrupt A_INT_2 @ CHUB
    {GPIO_H2_1, GPH2_1_NWEINT, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},

    // AK09918 MAGNETIC Sensor:: I2C2_CHUB @ XUSI_CHUB1 as I2C
    // I2C SCL / SDA
    {GPIO_H0_4, GPH0_4_USI01_CHUB_USI_RXD_SPICLK_SCL, GPIO_DAT_HIGH, GPIO_PUD_PULLUP_ENABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    {GPIO_H0_5, GPH0_5_USI01_CHUB_USI_TXD_SPIDO_SDA, GPIO_DAT_HIGH, GPIO_PUD_PULLUP_ENABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},

    // BMI280 PRESSURE:: I2C3_CHUB @ XUSI_CHUB1 as I2C
    /* I2C SCL / SDA */
    {GPIO_H0_6, GPH0_6_I2C_CHUB01_SCL, GPIO_DAT_HIGH, GPIO_PUD_PULLUP_ENABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    {GPIO_H0_7, GPH0_7_I2C_CHUB01_SDA, GPIO_DAT_HIGH, GPIO_PUD_PULLUP_ENABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    /*  Interrupt : */

    // RPR-0521RS Light Sensor:: I2C4_CHUB @ XUSI_CHUB2 as I2C
	/* I2C SCL / SDA */
	{GPIO_H1_0, GPH1_0_USI02_CHUB_USI_RXD_SPICLK_SCL, GPIO_DAT_HIGH, GPIO_PUD_PULLUP_ENABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
	{GPIO_H1_1, GPH1_1_USI02_CHUB_USI_TXD_SPIDO_SDA, GPIO_DAT_HIGH, GPIO_PUD_PULLUP_ENABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
	/*	Interrupt : */
	{GPIO_H2_2, GPH2_2_NWEINT, GPIO_DAT_HIGH, GPIO_PUD_PULLUP_ENABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},

#ifdef VDD_EN
  /* VDD_Sensor_EN */
	{GPIO_H2_3, 1, GPIO_DAT_HIGH, GPIO_PUD_PULLUP_ENABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
#endif
};

// Initialize GPIO Pads to meet erd9630 board requirement
void gpioBoardInit(void)
{
    uint32_t i, size;

    size = sizeof(mGpioPortInitValue) / sizeof(mGpioPortInitValue[0]);
	CSP_PRINTF_ERROR(" %s: size(%d) \n",__FUNCTION__,(int)size);

    for (i = 0; i < size; i++) {
        gpioConfig((GpioPinNumType) mGpioPortInitValue[i][0],
                   (uint32_t) mGpioPortInitValue[i][1]);
        gpioSetData((GpioPinNumType) mGpioPortInitValue[i][0],
                    (uint32_t) mGpioPortInitValue[i][2]);
        gpioSetPud((GpioPinNumType) mGpioPortInitValue[i][0],
                   (uint32_t) mGpioPortInitValue[i][3]);
        gpioConfigPdn((GpioPinNumType) mGpioPortInitValue[i][0],
                      (uint32_t) mGpioPortInitValue[i][4]);
        gpioSetPudPdn((GpioPinNumType) mGpioPortInitValue[i][0],
                      (uint32_t) mGpioPortInitValue[i][5]);
        gpioSetDrvStrength((GpioPinNumType) mGpioPortInitValue[i][0],
                           (uint32_t) mGpioPortInitValue[i][6]);
    }
#ifdef VDD_EN
    //sleep
    msleep(100);
#endif

#ifdef TEST_PRINT
	CSP_PRINTF_ERROR(" %s GPIO_H0 Register: 0x%x \n",__FUNCTION__, (int)__raw_readl((volatile uint32_t *)(GPIO_BASE_ADDRESS + 0x00)));
	CSP_PRINTF_ERROR(" %s GPIO_H1 Register: 0x%x \n",__FUNCTION__, (int)__raw_readl((volatile uint32_t *)(GPIO_BASE_ADDRESS + 0x20)));
	CSP_PRINTF_ERROR(" %s GPIO_H2 Register: 0x%x \n",__FUNCTION__, (int)__raw_readl((volatile uint32_t *)(GPIO_BASE_ADDRESS + 0x40)));
#endif
}

