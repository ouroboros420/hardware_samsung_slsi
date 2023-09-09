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

#ifndef _VARIANT_COMMON_EXYNOS_H_
#define _VARIANT_COMMON_EXYNOS_H_

#ifdef __cplusplus
extern "C" {
#endif

#define VARIANT_VER                 0x0000000A

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

#define MAX_SENSOR_NUM      10

//define tap sensor threshold
#define TAP_THRESHOLD   0x01

//define Accelerometer fast compensation config
#define ACC_FOC_CONFIG  0x3d

// Sensor vendor names
#define NANOHUB_VENDOR_BOSCH       UINT64_C(0x426F736368) // "Bosch"

#define VENDOR_NAME_BOSCH "Bosch"
#define VENDOR_NAME_ST    "ST"
#define VENDOR_NAME_AK    "AK"
#define VENDOR_NAME_ROHM  "Rhom"
#define VENDOR_NAME_AMS   "Ams"
#define VENDOR_NAME_TI    "TI"

enum sensor_type {
    SENSOR_TYPE_META_DATA = 0,
    SENSOR_TYPE_ACCELEROMETER = 1,
    SENSOR_TYPE_MAGNETIC_FIELD = 2,
    SENSOR_TYPE_GYROSCOPE = 4,
    SENSOR_TYPE_LIGHT = 5,
    SENSOR_TYPE_PRESSURE = 6,
    SENSOR_TYPE_TEMPERATURE = 7,
    SENSOR_TYPE_PROXIMITY = 8,
    SENSOR_TYPE_GRAVITY = 9,
    SENSOR_TYPE_LINEAR_ACCELERATION = 10,
    SENSOR_TYPE_RELATIVE_HUMIDITY = 12,
    SENSOR_TYPE_AMBIENT_TEMPERATURE = 13,
    SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED = 14,
    SENSOR_TYPE_GYROSCOPE_UNCALIBRATED = 16,
    SENSOR_TYPE_STEP_DETECTOR = 18,
    SENSOR_TYPE_STEP_COUNTER = 19,
    SENSOR_TYPE_HEART_RATE = 21,
    SENSOR_TYPE_HEART_BEAT = 31,
    SENSOR_TYPE_ACCELEROMETER_UNCALIBRATED = 35,
};

#ifdef __cplusplus
}
#endif

#endif
