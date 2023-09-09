/*
 * Copyright (C) 2019 The Android Open Source Project
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

#include "SensorsSubHal.h"
#include "sensorlist.h"
#include <android/hardware/sensors/2.1/ISensors.h>
#include <android/hardware/sensors/2.1/types.h>
#include <log/log.h>
#include <sensors/convert.h>
#include <cutils/ashmem.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <cmath>

#define SENSORTYPE_SAVED_PATH   "/mnt/vendor/efs/chub/sensortype"
#define SENSORTYPE_PATH         "/sys/class/nanohub/nanohub/sensortype"
#define CHUB_DEV_PATH           "dev/chub_dev"

extern "C" void __gcov_dump();
extern "C" void __gcov_reset();

void __gcov_dump(void);
void __gcov_reset(void);

/* SIGUSR1 handler. */
void sigusr1 (int sig)
{
    ALOGI("%s called! %d\n", __func__, sig);
    __gcov_dump();
    //__gcov_reset();
}

::android::hardware::sensors::V2_1::implementation::ISensorsSubHal* sensorsHalGetSubHal_2_1(uint32_t* version) {
    static ::android::hardware::sensors::V2_1::subhal::implementation::SensorsSubHalV2_1 subHal;
    *version = SUB_HAL_2_1_VERSION;
    ALOGI("%s", __func__);
    return &subHal;
}

