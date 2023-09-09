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
    {GPH0_0, USI_CHUB0__RXD_SPICLK_SCL,  GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_HIGH, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    {GPH0_1, USI_CHUB0__TXD_SPIDo_SDA,   GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_HIGH, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    {GPH0_2, USI_CHUB0__RTSn_SPIDi_NA,   GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_HIGH, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    {GPH0_3, USI_CHUB0__CTSn_SPInCS_NA,  GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_HIGH, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},

    {GPH0_4, USI_CHUB1__RXD_SPICLK_SCL,  GPIO_DAT_HIGH, GPIO_PUD_PULLUP, GPIO_CONPDN_OUTPUT_HIGH, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    {GPH0_5, USI_CHUB1__TXD_SPIDo_SDA,   GPIO_DAT_HIGH, GPIO_PUD_PULLUP, GPIO_CONPDN_OUTPUT_HIGH, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},

    {GPH0_6, I2C_CHUB1__SCL, GPIO_DAT_HIGH, GPIO_PUD_PULLUP, GPIO_CONPDN_OUTPUT_HIGH, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    {GPH0_7, I2C_CHUB1__SDA, GPIO_DAT_HIGH, GPIO_PUD_PULLUP, GPIO_CONPDN_OUTPUT_HIGH, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},

    {GPE0_0, GPIO_CON_EINT, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    {GPE1_0, GPIO_CON_EINT, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},

    // SOC2PM_SENSOR_EN
    {GPM12_0, EINT_CMGP2CHUB0__ALV, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    {GPM13_0, EINT_CMGP2CHUB1__ALV, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
};
#endif

