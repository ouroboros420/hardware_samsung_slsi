#include <hardware/exynos/hdrInterface.h>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <utils/Log.h>
#include <string.h>
#include <VendorVideoAPI.h>
#include <libxml/parser.h>
#include <libxml/xmlmemory.h>

using std::string;
using std::map;
using std::pair;
using std::make_pair;
using std::vector;
using std::stringstream;

struct layer_info {
    int dataspace;
    ExynosHdrStaticInfo static_meta;
    ExynosHdrDynamicInfo dynamic_meta;
    bool premult_alpha;
    enum HdrBpc bpc;
    enum RenderSource source;
    float tf_matrix[4][4];
    bool bypass;
};
#define HDR_LAYER_MAX	(3)
#define HDR_LAYER_MIN	(0)
#define HDR_ID_MAX	(HDR_HW_DPU)
#define HDR_ID_MIN	(HDR_HW_DPU)
enum HdrHwId support[] = {HDR_HW_DPU, HDR_HW_MAX};

class hdrImplementation: public hdrInterface {
private:
    int Llevel;
    int lum_max;
    int lum_min;
    int lum_sampling;
    struct hdrCoef hdrCoefBuf[1024];
    int hdrCoefBuf_n;
    struct layer_info LayerInfo[HDR_ID_MAX + 1][HDR_LAYER_MAX + 1];
    bool has_dynamic[HDR_ID_MAX + 1][HDR_LAYER_MAX + 1];
    bool has_static[HDR_ID_MAX + 1][HDR_LAYER_MAX + 1];
    int layer_checked[HDR_ID_MAX + 1];
    int hw_select;
    std::map<std::pair<int, int>, struct hdrCoef*> hdrCoef_map;
    struct HdrTargetInfo target_info;
    int push_hdr_stream(std::vector<std::string> &stream, std::string &map_input1, std::string &map_input2);
    void saveLuminanceMinMax(std::string &luminance);
    void init_hdrInterface(void);
    void setLuminanceMinMaxSampling(int min, int max, int sampling);
    int getHdr10pCoef(struct hdrCoefParcel *parcel, ExynosHdrDynamicInfo *dyn_meta);
    int getHdrCoef(android_dataspace_t ids, android_dataspace_t ods,
                int luminance, struct hdrCoef *output);
public:
    hdrImplementation(const char *docname);
    ~hdrImplementation();
    int setTargetInfo(struct HdrTargetInfo __attribute__((unused)) *tInfo);
    int initHdrCoefBuildup(enum HdrHwId __attribute__((unused)) hw_id);
    int getHdrCoefSize(enum HdrHwId __attribute__((unused)) hw_id);
    int setLayerInfo( int __attribute__((unused)) layer_index, int __attribute__((unused)) dataspace,
                void __attribute__((unused)) *static_metadata, int __attribute__((unused)) static_len,
                void __attribute__((unused)) *dynamic_metadata, int __attribute__((unused)) dynamic_len,
                bool __attribute__((unused)) premult_alpha, enum HdrBpc __attribute__((unused)) bpc,
                enum RenderSource __attribute__((unused)) source, float __attribute__((unused)) *tf_matrix,
                bool __attribute__((unused)) bypass);
    int getHdrCoefData(enum HdrHwId __attribute__((unused)) hw_id, int __attribute__((unused)) layer_index,
                struct hdrCoefParcel __attribute__((unused)) *parcel);
    int getHdrCoef(android_dataspace_t ids[], int mastering_luminance[], int n_layer,
            android_dataspace_t ods, int peak_luminance,
	    struct hdrCoef output[4], int res_map[4]);
    void setLogLevel(int __attribute__((unused)) log_level);
};

template<typename Out>
static void split(const string &s, char delim, Out result)
{
    stringstream ss(s);
    string item;

    while (getline(ss, item, delim))
        *(result++) = item;
}

static void split(const string &s, const char delim, vector<string> &result)
{
    split(s, delim, back_inserter(result));
}