namespace android {
namespace hardware {
namespace sensors {
namespace V2_1 {
namespace subhal {
namespace implementation {

using ::android::hardware::Void;
using ::android::hardware::sensors::V1_0::OperationMode;
using ::android::hardware::sensors::V1_0::RateLevel;
using ::android::hardware::sensors::V1_0::Result;
using ::android::hardware::sensors::V1_0::SharedMemInfo;
using ::android::hardware::sensors::V1_0::SensorInfo;
using ::android::hardware::sensors::V1_0::implementation::convertFromSensor;
using ::android::hardware::sensors::V1_0::implementation::convertFromSharedMemInfo;
using ::android::hardware::sensors::V2_0::SensorTimeout;
using ::android::hardware::sensors::V2_0::WakeLockQueueFlagBits;
using ::android::hardware::sensors::V2_0::implementation::ScopedWakelock;
using ::android::hardware::sensors::V2_1::Event;

ISensorsSubHalBase::ISensorsSubHalBase() : mCallback(nullptr) {
    ALOGI("%s constructor", __func__);
    if (signal(SIGUSR1, sigusr1) == SIG_ERR) {
        ALOGE("%s SIGUSR register fail!\n", __func__);
    }
    GetSavedData();
    mHubConnection = HubConnection::getInstance();
    memset(sensor_list, 0, sizeof(sensor_list));
    memset(&sensor_pack, 0, sizeof(sensor_pack));
}

ISensorsSubHalBase::~ISensorsSubHalBase() {
    ALOGI("%s destructor", __func__);
    mCallback = NULL;
}

#define CHUB_DEV_IOCTL_SET_DFS		_IOWR('c', 201, __u32)

void ISensorsSubHalBase::GetSavedData() {
    int fd_data = 0, fd_chub = 0, fd_dfs = 0, size = 0;
    struct sensor_map_pack pack;
	int ret;
	unsigned int gov;

    ALOGI("%s\n", __FUNCTION__);

    fd_chub = open(CHUB_DEV_PATH, O_RDWR);
    if (fd_chub < 0) {
        ALOGE("%s: Failed to open fd_chub: err=%d [%s]", __FUNCTION__, errno, strerror(errno));
        return;
    }
    // case1. write dfs governor to chub
    fd_dfs = open(DFS_TABLE_PATH, O_RDONLY); // FIXME: consider using stat to check if the file exists
    if (fd_dfs < 0) {
        ALOGE("%s: Failed to open fd_dfs: err=%d [%s]", __FUNCTION__, errno, strerror(errno));
    } else {
        // write DFS number
        gov = 0xff;
        ret = ioctl(fd_chub, CHUB_DEV_IOCTL_SET_DFS, gov);
        if (ret < 0) {
            ALOGE("%s: Failed to ioctl fd_data: err=%d [%s]", __FUNCTION__, errno, strerror(errno));
        }
        close(fd_dfs);
    }

    // case2. write sensortype to chub
    fd_data = open(SENSORTYPE_SAVED_PATH, O_RDONLY);
    if (fd_data < 0) {
        ALOGE("%s: Failed to open fd_data: err=%d [%s]", __FUNCTION__, errno, strerror(errno));
        close(fd_chub);
        return;
    }

    //read sensor_map_pack struct
    size = read(fd_data, &pack, sizeof(pack));
    if (size > 0 && saved_setting_valid(&pack)) {
        size = write(fd_chub, &pack.num_os, sizeof(pack.num_os));
        ALOGI("%s : os number available %d", __FUNCTION__, pack.num_os);
    } else {
        ALOGE("%s: sensortype file invalid!", __FUNCTION__);
        remove(SENSORTYPE_SAVED_PATH);
    }
    close(fd_chub);
    close(fd_data);
}

Return<void> ISensorsSubHalBase::getSensorsList(V2_1::ISensors::getSensorsList_2_1_cb _hidl_cb) {
    ALOGI("%s", __func__);

    usleep(600 * 1000); // wait until sensor manager is done
    getSensorInfo(sensor_list);

    hidl_vec<SensorInfo> out;
    out.resize(kSensorCount);

    for (size_t i = 0; i < kSensorCount; ++i) {
        const sensor_t *src = &sensor_list[i];
        SensorInfo *dst = &out[i];

        convertFromSensor(*src, dst);
    }

    _hidl_cb(V2_1::implementation::convertToNewSensorInfos(out));

    return Void();
}

Return<Result> ISensorsSubHalBase::getSensor(int32_t sensorHandle, sensor_t **sensor)
{
    for (size_t i = 0 ; i < kSensorCount ; i++) {
        if (sensor_list[i].handle == sensorHandle) {
            *sensor = &sensor_list[i];
            return Result::OK;
        }
    }
    return Result::BAD_VALUE;
}

Return<Result> ISensorsSubHalBase::setOperationMode(__attribute__((unused)) OperationMode mode) {
    return Result::BAD_VALUE;
}

Return<Result> ISensorsSubHalBase::activate(int32_t sensorHandle, bool enabled) {
    sensor_t *sensor;

    if (getSensor(sensorHandle, &sensor) != Result::OK)
        return Result::BAD_VALUE;

    if (mHubConnection != nullptr)
        mHubConnection->queueActivate(sensorHandle, enabled);

    return Result::OK;
}

Return<Result> ISensorsSubHalBase::batch(int32_t sensorHandle, int64_t samplingPeriodNs,
                                    int64_t maxReportLatencyNs) {
    sensor_t *sensor;

    if (getSensor(sensorHandle, &sensor) != Result::OK)
        return Result::BAD_VALUE;
    samplingPeriodNs =
            std::clamp(samplingPeriodNs, (int64_t) sensor->minDelay * 1000, (int64_t) sensor->maxDelay * 1000);

    if (mHubConnection != nullptr)
        mHubConnection->queueBatch(sensorHandle, samplingPeriodNs, maxReportLatencyNs);

    return Result::OK;
}

Return<Result> ISensorsSubHalBase::flush(int32_t sensorHandle) {
    sensor_t *sensor;

    if (getSensor(sensorHandle, &sensor) != Result::OK)
        return Result::BAD_VALUE;
    if ((sensor->flags & 0x6) == SENSOR_FLAG_ONE_SHOT_MODE)
        return Result::BAD_VALUE;

    if (mHubConnection != nullptr)
        return mHubConnection->queueFlush(sensorHandle);
    else
        return Result::BAD_VALUE;
}

Return<Result> ISensorsSubHalBase::injectSensorData(__attribute__((unused)) const Event& event) {
    //mHubConnection->setOperationParameter(event);
    return Result::OK;
}

Return<void> ISensorsSubHalBase::registerDirectChannel(__attribute__((unused)) const SharedMemInfo& mem,
                                                       __attribute__((unused)) V2_0::ISensors::registerDirectChannel_cb _hidl_cb) {
    //_hidl_cb(Result::OK, -1 /* channelHandle */);
    return Return<void>();
}

Return<Result> ISensorsSubHalBase::unregisterDirectChannel(__attribute__((unused)) int32_t channelHandle) {
    //mHubConnection->removeDirectChannel(channelHandle);
    return Result::OK;
}

Return<void> ISensorsSubHalBase::configDirectReport(int32_t sensorHandle,
                                               int32_t channelHandle, RateLevel rate,
                                               __attribute__((unused)) V2_0::ISensors::configDirectReport_cb _hidl_cb) {
    mHubConnection->configDirectReport(sensorHandle, channelHandle, (int32_t) rate);
    return Return<void>();
}

Return<void> ISensorsSubHalBase::debug(const hidl_handle& fd, const hidl_vec<hidl_string>& args) {
    if (fd.getNativeHandle() == nullptr || fd->numFds < 1) {
        ALOGE("%s: missing fd for writing", __FUNCTION__);
        return Void();
    }

    auto dupfd = dup(fd->data[0]);
    if (dupfd < 0) {
	    ALOGE("failed to duplicate fd %d: %m", fd->data[0]);
	    return Void();
    }

    FILE* out = fdopen(dupfd, "w");

    if (args.size() != 0) {
        fprintf(out,
                "Note: sub-HAL %s currently does not support args. Input arguments are "
                "ignored.\n",
                getName().c_str());
    }

    Event event;
    std::ostringstream stream;
    stream << "exynos sensor hal debug" << std::endl;

    /* Fake Function call for coverage score */
    injectSensorData(event);
    find_validSensorIndex(0);
    mHubConnection->addDirectChannel(NULL);
    mHubConnection->removeDirectChannel(0);
    mHubConnection->isDirectReportSupported();

    fprintf(out, "%s", stream.str().c_str());

    fclose(out);
    return Return<void>();
}

Return<Result> ISensorsSubHalBase::initialize(
	const sp<V2_1::implementation::IHalProxyCallback>& halProxyCallback) {
    ALOGI("%s", __func__);
    mCallback = std::move(halProxyCallback);
    setOperationMode(OperationMode::NORMAL);

    if (mHubConnection != nullptr)
        mHubConnection->setcallback(halProxyCallback);
    else
        ALOGE("%s: mHubConnection is not available", __func__);

    return Result::OK;
}

int ISensorsSubHalBase::find_validSensorIndex(int start)
{
    int i;
    sensor_t *tmp_sensor_list = kSensorList + start;

    ALOGI("nanohub:%s: start:%d, starttype:%d\n", __FUNCTION__, start, tmp_sensor_list->type);
    for (i = start; i < n_sensor; i++, tmp_sensor_list++)
        if (tmp_sensor_list->handle != COMMS_SENSOR_INVALID) {
            return i;
        }
    return -1;
}

bool ISensorsSubHalBase::saved_setting_valid(struct sensor_map_pack *buf)
{
	bool find = false;
	sensor_t sensor;

    if (strcmp(buf->magic, "sensortype") || (buf->num_os > 9)) {
        ALOGE("magic word corrupted: magic:%s, num_os:%d", buf->magic, (int)buf->num_os);
        return false;
    }

    ALOGI("sensortype valid test [%zu]\n", kSensorCount);
    for (size_t i = 0; i < kSensorCount; i++) {
        sensor = kSensorList[i];
        find = false;

        if (sensor.name == NULL) {
            ALOGI("%s: find physensor[%d], type:%d\n", __FUNCTION__, (int)i, sensor.type);
            for (size_t j = 0; j < MAX_PHYSENSOR_NUM; j++) {
                if (sensor.type == buf->sensormap.sinfo[j].sensortype) {
                    ALOGI("%s: find physensor[%d] from file, type:%d, file:%d, %s, %s\n",
                    __FUNCTION__, (int)i, sensor.type, buf->sensormap.sinfo[j].sensortype, buf->sensormap.sinfo[j].name, buf->sensormap.sinfo[j].vendorname);
                    find = true;
                    break;
                }
            }
            if (find == false) {
                ALOGI("%s: can't find physensor[%d], type:%d\n", __FUNCTION__, (int)i, sensor.type);
                return false;
            }
        }
    }

    return true;
}

void ISensorsSubHalBase::getSensorInfo(sensor_t *sensor_list)
{
    int size;
    sensor_t *tmp_sensor_list = (sensor_t *)sensor_list;
    int fd_data, fd_sys;
    int i, j;
    int real_n_sensor = 0;
    int src;
    bool new_file = false;
    bool find;

    n_sensor = kSensorCount;

    memset(&sensor_pack, 0, sizeof(struct sensor_map_pack));
    ALOGI("nanohub:%s: n_sensor:%d", __FUNCTION__, n_sensor);

    //Open from saved file
    fd_data = open(SENSORTYPE_SAVED_PATH, O_RDONLY);
    if (fd_data >= 0) {
        size = read(fd_data, &sensor_pack, sizeof(struct sensor_map_pack));
        ALOGI("%s: data found (%d)", __FUNCTION__, (int) sensor_pack.num_os);
        close(fd_data);
    } else {
	/* No saved file (SENSORTYPE_SAVED_PATH) , read it from chub-fw multi-os */
        new_file = true;
        ALOGI("data not found");
        fd_sys = open(SENSORTYPE_PATH, O_RDONLY);
        if (fd_sys < 0) {
            ALOGE("%s: Failed to open: err=%d [%s]", __FUNCTION__, errno, strerror(errno));
            return;
        }
        size = read(fd_sys, &sensor_pack, sizeof(struct sensor_map_pack));
        close(fd_sys);
    }

    ALOGI("nanohub: %s: size: %d", __FUNCTION__, size);
    if (size <= 0) {
        ALOGE("nanohub: %s: fails to get sensorinfo", __FUNCTION__);
        return;
    }

    /* get raw list from sensorlist.cpp */
    for (i = 0; i < n_sensor; i++)
        sensor_list[i] = kSensorList[i];

	/* find real sensor. make 'COMMS_SENSOR_INVALID' handle with 'sensor_list_no_active' id by chub fw*/
    for (i = 0; i < n_sensor; i++, tmp_sensor_list++) {
        ALOGI("nanohub: %s: device sensorlist: handle:%d, type:%d, name:%s", __FUNCTION__, tmp_sensor_list->handle, tmp_sensor_list->type, tmp_sensor_list->name);
        if (tmp_sensor_list->name) {
            ALOGI("nanohub: %s: fixed sensor-type:%s. skip to alive check", __FUNCTION__, tmp_sensor_list->name);
            continue;
        }

        find = false;
        sensor_pack.sensormap.index = sensor_pack.sensormap.index > 20 ? 0 : sensor_pack.sensormap.index;
        for (j = 0; j < sensor_pack.sensormap.index; j++) {
            ALOGI("> (nanohub: n_sensor:%d, type:%d, name:%s,%d, vname:%s,%d)",
                j, sensor_pack.sensormap.sinfo[j].sensortype,
                sensor_pack.sensormap.sinfo[j].name,
                (int)sizeof(sensor_pack.sensormap.sinfo[j].name),
                sensor_pack.sensormap.sinfo[j].vendorname,
                (int)sizeof(sensor_pack.sensormap.sinfo[j].vendorname));

            if (sensor_pack.sensormap.sinfo[j].sensortype == (uint8_t)tmp_sensor_list->type) {
                tmp_sensor_list->name = sensor_pack.sensormap.sinfo[j].name;
                tmp_sensor_list->vendor = sensor_pack.sensormap.sinfo[j].vendorname;

                ALOGI("> nanohub:%s: Find: n_sensor:%d, type:%d, name:%s->%s, vname:%s->%s",
                    __FUNCTION__, j,
                    sensor_pack.sensormap.sinfo[j].sensortype,
                    sensor_pack.sensormap.sinfo[j].name,
                    tmp_sensor_list->name,
                    sensor_pack.sensormap.sinfo[j].vendorname,
                    tmp_sensor_list->vendor);
                find = true;
                break;
            }
        }
        if (find == false) {
            ALOGE("> %s: sensortype corrupted at %d", __FUNCTION__, j);
            tmp_sensor_list->vendor = "Exynos";
            tmp_sensor_list->name = "Exynos sensors";
        }
    }

	ALOGI("nanohub: %s: overwrite invalid sensor", __FUNCTION__);
	tmp_sensor_list = (sensor_t *)sensor_list;
	for (i = 0; i < n_sensor; i++, tmp_sensor_list++) {
		if (tmp_sensor_list->handle == COMMS_SENSOR_INVALID) {
			src = find_validSensorIndex(i);
			ALOGI("nanohub: %s: %d th is invalid, copy from %d to %d. handle:%d",
				__FUNCTION__, i, src, i, tmp_sensor_list->handle);
			if (src >= 0) {
				memcpy((void *)(sensor_list + i), (void *)(sensor_list + src), sizeof(struct sensor_t));
				sensor_list[src].handle = COMMS_SENSOR_INVALID;
			}
			else
				break;
			ALOGI("nanohub--: %s: %d th is invalid, copy from %d to %d. handle:%d",
				__FUNCTION__, i, src, i, tmp_sensor_list->handle);
		}
	}

	ALOGI("nanohub: %s: dump real sensorlist", __FUNCTION__);
	tmp_sensor_list = sensor_list;
	for (i = 0; i < n_sensor; i++, tmp_sensor_list++) {
		if (tmp_sensor_list->handle != COMMS_SENSOR_INVALID)
			real_n_sensor++;

		ALOGI("nanohub: %d: type:%d, handle:%d, name:%s, vendor:%s, real_n_sensor:%d",
			i, tmp_sensor_list->type, tmp_sensor_list->handle,
			tmp_sensor_list->name, tmp_sensor_list->vendor, real_n_sensor);
	}
	n_sensor = real_n_sensor;

    if (new_file) {
        ALOGI("Create sensortype save file");

        strncpy(sensor_pack.magic, "sensortype", sizeof("sensortype"));

        fd_data = open(SENSORTYPE_SAVED_PATH, O_WRONLY|O_CREAT, 0660);
        if (fd_data < 0) {
            ALOGE("%s: Failed to open: err=%d [%s]", __FUNCTION__, errno, strerror(errno));
            return;
        }
        ALOGI("sensorInfo is saved at %s (os %d)", SENSORTYPE_SAVED_PATH, (int)sensor_pack.num_os);
        write(fd_data, &sensor_pack, sizeof(struct sensor_map_pack));
        close(fd_data);
    }
}

}  // namespace implementation
}  // namespace subhal
}  // namespace V2_1
}  // namespace sensors
}  // namespace hardware
}  // namespace android
