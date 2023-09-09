/*----------------------------------------------------------------------------
 *      Exynos SoC  -  UTC
 *----------------------------------------------------------------------------
 *      Name:    exynos_test_api.h
 *      Purpose: UTC API Implementation
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

#ifndef __EXYNOS_TEST_API_H__
#define __EXYNOS_TEST_API_H__

#include <csp_common.h>

typedef enum {
    // DO NOT change the order.
    // If you need to add, put it just before UTC_COMMAND_STOP
    UTC_COMMAND_AGING = 0,
    UTC_COMMAND_MEM,
    UTC_COMMAND_RTC,
    UTC_COMMAND_TIMER,
    UTC_COMMAND_GPIO,
    UTC_COMMAND_SPI,
    UTC_COMMAND_CMU,
    UTC_COMMAND_WDT,
    UTC_COMMAND_TIMESYNC,
    UTC_COMMAND_STATUS,
    UTC_COMMAND_STOP, // the last command
    UTC_COMMAND_MAX = 0xFF
} UtcCommandType;

void utcCommand(UtcCommandType uc);

#endif

