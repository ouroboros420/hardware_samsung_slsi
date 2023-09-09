/*----------------------------------------------------------------------------
 *      Exynos SoC  -  GPIO
 *----------------------------------------------------------------------------
 *      Name:    gpio6315.c
 *      Purpose: To implement GPIO driver functions
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

#ifndef __GPIO6315_C__
#define __GPIO6315_C__

#include CSP_HEADER(gpio)
#include CSP_HEADER(cmgp)

#define    REG_GPIO_GPH0CON             (GPIO_BASE_ADDRESS + 0x00)
#define    REG_GPIO_GPH1CON             (GPIO_BASE_ADDRESS + 0x20)
#define    REG_GPIO_GPH2CON             (GPIO_BASE_ADDRESS + 0x40)
#define    REG_GPIO_GPH3CON             (GPIO_BASE_ADDRESS + 0x60)
#define    REG_GPIO_GPH4CON             (GPIO_BASE_ADDRESS + 0x80)

#define    REG_GPIO_NWEINT_GPH0_CON     (GPIO_BASE_ADDRESS + 0x700)
#define    REG_GPIO_NWEINT_GPH1_CON     (GPIO_BASE_ADDRESS + 0x704)
#define    REG_GPIO_NWEINT_GPH2_CON     (GPIO_BASE_ADDRESS + 0x708)
#define    REG_GPIO_NWEINT_GPH3_CON     (GPIO_BASE_ADDRESS + 0x70C)
#define    REG_GPIO_NWEINT_GPH4_CON     (GPIO_BASE_ADDRESS + 0x710)
#define    REG_GPIO_NWEINT_GPH0_FLTCON0 (GPIO_BASE_ADDRESS + 0x800)
#define    REG_GPIO_NWEINT_GPH0_FLTCON1 (GPIO_BASE_ADDRESS + 0x804)
#define    REG_GPIO_NWEINT_GPH1_FLTCON0 (GPIO_BASE_ADDRESS + 0x808)
#define    REG_GPIO_NWEINT_GPH1_FLTCON1 (GPIO_BASE_ADDRESS + 0x80C)
#define    REG_GPIO_NWEINT_GPH2_FLTCON0 (GPIO_BASE_ADDRESS + 0x810)
#define    REG_GPIO_NWEINT_GPH3_FLTCON0 (GPIO_BASE_ADDRESS + 0x814)
#define    REG_GPIO_NWEINT_GPH3_FLTCON1 (GPIO_BASE_ADDRESS + 0x818)
#define    REG_GPIO_NWEINT_GPH4_FLTCON0 (GPIO_BASE_ADDRESS + 0x81C)

#define REG_DAT_OFFSET                   0x4
#define REG_PUD_OFFSET                   0x8
#define REG_DRV_OFFSET                   0xC

#define REG_NWEINT_FLTCON_OFFSET         0x100
#define REG_NWEINT_MASK_OFFSET           0x200
#define REG_NWEINT_PEND_OFFSET           0x300

#define REG_CON_BITFIELD                 (4)
#define REG_DAT_BITFIELD                 (1)
#define REG_PUD_BITFIELD                 (4)
#define REG_DRV_BITFIELD                 (4)

#define REG_NWEINT_CON_BITFIELD          (4)
#define REG_NWEINT_FLTCON_BITFIELD       (8)
#define REG_NWEINT_MASK_BITFIELD         (1)
#define REG_NWEINT_PEND_BITFIELD         (1)

#define REG_FILTER_EN_SHIFT              (7)
#define REG_FILTER_SEL_SHIFT             (6)
#define REG_FILTER_WIDTH_BITFIELD        (6)

#define DEC_CHUB_GPIO(group, idx, fltcon) \
    .pin = group##_##idx, \
    .index = idx, \
    .irqNum = EXTINT_##group##_##idx, \
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
    /* CHUB BPIO */
    { DEC_CHUB_GPIO(GPH0, 0, 0) },
    { DEC_CHUB_GPIO(GPH0, 1, 0) },
    { DEC_CHUB_GPIO(GPH0, 2, 0) },
    { DEC_CHUB_GPIO(GPH0, 3, 0) },
    { DEC_CHUB_GPIO(GPH0, 4, 1) },
    { DEC_CHUB_GPIO(GPH0, 5, 1) },
    { DEC_CHUB_GPIO(GPH0, 6, 1) },
    { DEC_CHUB_GPIO(GPH0, 7, 1) },

    { DEC_CHUB_GPIO(GPH1, 0, 0) },
    { DEC_CHUB_GPIO(GPH1, 1, 0) },
    { DEC_CHUB_GPIO(GPH1, 2, 0) },
    { DEC_CHUB_GPIO(GPH1, 3, 0) },
    { DEC_CHUB_GPIO(GPH1, 4, 1) },
    { DEC_CHUB_GPIO(GPH1, 5, 1) },
    { DEC_CHUB_GPIO(GPH1, 6, 1) },
    { DEC_CHUB_GPIO(GPH1, 7, 1) },

    { DEC_CHUB_GPIO(GPH2, 0, 0) },
    { DEC_CHUB_GPIO(GPH2, 1, 0) },
    { DEC_CHUB_GPIO(GPH2, 2, 0) },
    { DEC_CHUB_GPIO(GPH2, 3, 0) },

    { DEC_CHUB_GPIO(GPH3, 0, 0) },
    { DEC_CHUB_GPIO(GPH3, 1, 0) },
    { DEC_CHUB_GPIO(GPH3, 2, 0) },
    { DEC_CHUB_GPIO(GPH3, 3, 0) },
    { DEC_CHUB_GPIO(GPH3, 4, 1) },
    { DEC_CHUB_GPIO(GPH3, 5, 1) },
    { DEC_CHUB_GPIO(GPH3, 6, 1) },
    { DEC_CHUB_GPIO(GPH3, 7, 1) },

    { DEC_CHUB_GPIO(GPH4, 0, 0) },
    { DEC_CHUB_GPIO(GPH4, 1, 0) },
    /* CMGP GPIO */
};

#endif

