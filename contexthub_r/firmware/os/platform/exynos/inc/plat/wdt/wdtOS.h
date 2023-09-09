/*----------------------------------------------------------------------------
 *      Exynos SoC  -  WDT
 *----------------------------------------------------------------------------
 *      Name:    wdtOS.h
 *      Purpose: OS-dependent part for WDT driver
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

#ifndef __WDT_OS_H__
#define __WDT_OS_H__

#include <csp_common.h>
#if defined(SEOS)
	#include <cmsis.h>
#endif

// Requests from Public APIs
#define WDT_INIT               0
#define WDT_ENABLE             1
#define WDT_DISABLE            2

#define WDT_DEINIT             99

void wdt_IRQHandler(void);

#endif
