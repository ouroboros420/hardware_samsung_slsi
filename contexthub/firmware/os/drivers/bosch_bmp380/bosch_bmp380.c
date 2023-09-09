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
/*! \file bosch_bmp3.c
    \brief Bosch BMP3 sensor driver with nano-app */

/**
 * \page Bosch BMP3_userguide User/integration guide nano-app
 *
 * \section Bosch BMP3 Nano-App Introduction
 *   This nano-app is used to fetch pressure and temperature data from BMP3 and provide its output as input to other nano-apps which requests these sensors.
 *   This nano app send out the integer compensated pressure and temperature data output 
 *   - This is an internal nano-app.
 *   - This nano-app is designed to use BMP3 as a sensor with following SensorOps
 *
 *       For Pressure sensor:-
 *
 *      sensorPower          -> bmp3_sensor_power_baro
 *      sensorFirmwareUpload -> bmp3_sensor_firmware_baro
 *      sensorSetRate        -> bmp3_sensor_rate_baro
 *      sensorFlush          -> bmp3_sensor_flush_baro
 *      sensorCalibrate      -> bmp3_sensor_calibrate_baro
 *      sensorCfgData        -> bmp3_sensor_cfg_data_baro
 *
 *      sensorPower          -> bmp3_sensor_power_temp
 *      sensorFirmwareUpload -> bmp3_sensor_firmware_temp
 *      sensorSetRate        -> bmp3_sensor_rate_temp
 *      sensorFlush          -> bmp3_sensor_flush_temp
 *
 *   - The nano-app does the startup configuration for the BMP3 sensor. Configurations are
 *           => Sensor Operation in Forced mode
 *           => Oversampling settings set are pressure : 16x temperature : 2x
 *
 * \section Bosch BMP3 Events provided / outputs events
 *
 *  Separate events are provided for temperature and pressure
 *   - SENS_TYPE_TEMP 
 *   - SENS_TYPE_BARO
 *
 * Data types used are 
 *   - EmbeddedDataPoint
 *   - SingleAxisDataEvent
 *
 *  \section Bosch BMP3 Event subscriptions / input events
 *  Nil
 *
 * \code
 * INTERNAL_APP_INIT(BOSCH_BMP3_APP_ID, BOSCH_BMP3_APP_VERSION, bmp3_start_task, bmp3_end_task, bmp3_handle_event);
 *
 * \endcode
 */
/***************************************************************************/
/**\name        Header files
****************************************************************************/

#include "variantCommon.h"
#include "bosch_bmp380.h"
#include "floatRt.h"

/*********************************************************************/
/* macro definitions */
/*********************************************************************/

#define BOSCH_BMP3_MAX_PENDING_I2C_REQUESTS   UINT8_C(4)
#define BOSCH_BMP3_MAX_I2C_TRANSFER_SIZE      UINT8_C(6)

/*! This defines how many baro events we could handle being backed up in
 *  the queue. Use this to size our slab */
#define MAX_BARO_EVENTS                       UINT8_C(4)
/*! Nucleo board I2C bus ID used to connect the sensor */
#define I2C_BUS_ID                      UINT8_C(BARO_I2C_BUS_ID)
#define I2C_SPEED                       UINT32_C(400000)
/*! I2C address of the BMP3 sensor when its SDO is connected to GND */
#define I2C_ADDR                        UINT8_C(0x76)

#define ARRAY_SIZE(a)   (sizeof((a)) / sizeof((a)[0]))

/***************************************************************************/
/**\name        Enumerations
****************************************************************************/

#define INFO_PRINT(fmt, ...) do { \
        osLog(LOG_INFO, "%s " fmt, "[BMP380]", ##__VA_ARGS__); \
    } while (0);

/*! 
 * @brief This structure defines how many baro events we could handle being backed up in
 *  the queue. Use this to size our slab */
enum bmp3_sensor_evt
{
    EVT_SENSOR_I2C = EVT_APP_START + 1,
    EVT_BMP3_BARO_TIMER,
    EVT_BMP3_TEMP_TIMER,
};

/*!
 * @brief This structure contains BMP3 states
 */
enum bmp3_task_state
{
    STATE_NIL,
    STATE_CHECK_CMD_RDY,
    STATE_READ_ERR_REG,
    STATE_CHECK_ERR_REG,
    STATE_VERIFY_ID,
    STATE_PARSE_CALIB_DATA,
    STATE_FINISH_INIT,
    STATE_AWAITING_COMP_PARAMS,
    STATE_CONFIG,
    STATE_IDLE,
    STATE_ENABLE_BARO,
    STATE_ENABLE_TEMP,
    STATE_DISABLE_BARO,
    STATE_DISABLE_TEMP,
    STATE_SAMPLING,
};

/***************************************************************************/
/**\name        Local structures
****************************************************************************/
/*!
 * @brief I2C communication support structure
 */
struct bmp3_i2c_transfer
{
    size_t tx_size;
    size_t rx_size;
    int err;
    uint8_t txrx_buffer[BOSCH_BMP3_MAX_I2C_TRANSFER_SIZE];
    uint8_t state;
    bool in_use;
    bool last_transfer;
};

/*!
 * @brief nano app task structure
 */
static struct bmp3_task
{
    struct SlabAllocator *evt_slab;

    uint32_t id;
    uint32_t baro_handle;
    uint32_t temp_handle;
    uint32_t baro_timer_handle;
    uint32_t temp_timer_handle;
    uint64_t bmp3_sens_time;
    /* Delay state for the next transaction */
    uint8_t bmp3_delay_state;

    bool baro_on;
    bool temp_on;
    bool baro_reading;
    bool baro_calibrate;
    bool temp_reading;

    uint8_t pwr_ctrl_reg;
    uint8_t cmd_rdy_status;
    uint8_t cmd_err_status;

    /* Array to store calibration data */
    uint8_t calib_reg[BMP3_CALIB_DATA_LEN];
    struct bmp3_reg_calib_data calib_data;

    struct bmp3_i2c_transfer transfers[BOSCH_BMP3_MAX_PENDING_I2C_REQUESTS];

} m_task;

