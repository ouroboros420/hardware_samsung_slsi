/*----------------------------------------------------------------------------
 *      Exynos SoC  -  CSP
 *----------------------------------------------------------------------------
 *      Name:    csp_os.h
 *      Purpose: To define os api
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

#ifndef __CSP_OS_H__
#define __CSP_OS_H__

#include <csp_common.h>
#if defined(SEOS)
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <seos.h>
  #include <csp_printf.h>
#elif defined(EMBOS)
  #include <Device.h>
#endif
#if defined(DEF_IPC_DEBUG) || defined(DEF_DEBUG_MODE) || defined(DEF_IDLE_MONITOR)
#include <debug.h>
#endif

enum error_type {
	ERR_ASSERT,
	ERR_FAULT,
	ERR_REBOOT,
	ERR_ERROR,
	ERR_PANIC,
};

void *cspHeapAlloc(uint32_t sz);
void *cspHeapRealloc(void *ptr, uint32_t newSz);
void cspHeapFree(void *ptr);
void uSleep(uint32_t us);
void mSleep(uint32_t ms);
#define msleep(ms) mSleep(ms);
void cspNotify(enum error_type);
#endif
