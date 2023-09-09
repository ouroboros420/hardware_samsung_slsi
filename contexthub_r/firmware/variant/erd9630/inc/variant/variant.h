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

#ifndef _VARIANT_EXYNOS_H_
#define _VARIANT_EXYNOS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "variantCommon.h"

//#define HALL_PIN        GPIO_PD1(2)
//#define HALL_PIN        NWEINT_GPH1_EINT_0
//#define HALL_IRQ        EXTI9_5_IRQn

// ALS
#define ALS_I2C_BUS_ID      I2C_CHUB4
#define ALS_INT_PIN
#define ALS_GPIO_PIN

// PROX Sensor
#define PROX_I2C_BUS_ID     I2C_CHUB4
#define PROX_INT_PIN        GPH2_2
#define PROX_GPIO_PIN       GPH2_2
//#define PROX_IRQ        EXTI15_10_IRQn

// BAROMETER Sensor
#define BARO_I2C_BUS_ID     I2C_CHUB3
#define BARO_INT_PIN
#define BARO_GPIO_PIN

// MAG Sensor
#define MAG_I2C_BUS_ID      I2C_CHUB2
#define MAG_INT_PIN
#define MAG_GPIO_PIN
#define MAG_DRDY_PIN

#define BMI160_TO_ANDROID_COORDINATE(x, y, z)   \
    do {                                        \
        float xi = x, yi = y, zi = z;           \
        x = -yi; y = xi; z = zi;                \
    } while (0)

#define BMM150_TO_ANDROID_COORDINATE(x, y, z)   \
    do {                                        \
        float xi = x, yi = y, zi = z;         \
        x = xi; y = -yi; z = -zi;               \
    } while (0)

//AKM_AK09918
#define AKM09918_TO_ANDROID_COORDINATE(x, y, z) \
	do { \
		float xi = x, yi = y, zi = z; \
	    x = -yi; y = -xi; z = zi; \
		}while(0)

// ACCEL & GYRO Sensor
#define ACC_GYR_SPI_BUS_ID  SPI_CHUB0
#define ACC_GYR_INT_PIN1    GPH2_0
#define ACC_GYR_GPIO_PIN1   GPH2_0
#define ACC_GYR_INT_PIN2    GPH2_1
#define ACC_GYR_GPIO_PIN2   GPH2_1

// VDD_SENSOR_EN
#define VDD_SENSOR_EN_PIN   GPH2_3

#ifdef __cplusplus
}
#endif

#endif