/***************************************************************************/
/*! Static Function Declarations
****************************************************************************/

/*!
 * @brief Allocate a buffer and mark it as in use with the given state, or return NULL
 * if no buffers available. Must *not* be called from interrupt context.
 *
 * @param[in]   state    BMP3 state to which the next state transition occurs
 *
 * @param[out]  NULL 
 *
 * @return      struct bmp3_i2c_transfer *   pointer to the I2C transfer structure which is
 *                                     allocated for current use
 */
static struct bmp3_i2c_transfer *allocate_transfer_buffer(uint8_t state);

/*!
 * @brief I2C callback function to be called when an I2C transfer is complete
 *
 * @param[in]   cookie   Data sent to callback
 * @param[in]   tx_size  Size of the I2C buffer sent
 * @param[in]   rx_size  Size of the I2C buffer received
 * @param[in]   err      Error sent from the I2C driver
 *
 * @param[out]  NULL 
 *
 * @return      NULL
 */
static void i2c_callback(void *cookie, size_t tx_size, size_t rx_size, int err);

/*!
 * @brief Helper function to write a one byte register
 *
 * @param[in]   reg        BMP3 register address to be written  
 * @param[in]   value      Data to be written in the BMP3 register
 * @param[in]   state      BMP3 state which will be set next if lastxfer is true
 * @param[in]   lastxfer   Indication of whether or not is the current I2C transfer is the last one
 *
 * @param[out]  NULL 
 *
 * @return      Write transfer success / failure from I2C driver
 *              Returns true if we got a successful return value from i2cMasterTx().
 */
static bool bmp3_write_register(uint8_t reg, uint8_t value, uint8_t state, bool lastxfer);

/*!
 * @brief Helper function to read sensor registers
 *
 * @param[in]   reg        BMP3 register address to be read from  
 * @param[in]   value      Data read from the BMP3 registers
 * @param[in]   len        Number of bytes to be read from the sensor
 * @param[in]   state      BMP3 state which will be set next if lastxfer is true
 * @param[in]   lastxfer   Indication of whether or not is the current I2C transfer is the last one
 *
 * @param[out]  NULL 
 *
 * @return      Read transfer success / failure from I2C driver
 *              Returns true if we got a successful return value from i2cMasterTxRx().
 */
static bool bmp3_read_register(uint8_t reg, uint8_t* value, uint16_t len, uint8_t state, bool lastxfer);

/*!
 * @brief This internal API enables pressure/temperature or both sensors for sampling based 
 * on the desired settings
 *
 * @param[in]   on                Enable/Disable command for sensor
 * @param[in]   state             BMP3 next state variable to be set
 * @param[in]   desired_settings  BMP3_PRESS_EN_SEL / BMP3_TEMP_EN_SEL
 *
 * @param[out]  NULL
 *
 * @return      Result of enabling /disabling the sensor
 *              Returns true if we got a success in operation
 */
static bool bmp3_enable_sensor(bool on, uint8_t state, uint8_t desired_settings);

/*!
 * @brief This internal API starts the callback from the timer set for pressure sensor
 *
 * @param[in]   timer_id   Timer ID for the callback
 * @param[in]   cookie     Data passed on to the callback
 *
 * @param[out]  NULL
 *
 * @return      NULL
 */
static void baro_timer_callback(uint32_t timer_id, void *cookie);

/*!
 * @brief This internal API starts the callback from the timer set for temperature sensor
 *
 * @param[in]   timer_id   Timer ID for the callback
 * @param[in]   cookie     Data passed on to the callback
 *
 * @param[out]  NULL
 *
 * @return      NULL
 */
static void temp_timer_callback(uint32_t timer_id, void *cookie);

/*! Sensor operations / Registered callback definitions*/
/*!
 * @brief Power callback for pressure sensor
 * This function is called when there is a power on/off change in sensor
 */
static bool bmp3_sensor_power_baro(bool on, void *cookie);

/*!
 * @brief Firmware upload callback for pressure sensor
 * This function is called when there is a need to update firmware in sensor.
 */
static bool bmp3_sensor_firmware_baro(void *cookie);

/*!
 * @brief Rate callback for pressure sensor
 * This function is called when there is a rate change in sensor
 */
static bool bmp3_sensor_rate_baro(uint32_t rate, uint64_t latency, void *cookie);

/*!
 * @brief Flush callback for pressure sensor
 * This function is used to flush the sensor data
 */
static bool bmp3_sensor_flush_baro(void *cookie);

/*!
 * @brief Calibrate callback for pressure sensor
 * This function is a dummy function for now. Will be updated later
 */
static bool bmp3_sensor_calibrate_baro(void *cookie);

/*!
 * @brief Pressure data configuration callback
 * This function is a dummy function for now. Will be updated later
 */
static bool bmp3_sensor_cfg_data_baro(void *data, void *cookie);

/*!
 * @brief Power callback for temperature sensor
 * This function is called when there is a power on/off change in sensor
 */
static bool bmp3_sensor_power_temp(bool on, void *cookie);

/*!
 * @brief Firmware upload callback for temperature sensor
 * This function is called when there is a need to update firmware in sensor.
 */
static bool bmp3_sensor_firmware_temp(void *cookie);

/*!
 * @brief Rate callback for temperature sensor
 * This function is called when there is a rate change in sensor
 */
static bool bmp3_sensor_rate_temp(uint32_t rate, uint64_t latency, void *cookie);

/*!
 * @brief Flush callback for temperature sensor
 * This function is used to flush the sensor data
 */
static bool bmp3_sensor_flush_temp(void *cookie);

/*!
 * @brief This internal API writes the power mode in the sensor.
 */
static int8_t write_power_mode(uint8_t op_mode);

/*!
 *  @brief This internal API is used to parse the calibration data, compensates
 *  it and store it in device structure
 */
static void parse_calib_data(const uint8_t *reg_data);

