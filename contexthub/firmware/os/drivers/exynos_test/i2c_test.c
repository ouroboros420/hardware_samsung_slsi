/*----------------------------------------------------------------------------
 *      Exynos SoC  -  I2C UTC
 *----------------------------------------------------------------------------
 *      Name:    i2c_test.c
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
#include <i2c_test.h>
#include CSP_HEADER(exynos_test)


/*
    I2C write periodically using PWM timer
    This function intends to measure the CHUB scenario power.
*/
void i2c_test(void)
{
    uint8_t id;
    uint8_t buf[10];
    int ret;

    CSP_PRINTF_INFO("I2C TEST\n");

    if(i2cOpen( Test_I2C_BUS_ID, Test_I2C_SPEED)) {
            CSP_PRINTF_ERROR("I2C Open Error : busId=%d, speed=%d\n",
                            Test_I2C_BUS_ID, Test_I2C_SPEED);
    }

#if (RESET_REQUIRED)
    // Reset Device
    buf[0] = REG_VALUE;
    ret = i2cWrite(Test_I2C_BUS_ID, Test_I2C_ADDR, REG_RESET, 1, buf);
    if (ret != 0) {
            CSP_PRINTF_ERROR("Device Reset Error\n");
            return;
    }
#endif

    // Read ID
    ret = i2cRead(Test_I2C_BUS_ID, Test_I2C_ADDR, REG_ID, 1, &id);
    if (ret != 0) {
            CSP_PRINTF_ERROR("I2C Read Error : REG=0x%X\n", REG_ID);
            return;
    }
    CSP_PRINTF_INFO("ID = 0x%X, Read result: %s\n", id, (id==ID_VALUE)?"pass":"fail");
}

void i2c_test_clean_up(void)
{
    // Do clean-up

    CSP_PRINTF_ERROR("%s is called\n", __func__);
}

#endif


