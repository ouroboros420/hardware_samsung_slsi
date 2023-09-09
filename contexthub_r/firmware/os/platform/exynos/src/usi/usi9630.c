/*----------------------------------------------------------------------------
 *      Exynos SoC  -  USI
 *----------------------------------------------------------------------------
 *      Name:    usiSfrBase9630.h
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

#ifndef __USI_9630_C__
#define __USI_9630_C__

#include <csp_common.h>

typedef struct {
    uint32_t        base;
    uint32_t        sw_conf;
    uint32_t        protocol;
    uint32_t        state;
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
    DEC_USI(USI0_CMGP, SYSREG_CMGP_BASE_ADDRESS + 0x2000),
    DEC_USI(I2C1_CMGP, SYSREG_CMGP_BASE_ADDRESS + 0x2004),
    DEC_USI(USI1_CMGP, SYSREG_CMGP_BASE_ADDRESS + 0x2010),
    DEC_USI(I2C3_CMGP, SYSREG_CMGP_BASE_ADDRESS + 0x2014),
    DEC_USI(USI2_CMGP, SYSREG_CMGP_BASE_ADDRESS + 0x2020),
    DEC_USI(I2C5_CMGP, SYSREG_CMGP_BASE_ADDRESS + 0x2024),
    DEC_USI(USI3_CMGP, SYSREG_CMGP_BASE_ADDRESS + 0x2030),
    DEC_USI(I2C7_CMGP, SYSREG_CMGP_BASE_ADDRESS + 0x2034)
};

#endif /* __USI_SFR_BASE_H__ */