/*!
 * @brief This internal API is used to compensate the raw temperature data and
 * return the compensated temperature data in integer data type.
 * for eg return if temperature is 2549 which is 2549/100 = 25.49 degree Celsius
 */
static int64_t compensate_temp(uint32_t uncomp_temp, int64_t* t_lin);

/*!
 * @brief This internal API is used to compensate the raw pressure data and
 * return the compensated pressure data in integer data type.
 * for eg return if pressure is 9528709 which is 9528709/100 = 95287.09 Pascal or 952.8709 hecto Pascal
 */
static uint64_t compensate_baro(int64_t t_lin, uint32_t uncomp_baro_data);

/*!
 * @brief   This function is used to parse the register data of 6 bytes from the sensor
 * and store the corresponding pressure and temperature data after doing the integer compensation
 */
static void parse_temp_baro_data(const uint8_t *reg_data, float *pressure_pascal, float *temp_centigrade);

/*!
 * @brief   This function is used to do slab allocation for the pressure data
 */
static bool bmp3_alloc_baro_evt(struct SingleAxisDataEvent **evt_ptr, float sample, uint64_t time);

/*!
 * @brief   This function is free the allocated slab from the pressure data after enqueue
 */
static void bmp3_free_baro_evt(void *ptr);

/*!
 * @brief   This call back is called when the timer expires and
 * is used to call the handle I2C event based on the states set in cookie data
 */
static void bmp3_timer_cbk(uint32_t timerId, void *cookie);

/*!
 * @brief   This function is used to handle the I2C communication events and
 * switch between the state machines of bmp3 nano-app
 */
static void bmp3_handle_comm_evt(struct bmp3_i2c_transfer *xfer);


/**********************************************************************************/
/**\name        Globals/Static
***********************************************************************************/
/*!
 * @brief Pressure sensor supported rates
 * It should match "supported rates in length" and be the timer length for that rate in nanosecs
 */
static const uint64_t rate_timer_val_baro[] =
{
    1000000000ULL,
    1000000000ULL / 5,
    1000000000ULL / 10,
    1000000000ULL / 20,
};

/*!
 * @brief Temperature sensor supported rates
 * It should match "supported rates in length" and be the timer length for that rate in nanosecs
 */
static const uint64_t rate_timer_val_temp[] =
{
    10 * 1000000000ULL,
     1 * 1000000000ULL,
    1000000000ULL / 5,
    1000000000ULL / 10,
    1000000000ULL / 25,
};

/*!
 * @brief Pressure sensor supported rates
 * It should match the timer length for that rate in nanosecs
 */
static const uint32_t baro_rates_enabled[] =
{
    SENSOR_HZ(1),
    SENSOR_HZ(5),
    SENSOR_HZ(10),
    SENSOR_HZ(20),
    0
};

/*!
 * @brief Temperature sensor supported rates
 * It should match the timer length for that rate in nanosecs
 */
static const uint32_t temp_rates_enabled[] =
{
    SENSOR_HZ(0.1),
    SENSOR_HZ(1),
    SENSOR_HZ(5),
    SENSOR_HZ(10),
    SENSOR_HZ(25),
    0,
};

/*!
 * @brief sensor info initialization for pressure sensor
 */
static const struct SensorInfo bmp3_baro_info =
{
    .sensorName = "Pressure",
    .supportedRates = baro_rates_enabled,
    .sensorType = SENS_TYPE_BARO,
    .numAxis = NUM_AXIS_ONE,
    .interrupt = NANOHUB_INT_NONWAKEUP,
    .minSamples = 300
};

/*!
 * @brief sensor operations initialization for pressure sensor
 */
static const struct SensorOps bmp3_baro_ops =
{
    .sensorPower = bmp3_sensor_power_baro,
    .sensorFirmwareUpload = bmp3_sensor_firmware_baro,
    .sensorSetRate = bmp3_sensor_rate_baro,
    .sensorFlush = bmp3_sensor_flush_baro,
    .sensorCalibrate = bmp3_sensor_calibrate_baro,
    .sensorCfgData = bmp3_sensor_cfg_data_baro,
};

/*!
 * @brief sensor info initialization for temperature sensor
 */
static const struct SensorInfo bmp3_temp_info =
{
    .sensorName = "Temperature",
    .supportedRates = temp_rates_enabled,
    .sensorType = SENS_TYPE_TEMP,
    .numAxis = NUM_AXIS_EMBEDDED,
    .interrupt = NANOHUB_INT_NONWAKEUP,
    .minSamples = 20
};

/*!
 * @brief sensor operations initialization for temperature sensor
 */
static const struct SensorOps bmp3_temp_ops =
{
    .sensorPower = bmp3_sensor_power_temp,
    .sensorFirmwareUpload = bmp3_sensor_firmware_temp,
    .sensorSetRate = bmp3_sensor_rate_temp,
    .sensorFlush = bmp3_sensor_flush_temp,
};

/***************************************************************************/
/*! Static Function Definitions
****************************************************************************/
/*!
 * @brief       Helper function to release bmp3_i2c_transfer structure
 *
 * @param[in]   xfer    I2C transfer structure
 *
 * @param[out]  NULL
 *
 * @return      NULL
 */
static inline void release_transfer(struct bmp3_i2c_transfer *xfer)
{
    xfer->in_use = false;
}

/*!
 * @brief Allocate a buffer and mark it as in use with the given state, or return NULL
 * if no buffers available. Must *not* be called from interrupt context
 */
static struct bmp3_i2c_transfer *allocate_transfer_buffer(uint8_t state)
{
    size_t i;

    for (i = 0; i < ARRAY_SIZE(m_task.transfers); i++) {
        if (!m_task.transfers[i].in_use) {
            m_task.transfers[i].in_use = true;
            m_task.transfers[i].state = state;
            return &m_task.transfers[i];
        }
    }

    osLog(LOG_ERROR, "[BMP3]: Ran out of i2c buffers!");
    return NULL;
}

/*!
 * @brief I2C callback function to be called when an I2C transfer is complete
 */
