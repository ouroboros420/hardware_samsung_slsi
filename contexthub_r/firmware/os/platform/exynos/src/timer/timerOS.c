/*----------------------------------------------------------------------------
 *      Exynos SoC  -  TIMER
 *----------------------------------------------------------------------------
 *      Name:    timerNano.c
 *      Purpose: Nano kerel dependent part for TIMER driver
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
#include <timerOS.h>
#include <plat/timer/timer.h>
#include <timerDrv.h>

#if defined(SEOS)
extern bool timIntHandler(void);
#endif

void timerIRQHandler(void)
{
    timerDrvUnset();
#if defined(SEOS)
    timIntHandler();
#endif
}
