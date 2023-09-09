#ifndef SHMIPC_H
#define SHMIPC_H

/*
 * Copyright (C) 2018  Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the License.
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#define SHMIPC_READ_ERROR_PAYLOAD_SIZE -1
#define SHMIPC_READ_ERROR_MULTI_PART_MESSAGE -2
#define SHMIPC_READ_ERROR_UNKNOWN -3

/**
 * @fn shmipc_read
 *
 * @brief Read a shmipc message out of the shared memory region and return it's payload.
 *
 * @param payload The memory a shmipc payload will be copied in to.
 * @param length The number of bytes allocated for the payload.
 *
 * @return bytes read.
 */
int shmipc_read(void *payload, int length);

/**
 * @fn shmipc_write
 *
 * @brief Write a payload to a shared memory region.
 *
 * @param payload The data to be packaged into a shmipc message and written to memory.
 * @param length The number of bytes in the payload.
 *
 * @return bytes written. This should match the length parameter.
 */
int shmipc_write(void *payload, int length);

/**
 * @fn shmipc_init
 * 
 * @brief zero out the shmipc memory block.
 * 
 */
void shmipc_init(void);


#ifdef __cplusplus
}
#endif

#endif /* SHMIPC_H */
