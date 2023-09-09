/*----------------------------------------------------------------------------
 *      Exynos SoC  -  USI
 *----------------------------------------------------------------------------
 *      Name:    usiSfrBase9110.c
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

static UsiInfoType mUsiInfo[USI_CHANNEL_MAX] = {
    DEC_USI(USI0_CHUB, SYSREG_BASE_ADDRESS + 0x3000, SPI_CHUB0, I2C_CHUB0),
    DEC_USI(I2C0_CHUB, SYSREG_BASE_ADDRESS + 0x3004,         0, I2C_CHUB1),
    DEC_USI(I2C1_CHUB, SYSREG_BASE_ADDRESS + 0x3008,         0, I2C_CHUB2),
    //DEC_USI(USI0_CMGP, SYSREG_CMGP_BASE_ADDRESS + 0x2000, SPI_CMGP0, I2C_CMGP0),
    //DEC_USI(I2C1_CMGP, SYSREG_CMGP_BASE_ADDRESS + 0x2004,         0, I2C_CMGP1),
    DEC_USI(USI1_CMGP, SYSREG_CMGP_BASE_ADDRESS + 0x2010, SPI_CMGP1, I2C_CMGP2),
    DEC_USI(I2C3_CMGP, SYSREG_CMGP_BASE_ADDRESS + 0x2014,         0, I2C_CMGP3),
    DEC_USI(USI2_CMGP, SYSREG_CMGP_BASE_ADDRESS + 0x2020, SPI_CMGP2, I2C_CMGP4),
    DEC_USI(I2C5_CMGP, SYSREG_CMGP_BASE_ADDRESS + 0x2024,         0, I2C_CMGP5),
    DEC_USI(USI3_CMGP, SYSREG_CMGP_BASE_ADDRESS + 0x2030, SPI_CMGP3, I2C_CMGP6),
    DEC_USI(I2C7_CMGP, SYSREG_CMGP_BASE_ADDRESS + 0x2034,         0, I2C_CMGP7),
};
