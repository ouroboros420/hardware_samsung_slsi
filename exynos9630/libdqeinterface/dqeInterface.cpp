#include "include/hardware/exynos/dqeInterface.h"
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

int dqeInterface::push_dqe_stream(std::vector<std::string> &stream,
                    std::string &map_input1, std::string &map_input2,
		    int i_cgc, int i_hsc, int i_gamma, int i_aps)
{
    vector<string> out;
    int i, j, k, l;
    int map1, map2;

    map1 = stoi(map_input1);
    map2 = stoi(map_input2);

    out.clear();
    if (i_cgc != -1) {
        split(stream[i_cgc], ',', out);
        if (out.size() != 143) {
            ALOGE("number of CGC parameters not right (parsed = %d, should-be 143)",
                    (int)out.size());
            return -1;
        }
	g_dqeCoef[dqeCoefBuf_n].cgc_en = 1;
        for (i = 0; i < 143; i++) {
            g_dqeCoef[dqeCoefBuf_n].cgc_lut[i] = stoi(out[i]);
        }
        out.clear();
    }

    if (i_hsc != -1) {
        split(stream[i_hsc], ',', out);
        if (out.size() != 154) {
            ALOGE("number of HSC parameters not right (parsed = %d, should-be 154)",
                    (int)out.size());
            return -1;
        }
	g_dqeCoef[dqeCoefBuf_n].hsc_en = 1;
        for (i = 0; i < 154; i++) {
            g_dqeCoef[dqeCoefBuf_n].hsc_lut[i] = stoi(out[i]);
        }
        out.clear();
    }

    if (i_gamma != -1) {
        split(stream[i_gamma], ',', out);
        if (out.size() != 195) {
            ALOGE("number of GAMMA parameters not right (parsed = %d, should-be 195)",
                    (int)out.size());
            return -1;
        }
	g_dqeCoef[dqeCoefBuf_n].gamma_en = 1;
        for (i = 0, l = 0; i < 3; i++) {
            //k = (i == 2) ? 64 : 65;
            k = 65;
            for (j = 0; j < k; j++) {
                g_dqeCoef[dqeCoefBuf_n].gamma_lut[i][j] = stoi(out[l]);
                l++;
            }
        }
        out.clear();
    }

    if (i_aps != -1) {
        split(stream[i_aps], ',', out);
        if (out.size() != 24) {
            ALOGE("number of APS parameters not right (parsed = %d, should-be 24)",
                    (int)out.size());
            return -1;
        }
	g_dqeCoef[dqeCoefBuf_n].aps_en = 1;
        for (i = 0; i < 24; i++) {
            g_dqeCoef[dqeCoefBuf_n].aps_lut[i] = stoi(out[i]);
        }
        out.clear();
    }

    if (log_level > 0)
        ALOGD("map_input1(%s), map_input2(%s), map1(%d), map2(%d), "
		"cgc_en(%d), hsc_en(%d), gamma_en(%d), aps_en(%d), "
		"dqeCeofBuf_n(%d)",
            map_input1.c_str(), map_input2.c_str(), map1, map2,
	    g_dqeCoef[dqeCoefBuf_n].cgc_en, g_dqeCoef[dqeCoefBuf_n].hsc_en,
	    g_dqeCoef[dqeCoefBuf_n].gamma_en, g_dqeCoef[dqeCoefBuf_n].aps_en,
	    dqeCoefBuf_n);

    dqeCoef_map.insert(make_pair(make_pair(map1, map2), &g_dqeCoef[dqeCoefBuf_n++]));

    return 0;
}

void dqeInterface::init_dqeInterface(void)
{
    int i;

    for (i = 0; i < 256; i++) {
	    g_dqeCoef[i].cgc_en = 0;
	    g_dqeCoef[i].hsc_en = 0;
	    g_dqeCoef[i].gamma_en = 0;
	    g_dqeCoef[i].aps_en = 0;
    }
}

