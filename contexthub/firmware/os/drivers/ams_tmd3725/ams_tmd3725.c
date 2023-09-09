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

#include <stdlib.h>
#include <string.h>
#include <float.h>

#include <eventnums.h>
#include <gpio.h>
#include <timer.h>
#include <sensors.h>
#include <heap.h>
#include <hostIntf.h>
#include <isr.h>
#include <i2c.h>
#include <nanohubPacket.h>
#include <sensors.h>
#include <seos.h>

#include <plat/gpio/gpio.h>
#include <plat/syscfg.h>
#include <variant/variant.h>

#define AMS_TMD3725_APP_ID      APP_ID_MAKE(NANOHUB_VENDOR_GOOGLE, 12)
#define AMS_TMD3725_APP_VERSION 3

#ifndef PROX_INT_PIN
#error "PROX_INT_PIN is not defined; please define in variant.h"
#endif

#ifndef PROX_IRQ
#error "PROX_IRQ is not defined; please define in variant.h"
#endif

#define I2C_BUS_ID                             ALSP_I2C_BUS_ID
#define I2C_SPEED                              400000
#define I2C_ADDR                               0x39

#define AMS_TMD3725_REG_ENABLE                 0x80
#define AMS_TMD3725_REG_ATIME                  0x81
#define AMS_TMD3725_REG_PTIME                  0x82
#define AMS_TMD3725_REG_WTIME                  0x83
#define AMS_TMD3725_REG_AILTL                  0x84
#define AMS_TMD3725_REG_AILTH                  0x85
#define AMS_TMD3725_REG_AIHTL                  0x86
#define AMS_TMD3725_REG_AIHTH                  0x87
#define AMS_TMD3725_REG_PILT                   0x88
#define AMS_TMD3725_REG_PIHT                   0x8a
#define AMS_TMD3725_REG_PERS                   0x8c
#define AMS_TMD3725_REG_CFG0                   0x8d
#define AMS_TMD3725_REG_PCFG0                  0x8e
#define AMS_TMD3725_REG_PCFG1                  0x8f
#define AMS_TMD3725_REG_CFG1                   0x90
#define AMS_TMD3725_REG_REVID                  0x91
#define AMS_TMD3725_REG_ID                     0x92
#define AMS_TMD3725_REG_STATUS                 0x93
#define AMS_TMD3725_REG_CDATAL                 0x94
#define AMS_TMD3725_REG_CDATAH                 0x95
#define AMS_TMD3725_REG_RDATAL                 0x96
#define AMS_TMD3725_REG_RDATAH                 0x97
#define AMS_TMD3725_REG_GDATAL                 0x98
#define AMS_TMD3725_REG_GDATAH                 0x99
#define AMS_TMD3725_REG_BDATAL                 0x9A
#define AMS_TMD3725_REG_BDATAH                 0x9B
#define AMS_TMD3725_REG_PDATA                  0x9C
//#define AMS_TMD3725_REG_PDATAH                 0x9D
#define AMS_TMD3725_REG_REVID2                 0x9E
#define AMS_TMD3725_REG_CFG2                   0x9F
#define AMS_TMD3725_REG_CFG3                   0xAB

#define AMS_TMD3725_REG_POFFSETL               0xC0
#define AMS_TMD3725_REG_POFFSETH               0xC1

#define AMS_TMD3725_REG_AZ_CONFIG              0xD6
#define AMS_TMD3725_REG_CALIB                  0xD7
#define AMS_TMD3725_REG_CALIBCFG               0xD9
#define AMS_TMD3725_REG_CALIBSTAT              0xDC
#define AMS_TMD3725_REG_INTENAB                0xDD

#define AMS_TMD3725_ID                         0xE4

#define AMS_TMD3725_DEFAULT_RATE               SENSOR_HZ(5)

#define AMS_TMD3725_ATIME_SETTING              0x23 //100.8 ms
#define AMS_TMD3725_ATIME_MS                   ((AMS_TMD3725_ATIME_SETTING + 1) * 2.78)     // in milliseconds
#define AMS_TMD3725_MAX_ALS_CHANNEL_COUNT      ((AMS_TMD3725_ATIME_SETTING + 1) * 1024 - 1) // in ALS data units
#define AMS_TMD3725_ALS_MAX_REPORT_VALUE       150000.0f // in lux
#define AMS_TMD3725_PTIME_SETTING              0x38 // 5ms
#define AMS_TMD3725_PCFG0_SETTING              0x4F // pulse length: 16 us, pulse count: 23
#define AMS_TMD3725_PCFG1_SETTING              0x40 // gain: 1x, drive: 54 mA

#define AMS_TMD3725_ALS_DEBOUNCE_SAMPLES       5
#define AMS_TMD3725_ALS_GAIN_4X_THOLD          4000.0f
#define AMS_TMD3725_ALS_GAIN_16X_THOLD         1000.0f
#define AMS_TMD3725_ALS_GAIN_64X_THOLD         250.0f

#define ALS_C_COEFF     (1000)  // 1
#define ALS_R_COEFF     (-220)  // -0.22
#define ALS_G_COEFF     (110)   // 0.11
#define ALS_B_COEFF     (-1120) // -1.12
#define ALS_GA_FACTOR   (831)   // 831.54

/* AMS_TMD3725_REG_ENABLE */
#define PROX_ENABLE_BIT                        (1 << 2)
#define ALS_ENABLE_BIT                         (1 << 1)
#define POWER_ON_BIT                           (1 << 0)

/* AMS_TMD3725_REG_INTENAB */
#define PROX_INT_ENABLE_BIT                    (1 << 5)
#define ALS_INT_ENABLE_BIT                     (1 << 4)
#define CAL_INT_ENABLE_BIT                     (1 << 3)

#define AMS_TMD3725_REPORT_NEAR_VALUE          0.0f // centimeters
#define AMS_TMD3725_REPORT_FAR_VALUE           5.0f // centimeters
#define AMS_TMD3725_PROX_THRESHOLD_HIGH        100  // value in PS_DATA
#define AMS_TMD3725_PROX_THRESHOLD_LOW         25  // value in PS_DATA

