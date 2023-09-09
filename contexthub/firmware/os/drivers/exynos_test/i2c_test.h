/*----------------------------------------------------------------------------
 *      Exynos SoC  -  I2C UTC
 *----------------------------------------------------------------------------
 *      Name:    i2c_test.h
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

#ifndef __I2C_TEST_H__
#define __I2C_TEST_H__

#include <csp_common.h>
#include <csp_assert.h>
#include <csp_printf.h>
#include <timer.h>
#include <pwm.h>
#include <pwmOS.h>
#include <i2c.h>
#include <plat/i2c/i2c.h>
#include <plat/gpio/gpio.h>
#include CSP_HEADER(gpio)
#include CSP_HEADER(exynos_test)

void i2c_test(void);
void i2c_test_clean_up(void);

#endif
