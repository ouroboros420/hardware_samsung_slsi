#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>

#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <log/log.h>

#include <system/graphics.h>
#include <android/sync.h>
#include <linux/fb.h>
#include <hardware/exynos/ion.h>
#define USES_ARCH_ARM64
#include "DeconCommonHeader.h"
#include "DeconHeader.h"
#include "DeconFbHeader.h"

#include <hardware/exynos/sbwcdecoder_dpu.h>

#define DEVNAME_FB0	"/dev/graphics/fb0"
#define DEVNAME_FB1	"/dev/graphics/fb1"
#define DEVNAME_FB2	"/dev/graphics/fb2"

#define SBWCDECODER_ATTR_SECURE_BUFFER		(1 << 0)
#define ARRSIZE(arr)		(sizeof(arr) / sizeof(arr[0]))
#define ALIGN_SBWC(strd)	((strd + 31) & ~31)
#define DPU_WIDTH_MIN		(16)
#define DPU_HEIGHT_MIN		(16)
#define WIDTH_8K		(7680)
#define HEIGHT_8K		(4320)
#define WIDTH_4K		(3840)
#define HEIGHT_4K		(2160)
#define SPLIT_PADDING_SIZE	(4)

#define DPU_SCALE_DOWN_RATIO_LIMIT    (4)
#define LIBDPU_SCALE_DOWN_RATIO_LIMIT    (DPU_SCALE_DOWN_RATIO_LIMIT * DPU_SCALE_DOWN_RATIO_LIMIT)

enum {
    HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M      = 0x105,   /* HAL_PIXEL_FORMAT_YCbCr_420_SP */
    HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SPN       = 0x123,
    /* 10-bit format (2 fd, 10bit, 2x byte) custom formats */
    HAL_PIXEL_FORMAT_EXYNOS_YCbCr_P010_M        = 0x127,
    /* SBWC format */
    HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_SBWC = 0x130,
    HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SPN_SBWC  = 0x131,

    HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_10B_SBWC = 0x132,
    HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SPN_10B_SBWC  = 0x133,

    HAL_PIXEL_FORMAT_EXYNOS_YCrCb_420_SP_M_SBWC = 0x134,

    /* SBWC Lossy formats */
    HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_SBWC_L50 = 0x140,
    HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_SBWC_L75 = 0x141,
    HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SPN_SBWC_L50  = 0x150,
    HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SPN_SBWC_L75  = 0x151,

    HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_10B_SBWC_L40 = 0x160,
    HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_10B_SBWC_L60 = 0x161,
    HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_10B_SBWC_L80 = 0x162,
    HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SPN_10B_SBWC_L40  = 0x170,
    HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SPN_10B_SBWC_L60  = 0x171,
    HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SPN_10B_SBWC_L80  = 0x172,
};

const static unsigned int halfmtSBWC_to_dpufmtSBWC[][3] = {
    {HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_SBWC,     DECON_PIXEL_FORMAT_NV12M_SBWC_8B,   2},
    {HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_10B_SBWC, DECON_PIXEL_FORMAT_NV12M_SBWC_10B,  2},
    {HAL_PIXEL_FORMAT_EXYNOS_YCrCb_420_SP_M_SBWC,     DECON_PIXEL_FORMAT_NV21M_SBWC_8B,   2},
    // TODO : NV21M 10bit is not supported
    //{HAL_PIXEL_FORMAT_EXYNOS_YCrCb_420_SP_M_10B_SBWC, DECON_PIXEL_FORMAT_NV21M_SBWC_10B },
    {HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SPN_SBWC,      	  DECON_PIXEL_FORMAT_NV12N_SBWC_8B,   	 1},
    {HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SPN_10B_SBWC,  	  DECON_PIXEL_FORMAT_NV12N_SBWC_10B,  	 1},
    {HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_SBWC_L50, 	  DECON_PIXEL_FORMAT_NV12M_SBWC_8B_L50,  2},
    {HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_SBWC_L75, 	  DECON_PIXEL_FORMAT_NV12M_SBWC_8B_L75,  2},
    {HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_10B_SBWC_L40, DECON_PIXEL_FORMAT_NV12M_SBWC_10B_L40, 2},
    {HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_10B_SBWC_L60, DECON_PIXEL_FORMAT_NV12M_SBWC_10B_L60, 2},
    {HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_10B_SBWC_L80, DECON_PIXEL_FORMAT_NV12M_SBWC_10B_L80, 2},
    {HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M,          	  DECON_PIXEL_FORMAT_NV12M,  	         2},
    {HAL_PIXEL_FORMAT_EXYNOS_YCbCr_P010_M,            	  DECON_PIXEL_FORMAT_NV12M_P010,	 2},
};

const static unsigned int halfmtnonSBWC_to_dpufmtSBWC[][3] = {
    {HAL_PIXEL_FORMAT_YCBCR_420_888,     	DECON_PIXEL_FORMAT_NV12N,	1},
    {HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SPN,     DECON_PIXEL_FORMAT_NV12N,	1},
    {HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M,    DECON_PIXEL_FORMAT_NV12M,	2},
    {HAL_PIXEL_FORMAT_EXYNOS_YCbCr_P010_M,	DECON_PIXEL_FORMAT_NV12M_P010,	2},
    {HAL_PIXEL_FORMAT_RGBA_8888,		DECON_PIXEL_FORMAT_RGBA_8888,	1},
    {HAL_PIXEL_FORMAT_BGRA_8888,		DECON_PIXEL_FORMAT_BGRA_8888,	1},
};

