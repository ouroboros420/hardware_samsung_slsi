/*----------------------------------------------------------------------------
 *      Exynos SoC  -  CCC
 *----------------------------------------------------------------------------
 *      Name:    ccc.h
 *      Purpose: To expose CCC APIs and define macros
 *      Rev.:    V1.00
 *----------------------------------------------------------------------------
 *
 * Copyright (C) 2020 Sukmin Kang
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

#ifndef __EXYNOS_CCC_H__
#define __EXYNOS_CCC_H__

/******************************************************************************
 * Common Command Code
 ******************************************************************************/
#define I3C_CCC_BROADCAST_ENEC                  (0x00)
#define I3C_CCC_BROADCAST_DISEC                 (0x01)
#define I3C_CCC_BROADCAST_ENTAS0                (0x02)
#define I3C_CCC_BROADCAST_ENTAS1                (0x03)
#define I3C_CCC_BROADCAST_ENTAS2                (0x04)
#define I3C_CCC_BROADCAST_ENTAS3                (0x05)
#define I3C_CCC_BROADCAST_RSTDAA                (0x06)
#define I3C_CCC_BROADCAST_ENTDAA                (0x07)
#define I3C_CCC_BROADCAST_DEFSLVS               (0x08)
#define I3C_CCC_BROADCAST_SETMWL                (0x09)
#define I3C_CCC_BROADCAST_SETMRL                (0x0A)
#define I3C_CCC_BROADCAST_ENTTM                 (0x0B)

#define I3C_CCC_BROADCAST_ENTHDR0               (0x20)
#define I3C_CCC_BROADCAST_ENTHDR1               (0x21)
#define I3C_CCC_BROADCAST_ENTHDR2               (0x22)
#define I3C_CCC_BROADCAST_ENTHDR3               (0x23)
#define I3C_CCC_BROADCAST_ENTHDR4               (0x24)
#define I3C_CCC_BROADCAST_ENTHDR5               (0x25)
#define I3C_CCC_BROADCAST_ENTHDR6               (0x26)
#define I3C_CCC_BROADCAST_ENTHDR7               (0x27)
#define I3C_CCC_BROADCAST_SETXTIME              (0x28)

#define I3C_CCC_DIRECT                          (1 << 7)

#define I3C_CCC_DIRECT_ENEC                     (0x80)
#define I3C_CCC_DIRECT_DISEC                    (0x81)
#define I3C_CCC_DIRECT_ENTAS0                   (0x82)
#define I3C_CCC_DIRECT_ENTAS1                   (0x83)
#define I3C_CCC_DIRECT_ENTAS2                   (0x84)
#define I3C_CCC_DIRECT_ENTAS3                   (0x85)
#define I3C_CCC_DIRECT_RSTDAA                   (0x86)
#define I3C_CCC_DIRECT_SETDASA                  (0x87)
#define I3C_CCC_DIRECT_SETNEWDA                 (0x88)
#define I3C_CCC_DIRECT_SETMWL                   (0x89)
#define I3C_CCC_DIRECT_SETMRL                   (0x8A)
#define I3C_CCC_DIRECT_GETMWL                   (0x8B)
#define I3C_CCC_DIRECT_GETMRL                   (0x8C)
#define I3C_CCC_DIRECT_GETPID                   (0x8D)
#define I3C_CCC_DIRECT_GETBCR                   (0x8E)
#define I3C_CCC_DIRECT_GETDCR                   (0x8F)
#define I3C_CCC_DIRECT_GETSTATUS                (0x90)
#define I3C_CCC_DIRECT_GETACCMST                (0x91)

#define I3C_CCC_DIRECT_SETBRGTGT                (0x93)
#define I3C_CCC_DIRECT_GETMXDS                  (0x94)
#define I3C_CCC_DIRECT_GETHDRCAP                (0x95)

#define I3C_CCC_DIRECT_SETXTIME                 (0x98)
#define I3C_CCC_DIRECT_GETXTIME                 (0x99)

#define PRIVATE                                 0
#define CCC                                     1

struct I3cEvents {
    uint8_t events;
};
#define I3C_EVENT_INT           (1 << 0)
#define I3C_EVENT_MR            (1 << 1)
#define I3C_EVENT_HJ            (1 << 3)
#define I3C_EVENT_ALL           (I3C_EVENT_INT | I3C_EVENT_MR | I3C_EVENT_HJ)

struct I3cMrl {
    uint8_t msb;
    uint8_t lsb;
    uint8_t ibi;
} __packed;

struct I3cCccCmdPayload {
    uint16_t len;
    void *data;
};

struct I3cCccCmdDest {
    uint8_t index;      // deivce index for address
    struct I3cCccCmdPayload payload;
};

struct I3cCccCmd {
    uint8_t rnw;
    uint8_t cmd;
    uint8_t err;
    uint8_t ndests;
    struct I3cCccCmdDest *dests;
};
#endif  //  __EXYNOS_CCC_H__