int hdrImplementation::push_hdr_stream(std::vector<std::string> &stream, std::string &map_input1, std::string &map_input2)
{
    vector<string> out;
    int i, j;
    enum hdr_n_idx{CON = 0, TM_X, TM_Y, TM_COEF, TM_RNGX, TM_RNGY, GM, EOTF_X, EOTF_Y, OETF_X, OETF_Y, HDR_MAX};
    int hdr_n[HDR_MAX] = {5, 33, 33, 3, 2, 2, 9, 129, 129, 33, 33};
    struct hdrCoef tmp_hdrCoef;

    int map1, map2;

    map1 = stoi(map_input1);
    map2 = stoi(map_input2);

    for (i = CON; i < HDR_MAX; i++) {
        split(stream[i], ',', out);
        if ((int)out.size() != hdr_n[i]) {
            ALOGD("number of hdr parameters wrong (idx:%d), (parsed:%d), (should-be:%d)",
                    i, (int)out.size(), hdr_n[i]);
            return -1;
        }

        switch(i) {
            case CON:
                tmp_hdrCoef.hdr_en = stoi(out[0]);
                tmp_hdrCoef.eotf_en = stoi(out[1]);
                tmp_hdrCoef.gm_en = stoi(out[2]);
                tmp_hdrCoef.oetf_en = stoi(out[3]);
                tmp_hdrCoef.tm_en = stoi(out[4]);
                break;
            case TM_X:
                for (j = 0; j < hdr_n[i]; j++) {
                    tmp_hdrCoef.tm_x[j] = stoi(out[j]);
                }
                break;
            case TM_Y:
                for (j = 0; j < hdr_n[i]; j++) {
                    tmp_hdrCoef.tm_y[j] = stoi(out[j]);
                }
                break;
            case TM_COEF:
                for (j = 0; j < hdr_n[i]; j++) {
                    tmp_hdrCoef.tm_coef[j] = stoi(out[j]);
                }
                break;
            case TM_RNGX:
                for (j = 0; j < hdr_n[i]; j++) {
                    tmp_hdrCoef.tm_rngx[j] = stoi(out[j]);
                }
                break;
            case TM_RNGY:
                for (j = 0; j < hdr_n[i]; j++) {
                    tmp_hdrCoef.tm_rngy[j] = stoi(out[j]);
                }
                break;
            case GM:
                for (j = 0; j < hdr_n[i]; j++) {
                    tmp_hdrCoef.gm_coef[j] = stoi(out[j]);
                }
                break;
            case EOTF_X:
                for (j = 0; j < hdr_n[i]; j++) {
                    tmp_hdrCoef.eotf_x[j] = stoi(out[j]);
                }
                break;
            case EOTF_Y:
                for (j = 0; j < hdr_n[i]; j++) {
                    tmp_hdrCoef.eotf_y[j] = stoi(out[j]);
                }
                break;
            case OETF_X:
                for (j = 0; j < hdr_n[i]; j++) {
                    tmp_hdrCoef.oetf_x[j] = stoi(out[j]);
                }
                break;
            case OETF_Y:
                for (j = 0; j < hdr_n[i]; j++) {
                    tmp_hdrCoef.oetf_y[j] = stoi(out[j]);
                }
                break;
        }
        out.clear();
    }

    memcpy(&hdrCoefBuf[hdrCoefBuf_n], &tmp_hdrCoef, sizeof(struct hdrCoef));
    hdrCoef_map.insert(make_pair(make_pair(map1, map2), &hdrCoefBuf[hdrCoefBuf_n++]));

    if (Llevel > 1)
        ALOGD("map_input1(%s), map_input2(%s), map1(%d), map2(%d), gm_coef(%d)",
            map_input1.c_str(), map_input2.c_str(), map1, map2, hdrCoefBuf[hdrCoefBuf_n - 1].gm_coef[7]);

    return 0;
}

void hdrImplementation::setLuminanceMinMaxSampling(int min, int max, int sampling)
{
    lum_max = max;
    lum_min = min;
    lum_sampling = sampling;
}

void hdrImplementation::init_hdrInterface(void)
{
    for (int i = 0; i < 1024; i++) {
	    hdrCoefBuf[i].hdr_en = 0;
	    hdrCoefBuf[i].oetf_en = 0;
	    hdrCoefBuf[i].eotf_en = 0;
	    hdrCoefBuf[i].gm_en = 0;
	    hdrCoefBuf[i].tm_en = 0;
    }

    setLogLevel(0);
    setLuminanceMinMaxSampling(1000, 10000, 100);
    hdrCoefBuf_n = 0;

    for (int i = 0; i < (HDR_ID_MAX + 1); i++)
        layer_checked[i] = 0;
    hw_select = HDR_HW_MAX + 1;
}

