/*----------------------------------------------------------------------------
 *      Exynos SoC  -  Scenario UTC
 *----------------------------------------------------------------------------
 *      Name:    scenario_test.h
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

#ifndef __SCENARIO_TEST_H__
#define __SCENARIO_TEST_H__

#include <csp_common.h>
#include <csp_assert.h>
#include <csp_printf.h>
#include <timer.h>
#include <pwm.h>
#include <pwmOS.h>
#include <i2c.h>
#include <plat/gpio/gpio.h>
#include CSP_HEADER(gpio)

void scenario_test(void);
void scenario_test_clean_up(void);

#endif
