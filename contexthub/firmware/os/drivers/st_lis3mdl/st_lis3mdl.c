/*----------------------------------------------------------------------------
 *      Sensor - LIS3MDL magnetometer
 *----------------------------------------------------------------------------
 *      Name:    st_lis3mdl.c
 *      Purpose: To implement Sensor Driver of LIS3MDL Magnetometer
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

#include <gpio.h>
#include <float.h>
#include <eventnums.h>
#include <hostIntf.h>
#include <isr.h>
#include <slab.h>
#include <plat/gpio/gpio.h>
#include <sensors.h>
#include <i2c.h>
#include <rtc.h>
#include <timer.h>
#include <nanohubPacket.h>
#include <variant/sensType.h>
#include <variant/variant.h>
#include <platform.h>
#include <csp_common.h>

#define REG_LIS3MDL_WHO_AM_I        (0x0F)
#define REG_LIS3MDL_CTRL_REG1       (0x20)
#define REG_LIS3MDL_CTRL_REG2       (0x21)
#define REG_LIS3MDL_CTRL_REG3       (0x22)
#define REG_LIS3MDL_CTRL_REG4       (0x23)
#define REG_LIS3MDL_CTRL_REG5       (0x24)
#define REG_LIS3MDL_STATUS_REG      (0x27)
#define REG_LIS3MDL_OUT_X_L         (0x28)
#define REG_LIS3MDL_OUT_X_H         (0x29)
#define REG_LIS3MDL_OUT_Y_L         (0x2A)
#define REG_LIS3MDL_OUT_Y_H         (0x2B)
#define REG_LIS3MDL_OUT_Z_L         (0x2C)
#define REG_LIS3MDL_OUT_Z_H         (0x2D)
#define REG_LIS3MDL_TMEP_OUT_L      (0x2E)
#define REG_LIS3MDL_TEMP_OUT_H      (0x2F)
#define REG_LIS3MDL_INT_CFG         (0x30)
#define REG_LIS3MDL_INT_SRC         (0x31)
#define REG_LIS3MDL_INT_THS_L       (0x32)
#define REG_LIS3MDL_INT_THS_H       (0x33)

// WHO_AM_I (0xF)
#define LIS3MDL_ID                  (0x3D)
// CTRL_REG1 (20h)
#define LIS3MDL_SELF_TEST_SHIFT     (0x0)

#define LIS3MDL_FAST_ODR_SHIFT      (0x1)

#define LIS3MDL_OPMODE_SHIFT        (0x5)
#define LIS3MDL_OPMODE_MASK         (0x3)
#define LIS3MDL_OPMODE_LP           (0x0)
#define LIS3MDL_OPMODE_MP           (0x1)
#define LIS3MDL_OPMODE_HP           (0x2)
#define LIS3MDL_OPMODE_UHP          (0x3)

#define LIS3MDL_DATA_RATE_SHIFT     (0x2)
#define LIS3MDL_DATA_RATE_MASK      (0x7)
#define LIS3MDL_DATA_RATE_0p625     (0x0)
#define LIS3MDL_DATA_RATE_1p25      (0x1)
#define LIS3MDL_DATA_RATE_2p5       (0x2)
#define LIS3MDL_DATA_RATE_5         (0x3)
#define LIS3MDL_DATA_RATE_10        (0x4)
#define LIS3MDL_DATA_RATE_20        (0x5)
#define LIS3MDL_DATA_RATE_40        (0x6)
#define LIS3MDL_DATA_RATE_80        (0x7)

#define LIS3MDL_FAST_ODR_SHIFT      (0x1)

// CTRL_REG2 (21h)
#define LIS3MDL_SOFT_RST_SHIFT      (0x2)
#define LIS3MDL_REBOOT_SHIFT        (0x3)

#define LIS3MDL_FULL_SCALE_SHIFT    (0x5)
#define LIS3MDL_FULL_SCALE_MASK     (0x3)
#define LIS3MDL_FULL_SCALE_4        (0x0)
#define LIS3MDL_FULL_SCALE_8        (0x1)
#define LIS3MDL_FULL_SCALE_12       (0x2)
#define LIS3MDL_FULL_SCALE_16       (0x3)

// CTRL_REG3
#define LIS3MDL_MD_SHIFT            (0x0)

#define LIS3MDL_MD_CONTINUOUS       (0x0)
#define LIS3MDL_MD_SINGLE_CONV      (0x1)
#define LIS3MDL_MD_POWER_DOWN       (0x2)

#define LIS3MDL_SIM_SHIFT           (0x2)
#define LIS3MDL_LP_SHIFT            (0x5)

// CTRL_REG4
#define LIS3MDL_BLE_SHIFT           (0x1)
#define LIS3MDL_OMZ_SHIFT           (0x2)

//Z-axis operation mode
#define LIS3MDL_OMZ_LP              (0x0)
#define LIS3MDL_OMZ_MP              (0x1)
#define LIS3MDL_OMZ_HP              (0x2)
#define LIS3MDL_OMZ_UHP             (0x3)

// CTRL_REG4
#define LIS3MDL_FAST_READ_SHIFT     (0x7)
#define LIS3MDL_BDU_SHIFT           (0x6)

// STATUS_REG
#define LIS3MDL_STATUS_ZYXOR        (0x80)
#define LIS3MDL_STATUS_ZOR          (0x40)
#define LIS3MDL_STATUS_YOR          (0x20)
#define LIS3MDL_STATUS_XOR          (0x10)
#define LIS3MDL_STATUS_ZYXDA        (0x08)
#define LIS3MDL_STATUS_ZDA          (0x04)
#define LIS3MDL_STATUS_YDA          (0x02)
#define LIS3MDL_STATUS_XDA          (0x01)

// INT_CFT
#define LIS3MDL_INT_XIEN            (0x80) // X-axis Interrupt Enable
#define LIS3MDL_INT_YIEN            (0x40) // Y-axis Interrupt Enable
#define LIS3MDL_INT_ZIEN            (0x20) // Z-axis Interrupt Enable
#define LIS3MDL_INT_IEA             (0x04) // Interrupt Active Configuration : 0-Low, 1-High
#define LIS3MDL_INT_LIR             (0x02) // Latch Interrupt Request
#define LIS3MDL_INT_IEN             (0x01) // Interrupt Enable

// INT_SRC
#define LIS3MDL_INT_SRC_PTH_X       (0x80)
#define LIS3MDL_INT_SRC_PTH_Y       (0x40)
#define LIS3MDL_INT_SRC_PTH_Z       (0x20)
#define LIS3MDL_INT_SRC_NTH_X       (0x10)
#define LIS3MDL_INT_SRC_NTH_Y       (0x08)
#define LIS3MDL_INT_SRC_NTH_Z       (0x04)
#define LIS3MDL_INT_SRC_MROI        (0x02)
#define LIS3MDL_INT_SRC_INT         (0x01)

#define LIS3MDL_SLAVE_ADDR_SA0      (0x1C)
#define LIS3MDL_SLAVE_ADDR_SA1      (0x1E)

#define I2C_ADDR                    (LIS3MDL_SLAVE_ADDR_SA1)
#define LIS3MDL_ID                  (0x3D)

#define LSI3MDL_FAST_ODR            (0)

#define I2C_BUS_ID                  MAG_I2C_BUS_ID
#define I2C_SPEED                   400000


#define I2C_MULTIBYTE_FLAG          0x80

#define LIS3MDL_DEFAULT_DELAY       200000000ULL
#define LIS3MDL_DEFAULT_RATE        LIS3MDL_DATA_RATE_5

#define MAX_NUM_COMMS_EVENT_SAMPLES     4

static bool  enable_debug = false;

#define INFO_PRINT(fmt, ...) do { \
        if (enable_debug) \
            osLog(LOG_INFO, "[LIS3MDL] " fmt, ##__VA_ARGS__); \
    } while (0);

#define DEBUG_PRINT(fmt, ...) do { \
        if (enable_debug) \
            osLog(LOG_DEBUG, "[LIS3MDL] " fmt, ##__VA_ARGS__); \
    } while (0);

#define MIN_DELTA_TIME 1000000

struct LIS3MDLTask{
    uint32_t tid;
    uint32_t magHandle;
    uint32_t timerHandle;

    void (*isr)(uint32_t);

    struct TripleAxisDataEvent* tAxisData;
    struct SlabAllocator *dataSlab3Axis;

    struct Gpio *gpio;
    bool magOn;
    uint64_t lastTime;

    int16_t lastX;
    int16_t lastY;
    int16_t lastZ;

    uint8_t dataRate;
    uint8_t opMode;
    uint8_t fast_odr;
    uint8_t sysMode;
    uint8_t fullScale;


    uint8_t txrxBuf[16];
};

static uint32_t mDataRates[]={
    SENSOR_HZ(0.625f),
    SENSOR_HZ(1.25f),
    SENSOR_HZ(2.5f),
    SENSOR_HZ(5.0f),
    SENSOR_HZ(10.0f),
    SENSOR_HZ(15.0f), // added for cts
    SENSOR_HZ(20.0f),
    SENSOR_HZ(25.0f), // added for cts
    SENSOR_HZ(40.0f),
    SENSOR_HZ(50.0f), // added for cts
    SENSOR_HZ(80.0f),
    SENSOR_HZ(100.0f), // added for cts
    SENSOR_HZ(200.0f), // added for cts
    0
};

static uint64_t mRateTimerVals[]={
    1600000000ULL,
     800000000ULL,
     400000000ULL,
     200000000ULL,
     100000000ULL,
      66666667ULL, // added for cts
      50000000ULL,
      40000000ULL, // added for cts
      25000000ULL,
      20000000ULL, // added for cts
      12500000ULL,
      10000000ULL, // added for cts
       5000000ULL, // added for cts
};

enum LIS3MDLSensorEvents
{
    EVT_SENSOR_I2C = EVT_APP_START + 1,
    EVT_SENSOR_INTERRUPT,
    EVT_SENSOR_TIMER,
    EVT_TIME_SYNC
};

enum SensorState{
    SENSOR_STATE_RESET,
    SENSOR_STATE_VERIFY_ID,
    SENSOR_STATE_INITIALIZING,
    SENSOR_STATE_FINISH_INIT,
    SENSOR_STATE_ENABLING,
    SENSOR_STATE_DISABLING,
    SENSOR_STATE_CONFIG_CHANGING,
    SENSOR_STATE_MAG_SAMPLING,
    SENSOR_STATE_IDLE,
};

static struct LIS3MDLTask mTask;

static void lis3mdl_Isr(uint32_t data)
{
    osEnqueuePrivateEvt(EVT_SENSOR_INTERRUPT, NULL, NULL, mTask.tid);

    gpioClearPending(MAG_INT_PIN);
    return;
}

static void lis3mdl_timerCallback(uint32_t timerId, void *cookie)
{
    DEBUG_PRINT("timerCallback\n");
    osEnqueuePrivateEvt(EVT_SENSOR_TIMER, cookie, NULL, mTask.tid);
}

static bool lis3mdl_sensorPower(bool on, void* cookie)
{
    DEBUG_PRINT("%s: %d\n", __func__, on);

    if( on && !mTask.timerHandle){
        mTask.timerHandle = timTimerSet(LIS3MDL_DEFAULT_DELAY, 0, 50, lis3mdl_timerCallback, NULL, false);
        gpioSetExtInterrupt( MAG_INT_PIN, Falling_Edge, eDisFLT, 0, lis3mdl_Isr );
    }else if(!on && mTask.timerHandle){
        timTimerCancel(mTask.timerHandle);
        mTask.timerHandle = 0;
        gpioUnsetExtInterrupt( MAG_INT_PIN );
    }

    sensorSignalInternalEvt(mTask.magHandle, SENSOR_INTERNAL_EVT_POWER_STATE_CHG, on, 0);

    mTask.magOn = on;
    return true;
}

static bool lis3mdl_sensorRate(uint32_t rate, uint64_t latency, void *cookie)
{
    uint32_t i;
    uint32_t rateTblSize = sizeof(mDataRates)/sizeof(uint32_t);
    uint64_t timeVal;

    if(mTask.timerHandle)
        timTimerCancel(mTask.timerHandle);

    for( i = 0 ; i < rateTblSize ;i++) {
        if( rate == mDataRates[i])
            break;
    }

    if( i == rateTblSize ){
        i = LIS3MDL_DEFAULT_RATE;
    }

    timeVal =  mRateTimerVals[i];

    mTask.txrxBuf[0] = REG_LIS3MDL_CTRL_REG1;
    mTask.txrxBuf[1] = (mTask.opMode << LIS3MDL_OPMODE_SHIFT) | (i << LIS3MDL_DATA_RATE_SHIFT) |
                        (mTask.fast_odr << LIS3MDL_FAST_ODR_SHIFT);
    i2cMasterTx(I2C_BUS_ID, I2C_ADDR, mTask.txrxBuf, 2, NULL, NULL);

    mTask.dataRate = i;
    mTask.timerHandle = timTimerSet( timeVal, 0, 50, lis3mdl_timerCallback, NULL, false);

    return sensorSignalInternalEvt(mTask.magHandle, SENSOR_INTERNAL_EVT_RATE_CHG, rate, latency);
}

static bool lis3mdl_sensorFlush(void *cookie)
{
    return osEnqueueEvt(sensorGetMyEventType(SENS_TYPE_MAG),SENSOR_DATA_EVENT_FLUSH, NULL);
}

static bool lis3mdl_sensorCfg(void *data, void *cookie)
{
    float *values = data;

    INFO_PRINT("%s: %ld, %ld, %ld\n", __func__,
            (int32_t)(values[0]*1000), (int32_t)(values[1]*1000), (int32_t)(values[2]*1000));

    return true;
}

static bool lis3mdl_sensorFirmware(void *cookie)
{
    return sensorSignalInternalEvt(mTask.magHandle, SENSOR_INTERNAL_EVT_FW_STATE_CHG, 1, 0);
}

static void lis3mdl_i2cCallback(void *cookie, size_t tx, size_t rx, int err)
{
    if( err == 0 )
        osEnqueuePrivateEvt(EVT_SENSOR_I2C, cookie, NULL, mTask.tid);
    else
        INFO_PRINT("i2c error (%d)\n", err);
}

static void lis3mdl_endApp(void)
{
    gpioUnsetExtInterrupt( MAG_INT_PIN);
    gpioClearPending( MAG_INT_PIN );

    slabAllocatorFree(mTask.dataSlab3Axis, (struct TripleAxisDataEvent *)mTask.tAxisData);
    slabAllocatorDestroy(mTask.dataSlab3Axis);

    sensorUnregister( mTask.magHandle );

    i2cMasterRelease(I2C_BUS_ID);
}

static const struct SensorInfo mSensorInfoMagUncal =
{
    .sensorName     = "Magnetometer",
    .supportedRates = mDataRates,
    .sensorType     = SENS_TYPE_MAG,
    .numAxis        = NUM_AXIS_THREE,
    .interrupt      = NANOHUB_INT_NONWAKEUP,
    .minSamples     = 600,
    .biasType       = SENS_TYPE_MAG_BIAS
};

static const struct SensorOps mSensorOpsMagUncal =
{
    .sensorPower            = lis3mdl_sensorPower,
    .sensorFirmwareUpload   = lis3mdl_sensorFirmware,
    .sensorSetRate          = lis3mdl_sensorRate,
    .sensorFlush            = lis3mdl_sensorFlush,
    .sensorTriggerOndemand  = NULL,
    .sensorCalibrate        = NULL,
    .sensorCfgData          = lis3mdl_sensorCfg
};

static bool lis3mdl_initApp(uint32_t taskId)
{
    uint32_t slabSize;


    mTask.tid = taskId;
#ifdef EXYNOS_CONTEXTHUB
    CSP_PRINTF_INFO("LIS3MDL Task Starting\n");
    mTask.gpio = gpioRequest(MAG_GPIO_PIN);
#else
    mTask.gpio = (struct Gpio *)MAG_INT_PIN;
#endif
    //mTask.drdy = gpioRequest(MAG_DIRY_PIN);

    mTask.isr = lis3mdl_Isr;
    mTask.magOn = false;
    mTask.fast_odr = 0;
    mTask.opMode = LIS3MDL_OPMODE_MP;
    mTask.dataRate = LIS3MDL_DEFAULT_RATE;
    mTask.lastTime = 0;

    mTask.magHandle = sensorRegister(&mSensorInfoMagUncal, &mSensorOpsMagUncal, NULL, false);

    slabSize = sizeof(struct TripleAxisDataEvent) +
        MAX_NUM_COMMS_EVENT_SAMPLES * sizeof(struct TripleAxisDataPoint);
    mTask.dataSlab3Axis = slabAllocatorNew(slabSize, 4, 1);
    mTask.tAxisData = slabAllocatorAlloc(mTask.dataSlab3Axis);

    osEventSubscribe(taskId, EVT_APP_START);

    return true;
}

static bool lis3mdl_pushRawData(void)
{
    uint64_t rtcTime;

    rtcTime = rtcGetTime();

    if(mTask.lastTime == 0){
        mTask.tAxisData->samples[0].deltaTime = 0;
    }else{
        if( (rtcTime - mTask.lastTime) < MIN_DELTA_TIME ) return false;
        mTask.tAxisData->samples[0].deltaTime = rtcTime - mTask.lastTime;
    }

    mTask.lastTime = rtcTime;

    mTask.tAxisData->referenceTime = rtcTime;
    mTask.tAxisData->samples[0].firstSample.numSamples = 1;
    mTask.tAxisData->samples[0].x = *(int16_t*)(&mTask.txrxBuf[0]);
    mTask.tAxisData->samples[0].y = *(int16_t*)(&mTask.txrxBuf[2]);
    mTask.tAxisData->samples[0].z = *(int16_t*)(&mTask.txrxBuf[4]);

    return osEnqueueEvt(sensorGetMyEventType(SENS_TYPE_MAG), mTask.tAxisData, NULL);
}

static void lis3mdl_handleI2cEvent(int state)
{
    DEBUG_PRINT("%s : state = %d\n", __func__, state);

    switch(state)
    {
        case SENSOR_STATE_RESET:
            DEBUG_PRINT("read ID\n");
            mTask.txrxBuf[0] = REG_LIS3MDL_WHO_AM_I;
            i2cMasterTxRx( I2C_BUS_ID, I2C_ADDR, mTask.txrxBuf, 1, mTask.txrxBuf, 1,
                            &lis3mdl_i2cCallback, (void *)SENSOR_STATE_VERIFY_ID);
            break;

        case SENSOR_STATE_VERIFY_ID:
            DEBUG_PRINT("verify id : %d\n", mTask.txrxBuf[0]);
            if ( mTask.txrxBuf[0] != LIS3MDL_ID ){
                INFO_PRINT("not detected : %x\n", mTask.txrxBuf[0]);
                sensorUnregister(mTask.magHandle);
                break;
            }

            mTask.txrxBuf[0] = I2C_MULTIBYTE_FLAG | REG_LIS3MDL_CTRL_REG1;
            mTask.txrxBuf[1] = ( LIS3MDL_OPMODE_MP << LIS3MDL_OPMODE_SHIFT) |
                                (LIS3MDL_DATA_RATE_10 << LIS3MDL_DATA_RATE_SHIFT);
            mTask.txrxBuf[2] = (LIS3MDL_FULL_SCALE_4 << LIS3MDL_FULL_SCALE_SHIFT );
            mTask.txrxBuf[3] = (LIS3MDL_MD_CONTINUOUS << LIS3MDL_MD_SHIFT);
            mTask.txrxBuf[4] = (LIS3MDL_OMZ_MP << LIS3MDL_OMZ_SHIFT);
            i2cMasterTx(I2C_BUS_ID, I2C_ADDR, mTask.txrxBuf, 5, &lis3mdl_i2cCallback, (void *)SENSOR_STATE_INITIALIZING);
            break;

        case SENSOR_STATE_INITIALIZING:
            DEBUG_PRINT("initializing\n");
            mTask.txrxBuf[0] = REG_LIS3MDL_INT_CFG;
            mTask.txrxBuf[1] = LIS3MDL_INT_XIEN | LIS3MDL_INT_YIEN | LIS3MDL_INT_ZIEN |
                                LIS3MDL_INT_LIR;  // Interrupt Disable
            //                    LIS3MDL_INT_LIR | LIS3MDL_INT_IEN;
            i2cMasterTx(I2C_BUS_ID, I2C_ADDR, mTask.txrxBuf, 2, &lis3mdl_i2cCallback, (void *)SENSOR_STATE_FINISH_INIT);
            break;

        case SENSOR_STATE_FINISH_INIT:
            DEBUG_PRINT("initialize done\n");
            ipc_set_sensor_info(SENSOR_TYPE_MAGNETIC_FIELD, NULL, NULL, SENS_TYPE_MAG, LIS3MDL_ID);
            sensorRegisterInitComplete(mTask.magHandle);
            break;

        case SENSOR_STATE_ENABLING:
            DEBUG_PRINT("enabling\n");
            sensorSignalInternalEvt(mTask.magHandle, SENSOR_INTERNAL_EVT_POWER_STATE_CHG, true, 0);
            break;

        case SENSOR_STATE_DISABLING:
            DEBUG_PRINT("disabling\n");
            sensorSignalInternalEvt(mTask.magHandle, SENSOR_INTERNAL_EVT_POWER_STATE_CHG, false, 0);
            break;

        case SENSOR_STATE_MAG_SAMPLING:
            lis3mdl_pushRawData();
            break;
    }
}

static void lis3mdl_handleEvent(uint32_t evtType, const void* evtData)
{
    switch (evtType) {
    case EVT_APP_START:
        i2cMasterRequest(I2C_BUS_ID, I2C_SPEED);

        DEBUG_PRINT("reset\n");
        /* Reset Device */
        mTask.txrxBuf[0] = REG_LIS3MDL_CTRL_REG2;
        mTask.txrxBuf[1] = (0x1 << LIS3MDL_SOFT_RST_SHIFT);
        i2cMasterTx(I2C_BUS_ID, I2C_ADDR, mTask.txrxBuf, 2, NULL, NULL);

        DEBUG_PRINT("reset release\n");
        /* Reset Release */
        mTask.txrxBuf[0] = REG_LIS3MDL_CTRL_REG2;
        mTask.txrxBuf[1] = (0x0 << LIS3MDL_SOFT_RST_SHIFT);
        i2cMasterTx(I2C_BUS_ID, I2C_ADDR, mTask.txrxBuf, 2, &lis3mdl_i2cCallback, (void *)SENSOR_STATE_RESET);
        break;

    case EVT_SENSOR_I2C:
        lis3mdl_handleI2cEvent((int)evtData);
        break;

    case EVT_SENSOR_INTERRUPT:
        mTask.txrxBuf[0] = I2C_MULTIBYTE_FLAG | REG_LIS3MDL_OUT_X_L;
        i2cMasterTxRx(I2C_BUS_ID, I2C_ADDR, mTask.txrxBuf, 1, mTask.txrxBuf, 6, &lis3mdl_i2cCallback, (void *)SENSOR_STATE_MAG_SAMPLING);
        break;

    case EVT_SENSOR_TIMER:
        DEBUG_PRINT("%s : EVT_SENSOR_TIMER\n", __func__);
        mTask.txrxBuf[0] = I2C_MULTIBYTE_FLAG | REG_LIS3MDL_OUT_X_L;
        i2cMasterTxRx(I2C_BUS_ID, I2C_ADDR, mTask.txrxBuf, 1, mTask.txrxBuf, 6, &lis3mdl_i2cCallback, (void *)SENSOR_STATE_MAG_SAMPLING);
        break;
    }
}

#define LIS3MDL_APP_ID      APP_ID_MAKE(NANOHUB_VENDOR_GOOGLE, 15)
INTERNAL_APP_INIT(LIS3MDL_APP_ID, 1, lis3mdl_initApp, lis3mdl_endApp, lis3mdl_handleEvent);
