/*----------------------------------------------------------------------------
 *      Exynos SoC  -  CSP
 *----------------------------------------------------------------------------
 *      Name:    csp_common.h
 *      Purpose: To define common definitions for chip support package codes
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

#ifndef __CSP_COMMON_H__
#define __CSP_COMMON_H__

#include <stdint.h>
#include <stdbool.h>

// Directives for function parameters
#define IN
#define OUT
#define INOUT

// Global Macros for Chip Support Package codes
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef NULL
#define NULL 0
#endif

#include <typedef.h>

// OS Tick Resolution in milliseconds - applies to both nanohub and embos
// Examples are followings
// 1 -> Tick resolution is 1ms and we want 1000 times ticks per second
// 5 -> Tick resolution is 5ms and we want 200 times ticks per second
#define TICK_INTERNAL_IN_MS 1

#define STRINGFY(x) STR(x)
#define STR(x) #x
#define EXPAND(x) x
#define HEADERFILE(x, y) STRINGFY(EXPAND(x)EXPAND(y)EXPAND(.h))
#define SOURCEFILE(x, y) STRINGFY(EXPAND(x)EXPAND(y)EXPAND(.c))
#define CSP_HEADER(x) HEADERFILE(x,CHIP)
#define CSP_SOURCE(x) SOURCEFILE(x,CHIP)

#include CSP_HEADER(csp_common)


#include <csp_os.h>
#include <csp_assert.h>

#endif
