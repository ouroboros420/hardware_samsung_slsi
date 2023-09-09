#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <utils/Log.h>
#include <stdio.h>
#include <string.h>
#include <VendorVideoAPI.h>
#include <libxml/parser.h>
#include <libxml/xmlmemory.h>
#include <system/graphics.h>
#include <cmath>
#include <cutils/properties.h>

#include <fcntl.h>
#include <unistd.h>

#include "libhdr_plugin_exynos9815.h"

using std::string;
using std::map;
using std::pair;
using std::make_pair;
using std::vector;
using std::stringstream;

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

int hdrImplementation::push_hdr_stream(int dest, std::vector<std::string> &stream, std::string &map_input1, std::string &map_input2)
{
    vector<string> out;
    int i, j, k;
    /* mandatory : all in order */
    enum hdr_n_idx{HDR_EN = 0,
	    EOTF_EN, EOTF_X, EOTF_Y, NORMAL_EN, NORMAL_COEF, NORMAL_SHIFT,
	    OETF_EN, OETF_X, OETF_Y,
	    GM_EN, GM, GM_OFFSET,
	    TM_EN, TM_X, TM_Y, TM_COEF, TM_RNGX, TM_RNGY,
	    HDR_MAX};
    int hdr_n[HDR_MAX] = {1,
	    1, HDR_EOTF_POSX_LUT_CNT, HDR_EOTF_POSY_LUT_CNT, 1, 1, 1,
            1, HDR_OETF_H_POSX_LUT_CNT, HDR_OETF_H_POSY_LUT_CNT,
            1, 9, 3,
            1, HDR_TM_POSX_LUT_CNT, HDR_TM_POSY_LUT_CNT, 3, 2, 2};
    struct hdrCoef tmp_hdrCoef;

    int map1, map2;

    map1 = stoi(map_input1);
    map2 = stoi(map_input2);

    memset(&tmp_hdrCoef, 0, sizeof(hdrCoef));
    for (i = HDR_EN; i < HDR_MAX; i++) {
        split(stream[i], ',', out);
        if ((int)out.size() != hdr_n[i]) {
            ALOGD("number of hdr parameters wrong (idx:%d), (parsed:%d), (should-be:%d)",
                    i, (int)out.size(), hdr_n[i]);
            return -1;
        }

        switch(i) {
            case HDR_EN:
                tmp_hdrCoef.en = stoi(out[0]);
                tmp_hdrCoef.mul_en = 0;
                break;
            case EOTF_EN:
                tmp_hdrCoef.eotf.en = stoi(out[0]);
                break;
            case EOTF_X:
                for (j = 0; j < hdr_n[i]; j++)
                    tmp_hdrCoef.eotf.lut_x[j] = stoi(out[j]);
                break;
	    case EOTF_Y:
                for (j = 0; j < hdr_n[i]; j++)
                    tmp_hdrCoef.eotf.lut_y[j] = stoi(out[j]);
		break;
	    case NORMAL_EN:
                tmp_hdrCoef.eotf.pq_en = stoi(out[0]);
		break;
	    case NORMAL_COEF:
                tmp_hdrCoef.eotf.coef = stoi(out[0]);
		break;
	    case NORMAL_SHIFT:
                tmp_hdrCoef.eotf.shift = stoi(out[0]);
		break;
	    case OETF_EN:
                tmp_hdrCoef.oetf.en = stoi(out[0]);
		break;
	    case OETF_X:
                for (j = 0; j < hdr_n[i]; j++)
                    tmp_hdrCoef.oetf.lut_x[j] = stoi(out[j]);
		break;
	    case OETF_Y:
                for (j = 0; j < hdr_n[i]; j++)
                    tmp_hdrCoef.oetf.lut_y[j] = stoi(out[j]);
		break;
	    case GM_EN:
                tmp_hdrCoef.gm.en = stoi(out[0]);
		break;
	    case GM:
                for (j = 0; j < (hdr_n[i] / 3); j++)
                    for (k = 0; k < (hdr_n[i] / 3); k++)
			    tmp_hdrCoef.gm.coef[j][k] = stoi(out[(j * (hdr_n[i] / 3)) + k]);
		break;
	    case GM_OFFSET:
                for (j = 0; j < hdr_n[i]; j++)
                    tmp_hdrCoef.gm.offs[j] = stoi(out[j]);
		break;
	    case TM_EN:
                tmp_hdrCoef.tm.en = stoi(out[0]);
		break;
	    case TM_X:
                for (j = 0; j < hdr_n[i]; j++)
                    tmp_hdrCoef.tm.lut_x[j] = stoi(out[j]);
		break;
	    case TM_Y:
                for (j = 0; j < hdr_n[i]; j++)
                    tmp_hdrCoef.tm.lut_y[j] = stoi(out[j]);
		break;
	    case TM_COEF:
                tmp_hdrCoef.tm.coef_r = stoi(out[0]);
                tmp_hdrCoef.tm.coef_g = stoi(out[1]);
                tmp_hdrCoef.tm.coef_b = stoi(out[2]);
		break;
	    case TM_RNGX:
                tmp_hdrCoef.tm.min_x = stoi(out[0]);
                tmp_hdrCoef.tm.max_x = stoi(out[1]);
		break;
	    case TM_RNGY:
                tmp_hdrCoef.tm.min_y = stoi(out[0]);
                tmp_hdrCoef.tm.max_y = stoi(out[1]);
		break;
        }
        out.clear();
    }

    memcpy(&hdrCoefBuf[dest][hdrCoefBuf_n[dest]], &tmp_hdrCoef, sizeof(struct hdrCoef));
    if (dest == HDR)
        hdrCoef_HDR_map.insert(make_pair(make_pair(map1, map2), &hdrCoefBuf[dest][hdrCoefBuf_n[dest]++]));
    else if (dest == HDR10P)
        hdrCoef_HDR10P_map.insert(make_pair(make_pair(map1, map2), &hdrCoefBuf[dest][hdrCoefBuf_n[dest]++]));

    if (Llevel > 1)
        ALOGD("map_input1(%s), map_input2(%s), map1(%d), map2(%d), gm.coef(%d), dest(%d) hdrCoefBuf_n(%d)",
            map_input1.c_str(), map_input2.c_str(), map1, map2,
	    hdrCoefBuf[dest][hdrCoefBuf_n[dest] - 1].gm.coef[0][0],
	    dest, hdrCoefBuf_n[dest] - 1);

    return 0;
}

