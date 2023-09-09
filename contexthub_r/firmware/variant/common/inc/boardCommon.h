/*----------------------------------------------------------------------------
 *      Exynos  -  BOARD INIT
 *----------------------------------------------------------------------------
 *      Name:    boardInit.h
 *      Purpose: To expose Board Initialize APIs
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

#ifndef __BOARD_COMMON_H__
#define __BOARD_COMMON_H__

#include <csp_common.h>
#include <boardInit.h>
#include <usi.h>
#include <cmgp.h>

typedef struct {
    uint32_t port;
    UsiProtocolType protocol;
}UsiBoardConfigType;

typedef struct {
    uint32_t intNum;
    uint32_t priority;
}CmgpIrqBoardConfigType;

#endif

