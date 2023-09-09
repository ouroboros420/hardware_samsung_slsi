/*----------------------------------------------------------------------------
 * Exynos SoC  -  CMGP
 *
 * Copyright (C) 2022 Samsung Electronics Co., Ltd.
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

#ifndef __CMGP8835_H__
#define __CMGP8835_H__

typedef enum{
    CMGP_GPM00_0_IRQ = 0,
    CMGP_GPM01_0_IRQ = 1,
    CMGP_GPM02_0_IRQ = 2,
    CMGP_GPM03_0_IRQ = 3,
    CMGP_GPM04_0_IRQ = 4,
    CMGP_GPM05_0_IRQ = 5,
    CMGP_GPM06_0_IRQ = 6,
    CMGP_GPM07_0_IRQ = 7,
    CMGP_GPM08_0_IRQ = 8,
    CMGP_GPM09_0_IRQ = 9,
    CMGP_GPM10_0_IRQ = 10,
    CMGP_GPM11_0_IRQ = 11,
    CMGP_GPM12_0_IRQ = 12,
    CMGP_GPM13_0_IRQ = 13,
    CMGP_GPM14_0_IRQ = 14,
    CMGP_GPM15_0_IRQ = 15,
    CMGP_GPM16_0_IRQ = 16,
    CMGP_GPM17_0_IRQ = 17,
    CMGP_GPM18_0_IRQ = 18,
    CMGP_GPM19_0_IRQ = 19,
    CMGP_GPM20_0_IRQ = 20,
    CMGP_GPM21_0_IRQ = 21,
    CMGP_GPM22_0_IRQ = 22,
    CMGP_GPM23_0_IRQ = 23,
    CMGP_GPM24_0_IRQ = 24,
    CMGP_GPM25_0_IRQ = 25,
    CMGP_GPM26_0_IRQ = 26,
    CMGP_GPM27_0_IRQ = 27,
    CMGP_GPM28_0_IRQ = 28,
    CMGP_USI0_IRQ = 32,
    CMGP_I2C0_IRQ = 33,
    CMGP_USI1_IRQ = 34,
    CMGP_I2C1_IRQ = 35,
    CMGP_USI2_IRQ = 36,
    CMGP_I2C2_IRQ = 37,
    CMGP_USI3_IRQ = 38,
    CMGP_I2C3_IRQ = 39,
    CMGP_USI4_IRQ = 40,
    CMGP_I2C4_IRQ = 41,
    CMGP_I3C0_IRQ = 42,
    CMGP_I3C1_IRQ = 43,
    CMGP_USI5_IRQ = 44,
    CMGP_I2C5_IRQ = 45,
    CMGP_USI6_IRQ = 46,
    CMGP_I2C6_IRQ = 47,
    CMGP_GPC08_0_IRQ = 49,
    CMGP_GPC09_0_IRQ = 50,
    CMGP_GPC10_0_IRQ = 51,
    CMGP_GPC11_0_IRQ = 52,
    CMGP_GPC12_0_IRQ = 53,
    CMGP_GPC13_0_IRQ = 54,
    CMGP_GPC14_0_IRQ = 55,
    CMGP_MAX_IRQ,
}CmgpIrqType;

#endif  // #ifndef __CMGP8835_H__