#define CSC_BT_709_C	12
#define CSC_BT_2020_C	13
#define CSC_DCI_P3_C	14
#define CUSTOM_START	3
#define GEN_DPU_CSC_EQ(ST, RA)	((ST << CSC_STANDARD_SHIFT) |(RA << CSC_RANGE_SHIFT))
const static unsigned int androidDataspaceStandard_to_dpucscStandard[][2] = {
    {HAL_DATASPACE_STANDARD_BT709,			CSC_BT_709},
    {HAL_DATASPACE_STANDARD_BT2020,			CSC_BT_2020},
    {HAL_DATASPACE_STANDARD_DCI_P3,			CSC_DCI_P3},
    {HAL_DATASPACE_STANDARD_BT601_625,			CSC_BT_601},
    {HAL_DATASPACE_STANDARD_BT601_625_UNADJUSTED,	CSC_BT_601},
    {HAL_DATASPACE_STANDARD_BT601_525, 			CSC_BT_601},
    {HAL_DATASPACE_STANDARD_BT601_525_UNADJUSTED, 	CSC_BT_601},
    {HAL_DATASPACE_STANDARD_BT2020_CONSTANT_LUMINANCE, 	CSC_BT_2020_CONSTANT_LUMINANCE},
    {HAL_DATASPACE_STANDARD_BT470M,			CSC_BT_470M},
    {HAL_DATASPACE_STANDARD_FILM,			CSC_FILM},
    {HAL_DATASPACE_STANDARD_ADOBE_RGB,			CSC_ADOBE_RGB},
    {HAL_DATASPACE_STANDARD_BT709,			CSC_BT_709_C},
    {HAL_DATASPACE_STANDARD_BT2020,			CSC_BT_2020_C},
    {HAL_DATASPACE_STANDARD_DCI_P3,			CSC_DCI_P3_C},
};

const static unsigned int androidExtraDataspace_to_androidDataspace[][2] = {
    {HAL_DATASPACE_SRGB_LINEAR,			HAL_DATASPACE_V0_SRGB_LINEAR},
    {HAL_DATASPACE_SRGB,			HAL_DATASPACE_V0_SRGB},
    {HAL_DATASPACE_JFIF,			HAL_DATASPACE_V0_JFIF},
    {HAL_DATASPACE_BT601_625,			HAL_DATASPACE_V0_BT601_625},
    {HAL_DATASPACE_BT601_525,			HAL_DATASPACE_V0_BT601_525},
    {HAL_DATASPACE_BT709, 			HAL_DATASPACE_V0_BT709},
};

const static unsigned int androidDataspaceRange_to_dpucscRange[][2] = {
    {HAL_DATASPACE_RANGE_LIMITED,	CSC_RANGE_LIMITED},
    {HAL_DATASPACE_RANGE_FULL,		CSC_RANGE_FULL},
    {HAL_DATASPACE_RANGE_EXTENDED,	CSC_RANGE_EXTENDED},
    {HAL_DATASPACE_RANGE_UNSPECIFIED,	CSC_RANGE_UNSPECIFIED},
};

/* win - ch could never be different */
const static int reservedDpuChannelWin[][2] {
	{17 , 16},  /* ODMA WB case */
	{15 , 11},
};

DpuSbwcDecoder::DpuSbwcDecoder()
{
    mfbFd = open(DEVNAME_FB1, O_RDWR);
    if (mfbFd < 0) {
        mInitErr = -1;
        ALOGE("device(%s) failed", DEVNAME_FB1);
        return;
    }

    mIonClient = exynos_ion_open();
    if (mIonClient < 0) {
        mInitErr = -1;
        ALOGE("ion client(%d) open failed", mIonClient);
	return;
    }

    mTempBufAllocCnt = 2;
    mTempBufAlloc = false;
    for (int i = 0; i < mTempBufAllocCnt; i++) {
        mTempBuf[i][0] = -1;
        mTempBuf[i][1] = -1;
    }
    for (int i = 0; i < mTempBufAllocCnt; i++) {
        if (dmabufAlloc(mTempBuf[i], 2, ((WIDTH_8K/DPU_SCALE_DOWN_RATIO_LIMIT) * (HEIGHT_8K/DPU_SCALE_DOWN_RATIO_LIMIT) * 4)) == false) {
            mTempBufAllocCnt = i;
            mTempBufAlloc = true;
            return;
        }
    }
    mTempBufAlloc = true;
}

DpuSbwcDecoder::~DpuSbwcDecoder()
{
    if (mInitErr) {
        return;
    }

    if (mTempBufAlloc == true) {
        for (int i = 0; i < mTempBufAllocCnt; i++) {
            if (mTempBuf[i][0] != -1) {
                close(mTempBuf[i][0]);
	        mTempBuf[i][0] = -1;
	    }

            if (mTempBuf[i][1] != -1) {
                close(mTempBuf[i][1]);
	        mTempBuf[i][1] = -1;
	    }
	}
	mTempBufAlloc = false;
	mTempBufAllocCnt = 0;
    }

    close(mfbFd);
    exynos_ion_close(mIonClient);
}

bool DpuSbwcDecoder::setPowerOn(void)
{
    int ret = 0;
    if (mInitErr)
        return false;

    ret = ioctl(mfbFd, FBIOBLANK, FB_BLANK_UNBLANK);
    if (ret < 0) {
        ALOGE("power on failed");
        return false;
    }
    return true;
}

bool DpuSbwcDecoder::setPowerOff(void)
{
    int ret = 0;
    if (mInitErr)
        return false;

    ret = ioctl(mfbFd, FBIOBLANK, FB_BLANK_POWERDOWN);
    if (ret < 0) {
        ALOGE("power off failed");
        return false;
    }
    return true;
}

