/*----------------------------------------------------------------------------
 *      Sensor - AK09918C
 *----------------------------------------------------------------------------
 *      Name:    akm_ak09918.c
 *      Purpose: To implement Sensor Driver of AK09918C
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
#include <algos/time_sync.h>
#include <atomic.h>
#include <common/math/macros.h>
#include <cpu/cpuMath.h>
#include <errno.h>
#include <gpio.h>
#include <heap.h>
#include <halIntf.h>
#include <hostIntf.h>
#include <i2c.h>
#include <isr.h>
#include <nanohub_math.h>
#include <nanohubPacket.h>
#include <printf.h>
#include <plat/syscfg.h>
#include <plat/rtc.h>
#include <sensors.h>
#include <seos.h>
#include <slab.h>
#include <spi.h>
#include <timer.h>
#include <variant/sensType.h>
#include <variant/variant.h>
#include <platform.h>
#include <calibration/magnetometer/mag_cal/mag_cal.h>
#ifdef EXYNOS_CONTEXTHUB
  #include <cmgp.h>
  #include <plat/gpio/gpio.h>
#else
  #include <plat/exti.h>
  #include <plat/gpio.h>
#endif
#include <float.h>
#include <eventnums.h>
#include <hostIntf.h>
#include <isr.h>
#include <slab.h>
#include <sensors.h>
#include <i2c.h>
#include <rtc.h>
#include <atomic.h>
#include <timer.h>
#include <nanohubPacket.h>
#include <variant/sensType.h>
#include <variant/variant.h>
#include <platform.h>
#include <csp_common.h>

#include "AK09918.h"

#define TIME_NS_TO_US(ns)    cpuMathU64DivByU16(ns, 1000)


typedef struct _MAGNETIC_DATA {
	uint16_t raw_data[3];
	uint16_t calib_data[3];
	uint16_t cal_offset[3];

}MAGNETIC_DATA, *pMAGNETIC_DATA;

struct AK09918Task{
    uint32_t tid;
    uint32_t magHandle;
    uint32_t timerHandle;

    void (*isr)(uint32_t);

    struct TripleAxisDataEvent* tAxisData;
    struct SlabAllocator *dataSlab3Axis;

    bool magOn;
    uint64_t lastTime;

#if defined(AK09918_CAL_ENABLED)
    struct MagCal moc;
#endif

    int16_t lastX;
    int16_t lastY;
    int16_t lastZ;

    uint8_t dataRate;
    uint8_t fast_odr;
    uint8_t sysMode;
    uint8_t fullScale;
    uint8_t accurcy;

    uint8_t txrxBuf[16];
};

static uint32_t mDataRates[]={
#if defined(HIFI_SENSOR_SUPPORT)
    SENSOR_HZ(5.0f),
#endif
    SENSOR_HZ(10.0f),
    SENSOR_HZ(20.0f),
    SENSOR_HZ(50.0f),
    SENSOR_HZ(100.0f),
    0,
};

static uint64_t mRateTimerVals[]={
#if defined(HIFI_SENSOR_SUPPORT)
    200000000ULL,
#endif
    100000000ULL,
    50000000ULL,
    20000000ULL,
    10000000ULL,
};

enum AK09918SensorEvents
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

static struct AK09918Task mTask;

static bool  enable_debug = false;
#define INFO_PRINT(fmt, ...) do { \
        osLog(LOG_INFO, "[AK09918] " fmt, ##__VA_ARGS__); \
    } while (0);

#define DEBUG_PRINT(fmt, ...) do { \
        if (enable_debug) \
            osLog(LOG_DEBUG, "[AK09918] " fmt, ##__VA_ARGS__); \
    } while (0);

#define ERROR_PRINT(fmt, ...) do { \
        osLog(LOG_ERROR, "[AK09918] " fmt, ##__VA_ARGS__); \
    } while (0);

static void ak09918_dumpRegister(void)
{
    int ret;

    DEBUG_PRINT(" -----  %s -----\n", __func__);
    mTask.txrxBuf[0] = AK09918_REG_WIA1;
    ret = i2cMasterTxRx( I2C_BUS_ID, I2C_ADDR, mTask.txrxBuf, 1, mTask.txrxBuf, 2, NULL, NULL);
    if(ret)
        ERROR_PRINT("i2cMasterTxRx error @ %d\n", __LINE__);

    DEBUG_PRINT("REG_WIA1(0x%x: 0x%x) \n",AK09918_REG_WIA1, mTask.txrxBuf[0]);
    DEBUG_PRINT("REG_WIA2(0x%x: 0x%x) \n",AK09918_REG_WIA2, mTask.txrxBuf[1]);

    mTask.txrxBuf[0] = AK09918_REG_ST1;
    ret = i2cMasterTxRx( I2C_BUS_ID, I2C_ADDR, mTask.txrxBuf, 1, mTask.txrxBuf, 1, NULL, NULL);
    if(ret)
        ERROR_PRINT("i2cMasterTxRx error @ %d\n", __LINE__);

    DEBUG_PRINT("REG_ST1(0x%x: 0x%x) \n",AK09918_REG_ST1, mTask.txrxBuf[0]);

    mTask.txrxBuf[0] = AK09918_REG_HXL;
    ret = i2cMasterTxRx( I2C_BUS_ID, I2C_ADDR, mTask.txrxBuf, 1, mTask.txrxBuf, 6, NULL, NULL);
    if(ret)
        ERROR_PRINT("i2cMasterTxRx error @ %d\n", __LINE__);

    mTask.tAxisData->samples[0].x = *(int16_t*)(&mTask.txrxBuf[0]);
    mTask.tAxisData->samples[0].y = *(int16_t*)(&mTask.txrxBuf[2]);
    mTask.tAxisData->samples[0].z = *(int16_t*)(&mTask.txrxBuf[4]);
    DEBUG_PRINT("REG_HXL ~ REG_HZH\nx(%f), y(%f), z(%f)\n",
            (double)mTask.tAxisData->samples[0].x,
            (double)mTask.tAxisData->samples[0].y,
            (double)mTask.tAxisData->samples[0].z);

    mTask.txrxBuf[0] = AK09918_REG_ST2;
    ret = i2cMasterTxRx( I2C_BUS_ID, I2C_ADDR, mTask.txrxBuf, 1, mTask.txrxBuf, 1, NULL, NULL);
    if(ret)
        ERROR_PRINT("i2cMasterTxRx error @ %d\n", __LINE__);

    DEBUG_PRINT("REG_ST2(0x%x: 0x%x) \n",AK09918_REG_ST2, mTask.txrxBuf[0]);
    mTask.txrxBuf[0] = AK09918_REG_CNTL2;
    ret = i2cMasterTxRx( I2C_BUS_ID, I2C_ADDR, mTask.txrxBuf, 1, mTask.txrxBuf, 2, NULL, NULL);
    if(ret)
        ERROR_PRINT("i2cMasterTxRx error @ %d\n", __LINE__);

    DEBUG_PRINT("REG_CNTL2(0x%x: 0x%x) \n",AK09918_REG_CNTL2, mTask.txrxBuf[0]);
    DEBUG_PRINT("REG_CNTL3(0x%x: 0x%x) \n",AK09918_REG_CNTL3, mTask.txrxBuf[0]);

    DEBUG_PRINT(" ------------------\n");
}

static void ak09918_timerCallback(uint32_t timerId, void *cookie)
{
    DEBUG_PRINT("timerCallback\n");
    osEnqueuePrivateEvt(EVT_SENSOR_TIMER, cookie, NULL, mTask.tid);
}

static bool ak09918_sensorPower(bool on, void* cookie)
{
    int ret;

    INFO_PRINT("%s: %d\n", __func__, on);

    if( on && !mTask.timerHandle){
        mTask.txrxBuf[0] = AKM_REG_MODE;
        mTask.txrxBuf[1] = AKM_MODE_CONT_MEASURE1;
        ret = i2cMasterTx(I2C_BUS_ID, I2C_ADDR, mTask.txrxBuf, 2, NULL, NULL);
        if(ret)
            ERROR_PRINT("%d i2cMasterTx err\n",__LINE__);

        mTask.timerHandle = timTimerSet(mRateTimerVals[0]/*AK09918_DEFAULT_DELAY*/, 0, 50, ak09918_timerCallback, NULL, false);

    }else if(!on && mTask.timerHandle){
        mTask.txrxBuf[0] = AKM_REG_MODE;
        mTask.txrxBuf[1] = AKM_MODE_POWERDOWN;
        ret = i2cMasterTx(I2C_BUS_ID, I2C_ADDR, mTask.txrxBuf, 2, NULL, NULL);
        if(ret)
            ERROR_PRINT("%d i2cMasterTx err\n",__LINE__);


        timTimerCancel(mTask.timerHandle);
        mTask.timerHandle = 0;
    }

    sensorSignalInternalEvt(mTask.magHandle, SENSOR_INTERNAL_EVT_POWER_STATE_CHG, on, 0);

    mTask.magOn = on;
    return true;
}