static void i2c_callback(void *cookie, size_t tx_size, size_t rx_size, int err)
{
    struct bmp3_i2c_transfer *xfer = cookie;

    xfer->tx_size = tx_size;
    xfer->rx_size = rx_size;
    xfer->err = err;

    /* Do not run callback if not the last one in a set of i2c transfers */
    if (xfer && !xfer->last_transfer) {
        release_transfer(xfer);
        return;
    }

    if (xfer->last_transfer) {
        osEnqueuePrivateEvt(EVT_SENSOR_I2C, cookie, NULL, m_task.id);
    }

    if (err != 0)
        osLog(LOG_INFO, "[BMP3] i2c error (tx_size: %d, rx_size: %d, err: %d)\n", tx_size, rx_size, err);

}

/*!
 * @brief Helper function to write a one byte register
 */
static bool bmp3_write_register(uint8_t reg, uint8_t value, uint8_t state, bool lastxfer)
{
    struct bmp3_i2c_transfer *xfer = allocate_transfer_buffer(state);
    int ret = -1;

    if (xfer != NULL) {
        xfer->txrx_buffer[0] = reg;
        xfer->txrx_buffer[1] = value;
        xfer->last_transfer = lastxfer;
        ret = i2cMasterTx(I2C_BUS_ID, I2C_ADDR, xfer->txrx_buffer, 2, i2c_callback, xfer);
        if (ret)
            release_transfer(xfer);
    }

    return (ret == 0);
}

/*!
 * @brief Helper function to read sensor registers
 */
static bool bmp3_read_register(uint8_t reg, uint8_t* value, uint16_t len, uint8_t state, bool lastxfer)
{
    struct bmp3_i2c_transfer *new_transfer = allocate_transfer_buffer(state);
    int ret = -1;

    if (new_transfer != NULL) {
        new_transfer->txrx_buffer[0] = reg;
        new_transfer->last_transfer = lastxfer;

        if (value != NULL){
            ret = i2cMasterTxRx(I2C_BUS_ID, I2C_ADDR, new_transfer->txrx_buffer, 1, value, len, i2c_callback, new_transfer);
        } else {
            ret = i2cMasterTxRx(I2C_BUS_ID, I2C_ADDR, new_transfer->txrx_buffer, 1, new_transfer->txrx_buffer, len, i2c_callback, new_transfer);
        }

        if (ret)
            release_transfer(new_transfer);
    }

    return (ret == 0);
}

/*!
 * @brief This internal API enables pressure/temperature or both sensors for sampling based 
 * on the desired settings
 */
static bool bmp3_enable_sensor(bool on, uint8_t state, uint8_t desired_settings)
{
    bool rslt;

        if (desired_settings & BMP3_PRESS_EN_SEL)
        {
            /* Set the pressure enable settings in the register variable */
            m_task.pwr_ctrl_reg = BMP3_SET_BITS_POS_0(m_task.pwr_ctrl_reg, BMP3_PRESS_EN, on);

            /* Set the temperature enable settings in the register variable */
            m_task.pwr_ctrl_reg = BMP3_SET_BITS(m_task.pwr_ctrl_reg, BMP3_TEMP_EN, on);

        }
        if (desired_settings & BMP3_TEMP_EN_SEL)
        {
            /* Set the temperature enable settings in the register variable */
            m_task.pwr_ctrl_reg = BMP3_SET_BITS(m_task.pwr_ctrl_reg, BMP3_TEMP_EN, on);
        }

        /* Write the power control settings in the register */
        rslt = bmp3_write_register(BMP3_PWR_CTRL_ADDR, m_task.pwr_ctrl_reg, state, true);

    return rslt;
}

/*!
 * @brief This internal API starts the callback from the timer set for pressure sensor
 */
static void baro_timer_callback(uint32_t timer_id, void *cookie)
{
    osEnqueuePrivateEvt(EVT_BMP3_BARO_TIMER, cookie, NULL, m_task.id);
}

/*!
 * @brief This internal API starts the callback from the timer set for temperature sensor
 */
static void temp_timer_callback(uint32_t timer_id, void *cookie)
{
    osEnqueuePrivateEvt(EVT_BMP3_TEMP_TIMER, cookie, NULL, m_task.id);
}

/*! Sensor operations / Registered callback */
/*!
 * @brief Power callback for pressure sensor
 * This function is called when there is a power on/off change in sensor
 */
static bool bmp3_sensor_power_baro(bool on, void *cookie)
{
    INFO_PRINT("BaroPower %s\n", on?"on":"off");
    bool old_mode = m_task.baro_on || m_task.temp_on;
    bool new_mode = on || m_task.temp_on;

    if (!on && m_task.baro_timer_handle) {
        timTimerCancel(m_task.baro_timer_handle);
        m_task.baro_timer_handle = 0;
        m_task.baro_reading = false;
    }

    if (old_mode != new_mode)
        bmp3_enable_sensor(new_mode, (on ? STATE_ENABLE_BARO : STATE_DISABLE_BARO), BMP3_PRESS_EN_SEL);
    else
        sensorSignalInternalEvt(m_task.baro_handle, SENSOR_INTERNAL_EVT_POWER_STATE_CHG, on, 0);

    m_task.baro_on = on;

    return true;
}

/*!
 * @brief Firmware upload callback for pressure sensor
 * This function is called when there is a need to update firmware in sensor.
 */
static bool bmp3_sensor_firmware_baro(void *cookie)
{
    return sensorSignalInternalEvt(m_task.baro_handle, SENSOR_INTERNAL_EVT_FW_STATE_CHG, 1, 0);
}

/*!
 * @brief Rate callback for pressure sensor
 * This function is called when there is a rate change in sensor
 */
static bool bmp3_sensor_rate_baro(uint32_t rate, uint64_t latency, void *cookie)
{
    INFO_PRINT("BaroSetRate rate %lu latency %llu\n", rate, latency);
    if (m_task.baro_timer_handle)
        timTimerCancel(m_task.baro_timer_handle);

    m_task.baro_timer_handle = timTimerSet(sensorTimerLookupCommon(baro_rates_enabled, rate_timer_val_baro, rate), 0, 50, baro_timer_callback, NULL, false);

    return sensorSignalInternalEvt(m_task.baro_handle, SENSOR_INTERNAL_EVT_RATE_CHG, rate, latency);
}