hdrImplementation::hdrImplementation(const char *docname)
{
    xmlDocPtr doc;
    xmlNodePtr cur, node;
    xmlChar *key;
    vector<string> stream;
    string map_input1, map_input2;

    init_hdrInterface();

    doc = xmlParseFile(docname);
    if (doc == NULL) {
        ALOGE("can not parse document\n");
        return;
    }

    cur = xmlDocGetRootElement(doc);
    if (cur == NULL) {
        ALOGE("empty document\n");
        xmlFreeDoc(doc);
        return;
    }

    if (xmlStrcmp(cur->name, (const xmlChar *)"Calib_Data")) {
        ALOGE("document of the wrong type, root node != Calib_Data\n");
        xmlFreeDoc(doc);
        return;
    }

    cur = cur->xmlChildrenNode;
    while (cur != NULL) {
        if ((!xmlStrcmp(cur->name, (const xmlChar *)"mode"))) {
            xmlChar *mode_name, *ids_id, *ods_id, *luminance;

            mode_name = xmlGetProp(cur, (const xmlChar *)"name");

            if ((!xmlStrcmp(mode_name, (const xmlChar *)"HDR"))) {

                ids_id = xmlGetProp(cur, (const xmlChar *)"ids_id");
                luminance = xmlGetProp(cur, (const xmlChar *)"luminance");

                node = cur->xmlChildrenNode;

                while (node != NULL) {
                    if (!xmlStrcmp(node->name, (const xmlChar*)"CON")) {
                        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                        stream.push_back((char *)key);
                        xmlFree(key);
                    } else if (!xmlStrcmp(node->name, (const xmlChar*)"TM_X")) {
                        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                        stream.push_back((char *)key);
                        xmlFree(key);
                    } else if (!xmlStrcmp(node->name, (const xmlChar*)"TM_Y")) {
                        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                        stream.push_back((char *)key);
                        xmlFree(key);
                    } else if (!xmlStrcmp(node->name, (const xmlChar*)"TM_COEF")) {
                        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                        stream.push_back((char *)key);
                        xmlFree(key);
                    } else if (!xmlStrcmp(node->name, (const xmlChar*)"TM_RNGX")) {
                        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                        stream.push_back((char *)key);
                        xmlFree(key);
                    } else if (!xmlStrcmp(node->name, (const xmlChar*)"TM_RNGY")) {
                        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                        stream.push_back((char *)key);
                        xmlFree(key);
                    } else if (!xmlStrcmp(node->name, (const xmlChar*)"GM")) {
                        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                        stream.push_back((char *)key);
                        xmlFree(key);
                    } else if (!xmlStrcmp(node->name, (const xmlChar*)"EOTF_X")) {
                        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                        stream.push_back((char *)key);
                        xmlFree(key);
                    } else if (!xmlStrcmp(node->name, (const xmlChar*)"EOTF_Y")) {
                        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                        stream.push_back((char *)key);
                        xmlFree(key);
                    } else if (!xmlStrcmp(node->name, (const xmlChar*)"OETF_X")) {
                        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                        stream.push_back((char *)key);
                        xmlFree(key);
                    } else if (!xmlStrcmp(node->name, (const xmlChar*)"OETF_Y")) {
                        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                        stream.push_back((char *)key);
                        xmlFree(key);
                    }
                    node = node->next;
                }

		map_input1 = (const char*)ids_id;
		map_input2 = (const char*)luminance;

                saveLuminanceMinMax(map_input2);

                if (push_hdr_stream(stream, map_input1, map_input2)) {
                    ALOGE("no space for hdr stream");
                }
                stream.clear();

                xmlFree(ids_id);
                xmlFree(luminance);
            } else if ((!xmlStrcmp(mode_name, (const xmlChar *)"WCG"))) {

                ids_id = xmlGetProp(cur, (const xmlChar *)"ids_id");
                ods_id = xmlGetProp(cur, (const xmlChar *)"ods_id");

                node = cur->xmlChildrenNode;

                while (node != NULL) {
                    if (!xmlStrcmp(node->name, (const xmlChar*)"CON")) {
                        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                        stream.push_back((char *)key);
                        xmlFree(key);
                    } else if (!xmlStrcmp(node->name, (const xmlChar*)"TM_X")) {
                        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                        stream.push_back((char *)key);
                        xmlFree(key);
                    } else if (!xmlStrcmp(node->name, (const xmlChar*)"TM_Y")) {
                        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                        stream.push_back((char *)key);
                        xmlFree(key);
                    } else if (!xmlStrcmp(node->name, (const xmlChar*)"TM_COEF")) {
                        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                        stream.push_back((char *)key);
                        xmlFree(key);
                    } else if (!xmlStrcmp(node->name, (const xmlChar*)"TM_RNGX")) {
                        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                        stream.push_back((char *)key);
                        xmlFree(key);
                    } else if (!xmlStrcmp(node->name, (const xmlChar*)"TM_RNGY")) {
                        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                        stream.push_back((char *)key);
                        xmlFree(key);
                    } else if (!xmlStrcmp(node->name, (const xmlChar*)"GM")) {
                        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                        stream.push_back((char *)key);
                        xmlFree(key);
                    } else if (!xmlStrcmp(node->name, (const xmlChar*)"EOTF_X")) {
                        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                        stream.push_back((char *)key);
                        xmlFree(key);
                    } else if (!xmlStrcmp(node->name, (const xmlChar*)"EOTF_Y")) {
                        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                        stream.push_back((char *)key);
                        xmlFree(key);
                    } else if (!xmlStrcmp(node->name, (const xmlChar*)"OETF_X")) {
                        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                        stream.push_back((char *)key);
                        xmlFree(key);
                    } else if (!xmlStrcmp(node->name, (const xmlChar*)"OETF_Y")) {
                        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                        stream.push_back((char *)key);
                        xmlFree(key);
                    }
                    node = node->next;
                }

		map_input1 = (char*)ids_id;
		map_input2 = (char*)ods_id;
                if (push_hdr_stream(stream, map_input1, map_input2)) {
                    ALOGE("no space for hdr stream");
                }
                stream.clear();

                xmlFree(ids_id);
                xmlFree(ods_id);
            }
            xmlFree(mode_name);
        }
        cur = cur->next;
    }
    xmlFreeDoc(doc);
}