static bool ak09918_sensorRate(uint32_t rate, uint64_t latency, void *cookie)
{
    uint32_t i;
    uint32_t rateTblSize = sizeof(mDataRates)/sizeof(uint32_t);
    uint64_t timeVal;
    uint8_t mode;
    int ret;

    INFO_PRINT("%s(rate: %ld) \n", __func__, rate);
    if(mTask.timerHandle)
        timTimerCancel(mTask.timerHandle);

    if((rate < SENSOR_HZ(10))||(rate == SENSOR_HZ(10))){
	    mode = AKM_MODE_CONT_MEASURE1;
    }
    else if((rate < SENSOR_HZ(20))||(rate == SENSOR_HZ(20))){
	    mode = AKM_MODE_CONT_MEASURE2;
    }
    else if((rate < SENSOR_HZ(50))||(rate == SENSOR_HZ(50))){
	    mode = AKM_MODE_CONT_MEASURE3;
    }
    else{
	    mode = AKM_MODE_CONT_MEASURE4;
    }

    for( i = 0 ; i < rateTblSize ;i++) {
        if( rate == mDataRates[i])
            break;
    }

    if( i == rateTblSize ){
        ERROR_PRINT("%s: No matched rate(%ld) \n",__func__, rate);
	    i = AK09918_DEFAULT_RATE;
	    mode = AKM_MODE_CONT_MEASURE1;
    }

    INFO_PRINT("%s: Matched rate(index: %ld) is %ld \n",__func__, i, mDataRates[i]);
    timeVal =  mRateTimerVals[i];
    mTask.txrxBuf[0] = AKM_REG_MODE;
    mTask.txrxBuf[1] = mode;

    ret = i2cMasterTx(I2C_BUS_ID, I2C_ADDR, mTask.txrxBuf, 2, NULL, NULL);
    if(ret)
        ERROR_PRINT("%d i2cMasterTx err\n",__LINE__);


    mTask.dataRate = i;
    mTask.timerHandle = timTimerSet( timeVal, 0, 50, ak09918_timerCallback, NULL, false);
    return sensorSignalInternalEvt(mTask.magHandle, SENSOR_INTERNAL_EVT_RATE_CHG, rate, latency);
}

