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

#if defined(HEAP_DEBUG)

#include <heapDebug.h>
#include <csp_common.h>

#define MAX_SIZE 300 // It depends on how many allocations are required at some point

typedef struct {
    char *fn;
    int line;
    uint32_t sz;
    void *ptr;
} heapHistory;

static heapHistory heap[MAX_SIZE];
static int heapAllocCnt = 0;
static int heapAllocSize = 0;

void heapHistoryInit(void)
{
	heapAllocCnt = 0;
}

void heapHistoryPush(const char *fn, int line, uint32_t sz, void *ptr)
{
    int i;
    for(i = 0; i < MAX_SIZE; i++) {
        if(!heap[i].ptr) {
            heap[i].fn = (char *)fn;
            heap[i].line = line;
            heap[i].sz = sz;
            heap[i].ptr = ptr;

            if(ptr != NULL) heapAllocCnt++;
			heapAllocSize += sz;
            return;
        }
    }

    CSP_PRINTF_ERROR("heapHistory is full!\n");
}

void heapHistoryPop(void *value)
{
    int i;
    for(i = 0; i < MAX_SIZE; i++) {
        if(heap[i].ptr == value) {
            heap[i].fn = 0;
            heap[i].line = 0;
			heapAllocSize -= heap[i].sz;
            heap[i].sz = 0;
            heap[i].ptr = 0;

            heapAllocCnt--;
            return;
        }
    }

    CSP_PRINTF_ERROR("Oops!! not in heapHistory\n");
}

void heapHistoryPrint(void)
{
    int i;

    CSP_PRINTF_INFO("heapHistory-------------\n");
    for(i = 0; i < MAX_SIZE; i++) {
        if(heap[i].fn != 0) {
            CSP_PRINTF_INFO("%s, %d, sz=%d, ptr=0x%x\n", heap[i].fn, heap[i].line, (int)(heap[i].sz), (unsigned int)heap[i].ptr);
        }
    }

    CSP_PRINTF_INFO("[%d] %d bytes in total\n", heapAllocCnt, heapAllocSize);
}

#endif