#define AMS_TMD3725_ALS_INVALID                UINT32_MAX

#define AMS_TMD3725_ALS_TIMER_DELAY            200000000ULL

#define AMS_TMD3725_MAX_PENDING_I2C_REQUESTS   8
#define AMS_TMD3725_MAX_I2C_TRANSFER_SIZE      18

#define MIN2(a,b) (((a) < (b)) ? (a) : (b))
#define MAX2(a,b) (((a) > (b)) ? (a) : (b))

// NOTE: Define this to be 1 to enable streaming of proximity samples instead of
// using the interrupt
#define PROX_STREAMING 0
#define PROX_TIMER 0

#define INFO_PRINT(fmt, ...) do { \
        osLog(LOG_INFO, "%s " fmt, "[TMD3725]", ##__VA_ARGS__); \
    } while (0);

#define DEBUG_PRINT(fmt, ...) do { \
        if (enable_debug) {  \
            INFO_PRINT(fmt, ##__VA_ARGS__); \
        } \
    } while (0);

static const bool enable_debug = 0;

/* Private driver events */
enum SensorEvents
{
    EVT_SENSOR_I2C = EVT_APP_START + 1,
    EVT_SENSOR_ALS_TIMER,
    EVT_SENSOR_PROX_TIMER,
    EVT_SENSOR_ALS_INTERRUPT,
    EVT_SENSOR_PROX_INTERRUPT,
};

/* I2C state machine */
enum SensorState
{
    SENSOR_STATE_VERIFY_ID,
    SENSOR_STATE_INIT_0,
    SENSOR_STATE_INIT_1,
    SENSOR_STATE_INIT_2,
    SENSOR_STATE_FINISH_INIT,
    SENSOR_STATE_START_PROX_CALIBRATION_0,
    SENSOR_STATE_START_PROX_CALIBRATION_1,
    SENSOR_STATE_FINISH_PROX_CALIBRATION_0,
    SENSOR_STATE_FINISH_PROX_CALIBRATION_1,
    SENSOR_STATE_POLL_STATUS,
    SENSOR_STATE_ENABLING_ALS,
    SENSOR_STATE_ENABLING_PROX,
    SENSOR_STATE_ENABLING_PROX_2,
    SENSOR_STATE_DISABLING_ALS,
    SENSOR_STATE_DISABLING_ALS_2,
    SENSOR_STATE_DISABLING_PROX,
    SENSOR_STATE_DISABLING_PROX_2,
    SENSOR_STATE_DISABLING_PROX_3,
    SENSOR_STATE_ALS_CHANGING_GAIN,
    SENSOR_STATE_ALS_SAMPLING,
    SENSOR_STATE_ALS_CALIBRATION,
    SENSOR_STATE_PROX_SAMPLING,
    SENSOR_STATE_PROX_SAMPLING_2,
    SENSOR_STATE_PROX_TRANSITION_0,
    SENSOR_STATE_IDLE,
};

enum ProxState
{
    PROX_STATE_INIT,
    PROX_STATE_NEAR,
    PROX_STATE_FAR,
};

enum ProxOffsetIndex
{
    PROX_OFFSET_NORTH = 0,
    PROX_OFFSET_SOUTH = 1,
    PROX_OFFSET_WEST  = 2,
    PROX_OFFSET_EAST  = 3
};

enum AlsGain
{
    ALS_GAIN_1X  = 0,
    ALS_GAIN_4X  = 1,
    ALS_GAIN_16X = 2,
    ALS_GAIN_64X = 3
};

struct AlsProxTransfer
{
    size_t tx;
    size_t rx;
    int err;
    uint8_t txrxBuf[AMS_TMD3725_MAX_I2C_TRANSFER_SIZE];
    uint8_t state;
    bool inUse;
};

struct SensorData
{
    struct Gpio *pin;
    struct ChainedIsr isr;

    uint32_t tid;

    uint32_t alsHandle;
    uint32_t proxHandle;
    uint32_t alsTimerHandle;
    uint32_t proxTimerHandle;

    float alsOffset;

    union EmbeddedDataPoint lastAlsSample;

    struct AlsProxTransfer transfers[AMS_TMD3725_MAX_PENDING_I2C_REQUESTS];

    uint8_t lastProxState; // enum ProxState

    uint8_t alsGain;
    uint8_t nextAlsGain;
    uint8_t alsDebounceSamples;

    bool alsOn;
    bool proxOn;
    bool alsCalibrating;
    bool proxCalibrating;
    bool proxDirectMode;
    bool alsChangingGain;
    bool alsSkipSample;
};

static struct SensorData mTask;

struct AlsCalibrationData {
    struct HostHubRawPacket header;
    struct SensorAppEventHeader data_header;
    float offset;
} __attribute__((packed));

struct ProxCalibrationData {
    struct HostHubRawPacket header;
    struct SensorAppEventHeader data_header;
    int32_t offsets[4];
} __attribute__((packed));

static const uint32_t supportedRates[] =
{
    SENSOR_HZ(5),
    SENSOR_RATE_ONCHANGE,
    0,
};

static void i2cCallback(void *cookie, size_t tx, size_t rx, int err);

/*
 * Helper functions
 */

// Allocate a buffer and mark it as in use with the given state, or return NULL
// if no buffers available. Must *not* be called from interrupt context.
static struct AlsProxTransfer *allocXfer(uint8_t state)
{
    size_t i;

    for (i = 0; i < ARRAY_SIZE(mTask.transfers); i++) {
        if (!mTask.transfers[i].inUse) {
            mTask.transfers[i].inUse = true;
            mTask.transfers[i].state = state;
            return &mTask.transfers[i];
        }
    }

    INFO_PRINT("Ran out of i2c buffers!");
    return NULL;
}

// Helper function to write a one byte register. Returns true if we got a
// successful return value from i2cMasterTx().
static bool writeRegister(uint8_t reg, uint8_t value, uint8_t state)
{
    struct AlsProxTransfer *xfer = allocXfer(state);
    int ret = -1;

    if (xfer != NULL) {
        xfer->txrxBuf[0] = reg;
        xfer->txrxBuf[1] = value;
        ret = i2cMasterTx(I2C_BUS_ID, I2C_ADDR, xfer->txrxBuf, 2, i2cCallback, xfer);
    }

    return (ret == 0);
}

