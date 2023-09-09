#ifndef __HDR_EXYNOS9815__
#define __HDR_EXYNOS9815__

#include <hardware/exynos/hdrInterface.h>
#include <system/graphics.h>
#include "libhdr_exynos9815_struct.h"
#include "libhdr_exynos9815_wcg_lut.h"
#include "libhdr10p.h"
#include "dynamic_info_legacy_support.h"

#define ARRSIZE(arr)	(sizeof(arr) / sizeof(arr[0]))
#define DOC_LEN		(128)
#define POSTFIX_LEN	(16)
#define NUMC		(2)
#define HDR		(0)
#define HDR10P		(1)

struct layer_info {
    int dataspace;
    ExynosHdrStaticInfo static_meta;
    ExynosHdrDynamicInfo_t dynamic_meta;
    bool premult_alpha;
    enum HdrBpc bpc;
    enum RenderSource source;
    float tf_matrix[4][4];
    bool bypass;
};
#define HDR_LAYER_MAX	(8)
#define HDR_LAYER_MIN	(0)
#define HDR_ID_MAX	(HDR_HW_DPU)
#define HDR_ID_MIN	(HDR_HW_DPU)
enum HdrHwId support[] = {HDR_HW_DPU, HDR_HW_MAX};

class hdrImplementation: public hdrInterface {
private:
    int OS_Version;
    int Llevel;
    int lum_max;
    int lum_min;
    int lum_sampling;

    struct hdrCoef hdrCoefBuf[NUMC][128];
    int hdrCoefBuf_n[NUMC];

    std::map<std::pair<int, int>, struct hdrCoef*> hdrCoef_HDR_map;
    std::map<std::pair<int, int>, struct hdrCoef*> hdrCoef_HDR10P_map;

    struct layer_info LayerInfo[HDR_ID_MAX + 1][HDR_LAYER_MAX + 1];
    bool has_dynamic[HDR_ID_MAX + 1][HDR_LAYER_MAX + 1];
    bool has_static[HDR_ID_MAX + 1][HDR_LAYER_MAX + 1];
    int layer_checked[HDR_ID_MAX + 1];
    int hw_select;
    struct HdrTargetInfo target_info;
    struct HdrTargetInfo extra_target_info[NUMC];
    bool extra_target_info_valid[NUMC] = {false, false};

    int push_hdr_stream(int dest, std::vector<std::string> &stream,
		    std::string &map_input1, std::string &map_input2);
    void saveLuminanceMinMax(std::string &luminance);
    void init_hdrInterface(
                        char *docname_mod_default,
                        char *docname_mod,
                        const char *docname_default,
                        const char *docname,
                        const char (*docname_postfix)[POSTFIX_LEN],
                        const char *docname_ext,
	                int &doc_cnt);
    void setLuminanceMinMaxSampling(int min, int max, int sampling);

    int getHdr10pCoef(struct hdrCoefParcel *parcel,
                        ExynosHdrDynamicInfo_t *dyn_meta,
                        unsigned int target_max_lum,
                        unsigned int source_max_lum);
    int getHdrCoef(enum HdrHwId hw_id, int layer_index,
		    android_dataspace ids, android_dataspace ods,
		    int luminance, struct hdrCoef *output);
    void refineTransfer(android_dataspace &ids);
public:
    hdrImplementation(const char *docname);
    ~hdrImplementation();
    int setTargetInfo(struct HdrTargetInfo __attribute__((unused)) *tInfo);
    int initHdrCoefBuildup(enum HdrHwId __attribute__((unused)) hw_id);
    int getHdrCoefSize(enum HdrHwId __attribute__((unused)) hw_id);
    int setLayerInfo(int __attribute__((unused)) layer_index, struct HdrLayerInfo __attribute__((unused)) *lInfo);
    int getHdrCoefData(enum HdrHwId __attribute__((unused)) hw_id, int __attribute__((unused)) layer_index,
                struct hdrCoefParcel __attribute__((unused)) *parcel);
    void setLogLevel(int __attribute__((unused)) log_level);
};

#endif
