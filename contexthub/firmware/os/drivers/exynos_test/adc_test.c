/*----------------------------------------------------------------------------
 *      Exynos SoC  -  ADC UTC
 *----------------------------------------------------------------------------
 *      Name:    adc_test.c
 *      Purpose: To implement ADC UTC codes
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

#if defined(ADC_REQUIRED)

#include <adc_test.h>

/*
    ADC UTC
*/
void adc_test(void)
{
    int ch;
    int val;

    for (ch = 0 ; ch < ADC_CHANNEL_MAX ; ch++) {
        adcInit(ch, ADC_CONV_64TIME);
    }

    for (ch = 0 ; ch < ADC_CHANNEL_MAX ; ch++) {
        val = adcRead(ch);
        CSP_PRINTF_INFO("ADC TEST : Ch(%d) = %d\n", ch, val);
    }
}

#endif

