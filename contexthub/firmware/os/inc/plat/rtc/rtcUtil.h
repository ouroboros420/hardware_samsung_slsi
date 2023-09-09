/*----------------------------------------------------------------------------
 *      Exynos SoC  -  RTC
 *----------------------------------------------------------------------------
 *      Name:    rtcUtil.h
 *      Purpose: To Translate System time into value of seconds
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

#ifndef __RTC_UTIL_H__
#define __RTC_UTIL_H__

#include <csp_common.h>

// Public API
uint32_t rtcUtilGetSystemTimeSec(IN uint32_t year, IN uint32_t mon,
                                 IN uint32_t date, IN uint32_t hour,
                                 IN uint32_t min, IN uint32_t sec);
uint64_t rtcMkTime64(IN uint32_t year0, IN uint32_t mon0, IN uint32_t day,
                     IN uint32_t hour, IN uint32_t min, IN uint32_t sec);

#endif /*  */
