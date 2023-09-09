/*----------------------------------------------------------------------------
 *      Exynos SoC  -  USI
 *----------------------------------------------------------------------------
 *      Name:    usiSfrBase9815.c
 *      Purpose: To expose USI driver functions
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

#ifndef __USI_9815_C__
#define __USI_9815_C__

#include <csp_common.h>
#include <plat/i2c/i2c.h>
#include <plat/spi/spi.h>

typedef struct {
    uint32_t        base;
    uint32_t        sw_conf;
    uint32_t        protocol;
    uint32_t        state;
    uint32_t        ch_spi;
    uint32_t        ch_i2c;
    uint32_t        ch_uart;
}UsiInfoType;

#define DEC_USI(ch, s) { \
    .base = ch##_BASE_ADDRESS, \
    .sw_conf =  s, \
}

static UsiInfoType mUsiInfo[USI_CHANNEL_MAX] = {
    DEC_USI(USI0_CHUB, SYSREG_BASE_ADDRESS + 0x2000),
    DEC_USI(I2C1_CHUB, SYSREG_BASE_ADDRESS + 0x2004),
    DEC_USI(USI1_CHUB, SYSREG_BASE_ADDRESS + 0x2010),
    DEC_USI(I2C3_CHUB, SYSREG_BASE_ADDRESS + 0x2014),
    DEC_USI(USI2_CHUB, SYSREG_BASE_ADDRESS + 0x2020),
    DEC_USI(I2C5_CHUB, SYSREG_BASE_ADDRESS + 0x2024),
};

void usi0_IRQHandler(void)
{
    uint32_t protocol;

    protocol = __raw_readl(mUsiInfo[0].base + REG_USI_CONFIG);
    if (protocol == USI_PROTOCOL_I2C) {
        i2c_IRQHandler(0);
    } else if (protocol == USI_PROTOCOL_SPI) {
        spi_IRQHandler(0);
    }
}

void usi1_IRQHandler(void)
{
    i2c_IRQHandler(1);
}

void usi2_IRQHandler(void)
{
    uint32_t protocol;

    protocol = __raw_readl(mUsiInfo[2].base + REG_USI_CONFIG);
    if (protocol == USI_PROTOCOL_I2C) {
        i2c_IRQHandler(2);
    } else if (protocol == USI_PROTOCOL_SPI) {
        spi_IRQHandler(1);
    }
}

void usi3_IRQHandler(void)
{
    i2c_IRQHandler(3);
}

void usi4_IRQHandler(void)
{
    uint32_t protocol;

    protocol = __raw_readl(mUsiInfo[4].base + REG_USI_CONFIG);
    if (protocol == USI_PROTOCOL_I2C) {
        i2c_IRQHandler(4);
    } else if (protocol == USI_PROTOCOL_SPI) {
        spi_IRQHandler(2);
    }
}

void usi5_IRQHandler(void)
{
    i2c_IRQHandler(5);
}

#endif /* __USI_SFR_BASE_H__ */

