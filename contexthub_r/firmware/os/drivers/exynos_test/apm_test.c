/*----------------------------------------------------------------------------
 *      Exynos SoC  -  APM UTC
 *----------------------------------------------------------------------------
 *      Name:    apm_test.c
 *      Purpose: To implement i2c UTC codes
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

#include <csp_common.h>
#include <apm_test.h>
#include CSP_HEADER(exynos_test)
#include <sysreg.h>

void apm_test(void)
{
    CSP_PRINTF_INFO("%s is called\n", __func__);
}

void apm_test_clean_up(void)
{
    // Do clean-up

    CSP_PRINTF_INFO("%s is called\n", __func__);
}
#endif
