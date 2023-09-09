///
/// @file svts.inl
/// @copyright 2020 Samsung Electronics
///

// This file is used from Android Gralloc Mapper VTS

#undef SVTS_ENABLE_LOG

enum class SgrPixelFormat : int32_t {
        UNDEFINED = 0,                  // private
        RGBA_8888 = 1,
        RGBX_8888 = 2,
        RGB_888 = 3,
        RGB_565 = 4,
        BGRA_8888 = 5,
        YCBCR_422_SP = 16,
        YCRCB_420_SP = 17,
        YCBCR_422_I = 20,
        RGBA_FP16 = 22,
        RAW16 = 32,
        BLOB = 33,
        IMPLEMENTATION_DEFINED = 34,
        YCBCR_420_888 = 35,
        RAW_OPAQUE = 36,
        RAW10 = 37,
        RAW12 = 38,
        RGBA_1010102 = 43,
        DEPTH_16 = 48,
        DEPTH_24 = 49,
        DEPTH_24_STENCIL_8 = 50,
        DEPTH_32F = 51,
        DEPTH_32F_STENCIL_8 = 52,
        STENCIL_8 = 53,
        YCBCR_P010 = 54,
        HSV_888 = 55 /* 0x37 */,
        Y8 = 538982489,
        Y16 = 540422489,
        YV12 = 842094169,

        /// PrivateFormat
        /// Invariant: PrivateFormat should be non-overlapping with to PixelFormat
        /// From exynos definition hardware/samsung_slsi/exynos/include/exynos_format.h
        PRIVATE_YCBCR_420_P_M               = 0x101,
        PRIVATE_CBYCRY_422_I                = 0x103,
        PRIVATE_YCBCR_420_SP_M              = 0x105,
        PRIVATE_YCRCB_422_SP                = 0x106,
        PRIVATE_YCBCR_420_SP_M_TILED        = 0x107,
        PRIVATE_ARGB_8888                   = 0x108,
        PRIVATE_YCRCB_422_I                 = 0x116,   /* HAL_PIXEL_FORMAT_CUSTOM_YCrCb_422_I */
        PRIVATE_CRYCBY_422_I                = 0x118,   /* HAL_PIXEL_FORMAT_CUSTOM_CrYCbY_422_I */
        PRIVATE_YV12_M                      = 0x11C,   /* YV12 */
        PRIVATE_YCRCB_420_SP_M              = 0x11D,   /* YCrCb_420_SP */
        PRIVATE_YCRCB_420_SP_M_FULL         = 0x11E,   /* YCrCb_420_SP_FULL */
        PRIVATE_YCBCR_420_P                 = 0x11F,
        PRIVATE_YCBCR_420_SP                = 0x120,
        /* YCbCr_420_SP_M w/ private data buffer */
        PRIVATE_YCBCR_420_SP_M_PRIV         = 0x121,
        /* contiguous(single fd) custom formats */
        PRIVATE_YCBCR_420_PN                = 0x122,
        PRIVATE_YCBCR_420_SPN               = 0x123,
        PRIVATE_YCBCR_420_SPN_TILED         = 0x124,
        /* 10-bit format (8bit + separated 2bit) w/ private data buffer */
        PRIVATE_YCBCR_420_SP_M_S10B         = 0x125,
        /* 10-bit contiguous(single fd, 8bit + separated 2bit) custom formats */
        PRIVATE_YCBCR_420_SPN_S10B          = 0x126,
        /* 10-bit format (2 fd, 10bit, 2x byte) custom formats */
        PRIVATE_YCBCR_P010_M                = 0x127,
        /* YUV 4:2:2 planar, Y/Cb/Cr */
        PRIVATE_YCBCR_422_P = 0x128,
        /* SBWC format */
        PRIVATE_YCBCR_420_SP_M_SBWC         = 0x130,
        PRIVATE_YCBCR_420_SPN_SBWC          = 0x131,
        PRIVATE_YCBCR_420_SP_M_10B_SBWC     = 0x132,
        PRIVATE_YCBCR_420_SPN_10B_SBWC      = 0x133,
        PRIVATE_YCrCb_420_SP_M_SBWC         = 0x134,
        PRIVATE_YCrCb_420_SP_M_10B_SBWC     = 0x135,
        /* SBWC Lossy formats */
        PRIVATE_YCBCR_420_SP_M_SBWC_L50     = 0x140,
        PRIVATE_YCBCR_420_SP_M_SBWC_L75     = 0x141,
        PRIVATE_YCBCR_420_SPN_SBWC_L50      = 0x150,
        PRIVATE_YCBCR_420_SPN_SBWC_L75      = 0x151,
        PRIVATE_YCBCR_420_SP_M_10B_SBWC_L40 = 0x160,
        PRIVATE_YCBCR_420_SP_M_10B_SBWC_L60 = 0x161,
        PRIVATE_YCBCR_420_SP_M_10B_SBWC_L80 = 0x162,
        PRIVATE_YCBCR_420_SPN_10B_SBWC_L40  = 0x170,
        PRIVATE_YCBCR_420_SPN_10B_SBWC_L60  = 0x171,
        PRIVATE_YCBCR_420_SPN_10B_SBWC_L80  = 0x172,
};

