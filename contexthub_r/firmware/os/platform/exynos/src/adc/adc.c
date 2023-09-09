/*----------------------------------------------------------------------------
 *      Exynos SoC  -  ADC
 *----------------------------------------------------------------------------
 *      Name:    adc.c
 *      Purpose: To implement ADC APIs
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
#include <csp_printf.h>

#if defined(SEOS)
    #include <cmsis.h>
    #include <atomic.h>
    #include <platform.h>
#endif

#include <adc.h>
#include <adcDrv.h>

#if defined(ADC_REQUIRED)

static int32_t adcCheckChannel(IN AdcChType ch)
{
    if (ch >= ADC_CHANNEL_MAX) {
        CSP_PRINTF_ERROR("ADC Channel error : %d \n", ch);
        return -1;
    }

    return 0;
}

int32_t adcInit(IN AdcChType ch, IN AdcConvModeType convMode)
{
    if (adcCheckChannel(ch)) {
        return -1;
    }

    return adcDrvInit(ch, convMode);
}

int32_t adcRead(IN AdcChType ch)
{
    if (adcCheckChannel(ch)) {
        return -1;
    }

    return adcDrvRead(ch);
}

#endif