void hdrImplementation::setLuminanceMinMaxSampling(int min, int max, int sampling)
{
    lum_max = max;
    lum_min = min;
    lum_sampling = sampling;
}

static int getOSVersion(void)
{
    char PROP_OS_VERSION[PROPERTY_VALUE_MAX] = "ro.build.version.release";
    char str_value[PROPERTY_VALUE_MAX] = {0};
    property_get(PROP_OS_VERSION, str_value, "0");
    int OS_Version = stoi(std::string(str_value));
    ALOGD("OS VERSION : %d", OS_Version);
    return OS_Version;
}

void hdrImplementation::init_hdrInterface(
                                    char *docname_mod_default,
                                    char *docname_mod,
                                    const char *docname_default,
                                    const char *docname,
                                    const char (*docname_postfix)[POSTFIX_LEN],
                                    const char *docname_ext,
	                            int &doc_cnt)
{
    for (int i = 0; i < 128; i++) {
	    hdrCoefBuf[doc_cnt][i].en = 0;
	    hdrCoefBuf[doc_cnt][i].mul_en = 0;
	    hdrCoefBuf[doc_cnt][i].oetf.en = 0;
	    hdrCoefBuf[doc_cnt][i].eotf.en = 0;
	    hdrCoefBuf[doc_cnt][i].gm.en = 0;
	    hdrCoefBuf[doc_cnt][i].tm.en = 0;

    }
    hdrCoefBuf_n[doc_cnt] = 0;

    setLogLevel(0);
    setLuminanceMinMaxSampling(1000, 10000, 100);
    this->OS_Version = getOSVersion();

    for (int i = 0; i < (HDR_ID_MAX + 1); i++)
        layer_checked[i] = 0;
    hw_select = HDR_HW_MAX + 1;

    memset(docname_mod, 0, DOC_LEN);
    strncpy(docname_mod, docname, strlen(docname));
    strncpy(docname_mod + strlen(docname) - strlen(docname_ext),
        docname_postfix[doc_cnt], strlen(docname_postfix[doc_cnt]));
    strncpy(docname_mod + strlen(docname) - strlen(docname_ext) + strlen(docname_postfix[doc_cnt]),
        docname_ext, strlen(docname_ext));

    memset(docname_mod_default, 0, DOC_LEN);
    strncpy(docname_mod_default, docname_default, strlen(docname_default));
    strncpy(docname_mod_default + strlen(docname_default) - strlen(docname_ext),
        docname_postfix[doc_cnt], strlen(docname_postfix[doc_cnt]));
    strncpy(docname_mod_default + strlen(docname_default) - strlen(docname_ext) + strlen(docname_postfix[doc_cnt]),
        docname_ext, strlen(docname_ext));
}

