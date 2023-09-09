#include <hardware/exynos/sbwcdecoder_dpu.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <linux/fb.h>
#include <fcntl.h>

#include <sys/stat.h>
#include <sys/ioctl.h>
#include <log/log.h>

#include <android/sync.h>
#include <hardware/exynos/ion.h>
#include <system/graphics.h>
#define USES_ARCH_ARM64
#include "DeconCommonHeader.h"
#include "DeconFbHeader.h"
#include "DeconHeader.h"

#define NV12_8B_8K_SBWC_FILE_NAME	"/data/out_7680x4320.nv12_sbwc"
#define MAX_BUF_IMG_W 8192
#define MAX_BUF_IMG_H 4320
#define IMG_FULL_WIDTH 7680
#define IMG_FULL_HEIGHT 4320
#define IMG_LR_FUL_WIDTH 5760
#define IMG_LR_FULL_HEIGHT 4320
#define BUF_NUM 2

#define DEVNAME_FB0	"/dev/graphics/fb0"
#define DEVNAME_FB1	"/dev/graphics/fb1"
#define DEVNAME_FB2	"/dev/graphics/fb2"

#define ARRSIZE(arr)		(sizeof(arr) / sizeof(arr[0]))
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

const static unsigned int halfmtnonSBWC_to_dpufmtSBWC[][3] = {
    {HAL_PIXEL_FORMAT_YCBCR_420_888,     	DECON_PIXEL_FORMAT_NV12N,	1},
    {HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SPN,     DECON_PIXEL_FORMAT_NV12N,	1},
    {HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M,    DECON_PIXEL_FORMAT_NV12M,	2},
    {HAL_PIXEL_FORMAT_EXYNOS_YCbCr_P010_M,	DECON_PIXEL_FORMAT_NV12M_P010,	2},
    {HAL_PIXEL_FORMAT_RGBA_8888,		DECON_PIXEL_FORMAT_RGBA_8888,	1},
    {HAL_PIXEL_FORMAT_BGRA_8888,		DECON_PIXEL_FORMAT_BGRA_8888,	1},
};

static int file_to_buf(const char *file_name, char *buf_addr)
{
	FILE *fp;
	int file_size;

	/* file open */
	fp = fopen(file_name, "rb");
	if (fp == NULL) {
		printf("failed to open %s file\n", file_name);
		return -1;
	}

	/* file seeking for size check */
	fseek(fp, 0, SEEK_END);

	/* file size check */
	file_size = ftell(fp);
	printf("file_size = %d\n", file_size);

	/* point of file first address */
	fseek(fp, 0 , SEEK_SET);

	/* file read */
	fread(buf_addr, file_size, 1, fp);

	/* file close */
	fclose(fp);

	return file_size;
}

static int buf_to_file(char *buf_addr, const char *file_name, int size)
{
	FILE *fp;

	/* file open */
	fp = fopen(file_name, "w+b");
	if (fp == NULL) {
		printf("failed to open %s file\n", file_name);
		return -1;
	}

	/* file seeking for size check */
	fseek(fp, 0, SEEK_END);

	/* file read */
	fwrite(buf_addr, size, 1, fp);

	/* file close */
	fclose(fp);

	return 0;
}

static int buf_alloc_and_map(int client, int *buffer, char **buf_addr,
		unsigned int **addr, int buf_cnt, int buf_size)
{
	int i;

	for (i = 0; i < buf_cnt; i++) {
		/* allocate buffer(s) */
		buffer[i] = exynos_ion_alloc(client, buf_size, EXYNOS_ION_HEAP_SYSTEM_MASK, 0);
		if (buffer[i] < 0) {
			printf("ion buffer[%d] allocation is failed\n", i);
			return -1;
		}

		buf_addr[i] = (char *)mmap(NULL, buf_size, PROT_READ | PROT_WRITE, MAP_SHARED,
				buffer[i], 0);
		if (buf_addr[i] == NULL) {
			printf("buffer[%d] mapping is failed\n", i);
			return -1;
		}

		if (addr != NULL)
			addr[i] = (unsigned int *)buf_addr[i];
	}
	return 0;
}