/*!
 * @brief Flush callback for pressure sensor
 * This function is used to flush the sensor data
 */
static bool bmp3_sensor_flush_baro(void *cookie)
{
    return osEnqueueEvt(sensorGetMyEventType(SENS_TYPE_BARO), SENSOR_DATA_EVENT_FLUSH, NULL);
}

/*!
 * @brief Calibrate callback for pressure sensor
 * This function is a dummy function for now. Will be updated later
 */
static bool bmp3_sensor_calibrate_baro(void *cookie)
{
    return true;
}

/*!
 * @brief Pressure data configuration callback
 * This function is a dummy function for now. Will be updated later
 */
static bool bmp3_sensor_cfg_data_baro(void *data, void *cookie)
{
    return true;
}

/*!
 * @brief Power callback for temperature sensor
 * This function is called when there is a power on/off change in sensor
 */
static bool bmp3_sensor_power_temp(bool on, void *cookie)
{
    bool old_mode = m_task.baro_on || m_task.temp_on;
    bool new_mode = on || m_task.baro_on;

    if (!on && m_task.temp_timer_handle) {
        timTimerCancel(m_task.temp_timer_handle);
        m_task.temp_timer_handle = 0;
        m_task.temp_reading = false;
    }

    if (old_mode != new_mode)
        bmp3_enable_sensor(new_mode, (on ? STATE_ENABLE_TEMP : STATE_DISABLE_TEMP), BMP3_TEMP_EN_SEL);
    else
        sensorSignalInternalEvt(m_task.temp_handle, SENSOR_INTERNAL_EVT_POWER_STATE_CHG, on, 0);

    m_task.temp_on = on;

    return true;
}

/*!
 * @brief Firmware upload callback for temperature sensor
 * This function is called when there is a need to update firmware in sensor.
 */
static bool bmp3_sensor_firmware_temp(void *cookie)
{
    sensorSignalInternalEvt(m_task.temp_handle, SENSOR_INTERNAL_EVT_FW_STATE_CHG, 1, 0);
    return true;
}

/*!
 * @brief Rate callback for temperature sensor
 * This function is called when there is a rate change in sensor
 */
static bool bmp3_sensor_rate_temp(uint32_t rate, uint64_t latency, void *cookie)
{
    if (m_task.temp_timer_handle)
        timTimerCancel(m_task.temp_timer_handle);

    /* Enable a timer to poll the data for the set rate*/
    m_task.temp_timer_handle = timTimerSet(sensorTimerLookupCommon(temp_rates_enabled, rate_timer_val_temp, rate), 0, 50, temp_timer_callback, NULL, false);

    /*Signal the rate callback is over to seos */
    sensorSignalInternalEvt(m_task.temp_handle, SENSOR_INTERNAL_EVT_RATE_CHG, rate, latency);

    return true;
}

/*!
 * @brief Flush callback for temperature sensor
 * This function is used to flush the sensor data
 */
static bool bmp3_sensor_flush_temp(void *cookie)
{
    return osEnqueueEvt(sensorGetMyEventType(SENS_TYPE_TEMP), SENSOR_DATA_EVENT_FLUSH, NULL);
}

/*!
 * @brief This internal API writes the power mode in the sensor.
 */
static int8_t write_power_mode(uint8_t op_mode)
{
    int8_t rslt;

    m_task.pwr_ctrl_reg = BMP3_SET_BITS(m_task.pwr_ctrl_reg, BMP3_OP_MODE, op_mode);

    /* Write the power mode in the register */
    rslt = bmp3_write_register(BMP3_PWR_CTRL_ADDR, m_task.pwr_ctrl_reg, STATE_NIL, false);

    return rslt;
}

/*!
 *  @brief This internal API is used to parse the calibration data, compensates
 *  it and store it in device structure
 */
static void parse_calib_data(const uint8_t *reg_data)
{
    m_task.calib_data.par_t1 = BMP3_CONCAT_BYTES(reg_data[1], reg_data[0]);
    m_task.calib_data.par_t2 = BMP3_CONCAT_BYTES(reg_data[3], reg_data[2]);
    m_task.calib_data.par_t3 = (int8_t)reg_data[4];
    m_task.calib_data.par_p1 = (int16_t)BMP3_CONCAT_BYTES(reg_data[6], reg_data[5]);
    m_task.calib_data.par_p2 = (int16_t)BMP3_CONCAT_BYTES(reg_data[8], reg_data[7]);
    m_task.calib_data.par_p3 = (int8_t)reg_data[9];
    m_task.calib_data.par_p4 = (int8_t)reg_data[10];
    m_task.calib_data.par_p5 = BMP3_CONCAT_BYTES(reg_data[12], reg_data[11]);
    m_task.calib_data.par_p6 = BMP3_CONCAT_BYTES(reg_data[14], reg_data[13]);
    m_task.calib_data.par_p7 = (int8_t)reg_data[15];
    m_task.calib_data.par_p8 = (int8_t)reg_data[16];
    m_task.calib_data.par_p9 = (int16_t)BMP3_CONCAT_BYTES(reg_data[18], reg_data[17]);
    m_task.calib_data.par_p10 = (int8_t)reg_data[19];
    m_task.calib_data.par_p11 = (int8_t)reg_data[20];
}

/*!
 * @brief This internal API is used to compensate the raw temperature data and
 * return the compensated temperature data in integer data type.
 * for eg return if temperature is 2549 which is 2549/100 = 25.49 degree Celsius
 */
