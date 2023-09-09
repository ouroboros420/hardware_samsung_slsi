/*----------------------------------------------------------------------------
 *      Exynos SoC  -  UART
 *----------------------------------------------------------------------------
 *      Name:    uartDrv.c
 *      Purpose: To implement UART driver functions
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
#include <uartDrv.h>
#include <uartSfrBase.h>

#define    REG_UART_ULCON	            (0x00)
#define    REG_UART_UCON	            (0x04)
#define    REG_UART_UFCON	            (0x08)
#define    REG_UART_UMCON	            (0x0C)
#define    REG_UART_UTRSTAT	            (0x10)
#define    REG_UART_UERSTAT	            (0x14)
#define    REG_UART_UFSTAT	            (0x18)
#define    REG_UART_UMSTAT	            (0x1C)
#define    REG_UART_UTXH	            (0x20)
#define    REG_UART_URXH	            (0x24)
#define    REG_UART_UBRDIV	            (0x28)
#define    REG_UART_FRACVAL	            (0x2C)
#define    REG_UART_INTP	            (0x30)
#define    REG_UART_INTS	            (0x34)
#define    REG_UART_INTM	            (0x38)

// Uart Line Control (ULCON)

// -- Word length
#define DATA_BITS_5                     0
#define DATA_BITS_6                     1
#define DATA_BITS_7                     2
#define DATA_BITS_8                     3
#define DATA_BITS_MASK                  3
#define DATA_BITS_SHIFT                 0

// -- the number of stop bit
#define STOP_BITS_1                     0
#define STOP_BITS_2                     1
#define STOP_BITS_MASK                  1
#define STOP_BITS_SHIFT                 2

// -- Parity mode
#define PARITY_NONE                     0
#define PARITY_ODD                      4
#define PARITY_EVEN                     5
#define PARITY_FORCED_CHECK_AS_1        6
#define PARITY_FORCED_CHECK_AS_0        7
#define PARITY_MASK                     7
#define PARITY_SHIFT                    3

// Uart Control (UCON)

// -- Tx, Rx mode
#define MODE_DISABLE 	               	0
#define MODE_IRQ_OR_POLLING            	1
#define MODE_DMA                	    2
#define TX_MODE                         (MODE_IRQ_OR_POLLING << 2)
#define RX_MODE                         (MODE_IRQ_OR_POLLING << 0)

// -- Rx Error Status Interrupt Enable
#define RX_ERROR_STATUS_INT_ENABLE      (1 << 6)

// -- Rx Timeout Enable
#define RX_TIMEOUT                      3 // default value
#define RX_TIMEOUT_ENABLE               ((1 << 7) | (RX_TIMEOUT << 12))

// Uart FIFO Control (UFCON)

// -- FIFO enable
#define FIFO_ENABLE                     1

// -- FIFO reset
#define FIFO_RESET                      (3 << 1)

// FIFO Trigger Level
#define FIFO_RX_TRIGGER_LEVEL           (3 << 4) // 128bytes in case FIFO_DEPTH is 256B
#define FIFO_TX_TRIGGER_LEVEL           (3 << 8) // 128bytes in case FIFO_DEPTH is 256B
#define FIFO_TRIGGER_LEVEL              (FIFO_TX_TRIGGER_LEVEL) | (FIFO_RX_TRIGGER_LEVEL)

// Uart Flow-control Control (UMCON)
#define INACTIVATE_RTS                  0
#define DISABLE_RFC                     0
#define RTS_TRIGGER_LEVEL               0

// Uart Interrupt Mask
#define MASK_INTM_CTS                   (1 << 3)
#define MASK_INTM_TXD                   (1 << 2)
#define MASK_INTM_ERROR                 (1 << 1)
#define MASK_INTM_RXD                   (1 << 0)
#define MASK_ALL                        ((MASK_INTM_CTS) | (MASK_INTM_TXD) | (MASK_INTM_ERROR) | (MASK_INTM_RXD))

// Uart UFSTAT
#define TX_FIFO_FULL                    (1 << 24)
#define RX_FIFO_FULL                    (1 << 8)

#if defined(zebu)
    #define UART_IP_CLOCK           6500000
#else
    #define UART_IP_CLOCK           180000000
#endif

static int uartDrvSetDatabit(IN uint32_t port, IN UartDatabitLenType dataBits);
static int uartDrvSetStopbit(IN uint32_t port, IN UartStopbitLenType stopBits);
static int uartDrvSetParity(IN uint32_t port, IN UartParityType parity);
static void uartDrvDisableInterrupt(IN uint32_t port);


//
static int uartDrvSetBaudrate(IN uint32_t port, IN UartBaudrateType baudRate)
{
    uint32_t div, frac, baud;

    switch(baudRate) {
        case UART_BAUDRATE_115200:
            baud = 115200;
        break;

        default:
            CSP_PRINTF_ERROR("uartDrvSetBaudrateDbg() - invalid baudRate(%d)\n", baudRate);
            CSP_ASSERT(0);
        break;
    }

    div = UART_IP_CLOCK / (baud * 16) - 1;
    frac = (UART_IP_CLOCK / baud) - 16 * (1 + div);

    __raw_writel(div,  mUartInfo[port].base + REG_UART_UBRDIV);
    __raw_writel(frac, mUartInfo[port].base + REG_UART_FRACVAL);

    return 0;
}

static int uartDrvSetDatabit(IN uint32_t port, IN UartDatabitLenType dataBits)
{
    uint32_t regValue;

    regValue = __raw_readl( mUartInfo[port].base + REG_UART_ULCON );
    regValue &= (uint32_t)(~(DATA_BITS_MASK << DATA_BITS_SHIFT));

    // Uart Line Control
    switch(dataBits) {
        case UART_DATA_BITS_5:
            regValue |= (DATA_BITS_5 << DATA_BITS_SHIFT);
        break;

        case UART_DATA_BITS_6:
            regValue |= (DATA_BITS_6 << DATA_BITS_SHIFT);
        break;

        case UART_DATA_BITS_7:
            regValue |= (DATA_BITS_7 << DATA_BITS_SHIFT);
        break;

        case UART_DATA_BITS_8:
            regValue |= (DATA_BITS_8 << DATA_BITS_SHIFT);
        break;

        default:
            CSP_PRINTF_ERROR("uartDrvOpenDbg() - invalid dataBits(%d)\n", dataBits);
            CSP_ASSERT(0);
        break;
    }

    __raw_writel( regValue, mUartInfo[port].base + REG_UART_ULCON );

    return 0;
}

static int uartDrvSetStopbit(IN uint32_t port, IN UartStopbitLenType stopBits)
{
    uint32_t regValue;

    regValue = __raw_readl( mUartInfo[port].base + REG_UART_ULCON );
    regValue &= (uint32_t)(~(STOP_BITS_MASK << STOP_BITS_SHIFT));

    switch(stopBits) {
        case UART_STOP_BITS_1:
            regValue |= (STOP_BITS_1 << STOP_BITS_SHIFT);
        break;

        case UART_STOP_BITS_2:
            regValue |= (STOP_BITS_2 << STOP_BITS_SHIFT);
        break;

        default:
            CSP_PRINTF_ERROR("uartDrvOpenDbg() - invalid stopBits(%d)\n", stopBits);
            CSP_ASSERT(0);
        break;
    }

    __raw_writel( regValue, mUartInfo[port].base + REG_UART_ULCON );

    return 0;
}

static int uartDrvSetParity(IN uint32_t port, IN UartParityType parity)
{
    uint32_t regValue;

    regValue = __raw_readl( mUartInfo[port].base + REG_UART_ULCON );
    regValue &= (uint32_t)(~(PARITY_MASK << PARITY_SHIFT));

    switch(parity) {
        case UART_PARITY_NONE:
            regValue |= (PARITY_NONE << PARITY_SHIFT);
        break;

        case UART_PARITY_ODD:
            regValue |= (PARITY_ODD << PARITY_SHIFT);
        break;

        case UART_PARITY_EVEN:
            regValue |= (PARITY_EVEN << PARITY_SHIFT);
        break;

        case UART_PARITY_FORCED_CHECK_AS_1:
            regValue |= (PARITY_FORCED_CHECK_AS_1 << PARITY_SHIFT);
        break;

        case UART_PARITY_FORCED_CHECK_AS_0:
            regValue |= (PARITY_FORCED_CHECK_AS_0 << PARITY_SHIFT);
        break;

    	default:
    		CSP_PRINTF_ERROR("uartDrvOpenDbg() - invalid parity(%d)\n", parity);
    		CSP_ASSERT(0);
    	break;

    }

    __raw_writel( regValue, mUartInfo[port].base + REG_UART_ULCON );

    return 0;
}

static void uartDrvDisableInterrupt(IN uint32_t port)
{
    // Uart interrupt mask
    __raw_writel( MASK_ALL, mUartInfo[port].base + REG_UART_INTM);
}
//
int uartDrvOpen(uint32_t port, IN UartBaudrateType baudRate, IN UartDatabitLenType dataBits, IN UartStopbitLenType IN stopBits, IN UartParityType parity)
{
    uint32_t regValue = 0;
    int      ret = 0;

    if ( port >= UART_CHANNEL_MAX )
        return -EINVAL;

    if( mUartInfo[port].state == UART_STATE_OPEN )
    {
        CSP_PRINTF_ERROR("%s : port already opened (%d)\n", __func__, (int)port);
        return -1;
    }

    uartDrvDisableInterrupt( port );

    ret = uartDrvSetBaudrate( port, baudRate);
    if( ret != 0 )
        return ret;

    ret = uartDrvSetDatabit( port, dataBits );
    if( ret != 0 )
        return ret;

    ret = uartDrvSetStopbit( port, stopBits );
    if( ret != 0 )
        return ret;

    ret = uartDrvSetParity( port, parity );
    if( ret != 0 )
        return ret;

    // Uart Control
    regValue = TX_MODE | RX_MODE | RX_TIMEOUT_ENABLE;

    __raw_writel( regValue, mUartInfo[port].base + REG_UART_UCON );

    // Uart FIFO Control
    regValue = FIFO_RESET;
    __raw_writel( regValue, mUartInfo[port].base + REG_UART_UFCON );
    regValue = FIFO_ENABLE | FIFO_TRIGGER_LEVEL;
    __raw_writel( regValue, mUartInfo[port].base + REG_UART_UFCON );

    // Uart flow-control Control
    regValue = INACTIVATE_RTS | DISABLE_RFC | RTS_TRIGGER_LEVEL;
    __raw_writel( regValue, mUartInfo[port].base + REG_UART_UMCON );

    mUartInfo[port].state = UART_STATE_OPEN;

    return 0;
}

int uartDrvWrite(IN uint32_t port, IN uint8_t *data, IN uint32_t len)
{
    uint32_t i;

    if( port >= UART_CHANNEL_MAX ) return 0;
    if(mUartInfo[port].state == UART_STATE_CLOSE) return 0;

    for( i = 0 ; i < len ; i++) {
        uartDrvPutChar( port, *data);
        data++;
    }

    return i;
}

int uartDrvPutChar(IN uint32_t port, IN uint8_t ch)
{
    if( port >= UART_CHANNEL_MAX ) return 0;
    if(mUartInfo[port].state == UART_STATE_CLOSE) return 0;

    // Wait if tx fifio is full, This is hack to be fixed
    while((__raw_readl(mUartInfo[port].base + REG_UART_UFSTAT)) & TX_FIFO_FULL);

    if (ch == '\n') {
        __raw_writel('\r', mUartInfo[port].base + REG_UART_UTXH);
    }
    __raw_writel(ch, mUartInfo[port].base + REG_UART_UTXH);

    return 1;
}

int uartDrvRead(IN uint32_t port, OUT uint8_t *data, IN uint32_t len)
{
    uint32_t i;

    if( port >= UART_CHANNEL_MAX ) return 0;
    if(mUartInfo[port].state == UART_STATE_CLOSE) return 0;

    for( i = 0 ; i < len ; i++ ) {
        *data = uartDrvGetChar( port );
        data++;
    }

    return i;
}

uint8_t uartDrvGetChar(IN uint32_t port)
{
    uint8_t data;
    uint32_t regValue;

    if( port >= UART_CHANNEL_MAX ) return 0;
    if(mUartInfo[port].state == UART_STATE_CLOSE) return 0;

    // Wait if rx fifio is empty
    while(1) {
        regValue = __raw_readl(mUartInfo[port].base + REG_UART_UFSTAT);
        if( regValue & 0xFF ) break; // FIFO is not empty
        else if( regValue & RX_FIFO_FULL ) break; // FIFO is full
    }

    data = (uint8_t)__raw_readl( mUartInfo[port].base + REG_UART_URXH );

    return data;
}

int uartDrvClose(IN uint32_t port)
{
    if( port >= UART_CHANNEL_MAX ) return -EINVAL;

    mUartInfo[port].state = UART_STATE_CLOSE;
    return 0;
}

#endif

