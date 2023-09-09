/*----------------------------------------------------------------------------
 *      Exynos SoC  -  CMGP
 *----------------------------------------------------------------------------
 *      Name:    cmgpOS.c
 *      Purpose: OS-dependent part for CMGP driver
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
#include <cmgp.h>
#include <cmgpOS.h>
#include <cmgpDrv.h>
#include <usi.h>
#include <plat/i2c/i2c.h>
#include <plat/gpio/gpio.h>
#include CSP_HEADER(gpio)

void cmgp_IRQHandler(void)
{
    CmgpIrqInfoType irqInfo;

    cmgpGetIrqInfo( &irqInfo);

    switch( irqInfo.irqGroup ) {
        case CMGP_IRQ_GROUP_USI:
            //usiCmgpIrqHandler(irqInfo.irqSource);
            break;
        case CMGP_IRQ_GROUP_I2C:
            //i2cCmgpIrqHandler(irqInfo.irqSource);
            break;
        case CMGP_IRQ_GROUP_ADC:
            //adcCmgpIrqHandler(irqInfo.irqSource);
            break;
        case CMGP_IRQ_GROUP_GPIO:
            gpioCmgpIRQHandler((GpioPinNumType)irqInfo.irqSource);
            break;
        default:
            CSP_ASSERT(0);
            break;
    }
}