dqeInterface::dqeInterface(const char *docname)
{
    xmlDocPtr doc;
    xmlNodePtr cur, node;
    xmlChar *key;
    vector<string> stream;
    string map_input1, map_input2;
    int i;
    int i_cgc, i_hsc, i_gamma, i_aps;

    init_dqeInterface();

    setLogLevel(0);
    setLuminanceMinMaxSampling(1000, 4000, 100);
    dqeCoefBuf_n = 0;

    doc = xmlParseFile(docname);
    if (doc == NULL) {
        ALOGE("can not parse document(%s)\n", docname);
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
            xmlChar *mode_name, *ids_id, *luminance;

            mode_name = xmlGetProp(cur, (const xmlChar *)"name");

            if(!xmlStrcmp(mode_name, (const xmlChar *)"HDR_DQE")) {
                i = 0;
		i_cgc = -1;
		i_hsc = -1;
		i_gamma = -1;
		i_aps = -1;

                ids_id = xmlGetProp(cur, (const xmlChar *)"ids_id");
                luminance = xmlGetProp(cur, (const xmlChar *)"luminance");

                node = cur->xmlChildrenNode;

                while (node != NULL) {
                    if ((!xmlStrcmp(node->name, (const xmlChar *)"cgc"))) {
                        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                        stream.push_back((char *)key);
                        xmlFree(key);
                        i_cgc = i++;
                    }

                    if ((!xmlStrcmp(node->name, (const xmlChar *)"gamma"))) {
                        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                        stream.push_back((char *)key);
                        xmlFree(key);
                        i_gamma = i++;
                    }

                    if ((!xmlStrcmp(node->name, (const xmlChar *)"hsc"))) {
                        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                        stream.push_back((char *)key);
                        xmlFree(key);
                        i_hsc = i++;
                    }

                    if ((!xmlStrcmp(node->name, (const xmlChar *)"aps"))) {
                        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                        stream.push_back((char *)key);
                        xmlFree(key);
                        i_aps = i++;
                    }

                    node = node->next;
                }

		map_input1 = (const char*)ids_id;
		map_input2 = (const char*)luminance;

                saveLuminanceMinMax(map_input2);

                if (push_dqe_stream(stream, map_input1, map_input2,
				i_cgc, i_hsc, i_gamma, i_aps)) {
                    ALOGE("no space for dqe stream");
                }
                stream.clear();

                xmlFree(ids_id);
                xmlFree(luminance);
            }
            xmlFree(mode_name);
        }
        cur = cur->next;
    }
    xmlFreeDoc(doc);
}

dqeInterface::~dqeInterface()
{
}

void printDqeCoef(struct dqeCoef *output)
{
    int i, j;

    if (output->aps_en) {
        for (i = 0; i < 20; i+=10) {
            ALOGD("aps[%7d %7d %7d %7d %7d %7d %7d %7d %7d %7d]",
                output->aps_lut[i], output->aps_lut[i+1],
                output->aps_lut[i+2], output->aps_lut[i+3],
                output->aps_lut[i+4], output->aps_lut[i+5],
                output->aps_lut[i+6], output->aps_lut[i+7],
                output->aps_lut[i+8], output->aps_lut[i+9]);
        }
        ALOGD("aps[%7d %7d %7d %7d]",
            output->aps_lut[i], output->aps_lut[i+1],
            output->aps_lut[i+2], output->aps_lut[i+3]);
    }

    if (output->hsc_en) {
        for (i = 0; i < 150; i+=10) {
            ALOGD("hsc[%7d %7d %7d %7d %7d %7d %7d %7d %7d %7d]",
                output->hsc_lut[i],   output->hsc_lut[i+1],
                output->hsc_lut[i+2], output->hsc_lut[i+3],
                output->hsc_lut[i+4], output->hsc_lut[i+5],
                output->hsc_lut[i+6], output->hsc_lut[i+7],
                output->hsc_lut[i+8], output->hsc_lut[i+9]);
        }
        ALOGD("hsc[%7d %7d %7d %7d]",
            output->hsc_lut[i],   output->hsc_lut[i+1],
            output->hsc_lut[i+2], output->hsc_lut[i+3]);
    }

    if (output->gamma_en) {
        for (i = 0; i < 3; i++) {
            for (j = 0; j < 60; j+=10) {
                ALOGD("gamma[%d][%7d %7d %7d %7d %7d %7d %7d %7d %7d %7d]", i,
                    output->gamma_lut[i][j],   output->gamma_lut[i][j+1],
                    output->gamma_lut[i][j+2], output->gamma_lut[i][j+3],
                    output->gamma_lut[i][j+4], output->gamma_lut[i][j+5],
                    output->gamma_lut[i][j+6], output->gamma_lut[i][j+7],
                    output->gamma_lut[i][j+8], output->gamma_lut[i][j+9]);
            }
            ALOGD("gamma[%d][%7d %7d %7d %7d %7d]", i,
                output->gamma_lut[i][j],   output->gamma_lut[i][j+1],
                output->gamma_lut[i][j+2], output->gamma_lut[i][j+3],
                output->gamma_lut[i][j+4]);
        }
    }

    if (output->cgc_en) {
        for (i = 0; i < 140; i+=10) {
            ALOGD("cgc[%7d %7d %7d %7d %7d %7d %7d %7d %7d %7d]",
                output->cgc_lut[i],   output->cgc_lut[i+1],
                output->cgc_lut[i+2], output->cgc_lut[i+3],
                output->cgc_lut[i+4], output->cgc_lut[i+5],
                output->cgc_lut[i+6], output->cgc_lut[i+7],
                output->cgc_lut[i+8], output->cgc_lut[i+9]);
        }
        ALOGD("cgc[%7d %7d %7d]",
            output->cgc_lut[i],   output->cgc_lut[i+1],
            output->cgc_lut[i+2]);
    }
}

