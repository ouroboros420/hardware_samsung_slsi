/*----------------------------------------------------------------------------
 *      Exynos SoC  -  CMU
 *----------------------------------------------------------------------------
 *      Name:    cmuOS.c
 *      Purpose: OS-dependent part for CMU driver
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

#include <plat/csp/csp_common.h>
#include <plat/csp/csp_assert.h>
#include <plat/csp/csp_printf.h>
#include <plat/cmu.h>
#include <plat/cmu/cmuOS.h>
#include <plat/cmu/cmuDrv.h>

// Initialize OS-dependent parts
static void cmuOSInit(void)
{
    // To Do
    // Allocate OS resources such as heap memory, semaphore if needed

    // Initialize CMU registers to make sure clock settings as we want
    cmuDrvInit();
}

// Get clock speed for the given ip
static uint32_t cmuOSGetSpeed(IN CmuIpType ip)
{
    return cmuDrvGetSpeed(ip);
}

// Set divider for the given ip
static bool cmuOSSetDivider(IN CmuIpType ip, IN uint32_t divider)
{
    return cmuDrvSetDivider(ip, divider);
}

// enable or disable drcg hwacg
static bool cmuOSControlHwacg(IN uint32_t enable)
{
    return cmuDrvControlHwacg(enable);
}

// enable or disable hwacg of each ip
static bool cmuOSControlHwacgIP(IN CmuHwacgIpType ip, IN uint32_t enable)
{
    return cmuDrvControlHwacgIP(ip, enable);
}

// Reclaim OS resources
static void cmuOSDeinit(void)
{
    // To Do
    // Free OS resources allocated by cmuOSInit

    cmuDrvDeinit();
}


// Deal with requests from CMU APIs
void cmuOSRequest(IN uint8_t request, IN CmuOSRequestType *requestPtr)
{
    bool r;

    switch(request)
    {
        case CMU_INIT:
            cmuOSInit();
            break;

        case CMU_GET_SPEED:
            requestPtr->data1 = cmuOSGetSpeed((CmuIpType)(requestPtr->data));
            break;

        case CMU_SET_DIVIDER:
            requestPtr->data1 = cmuOSSetDivider((CmuIpType)(requestPtr->data), requestPtr->data1);
            break;

        case CMU_CONTROL_HWACG:
            r = cmuOSControlHwacg(requestPtr->data);
            requestPtr->data = r;
            break;

        case CMU_CONTROL_HWACG_IP:
            r = cmuOSControlHwacgIP((CmuHwacgIpType)(requestPtr->data), requestPtr->data1);
            requestPtr->data = r;
            break;

        case CMU_DEINIT:
            cmuOSDeinit();
            break;

        default:
            CSP_PRINTF_ERROR("cmuOSRequest() - unknown request(%d)\n", request);
			CSP_ASSERT(0);
        	break;
    }
}