int buf_unmap_and_free(int *buffer, char **buf_addr, int buf_cnt, int buf_size)
{
	int i;
	int ret;

	for (i = 0; i < buf_cnt; i++) {
		ret = munmap(buf_addr[i], buf_size);
		if (ret < 0) {
			printf("failed to ion_unmap buf[%d](%d)\n", i, ret);
			return -1;
		}
		close(buffer[i]);
	}

	return 0;
}

int get_plane_cnt(enum decon_pixel_format format, bool is_hdr)
{
	printf("%s+  format : %d\n", __func__, format);
	switch (format) {
	case DECON_PIXEL_FORMAT_ARGB_8888:
	case DECON_PIXEL_FORMAT_ABGR_8888:
	case DECON_PIXEL_FORMAT_RGBA_8888:
	case DECON_PIXEL_FORMAT_BGRA_8888:
	case DECON_PIXEL_FORMAT_XRGB_8888:
	case DECON_PIXEL_FORMAT_XBGR_8888:
	case DECON_PIXEL_FORMAT_RGBX_8888:
	case DECON_PIXEL_FORMAT_BGRX_8888:
	case DECON_PIXEL_FORMAT_RGBA_5551:
	case DECON_PIXEL_FORMAT_RGB_565:
	case DECON_PIXEL_FORMAT_BGR_565:
	case DECON_PIXEL_FORMAT_NV12N:
		return 1;

	case DECON_PIXEL_FORMAT_NV16:
	case DECON_PIXEL_FORMAT_NV61:
	case DECON_PIXEL_FORMAT_NV12:
	case DECON_PIXEL_FORMAT_NV21:
	case DECON_PIXEL_FORMAT_NV12M:
	case DECON_PIXEL_FORMAT_NV21M:
	case DECON_PIXEL_FORMAT_NV12M_S10B:
	case DECON_PIXEL_FORMAT_NV21M_S10B:
	case DECON_PIXEL_FORMAT_NV12M_P010:
	case DECON_PIXEL_FORMAT_NV21M_P010:
	case DECON_PIXEL_FORMAT_NV16M_P210:
	case DECON_PIXEL_FORMAT_NV61M_P210:
	case DECON_PIXEL_FORMAT_NV16M_S10B:
	case DECON_PIXEL_FORMAT_NV61M_S10B:
		if (is_hdr)
			return 3;
		else
			return 2;

	case DECON_PIXEL_FORMAT_NV12N_10B:
	case DECON_PIXEL_FORMAT_ARGB_2101010:
	case DECON_PIXEL_FORMAT_ABGR_2101010:
	case DECON_PIXEL_FORMAT_RGBA_1010102:
	case DECON_PIXEL_FORMAT_BGRA_1010102:
		if (is_hdr)
			return 2;
		else
			return 1;

	case DECON_PIXEL_FORMAT_YVU422_3P:
	case DECON_PIXEL_FORMAT_YUV420:
	case DECON_PIXEL_FORMAT_YVU420:
	case DECON_PIXEL_FORMAT_YUV420M:
	case DECON_PIXEL_FORMAT_YVU420M:
		return 3;

	case DECON_PIXEL_FORMAT_NV12M_SBWC_8B:
	case DECON_PIXEL_FORMAT_NV12M_SBWC_10B:
	case DECON_PIXEL_FORMAT_NV21M_SBWC_8B:
	case DECON_PIXEL_FORMAT_NV21M_SBWC_10B:
		return 2;
	case DECON_PIXEL_FORMAT_NV12N_SBWC_8B:
	case DECON_PIXEL_FORMAT_NV12N_SBWC_10B:
		return 1;
	case DECON_PIXEL_FORMAT_NV12M_SBWC_8B_L50:
	case DECON_PIXEL_FORMAT_NV12M_SBWC_8B_L75:
	case DECON_PIXEL_FORMAT_NV12M_SBWC_10B_L40:
	case DECON_PIXEL_FORMAT_NV12M_SBWC_10B_L60:
	case DECON_PIXEL_FORMAT_NV12M_SBWC_10B_L80:
	       return 2;
	case DECON_PIXEL_FORMAT_NV12N_SBWC_8B_L50: /* single fd : [0]-Y_PL */
	case DECON_PIXEL_FORMAT_NV12N_SBWC_8B_L75: /* single fd : [0]-Y_PL */
	case DECON_PIXEL_FORMAT_NV12N_SBWC_10B_L40: /* single fd : [0]-Y_PL */
	case DECON_PIXEL_FORMAT_NV12N_SBWC_10B_L60: /* single fd : [0]-Y_PL */
	case DECON_PIXEL_FORMAT_NV12N_SBWC_10B_L80: /* single fd : [0]-Y_PL */
	       return 1;


	default:
		printf("invalid format(%d)\n", format);
		return 1;
	}
}


