/*
 * Copyright (C) 2019 Bosch Sensortec GmbH
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
#ifndef CONTEXTHUB_FIRMWARE_OS_DRIVERS_BOSCH_BMP380_NANO_APP_H_
#define CONTEXTHUB_FIRMWARE_OS_DRIVERS_BOSCH_BMP380_NANO_APP_H_

#include <stdbool.h>
#include <stdint.h>

#include <stdlib.h>
#include <string.h>
#include <float.h>

#include <seos.h>
#include <sensors.h>
#include <slab.h>
#include <nanohubPacket.h>
#include <timer.h>
#include <i2c.h>

#include <heap.h>
#include <hostIntf.h>
#include <util.h>


/*********************************************************************/
/* macro definitions */
/*********************************************************************/

/*! Fusion nano-app id*/
#define BOSCH_BMP3_APP_ID 				APP_ID_MAKE(NANOHUB_VENDOR_BOSCH, 1)

/*! Fusion nano-app version*/
#define BOSCH_BMP3_APP_VERSION 			(1)

#define BMP3_OK    UINT8_C(0x00)

/**
�* @defgroup Register Map definitions
�* @{
�*/
#define BMP3_CHIP_ID_ADDR                 UINT8_C(0x00)
#define BMP3_ERR_REG_ADDR                 UINT8_C(0x02)
#define BMP3_SENS_STATUS_REG_ADDR         UINT8_C(0x03)
#define BMP3_DATA_ADDR                    UINT8_C(0x04)
#define BMP3_PWR_CTRL_ADDR                UINT8_C(0x1B)
#define BMP3_OSR_ADDR                     UINT8_C(0x1C)
#define BMP3_CALIB_DATA_ADDR              UINT8_C(0x31)
#define BMP3_CMD_ADDR                     UINT8_C(0x7E)
/** @} */


/*! Status macros */
#define BMP3_CMD_RDY                      UINT8_C(0x10)
#define BMP3_CMD_ERR                      UINT8_C(0x02)

/*! BMP3 chip identifier */
#define BMP3_CHIP_ID                      UINT8_C(0x50)

/*! Macros related to size */
#define BMP3_CALIB_DATA_LEN               UINT8_C(21)

/*! Macro to combine two 8 bit data's to form a 16 bit data */
#define BMP3_CONCAT_BYTES(msb, lsb) (((uint16_t)msb << 8) | (uint16_t)lsb)

#define BMP3_SET_BITS(reg_data, bitname, data) ((reg_data & ~(bitname##_MSK)) | ((data << bitname##_POS) & bitname##_MSK))

/*! Macro variant to handle the bitname position if it is zero */
#define BMP3_SET_BITS_POS_0(reg_data, bitname, data) ((reg_data & ~(bitname##_MSK)) | (data & bitname##_MSK))

#define BMP3_GET_BITS(reg_data, bitname)       ((reg_data & (bitname##_MSK)) >> (bitname##_POS))

/*! Macro variant to handle the bitname position if it is zero */
#define BMP3_GET_BITS_POS_0(reg_data, bitname) (reg_data & (bitname##_MSK))

#define BMP3_GET_LSB(var)                      (uint8_t)(var & BMP3_SET_LOW_BYTE)
#define BMP3_GET_MSB(var)                      (uint8_t)((var & BMP3_SET_HIGH_BYTE) >> 8)


#define BMP3_PRESS_EN_MSK                 UINT8_C(0x01)

#define BMP3_TEMP_EN_MSK                  UINT8_C(0x02)
#define BMP3_TEMP_EN_POS                  UINT8_C(0x01)

#define BMP3_OP_MODE_MSK                  UINT8_C(0x30)
#define BMP3_OP_MODE_POS                  UINT8_C(0x04)


/*! Macros to select the specific sensor type for configuring them
 *  These values are internal for API implementation. Don't relate this to
 *  data sheet */
#define BMP3_PRESS_EN_SEL                 UINT16_C(1 << 1)
#define BMP3_TEMP_EN_SEL                  UINT16_C(1 << 2)

/**
�* @defgroup Power_mode_macros BMP3 Power mode macros
�* @{
�*/
#define BMP3_SLEEP_MODE                   UINT8_C(0x00)
#define BMP3_FORCED_MODE                  UINT8_C(0x01)
#define BMP3_NORMAL_MODE                  UINT8_C(0x03)
/** @} */

/*!
 * @brief Register Trim Variables
 */
struct bmp3_reg_calib_data
{
    /* @ Trim Variables */
    /**@{*/
    uint16_t par_t1;
    uint16_t par_t2;
    int8_t par_t3;
    int16_t par_p1;
    int16_t par_p2;
    int8_t par_p3;
    int8_t par_p4;
    uint16_t par_p5;
    uint16_t par_p6;
    int8_t par_p7;
    int8_t par_p8;
    int16_t par_p9;
    int8_t par_p10;
    int8_t par_p11;
	/* Read from temp data */
    int64_t t_lin;

    /**@}*/
};

#endif /*CONTEXTHUB_FIRMWARE_OS_DRIVERS_BOSCH_BMP380_NANO_APP_H_*/