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

#ifndef _CMSIS_H_
#define _CMSIS_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "csp_common.h"

// No group priority is used. All IRQs are just simply prioritized by Interrupt Priority Registers
// 0 is the highest priority and 0xff is the lowest one
#define __NVIC_PRIO_BITS 8

#if defined(SEOS)
    #include <cpu/cmsis/core_cm4.h>
#endif

#ifdef __cplusplus
}
#endif


#endif