hdrImplementation::~hdrImplementation()
{
}

static void printhdrCoef(struct hdrCoef *output)
{
    int i;

    ALOGD("%s +", __func__);
    ALOGD("output->hdr_en : %d", output->hdr_en);
    if (!output->hdr_en)
        return;

    if (output->oetf_en) {
        for (i = 0; i < 30; i+=10) {
            ALOGD("oetf_x[%7d %7d %7d %7d %7d %7d %7d %7d %7d %7d]",
                output->oetf_x[i],   output->oetf_x[i+1],
                output->oetf_x[i+2], output->oetf_x[i+3],
                output->oetf_x[i+4], output->oetf_x[i+5],
                output->oetf_x[i+6], output->oetf_x[i+7],
                output->oetf_x[i+8], output->oetf_x[i+9]);
        }
        ALOGD("oetf_x[%7d %7d %7d]",
            output->oetf_x[i],   output->oetf_x[i+1], output->oetf_x[i+2]);

        for (i = 0; i < 30; i+=10) {
            ALOGD("oetf_y[%7d %7d %7d %7d %7d %7d %7d %7d %7d %7d]",
                output->oetf_y[i],   output->oetf_y[i+1],
                output->oetf_y[i+2], output->oetf_y[i+3],
                output->oetf_y[i+4], output->oetf_y[i+5],
                output->oetf_y[i+6], output->oetf_y[i+7],
                output->oetf_y[i+8], output->oetf_y[i+9]);
        }
        ALOGD("oetf_y[%7d %7d %7d]",
            output->oetf_y[i],   output->oetf_y[i+1], output->oetf_y[i+2]);
    }

    if (output->eotf_en) {
        for (i = 0; i < 120; i+=10) {
            ALOGD("eotf_x[%7d %7d %7d %7d %7d %7d %7d %7d %7d %7d]",
                output->eotf_x[i],   output->eotf_x[i+1],
                output->eotf_x[i+2], output->eotf_x[i+3],
                output->eotf_x[i+4], output->eotf_x[i+5],
                output->eotf_x[i+6], output->eotf_x[i+7],
                output->eotf_x[i+8], output->eotf_x[i+9]);
        }
        ALOGD("eotf_x[%7d %7d %7d %7d %7d %7d %7d %7d %7d]",
            output->eotf_x[i],   output->eotf_x[i+1],
            output->eotf_x[i+2], output->eotf_x[i+3],
            output->eotf_x[i+4], output->eotf_x[i+5],
            output->eotf_x[i+6], output->eotf_x[i+7],
            output->eotf_x[i+8]);

        for (i = 0; i < 120; i+=10) {
            ALOGD("eotf_y[%7d %7d %7d %7d %7d %7d %7d %7d %7d %7d]",
                output->eotf_y[i],   output->eotf_y[i+1],
                output->eotf_y[i+2], output->eotf_y[i+3],
                output->eotf_y[i+4], output->eotf_y[i+5],
                output->eotf_y[i+6], output->eotf_y[i+7],
                output->eotf_y[i+8], output->eotf_y[i+9]);
        }
        ALOGD("eotf_y[%7d %7d %7d %7d %7d %7d %7d %7d %7d]",
            output->eotf_y[i],   output->eotf_y[i+1],
            output->eotf_y[i+2], output->eotf_y[i+3],
            output->eotf_y[i+4], output->eotf_y[i+5],
            output->eotf_y[i+6], output->eotf_y[i+7],
            output->eotf_y[i+8]);
    }

    if (output->gm_en) {
        i = 0;
        ALOGD("gm[%7d %7d %7d %7d %7d %7d %7d %7d %7d]",
            output->gm_coef[i],   output->gm_coef[i+1],
            output->gm_coef[i+2], output->gm_coef[i+3],
            output->gm_coef[i+4], output->gm_coef[i+5],
            output->gm_coef[i+6], output->gm_coef[i+7],
            output->gm_coef[i+8]);
    }

    if (output->tm_en) {
        ALOGD("tm_coef[%7d %7d %7d]", output->tm_coef[0],   output->tm_coef[1], output->tm_coef[2]);
        ALOGD("tm_rngx[%7d %7d]", output->tm_rngx[0],   output->tm_rngx[1]);
        ALOGD("tm_rngy[%7d %7d]", output->tm_rngy[0],   output->tm_rngy[1]);

        for (i = 0; i < 30; i+=10) {
            ALOGD("tm_x[%7d %7d %7d %7d %7d %7d %7d %7d %7d %7d]",
                output->tm_x[i],   output->tm_x[i+1],
                output->tm_x[i+2], output->tm_x[i+3],
                output->tm_x[i+4], output->tm_x[i+5],
                output->tm_x[i+6], output->tm_x[i+7],
                output->tm_x[i+8], output->tm_x[i+9]);
        }
        ALOGD("tm_x[%7d %7d %7d]",
            output->tm_x[i],   output->tm_x[i+1], output->tm_x[i+2]);

        for (i = 0; i < 30; i+=10) {
            ALOGD("tm_y[%7d %7d %7d %7d %7d %7d %7d %7d %7d %7d]",
                output->tm_y[i],   output->tm_y[i+1],
                output->tm_y[i+2], output->tm_y[i+3],
                output->tm_y[i+4], output->tm_y[i+5],
                output->tm_y[i+6], output->tm_y[i+7],
                output->tm_y[i+8], output->tm_y[i+9]);
        }
        ALOGD("ty_y[%7d %7d %7d]",
            output->tm_y[i],   output->tm_y[i+1], output->tm_y[i+2]);
    }
    ALOGD("%s -", __func__);
}

