/*
 * Copyright (C) 2016 Samsung Electronics Co., Ltd.
 *
 * Shinkyu Park <shinkyu.park@samsung.com>
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


#include <timer.h>
#include <sensors.h>
#include <heap.h>
#include <hostIntf.h>
//#include <nanohubPacket.h>
#include <seos.h>
#include <util.h>

#include <gpio.h>
#include <i2c.h>
#include <plat/gpio/gpio.h>
#include <variant/variant.h>


#define OPT3007_APP_ID                 APP_ID_MAKE(NANOHUB_VENDOR_TI, 1)
#define OPT3007_APP_VERSION            1

#define I2C_BUS_ID                     ALS_I2C_BUS_ID
#define I2C_SPEED                      400000
#define I2C_ADDR                       0x45

#define OPT3007_REG_RESULT             0x00
#define OPT3007_REG_CONFIGURATION      0x01
#define OPT3007_REG_LOW_LIMIT          0x02
#define OPT3007_REG_HIGH_LIMIT         0x03
#define OPT3007_REG_MANUFACTURER_ID    0x7E
#define OPT3007_REG_DEVICE_ID          0x7F

#define OPT3007_MANUFACTURER_ID        0x5449
#define OPT3007_DEVICE_ID              0x3001

#define CONVERSION_TIME_100MS          0
#define CONVERSION_TIME_800MS          1

#define EXP_BIT                        12
#define EXP_VALUE_MASK                 0xF
#define FRAC_VALUE_MASK                0xFFF

#define CONVERSION_MODE_SHUTDOWN       0
#define CONVERSION_MODE_SINGLE         1
#define CONVERSION_MODE_CONTINUOUS     2

#define OVERFLOW_FLAG                  (0x1 << 8)
#define CONVERSION_READY_FLAG          (0x1 << 7)
#define FLAG_HIGH                      (0x1 << 6)
#define FLAG_LOW                       (0x1 << 5)

#define MASK_EXPONENT                  (0x1 << 2)
#define ONE_FAULT_COUNT                0
#define TWO_FAULT_COUNT                1
#define FOUR_FAULT_COUNT               2
#define EIGHT_FAULT_COUNT              3

#define RANGE_NUMBER_AUTO              0xC
#define DEFAULT_RANGE_NUMBER           RANGE_NUMBER_AUTO

#define OPT3007_MAX_I2C_REQUESTS       8
#define OPT3007_MAX_I2C_TRANSFER_SIZE  18

#define OPT3007_TIMER_DELAY            200000000ULL
#define OPT3007_ALS_INVALID            UINT32_MAX
#define OPT3007_DEFAULT_RATE           SENSOR_HZ(5)


float expValue[12] = {0.01, 0.02, 0.04, 0.08,  0.16,  0.32,
                      0.64, 1.28, 2.56, 5.12, 10.24, 20.48};

#define DEBUG_ENABLE   0

#define INFO_PRINT(fmt, ...) do { \
        osLog(LOG_INFO, "[OPT3007]" fmt, ##__VA_ARGS__); \
    } while (0);

#if(DEBUG_ENABLE)
    #define DEBUG_PRINT(fmt, ...) do { \
            osLog(LOG_INFO, "[OPT3007]" fmt, ##__VA_ARGS__); \
        } while (0);
#else
    #define DEBUG_PRINT(fmt, ...)
#endif


enum SensorEvents
{
    EVT_SENSOR_I2C = EVT_APP_START + 1,
    EVT_SENSOR_ALS_TIMER,
};

enum SensorState
{
    SENSOR_STATE_VERIFY_ID,
    SENSOR_STATE_INIT,
    SENSOR_STATE_FINISH_INIT,
    SENSOR_STATE_ENABLING_ALS,
    SENSOR_STATE_DISABLING_ALS,
    SENSOR_STATE_ALS_CHANGING_GAIN,
    SENSOR_STATE_ALS_SAMPLING,
    SENSOR_STATE_CALIBRATION,
    SENSOR_STATE_IDLE,
};

struct I2cTransfer
{
    size_t tx;
    size_t rx;
    int err;
    uint8_t txrxBuf[OPT3007_MAX_I2C_TRANSFER_SIZE];
    uint8_t state;
    bool inUse;
};


struct SensorData
{
    uint32_t tid;

    uint32_t handle;
    uint32_t timerHandle;

    float alsOffset;

    union EmbeddedDataPoint lastAlsSample;
    struct I2cTransfer transfers[OPT3007_MAX_I2C_REQUESTS];

    uint32_t rangeNumber;
    uint32_t conversionTime;
    uint32_t alsRate;

    bool alsOn;
    bool alsCalibration;
    bool alsChangingGain;

    uint8_t faultCount;
};

static struct SensorData mTask;


struct AlsCalibrationData {
    struct HostHubRawPacket header;
    struct SensorAppEventHeader data_header;
    float offset;
} __attribute__((packed));

static const uint32_t supportedRates[] =
{
    SENSOR_HZ(5),
    SENSOR_RATE_ONCHANGE,
    0,
};

static void i2cCallback(void *cookie, size_t tx, size_t rx, int err);


// Allocate a buffer and mark it as in use with the given state, or return NULL
// if no buffers available. Must *not* be called from interrupt context.
static struct I2cTransfer *allocXfer(uint8_t state)
{
    size_t i;

    for (i = 0; i < ARRAY_SIZE(mTask.transfers); i++) {
        if (!mTask.transfers[i].inUse) {
            mTask.transfers[i].inUse = true;
            mTask.transfers[i].state = state;
            return &mTask.transfers[i];
        }
    }

    osLog(LOG_ERROR, "[OPT3007] Ran out of i2c buffers!");
    return NULL;
}

static bool writeRegister(uint8_t reg, uint16_t value, uint8_t state)
{
    struct I2cTransfer *xfer = allocXfer(state);
    int ret = -1;

    if (xfer != NULL) {
        xfer->txrxBuf[0] = reg;
        xfer->txrxBuf[1] = (uint8_t)((value >> 8) & 0xFF);
        xfer->txrxBuf[2] = (uint8_t)(value & 0xFF);
        ret = i2cMasterTx(I2C_BUS_ID, I2C_ADDR, xfer->txrxBuf, 3, i2cCallback, xfer);
    }

    return (ret == 0);
}

static void setMode(bool alsOn, uint8_t state)
{
    uint16_t regValue;

    regValue = (mTask.rangeNumber << 12) | (mTask.conversionTime << 11) | FOUR_FAULT_COUNT;

    if (alsOn)
        regValue |= (CONVERSION_MODE_CONTINUOUS << 9);
    else
        regValue |= (CONVERSION_MODE_SHUTDOWN << 9);


    writeRegister(OPT3007_REG_CONFIGURATION, regValue, state);
}


static void timerCallback(uint32_t timerId, void *cookie)
{
    osEnqueuePrivateEvt(EVT_SENSOR_ALS_TIMER, cookie, NULL, mTask.tid);
}


static bool sensorPowerAls(bool on, void *cookie)
{
    DEBUG_PRINT("sensorPowerAls: %d\n", on);

    if (on && !mTask.timerHandle) {
        mTask.timerHandle = timTimerSet(OPT3007_TIMER_DELAY, 0, 50, timerCallback, NULL, false);
    } else if (!on && mTask.timerHandle) {
        timTimerCancel(mTask.timerHandle);
        mTask.timerHandle = 0;
    }

    mTask.lastAlsSample.idata = OPT3007_ALS_INVALID;
    mTask.alsOn = on;

    setMode(on, (on) ? SENSOR_STATE_ENABLING_ALS : SENSOR_STATE_DISABLING_ALS);

    return true;
}


static bool sensorFirmwareAls(void *cookie)
{
    return sensorSignalInternalEvt(mTask.handle, SENSOR_INTERNAL_EVT_FW_STATE_CHG, 1, 0);
}


static bool sensorRateAls(uint32_t rate, uint64_t latency, void *cookie)
{
    DEBUG_PRINT("sensorRateAls: rate=%ld Hz latency=%lld ns\n", rate/1024, latency);

    mTask.alsRate = rate;

    return sensorSignalInternalEvt(mTask.handle, SENSOR_INTERNAL_EVT_RATE_CHG, rate, latency);
}

static bool sensorFlushAls(void *cookie)
{
    return osEnqueueEvt(sensorGetMyEventType(SENS_TYPE_ALS), SENSOR_DATA_EVENT_FLUSH, NULL);
}

static void sendCalibrationResult(uint8_t status, float offset) {
    struct AlsCalibrationData *data = heapAlloc(sizeof(struct AlsCalibrationData));
    if (!data) {
        osLog(LOG_WARN, "Couldn't alloc als cal result pkt");
        return;
    }

    data->header.appId = OPT3007_APP_ID;
    data->header.dataLen = (sizeof(struct AlsCalibrationData) - sizeof(struct HostHubRawPacket));
    data->data_header.msgId = SENSOR_APP_MSG_ID_CAL_RESULT;
    data->data_header.sensorType = SENS_TYPE_ALS;
    data->data_header.status = status;
    data->offset = offset;

    if (!osEnqueueEvtOrFree(EVT_APP_TO_HOST, data, heapFree))
        osLog(LOG_WARN, "Couldn't send als cal result evt");
}


static bool sensorCalibrateAls(void *cookie)
{
    DEBUG_PRINT("sensorCalibrateAls");

    if (mTask.alsOn) {
        INFO_PRINT("cannot calibrate while ALS is active\n");
        sendCalibrationResult(SENSOR_APP_EVT_STATUS_BUSY, 0.0f);
        return false;
    }

    mTask.alsOn = true;
    mTask.alsCalibration = true;
    mTask.lastAlsSample.idata = OPT3007_ALS_INVALID;
    mTask.alsOffset = 1.0f;

    setMode(true, SENSOR_STATE_CALIBRATION);

    return true;
}

static bool sensorCfgDataAls(void *data, void *cookie)
{
    DEBUG_PRINT("sensorCfgDataAls");

    mTask.alsOffset = *(float*)data;

    INFO_PRINT("Received als cfg data: %d\n", (int)mTask.alsOffset);

    return true;
}

static bool sendLastSampleAls(void *cookie, uint32_t tid) {
    bool result = true;

    INFO_PRINT("Send Last ALS data\n");
    if (mTask.lastAlsSample.idata != OPT3007_ALS_INVALID) {
        result = osEnqueuePrivateEvt(sensorGetMyEventType(SENS_TYPE_ALS), mTask.lastAlsSample.vptr, NULL, tid);
    }

    return result;
}

static const struct SensorInfo sensorInfoAls =
{
    .sensorName = "ALS",
    .supportedRates = supportedRates,
    .sensorType = SENS_TYPE_ALS,
    .numAxis = NUM_AXIS_EMBEDDED,
    .interrupt = NANOHUB_INT_NONWAKEUP,
    .minSamples = 20
};

static const struct SensorOps sensorOpsAls =
{
    .sensorPower = sensorPowerAls,
    .sensorFirmwareUpload = sensorFirmwareAls,
    .sensorSetRate = sensorRateAls,
    .sensorFlush = sensorFlushAls,
    .sensorTriggerOndemand = NULL,
    .sensorCalibrate = sensorCalibrateAls,
    .sensorCfgData = sensorCfgDataAls,
    .sensorSendOneDirectEvt = sendLastSampleAls
};


static void i2cCallback(void *cookie, size_t tx, size_t rx, int err)
{
    struct I2cTransfer *xfer = cookie;

    xfer->tx = tx;
    xfer->rx = rx;
    xfer->err = err;

    osEnqueuePrivateEvt(EVT_SENSOR_I2C, cookie, NULL, mTask.tid);
    if (err != 0)
        INFO_PRINT("i2c error (tx: %d, rx: %d, err: %d)\n", tx, rx, err);
}


static bool init_app(uint32_t myTid)
{
    mTask.tid = myTid;
    mTask.alsOn = false;
    mTask.alsCalibration = false;
    mTask.alsChangingGain = false;
    mTask.lastAlsSample.idata = OPT3007_ALS_INVALID;
    mTask.rangeNumber = DEFAULT_RANGE_NUMBER;
    mTask.conversionTime = CONVERSION_TIME_100MS;
    mTask.alsOffset = 1.0;
    mTask.alsRate = 0 ;

    mTask.handle = sensorRegister(&sensorInfoAls, &sensorOpsAls, NULL, false);

    osEventSubscribe(myTid, EVT_APP_START);

#ifdef EXYNOS_CONTEXTHUB
    ipc_set_sensor_info(SENSOR_TYPE_LIGHT, "OPT3007 ALS", VENDOR_NAME_TI,  0, 0);
#endif

    return true;
}

static void end_app(void)
{
    sensorUnregister(mTask.handle);
    i2cMasterRelease(I2C_BUS_ID);
}

static void verifySensorId(const struct I2cTransfer *xfer)
{
    struct I2cTransfer *nextXfer;
    uint8_t  regValue[2];
    uint16_t id;

    id = xfer->txrxBuf[0];
    id = (id << 8) | xfer->txrxBuf[1];
    DEBUG_PRINT("ID = 0x%02x%02x(%04x)\n", xfer->txrxBuf[0], xfer->txrxBuf[1],
                 OPT3007_DEVICE_ID);
        
    if (xfer->err !=0 || id != OPT3007_DEVICE_ID) {
        INFO_PRINT("not detected\n");
        sensorUnregister(mTask.handle);
        return;
    }

    nextXfer = allocXfer(SENSOR_STATE_FINISH_INIT);
    if (nextXfer == NULL) {
        return;
    }

    regValue[0] = (DEFAULT_RANGE_NUMBER << 4) | (CONVERSION_TIME_100MS << 3) |
                  (CONVERSION_MODE_SHUTDOWN << 1);
    regValue[1] = FOUR_FAULT_COUNT;

    i2cMasterTx(I2C_BUS_ID, I2C_ADDR, nextXfer->txrxBuf, 2, i2cCallback, nextXfer);
}

static void handleAlsSample(const struct I2cTransfer *xfer)
{
    union EmbeddedDataPoint sample = {0,};
    uint16_t exp;
    uint16_t frac;
    float lux;

    exp = (xfer->txrxBuf[0] >> 4);
    frac = (xfer->txrxBuf[0] & 0xF);
    frac = (frac << 8) | xfer->txrxBuf[1];

    lux = expValue[exp] * frac;

    DEBUG_PRINT("ALS sample : %x %x(lux=%f)\n", (unsigned int)exp,
                 (unsigned int)frac, (double)lux);

    if (mTask.alsOn) {
        sample.fdata = lux * mTask.alsOffset;

        if (mTask.alsCalibration) {
            sendCalibrationResult(SENSOR_APP_EVT_STATUS_SUCCESS, sample.fdata);
            mTask.alsOn = false;
            mTask.alsCalibration = false;
            return;
        } else if (mTask.alsRate != SENSOR_RATE_ONCHANGE) {
	          osEnqueueEvt(sensorGetMyEventType(SENS_TYPE_ALS), sample.vptr, NULL);
        } else if (mTask.lastAlsSample.idata != sample.idata){
	          osEnqueueEvt(sensorGetMyEventType(SENS_TYPE_ALS), sample.vptr, NULL);
        }
        mTask.lastAlsSample.fdata = sample.fdata;
    }
}

static void handle_i2c_event(struct I2cTransfer *xfer)
{
    int i;
    struct I2cTransfer *nextXfer;

    switch (xfer->state) {
    case SENSOR_STATE_VERIFY_ID:
        verifySensorId(xfer);
        break;
    case SENSOR_STATE_FINISH_INIT:
	      ipc_set_sensor_info(SENSOR_TYPE_LIGHT, NULL, NULL, SENS_TYPE_ALS,
                            (uint8_t)(OPT3007_DEVICE_ID & 0xFF));
        sensorRegisterInitComplete(mTask.handle);
        INFO_PRINT("Initialize Done.\n");
        break;
    case SENSOR_STATE_CALIBRATION:
        INFO_PRINT("Read ALS data for calibration\n");
        mTask.timerHandle = timTimerSet(150000000, 0, 50, timerCallback, NULL, true);
        break;
    case SENSOR_STATE_ENABLING_ALS:
        sensorSignalInternalEvt(mTask.handle, SENSOR_INTERNAL_EVT_POWER_STATE_CHG, true, 0);
        break;
    case SENSOR_STATE_DISABLING_ALS:
        sensorSignalInternalEvt(mTask.handle, SENSOR_INTERNAL_EVT_POWER_STATE_CHG, false, 0);
        break;
    case SENSOR_STATE_ALS_SAMPLING:
        handleAlsSample(xfer);
        break;
    }

    xfer->inUse = false;
}

static void handle_event(uint32_t evtType, const void* evtData)
{
    struct I2cTransfer *xfer;

    switch (evtType) {
    case EVT_APP_START:
        i2cMasterRequest(I2C_BUS_ID, I2C_SPEED);

        // Read Sensor ID
        xfer = allocXfer(SENSOR_STATE_VERIFY_ID);
        if (xfer != NULL) {
            xfer->txrxBuf[0] = OPT3007_REG_DEVICE_ID;
            i2cMasterTxRx(I2C_BUS_ID, I2C_ADDR, xfer->txrxBuf, 1, xfer->txrxBuf, 2, i2cCallback, xfer);
        }
        break;
    case EVT_SENSOR_I2C:
        handle_i2c_event((struct I2cTransfer *) evtData);
        break;
    case EVT_SENSOR_ALS_TIMER:
        xfer = allocXfer(SENSOR_STATE_ALS_SAMPLING);
        if (xfer != NULL) {
            xfer->txrxBuf[0] = OPT3007_REG_RESULT;
            i2cMasterTxRx(I2C_BUS_ID, I2C_ADDR, xfer->txrxBuf, 1, xfer->txrxBuf, 2, i2cCallback, xfer);
        }
        break;
    }
}

INTERNAL_APP_INIT(OPT3007_APP_ID, OPT3007_APP_VERSION, init_app, end_app, handle_event);
