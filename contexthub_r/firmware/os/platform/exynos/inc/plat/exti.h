/*
 * Copyright (C) 2016 The Android Open Source Project
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
	 // Will clean up codes after verification

#ifndef _EXTI_H_
#define _EXTI_H_

#include <isr.h>
#include <stdbool.h>
#include <cmsis.h>
#include <gpio.h>
#include <gpio/gpio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GPIO_HANDLE_OFFSET    1  /* to make sure that 0 stays an invalid number */
#define GPIO_PIN_MASK 0xf

enum ExtiTrigger
{
    EXTI_TRIGGER_RISING = 0,
    EXTI_TRIGGER_FALLING,
    EXTI_TRIGGER_BOTH,
};

enum ExtiLine
{
    EXTI_LINE_P0 = 0,
    EXTI_LINE_P1,
    EXTI_LINE_P2,
    EXTI_LINE_P3,
    EXTI_LINE_P4,
    EXTI_LINE_P5,
    EXTI_LINE_P6,
    EXTI_LINE_P7,
    EXTI_LINE_P8,
    EXTI_LINE_P9,
    EXTI_LINE_P10,
    EXTI_LINE_P11,
    EXTI_LINE_P12,
    EXTI_LINE_P13,
    EXTI_LINE_P14,
    EXTI_LINE_P15,
    EXTI_LINE_PVD = 16,
    EXTI_LINE_RTC_ALARM = 17,
    EXTI_LINE_USB_OTG_FS_WKUP = 18,
    EXTI_LINE_RTC_TAMPER_TS = 21,
    EXTI_LINE_RTC_WKUP = 22,
};

void extiEnableIntLine(const enum ExtiLine line, enum ExtiTrigger trigger);
void extiDisableIntLine(const enum ExtiLine line);
bool extiIsPendingLine(const enum ExtiLine line);
void extiClearPendingLine(const enum ExtiLine line);

int extiChainIsr(IRQn_Type n, struct ChainedIsr *isr);
int extiUnchainIsr(IRQn_Type n, struct ChainedIsr *isr);
int extiUnchainAll(uint32_t tid);

static inline void extiEnableIntGpio(const struct Gpio *__restrict gpioHandle, enum ExtiTrigger trigger)
{
    IntTriggerType trigType;

    if(trigger == EXTI_TRIGGER_RISING ) {
        trigType = Rising_Edge;
    } else if(trigger == EXTI_TRIGGER_FALLING ) {
        trigType = Falling_Edge;
    } else if(trigger ==EXTI_TRIGGER_BOTH ){
        trigType = Both_Edge;
    }

    if (gpioHandle) {
        gpioSetExtInterrupt((GpioEintNumType)gpioHandle, trigType, 0, 0, NULL);
    }
}

static inline void extiSetIntGpio(const struct Gpio *__restrict gpioHandle, enum ExtiTrigger trigger, const struct ChainedIsr *isr)
{
    IntTriggerType trigType;

    if(trigger == EXTI_TRIGGER_RISING ) {
        trigType = Rising_Edge;
    } else if(trigger == EXTI_TRIGGER_FALLING ) {
        trigType = Falling_Edge;
    } else if(trigger ==EXTI_TRIGGER_BOTH ){
        trigType = Both_Edge;
    }

    if (gpioHandle) {
        gpioSetExtInterrupt((GpioEintNumType)gpioHandle, trigType, 0, 0, (void *)isr->func);
    }
}
static inline void extiDisableIntGpio(const struct Gpio *__restrict gpioHandle)
{
    if (gpioHandle) {
        gpioUnsetExtInterrupt((GpioEintNumType)gpioHandle);
    }
}
static inline bool extiIsPendingGpio(const struct Gpio *__restrict gpioHandle)
{
    if (gpioHandle) {
        return gpioIsPending((GpioEintNumType)gpioHandle);
    }
    return false;
}
static inline void extiClearPendingGpio(const struct Gpio *__restrict gpioHandle)
{
    if (gpioHandle) {
        gpioClearPending((GpioEintNumType)gpioHandle);
    }
}

#ifdef __cplusplus
}
#endif

#endif
