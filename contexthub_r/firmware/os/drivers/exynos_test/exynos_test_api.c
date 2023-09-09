/*----------------------------------------------------------------------------
 *      Exynos SoC  -  UTC
 *----------------------------------------------------------------------------
 *      Name:    exynos_test_api.c
 *      Purpose: UTC API Implementation
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

#include <exynos_test_api.h>
#include <exynos_test.h>

void utcCommand(UtcCommandType uc)
{
#if defined(UTC_REQUIRED)
    utcCommandInternal(uc, NULL);
#endif
}