static int64_t compensate_temp(uint32_t uncomp_temp, int64_t* t_lin)
{
    int64_t partial_data1;
    int64_t partial_data2;
    int64_t partial_data3;
    int64_t partial_data4;
    int64_t partial_data5;
    int64_t partial_data6;
    int64_t comp_temp;

    partial_data1 = ((int64_t)uncomp_temp - (256 * m_task.calib_data.par_t1));
    partial_data2 = m_task.calib_data.par_t2 * partial_data1;
    partial_data3 = (partial_data1 * partial_data1);
    partial_data4 = (int64_t)partial_data3 * m_task.calib_data.par_t3;
    partial_data5 = ((int64_t)(partial_data2 * 262144) + partial_data4);
    partial_data6 = partial_data5 / 4294967296;

    /* Store t_lin in dev. structure for pressure calculation */
    *t_lin = partial_data6;
    comp_temp = (int64_t)((partial_data6 * 25) / 16384);

    return comp_temp;
}

/*!
 * @brief This internal API is used to compensate the raw pressure data and
 * return the compensated pressure data in integer data type.
 * for eg return if pressure is 9528709 which is 9528709/100 = 95287.09 Pascal or 952.8709 hecto Pascal
 */
static uint64_t compensate_baro(int64_t t_lin, uint32_t uncomp_baro_data)
{
    int64_t partial_data1;
    int64_t partial_data2;
    int64_t partial_data3;
    int64_t partial_data4;
    int64_t partial_data5;
    int64_t partial_data6;
    int64_t offset;
    int64_t sensitivity;
    uint64_t comp_press;

    partial_data1 = t_lin * t_lin;
    partial_data2 = partial_data1 / 64;
    partial_data3 = (partial_data2 * t_lin) / 256;
    partial_data4 = (m_task.calib_data.par_p8 * partial_data3) / 32;
    partial_data5 = (m_task.calib_data.par_p7 * partial_data1) * 16;
    partial_data6 = (m_task.calib_data.par_p6 * t_lin) * 4194304;
    offset = (m_task.calib_data.par_p5 * 140737488355328) + partial_data4 + partial_data5 + partial_data6;
    partial_data2 = (m_task.calib_data.par_p4 * partial_data3) / 32;
    partial_data4 = (m_task.calib_data.par_p3 * partial_data1) * 4;
    partial_data5 = (m_task.calib_data.par_p2 - 16384) * t_lin * 2097152;
    sensitivity = ((m_task.calib_data.par_p1 - 16384) * 70368744177664) + partial_data2 + partial_data4 + partial_data5;
    partial_data1 = (sensitivity / 16777216) * uncomp_baro_data;
    partial_data2 = m_task.calib_data.par_p10 * t_lin;
    partial_data3 = partial_data2 + (65536 * m_task.calib_data.par_p9);
    partial_data4 = (partial_data3 * uncomp_baro_data) / 8192;
    /*dividing by 10 followed by multiplying by 10 to avoid overflow caused by (uncomp_baro_data * partial_data4) */
    partial_data5 = (uncomp_baro_data * (partial_data4/16))/512;
    partial_data5 = partial_data5 * 16;
    partial_data6 = (int64_t)((uint64_t)uncomp_baro_data * (uint64_t)uncomp_baro_data);
    partial_data2 = (m_task.calib_data.par_p11 * partial_data6) / 65536;
    partial_data3 = (partial_data2 * uncomp_baro_data) / 128;
    partial_data4 = (offset / 4) + partial_data1 + partial_data5 + partial_data3;
    comp_press = (((uint64_t)partial_data4 * 25) / (uint64_t)1099511627776);

    return comp_press;
}

/*!
 * @brief   This function is used to parse the register data of 6 bytes from the sensor
 * and store the corresponding pressure and temperature data after doing the integer compensation
 */
static void parse_temp_baro_data(const uint8_t *reg_data, float *pressure_pascal, float *temp_centigrade)
{
    /* Temporary variables to store the sensor data */
    uint32_t data_xlsb;
    uint32_t data_lsb;
    uint32_t data_msb;

    /* Store the parsed register values for pressure data */
    data_xlsb = (uint32_t)reg_data[0];
    data_lsb = (uint32_t)reg_data[1] << 8;
    data_msb = (uint32_t)reg_data[2] << 16;
    uint32_t pres_adc = data_msb | data_lsb | data_xlsb;

    /* Store the parsed register values for temperature data */
    data_xlsb = (uint32_t)reg_data[3];
    data_lsb = (uint32_t)reg_data[4] << 8;
    data_msb = (uint32_t)reg_data[5] << 16;
    uint32_t temp_adc = data_msb | data_lsb | data_xlsb;

    int64_t T_fine;
    int64_t temp = compensate_temp(temp_adc, &T_fine);
    uint64_t pres = compensate_baro(T_fine, pres_adc);

    *temp_centigrade = floatFromInt64(temp) * 0.01f;
    *pressure_pascal = floatFromUint64(pres) * 0.01f;
}

/*!
 * @brief   This function is used to do slab allocation for the pressure data
 */
static bool bmp3_alloc_baro_evt(struct SingleAxisDataEvent **evt_ptr, float sample, uint64_t time)
{
    struct SingleAxisDataEvent *ev;

    *evt_ptr = slabAllocatorAlloc(m_task.evt_slab);

    ev = *evt_ptr;
    if (!ev) {
        osLog(LOG_ERROR, "[BMP3] slabAllocatorAlloc() failed\n");
        return false;
    }

    memset(&ev->samples[0].firstSample, 0x00, sizeof(struct SensorFirstSample));
    ev->referenceTime = time;
    ev->samples[0].firstSample.numSamples = 1;
    ev->samples[0].fdata = sample;

    return true;
}

/*!
 * @brief   This function is free the allocated slab from the pressure data after enqueue
 */
static void bmp3_free_baro_evt(void *ptr)
{
    slabAllocatorFree(m_task.evt_slab, ptr);
}

/*!
 * @brief   This call back is called when the timer expires and
 * is used to call the handle I2C event based on the states set in cookie data
 */
static void bmp3_timer_cbk(uint32_t timerId, void *cookie)
{
    uint8_t state = *((uint8_t*)cookie);
    struct bmp3_i2c_transfer *xfer = allocate_transfer_buffer(state);

    if (!osEnqueuePrivateEvt(EVT_SENSOR_I2C, xfer, NULL, m_task.id)){
        osLog(LOG_ERROR, "[BMP3] Sensor timer call back failed to enqueue event \n");
    }
}

