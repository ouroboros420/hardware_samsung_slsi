/*
 * Copyright Samsung Electronics Co.,LTD.
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef FIMG2D_COMPROMISE_TABLE
#define FIMG2D_COMPROMISE_TABLE
#include <stdlib.h>
#include <stdio.h>

/* int comp_value[src][dst][scale][filter_mode][blending_mode]
 * [src]
 *  0 : kRGB_565_Config
 *  1 : kARGB_4444_Config
 *  2 : kARGB_8888_Config
 *  3 : kNo_Config
 *  [dst]
 *  0 : kRGB_565_Config
 *  1 : kARGB_4444_Config
 *  2 : kARGB_8888_Config
 *  [scale]
 *  0 : No scaling
 *  1 : Scaling_up
 *  2 : Scaling_down
 *  [filter_mode]
 *  0 : nearest
 *  1 : bilinear
 *  [blending_mode]
 *  0 : SRC
 *  1 : SRC_OVER
 */
#define FIMGAPI_FREQLEVELING_USE
#define MIN_LEVEL 4

#undef FIMGAPI_HAL_FREQLEVELING
#define FIMGAPI_HAL_COMPROMISE

unsigned int fimg_standard_size[MIN_LEVEL+1] = {
    1559*877,
    1329*747,
    1131*636,
    1091*614,
    937*532
};

int comp_value[3][2][3][2][2] = {
//rgb565 to rgb565
//{      nearest      }, {      bilinear      }
//{   SRC  , SRC_OVER }, {   SRC   , SRC_OVER }
{{
{{ 958* 540,2560*1440 }, { 887* 497, 922* 519 }},  //No scaling
{{ 960* 544, 438* 246 }, { 788* 440, 464* 259 }},  //Scaling up
{{ 960* 544, 374* 211 }, { 682* 383, 464* 259 }}}, //Scaling down
//rgb565 to argb8888
{
{{ 679* 381, 221* 124 }, { 689* 388, 203* 114 }},  //No Scaling
{{ 703* 396, 225* 125 }, { 542* 304, 183* 101 }},  //Scaling up
{{ 702* 396, 224* 123 }, { 437* 245, 143*  81 }}}  //Scaling down
},
//argb8888 to rgb565
{{
{{ 320* 180, 343* 193 }, { 153*  93, 357* 201 }},  //No Scaling
{{ 256* 144, 282* 159 }, { 133*  72, 143*  81 }},  //Scaling up
{{ 158*  93, 243* 137 }, { 114*  64, 122*  69 }}}, //Scaling down
//argb8888 to argb8888
{
{{1504* 846,1488* 837 }, { 688* 387, 672* 378 }},  //No Scaling
{{ 896* 504, 512* 288 }, { 656* 362, 272* 153 }},  //Scaling up
{{ 896* 504, 514* 289 }, { 656* 362, 256* 144 }}}  //Scaling down
},
//No Src to rgb565 (Not measured yet)
{{
{{ 164*  94, 256* 124 }, { 512* 288, 400* 225 }},  //No Scaling
{{ 164*  94, 256* 124 }, { 512* 288, 400* 225 }},  //Scaling up
{{ 164*  94, 256* 124 }, { 512* 288, 400* 225 }}}, //Scaling down
//No src to argb8888 (Fill to argb8888)
{
{{2560*1440,2560*1440 }, {2560*1440,2560*1440 }},  //No Scaling
{{2560*1440,2560*1440 }, {2560*1440,2560*1440 }},  //Scaling up
{{2560*1440,2560*1440 }, {2560*1440,2560*1440 }}}  //Scaling down
}
};
#endif
