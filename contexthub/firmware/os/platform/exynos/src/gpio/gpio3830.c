/*----------------------------------------------------------------------------
 * Exynos SoC  -  GPIO
 *
 * Copyright (C) 2021 Samsung Electronics Co., Ltd.
 *
 * Huiling wu <huiling.wu@samsung.com>
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

#include CSP_HEADER(gpio)
#include CSP_HEADER(cmgp)

#define REG_GPIO_GPM00CON               (GPIO_CMGP_BASE_ADDRESS + 0x00)
#define REG_GPIO_GPM01CON               (GPIO_CMGP_BASE_ADDRESS + 0x20)
#define REG_GPIO_GPM02CON               (GPIO_CMGP_BASE_ADDRESS + 0x40)
#define REG_GPIO_GPM03CON               (GPIO_CMGP_BASE_ADDRESS + 0x60)
#define REG_GPIO_GPM04CON               (GPIO_CMGP_BASE_ADDRESS + 0x80)
#define REG_GPIO_GPM05CON               (GPIO_CMGP_BASE_ADDRESS + 0xA0)
#define REG_GPIO_GPM06CON               (GPIO_CMGP_BASE_ADDRESS + 0xC0)
#define REG_GPIO_GPM07CON               (GPIO_CMGP_BASE_ADDRESS + 0xE0)

#define REG_GPIO_NWEINT_GPM00_CON       (GPIO_CMGP_BASE_ADDRESS + 0x700)
#define REG_GPIO_NWEINT_GPM01_CON       (GPIO_CMGP_BASE_ADDRESS + 0x704)
#define REG_GPIO_NWEINT_GPM02_CON       (GPIO_CMGP_BASE_ADDRESS + 0x708)
#define REG_GPIO_NWEINT_GPM03_CON       (GPIO_CMGP_BASE_ADDRESS + 0x70C)
#define REG_GPIO_NWEINT_GPM04_CON       (GPIO_CMGP_BASE_ADDRESS + 0x710)
#define REG_GPIO_NWEINT_GPM05_CON       (GPIO_CMGP_BASE_ADDRESS + 0x714)
#define REG_GPIO_NWEINT_GPM06_CON       (GPIO_CMGP_BASE_ADDRESS + 0x718)
#define REG_GPIO_NWEINT_GPM07_CON       (GPIO_CMGP_BASE_ADDRESS + 0x71C)

#define REG_GPIO_NWEINT_GPM00_FLTCON0   (GPIO_CMGP_BASE_ADDRESS + 0x800)
#define REG_GPIO_NWEINT_GPM01_FLTCON0   (GPIO_CMGP_BASE_ADDRESS + 0x804)
#define REG_GPIO_NWEINT_GPM02_FLTCON0   (GPIO_CMGP_BASE_ADDRESS + 0x808)
#define REG_GPIO_NWEINT_GPM03_FLTCON0   (GPIO_CMGP_BASE_ADDRESS + 0x80C)
#define REG_GPIO_NWEINT_GPM04_FLTCON0   (GPIO_CMGP_BASE_ADDRESS + 0x810)
#define REG_GPIO_NWEINT_GPM05_FLTCON0   (GPIO_CMGP_BASE_ADDRESS + 0x814)
#define REG_GPIO_NWEINT_GPM06_FLTCON0   (GPIO_CMGP_BASE_ADDRESS + 0x818)
#define REG_GPIO_NWEINT_GPM07_FLTCON0   (GPIO_CMGP_BASE_ADDRESS + 0x81C)

#define REG_DAT_OFFSET               0x4
#define REG_PUD_OFFSET               0x8
#define REG_DRV_OFFSET               0xC

#define REG_NWEINT_FLTCON_OFFSET     0x100
#define REG_NWEINT_MASK_OFFSET       0x200
#define REG_NWEINT_PEND_OFFSET       0x300

#define REG_CON_BITFIELD             (4)
#define REG_DAT_BITFIELD             (1)
#define REG_PUD_BITFIELD             (4)
#define REG_DRV_BITFIELD             (4)

#define REG_NWEINT_CON_BITFIELD      (4)
#define REG_NWEINT_FLTCON_BITFIELD   (8)
#define REG_NWEINT_MASK_BITFIELD     (1)
#define REG_NWEINT_PEND_BITFIELD     (1)

#define REG_FILTER_EN_SHIFT          (7)
#define REG_FILTER_SEL_SHIFT         (6)
#define REG_FILTER_WIDTH_BITFIELD    (6)

#define DEC_CHUB_GPIO(group, idx, fltcon) \
    .pin = group##_##idx, \
    .index = idx, \
    .irqNum = EXTINT_##group##_##idx##_IRQn, \
    .conBase = REG_GPIO_##group##CON, \
    .eintBase = REG_GPIO_NWEINT_##group##_CON, \
    .eintFltcon = REG_GPIO_NWEINT_##group##_FLTCON##fltcon

#define DEC_CHUBEINT_GPIO(group, idx, fltcon, intnum) \
		.pin = group##_##idx, \
		.index = idx, \
		.irqNum = INTREQ_EXT_INTE##intnum##_IRQn, \
		.conBase = REG_GPIO_##group##CON, \
		.eintBase = REG_GPIO_NWEINT_##group##_CON, \
		.eintFltcon = REG_GPIO_NWEINT_##group##_FLTCON##fltcon

#define DEC_CMGP_GPIO(group, idx, fltcon) \
    .pin = group##_##idx, \
    .index = idx, \
    .irqNum = CMGP_##group##_IRQ, \
    .conBase = REG_GPIO_##group##CON, \
    .eintBase = REG_GPIO_NWEINT_##group##_CON, \
    .eintFltcon = REG_GPIO_NWEINT_##group##_FLTCON##fltcon

struct Gpio mGpioPinInfo[] = {
    { DEC_CMGP_GPIO(GPM00, 0, 0) },
    { DEC_CMGP_GPIO(GPM01, 0, 0) },
    { DEC_CMGP_GPIO(GPM02, 0, 0) },
    { DEC_CMGP_GPIO(GPM03, 0, 0) },
    { DEC_CMGP_GPIO(GPM04, 0, 0) },
    { DEC_CMGP_GPIO(GPM05, 0, 0) },
    { DEC_CMGP_GPIO(GPM06, 0, 0) },
    { DEC_CMGP_GPIO(GPM07, 0, 0) },
 };