bool DpuSbwcDecoder::checkVarRestriction(unsigned int src_format, unsigned int dst_format, unsigned int colorspace,
                                           unsigned int src_width, unsigned int src_height,
                                           unsigned int dst_width, unsigned int dst_height,
                                           unsigned int stride, unsigned int dst_stride, unsigned int attr)
{
    int i;
    /* reserved layer */
    if (ARRSIZE(reservedDpuChannelWin) == 1) {
        ALOGE("there is no reserved Channel and Window for libsbwc-dpu\n");
        return false;
    }
    if (ARRSIZE(reservedDpuChannelWin) < 3) {
        if ((src_width > WIDTH_4K) || (src_height > HEIGHT_4K)) {
            ALOGE("size over 4K is not allowed if number of the reserved channels or win is less than 2 (src_width(%d), src_height(%d)",
                            src_width, src_height);
	    return false;
	}
    }

    /* src_format */
    for (i = 0; i < ARRSIZE(halfmtSBWC_to_dpufmtSBWC); i++)
        if (src_format == halfmtSBWC_to_dpufmtSBWC[i][0])
            break;
    if (i == ARRSIZE(halfmtSBWC_to_dpufmtSBWC)) {
        ALOGE("unsupported HAL src format (%d) for sbwc decoder", src_format);
        return false;
    }

    /* dst_format */
    for (i = 0; i < ARRSIZE(halfmtnonSBWC_to_dpufmtSBWC); i++)
        if (dst_format == halfmtnonSBWC_to_dpufmtSBWC[i][0])
            break;
    if (i == ARRSIZE(halfmtnonSBWC_to_dpufmtSBWC)) {
        ALOGE("unsupported HAL dst format (%d) for sbwc decoder", dst_format);
        return false;
    }

    /* colorspace */
    int android_standard = (colorspace & HAL_DATASPACE_STANDARD_MASK);
    for (i = 0; i < ARRSIZE(androidDataspaceStandard_to_dpucscStandard); i++)
        if (android_standard == androidDataspaceStandard_to_dpucscStandard[i][0])
            break;
    if (i == ARRSIZE(androidDataspaceStandard_to_dpucscStandard)) {
        ALOGE("unsupported dataspace(%d) standard(%d)", colorspace, android_standard);
        return false;
    }

    int android_range = (colorspace & HAL_DATASPACE_RANGE_MASK);
    for (i = 0; i < ARRSIZE(androidDataspaceRange_to_dpucscRange); i++)
        if (android_range == androidDataspaceRange_to_dpucscRange[i][0])
            break;
    if (i == ARRSIZE(androidDataspaceRange_to_dpucscRange)) {
        ALOGE("unsupported dataspace(%d) range(%d)", colorspace, android_range);
        return false;
    }

    /* src / dst */
    if ((src_width < dst_width) || (src_height < dst_height)) {
        ALOGE("scale up not supported src_width(%d), src_height(%d), dst_width(%d), dst_height(%d)",
                src_width, src_height, dst_width, dst_height);
        return false;
    }
    if ((src_width > (dst_width * LIBDPU_SCALE_DOWN_RATIO_LIMIT))
        || (src_height > (dst_height * LIBDPU_SCALE_DOWN_RATIO_LIMIT))) {
        ALOGE("less than 1/%d scale down ratio not supported (src_width(%d), src_height(%d), dst_width(%d), dst_height(%d))",
                LIBDPU_SCALE_DOWN_RATIO_LIMIT, src_width, src_height, dst_width, dst_height);
        return false;
    }
    if (src_width % 2) {
        ALOGE("src width should be 2 byte aligned since SBWC only supports YUV format(src_width (%d))", src_width);
        return false;
    }
    if ((src_width > dst_width) || (src_width > WIDTH_4K)) {
        if (src_width % (2 * DPU_SCALE_DOWN_RATIO_LIMIT)) {
            ALOGE("if src width(%d) is bigger than 4K width(%d), then it should be %d byte aligned (dst_width(%d)",
                    src_width, WIDTH_4K, (2 * DPU_SCALE_DOWN_RATIO_LIMIT), dst_width);
            return false;
        }
    }

    if (src_width > WIDTH_8K || src_height > HEIGHT_8K
	|| dst_width > WIDTH_8K || dst_height > HEIGHT_8K) {
        ALOGE("size off limit max(8K), srcWidth(%d), srcHeight(%d), dstWidth(%d), dstHeight(%d)",
			src_width, src_height, dst_height, dst_height);
        return false;
    }
    if ((src_width < DPU_WIDTH_MIN) || (src_height < DPU_HEIGHT_MIN)
	|| (dst_width < DPU_WIDTH_MIN) || (dst_height < DPU_HEIGHT_MIN)) {
        ALOGE("size off limit width_min(%d), height_min(%d), srcWidth(%d), srcHeight(%d), dstWidth(%d), dstHeight(%d)",
			DPU_WIDTH_MIN, DPU_HEIGHT_MIN, src_width, src_height, dst_height, dst_height);
        return false;
    }

    /* stride */
    if (stride & 31) {
        ALOGE("stride align wrong (now=%d), (if aligned=%d)", stride, ALIGN_SBWC(stride));
        return false;
    }
    if (stride < src_width) {
        ALOGE("stride less than src_width (stride=%d, src_width=%d)", stride, src_width);
        return false;
    }

    if (dst_stride < dst_width) {
        ALOGE("dst_stride less than src_width (dst_stride=%d, dst_width=%d)", dst_stride, dst_width);
        return false;
    }
    /* attr */
    if (attr & ~SBWCDECODER_ATTR_SECURE_BUFFER)
        ALOGD("warning : unsupported attr(%d)\n", attr);

    return true;
}

