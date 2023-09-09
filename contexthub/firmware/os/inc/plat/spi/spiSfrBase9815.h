/*----------------------------------------------------------------------------
 *      Exynos SoC  -  SPI
 *----------------------------------------------------------------------------
 *      Name:    spiSfrBase9815.h
 *      Purpose: To expose SPI driver functions
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
#ifndef __SPI_SFR_BASE9815_H__
#define __SPI_SFR_BASE9815_H__

#include <csp_common.h>

static const uint32_t mSpiSfrBase[SPI_CHANNEL_MAX] = {
    SPI0_CHUB_BASE_ADDRESS,
    SPI1_CHUB_BASE_ADDRESS,
    SPI2_CHUB_BASE_ADDRESS,
    };

#endif

