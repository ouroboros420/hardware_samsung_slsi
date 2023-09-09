/*----------------------------------------------------------------------------
 *      Exynos SoC  -  UART
 *----------------------------------------------------------------------------
 *      Name:    uartSfrBase.h
 *      Purpose: To expose I2C driver functions
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

#if defined(UART_REQUIRED)

#ifndef __UART_SFR_BASE_H__
#define __UART_SFR_BASE_H__

#include <csp_common.h>

#define UART_STATE_OPEN                 (1)
#define UART_STATE_CLOSE                (0)

typedef struct {
    uint32_t base;
    uint32_t state;
}UartInfoType;

#include CSP_HEADER(uartSfrBase)

#endif //__UART_SFR_BASE_H__

#endif //#if defined(UART_REQUIRED)