/*!
 * @brief   This function is used to handle the I2C communication events and
 * switch between the state machines of bmp3 nano-app
 */
static void bmp3_handle_comm_evt(struct bmp3_i2c_transfer *xfer)
{
    union EmbeddedDataPoint embeddedSample;
    struct SingleAxisDataEvent *baroSample;
    float pressure_pascal;
    float temp_centigrade;
#ifdef BMP3_BEBUG_INFO_EN
    uint8_t index;
#endif
    static uint8_t bmp3_retry_cnt = 0;

    switch (xfer->state) {

        case STATE_CHECK_CMD_RDY:
            if (xfer->err != 0 || (((m_task.cmd_rdy_status & BMP3_CMD_RDY) != BMP3_CMD_RDY))) {

                if(bmp3_retry_cnt > 5) {
                    osLog(LOG_ERROR, "[BMP3] Device is not ready to accept a command : %d \n",m_task.cmd_rdy_status);
                    break;
                }

                /* Read the command ready status */
                bmp3_read_register(BMP3_SENS_STATUS_REG_ADDR, &m_task.cmd_rdy_status, 1, STATE_NIL, false);

                osLog(LOG_INFO, "[BMP3] Command register value : %d \n",m_task.cmd_rdy_status);

                bmp3_retry_cnt++;

                /* Add a 2ms delay */
                m_task.bmp3_delay_state = STATE_CHECK_CMD_RDY;
                if (timTimerSet(2000000, 100, 100, bmp3_timer_cbk, &m_task.bmp3_delay_state, true) == 0)
                     osLog(LOG_ERROR, "[BMP3] Failed to set timer delay for cmd_rdy read\n");

                break;
            }

            /* Write the soft reset command in the sensor */
            bmp3_write_register(BMP3_CMD_ADDR, 0xB6, 0 , false);

            /* Add a 2ms delay after soft-reset */
            m_task.bmp3_delay_state = STATE_READ_ERR_REG;
            if (timTimerSet(2000000, 100, 100, bmp3_timer_cbk, &m_task.bmp3_delay_state, true) == 0)
                 osLog(LOG_ERROR, "[BMP3] Failed to set a timer delay for soft-reset\n");
            break;

        case STATE_READ_ERR_REG:
            /* Read the command error status */
            bmp3_read_register(BMP3_ERR_REG_ADDR, &m_task.cmd_err_status, 1, STATE_CHECK_ERR_REG, true);
            break;

        case STATE_CHECK_ERR_REG:
            if (xfer->err != 0 || (m_task.cmd_err_status & BMP3_CMD_ERR)) {
                osLog(LOG_ERROR, "[BMP3] Command Execution failed \n");
                break;
            }

            /* Read the Chip Id of BMP3 for verification */
             bmp3_read_register(BMP3_CHIP_ID_ADDR, NULL, 1, STATE_VERIFY_ID, true);

            break;

        case STATE_VERIFY_ID:
             /* Check the sensor ID */
             if (xfer->err != 0 || xfer->txrx_buffer[0] != BMP3_CHIP_ID) {
                 osLog(LOG_ERROR, "[BMP3] not detected : %d \n", xfer->txrx_buffer[0]);
                 break;
             }
             osLog(LOG_INFO, "[BMP3] detected , chip ID : %d \n",xfer->txrx_buffer[0]);

             /* Read the calibration data from sensor */
              bmp3_read_register(BMP3_CALIB_DATA_ADDR, m_task.calib_reg, BMP3_CALIB_DATA_LEN, STATE_PARSE_CALIB_DATA, true);
            break;

        case STATE_PARSE_CALIB_DATA:

            parse_calib_data(m_task.calib_reg);

#ifdef BMP3_BEBUG_INFO_EN
            for (index = 0; index < BMP3_CALIB_DATA_LEN; index++) {
                 osLog(LOG_INFO, "[BMP3] CALIB DATA REG: %d \n",m_task.calib_reg[index]);
            }

            osLog(LOG_INFO, "[BMP3] m_task.calib_data.  : %d \n",m_task.calib_data.par_t1);
            osLog(LOG_INFO, "[BMP3] m_task.calib_data.par_t2 : %d \n",m_task.calib_data.par_t2);
            osLog(LOG_INFO, "[BMP3] m_task.calib_data.par_t3 : %d \n",m_task.calib_data.par_t3);
            osLog(LOG_INFO, "[BMP3] m_task.calib_data.par_p1 : %d \n",m_task.calib_data.par_p1);
            osLog(LOG_INFO, "[BMP3] m_task.calib_data.par_p2 : %d \n",m_task.calib_data.par_p2);
            osLog(LOG_INFO, "[BMP3] m_task.calib_data.par_p3 : %d \n",m_task.calib_data.par_p3);
            osLog(LOG_INFO, "[BMP3] m_task.calib_data.par_p4 : %d \n",m_task.calib_data.par_p4);
            osLog(LOG_INFO, "[BMP3] m_task.calib_data.par_p5 : %d \n",m_task.calib_data.par_p5);
            osLog(LOG_INFO, "[BMP3] m_task.calib_data.par_p6 : %d \n",m_task.calib_data.par_p6);
            osLog(LOG_INFO, "[BMP3] m_task.calib_data.par_p7 : %d \n",m_task.calib_data.par_p7);
            osLog(LOG_INFO, "[BMP3] m_task.calib_data.par_p8 : %d \n",m_task.calib_data.par_p8);
            osLog(LOG_INFO, "[BMP3] m_task.calib_data.par_p9 : %d \n",m_task.calib_data.par_p9);
            osLog(LOG_INFO, "[BMP3] m_task.calib_data.par_p10: %d \n",m_task.calib_data.par_p10);
            osLog(LOG_INFO, "[BMP3] m_task.calib_data.par_p11: %d \n",m_task.calib_data.par_p11);
#endif

            /* Write osr as 16x pressure and 2x temperature : 0x0C */
            bmp3_write_register(BMP3_OSR_ADDR, 0x0C, STATE_FINISH_INIT, true);
            break;

        case STATE_FINISH_INIT:
            sensorRegisterInitComplete(m_task.baro_handle);
            sensorRegisterInitComplete(m_task.temp_handle);
            osLog(LOG_INFO, "[BMP3] Ready to use\n");
            break;

        case STATE_ENABLE_BARO:

            write_power_mode(BMP3_FORCED_MODE);

            sensorSignalInternalEvt(m_task.baro_handle, SENSOR_INTERNAL_EVT_POWER_STATE_CHG, true, 0);
            break;

        case STATE_ENABLE_TEMP:
            write_power_mode(BMP3_FORCED_MODE);
            sensorSignalInternalEvt(m_task.temp_handle, SENSOR_INTERNAL_EVT_POWER_STATE_CHG, true, 0);
            break;
        case STATE_DISABLE_BARO:
            sensorSignalInternalEvt(m_task.baro_handle, SENSOR_INTERNAL_EVT_POWER_STATE_CHG, false, 0);
            break;

        case STATE_DISABLE_TEMP:
            sensorSignalInternalEvt(m_task.temp_handle, SENSOR_INTERNAL_EVT_POWER_STATE_CHG, false, 0);
            break;

        case STATE_SAMPLING:
            write_power_mode(BMP3_FORCED_MODE);

            parse_temp_baro_data(xfer->txrx_buffer, &pressure_pascal, &temp_centigrade);

#ifdef BMP3_BEBUG_INFO_EN
            osLog(LOG_INFO, "[BMP3] Pressure sensor data : %ld [/100(Pa)]\n", ((uint32_t)(pressure_pascal*100)));
            osLog(LOG_INFO, "[BMP3] Temperature sensor data : %ld [/100(degree Celsius)]\n", ((uint32_t)(temp_centigrade*100)));
#endif

            if (m_task.baro_on && m_task.baro_reading) {
                if (bmp3_alloc_baro_evt(&baroSample, pressure_pascal * 0.01f, m_task.bmp3_sens_time)) {
                    if (!osEnqueueEvtOrFree(EVENT_TYPE_BIT_DISCARDABLE | sensorGetMyEventType(SENS_TYPE_BARO), baroSample, bmp3_free_baro_evt)) {
                        osLog(LOG_ERROR, "[BMP3] failed to enqueue baro sample\n");
                    }
                }
            }

            if (m_task.temp_on && m_task.temp_reading) {
                embeddedSample.fdata = temp_centigrade;
                osEnqueueEvt(sensorGetMyEventType(SENS_TYPE_TEMP), embeddedSample.vptr, NULL);
            }

            /* reading is set to false to denote that a sample can be read for processing now */
            m_task.baro_reading = false;
            m_task.temp_reading = false;

            break;

        default:
            break;
    }

    release_transfer(xfer);
}

