/*----------------------------------------------------------------------------
 *      Exynos SoC  -  Mem UTC
 *----------------------------------------------------------------------------
 *      Name:    mem_test.c
 *      Purpose: To implement mem-test UTC codes
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

#if defined(UTC_REQUIRED)

#include <mem_test.h>
#include <string.h>

#define MEM_SIZE 1024

/*
    Allocate memory, set specific values, copy memory from src to dst, and then compare both to see if any different part is seen.
*/
void mem_test(void)
{
    static int cnt = 0;
    static void *src = NULL;
    static void *dst = NULL;
    static unsigned char val = 0;

    // Allocate memory from heap for source
    src = cspHeapAlloc(MEM_SIZE);
    if(src == NULL) {
        CSP_PRINTF_ERROR("Oops!, cspHeapAlloc(src) failed\n");
        return;
    }

    // Allocate memory from heap for destination
    dst = cspHeapAlloc(MEM_SIZE);
    if(dst == NULL) {
        CSP_PRINTF_ERROR("Oops!, cspHeapAlloc(dst) failed\n");
        return;
    }

    // Set specific value to source memory
    memset(src, val++, MEM_SIZE);

    // Copy data from source to destination
    memcpy(dst, src, MEM_SIZE);

    // Compare source with destination
    if(memcmp(src, dst, MEM_SIZE) == 0) {
        cnt++;
        CSP_PRINTF_INFO("%d-times mem_test succeeded\n", cnt);
    }
    else {
        CSP_PRINTF_ERROR("Oops!, mem_test failed\n");
    }

    // Free memory
    cspHeapFree(src);
    cspHeapFree(dst);
}

void mem_test_clean_up(void)
{
    // Do clean-up

	CSP_PRINTF_ERROR("%s is called\n", __func__);
}

#endif

