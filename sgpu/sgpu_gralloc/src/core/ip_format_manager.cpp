///
/// @file ip_format_manager.cpp
/// @copyright 2021 Samsung Electronics
///

#include <android-base/properties.h>

#include "config.h"
#include "ip_format_manager.h"
#include "layout_manager.h"
#include "util/util.h"

namespace android {
namespace samsung {
namespace gralloc {

static bool is_sajc_enabled(const BufferDescriptor &descriptor)
{
        bool ret = false;
        if(android::base::GetBoolProperty(CONFIG_SAJC, CONFIG_SAJC_DEFAULT) == true) {
                std::string sajc_application_name = android::base::GetProperty(CONFIG_SAJC_BUFFER_NAME_FILTER,
                                                                       CONFIG_SAJC_BUFFER_NAME_FILTER_DEFAULT);

                if (sajc_application_name.empty() ||
                   (std::string(descriptor.name).find(sajc_application_name) != std::string::npos)) {
                        ret = true;
                }
        }

        return ret;
}

inline bool check_sajc_threshold(const BufferDescriptor &descriptor)
{
        constexpr int min_size = 20;
        int buffer_size = static_cast<int>(descriptor.width * descriptor.height);

        int display_width  = android::base::GetIntProperty(CONFIG_DISPLAY_W, CONFIG_DISPLAY_W_DEFAULT);
        int display_height = android::base::GetIntProperty(CONFIG_DISPLAY_H, CONFIG_DISPLAY_H_DEFAULT);

        int display_size = display_width * display_height;
        SGR_ASSERT(display_size != 0);

        return ((buffer_size * 100 ) / display_size) >= min_size;
}

///
/// @brief disable SAJC layout based on config or usages
///
/// @param[in]     descriptor Buffer descriptor
/// @param[in/out] layout     layout which wil be updated
///
void IpFormatManager::disable_sajc_if_needed(const BufferDescriptor &descriptor, uint32_t *layout) const
{
        constexpr uint64_t no_sajc_usage = static_cast<uint64_t>(BufferUsage::PRIVATE_NO_SAJC);
        constexpr uint64_t render_target_usage = static_cast<uint64_t>(BufferUsage::GPU_RENDER_TARGET);

        if (!is_sajc_enabled(descriptor) ||
            is_any_bitmask_set_64(descriptor.usage, no_sajc_usage) ||
            !is_any_bitmask_set_64(descriptor.usage, render_target_usage) || // Enable DCC only for render targets
            !check_sajc_threshold(descriptor)) {
                clear_bitmask(layout, static_cast<uint32_t>(FormatLayoutBitMask::DCC));
        }
}

} // gralloc
} // samsung
} // android
