/*!
 * @section LICENSE
 *
 * (C) Copyright 2011~2021 Bosch Sensortec GmbH All Rights Reserved
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
 *
 *------------------------------------------------------------------------------
 *  Disclaimer
 *
 * Common: Bosch Sensortec products are developed for the consumer goods
 * industry. They may only be used within the parameters of the respective valid
 * product data sheet.  Bosch Sensortec products are provided with the express
 * understanding that there is no warranty of fitness for a particular purpose.
 * They are not fit for use in life-sustaining, safety or security sensitive
 * systems or any system or device that may lead to bodily harm or property
 * damage if the system or device malfunctions. In addition, Bosch Sensortec
 * products are not fit for use in products which interact with motor vehicle
 * systems.  The resale and/or use of products are at the purchaser's own risk
 * and his own responsibility. The examination of fitness for the intended use
 * is the sole responsibility of the Purchaser.
 *
 * The purchaser shall indemnify Bosch Sensortec from all third party claims,
 * including any claims for incidental, or consequential damages, arising from
 * any product use not covered by the parameters of the respective valid product
 * data sheet or not approved by Bosch Sensortec and reimburse Bosch Sensortec
 * for all costs in connection with such claims.
 *
 * The purchaser must monitor the market for the purchased products,
 * particularly with regard to product safety and inform Bosch Sensortec without
 * delay of all security relevant incidents.
 *
 * Engineering Samples are marked with an asterisk (*) or (e). Samples may vary
 * from the valid technical specifications of the product series. They are
 * therefore not intended or fit for resale to third parties or for use in end
 * products. Their sole purpose is internal client testing. The testing of an
 * engineering sample may in no way replace the testing of a product series.
 * Bosch Sensortec assumes no liability for the use of engineering samples. By
 * accepting the engineering samples, the Purchaser agrees to indemnify Bosch
 * Sensortec from all claims arising from the use of engineering samples.
 *
 * Special: This software module (hereinafter called "Software") and any
 * information on application-sheets (hereinafter called "Information") is
 * provided free of charge for the sole purpose to support your application
 * work. The Software and Information is subject to the following terms and
 * conditions:
 *
 * The Software is specifically designed for the exclusive use for Bosch
 * Sensortec products by personnel who have special experience and training. Do
 * not use this Software if you do not have the proper experience or training.
 *
 * This Software package is provided `` as is `` and without any expressed or
 * implied warranties, including without limitation, the implied warranties of
 * merchantability and fitness for a particular purpose.
 *
 * Bosch Sensortec and their representatives and agents deny any liability for
 * the functional impairment of this Software in terms of fitness, performance
 * and safety. Bosch Sensortec and their representatives and agents shall not be
 * liable for any direct or indirect damages or injury, except as otherwise
 * stipulated in mandatory applicable law.
 *
 * The Information provided is believed to be accurate and reliable. Bosch
 * Sensortec assumes no responsibility for the consequences of use of such
 * Information nor for any infringement of patents or other rights of third
 * parties which may result from its use.
 * @file	bmi2_defs.h
 * @date	01-02-2021
 * @version	v1.0.0.1
 * @brief	Sensor driver for BMI2XY sensor
 *
 */

#ifndef BMI2_DEFS_H_
#define BMI2_DEFS_H_

