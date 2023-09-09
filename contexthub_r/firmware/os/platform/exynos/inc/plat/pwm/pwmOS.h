/*----------------------------------------------------------------------------
 *      Exynos SoC  -  PWM
 *----------------------------------------------------------------------------
 *      Name:    pwmOS.h
 *      Purpose: OS-dependent part for PWM driver
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

#ifndef __PWM_OS_H__
#define __PWM_OS_H__

#include <csp_common.h>

enum PwmId {
    PwmId1      = 1,
    PwmId2      = 2,
    PwmId3      = 3,
    PwmIdMax    = 4,
};

struct PwmIsr {
    void (*func)(void);
};

void pwm0_IRQHandler_Process(void);
void pwm0_IRQHandler(void);
void pwm1_IRQHandler(void);
void pwm2_IRQHandler(void);
void pwm3_IRQHandler(void);
void pwm_SetHandler(uint8_t id, void *isr);
void pwm_UnsetHandler(uint8_t id);
#endif
