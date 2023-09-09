/*----------------------------------------------------------------------------
 *      Exynos SoC  -  CSP
 *----------------------------------------------------------------------------
 *      Name:    csp_printf.h
 *      Purpose: To define printf macro
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

#ifndef __CSP_PRINTF_H__
#define __CSP_PRINTF_H__

#if defined(SEOS)

  #include <seos.h>

  #if defined(ZEBU)
    #define CSP_PRINTF_INFO(fmt, ...) {}
    #define CSP_PRINTF_ERROR(fmt, ...) {}
    #define CSP_PRINTF_DEBUG(fmt, ...) {}
  #else
    #define CSP_PRINTF_INFO(fmt, ...) osLog(LOG_INFO, fmt, ##__VA_ARGS__)
    #define CSP_PRINTF_CAUTION(fmt, ...) osLog(LOG_CAUTION, fmt, ##__VA_ARGS__)
    #define CSP_PRINTF_WARN(fmt, ...) osLog(LOG_WARN, fmt, ##__VA_ARGS__)
    #define CSP_PRINTF_ERROR(fmt, ...) osLog(LOG_ERROR, fmt, ##__VA_ARGS__)

    #ifdef DEF_DEBUG_MODE
      #define CSP_PRINTF_DEBUG(fmt, ...) osLog(LOG_DEBUG, fmt, ##__VA_ARGS__)
    #else
      #define CSP_PRINTF_DEBUG(fmt, ...) {}
    #endif
  #endif

#elif defined(EMBOS)

  #include <stdio.h>
  #include <csp_common.h>

  enum LogLevel {
      LOG_ERROR = 'E',
      LOG_WARN  = 'W',
      LOG_INFO  = 'I',
      LOG_DEBUG = 'D',
  };

  #define CSP_PRINTF_DEBUG(fmt, ...) do { \
          printf("[D]" fmt, ##__VA_ARGS__); \
      } while (0);

  #define CSP_PRINTF_INFO(fmt, ...) do { \
          printf("[I]" fmt, ##__VA_ARGS__); \
      } while (0);

  #define CSP_PRINTF_ERROR(fmt, ...) do { \
          printf("[E]" fmt, ##__VA_ARGS__); \
      } while (0);

  #define LOGHEADER(loglevel) STRINGFY(EXPAND([)EXPAND(loglevel)EXPAND(]))

  #define osLog(loglevel, fmt, ...) do { \
	      printf( LOGHEADER(loglevel) fmt, ##__VA_ARGS__); \
      } while (0);

#endif //if defined(SEOS)
#endif //ifndef __CSP_PRINTF_H__
