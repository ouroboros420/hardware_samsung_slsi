#ifndef __HDR_ST_EXYNOS9815__
#define __HDR_ST_EXYNOS9815__

typedef unsigned int u32;
typedef int s32;

#define HDR_OETF_H_POSX_LUT_REG_CNT	(65)
#define HDR_OETF_H_POSX_LUT_CNT	(HDR_OETF_H_POSX_LUT_REG_CNT)
#define HDR_OETF_H_POSY_LUT_REG_CNT	(33)
#define HDR_OETF_H_POSY_LUT_CNT	(HDR_OETF_H_POSY_LUT_REG_CNT * 2 - 1)
struct hdr_lut_oetf {
	u32 en;
	u32 lut_x[HDR_OETF_H_POSX_LUT_CNT];
	u32 lut_y[HDR_OETF_H_POSY_LUT_CNT];
};

#define HDR_EOTF_POSX_LUT_REG_CNT	(33)
#define HDR_EOTF_POSX_LUT_CNT		(HDR_EOTF_POSX_LUT_REG_CNT * 2 - 1)
#define HDR_EOTF_POSY_LUT_REG_CNT	(65)
#define HDR_EOTF_POSY_LUT_CNT		(HDR_EOTF_POSY_LUT_REG_CNT)
struct hdr_lut_eotf {
	u32 en;
	u32 lut_x[HDR_EOTF_POSX_LUT_CNT];
	u32 lut_y[HDR_EOTF_POSY_LUT_CNT];
	/* only valid at HDR10PFLH */
	u32 pq_en;
	u32 coef;
	u32 shift;
};

struct hdr_lut_gm {
	u32 en;
	s32 coef[3][3];
	s32 offs[3];
};

#define HDR_TM_POSX_LUT_REG_CNT		(33)
#define HDR_TM_POSX_LUT_CNT		(HDR_TM_POSX_LUT_REG_CNT)
#define HDR_TM_POSY_LUT_REG_CNT		(33)
#define HDR_TM_POSY_LUT_CNT		(HDR_TM_POSY_LUT_REG_CNT)
struct hdr_lut_tm {
	u32 en;
	u32 lut_x[HDR_TM_POSX_LUT_CNT];
	u32 lut_y[HDR_TM_POSY_LUT_CNT];
	u32 coef_r;
	u32 coef_g;
	u32 coef_b;
	u32 min_x;
	u32 max_x;
	u32 min_y;
	u32 max_y;
};

struct hdrCoef {
	u32 en;
	u32 mul_en; /* de-multiplication -> HDR -> re-multiplication */
	struct hdr_lut_oetf oetf;
	struct hdr_lut_eotf eotf;
	struct hdr_lut_gm gm;
	struct hdr_lut_tm tm;
};

#endif
