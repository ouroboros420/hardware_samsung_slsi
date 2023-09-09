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

#ifndef _VARIANT_SENS_TYPE_H_
#define _VARIANT_SENS_TYPE_H_

#ifdef __cplusplus
extern "C" {
#endif

#define SENS_TYPE_GYRO_BIAS       (SENS_TYPE_FIRST_USER + 0)
#define SENS_TYPE_MAG_BIAS        (SENS_TYPE_FIRST_USER + 1)

// SENS_TYPE_ACCEL_BIAS is not defined in AOSP.
// Define it to compile bosch_bmi160.c with ACCEL_CAL_ENABLED enabled.
// Need to check AOSP later....
#define SENS_TYPE_ACCEL_BIAS      (SENS_TYPE_FIRST_USER + 2)

/* sensor list and id sync with chre sensorhal */
enum vendor_sensor_list_id {
	sensor_list_no_active,
	sensor_list_rpr0521_prox,
	sensor_list_rpr0521_ligth,
	sensor_list_bmi160_accel,
	sensor_list_bmi160_accel_ucal,
	sensor_list_bmi160_gyro,
	sensor_list_bmi160_gyro_ucal,
	sensor_list_stlis3mdl_mag,
	sensor_list_stlis3mdl_mag_ucal,
	sensor_list_bmp280_press,
	sensor_list_max,
};

#ifdef __cplusplus
}
#endif

#endif