static bool ak09918_sensorFlush(void *cookie)
{
    return osEnqueueEvt(sensorGetMyEventType(SENS_TYPE_MAG),SENSOR_DATA_EVENT_FLUSH, NULL);
}

static bool ak09918_sensorCfg(void *data, void *cookie)
{
    float *values = data;

    INFO_PRINT("%s: %ld, %ld, %ld\n", __func__,
            (int32_t)(values[0]*1000), (int32_t)(values[1]*1000), (int32_t)(values[2]*1000));

    return true;
}

static bool ak09918_sensorFirmware(void *cookie)
{
    return sensorSignalInternalEvt(mTask.magHandle, SENSOR_INTERNAL_EVT_FW_STATE_CHG, 1, 0);
}

static void ak09918_i2cCallback(void *cookie, size_t tx, size_t rx, int err)
{
    if( err == 0 )
        osEnqueuePrivateEvt(EVT_SENSOR_I2C, cookie, NULL, mTask.tid);
    else
        ERROR_PRINT("i2c error (%d)\n", err);
}

static void ak09918_endApp(void)
{
#if defined(AK09918_CAL_ENABLED)
    magCalDestroy(&mTask.moc);
#endif
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
    .sensorPower            = ak09918_sensorPower,
    .sensorFirmwareUpload   = ak09918_sensorFirmware,
    .sensorSetRate          = ak09918_sensorRate,
    .sensorFlush            = ak09918_sensorFlush,
    .sensorTriggerOndemand  = NULL,
    .sensorCalibrate        = NULL,
    .sensorCfgData          = ak09918_sensorCfg
};

