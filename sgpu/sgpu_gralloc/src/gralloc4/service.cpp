///
/// @file service.cpp
/// @copyright 2020 Samsung Electronics
///

#define LOG_TAG "android.hardware.graphics.allocator@4.0-service"

#include <hidl/HidlTransportSupport.h>

#include "AllocatorHidl.h"
#include "util/util.h"

using android::sp;

/// libhwbinder:
using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;

/// Generated HIDL files
using android::hardware::graphics::allocator::V4_0::IAllocator;
using android::hardware::graphics::allocator::V4_0::implementation::Allocator;

using android::status_t;
using android::OK;

int main() {
    android::sp<IAllocator> service = new Allocator();

    configureRpcThreadpool(1, true);

    status_t status = service->registerAsService();

    if (status != OK) {
        SGR_LOGE("Cannot register Allocator 4.0 HAL service");
        /// @todo GFXSW-4079 Decide on return value
        return 1;
    }

    SGR_LOGI("Allocator 4.0 HAL Ready.");
    joinRpcThreadpool();
    /// Under normal cases, execution will not reach this line.
    SGR_LOGI("Allocator 4.0 HAL failed to join thread pool.");
    return 1;
}