bool DpuSbwcDecoder::setInputVariables(unsigned int src_format, unsigned int dst_format, unsigned int colorspace,
            unsigned int src_width, unsigned int src_height,
            unsigned int dst_width, unsigned int dst_height,
            unsigned int stride, unsigned int dst_stride, unsigned int attr)
{
    int i;
    int android_standard = 0, android_range = 0;
    if (mInitErr)
        return false;

    if (log_level > 1) {
        ALOGD("%s:src_format(%d), dst_format(%d), colorspace(%d)",
                __func__, src_format, dst_format, colorspace);
        ALOGD("%s:src_width(%d), src_height(%d), dst_width(%d), dst_height(%d)",
                __func__, src_width, src_height, dst_width, dst_height);
        ALOGD("%s:stride(%d), dst_stride(%d), attr(%d)",
                __func__, stride, dst_stride, attr);
    }

    for (i = 0; i < ARRSIZE(androidExtraDataspace_to_androidDataspace); i++) {
        if (colorspace == androidExtraDataspace_to_androidDataspace[i][0])
	    colorspace = androidExtraDataspace_to_androidDataspace[i][1];
    }

    if (checkVarRestriction(src_format, dst_format, colorspace, src_width, src_height,
                dst_width, dst_height, stride, dst_stride, attr) == false)
        return false;

    for (i = 0; i < ARRSIZE(halfmtSBWC_to_dpufmtSBWC); i++) {
        if (src_format == halfmtSBWC_to_dpufmtSBWC[i][0]) {
            mFmtSrcSBWC = halfmtSBWC_to_dpufmtSBWC[i][1];
            mNumSrcFd = halfmtSBWC_to_dpufmtSBWC[i][2];
            break;
        }
    }
    for (i = 0; i < ARRSIZE(halfmtnonSBWC_to_dpufmtSBWC); i++) {
        if (dst_format == halfmtnonSBWC_to_dpufmtSBWC[i][0]) {
            mFmtDstSBWC = halfmtnonSBWC_to_dpufmtSBWC[i][1];
            mNumDstFd = halfmtnonSBWC_to_dpufmtSBWC[i][2];
            break;
        }
    }
    mStride = stride;
    mDstStride = dst_stride;
    mSrcWidth = src_width;
    mSrcHeight = src_height;
    mDstWidth = dst_width;
    mDstHeight = dst_height;

    unsigned int dpu_standard = (unsigned int)CSC_BT_601;
    android_standard = (colorspace & HAL_DATASPACE_STANDARD_MASK);
    for (i = 0; i < (ARRSIZE(androidDataspaceStandard_to_dpucscStandard) - CUSTOM_START); i++) {
        if (android_standard == androidDataspaceStandard_to_dpucscStandard[i][0]) {
            dpu_standard = androidDataspaceStandard_to_dpucscStandard[i][1];
	    break;
	}
    }
    unsigned int dpu_standard_out = (unsigned int)CSC_BT_601;
    for (i = CUSTOM_START; i < ARRSIZE(androidDataspaceStandard_to_dpucscStandard); i++) {
        if (android_standard == androidDataspaceStandard_to_dpucscStandard[i][0]) {
            dpu_standard_out = androidDataspaceStandard_to_dpucscStandard[i][1];
	    break;
	}
    }

    unsigned int dpu_range = (unsigned int)CSC_RANGE_LIMITED;
    android_range = (colorspace & HAL_DATASPACE_RANGE_MASK);
    for (i = 0; i < ARRSIZE(androidDataspaceRange_to_dpucscRange); i++) {
        if (android_range == androidDataspaceRange_to_dpucscRange[i][0]) {
            dpu_range = androidDataspaceRange_to_dpucscRange[i][1];
	    break;
	}
    }

    mColorSpace = GEN_DPU_CSC_EQ(dpu_standard, dpu_range);
    mColorSpaceOut = GEN_DPU_CSC_EQ(dpu_standard_out, dpu_range);

    mIsProtected = !!(attr & SBWCDECODER_ATTR_SECURE_BUFFER);

    return true;
}

bool DpuSbwcDecoder::initData(struct decon_win_config_data *win_data)
{
    struct decon_win_config *config = win_data->config;
    int i;
    int ratio_w, ratio_h, src, target, remainder;

    memset(win_data, 0, sizeof(decon_win_config_data));

    config[reservedDpuChannelWin[0][0]].fd_lut = -1;
    config[reservedDpuChannelWin[0][0]].acq_fence = -1;
    config[reservedDpuChannelWin[0][0]].rel_fence = -1;
    config[reservedDpuChannelWin[0][0]].dpp_parm.eq_mode = (enum dpp_csc_eq)mColorSpaceOut;
    config[reservedDpuChannelWin[0][0]].idma_type = (enum decon_idma_type)reservedDpuChannelWin[0][1];
    for (i = 1; i < ARRSIZE(reservedDpuChannelWin); i++) {
        config[reservedDpuChannelWin[i][0]].fd_lut = -1;
        config[reservedDpuChannelWin[i][0]].acq_fence = -1;
        config[reservedDpuChannelWin[i][0]].rel_fence = -1;
        config[reservedDpuChannelWin[i][0]].dpp_parm.eq_mode = (enum dpp_csc_eq)mColorSpace;
        config[reservedDpuChannelWin[i][0]].idma_type = (enum decon_idma_type)reservedDpuChannelWin[i][1];
    }

    ratio_w = mSrcWidth / mDstWidth;
    ratio_h = mSrcHeight / mDstHeight;
    if ((ratio_w < DPU_SCALE_DOWN_RATIO_LIMIT)
	&& (ratio_h < DPU_SCALE_DOWN_RATIO_LIMIT)) {
        mFrameUpdateAgainCountMax = 0;
	goto end;
    } else if (ratio_w > ratio_h) {
        src = mSrcWidth;
        target = mDstWidth;
    } else {
        src = mSrcHeight;
        target = mDstHeight;
    }

    mFrameUpdateAgainCountMax = 0;
    while (src){
	remainder = src % DPU_SCALE_DOWN_RATIO_LIMIT;
        src = src / DPU_SCALE_DOWN_RATIO_LIMIT;
        mFrameUpdateAgainCountMax++;
	if (src < target)
		break;
	if ((src == target) && (remainder == 0))
		break;
    }

end:
    if (log_level > 2) {
        for (i = 0; i < ARRSIZE(reservedDpuChannelWin); i++) {
            ALOGD("%s:win(%d), channel(%d)",
                    __func__,
                    reservedDpuChannelWin[i][0],
                    (int)config[reservedDpuChannelWin[i][0]].idma_type);
	}
        ALOGD("%s:ratio_w(%d), mSrcWidth(%d), mDstWidth(%d)", __func__, ratio_w, mSrcWidth, mDstWidth);
        ALOGD("%s:ratio_h(%d), mSrcHeight(%d), mDstHeight(%d)", __func__, ratio_w, mSrcHeight, mDstHeight);
        ALOGD("%s:mFrameUpdateAgainCountMax(%d)", __func__, mFrameUpdateAgainCountMax);
    }
    return true;
}