static void refineTransfer(android_dataspace &ids) {
    int transfer = ids & HAL_DATASPACE_TRANSFER_MASK;
    int result = ids;
    switch (transfer) {
        case HAL_DATASPACE_TRANSFER_SRGB:
        case HAL_DATASPACE_TRANSFER_LINEAR:
        case HAL_DATASPACE_TRANSFER_ST2084:
        case HAL_DATASPACE_TRANSFER_HLG:
            break;
        default:
            result = result & ~(HAL_DATASPACE_TRANSFER_MASK);
            ids = (android_dataspace)(result | HAL_DATASPACE_TRANSFER_SRGB);
            break;
    }
}

int hdrImplementation::setTargetInfo(struct HdrTargetInfo __attribute__((unused)) *tInfo)
{
    memcpy(&target_info, tInfo, sizeof(struct HdrTargetInfo));
    return HDR_ERR_NO;
}

int hdrImplementation::initHdrCoefBuildup(enum HdrHwId __attribute__((unused)) hw_id)
{
    if (hw_id < HDR_ID_MIN || hw_id > HDR_ID_MAX) {
	ALOGE("%s:selected hw id (%d) out of range (max:%d, min:%d)",
			__func__, hw_id, HDR_ID_MAX, HDR_ID_MIN);
	hw_select = HDR_HW_MAX + 1;
        return HDR_ERR_INVAL;
    }

    layer_checked[hw_id] = 0;
    hw_select = hw_id;
    for (int i = 0; i < (HDR_LAYER_MAX + 1); i++) {
        has_static[hw_select][i] = 0;
        has_dynamic[hw_select][i] = 0;
    }

    return HDR_ERR_NO;
}

int hdrImplementation::getHdrCoefSize(enum HdrHwId __attribute__((unused)) hw_id)
{
    int size = 0;
    int i;
    for (i = 0; i < (HDR_ID_MAX + 1); i++) {
        if (Llevel > 1)
	    ALOGD("%s: idx(%d), support_hw_id(%d), hw_id(%d)",
			    __func__, i, support[i], hw_id);
        if (support[i] == hw_id) {
            if (Llevel > 1)
	        ALOGD("%s: hw id(%d) supported", __func__, hw_id);
            size = sizeof(struct hdrCoef);
	    return size;
	}
    }
    ALOGE("%s:hw id(%d) not supported", __func__, hw_id);
    size = 0;
    return size;
}

