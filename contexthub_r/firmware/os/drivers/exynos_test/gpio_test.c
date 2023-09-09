/*----------------------------------------------------------------------------
 *      Exynos SoC  -  GPIO UTC
 *----------------------------------------------------------------------------
 *      Name:    gpio_test.c
 *      Purpose: To implement gpio UTC codes
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

#include <gpio_test.h>
#include CSP_HEADER(exynos_test)

static void gpio_eint_callback(uint32_t v)
{
    static uint32_t cnt = 0;
    CSP_PRINTF_INFO("EINT callback is called (%d) times\n", (int)++cnt);
}



void gpio_test(void)
{
    uint32_t cnt = 0;

    // Configure GPIO line as output direction, and set it low-level
    gpioConfig(GPIO_NO, GPIO_CON_OUTPUT);
    gpioSetData(GPIO_NO, GPIO_DAT_LOW);

    // Read the level of the GPIO
    if(gpioGetData(GPIO_NO) == GPIO_DAT_LOW) {
        CSP_PRINTF_INFO("GPIO is low\n");
    }
    else {
        CSP_PRINTF_ERROR("Oops! Wrong level\n");
    }

    // Configure GPIO line as external interrupt, and enable eint
    gpioSetExtInterrupt(EINT_NO, Low_Level, eDisFLT, 0, gpio_eint_callback);

    // Cause EINT by using NVIC STIR register
    while(1) {
#if defined(SWINT_REQUIRED)
        swintTrigger(IRQ_NO);
#endif
        if(cnt++ > MAX_COUNT_GPIO_EINT) break;
    }

    // Disable eint
    gpioUnsetExtInterrupt(EINT_NO);
}

void gpio_test_clean_up(void)
{
    // Do clean-up

	CSP_PRINTF_ERROR("%s is called\n", __func__);
}

#endif