bool DpuSbwcDecoder::initWinState(struct decon_win_config_data *win_data)
{
    int i;
    struct decon_win_config *config = win_data->config;
    for (i = 0; i < ARRSIZE(reservedDpuChannelWin); i++)
        config[reservedDpuChannelWin[i][0]].state = decon_win_config::DECON_WIN_STATE_DISABLED;
    return true;
}

void DpuSbwcDecoder::printDeconFrame(struct decon_frame *frame)
{
    ALOGD("setSize_WinData: %s: x(%d), y(%d), w(%d), h(%d), f_w(%d), f_h(%d)", __func__,
        frame->x, frame->y, frame->w, frame->h, frame->f_w, frame->f_h);
}

void DpuSbwcDecoder::setSize_WinData(struct decon_win_config_data *win_data)
{
    struct decon_win_config *config = win_data->config;
    /* only modifies size related variable src/dst/aux */

    /* output */
    config[reservedDpuChannelWin[0][0]].src.x = 0;
    config[reservedDpuChannelWin[0][0]].src.y = 0;
    config[reservedDpuChannelWin[0][0]].dst.x = 0;
    config[reservedDpuChannelWin[0][0]].dst.y = 0;
    config[reservedDpuChannelWin[0][0]].src.w = mTempDstWidth;
    config[reservedDpuChannelWin[0][0]].src.h = mTempDstHeight;
    config[reservedDpuChannelWin[0][0]].dst.w = mTempDstWidth;
    config[reservedDpuChannelWin[0][0]].dst.h = mTempDstHeight;
    config[reservedDpuChannelWin[0][0]].src.f_w = mTempDstStride;
    config[reservedDpuChannelWin[0][0]].src.f_h = mTempDstHeight;
    config[reservedDpuChannelWin[0][0]].dst.f_w = mTempDstStride;
    config[reservedDpuChannelWin[0][0]].dst.f_h = mTempDstHeight;
    if (log_level > 3) {
        ALOGD("%s: output->src", __func__);
        printDeconFrame(&config[reservedDpuChannelWin[0][0]].src);
        ALOGD("%s: output->dst", __func__);
        printDeconFrame(&config[reservedDpuChannelWin[0][0]].dst);
    }

    /* input */
    if (mNeedTwoChannel == false) {
        config[reservedDpuChannelWin[1][0]].src.x = 0;
        config[reservedDpuChannelWin[1][0]].src.y = 0;
        config[reservedDpuChannelWin[1][0]].dst.x = 0;
        config[reservedDpuChannelWin[1][0]].dst.y = 0;
        config[reservedDpuChannelWin[1][0]].src.w = mTempSrcWidth;
        config[reservedDpuChannelWin[1][0]].src.h = mTempSrcHeight;
        config[reservedDpuChannelWin[1][0]].dst.w = mTempDstWidth;
        config[reservedDpuChannelWin[1][0]].dst.h = mTempDstHeight;
        config[reservedDpuChannelWin[1][0]].src.f_w = mTempSrcStride;
        config[reservedDpuChannelWin[1][0]].src.f_h = mTempSrcHeight;
        config[reservedDpuChannelWin[1][0]].dst.f_w = mTempDstStride;
        config[reservedDpuChannelWin[1][0]].dst.f_h = mTempDstHeight;
        if (log_level > 3) {
            ALOGD("%s: input1->src", __func__);
            printDeconFrame(&config[reservedDpuChannelWin[1][0]].src);
            ALOGD("%s: input1->dst", __func__);
            printDeconFrame(&config[reservedDpuChannelWin[1][0]].dst);
        }
    } else {
        config[reservedDpuChannelWin[1][0]].src.x = 0;
        config[reservedDpuChannelWin[1][0]].src.y = 0;
        config[reservedDpuChannelWin[1][0]].dst.x = 0;
        config[reservedDpuChannelWin[1][0]].dst.y = 0;
        config[reservedDpuChannelWin[1][0]].src.w = mTempSrcWidth / 2 + SPLIT_PADDING_SIZE;
        config[reservedDpuChannelWin[1][0]].src.h = mTempSrcHeight;
        config[reservedDpuChannelWin[1][0]].dst.w = mTempDstWidth / 2 + SPLIT_PADDING_SIZE;
        config[reservedDpuChannelWin[1][0]].dst.h = mTempDstHeight;
        config[reservedDpuChannelWin[1][0]].src.f_w = mTempSrcStride;
        config[reservedDpuChannelWin[1][0]].src.f_h = mTempSrcHeight;
        config[reservedDpuChannelWin[1][0]].dst.f_w = mTempDstStride;
        config[reservedDpuChannelWin[1][0]].dst.f_h = mTempDstHeight;
        config[reservedDpuChannelWin[1][0]].aux_src.spl_en = DPP_SPLIT_LEFT;
        config[reservedDpuChannelWin[1][0]].aux_src.padd_w = SPLIT_PADDING_SIZE;
        config[reservedDpuChannelWin[1][0]].aux_src.padd_h = 0;
        config[reservedDpuChannelWin[1][0]].aux_src.spl_drtn = DPP_SPLIT_VERTICAL;
        if (log_level > 3) {
            ALOGD("%s: input1->src", __func__);
            printDeconFrame(&config[reservedDpuChannelWin[1][0]].src);
            ALOGD("%s: input1->dst", __func__);
            printDeconFrame(&config[reservedDpuChannelWin[1][0]].dst);
        }

        if (ARRSIZE(reservedDpuChannelWin) == 3) {
            config[reservedDpuChannelWin[2][0]].src.x = (mTempSrcWidth / 2) - SPLIT_PADDING_SIZE;
            config[reservedDpuChannelWin[2][0]].src.y = 0;
            config[reservedDpuChannelWin[2][0]].dst.x = (mTempDstWidth / 2) - SPLIT_PADDING_SIZE;
            config[reservedDpuChannelWin[2][0]].dst.y = 0;
            config[reservedDpuChannelWin[2][0]].src.w = mTempSrcWidth / 2 + SPLIT_PADDING_SIZE;
            config[reservedDpuChannelWin[2][0]].src.h = mTempSrcHeight;
            config[reservedDpuChannelWin[2][0]].dst.w = mTempDstWidth / 2 + SPLIT_PADDING_SIZE;
            config[reservedDpuChannelWin[2][0]].dst.h = mTempDstHeight;
            config[reservedDpuChannelWin[2][0]].src.f_w = mTempSrcStride;
            config[reservedDpuChannelWin[2][0]].src.f_h = mTempSrcHeight;
            config[reservedDpuChannelWin[2][0]].dst.f_w = mTempDstStride;
            config[reservedDpuChannelWin[2][0]].dst.f_h = mTempDstHeight;
            config[reservedDpuChannelWin[2][0]].aux_src.spl_en = DPP_SPLIT_RIGHT;
            config[reservedDpuChannelWin[2][0]].aux_src.padd_w = SPLIT_PADDING_SIZE;
            config[reservedDpuChannelWin[2][0]].aux_src.padd_h = 0;
            config[reservedDpuChannelWin[2][0]].aux_src.spl_drtn = DPP_SPLIT_VERTICAL;

            if (log_level > 3) {
                ALOGD("%s: input2->src", __func__);
                printDeconFrame(&config[reservedDpuChannelWin[2][0]].src);
                ALOGD("%s: input2->dst", __func__);
                printDeconFrame(&config[reservedDpuChannelWin[2][0]].dst);
            }
        }
    }
}

