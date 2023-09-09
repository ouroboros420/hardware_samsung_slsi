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

#ifndef _EXYNOS_PLAT_H_
#define _EXYNOS_PLAT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <seos.h>

void SysTick_Handler(void);

static inline const struct AppHdr* platGetInternalAppList(uint32_t *numAppsP)
{
    extern const struct AppHdr __internal_app_start, __internal_app_end;

    *numAppsP = &__internal_app_end - &__internal_app_start;
    return &__internal_app_start;
}

static inline uint8_t* platGetSharedAreaInfo(uint32_t *areaSzP)
{
    extern uint8_t __shared_start[];
    extern uint8_t __shared_end[];

    *areaSzP = __shared_end - __shared_start;
    return __shared_start;
}

//used for dropbox
void* platGetPersistentRamStore(uint32_t *bytes);

static inline void platWake(void)
{
}

void logbuf_callback(void);

// GCC aligns 64-bit types on an 8-byte boundary, but Cortex-M4 only requires
// 4-byte alignment for these types. So limit the return value, as we're
// interested in the minimum alignment requirement.
#if defined(__GNUC__) && !defined(alignof)
#define alignof(type) ((__alignof__(type) > 4) ? 4 : __alignof__(type))
#endif

#ifdef __cplusplus
}
#endif

#endif