hdrImplementation::hdrImplementation(const char *docname)
{
    int doc_cnt = 0;
    const char docname_postfix[NUMC][POSTFIX_LEN] = {"_HDR", "_HDR10P"};
    const char docname_ext[POSTFIX_LEN] = ".xml";
    char docname_mod[DOC_LEN];
#define DEFAULT_HDR_FILE	"/vendor/etc/dqe/HDR_coef_data_default.xml"
    char docname_mod_default[DOC_LEN];
    xmlDocPtr doc;
    xmlNodePtr cur, node;
    xmlChar *key;
    vector<string> stream;
    string map_input1, map_input2;

    char target_name[DOC_LEN] = "\0";
    char docname_t[DOC_LEN] = "\0";
    int target_name_fd;
#define TARGET_NAME_SYSFS_NODE	"/sys/class/dqe/dqe/xml"
#define TARGET_NAME_PATH	"/vendor/etc/dqe/"

    target_name_fd = open(TARGET_NAME_SYSFS_NODE, O_RDONLY);
    if (target_name_fd > 2 && read(target_name_fd, target_name, DOC_LEN)) {
        strncpy(docname_t,
		docname,
		strlen(docname) - strlen(docname_ext) - strlen("default"));
	docname_t[strlen(docname) - strlen(docname_ext) - strlen("default")] = '\0';
        strncpy(docname_t + strlen(docname_t),
		target_name + strlen(TARGET_NAME_PATH),
		strlen(target_name) - strlen(TARGET_NAME_PATH) - 1);
        docname_t[strlen(docname) - strlen(docname_ext) - strlen("default")
		+ strlen(target_name) - strlen(TARGET_NAME_PATH) - 1] = '\0';
        close(target_name_fd);
    } else {
        ALOGD("open(%s) failed", TARGET_NAME_SYSFS_NODE);
	strncpy(docname_t, docname, strlen(docname));
    }
    ALOGD("docname_t : %s", docname_t);

doc_parse_again:

    init_hdrInterface(docname_mod_default, docname_mod,
                    DEFAULT_HDR_FILE, docname_t,
                    docname_postfix, docname_ext,
                    doc_cnt);

    doc = xmlParseFile(docname_mod_default);
    if (doc == NULL) {
        ALOGD("no default document or can not parse document(%s)\n", docname_mod_default);
        doc = xmlParseFile(docname_mod);
        if (doc == NULL) {
            ALOGE("no document or can not parse document(%s)\n", docname_mod);
            return;
	}
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
                    if (!xmlStrcmp(node->name, (const xmlChar*)"HDR_EN")) {
                        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                        stream.push_back((char *)key);
                        xmlFree(key);
                    } else if (!xmlStrcmp(node->name, (const xmlChar*)"EOTF_EN")) {
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
                    } else if (!xmlStrcmp(node->name, (const xmlChar*)"NORMAL_EN")) {
                        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                        stream.push_back((char *)key);
                        xmlFree(key);
                    } else if (!xmlStrcmp(node->name, (const xmlChar*)"NORMAL_COEF")) {
                        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                        stream.push_back((char *)key);
                        xmlFree(key);
                    } else if (!xmlStrcmp(node->name, (const xmlChar*)"NORMAL_SHIFT")) {
                        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                        stream.push_back((char *)key);
                        xmlFree(key);
                    } else if (!xmlStrcmp(node->name, (const xmlChar*)"OETF_EN")) {
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
                    } else if (!xmlStrcmp(node->name, (const xmlChar*)"GM_EN")) {
                        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                        stream.push_back((char *)key);
                        xmlFree(key);
                    } else if (!xmlStrcmp(node->name, (const xmlChar*)"GM")) {
                        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                        stream.push_back((char *)key);
                        xmlFree(key);
                    } else if (!xmlStrcmp(node->name, (const xmlChar*)"GM_OFFSET")) {
                        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                        stream.push_back((char *)key);
                        xmlFree(key);
                    } else if (!xmlStrcmp(node->name, (const xmlChar*)"TM_EN")) {
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
                    }
                    node = node->next;
                }

		map_input1 = (const char*)ids_id;
		map_input2 = (const char*)luminance;

                saveLuminanceMinMax(map_input2);

                if (push_hdr_stream(doc_cnt, stream, map_input1, map_input2)) {
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
                    if (!xmlStrcmp(node->name, (const xmlChar*)"HDR_EN")) {
                        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                        stream.push_back((char *)key);
                        xmlFree(key);
                    } else if (!xmlStrcmp(node->name, (const xmlChar*)"EOTF_EN")) {
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
                    } else if (!xmlStrcmp(node->name, (const xmlChar*)"NORMAL_EN")) {
                        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                        stream.push_back((char *)key);
                        xmlFree(key);
                    } else if (!xmlStrcmp(node->name, (const xmlChar*)"NORMAL_COEF")) {
                        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                        stream.push_back((char *)key);
                        xmlFree(key);
                    } else if (!xmlStrcmp(node->name, (const xmlChar*)"NORMAL_SHIFT")) {
                        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                        stream.push_back((char *)key);
                        xmlFree(key);
                    } else if (!xmlStrcmp(node->name, (const xmlChar*)"OETF_EN")) {
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
                    } else if (!xmlStrcmp(node->name, (const xmlChar*)"GM_EN")) {
                        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                        stream.push_back((char *)key);
                        xmlFree(key);
                    } else if (!xmlStrcmp(node->name, (const xmlChar*)"GM")) {
                        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                        stream.push_back((char *)key);
                        xmlFree(key);
                    } else if (!xmlStrcmp(node->name, (const xmlChar*)"GM_OFFSET")) {
                        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                        stream.push_back((char *)key);
                        xmlFree(key);
                    } else if (!xmlStrcmp(node->name, (const xmlChar*)"TM_EN")) {
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
                    }
                    node = node->next;
                }

		map_input1 = (char*)ids_id;
		map_input2 = (char*)ods_id;
                if (push_hdr_stream(doc_cnt, stream, map_input1, map_input2)) {
                    ALOGE("no space for hdr stream");
                }
                stream.clear();

                xmlFree(ids_id);
                xmlFree(ods_id);
            } else if ((!xmlStrcmp(mode_name, (const xmlChar *)"ExtraTargetInfo"))) {
                node = cur->xmlChildrenNode;
		extra_target_info_valid[doc_cnt] = true;
		std::string luminance;
                while (node != NULL) {
                    if (!xmlStrcmp(node->name, (const xmlChar*)"MIN_LUM")) {
                        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
			luminance = (const char*)key;
			extra_target_info[doc_cnt].min_luminance = (unsigned)stoi(luminance);
                        xmlFree(key);
		    } else if (!xmlStrcmp(node->name, (const xmlChar*)"MAX_LUM")) {
                        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
			luminance = (const char*)key;
			extra_target_info[doc_cnt].max_luminance = (unsigned)stoi(luminance);
                        xmlFree(key);
		    }
                    node = node->next;
                }
	    }
            xmlFree(mode_name);
        }
        cur = cur->next;
    }
    xmlFreeDoc(doc);
    doc_cnt++;

    if (doc_cnt != ARRSIZE(docname_postfix))
	    goto doc_parse_again;
}

hdrImplementation::~hdrImplementation()
{
}

static void printhdrCoef(int layer_index, 
		unsigned int ids, unsigned int ods, unsigned int luminance,
		struct hdrCoef *output)
{
    int i;

    ALOGD("%s +", __func__);
    ALOGD("layer_index : %d, ids: %d, ods: %d, luminance: %d", layer_index, ids, ods, luminance);
    ALOGD("output->en : %d", output->en);
    if (!output->en)
        return;

    ALOGD("output->mul_en : %d", output->mul_en);

