/*----------------------------------------------------------------------------
 *      Exynos SoC  -  CSP
 *----------------------------------------------------------------------------
 *      Name:    csp_assert.h
 *      Purpose: To define assert macro
 *      Rev.:    V1.00
 *----------------------------------------------------------------------------
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __CSP_ASSERT_H__
#define __CSP_ASSERT_H__

#include "csp_os.h"

#define CSP_ASSERT(test)\
    if (!(test))	\
    {			\
        CSP_PRINTF_ERROR("[assert]%s, %d\n", __func__, __LINE__); \
        CSP_PRINTF_ERROR("%s\n", __FILE__); \
        cspNotify(ERR_ASSERT);	\
    }

#define CSP_FAULT(test)\
    if (!(test))	\
    {			\
        CSP_PRINTF_ERROR("[fault]%s, %d\n", __func__, __LINE__); \
        CSP_PRINTF_ERROR("%s\n", __FILE__); \
        cspNotify(ERR_FAULT);	\
    }

#define CSP_ERROR(test)\
    if (!(test))	\
    {			\
        CSP_PRINTF_ERROR("[error]%s, %d\n", __func__, __LINE__); \
        CSP_PRINTF_ERROR("%s\n", __FILE__); \
    }

#define CSP_REBOOT(test)\
    if (!(test))	\
    {			\
        CSP_PRINTF_ERROR("[fault]%s, %d\n", __func__, __LINE__); \
        CSP_PRINTF_ERROR("%s\n", __FILE__); \
        cspNotify(ERR_REBOOT);	\
    }

#define CSP_PANIC(test)\
    if (!(test))	\
    {			\
        CSP_PRINTF_ERROR("[panic]%s, %d\n", __func__, __LINE__); \
        CSP_PRINTF_ERROR("%s\n", __FILE__); \
        cspNotify(ERR_PANIC);	\
    }

#endif
