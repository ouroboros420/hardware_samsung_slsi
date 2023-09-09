/*----------------------------------------------------------------------------
 *      Exynos SoC  -  SCENARIO UTC
 *----------------------------------------------------------------------------
 *      Name:    scenario_test.c
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

#include <scenario_test.h>

// PWM Timer value to repeat
static uint32_t timer_in_20ms = 20; // 20 ms
static uint32_t timer_in_100ms = 100; // 100 ms

// I2C related settings
#if  defined(s5e9610)
#define Test_I2C0_BUS_ID                          I2C_CMGP0
#define Test_I2C1_BUS_ID                          I2C_CMGP1
#endif
#define Test_I2C0_ADDR                                0x30
#define Test_I2C1_ADDR                                0x60
#define Test_I2C_SPEED                               400000
#if defined(s5e9610)
static uint32_t txrxBuf1[2] = {0x7d, 0x5a};
#endif

//GPIO value
#define Pwm1TestGpio GPIO_M02_0
#define Pwm2TestGpio GPIO_M03_0

uint32_t CntHdl_1, CntHdl_2;
uint32_t agingTest;

#if defined(PWM_REQUIRED)
static void pwm1_TestIRQHandler(void)
{
    // Do Something
    gpioSetData((GpioPinNumType)Pwm1TestGpio,(uint32_t)0x1);/*Set gpio output HIGH */

    gpioSetData((GpioPinNumType)Pwm1TestGpio,(uint32_t)0x0);/*Set gpio output LOW */
    CntHdl_1++;
}

static void pwm2_TestIRQHandler(void)
{
    // Do Something
    gpioSetData((GpioPinNumType) Pwm2TestGpio,(uint32_t)0x1);/*Set gpio output HIGH */

#if defined(s5e9610)
    if(i2cMasterTx(Test_I2C1_BUS_ID, Test_I2C1_ADDR, txrxBuf1, 2, NULL, NULL))
        CSP_PRINTF_INFO("%s: i2cMasterTx error!\n",__FUNCTION__);
#endif

    gpioSetData((GpioPinNumType) Pwm2TestGpio,(uint32_t)0x0);/*Set gpio output LOW */
    CntHdl_2++;
}
#endif

/*
    I2C write periodically using PWM timer
    This function intends to measure the CHUB scenario power.
*/
void scenario_test(void)
{
    CSP_PRINTF_INFO("%s: Timers(%d(ms) and %d(ms)) are happening periodically and send I2C command\n"
        ,__FUNCTION__ , (int)timer_in_20ms, (int)timer_in_100ms);

#if defined(s5e9610)
    i2cMasterRequest(Test_I2C1_BUS_ID, Test_I2C_SPEED);
#endif

#if defined(PWM_REQUIRED)
#if defined(s5e9610)
    gpioConfig((GpioPinNumType) GPIO_M02_0,(uint32_t) 0x1);
    gpioConfig((GpioPinNumType) GPIO_M03_0,(uint32_t) 0x1);
#endif
    pwmInit(PWM_TIMER1, timer_in_20ms*1000, PWM_TIMER_MODE_INTERVAL, 1);
    pwmInit(PWM_TIMER2, timer_in_100ms*1000, PWM_TIMER_MODE_INTERVAL, 1);

    pwm_SetHandler((uint8_t)PWM_TIMER1, (void*)pwm1_TestIRQHandler);
    pwm_SetHandler((uint8_t)PWM_TIMER2, (void*)pwm2_TestIRQHandler);

    pwmStartTimer(PWM_TIMER1);
    pwmStartTimer(PWM_TIMER2);
#else
#if defined(s5e9610)
    i2cMasterTx(Test_I2C1_BUS_ID, Test_I2C1_ADDR, txrxBuf1, 2, NULL, NULL);
#endif
#endif
    if(!agingTest){
        if(CntHdl_2 > 30*10){
            pwmStopTimer(PWM_TIMER1);
            pwmStopTimer(PWM_TIMER2);
        }
    }
}

void scenario_test_clean_up(void)
{
    // Do clean-up

    CSP_PRINTF_ERROR("%s is called\n", __func__);
}

#endif