enum class SgrBufferUsage : uint64_t {
        CPU_READ_MASK = 15ull /* 0xfULL */,
        CPU_READ_NEVER = 0ull,
        CPU_READ_RARELY = 2ull,
        CPU_READ_OFTEN = 3ull,
        CPU_WRITE_MASK = 240ull /* (0xfULL << 4) */,
        CPU_WRITE_NEVER = 0ull /* (0 << 4) */,
        CPU_WRITE_RARELY = 32ull /* (2 << 4) */,
        CPU_WRITE_OFTEN = 48ull /* (3 << 4) */,
        GPU_TEXTURE = 256ull /* (1ULL << 8) */,
        GPU_RENDER_TARGET = 512ull /* (1ULL << 9) */,
        COMPOSER_OVERLAY = 2048ull /* (1ULL << 11) */,
        COMPOSER_CLIENT_TARGET = 4096ull /* (1ULL << 12) */,
        PROTECTED = 16384ull /* (1ULL << 14) */,
        COMPOSER_CURSOR = 32768ull /* (1ULL << 15) */,
        VIDEO_ENCODER = 65536ull /* (1ULL << 16) */,
        CAMERA_OUTPUT = 131072ull /* (1ULL << 17) */,
        CAMERA_INPUT = 262144ull /* (1ULL << 18) */,
        RENDERSCRIPT = 1048576ull /* (1ULL << 20) */,
        VIDEO_DECODER = 4194304ull /* (1ULL << 22) */,
        SENSOR_DIRECT_DATA = 8388608ull /* (1ULL << 23) */,
        GPU_DATA_BUFFER = 16777216ull /* (1ULL << 24) */,
        GPU_CUBE_MAP = 33554432ull /* (1ULL << 25) */,
        GPU_MIPMAP_COMPLETE = 67108864ull /* (1ULL << 26) */,
        HW_IMAGE_ENCODER = 134217728ull /* (1ULL << 27) */,

        VENDOR_MASK = 4026531840ull /* (0xfULL << 28) */,
        PRIVATE_PROTECTED_DPB = (1ULL << 28),
        /* = (1ULL << 29), */
        PRIVATE_CAMERA_RESERVED = (1ULL << 30),
        PRIVATE_SECURE_CAMERA_RESERVED = (1ULL << 31),

        VENDOR_MASK_HI = 18446462598732840960ull /* (0xffffULL << 48) */,
        /* = (1ULL << 48), */
        /* = (1ULL << 49), */
        /* = (1ULL << 50), */
        /* = (1ULL << 51), */
        PRIVATE_ROIINFO = (1ULL << 52),
        /* = (1ULL << 53), */
        PRIVATE_FORCE_BACKBUFFER = (1ULL << 54),
        PRIVATE_FRONTBUFFER = (1ULL << 55),
        PRIVATE_SBWC_REQUEST_10BIT = (1ULL << 56),
        PRIVATE_HFR_MODE = (1ULL << 57),
        PRIVATE_NOZEROED = (1ULL << 58),
        PRIVATE_PRIVATE_NONSECURE = (1ULL << 59),
        PRIVATE_VIDEO_PRIVATE_DATA = (1ULL << 60),
        PRIVATE_VIDEO_EXT = (1ULL << 61),
        PRIVATE_DAYDREAM_SINGLE_BUFFER_MODE = (1ULL << 62),
        PRIVATE_YUV_RANGE_FULL = (1ULL << 63),
};