bool DpuSbwcDecoder::setSize(struct decon_win_config_data *win_data)
{
    /* here only modifies size related variable (src,dst,aux.. etc.) */
    if (mFrameUpdateAgainCount == 0) {
        mTempSrcWidth = mSrcWidth;
        mTempSrcHeight = mSrcHeight;
        mTempSrcStride = mStride;
    } else {
        mTempSrcWidth = mTempDstWidth;
        mTempSrcHeight = mTempDstHeight;
        mTempSrcStride = mTempDstStride;
    }
    mTempDstWidth = mDstWidth;
    mTempDstHeight = mDstHeight;
    mTempDstStride = mDstStride;

    mFrameUpdateAgain = false;
    mNeedTwoChannel = false;

    if ((mTempSrcWidth > (mDstWidth * DPU_SCALE_DOWN_RATIO_LIMIT))
		    || (mTempSrcHeight > (mDstHeight * DPU_SCALE_DOWN_RATIO_LIMIT))) {
        ALOGD("frame update again! = tmpSrcWidth(%d), tmpSrcHeight(%d), targetDstWidth(%d), targetDstHeight(%d)",
			mTempSrcWidth, mTempSrcHeight, mDstWidth, mDstHeight);
        if ((mTempSrcWidth > (mDstWidth * DPU_SCALE_DOWN_RATIO_LIMIT))) {
	    mTempDstWidth = mTempSrcWidth / DPU_SCALE_DOWN_RATIO_LIMIT;
	    if (mTempDstWidth > mTempDstStride)
                mTempDstStride = mTempDstWidth;
	}
        if ((mTempSrcHeight > (mDstHeight * DPU_SCALE_DOWN_RATIO_LIMIT))) {
	    mTempDstHeight = mTempSrcHeight / DPU_SCALE_DOWN_RATIO_LIMIT;
	}
        mFrameUpdateAgain = true;
	mFrameUpdateAgainCount++;
    }

    if (mTempSrcWidth > WIDTH_4K)
        mNeedTwoChannel = true;
    else
        mNeedTwoChannel = false;

    if (log_level > 2) {
        ALOGD("%s:mFrameUpdateAgain(%d), mNeedTwoChannel (%d), mFrameUpdateAgainCount(%d)",
                __func__, mFrameUpdateAgain ? 1 : 0, mNeedTwoChannel ? 1 : 0, mFrameUpdateAgainCount);
        ALOGD("%s:mSrcWidth(%d), mSrcHeight (%d), mDstWidth(%d), mDstHeight(%d)",
                __func__, mSrcWidth, mSrcHeight, mDstWidth, mDstHeight);
        ALOGD("%s:mTempSrcWidth(%d), mTempSrcHeight (%d), mTempDstWidth(%d), mTempDstHeight(%d)",
                __func__, mTempSrcWidth, mTempSrcHeight, mTempDstWidth, mTempDstHeight);
    }

    setSize_WinData(win_data);

    return true;
}

bool DpuSbwcDecoder::dmabufAlloc(int *buffer, int buf_cnt, int buf_size)
{
    int i, err_cnt;

    if (mIonClient < 0)
        return false;

    if (buffer == NULL) {
        ALOGE("%s:buffer null\n", __func__);
        return false;
    }

    for (i = 0; i < buf_cnt; i++) {
        buffer[i] = exynos_ion_alloc(mIonClient, buf_size, EXYNOS_ION_HEAP_SYSTEM_MASK, 0);
        if (buffer[i] < 0) {
            ALOGE("%s:ion buffer[%d] allocation failed\n", __func__, i);
            err_cnt = i;
            goto err;
        }
    }
    return true;

err:
    for (i = 0; i < err_cnt; i++)
        close(buffer[i]);
    return false;
}