static bool writeRegisters(uint8_t reg, uint8_t * value, uint8_t size, uint8_t state)
{
    struct AlsProxTransfer *xfer = allocXfer(state);
    int ret = -1;
    int i;

    if (xfer != NULL) {
        xfer->txrxBuf[0] = reg;
        for (i = 0; i < size; i++)
            xfer->txrxBuf[i + 1] = value[i];

        ret = i2cMasterTx(I2C_BUS_ID, I2C_ADDR, xfer->txrxBuf, size + 1, i2cCallback, xfer);
    }

    return (ret == 0);
}

static bool proxIsr(struct ChainedIsr *localIsr)
{
    INFO_PRINT("proxIsr\n");
    struct SensorData *data = &mTask;
    uint8_t lastProxState = data->lastProxState;
    union EmbeddedDataPoint sample;
    bool pinState;

    if (!gpioIsPending(PROX_INT_PIN)) {
        return false;
    }

    pinState = gpioGetData(PROX_INT_PIN);

    if (data->proxOn) {
#if PROX_STREAMING
        (void)sample;
        (void)pinState;
        (void)lastProxState;
        if (!pinState)
            osEnqueuePrivateEvt(EVT_SENSOR_PROX_INTERRUPT, NULL, NULL, mTask.tid);
#else
        if (data->proxDirectMode) {
            sample.fdata = (pinState) ? AMS_TMD3725_REPORT_FAR_VALUE : AMS_TMD3725_REPORT_NEAR_VALUE;
            data->lastProxState = (pinState) ? PROX_STATE_FAR : PROX_STATE_NEAR;
            if (data->lastProxState != lastProxState)
                osEnqueueEvt(sensorGetMyEventType(SENS_TYPE_PROX), sample.vptr, NULL);
        } else {
            osEnqueuePrivateEvt(EVT_SENSOR_PROX_INTERRUPT, NULL, NULL, mTask.tid);
        }
#endif
    } else if (data->alsOn && data->alsCalibrating && !pinState) {
        osEnqueuePrivateEvt(EVT_SENSOR_ALS_INTERRUPT, NULL, NULL, mTask.tid);
    }

    gpioClearPending(PROX_INT_PIN);
    return true;
}

#if !PROX_TIMER
static bool enableInterrupt(struct Gpio *pin, struct ChainedIsr *isr)
{
    gpioSetInterrupt(pin, Falling_Edge, eDigitalFLT, 0x10, (void (*)(void))(isr->func));
    return true;
}
#endif

static bool disableInterrupt(struct Gpio *pin, struct ChainedIsr *isr)
{
    gpioUnsetExtInterrupt(PROX_INT_PIN);
    return true;
}

static void i2cCallback(void *cookie, size_t tx, size_t rx, int err)
{
    struct AlsProxTransfer *xfer = cookie;

    xfer->tx = tx;
    xfer->rx = rx;
    xfer->err = err;

    osEnqueuePrivateEvt(EVT_SENSOR_I2C, cookie, NULL, mTask.tid);
    if (err != 0)
        INFO_PRINT("i2c error (tx: %d, rx: %d, err: %d)\n", tx, rx, err);
}

static void alsTimerCallback(uint32_t timerId, void *cookie)
{
    osEnqueuePrivateEvt(EVT_SENSOR_ALS_TIMER, cookie, NULL, mTask.tid);
}

#if PROX_TIMER
static void proxTimerCallback(uint32_t timerId, void *cookie)
{
    osEnqueuePrivateEvt(EVT_SENSOR_PROX_TIMER, cookie, NULL, mTask.tid);
}
#endif

static inline uint32_t getAlsGainFromSetting(uint8_t gainSetting)
{
    return 0x1 << (2 * gainSetting);
}

static float getLuxFromAlsData(uint16_t c, uint16_t r, uint16_t g, uint16_t b)
{
    float lux;

    lux = (float)c / getAlsGainFromSetting(mTask.alsGain);

    return lux;
}

static bool checkForAlsAutoGain(float sample)
{
    if ((mTask.alsGain != ALS_GAIN_64X) && (sample < AMS_TMD3725_ALS_GAIN_64X_THOLD)) {
        mTask.alsDebounceSamples = (mTask.nextAlsGain == ALS_GAIN_64X) ? (mTask.alsDebounceSamples + 1) : 1;
        mTask.nextAlsGain = ALS_GAIN_64X;
    } else if ((mTask.alsGain != ALS_GAIN_16X) && (sample >= AMS_TMD3725_ALS_GAIN_64X_THOLD) && (sample < AMS_TMD3725_ALS_GAIN_16X_THOLD)) {
        mTask.alsDebounceSamples = (mTask.nextAlsGain == ALS_GAIN_16X) ? (mTask.alsDebounceSamples + 1) : 1;
        mTask.nextAlsGain = ALS_GAIN_16X;
    } else if ((mTask.alsGain != ALS_GAIN_4X) && (sample >= AMS_TMD3725_ALS_GAIN_16X_THOLD) && (sample < AMS_TMD3725_ALS_GAIN_4X_THOLD)) {
        mTask.alsDebounceSamples = (mTask.nextAlsGain == ALS_GAIN_4X) ? (mTask.alsDebounceSamples + 1) : 1;
        mTask.nextAlsGain = ALS_GAIN_4X;
    } else if ((mTask.alsGain != ALS_GAIN_1X) && (sample >= AMS_TMD3725_ALS_GAIN_4X_THOLD)) {
        mTask.alsDebounceSamples = (mTask.nextAlsGain == ALS_GAIN_1X) ? (mTask.alsDebounceSamples + 1) : 1;
        mTask.nextAlsGain = ALS_GAIN_1X;
    }

    return (mTask.alsDebounceSamples >= AMS_TMD3725_ALS_DEBOUNCE_SAMPLES);
}

