/*----------------------------------------------------------------------------
 * Exynos SoC  -  USI
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

#define DEC_USI(ch, s, spi, i2c) { \
    .base = ch##_BASE_ADDRESS, \
    .sw_conf =  s, \
    .ch_spi = spi, \
    .ch_i2c = i2c  \
}

static UsiInfoType mUsiInfo[I2C_CHANNEL_MAX] = {
    DEC_USI(USI0_CHUB,  SYSREG_BASE_ADDRESS + 0x2000, SPI_CHUB0, I2C_CHUB0),
    DEC_USI(USI1_CHUB,  SYSREG_BASE_ADDRESS + 0x2004,         0, I2C_CHUB1),
    DEC_USI(USI2_CHUB,  SYSREG_BASE_ADDRESS + 0x2010, SPI_CHUB1, I2C_CHUB2),
    DEC_USI(USI3_CHUB,  SYSREG_BASE_ADDRESS + 0x2014,         0, I2C_CHUB3),
    DEC_USI(USI4_CHUB,  SYSREG_BASE_ADDRESS + 0x2020, SPI_CHUB2, I2C_CHUB4),
    DEC_USI(USI5_CHUB,  SYSREG_BASE_ADDRESS + 0x2030, SPI_CHUB3, I2C_CHUB5),
    DEC_USI(USI6_CHUB,  SYSREG_BASE_ADDRESS + 0x2040,         0, I2C_CHUB6),

    DEC_USI(USI0_CMGP,  SYSREG_CMGP_BASE_ADDRESS + 0x2000, SPI_CMGP0, I2C_CMGP0),
    DEC_USI(USI1_CMGP,  SYSREG_CMGP_BASE_ADDRESS + 0x2070,         0, I2C_CMGP1),
    DEC_USI(USI2_CMGP,  SYSREG_CMGP_BASE_ADDRESS + 0x2010, SPI_CMGP1, I2C_CMGP2),
    DEC_USI(USI3_CMGP,  SYSREG_CMGP_BASE_ADDRESS + 0x2074,         0, I2C_CMGP3),
    DEC_USI(USI4_CMGP,  SYSREG_CMGP_BASE_ADDRESS + 0x2020, SPI_CMGP2, I2C_CMGP4),
    DEC_USI(USI5_CMGP,  SYSREG_CMGP_BASE_ADDRESS + 0x2024,         0, I2C_CMGP5),
    DEC_USI(USI6_CMGP,  SYSREG_CMGP_BASE_ADDRESS + 0x2030, SPI_CMGP3, I2C_CMGP6),
    DEC_USI(USI7_CMGP,  SYSREG_CMGP_BASE_ADDRESS + 0x2034,         0, I2C_CMGP7),
    DEC_USI(USI8_CMGP,  SYSREG_CMGP_BASE_ADDRESS + 0x2040, SPI_CMGP4, I2C_CMGP8),
    DEC_USI(USI9_CMGP,  SYSREG_CMGP_BASE_ADDRESS + 0x2044,         0, I2C_CMGP9),
    DEC_USI(USI10_CMGP, SYSREG_CMGP_BASE_ADDRESS + 0x2050, SPI_CMGP5, I2C_CMGP10),
    DEC_USI(USI11_CMGP, SYSREG_CMGP_BASE_ADDRESS + 0x2054,         0, I2C_CMGP11),
    DEC_USI(USI12_CMGP, SYSREG_CMGP_BASE_ADDRESS + 0x2060, SPI_CMGP6, I2C_CMGP12),
    DEC_USI(USI13_CMGP, SYSREG_CMGP_BASE_ADDRESS + 0x2064,         0, I2C_CMGP13),
};

void usi0_IRQHandler(void)
{
    uint32_t protocol;

    protocol = __raw_readl(mUsiInfo[0].base + REG_USI_CONFIG);
    if (protocol == USI_PROTOCOL_I2C) {
        i2c_IRQHandler(I2C_CHUB0);
    } else if (protocol == USI_PROTOCOL_SPI) {
        spi_IRQHandler(SPI_CHUB0);
    }
}

void usi1_IRQHandler(void)
{
    i2c_IRQHandler(I2C_CHUB1);
}

void usi2_IRQHandler(void)
{
    i2c_IRQHandler(I2C_CHUB2);
}

void usi3_IRQHandler(void)
{
    uint32_t protocol;

    protocol = __raw_readl(mUsiInfo[3].base + REG_USI_CONFIG);
    if (protocol == USI_PROTOCOL_I2C) {
        i2c_IRQHandler(I2C_CMGP0);
    } else if (protocol == USI_PROTOCOL_SPI) {
        spi_IRQHandler(SPI_CMGP0);
    }
}

void usi4_IRQHandler(void)
{
    i2c_IRQHandler(I2C_CMGP1);
}

void usi5_IRQHandler(void)
{
    uint32_t protocol;

    protocol = __raw_readl(mUsiInfo[5].base + REG_USI_CONFIG);
    if (protocol == USI_PROTOCOL_I2C) {
        i2c_IRQHandler(I2C_CMGP2);
    } else if (protocol == USI_PROTOCOL_SPI) {
        spi_IRQHandler(SPI_CMGP1);
    }
}

void usi6_IRQHandler(void)
{
    i2c_IRQHandler(I2C_CMGP3);
}

void usi7_IRQHandler(void)
{
    uint32_t protocol;

    protocol = __raw_readl(mUsiInfo[7].base + REG_USI_CONFIG);
    if (protocol == USI_PROTOCOL_I2C) {
        i2c_IRQHandler(I2C_CMGP4);
    } else if (protocol == USI_PROTOCOL_SPI) {
        spi_IRQHandler(SPI_CMGP2);
    }
}

void usi8_IRQHandler(void)
{
    i2c_IRQHandler(I2C_CMGP5);
}

void usi9_IRQHandler(void)
{
    uint32_t protocol;

    protocol = __raw_readl(mUsiInfo[9].base + REG_USI_CONFIG);
    if (protocol == USI_PROTOCOL_I2C) {
        i2c_IRQHandler(I2C_CMGP6);
    } else if (protocol == USI_PROTOCOL_SPI) {
        spi_IRQHandler(SPI_CMGP3);
    }
}

void usi10_IRQHandler(void)
{
    i2c_IRQHandler(I2C_CMGP7);
}

void usi11_IRQHandler(void)
{
    i2c_IRQHandler(I2C_CMGP8);
}

void usi12_IRQHandler(void)
{
    i2c_IRQHandler(I2C_CMGP9);
}

void usi13_IRQHandler(void)
{
    i2c_IRQHandler(I2C_CMGP10);
}