void DpuSbwcDecoder::setBuf_WinData(struct decon_win_config_data *win_data,
		int inTmpBuf[], int outTmpBuf[])
{
    struct decon_win_config *config = win_data->config;
    int i, j, num_fd;

    /* output */
    config[reservedDpuChannelWin[0][0]].protection = mTempDstProtected;
    config[reservedDpuChannelWin[0][0]].state = decon_win_config::DECON_WIN_STATE_BUFFER;
    for (i = 0; i < mNumDstFd; i++) {
        config[reservedDpuChannelWin[0][0]].fd_idma[i] = outTmpBuf[i];
    }

    /* input */
    if (mNeedTwoChannel == false) {
        config[reservedDpuChannelWin[1][0]].protection = mTempSrcProtected;
        config[reservedDpuChannelWin[1][0]].state = decon_win_config::DECON_WIN_STATE_BUFFER_LIBREQ;
        for (i = 0; i < mTempNumSrcFd; i++) {
            config[reservedDpuChannelWin[1][0]].fd_idma[i] = inTmpBuf[i];
	}
    } else {
        config[reservedDpuChannelWin[1][0]].protection = mTempSrcProtected;
        if (ARRSIZE(reservedDpuChannelWin) == 3)
            config[reservedDpuChannelWin[2][0]].protection = mTempSrcProtected;

        config[reservedDpuChannelWin[1][0]].state = decon_win_config::DECON_WIN_STATE_BUFFER_LIBREQ;
        if (ARRSIZE(reservedDpuChannelWin) == 3)
            config[reservedDpuChannelWin[2][0]].state = decon_win_config::DECON_WIN_STATE_BUFFER_LIBREQ;
        for (i = 0; i < mTempNumSrcFd; i++) {
            config[reservedDpuChannelWin[1][0]].fd_idma[i] = inTmpBuf[i];
            if (ARRSIZE(reservedDpuChannelWin) == 3)
                config[reservedDpuChannelWin[2][0]].fd_idma[i] = inTmpBuf[i];
	}
    }
    if (log_level > 3) {
        for (i = 0; i < ARRSIZE(reservedDpuChannelWin); i++) {
	    if (i == 0)
                num_fd = mNumDstFd;
	    else
                num_fd = mTempNumSrcFd;
            ALOGD("%s:win(%d), channel(%d), state(%d), num_fd(%d)", __func__,
                reservedDpuChannelWin[i][0], (int)config[reservedDpuChannelWin[i][0]].idma_type,
		(int)config[reservedDpuChannelWin[i][0]].state, num_fd);
	    for (j = 0; j < num_fd; j++) {
                ALOGD("%s:fd_idma[%d] : %d", __func__,
			j, config[reservedDpuChannelWin[i][0]].fd_idma[j]);
	    }
	}
    }
}

bool DpuSbwcDecoder::setBuf(struct decon_win_config_data *win_data,
				int inBuf[], int outBuf[])
{
    /* here only modifies buffer related variable (protection, fd_idma, buf_state.. etc.) */
    if (mFrameUpdateAgain == true) {
	if (mFrameUpdateAgainCount == 1) {
            mTempSrcProtected = mIsProtected;
            mTempDstProtected = 0;

            mTempNumSrcFd = mNumSrcFd;
            setBuf_WinData(win_data, inBuf, mTempBuf[((mFrameUpdateAgainCount + 1) % 2)]);
	} else {
            mTempSrcProtected = 0;
            mTempDstProtected = 0;

	    mTempNumSrcFd = mNumDstFd;
            setBuf_WinData(win_data,
		mTempBuf[((mFrameUpdateAgainCount) % 2)],
		mTempBuf[((mFrameUpdateAgainCount + 1) % 2)]);
	}
    } else {
        if (mFrameUpdateAgainCount >= 1) {
            mTempSrcProtected = 0;
            mTempDstProtected = mIsProtected;

	    mTempNumSrcFd = mNumDstFd;
            setBuf_WinData(win_data, mTempBuf[((mFrameUpdateAgainCount + 1) % 2)], outBuf);
	} else {
            mTempSrcProtected = mIsProtected;
            mTempDstProtected = mIsProtected;

            mTempNumSrcFd = mNumSrcFd;
            setBuf_WinData(win_data, inBuf, outBuf);
	}
    }

    return true;
}

bool DpuSbwcDecoder::setFmt_Windata(struct decon_win_config_data *win_data)
{
    struct decon_win_config *config = win_data->config;
    int i;
    config[reservedDpuChannelWin[0][0]].format = (enum decon_pixel_format)mFmtDstSBWC;
    if (mFrameUpdateAgain == true) {
	if (mFrameUpdateAgainCount == 1) {
            config[reservedDpuChannelWin[1][0]].format = (enum decon_pixel_format)mFmtSrcSBWC;
            if (ARRSIZE(reservedDpuChannelWin) == 3)
                config[reservedDpuChannelWin[2][0]].format = (enum decon_pixel_format)mFmtSrcSBWC;
	} else {
            config[reservedDpuChannelWin[1][0]].format = (enum decon_pixel_format)mFmtDstSBWC;
            if (ARRSIZE(reservedDpuChannelWin) == 3)
                config[reservedDpuChannelWin[2][0]].format = (enum decon_pixel_format)mFmtDstSBWC;
	}
    } else {
        if (mFrameUpdateAgainCount >= 1) {
            config[reservedDpuChannelWin[1][0]].format = (enum decon_pixel_format)mFmtDstSBWC;
            if (ARRSIZE(reservedDpuChannelWin) == 3)
                config[reservedDpuChannelWin[2][0]].format = (enum decon_pixel_format)mFmtDstSBWC;
	} else {
            config[reservedDpuChannelWin[1][0]].format = (enum decon_pixel_format)mFmtSrcSBWC;
            if (ARRSIZE(reservedDpuChannelWin) == 3)
                config[reservedDpuChannelWin[2][0]].format = (enum decon_pixel_format)mFmtSrcSBWC;
	}
    }

    if (log_level > 3) {
        for (i = 0; i < ARRSIZE(reservedDpuChannelWin); i++) {
            ALOGD("%s:win(%d), channel(%d) -> format(%d)", __func__,
                reservedDpuChannelWin[i][0], (int)config[reservedDpuChannelWin[i][0]].idma_type,
		(int)config[reservedDpuChannelWin[i][0]].format);
	}
    }
    return true;
}

