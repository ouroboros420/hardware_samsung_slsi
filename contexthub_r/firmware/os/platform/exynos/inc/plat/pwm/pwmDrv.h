/*----------------------------------------------------------------------------
 *      Exynos SoC  -  PWM
 *----------------------------------------------------------------------------
 *      Name:    pwmDrv.h
 *      Purpose: To expose PWM driver functions
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

#ifndef __PWM_DRV_H__
#define __PWM_DRV_H__

#include <csp_common.h>
#include <pwm.h>

int pwmDrvOpen( PwmRequestType *request);
int pwmDrvClearPend( PwmTimerNumType timer);
int pwmDrvEnableInterrupt( PwmTimerNumType timer );
int pwmDrvDisableInterrupt( PwmTimerNumType timer );
int pwmDrvStartTimer( PwmTimerNumType timer );
int pwmDrvStopTimer( PwmTimerNumType timer );
void pwmDrvStopAllTimer( void );
int pwmDrvSetTime( PwmTimerNumType timer, uint32_t uSeconds );
int pwmDrvSetDuty( PwmTimerNumType timer, uint32_t duty );
int pwmDrvSetMode( PwmTimerNumType timer, PwmTimerModeType mode );
int pwmDrvSetDefaultLevel( PwmTimerNumType timer, uint32_t level );
uint32_t pwmDrvGetCount( PwmTimerNumType timer);
uint32_t pwmDrvGetCurrentTime( PwmTimerNumType timer );
uint32_t pwmDrvGetRemainTime( PwmTimerNumType timer );
int pwmDrvClose ( PwmTimerNumType timer );

#endif

