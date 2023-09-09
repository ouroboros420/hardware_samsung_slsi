///
/// @file environment_manager.h
/// @copyright 2020-2022 Samsung Electronics
///
#pragma once

#include <android-base/properties.h>

namespace android {
namespace samsung {
namespace gralloc {

///
/// Used to get api_level of current & launching
/// The default values are 31 as we've done SGR bringup on Android S
///

#define CONFIG_PRODUCT_FIRST_API_LEVEL         "ro.product.first_api_level"
#define CONFIG_PRODUCT_FIRST_API_LEVEL_DEFAULT 31

#define CONFIG_BUILD_VERSION_SDK               "ro.build.version.sdk"
#define CONFIG_BUILD_VERSION_SDK_DEFAULT       31

enum class API_LEVEL : int {
        R        = 30,
        S        = 31,
        S_V2     = 32,
        TIRAMISU = 33,
};

class EnvironmentHelper{
private:
        EnvironmentHelper()
        {
                m_product_first_api_level = android::base::GetIntProperty(CONFIG_PRODUCT_FIRST_API_LEVEL,
                                                                          CONFIG_PRODUCT_FIRST_API_LEVEL_DEFAULT);

                m_build_version_sdk = android::base::GetIntProperty(CONFIG_BUILD_VERSION_SDK,
                                                                    CONFIG_BUILD_VERSION_SDK_DEFAULT);
        }

        int m_product_first_api_level;
        int m_build_version_sdk;

public:
        static const EnvironmentHelper* GetInstance()
        {
                static const EnvironmentHelper ins;
                return &ins;
        }

        int get_launch_api_level() const { return m_product_first_api_level; }
        int get_system_api_level() const { return m_build_version_sdk; }

        bool is_launch_gt_s() const { return (m_product_first_api_level > static_cast<int>(API_LEVEL::S_V2)); }
        bool is_system_gt_s() const { return (m_build_version_sdk > static_cast<int>(API_LEVEL::S_V2)); }
};

} // gralloc
} // samsung
} // android

