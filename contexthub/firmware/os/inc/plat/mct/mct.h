/*----------------------------------------------------------------------------
 *      Exynos SoC  -  MCT
 *----------------------------------------------------------------------------
 *      Name:    mct.h
 *      Purpose: To expose I3C APIs and define macros
 *      Rev.:    V1.00
 *----------------------------------------------------------------------------
 *
 * Copyright (C) 2020 Sukmin Kang
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

#ifndef __EXYNOS_MCT_H__
#define __EXYNOS_MCT_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

void mctSetClockState(bool en);
uint64_t mctGetTimeStampNS(void);
uint64_t mctGetTimeStampUS(void);
void mctInit(void);

#endif