void set_hdr_win_config(struct decon_win_config *config, int win,
		enum decon_idma_type type, enum decon_pixel_format fmt,
		int *buf_fd, struct decon_frame *src, struct decon_frame *dst,
		enum dpp_csc_eq eq, enum dpp_hdr_standard hdr)
{
	int plane_cnt, i;

	config[win].state = decon_win_config::DECON_WIN_STATE_BUFFER;
	config[win].acq_fence = -1;
	config[win].blending = DECON_BLENDING_NONE;
	config[win].idma_type = type;
	config[win].format = fmt;

	plane_cnt = get_plane_cnt(fmt, hdr);
	printf("plane count(%d) of window(%d)\n", plane_cnt, win);
	for (i = 0; i < plane_cnt; ++i) {
		if (buf_fd == NULL) {
			printf("buffer fd is not required for window%d\n", win);
			break;
		}

		config[win].fd_idma[i] = buf_fd[i];
	}

	config[win].src.x = src->x;
	config[win].src.y = src->y;
	config[win].src.w = src->w;
	config[win].src.h = src->h;
	config[win].src.f_w = src->f_w;
	config[win].src.f_h = src->f_h;

	config[win].dst.x = dst->x;
	config[win].dst.y = dst->y;
	config[win].dst.w = dst->w;
	config[win].dst.h = dst->h;
	config[win].dst.f_w = dst->f_w;
	config[win].dst.f_h = dst->f_h;

	config[win].dpp_parm.eq_mode = eq;
	config[win].dpp_parm.rot = DPP_ROT_NORMAL;
	config[win].dpp_parm.hdr_std = hdr;
}