std::string get_pixelformat_string(SgrPixelFormat info)
{
        switch(info) {
        case SgrPixelFormat::RGBA_8888:                           return std::string("RGBA_8888");
        case SgrPixelFormat::RGBX_8888:                           return std::string("RGBX_8888");
        case SgrPixelFormat::RGB_888:                             return std::string("RGB_888");
        case SgrPixelFormat::RGB_565:                             return std::string("RGB_565");
        case SgrPixelFormat::BGRA_8888:                           return std::string("BGRA_8888");
        case SgrPixelFormat::YCBCR_422_SP:                        return std::string("YCBCR_422_SP");
        case SgrPixelFormat::YCRCB_420_SP:                        return std::string("YCRCB_420_SP");
        case SgrPixelFormat::YCBCR_422_I:                         return std::string("YCBCR_422_I");
        case SgrPixelFormat::RGBA_FP16:                           return std::string("RGBA_FP16");
        case SgrPixelFormat::RAW16:                               return std::string("RAW16");
        case SgrPixelFormat::BLOB:                                return std::string("BLOB");
        case SgrPixelFormat::IMPLEMENTATION_DEFINED:              return std::string("IMPLEMENTATION_DEFINED");
        case SgrPixelFormat::YCBCR_420_888:                       return std::string("YCBCR_420_888");
        case SgrPixelFormat::RAW_OPAQUE:                          return std::string("RAW_OPAQUE");
        case SgrPixelFormat::RAW10:                               return std::string("RAW10");
        case SgrPixelFormat::RAW12:                               return std::string("RAW12");
        case SgrPixelFormat::RGBA_1010102:                        return std::string("RGBA_1010102");
        case SgrPixelFormat::DEPTH_16:                            return std::string("DEPTH_16");
        case SgrPixelFormat::DEPTH_24:                            return std::string("DEPTH_24");
        case SgrPixelFormat::DEPTH_24_STENCIL_8:                  return std::string("DEPTH_24_STENCIL_8");
        case SgrPixelFormat::DEPTH_32F:                           return std::string("DEPTH_32F");
        case SgrPixelFormat::DEPTH_32F_STENCIL_8:                 return std::string("DEPTH_32F_STENCIL_8");
        case SgrPixelFormat::STENCIL_8:                           return std::string("STENCIL_8");
        case SgrPixelFormat::YCBCR_P010:                          return std::string("YCBCR_P010");
        case SgrPixelFormat::HSV_888:                             return std::string("HSV_888");
        case SgrPixelFormat::Y8:                                  return std::string("Y8");
        case SgrPixelFormat::Y16:                                 return std::string("Y16");
        case SgrPixelFormat::YV12:                                return std::string("YV12");
        case SgrPixelFormat::PRIVATE_YCBCR_420_P_M:               return std::string("PRIVATE_YCBCR_420_P_M");
        case SgrPixelFormat::PRIVATE_CBYCRY_422_I:                return std::string("PRIVATE_CBYCRY_422_I");
        case SgrPixelFormat::PRIVATE_YCBCR_420_SP_M:              return std::string("PRIVATE_YCBCR_420_SP_M");
        case SgrPixelFormat::PRIVATE_YCRCB_422_SP:                return std::string("PRIVATE_YCRCB_422_SP");
        case SgrPixelFormat::PRIVATE_YCBCR_420_SP_M_TILED:        return std::string("PRIVATE_YCBCR_420_SP_M_TILED");
        case SgrPixelFormat::PRIVATE_ARGB_8888:                   return std::string("PRIVATE_ARGB_8888");
        case SgrPixelFormat::PRIVATE_YCRCB_422_I:                 return std::string("PRIVATE_YCRCB_422_I");
        case SgrPixelFormat::PRIVATE_CRYCBY_422_I:                return std::string("PRIVATE_CRYCBY_422_I");
        case SgrPixelFormat::PRIVATE_YV12_M:                      return std::string("PRIVATE_YV12_M");
        case SgrPixelFormat::PRIVATE_YCRCB_420_SP_M:              return std::string("PRIVATE_YCRCB_420_SP_M");
        case SgrPixelFormat::PRIVATE_YCRCB_420_SP_M_FULL:         return std::string("PRIVATE_YCRCB_420_SP_M_FULL");
        case SgrPixelFormat::PRIVATE_YCBCR_420_P:                 return std::string("PRIVATE_YCBCR_420_P");
        case SgrPixelFormat::PRIVATE_YCBCR_420_SP:                return std::string("PRIVATE_YCBCR_420_SP");
        case SgrPixelFormat::PRIVATE_YCBCR_420_SP_M_PRIV:         return std::string("PRIVATE_YCBCR_420_SP_M_PRIV");
        case SgrPixelFormat::PRIVATE_YCBCR_420_PN:                return std::string("PRIVATE_YCBCR_420_PN");
        case SgrPixelFormat::PRIVATE_YCBCR_420_SPN:               return std::string("PRIVATE_YCBCR_420_SPN");
        case SgrPixelFormat::PRIVATE_YCBCR_420_SPN_TILED:         return std::string("PRIVATE_YCBCR_420_SPN_TILED");
        case SgrPixelFormat::PRIVATE_YCBCR_420_SP_M_S10B:         return std::string("PRIVATE_YCBCR_420_SP_M_S10B");
        case SgrPixelFormat::PRIVATE_YCBCR_420_SPN_S10B:          return std::string("PRIVATE_YCBCR_420_SPN_S10B");
        case SgrPixelFormat::PRIVATE_YCBCR_P010_M:                return std::string("PRIVATE_YCBCR_P010_M");
        case SgrPixelFormat::PRIVATE_YCBCR_422_P:                 return std::string("PRIVATE_YCBCR_422_P");
        case SgrPixelFormat::PRIVATE_YCBCR_420_SP_M_SBWC:         return std::string("PRIVATE_YCBCR_420_SP_M_SBWC");
        case SgrPixelFormat::PRIVATE_YCBCR_420_SPN_SBWC:          return std::string("PRIVATE_YCBCR_420_SPN_SBWC");
        case SgrPixelFormat::PRIVATE_YCBCR_420_SP_M_10B_SBWC:     return std::string("PRIVATE_YCBCR_420_SP_M_10B_SBWC");
        case SgrPixelFormat::PRIVATE_YCBCR_420_SPN_10B_SBWC:      return std::string("PRIVATE_YCBCR_420_SPN_10B_SBWC");
        case SgrPixelFormat::PRIVATE_YCrCb_420_SP_M_SBWC:         return std::string("PRIVATE_YCrCb_420_SP_M_SBWC");
        case SgrPixelFormat::PRIVATE_YCrCb_420_SP_M_10B_SBWC:     return std::string("PRIVATE_YCrCb_420_SP_M_10B_SBWC");
        case SgrPixelFormat::PRIVATE_YCBCR_420_SP_M_SBWC_L50:     return std::string("PRIVATE_YCBCR_420_SP_M_SBWC_L50");
        case SgrPixelFormat::PRIVATE_YCBCR_420_SP_M_SBWC_L75:     return std::string("PRIVATE_YCBCR_420_SP_M_SBWC_L75");
        case SgrPixelFormat::PRIVATE_YCBCR_420_SPN_SBWC_L50:      return std::string("PRIVATE_YCBCR_420_SPN_SBWC_L50");
        case SgrPixelFormat::PRIVATE_YCBCR_420_SPN_SBWC_L75:      return std::string("PRIVATE_YCBCR_420_SPN_SBWC_L75");
        case SgrPixelFormat::PRIVATE_YCBCR_420_SP_M_10B_SBWC_L40: return std::string("PRIVATE_YCBCR_420_SP_M_10B_SBWC_L40");
        case SgrPixelFormat::PRIVATE_YCBCR_420_SP_M_10B_SBWC_L60: return std::string("PRIVATE_YCBCR_420_SP_M_10B_SBWC_L60");
        case SgrPixelFormat::PRIVATE_YCBCR_420_SP_M_10B_SBWC_L80: return std::string("PRIVATE_YCBCR_420_SP_M_10B_SBWC_L80");
        case SgrPixelFormat::PRIVATE_YCBCR_420_SPN_10B_SBWC_L40:  return std::string("PRIVATE_YCBCR_420_SPN_10B_SBWC_L40");
        case SgrPixelFormat::PRIVATE_YCBCR_420_SPN_10B_SBWC_L60:  return std::string("PRIVATE_YCBCR_420_SPN_10B_SBWC_L60");
        case SgrPixelFormat::PRIVATE_YCBCR_420_SPN_10B_SBWC_L80:  return std::string("PRIVATE_YCBCR_420_SPN_10B_SBWC_L80");
        default:                                               return std::to_string(static_cast<uint32_t>(info));
        }
}