int hdrImplementation::setLayerInfo(int __attribute__((unused)) layer_index, int __attribute__((unused)) dataspace,
                void __attribute__((unused)) *static_metadata, int __attribute__((unused)) static_len,
                void __attribute__((unused)) *dynamic_metadata, int __attribute__((unused)) dynamic_len,
                bool __attribute__((unused)) premult_alpha, enum HdrBpc __attribute__((unused)) bpc,
                enum RenderSource __attribute__((unused)) source, float __attribute__((unused)) *tf_matrix,
                bool __attribute__((unused)) bypass)
{
    if (hw_select < HDR_ID_MIN || hw_select > HDR_ID_MAX) {
	ALOGE("%s:selected hw id (%d) out of range (max=%d, min=%d)",
			__func__, hw_select, HDR_ID_MAX, HDR_ID_MIN);
        return HDR_ERR_INVAL;
    }

    if (layer_index < HDR_LAYER_MIN || layer_index > HDR_LAYER_MAX) {
	ALOGE("%s:selected layer index(%d) out of range (max=%d, min=%d)",
			__func__, layer_index, HDR_LAYER_MAX, HDR_LAYER_MIN);
        return HDR_ERR_INVAL;
    }

    layer_checked[hw_select] |= (1 << layer_index);
    LayerInfo[hw_select][layer_index].dataspace = dataspace;

    if (static_metadata) {
        if (static_len == sizeof(ExynosHdrStaticInfo)) {
            memcpy(&LayerInfo[hw_select][layer_index].static_meta, static_metadata, static_len);
	    has_static[hw_select][layer_index] = 1;
	} else
	    ALOGE("%s:unexpected static meta size(%d), expected(%u)",
			    __func__, static_len, (unsigned int)sizeof(ExynosHdrStaticInfo));
    }

    if (dynamic_metadata) {
        if (dynamic_len == sizeof(ExynosHdrDynamicInfo)) {
            memcpy(&LayerInfo[hw_select][layer_index].dynamic_meta, dynamic_metadata, dynamic_len);
	    has_dynamic[hw_select][layer_index] = 1;
	} else
	    ALOGE("%s:unexpected dynamic meta size(%d), expected(%u)",
			    __func__, dynamic_len, (unsigned int)sizeof(ExynosHdrDynamicInfo));
    }

    LayerInfo[hw_select][layer_index].premult_alpha = premult_alpha;
    LayerInfo[hw_select][layer_index].bpc = bpc;
    LayerInfo[hw_select][layer_index].source = source;
    if (tf_matrix)
        memcpy(LayerInfo[hw_select][layer_index].tf_matrix, tf_matrix, 16);
    LayerInfo[hw_select][layer_index].bypass = bypass;
    return HDR_ERR_NO;
}

int hdrImplementation::getHdrCoef(android_dataspace ids, android_dataspace ods,
        int luminance, struct hdrCoef *output)
{
    struct hdrCoef *mapped;
    int in_st_tf = 0;
    int pq_ds = (HAL_DATASPACE_STANDARD_BT2020 | HAL_DATASPACE_TRANSFER_ST2084);
    int hlg_ds = (HAL_DATASPACE_STANDARD_BT2020 | HAL_DATASPACE_TRANSFER_HLG);
    int temp_lum = 0;

    refineTransfer(ids);

    in_st_tf = ids & (HAL_DATASPACE_STANDARD_MASK | HAL_DATASPACE_TRANSFER_MASK);

    if (in_st_tf == pq_ds) {
	temp_lum = luminance;
	if (temp_lum > 10000) {
		temp_lum = 0;
	}

	do {
            if (Llevel > 0) {
                ALOGD("searching hdr(PQ) coef set by ids(%d), lum(%d), temp_lum(%d)",
                (int)ids, luminance, temp_lum);
            }
            mapped = hdrCoef_map[make_pair((int)ids, (int)temp_lum)];
	    temp_lum += lum_sampling;
	    temp_lum = temp_lum - (temp_lum % lum_sampling);
	} while ((temp_lum <= lum_max) && !mapped);
        if (!mapped) {
            ALOGE("no hdr(PQ) coef set by ids(%d), lum(%d), temp_lum(%d)",
                (int)ids, luminance, temp_lum);
            return -1;
        }
    } else if (in_st_tf == hlg_ds) {
        temp_lum = luminance;
        luminance = -1;
        mapped = hdrCoef_map[make_pair((int)ids, (int)luminance)];
	if (Llevel > 0) {
            ALOGD("searching hdr(hlg) coef set by ids(%d), lum(%d)",
                (int)ids, (int)luminance);
	}
        if (!mapped) {
            ALOGE("no hdr(HLG) coef set by ids(%d), lum(%d), temp_lum(%d)\n",
                (int)ids, luminance, temp_lum);
            return -1;
	}
    } else {
        mapped = hdrCoef_map[make_pair((int)ids, (int)ods)];
	if (Llevel > 0) {
            ALOGD("searching wcg coef set by ids(%d), ods(%d)",
                (int)ids, (int)ods);
	}
        if (!mapped) {
	    if (Llevel > 0) {
                ALOGE("no wcg coef set by ids(%d), ods(%d)\n",
                    (int)ids, (int)ods);
	    }
            return -1;
	}
    }

    memcpy(output , mapped, sizeof(struct hdrCoef));

    if (Llevel > 1)
        printhdrCoef(output);

    return 0;
}