int main(int argc, char *argv[])
{
	/* timer */
	struct timeval	start;
	struct timeval	end;

	/* buf related */
	int client;
	int src_buffer[BUF_NUM];
	int dst_buffer[BUF_NUM];
	char *src_buf_addr[BUF_NUM];
	char *dst_buf_addr[BUF_NUM];
	unsigned int *src_addr[BUF_NUM];
	unsigned int *dst_addr[BUF_NUM];
	int src_buf_size = MAX_BUF_IMG_W * MAX_BUF_IMG_H * 4;
	int dst_buf_size = MAX_BUF_IMG_W * MAX_BUF_IMG_H * 4;

	struct decon_win_config_data win_data;
	struct decon_win_config *config = win_data.config;
	struct decon_frame src, dst;
	int ret = 0;
	int fb_fd = 0;

	class DpuSbwcDecoder test;
	unsigned int src_format, src_width, src_height, dst_format;
	unsigned int dst_width, dst_height, stride, dst_stride, attr;
	int inBuf[BUF_NUM];
	int outBuf[BUF_NUM];
	int i;
	int log_level;

	if (argc != 9) {
		printf("number of argument error\n");
		return -1;
	}

	client = exynos_ion_open();

	buf_alloc_and_map(client, src_buffer, src_buf_addr, src_addr, 1, src_buf_size);
	buf_alloc_and_map(client, dst_buffer, dst_buf_addr, dst_addr, 2, dst_buf_size);

	/***********************/
	/* sbwc input variable */
	/***********************/
	file_to_buf(argv[1], src_buf_addr[0]);
	stride = 	atoi(argv[2]);
	src_width =  	atoi(argv[2]);
	src_height = 	atoi(argv[3]);
	dst_stride = 	atoi(argv[4]);
	dst_width = 	atoi(argv[4]);
	dst_height = 	atoi(argv[5]);
	src_format = 	atoi(argv[6]);
	dst_format = 	atoi(argv[7]);
	log_level =	atoi(argv[8]);
	attr = 0;

	inBuf[0] = 	src_buffer[0];
	outBuf[0] = 	dst_buffer[0];
	outBuf[1] = 	dst_buffer[1];

	gettimeofday(&start, NULL);
	test.setLogLevel(log_level);
	if (test.decodeSBWC(src_format, dst_format, src_width, src_height,
			dst_width, dst_height, stride, dst_stride, attr, inBuf, outBuf) == false) {
		printf("decode SBWC failed\n");
		goto mem_err;
	}

	gettimeofday(&end, NULL);
	printf("difftime for decodeSBWC: %ld usec\n",
	    ((end.tv_sec * 1000000 + end.tv_usec)
	             - (start.tv_sec * 1000000 + start.tv_usec)));
	buf_to_file(dst_buf_addr[0], "/data/test_image", dst_buf_size);

	/****************/
	/* frame update */
	/****************/
	fb_fd = open(DEVNAME_FB0, O_RDWR);
	if (fb_fd < 0) {
		printf("failed to FB_BLANK_UNBLANK\n");
		goto fb_err;
	}

	ret = ioctl(fb_fd, FBIOBLANK, FB_BLANK_UNBLANK);
	if (ret < 0) {
		printf("failed to FB_BLANK_UNBLANK\n");
		goto fb_blank_err;
	}

	src.x = src.y = 0;
	src.w = dst_width;
	src.f_w = dst_stride;
	src.h = dst_height;
	src.f_h = dst_height;
	dst.x = dst.y = 0;
	dst.w = dst_height;
	dst.f_w = dst_height;
	dst.h = dst_width;
	dst.f_h = dst_stride;
        for (i = 0; i < ARRSIZE(halfmtnonSBWC_to_dpufmtSBWC); i++) {
            if (dst_format == halfmtnonSBWC_to_dpufmtSBWC[i][0]) {
		dst_format = halfmtnonSBWC_to_dpufmtSBWC[i][1];
                break;
	    }
	}
	set_hdr_win_config(config, 13, IDMA_VG1,
			(enum decon_pixel_format)dst_format, outBuf,
			&src, &dst, CSC_DCI_P3, DPP_HDR_OFF);
	if (dst_format == DECON_PIXEL_FORMAT_RGBA_8888
			|| dst_format == DECON_PIXEL_FORMAT_BGRA_8888)
		config[13].dpp_parm.rot = DPP_ROT_NORMAL;
	else
		config[13].dpp_parm.rot = DPP_ROT_90;


	ret = ioctl(fb_fd, S3CFB_WIN_CONFIG, &win_data);
	if (ret < 0) {
		printf("failed to S3CFB_WIN_CONFIG ioctl ret = %d\n", ret);
		goto fb_err;
	}
	sleep(5);

	ret = ioctl(fb_fd, FBIOBLANK, FB_BLANK_POWERDOWN);
	if (ret < 0) {
		printf("failed to FB_BLANK_POWERDOWN\n");
	}
	close(fb_fd);

	buf_unmap_and_free(src_buffer, src_buf_addr, 1, src_buf_size);
	buf_unmap_and_free(dst_buffer, dst_buf_addr, 2, dst_buf_size);

	return 0;
fb_err:
	ret = ioctl(fb_fd, FBIOBLANK, FB_BLANK_POWERDOWN);
	if (ret < 0) {
		printf("failed to FB_BLANK_POWERDOWN\n");
	}
fb_blank_err:
	close(fb_fd);

mem_err:
	buf_unmap_and_free(src_buffer, src_buf_addr, 1, src_buf_size);
	buf_unmap_and_free(dst_buffer, dst_buf_addr, 2, dst_buf_size);

	return -1;
}
