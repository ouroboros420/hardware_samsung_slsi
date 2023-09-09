/*----------------------------------------------------------------------------
 *      Exynos SoC  -  PDMA
 *----------------------------------------------------------------------------
 *      Name:    pdma.c
 *      Purpose: To implement PDMA APIs
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

#include <pdma.h>
#include <pdmaOS.h>

// Public API to initialize PDMA. This should be called when OS starts
void pdmaInit(void)
{
    pdmaOSRequest(PDMA_INIT, NULL);
}

// Implement...


// Public API to save PDMA SFRs before entering system power mode
void pdmaSaveState(void)
{

}

// Public API to restore PDMA SFRs after exiting system power mode
void pdmaRestoreState(void)
{

}

// Public API to shutdown PDMA
void pdmaDeinit(void)
{
    pdmaOSRequest(PDMA_DEINIT, NULL);
}