/******************************************************************************/
/*! @name        General Macro Definitions                */
/******************************************************************************/
/*! @name  Utility macros */
#define BMI2_SET_BITS(reg_data, bitname, data) \
    ((reg_data & ~(bitname##_MASK)) | \
     ((data << bitname##_POS) & bitname##_MASK))

#define BMI2_GET_BITS(reg_data, bitname) \
    ((reg_data & (bitname##_MASK)) >> \
     (bitname##_POS))

#define BMI2_SET_BIT_POS0(reg_data, bitname, data) \
    ((reg_data & ~(bitname##_MASK)) | \
     (data & bitname##_MASK))

#define BMI2_GET_BIT_POS0(reg_data, bitname) (reg_data & (bitname##_MASK))
#define BMI2_SET_BIT_VAL0(reg_data, bitname) (reg_data & ~(bitname##_MASK))
#define BST_CLR_VAL_BIT(val, bit)      (val & (~(1 << bit)))

/*! @name For getting LSB and MSB */
#define BMI2_GET_LSB(var)                    (uint8_t)(var & BMI2_SET_LOW_BYTE)
#define BMI2_GET_MSB(var)                    (uint8_t)((var & BMI2_SET_HIGH_BYTE) >> 8)

/*! @name For defining absolute values */
#define BMI2_ABS(a)                          ((a) > 0 ? (a) : -(a))

/*! @name LSB and MSB mask definitions */
#define BMI2_SET_LOW_BYTE                        UINT16_C(0x00FF)
#define BMI2_SET_HIGH_BYTE                       UINT16_C(0xFF00)
#define BMI2_SET_LOW_NIBBLE                      UINT8_C(0x0F)

/*! @name For enable and disable */
#define BMI2_ENABLE                              UINT8_C(1)
#define BMI2_DISABLE                             UINT8_C(0)

/*! @name To define TRUE or FALSE */
#define BMI2_TRUE                                UINT8_C(1)
#define BMI2_FALSE                               UINT8_C(0)

/*! @name To define success code */
#define BMI2_OK                                  INT8_C(0)

/*! @name To define error codes */
#define BMI2_E_NULL_PTR                          INT8_C(-1)
#define BMI2_E_COM_FAIL                          INT8_C(-2)
#define BMI2_E_DEV_NOT_FOUND                     INT8_C(-3)
#define BMI2_E_OUT_OF_RANGE                      INT8_C(-4)
#define BMI2_E_ACC_INVALID_CFG                   INT8_C(-5)
#define BMI2_E_GYRO_INVALID_CFG                  INT8_C(-6)
#define BMI2_E_ACC_GYR_INVALID_CFG               INT8_C(-7)
#define BMI2_E_INVALID_SENSOR                    INT8_C(-8)
#define BMI2_E_CONFIG_LOAD                       INT8_C(-9)
#define BMI2_E_INVALID_PAGE                      INT8_C(-10)
#define BMI2_E_INVALID_FEAT_BIT                  INT8_C(-11)
#define BMI2_E_INVALID_INT_PIN                   INT8_C(-12)
#define BMI2_E_SET_APS_FAIL                      INT8_C(-13)
#define BMI2_E_AUX_INVALID_CFG                   INT8_C(-14)
#define BMI2_E_AUX_BUSY                          INT8_C(-15)
#define BMI2_E_SELF_TEST_FAIL                    INT8_C(-16)
#define BMI2_E_REMAP_ERROR                       INT8_C(-17)
#define BMI2_E_GYR_USER_GAIN_UPD_FAIL            INT8_C(-18)
#define BMI2_E_SELF_TEST_NOT_DONE                INT8_C(-19)
#define BMI2_E_INVALID_INPUT                     INT8_C(-20)
#define BMI2_E_INVALID_STATUS                    INT8_C(-21)
#define BMI2_E_CRT_ERROR                         INT8_C(-22)
#define BMI2_E_ST_ALREADY_RUNNING                INT8_C(-23)
#define BMI2_E_CRT_READY_FOR_DL_FAIL_ABORT       INT8_C(-24)
#define BMI2_E_DL_ERROR                          INT8_C(-25)
#define BMI2_E_PRECON_ERROR                      INT8_C(-26)
#define BMI2_E_ABORT_ERROR                       INT8_C(-27)
#define BMI2_E_GYRO_SELF_TEST_ERROR              INT8_C(-28)
#define BMI2_E_GYRO_SELF_TEST_TIMEOUT            INT8_C(-29)
#define BMI2_E_WRITE_CYCLE_ONGOING               INT8_C(-30)
#define BMI2_E_WRITE_CYCLE_TIMEOUT               INT8_C(-31)
#define BMI2_E_ST_NOT_RUNING                     INT8_C(-32)
#define BMI2_E_DATA_RDY_INT_FAILED               INT8_C(-33)

/*! @name To define warnings for FIFO activity */
#define BMI2_W_FIFO_EMPTY                        INT8_C(1)
#define BMI2_W_PARTIAL_READ                      INT8_C(2)

/*! @name Bit wise to define information */
#define BMI2_I_MIN_VALUE                         UINT8_C(1)
#define BMI2_I_MAX_VALUE                         UINT8_C(2)

/*! @name BMI2 register addresses */
#define BMI2_CHIP_ID_ADDR                        UINT8_C(0x00)
#define BMI2_STATUS_ADDR                         UINT8_C(0x03)
#define BMI2_AUX_X_LSB_ADDR                      UINT8_C(0x04)
#define BMI2_ACC_X_LSB_ADDR                      UINT8_C(0x0C)
#define BMI2_GYR_X_LSB_ADDR                      UINT8_C(0x12)
#define BMI2_SENSOR_TIME_ADDR                    UINT8_C(0x18)
#define BMI2_EVENT_ADDR                          UINT8_C(0x1B)
#define BMI2_INT_STATUS_0_ADDR                   UINT8_C(0x1C)
#define BMI2_INT_STATUS_1_ADDR                   UINT8_C(0x1D)
#define BMI2_SC_OUT_0_ADDR                       UINT8_C(0x1E)
#define BMI2_SYNC_COMMAND_ADDR                   UINT8_C(0x1E)
#define BMI2_INTERNAL_STATUS_ADDR                UINT8_C(0x21)
#define BMI2_FIFO_LENGTH_0_ADDR                  UINT8_C(0X24)
#define BMI2_FIFO_DATA_ADDR                      UINT8_C(0X26)
#define BMI2_FEAT_PAGE_ADDR                      UINT8_C(0x2F)
#define BMI2_FEATURES_REG_ADDR                   UINT8_C(0x30)
#define BMI2_ACC_CONF_ADDR                       UINT8_C(0x40)
#define BMI2_GYR_CONF_ADDR                       UINT8_C(0x42)
#define BMI2_AUX_CONF_ADDR                       UINT8_C(0x44)
#define BMI2_FIFO_DOWNS_ADDR                     UINT8_C(0X45)
#define BMI2_FIFO_WTM_0_ADDR                     UINT8_C(0X46)
#define BMI2_FIFO_WTM_1_ADDR                     UINT8_C(0X47)
#define BMI2_FIFO_CONFIG_0_ADDR                  UINT8_C(0X48)
#define BMI2_FIFO_CONFIG_1_ADDR                  UINT8_C(0X49)
#define BMI2_AUX_DEV_ID_ADDR                     UINT8_C(0x4B)
#define BMI2_AUX_IF_CONF_ADDR                    UINT8_C(0x4C)
#define BMI2_AUX_RD_ADDR                         UINT8_C(0x4D)
#define BMI2_AUX_WR_ADDR                         UINT8_C(0x4E)
#define BMI2_AUX_WR_DATA_ADDR                    UINT8_C(0x4F)
#define BMI2_INT1_IO_CTRL_ADDR                   UINT8_C(0x53)
#define BMI2_INT2_IO_CTRL_ADDR                   UINT8_C(0x54)
#define BMI2_INT1_MAP_FEAT_ADDR                  UINT8_C(0x56)
#define BMI2_INT2_MAP_FEAT_ADDR                  UINT8_C(0x57)
#define BMI2_INT_MAP_DATA_ADDR                   UINT8_C(0x58)
#define BMI2_INIT_CTRL_ADDR                      UINT8_C(0x59)
#define BMI2_INIT_ADDR_0                         UINT8_C(0x5B)
#define BMI2_INIT_ADDR_1                         UINT8_C(0x5C)
#define BMI2_INIT_DATA_ADDR                      UINT8_C(0x5E)
#define BMI2_GYR_CRT_CONF_ADDR                   UINT8_C(0X69)
#define BMI2_NVM_CONF_ADDR                       UINT8_C(0x6A)
#define BMI2_IF_CONF_ADDR                        UINT8_C(0X6B)
#define BMI2_ACC_SELF_TEST_ADDR                  UINT8_C(0X6D)
#define BMI2_GYR_SELF_TEST_AXES_ADDR             UINT8_C(0x6E)
#define BMI2_NV_CONF_ADDR                        UINT8_C(0x70)
#define BMI2_ACC_OFF_COMP_0_ADDR                 UINT8_C(0X71)
#define BMI2_GYR_OFF_COMP_3_ADDR                 UINT8_C(0X74)
#define BMI2_GYR_OFF_COMP_6_ADDR                 UINT8_C(0X77)
#define BMI2_GYR_USR_GAIN_0_ADDR                 UINT8_C(0X78)
#define BMI2_PWR_CONF_ADDR                       UINT8_C(0x7C)
#define BMI2_PWR_CTRL_ADDR                       UINT8_C(0x7D)
#define BMI2_CMD_REG_ADDR                        UINT8_C(0x7E)

/*! @name BMI2 I2C address */
#define BMI2_I2C_PRIM_ADDR                       UINT8_C(0x68)
#define BMI2_I2C_SEC_ADDR                        UINT8_C(0x69)

/*! @name BMI2 Commands */
#define BMI2_G_TRIGGER_CMD                       UINT8_C(0x02)
#define BMI2_USR_GAIN_CMD                        UINT8_C(0x03)
#define BMI2_NVM_PROG_CMD                        UINT8_C(0xA0)
#define BMI2_SOFT_RESET_CMD                      UINT8_C(0xB6)
#define BMI2_FIFO_FLUSH_CMD                      UINT8_C(0xB0)

/*! @name Mask definitions for ..... */
#define BMI2_GYR_RDY_FOR_DL_MASK                 UINT8_C(0x08)
#define BMI2_GYR_CRT_RUNNING_MASK                UINT8_C(0x04)

/*! @name Bit position definitions..... */
#define BMI2_GYR_RDY_FOR_DL_POS                  UINT8_C(0x03)
#define BMI2_GYR_CRT_RUNNING_POS                 UINT8_C(0x02)

/*! @name BMI2 sensor data bytes */
#define BMI2_ACC_GYR_NUM_BYTES                   UINT8_C(6)
#define BMI2_AUX_NUM_BYTES                       UINT8_C(8)
#define BMI2_CONFIG_FILE_SIZE                    UINT16_C(8192)
#define BMI2_CRT_CONFIG_FILE_SIZE                UINT16_C(2048)
#define BMI2_FEAT_SIZE_IN_BYTES                  UINT8_C(16)
#define BMI2_ACC_CONFIG_LENGTH                   UINT8_C(2)

/*! @name BMI2 configuration load status */
#define BMI2_CONFIG_LOAD_SUCCESS                 UINT8_C(1)

/*! @name To define BMI2 pages */
#define BMI2_PAGE_0                              UINT8_C(0)
#define BMI2_PAGE_1                              UINT8_C(1)
#define BMI2_PAGE_2                              UINT8_C(2)
#define BMI2_PAGE_3                              UINT8_C(3)
#define BMI2_PAGE_4                              UINT8_C(4)
#define BMI2_PAGE_5                              UINT8_C(5)
#define BMI2_PAGE_6                              UINT8_C(6)
#define BMI2_PAGE_7                              UINT8_C(7)

/*! @name Array Parameter DefinItions */
#define BMI2_SENSOR_TIME_LSB_BYTE                UINT8_C(0)
#define BMI2_SENSOR_TIME_XLSB_BYTE               UINT8_C(1)
#define BMI2_SENSOR_TIME_MSB_BYTE                UINT8_C(2)

/*! @name mask definition for status register */
#define BMI2_AUX_BUSY_MASK                       UINT8_C(0x04)
#define BMI2_CMD_RDY_MASK                        UINT8_C(0x10)
#define BMI2_DRDY_AUX_MASK                       UINT8_C(0x20)
#define BMI2_DRDY_GYR_MASK                       UINT8_C(0x40)
#define BMI2_DRDY_ACC_MASK                       UINT8_C(0x80)

/*! @name Bit position for status register*/
#define BMI2_AUX_BUSY_POS                        UINT8_C(0x02)
#define BMI2_CMD_RDY_POS                         UINT8_C(0x04)
#define BMI2_DRDY_AUX_POS                        UINT8_C(0x05)
#define BMI2_DRDY_GYR_POS                        UINT8_C(0x06)
#define BMI2_DRDY_ACC_POS                        UINT8_C(0x07)

/*! @name Mask definitions for SPI read/write address */
#define BMI2_SPI_RD_MASK                         UINT8_C(0x80)
#define BMI2_SPI_WR_MASK                         UINT8_C(0x7F)

/*! @name Mask definitions for power configuration register */
#define BMI2_ADV_POW_EN_MASK                     UINT8_C(0x01)

/*! @name Mask definitions for initialization control register */
#define BMI2_CONF_LOAD_EN_MASK                   UINT8_C(0x01)

/*! @name Mask definitions for power control register */
#define BMI2_AUX_EN_MASK                         UINT8_C(0x01)
#define BMI2_GYR_EN_MASK                         UINT8_C(0x02)
#define BMI2_ACC_EN_MASK                         UINT8_C(0x04)
#define BMI2_TEMP_EN_MASK                        UINT8_C(0x08)

/*! @name Bit position definitions for power control register */
#define BMI2_GYR_EN_POS                          UINT8_C(0x01)
#define BMI2_ACC_EN_POS                          UINT8_C(0x02)
#define BMI2_TEMP_EN_POS                         UINT8_C(0x03)

/*! @name Mask definitions for sensor event flags */
#define BMI2_EVENT_FLAG_MASK                     UINT8_C(0x1C)

/*! @name Bit position definitions for sensor event flags */
#define BMI2_EVENT_FLAG_POS                      UINT8_C(0x02)

/*! @name Mask definitions to switch page */
#define BMI2_SWITCH_PAGE_EN_MASK                 UINT8_C(0x07)

/*! @name Accelerometer and Gyroscope Filter/Noise performance modes */
/* Power optimized mode */
#define BMI2_POWER_OPT_MODE                      UINT8_C(0)

/* Performance optimized  */
#define BMI2_PERF_OPT_MODE                       UINT8_C(1)

/*! @name Mask definitions of NVM register */
#define BMI2_NV_ACC_OFFSET_MASK                  UINT8_C(0x08)

/*! @name Bit position definitions of NVM register */
#define BMI2_NV_ACC_OFFSET_POS                   UINT8_C(0x03)

/*! @name Mask definition for config version */
#define BMI2_CONFIG_MAJOR_MASK                   UINT16_C(0x3C0)
#define BMI2_CONFIG_MINOR_MASK                   UINT8_C(0x3F)

/*! @name Bit position for major version from config */
#define BMI2_CONFIG_MAJOR_POS                    UINT8_C(0x06)

#define BMI2_GYRO_CROSS_AXES_SENSE_MASK          UINT8_C(0x7F)
#define BMI2_GYRO_CROSS_AXES_SENSE_SIGN_BIT_MASK UINT8_C(0x40)

/*! @name index for config major minor information */
#define BMI2_CONFIG_INFO_LOWER                   UINT8_C(52)
#define BMI2_CONFIG_INFO_HIGHER                  UINT8_C(53)

/*! @name Sensor status */
#define BMI2_DRDY_ACC                            UINT8_C(0x80)
#define BMI2_DRDY_GYR                            UINT8_C(0x40)
#define BMI2_DRDY_AUX                            UINT8_C(0x20)
#define BMI2_CMD_RDY                             UINT8_C(0x10)
#define BMI2_AUX_BUSY                            UINT8_C(0x04)

/*! @name Macro to define accelerometer configuration value for FOC */
#define BMI2_FOC_ACC_CONF_VAL                    UINT8_C(0xB7)

/*! @name Macro to define gyroscope configuration value for FOC */
#define BMI2_FOC_GYR_CONF_VAL                    UINT8_C(0xB6)

/*! @name Macro to define X Y and Z axis for an array */
#define BMI2_X_AXIS                              UINT8_C(0)
#define BMI2_Y_AXIS                              UINT8_C(1)
#define BMI2_Z_AXIS                              UINT8_C(2)

/******************************************************************************/
/*! @name        Sensor Macro Definitions                 */
/******************************************************************************/
/*!  @name Macros to define BMI2 sensor/feature types */
#define BMI2_ACCEL                    UINT8_C(0)
#define BMI2_GYRO                     UINT8_C(1)
#define BMI2_AUX                      UINT8_C(2)
#define BMI2_TEMP                     UINT8_C(3)
#define BMI2_ANY_MOTION               UINT8_C(4)
#define BMI2_NO_MOTION                UINT8_C(5)
#define BMI2_TILT                     UINT8_C(6)
#define BMI2_ORIENTATION              UINT8_C(7)
#define BMI2_SIG_MOTION               UINT8_C(8)
#define BMI2_STEP_DETECTOR            UINT8_C(9)
#define BMI2_STEP_COUNTER             UINT8_C(10)
#define BMI2_STEP_ACTIVITY            UINT8_C(11)
#define BMI2_GYRO_GAIN_UPDATE         UINT8_C(12)
#define BMI2_UP_HOLD_TO_WAKE          UINT8_C(13)
#define BMI2_GLANCE_DETECTOR          UINT8_C(14)
#define BMI2_WAKE_UP                  UINT8_C(15)
#define BMI2_HIGH_G                   UINT8_C(16)
#define BMI2_LOW_G                    UINT8_C(17)
#define BMI2_FLAT                     UINT8_C(18)
#define BMI2_EXT_SENS_SYNC            UINT8_C(19)
#define BMI2_WRIST_GESTURE            UINT8_C(20)
#define BMI2_WRIST_WEAR_WAKE_UP       UINT8_C(21)
#define BMI2_ACTIVITY_RECOGNITION     UINT8_C(22)

#define BMI2_AXIS_MAP                 UINT8_C(23)
#define BMI2_GYRO_SELF_OFF            UINT8_C(24)
#define BMI2_STEP_COUNTER_PARAMS      UINT8_C(25)
#define BMI2_NVM_STATUS               UINT8_C(26)
#define BMI2_VFRM_STATUS              UINT8_C(27)
#define BMI2_GYRO_CROSS_SENSE         UINT8_C(28)
#define BMI2_ACCEL_SELF_TEST          UINT8_C(29)
#define BMI2_MAX_BURST_LEN            UINT8_C(30)

#define BMI2_SENS_MAX_NUM             UINT8_C(31)
#define BMI2_CRT_GYRO_SELF_TEST       UINT8_C(32)
#define BMI2_ABORT_CRT_GYRO_SELF_TEST UINT8_C(33)
#define BMI2_NVM_PROG_PREP            UINT8_C(34)

/*! @name Bit wise for selecting BMI2 sensors/features */
#define BMI2_ACCEL_SENS_SEL           (1)
#define BMI2_GYRO_SENS_SEL            (1 << BMI2_GYRO)
#define BMI2_AUX_SENS_SEL             (1 << BMI2_AUX)
#define BMI2_TEMP_SENS_SEL            (1 << BMI2_TEMP)
#define BMI2_ANY_MOT_SEL              (1 << BMI2_ANY_MOTION)
#define BMI2_NO_MOT_SEL               (1 << BMI2_NO_MOTION)
#define BMI2_TILT_SEL                 (1 << BMI2_TILT)
#define BMI2_ORIENT_SEL               (1 << BMI2_ORIENTATION)
#define BMI2_SIG_MOTION_SEL           (1 << BMI2_SIG_MOTION)
#define BMI2_STEP_DETECT_SEL          (1 << BMI2_STEP_DETECTOR)
#define BMI2_STEP_COUNT_SEL           (1 << BMI2_STEP_COUNTER)
#define BMI2_STEP_ACT_SEL             (1 << BMI2_STEP_ACTIVITY)
#define BMI2_GYRO_GAIN_UPDATE_SEL     (1 << BMI2_GYRO_GAIN_UPDATE)
#define BMI2_UP_HOLD_TO_WAKE_SEL      (1 << BMI2_UP_HOLD_TO_WAKE)
#define BMI2_GLANCE_DET_SEL           (1 << BMI2_GLANCE_DETECTOR)
#define BMI2_WAKE_UP_SEL              (1 << BMI2_WAKE_UP)
#define BMI2_HIGH_G_SEL               (1 << BMI2_HIGH_G)
#define BMI2_LOW_G_SEL                (1 << BMI2_LOW_G)
#define BMI2_FLAT_SEL                 (1 << BMI2_FLAT)
#define BMI2_EXT_SENS_SEL             (1 << BMI2_EXT_SENS_SYNC)
#define BMI2_GYRO_SELF_OFF_SEL        (1 << BMI2_GYRO_SELF_OFF)
#define BMI2_WRIST_GEST_SEL           (1 << BMI2_WRIST_GESTURE)
#define BMI2_WRIST_WEAR_WAKE_UP_SEL   (1 << BMI2_WRIST_WEAR_WAKE_UP)
#define BMI2_ACTIVITY_RECOGNITION_SEL (1 << BMI2_ACTIVITY_RECOGNITION)
#define BMI2_ACCEL_SELF_TEST_SEL      (1 << BMI2_ACCEL_SELF_TEST)

/*! @name Bit wise selection of BMI2 sensors */
#define BMI2_MAIN_SENSORS \
    (BMI2_ACCEL_SENS_SEL | BMI2_GYRO_SENS_SEL \
     | BMI2_AUX_SENS_SEL | BMI2_TEMP_SENS_SEL)

/*!  @name Maximum number of BMI2 main sensors */
#define BMI2_MAIN_SENS_MAX_NUM     UINT8_C(4)

/*! @name Macro to specify the number of step counter parameters */
#define BMI2_STEP_CNT_N_PARAMS     UINT8_C(25)

/*!  @name Macro to select between single and double tap */
#define BMI2_DOUBLE_TAP_SEL        UINT8_C(0)
#define BMI2_SINGLE_TAP_SEL        UINT8_C(1)

#define BMI2_SELECT_GYRO_SELF_TEST UINT8_C(0)
#define BMI2_SELECT_CRT            UINT8_C(1)

/*! @name Macro for NVM enable */
#define BMI2_NVM_UNLOCK_ENABLE     UINT8_C(0x02)
#define BMI2_NVM_UNLOCK_DISABLE    UINT8_C(0x00)

/*! @name macro to select between gyro self test and CRT */
#define BMI2_GYRO_SELF_TEST_SEL    UINT8_C(0)
#define BMI2_CRT_SEL               UINT8_C(1)

/******************************************************************************/
/*! @name       Accelerometer Macro Definitions               */
/******************************************************************************/
/*! @name Accelerometer Bandwidth parameters */
#define BMI2_ACC_OSR4_AVG1             UINT8_C(0x00)
#define BMI2_ACC_OSR2_AVG2             UINT8_C(0x01)
#define BMI2_ACC_NORMAL_AVG4           UINT8_C(0x02)
#define BMI2_ACC_CIC_AVG8              UINT8_C(0x03)
#define BMI2_ACC_RES_AVG16             UINT8_C(0x04)
#define BMI2_ACC_RES_AVG32             UINT8_C(0x05)
#define BMI2_ACC_RES_AVG64             UINT8_C(0x06)
#define BMI2_ACC_RES_AVG128            UINT8_C(0x07)

/*! @name Accelerometer Output Data Rate */
#define BMI2_ACC_ODR_0_78HZ            UINT8_C(0x01)
#define BMI2_ACC_ODR_1_56HZ            UINT8_C(0x02)
#define BMI2_ACC_ODR_3_12HZ            UINT8_C(0x03)
#define BMI2_ACC_ODR_6_25HZ            UINT8_C(0x04)
#define BMI2_ACC_ODR_12_5HZ            UINT8_C(0x05)
#define BMI2_ACC_ODR_25HZ              UINT8_C(0x06)
#define BMI2_ACC_ODR_50HZ              UINT8_C(0x07)
#define BMI2_ACC_ODR_100HZ             UINT8_C(0x08)
#define BMI2_ACC_ODR_200HZ             UINT8_C(0x09)
#define BMI2_ACC_ODR_400HZ             UINT8_C(0x0A)
#define BMI2_ACC_ODR_800HZ             UINT8_C(0x0B)
#define BMI2_ACC_ODR_1600HZ            UINT8_C(0x0C)

/*! @name Accelerometer G Range */
#define BMI2_ACC_RANGE_2G              UINT8_C(0x00)
#define BMI2_ACC_RANGE_4G              UINT8_C(0x01)
#define BMI2_ACC_RANGE_8G              UINT8_C(0x02)
#define BMI2_ACC_RANGE_16G             UINT8_C(0x03)

/*! @name Mask definitions for accelerometer configuration register */
#define BMI2_ACC_RANGE_MASK            UINT8_C(0x03)
#define BMI2_ACC_ODR_MASK              UINT8_C(0x0F)
#define BMI2_ACC_BW_PARAM_MASK         UINT8_C(0x70)
#define BMI2_ACC_FILTER_PERF_MODE_MASK UINT8_C(0x80)

/*! @name Bit position definitions for accelerometer configuration register */
#define BMI2_ACC_BW_PARAM_POS          UINT8_C(0x04)
#define BMI2_ACC_FILTER_PERF_MODE_POS  UINT8_C(0x07)

/*! @name Self test macro to define range */
#define BMI2_ACC_SELF_TEST_RANGE       UINT8_C(16)

/*! @name Self test macro to show resulting minimum and maximum difference
 * signal of the axes in mg
 */
#define BMI2_ST_ACC_X_SIG_MIN_DIFF     INT16_C(1800)
#define BMI2_ST_ACC_X_SIG_MAX_DIFF     INT16_C(10200)

#define BMI2_ST_ACC_Y_SIG_MIN_DIFF     INT16_C(-10200)
#define BMI2_ST_ACC_Y_SIG_MAX_DIFF     INT16_C(-1800)

#define BMI2_ST_ACC_Z_SIG_MIN_DIFF     INT16_C(800)
#define BMI2_ST_ACC_Z_SIG_MAX_DIFF     INT16_C(10200)

/*! @name Mask definitions for accelerometer self-test */
#define BMI2_ACC_SELF_TEST_EN_MASK     UINT8_C(0x01)
#define BMI2_ACC_SELF_TEST_SIGN_MASK   UINT8_C(0x04)
#define BMI2_ACC_SELF_TEST_AMP_MASK    UINT8_C(0x08)

/*! @name Bit Positions for accelerometer self-test */
#define BMI2_ACC_SELF_TEST_SIGN_POS    UINT8_C(0x02)
#define BMI2_ACC_SELF_TEST_AMP_POS     UINT8_C(0x03)

/*! @name MASK definition for gyro self test status  */
#define BMI2_GYR_ST_AXES_DONE_MASK     UINT8_C(0X01)
#define BMI2_GYR_AXIS_X_OK_MASK        UINT8_C(0x02)
#define BMI2_GYR_AXIS_Y_OK_MASK        UINT8_C(0x04)
#define BMI2_GYR_AXIS_Z_OK_MASK        UINT8_C(0x08)

/*! @name Bit position for gyro self test status  */
#define BMI2_GYR_AXIS_X_OK_POS         UINT8_C(0x01)
#define BMI2_GYR_AXIS_Y_OK_POS         UINT8_C(0x02)
#define BMI2_GYR_AXIS_Z_OK_POS         UINT8_C(0x03)

/******************************************************************************/
/*! @name       Gyroscope Macro Definitions               */
/******************************************************************************/
/*! @name Gyroscope Bandwidth parameters */
#define BMI2_GYR_OSR4_MODE             UINT8_C(0x00)
#define BMI2_GYR_OSR2_MODE             UINT8_C(0x01)
#define BMI2_GYR_NORMAL_MODE           UINT8_C(0x02)
#define BMI2_GYR_CIC_MODE              UINT8_C(0x03)

/*! @name Gyroscope Output Data Rate */
#define BMI2_GYR_ODR_25HZ              UINT8_C(0x06)
#define BMI2_GYR_ODR_50HZ              UINT8_C(0x07)
#define BMI2_GYR_ODR_100HZ             UINT8_C(0x08)
#define BMI2_GYR_ODR_200HZ             UINT8_C(0x09)
#define BMI2_GYR_ODR_400HZ             UINT8_C(0x0A)
#define BMI2_GYR_ODR_800HZ             UINT8_C(0x0B)
#define BMI2_GYR_ODR_1600HZ            UINT8_C(0x0C)
#define BMI2_GYR_ODR_3200HZ            UINT8_C(0x0D)

/*! @name Gyroscope OIS Range */
#define BMI2_GYR_OIS_250               UINT8_C(0x00)
#define BMI2_GYR_OIS_2000              UINT8_C(0x01)

/*! @name Gyroscope Angular Rate Measurement Range */
#define BMI2_GYR_RANGE_2000            UINT8_C(0x00)
#define BMI2_GYR_RANGE_1000            UINT8_C(0x01)
#define BMI2_GYR_RANGE_500             UINT8_C(0x02)
#define BMI2_GYR_RANGE_250             UINT8_C(0x03)
#define BMI2_GYR_RANGE_125             UINT8_C(0x04)

/*! @name Mask definitions for gyroscope configuration register */
#define BMI2_GYR_RANGE_MASK            UINT8_C(0x07)
#define BMI2_GYR_OIS_RANGE_MASK        UINT8_C(0x08)
#define BMI2_GYR_ODR_MASK              UINT8_C(0x0F)
#define BMI2_GYR_BW_PARAM_MASK         UINT8_C(0x30)
#define BMI2_GYR_NOISE_PERF_MODE_MASK  UINT8_C(0x40)
#define BMI2_GYR_FILTER_PERF_MODE_MASK UINT8_C(0x80)

/*! @name Bit position definitions for gyroscope configuration register */
#define BMI2_GYR_OIS_RANGE_POS         UINT8_C(0x03)
#define BMI2_GYR_BW_PARAM_POS          UINT8_C(0x04)
#define BMI2_GYR_NOISE_PERF_MODE_POS   UINT8_C(0x06)
#define BMI2_GYR_FILTER_PERF_MODE_POS  UINT8_C(0x07)

/******************************************************************************/
/*! @name       Auxiliary Macro Definitions               */
/******************************************************************************/
/*! @name Auxiliary Output Data Rate */
#define BMI2_AUX_ODR_RESERVED         UINT8_C(0x00)
#define BMI2_AUX_ODR_0_78HZ           UINT8_C(0x01)
#define BMI2_AUX_ODR_1_56HZ           UINT8_C(0x02)
#define BMI2_AUX_ODR_3_12HZ           UINT8_C(0x03)
#define BMI2_AUX_ODR_6_25HZ           UINT8_C(0x04)
#define BMI2_AUX_ODR_12_5HZ           UINT8_C(0x05)
#define BMI2_AUX_ODR_25HZ             UINT8_C(0x06)
#define BMI2_AUX_ODR_50HZ             UINT8_C(0x07)
#define BMI2_AUX_ODR_100HZ            UINT8_C(0x08)
#define BMI2_AUX_ODR_200HZ            UINT8_C(0x09)
#define BMI2_AUX_ODR_400HZ            UINT8_C(0x0A)
#define BMI2_AUX_ODR_800HZ            UINT8_C(0x0B)

/*! @name Macro to define burst read lengths for both manual and auto modes */
#define BMI2_AUX_READ_LEN_0           UINT8_C(0x00)
#define BMI2_AUX_READ_LEN_1           UINT8_C(0x01)
#define BMI2_AUX_READ_LEN_2           UINT8_C(0x02)
#define BMI2_AUX_READ_LEN_3           UINT8_C(0x03)

/*! @name Mask definitions for auxiliary interface configuration register */
#define BMI2_AUX_SET_I2C_ADDR_MASK    UINT8_C(0xFE)
#define BMI2_AUX_MAN_MODE_EN_MASK     UINT8_C(0x80)
#define BMI2_AUX_FCU_WR_EN_MASK       UINT8_C(0x40)
#define BMI2_AUX_MAN_READ_BURST_MASK  UINT8_C(0x0C)
#define BMI2_AUX_READ_BURST_MASK      UINT8_C(0x03)
#define BMI2_AUX_ODR_EN_MASK          UINT8_C(0x0F)
#define BMI2_AUX_OFFSET_READ_OUT_MASK UINT8_C(0xF0)

/*! @name Bit positions for auxiliary interface configuration register */
#define BMI2_AUX_SET_I2C_ADDR_POS     UINT8_C(0x01)
#define BMI2_AUX_MAN_MODE_EN_POS      UINT8_C(0x07)
#define BMI2_AUX_FCU_WR_EN_POS        UINT8_C(0x06)
#define BMI2_AUX_MAN_READ_BURST_POS   UINT8_C(0x02)
#define BMI2_AUX_OFFSET_READ_OUT_POS  UINT8_C(0x04)

/******************************************************************************/
/*! @name       FIFO Macro Definitions                                        */
/******************************************************************************/
/*! @name Macros to define virtual FIFO frame mode */
#define BMI2_FIFO_VIRT_FRM_MODE         UINT8_C(0x03)

/*! @name FIFO Header Mask definitions */
#define BMI2_FIFO_HEADER_ACC_FRM        UINT8_C(0x84)
#define BMI2_FIFO_HEADER_AUX_FRM        UINT8_C(0x90)
#define BMI2_FIFO_HEADER_GYR_FRM        UINT8_C(0x88)
#define BMI2_FIFO_HEADER_GYR_ACC_FRM    UINT8_C(0x8C)
#define BMI2_FIFO_HEADER_AUX_ACC_FRM    UINT8_C(0x94)
#define BMI2_FIFO_HEADER_AUX_GYR_FRM    UINT8_C(0x98)
#define BMI2_FIFO_HEADER_ALL_FRM        UINT8_C(0x9C)
#define BMI2_FIFO_HEADER_SENS_TIME_FRM  UINT8_C(0x44)
#define BMI2_FIFO_HEADER_SKIP_FRM       UINT8_C(0x40)
#define BMI2_FIFO_HEADER_INPUT_CFG_FRM  UINT8_C(0x48)
#define BMI2_FIFO_HEAD_OVER_READ_MSB    UINT8_C(0x80)
#define BMI2_FIFO_VIRT_ACT_RECOG_FRM    UINT8_C(0xC8)

/*! @name BMI2 sensor selection for header-less frames  */
#define BMI2_FIFO_HEAD_LESS_ACC_FRM     UINT8_C(0x40)
#define BMI2_FIFO_HEAD_LESS_AUX_FRM     UINT8_C(0x20)
#define BMI2_FIFO_HEAD_LESS_GYR_FRM     UINT8_C(0x80)
#define BMI2_FIFO_HEAD_LESS_GYR_AUX_FRM UINT8_C(0xA0)
#define BMI2_FIFO_HEAD_LESS_GYR_ACC_FRM UINT8_C(0xC0)
#define BMI2_FIFO_HEAD_LESS_AUX_ACC_FRM UINT8_C(0x60)
#define BMI2_FIFO_HEAD_LESS_ALL_FRM     UINT8_C(0xE0)

/*! @name Mask definitions for FIFO frame content configuration */
#define BMI2_FIFO_STOP_ON_FULL          UINT16_C(0x0001)
#define BMI2_FIFO_TIME_EN               UINT16_C(0x0002)
#define BMI2_FIFO_TAG_INT1              UINT16_C(0x0300)
#define BMI2_FIFO_TAG_INT2              UINT16_C(0x0C00)
#define BMI2_FIFO_HEADER_EN             UINT16_C(0x1000)
#define BMI2_FIFO_AUX_EN                UINT16_C(0x2000)
#define BMI2_FIFO_ACC_EN                UINT16_C(0x4000)
#define BMI2_FIFO_GYR_EN                UINT16_C(0x8000)
#define BMI2_FIFO_ALL_EN                UINT16_C(0xE000)

/*! @name FIFO sensor data lengths */
#define BMI2_FIFO_ACC_LENGTH            UINT8_C(6)
#define BMI2_FIFO_GYR_LENGTH            UINT8_C(6)
#define BMI2_FIFO_AUX_LENGTH            UINT8_C(8)
#define BMI2_FIFO_ACC_AUX_LENGTH        UINT8_C(14)
#define BMI2_FIFO_GYR_AUX_LENGTH        UINT8_C(14)
#define BMI2_FIFO_ACC_GYR_LENGTH        UINT8_C(12)
#define BMI2_FIFO_ALL_LENGTH            UINT8_C(20)
#define BMI2_SENSOR_TIME_LENGTH         UINT8_C(3)
#define BMI2_FIFO_CONFIG_LENGTH         UINT8_C(2)
#define BMI2_FIFO_WM_LENGTH             UINT8_C(2)
#define BMI2_MAX_VALUE_FIFO_FILTER      UINT8_C(1)
#define BMI2_FIFO_DATA_LENGTH           UINT8_C(2)
#define BMI2_FIFO_LENGTH_MSB_BYTE       UINT8_C(1)
#define BMI2_FIFO_INPUT_CFG_LENGTH      UINT8_C(4)
#define BMI2_FIFO_SKIP_FRM_LENGTH       UINT8_C(1)

/*! @name FIFO sensor virtual data lengths: sensor data plus sensor time */
#define BMI2_FIFO_VIRT_ACC_LENGTH       UINT8_C(9)
#define BMI2_FIFO_VIRT_GYR_LENGTH       UINT8_C(9)
#define BMI2_FIFO_VIRT_AUX_LENGTH       UINT8_C(11)
#define BMI2_FIFO_VIRT_ACC_AUX_LENGTH   UINT8_C(17)
#define BMI2_FIFO_VIRT_GYR_AUX_LENGTH   UINT8_C(17)
#define BMI2_FIFO_VIRT_ACC_GYR_LENGTH   UINT8_C(15)
#define BMI2_FIFO_VIRT_ALL_LENGTH       UINT8_C(23)

/*! @name FIFO sensor virtual data lengths: activity recognition */
#define BMI2_FIFO_VIRT_ACT_DATA_LENGTH  UINT8_C(6)
#define BMI2_FIFO_VIRT_ACT_TIME_LENGTH  UINT8_C(4)
#define BMI2_FIFO_VIRT_ACT_TYPE_LENGTH  UINT8_C(1)
#define BMI2_FIFO_VIRT_ACT_STAT_LENGTH  UINT8_C(1)

/*! @name BMI2 FIFO data filter modes */
#define BMI2_FIFO_UNFILTERED_DATA       UINT8_C(0)
#define BMI2_FIFO_FILTERED_DATA         UINT8_C(1)

/*! @name FIFO frame masks */
#define BMI2_FIFO_LSB_CONFIG_CHECK      UINT8_C(0x00)
#define BMI2_FIFO_MSB_CONFIG_CHECK      UINT8_C(0x80)
#define BMI2_FIFO_TAG_INTR_MASK         UINT8_C(0xFF)

/*! @name BMI2 Mask definitions of FIFO configuration registers */
#define BMI2_FIFO_CONFIG_0_MASK         UINT16_C(0x0003)
#define BMI2_FIFO_CONFIG_1_MASK         UINT16_C(0xFF00)

/*! @name FIFO self wake-up mask definition */
#define BMI2_FIFO_SELF_WAKE_UP_MASK     UINT8_C(0x02)

/*! @name FIFO down sampling mask definition */
#define BMI2_ACC_FIFO_DOWNS_MASK        UINT8_C(0x70)
#define BMI2_GYR_FIFO_DOWNS_MASK        UINT8_C(0x07)

/*! @name FIFO down sampling bit positions */
#define BMI2_ACC_FIFO_DOWNS_POS         UINT8_C(0x04)

/*! @name FIFO filter mask definition */
#define BMI2_ACC_FIFO_FILT_DATA_MASK    UINT8_C(0x80)
#define BMI2_GYR_FIFO_FILT_DATA_MASK    UINT8_C(0x08)

/*! @name FIFO filter bit positions */
#define BMI2_ACC_FIFO_FILT_DATA_POS     UINT8_C(0x07)
#define BMI2_GYR_FIFO_FILT_DATA_POS     UINT8_C(0x03)

/*! @name FIFO byte counter mask definition */
#define BMI2_FIFO_BYTE_COUNTER_MSB_MASK UINT8_C(0x3F)

/*! @name FIFO self wake-up bit positions */
#define BMI2_FIFO_SELF_WAKE_UP_POS      UINT8_C(0x01)

/*! @name Mask Definitions for Virtual FIFO frames */
#define BMI2_FIFO_VIRT_FRM_MODE_MASK    UINT8_C(0xC0)
#define BMI2_FIFO_VIRT_PAYLOAD_MASK     UINT8_C(0x3C)

/*! @name Bit Positions for Virtual FIFO frames */
#define BMI2_FIFO_VIRT_FRM_MODE_POS     UINT8_C(0x06)
#define BMI2_FIFO_VIRT_PAYLOAD_POS      UINT8_C(0x02)

/******************************************************************************/
/*! @name        Interrupt Macro Definitions                  */
/******************************************************************************/
/*! @name BMI2 Interrupt Modes */
/* Non latched */
#define BMI2_INT_NON_LATCH         UINT8_C(0)

/* Permanently latched */
#define BMI2_INT_LATCH             UINT8_C(1)

/*! @name BMI2 Interrupt Pin Behavior */
#define BMI2_INT_PUSH_PULL         UINT8_C(0)
#define BMI2_INT_OPEN_DRAIN        UINT8_C(1)

/*! @name BMI2 Interrupt Pin Level */
#define BMI2_INT_ACTIVE_LOW        UINT8_C(0)
#define BMI2_INT_ACTIVE_HIGH       UINT8_C(1)

/*! @name BMI2 Interrupt Output Enable */
#define BMI2_INT_OUTPUT_DISABLE    UINT8_C(0)
#define BMI2_INT_OUTPUT_ENABLE     UINT8_C(1)

/*! @name BMI2 Interrupt Input Enable */
#define BMI2_INT_INPUT_DISABLE     UINT8_C(0)
#define BMI2_INT_INPUT_ENABLE      UINT8_C(1)

/*! @name Mask definitions for interrupt pin configuration */
#define BMI2_INT_LATCH_MASK        UINT8_C(0x01)
#define BMI2_INT_LEVEL_MASK        UINT8_C(0x02)
#define BMI2_INT_OPEN_DRAIN_MASK   UINT8_C(0x04)
#define BMI2_INT_OUTPUT_EN_MASK    UINT8_C(0x08)
#define BMI2_INT_INPUT_EN_MASK     UINT8_C(0x10)

/*! @name Bit position definitions for interrupt pin configuration */
#define BMI2_INT_LEVEL_POS         UINT8_C(0x01)
#define BMI2_INT_OPEN_DRAIN_POS    UINT8_C(0x02)
#define BMI2_INT_OUTPUT_EN_POS     UINT8_C(0x03)
#define BMI2_INT_INPUT_EN_POS      UINT8_C(0x04)

/*! @name Mask definitions for data interrupt mapping */
#define BMI2_FFULL_INT             UINT8_C(0x01)
#define BMI2_FWM_INT               UINT8_C(0x02)
#define BMI2_DRDY_INT              UINT8_C(0x04)
#define BMI2_ERR_INT               UINT8_C(0x08)

/*! @name Mask definitions for data interrupt status bits */
#define BMI2_FFULL_INT_STATUS_MASK UINT16_C(0x0100)
#define BMI2_FWM_INT_STATUS_MASK   UINT16_C(0x0200)
#define BMI2_ERR_INT_STATUS_MASK   UINT16_C(0x0400)
#define BMI2_AUX_DRDY_INT_MASK     UINT16_C(0x2000)
#define BMI2_GYR_DRDY_INT_MASK     UINT16_C(0x4000)
#define BMI2_ACC_DRDY_INT_MASK     UINT16_C(0x8000)

/*!  @name Maximum number of interrupt pins */
#define BMI2_INT_PIN_MAX_NUM       UINT8_C(2)

/*!  @name Macro for mapping feature interrupts */
#define BMI2_FEAT_BIT_DISABLE      UINT8_C(0)
#define BMI2_FEAT_BIT0             UINT8_C(1)
#define BMI2_FEAT_BIT1             UINT8_C(2)
#define BMI2_FEAT_BIT2             UINT8_C(3)
#define BMI2_FEAT_BIT3             UINT8_C(4)
#define BMI2_FEAT_BIT4             UINT8_C(5)
#define BMI2_FEAT_BIT5             UINT8_C(6)
#define BMI2_FEAT_BIT6             UINT8_C(7)
#define BMI2_FEAT_BIT7             UINT8_C(8)
#define BMI2_FEAT_BIT_MAX          UINT8_C(9)

/******************************************************************************/
/*! @name               OIS Interface Macro Definitions                       */
/******************************************************************************/
/*! @name Mask definitions for interface configuration register */
#define BMI2_OIS_IF_EN_MASK UINT8_C(0x10)
#define BMI2_AUX_IF_EN_MASK UINT8_C(0x20)

/*! @name Bit positions for OIS interface enable */
#define BMI2_OIS_IF_EN_POS  UINT8_C(0x04)
#define BMI2_AUX_IF_EN_POS  UINT8_C(0x05)

/******************************************************************************/
/*! @name       Macro Definitions for Axes re-mapping             */
/******************************************************************************/
/*! @name Macros for the user-defined values of axes and their polarities */
#define BMI2_X         UINT8_C(0x01)
#define BMI2_NEG_X     UINT8_C(0x09)
#define BMI2_Y         UINT8_C(0x02)
#define BMI2_NEG_Y     UINT8_C(0x0A)
#define BMI2_Z         UINT8_C(0x04)
#define BMI2_NEG_Z     UINT8_C(0x0C)
#define BMI2_AXIS_MASK UINT8_C(0x07)
#define BMI2_AXIS_SIGN UINT8_C(0x08)

/******************************************************************************/
/*! @name         Macro Definitions for offset and gain compensation          */
/******************************************************************************/
/*! @name Mask definitions of gyroscope offset compensation registers */
#define BMI2_GYR_GAIN_EN_MASK       UINT8_C(0x80)
#define BMI2_GYR_OFF_COMP_EN_MASK   UINT8_C(0x40)

/*! @name Bit positions of gyroscope offset compensation registers */
#define BMI2_GYR_OFF_COMP_EN_POS    UINT8_C(0x06)

/*! @name Mask definitions of gyroscope user-gain registers */
#define BMI2_GYR_USR_GAIN_X_MASK    UINT8_C(0x7F)
#define BMI2_GYR_USR_GAIN_Y_MASK    UINT8_C(0x7F)
#define BMI2_GYR_USR_GAIN_Z_MASK    UINT8_C(0x7F)

/*! @name Bit positions of gyroscope offset compensation registers */
#define BMI2_GYR_GAIN_EN_POS        UINT8_C(0x07)

/******************************************************************************/
/*! @name       Macro Definitions for internal status                 */
/******************************************************************************/
#define BMI2_NOT_INIT               UINT8_C(0x00)
#define BMI2_INIT_OK                UINT8_C(0x01)
#define BMI2_INIT_ERR               UINT8_C(0x02)
#define BMI2_DRV_ERR                UINT8_C(0x03)
#define BMI2_SNS_STOP               UINT8_C(0x04)
#define BMI2_NVM_ERROR              UINT8_C(0x05)
#define BMI2_START_UP_ERROR         UINT8_C(0x06)
#define BMI2_COMPAT_ERROR           UINT8_C(0x07)
#define BMI2_VFM_SKIPPED            UINT8_C(0x10)
#define BMI2_AXES_MAP_ERROR         UINT8_C(0x20)
#define BMI2_ODR_50_HZ_ERROR        UINT8_C(0x40)
#define BMI2_ODR_HIGH_ERROR         UINT8_C(0x80)

/******************************************************************************/
/*! @name        error status form gyro gain update status.               */
/******************************************************************************/
#define BMI2_G_TRIGGER_NO_ERROR     UINT8_C(0x00)

#define BMI2_G_TRIGGER_PRECON_ERROR UINT8_C(0x01)
#define BMI2_G_TRIGGER_DL_ERROR     UINT8_C(0x02)
#define BMI2_G_TRIGGER_ABORT_ERROR  UINT8_C(0x03)

/******************************************************************************/
/*! @name       Variant specific features selection macros            */
/******************************************************************************/
#define BMI2_CRT_RTOSK_ENABLE       UINT8_C(0x01)
#define BMI2_GYRO_CROSS_SENS_ENABLE UINT8_C(0x02)
#define BMI_GYRO_USER_GAIN_ENABLE   UINT8_C(0x08)
#define BMI2_NO_FEATURE_ENABLE      UINT8_C(0x00)
#define BMI2_CRT_IN_FIFO_NOT_REQ    UINT8_C(0x10)

/*! @name Offsets from feature start address for BMI2 feature enable/disable */
#define ANY_MOT_FEAT_EN_OFFSET           UINT8_C(0x03)
#define NO_MOT_FEAT_EN_OFFSET            UINT8_C(0x03)
#define SIG_MOT_FEAT_EN_OFFSET           UINT8_C(0x0A)
#define STEP_COUNT_FEAT_EN_OFFSET        UINT8_C(0x01)
#define GYR_USER_GAIN_FEAT_EN_OFFSET     UINT8_C(0x05)
#define HIGH_G_FEAT_EN_OFFSET            UINT8_C(0x03)
#define LOW_G_FEAT_EN_OFFSET             UINT8_C(0x03)

/*! @name Mask definitions for BMI2 feature enable/disable */
#define ANY_NO_MOT_EN_MASK               UINT8_C(0x80)
#define TILT_FEAT_EN_MASK                UINT8_C(0x01)
#define ORIENT_FEAT_EN_MASK              UINT8_C(0x01)
#define SIG_MOT_FEAT_EN_MASK             UINT8_C(0x01)
#define STEP_DET_FEAT_EN_MASK            UINT8_C(0x08)
#define STEP_COUNT_FEAT_EN_MASK          UINT8_C(0x10)
#define STEP_ACT_FEAT_EN_MASK            UINT8_C(0x20)
#define GYR_USER_GAIN_FEAT_EN_MASK       UINT8_C(0x08)
#define UP_HOLD_TO_WAKE_FEAT_EN_MASK     UINT8_C(0x01)
#define GLANCE_FEAT_EN_MASK              UINT8_C(0x01)
#define WAKE_UP_FEAT_EN_MASK             UINT8_C(0x01)
#define HIGH_G_FEAT_EN_MASK              UINT8_C(0x80)
#define LOW_G_FEAT_EN_MASK               UINT8_C(0x10)
#define FLAT_FEAT_EN_MASK                UINT8_C(0x01)
#define EXT_SENS_SYNC_FEAT_EN_MASK       UINT8_C(0x01)
#define GYR_SELF_OFF_CORR_FEAT_EN_MASK   UINT8_C(0x02)
#define WRIST_GEST_FEAT_EN_MASK          UINT8_C(0x20)
#define WRIST_WEAR_WAKE_UP_FEAT_EN_MASK  UINT8_C(0x10)
#define ACTIVITY_RECOG_EN_MASK           UINT8_C(0x01)
#define ACC_SELF_TEST_FEAT_EN_MASK       UINT8_C(0x02)
#define GYRO_SELF_TEST_CRT_EN_MASK       UINT8_C(0x01)
#define ABORT_FEATURE_EN_MASK            UINT8_C(0x02)
#define NVM_PREP_FEATURE_EN_MASK         UINT8_C(0x04)

/*! @name Bit position definitions for BMI2 feature enable/disable */
#define ANY_NO_MOT_EN_POS                UINT8_C(0x07)
#define STEP_DET_FEAT_EN_POS             UINT8_C(0x03)
#define STEP_COUNT_FEAT_EN_POS           UINT8_C(0x04)
#define STEP_ACT_FEAT_EN_POS             UINT8_C(0x05)
#define GYR_USER_GAIN_FEAT_EN_POS        UINT8_C(0x03)
#define HIGH_G_FEAT_EN_POS               UINT8_C(0x07)
#define LOW_G_FEAT_EN_POS                UINT8_C(0x04)
#define GYR_SELF_OFF_CORR_FEAT_EN_POS    UINT8_C(0x01)
#define WRIST_GEST_FEAT_EN_POS           UINT8_C(0x05)
#define WRIST_WEAR_WAKE_UP_FEAT_EN_POS   UINT8_C(0x04)
#define ACC_SELF_TEST_FEAT_EN_POS        UINT8_C(0x01)
#define ABORT_FEATURE_EN_POS             UINT8_C(0x1)
#define NVM_PREP_FEATURE_EN_POS          UINT8_C(0x02)

/*! @name Mask definitions for BMI2 any and no-motion feature configuration */
#define ANY_NO_MOT_DUR_MASK              UINT16_C(0x1FFF)
#define ANY_NO_MOT_X_SEL_MASK            UINT16_C(0x2000)
#define ANY_NO_MOT_Y_SEL_MASK            UINT16_C(0x4000)
#define ANY_NO_MOT_Z_SEL_MASK            UINT16_C(0x8000)
#define ANY_NO_MOT_THRES_MASK            UINT16_C(0x07FF)
#define ANY_NO_MOT_OUT_CONF_MASK         UINT16_C(0x7800)

/*! @name Bit position definitions for BMI2 any and no-motion feature
 * configuration
 */
#define ANY_NO_MOT_X_SEL_POS             UINT8_C(0x0D)
#define ANY_NO_MOT_Y_SEL_POS             UINT8_C(0x0E)
#define ANY_NO_MOT_Z_SEL_POS             UINT8_C(0x0F)
#define ANY_NO_MOT_OUT_CONF_POS          UINT8_C(0x0B)

/*! @name Mask definitions for BMI2 tilt feature configuration */
#define TILT_OUT_CONF_MASK               UINT16_C(0x001E)

/*! @name Bit position definitions for BMI2 tilt feature configuration */
#define TILT_OUT_CONF_POS                UINT8_C(0x01)

/*! @name Mask definitions for BMI2 orientation feature configuration */
#define ORIENT_UP_DOWN_MASK              UINT16_C(0x0002)
#define ORIENT_SYMM_MODE_MASK            UINT16_C(0x000C)
#define ORIENT_BLOCK_MODE_MASK           UINT16_C(0x0030)
#define ORIENT_THETA_MASK                UINT16_C(0x0FC0)
#define ORIENT_HYST_MASK                 UINT16_C(0x07FF)
#define ORIENT_OUT_CONF_MASK             UINT16_C(0x7800)

/*! @name Bit position definitions for BMI2 orientation feature configuration */
#define ORIENT_UP_DOWN_POS               UINT8_C(0x01)
#define ORIENT_SYMM_MODE_POS             UINT8_C(0x02)
#define ORIENT_BLOCK_MODE_POS            UINT8_C(0x04)
#define ORIENT_THETA_POS                 UINT8_C(0x06)
#define ORIENT_OUT_CONF_POS              UINT8_C(0x0B)

/*! @name Mask definitions for BMI2 sig-motion feature configuration */
#define SIG_MOT_PARAM_1_MASK             UINT16_C(0xFFFF)
#define SIG_MOT_PARAM_2_MASK             UINT16_C(0xFFFF)
#define SIG_MOT_PARAM_3_MASK             UINT16_C(0xFFFF)
#define SIG_MOT_PARAM_4_MASK             UINT16_C(0xFFFF)
#define SIG_MOT_PARAM_5_MASK             UINT16_C(0xFFFF)
#define SIG_MOT_OUT_CONF_MASK            UINT16_C(0x001E)

/*! @name Bit position definitions for BMI2 sig-motion feature configuration */
#define SIG_MOT_OUT_CONF_POS             UINT8_C(0x01)

/*! @name Mask definitions for BMI2 parameter configurations */
#define STEP_COUNT_PARAMS_MASK           UINT16_C(0xFFFF)

/*! @name Mask definitions for BMI2 step-counter/detector feature configuration */
#define STEP_COUNT_WM_LEVEL_MASK         UINT16_C(0x03FF)
#define STEP_COUNT_RST_CNT_MASK          UINT16_C(0x0400)
#define STEP_DET_OUT_CONF_MASK           UINT16_C(0x000F)
#define STEP_ACT_OUT_CONF_MASK           UINT16_C(0x00F0)
#define STEP_BUFFER_SIZE_MASK            UINT16_C(0XFF00)

/*! @name Bit position definitions for BMI2 step-counter/detector feature
 * configuration
 */
#define STEP_COUNT_RST_CNT_POS           UINT8_C(0x0A)
#define STEP_ACT_OUT_CONF_POS            UINT8_C(0x04)
#define STEP_BUFFER_SIZE_POS             UINT8_C(0X08)

/*! @name Mask definitions for BMI2 gyroscope user gain feature
 * configuration
 */
#define GYR_USER_GAIN_RATIO_X_MASK       UINT16_C(0x07FF)
#define GYR_USER_GAIN_RATIO_Y_MASK       UINT16_C(0x07FF)
#define GYR_USER_GAIN_RATIO_Z_MASK       UINT16_C(0x07FF)

/*! @name Mask definitions for BMI2 gyroscope user gain saturation status */
#define GYR_USER_GAIN_SAT_STAT_X_MASK    UINT8_C(0x01)
#define GYR_USER_GAIN_SAT_STAT_Y_MASK    UINT8_C(0x02)
#define GYR_USER_GAIN_SAT_STAT_Z_MASK    UINT8_C(0x04)
#define G_TRIGGER_STAT_MASK              UINT8_C(0x38)

/*! @name Bit position definitions for BMI2 gyroscope user gain saturation status */
#define GYR_USER_GAIN_SAT_STAT_Y_POS     UINT8_C(0x01)
#define GYR_USER_GAIN_SAT_STAT_Z_POS     UINT8_C(0x02)
#define G_TRIGGER_STAT_POS               UINT8_C(0x03)

/*! @name Mask definitions for MSB values of BMI2 gyroscope compensation */
#define GYR_OFF_COMP_MSB_X_MASK          UINT8_C(0x03)
#define GYR_OFF_COMP_MSB_Y_MASK          UINT8_C(0x0C)
#define GYR_OFF_COMP_MSB_Z_MASK          UINT8_C(0x30)

/*! @name Bit positions for MSB values of BMI2 gyroscope compensation */
#define GYR_OFF_COMP_MSB_Y_POS           UINT8_C(0x02)
#define GYR_OFF_COMP_MSB_Z_POS           UINT8_C(0x04)

/*! @name Mask definitions for MSB values of BMI2 gyroscope compensation from user input */
#define GYR_OFF_COMP_MSB_MASK            UINT16_C(0x0300)
#define GYR_OFF_COMP_LSB_MASK            UINT16_C(0x00FF)

/*! @name Mask definitions for BMI2 orientation status */
#define BMI2_ORIENT_DETECT_MASK          UINT8_C(0x03)
#define BMI2_ORIENT_FACE_UP_DWN_MASK     UINT8_C(0x04)

/*! @name Bit position definitions for BMI2 orientation status */
#define BMI2_ORIENT_FACE_UP_DWN_POS      UINT8_C(0x02)

/*! @name Mask definitions for NVM-VFRM error status */
#define NVM_LOAD_ERR_STATUS_MASK         UINT8_C(0x01)
#define NVM_PROG_ERR_STATUS_MASK         UINT8_C(0x02)
#define NVM_ERASE_ERR_STATUS_MASK        UINT8_C(0x04)
#define NVM_END_EXCEED_STATUS_MASK       UINT8_C(0x08)
#define NVM_PRIV_ERR_STATUS_MASK         UINT8_C(0x10)
#define VFRM_LOCK_ERR_STATUS_MASK        UINT8_C(0x20)
#define VFRM_WRITE_ERR_STATUS_MASK       UINT8_C(0x40)
#define VFRM_FATAL_ERR_STATUS_MASK       UINT8_C(0x80)

/*! @name Bit positions for NVM-VFRM error status */
#define NVM_PROG_ERR_STATUS_POS          UINT8_C(0x01)
#define NVM_ERASE_ERR_STATUS_POS         UINT8_C(0x02)
#define NVM_END_EXCEED_STATUS_POS        UINT8_C(0x03)
#define NVM_PRIV_ERR_STATUS_POS          UINT8_C(0x04)
#define VFRM_LOCK_ERR_STATUS_POS         UINT8_C(0x05)
#define VFRM_WRITE_ERR_STATUS_POS        UINT8_C(0x06)
#define VFRM_FATAL_ERR_STATUS_POS        UINT8_C(0x07)

/*! @name Mask definitions for accelerometer self test status */
#define ACC_SELF_TEST_DONE_MASK          UINT8_C(0x01)
#define ACC_X_OK_MASK                    UINT8_C(0x02)
#define ACC_Y_OK_MASK                    UINT8_C(0x04)
#define ACC_Z_OK_MASK                    UINT8_C(0x08)

/*! @name Bit Positions for accelerometer self test status */
#define ACC_X_OK_POS                     UINT8_C(0x01)
#define ACC_Y_OK_POS                     UINT8_C(0x02)
#define ACC_Z_OK_POS                     UINT8_C(0x03)

/*! @name Mask definitions for BMI2 uphold to wake feature configuration */
#define UP_HOLD_TO_WAKE_OUT_CONF_MASK    UINT16_C(0x001E)

/*! @name Bit position definitions for BMI2 uphold to wake feature configuration */
#define UP_HOLD_TO_WAKE_OUT_CONF_POS     UINT8_C(0x01)

/*! @name Mask definitions for BMI2 glance detector feature configuration */
#define GLANCE_DET_OUT_CONF_MASK         UINT16_C(0x001E)

/*! @name Bit position definitions for BMI2 glance detector feature
 * configuration
 */
#define GLANCE_DET_OUT_CONF_POS          UINT8_C(0x01)

/*! @name Mask definitions for BMI2 wake-up feature configuration */
#define WAKE_UP_SENSITIVITY_MASK         UINT16_C(0x000E)
#define WAKE_UP_SINGLE_TAP_EN_MASK       UINT16_C(0x0010)
#define WAKE_UP_OUT_CONF_MASK            UINT16_C(0x01E0)

/*! @name Bit position definitions for BMI2 wake-up feature configuration */
#define WAKE_UP_SENSITIVITY_POS          UINT8_C(0x01)
#define WAKE_UP_SINGLE_TAP_EN_POS        UINT8_C(0x04)
#define WAKE_UP_OUT_CONF_POS             UINT8_C(0x05)

/*! @name Mask definitions for BMI2 high-g feature configuration */
#define HIGH_G_THRES_MASK                UINT16_C(0x7FFF)
#define HIGH_G_HYST_MASK                 UINT16_C(0x0FFF)
#define HIGH_G_X_SEL_MASK                UINT16_C(0x1000)
#define HIGH_G_Y_SEL_MASK                UINT16_C(0x2000)
#define HIGH_G_Z_SEL_MASK                UINT16_C(0x4000)
#define HIGH_G_DUR_MASK                  UINT16_C(0x0FFF)
#define HIGH_G_OUT_CONF_MASK             UINT16_C(0xF000)

/*! @name Bit position definitions for BMI2 high-g feature configuration */
#define HIGH_G_OUT_CONF_POS              UINT8_C(0x0C)
#define HIGH_G_X_SEL_POS                 UINT8_C(0x0C)
#define HIGH_G_Y_SEL_POS                 UINT8_C(0x0D)
#define HIGH_G_Z_SEL_POS                 UINT8_C(0x0E)

/*! @name Mask definitions for BMI2 low-g feature configuration */
#define LOW_G_THRES_MASK                 UINT16_C(0x7FFF)
#define LOW_G_HYST_MASK                  UINT16_C(0x0FFF)
#define LOW_G_DUR_MASK                   UINT16_C(0x0FFF)
#define LOW_G_OUT_CONF_MASK              UINT16_C(0xF000)

/*! @name Bit position definitions for BMI2 low-g feature configuration */
#define LOW_G_OUT_CONF_POS               UINT8_C(0x0C)

/*! @name Mask definitions for BMI2 flat feature configuration */
#define FLAT_THETA_MASK                  UINT16_C(0x007E)
#define FLAT_BLOCK_MASK                  UINT16_C(0x0180)
#define FLAT_OUT_CONF_MASK               UINT16_C(0x1E00)
#define FLAT_HYST_MASK                   UINT16_C(0x003F)
#define FLAT_HOLD_TIME_MASK              UINT16_C(0x3FC0)

/*! @name Bit position definitions for BMI2 flat feature configuration */
#define FLAT_THETA_POS                   UINT8_C(0x01)
#define FLAT_BLOCK_POS                   UINT8_C(0x07)
#define FLAT_OUT_CONF_POS                UINT8_C(0x09)
#define FLAT_HOLD_TIME_POS               UINT8_C(0x06)

/*! @name Mask definitions for BMI2 external sensor sync configuration */
#define EXT_SENS_SYNC_OUT_CONF_MASK      UINT16_C(0x001E)

/*! @name Bit position definitions for external sensor sync configuration */
#define EXT_SENS_SYNC_OUT_CONF_POS       UINT8_C(0x01)

/*! @name Mask definitions for BMI2 wrist gesture configuration */
#define WRIST_GEST_WEAR_ARM_MASK         UINT16_C(0x0010)
#define WRIST_GEST_OUT_CONF_MASK         UINT16_C(0x000F)

/*! @name Bit position definitions for wrist gesture configuration */
#define WRIST_GEST_WEAR_ARM_POS          UINT8_C(0x04)
#define WRIST_GEST_OUT_CONF_POS          UINT8_C(0x00)

/*! @name Mask definitions for BMI2 wrist wear wake-up configuration */
#define WRIST_WAKE_UP_OUT_CONF_MASK      UINT16_C(0x000F)

/*! @name Bit position definitions for wrist wear wake-up configuration */
#define WRIST_WAKE_UP_OUT_CONF_POS       UINT8_C(0x00)

/*! @name Macros to define values of BMI2 axis and its sign for re-map
 * settings
 */
#define MAP_X_AXIS                       UINT8_C(0x00)
#define MAP_Y_AXIS                       UINT8_C(0x01)
#define MAP_Z_AXIS                       UINT8_C(0x02)
#define MAP_POSITIVE                     UINT8_C(0x00)
#define MAP_NEGATIVE                     UINT8_C(0x01)

/*! @name Mask definitions of BMI2 axis re-mapping */
#define X_AXIS_MASK                      UINT8_C(0x03)
#define X_AXIS_SIGN_MASK                 UINT8_C(0x04)
#define Y_AXIS_MASK                      UINT8_C(0x18)
#define Y_AXIS_SIGN_MASK                 UINT8_C(0x20)
#define Z_AXIS_MASK                      UINT8_C(0xC0)
#define Z_AXIS_SIGN_MASK                 UINT8_C(0x01)

/*! @name Bit position definitions of BMI2 axis re-mapping */
#define X_AXIS_SIGN_POS                  UINT8_C(0x02)
#define Y_AXIS_POS                       UINT8_C(0x03)
#define Y_AXIS_SIGN_POS                  UINT8_C(0x05)
#define Z_AXIS_POS                       UINT8_C(0x06)

/*! @name Macros to define polarity */
#define NEG_SIGN                         INT16_C(-1)
#define POS_SIGN                         INT16_C(1)

/*! @name Macro to define related to CRT */
#define CRT_READY_FOR_DOWNLOAD_MS        UINT8_C(2)
#define CRT_READY_FOR_DOWNLOAD_RETRY     UINT8_C(100)

#define CRT_WAIT_RUNNING_MS              UINT8_C(10)
#define CRT_WAIT_RUNNING_RETRY_EXECUTION UINT8_C(200)

#define CRT_MIN_BURST_WORD_LENGTH        UINT8_C(2)
#define CRT_MAX_BURST_WORD_LENGTH        UINT16_C(255)

#define ACC_FOC_2G_REF                   UINT16_C(16384)
#define ACC_FOC_4G_REF                   UINT16_C(8192)
#define ACC_FOC_8G_REF                   UINT16_C(4096)
#define ACC_FOC_16G_REF                  UINT16_C(2048)

#define ACC_FOC_2G_NOISE_LIMIT           UINT8_C(500)
#define ACC_FOC_4G_NOISE_LIMIT           UINT8_C(500)
#define ACC_FOC_8G_NOISE_LIMIT           UINT8_C(250)
#define ACC_FOC_16G_NOISE_LIMIT          UINT8_C(250)

/* reference value with positive and negative noise range in lsb */
#define ACC_2G_MAX_NOISE_LIMIT           UINT16_C(ACC_FOC_2G_REF + ACC_FOC_2G_NOISE_LIMIT)
#define ACC_2G_MIN_NOISE_LIMIT           UINT16_C(ACC_FOC_2G_REF - ACC_FOC_2G_NOISE_LIMIT)
#define ACC_4G_MAX_NOISE_LIMIT           UINT16_C(ACC_FOC_4G_REF + ACC_FOC_4G_NOISE_LIMIT)
#define ACC_4G_MIN_NOISE_LIMIT           UINT16_C(ACC_FOC_4G_REF - ACC_FOC_4G_NOISE_LIMIT)
#define ACC_8G_MAX_NOISE_LIMIT           UINT16_C(ACC_FOC_8G_REF + ACC_FOC_8G_NOISE_LIMIT)
#define ACC_8G_MIN_NOISE_LIMIT           UINT16_C(ACC_FOC_8G_REF - ACC_FOC_8G_NOISE_LIMIT)
#define ACC_16G_MAX_NOISE_LIMIT          UINT16_C(ACC_FOC_16G_REF + ACC_FOC_16G_NOISE_LIMIT)
#define ACC_16G_MIN_NOISE_LIMIT          UINT16_C(ACC_FOC_16G_REF - ACC_FOC_16G_NOISE_LIMIT)

#define BMI2_FOC_SAMPLE_LIMIT            UINT8_C(128)



#endif /* BMI2_DEFS_H_ */