static bool ak09918_initApp(uint32_t taskId)
{
    uint32_t slabSize;
#if defined(AK09918_CAL_Enabled)
    struct MagCalParameters mag_cal_parameters;
    struct DiversityCheckerParameters diverse_parameters;
#endif

    mTask.tid = taskId;
    INFO_PRINT("##### %s !!!!!!\n",__FUNCTION__);

    mTask.magOn = false;
    mTask.fast_odr = 0;
    mTask.dataRate = AK09918_DEFAULT_RATE;
    mTask.lastTime = 0;

    mTask.magHandle = sensorRegister(&mSensorInfoMagUncal, &mSensorOpsMagUncal, NULL, false);

    slabSize = sizeof(struct TripleAxisDataEvent) +
        MAX_NUM_COMMS_EVENT_SAMPLES * sizeof(struct TripleAxisDataPoint);
    mTask.dataSlab3Axis = slabAllocatorNew(slabSize, 4, 1);
    mTask.tAxisData = slabAllocatorAlloc(mTask.dataSlab3Axis);

#if defined(AK09918_CAL_Enabled)
    mag_cal_parameters.min_batch_window_in_micres = 3000000;
    mag_cal_parameters.x_bias = 0.0f;
    mag_cal_parameters.y_bias = 0.0f;
    mag_cal_parameters.z_bias = 0.0f;
    mag_cal_parameters.c00 = 1.0f;
    mag_cal_parameters.c01 = 0.0f;
    mag_cal_parameters.c02 = 0.0f;
    mag_cal_parameters.c10 = 0.0f;
    mag_cal_parameters.c11 = 1.0f;
    mag_cal_parameters.c12 = 0.0f;
    mag_cal_parameters.c20 = 0.0f;
    mag_cal_parameters.c21 = 0.0f;
    mag_cal_parameters.c22 = 1.0f;
    diverse_parameters.var_threshold = 6.0f;
    diverse_parameters.max_min_threshold = 10.0f;
    diverse_parameters.local_field = 48.0f;
    diverse_parameters.threshold_tuning_param = 0.5f;
    diverse_parameters.max_distance_tuning_param = 2.552f;
    diverse_parameters.min_num_diverse_vectors = 8;
    diverse_parameters.max_num_max_distance = 1;
    initMagCal(&mTask.moc, &mag_cal_parameters, &diverse_parameters);
#endif
    osEventSubscribe(taskId, EVT_APP_START);
#ifdef EXYNOS_CONTEXTHUB
	ipc_set_sensor_info(SENSOR_TYPE_MAGNETIC_FIELD, "AK09918 Mag", VENDOR_NAME_AK, 0, 0);
	ipc_set_sensor_info(SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED, "AK09918 Mag(uncal)", VENDOR_NAME_AK, 0, 0);
#endif

    return true;
}

