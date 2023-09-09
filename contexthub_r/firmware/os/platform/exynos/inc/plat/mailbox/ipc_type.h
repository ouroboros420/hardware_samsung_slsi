/*
 * Copyright (C) 2017 Samsung Electronics Co., Ltd.
 *
 * Contact: Boojin Kim <boojin.kim@samsung.com>
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

#ifndef _MAILBOX_IPC_TYPE_H
#define _MAILBOX_IPC_TYPE_H

#define CIPC_RAW_READL(a)      (*(volatile unsigned int *)(a))
#define CIPC_RAW_WRITEL(v, a)  (*(volatile unsigned int *)(a) = ((unsigned int)v))

#define CIPC_TRUE 1
#define CIPC_FALSE 0
#define CIPC_NULL 0
#define CIPC_ERR -1

#ifndef CU64
typedef unsigned long long CU64;
#endif

#ifndef CU32
typedef unsigned long int CU32;
#endif
#endif