    if (output->eotf.en && output->eotf.pq_en) {
        ALOGD("output->pq_en : %d", output->eotf.pq_en);
        ALOGD("output->coef : %d", output->eotf.coef);
        ALOGD("output->shift : %d", output->eotf.shift);
    } else if (output->eotf.en) {
        for (i = 0; i < HDR_EOTF_POSX_LUT_CNT; i+=10) {
            if (i >= (HDR_EOTF_POSX_LUT_CNT - 10)) {
                ALOGD("eotf.lut_x[%11d %11d %11d %11d %11d]",
                    output->eotf.lut_x[i],   output->eotf.lut_x[i+1],
                    output->eotf.lut_x[i+2], output->eotf.lut_x[i+3],
                    output->eotf.lut_x[i+4]);
		break;
	    }
            ALOGD("eotf.lut_x[%11d %11d %11d %11d %11d %11d %11d %11d %11d %11d]",
                output->eotf.lut_x[i],   output->eotf.lut_x[i+1],
                output->eotf.lut_x[i+2], output->eotf.lut_x[i+3],
                output->eotf.lut_x[i+4],
                output->eotf.lut_x[i+5], output->eotf.lut_x[i+6],
                output->eotf.lut_x[i+7], output->eotf.lut_x[i+8],
                output->eotf.lut_x[i+9]);
        }

        for (i = 0; i < HDR_EOTF_POSY_LUT_CNT; i+=10) {
            if (i >= (HDR_EOTF_POSY_LUT_CNT - 10)) {
                ALOGD("eotf.lut_y[%11d %11d %11d %11d %11d]",
                    output->eotf.lut_y[i],   output->eotf.lut_y[i+1],
                    output->eotf.lut_y[i+2], output->eotf.lut_y[i+3],
                    output->eotf.lut_y[i+4]);
		break;
	    }
            ALOGD("eotf.lut_y[%11d %11d %11d %11d %11d %11d %11d %11d %11d %11d]",
                output->eotf.lut_y[i],   output->eotf.lut_y[i+1],
                output->eotf.lut_y[i+2], output->eotf.lut_y[i+3],
                output->eotf.lut_y[i+4],
                output->eotf.lut_y[i+5], output->eotf.lut_y[i+6],
                output->eotf.lut_y[i+7], output->eotf.lut_y[i+8],
                output->eotf.lut_y[i+9]);
        }
    }

    if (output->gm.en) {
        ALOGD("gm.coef[%11d %11d %11d %11d %11d %11d %11d %11d %11d]",
            output->gm.coef[0][0], output->gm.coef[0][1], output->gm.coef[0][2],
	    output->gm.coef[1][0], output->gm.coef[1][1], output->gm.coef[1][2],
            output->gm.coef[2][0], output->gm.coef[2][1], output->gm.coef[2][2]);
        ALOGD("gm.offs[%11d %11d %11d]",
            output->gm.offs[0], output->gm.offs[1], output->gm.offs[2]);
    }

    if (output->tm.en) {
        for (i = 0; i < HDR_TM_POSX_LUT_CNT; i+=10) {
            if (i >= (HDR_TM_POSX_LUT_CNT - 10)) {
                ALOGD("tm.lut_x[%11d %11d %11d]",
                    output->tm.lut_x[i],   output->tm.lut_x[i+1], output->tm.lut_x[i+2]);
		break;
	    }
            ALOGD("tm.lut_x[%11d %11d %11d %11d %11d %11d %11d %11d %11d %11d]",
                output->tm.lut_x[i],   output->tm.lut_x[i+1],
                output->tm.lut_x[i+2], output->tm.lut_x[i+3],
                output->tm.lut_x[i+4], output->tm.lut_x[i+5],
                output->tm.lut_x[i+6], output->tm.lut_x[i+7],
                output->tm.lut_x[i+8], output->tm.lut_x[i+9]);
        }

        for (i = 0; i < HDR_TM_POSY_LUT_CNT; i+=10) {
            if (i >= (HDR_TM_POSY_LUT_CNT - 10)) {
                ALOGD("tm.lut_y[%11d %11d %11d]",
                    output->tm.lut_y[i],   output->tm.lut_y[i+1], output->tm.lut_y[i+2]);
		break;
	    }
            ALOGD("tm.lut_y[%11d %11d %11d %11d %11d %11d %11d %11d %11d %11d]",
                output->tm.lut_y[i],   output->tm.lut_y[i+1],
                output->tm.lut_y[i+2], output->tm.lut_y[i+3],
                output->tm.lut_y[i+4], output->tm.lut_y[i+5],
                output->tm.lut_y[i+6], output->tm.lut_y[i+7],
                output->tm.lut_y[i+8], output->tm.lut_y[i+9]);
        }

        ALOGD("tm.coef[%11d %11d %11d]", output->tm.coef_r, output->tm.coef_g, output->tm.coef_b);
        ALOGD("tm.rngx[%11d %11d]", output->tm.min_x,   output->tm.max_x);
        ALOGD("tm.rngy[%11d %11d]", output->tm.min_y,   output->tm.max_y);
    }

    if (output->oetf.en) {
        for (i = 0; i < HDR_OETF_H_POSX_LUT_CNT; i+=10) {
            if (i >= (HDR_OETF_H_POSX_LUT_CNT - 10)) {
                ALOGD("oetf.lut_x[%11d %11d %11d %11d %11d]",
                    output->oetf.lut_x[i],   output->oetf.lut_x[i+1],
                    output->oetf.lut_x[i+2], output->oetf.lut_x[i+3],
                    output->oetf.lut_x[i+4]);
		    break;
	    }
            ALOGD("oetf.lut_x[%11d %11d %11d %11d %11d %11d %11d %11d %11d %11d]",
                output->oetf.lut_x[i],   output->oetf.lut_x[i+1],
                output->oetf.lut_x[i+2], output->oetf.lut_x[i+3],
                output->oetf.lut_x[i+4], output->oetf.lut_x[i+5],
                output->oetf.lut_x[i+6], output->oetf.lut_x[i+7],
                output->oetf.lut_x[i+8], output->oetf.lut_x[i+9]);
        }

        for (i = 0; i < HDR_OETF_H_POSY_LUT_CNT; i+=10) {
            if (i >= (HDR_OETF_H_POSY_LUT_CNT - 10)) {
                ALOGD("oetf.lut_y[%11d %11d %11d %11d %11d]",
                    output->oetf.lut_y[i],   output->oetf.lut_y[i+1],
                    output->oetf.lut_y[i+2], output->oetf.lut_y[i+3],
                    output->oetf.lut_y[i+4]);
		break;
	    }
            ALOGD("oetf.lut_y[%11d %11d %11d %11d %11d %11d %11d %11d %11d %11d]",
                output->oetf.lut_y[i],   output->oetf.lut_y[i+1],
                output->oetf.lut_y[i+2], output->oetf.lut_y[i+3],
                output->oetf.lut_y[i+4], output->oetf.lut_y[i+5],
                output->oetf.lut_y[i+6], output->oetf.lut_y[i+7],
                output->oetf.lut_y[i+8], output->oetf.lut_y[i+9]);
        }
    }