class SgrMapperBase {
 protected:
        void SetUp() {
                ASSERT_NO_FATAL_FAILURE(mGralloc = std::make_unique<Gralloc>());

                ASSERT_NE(nullptr, mGralloc->getAllocator().get());
                ASSERT_NE(nullptr, mGralloc->getMapper().get());

                mDummyDescriptorInfo.name = "dummy";
                mDummyDescriptorInfo.width = 64;
                mDummyDescriptorInfo.height = 64;
                mDummyDescriptorInfo.layerCount = 1;
                mDummyDescriptorInfo.format = PixelFormat::RGBA_8888;
                mDummyDescriptorInfo.usage =
                        static_cast<uint64_t>(BufferUsage::CPU_WRITE_OFTEN | BufferUsage::CPU_READ_OFTEN);
                mDummyDescriptorInfo.reservedSize = 0;
        }

        void TearDown() {}

        void dump_descriptor(IMapper::BufferDescriptorInfo info)
        {
#ifdef SVTS_ENABLE_LOG
                std::cout<< ".width = "      << info.width                         << ", ";
                std::cout<< ".height = "     << info.height                        << ", ";
                std::cout<< ".layerCount = " << info.layerCount                    << ", ";
                std::cout<< ".format = "     << static_cast<uint64_t>(info.format) << ", ";
                std::cout<< ".usage = "      << info.usage                         << "\n";
#else
                (void)info;
#endif
        }

        std::unique_ptr<Gralloc> mGralloc;
        IMapper::BufferDescriptorInfo mDummyDescriptorInfo{};
};

//-----------------------------------------------------------------------------
class SgrFormatTest :
public SgrMapperBase,
public ::testing::TestWithParam<SgrPixelFormat> {
 protected:
        void SetUp() override {
                SgrMapperBase::SetUp();
        }

        void TearDown() override {
                SgrMapperBase::TearDown();
        }
};

TEST_P(SgrFormatTest, IsSupported) {
    auto info = mDummyDescriptorInfo;
    info.format = static_cast<PixelFormat>(GetParam());
    bool supported = false;

    supported = mGralloc->isSupported(info);
    ASSERT_NO_FATAL_FAILURE(supported = mGralloc->isSupported(info));
}


std::string get_parameterized_string(testing::TestParamInfo<SgrFormatTest::ParamType> info)
{
        return get_pixelformat_string(info.param);
}

INSTANTIATE_TEST_CASE_P(
        , SgrFormatTest,
        testing::Values(
                SgrPixelFormat::RGBA_8888,
                SgrPixelFormat::RGBX_8888,
                SgrPixelFormat::RGB_888,
                SgrPixelFormat::RGB_565,
                SgrPixelFormat::BGRA_8888,
                SgrPixelFormat::YCBCR_422_SP,
                SgrPixelFormat::YCRCB_420_SP,
                SgrPixelFormat::YCBCR_422_I,
                SgrPixelFormat::RGBA_FP16,
                SgrPixelFormat::RAW16,
                SgrPixelFormat::BLOB,
                SgrPixelFormat::IMPLEMENTATION_DEFINED,
                SgrPixelFormat::YCBCR_420_888,
                SgrPixelFormat::RAW_OPAQUE,
                SgrPixelFormat::RAW10,
                SgrPixelFormat::RAW12,
                SgrPixelFormat::RGBA_1010102,
                SgrPixelFormat::DEPTH_16,
                SgrPixelFormat::DEPTH_24,
                SgrPixelFormat::DEPTH_24_STENCIL_8,
                SgrPixelFormat::DEPTH_32F,
                SgrPixelFormat::DEPTH_32F_STENCIL_8,
                SgrPixelFormat::STENCIL_8,
                SgrPixelFormat::YCBCR_P010,
                SgrPixelFormat::HSV_888,
                SgrPixelFormat::Y8,
                SgrPixelFormat::Y16,
                SgrPixelFormat::YV12,
                SgrPixelFormat::PRIVATE_YCBCR_420_P_M,
                SgrPixelFormat::PRIVATE_YCBCR_420_SP_M,
                SgrPixelFormat::PRIVATE_YCBCR_420_SP_M_TILED,
                SgrPixelFormat::PRIVATE_YV12_M,
                SgrPixelFormat::PRIVATE_YCRCB_420_SP_M,
                SgrPixelFormat::PRIVATE_YCRCB_420_SP_M_FULL,
                SgrPixelFormat::PRIVATE_YCBCR_420_P,
                SgrPixelFormat::PRIVATE_YCBCR_420_SP,
                SgrPixelFormat::PRIVATE_YCBCR_420_SPN,
                SgrPixelFormat::PRIVATE_YCBCR_420_SP_M_S10B,
                SgrPixelFormat::PRIVATE_YCBCR_420_SPN_S10B,
                SgrPixelFormat::PRIVATE_YCBCR_P010_M,
                SgrPixelFormat::PRIVATE_YCBCR_420_SP_M_SBWC,
                SgrPixelFormat::PRIVATE_YCBCR_420_SPN_SBWC,
                SgrPixelFormat::PRIVATE_YCBCR_420_SP_M_10B_SBWC,
                SgrPixelFormat::PRIVATE_YCBCR_420_SPN_10B_SBWC,
                SgrPixelFormat::PRIVATE_YCrCb_420_SP_M_SBWC,
                SgrPixelFormat::PRIVATE_YCrCb_420_SP_M_10B_SBWC,
                SgrPixelFormat::PRIVATE_YCBCR_420_SP_M_SBWC_L50,
                SgrPixelFormat::PRIVATE_YCBCR_420_SP_M_SBWC_L75,
                SgrPixelFormat::PRIVATE_YCBCR_420_SPN_SBWC_L50,
                SgrPixelFormat::PRIVATE_YCBCR_420_SPN_SBWC_L75,
                SgrPixelFormat::PRIVATE_YCBCR_420_SP_M_10B_SBWC_L40,
                SgrPixelFormat::PRIVATE_YCBCR_420_SP_M_10B_SBWC_L60,
                SgrPixelFormat::PRIVATE_YCBCR_420_SP_M_10B_SBWC_L80,
                SgrPixelFormat::PRIVATE_YCBCR_420_SPN_10B_SBWC_L40,
                SgrPixelFormat::PRIVATE_YCBCR_420_SPN_10B_SBWC_L60,
                SgrPixelFormat::PRIVATE_YCBCR_420_SPN_10B_SBWC_L80
        ),
        get_parameterized_string);

