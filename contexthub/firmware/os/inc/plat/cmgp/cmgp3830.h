/*----------------------------------------------------------------------------
 * Exynos SoC  -  CMGP
 *
 * Copyright (C) 2021 Samsung Electronics Co., Ltd.
 *
 * Huiling Wu <huiling.wu@samsung.com>
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

#ifndef __CMGP3830_H__
#define __CMGP3830_H__

typedef enum{
    CMGP_USI0_IRQ = 0,
    CMGP_USI1_IRQ = 2,

    CMGP_GPM00_IRQ = 20,
    CMGP_GPM01_IRQ = 21,
    CMGP_GPM02_IRQ = 22,
    CMGP_GPM03_IRQ = 23,
    CMGP_GPM04_IRQ = 24,
    CMGP_GPM05_IRQ = 25,
    CMGP_GPM06_IRQ = 26,
    CMGP_GPM07_IRQ = 27,

    CMGP_MAX_IRQ,
}CmgpIrqType;

#endif  // #ifndef __CMGP3830_H__
