/*----------------------------------------------------------------------------
 *      Exynos SoC  -  UART
 *----------------------------------------------------------------------------
 *      Name:    uart.h
 *      Purpose: To expose UART APIs and define macros
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

#ifndef __UART_H__
#define __UART_H__

#include <csp_common.h>
#include <csp_assert.h>
#include <csp_printf.h>

// Uart Baudrate
typedef enum {
    UART_BAUDRATE_115200 = 0,
    UART_BAUDRATE_MAX
} UartBaudrateType;

// Uart Databit
typedef enum {
    UART_DATA_BITS_5 = 0,
    UART_DATA_BITS_6,
    UART_DATA_BITS_7,
    UART_DATA_BITS_8,
    UART_DATA_BITS_MAX
} UartDatabitLenType;

// Uart Stopbit
typedef enum {
    UART_STOP_BITS_1 = 0,
    UART_STOP_BITS_2,
    UART_STOP_BITS_MAX,
} UartStopbitLenType;

// Uart Parity
typedef enum {
    UART_PARITY_NONE = 0,
    UART_PARITY_ODD,
    UART_PARITY_EVEN,
    UART_PARITY_FORCED_CHECK_AS_1,
    UART_PARITY_FORCED_CHECK_AS_0,
    UART_PARITY_MAX
} UartParityType;

int uartOpen(IN uint32_t port, IN UartBaudrateType baudRate,
             IN UartDatabitLenType dataBits, IN UartStopbitLenType stopBits,
             IN UartParityType parity);
int uartWrite(IN uint32_t port, IN uint8_t * data, IN uint32_t len);
int uartPutChar(IN uint32_t port, IN uint8_t data);
int uartRead(IN uint32_t port, OUT uint8_t * data, IN uint32_t len);
uint8_t uartGetChar(IN uint32_t port);
int uartClose(IN uint32_t port);
void uartOpenDbg(IN UartBaudrateType baudRate, IN UartDatabitLenType dataBits,
                 IN UartStopbitLenType stopBits, IN UartParityType parity);
void uartPutCharDbg(IN uint8_t ch);
void uartSaveState(void);
void uartRestoreState(void);
void uartDeinit(void);

#endif

#endif

