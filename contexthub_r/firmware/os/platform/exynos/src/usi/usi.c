/*----------------------------------------------------------------------------
 *      Exynos SoC  -  USI
 *----------------------------------------------------------------------------
 *      Name:    usi.c
 *      Purpose: To implement USI APIs
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
#include <usi.h>

// SFR OFFSETs
#define    REG_USI_CONFIG               (0xC0)
#define    REG_USI_CON                  (0xC4)
#define    REG_USI_OPTION               (0xC8)
#define    REG_USI_USI_VERSION          (0xCC)
#define    REG_USI_UART_VERSION         (0xD0)
#define    REG_USI_SPI_VERSION          (0xD4)
#define    REG_USI_I2C_VERSION          (0xD8)
#define    REG_USI_FIFO_DEPTH           (0xDC)

// USI_OPTION
// -- USI_HWACG
#define CLKREQ_ON_HIGH                  (1 << 1)
#define CLKREQ_ON_LOW                   (0 << 1)
#define CLKSTOP_ON_HIGH                 (1 << 2)
#define CLKSTOP_ON_LOW                  (0 << 2)
#define USI_HWACG_OFF                   (CLKREQ_ON_HIGH | CLKSTOP_ON_LOW)
#define USI_HWACG_ON                    (CLKREQ_ON_LOW | CLKSTOP_ON_LOW)

#define USI_HWACG                       USI_HWACG_ON

// -- USI_MASTER
#define MASTER                          (1 << 0)

// USI_CON

// -- USI_RESET
#define USI_RESET_HIGH                  (1 << 0)
#define USI_RESET_LOW                   (0 << 0)

#define USI_RESET_ASSERT                (1 << 0)
#define USI_RESET_DEASSERT              (0 << 0)

#define USI_STATE_OPEN                  (1)
#define USI_STATE_CLOSE                 (0)

#define USI_PROTOCOL_MASK               (0x7)

#include CSP_SOURCE(usi)

#define PRINTF_ERROR   CSP_PRINTF_ERROR


static int32_t usiCheckCh(IN uint32_t port, uint32_t fid)
{
    if (port >= USI_CHANNEL_MAX) {
        PRINTF_ERROR("Invalid USI channel %lu (%lu)\n", port, fid);
        return -1;
    }

    return 0;
}

int32_t usiSetProtocol(IN uint32_t port, IN UsiProtocolType protocol)
{
    if (usiCheckCh(port, 0))
        return -1;

    // USI Reset Assert
    __raw_writel( USI_RESET_ASSERT , mUsiInfo[port].base + REG_USI_CON);

    switch (protocol)
    {
        case USI_PROTOCOL_UART:
        case USI_PROTOCOL_SPI:
        case USI_PROTOCOL_I2C:
            __raw_writel(protocol, mUsiInfo[port].sw_conf);
            break;
        default:
            PRINTF_ERROR("%s : Invalid protocol %d\n", __func__, (int)protocol);
            return -1;
    }

    // USI Reset Deassert
    __raw_writel( USI_RESET_DEASSERT , mUsiInfo[port].base + REG_USI_CON);

    __raw_writel(USI_HWACG | MASTER, mUsiInfo[port].base + REG_USI_OPTION);

    mUsiInfo[port].protocol = protocol;

    return 0;
}

// Public API to initialize USI. This should be called when OS starts
int32_t usiOpen(IN uint32_t port, IN UsiProtocolType protocol)
{
    if (usiCheckCh(port, 1))
        return -1;

    if (mUsiInfo[port].state == USI_STATE_OPEN)
        return 0;

    if (usiSetProtocol(port, protocol))
        return -1;


    mUsiInfo[port].state = USI_STATE_OPEN;

    return 0;
}

UsiProtocolType usiGetProtocol(IN uint32_t port)
{
    uint32_t regValue;

    if (usiCheckCh(port, 2))
        return USI_PROTOCOL_MAX;

    regValue = __raw_readl( mUsiInfo[port].sw_conf ) & USI_PROTOCOL_MASK;

    return (UsiProtocolType)regValue;
}

#if defined(UART_REQUIRED)
//
int32_t usiSetUartDbg(void)
{
    return usiOpen(UART_CHANNEL_DEBUG, USI_PROTOCOL_UART);
}

#endif

// Public API to save USI SFRs before entering system power mode
void usiSaveState(void)
{
    //
}

// Public API to restore USI SFRs after exiting system power mode
void usiRestoreState(void)
{
    int i;

    for (i = 0 ; i < USI_CHUB_CHANNEL_MAX ; i++){
        if (mUsiInfo[i].state == USI_STATE_OPEN)
            usiOpen(i, (UsiProtocolType)mUsiInfo[i].protocol);
    }
}

void usiCmgpSaveState(void)
{
    // Don't need to save SFRs
}

void usiCmgpRestoreState(void)
{
    // Don't need to restore SFRs
}

// Public API to shutdown USI
void usiClose(uint32_t port)
{
    if (usiCheckCh(port, 3))
        return;

    if (mUsiInfo[port].state != USI_STATE_OPEN)
        return;

    mUsiInfo[port].protocol = 0;
    mUsiInfo[port].state = USI_STATE_CLOSE;
}
