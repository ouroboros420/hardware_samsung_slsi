/*----------------------------------------------------------------------------
 * Exynos SoC  -  CMGP
 *
 * Copyright (C) 2021 Samsung Electronics Co., Ltd.
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

#ifndef __CMGP8825_H__
#define __CMGP8825_H__

typedef enum{
    CMGP_GPM00_IRQ = 0,
    CMGP_GPM01_IRQ = 1,
    CMGP_GPM02_IRQ = 2,
    CMGP_GPM03_IRQ = 3,
    CMGP_GPM04_IRQ = 4,
    CMGP_GPM05_IRQ = 5,
    CMGP_GPM06_IRQ = 6,
    CMGP_GPM07_IRQ = 7,
    CMGP_GPM08_IRQ = 8,
    CMGP_GPM09_IRQ = 9,
    CMGP_GPM10_IRQ = 10,
    CMGP_GPM11_IRQ = 11,
    CMGP_GPM12_IRQ = 12,
    CMGP_GPM13_IRQ = 13,
    CMGP_GPM14_IRQ = 14,
    CMGP_GPM15_IRQ = 15,
    CMGP_GPM16_IRQ = 16,
    CMGP_GPM17_IRQ = 17,
    CMGP_GPM18_IRQ = 18,
    CMGP_GPM19_IRQ = 19,
    CMGP_GPM20_IRQ = 20,

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

    CMGP_MAX_IRQ,
}CmgpIrqType;

#endif  // #ifndef __CMGP8825_H__
