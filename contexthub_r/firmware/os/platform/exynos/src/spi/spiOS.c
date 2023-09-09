/*----------------------------------------------------------------------------
 *      Exynos SoC  -  SPI
 *----------------------------------------------------------------------------
 *      Name:    spiOS.c
 *      Purpose: OS-dependent part for SPI driver
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
#include <csp_assert.h>
#include <csp_printf.h>
#include <plat/spi/spi.h>
#include <spiOS.h>
#include <spiDrv.h>
//#include CSP_HEADER(gpioSmdk)

#if defined(SEOS)
// Initialize OS-dependent parts
static void spiOSInit(uint8_t *portNum)
{
    // To Do
    // Allocate OS resources such as heap memory, semaphore if needed
}

// Reclaim OS resources
static void spiOSDeinit(uint8_t *portNum)
{
    // To Do
    // Free OS resources allocated by Spi_OS_Init
}

static void spiOSConfig(uint8_t *portNum)
{
    // To Do
    // Config SPI as requested
}
#endif

void spiOSRunCallback(SpiCbkF callback, void *cookie, int ret)
{
#if defined(SEOS)
    uint32_t curTid = osGetCurrentTid();
    uint32_t oldTid;

    oldTid = osSetCurrentTid(curTid);
#endif

    if( callback != NULL )
        callback(cookie, ret);

#if defined(SEOS)
    osSetCurrentTid(oldTid);
#endif
}

// Deal with requests from PDMA APIs
#if defined(SEOS)
int spiOSRequest(IN uint8_t request, IN uint8_t *portNum)
{
    SPI_DEBUG_PRINT("spiOSRequest request: %d \n", request);
    switch(request)
    {
        case SPI_INIT:
            spiOSInit(portNum);
            break;
        case SPI_CONFIG:
            spiOSConfig(portNum);
            break;

        case SPI_DEINIT:
            spiOSDeinit(portNum);
            break;

        default:
            CSP_PRINTF_ERROR("spiOSRequest() - unknown request(%d)\n", request);
            CSP_ASSERT(0);
            break;
    }
    return 0;
}
#else
//Add API for embOS
#endif