int hdrImplementation::getHdr10pCoef(struct hdrCoefParcel *parcel, ExynosHdrDynamicInfo *dyn_meta)
{
    if (Llevel > 3)
	ALOGD("%s:parcel(0x%lx), dyn_meta(0x%lx)", __func__, (unsigned long)parcel, (unsigned long)dyn_meta);
    return 0;
}

int hdrImplementation::getHdrCoefData(
		enum HdrHwId __attribute__((unused)) hw_id,
		int __attribute__((unused)) layer_index,
                struct hdrCoefParcel __attribute__((unused)) *parcel)
{
    int ids, ods, luminance;

    if (hw_id < HDR_ID_MIN || hw_id > HDR_ID_MAX) {
	ALOGE("%s:selected hw id (%d) out of range (max=%d, min=%d)",
			__func__, hw_select, HDR_ID_MAX, HDR_ID_MIN);
        return HDR_ERR_INVAL;
    }
    if (hw_id != hw_select) {
	ALOGE("%s:hw_id(%d) intended deos not match hw id(%d) selected from init()",
			__func__, hw_id, hw_select);
        return HDR_ERR_INVAL;
    }
    if (layer_index < HDR_LAYER_MIN || layer_index > HDR_LAYER_MAX) {
	ALOGE("%s:selected layer index(%d) out of range (max=%d, min=%d)",
			__func__, layer_index, HDR_LAYER_MAX, HDR_LAYER_MIN);
        return HDR_ERR_INVAL;
    }
    if (!(layer_checked[hw_select] & (1 << layer_index))) {
	ALOGE("%s:layer_index(%d) was not configure from setLayerInfo()(->layer_checked(%d))",
			__func__, 1 << layer_index, layer_checked[hw_select]);
        return HDR_ERR_INVAL;
    }

    ids = LayerInfo[hw_id][layer_index].dataspace;
    if (has_static[hw_id][layer_index])
        luminance = LayerInfo[hw_id][layer_index].static_meta.sType1.mMaxDisplayLuminance;
    else {
	if (Llevel > 1)
	    ALOGD("%s:no static meta data for the layer(%d) hw_id(%d)",
			    __func__, layer_index, hw_id);
        luminance = lum_max;
    }
    ods = target_info.dataspace;

    if (parcel->hdrCoef == NULL)
        return HDR_ERR_INVAL;

    if (getHdrCoef((android_dataspace)ids, (android_dataspace)ods, luminance, (struct hdrCoef*)parcel->hdrCoef))
        return HDR_ERR_INVAL;

    if (has_dynamic[hw_id][layer_index])
        getHdr10pCoef(parcel, &LayerInfo[hw_id][layer_index].dynamic_meta);

    return HDR_ERR_NO;
}

void hdrImplementation::setLogLevel(int __attribute__((unused)) log_level)
{
    Llevel = log_level;
}

void hdrImplementation::saveLuminanceMinMax(std::string &luminance)
{
	int lum = stoi(luminance);

	if (lum < lum_min) {
		lum_min = lum;
	}

	if (lum > lum_max) {
		lum_max = lum;
	}
}

hdrInterface *hdrInterface::createInstance(const char __attribute__((unused)) *docname) {
    return new hdrImplementation(docname);
}

int hdrImplementation::getHdrCoef(android_dataspace_t ids[], int mastering_luminance[], int n_layer,
            android_dataspace_t ods, int peak_luminance,
	    struct hdrCoef output[4], int res_map[4])
{
    android_dataspace ids_map[4];
    int mapped_st_tf, input_st_tf;
    int input_lum;
    int lum_map[4];
    int map_index;
    int i, j, unmapped;
    int debug_map;

    if (n_layer > 16) {
	ALOGE("number of layers limit exceeded(n_layer : %d, max = 16)\n", n_layer);
        return -1;
    } else if (n_layer <= 0) {
	ALOGE("number of layers less than expected(n_layer : %d, min = 1)\n", n_layer);
	return -1;
    }

    peak_luminance = 0;
    for (i = 0; i < 4; i++) {
        res_map[i] = 0;
	ids_map[i] = (android_dataspace)-1;
	lum_map[i] = -1;
    }
    for (i = 0; i < n_layer; i++)
        refineTransfer(ids[i]);