static void sendCalibrationResultAls(uint8_t status, float offset) {
    struct AlsCalibrationData *data = heapAlloc(sizeof(struct AlsCalibrationData));
    if (!data) {
        osLog(LOG_WARN, "Couldn't alloc als cal result pkt");
        return;
    }

    data->header.appId = AMS_TMD3725_APP_ID;
    data->header.dataLen = (sizeof(struct AlsCalibrationData) - sizeof(struct HostHubRawPacket));
    data->data_header.msgId = SENSOR_APP_MSG_ID_CAL_RESULT;
    data->data_header.sensorType = SENS_TYPE_ALS;
    data->data_header.status = status;
    data->offset = offset;

    if (!osEnqueueEvtOrFree(EVT_APP_TO_HOST, data, heapFree))
        osLog(LOG_WARN, "Couldn't send als cal result evt");
}

static void sendCalibrationResultProx(uint8_t status, int16_t *offsets) {
    int i;

    struct ProxCalibrationData *data = heapAlloc(sizeof(struct ProxCalibrationData));
    if (!data) {
        osLog(LOG_WARN, "Couldn't alloc prox cal result pkt");
        return;
    }

    data->header.appId = AMS_TMD3725_APP_ID;
    data->header.dataLen = (sizeof(struct ProxCalibrationData) - sizeof(struct HostHubRawPacket));
    data->data_header.msgId = SENSOR_APP_MSG_ID_CAL_RESULT;
    data->data_header.sensorType = SENS_TYPE_PROX;
    data->data_header.status = status;

    // The offsets are cast from int16_t to int32_t, so I can't use memcpy
    for (i = 0; i < 4; i++)
        data->offsets[i] = offsets[i];

    if (!osEnqueueEvtOrFree(EVT_APP_TO_HOST, data, heapFree))
        osLog(LOG_WARN, "Couldn't send prox cal result evt");
}

static void setMode(bool alsOn, bool proxOn, uint8_t state)
{
    uint8_t regEnable =
        ((alsOn || proxOn) ? POWER_ON_BIT : 0) |
        (alsOn ? ALS_ENABLE_BIT : 0) |
        (proxOn ? PROX_ENABLE_BIT : 0);
    writeRegister(AMS_TMD3725_REG_ENABLE, regEnable, state);
}

static bool sensorPowerAls(bool on, void *cookie)
{
    INFO_PRINT("sensorPowerAls: %d\n", on);

    if (on && !mTask.alsTimerHandle) {
        mTask.alsTimerHandle = timTimerSet(AMS_TMD3725_ALS_TIMER_DELAY, 0, 50, alsTimerCallback, NULL, false);
    } else if (!on && mTask.alsTimerHandle) {
        timTimerCancel(mTask.alsTimerHandle);
        mTask.alsTimerHandle = 0;
    }

    mTask.lastAlsSample.idata = AMS_TMD3725_ALS_INVALID;
    mTask.alsOn = on;
    mTask.nextAlsGain = ALS_GAIN_4X;
    mTask.alsChangingGain = false;
    // skip first sample to make sure we get an entire integration cycle
    mTask.alsSkipSample = true;
    mTask.alsDebounceSamples = 0;

    setMode(on, mTask.proxOn, (on) ? SENSOR_STATE_ENABLING_ALS : SENSOR_STATE_DISABLING_ALS);
    return true;
}

static bool sensorFirmwareAls(void *cookie)
{
    return sensorSignalInternalEvt(mTask.alsHandle, SENSOR_INTERNAL_EVT_FW_STATE_CHG, 1, 0);
}

static bool sensorRateAls(uint32_t rate, uint64_t latency, void *cookie)
{
    if (rate == SENSOR_RATE_ONCHANGE)
        rate = AMS_TMD3725_DEFAULT_RATE;

    DEBUG_PRINT("sensorRateAls: rate=%ld Hz latency=%lld ns\n", rate/1024, latency);

    return sensorSignalInternalEvt(mTask.alsHandle, SENSOR_INTERNAL_EVT_RATE_CHG, rate, latency);
}

static bool sensorFlushAls(void *cookie)
{
    return osEnqueueEvt(sensorGetMyEventType(SENS_TYPE_ALS), SENSOR_DATA_EVENT_FLUSH, NULL);
}

static bool sensorCalibrateAls(void *cookie)
{
    INFO_PRINT("sensorCalibrateAls");

    if (mTask.alsOn || mTask.proxOn) {
        INFO_PRINT("cannot calibrate while als or prox are active\n");
        sendCalibrationResultAls(SENSOR_APP_EVT_STATUS_BUSY, 0.0f);
        return false;
    }

    mTask.alsOn = true;
    mTask.lastAlsSample.idata = AMS_TMD3725_ALS_INVALID;
    mTask.alsCalibrating = true;
    mTask.alsOffset = 1.0f;
    mTask.alsChangingGain = false;
    mTask.alsSkipSample = false;

    //extiClearPendingGpio(mTask.pin);
    //enableInterrupt(mTask.pin, &mTask.isr);

    return writeRegister(AMS_TMD3725_REG_ENABLE, (POWER_ON_BIT | ALS_ENABLE_BIT),
                         SENSOR_STATE_ALS_CALIBRATION);
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

    // If we don't end up doing anything here, the expectation is that we are powering up/haven't got the
    // first sample yet, so the client will get a broadcast event soon
    if (mTask.lastAlsSample.idata != AMS_TMD3725_ALS_INVALID) {
        result = osEnqueuePrivateEvt(sensorGetMyEventType(SENS_TYPE_ALS), mTask.lastAlsSample.vptr, NULL, tid);
    }
    return result;
}

