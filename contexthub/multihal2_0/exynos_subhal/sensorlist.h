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

#ifndef SENSORLIST_H_

#define SENSORLIST_H_

#include <hardware/sensors.h>

#define SENSOR_TYPE_MAX (SENSOR_TYPE_ACCELEROMETER_UNCALIBRATED + 1) /* match chub fw */

extern const float kScaleAccel;
extern const float kScaleMag;
// A list of sensors provided by a device.
extern sensor_t kSensorList[];
extern const size_t kSensorCount;

#ifdef EXYNOS_CONTEXTHUB
#define MAX_PHYSENSOR_NUM (10)
#define MAX_SENSOR_NAME (24)
#define MAX_SENSOR_VENDOR_NAME (8)

struct sensor_info {
	uint8_t sensortype;
	uint8_t senstype;
	uint8_t chipid;
	char vendorname[MAX_SENSOR_VENDOR_NAME];
	char name[MAX_SENSOR_NAME];
};

struct sensor_map {
	char magic[16];
	int index;
	struct sensor_info sinfo[MAX_PHYSENSOR_NUM];
};

struct sensor_map_pack {
    char magic[15];
    uint8_t num_os;
	struct sensor_map sensormap;
};
#endif

//for backward compatibility
struct vendor_sensor_list_t {
	const char *name;
	const char *vendor;
};

extern vendor_sensor_list_t vensor_sensor_list[];
constexpr char unactive_sensor[] = "unactive sensor";
constexpr char default_vendor[] = "exynos";

#endif  // SENSORLIST_H_
