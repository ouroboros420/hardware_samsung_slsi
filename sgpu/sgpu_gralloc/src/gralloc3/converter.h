///
/// @file converter.h
/// @copyright 2020 Samsung Electronics
///

#pragma once

#include <android/hardware/graphics/mapper/3.0/IMapper.h>

#include "core/hidl_defines.h"

namespace android {
namespace samsung {
namespace gralloc {

inline PixelFormat convert_to_format(const hardware::graphics::common::V1_2::PixelFormat value)
{
        return static_cast<PixelFormat>(value);
}

inline BufferDescriptorInfo convert_to_buffer_descriptor_info(const hardware::graphics::mapper::V3_0::IMapper::BufferDescriptorInfo &in)
{
        BufferDescriptorInfo info = {
                .name         = hardware::hidl_string("sgr"),
                .width        = in.width,
                .height       = in.height,
                .layerCount   = in.layerCount,
                .format       = convert_to_format(in.format),
                .usage        = in.usage,
                .reservedSize = 0
        };

        return info;
}

inline hardware::graphics::mapper::V3_0::Error convert_to_hidl_error(Error code)
{
        using namespace hardware::graphics::mapper;

        V3_0::Error new_code = static_cast<V3_0::Error>(code);

        static_assert(V3_0::Error::NONE           == static_cast<V3_0::Error>(Error::NONE), "Error enum mismatch");
        static_assert(V3_0::Error::BAD_DESCRIPTOR == static_cast<V3_0::Error>(Error::BAD_DESCRIPTOR), "Error enum mismatch");
        static_assert(V3_0::Error::BAD_BUFFER     == static_cast<V3_0::Error>(Error::BAD_BUFFER), "Error enum mismatch");
        static_assert(V3_0::Error::BAD_VALUE      == static_cast<V3_0::Error>(Error::BAD_VALUE), "Error enum mismatch");
        static_assert(V3_0::Error::NO_RESOURCES   == static_cast<V3_0::Error>(Error::NO_RESOURCES), "Error enum mismatch");
        static_assert(V3_0::Error::UNSUPPORTED    == static_cast<V3_0::Error>(Error::UNSUPPORTED), "Error enum mismatch");

        return new_code;
}

inline Rect convert_to_rect(const hardware::graphics::mapper::V3_0::IMapper::Rect& value)
{
        Rect rect = {
                .left   = value.left,
                .top    = value.top,
                .width  = value.width,
                .height = value.height
        };

        return rect;
}

} // gralloc
} // samsung
} // android
