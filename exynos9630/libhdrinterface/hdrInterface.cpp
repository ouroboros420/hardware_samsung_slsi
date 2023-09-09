#include <hardware/exynos/hdrInterface.h>
#include <libxml/parser.h>
#include <libxml/xmlmemory.h>

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

int hdrInterface::push_hdr_stream(std::vector<std::string> &stream, std::string &map_input1, std::string &map_input2)
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

    if (log_level > 1)
        ALOGD("map_input1(%s), map_input2(%s), map1(%d), map2(%d), gm_coef(%d)",
            map_input1.c_str(), map_input2.c_str(), map1, map2, hdrCoefBuf[hdrCoefBuf_n - 1].gm_coef[7]);

    return 0;
}

void hdrInterface::init_hdrInterface(void)
{
    for (int i = 0; i < 1024; i++) {
	    hdrCoefBuf[i].hdr_en = 0;
	    hdrCoefBuf[i].oetf_en = 0;
	    hdrCoefBuf[i].eotf_en = 0;
	    hdrCoefBuf[i].gm_en = 0;
	    hdrCoefBuf[i].tm_en = 0;
    }

    setLogLevel(0);
    setLuminanceMinMaxSampling(1000, 4000, 100);
    hdrCoefBuf_n = 0;
}

hdrInterface::hdrInterface(const char *docname)
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

hdrInterface::~hdrInterface()
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

int hdrInterface::getHdrCoef(android_dataspace ids, android_dataspace ods,
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
            if (log_level > 0) {
                ALOGD("searching hdr(PQ) coef set by ids(%d), lum(%d), temp_lum(%d)\n",
                (int)ids, luminance, temp_lum);
            }
            mapped = hdrCoef_map[make_pair((int)ids, (int)temp_lum)];
	    temp_lum += lum_sampling;
	    temp_lum = temp_lum - (temp_lum % lum_sampling);
	} while ((temp_lum <= lum_max) && !mapped);
        if (!mapped) {
            ALOGE("no hdr(PQ) coef set by ids(%d), lum(%d), temp_lum(%d)\n",
                (int)ids, luminance, temp_lum);
            return -1;
        }
    } else if (in_st_tf == hlg_ds) {
        temp_lum = luminance;
        luminance = -1;
        mapped = hdrCoef_map[make_pair((int)ids, (int)luminance)];
	if (log_level > 0) {
            ALOGD("searching hdr(hlg) coef set by ids(%d), lum(%d)\n",
                (int)ids, (int)luminance);
	}
        if (!mapped) {
            ALOGE("no hdr(HLG) coef set by ids(%d), lum(%d), temp_lum(%d)\n",
                (int)ids, luminance, temp_lum);
            return -1;
	}
    } else {
        mapped = hdrCoef_map[make_pair((int)ids, (int)ods)];
	if (log_level > 0) {
            ALOGD("searching wcg coef set by ids(%d), ods(%d)\n",
                (int)ids, (int)ods);
	}
        if (!mapped) {
	    if (log_level > 0) {
                ALOGE("no wcg coef set by ids(%d), ods(%d)\n",
                    (int)ids, (int)ods);
	    }
            return -1;
	}
    }

    memcpy(output , mapped, sizeof(struct hdrCoef));

    if (log_level > 1)
        printhdrCoef(output);

    return 0;
}