/*!
 * @brief   This function is used to handle events coming to bmp3  nano-app.
 */
static void bmp3_handle_event(uint32_t evtType, const void* evtData)
{

    switch (evtType) {
        case EVT_APP_START:
            osEventUnsubscribe(m_task.id, EVT_APP_START);
            i2cMasterRequest(I2C_BUS_ID, I2C_SPEED);

            /* Read the command ready status */
            bmp3_read_register(BMP3_SENS_STATUS_REG_ADDR, &m_task.cmd_rdy_status, 1, STATE_CHECK_CMD_RDY, true);
            break;

        case EVT_SENSOR_I2C:
            bmp3_handle_comm_evt((struct bmp3_i2c_transfer *)evtData);
            break;

        case EVT_BMP3_BARO_TIMER:
            /* Start sampling for a value */
            if (!m_task.baro_reading && !m_task.temp_reading) {
                m_task.bmp3_sens_time = sensorGetTime();
                bmp3_read_register(BMP3_DATA_ADDR, NULL, 6, STATE_SAMPLING, true);
            }

            m_task.baro_reading = true;
            break;

        case EVT_BMP3_TEMP_TIMER:
            /* Start sampling for a value */
            if (!m_task.baro_reading && !m_task.temp_reading) {
                bmp3_read_register(BMP3_DATA_ADDR, NULL, 6, STATE_SAMPLING, true);
            }

            m_task.temp_reading = true;
            break;
    }

}

/*!
 * @brief  This function is handler called by seos during the nano-app start.
 */
static bool bmp3_start_task(uint32_t taskId)
{
    m_task.id = taskId;

    m_task.pwr_ctrl_reg = 0;

    /* Register sensors */
    m_task.baro_handle = sensorRegister(&bmp3_baro_info, &bmp3_baro_ops, NULL, false);
    //m_task.temp_handle = sensorRegister(&bmp3_temp_info, &bmp3_temp_ops, NULL, false);

#ifdef BMP3_BEBUG_INFO_EN
    osLog(LOG_INFO, "[BMP3] NANOAPP STARTED \n");
#endif

    m_task.evt_slab = slabAllocatorNew(sizeof(struct SingleAxisDataEvent) + sizeof(struct SingleAxisDataPoint), 4, MAX_BARO_EVENTS);
    if (!m_task.evt_slab) {
        osLog(LOG_ERROR, "[BMP3] slabAllocatorNew() failed\n");
        return false;
    }

    osEventSubscribe(taskId, EVT_APP_START);
#ifdef EXYNOS_CONTEXTHUB
    ipc_set_sensor_info(SENSOR_TYPE_PRESSURE, "BMP380 Press", VENDOR_NAME_BOSCH,  0, 0);
#endif

    return true;
}

/*!
 * @brief  This function is handler called by seos during the nano-app off-loading.
 */
static void bmp3_end_task(void)
{
    slabAllocatorDestroy(m_task.evt_slab);
}

INTERNAL_APP_INIT(BOSCH_BMP3_APP_ID, BOSCH_BMP3_APP_VERSION, bmp3_start_task, bmp3_end_task, bmp3_handle_event);
