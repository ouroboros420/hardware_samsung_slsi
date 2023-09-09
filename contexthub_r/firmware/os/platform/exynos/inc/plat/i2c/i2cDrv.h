/*----------------------------------------------------------------------------
 *      Exynos SoC  -  I2C
 *----------------------------------------------------------------------------
 *      Name:    i2cDrv.h
 *      Purpose: To expose I2C driver functions
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

#ifndef __I2C_DRIVER_H__
#define __I2C_DRIVER_H__

#include <csp_common.h>
#include <plat/i2c/i2c.h>

void i2cDrvReset(uint32_t busId);
int i2cDrvMasterOpen(uint32_t busId, uint32_t speedInHz);
int i2cDrvClose(uint32_t busId);
int i2cDrvMasterTxRx(uint32_t busId, I2cMsgType *i2cMsg);
uint32_t i2cDrvGetSpeed(uint32_t busId);


#endif