    ALOGD("%s -", __func__);
}

void hdrImplementation::refineTransfer(android_dataspace &ids) {
    int transfer = ids & HAL_DATASPACE_TRANSFER_MASK;
    int result = ids;
    switch (transfer) {
        case HAL_DATASPACE_TRANSFER_SRGB:
        case HAL_DATASPACE_TRANSFER_LINEAR:
        case HAL_DATASPACE_TRANSFER_ST2084:
        case HAL_DATASPACE_TRANSFER_HLG:
            break;
        case HAL_DATASPACE_TRANSFER_SMPTE_170M:
        case HAL_DATASPACE_TRANSFER_GAMMA2_2:
        case HAL_DATASPACE_TRANSFER_GAMMA2_6:
        case HAL_DATASPACE_TRANSFER_GAMMA2_8:
            if (this->OS_Version > 12)
                break;
            [[fallthrough]];
        default:
            result = result & ~(HAL_DATASPACE_TRANSFER_MASK);
            ids = (android_dataspace)(result | HAL_DATASPACE_TRANSFER_SRGB);
            break;
    }
}

int hdrImplementation::setTargetInfo(struct HdrTargetInfo __attribute__((unused)) *tInfo)
{
    if (tInfo->dataspace == 0)
	    tInfo->dataspace = (int)HAL_DATASPACE_V0_SRGB;
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

#ifndef USE_FULL_ST2094_40_INFO
void ConvertDynamicMeta(ExynosHdrDynamicInfo_t __unused *dyn_meta, ExynosHdrDynamicInfo __unused *dynamic_metadata)
{
    return;
}
#else
void ConvertDynamicMeta(ExynosHdrDynamicInfo_t *dyn_meta, ExynosHdrDynamicInfo *dynamic_metadata)
{
    int i;
    dyn_meta->valid                                     = dynamic_metadata->valid;
    dyn_meta->data.country_code                         = dynamic_metadata->data.country_code;
    dyn_meta->data.provider_code                        = dynamic_metadata->data.provider_code;
    dyn_meta->data.provider_oriented_code               = dynamic_metadata->data.provider_oriented_code;
    dyn_meta->data.application_identifier               = dynamic_metadata->data.application_identifier;
    dyn_meta->data.application_version                  = dynamic_metadata->data.application_version;
    dyn_meta->data.display_maximum_luminance            = dynamic_metadata->data.targeted_system_display_maximum_luminance;
    for (i = 0; i < 3; i++)
        dyn_meta->data.maxscl[i]                        = dynamic_metadata->data.maxscl[0][i];
    for (i = 0; i < 15; i++) {
        dyn_meta->data.maxrgb_percentages[i]            = dynamic_metadata->data.maxrgb_percentages[0][i];
        dyn_meta->data.maxrgb_percentiles[i]            = dynamic_metadata->data.maxrgb_percentiles[0][i];
    }
    dyn_meta->data.tone_mapping.tone_mapping_flag       = dynamic_metadata->data.tone_mapping.tone_mapping_flag[0];
    dyn_meta->data.tone_mapping.knee_point_x            = dynamic_metadata->data.tone_mapping.knee_point_x[0];
    dyn_meta->data.tone_mapping.knee_point_y            = dynamic_metadata->data.tone_mapping.knee_point_y[0];
    dyn_meta->data.tone_mapping.num_bezier_curve_anchors= dynamic_metadata->data.tone_mapping.num_bezier_curve_anchors[0];
    for (i = 0; i < 15; i++)
        dyn_meta->data.tone_mapping.bezier_curve_anchors[i] = dynamic_metadata->data.tone_mapping.bezier_curve_anchors[0][i];
}
#endif

int hdrImplementation::setLayerInfo(int __attribute__((unused)) layer_index, struct HdrLayerInfo __attribute__((unused)) *lInfo)
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
    int dataspace = lInfo->dataspace;
    void *static_metadata = lInfo->static_metadata;
    int static_len = lInfo->static_len;
    void *dynamic_metadata = lInfo->dynamic_metadata;
    int dynamic_len = lInfo->dynamic_len;
    bool premult_alpha = lInfo->premult_alpha;
    HdrBpc bpc = lInfo->bpc;
    RenderSource source = lInfo->source;
    float *tf_matrix = lInfo->tf_matrix;
    bool bypass = lInfo->bypass;

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
            if (sizeof(ExynosHdrDynamicInfo) != sizeof(ExynosHdrDynamicInfo_t)) {
                ExynosHdrDynamicInfo_t dyn_meta;
                ConvertDynamicMeta(&dyn_meta, (ExynosHdrDynamicInfo*)dynamic_metadata);
                memcpy(&LayerInfo[hw_select][layer_index].dynamic_meta, &dyn_meta, sizeof(ExynosHdrDynamicInfo_t));
            } else
                memcpy(&LayerInfo[hw_select][layer_index].dynamic_meta, dynamic_metadata, sizeof(ExynosHdrDynamicInfo_t));
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

static int getWcgCoef(int layer_index, struct hdrCoef *output,
		int in_standard, int in_transfer,
		int out_standard, int out_transfer,
		bool __attribute__((unused)) premult_alpha)
{
    int ret = 0;

	if ((in_transfer >= ARRSIZE(eotfLutMap_H)) || (in_transfer <= 0)) {
	/* default */
        output->eotf.en = 0;
    } else {
        if (layer_index == 1 || layer_index == 3 || layer_index == 7)
            memcpy(&output->eotf, &eotfLutMap_H[in_transfer], sizeof(struct hdr_lut_eotf));
	else
            memcpy(&output->eotf, &eotfLutMap_L[in_transfer], sizeof(struct hdr_lut_eotf));
    }

    if ((in_standard >= ARRSIZE(gmLutMap_H)) || (in_standard <= 0)
        || (out_standard >= ARRSIZE(gmLutMap_H[0])) || (out_standard <= 0)) {
	/* default */
        output->gm.en = 0;
    } else {
        if (layer_index == 1 || layer_index == 3 || layer_index == 7)
            memcpy(&output->gm, &gmLutMap_H[in_standard][out_standard], sizeof(struct hdr_lut_gm));
	else
            memcpy(&output->gm, &gmLutMap_L[in_standard][out_standard], sizeof(struct hdr_lut_gm));
    }

    if ((out_transfer >= ARRSIZE(oetfLutMap_H)) || (out_transfer <= 0)) {
	/* default */
        output->oetf.en = 0;
    } else {
        if (layer_index == 1 || layer_index == 3 || layer_index == 7)
            memcpy(&output->oetf, &oetfLutMap_H[out_transfer], sizeof(struct hdr_lut_oetf));
	else
            memcpy(&output->oetf, &oetfLutMap_L[out_transfer], sizeof(struct hdr_lut_oetf));
    }

    output->tm.en = 0;

    if (output->oetf.en || output->eotf.en || output->gm.en) {
	    output->en = 1;
	    output->mul_en = 0;
    } else {
	    output->en = 0;
	    output->mul_en = 0;
    }

    return ret;
}

#define MAX(x,y)	(((x) > (y)) ? (x) : (y))
static double get_SMPTE_PQ (double N)
{
	double m1 = 0.1593017578125;
	double m2 = 78.84375;
	double c2 = 18.8515625;
	double c3 = 18.6875;
	double c1 = c3 - c2 + 1.0;

	return pow((MAX((pow(N, (1/m2)))-c1, 0)
			/ (c2 - c3 * (pow(N, (1/m2)))))
			,(1/m1));
}

static void getEotfPQ_HDR(int max_nit, struct hdrCoef *output)
{
	int normalize = (int)((get_SMPTE_PQ(924.0/1023.0)
				/ (max_nit/10000.0))
				* 65536);
	int shift = 0;

	for (int exp = 21; exp >= 0; exp--) {
		if (normalize >= pow(2, exp)) {
			shift = 16 - exp;
			break;
		}
	}

	int coef = 0;
	if (shift < 0)
		coef = (normalize >> (0 - shift)) - 65536;
	else
		coef = (normalize << shift) - 65536;
	shift += 16;

	output->eotf.en = 1;
	output->eotf.pq_en = 1;
	output->eotf.coef = coef;
	output->eotf.shift = shift;
}

static void getEotfPQ_HDR10P(int max_nit, struct hdrCoef *output)
{
	int normalize = (int)((get_SMPTE_PQ(924.0/1023.0)
				/ (max_nit/10000.0))
				* 65536);
	int shift = 0;

	for (int exp = 21; exp >= 0; exp--) {
		if (normalize >= pow(2, exp)) {
			shift = 16 - exp;
			break;
		}
	}

	int coef = 0;
	if (shift < 0)
		coef = (normalize >> (0 - shift)) - 65536;
	else
		coef = (normalize << shift) - 65536;
	shift += 16;

	output->eotf.en = 1;
	output->eotf.pq_en = 1;
	output->eotf.coef = coef;
	output->eotf.shift = shift;
}

#define META_JSON_2094_100K_PERCENTILE_DIVIDE    10.0
static float getSrcMaxLuminance(ExynosHdrDynamicInfo_t *dyn_meta)
{
	unsigned int order = dyn_meta->data.num_maxrgb_percentiles;
	double lumNorm = META_JSON_2094_100K_PERCENTILE_DIVIDE;
	float maxscl[3];
	unsigned int percentages[15];
	double percentiles[15];
	float maxLuminance = 0;

        for (unsigned int i = 0; i < 3; i++)
		maxscl[i] = dyn_meta->data.maxscl[i] / lumNorm;
        for (unsigned int i = 0; i < order; i++) {
		percentiles[i] = dyn_meta->data.maxrgb_percentiles[i] / lumNorm;
		percentages[i] = dyn_meta->data.maxrgb_percentages[i];
	}

	for (unsigned int i = 0; i < 3; i++)
		maxLuminance = std::max(maxLuminance, maxscl[i]);

	if ((order > 0) && (percentages[order - 1] == 99))
		maxLuminance = std::min(percentiles[order - 1], static_cast<double> (maxLuminance));

	return maxLuminance;
}

int hdrImplementation::getHdrCoef(
	enum HdrHwId hw_id,
	int layer_index,
        android_dataspace ids, android_dataspace ods,
        int luminance,
	struct hdrCoef *output)
{
    int ret = 0;
    struct hdrCoef *mapped;
    int in_st_tf = 0;
    int in_standard, in_transfer, out_standard, out_transfer;
    int pq_ds = (HAL_DATASPACE_STANDARD_BT2020 | HAL_DATASPACE_TRANSFER_ST2084);
    int hlg_ds = (HAL_DATASPACE_STANDARD_BT2020 | HAL_DATASPACE_TRANSFER_HLG);
    int temp_lum = 0;

    refineTransfer(ids);

    in_st_tf = ids & (HAL_DATASPACE_STANDARD_MASK | HAL_DATASPACE_TRANSFER_MASK);

    if (in_st_tf == pq_ds) {
        if (layer_index != 1 && layer_index != 3 && layer_index != 7) {
            ALOGE("Layer(%d) doesn't support PQ(%d)", layer_index, ids);
            return -1;
	}

	temp_lum = luminance;
	if (temp_lum > 10000) {
		temp_lum = 0;
	}

	do {
            if (Llevel > 0) {
                ALOGD("searching hdr(PQ) coef set by ids(%d), lum(%d), temp_lum(%d)",
                (int)ids, luminance, temp_lum);
            }

	    if (!has_dynamic[hw_id][layer_index])
                mapped = hdrCoef_HDR_map[make_pair((int)ids, (int)temp_lum)];
	    else
                mapped = hdrCoef_HDR10P_map[make_pair((int)ids, (int)temp_lum)];

	    temp_lum += lum_sampling;
	    temp_lum = temp_lum - (temp_lum % lum_sampling);
	} while ((temp_lum <= lum_max) && !mapped);
        if (!mapped) {
            ALOGE("no hdr(PQ) coef set by ids(%d), lum(%d), temp_lum(%d)",
                (int)ids, luminance, temp_lum);
            return -1;
        }

        memcpy(output , mapped, sizeof(struct hdrCoef));

	if (luminance >= 250 && luminance <= 4000)
		getEotfPQ_HDR(luminance, output);

	if (Llevel > 1 && !has_dynamic[hw_id][layer_index])
		printhdrCoef(layer_index, (unsigned int)ids,
				(unsigned int)ods, (unsigned int)luminance, output);
    } else if (in_st_tf == hlg_ds) {
        if (layer_index != 1 && layer_index != 3 && layer_index != 7) {
            ALOGE("Layer(%d) doesn't support HLG(%d)", layer_index, ids);
            return -1;
	}

        temp_lum = luminance;
        luminance = -1;

	if (!has_dynamic[hw_id][layer_index])
            mapped = hdrCoef_HDR_map[make_pair((int)ids, (int)luminance)];
	else
            mapped = hdrCoef_HDR10P_map[make_pair((int)ids, (int)luminance)];
	if (Llevel > 0) {
            ALOGD("searching hdr(hlg) coef set by ids(%d), lum(%d)",
                (int)ids, (int)luminance);
	}
        if (!mapped) {
            ALOGE("no hdr(HLG) coef set by ids(%d), lum(%d), temp_lum(%d)\n",
                (int)ids, luminance, temp_lum);
            return -1;
	}

	memcpy(output , mapped, sizeof(struct hdrCoef));

	if (luminance >= 250 && luminance <= 4000)
		getEotfPQ_HDR(luminance, output);

	if (Llevel > 1 && !has_dynamic[hw_id][layer_index])
		printhdrCoef(layer_index, (unsigned int)ids,
				(unsigned int)ods, (unsigned int)luminance, output);
    } else {
        in_standard = ids & HAL_DATASPACE_STANDARD_MASK;
        in_transfer = ids & HAL_DATASPACE_TRANSFER_MASK;
        out_standard = ods & HAL_DATASPACE_STANDARD_MASK;
        out_transfer = ods & HAL_DATASPACE_TRANSFER_MASK;

        in_standard = in_standard >> HAL_DATASPACE_STANDARD_SHIFT;
        in_transfer = in_transfer >> HAL_DATASPACE_TRANSFER_SHIFT;
        out_standard = out_standard >> HAL_DATASPACE_STANDARD_SHIFT;
        out_transfer = out_transfer >> HAL_DATASPACE_TRANSFER_SHIFT;

	ret = getWcgCoef(layer_index, output,
                    in_standard, in_transfer,
		    out_standard, out_transfer,
		    LayerInfo[hw_select][layer_index].premult_alpha);
	if (ret) {
            ALOGE("no WCG coef set by in_standard(%d), in_transfer(%d), out_standard(%d), out_transfer(%d)\n",
                        in_standard, in_transfer, out_standard, out_transfer);
	    return -1;
	} else {
            if (Llevel > 1)
                printhdrCoef(layer_index, (unsigned int)ids,
		    (unsigned int)ods, (unsigned int)luminance, output);

            return 0;
	}
    }

    return 0;
}

static void printDynamicMeta(ExynosHdrDynamicInfo_t *dyn_meta)
{
	int i;

	ALOGD("%s:+", __func__);
	ALOGD("valid : %u",			dyn_meta->valid);
	ALOGD("country_code : %u", 		dyn_meta->data.country_code);
	ALOGD("provider_code : %u", 		dyn_meta->data.provider_code);
	ALOGD("provider_oriented_code : %u", 	dyn_meta->data.provider_oriented_code);
	ALOGD("application_identifier : %u", 	dyn_meta->data.application_identifier);
	ALOGD("display_max_lum : %u", 		dyn_meta->data.display_maximum_luminance);
	ALOGD("maxscl : [%u %u %u]", 		dyn_meta->data.maxscl[0], dyn_meta->data.maxscl[1], dyn_meta->data.maxscl[2]);
	ALOGD("num_maxrgb_percentiles : %u", 	dyn_meta->data.num_maxrgb_percentiles);
	for (i = 0; i < 15; i+=5) {
		ALOGD("maxrgb_percentages[%4d~%4d] : %5d %5d %5d %5d %5d", i, i + 4,
			dyn_meta->data.maxrgb_percentages[i + 0], dyn_meta->data.maxrgb_percentages[i + 1],
			dyn_meta->data.maxrgb_percentages[i + 2], dyn_meta->data.maxrgb_percentages[i + 3],
			dyn_meta->data.maxrgb_percentages[i + 4]);
	}
	for (i = 0; i < 15; i+=5) {
		ALOGD("maxrgb_percentiles[%4d~%4d] : %5d %5d %5d %5d %5d", i, i + 4,
			dyn_meta->data.maxrgb_percentiles[i + 0], dyn_meta->data.maxrgb_percentiles[i + 1],
			dyn_meta->data.maxrgb_percentiles[i + 2], dyn_meta->data.maxrgb_percentiles[i + 3],
			dyn_meta->data.maxrgb_percentiles[i + 4]);
	}
	ALOGD("tone_mapping_flag : %d", dyn_meta->data.tone_mapping.tone_mapping_flag);
	ALOGD("knee point x : %d", dyn_meta->data.tone_mapping.knee_point_x);
	ALOGD("knee point y : %d", dyn_meta->data.tone_mapping.knee_point_y);
	ALOGD("num bezier curve anchors : %d", dyn_meta->data.tone_mapping.num_bezier_curve_anchors);
	for (i = 0; i < 15; i+=5) {
		ALOGD("bezier_curve_anchors[%4d~%4d] : %5d %5d %5d %5d %5d", i, i + 4,
			dyn_meta->data.tone_mapping.bezier_curve_anchors[i + 0], dyn_meta->data.tone_mapping.bezier_curve_anchors[i + 1],
			dyn_meta->data.tone_mapping.bezier_curve_anchors[i + 2], dyn_meta->data.tone_mapping.bezier_curve_anchors[i + 3],
			dyn_meta->data.tone_mapping.bezier_curve_anchors[i + 4]);
	}
	ALOGD("%s:-", __func__);
}

static int validateDynamicMeta(ExynosHdrDynamicInfo_t *dyn_meta)
{
    int i;
    for (i = 1; i < dyn_meta->data.num_maxrgb_percentiles; i++) {
        if (!dyn_meta->data.maxrgb_percentiles[i]) {
            ALOGE("invalid dynamic meta: num_maxrgb_percentiles(%d) exceed the array(idx:%d, err:%d)",
                dyn_meta->data.num_maxrgb_percentiles, i, dyn_meta->data.maxrgb_percentiles[i]);
            return HDR_ERR_INVAL;
	}
        if (!dyn_meta->data.maxrgb_percentages[i]) {
            ALOGE("invalid dynamic meta: num_maxrgb_percentages(%d) exceed the array(idx:%d, err:%d)",
                dyn_meta->data.num_maxrgb_percentiles, i, dyn_meta->data.maxrgb_percentages[i]);
            return HDR_ERR_INVAL;
	}
    }
    if (dyn_meta->data.tone_mapping.tone_mapping_flag)
    for (i = 1; i < dyn_meta->data.tone_mapping.num_bezier_curve_anchors; i++) {
        if (!dyn_meta->data.tone_mapping.bezier_curve_anchors[i]) {
            ALOGE("invalid dynamic meta: num_bezier_curve_anchors(%d) exceed the array(idx:%d, err:%d)",
                dyn_meta->data.tone_mapping.num_bezier_curve_anchors, i, dyn_meta->data.tone_mapping.bezier_curve_anchors[i]);
            return HDR_ERR_INVAL;
	}
    }
    return 0;
}

int hdrImplementation::getHdr10pCoef(struct hdrCoefParcel *parcel, ExynosHdrDynamicInfo_t *dyn_meta, unsigned int target_max_lum, unsigned int source_max_lum)
{
    if (Llevel > 3) {
	ALOGD("%s:parcel(0x%lx), dyn_meta(0x%lx)",
		__func__, (unsigned long)parcel, (unsigned long)dyn_meta);
	printDynamicMeta(dyn_meta);
    }

    meta2lut_dtm(target_max_lum, source_max_lum, dyn_meta, 
            ((struct hdrCoef*)parcel->hdrCoef)->tm.lut_x,
            ((struct hdrCoef*)parcel->hdrCoef)->tm.lut_y,
            HDR_TM_POSX_LUT_CNT);

    ((struct hdrCoef*)parcel->hdrCoef)->tm.lut_y[0] = ((struct hdrCoef*)parcel->hdrCoef)->tm.lut_y[1];

    return 0;
}

int hdrImplementation::getHdrCoefData(
		enum HdrHwId __attribute__((unused)) hw_id,
		int __attribute__((unused)) layer_index,
                struct hdrCoefParcel __attribute__((unused)) *parcel)
{
    int ids, ods, luminance;
    float luminance_dyn;
    unsigned int min_lum, max_lum;

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
        luminance = (LayerInfo[hw_id][layer_index].static_meta.sType1.mMaxDisplayLuminance) / 10000;
    else {
	if (Llevel > 1)
	    ALOGD("%s:no static meta data for the layer(%d) hw_id(%d)",
			    __func__, layer_index, hw_id);
        luminance = lum_max;
    }
    ods = target_info.dataspace;

    if (parcel->hdrCoef == NULL)
        return HDR_ERR_INVAL;

    if (has_dynamic[hw_id][layer_index]) {
        if (validateDynamicMeta(&LayerInfo[hw_id][layer_index].dynamic_meta)) {
	    printDynamicMeta(&LayerInfo[hw_id][layer_index].dynamic_meta);
            return HDR_ERR_INVAL;
        }

	if (extra_target_info_valid[HDR10P] == true) {
	    min_lum = extra_target_info[HDR10P].min_luminance;
	    max_lum = extra_target_info[HDR10P].max_luminance;
	} else {
	    min_lum = target_info.min_luminance;
	    max_lum = target_info.max_luminance;
	}

	printDynamicMeta(&LayerInfo[hw_id][layer_index].dynamic_meta);
        luminance_dyn = getSrcMaxLuminance(&LayerInfo[hw_id][layer_index].dynamic_meta);
        luminance = ((int)luminance_dyn > (int)max_lum) ? (int)luminance_dyn : (int)max_lum;
    } else {
	if (extra_target_info_valid[HDR] == true) {
	    min_lum = extra_target_info[HDR].min_luminance;
	    max_lum = extra_target_info[HDR].max_luminance;
	} else {
	    min_lum = target_info.min_luminance;
	    max_lum = target_info.max_luminance;
	}
    }

    if (getHdrCoef(hw_id, layer_index, (android_dataspace)ids, (android_dataspace)ods,
		luminance, (struct hdrCoef*)parcel->hdrCoef))
        return HDR_ERR_INVAL;

    if (has_dynamic[hw_id][layer_index]) {
        if (Llevel > 1)
	    ALOGD("luminance(%d) luminance_dyn(%d) target_max_lum(%d) target_min_lum(%d)",
				luminance, (int)luminance_dyn, max_lum, min_lum);
	getEotfPQ_HDR10P(luminance, (struct hdrCoef*)parcel->hdrCoef);

        if(getHdr10pCoef(parcel, &LayerInfo[hw_id][layer_index].dynamic_meta, max_lum, luminance) != 0)
            return HDR_ERR_INVAL;

	if (Llevel > 1)
            printhdrCoef(layer_index, (unsigned int)ids,
		(unsigned int)ods, (unsigned int)luminance, (struct hdrCoef*)parcel->hdrCoef);
    }

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

hdrInterface *hdrInterface::createInstance(void) {
    const char *docname = "/vendor/etc/dqe/HDR_coef_data_default.xml";
    return new hdrImplementation(docname);
}

