/*----------------------------------------------------------------------------
 * Exynos SoC  -  CMGP
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

#ifndef __CMGP5515_H__
#define __CMGP5515_H__

typedef enum{
    CMGP_USI0_IRQ = 0,
    CMGP_I2C0_IRQ = 1,
    CMGP_USI1_IRQ = 2,
    CMGP_I2C1_IRQ = 3,
    CMGP_USI2_IRQ = 4,
    CMGP_I2C2_IRQ = 5,
    CMGP_USI3_IRQ = 6,
    CMGP_I2C3_IRQ = 7,
    CMGP_I2C4_IRQ = 8,
    CMGP_I2C5_IRQ = 9,
    CMGP_I2C6_IRQ = 10,

    CMGP_ADC_IRQ = 16,

    CMGP_GPM00_IRQ = 28,
    CMGP_GPM01_IRQ = 29,
    CMGP_GPM02_IRQ = 30,
    CMGP_GPM03_IRQ = 31,
    CMGP_GPM04_IRQ = 32,
    CMGP_GPM05_IRQ = 33,
    CMGP_GPM06_IRQ = 34,
    CMGP_GPM07_IRQ = 35,
    CMGP_GPM08_IRQ = 36,
    CMGP_GPM09_IRQ = 37,
    CMGP_GPM10_IRQ = 38,
    CMGP_GPM11_IRQ = 39,
    CMGP_GPM12_IRQ = 40,
    CMGP_GPM13_IRQ = 41,
    CMGP_GPM14_IRQ = 42,
    CMGP_GPM15_IRQ = 43,
    CMGP_GPM16_IRQ = 44,
    CMGP_GPM17_IRQ = 45,
    CMGP_GPM18_IRQ = 46,
    CMGP_GPM19_IRQ = 47,
    CMGP_GPM20_IRQ = 48,
    CMGP_GPM21_IRQ = 49,
    CMGP_GPM22_IRQ = 50,
    CMGP_GPM23_IRQ = 51,
    CMGP_GPM24_IRQ = 52,
    CMGP_GPM25_IRQ = 53,
    CMGP_MAX_IRQ,
}CmgpIrqType;

#endif  // #ifndef __CMGP5515_H__