    for (i = 0, map_index = 0; i < n_layer; i++) {
        input_st_tf = (ids[i] & (HAL_DATASPACE_STANDARD_MASK | HAL_DATASPACE_TRANSFER_MASK));
        if (input_st_tf == (HAL_DATASPACE_STANDARD_BT2020 | HAL_DATASPACE_TRANSFER_ST2084)) {
	    input_lum = mastering_luminance[i];
	    if (i == 0) {
                ids_map[map_index] = ids[i];
                res_map[map_index] |= (1 << i);
		lum_map[map_index] = input_lum;
		debug_map = map_index;
            } else {
	        for (j = 0, unmapped = 1; j < (map_index + 1); j++) {
                    mapped_st_tf = (ids_map[j] & (HAL_DATASPACE_STANDARD_MASK | HAL_DATASPACE_TRANSFER_MASK));
                    if ((mapped_st_tf == input_st_tf) && (input_lum == lum_map[map_index])) {
                        res_map[j] |= (1 << i);
			unmapped = 0;
			debug_map = j;
			break;
		    }
	        }

		if (unmapped) {
	            map_index++;
	            if (map_index > 3)
                        break;
                    ids_map[map_index] = ids[i];
                    res_map[map_index] |= (1 << i);
	            lum_map[map_index] = input_lum;
		    debug_map = map_index;
		}
            }
	    if (Llevel > 1) {
                mapped_st_tf = (ids_map[map_index] & (HAL_DATASPACE_STANDARD_MASK | HAL_DATASPACE_TRANSFER_MASK));
	        ALOGD("resource mapping... layer[%d](ids:st_tf=%d:%d,ids_map[%d]:mapped=%d:%d,res_map[%d]=%d,lum_map[%d]=%d)\n",
		    i, (int)ids[i], input_st_tf,
	            debug_map, (int)ids_map[debug_map], mapped_st_tf, debug_map, (int)res_map[debug_map], debug_map, lum_map[debug_map]);
	    }
        } else {
	    if (i == 0) {
                ids_map[map_index] = ids[i];
                res_map[map_index] |= (1 << i);
		debug_map = map_index;
            } else {
	        for (j = 0, unmapped = 1; j < (map_index + 1); j++) {
                    mapped_st_tf = (ids_map[j] & (HAL_DATASPACE_STANDARD_MASK | HAL_DATASPACE_TRANSFER_MASK));
                    if (mapped_st_tf == input_st_tf) {
                        res_map[j] |= (1 << i);
			unmapped = 0;
		        debug_map = j;
			break;
		    }
	        }

		if (unmapped) {
	            map_index++;
	            if (map_index > 3)
                        break;
                    ids_map[map_index] = ids[i];
                    res_map[map_index] |= (1 << i);
		    debug_map = map_index;
		}
            }
	    if (Llevel > 1) {
                mapped_st_tf = (ids_map[map_index] & (HAL_DATASPACE_STANDARD_MASK | HAL_DATASPACE_TRANSFER_MASK));
	        ALOGD("resource mapping... layer[%d](ids:st_tf=%d:%d,ids_map[%d]:mapped=%d:%d,res_map[%d]=%d\n",
		    i, (int)ids[i], input_st_tf,
	            debug_map, (int)ids_map[debug_map], mapped_st_tf, debug_map, (int)res_map[debug_map]);
	    }
        }
    }

    if (map_index > 3) {
        ALOGE("resource map fails (number of pipelines needed more than %d(max:4),\
		last layer index(%d), n_layer(%d))\n",
			map_index + 1, i, n_layer);
    /*    if (Llevel > 0) {
            print_resource_map(res_map);
            print_ds_dump(ids, mastering_luminance, n_layer, ods, peak_luminance);
        }*/
        return -2;
    }

    if (map_index == 0) {
        if ((ids_map[map_index] == HAL_DATASPACE_UNKNOWN) || (ids_map[map_index] == ods)) {
            if (Llevel > 0)
                ALOGD("hdr/wcg no needed\n");
            return 1;
	}
    }

    for (i = 0; i < map_index + 1; i++) {
        if (ids_map[i] == (android_dataspace)-1) {
            for (j = 0; j < map_index; j++) {
	        ALOGE("resource map wrong(ids_map[%d]:%d, lum_map[%d]:%d)\n",
                        j, (int)ids_map[j], j, lum_map[j]);
	    }
            return -2;
	}

	if (ids_map[i] == HAL_DATASPACE_UNKNOWN) {
            output[i].hdr_en = 0;
	    if (Llevel > 0)
                ALOGD("wcg coef(unknown) set by ids(%d), ods(%d)\n", (int)ids_map[i], (int)ods);
	} else if (getHdrCoef(ids_map[i], ods, lum_map[i], &output[i]))
            return -3;
    }

/*    if (Llevel > 0)
	print_resource_map(res_map);*/

    return 0;
}

