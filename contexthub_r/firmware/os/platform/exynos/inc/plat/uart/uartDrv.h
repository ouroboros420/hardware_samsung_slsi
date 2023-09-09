/*----------------------------------------------------------------------------
 *      Exynos SoC  -  UART
 *----------------------------------------------------------------------------
 *      Name:    uartDrv.h
 *      Purpose: To expose UART driver functions
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

#ifndef __UART_DRIVER_H__
#define __UART_DRIVER_H__

#include <csp_common.h>
#include <uart.h>

int uartDrvOpen(uint32_t port, IN UartBaudrateType baudRate, IN UartDatabitLenType dataBits, IN UartStopbitLenType IN stopBits, IN UartParityType parity);
int uartDrvWrite(IN uint32_t port, IN uint8_t *data, IN uint32_t len);
int uartDrvPutChar(IN uint32_t port, IN uint8_t ch);
int uartDrvRead(IN uint32_t port, OUT uint8_t *data, IN uint32_t len);
uint8_t uartDrvGetChar(IN uint32_t port);
int uartDrvClose(IN uint32_t port);

#endif

#endif

