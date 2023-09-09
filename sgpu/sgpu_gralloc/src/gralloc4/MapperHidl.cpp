///
/// @file MapperHidl.cpp
/// @copyright 2020 Samsung Electronics
///

#include <aidl/android/hardware/graphics/common/StandardMetadataType.h>
#include <cutils/native_handle.h>
#include <gralloctypes/Gralloc4.h>

#include "converter.h"
#include "core/debug_helper.h"
#include "MapperHidl.h"
#include "util/util.h"

#define __STDC_FORMAT_MACROS
#include <cinttypes>

namespace android {
namespace hardware {
namespace graphics {
namespace mapper {
namespace V4_0 {
namespace implementation {

static int32_t get_fence_fd(const hidl_handle& fence)
{
        const native_handle_t* handle = fence.getNativeHandle();
        return (handle != nullptr && handle->numFds == 1) ? handle->data[0] : -1;
}

// Methods from ::android::hardware::graphics::mapper::V4_0::IMapper follow.
Return<void> Mapper::createDescriptor(const ::android::hardware::graphics::mapper::V4_0::IMapper::BufferDescriptorInfo& description, createDescriptor_cb _hidl_cb) {
        hidl_vec<uint8_t> descriptor(sizeof(samsung::gralloc::BufferDescriptor));
        auto buffer_descriptor = reinterpret_cast<samsung::gralloc::BufferDescriptor*>(descriptor.data());
        Error error = convert_to_hidl_error(m_mapper.create_descriptor(
                            samsung::gralloc::convert_to_buffer_descriptor_info(description),
                            buffer_descriptor
                        ));
        if (error != Error::NONE) {
                descriptor.resize(0);
        }

        _hidl_cb(error, descriptor);

        return Void();
}

Return<void> Mapper::importBuffer(const hidl_handle& rawHandle, importBuffer_cb _hidl_cb) {
        if (rawHandle.getNativeHandle() == nullptr) {
                _hidl_cb(Error::BAD_BUFFER, nullptr);
                return Void();
        }

        native_handle_t *handle = nullptr;
        Error error = convert_to_hidl_error(m_mapper.import_buffer(rawHandle.getNativeHandle(), &handle));

        _hidl_cb(error, reinterpret_cast<void*>(handle));

        return Void();
}

Return<::android::hardware::graphics::mapper::V4_0::Error> Mapper::freeBuffer(void* buffer) {
        if (buffer == nullptr) {
                return Error::BAD_BUFFER;
        }

        auto handle = reinterpret_cast<native_handle_t*>(buffer);
        return convert_to_hidl_error(m_mapper.free_buffer(handle));
}

Return<::android::hardware::graphics::mapper::V4_0::Error> Mapper::validateBufferSize(void* buffer, const ::android::hardware::graphics::mapper::V4_0::IMapper::BufferDescriptorInfo& description, uint32_t stride) {
        if (buffer == nullptr) {
                return Error::BAD_BUFFER;
        }

        auto handle = reinterpret_cast<const native_handle_t*>(buffer);
        auto buffer_descriptor_info = samsung::gralloc::convert_to_buffer_descriptor_info(description);

        return convert_to_hidl_error(m_mapper.validate_buffer_size(handle, buffer_descriptor_info, stride));
}

Return<void> Mapper::getTransportSize(void* buffer, getTransportSize_cb _hidl_cb) {
        if (buffer == nullptr) {
                _hidl_cb(Error::BAD_BUFFER, 0, 0);
                return Void();
        }

        auto handle = reinterpret_cast<const native_handle_t*>(buffer);
        int32_t num_fds = 0;
        int32_t num_ints = 0;

        Error error = convert_to_hidl_error(m_mapper.get_transport_size(handle, &num_fds, &num_ints));

        _hidl_cb(error, num_fds, num_ints);

        return Void();
}

Return<void> Mapper::lock(void* buffer, uint64_t cpuUsage, const ::android::hardware::graphics::mapper::V4_0::IMapper::Rect& accessRegion, const hidl_handle& acquireFence, lock_cb _hidl_cb) {
        if (buffer == nullptr) {
                _hidl_cb(Error::BAD_BUFFER, nullptr);
                return Void();
        }

        if (cpuUsage == 0) {
                _hidl_cb(Error::BAD_VALUE, nullptr);
                return Void();
        }

        auto handle = reinterpret_cast<native_handle_t*>(buffer);
        auto access_region = samsung::gralloc::convert_to_rect(accessRegion);
        auto acquire_fence = get_fence_fd(acquireFence);
        void *data = nullptr;
        int32_t bytesPerPixel = -1;
        int32_t bytesPerStride = -1;

        Error error = convert_to_hidl_error(m_mapper.lock(handle, cpuUsage, access_region, acquire_fence,
                                                          &data, &bytesPerPixel, &bytesPerStride));

        _hidl_cb(error, data);

        return Void();
}

Return<void> Mapper::unlock(void* buffer, unlock_cb _hidl_cb) {
        if (buffer == nullptr) {
                _hidl_cb(Error::BAD_BUFFER, nullptr);
                return Void();
        }

        auto handle = reinterpret_cast<native_handle_t*>(buffer);
        int fence = -1;

        Error error = convert_to_hidl_error(m_mapper.unlock(handle, &fence));

        native_handle_t *fence_handle = nullptr;
        if (fence >= 0) {
                NATIVE_HANDLE_DECLARE_STORAGE(storage, 1, 0);
                fence_handle = native_handle_init(storage, 1, 0);
                fence_handle->data[0] = fence;
        }

        _hidl_cb(error, fence_handle);

        return Void();
}

Return<void> Mapper::flushLockedBuffer(void* buffer, flushLockedBuffer_cb _hidl_cb) {
        if (buffer == nullptr) {
                _hidl_cb(Error::BAD_BUFFER, nullptr);
                return Void();
        }

        auto handle = reinterpret_cast<native_handle_t*>(buffer);
        int fence = -1;

        Error error = convert_to_hidl_error(m_mapper.flush_locked_buffer(handle, &fence));

        hidl_handle release_fence = hidl_handle();
        if (fence != -1) {
                NATIVE_HANDLE_DECLARE_STORAGE(storage, 1, 0);
                native_handle_t *fence_handle = native_handle_init(storage, 1, 0);
                fence_handle->data[0] = fence;
                release_fence.setTo(fence_handle, false);
        }

        _hidl_cb(error, release_fence);

        return Void();
}

Return<::android::hardware::graphics::mapper::V4_0::Error> Mapper::rereadLockedBuffer(void* buffer) {
        if (buffer == nullptr) {
                return Error::BAD_BUFFER;
        }

        auto handle = reinterpret_cast<native_handle_t*>(buffer);

        return convert_to_hidl_error(m_mapper.reread_locked_buffer(handle));
}

Return<void> Mapper::isSupported(const ::android::hardware::graphics::mapper::V4_0::IMapper::BufferDescriptorInfo& description, isSupported_cb _hidl_cb) {
        bool supported = m_mapper.is_supported(samsung::gralloc::convert_to_buffer_descriptor_info(description));

        _hidl_cb(Error::NONE, supported);

        return Void();
}

Return<void> Mapper::get(void* buffer, const ::android::hardware::graphics::mapper::V4_0::IMapper::MetadataType& metadataType, get_cb _hidl_cb) {
        hidl_vec<uint8_t> metadata;

        if (buffer == nullptr) {
                _hidl_cb(Error::BAD_BUFFER, metadata);
                return Void();
        }

        if (strcmp(metadataType.name.c_str(), GRALLOC4_STANDARD_METADATA_TYPE) != 0) {
                SGR_LOGE("Only StandardMetadataType is supported, got %s", metadataType.name.c_str());
                _hidl_cb(Error::UNSUPPORTED, metadata);
                return Void();
        }

        using aidl::android::hardware::graphics::common::StandardMetadataType;
        SGR_LOGV("MetadataType (%s, %s)", metadataType.name.c_str(), toString(static_cast<StandardMetadataType>(metadataType.value)).c_str());

        Error error = Error::NONE;
        status_t ret = OK;
        auto handle = reinterpret_cast<const native_handle_t*>(buffer);

        switch (static_cast<StandardMetadataType>(metadataType.value)) {
        case StandardMetadataType::CTA861_3:
        case StandardMetadataType::SMPTE2086:
        case StandardMetadataType::SMPTE2094_40:
                if (m_mapper.is_registered_handle(handle) == false) {
                        SGR_LOGE("handle is not imported... failed to get MetadataType(%s)", toString(static_cast<StandardMetadataType>(metadataType.value)).c_str());
                        _hidl_cb(Error::BAD_BUFFER, metadata);
                        return Void();
                }
                break;
        default:
                break;
        }

        // metadata resizing is done inside the encode function
        switch (static_cast<StandardMetadataType>(metadataType.value)) {
        case StandardMetadataType::INVALID: {
                SGR_LOGE("Not supported");
                error = Error::UNSUPPORTED;
                break;
        }
        case StandardMetadataType::BUFFER_ID: {
                uint64_t buffer_id = 0;
                error = convert_to_hidl_error(m_mapper.get_metadata_manager()->
                                              get_buffer_id(handle, &buffer_id));
                ret = android::gralloc4::encodeBufferId(buffer_id, &metadata);
                break;
        }
        case StandardMetadataType::NAME: {
                std::string name;
                error = convert_to_hidl_error(m_mapper.get_metadata_manager()->
                                              get_name(handle, &name));
                ret = android::gralloc4::encodeName(name, &metadata);
                break;
        }
        case StandardMetadataType::WIDTH: {
                uint64_t width = 0;
                error = convert_to_hidl_error(m_mapper.get_metadata_manager()->
                                              get_width(handle, &width));
                SGR_LOGV("Width %" PRId64, width);
                ret = android::gralloc4::encodeWidth(width, &metadata);
                break;
        }
        case StandardMetadataType::HEIGHT: {
                uint64_t height = 0;
                error = convert_to_hidl_error(m_mapper.get_metadata_manager()->
                                              get_height(handle, &height));
                SGR_LOGV("Height %" PRId64, height);
                ret = android::gralloc4::encodeHeight(height, &metadata);
                break;
        }
        case StandardMetadataType::LAYER_COUNT: {
                uint64_t layer_count = 0;
                error = convert_to_hidl_error(m_mapper.get_metadata_manager()->
                                              get_layer_count(handle, &layer_count));
                SGR_LOGV("Layer Count %" PRId64, layer_count);
                ret = android::gralloc4::encodeLayerCount(layer_count, &metadata);
                break;
        }
        case StandardMetadataType::PIXEL_FORMAT_REQUESTED: {
                int32_t pixel_format_requested = 0;
                error = convert_to_hidl_error(m_mapper.get_metadata_manager()->
                                              get_pixel_format_requested(handle, &pixel_format_requested));
                SGR_LOGV("Pixel Format Requested %" PRId32 "[%s]", pixel_format_requested,
                         samsung::gralloc::get_pixel_format_string(
                                 static_cast<samsung::gralloc::PixelFormat>(pixel_format_requested)).c_str());
                ret = android::gralloc4::encodePixelFormatRequested(
                              static_cast<hardware::graphics::common::V1_2::PixelFormat>(pixel_format_requested),
                              &metadata);
                break;
        }
        case StandardMetadataType::PIXEL_FORMAT_FOURCC: {
                uint32_t pixel_format_fourcc = 0;
                error = convert_to_hidl_error(m_mapper.get_metadata_manager()->
                                              get_pixel_format_fourcc(handle, &pixel_format_fourcc));
                SGR_LOGV("Pixel Format FourCC = 0x%" PRIx32 "[%s]", pixel_format_fourcc,
                         samsung::gralloc::get_pixel_format_fourcc_string(pixel_format_fourcc).c_str());
                ret = android::gralloc4::encodePixelFormatFourCC(pixel_format_fourcc, &metadata);
                break;
        }
        case StandardMetadataType::PIXEL_FORMAT_MODIFIER: {
                uint64_t pixel_format_modifier = 0;
                error = convert_to_hidl_error(m_mapper.get_metadata_manager()->
                                              get_pixel_format_modifier(handle, &pixel_format_modifier));
                ret = android::gralloc4::encodePixelFormatModifier(pixel_format_modifier, &metadata);
                break;
        }
        case StandardMetadataType::USAGE: {
                uint64_t usage = 0;
                error = convert_to_hidl_error(m_mapper.get_metadata_manager()->
                                              get_usage(handle, &usage));
                SGR_LOGV("%s", samsung::gralloc::get_usage_string(usage).c_str());
                ret = android::gralloc4::encodeUsage(usage, &metadata);
                break;
        }
        case StandardMetadataType::ALLOCATION_SIZE: {
                uint64_t allocation_size = 0;
                error = convert_to_hidl_error(m_mapper.get_metadata_manager()->
                                              get_allocation_size(handle, &allocation_size));
                SGR_LOGV("Allocation Size = 0x%" PRIx64, allocation_size);
                ret = android::gralloc4::encodeAllocationSize(allocation_size, &metadata);
                break;
        }
        case StandardMetadataType::PROTECTED_CONTENT: {
                uint64_t protected_content = 0;
                error = convert_to_hidl_error(m_mapper.get_metadata_manager()->
                                              get_protected_content(handle, &protected_content));
                SGR_LOGV("Protected Content = 0x%" PRIx64, protected_content);
                ret = android::gralloc4::encodeProtectedContent(protected_content, &metadata);
                break;
        }
        case StandardMetadataType::COMPRESSION: {
                aidl::android::hardware::graphics::common::ExtendableType compression;
                error = convert_to_hidl_error(m_mapper.get_metadata_manager()->
                                              get_compression(handle, &compression));
                compression.name = GRALLOC4_STANDARD_COMPRESSION;
                ret = android::gralloc4::encodeCompression(compression, &metadata);
                break;
        }
        case StandardMetadataType::INTERLACED: {
                aidl::android::hardware::graphics::common::ExtendableType interlaced;
                error = convert_to_hidl_error(m_mapper.get_metadata_manager()->
                                              get_interlaced(handle, &interlaced));
                interlaced.name = GRALLOC4_STANDARD_INTERLACED;
                ret = android::gralloc4::encodeInterlaced(interlaced, &metadata);
                break;
        }
        case StandardMetadataType::CHROMA_SITING: {
                aidl::android::hardware::graphics::common::ExtendableType chroma_siting;
                error = convert_to_hidl_error(m_mapper.get_metadata_manager()->
                                              get_chroma_siting(handle, &chroma_siting));
                chroma_siting.name = GRALLOC4_STANDARD_CHROMA_SITING;
                ret = android::gralloc4::encodeChromaSiting(chroma_siting, &metadata);
                break;
        }
        case StandardMetadataType::PLANE_LAYOUTS: {
                std::vector<aidl::android::hardware::graphics::common::PlaneLayout> plane_layouts = {};
                error = convert_to_hidl_error(m_mapper.get_metadata_manager()->
                                              get_plane_layouts(handle, &plane_layouts));

                SGR_LOGV("num plane_layouts = %zu", plane_layouts.size());
                ret = android::gralloc4::encodePlaneLayouts(plane_layouts, &metadata);
                break;
        }
        case StandardMetadataType::CROP: {
                std::vector<aidl::android::hardware::graphics::common::Rect> crop = {};
                error = convert_to_hidl_error(m_mapper.get_metadata_manager()->
                                              get_crops(handle, &crop));
                SGR_LOGV("num crop = %zu", crop.size());
                for (uint32_t i = 0; i < crop.size(); i++) {
                        SGR_LOGV("crop [%d], %d, %d, %d, %d", i, crop[i].left, crop[i].top, crop[i].right, crop[i].bottom);
                }
                ret = android::gralloc4::encodeCrop(crop, &metadata);
                break;
        }
        case StandardMetadataType::DATASPACE: {
                int32_t dataspace = 0;
                error = convert_to_hidl_error(m_mapper.get_metadata_manager()->
                                              get_dataspace(handle, &dataspace));
                SGR_LOGV("Dataspace = %" PRId32, dataspace);
                ret = android::gralloc4::encodeDataspace(
                              static_cast<aidl::android::hardware::graphics::common::Dataspace>(dataspace), &metadata);
                break;
        }
        case StandardMetadataType::BLEND_MODE: {
                int32_t blend_mode = 0;
                error = convert_to_hidl_error(m_mapper.get_metadata_manager()->
                                              get_blend_mode(handle, static_cast<int32_t *>(&blend_mode)));
                SGR_LOGV("Blend Mode = %" PRId32, blend_mode);
                ret = android::gralloc4::encodeBlendMode(
                              static_cast<aidl::android::hardware::graphics::common::BlendMode>(blend_mode), &metadata);
                break;
        }
        case StandardMetadataType::SMPTE2086: {
                std::optional<aidl::android::hardware::graphics::common::Smpte2086> smpte2086 = {};
                error = convert_to_hidl_error(m_mapper.get_metadata_manager()->
                                              get_smpte2086(handle, &smpte2086));
                ret = android::gralloc4::encodeSmpte2086(smpte2086, &metadata);
                break;
        }
        case StandardMetadataType::CTA861_3: {
                std::optional<aidl::android::hardware::graphics::common::Cta861_3> cta861_3 = {};
                error = convert_to_hidl_error(m_mapper.get_metadata_manager()->
                                              get_cta861_3(handle, &cta861_3));
                ret = android::gralloc4::encodeCta861_3(cta861_3, &metadata);
                break;
        }
        case StandardMetadataType::SMPTE2094_40: {
                std::optional<std::vector<uint8_t>> smpte2094_40;
                error = convert_to_hidl_error(m_mapper.get_metadata_manager()->
                                              get_smpte2094_40(handle, &smpte2094_40));
                ret = android::gralloc4::encodeSmpte2094_40(smpte2094_40, &metadata);
                break;
        }
        default:
                SGR_LOGE("Not supported MetadataType (%s, %s)",
                         metadataType.name.c_str(), toString(static_cast<StandardMetadataType>(metadataType.value)).c_str());
                error = Error::UNSUPPORTED;
                break;
        }

        if (ret != OK) {
                SGR_LOGD("Encode return value is %" PRId32, ret);
        }

        if (error != Error::NONE) {
                metadata.resize(0);
        }

        _hidl_cb(error, metadata);

        return Void();
}

Return<::android::hardware::graphics::mapper::V4_0::Error> Mapper::set(void* buffer, const ::android::hardware::graphics::mapper::V4_0::IMapper::MetadataType& metadataType, const hidl_vec<uint8_t>& metadata) {
        if (buffer == nullptr) {
                return Error::BAD_BUFFER;
        }

        if (strcmp(metadataType.name.c_str(), GRALLOC4_STANDARD_METADATA_TYPE) != 0) {
                SGR_LOGE("Only StandardMetadataType is supported, got %s", metadataType.name.c_str());
                return Error::UNSUPPORTED;
        }

        if (metadata.size() == 0) {
                // SGR_LOGE("Metadata is empty");
                return Error::UNSUPPORTED;
        }

        using aidl::android::hardware::graphics::common::StandardMetadataType;
        SGR_LOGV("MetadataType (%s, %s)", metadataType.name.c_str(), toString(static_cast<StandardMetadataType>(metadataType.value)).c_str());

        Error error = Error::UNSUPPORTED;
        status_t ret = OK;
        auto handle = reinterpret_cast<native_handle_t*>(buffer);

        switch (static_cast<StandardMetadataType>(metadataType.value)) {
        case StandardMetadataType::CTA861_3:
        case StandardMetadataType::SMPTE2086:
        case StandardMetadataType::SMPTE2094_40:
                if (m_mapper.is_registered_handle(handle) == false) {
                        SGR_LOGE("handle is not imported... failed to set MetadataType(%s)", toString(static_cast<StandardMetadataType>(metadataType.value)).c_str());
                        return Error::BAD_BUFFER;
                }
                break;
        default:
                break;
        }

        switch (static_cast<StandardMetadataType>(metadataType.value)) {

        case StandardMetadataType::INVALID:
                error = Error::UNSUPPORTED;
                break;

        /// The following metadata types cannot be changed after allocation.
        case StandardMetadataType::BUFFER_ID:
        case StandardMetadataType::NAME:
        case StandardMetadataType::WIDTH:
        case StandardMetadataType::HEIGHT:
        case StandardMetadataType::LAYER_COUNT:
        case StandardMetadataType::PIXEL_FORMAT_REQUESTED:
        case StandardMetadataType::USAGE:
                error = Error::BAD_VALUE;
                break;

        case StandardMetadataType::PIXEL_FORMAT_FOURCC: {
                uint32_t pixel_format_fourcc = 0;
                ret = android::gralloc4::decodePixelFormatFourCC(metadata, &pixel_format_fourcc);
                if (ret == OK) {
                        error = convert_to_hidl_error(m_mapper.get_metadata_manager()->
                                                      set_pixel_format_fourcc(handle, pixel_format_fourcc));
                }
                break;
        }
        case StandardMetadataType::PIXEL_FORMAT_MODIFIER: {
                uint64_t pixel_format_modifier = 0;
                ret = android::gralloc4::decodePixelFormatModifier(metadata, &pixel_format_modifier);
                if (ret == OK) {
                        error = convert_to_hidl_error(m_mapper.get_metadata_manager()->
                                                      set_pixel_format_modifier(handle, pixel_format_modifier));
                }
                break;
        }
        case StandardMetadataType::ALLOCATION_SIZE: {
                uint64_t allocation_size = 0;
                ret = android::gralloc4::decodeAllocationSize(metadata, &allocation_size);
                if (ret == OK) {
                        error = convert_to_hidl_error(m_mapper.get_metadata_manager()->
                                                      set_allocation_size(handle, allocation_size));
                }
                break;
        }
        case StandardMetadataType::PROTECTED_CONTENT: {
                uint64_t protected_content = 0;
                ret = android::gralloc4::decodeProtectedContent(metadata, &protected_content);
                if (ret == OK) {
                        error = convert_to_hidl_error(m_mapper.get_metadata_manager()->
                                                      set_protected_content(handle, protected_content));
                }
                break;
        }
        case StandardMetadataType::COMPRESSION: {
                aidl::android::hardware::graphics::common::ExtendableType compression;
                ret = android::gralloc4::decodeCompression(metadata, &compression);
                if (ret == OK) {
                        error = convert_to_hidl_error(m_mapper.get_metadata_manager()->
                                                      set_compression(handle, compression));
                }
                break;
        }
        case StandardMetadataType::INTERLACED: {
                aidl::android::hardware::graphics::common::ExtendableType interlaced;
                ret = android::gralloc4::decodeInterlaced(metadata, &interlaced);
                if (ret == OK) {
                        error = convert_to_hidl_error(m_mapper.get_metadata_manager()->
                                                      set_interlaced(handle, interlaced));
                }
                break;
        }
        case StandardMetadataType::CHROMA_SITING: {
                aidl::android::hardware::graphics::common::ExtendableType chroma_siting;
                ret = android::gralloc4::decodeChromaSiting(metadata, &chroma_siting);
                if (ret == OK) {
                        error = convert_to_hidl_error(m_mapper.get_metadata_manager()->
                                                      set_chroma_siting(handle, chroma_siting));
                }
                break;
        }
        case StandardMetadataType::PLANE_LAYOUTS: {
                std::vector<aidl::android::hardware::graphics::common::PlaneLayout> plane_layouts = {};
                ret = android::gralloc4::decodePlaneLayouts(metadata, &plane_layouts);
                if (ret == OK) {
                        SGR_LOGV("num plane_layouts = %zu", plane_layouts.size());
                        error = convert_to_hidl_error(m_mapper.get_metadata_manager()->
                                                      set_plane_layouts(handle, plane_layouts));
                }
                break;
        }
        case StandardMetadataType::CROP: {
                std::vector<aidl::android::hardware::graphics::common::Rect> crop = {};
                ret = android::gralloc4::decodeCrop(metadata, &crop);
                if (ret == OK) {
                        SGR_LOGV("num crop = %zu", crop.size());
                        for (uint32_t i = 0; i < crop.size(); i++) {
                                SGR_LOGD("crop [%d], %d, %d, %d, %d", i, crop[i].left,
                                             crop[i].top, crop[i].right, crop[i].bottom);
                        }
                        error = convert_to_hidl_error(m_mapper.get_metadata_manager()->
                                                      set_crops(handle, crop));
                }
                break;
        }
        case StandardMetadataType::DATASPACE: {
                aidl::android::hardware::graphics::common::Dataspace dataspace = {};
                ret = android::gralloc4::decodeDataspace(metadata, &dataspace);
                if (ret == OK) {
                        error = convert_to_hidl_error(m_mapper.get_metadata_manager()->
                                                      set_dataspace(handle, static_cast<int32_t>(dataspace)));
                }
                break;
        }
        case StandardMetadataType::BLEND_MODE: {
                aidl::android::hardware::graphics::common::BlendMode blend_mode = {};
                ret = android::gralloc4::decodeBlendMode(metadata, &blend_mode);
                if (ret == OK) {
                        error = convert_to_hidl_error(m_mapper.get_metadata_manager()->
                                                      set_blend_mode(handle, static_cast<int32_t>(blend_mode)));
                }
                break;
        }
        case StandardMetadataType::SMPTE2086: {
                std::optional<aidl::android::hardware::graphics::common::Smpte2086> smpte2086 = {};
                ret = android::gralloc4::decodeSmpte2086(metadata, &smpte2086);
                if (ret == OK) {
                        error = convert_to_hidl_error(m_mapper.get_metadata_manager()->
                                                      set_smpte2086(handle, smpte2086));
                }
                break;
        }
        case StandardMetadataType::CTA861_3: {
                std::optional<aidl::android::hardware::graphics::common::Cta861_3> cta861_3 = {};
                ret = android::gralloc4::decodeCta861_3(metadata, &cta861_3);
                if (ret == OK) {
                        error = convert_to_hidl_error(m_mapper.get_metadata_manager()->
                                                      set_cta861_3(handle, cta861_3));
                }
                break;
        }
        case StandardMetadataType::SMPTE2094_40: {
                std::optional<std::vector<uint8_t>> smpte2094_40;
                ret = android::gralloc4::decodeSmpte2094_40(metadata, &smpte2094_40);
                if (ret == OK) {
                        // decoding from byte stream can be std::nullopt for reset purpose
                        bool is_reset = (smpte2094_40 == std::nullopt) ? true : false;
                        error = convert_to_hidl_error(m_mapper.get_metadata_manager()->
                                                      set_smpte2094_40(handle, smpte2094_40, is_reset));
                }
                break;
        }
        default:
                SGR_LOGE("Not supported MetadataType (%s, %s)",
                         metadataType.name.c_str(), toString(static_cast<StandardMetadataType>(metadataType.value)).c_str());
                error = Error::UNSUPPORTED;
                break;
        }

        if (ret != OK) {
                SGR_LOGD("Decode return value is %" PRId32, ret);
        }

        return error;
}

Return<void> Mapper::getFromBufferDescriptorInfo(const ::android::hardware::graphics::mapper::V4_0::IMapper::BufferDescriptorInfo& description, const ::android::hardware::graphics::mapper::V4_0::IMapper::MetadataType& metadataType, getFromBufferDescriptorInfo_cb _hidl_cb) {
        hidl_vec<uint8_t> metadata;

        if (strcmp(metadataType.name.c_str(), GRALLOC4_STANDARD_METADATA_TYPE) != 0) {
                SGR_LOGE("Only StandardMetadataType is supported, got %s", metadataType.name.c_str());
                _hidl_cb(Error::UNSUPPORTED, metadata);
                return Void();
        }

        using aidl::android::hardware::graphics::common::StandardMetadataType;
        SGR_LOGV("MetadataType (%s, %s)", metadataType.name.c_str(), toString(static_cast<StandardMetadataType>(metadataType.value)).c_str());

        Error error = Error::NONE;
        status_t ret = OK;

        switch (static_cast<StandardMetadataType>(metadataType.value)) {
        case StandardMetadataType::NAME: {
                SGR_ASSERT(description.name.size() >= 0);
                SGR_ASSERT(description.name.c_str() != nullptr);

                if (description.name.size() > SGR_MAX_LENGTH_NAME) {
                        SGR_LOGD("Warning: Name length is %zu while maximum length can be %d",
                                 description.name.size(), SGR_MAX_LENGTH_NAME);
                }
                std::string name = description.name.c_str();
                SGR_LOGV("Name = %s", name.c_str());
                ret = android::gralloc4::encodeName(name, &metadata);
                break;
        }
        case StandardMetadataType::WIDTH: {
                uint64_t width = static_cast<uint64_t>(description.width);
                SGR_LOGV("Width = %" PRId64, width);
                ret = android::gralloc4::encodeWidth(width, &metadata);
                break;
        }
        case StandardMetadataType::HEIGHT: {
                uint64_t height = static_cast<uint64_t>(description.height);
                SGR_LOGV("Height = %" PRId64, height);
                ret = android::gralloc4::encodeHeight(height, &metadata);
                break;
        }
        case StandardMetadataType::LAYER_COUNT: {
                uint64_t layer_count = static_cast<uint64_t>(description.layerCount);
                SGR_LOGV("Layer Count = %" PRId64, layer_count);
                ret = android::gralloc4::encodeLayerCount(layer_count, &metadata);
                break;
        }
        case StandardMetadataType::PIXEL_FORMAT_REQUESTED: {
                SGR_LOGV("Pixel Format Requested = 0x%" PRIx32, description.format);
                ret = android::gralloc4::encodePixelFormatRequested(description.format, &metadata);
                break;
        }
        case StandardMetadataType::USAGE: {
                uint64_t usage = static_cast<uint64_t>(description.usage);
                SGR_LOGV("Usage = 0x%" PRIx64, usage);
                ret = android::gralloc4::encodeUsage(usage, &metadata);
                break;
        }
        default:
                ///@todo GFXSW-5091 Add support for more MetadataTypes
                SGR_LOGE("Not supported MetadataType (%s, %s)", metadataType.name.c_str(),
                         toString(static_cast<StandardMetadataType>(metadataType.value)).c_str());
                error = Error::UNSUPPORTED;
                break;
        }

        SGR_ASSERT(ret == OK);
        if (error != Error::NONE) {
                metadata.resize(0);
        }

        _hidl_cb(error, metadata);

        return Void();
}

Return<void> Mapper::listSupportedMetadataTypes(listSupportedMetadataTypes_cb _hidl_cb) {

        const std::vector<SGR_METADATA_TYPE_DESCRIPTION> *in_ptr = m_mapper.get_metadata_manager()->get_metadata_list();
        const std::vector<AIDL_METADATA_TYPE_DESCRIPTION> *out_ptr = convert_to_hidl_metadata(in_ptr);

        hidl_vec<AIDL_METADATA_TYPE_DESCRIPTION> metadata_description_list{*out_ptr};
        _hidl_cb(Error::NONE, metadata_description_list);
        return Void();
}

Return<void> Mapper::dumpBuffer(void* buffer, dumpBuffer_cb _hidl_cb) {
        if (buffer == nullptr) {
                SGR_LOGE("Invalid buffer handle");
                _hidl_cb(Error::BAD_BUFFER, IMapper::BufferDump());
                return Void();
        }

        const std::vector<SGR_METADATA_TYPE_DESCRIPTION> *supported_metadata_list =
                                                                m_mapper.get_metadata_manager()->get_metadata_list();

        std::vector<IMapper::MetadataDump> metadata_dumps{};
        for (auto list_entry : *supported_metadata_list) {
                if (list_entry.is_gettable == false) {
                        continue;
                }

                IMapper::MetadataType metadata_type{list_entry.metadata_type.name, list_entry.metadata_type.value};
                get(buffer, metadata_type, [&metadata_dumps, &metadata_type](Error error, hidl_vec<uint8_t> metadata) {
                        switch (error) {
                        case Error::NONE:
                                metadata_dumps.push_back({metadata_type, metadata});
                                break;
                        default:
                                break;
                        }
                });
        }

        IMapper::BufferDump buffer_dump{metadata_dumps};
        _hidl_cb(Error::NONE, buffer_dump);

        return Void();
}

Return<void> Mapper::dumpBuffers(dumpBuffers_cb _hidl_cb) {
        std::vector<IMapper::BufferDump> buffer_dumps{};
        Error return_error = Error::NONE;
        {
                std::lock_guard<std::mutex> lock(m_mapper.get_imported_handles_lock());
                const std::unordered_set<const native_handle_t *> &mapper_imported_handles = m_mapper.get_imported_handles();
                for(const native_handle_t *buffer : mapper_imported_handles) {
                        dumpBuffer(static_cast<void *>(const_cast<native_handle_t *>(buffer)), [&](Error error, IMapper::BufferDump buffer_dump){
                                SGR_ASSERT_MSG(error != Error::BAD_BUFFER,
                                                "MapperHidl::dumpBuffer() returned Error::BAD_BUFFER for native_handle_t = %p",
                                                buffer);

                                switch (error) {
                                case Error::NONE:
                                        buffer_dumps.push_back(buffer_dump);
                                        break;
                                case Error::NO_RESOURCES:
                                        return_error = error;
                                        break;
                                default:
                                        break;
                                }
                        });
                }
        }

        _hidl_cb(return_error, buffer_dumps);

        return Void();
}

Return<void> Mapper::getReservedRegion(void* buffer, getReservedRegion_cb _hidl_cb) {
        if (buffer == nullptr) {
                _hidl_cb(Error::BAD_BUFFER, 0, 0);
                return Void();
        }

        auto handle = reinterpret_cast<native_handle_t*>(buffer);
        uint64_t reserved_region_size = 0;
        void *reserved_region = nullptr;
        Error error = convert_to_hidl_error(m_mapper.get_reserved_region(handle, &reserved_region,
                                                                         &reserved_region_size));

        _hidl_cb(error, reserved_region, reserved_region_size);

        return Void();
}


// Methods from ::android::hidl::base::V1_0::IBase follow.

IMapper* HIDL_FETCH_IMapper(const char*  name) {
        SGR_UNUSED(name);
        SGR_LOGI("android.hardware.graphics.mapper@4.0: Loaded Mapper successfully.");

        return new Mapper();
}

}  // namespace implementation
}  // namespace V4_0
}  // namespace mapper
}  // namespace graphics
}  // namespace hardware
}  // namespace android