static bool ak09918_pushRawData(void)
{
    uint64_t rtcTime;
    uint8_t regSt1;
    uint8_t regSt2;

    regSt1 = mTask.txrxBuf[0];
    regSt2 = mTask.txrxBuf[8];
    DEBUG_PRINT("%s %d: regSt1(0x%x) DOR(%s), DRDY(%s)!!! \n"
            ,__func__, __LINE__, regSt1, (regSt1&0x2)?"true":"false", (regSt1&0x1)?"true":"false");

    if(regSt2 & 0x8){
        ERROR_PRINT("%s %d: regSt2(0x%x) Overflow occurred!!! \n",__func__, __LINE__, regSt2);
    }

    rtcTime = rtcGetTime();

    if(mTask.lastTime == 0){
        mTask.tAxisData->samples[0].deltaTime = 0;
    }else{
        if( (rtcTime - mTask.lastTime) < MIN_DELTA_TIME ) return false;
        mTask.tAxisData->samples[0].deltaTime = rtcTime - mTask.lastTime;
    }

    mTask.lastTime = rtcTime;

    mTask.tAxisData->referenceTime = rtcTime;
    memset(&mTask.tAxisData->samples[0].firstSample, 0x00, sizeof(struct SensorFirstSample));
    mTask.tAxisData->samples[0].firstSample.numSamples = 1;
    //mTask.tAxisData->samples[0].x = *(int16_t*)(&mTask.txrxBuf[1]);
    //mTask.tAxisData->samples[0].y = *(int16_t*)(&mTask.txrxBuf[3]);
    //mTask.tAxisData->samples[0].z = *(int16_t*)(&mTask.txrxBuf[5]);

    mTask.tAxisData->samples[0].x = (int16_t)(mTask.txrxBuf[2] << 8) + (int16_t)(mTask.txrxBuf[1]);
    mTask.tAxisData->samples[0].y = (int16_t)(mTask.txrxBuf[4] << 8) + (int16_t)(mTask.txrxBuf[3]);
    mTask.tAxisData->samples[0].z = (int16_t)(mTask.txrxBuf[6] << 8) + (int16_t)(mTask.txrxBuf[5]);

    // change ak09918 axis to adapt android axis
    mTask.tAxisData->samples[0].x = -mTask.tAxisData->samples[0].x *0.15 ;
    mTask.tAxisData->samples[0].y = -mTask.tAxisData->samples[0].y *0.15 ;
    mTask.tAxisData->samples[0].z = mTask.tAxisData->samples[0].z *0.15 ;

     AKM09918_TO_ANDROID_COORDINATE(mTask.tAxisData->samples[0].x,
		    		mTask.tAxisData->samples[0].y,
				mTask.tAxisData->samples[0].z);

    DEBUG_PRINT("mag_data x, y, z: %f, %f, %f\n",
            (double)mTask.tAxisData->samples[0].x,
            (double)mTask.tAxisData->samples[0].y,
            (double)mTask.tAxisData->samples[0].z);

#if defined(AK09918_CAL_ENABLED)
    float x, y, z;
    bool newMagCalibData;

    magCalRemoveSoftiron(&mTask.moc,
                mTask.tAxisData->samples[0].x,
                mTask.tAxisData->samples[0].y,
                mTask.tAxisData->samples[0].z,
                &x, &y, &z);
    newMagCalibData = magCalUpdate(&mTask.moc, TIME_NS_TO_US(mTask.lastTime), x, y, z);
    magCalRemoveBias(&mTask.moc,
                mTask.tAxisData->samples[0].x,
                mTask.tAxisData->samples[0].y,
                mTask.tAxisData->samples[0].z,
                &x, &y, &z);

    mTask.tAxisData->samples[0].x = x;
    mTask.tAxisData->samples[0].y = y;
    mTask.tAxisData->samples[0].z = z;

    if (newMagCalibData) {
        mTask.tAxisData->samples[1].deltaTime = 0;
        magCalGetBias(&mTask.moc,
                &mTask.tAxisData->samples[1].x,
                &mTask.tAxisData->samples[1].y,
                &mTask.tAxisData->samples[1].z);

        mTask.tAxisData->samples[0].firstSample.numSamples = 2;
        mTask.tAxisData->samples[0].firstSample.biasCurrent = true;
        mTask.tAxisData->samples[0].firstSample.biasPresent = 1;
        mTask.tAxisData->samples[0].firstSample.biasSample = 1;
    }
#endif

    return osEnqueueEvt(sensorGetMyEventType(SENS_TYPE_MAG), mTask.tAxisData, NULL);
}

