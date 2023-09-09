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

#pragma once

#include "V2_0/SubHal.h"
#include "V2_1/SubHal.h"
#include "convertV2_1.h"
#include "hubconnection2/hubconnection.h"
#include <vector>

namespace android {
namespace hardware {
namespace sensors {
namespace V2_1 {
namespace subhal {
namespace implementation {

using ::android::hardware::sensors::V1_0::OperationMode;
using ::android::hardware::sensors::V1_0::Result;
using ::android::hardware::sensors::V1_0::SensorType;
using ::android::hardware::sensors::V1_0::OperationMode;
using ::android::hardware::sensors::V2_1::Event;

/**
 * Implementation of a ISensorsSubHal that can be used to test the implementation of multihal 2.0.
 * See the README file for more details on how this class can be used for testing.
 */
class ISensorsSubHalBase {
  protected:
    using Event = ::android::hardware::sensors::V2_1::Event;
    using RateLevel = ::android::hardware::sensors::V1_0::RateLevel;
    using SharedMemInfo = ::android::hardware::sensors::V1_0::SharedMemInfo;

  public:
    ISensorsSubHalBase();
    virtual ~ISensorsSubHalBase();

    Return<void> getSensorsList(V2_1::ISensors::getSensorsList_2_1_cb _hidl_cb);
    Return<Result> injectSensorData(const Event& event);
    Return<Result> initialize(const sp<V2_1::implementation::IHalProxyCallback>& halProxyCallback);

    // Methods from ::android::hardware::sensors::V2_0::ISensors follow.
    virtual Return<Result> setOperationMode(OperationMode mode);

    Return<Result> getSensor(int32_t sensorHandle, sensor_t **sensor);

    OperationMode getOperationMode() const { return mCurrentOperationMode; }

    Return<Result> activate(int32_t sensorHandle, bool enabled);

    Return<Result> batch(int32_t sensorHandle, int64_t samplingPeriodNs,
                         int64_t maxReportLatencyNs);

    Return<Result> flush(int32_t sensorHandle);

    Return<void> registerDirectChannel(const SharedMemInfo& mem,
                                       V2_0::ISensors::registerDirectChannel_cb _hidl_cb);

    Return<Result> unregisterDirectChannel(int32_t channelHandle);

    Return<void> configDirectReport(int32_t sensorHandle, int32_t channelHandle, RateLevel rate,
                                    V2_0::ISensors::configDirectReport_cb _hidl_cb);

    Return<void> debug(const hidl_handle& fd, const hidl_vec<hidl_string>& args);

    // Methods from ::android::hardware::sensors::V2_0::implementation::ISensorsSubHal follow.
    const std::string getName() {
#ifdef SUB_HAL_NAME
        return SUB_HAL_NAME;
#else   // SUB_HAL_NAME
        return "SubHal";
#endif  // SUB_HAL_NAME
    }

    // Method for exynos
    bool saved_setting_valid(struct sensor_map_pack *buf);
    void getSensorInfo(sensor_t *sensor_list);
    void GetSavedData();
    int find_validSensorIndex(int start);

    sensor_t sensor_list[40];
    bool gHubAlive = 0;
    int n_sensor = 0;

    struct sensor_map_pack sensor_pack;

  protected:
    /**
     * Callback used to communicate to the HalProxy when dynamic sensors are connected /
     * disconnected, sensor events need to be sent to the framework, and when a wakelock should be
     * acquired.
     */
    sp<V2_1::implementation::IHalProxyCallback> mCallback;

  private:
    /**
     * The current operation mode of the multihal framework. Ensures that all subhals are set to
     * the same operation mode.
     */
    OperationMode mCurrentOperationMode = OperationMode::NORMAL;

    sp<HubConnection> mHubConnection;
};

class SensorsSubHalBase : public ISensorsSubHalBase, public V2_1::implementation::ISensorsSubHal {
  public:
    Return<Result> setOperationMode(OperationMode mode) override {
        return ISensorsSubHalBase::setOperationMode(mode);
    }
    Return<Result> activate(int32_t sensorHandle, bool enabled) override {
        return ISensorsSubHalBase::activate(sensorHandle, enabled);
    }
    Return<Result> batch(int32_t sensorHandle, int64_t samplingPeriodNs,
                         int64_t maxReportLatencyNs) override {
        return ISensorsSubHalBase::batch(sensorHandle, samplingPeriodNs, maxReportLatencyNs);
    }
    Return<Result> flush(int32_t sensorHandle) override {
        return ISensorsSubHalBase::flush(sensorHandle);
    }
    Return<void> registerDirectChannel(const SharedMemInfo& mem,
                                       V2_0::ISensors::registerDirectChannel_cb _hidl_cb) override {
        return ISensorsSubHalBase::registerDirectChannel(mem, _hidl_cb);
    }
    Return<Result> unregisterDirectChannel(int32_t channelHandle) override {
        return ISensorsSubHalBase::unregisterDirectChannel(channelHandle);
    }
    Return<void> configDirectReport(int32_t sensorHandle, int32_t channelHandle, RateLevel rate,
                                    V2_0::ISensors::configDirectReport_cb _hidl_cb) override {
        return ISensorsSubHalBase::configDirectReport(sensorHandle, channelHandle, rate, _hidl_cb);
    }
    Return<void> debug(const hidl_handle& fd, const hidl_vec<hidl_string>& args) override {
        SharedMemInfo info;
        V2_0::ISensors::registerDirectChannel_cb cb;

        flush(1);
        registerDirectChannel(info, cb);
        unregisterDirectChannel(0);
        configDirectReport(0, 0, RateLevel::NORMAL, cb);
        return ISensorsSubHalBase::debug(fd, args);
    }
    const std::string getName() override { return ISensorsSubHalBase::getName(); }
};

class SensorsSubHalV2_1 : public SensorsSubHalBase {
  public:
    Return<void> getSensorsList_2_1(V2_1::ISensors::getSensorsList_2_1_cb _hidl_cb) override {
        return ISensorsSubHalBase::getSensorsList(_hidl_cb);
    }
    Return<Result> injectSensorData_2_1(__attribute__((unused)) const V2_1::Event& event) override {
        return Result::OK;
    }

    virtual Return<V1_0::Result> initialize(const sp<V2_1::implementation::IHalProxyCallback>& halProxyCallback) override {
        return ISensorsSubHalBase::initialize(halProxyCallback);
    };
};

}  // namespace implementation
}  // namespace subhal
}  // namespace V2_1
}  // namespace sensors
}  // namespace hardware
}  // namespace android
