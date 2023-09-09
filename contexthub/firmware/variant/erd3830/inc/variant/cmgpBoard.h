/*----------------------------------------------------------------------------
 *      EXYNOS - CMGP
 *----------------------------------------------------------------------------
 *      Name:    cmgpBoard.h
 *      Purpose: To expose CMGP Board APIs
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

#ifndef __CMGP_BOARD_H__
#define __CMGP_BOARD_H__

#include <csp_common.h>
#include <csp_printf.h>
#include <boardCommon.h>

static UsiBoardConfigType mUsiBoardConfig[] = {
    {USI_CMGP0,         USI_PROTOCOL_SPI},
    {USI_CMGP1,         USI_PROTOCOL_I2C},
};

static CmgpIrqBoardConfigType mCmgpIrqBoardConfig[] = {
	//{CMGP_USI0_IRQ,  8},
	//{CMGP_USI1_IRQ,  8},
	{CMGP_GPM06_IRQ,  8},
	//{CMGP_GPM07_IRQ,  8},
};

#endif

