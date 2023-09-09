#ifndef EXYNOS_DQE_INTERFACE_H__
#define EXYNOS_DQE_INTERFACE_H__

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <utils/Log.h>
#include <system/graphics.h>
#include <string.h>

struct dqeCoef {
    unsigned int aps_en;
    unsigned int aps_lut[24];
    unsigned int hsc_en;
    unsigned int hsc_lut[154];
    unsigned int gamma_en;
    unsigned int gamma_lut[3][65];
    unsigned int cgc_en;
    unsigned int cgc_lut[143];
};

class dqeInterface {
private:
    int log_level;
    int lum_max;
    int lum_min;
    int lum_sampling;
    struct dqeCoef g_dqeCoef[256];
    int dqeCoefBuf_n;
    std::map<std::pair<int, int>, struct dqeCoef*> dqeCoef_map;
    int push_dqe_stream(std::vector<std::string> &stream,
		    std::string &map_input1, std::string &map_input2,
		    int i_cgc, int i_hsc, int i_gamma, int i_aps);
    void saveLuminanceMinMax(std::string &luminance);
    void init_dqeInterface(void);
public:
    dqeInterface(const char *docname);
    ~dqeInterface();
    int getDqeCoef(struct dqeCoef *output);
    int getDqeCoef(android_dataspace ids, android_dataspace ods,
            int luminance, struct dqeCoef *output);
    void setLuminanceMinMaxSampling(int min, int max, int sampling);
    void setLogLevel(int level);
};
#endif