//-----------------------------------------------------------------------------
#define DEFINE_DESCRIPTOR_PARMETERIZED(test_name, descriptors)          \
class SgrDescriptorTest##test_name :                                    \
public SgrMapperBase,                                                   \
public ::testing::TestWithParam<IMapper::BufferDescriptorInfo> {        \
 protected:                                                             \
        void SetUp() override {                                         \
                SgrMapperBase::SetUp();                                 \
        }                                                               \
                                                                        \
        void TearDown() override {                                      \
                SgrMapperBase::TearDown();                              \
        }                                                               \
};                                                                      \
                                                                        \
TEST_P(SgrDescriptorTest##test_name, AllocatorAllocate) {               \
        auto info = GetParam();                                         \
                                                                        \
        dump_descriptor(info);                                          \
        const native_handle_t *bufferHandle = nullptr;                  \
        ASSERT_NO_FATAL_FAILURE(bufferHandle = mGralloc->allocate(info, true)); \
        mGralloc->freeBuffer(bufferHandle);                             \
}                                                                       \
                                                                        \
INSTANTIATE_TEST_CASE_P(,                                               \
        SgrDescriptorTest##test_name,                                   \
        ::testing::ValuesIn(descriptors));

IMapper::BufferDescriptorInfo DescriptorInfoListHomeScreen [] = {
        {
                .width = 960,
                .height = 800,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_1010102),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 46,
                .height = 46,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET)
        },
        {
                .width = 16384,
                .height = 1,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::BLOB),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN)
        },
        {
                .width = 1440,
                .height = 3200,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET)
        },
        {
                .width = 1,
                .height = 1,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 88,
                .height = 94,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE)
        },
        {
                .width = 1440,
                .height = 3200,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 32768,
                .height = 1,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::BLOB),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN)
        },
        {
                .width = 1440,
                .height = 72,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 1440,
                .height = 3200,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_CLIENT_TARGET)
        },
        {
                .width = 1440,
                .height = 144,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 1440,
                .height = 3200,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 167,
                .height = 167,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE)
        },
        {
                .width = 1440,
                .height = 3200,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_1010102),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
};

IMapper::BufferDescriptorInfo DescriptorInfoListBasicApps [] = {
        {
                .width = 1440,
                .height = 3200,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET)
        },
        {
                .width = 1440,
                .height = 3200,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 1440,
                .height = 3200,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 720,
                .height = 1600,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE)
        },
        {
                .width = 1560,
                .height = 3174,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 1440,
                .height = 2984,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBX_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
};

IMapper::BufferDescriptorInfo DescriptorInfoListGfxBench [] = {
        {
                .width = 2678,
                .height = 1440,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 1440,
                .height = 2678,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 1440,
                .height = 3200,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET)
        },
        {
                .width = 88,
                .height = 94,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE)
        },
        {
                .width = 1440,
                .height = 3200,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY) |
                         static_cast<uint64_t>(SgrBufferUsage::PRIVATE_PRIVATE_NONSECURE)
        },
        {
                .width = 1440,
                .height = 2678,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBX_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 1440,
                .height = 3200,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 1440,
                .height = 72,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 1440,
                .height = 2678,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 3200,
                .height = 1440,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET)
        },
        {
                .width = 1440,
                .height = 144,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 72,
                .height = 3200,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
};

IMapper::BufferDescriptorInfo DescriptorInfoListMxplayer [] = {
        {
                .width = 3200,
                .height = 72,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 3840,
                .height = 2160,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::PRIVATE_YCBCR_420_SP_M_SBWC),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY) |
                         static_cast<uint64_t>(SgrBufferUsage::VIDEO_ENCODER)
        },
        {
                .width = 1440,
                .height = 72,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 4096,
                .height = 1,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::BLOB),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN)
        },
        {
                .width = 1024,
                .height = 1,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::BLOB),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::VIDEO_ENCODER)
        },
        {
                .width = 72,
                .height = 3200,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 720,
                .height = 1600,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE)
        },
        {
                .width = 1440,
                .height = 3200,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET)
        },
        {
                .width = 88,
                .height = 94,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE)
        },
        {
                .width = 1440,
                .height = 3200,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY) |
                         static_cast<uint64_t>(SgrBufferUsage::PRIVATE_PRIVATE_NONSECURE)
        },
        {
                .width = 3056,
                .height = 1440,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET)
        },
        {
                .width = 2048,
                .height = 1,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::BLOB),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN)
        },
        {
                .width = 1920,
                .height = 1088,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::YCBCR_420_888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY) |
                         static_cast<uint64_t>(SgrBufferUsage::VIDEO_ENCODER) |
                         static_cast<uint64_t>(SgrBufferUsage::PRIVATE_VIDEO_PRIVATE_DATA)
        },
        {
                .width = 8192,
                .height = 1,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::BLOB),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN)
        },
        {
                .width = 1920,
                .height = 1088,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::PRIVATE_YCBCR_420_SP_M_SBWC),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY) |
                         static_cast<uint64_t>(SgrBufferUsage::VIDEO_ENCODER)
        },
        {
                .width = 1440,
                .height = 3200,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 3200,
                .height = 1440,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET)
        },
        {
                .width = 1560,
                .height = 3174,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 1440,
                .height = 3200,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 3840,
                .height = 2160,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::YCBCR_420_888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY) |
                         static_cast<uint64_t>(SgrBufferUsage::VIDEO_ENCODER) |
                         static_cast<uint64_t>(SgrBufferUsage::PRIVATE_VIDEO_PRIVATE_DATA)
        },
        {
                .width = 1440,
                .height = 144,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 1343,
                .height = 570,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 640,
                .height = 480,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::YCBCR_420_888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 7340032,
                .height = 1,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::BLOB),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN)
        },
        {
                .width = 1440,
                .height = 951,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
};

