///
/// @file converter.h
/// @copyright 2020 Samsung Electronics
///

#pragma once

#include <android/hardware/graphics/mapper/4.0/IMapper.h>
#include <type_traits>

#include "core/hidl_defines.h"
#include "util/util.h"

#define AIDL_METADATA_TYPE_DESCRIPTION hardware::graphics::mapper::V4_0::IMapper::MetadataTypeDescription
#define SGR_METADATA_TYPE_DESCRIPTION android::samsung::gralloc::MetadataTypeDescription

namespace android {
namespace samsung {
namespace gralloc {

inline PixelFormat convert_to_format(const hardware::graphics::common::V1_2::PixelFormat value)
{
        return static_cast<PixelFormat>(value);
}

inline BufferDescriptorInfo convert_to_buffer_descriptor_info(const hardware::graphics::mapper::V4_0::IMapper::BufferDescriptorInfo &in)
{
        SGR_ASSERT(in.name.size() <= SGR_MAX_LENGTH_NAME);

        BufferDescriptorInfo info = {
                .name         = in.name,
                .width        = in.width,
                .height       = in.height,
                .layerCount   = in.layerCount,
                .format       = convert_to_format(in.format),
                .usage        = in.usage,
                .reservedSize = in.reservedSize
        };

        return info;
}

inline hardware::graphics::mapper::V4_0::Error convert_to_hidl_error(Error code)
{
        using namespace hardware::graphics::mapper;

        V4_0::Error new_code = static_cast<V4_0::Error>(code);

        static_assert(V4_0::Error::NONE           == static_cast<V4_0::Error>(Error::NONE), "Error enum mismatch");
        static_assert(V4_0::Error::BAD_DESCRIPTOR == static_cast<V4_0::Error>(Error::BAD_DESCRIPTOR), "Error enum mismatch");
        static_assert(V4_0::Error::BAD_BUFFER     == static_cast<V4_0::Error>(Error::BAD_BUFFER), "Error enum mismatch");
        static_assert(V4_0::Error::BAD_VALUE      == static_cast<V4_0::Error>(Error::BAD_VALUE), "Error enum mismatch");
        static_assert(V4_0::Error::NO_RESOURCES   == static_cast<V4_0::Error>(Error::NO_RESOURCES), "Error enum mismatch");
        static_assert(V4_0::Error::UNSUPPORTED    == static_cast<V4_0::Error>(Error::UNSUPPORTED), "Error enum mismatch");

        return new_code;
}

inline Rect convert_to_rect(const hardware::graphics::mapper::V4_0::IMapper::Rect& value)
{
        Rect rect = {
                .left   = value.left,
                .top    = value.top,
                .width  = value.width,
                .height = value.height
        };

        return rect;
}

inline MetadataType convert_to_metadata_type(const hardware::graphics::mapper::V4_0::IMapper::MetadataType& value)
{
        MetadataType metadata_type = {
                .name = value.name,
                .value = value.value
        };

        return metadata_type;
}

#define STATIC_ASSERT_CHECK_TYPE_AND_OFFSET_MSG(type1, field1, base_adjust1,                                    \
                                                type2, field2, base_adjust2, ...)                               \
{                                                                                                               \
        static_assert(std::is_same<decltype(type1::field1), decltype(type2::field2)>::value, __VA_ARGS__);      \
        static_assert((offsetof(type1, field1) - base_adjust1) == (offsetof(type2, field2) - base_adjust2),     \
                       __VA_ARGS__);                                                                            \
}

inline const std::vector<AIDL_METADATA_TYPE_DESCRIPTION> *convert_to_hidl_metadata(
                                                                const std::vector<SGR_METADATA_TYPE_DESCRIPTION> *in)
{

        constexpr uint32_t aidl_metadata_type_description_base_adjust = offsetof(AIDL_METADATA_TYPE_DESCRIPTION,
                                                                                 metadataType.value);
        constexpr uint32_t sgr_metadata_type_description_base_adjust  = offsetof(SGR_METADATA_TYPE_DESCRIPTION,
                                                                                 metadata_type.value);

        STATIC_ASSERT_CHECK_TYPE_AND_OFFSET_MSG(AIDL_METADATA_TYPE_DESCRIPTION, metadataType.value,
                                                aidl_metadata_type_description_base_adjust,
                                                SGR_METADATA_TYPE_DESCRIPTION, metadata_type.value,
                                                sgr_metadata_type_description_base_adjust,
                                                "MetadataTypeDescription::type::value mismatch");

        STATIC_ASSERT_CHECK_TYPE_AND_OFFSET_MSG(AIDL_METADATA_TYPE_DESCRIPTION, isGettable,
                                                aidl_metadata_type_description_base_adjust,
                                                SGR_METADATA_TYPE_DESCRIPTION, is_gettable,
                                                sgr_metadata_type_description_base_adjust,
                                                "MetadataTypeDescription::type::value mismatch");

        STATIC_ASSERT_CHECK_TYPE_AND_OFFSET_MSG(AIDL_METADATA_TYPE_DESCRIPTION, isSettable,
                                                aidl_metadata_type_description_base_adjust,
                                                SGR_METADATA_TYPE_DESCRIPTION, is_settable,
                                                sgr_metadata_type_description_base_adjust,
                                                "MetadataTypeDescription::type::value mismatch");

        return reinterpret_cast<const std::vector<AIDL_METADATA_TYPE_DESCRIPTION>*>(in);
}

} // gralloc
} // samsung
} // android
