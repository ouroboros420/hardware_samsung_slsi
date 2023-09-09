/*----------------------------------------------------------------------------
 *      Exynos SoC  -  CMU
 *----------------------------------------------------------------------------
 *      Name:    cmu9110.h
 *      Purpose: To expose CMU APIs and define macros
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

#ifndef __CMU9110_H__
#define __CMU9110_H__

typedef enum {
    CMU_CLK_OSCCLK_RCO = 0,
    CMU_CLK_RTCCLK,
    CMU_CLK_OUTPUT_CMUCMU,
    CMU_CLK_OUTPUT_CMUAPM,
    CMU_CLK_OUTPUT_CPU,
    CMU_CLK_CHUB_TIMER,
    CMU_CLK_CHUB_BUS,
    CMU_CLK_CHUB_WDT,

    CMU_CLK_CMGP_I2C00,
    CMU_CLK_CMGP_I2C01,
    CMU_CLK_CMGP_I2C02,
    CMU_CLK_CMGP_I2C03,
    CMU_CLK_CMGP_USI00,
    CMU_CLK_CMGP_USI01,
    CMU_CLK_CMGP_USI02,
    CMU_CLK_CMGP_USI03,

    CMU_CLK_I2C,
    CMU_CLK_I2C00 = CMU_CLK_I2C,
    CMU_CLK_I2C01,
    CMU_CLK_I2C02,
    CMU_CLK_I2C03,
    CMU_CLK_I2C04,
    CMU_CLK_I2C05,
    CMU_CLK_I2C06,
    CMU_CLK_I2C07,
    CMU_CLK_I2C08,
    CMU_CLK_I2C09,
    CMU_CLK_I2C10,

    CMU_MAX_IP_TYPE
} CmuIpType;

typedef enum {
    CMU_HWACG_IP_CM4 = 0,
    CMU_HWACG_IP_I2C_CHUB00,
    CMU_HWACG_IP_I2C_CHUB01,
    CMU_HWACG_IP_PDMA,
    CMU_HWACG_IP_PWM,
    CMU_HWACG_IP_TIMER,
    CMU_HWACG_IP_WDT,

    CMU_HWACG_MAX_IP_TYPE
} CmuHwacgIpType;

typedef enum {
    CMU_OSC_CLK = 0,
    CMU_RCO_CLK,
    CMU_CHUB_BUS_CLK,
    CMU_SRC_MAX_CLK
} ChubClkSrcType;

// Constant values of clocks =============
#define CLKCMU_RTC              32768 // 32.768Khz
#define CLKCMU_XTAL             26000000 // 26Mhz
#define CLKCMU_RCO_ALV          49152000
#define CLKCMU_CHUB_DLL_USER    360000000 // 360Mhz
#define CLKCMU_CMGP_BUS         400000000 // 400Mhz
#define CLKCMU_APM_DLL_CMGP     180000000 // 180Mhz

#define CHUB_MAIN_CLK           CLKCMU_RCO_ALV

#endif