IMapper::BufferDescriptorInfo DescriptorInfoListCamera [] = {
        {
                .width = 3840,
                .height = 2160,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::YV12),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN)
        },
        {
                .width = 16384,
                .height = 1,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::BLOB),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN)
        },
        {
                .width = 6220800,
                .height = 1,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::BLOB),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN)
        },
        {
                .width = 32768,
                .height = 1,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::BLOB),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN)
        },
        {
                .width = 3110400,
                .height = 1,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::BLOB),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::VIDEO_ENCODER)
        },
        {
                .width = 1440,
                .height = 1080,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::IMPLEMENTATION_DEFINED),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::CAMERA_OUTPUT)
        },
        {
                .width = 1920,
                .height = 1080,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::PRIVATE_YCRCB_420_SP_M),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::VIDEO_ENCODER)
        },
        {
                .width = 1440,
                .height = 3200,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET)
        },
        {
                .width = 1920,
                .height = 1080,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::YV12),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN)
        },
        {
                .width = 2048,
                .height = 1,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::BLOB),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN)
        },
        {
                .width = 2097152,
                .height = 1,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::BLOB),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN)
        },
        {
                .width = 6144,
                .height = 1,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::BLOB),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN)
        },
        {
                .width = 1920,
                .height = 1080,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::PRIVATE_YCRCB_420_SP_M),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::VIDEO_ENCODER) |
                         static_cast<uint64_t>(SgrBufferUsage::CAMERA_OUTPUT)
        },
        {
                .width = 1440,
                .height = 3200,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 1920,
                .height = 1080,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::IMPLEMENTATION_DEFINED),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::CAMERA_OUTPUT)
        },
        {
                .width = 6220800,
                .height = 1,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::BLOB),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::VIDEO_ENCODER)
        },
        {
                .width = 320,
                .height = 240,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::YV12),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN)
        },
        {
                .width = 3840,
                .height = 2160,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::PRIVATE_YCRCB_420_SP_M),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::VIDEO_ENCODER)
        },
        {
                .width = 1440,
                .height = 3200,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 3648,
                .height = 2736,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::YCBCR_420_888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CAMERA_OUTPUT)
        },
        {
                .width = 3840,
                .height = 2160,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::PRIVATE_YCRCB_420_SP_M),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::VIDEO_ENCODER) |
                         static_cast<uint64_t>(SgrBufferUsage::CAMERA_OUTPUT)
        },
};

IMapper::BufferDescriptorInfo DescriptorInfoListZeroCamera1 [] = {
        {
                .width = 640,
                .height = 360,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::IMPLEMENTATION_DEFINED),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY) |
                         static_cast<uint64_t>(SgrBufferUsage::CAMERA_OUTPUT)
        },
        {
                .width = 1920,
                .height = 1080,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::IMPLEMENTATION_DEFINED),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY) |
                         static_cast<uint64_t>(SgrBufferUsage::CAMERA_OUTPUT)
        },
        {
                .width = 720,
                .height = 1600,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE)
        },
        {
                .width = 1152,
                .height = 3174,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 3110400,
                .height = 1,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::BLOB),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::VIDEO_ENCODER)
        },
        {
                .width = 1920,
                .height = 1080,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::PRIVATE_YCRCB_420_SP_M),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::VIDEO_ENCODER)
        },
        {
                .width = 1440,
                .height = 3200,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET)
        },
        {
                .width = 2048,
                .height = 1,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::BLOB),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN)
        },
        {
                .width = 6144,
                .height = 1,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::BLOB),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN)
        },
        {
                .width = 492,
                .height = 1344,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 1920,
                .height = 1080,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::PRIVATE_YCRCB_420_SP_M),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::VIDEO_ENCODER) |
                         static_cast<uint64_t>(SgrBufferUsage::CAMERA_OUTPUT)
        },
        {
                .width = 1440,
                .height = 3200,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 960,
                .height = 245,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 6220800,
                .height = 1,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::BLOB),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::VIDEO_ENCODER)
        },
        {
                .width = 742,
                .height = 2064,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 742,
                .height = 1920,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 1560,
                .height = 3174,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 780,
                .height = 768,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 18289152,
                .height = 1,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::BLOB),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CAMERA_OUTPUT)
        },
        {
                .width = 1440,
                .height = 3200,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 3840,
                .height = 2160,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::PRIVATE_YCRCB_420_SP_M),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::VIDEO_ENCODER) |
                         static_cast<uint64_t>(SgrBufferUsage::CAMERA_OUTPUT)
        },
};

