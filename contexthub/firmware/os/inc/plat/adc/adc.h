/*
 * Exynos SOC - ADC
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

#ifndef __ADC_H__
#define __ADC_H__

#include <csp_common.h>

typedef enum {
    ADC_CH_0,
    ADC_CH_1,
    ADC_CH_2,
    ADC_CH_3,
    ADC_CH_4,
    ADC_CH_5,
    ADC_CH_6,
    ADC_CH_7,
    ADC_CH_8,
    ADC_CH_9,
    ADC_CH_10,
    ADC_CH_11
} AdcChType;

typedef enum {
    ADC_CONV_1TIME,
    ADC_CONV_2TIME,
    ADC_CONV_4TIME,
    ADC_CONV_8TIME,
    ADC_CONV_16TIME,
    ADC_CONV_32TIME,
    ADC_CONV_64TIME
} AdcConvModeType;


int32_t adcInit(IN AdcChType ch, IN AdcConvModeType convMode);
int32_t adcRead(IN AdcChType ch);

#endif