static void ak09918_handleI2cEvent(int state)
{
    int ret;

    DEBUG_PRINT("%s : state = %d\n", __func__, state);

    switch(state)
    {
        case SENSOR_STATE_RESET:
            INFO_PRINT("read ID\n");
            mTask.txrxBuf[0] = AK09918_REG_WIA1;
            ret = i2cMasterTxRx( I2C_BUS_ID, I2C_ADDR, mTask.txrxBuf, 1, mTask.txrxBuf, 2,
                            &ak09918_i2cCallback, (void *)SENSOR_STATE_VERIFY_ID);
            if(ret)
                ERROR_PRINT("i2cMasterTxRx error @ %d\n", __LINE__);
            break;

        case SENSOR_STATE_VERIFY_ID:
            INFO_PRINT("verify id : %d\n", mTask.txrxBuf[0]);
            if ( mTask.txrxBuf[0] !=  GEOMAGNETIC_WHOAMI_RESPONSE){
                ERROR_PRINT("not detected : %x\n", mTask.txrxBuf[0]);
                sensorUnregister(mTask.magHandle);
                break;
            }
            //mTask.txrxBuf[0] = I2C_MULTIBYTE_FLAG | AKM_REG_RESET;
            mTask.txrxBuf[0] = AKM_REG_RESET;
            mTask.txrxBuf[1] = AKM_MODE_POWERDOWN;
            i2cMasterTx(I2C_BUS_ID, I2C_ADDR, mTask.txrxBuf, 2, &ak09918_i2cCallback, (void *)SENSOR_STATE_FINISH_INIT);
            //i2cMasterTx(I2C_BUS_ID, I2C_ADDR, mTask.txrxBuf, 2, &ak09918_i2cCallback, (void *)SENSOR_STATE_INITIALIZING);
            break;

        case SENSOR_STATE_INITIALIZING:
            //mTask.txrxBuf[0] = I2C_MULTIBYTE_FLAG | AKM_REG_MODE;
            mTask.txrxBuf[0] = AKM_REG_MODE;
            mTask.txrxBuf[1] = AKM_MODE_POWERDOWN;
            i2cMasterTx(I2C_BUS_ID, I2C_ADDR, mTask.txrxBuf, 2, &ak09918_i2cCallback, (void *)SENSOR_STATE_FINISH_INIT);
            break;

        case SENSOR_STATE_FINISH_INIT:
            INFO_PRINT("initialize done\n");
            ipc_set_sensor_info(SENSOR_TYPE_MAGNETIC_FIELD, NULL, NULL, SENS_TYPE_MAG, GEOMAGNETIC_WHOAMI_RESPONSE);
            if(enable_debug)
                ak09918_dumpRegister();
            sensorRegisterInitComplete(mTask.magHandle);
            break;

        case SENSOR_STATE_ENABLING:
            INFO_PRINT("enabling\n");
            sensorSignalInternalEvt(mTask.magHandle, SENSOR_INTERNAL_EVT_POWER_STATE_CHG, true, 0);
            break;

        case SENSOR_STATE_DISABLING:
            INFO_PRINT("disabling\n");
            sensorSignalInternalEvt(mTask.magHandle, SENSOR_INTERNAL_EVT_POWER_STATE_CHG, false, 0);
            break;

        case SENSOR_STATE_MAG_SAMPLING:
            ak09918_pushRawData();
            break;
    }
}

static void ak09918_handleEvent(uint32_t evtType, const void* evtData)
{
    int ret;

    switch (evtType) {
    case EVT_APP_START:
        i2cMasterRequest(I2C_BUS_ID, I2C_SPEED);

        INFO_PRINT("reset\n");
        /* Reset Device */
        mTask.txrxBuf[0] = AKM_REG_RESET;
        mTask.txrxBuf[1] = AKM_RESET_DATA;
        ret = i2cMasterTx(I2C_BUS_ID, I2C_ADDR, mTask.txrxBuf, 2, &ak09918_i2cCallback, (void *)SENSOR_STATE_RESET);
        if(ret)
            ERROR_PRINT("i2cMasterTx error @ %d\n", __LINE__);
        break;

    case EVT_SENSOR_I2C:
        ak09918_handleI2cEvent((int)evtData);
        break;

    case EVT_SENSOR_TIMER:
        DEBUG_PRINT("%s : EVT_SENSOR_TIMER\n", __func__);

	    /* Read Data */
	    mTask.txrxBuf[0] = AKM_REG_STATUS;
        ret = i2cMasterTxRx(I2C_BUS_ID, I2C_ADDR, mTask.txrxBuf, 1, mTask.txrxBuf, 9, &ak09918_i2cCallback, (void *)SENSOR_STATE_MAG_SAMPLING);
        if(ret)
            ERROR_PRINT("i2cMasterTxRx error @ %d\n", __LINE__);
        break;
    }
}

INTERNAL_APP_INIT(AK09918_APP_ID, 1, ak09918_initApp, ak09918_endApp, ak09918_handleEvent);
