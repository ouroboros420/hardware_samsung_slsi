/*----------------------------------------------------------------------------
 *      Exynos SoC  -  WDT UTC
 *----------------------------------------------------------------------------
 *      Name:    wdt_test.c
 *      Purpose: To implement wdt UTC codes
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

#include <wdt_test.h>

/*
    WDT UTC
*/
void wdt_test(void)
{
    CSP_PRINTF_INFO("wdt_test starts to make happen 5-sec watchdog interrupt\n");

    // Start 5-sec watchdog timer to make happen interrupt to AP
    wdtInit();
    wdtSetTime(5000);
}

void wdt_kick(void)
{
    CSP_PRINTF_INFO("kicking wdt\n");

    wdtPing();
}

void wdt_test_clean_up(void)
{
    // Do clean-up
    wdtDeinit();

	CSP_PRINTF_ERROR("%s is called\n", __func__);
}

#endif

