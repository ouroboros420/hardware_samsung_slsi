/*----------------------------------------------------------------------------
 * Exynos SoC  -  UART
 *
 * Copyright (C) 2020 Samsung Electronics Co., Ltd.
 *
 * Shinkyu Park <shinkyu.park@samsung.com>
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

#ifndef __UART_SFR_BASE5515_H__
#define __UART_SFR_BASE5515_H__

#include <csp_common.h>

    UartInfoType mUartInfo[UART_CHANNEL_MAX] = {
        {USI0_CHUB_BASE_ADDRESS, UART_STATE_CLOSE}, // UART0_CHUB
        {USI0_CMGP_BASE_ADDRESS, UART_STATE_CLOSE}, // UART0_CMGP
        {USI1_CMGP_BASE_ADDRESS, UART_STATE_CLOSE}, // UART1_CMGP
        {USI2_CMGP_BASE_ADDRESS, UART_STATE_CLOSE}, // UART2_CMGP
        {USI3_CMGP_BASE_ADDRESS, UART_STATE_CLOSE}, // UART1_CMGP
    };

#endif