int dqeInterface::getDqeCoef(android_dataspace ids, android_dataspace ods,
        int luminance, struct dqeCoef *output)
{
    struct dqeCoef *mapped;
    int in_st_tf = ids & (HAL_DATASPACE_STANDARD_MASK | HAL_DATASPACE_TRANSFER_MASK);
    int pq_ds = (HAL_DATASPACE_STANDARD_BT2020 | HAL_DATASPACE_TRANSFER_ST2084);
    int hlg_ds = (HAL_DATASPACE_STANDARD_BT2020 | HAL_DATASPACE_TRANSFER_HLG);
    int temp_lum = 0;

    if (in_st_tf == pq_ds) {
	temp_lum = luminance;
	if (temp_lum > 10000)
		temp_lum = 0;
	do {
            if (log_level > 0) {
                ALOGD("searching dqe(PQ) coef set by ids(%d), lum(%d), temp_lum(%d)\n",
                (int)ids, luminance, temp_lum);
            }
            mapped = dqeCoef_map[make_pair((int)ids, (int)temp_lum)];
	    temp_lum += lum_sampling;
	    temp_lum = temp_lum - (temp_lum % lum_sampling);
	} while ((temp_lum <= lum_max) && !mapped);
        if (!mapped) {
            ALOGE("no dqe(PQ) coef set by ids(%d), lum(%d), temp_lum(%d)\n",
                (int)ids, luminance, temp_lum);
            return -1;
        }
    } else if (in_st_tf == hlg_ds) {
        temp_lum = luminance;
        luminance = -1;
        mapped = dqeCoef_map[make_pair((int)ids, (int)luminance)];
        if (!mapped) {
            ALOGE("no dqe(HLG) coef set by ids(%d), lum(%d), temp_lum(%d)\n",
                (int)ids, luminance, temp_lum);
            return -1;
	}
    } else {
        if (log_level > 2) {
            ALOGE("no dqe coef set by ids(%d), ods(%d), lum(%d), temp_lum(%d)\n",
                (int)ids, (int)ods, luminance, temp_lum);
	}
	return 0;
    }

    memcpy(output , mapped, sizeof(struct dqeCoef));

    if(log_level > 2)
        printDqeCoef(output);

    return 0;
}

int dqeInterface::getDqeCoef(struct dqeCoef *output)
{
    memcpy((void*)output, (void*)&g_dqeCoef[0], sizeof(struct dqeCoef));

    if(log_level > 2)
        printDqeCoef(output);

    return 0;
}

void dqeInterface::setLuminanceMinMaxSampling(int min, int max, int sampling)
{
    lum_max = max;
    lum_min = min;
    lum_sampling = sampling;
}

void dqeInterface::setLogLevel(int level)
{
    log_level = level;
}

void dqeInterface::saveLuminanceMinMax(std::string &luminance)
{
	int lum = stoi(luminance);

	if (lum < lum_min) {
		lum_min = lum;
	}

	if (lum > lum_max) {
		lum_max = lum;
	}
}
