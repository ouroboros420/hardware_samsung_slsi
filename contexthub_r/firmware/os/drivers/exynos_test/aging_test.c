/*----------------------------------------------------------------------------
 *      Exynos SoC  -  Aging UTC
 *----------------------------------------------------------------------------
 *      Name:    aging_test.c
 *      Purpose: To implement aging-test codes
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

#if defined(UTC_REQUIRED)

#include <aging_test.h>
#include <mem_test.h>
#include <cmu_test.h>
#include <exynos_test.h>
#include <exynos_test_api.h>

// Aging Test Flat
static uint32_t onAgingTest;

#if defined(SEOS)
// Thread ID
static uint32_t agingTimerId;

static void agingCb(uint32_t timerId, void *cookie)
{
    // Infinite repeation
    utcCommandInternal(UTC_COMMAND_AGING, cookie);
}
#endif

/*
    Start aging-test
*/
void aging_test(void)
{
    onAgingTest = 1;
    // Memory alloc copy, compare and free
    mem_test();

    // Print out clock information
    cmu_test();

#if defined(SEOS)
    // Timer kicking to repeat
    agingTimerId = timTimerSet(1000000000, 0, 0, agingCb, NULL, true);
#endif
}


void aging_test_clean_up(void)
{
    onAgingTest = 0;

#if defined(SEOS)
    // Do clean-up
    timTimerCancel(agingTimerId);
#endif

    CSP_PRINTF_ERROR("%s is called\n", __func__);
}

#endif

