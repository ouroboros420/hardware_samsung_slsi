/*----------------------------------------------------------------------------
 *      Exynos SoC  -  GPIO
 *----------------------------------------------------------------------------
 *      Name:    gpioOS.c
 *      Purpose: OS-dependent part for GPIO driver
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
#include <csp_assert.h>
#include <csp_printf.h>
#include <gpio.h>
#include <gpioOS.h>
#include <gpioDrv.h>
#if defined(SEOS)
#include <cmsis.h>
#endif

// User call back functions for external interrupts
typedef struct {
    uint32_t enabled; // 1 means EINT is enabled and registered. 0 means INT is not enabled
    void (*mGpioDrvEINTCallback) (uint32_t);
    void *userData; // for future use
} GpioOSEINTDataType;
static GpioOSEINTDataType mGpioOSEINTData[GPIO_MAX_EINT_NUM];

// Set EINT
void gpioOSSetExtInterrupt(IN GpioEintNumType gpioEintNum, IN IntTriggerType intTrigger, IN IntFilterType intFilter, IN uint32_t intFilterWidth, IN void (*callbackFunction)(uint32_t))
{
    mGpioOSEINTData[gpioEintNum].mGpioDrvEINTCallback = callbackFunction;
    mGpioOSEINTData[gpioEintNum].enabled = 1;
    gpioDrvSetExtInterrupt(gpioEintNum, intTrigger, intFilter, intFilterWidth, callbackFunction);
#ifdef s5e9630
    gpioDrvSetExtInterruptFilterSel(gpioEintNum, eDigitalFLT);
#endif
}

// Unset EINT
void gpioOSUnsetExtInterrupt(IN GpioEintNumType gpioEintNum)
{
    gpioDrvUnsetExtInterrupt(gpioEintNum);
    mGpioOSEINTData[gpioEintNum].enabled = 0;
    mGpioOSEINTData[gpioEintNum].mGpioDrvEINTCallback = NULL;
}

// GPIO Interrupt Service Routine that sits in VIC vector table
void gpio_IRQHandler(void)
{
    GpioEintNumType i;
    uint32_t cleared = 0;

#ifdef TEST_PRINT
    CSP_PRINTF_ERROR("\n##### --------->>> %s\n\n", __func__);
#endif
    perfMonitorSetEvt(chub_prof_gpio_isr);
#if defined(s5e9610)
    NVIC_DisableIRQ(GPIO_CHUB_IRQn);
#elif defined(s5e9630)
    for(int j=0; j<21; j++)
        NVIC_DisableIRQ(EXT_INTH0_0+j);
#else
#endif

    // Check which chub-eint was happening and run callback function
    for (i = (GpioEintNumType)0; i < GPIO_CHUB_MAX_EINT_NUM; i++) {
        if (gpioDrvIsPending(i)) {
            cleared = 1;
            if (mGpioOSEINTData[i].enabled) {
                if (mGpioOSEINTData[i].mGpioDrvEINTCallback != NULL) {
                    // pending clear should be done in callback
                    mGpioOSEINTData[i].mGpioDrvEINTCallback (gpioDrvGetDataEINT(i));
                } else {
                    CSP_PRINTF_ERROR("%s, cb null (%d)\n", __func__, (int)i);
                    // pending clear
                    gpioDrvClearPend(i);
                }
            } else {
                CSP_PRINTF_ERROR("%s, int seems supurious (%d)\n", __func__, (int)i);
                // pending clear
                gpioDrvClearPend(i);
            }
        }
    }

    if(cleared == 0) {
        CSP_PRINTF_ERROR("%s, no pending int\n", __func__);
    }

#if defined(s5e9610)
    NVIC_EnableIRQ(GPIO_CHUB_IRQn);
#elif defined(s5e9630)
    for(int k=EXT_INTH0_0; k<NUM_INTERRUPTS; k++)
        NVIC_EnableIRQ(k);
#else
#endif
}

void gpioCmgpIRQHandler(IN GpioEintNumType gpioEintNum)
{
    // pending clear
    if (gpioDrvIsPending(gpioEintNum)) {
        if (mGpioOSEINTData[gpioEintNum].enabled) {
            if (mGpioOSEINTData[gpioEintNum].mGpioDrvEINTCallback != NULL) {
                // pending clear should be done in callback
                mGpioOSEINTData[gpioEintNum].mGpioDrvEINTCallback (gpioDrvGetDataEINT(gpioEintNum));
            } else {
                CSP_PRINTF_ERROR("%s, cb null (%d)\n", __func__, (int)gpioEintNum);
                // pending clear
                gpioDrvClearPend(gpioEintNum);
            }
        } else {
            CSP_PRINTF_ERROR("%s, int seems supurious (%d)\n", __func__, (int)gpioEintNum);
            // pending clear
            gpioDrvClearPend(gpioEintNum);
        }
    }
    else {
        CSP_PRINTF_ERROR("%s, int is not pending (%d)\n", __func__, (int)gpioEintNum);
    }
}