static bool sensorPowerProx(bool on, void *cookie)
{
    INFO_PRINT("sensorPowerProx: %d\n", on);
#if PROX_TIMER
    if (on && !mTask.alsTimerHandle) {
        mTask.proxTimerHandle = timTimerSet(AMS_TMD3725_ALS_TIMER_DELAY, 0, 50, proxTimerCallback, NULL, false);
    } else if (!on && mTask.proxTimerHandle) {
        timTimerCancel(mTask.proxTimerHandle);
        mTask.proxTimerHandle = 0;
    }


#else
    if (on) {
        gpioClearPending(PROX_INT_PIN);
        enableInterrupt(mTask.pin, &mTask.isr);
    } else {
        disableInterrupt(mTask.pin, &mTask.isr);
        gpioClearPending(PROX_INT_PIN);
    }
#endif

    mTask.lastProxState = PROX_STATE_INIT;
    mTask.proxOn = on;
    mTask.proxDirectMode = false;

    setMode(mTask.alsOn, on, (on) ? SENSOR_STATE_ENABLING_PROX : SENSOR_STATE_DISABLING_PROX);
    return true;
}

static bool sensorFirmwareProx(void *cookie)
{
    return sensorSignalInternalEvt(mTask.proxHandle, SENSOR_INTERNAL_EVT_FW_STATE_CHG, 1, 0);
}

static bool sensorRateProx(uint32_t rate, uint64_t latency, void *cookie)
{
    if (rate == SENSOR_RATE_ONCHANGE)
        rate = AMS_TMD3725_DEFAULT_RATE;

    DEBUG_PRINT("sensorRateProx: rate=%ld Hz latency=%lld ns\n", rate/1024, latency);

    return sensorSignalInternalEvt(mTask.proxHandle, SENSOR_INTERNAL_EVT_RATE_CHG, rate, latency);
}

static bool sensorFlushProx(void *cookie)
{
    return osEnqueueEvt(sensorGetMyEventType(SENS_TYPE_PROX), SENSOR_DATA_EVENT_FLUSH, NULL);
}

/*
static bool sensorCalibrateProx(void *cookie)
{
    int16_t failOffsets[4] = {0, 0, 0, 0};
    DEBUG_PRINT("sensorCalibrateProx");

    if (mTask.alsOn || mTask.proxOn) {
        INFO_PRINT("cannot calibrate while als or prox are active\n");
        sendCalibrationResultProx(SENSOR_APP_EVT_STATUS_BUSY, failOffsets);
        return false;
    }

    mTask.lastProxState = PROX_STATE_INIT;
    mTask.proxOn = true;
    mTask.proxCalibrating = true;
    mTask.proxDirectMode = false;

    extiClearPendingGpio(mTask.pin);
    enableInterrupt(mTask.pin, &mTask.isr);

    return writeRegister(AMS_TMD3725_REG_ENABLE, POWER_ON_BIT,
                         SENSOR_STATE_START_PROX_CALIBRATION_0);
}

static bool sensorCfgDataProx(void *data, void *cookie)
{
    struct AlsProxTransfer *xfer;
    int32_t *offsets = (int32_t *) data;

    INFO_PRINT("Received cfg data: {%d, %d, %d, %d}\n",
                (int)offsets[0], (int)offsets[1], (int)offsets[2], (int)offsets[3]);

    xfer = allocXfer(SENSOR_STATE_IDLE);
    if (xfer != NULL) {
        xfer->txrxBuf[0] = AMS_TMD3725_REG_OFFSETNL;
        *((int16_t*)&xfer->txrxBuf[1]) = offsets[0];
        *((int16_t*)&xfer->txrxBuf[3]) = offsets[1];
        *((int16_t*)&xfer->txrxBuf[5]) = offsets[2];
        *((int16_t*)&xfer->txrxBuf[7]) = offsets[3];
        i2cMasterTx(I2C_BUS_ID, I2C_ADDR, xfer->txrxBuf, 9, i2cCallback, xfer);
    }
    return true;
}
*/