IMapper::BufferDescriptorInfo DescriptorInfoListZeroCamera2 [] = {
        {
                .width = 2012316,
                .height = 1,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::BLOB),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CAMERA_OUTPUT)
        },
        {
                .width = 1440,
                .height = 3200,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::PRIVATE_YCBCR_420_SPN_SBWC),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY) |
                         static_cast<uint64_t>(SgrBufferUsage::PRIVATE_PRIVATE_NONSECURE)
        },
        {
                .width = 626,
                .height = 196,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 773121,
                .height = 1,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::BLOB),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CAMERA_OUTPUT)
        },
        {
                .width = 1284090,
                .height = 1,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::BLOB),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CAMERA_OUTPUT)
        },
        {
                .width = 256,
                .height = 144,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::IMPLEMENTATION_DEFINED),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY) |
                         static_cast<uint64_t>(SgrBufferUsage::CAMERA_OUTPUT)
        },
        {
                .width = 640,
                .height = 360,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::IMPLEMENTATION_DEFINED),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY) |
                         static_cast<uint64_t>(SgrBufferUsage::CAMERA_OUTPUT)
        },
        {
                .width = 352,
                .height = 288,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::IMPLEMENTATION_DEFINED),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY) |
                         static_cast<uint64_t>(SgrBufferUsage::CAMERA_OUTPUT)
        },
        {
                .width = 1920,
                .height = 1080,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::IMPLEMENTATION_DEFINED),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY) |
                         static_cast<uint64_t>(SgrBufferUsage::CAMERA_OUTPUT)
        },
        {
                .width = 3327968,
                .height = 1,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::BLOB),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CAMERA_OUTPUT)
        },
        {
                .width = 716347,
                .height = 1,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::BLOB),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CAMERA_OUTPUT)
        },
        {
                .width = 12525417,
                .height = 1,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::BLOB),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CAMERA_OUTPUT)
        },
        {
                .width = 960,
                .height = 720,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::IMPLEMENTATION_DEFINED),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY) |
                         static_cast<uint64_t>(SgrBufferUsage::CAMERA_OUTPUT)
        },
        {
                .width = 1280,
                .height = 720,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::IMPLEMENTATION_DEFINED),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY) |
                         static_cast<uint64_t>(SgrBufferUsage::CAMERA_OUTPUT)
        },
        {
                .width = 1624737,
                .height = 1,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::BLOB),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CAMERA_OUTPUT)
        },
        {
                .width = 412036,
                .height = 1,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::BLOB),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CAMERA_OUTPUT)
        },
        {
                .width = 176,
                .height = 144,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::IMPLEMENTATION_DEFINED),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY) |
                         static_cast<uint64_t>(SgrBufferUsage::CAMERA_OUTPUT)
        },
        {
                .width = 13782402,
                .height = 1,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::BLOB),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CAMERA_OUTPUT)
        },
        {
                .width = 315,
                .height = 147,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 375700,
                .height = 1,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::BLOB),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CAMERA_OUTPUT)
        },
        {
                .width = 320,
                .height = 240,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::IMPLEMENTATION_DEFINED),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY) |
                         static_cast<uint64_t>(SgrBufferUsage::CAMERA_OUTPUT)
        },
        {
                .width = 640,
                .height = 196,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 720,
                .height = 480,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::IMPLEMENTATION_DEFINED),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY) |
                         static_cast<uint64_t>(SgrBufferUsage::CAMERA_OUTPUT)
        },
        {
                .width = 1440,
                .height = 3200,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 2561514,
                .height = 1,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::BLOB),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CAMERA_OUTPUT)
        },
        {
                .width = 1440,
                .height = 1080,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::IMPLEMENTATION_DEFINED),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY) |
                         static_cast<uint64_t>(SgrBufferUsage::CAMERA_OUTPUT)
        },
        {
                .width = 780,
                .height = 768,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 1088,
                .height = 1088,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::IMPLEMENTATION_DEFINED),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY) |
                         static_cast<uint64_t>(SgrBufferUsage::CAMERA_OUTPUT)
        },
        {
                .width = 664,
                .height = 196,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 18289152,
                .height = 1,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::BLOB),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CAMERA_OUTPUT)
        },
        {
                .width = 616,
                .height = 196,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 602798,
                .height = 1,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::BLOB),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CAMERA_OUTPUT)
        },
        {
                .width = 640,
                .height = 480,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::IMPLEMENTATION_DEFINED),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY) |
                         static_cast<uint64_t>(SgrBufferUsage::CAMERA_OUTPUT)
        },
};

IMapper::BufferDescriptorInfo DescriptorInfoListZeroCamera3 [] = {
        {
                .width = 1440,
                .height = 3200,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 1440,
                .height = 3200,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET)
        },
        {
                .width = 1560,
                .height = 772,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 1440,
                .height = 3200,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
};

IMapper::BufferDescriptorInfo DescriptorInfoListZeroCamera4 [] = {
        {
                .width = 199,
                .height = 147,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 767,
                .height = 147,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 1440,
                .height = 3200,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET)
        },
        {
                .width = 1920,
                .height = 1080,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::IMPLEMENTATION_DEFINED),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY) |
                         static_cast<uint64_t>(SgrBufferUsage::CAMERA_OUTPUT)
        },
        {
                .width = 1440,
                .height = 3200,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 1440,
                .height = 3200,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 1560,
                .height = 772,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
};

IMapper::BufferDescriptorInfo DescriptorInfoListGl2SecureRendering [] = {
        {
                .width = 1440,
                .height = 3200,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 1440,
                .height = 2816,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBX_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY) |
                         static_cast<uint64_t>(SgrBufferUsage::PROTECTED)
        },
        {
                .width = 1440,
                .height = 3200,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 688,
                .height = 147,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 1440,
                .height = 3200,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_CLIENT_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::PROTECTED)
        },
        {
                .width = 1440,
                .height = 144,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 780,
                .height = 336,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 1560,
                .height = 921,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 648,
                .height = 147,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY)
        },
        {
                .width = 1440,
                .height = 3200,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_OVERLAY) |
                         static_cast<uint64_t>(SgrBufferUsage::COMPOSER_CLIENT_TARGET)
        },
        {
                .width = 1440,
                .height = 3200,
                .layerCount = 1,
                .format = static_cast<PixelFormat>(SgrPixelFormat::RGBA_8888),
                .usage = static_cast<uint64_t>(SgrBufferUsage::CPU_READ_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_READ_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_RARELY) |
                         static_cast<uint64_t>(SgrBufferUsage::CPU_WRITE_OFTEN) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_TEXTURE) |
                         static_cast<uint64_t>(SgrBufferUsage::GPU_RENDER_TARGET)
        }
};

