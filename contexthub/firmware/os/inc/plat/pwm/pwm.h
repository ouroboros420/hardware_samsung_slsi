/*----------------------------------------------------------------------------
 *      Exynos SoC  -  PWM
 *----------------------------------------------------------------------------
 *      Name:    pwm.h
 *      Purpose: To expose PWM APIs and define macros
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

#ifndef __PWM_H__
#define __PWM_H__

#include <csp_common.h>
#include <csp_assert.h>
#include <csp_printf.h>

#define LOW       0
#define HIGH      1

typedef enum {
    PWM_TIMER_NUM_MIN,
    PWM_TIMER0 = PWM_TIMER_NUM_MIN, // Only for Variable SysTick
    PWM_TIMER1,
    PWM_TIMER2,
    PWM_TIMER3,
#if (SUPPORT_PWM_TIMER4)
    PWM_TIMER4,
#endif
    PWM_TIMER_NUM_MAX
} PwmTimerNumType;

typedef enum {
    PWM_TIMER_MODE_ONESHOT,
    PWM_TIMER_MODE_INTERVAL,
    PWM_TIMER_MODE_MAX
} PwmTimerModeType;

#if defined(FIRMWARE_ONLY)
  #define PWM_MAX_TIME_TO_EXPIRE_IN_MICROSECOND 3000000 // 3 seconds
#else
  #define PWM_MAX_TIME_TO_EXPIRE_IN_MICROSECOND 100000000 // 100 seconds
#endif
#define PWM_MAX_TICK_TO_EXPIRE ((300000000 / 1000) / TICK_INTERNAL_IN_MS)

int pwmInit( PwmTimerNumType timer, uint32_t uSecond,
             PwmTimerModeType mode, uint32_t intEn );
int pwmEnableInterrut( PwmTimerNumType timer );
int pwmDisableInterrupt( PwmTimerNumType timer );
int pwmStartTimer( PwmTimerNumType timer );
int pwmStopTimer( PwmTimerNumType timer );
void pwmStopAllTimer( void );
int pwmSetTime( PwmTimerNumType timer, uint32_t uSecond );
int pwmSetDuty( PwmTimerNumType timer, uint32_t duty );
int pwmSetMode( PwmTimerNumType timer, PwmTimerModeType mode );
int pwmSetDefaultLevel( PwmTimerNumType timer, uint32_t level );
int pwmEnableInterrupt(PwmTimerNumType timer );
int pwmDisableInterrupt(PwmTimerNumType timer );
uint32_t pwmGetCount( PwmTimerNumType timer);
uint32_t pwmGetCurrentTime( PwmTimerNumType timer);
#if defined(LOCAL_POWERGATE)
void pwmSaveState(void);
void pwmRestoreState(void);
#endif
uint64_t pwmGetTimeStampUS(void);
int pwmClose( PwmTimerNumType timer );
void pwmSetHandler(PwmTimerNumType timer, void (*handler)(void));
void pwm0_IRQHandler(void);
void pwm1_IRQHandler(void);
void pwm2_IRQHandler(void);
void pwm3_IRQHandler(void);
void pwm4_IRQHandler(void);

#endif

