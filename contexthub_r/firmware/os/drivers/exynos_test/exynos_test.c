/*----------------------------------------------------------------------------
 *      Exynos SoC  -  UTC
 *----------------------------------------------------------------------------
 *      Name:    exynos_test.c
 *      Purpose: UTC Implementation
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

#include <timer.h>
#include <aging_test.h>
#include <mem_test.h>
#include <spi_test.h>
#if defined(RTC_REQUIRED)
	#include <rtc_test.h>
#endif
#if defined(ADC_REQUIRED)
	#include <adc_test.h>
#endif
#include <timer_test.h>
#include <gpio_test.h>
#include <cmu_test.h>
#include <wdt_test.h>
#include <i2c_test.h>
#include <apm_test.h>
#if defined(MBGNSS_REQUIRED)
#include <mailboxGnss_test.h>
#include <gnssChubApi_test.h>
#endif
#include <scenario_test.h>
#include <exynos_test.h>
#include <exynos_test_api.h>
#if defined(SEOS)
	#include <platform.h>
#endif

#if defined(SEOS)
	// Thread ID
	static uint32_t mUtcTid;
#endif

#ifndef  EVT_NO_FIRST_USER_EVENT
	#define EVT_NO_FIRST_USER_EVENT 	0x00000100
#endif

// UTC Events
#define EVT_UTC_EVENT_AGING        (EVT_NO_FIRST_USER_EVENT)
#define EVT_UTC_EVENT_MEM          (EVT_NO_FIRST_USER_EVENT+1)
#define EVT_UTC_EVENT_RTC          (EVT_NO_FIRST_USER_EVENT+2)
#define EVT_UTC_EVENT_TIMER        (EVT_NO_FIRST_USER_EVENT+3)
#define EVT_UTC_EVENT_GPIO         (EVT_NO_FIRST_USER_EVENT+4)
#define EVT_UTC_EVENT_SPI          (EVT_NO_FIRST_USER_EVENT+5)
#define EVT_UTC_EVENT_CMU          (EVT_NO_FIRST_USER_EVENT+6)
#define EVT_UTC_EVENT_WDT          (EVT_NO_FIRST_USER_EVENT+7)
#define EVT_UTC_EVENT_TIMESYNC     (EVT_NO_FIRST_USER_EVENT+8)
#define EVT_UTC_EVENT_STATUS       (EVT_NO_FIRST_USER_EVENT+9)
#define EVT_UTC_EVENT_I2C          (EVT_NO_FIRST_USER_EVENT+10)
#define EVT_UTC_EVENT_SCENARIO     (EVT_NO_FIRST_USER_EVENT+11)
#define EVT_UTC_EVENT_MB_GNSS      (EVT_NO_FIRST_USER_EVENT+12)
#define EVT_UTC_EVENT_STOP         (EVT_NO_FIRST_USER_EVENT+13)


void utcCommandInternal(UtcCommandType uc, void* cookie)
{
#if defined(SEOS)
    osEnqueuePrivateEvt(EVT_UTC_EVENT_AGING + (uint32_t)uc, cookie, NULL, mUtcTid);
#endif
}

static void cleanup_utc(void)
{
    aging_test_clean_up();
    mem_test_clean_up();
#if defined(RTC_REQUIRED)
    rtc_test_clean_up();
#endif
    timer_test_clean_up();
    gpio_test_clean_up();
    spi_test_clean_up();
    cmu_test_clean_up();
    wdt_test_clean_up();
    i2c_test_clean_up();
#if defined(MBGNSS_REQUIRED)
    mailboxGnss_test_clean_up();
#endif
}

static void checkStatus(void)
{
// Checking stack overflow
#if defined(STACK_OVERFLOW_CHECK)
    extern bool check_stack_overflow(void);
    if(check_stack_overflow() == false) {
        CSP_PRINTF_INFO("[Oops] Stack check failed\n");
    }
    else {
        CSP_PRINTF_INFO("[OK] Stack check\n");
    }
#else
    CSP_PRINTF_INFO("STACK_OVERFLOW_CHECK is not enabled");
#endif

// You can add more to check other status of chub

}

static void utcHandleEvent(uint32_t evtType, const void* evtData)
{
    CSP_PRINTF_ERROR("%s: evtType: 0x%x\n", __func__, (unsigned int)evtType);

    switch (evtType) {
        case EVT_UTC_EVENT_STOP:
            cleanup_utc();
        break;
#if defined(SEOS)
        case EVT_APP_START:
        break;
#endif
        case EVT_UTC_EVENT_AGING:
            aging_test();
        break;

        case EVT_UTC_EVENT_MEM:
            mem_test();
        break;

        case EVT_UTC_EVENT_RTC:
#if defined(RTC_REQUIRED)
            rtc_test();
#endif
        break;

        case EVT_UTC_EVENT_TIMESYNC:
#if defined(RTC_REQUIRED)
            timeSyncTest();
#endif
        break;

        case EVT_UTC_EVENT_STATUS:
            checkStatus();
        break;

        case EVT_UTC_EVENT_TIMER:
            timer_test();
        break;

        case EVT_UTC_EVENT_GPIO:
            gpio_test();
        break;

        case EVT_UTC_EVENT_SPI:
            spi_test();
        break;

        case EVT_UTC_EVENT_CMU:
            cmu_test();
        break;

        case EVT_UTC_EVENT_WDT:
            wdt_test();
        break;

        case EVT_UTC_EVENT_I2C:
            i2c_test();
        break;

        case EVT_UTC_EVENT_SCENARIO:
            scenario_test();
        break;

        case EVT_UTC_EVENT_MB_GNSS:
        break;
#if defined(SEOS)
        case EVT_APP_TIMER:
        break;
#endif
        default:
            CSP_PRINTF_ERROR("[%s], Unknown command (%u)\n", __func__, (int)evtType);
        break;
    }
}

#if defined(SEOS)
static bool utcTaskStart(uint32_t taskId)
{
    mUtcTid = taskId;

    /* Kicking UTC Task to start */
    osEventSubscribe(taskId, EVT_APP_START);

    CSP_PRINTF_INFO("utcTaskStart starts...\n");

    return true;
}

static void utcTaskEnd(void)
{

}

INTERNAL_APP_INIT(APP_ID_MAKE(NANOHUB_VENDOR_EXYNOS, 0), 0, utcTaskStart, utcTaskEnd, utcHandleEvent);

#endif

#endif

