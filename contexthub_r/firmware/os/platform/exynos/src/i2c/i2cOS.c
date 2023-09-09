/*----------------------------------------------------------------------------
 *      Exynos SoC  -  I2C
 *----------------------------------------------------------------------------
 *      Name:    i2cOS.c
 *      Purpose: OS-dependent part for I2C driver
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

#if defined(SEOS)
#include "seos.h"
#endif
#include <csp_common.h>
#include <csp_assert.h>
#include <csp_printf.h>
#include <i2c.h>
#include <i2cOS.h>
#include <i2cDrv.h>

// Run callback function
void i2cOSRunCallback(I2cCallbackF callback, void *cookie, size_t txSize, size_t rxSize, int ret)
{
#if defined(SEOS)
    uint32_t curTid = osGetCurrentTid();
    uint32_t oldTid;

    oldTid = osSetCurrentTid(curTid);
#endif

    callback( cookie, txSize, rxSize, ret );

#if defined(SEOS)
    osSetCurrentTid(oldTid);
#endif
}
