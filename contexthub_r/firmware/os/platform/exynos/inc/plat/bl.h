/*
 * Copyright (C) 2016 The Android Open Source Project
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

#ifndef _BOOTLOADER_H_
#define _BOOTLOADER_H_

#ifdef __cplusplus
extern "C" {
#endif

#define BL_FLASH_KEY1       0x424C4558  //BLEX
#define BL_FLASH_KEY2       0x594E4F53  //YNOS

struct Sha2state;
struct RsaState;
struct AesContext;
struct AesSetupTempWorksSpace;
struct AesCbcContext;

#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>
#include <csp_common.h>

#include CSP_HEADER(bl)

#define OS_UPDT_SUCCESS                0
#define OS_UPDT_HDR_CHECK_FAILED       1
#define OS_UPDT_HDR_MARKER_INVALID     2
#define OS_UPDT_UNKNOWN_PUBKEY         3
#define OS_UPDT_INVALID_SIGNATURE      4
#define OS_UPDT_INVALID_SIGNATURE_HASH 5

#ifndef BL_STACK_SIZE
  #ifdef SUPPORT_EXT_APP
    #define BL_STACK_SIZE 4096
  #else
    #define BL_STACK_SIZE 512
  #endif
#endif

#ifdef __cplusplus
}
#endif

#endif
