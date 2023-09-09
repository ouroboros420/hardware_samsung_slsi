/*----------------------------------------------------------------------------
 *      Exynos SoC  -  CMGP
 *----------------------------------------------------------------------------
 *      Name:    cmgp.h
 *      Purpose: To expose CMGP APIs and define macros
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

#ifndef __CMGP_H__
#define __CMGP_H__

#include <csp_common.h>
#include <csp_assert.h>
#include <csp_printf.h>

#include CSP_HEADER(cmgp)

typedef enum {
    CMGP_IRQ_GROUP_USI,
    CMGP_IRQ_GROUP_I2C,
    CMGP_IRQ_GROUP_ADC,
    CMGP_IRQ_GROUP_GPIO,
    CMGP_IRQ_GROUP_MAX
}CmgpIrqGroupType;

struct CmgpIrqInfoType {
    uint8_t    irqSource;
    uint8_t    irqGroup;
};

void cmgpInit(void);
void cmgpEnableInterrupt(uint32_t intNum);
void cmgpDisableInterrupt(uint32_t intNum);
void cmgpSetIrqPriority(uint32_t intNum, uint32_t priority);
int cmgpGetIrqNum(void);
void cmgpUSISetFunction(int usiChannel, uint32_t function);
void cmgp_IRQHandler(void);

#endif
