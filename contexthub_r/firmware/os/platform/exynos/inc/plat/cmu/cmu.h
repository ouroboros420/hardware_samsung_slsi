/*----------------------------------------------------------------------------
 *      Exynos SoC  -  CMU
 *----------------------------------------------------------------------------
 *      Name:    cmu.h
 *      Purpose: To expose CMU APIs and define macros
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

#ifndef __CMU_H__
#define __CMU_H__

#include <csp_common.h>
#include <csp_assert.h>
#include <csp_printf.h>

#include CSP_HEADER(cmu)

typedef enum {
    CMU_HWACG_CONTROL_DISABLE = 0,
    CMU_HWACG_CONTROL_ENABLE
} CmuHwacgControlType;

#if defined(HWACG_OFF)
#define CMU_DEFAULT_HWACG CMU_HWACG_CONTROL_DISABLE
#else
#define CMU_DEFAULT_HWACG CMU_HWACG_CONTROL_ENABLE
#endif

// Public API
void cmuInit(uint32_t mainclk);
uint32_t cmuGetSpeed(IN CmuIpType ip);
int cmuSetCpuClk(uint32_t freq);
bool cmuSetDivider(IN CmuIpType ip, IN uint32_t divider);
uint32_t cmuGetCpuSpeed(void);
uint32_t cmuGetReloadValForOSTick(void);
bool cmuControlHwacg(IN CmuHwacgControlType enable);
bool cmuControlHwacgIP(IN CmuHwacgIpType ip, IN CmuHwacgControlType enable);
#endif

