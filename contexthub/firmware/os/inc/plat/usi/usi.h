/*----------------------------------------------------------------------------
 *      Exynos SoC  -  USI
 *----------------------------------------------------------------------------
 *      Name:    usi.h
 *      Purpose: To expose USI APIs and define macros
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

#ifndef __USI_H__
#define __USI_H__

#include <csp_common.h>
#include <csp_assert.h>
#include <csp_printf.h>

#include CSP_HEADER(usi)

typedef enum {
    USI_PROTOCOL_UART = 0x1,
    USI_PROTOCOL_SPI = 0x2,
    USI_PROTOCOL_I2C = 0x4,
    USI_PROTOCOL_MAX
} UsiProtocolType;

typedef enum {
    CMGP_HWACG_CONTROL_DISABLE = 0,
    CMGP_HWACG_CONTROL_ENABLE
} CmgpHwacgControlType;

#if defined(HWACG_OFF)
#define CMGP_DEFAULT_HWACG CMGP_HWACG_CONTROL_DISABLE
#else
#define CMGP_DEFAULT_HWACG CMGP_HWACG_CONTROL_ENABLE
#endif

int32_t usiOpen(IN uint32_t port, IN UsiProtocolType protocol);
int32_t usiSetProtocol(IN uint32_t port, IN UsiProtocolType protocol);
UsiProtocolType usiGetProtocol(IN uint32_t port);
int32_t usiSetUartDbg(void);
void usiSaveState(void);
void usiRestoreState(void);
void usiCmgpIrqHandler(uint32_t port);
void usiCmgpSaveState(void);
void usiCmgpRestoreState(void);
void usiClose(uint32_t port);

#endif