int hdrInterface::get_one_HdrCoef(android_dataspace ids, android_dataspace ods,
        int luminance, struct hdrCoef *output)
{
    struct hdrCoef *mapped;
    int in_st_tf = ids & (HAL_DATASPACE_STANDARD_MASK | HAL_DATASPACE_TRANSFER_MASK);
    int pq_ds = (HAL_DATASPACE_STANDARD_BT2020 | HAL_DATASPACE_TRANSFER_ST2084);
    int hlg_ds = (HAL_DATASPACE_STANDARD_BT2020 | HAL_DATASPACE_TRANSFER_HLG);
    int temp_lum = 0;

    if (in_st_tf == pq_ds) {
	temp_lum = luminance;
	if (temp_lum > 10000) {
		temp_lum = 0;
	}

	do {
            if (log_level > 0) {
                ALOGD("searching hdr(PQ) coef set by ids(%d), lum(%d), temp_lum(%d)\n",
                (int)ids, luminance, temp_lum);
            }
            mapped = hdrCoef_map[make_pair((int)ids, (int)temp_lum)];
	    temp_lum += lum_sampling;
	    temp_lum = temp_lum - (temp_lum % lum_sampling);
	} while ((temp_lum <= lum_max) && !mapped);
        if (!mapped) {
            ALOGE("no hdr(PQ) coef set by ids(%d), lum(%d), temp_lum(%d)\n",
                (int)ids, luminance, temp_lum);
            return -1;
        }
    } else if (in_st_tf == hlg_ds) {
        temp_lum = luminance;
        luminance = -1;
        mapped = hdrCoef_map[make_pair((int)ids, (int)luminance)];
        if (log_level > 0) {
                ALOGD("searching hdr(hlg) coef set by ids(%d), lum(%d)\n",
				ids, luminance);
	}
        if (!mapped) {
            ALOGE("no hdr(HLG) coef set by ids(%d), lum(%d), temp_lum(%d)\n",
                (int)ids, luminance, temp_lum);
            return -1;
	}
    } else {
        mapped = hdrCoef_map[make_pair((int)ids, (int)ods)];
        if (log_level > 0) {
                ALOGD("searching wcg coef set by ids(%d), ods(%d)\n",
				ids, ods);
	}
        if (!mapped) {
	    if (log_level > 0) {
                ALOGE("no wcg coef set by ids(%d), ods(%d)\n",
				(int)ids, (int)ods);
	    }
            return -1;
	}
    }

    memcpy(output , mapped, sizeof(struct hdrCoef));

    if (log_level > 1)
        printhdrCoef(output);

    return 0;
}

void hdrInterface::print_resource_map(int res_map[4])
{
    int i, j;
    char debugging[256];
    char debug_cat[256];
    for (i = 0; i < 4; i++) {
        sprintf(debugging, "res_map[%d]:", i);
        for (j = 0; j < 16; j++) {
            if (res_map[i] & (1 << j)) {
		    sprintf(debug_cat, "[%d]", j);
		    strcat(debugging, debug_cat);
	    }
        }
        ALOGD("%s\n", debugging);
    }
}

void hdrInterface::print_ds_dump(android_dataspace ids[], int mastering_luminance[], int n_layer,
            android_dataspace ods, int peak_luminance)
{
    int i;

    ALOGD("n_layer : %d\n", n_layer);
    for (i = 0; i < n_layer; i++) {
        ALOGD("ids[%2d] = origin:st_tf:lum(%10d:%10d:%5d)\n", i,
                (int)ids[i], (int)(ids[i] & (HAL_DATASPACE_STANDARD_MASK | HAL_DATASPACE_TRANSFER_MASK)), mastering_luminance[i]);
    }
    ALOGD("ods = %10d, peak_luminance = %5d\n", (int)ods, peak_luminance);
}

int hdrInterface::getHdrCoef(android_dataspace ids[], int mastering_luminance[], int n_layer,
            android_dataspace ods, int peak_luminance,
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
	    if (log_level > 1) {
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
	    if (log_level > 1) {
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
        if (log_level > 0) {
            print_resource_map(res_map);
            print_ds_dump(ids, mastering_luminance, n_layer, ods, peak_luminance);
        }
        return -2;
    }

    if (map_index == 0) {
        if ((ids_map[map_index] == HAL_DATASPACE_UNKNOWN) || (ids_map[map_index] == ods)) {
            if (log_level > 0)
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
	    if (log_level > 0)
                ALOGD("wcg coef(unknown) set by ids(%d), ods(%d)\n", (int)ids_map[i], (int)ods);
	} else if (get_one_HdrCoef(ids_map[i], ods, lum_map[i], &output[i]))
            return -3;
    }

    if (log_level > 0)
	print_resource_map(res_map);

    return 0;
}

void hdrInterface::setLuminanceMinMaxSampling(int min, int max, int sampling)
{
    lum_max = max;
    lum_min = min;
    lum_sampling = sampling;
}

void hdrInterface::setLogLevel(int level)
{
    log_level = level;
}

void hdrInterface::saveLuminanceMinMax(std::string &luminance)
{
	int lum = stoi(luminance);

	if (lum < lum_min) {
		lum_min = lum;
	}

	if (lum > lum_max) {
		lum_max = lum;
	}
}
