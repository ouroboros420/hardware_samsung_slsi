/*----------------------------------------------------------------------------
 *      Exynos SoC  -  PDMA
 *----------------------------------------------------------------------------
 *      Name:    pdmaOS.c
 *      Purpose: OS-dependent part for PDMA driver
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
#include <pdma.h>
#include <pdmaOS.h>
#include <pdmaDrv.h>

// Initialize OS-dependent parts
static void pdmaOSInit(void)
{
    // To Do
    // Allocate OS resources such as heap memory, semaphore if needed
}

// Reclaim OS resources
static void pdmaOSDeinit(void)
{
    // To Do
    // Free OS resources allocated by pdmaOSDeinit
}


// Deal with requests from PDMA APIs
void pdmaOSRequest(IN uint8_t request, IN PdmaOSRequestType *requestPtr)
{
    switch(request)
    {
        case PDMA_INIT:
            pdmaOSInit();
            break;

        case PDMA_DEINIT:
            pdmaOSDeinit();
            break;

        default:
            CSP_PRINTF_ERROR("pdmaOSRequest() - unknown request(%d)\n", request);
            CSP_ASSERT(0);
            break;
    }
}