DEFINE_DESCRIPTOR_PARMETERIZED(HomeScreen, DescriptorInfoListHomeScreen);
DEFINE_DESCRIPTOR_PARMETERIZED(BasicApps, DescriptorInfoListBasicApps);
DEFINE_DESCRIPTOR_PARMETERIZED(GfxBench, DescriptorInfoListGfxBench);
DEFINE_DESCRIPTOR_PARMETERIZED(Mxplayer, DescriptorInfoListMxplayer);
DEFINE_DESCRIPTOR_PARMETERIZED(Camera, DescriptorInfoListCamera);
DEFINE_DESCRIPTOR_PARMETERIZED(ZeroCamera1, DescriptorInfoListZeroCamera1);
DEFINE_DESCRIPTOR_PARMETERIZED(ZeroCamera2, DescriptorInfoListZeroCamera2);
DEFINE_DESCRIPTOR_PARMETERIZED(ZeroCamera3, DescriptorInfoListZeroCamera3);
DEFINE_DESCRIPTOR_PARMETERIZED(ZeroCamera4, DescriptorInfoListZeroCamera4);
DEFINE_DESCRIPTOR_PARMETERIZED(Gl2SecureRendering, DescriptorInfoListGl2SecureRendering);

//-----------------------------------------------------------------------------
class SgrSBWCTest :
public SgrMapperBase,
public ::testing::TestWithParam<std::tuple<SgrPixelFormat, std::tuple<uint32_t, uint32_t>>> {
 protected:
        void SetUp() override {
                SgrMapperBase::SetUp();
        }

        void TearDown() override {
                SgrMapperBase::TearDown();
        }
};

TEST_P(SgrSBWCTest, SBWCAllocTests) {
        auto info = mDummyDescriptorInfo;
        info.format = static_cast<PixelFormat>(std::get<0>(GetParam()));
        std::tuple<uint32_t, uint32_t> dimensions = std::get<1>(GetParam());
        std::tie(info.width, info.height) = dimensions;
        info.usage = static_cast<uint64_t>(BufferUsage::VIDEO_DECODER | BufferUsage::VIDEO_ENCODER
                                           | BufferUsage::COMPOSER_OVERLAY | BufferUsage::COMPOSER_CLIENT_TARGET
                                           | BufferUsage::COMPOSER_CURSOR);

#ifdef SVTS_ENABLE_LOG
        std::cout<< ".width = "      << info.width                         << ", ";
        std::cout<< ".height = "     << info.height                        << ", ";
        std::cout<< ".layerCount = " << info.layerCount                    << ", ";
        std::cout<< ".format = "     << static_cast<uint64_t>(info.format) << ", ";
        std::cout<< ".usage = "      << info.usage                         << "\n";
#endif

        const native_handle_t *bufferHandle = nullptr;
        ASSERT_NO_FATAL_FAILURE(bufferHandle = mGralloc->allocate(info, true));
        ASSERT_NO_FATAL_FAILURE(mGralloc->freeBuffer(bufferHandle));
}

std::string get_SBWC_string(testing::TestParamInfo<SgrSBWCTest::ParamType> info)
{
        std::string result;
        result = get_pixelformat_string(std::get<0>(info.param));
        result += std::string("_") + std::to_string(std::get<0>(std::get<1>(info.param)));
        result += std::string("x") + std::to_string(std::get<1>(std::get<1>(info.param)));

        return result;
}

INSTANTIATE_TEST_CASE_P(
        , SgrSBWCTest,
        ::testing::Combine(
               ::testing::Values(
                        SgrPixelFormat::PRIVATE_YCBCR_420_SP_M_SBWC,
                        SgrPixelFormat::PRIVATE_YCBCR_420_SPN_SBWC,
                        SgrPixelFormat::PRIVATE_YCBCR_420_SP_M_10B_SBWC,
                        SgrPixelFormat::PRIVATE_YCBCR_420_SPN_10B_SBWC,
                        SgrPixelFormat::PRIVATE_YCrCb_420_SP_M_SBWC,
                        SgrPixelFormat::PRIVATE_YCrCb_420_SP_M_10B_SBWC,
                        SgrPixelFormat::PRIVATE_YCBCR_420_SP_M_SBWC_L50,
                        SgrPixelFormat::PRIVATE_YCBCR_420_SP_M_SBWC_L75,
                        SgrPixelFormat::PRIVATE_YCBCR_420_SPN_SBWC_L50,
                        SgrPixelFormat::PRIVATE_YCBCR_420_SPN_SBWC_L75,
                        SgrPixelFormat::PRIVATE_YCBCR_420_SP_M_10B_SBWC_L40,
                        SgrPixelFormat::PRIVATE_YCBCR_420_SP_M_10B_SBWC_L60,
                        SgrPixelFormat::PRIVATE_YCBCR_420_SP_M_10B_SBWC_L80,
                        SgrPixelFormat::PRIVATE_YCBCR_420_SPN_10B_SBWC_L40,
                        SgrPixelFormat::PRIVATE_YCBCR_420_SPN_10B_SBWC_L60,
                        SgrPixelFormat::PRIVATE_YCBCR_420_SPN_10B_SBWC_L80
                        ),
               ::testing::Values(
                        std::make_tuple(1920, 1080),
                        std::make_tuple(2560, 1440),
                        std::make_tuple(3840, 2160)
                )
        ),
        get_SBWC_string);
