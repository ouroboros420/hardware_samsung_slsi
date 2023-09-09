/*----------------------------------------------------------------------------
 *      Exynos SoC  -  UART
 *----------------------------------------------------------------------------
 *      Name:    uart.c
 *      Purpose: To implement UART APIs
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

#include <csp_common.h>
#if defined(SEOS)
    #include <errno.h>
#endif
#include <uart.h>
#include <uartOS.h>
#include <uartDrv.h>

//
int uartOpen(IN uint32_t port, IN UartBaudrateType baudRate, IN UartDatabitLenType dataBits, IN UartStopbitLenType stopBits, IN UartParityType parity)
{
    if ( port >= UART_CHANNEL_MAX )
    {
        CSP_PRINTF_ERROR("%s : invalid port (%d)\n", __func__, baudRate);
        CSP_ASSERT(0);
        return -EINVAL;
    }

    if ( baudRate >= UART_BAUDRATE_MAX )
        return -EINVAL;

    if ( dataBits >= UART_DATA_BITS_MAX )
        return -EINVAL;

    if ( stopBits >= UART_STOP_BITS_MAX )
        return -EINVAL;

    if ( parity >= UART_PARITY_MAX )
        return -EINVAL;

    return uartDrvOpen( port, baudRate, dataBits, stopBits, parity );
}

//
int uartWrite(IN uint32_t port, IN uint8_t *data, IN uint32_t len)
{
    int ret = 0;

    if ( port >= UART_CHANNEL_MAX )
        return 0;

    if ( data == NULL )
        return 0;

    if ( len == 0 )
        return 0;

    ret = uartDrvWrite( port, data, len );

    return ret;
}

int uartPutChar(IN uint32_t port, IN uint8_t data)
{
    return uartDrvPutChar( port, data );
}

//
int uartRead(IN uint32_t port, OUT uint8_t *data, IN uint32_t len)
{
    int ret = 0;

    if ( port >= UART_CHANNEL_MAX )
        return 0;

    if ( data == NULL )
        return 0;

    if ( len == 0 )
        return 0;

    ret = uartDrvRead( port, data, len );

    return ret;
}

uint8_t uartGetChar(IN uint32_t port)
{
    return uartDrvGetChar( port );
}

//
int uartClose(IN uint32_t port)
{
    uartDrvClose(port);
    return 0;
}

//
void uartOpenDbg(IN UartBaudrateType baudRate, IN UartDatabitLenType dataBits, IN UartStopbitLenType stopBits, IN UartParityType parity)
{
    if(baudRate >= UART_BAUDRATE_MAX){
        CSP_PRINTF_ERROR("uartOpen, invalid baudrate (%d)\n", baudRate);
        CSP_ASSERT(0);
        return;
    }

    if(dataBits >= UART_DATA_BITS_MAX){
        CSP_PRINTF_ERROR("uartOpen, invalid databit (%d)\n", dataBits);
        CSP_ASSERT(0);
        return;
    }

    if(stopBits >= UART_STOP_BITS_MAX){
        CSP_PRINTF_ERROR("uartOpen, invalid stopbit (%d)\n", stopBits);
        CSP_ASSERT(0);
        return;
    }

    if(parity >= UART_PARITY_MAX){
        CSP_PRINTF_ERROR("uartOpen, invalid parity (%d)\n", parity);
        CSP_ASSERT(0);
        return;
    }

    uartOpen( UART_CHANNEL_DEBUG, baudRate, dataBits, stopBits, parity);
}

void uartPutCharDbg(IN uint8_t ch)
{
    uartPutChar( UART_CHANNEL_DEBUG, ch);
}

#if defined(SFR_SAVE_RESTORE)
// Public API to save UART SFRs before entering system power mode
void uartSaveState(void)
{

}

// Public API to restore UART SFRs after exiting system power mode
void uartRestoreState(void)
{

}
#endif

#endif

#endif

