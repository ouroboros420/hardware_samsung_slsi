
/*----------------------------------------------------------------------------
 * Exynos SoC  -  USI
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
    DEC_USI(USI0,  SYSREG_CMGP_BASE_ADDRESS + 0x2000, SPI_CMGP0, I2C_CMGP0),
    DEC_USI(USI1,  SYSREG_CMGP_BASE_ADDRESS + 0x2010, SPI_CMGP1, I2C_CMGP1),
};

void usi0_IRQHandler(void) {
    uint32_t protocol;

    protocol = __raw_readl(mUsiInfo[0].base + REG_USI_CONFIG);
    if (protocol == USI_PROTOCOL_I2C) {
        i2c_IRQHandler(I2C_CMGP0);
    } else if (protocol == USI_PROTOCOL_SPI) {
        spi_IRQHandler(SPI_CMGP0);
    }
}

void usi1_IRQHandler(void) {
    uint32_t protocol;

    protocol = __raw_readl(mUsiInfo[1].base + REG_USI_CONFIG);
    if (protocol == USI_PROTOCOL_I2C) {
        i2c_IRQHandler(I2C_CMGP1);
    } else if (protocol == USI_PROTOCOL_SPI) {
        spi_IRQHandler(SPI_CMGP1);
    }
}