static bool sendLastSampleProx(void *cookie, uint32_t tid) {
    union EmbeddedDataPoint sample;
    bool result = true;

    // See note in sendLastSampleAls
    if (mTask.lastProxState != PROX_STATE_INIT) {
        sample.fdata = (mTask.lastProxState == PROX_STATE_NEAR) ? AMS_TMD3725_REPORT_NEAR_VALUE : AMS_TMD3725_REPORT_FAR_VALUE;
        result = osEnqueuePrivateEvt(sensorGetMyEventType(SENS_TYPE_PROX), sample.vptr, NULL, tid);
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

static const struct SensorInfo sensorInfoProx =
{
    .sensorName = "Proximity",
    .supportedRates = supportedRates,
    .sensorType = SENS_TYPE_PROX,
    .numAxis = NUM_AXIS_EMBEDDED,
    .interrupt = NANOHUB_INT_WAKEUP,
    .minSamples = 300
};

static const struct SensorOps sensorOpsProx =
{
    .sensorPower = sensorPowerProx,
    .sensorFirmwareUpload = sensorFirmwareProx,
    .sensorSetRate = sensorRateProx,
    .sensorFlush = sensorFlushProx,
    .sensorTriggerOndemand = NULL,
    //.sensorCalibrate = sensorCalibrateProx,
    //.sensorCfgData = sensorCfgDataProx,
    .sensorSendOneDirectEvt = sendLastSampleProx
};

static void verifySensorId(const struct AlsProxTransfer *xfer)
{
    struct AlsProxTransfer *nextXfer;
    INFO_PRINT("REVID = 0x%02x, ID = 0x%02x\n", xfer->txrxBuf[0], xfer->txrxBuf[1]);

    // Check the sensor ID
    if (xfer->err != 0 || xfer->txrxBuf[1] != AMS_TMD3725_ID) {
        INFO_PRINT("not detected\n");
        sensorUnregister(mTask.alsHandle);
        sensorUnregister(mTask.proxHandle);
        return;
    }

    nextXfer = allocXfer(SENSOR_STATE_INIT_0);
    if (nextXfer == NULL) {
        return;
    }

    // There is no SW reset on the AMS TMD3725, so we have to reset all registers manually
    nextXfer->txrxBuf[0]  = AMS_TMD3725_REG_ENABLE;
    nextXfer->txrxBuf[1]  = 0x00;                                           // REG_ENABLE - reset value from datasheet
    nextXfer->txrxBuf[2]  = AMS_TMD3725_ATIME_SETTING;                      // REG_ATIME - 100 ms
    nextXfer->txrxBuf[3]  = AMS_TMD3725_PTIME_SETTING;                      // REG_PTIME - 50 ms
    nextXfer->txrxBuf[4]  = 0x00;                                           // REG_WTIME - reset value from datasheet
    nextXfer->txrxBuf[5]  = 0x00;                                           // REG_AILTL - reset value from datasheet
    nextXfer->txrxBuf[6]  = 0x00;                                           // REG_AILTH - reset value from datasheet
    nextXfer->txrxBuf[7]  = 0x00;                                           // REG_AIHTL - reset value from datasheet
    nextXfer->txrxBuf[8]  = 0x00;                                           // REG_AIHTH - reset value from datasheet
    nextXfer->txrxBuf[9]  = AMS_TMD3725_PROX_THRESHOLD_LOW;                 // REG_PILTL
    nextXfer->txrxBuf[11] = AMS_TMD3725_PROX_THRESHOLD_HIGH;                // REG_PIHTL
    nextXfer->txrxBuf[13] = 0x10;                                           // REG_PERS - reset value from datasheet
    nextXfer->txrxBuf[14] = 0x80;                                           // REG_CFG0 - reset value from datasheet
    nextXfer->txrxBuf[15] = AMS_TMD3725_PCFG0_SETTING;                      // REG_PCFG0
    nextXfer->txrxBuf[16] = AMS_TMD3725_PCFG1_SETTING;                      // REG_PCFG1
    nextXfer->txrxBuf[17] = mTask.alsGain;                                  // REG_CFG1

    i2cMasterTx(I2C_BUS_ID, I2C_ADDR, nextXfer->txrxBuf, 18, i2cCallback, nextXfer);
}

static void handleAlsSample(const struct AlsProxTransfer *xfer)
{
    union EmbeddedDataPoint sample;
    uint16_t c = *(uint16_t*)(xfer->txrxBuf);
    uint16_t r = *(uint16_t*)(xfer->txrxBuf+2);
    uint16_t g = *(uint16_t*)(xfer->txrxBuf+4);
    uint16_t b = *(uint16_t*)(xfer->txrxBuf+6);

    if (mTask.alsOn) {
        sample.fdata = getLuxFromAlsData(c, r, g, b);
        DEBUG_PRINT("als sample ready: c=%u r=%u g=%u b=%u, gain=%dx, lux=%d\n", c, r, g, b,
                    (int)getAlsGainFromSetting(mTask.alsGain), (int)sample.fdata);

        if (mTask.alsCalibrating) {
            sendCalibrationResultAls(SENSOR_APP_EVT_STATUS_SUCCESS, sample.fdata);

            mTask.alsOn = false;
            mTask.alsCalibrating = false;

            writeRegister(AMS_TMD3725_REG_ENABLE, 0, SENSOR_STATE_IDLE);
        } else if (mTask.alsSkipSample) {
            mTask.alsSkipSample = false;
        } else if (!mTask.alsChangingGain) {
            if (mTask.lastAlsSample.idata != sample.idata) {
                sample.fdata = sample.fdata * mTask.alsOffset;
                osEnqueueEvt(sensorGetMyEventType(SENS_TYPE_ALS), sample.vptr, NULL);
                mTask.lastAlsSample.fdata = sample.fdata;
            }

            if (checkForAlsAutoGain(sample.fdata)) {
                DEBUG_PRINT("Changing ALS gain from %dx to %dx\n", (int)getAlsGainFromSetting(mTask.alsGain),
                            (int)getAlsGainFromSetting(mTask.nextAlsGain));
                if (writeRegister(AMS_TMD3725_REG_CFG1, mTask.nextAlsGain,
                                  SENSOR_STATE_ALS_CHANGING_GAIN)) {
                    mTask.alsChangingGain = true;
                }
            }
        }
    }
}

static void handleProxSample(const struct AlsProxTransfer *xfer)
{
    union EmbeddedDataPoint sample;
    uint8_t ps = xfer->txrxBuf[0];
    uint8_t lastProxState = mTask.lastProxState;
    uint8_t tx[AMS_TMD3725_MAX_I2C_TRANSFER_SIZE];

    INFO_PRINT("prox sample ready: prox=%u\n", ps);
    if (mTask.proxOn) {
#if PROX_STREAMING || PROX_TIMER
        (void)lastProxState;
        sample.fdata = ps;
        osEnqueueEvt(sensorGetMyEventType(SENS_TYPE_PROX), sample.vptr, NULL);
#else
        // Lower the bar for "near" threshold so it reports "near" when the prox
        // value is within the hysteresis threshold
        if (ps > AMS_TMD3725_PROX_THRESHOLD_LOW) {
            sample.fdata = AMS_TMD3725_REPORT_NEAR_VALUE;
            mTask.lastProxState = PROX_STATE_NEAR;
        } else {
            sample.fdata = AMS_TMD3725_REPORT_FAR_VALUE;
            mTask.lastProxState = PROX_STATE_FAR;
        }

        if (mTask.lastProxState != lastProxState) {
            osEnqueueEvt(sensorGetMyEventType(SENS_TYPE_PROX), sample.vptr, NULL);
            if (mTask.lastProxState == PROX_STATE_FAR) {
                tx[0] = 0x0;
                tx[2] = AMS_TMD3725_PROX_THRESHOLD_HIGH;
            } else {
                tx[0] = AMS_TMD3725_PROX_THRESHOLD_LOW;
                tx[2] = 0xFF;
            }
            writeRegisters(AMS_TMD3725_REG_PILT, tx, 3, SENSOR_STATE_PROX_SAMPLING_2);
        } else {
            // Clear Pending
            writeRegister(AMS_TMD3725_REG_STATUS, PROX_INT_ENABLE_BIT, SENSOR_STATE_IDLE);
        }

#endif

#if PROX_STREAMING
        // reset proximity interrupts
        //writeRegister(AMS_TMD3725_REG_INTCLEAR, 0x60, SENSOR_STATE_IDLE);
#else
        // The TMD3725 direct interrupt mode does not work properly if enabled while something is covering the sensor,
        // so we need to wait until it is far.
        if (mTask.lastProxState == PROX_STATE_FAR) {
            //disableInterrupt(mTask.pin, &mTask.isr);
            //extiClearPendingGpio(mTask.pin);

            // Switch to proximity interrupt direct mode
            //writeRegister(AMS_TMD3725_REG_CFG4, 0x27, SENSOR_STATE_PROX_TRANSITION_0);
        } else {
            // If we are in the "near" state, we cannot change to direct interrupt mode, so just clear the interrupt
            //writeRegister(AMS_TMD3725_REG_INTCLEAR, 0x60, SENSOR_STATE_IDLE);
        }
#endif
    }
}

static void readSensorInit(const struct AlsProxTransfer *xfer)
{
    struct AlsProxTransfer *nextXfer;
    nextXfer = allocXfer(SENSOR_STATE_FINISH_INIT);
    if (nextXfer == NULL) {
        return;
    }

    nextXfer->txrxBuf[0]  = AMS_TMD3725_REG_ENABLE;
    
    i2cMasterTxRx(I2C_BUS_ID, I2C_ADDR, nextXfer->txrxBuf, 1, nextXfer->txrxBuf, 17, i2cCallback, nextXfer);
}

/*
 * Sensor i2c state machine
 */
static void handle_i2c_event(struct AlsProxTransfer *xfer)
{
    int i;
    uint8_t tx[AMS_TMD3725_MAX_I2C_TRANSFER_SIZE];
    struct AlsProxTransfer *nextXfer;

    switch (xfer->state) {
    case SENSOR_STATE_VERIFY_ID:
        verifySensorId(xfer);
        break;

    case SENSOR_STATE_INIT_0:
        tx[0] = 0x4;
        tx[1] = 0x10;
        writeRegisters(AMS_TMD3725_REG_CFG2, tx, 2, SENSOR_STATE_INIT_1);
        break;

    case SENSOR_STATE_INIT_1:
        readSensorInit(xfer);
        break;

    case SENSOR_STATE_FINISH_INIT:

        for (i = 0; i < 17; i++)
            DEBUG_PRINT("REG 0x%X = 0x%X", 0x80 + i, xfer->txrxBuf[i]);

        INFO_PRINT("initialize done\n");
        sensorRegisterInitComplete(mTask.alsHandle);
        sensorRegisterInitComplete(mTask.proxHandle);
        break;

    case SENSOR_STATE_START_PROX_CALIBRATION_0:
        // Write REG_INTENAB to enable calibration interrupt
        writeRegister(AMS_TMD3725_REG_INTENAB, CAL_INT_ENABLE_BIT,
                      SENSOR_STATE_START_PROX_CALIBRATION_1);
        break;

    case SENSOR_STATE_START_PROX_CALIBRATION_1:
        // Write REG_CALIB to start calibration
        writeRegister(AMS_TMD3725_REG_CALIB, 0x01, SENSOR_STATE_IDLE);
        break;

    case SENSOR_STATE_FINISH_PROX_CALIBRATION_0:
        disableInterrupt(mTask.pin, &mTask.isr);
        gpioClearPending(PROX_INT_PIN);

        mTask.proxOn = false;
        mTask.proxCalibrating = false;

        INFO_PRINT("Calibration offsets = {%d, %d, %d, %d}\n", *((int16_t*)&xfer->txrxBuf[0]),
                    *((int16_t*)&xfer->txrxBuf[2]), *((int16_t*)&xfer->txrxBuf[4]),
                    *((int16_t*)&xfer->txrxBuf[6]));

        // Send calibration result
        sendCalibrationResultProx(SENSOR_APP_EVT_STATUS_SUCCESS, (int16_t*)xfer->txrxBuf);

        // Write REG_INTENAB to disable all interrupts
        writeRegister(AMS_TMD3725_REG_INTENAB, 0x00,
                      SENSOR_STATE_FINISH_PROX_CALIBRATION_1);
        break;

    case SENSOR_STATE_FINISH_PROX_CALIBRATION_1:
        writeRegister(AMS_TMD3725_REG_ENABLE, 0, SENSOR_STATE_IDLE);
        break;

    case SENSOR_STATE_ENABLING_ALS:
        sensorSignalInternalEvt(mTask.alsHandle, SENSOR_INTERNAL_EVT_POWER_STATE_CHG, true, 0);
        break;

    case SENSOR_STATE_ENABLING_PROX:
        writeRegister(AMS_TMD3725_REG_INTENAB, PROX_INT_ENABLE_BIT, SENSOR_STATE_ENABLING_PROX_2);
        break;

    case SENSOR_STATE_ENABLING_PROX_2:
        sensorSignalInternalEvt(mTask.proxHandle, SENSOR_INTERNAL_EVT_POWER_STATE_CHG, true, 0);
        break;

    case SENSOR_STATE_DISABLING_ALS:
        // Reset AGAIN to 4x
        mTask.alsGain = ALS_GAIN_4X;
        writeRegister(AMS_TMD3725_REG_CFG1, mTask.alsGain, SENSOR_STATE_DISABLING_ALS_2);
        break;

    case SENSOR_STATE_DISABLING_ALS_2:
        sensorSignalInternalEvt(mTask.alsHandle, SENSOR_INTERNAL_EVT_POWER_STATE_CHG, false, 0);
        break;

    case SENSOR_STATE_DISABLING_PROX:
        writeRegister(AMS_TMD3725_REG_INTENAB, 0, SENSOR_STATE_DISABLING_PROX_2);
        break;

    case SENSOR_STATE_DISABLING_PROX_2:
        writeRegister(AMS_TMD3725_REG_STATUS, PROX_INT_ENABLE_BIT, SENSOR_STATE_DISABLING_PROX_3);
        break;

    case SENSOR_STATE_DISABLING_PROX_3:
        sensorSignalInternalEvt(mTask.proxHandle, SENSOR_INTERNAL_EVT_POWER_STATE_CHG, false, 0);
        break;

    case SENSOR_STATE_ALS_CHANGING_GAIN:
        if (mTask.alsOn) {
            mTask.alsChangingGain = false;
            mTask.alsGain = mTask.nextAlsGain;
            mTask.alsDebounceSamples = 0;
            mTask.alsSkipSample = true;
        }
        break;

    case SENSOR_STATE_ALS_SAMPLING:
        handleAlsSample(xfer);
        break;

    case SENSOR_STATE_PROX_SAMPLING:
        handleProxSample(xfer);
        break;

    case SENSOR_STATE_PROX_SAMPLING_2:
        writeRegister(AMS_TMD3725_REG_STATUS, PROX_INT_ENABLE_BIT, SENSOR_STATE_IDLE);
        break;

    case SENSOR_STATE_PROX_TRANSITION_0:
        if (mTask.proxOn) {
            mTask.proxDirectMode = true;
            gpioClearPending(PROX_INT_PIN);
            //enableInterrupt(mTask.pin, &mTask.isr, EXTI_TRIGGER_BOTH);
        }
        break;
    case SENSOR_STATE_ALS_CALIBRATION:
        INFO_PRINT("Read ALS data for calibration\n");
        nextXfer = allocXfer(SENSOR_STATE_ALS_SAMPLING);
        if (nextXfer != NULL) {
            nextXfer->txrxBuf[0] = AMS_TMD3725_REG_CDATAL;
            i2cMasterTxRx(I2C_BUS_ID, I2C_ADDR, nextXfer->txrxBuf, 1, nextXfer->txrxBuf, 8, i2cCallback, nextXfer);
        }
        break;

    default:
        break;
    }

    xfer->inUse = false;
}

/*
 * Main driver entry points
 */

static bool init_app(uint32_t myTid)
{
    /* Set up driver private data */
    mTask.tid = myTid;
    mTask.alsOn = false;
    mTask.proxOn = false;
    mTask.lastAlsSample.idata = AMS_TMD3725_ALS_INVALID;
    mTask.lastProxState = PROX_STATE_INIT;
    mTask.proxCalibrating = false;
    mTask.alsOffset = 1.0f;
    mTask.alsGain = ALS_GAIN_4X;

    mTask.pin = gpioRequest(PROX_INT_PIN);
    mTask.isr.func = proxIsr;

    mTask.alsHandle = sensorRegister(&sensorInfoAls, &sensorOpsAls, NULL, false);
    mTask.proxHandle = sensorRegister(&sensorInfoProx, &sensorOpsProx, NULL, false);

    osEventSubscribe(myTid, EVT_APP_START);
#ifdef EXYNOS_CONTEXTHUB
    ipc_set_sensor_info(SENSOR_TYPE_PROXIMITY, "TMD3725 Proximity", VENDOR_NAME_AMS, 0, 0);
    ipc_set_sensor_info(SENSOR_TYPE_LIGHT, "TMD3725 Light", VENDOR_NAME_AMS, 0, 0);
#endif

    return true;
}

static void end_app(void)
{
    disableInterrupt(mTask.pin, &mTask.isr);
    gpioRelease(mTask.pin);

    sensorUnregister(mTask.alsHandle);
    sensorUnregister(mTask.proxHandle);

    i2cMasterRelease(I2C_BUS_ID);
}

static void handle_event(uint32_t evtType, const void* evtData)
{
    struct AlsProxTransfer *xfer;

    switch (evtType) {
    case EVT_APP_START:
        i2cMasterRequest(I2C_BUS_ID, I2C_SPEED);

        // Read the ID
        xfer = allocXfer(SENSOR_STATE_VERIFY_ID);
        if (xfer != NULL) {
            xfer->txrxBuf[0] = AMS_TMD3725_REG_REVID;
            i2cMasterTxRx(I2C_BUS_ID, I2C_ADDR, xfer->txrxBuf, 1, xfer->txrxBuf, 2, i2cCallback, xfer);
        }
        break;

    case EVT_SENSOR_I2C:
        // Dropping const here (we own this memory)
        handle_i2c_event((struct AlsProxTransfer *) evtData);
        break;

    case EVT_SENSOR_ALS_INTERRUPT:
        disableInterrupt(mTask.pin, &mTask.isr);
        gpioClearPending(PROX_INT_PIN);
        // NOTE: fall-through to initiate read of ALS data registers

    case EVT_SENSOR_ALS_TIMER:
        xfer = allocXfer(SENSOR_STATE_ALS_SAMPLING);
        if (xfer != NULL) {
            xfer->txrxBuf[0] = AMS_TMD3725_REG_CDATAL;
            i2cMasterTxRx(I2C_BUS_ID, I2C_ADDR, xfer->txrxBuf, 1, xfer->txrxBuf, 8, i2cCallback, xfer);
        }
        break;

    case EVT_SENSOR_PROX_TIMER:
        xfer = allocXfer(SENSOR_STATE_PROX_SAMPLING);
        if (xfer != NULL) {
            xfer->txrxBuf[0] = AMS_TMD3725_REG_PDATA;
            i2cMasterTxRx(I2C_BUS_ID, I2C_ADDR, xfer->txrxBuf, 1, xfer->txrxBuf, 2, i2cCallback, xfer);
        }
        break;

    case EVT_SENSOR_PROX_INTERRUPT:
        xfer = allocXfer(SENSOR_STATE_PROX_SAMPLING);
        if (xfer != NULL) {
            if (mTask.proxCalibrating) {
                /*
                xfer->txrxBuf[0] = AMS_TMD3725_REG_OFFSETNL;
                xfer->state = SENSOR_STATE_FINISH_PROX_CALIBRATION_0;
                i2cMasterTxRx(I2C_BUS_ID, I2C_ADDR, xfer->txrxBuf, 1, xfer->txrxBuf, 8, i2cCallback, xfer);
                */
            } else {
                xfer->txrxBuf[0] = AMS_TMD3725_REG_PDATA;
                i2cMasterTxRx(I2C_BUS_ID, I2C_ADDR, xfer->txrxBuf, 1, xfer->txrxBuf, 1, i2cCallback, xfer);
            }
        }
        break;

    }
}

INTERNAL_APP_INIT(AMS_TMD3725_APP_ID, AMS_TMD3725_APP_VERSION, init_app, end_app, handle_event);
