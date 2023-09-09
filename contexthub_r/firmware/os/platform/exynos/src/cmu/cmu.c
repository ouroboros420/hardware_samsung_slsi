/*----------------------------------------------------------------------------
 *      Exynos SoC  -  CMU
 *----------------------------------------------------------------------------
 *      Name:    cmu.c
 *      Purpose: To implement CMU APIs
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

#include <cmu.h>
#include <cmuDrv.h>

#include CSP_SOURCE(cmuDrv)

// Public API to initialize CMU. This should be called when OS starts
void cmuInit(uint32_t mainclk)
{
    cmuDrvInit(mainclk);
}

//
uint32_t cmuGetSpeed(IN CmuIpType ip)
{
    if (ip >= CMU_MAX_IP_TYPE) {
        CSP_ASSERT(0);
        return 0;
    }

    return cmuDrvGetSpeed(ip);
}

bool cmuSetDivider(IN CmuIpType ip, IN uint32_t divider)
{
    bool ret;

    // Need to update check routines to make sure supported IPs
    if (ip >= CMU_MAX_IP_TYPE) {
        return false;
    }

    if(divider > 15)
    {
        return false;
    }

    ret = cmuDrvSetDivider(ip, divider);
    if (ip == CMU_CLK_CHUB_BUS)
        CSP_PRINTF_INFO("CPU is operating @ %luHz\n", cmuGetCpuSpeed());

    return ret;
}

uint32_t cmuGetCpuSpeed(void)
{
    return cmuGetSpeed(CMU_CLK_OUTPUT_CPU);
}

int cmuSetCpuClk(uint32_t freq)
{
    if (cmuDrvSetCpuClk(freq)) {
        CSP_PRINTF_ERROR("cmuSetCpuClk fail\n");
        return -1;
    }
    CSP_PRINTF_INFO("CPU is operating @ %luHz\n", cmuGetCpuSpeed());
    return 0;
}

uint32_t cmuGetReloadValForOSTick(void)
{
    uint32_t cpuSpeed = 0;

    cpuSpeed = cmuGetCpuSpeed();
    CSP_PRINTF_INFO("CPU is operating @ %uHz\n", (unsigned int)cpuSpeed);
    cpuSpeed /= (1000 / TICK_INTERNAL_IN_MS);

    // SYST_RVR is 24bit-wide, thus the value to set has to be less than 0x1000000
    if(cpuSpeed >= 0x1000000) {
        CSP_ASSERT(0);
        return 0;
    }

    return cpuSpeed;
}

bool cmuControlHwacg(IN CmuHwacgControlType enable)
{
    return cmuDrvControlHwacg(enable);
}

bool cmuControlHwacgIP(IN CmuHwacgIpType ip, IN CmuHwacgControlType enable)
{
    return cmuDrvControlHwacgIP(ip, enable);
}

