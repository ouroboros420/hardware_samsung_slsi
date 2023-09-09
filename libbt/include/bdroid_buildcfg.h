/*
 * Copyright (C) 2012 The Android Open Source Project
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

#ifndef _BDROID_BUILDCFG_H
#define _BDROID_BUILDCFG_H

#define BTE_DID_CONF_FILE "/vendor/etc/bluetooth/bt_did.conf"

/**
 * SD-1101 Optimize RX SPP throughput
 * Optimize RX SPP throughput by making sure that
 * the Peer device do not run out of rfcomm credits
 * Originally defined in bt_target.h
 * SSB-5680: Re-optimize for OPP throughput
 */
#define PORT_RX_BUF_LOW_WM          (20)
#define PORT_RX_BUF_HIGH_WM         (40)
#define PORT_RX_BUF_CRITICAL_WM     (45)

/*
 * SSB-213 New extension channel needed for Android-defined vendor specific commands.
 * Toggles support for vendor specific extensions such as RPA offloading,
 * feature discovery, multi-adv etc.
 */
#define BLE_VND_INCLUDED        TRUE

/**
 * SSB-4621 Change number of credits allocated for Bluetooth LE data in the Android host
 * Based in investigations initiated by MTK-86 it is recommended to change the number of
 * credits allocated for BLE data from 1 (default) to 2.
 */
#define L2C_DEF_NUM_BLE_BUF_SHARED      2

/**
 * SSB-582 Improve data scheduling to HCI when A2DP is involved
 * Ensure that A2DP data is scheduled in a way that suits the scheduling in Coex and BT-only scenarios
 */
#define L2CAP_HIGH_PRI_MIN_XMIT_QUOTA       6

/**
 * HOST-10876 When transferring data via OBEX the application sending the data
 * may look a lot faster than the receiver. By reducing the buffer quota from
 * 100 to 10 the stack data transfer rate will be more in sync with the UI,
 * because it will limit the number of packet which can be buffered in the stack
 */
#define L2CAP_CBB_DEFAULT_DATA_RATE_BUFF_QUOTA 10

/**
 * Enable a2dp sink role, needs change in app as well
 */
#define BTA_AV_SINK_INCLUDED TRUE

#endif