bool DpuSbwcDecoder::setFmt(struct decon_win_config_data *win_data)
{
    if (setFmt_Windata(win_data) == false)
        return false;
    return true;
}

bool DpuSbwcDecoder::setDecode(struct decon_win_config_data *win_data)
{
    struct decon_win_config *config = win_data->config;

    if (ioctl(mfbFd, S3CFB_WIN_CONFIG, win_data) < 0) {
	if (log_level > 1)
            ALOGE("win config ioctl failed");
        return false;
    }

    for (int i = 0; i < ARRSIZE(reservedDpuChannelWin); i++) {
        mTempReleaseFence[i] = config[reservedDpuChannelWin[i][0]].rel_fence;
    }

    mTempRetireFence = win_data->present_fence;
    if (mTempRetireFence < 0) {
        ALOGE("retire fence error, retire_fence(%d)", mTempRetireFence);
        return false;
    }

    if (sync_wait(mTempRetireFence, 5000) < 0) {
        ALOGE("fence wait time out, retire_fence(%d)", mTempRetireFence);
        return false;
    }

    return true;
}

void DpuSbwcDecoder::releaseResources(void)
{
    mFrameUpdateAgainCount = 0;
    mFrameUpdateAgain = false;
    mFrameUpdateAgainCountMax = 0;
    mNeedTwoChannel = false;

    if (mTempRetireFence > 0) {
        close(mTempRetireFence);
        mTempRetireFence = -1;
    }

    for (int i = 0; i < ARRSIZE(reservedDpuChannelWin); i++) {
        if (mTempReleaseFence[i] > 0)
            close(mTempReleaseFence[i]);
        mTempReleaseFence[i] = -1;
    }

}

bool DpuSbwcDecoder::checkInputVar(int inBuf[], int outBuf[])
{
    if (mInitErr)
        return false;

    if (inBuf == NULL || outBuf == NULL) {
        ALOGE("%s: buf fd null (in(0x%lx) or out (0x%lx))", __func__,
			(unsigned long)inBuf, (unsigned long)outBuf);
        return false;
    }

    if (inBuf[0] <= 3 && inBuf[0] >= 0) {
        ALOGE("%s: buf fd out of limit (outBuf[0] = %d)", __func__, outBuf[0]);
        return false;
    }

    if (outBuf[0] <= 3 && outBuf[0] >= 0) {
        ALOGE("%s: buf fd out of limit (outBuf[0] = %d)", __func__, outBuf[0]);
        return false;
    }
    return true;
}

bool DpuSbwcDecoder::decode(int inBuf[], int outBuf[])
{
    struct decon_win_config_data win_data;

    if (checkInputVar(inBuf, outBuf) == false)
        goto err;

    if (initData(&win_data) == false)
        goto err;

again:
    if (initWinState(&win_data) == false)
        goto err;

    if (setSize(&win_data) == false)
        goto err;

    if (setBuf(&win_data, inBuf, outBuf) == false)
        goto err;

    if (setFmt(&win_data) == false)
        goto err;

    if (setDecode(&win_data) == false)
        goto err;

    if (mFrameUpdateAgain == true)
        goto again;
    else
        goto end;

end:
    releaseResources();
    return true;

err:
    releaseResources();
    return false;
}

bool DpuSbwcDecoder::decodeSBWC (unsigned int src_format, unsigned int dst_format,
			unsigned int src_width, unsigned int src_height,
			unsigned int dst_width, unsigned int dst_height,
			unsigned int stride, unsigned int dst_stride, unsigned int attr,
			int inBuf[], int outBuf[])
{
    unsigned int colorspace = (unsigned int)(HAL_DATASPACE_STANDARD_BT601_625 | HAL_DATASPACE_RANGE_LIMITED);
    struct timeval start;
    struct timeval end;

    gettimeofday(&start, NULL);
    if (decodeSBWC(src_format, dst_format, colorspace, src_width, src_height,
            dst_width, dst_height, stride, dst_stride, attr, inBuf, outBuf) == false)
        return false;
    gettimeofday(&end, NULL);

    if (log_level > 0) {
	ALOGD("difftime for %s : %ld usec\n", __func__,
	    ((end.tv_sec * 1000000 + end.tv_usec)
	         - (start.tv_sec * 1000000 + start.tv_usec)));
    }

    return true;
}

bool DpuSbwcDecoder::decodeSBWC (unsigned int src_format, unsigned int dst_format, unsigned int colorspace,
			unsigned int src_width, unsigned int src_height,
			unsigned int dst_width, unsigned int dst_height,
			unsigned int stride, unsigned int dst_stride, unsigned int attr,
			int inBuf[], int outBuf[])
{
    if (setPowerOn() == false)
        goto err_power;

    if (setInputVariables(src_format, dst_format, colorspace,
			    src_width, src_height,
			    dst_width, dst_height,
			    stride, dst_stride, attr) == false)
        goto err_decode;

    if (decode(inBuf, outBuf) == false)
        goto err_decode;

    if (setPowerOff() == false)
        goto err_decode;

    return true;

err_decode:
    setPowerOff();

err_power:
    return false;
}

void DpuSbwcDecoder::setLogLevel(unsigned int __log_level)
{
	/* log_level 0 : error check */
	/* log_level 1 : performance check */
	/* log_level 2 : init variable check */
	/* log_level 3 : input & intermediate variable check */
	/* log_level 4 : final variable check */
	log_level = __log_level;
}
