/*----------------------------------------------------------------------------
 *      Exynos SoC  -  Timer UTC
 *----------------------------------------------------------------------------
 *      Name:    timer_test.c
 *      Purpose: To implement timer UTC codes
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

#include <timer_test.h>
#include <rtc.h>

#define MS_to_NANO 1000000

// To keep track of time
static uint64_t lastTime;

// Timer value to repeat
static uint32_t timer_in_ms = 200; // 200 ms

#if defined(SEOS)
// Timer ID
static uint32_t timerTimerId;
#endif


static void timerCb(uint32_t timerId, void *cookie)
{
    uint64_t now;

    now = rtcGetTime();
    CSP_PRINTF_INFO("%d(ms)-Timer expires, elapsedTime=%llu(ns)\n", (int)timer_in_ms, now - lastTime);

    lastTime = now;
}


/*
    Start oneshot software timer
*/
void timer_test(void)
{
    lastTime = rtcGetTime();

    CSP_PRINTF_INFO("%d(ms)-Timer starts, currentTime=%llu(ns)\n", (int)timer_in_ms, lastTime);

#if defined(SEOS)
    timerTimerId = timTimerSet((uint64_t)timer_in_ms * MS_to_NANO, 0, 0, timerCb, (void *)timer_in_ms, false);
#endif
}

void timer_test_clean_up(void)
{
#if defined(SEOS)
    // Do clean-up
    timTimerCancel(timerTimerId);
#endif

    CSP_PRINTF_ERROR("%s is called\n", __func__);
}

#endif

