/*----------------------------------------------------------------------------
 *      Exynos SoC  -  UTC UTC
 *----------------------------------------------------------------------------
 *      Name:    rtc_test.c
 *      Purpose: To implement rtc UTC codes
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

#include <rtc_test.h>
#include <ipc_chub.h>
#include <timer.h>

/*
    RTC UTC
*/
void rtc_test(void)
{
    //uint64_t rtcTick;
    uint32_t year, month, date, day, hour, min, sec;

    rtcGetSystemTime(&year, &month, &date, &day, &hour, &min, &sec);
    //rtcTick = rtcGetTimeStampNS();

    CSP_PRINTF_INFO("rtcTick(ns) %llu\n", rtcGetTimeStampNS()/*rtcTick*/);
    CSP_PRINTF_INFO("year(%u), month(%u), date(%u), day(%u), hour(%u), min(%u), sec(%u)\n", (int)year, (int)month, (int)date, (int)day, (int)hour, (int)min, (int)sec);
}

void rtc_test_clean_up(void)
{
    // Do clean-up

	CSP_PRINTF_ERROR("%s is called\n", __func__);
}

enum {
    APT,
    RTCT,
    SYST,
    TIME_MAX
};

struct timetest {
    u64 cur;
    u64 pre;
    u64 curDelta;
    u64 preDelta;
} t[TIME_MAX];

void timeSyncTest(void)
{
    int i;

    t[APT].cur = ipc_read_val(AP);
    t[RTCT].cur = (u64)rtcGetTime();
#if defined(SEOS)
    t[SYST].cur = (u64)timGetTime();
#endif

    for (i = 0; i < TIME_MAX; i++)
	    t[i].curDelta = t[i].cur - t[i].pre;

    osLog(LOG_DEBUG, "[Time] ap:%llu(%llu)\trtc:%llu(%llu)\tsys:%llu(%llu)\n",
          t[APT].cur, t[APT].curDelta,
          t[RTCT].cur, t[RTCT].curDelta,
          t[SYST].cur, t[SYST].curDelta);


    osLog(LOG_DEBUG, "[Delta] ap:%d\trtc:%d\tsys:%d\n ",
          (int)(t[APT].preDelta - t[APT].curDelta),
          (int)(t[RTCT].preDelta - t[RTCT].curDelta),
          (int)(t[SYST].preDelta - t[SYST].curDelta));

    osLog(LOG_DEBUG, "[Diff] ap<->rtc:%d\trtc<->sys:%d\n\n",
          (int)(t[APT].curDelta - t[RTCT].curDelta),
          (int)(t[RTCT].curDelta - t[SYST].curDelta));

    for (i = 0; i < TIME_MAX; i++) {
	    t[i].pre = t[i].cur;
	    t[i].preDelta = t[i].curDelta;
    }
}

#endif

