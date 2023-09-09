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

#ifndef _VARIANT_LUNCHBOX_H_
#define _VARIANT_LUNCHBOX_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "variantCommon.h"
//we have LSE in lunchbox
#define RTC_CLK                     RTC_CLK_LSE
// fCAL = fRTCCLK x [1 + (256 - CALM) / (2^20 + CALM - 256)]
// 32764.505 * (1 + (256 - 144)/(1048576 + 144 - 256)) = 32768.005
#define RTC_PREDIV_A                0UL
#define RTC_PREDIV_S                32759UL
#define RTC_CALM                    144UL
#define RTC_CALP                    0UL

//spi bus for comms
#define PLATFORM_HOST_INTF_SPI_BUS  0

#define SH_INT_WAKEUP               GPIO_PA(2)
#define SH_EXTI_WAKEUP_IRQ          EXTI2_IRQn
#define AP_INT_WAKEUP               GPIO_PA(3)
#undef AP_INT_NONWAKEUP

//#define DEBUG_LOG_EVT               0x3B474F4C
#define IPC_PRINT

#define BL_FLASH_TABLE                                                  \
{                                                                       \
    { (uint8_t *)(&BL),                      0x04000, BL_FLASH_BL     },\
    { (uint8_t *)(__eedata_start),           0x04000, BL_FLASH_EEDATA },\
    { (uint8_t *)(__eedata_start + 0x04000), 0x04000, BL_FLASH_EEDATA },\
    { (uint8_t *)(__code_start),             0x04000, BL_FLASH_KERNEL },\
    { (uint8_t *)(__code_start + 0x04000),   0x10000, BL_FLASH_KERNEL },\
    { (uint8_t *)(__code_start + 0x10000),   0x20000, BL_FLASH_KERNEL },\
    { (uint8_t *)(__shared_start),           0x20000, BL_FLASH_SHARED },\
    { (uint8_t *)(__shared_start + 0x20000), 0x20000, BL_FLASH_SHARED },\
}

#define BMI160_TO_ANDROID_COORDINATE(x, y, z)   \
    do {                                        \
        float xi = x, yi = y, zi = z;           \
        x = -yi; y = -xi; z = -zi;                \
    } while (0)

#define BMM150_TO_ANDROID_COORDINATE(x, y, z)   \
    do {                                        \
        float xi = x, yi = y, zi = z;         \
        x = xi; y = -yi; z = -zi;               \
    } while (0)

//#define HALL_PIN        GPIO_PD1(2)
//#define HALL_PIN        NWEINT_GPH1_EINT_0
//#define HALL_IRQ        EXTI9_5_IRQn

// ALS & PROX Sensor
#define ALSP_I2C_BUS_ID     I2C_CMGP1
#define ALS_I2C_BUS_ID      I2C_CMGP1
#define PROX_I2C_BUS_ID     ALSP_I2C_BUS_ID
#define PROX_INT_PIN        GPM07_0
#define PROX_GPIO_PIN       GPM07_0
#define PROX_IRQ

// BAROMETER Sensor
#define BARO_I2C_BUS_ID     I2C_CMGP1
#define BARO_INT_PIN
#define BARO_GPIO_PIN

// MAG Sensor
#define MAG_I2C_BUS_ID      I2C_CMGP1
#define MAG_INT_PIN
#define MAG_GPIO_PIN
#define MAG_DRDY_PIN

//AKM_AK09918
#define AKM09918_TO_ANDROID_COORDINATE(x, y, z) \
	do { \
		float xi = x, yi = y, zi = z; \
	    x = xi; y = yi; z = zi; \
		}while(0)


#define ACCEL_GYRO_ROT_MATRIX  0, 1, 0, -1, 0, 0, 0, 0, 1

// BMI26X Configuration
#define BMI26X_TO_ANDROID_COORDINATE(x, y, z)   \
    do {                                        \
        float xi = x, yi = y, zi = z;           \
        x = -yi; y = xi; z = zi;                \
    } while (0)

#define BMI26X_INT1_IRQ         INTREQ_EXT_INTE1_IRQn//EXTINT_GPH01_0_IRQn
#define BMI26X_INT1_PIN         GPE01_0//GPM13_0//GPH01_0
#undef USE_INT2
#define BMI26X_INT2_IRQ
#define BMI26X_INT2_PIN

// ACCEL & GYRO Sensor
#define ACC_GYR_SPI_BUS_ID  SPI_CMGP0
#define ACC_GYR_INT_PIN1    GPM06_0
#define ACC_GYR_GPIO_PIN1   GPM06_0
#define ACC_GYR_INT1_IRQ    GPM06_0
#define BMI26X_ACC_SAMPLES  1000
#if (SUPPORT_ACC_GYR_INT_PIN2)
#define ACC_GYR_INT2_PIN
#define ACC_GYR_INT2_IRQ
#endif
#define SUPPORT_ACC_GYR_INT_PIN2    0
#define ACC_GYR_SPI_CLOCK   8000000

#define ACCEL_CALIB_ENABLED  0
#define GYRO_CALIB_ENABLED   0

// VDD_SENSOR_EN
#define VDD_SENSOR_EN_PIN


#define MAX_SENSOR_NUM      10


//define tap sensor threshold
#define TAP_THRESHOLD   0x01

//define Accelerometer fast compensation config
#define ACC_FOC_CONFIG  0x3d

#define SLIF_TX_BUF_SIZE 512 // Should be multiple of 4
#define SLIF_RX_BUF_SIZE 512 // Should be multiple of 4


#ifdef __cplusplus
}
#endif

#endif
