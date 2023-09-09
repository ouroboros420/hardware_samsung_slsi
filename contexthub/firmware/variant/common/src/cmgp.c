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

#include <csp_common.h>
#include <cmgpBoard.h>
#include <usi.h>
#include <cmgp.h>

void cmgpBoardInit(void)
{
    uint32_t i, size;

    size = sizeof(mUsiBoardConfig) / sizeof(mUsiBoardConfig[0]);

    for( i = 0 ; i < size ; i++)
        usiOpen( mUsiBoardConfig[i].port, mUsiBoardConfig[i].protocol);

    size = sizeof(mCmgpIrqBoardConfig) / sizeof(mCmgpIrqBoardConfig[0]);

    for( i = 0 ; i < size ; i++) {
        cmgpSetIrqPriority( mCmgpIrqBoardConfig[i].intNum, mCmgpIrqBoardConfig[i].priority );
        cmgpEnableInterrupt(mCmgpIrqBoardConfig[i].intNum);
    }
}
