/*!
 * @section LICENSE
 *
 * Copyright (C) 2016 The Android Open Source Project
 * Modifications Copyright (C) 2021 Bosch Sensortec GmbH
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
/*! @file	    bosch_bmi26x.c
 *  @date	    01-02-2021
 *  @version	v1.0.0.1
 */


#define CHRE_LATESTONE

#include <algos/time_sync.h>
#include <atomic.h>
#ifdef CHRE_LATESTONE
#include <common/math/macros.h>
#endif
#include <cpu/cpuMath.h>
#include <errno.h>
#include <gpio.h>
#include <heap.h>
#include <halIntf.h>
#include <hostIntf.h>
#include <i2c.h>
#include <i3c/i3c.h>
#include <isr.h>
#include <nanohub_math.h>
#include <nanohubPacket.h>
#include <printf.h>
#ifdef EXYNOS_CONTEXTHUB
    #include <plat/gpio/gpio.h>
#else
    #include <plat/exti.h>
#endif
#include <plat/syscfg.h>
#include <rtc.h>
#include <sensors.h>
#include <seos.h>
#include <slab.h>
#include <spi.h>
#include <timer.h>
#include <variant/sensType.h>
#include <variant/variant.h>

#define BMI26X_SPI_WRITE          0x00
#define BMI26X_SPI_READ           0x80

#ifdef ACCEL_CAL_ENABLED
#include <calibration/accelerometer/accel_cal.h>
#endif

#if defined(OVERTEMPCAL_ENABLED) && !defined(GYRO_CAL_ENABLED)
#undef OVERTEMPCAL_ENABLED
#endif

#if defined(GYRO_CAL_DBG_ENABLED) && !defined(GYRO_CAL_ENABLED)
#undef GYRO_CAL_DBG_ENABLED
#endif

#if defined(OVERTEMPCAL_DBG_ENABLED) && !defined(OVERTEMPCAL_ENABLED)
#undef OVERTEMPCAL_DBG_ENABLED
#endif

#ifdef GYRO_CAL_ENABLED
#include <calibration/gyroscope/gyro_cal.h>
#endif  // GYRO_CAL_ENABLED

#ifdef OVERTEMPCAL_ENABLED
#include <calibration/over_temp/over_temp_cal.h>
#endif  // OVERTEMPCAL_ENABLED

/*! Nano-app configurations to be set by user */

/* Enable this to configure the I2C communication */
// #define BMI26X_I2C_BUS_ID
/* Enable the required macro to select the needed sensor */
#define BMI263
/* Heartbeat timer support */
#define BM260_HEART_BEAT_TIMER 0
/* Anymotion feature support */
#define NO_NEED_ANYMOTION_FOR_VIVO 1

/* Axes remapping for BMI26X variants */
#ifndef BMI26X_TO_ANDROID_COORDINATE
#define BMI26X_TO_ANDROID_COORDINATE(x, y, z)   \
    do {                                        \
        float xi = x, yi = y, zi = z;           \
        x = xi; y = yi; z = zi;                 \
    } while (0)
#endif

#include <limits.h>
#include <stdlib.h>
#include <string.h>

#if defined(BMI260)
#include "bmi260.h"
#endif

#if defined(BMI263)
#include "bmi263.h"
#endif

#if defined(BMI270)
#include "bmi270.h"
#endif

#include "bmi2_defs.h"
#include "bmi2xy_ver.h"

#ifdef CHRE_LATESTONE
#define VERBOSE_PRINT(fmt, ...) do { \
        osLog(LOG_VERBOSE, "%s " fmt, "[BMI26X]", ##__VA_ARGS__); \
    } while (0);
#else
#define VERBOSE_PRINT(fmt, ...) do { \
        osLog(LOG_INFO, "%s " fmt, "[BMI26X]", ##__VA_ARGS__); \
    } while (0);
#endif
#define INFO_PRINT(fmt, ...) do { \
        /*lint -e10*/  \
        osLog(LOG_INFO, "%s " fmt, "[BMI26X]", ##__VA_ARGS__); \
        /*lint +e10*/  \
    } while (0);

#define ERROR_PRINT(fmt, ...) do { \
        /*lint -e10*/              \
        osLog(LOG_ERROR, "%s " fmt, "[BMI26X] ERROR:", ##__VA_ARGS__); \
        /*lint +e10*/                                                  \
    } while (0);

#define DEBUG_PRINT(fmt, ...) do {           \
        /*lint -e506*/                       \
        if (DBG_ENABLE) {                    \
            osLog(LOG_DEBUG, "%s " fmt, "[BMI26X]", ##__VA_ARGS__); \
        }                                    \
        /*lint +e506*/                       \
    } while (0);

#define DEBUG_PRINT_IF(cond, fmt, ...) do {  \
        /*lint -e506*/                       \
        if ((cond) && DBG_ENABLE) {          \
            osLog(LOG_DEBUG, "%s " fmt, "[BMI26X]", ##__VA_ARGS__); \
        }                                    \
        /*lint +e506*/                       \
    } while (0);

/* Nano-app debuging configurations */
#define DBG_ENABLE                0
#define DBG_CHUNKED               0
#define DBG_INT                   0
#define DBG_SHALLOW_PARSE         0
#define DBG_STATE                 0
#define DBG_WM_CALC               0
#define TIMESTAMP_DBG             0

#define BMI26X_APP_VERSION        6

#define BMI26X_APP_ID APP_ID_MAKE(NANOHUB_VENDOR_GOOGLE, 2)


#ifdef BMI26X_I2C_BUS_ID
#define BMI26X_USE_I2C

#ifndef BMI26X_I2C_SPEED
#define BMI26X_I2C_SPEED          400000
#endif
#ifndef BMI26X_I2C_ADDR
#define BMI26X_I2C_ADDR           0x68
#endif

#endif //BMI26X_USE_I2C

#ifdef BMI26X_I3C_BUS_ID
#define BMI26X_USE_I3C
#define BMI26X_USE_I2C
struct I3cPid mI3cPid={{0x07,0x70,0x10,0xA0,0x00,0x00}};
#endif //BMI26X_I3C_BUS_ID

#define BMI26X_SPI_WRITE          0x00
#define BMI26X_SPI_READ           0x80

#define BMI26X_SPI_BUS_ID         ACC_GYR_SPI_BUS_ID
#define BMI26X_SPI_SPEED_HZ       ACC_GYR_SPI_CLOCK
#define BMI26X_SPI_MODE           3

#ifndef BMI26X_INT1_IRQ
#define BMI26X_INT1_IRQ           ACC_GYR_INT1_IRQ
#endif
#ifndef BMI26X_INT1_PIN
#define BMI26X_INT1_PIN           ACC_GYR_INT1_PIN
#endif

#if (SUPPORT_ACC_GYR_INT_PIN2)
#ifndef BMI26X_INT2_IRQ
#define BMI26X_INT2_IRQ           ACC_GYR_INT2_IRQ
#endif

#ifndef BMI26X_INT2_PIN
#define BMI26X_INT2_PIN           ACC_GYR_INT2_PIN
#endif
#endif

#if defined(BMI260)
#define BMI26X_ID                 BMI260_CHIP_ID
#elif defined(BMI263)
/*! BMI263 chip identifier */
#define BMI26X_ID                 BMI263_CHIP_ID    //0xA0
#else
#define BMI26X_ID                 0x24
#endif


#define INT_ANY_MOTION  0x40
#define INT_FIFO_WM     0x02


#define BMI26X_FRAME_HEADER_INVALID  0x80   // mark the end of valid data
#define BMI26X_FRAME_HEADER_SKIP     0x81   // not defined by hw, used for skip a byte in buffer

#define WATERMARK_MIN                1
#define WATERMARK_MAX                800    // must <= 255 (0xff)

#define WATERMARK_MAX_SENSOR_RATE    400    // Accel and gyro are 400 Hz max
#define WATERMARK_TIME_UNIT_NS       (1000000000ULL/(WATERMARK_MAX_SENSOR_RATE))

#define gSPI    BMI26X_SPI_BUS_ID
#define ACCL_INT_LINE EXTI_LINE_P6
#define GYR_INT_LINE EXTI_LINE_P7

#define BMI2XY_WRITE_LEN       1020

#define SPI_WRITE_0(addr, data) spiQueueWrite(addr, data, 2)
#define SPI_WRITE_1(addr, data, delay) spiQueueWrite(addr, data, delay)
#define GET_SPI_WRITE_MACRO(_1,_2,_3,NAME,...) NAME
#define SPI_WRITE(...) GET_SPI_WRITE_MACRO(__VA_ARGS__, SPI_WRITE_1, SPI_WRITE_0)(__VA_ARGS__)

#define SPI_READ_0(addr, size, buf) spiQueueRead(addr, size, buf, 0)
#define SPI_READ_1(addr, size, buf, delay) spiQueueRead(addr, size, buf, delay)
#define GET_SPI_READ_MACRO(_1,_2,_3,_4,NAME,...) NAME
#define SPI_READ(...) GET_SPI_READ_MACRO(__VA_ARGS__, SPI_READ_1, SPI_READ_0)(__VA_ARGS__)

#define EVT_SENSOR_ACC_DATA_RDY sensorGetMyEventType(SENS_TYPE_ACCEL)
#define EVT_SENSOR_GYR_DATA_RDY sensorGetMyEventType(SENS_TYPE_GYRO)
#define EVT_SENSOR_ANY_MOTION sensorGetMyEventType(SENS_TYPE_ANY_MOTION)

#define MAX_NUM_COMMS_EVENT_SAMPLES 15

#ifndef BMI26X_ACC_SAMPLES
#define BMI26X_ACC_SAMPLES 3000
#endif

#ifndef BMI26X_GYRO_SAMPLES
#define BMI26X_GYRO_SAMPLES 20
#endif



// Default accel range is 8g
#ifndef BMI26X_ACC_RANGE_G
#define BMI26X_ACC_RANGE_G 8
#endif

#if BMI26X_ACC_RANGE_G == 16
#define ACC_RANGE_SETTING 0x03
#elif BMI26X_ACC_RANGE_G == 8
#define ACC_RANGE_SETTING 0x02
#else
#error "Invalid BMI26x_ACC_RANGE_G setting: valid values are 8, 16"
#endif

#define kScale_acc    (9.81f * BMI26X_ACC_RANGE_G / 32768.0f)
#define kScale_gyr    0.00053263221f  // GYR_range * M_PI / (180.0f * 32768.0f);
#define kScale_temp   0.001953125f    // temperature in deg C
#define kTempInvalid  -1000.0f

#define kTimeSyncPeriodNs        100000000ull // sync sensor and RTC time every 100ms
#define kSensorTimerIntervalUs   39ull        // bmi26x clock increaments every 39000ns

#define kMinRTCTimeIncrementNs   1250000ull // forced min rtc time increment, 1.25ms for 400Hz
#define kMinSensorTimeIncrement  64         // forced min sensortime increment,
                                            // 64 = 2.5 msec for 400Hz

#define ACC_MIN_RATE    5
#define GYR_MIN_RATE    6
#define ACC_MAX_RATE    12
#define GYR_MAX_RATE    13
#define MAG_MAX_RATE    11
#define ACC_MAX_OSR     3
#define GYR_MAX_OSR     4
#define OSR_THRESHOLD   8

#define MOTION_ODR         7

#define RETRY_CNT_CALIBRATION 10
#define RETRY_CNT_ID 5
#define RETRY_CNT_MAG 30

#define SPI_PACKET_SIZE 30
#define FIFO_READ_SIZE  (2048+4)
#define CHUNKED_READ_SIZE (64)
#define BUF_MARGIN 32   // some extra buffer for additional reg RW when a FIFO read happens
#define SPI_BUF_SIZE (FIFO_READ_SIZE + CHUNKED_READ_SIZE + BUF_MARGIN)

#ifndef ABS
#define ABS(x) (((x) > 0) ? (x) : -(x))
#endif

enum SensorIndex {
    FIRST_CONT_SENSOR = 0,
    ACC = FIRST_CONT_SENSOR,
    GYR,
#if defined(NO_NEED_ANYMOTION_FOR_VIVO)
    NUM_CONT_SENSOR,
    NUM_OF_SENSOR = NUM_CONT_SENSOR,
#else
    NUM_CONT_SENSOR,
    FIRST_ONESHOT_SENSOR = NUM_CONT_SENSOR,
    ANYMO = FIRST_ONESHOT_SENSOR,
    NUM_OF_SENSOR,
#endif
};

enum SensorEvents {
    NO_EVT = -1,
    EVT_SPI_DONE = EVT_APP_START + 1,
    EVT_SENSOR_INTERRUPT_1,
    EVT_SENSOR_INTERRUPT_2,
    EVT_TIME_SYNC,
};

enum InitState {
    RESET_BMI26X,
    INIT_BMI26X,
    INIT_SENSOR_LOAD_CONFIG_FILE0,
    INIT_SENSOR_LOAD_CONFIG_FILE1,
    INIT_SENSOR_LOAD_CONFIG_FILE2,
    INIT_SENSOR_LOAD_CONFIG_FILE3,
    INIT_SENSOR_LOAD_CONFIG_FILE4,
    INIT_SENSOR_LOAD_CONFIG_FILE5,
    INIT_SENSOR_LOAD_CONFIG_FILE6,
    INIT_SENSOR_LOAD_CONFIG_FILE7,
    INIT_SENSOR_LOAD_CONFIG_FILE8,
    INIT_SENSOR_LOAD_CONFIG_FILE_CHECK,
    INIT_ON_CHANGE_SENSORS,
    INIT_DONE,
};

enum CalibrationState {
    CALIBRATION_START,
    CALIBRATION_FOC_PRE,
    CALIBRATION_FOC,
    CALIBRATION_WAIT_FOC_DONE,
    CALIBRATION_SET_OFFSET,
    CALIBRATION_DONE,
    CALIBRATION_CHECK,
    CALIBRATION_TIMEOUT,
};

enum AccTestState {
    ACC_TEST_START,
    ACC_TEST_CONFIG,
    ACC_TEST_RUN_0,
    ACC_TEST_RUN_1,
    ACC_TEST_VERIFY,
    ACC_TEST_INIT_REG,
    ACC_TEST_INIT_LOAD_CONFIG0,//6
    ACC_TEST_INIT_LOAD_CONFIG1,//7
    ACC_TEST_INIT_LOAD_CONFIG2,//8
    ACC_TEST_INIT_LOAD_CONFIG3,//9
    ACC_TEST_INIT_LOAD_CONFIG4,//10
    ACC_TEST_INIT_LOAD_CONFIG5,//11
    ACC_TEST_INIT_LOAD_CONFIG6,//12
    ACC_TEST_INIT_LOAD_CONFIG7,//13
    ACC_TEST_INIT_LOAD_CONFIG8,//14
    ACC_TEST_INIT_LOAD_CONFIG_CHECk,//15
    ACC_TEST_INIT_LOAD_CONFIG_CHECk_DONE,//16
    ACC_TEST_RESTORE_REG,//17
    ACC_TEST_DONE//18
};

enum GyroTestState {
    GYRO_TEST_START,
    GYRO_TEST_RUN,
    GYRO_TEST_VERIFY,
    GYRO_TEST_INIT_REG,
    GYRO_TEST_INIT_LOAD_CONFIG0 = 6,//6
    GYRO_TEST_INIT_LOAD_CONFIG1,//7
    GYRO_TEST_INIT_LOAD_CONFIG2,//8
    GYRO_TEST_INIT_LOAD_CONFIG3,//9
    GYRO_TEST_INIT_LOAD_CONFIG4,//10
    GYRO_TEST_INIT_LOAD_CONFIG5,//11
    GYRO_TEST_INIT_LOAD_CONFIG6,//12
    GYRO_TEST_INIT_LOAD_CONFIG7,//13
    GYRO_TEST_INIT_LOAD_CONFIG8,//14
    GYRO_TEST_INIT_LOAD_CONFIG_CHECk,//15
    GYRO_TEST_INIT_LOAD_CONFIG_CHECk_DONE,//16
    GYRO_TEST_RESTORE_REG,//17
    GYRO_TEST_DONE//18
};

enum SensorState {
    // keep this in sync with getStateName
    SENSOR_BOOT,
    SENSOR_VERIFY_ID,
    SENSOR_INITIALIZING,
    SENSOR_IDLE,
    SENSOR_POWERING_UP,
    SENSOR_POWERING_DOWN,
    SENSOR_CONFIG_CHANGING,
    SENSOR_INT_1_HANDLING,
    SENSOR_INT_1_HANDLING_DATA,
    SENSOR_INT_2_HANDLING,
    SENSOR_CALIBRATING,
    SENSOR_TESTING,
    SENSOR_TIME_SYNC,
    SENSOR_SAVE_CALIBRATION,
    SENSOR_NUM_OF_STATE
};
#if DBG_STATE
#define PRI_STATE "s"
static const char * getStateName(int32_t s) {
    // keep this in sync with SensorState
    static const char* const l[] = {"BOOT", "VERIFY_ID", "INIT", "IDLE", "PWR_UP",
            "PWR-DN", "CFG_CHANGE", "INT1", "INT1_DATA", "INT2", "CALIB", "TESTING", "SYNC", "SAVE_CALIB"};
    if (s >= 0 && s < SENSOR_NUM_OF_STATE) {
        return l[s];
    }
    return "Invalid";
#else
#define PRI_STATE PRIi32
static int32_t getStateName(int32_t s) {
    return s;
#endif
}

struct ConfigStat {
    uint64_t latency;
    uint32_t rate;
    bool enable;
};

struct CalibrationData {
    struct HostHubRawPacket header;
    struct SensorAppEventHeader data_header;
    int32_t xBias;
    int32_t yBias;
    int32_t zBias;
} __attribute__((packed));

struct TestResultData {
    struct HostHubRawPacket header;
    struct SensorAppEventHeader data_header;
} __attribute__((packed));

struct BMI26XSensor {
    struct ConfigStat pConfig; // pending config status request
    struct TripleAxisDataEvent *data_evt;
    uint32_t handle;
    uint32_t rate;
    uint64_t latency;
    uint64_t prev_rtc_time;
    uint32_t offset[3];
    bool powered; // activate status
    bool configed; // configure status
    bool offset_enable;
    uint8_t flush;
    uint8_t samplesToDiscard;
    enum SensorIndex idx;
};

struct OtcGyroUpdateBuffer {
    struct AppToSensorHalDataBuffer head;
    struct GyroOtcData data;
    volatile uint8_t lock; // lock for static object
    bool sendToHostRequest;
} __attribute__((packed));

struct BMI26XTask {
    uint32_t tid;
    struct BMI26XSensor sensors[NUM_OF_SENSOR];

#ifdef GYRO_CAL_ENABLED
    // Gyro Cal -- Declaration.
    struct GyroCal gyro_cal;
#endif  //  GYRO_CAL_ENABLED

#ifdef OVERTEMPCAL_ENABLED
    // Over-temp gyro calibration object.
    struct OverTempCal over_temp_gyro_cal;
    struct OtcGyroUpdateBuffer otcGyroUpdateBuffer;
#endif  //  OVERTEMPCAL_ENABLED

    // time keeping.
    uint64_t last_sensortime;
    uint64_t frame_sensortime;
    uint64_t prev_frame_time[NUM_CONT_SENSOR];
    uint64_t time_delta[NUM_CONT_SENSOR];
    uint64_t next_delta[NUM_CONT_SENSOR];
    int32_t accel_value[3];
    int32_t gyro_value[3];
    int32_t accel_avg[3];
    int32_t gyro_avg[3];
    uint8_t *reg59_val;
    uint8_t saved_reg59_val;
    uint64_t tempTime;
    uint64_t timesync_rtc_time;

    // spi and interrupt
    spi_cs_t cs;
    struct SpiMode mode;
    struct SpiPacket packets[SPI_PACKET_SIZE];
    struct SpiDevice *spiDev;
    struct Gpio *Int1;
    struct Gpio *Int2;
    IRQn_Type Irq1;
    IRQn_Type Irq2;
    struct ChainedIsr Isr1;
    struct ChainedIsr Isr2;
#ifdef ACCEL_CAL_ENABLED
    struct AccelCal acc;
#endif
    time_sync_t gSensorTime2RTC;

    float tempCelsius;
    float last_charging_bias_x;
    uint32_t total_step_cnt;
    uint32_t last_step_cnt;
    uint32_t poll_generation;
    uint32_t active_poll_generation;
    uint8_t active_oneshot_sensor_cnt;
    uint8_t interrupt_enable_0;
    uint8_t interrupt_enable_2;
    uint8_t acc_downsample;
    uint8_t gyr_downsample;
    bool magBiasPosted;
    bool magBiasCurrent;
    bool fifo_enabled[NUM_CONT_SENSOR];

    // for step count
    uint32_t stepCntSamplingTimerHandle;
    bool step_cnt_changed;

    // spi buffers
    uint32_t xferCnt;
    uint8_t *intStatus;
    uint16_t fifoLength;
    uint8_t *dataBuffer;
    uint8_t *statusBuffer;
    uint8_t *chipid;
    uint8_t *sensorTimeBuffer;
    uint8_t *temperatureBuffer;
    //selftest
    uint8_t *reg40_49;
    uint8_t *reg56_58;
    uint8_t *reg7c_7d;
    uint8_t reg_val[17];
    uint8_t txrxBuffer[SPI_BUF_SIZE];
    #if defined(BM260_HEART_BEAT_TIMER)
    uint8_t need_fifo_clean;
    uint16_t demp_reg;
    uint8_t *reg1c_21;
    #endif
    // states
    volatile uint8_t state;  //task state, type enum SensorState, do NOT change this directly
    enum InitState init_state;
    enum CalibrationState calibration_state;
    enum AccTestState acc_test_state;
    enum GyroTestState gyro_test_state;
    int16_t acc_foc_read;
    int16_t gyro_foc_read;
    // for self-test
    int16_t accTestX, accTestY, accTestZ;

    //config file
    uint8_t *config_stream_status;

    // pending configs
    bool pending_int[2];
    bool pending_step_cnt;
    bool pending_config[NUM_OF_SENSOR];
    bool pending_calibration_save;
    bool pending_time_sync;
    bool pending_delta[NUM_CONT_SENSOR];
    bool pending_dispatch;
    bool frame_sensortime_valid;

    // FIFO setting
    uint16_t chunkReadSize;
    uint16_t watermark;

    // spi rw
    struct SlabAllocator *mDataSlab;
    uint16_t mWbufCnt;
    uint8_t mRegCnt;
#ifdef BMI26X_USE_I2C
    /* Number of packets in a single batch I2C transaction */
    uint8_t cReg;
    /* Callback for I2C transaction */
    SpiCbkF sCallback;
#endif

#ifdef BMI26X_USE_I3C
    uint32_t i3cHandle;
#endif

    uint8_t mRetryLeft;
    bool spiInUse;
};

static uint32_t AccRates[] = {
    SENSOR_HZ(25.0f/2.0f),
    SENSOR_HZ(25.0f),
    SENSOR_HZ(50.0f),
    SENSOR_HZ(100.0f),
    SENSOR_HZ(200.0f),
    SENSOR_HZ(400.0f),
    0,
};

static uint32_t GyrRates[] = {
    SENSOR_HZ(25.0f/2.0f),
    SENSOR_HZ(25.0f),
    SENSOR_HZ(50.0f),
    SENSOR_HZ(100.0f),
    SENSOR_HZ(200.0f),
    SENSOR_HZ(400.0f),
    0,
};


static struct BMI26XTask mTask;

#define DEC_INFO(name, type, axis, inter, samples) \
    .sensorName = name, \
    .sensorType = type, \
    .numAxis = axis, \
    .interrupt = inter, \
    .minSamples = samples

#define DEC_INFO_RATE(name, rates, type, axis, inter, samples) \
    DEC_INFO(name, type, axis, inter, samples), \
    .supportedRates = rates

#define DEC_INFO_RATE_RAW(name, rates, type, axis, inter, samples, raw, scale) \
    DEC_INFO(name, type, axis, inter, samples), \
    .supportedRates = rates, \
    .flags1 = SENSOR_INFO_FLAGS1_RAW, \
    .rawType = raw, \
    .rawScale = scale

#define DEC_INFO_RATE_BIAS(name, rates, type, axis, inter, samples, bias) \
    DEC_INFO(name, type, axis, inter, samples), \
    .supportedRates = rates, \
    .flags1 = SENSOR_INFO_FLAGS1_BIAS, \
    .biasType = bias

#define DEC_INFO_RATE_RAW_BIAS(name, rates, type, axis, inter, samples, raw, scale, bias) \
    DEC_INFO_RATE_RAW(name, rates, type, axis, inter, samples, raw, scale), \
    .flags1 = SENSOR_INFO_FLAGS1_RAW | SENSOR_INFO_FLAGS1_BIAS, \
    .biasType = bias

typedef struct BMI26XTask _Task;
#define TASK  _Task* const _task

// To get rid of static variables all task functions should have a task structure pointer input.
// This is an intermediate step.
#define TDECL()  TASK = &mTask; (void)_task

// Access task variables without explicitly specify the task structure pointer.
#define T(v)  (_task->v)

// Atomic get state
#define GET_STATE() (atomicReadByte(&(_task->state)))

// Atomic set state, this set the state to arbitrary value, use with caution
#define SET_STATE(s) do{                                                            \
		/*lint -e10*/                                                               \
        DEBUG_PRINT_IF(DBG_STATE, "set state %" PRI_STATE "\n", getStateName(s));   \
        atomicWriteByte(&(_task->state), (s));                                      \
		/*lint +e10*/                                                               \
    }while(0)

// Atomic switch state from IDLE to desired state.
static bool trySwitchState_(TASK, enum SensorState newState) {
#if DBG_STATE
    bool ret = atomicCmpXchgByte(&T(state), SENSOR_IDLE, newState);
    uint8_t prevState = ret ? SENSOR_IDLE : GET_STATE();
    DEBUG_PRINT("switch state %" PRI_STATE "->%" PRI_STATE ", %s\n",
            getStateName(prevState), getStateName(newState), ret ? "ok" : "failed");
    return ret;
#else
    return atomicCmpXchgByte(&T(state), SENSOR_IDLE, newState);
#endif
}
// Short-hand
#define trySwitchState(s) trySwitchState_(_task, (s))

// Chunked FIFO read functions
static void chunkedReadInit_(TASK, int read_index, int size);
#define chunkedReadInit(a,b) chunkedReadInit_(_task, (a), (b))
static void chunkedReadSpiCallback(void *cookie, int error);
static void initiateFifoRead_(TASK, bool isInterruptContext);
#define initiateFifoRead(a) initiateFifoRead_(_task, (a))
static uint8_t* shallowParseFrame(uint8_t * buf, int size);

#ifdef OVERTEMPCAL_ENABLED
// otc gyro cal save restore functions
static void handleOtcGyroConfig_(TASK, const struct AppToSensorHalDataPayload *data);
#define handleOtcGyroConfig(a) handleOtcGyroConfig_(_task, (a))
static bool sendOtcGyroUpdate_();
#define sendOtcGyroUpdate() sendOtcGyroUpdate_(_task)
static void unlockOtcGyroUpdateBuffer();
#endif  // OVERTEMPCAL_ENABLED

// Binary dump to osLog
static void dumpBinary(void* buf, unsigned int address, size_t size);

// Watermark calculation
static uint16_t calcWatermark2_(TASK);
#define calcWatermark2() calcWatermark2_(_task)
/*lint -e10 -e134*/
static const struct SensorInfo mSensorInfo[NUM_OF_SENSOR] =
{
#ifdef ACCEL_CAL_ENABLED
    { DEC_INFO_RATE_RAW_BIAS("Accelerometer", AccRates, SENS_TYPE_ACCEL, NUM_AXIS_THREE,
            NANOHUB_INT_NONWAKEUP, BMI26X_ACC_SAMPLES, SENS_TYPE_ACCEL_RAW,
            1.0/kScale_acc, SENS_TYPE_ACCEL_BIAS) },
#else
    { DEC_INFO_RATE_RAW("Accelerometer", AccRates, SENS_TYPE_ACCEL, NUM_AXIS_THREE,
            NANOHUB_INT_NONWAKEUP, BMI26X_ACC_SAMPLES, SENS_TYPE_ACCEL_RAW,
            (1.0/kScale_acc)) },
#endif
    { DEC_INFO_RATE_BIAS("Gyroscope", GyrRates, SENS_TYPE_GYRO, NUM_AXIS_THREE,
            NANOHUB_INT_NONWAKEUP, BMI26X_GYRO_SAMPLES, SENS_TYPE_GYRO_BIAS) },
#if !defined(NO_NEED_ANYMOTION_FOR_VIVO)
    { DEC_INFO("Any Motion", SENS_TYPE_ANY_MOTION, NUM_AXIS_EMBEDDED, NANOHUB_INT_NONWAKEUP, 20) },
#endif
};
/*lint +e10 +e134*/
static void time_init(void) {
    (void)time_sync_init(&mTask.gSensorTime2RTC);
}

static bool sensortime_to_rtc_time(uint64_t sensor_time, uint64_t *rtc_time_ns) {
// fixme: nsec?
    return time_sync_estimate_time1(
            &mTask.gSensorTime2RTC, sensor_time * 39ull, rtc_time_ns);
}

static void map_sensortime_to_rtc_time(uint64_t sensor_time, uint64_t rtc_time_ns) {
// fixme: nsec?
    bool ret_val = time_sync_add(&mTask.gSensorTime2RTC, rtc_time_ns, sensor_time * 39ull);
	(void)ret_val;
}

static void invalidate_sensortime_to_rtc_time(void) {
    time_sync_reset(&mTask.gSensorTime2RTC);
}

static void minimize_sensortime_history(void) {
    // truncate datapoints to the latest two to maintain valid sensortime to rtc
    // mapping and minimize the inflence of the past mapping
    time_sync_truncate(&mTask.gSensorTime2RTC, 2);

    // drop the oldest datapoint when a new one arrives for two times to
    // completely shift out the influence of the past mapping
    time_sync_hold(&mTask.gSensorTime2RTC, 2);
}

static void dataEvtFree(void *ptr)
{
    TDECL();
    struct TripleAxisDataEvent *ev = (struct TripleAxisDataEvent *)ptr;
    slabAllocatorFree(T(mDataSlab), ev);
}



static void spiQueueWrite(uint8_t addr, uint8_t data, uint32_t delay)
{
    TDECL();

    if (T(spiInUse)) {
        ERROR_PRINT("SPI in use, cannot queue write\n");
        return;
    }

    T(packets[T(mRegCnt)]).size = 2;
    T(packets[T(mRegCnt)]).txBuf = &T(txrxBuffer[T(mWbufCnt)]);
    T(packets[T(mRegCnt)]).rxBuf = &T(txrxBuffer[T(mWbufCnt)]);
    T(packets[T(mRegCnt)]).delay = delay * 1000;
    T(txrxBuffer[T(mWbufCnt++)]) = BMI26X_SPI_WRITE | addr;
    T(txrxBuffer[T(mWbufCnt++)]) = data;
    T(mRegCnt)++;
}

static void spiQueueWriteBlock(uint8_t *data, uint32_t len, uint32_t delay)
{
    TDECL();

    if (T(spiInUse)) {
        ERROR_PRINT("SPI in use, cannot queue write\n");
        return;
    }

    T(packets[T(mRegCnt)]).size = len;
    T(packets[T(mRegCnt)]).txBuf = data;
    T(packets[T(mRegCnt)]).rxBuf = bmi26x_config_file_back;
    T(packets[T(mRegCnt)]).delay = delay * 1000;
    T(mRegCnt)++;
}


/*!
 * @brief SPI / I2C transactions are queued and processed later in interrupt mode
 *
 * This Nano-app has BMI26x sensors which has a variety of dummy bytes
 * when we read data from the register map
 *
 * @verbatim
 *  ____________________________________________________
 * |          |    SPI      |  I2C     |    I2C-BMI263 |
 * |----------|-------------|----------|---------------|
 * |DummyByte |     1       |    0     |      2        |
 * |----------|-------------|----------|---------------|
 * |Buffersent|[0] - ADDR   |  ADDR    |     ADDR      |
 * |----------|-------------|----------|---------------|
 * |Buffer    |[0] - ADDR   |  DATA    |     DUMMY     |
 * |received  |[1] - DUMMY  |          |     DUMMY     |
 * |          |[2] - DATA   |          |     DATA      |
 * |          |[3] -        |          |               |
 * |----------------------------------------------------
 *
 * @endverbatim
 *
 * NOTE: need to be sure size of buf is larger than read size
 *
 */
static void spiQueueRead(uint8_t addr, size_t size, uint8_t **buf, uint32_t delay)
{
    TDECL();
    if (T(spiInUse)) {
        ERROR_PRINT("SPI in use, cannot queue read %d %d\n", (int)addr, (int)size);
        return;
    }

    *buf = &T(txrxBuffer[T(mWbufCnt)]);

    T(packets[T(mRegCnt)]).size = size + 1; // first byte will not contain valid data
    T(packets[T(mRegCnt)]).txBuf = &T(txrxBuffer[T(mWbufCnt)]);
    T(packets[T(mRegCnt)]).rxBuf = *buf;
    T(packets[T(mRegCnt)]).delay = delay * 1000;
    T(txrxBuffer[T(mWbufCnt)++]) = BMI26X_SPI_READ | addr;
    T(mWbufCnt) += size;
    T(mRegCnt)++;
}

#ifdef BMI26X_USE_I2C
static void i2cBatchTxRx(void *evtData, int err);
#endif

static void spiBatchTxRx(struct SpiMode *mode,
        SpiCbkF callback, void *cookie, const char * src)
{
    TDECL();
    if (T(mWbufCnt) > SPI_BUF_SIZE) {
        ERROR_PRINT("NO enough SPI buffer space, dropping transaction.\n");
        return;
    }

    if (T(mRegCnt) > SPI_PACKET_SIZE) {
        ERROR_PRINT("spiBatchTxRx too many packets!\n");
        return;
    }

    T(spiInUse) = true;
    T(mWbufCnt) = 0;

#ifdef BMI26X_USE_I2C
    T(cReg) = 0;
    T(sCallback) = callback;
    i2cBatchTxRx(cookie, 0);
#else
    // Reset variables before issuing SPI transaction.
    // SPI may finish before spiMasterRxTx finish
    uint8_t regCount = T(mRegCnt);
    T(mRegCnt) = 0;

    if (spiMasterRxTx(T(spiDev), T(cs), T(packets), regCount, mode, callback, cookie) < 0) {
        ERROR_PRINT("spiMasterRxTx failed!\n");
    }
#endif
}

static void bmi2xyConfigLoadConfigFile(uint16_t conf_index)
{
    uint8_t asic_msb = (uint8_t)((conf_index / 2) >> 4);
    uint8_t asic_lsb = ((conf_index / 2) & 0x0F);
    /* Write to feature config register */
    SPI_WRITE(BMI2_INIT_ADDR_0, asic_lsb, 500);
    /* Write to feature config register */
    SPI_WRITE(BMI2_INIT_ADDR_1, asic_msb, 500);
    if (conf_index == 8160) {
        internel_write[0] = 0x5e;

        #if defined(BMI260)
        memcpy(&internel_write[1], &bmi260_config_file[conf_index], 32);
        #endif

		#if defined(BMI263)
        memcpy(&internel_write[1], &bmi263_config_file[conf_index], 32);
        #endif

        #if defined(BMI270)
        memcpy(&internel_write[1], &bmi270_config_file[conf_index], 32);
        #endif

        DEBUG_PRINT("internel_write last %p %x %x %x %x %x\n", internel_write,
        internel_write[0], internel_write[1], internel_write[2], internel_write[3],
        internel_write[32]);
        /* Write to feature config registers */
        spiQueueWriteBlock(internel_write, (32 + 1), 1000);
    } else {
        internel_write[0] = 0x5e;

        #if defined(BMI260)
        memcpy(&internel_write[1], &bmi260_config_file[conf_index], 1020);
        #endif

        #if defined(BMI263)
        memcpy(&internel_write[1], &bmi263_config_file[conf_index], 1020);
        #endif

        #if defined(BMI270)
        memcpy(&internel_write[1], &bmi270_config_file[conf_index], 1020);
        #endif

        DEBUG_PRINT("internel_write %p %x %x %x %x %x %x\n", internel_write,
        internel_write[0], internel_write[1], internel_write[2], internel_write[3],
        internel_write[1019], internel_write[1020]);
        /* Write to feature config registers */
        spiQueueWriteBlock(internel_write, (1020 + 1), 1000);
    }
}

static void sensorSpiCallback(void *cookie, int err)
{
    mTask.spiInUse = false;

    if (!osEnqueuePrivateEvt(EVT_SPI_DONE, cookie, NULL, mTask.tid))
        ERROR_PRINT("sensorSpiCallback: osEnqueuePrivateEvt() failed\n");
}


static bool bmi26xIsr1(struct ChainedIsr *isr)
{
#ifdef EXYNOS_CONTEXTHUB
    TDECL();
    DEBUG_PRINT_IF(DBG_INT, "i1\n");
    if (trySwitchState(SENSOR_INT_1_HANDLING)) {
        /*read the interrupt status temperature and  fifo length*/
        /* 1c 1d 1e 1f 20 21 22 23 24 25 */
        //SPI_READ(BMI2_INT_STATUS_0_ADDR, 11, &mTask.intStatus);
        SPI_READ(BMI2_INT_STATUS_0_ADDR, 11, &mTask.intStatus);
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask, __FUNCTION__);
    } else {
            T(pending_int[0]) = true;
    }
    gpioClearPending(BMI26X_INT1_PIN);
#else
	/*lint -e10 -e26 -e40*/
    TASK = container_of(isr, struct BMI26XTask, Isr1);
	/*lint +e10 +e26 +e40*/

    if (!extiIsPendingGpio(T(Int1))) {
        return false;
    }
    DEBUG_PRINT_IF(DBG_INT, "i1\n");
    if (trySwitchState(SENSOR_INT_1_HANDLING)) {
        /*read the interrupt status temperature and  fifo length*/
        /* 1c 1d 1e 1f 20 21 22 23 24 25 */
        //SPI_READ(BMI2_INT_STATUS_0_ADDR, 11, &mTask.intStatus);
        SPI_READ(BMI2_INT_STATUS_0_ADDR, 11, &mTask.intStatus);
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask, __FUNCTION__);
    } else {
            T(pending_int[0]) = true;
    }
    extiClearPendingGpio(T(Int1));
#endif
    return true;
}

#if (SUPPORT_ACC_GYR_INT_PIN2)
static bool bmi26xIsr2(struct ChainedIsr *isr)
{
	/*lint -e10 -e26 -e40*/
    TASK = container_of(isr, struct BMI26XTask, Isr2);
	/*lint +e10 +e26 +e40*/

    if (!extiIsPendingGpio(T(Int2)))
        return false;

    DEBUG_PRINT_IF(DBG_INT, "i2\n");
    if (!osEnqueuePrivateEvt(EVT_SENSOR_INTERRUPT_2, _task, NULL, T(tid)))
        ERROR_PRINT("bmi26xIsr2: osEnqueuePrivateEvt() failed\n");
    extiClearPendingGpio(T(Int2));
    return true;
}
#endif


static void sensorTimerCallback(uint32_t timerId, void *data)
{
    if (!osEnqueuePrivateEvt(EVT_SPI_DONE, data, NULL, mTask.tid))
        ERROR_PRINT("sensorTimerCallback: osEnqueuePrivateEvt() failed\n")
}

static void timeSyncCallback(uint32_t timerId, void *data)
{
    if (!osEnqueuePrivateEvt(EVT_TIME_SYNC, data, NULL, mTask.tid))
        ERROR_PRINT("timeSyncCallback: osEnqueuePrivateEvt() failed\n");
}

static bool accFirmwareUpload(void *cookie)
{
    (void)sensorSignalInternalEvt(mTask.sensors[ACC].handle,
            SENSOR_INTERNAL_EVT_FW_STATE_CHG, 1, 0);
    return true;
}

static bool gyrFirmwareUpload(void *cookie)
{
    (void)sensorSignalInternalEvt(mTask.sensors[GYR].handle,
            SENSOR_INTERNAL_EVT_FW_STATE_CHG, 1, 0);
    return true;
}
#if !defined(NO_NEED_ANYMOTION_FOR_VIVO)
static bool anyMotionFirmwareUpload(void *cookie)
{
    (void)sensorSignalInternalEvt(mTask.sensors[ANYMO].handle,
            SENSOR_INTERNAL_EVT_FW_STATE_CHG, 1, 0);
    return true;
}
#endif

#ifdef EXYNOS_CONTEXTHUB
static bool enableInterrupt(struct Gpio *pin, struct ChainedIsr *isr)
{
    gpioSetInterrupt(pin, Rising_Edge, eDigitalFLT, 0x10, (void (*)(void))(isr->func));
    return true;
}
#else
static bool enableInterrupt(struct Gpio *pin, IRQn_Type irq, struct ChainedIsr *isr)
{
    gpioConfigInput(pin, GPIO_SPEED_LOW, GPIO_PULL_NONE);
    syscfgSetExtiPort(pin);
    extiEnableIntGpio(pin, EXTI_TRIGGER_RISING);
    (void)extiChainIsr(irq, isr);
    return true;
}
#endif

static bool disableInterrupt(struct Gpio *pin, IRQn_Type irq, struct ChainedIsr *isr)
{
#ifdef EXYNOS_CONTEXTHUB
    gpioUnsetExtInterrupt(BMI26X_INT1_PIN);
#else
    (void)extiUnchainIsr(irq, isr);
    extiDisableIntGpio(pin);
#endif
    return true;
}

static bool flushData(struct BMI26XSensor *sensor, uint32_t eventId)
{
    bool success = false;

    if (sensor->data_evt) {
        success = osEnqueueEvtOrFree(eventId, sensor->data_evt, dataEvtFree);
        sensor->data_evt = NULL;
    }

    return success;
}

static void flushAllData(void)
{
    int i;
    for (i = FIRST_CONT_SENSOR; i < NUM_CONT_SENSOR; i++) {
        (void)flushData(&mTask.sensors[i],
                EVENT_TYPE_BIT_DISCARDABLE | sensorGetMyEventType(mSensorInfo[i].sensorType));
    }
}

static bool allocateDataEvt(struct BMI26XSensor *mSensor, uint64_t rtc_time)
{
    TDECL();
    mSensor->data_evt = slabAllocatorAlloc(T(mDataSlab));
    if (mSensor->data_evt == NULL) {
        // slab allocation failed
        ERROR_PRINT("slabAllocatorAlloc() failed\n");
        return false;
    }

    // delta time for the first sample is sample count
    memset(&mSensor->data_evt->samples[0].firstSample, 0x00, sizeof(struct SensorFirstSample));
    mSensor->data_evt->referenceTime = rtc_time;
    mSensor->prev_rtc_time = rtc_time;

    return true;
}

static inline bool anyFifoEnabled(void)
{
    bool anyFifoEnable = mTask.fifo_enabled[ACC] || mTask.fifo_enabled[GYR];
    return anyFifoEnable;
}

static void configFifo(void)
{
    TDECL();
    int i;
    uint8_t val = 0x10;
    uint8_t data[2] = {0};
    bool any_fifo_enabled_prev = anyFifoEnabled();
    // config the fifo register
    //SPI_WRITE(BMI2_FIFO_CONFIG_1_ADDR, val);
    //SPI_WRITE(BMI2_CMD_REG_ADDR, 0xb0);
    // if ACC is configed, enable ACC bit in fifo_config reg.
    if (mTask.sensors[ACC].configed && mTask.sensors[ACC].latency != SENSOR_LATENCY_NODATA) {
        val |= 0x40;
        mTask.fifo_enabled[ACC] = true;
    } else {
        mTask.fifo_enabled[ACC] = false;
    }

    // if GYR is configed, enable GYR bit in fifo_config reg.
    if (mTask.sensors[GYR].configed && mTask.sensors[GYR].latency != SENSOR_LATENCY_NODATA) {
        val |= 0x80;
        mTask.fifo_enabled[GYR] = true;
    } else {
        mTask.fifo_enabled[GYR] = false;
    }

    // if this is the first data sensor fifo to enable, start to
    // sync the sensor time and rtc time
    if (!any_fifo_enabled_prev && anyFifoEnabled()) {
        invalidate_sensortime_to_rtc_time();

        // start a new poll generation and attach the generation number to event
        if (!osEnqueuePrivateEvt(EVT_TIME_SYNC, (void *)mTask.poll_generation, NULL, mTask.tid))
            ERROR_PRINT("configFifo: osEnqueuePrivateEvt() failed\n");
    }

    // cancel current poll generation
    if (any_fifo_enabled_prev && !anyFifoEnabled()) {
        ++mTask.poll_generation;
    }

    // if this is not the first fifo enabled or last fifo disabled, flush all fifo data;
    //if (any_fifo_enabled_prev && anyFifoEnabled()) {
    //    mTask.pending_dispatch = true;
    //    mTask.xferCnt = FIFO_READ_SIZE;
    //    SPI_READ(BMI2_FIFO_DATA_ADDR, mTask.xferCnt, &mTask.dataBuffer);
    //}

    // calculate the new watermark level
    if (anyFifoEnabled()) {
        mTask.watermark = calcWatermark2_(_task);
        if (mTask.watermark == 0)
        {
          mTask.watermark = 1;
        }
        mTask.watermark = 1;
        DEBUG_PRINT("wm=%d", mTask.watermark);
        /* config the watermark */
        data[0] = BMI2_GET_LSB(mTask.watermark);
        data[1] = BMI2_GET_MSB(mTask.watermark);
        SPI_WRITE(BMI2_FIFO_WTM_0_ADDR, data[0]);
        SPI_WRITE(BMI2_FIFO_WTM_1_ADDR, data[1]);
    }

    // config the fifo register
    SPI_WRITE(BMI2_FIFO_CONFIG_1_ADDR, val);

    // if no more fifo enabled, we need to cleanup the fifo and invalidate time
    if (!anyFifoEnabled()) {
        SPI_WRITE(BMI2_CMD_REG_ADDR, 0xb0);
        mTask.frame_sensortime_valid = false;
        for (i = FIRST_CONT_SENSOR; i < NUM_CONT_SENSOR; i++) {
            mTask.pending_delta[i] = false;
            mTask.prev_frame_time[i] = ULONG_LONG_MAX;
        }
    }

}

static bool accPower(bool on, void *cookie)
{
    TDECL();
    uint8_t val = 0;
    INFO_PRINT("accPower: on=%d, state=%" PRI_STATE "\n", on, getStateName(GET_STATE()));
    if (trySwitchState(on ? SENSOR_POWERING_UP : SENSOR_POWERING_DOWN)) {
        if (on) {
            // set ACC power mode to NORMAL
            //SPI_WRITE(BMI26X_REG_CMD, 0x11, 50000);
            if (mTask.sensors[GYR].powered == 1) {
                val = 0x0E;/* bit 3 temp, bit 2 acc bit 1 gyr bit 0 aux */
            } else {
                val = 0x0C;/* bit 3 temp, bit 2 acc bit 1 gyr bit 0 aux */
            }
            /* enable the acc power bit set acc normal power mode*/
            SPI_WRITE(BMI2_PWR_CTRL_ADDR, val, 50000);
        } else {
            // set ACC power mode to SUSPEND
            mTask.sensors[ACC].configed = false;
            configFifo();
#if !defined(NO_NEED_ANYMOTION_FOR_VIVO)
            //anymotion disable
            if (!mTask.sensors[ANYMO].powered) {
                if ((mTask.sensors[GYR].powered == 1)) {
                    val = 0x0A;/* bit 3 temp, bit 2 acc bit 1 gyr bit 0 aux */
                } else {
                    val = 0x00;/* bit 3 temp, bit 2 acc bit 1 gyr bit 0 aux */
                }
                //anymotion enable
            } else {
              if (mTask.sensors[GYR].powered == 1) {
                  val = 0x0E;/* bit 3 temp, bit 2 acc bit 1 gyr bit 0 aux */
              } else {
                  val = 0x04;/* bit 3 temp, bit 2 acc bit 1 gyr bit 0 aux */
              }
            }
#else
            {
              if (mTask.sensors[GYR].powered == 1) {
                  val = 0x0E;/* bit 3 temp, bit 2 acc bit 1 gyr bit 0 aux */
              } else {
                  val = 0x04;/* bit 3 temp, bit 2 acc bit 1 gyr bit 0 aux */
              }
            }
#endif
            /* enable the acc power bit set acc normal power mode*/
            SPI_WRITE(BMI2_PWR_CTRL_ADDR, val, 50000);
        }
        mTask.sensors[ACC].powered = on;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[ACC], __FUNCTION__);
    } else {
        mTask.pending_config[ACC] = true;
        mTask.sensors[ACC].pConfig.enable = on;
    }
    return true;
}

static bool gyrPower(bool on, void *cookie)
{
    TDECL();
    INFO_PRINT("gyrPower: on=%d, state=%" PRI_STATE "\n", on, getStateName(GET_STATE()));
    uint8_t val = 0;
    if (trySwitchState(on ? SENSOR_POWERING_UP : SENSOR_POWERING_DOWN)) {
        if (on) {
            //anymotion and acc do not enable
            if (
#if !defined(NO_NEED_ANYMOTION_FOR_VIVO)
                !mTask.sensors[ANYMO].powered &&
#endif
                !mTask.sensors[ACC].powered) {
                val = 0x0A;/* bit 3 temp, bit 2 acc bit 1 gyr bit 0 aux */
            } else {
                val = 0x0E;/* bit 3 temp, bit 2 acc bit 1 gyr bit 0 aux */
            }
            /* enable the gyro power bit set gyro normal power mode*/
            SPI_WRITE(BMI2_PWR_CTRL_ADDR, val, 5000);
        } else {
            // set GYR power mode to SUSPEND
            mTask.sensors[GYR].configed = false;
            configFifo();
            //anymotion and acc do not enable
            if (
#if !defined(NO_NEED_ANYMOTION_FOR_VIVO)
                !mTask.sensors[ANYMO].powered &&
#endif
                !mTask.sensors[ACC].powered) {
                val = 0x00;/* bit 3 temp, bit 2 acc bit 1 gyr bit 0 aux */
            } else {
                val = 0x0C;/* bit 3 temp, bit 2 acc bit 1 gyr bit 0 aux */
            }
            //SPI_WRITE(BMI26X_REG_CMD, 0x14, 5000);
            /* enable the gyro power bit set gyro normal power mode*/
            SPI_WRITE(BMI2_PWR_CTRL_ADDR, val, 5000);
        }

        if (anyFifoEnabled() && on != mTask.sensors[GYR].powered) {
#if TIMESTAMP_DBG
            DEBUG_PRINT("minimize_sensortime_history()\n");
#endif
            minimize_sensortime_history();
        }

        mTask.sensors[GYR].powered = on;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[GYR], __FUNCTION__);
    } else {
        mTask.pending_config[GYR] = true;
        mTask.sensors[GYR].pConfig.enable = on;
    }
    return true;
}
#if !defined(NO_NEED_ANYMOTION_FOR_VIVO)
static bool anyMotionPower(bool on, void *cookie)
{
    TDECL();
    INFO_PRINT("anyMotionPower: on=%d, oneshot_cnt %d, state=%" PRI_STATE "\n",
            on, mTask.active_oneshot_sensor_cnt, getStateName(GET_STATE()));
    uint8_t val = 0;
    if (trySwitchState(on ? SENSOR_POWERING_UP : SENSOR_POWERING_DOWN)) {
        if (on) {
            /* map the anymotion to the int 1 */
            SPI_WRITE(BMI2_INT1_MAP_FEAT_ADDR, 0x40, 450);
            /* enable the acc power */
            val = 0x04;/* bit 3 temp, bit 2 acc bit 1 gyr bit 0 aux */
            if (mTask.sensors[GYR].powered) {
                val = val | 0x0A;/* bit 3 temp, bit 2 acc bit 1 gyr bit 0 aux */
            }
            SPI_WRITE(BMI2_PWR_CTRL_ADDR, val, 5000);
        } else {
            /* no map the anymotion to the int 1 */
            SPI_WRITE(BMI2_INT1_MAP_FEAT_ADDR, 0x00, 450);
            /* check the acc power */
            if (mTask.sensors[ACC].powered) {
                val = val | 0x0C;/* bit 3 temp, bit 2 acc bit 1 gyr bit 0 aux */
            }
            if (mTask.sensors[GYR].powered) {
                val = val | 0x0A;/* bit 3 temp, bit 2 acc bit 1 gyr bit 0 aux */
            }
            SPI_WRITE(BMI2_PWR_CTRL_ADDR, val, 5000);
            mTask.sensors[ANYMO].configed = false;
        }
        mTask.sensors[ANYMO].powered = on;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[ANYMO], __FUNCTION__);
    } else {
        mTask.pending_config[ANYMO] = true;
        mTask.sensors[ANYMO].pConfig.enable = on;
    }
    return true;
}
#endif
static void updateTimeDelta(uint8_t idx, uint8_t odr)
{
    if (mTask.fifo_enabled[idx]) {
        // wait till control frame to update, if not disabled
        mTask.next_delta[idx] = 1ull << (16 - odr);
        mTask.pending_delta[idx] = true;
    } else {
        mTask.time_delta[idx] = 1ull << (16 - odr);
    }
}

// compute the register value from sensor rate.
static uint8_t computeOdr(uint32_t rate)
{
    uint8_t odr = 0x00;
    switch (rate) {
    // lint -fallthrough fall through intended to get the correct register value
	/*lint -e616*/
    case SENSOR_HZ(3200): odr ++;
    case SENSOR_HZ(1600): odr ++;
    case SENSOR_HZ(800): odr ++;
    case SENSOR_HZ(400): odr ++;
    case SENSOR_HZ(200): odr ++;
    case SENSOR_HZ(100): odr ++;
    case SENSOR_HZ(50): odr ++;
    case SENSOR_HZ(25): odr ++;
    case SENSOR_HZ(25.0f/2.0f): odr ++;
    case SENSOR_HZ(25.0f/4.0f): odr ++;
    case SENSOR_HZ(25.0f/8.0f): odr ++;
    case SENSOR_HZ(25.0f/16.0f): odr ++;
    case SENSOR_HZ(25.0f/32.0f): odr ++;
    default:
        return odr;
    }
	/*lint +e616*/
}

static bool accSetRate(uint32_t rate, uint64_t latency, void *cookie)
{
    TDECL();
    int odr = 0;

    // change this to DEBUG_PRINT as there will be frequent (un)subscribings
    // to accel with different rate/latency requirements.
    INFO_PRINT("accSetRate: rate=%ld, latency=%lld, state=%" PRI_STATE "\n",
                rate / 1024, latency, getStateName(GET_STATE()));

    if (trySwitchState(SENSOR_CONFIG_CHANGING)) {
        odr = computeOdr(rate);
        if (!odr) {
            ERROR_PRINT("invalid acc rate\n");
            return false;
        }

        updateTimeDelta(ACC, odr);

        // minimum supported rate for ACCEL is 12.5Hz.
        // Anything lower than that shall be acheived by downsampling.
      // if (odr < ACC_MIN_RATE) {
      //      osr = ACC_MIN_RATE - odr;
        //    odr = ACC_MIN_RATE;
        //}

        // for high odrs, oversample to reduce hw latency and downsample
        // to get desired odr
        //if (odr > OSR_THRESHOLD) {
        //    osr = (ACC_MAX_OSR + odr) > ACC_MAX_RATE ? (ACC_MAX_RATE - odr) : ACC_MAX_OSR;
         //   odr += osr;
        //}

        mTask.sensors[ACC].rate = rate;
        mTask.sensors[ACC].latency = latency;
        mTask.sensors[ACC].configed = true;
        mTask.acc_downsample = 0;
       // if ((mTask.fifo_enabled[ACC] == true) && (mTask.fifo_enabled[GYR] == false)) {
        //    val = 0x10;
        // } else if((mTask.fifo_enabled[GYR] == true)) {
        //    val = 0x90;
        // }
         //SPI_WRITE(BMI2_FIFO_CONFIG_1_ADDR, 0x10);
        // set ACC bandwidth parameter to 2 (bits[4:6])
        // set the rate (bits[0:3])
        SPI_WRITE(BMI2_ACC_CONF_ADDR, 0xA0 | odr);

        // configure down sampling ratio, 0x88 is to specify we are using
        // filtered samples
       // SPI_WRITE(BMI2_FIFO_DOWNS_ADDR, (mTask.acc_downsample << 4) | mTask.gyr_downsample | 0x88);
        DEBUG_PRINT("accSetRatevalue: rate=%d, downsampleacc = %d gyr_downsample %d\n",
                     (0xA0 | odr), mTask.acc_downsample, mTask.gyr_downsample);

        // flush the data and configure the fifo
        configFifo();

        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[ACC], __FUNCTION__);
    } else {
        mTask.pending_config[ACC] = true;
        mTask.sensors[ACC].pConfig.enable = 1;
        mTask.sensors[ACC].pConfig.rate = rate;
        mTask.sensors[ACC].pConfig.latency = latency;
    }
    return true;
}

static bool gyrSetRate(uint32_t rate, uint64_t latency, void *cookie)
{
    TDECL();
    int odr, osr = 0;
    INFO_PRINT("gyrSetRate: rate=%ld, latency=%lld, state=%" PRI_STATE "\n",
               rate, latency, getStateName(GET_STATE()));

    if (trySwitchState(SENSOR_CONFIG_CHANGING)) {
        odr = computeOdr(rate);
        if (!odr) {
            ERROR_PRINT("invalid gyr rate\n");
            return false;
        }

        updateTimeDelta(GYR, odr);

        // minimum supported rate for GYRO is 25.0Hz.
        // Anything lower than that shall be acheived by downsampling.
        if (odr < GYR_MIN_RATE) {
            osr = GYR_MIN_RATE - odr;
            odr = GYR_MIN_RATE;
        } else {
            osr = 0;
        }

        // for high odrs, oversample to reduce hw latency and downsample
        // to get desired odr
       // if (odr > OSR_THRESHOLD) {
        //    osr = (GYR_MAX_OSR + odr) > GYR_MAX_RATE ? (GYR_MAX_RATE - odr) : GYR_MAX_OSR;
        //    odr += osr;
       // }
        mTask.sensors[GYR].rate = rate;
        mTask.sensors[GYR].latency = latency;
        mTask.sensors[GYR].configed = true;
        mTask.gyr_downsample = osr;
        //SPI_WRITE(BMI2_FIFO_CONFIG_1_ADDR, 0x10);

        // set GYR bandwidth parameter to 2 (bits[4:6])
        // set the rate (bits[0:3])
        SPI_WRITE(BMI2_GYR_CONF_ADDR, 0xE0 | odr);

        // configure down sampling ratio, 0x88 is to specify we are using
        // filtered samples
        SPI_WRITE(BMI2_FIFO_DOWNS_ADDR, (mTask.acc_downsample << 4) | mTask.gyr_downsample | 0x88);
        DEBUG_PRINT("gyroSetRatevalue: rate=%d, downsampleacc = %d gyr_downsample %d\n",
                     (0xE0 | odr), mTask.acc_downsample, mTask.gyr_downsample);

        // flush the data and configure the fifo
        configFifo();

        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[GYR], __FUNCTION__);
    } else {
        mTask.pending_config[GYR] = true;
        mTask.sensors[GYR].pConfig.enable = 1;
        mTask.sensors[GYR].pConfig.rate = rate;
        mTask.sensors[GYR].pConfig.latency = latency;
    }
    return true;
}
#if !defined(NO_NEED_ANYMOTION_FOR_VIVO)

static bool anyMotionSetRate(uint32_t rate, uint64_t latency, void *cookie)
{
    mTask.sensors[ANYMO].rate = rate;
    mTask.sensors[ANYMO].latency = latency;
    mTask.sensors[ANYMO].configed = true;

    (void)sensorSignalInternalEvt(mTask.sensors[ANYMO].handle,
            SENSOR_INTERNAL_EVT_RATE_CHG, rate, latency);

    return true;
}
#endif
static void sendFlushEvt(void)
{
    while (mTask.sensors[ACC].flush > 0) {
        (void)osEnqueueEvt(EVT_SENSOR_ACC_DATA_RDY, SENSOR_DATA_EVENT_FLUSH, NULL);
        mTask.sensors[ACC].flush--;
    }
    while (mTask.sensors[GYR].flush > 0) {
        (void)osEnqueueEvt(EVT_SENSOR_GYR_DATA_RDY, SENSOR_DATA_EVENT_FLUSH, NULL);
        mTask.sensors[GYR].flush--;
    }
}

static bool accFlush(void *cookie)
{
    TDECL();
    DEBUG_PRINT("accFlush\n");
    mTask.sensors[ACC].flush++;
    initiateFifoRead(false /*isInterruptContext*/);
    return true;
}

static bool gyrFlush(void *cookie)
{
    TDECL();
    DEBUG_PRINT("gyrFlush\n");
    mTask.sensors[GYR].flush++;
    initiateFifoRead(false /*isInterruptContext*/);
    return true;
}


#if !defined(NO_NEED_ANYMOTION_FOR_VIVO)
static bool anyMotionFlush(void *cookie)
{
    return osEnqueueEvt(EVT_SENSOR_ANY_MOTION, SENSOR_DATA_EVENT_FLUSH, NULL);
}
#endif
static uint64_t parseSensortime(uint32_t sensor_time24)
{
    uint32_t prev_time24;
    uint32_t kHalf = 1ul << 23;
    uint64_t full;

    prev_time24 = (uint32_t)mTask.last_sensortime & 0xffffff;

    if (mTask.last_sensortime == 0) {
        mTask.last_sensortime = (uint64_t)sensor_time24;
        return (uint64_t)(sensor_time24);
    }

    if (sensor_time24 == prev_time24) {
        return (uint64_t)(mTask.last_sensortime);
    }

    full = (mTask.last_sensortime & ~0xffffffull) | sensor_time24;

    if (((prev_time24 < sensor_time24) && (sensor_time24 - prev_time24) < kHalf)
            || ((prev_time24 > sensor_time24) && (prev_time24 - sensor_time24) > kHalf)) {
        if (full < mTask.last_sensortime) {
            full += 0x1000000ull;
        }
        mTask.last_sensortime = full;
        return mTask.last_sensortime;
    }

    if (full < mTask.last_sensortime) {
        return full;
    }

    return (full -  0x1000000ull);
}

static void parseRawData(struct BMI26XSensor *mSensor, uint8_t *buf, float kScale, uint64_t sensorTime)
{
    TDECL();
    struct TripleAxisDataPoint *sample;
    uint64_t rtc_time, cur_time;
    int32_t delta_time;
    float x, y, z;
    int16_t raw_x, raw_y, raw_z;

    if (!sensortime_to_rtc_time(sensorTime, &rtc_time)) {
        return;
    }

    cur_time = sensorGetTime();
    if (rtc_time > cur_time + kMinRTCTimeIncrementNs) { // + tolerance to prevent frequent tripping
        DEBUG_PRINT("Future ts %s: rtc_time = %llu, cur_time = %llu",
                mSensorInfo[mSensor->idx].sensorName, rtc_time, cur_time);
        // clamp to current time
        rtc_time = cur_time + kMinRTCTimeIncrementNs;
    }

    if (rtc_time < mSensor->prev_rtc_time + kMinRTCTimeIncrementNs) {
#if TIMESTAMP_DBG
        DEBUG_PRINT("%s prev rtc 0x%08x %08x, curr 0x%08x %08x, delta %d usec\n",
                mSensorInfo[mSensor->idx].sensorName,
                (unsigned int)((mSensor->prev_rtc_time >> 32) & 0xffffffff),
                (unsigned int)(mSensor->prev_rtc_time & 0xffffffff),
                (unsigned int)((rtc_time >> 32) & 0xffffffff),
                (unsigned int)(rtc_time & 0xffffffff),
                (int)(rtc_time - mSensor->prev_rtc_time) / 1000);
#endif
        rtc_time = mSensor->prev_rtc_time + kMinRTCTimeIncrementNs;
    }


    {
        raw_x = (buf[0] | buf[1] << 8);
        raw_y = (buf[2] | buf[3] << 8);
        raw_z = (buf[4] | buf[5] << 8);

        BMI26X_TO_ANDROID_COORDINATE(raw_x, raw_y, raw_z);

        x = (float)raw_x * kScale;
        y = (float)raw_y * kScale;
        z = (float)raw_z * kScale;




        if (mSensor->idx == ACC) {

#ifdef ACCEL_CAL_ENABLED
          accelCalRun(&mTask.acc, rtc_time,
                      x, y, z, mTask.tempCelsius);

          accelCalBiasRemove(&mTask.acc, &x, &y, &z);
#ifdef ACCEL_CAL_DBG_ENABLED
          // Prints debug data report.
          accelCalDebPrint(&mTask.acc, mTask.tempCelsius);
#endif  // ACCEL_CAL_DBG_ENABLED
#endif  // ACCEL_CAL_ENABLED

#ifdef GYRO_CAL_ENABLED
          // Gyro Cal -- Add accelerometer sample.
          gyroCalUpdateAccel(&mTask.gyro_cal,
                             rtc_time,  // nsec
                             x, y, z);
#endif  // GYRO_CAL_ENABLED
        } else if (mSensor->idx == GYR) {
#ifdef GYRO_CAL_ENABLED
          // Gyro Cal -- Add gyroscope and temperature sample.
          gyroCalUpdateGyro(&mTask.gyro_cal,
                            rtc_time,  // nsec
                            x, y, z, mTask.tempCelsius);

#ifdef OVERTEMPCAL_ENABLED
          // Over-Temp Gyro Cal -- Update measured temperature.
          overTempCalSetTemperature(&mTask.over_temp_gyro_cal, rtc_time,
                                    mTask.tempCelsius);

          // Over-Temp Gyro Cal -- Apply over-temp calibration correction.
          overTempCalRemoveOffset(&mTask.over_temp_gyro_cal, rtc_time,
                                  x, y, z,    /* input values */
                                  &x, &y, &z  /* calibrated output */);
#else  // OVERTEMPCAL_ENABLED
          // Gyro Cal -- Apply calibration correction.
          gyroCalRemoveBias(&mTask.gyro_cal,
                            x, y, z,    /* input values */
                            &x, &y, &z  /* calibrated output */);
#endif  // OVERTEMPCAL_ENABLED

#if defined(GYRO_CAL_DBG_ENABLED) || defined(OVERTEMPCAL_DBG_ENABLED)
          // This flag keeps GyroCal and OverTempCal from printing back-to-back.
          // If they do, then sometimes important print log data gets dropped.
          static size_t print_flag = 0;

          if (print_flag > 0) {
#ifdef GYRO_CAL_DBG_ENABLED
            // Gyro Cal -- Read out Debug data.
            gyroCalDebugPrint(&mTask.gyro_cal, rtc_time);
#endif  // GYRO_CAL_DBG_ENABLED
            print_flag = 0;
          } else {
#ifdef OVERTEMPCAL_ENABLED
#ifdef OVERTEMPCAL_DBG_ENABLED
            // Over-Temp Gyro Cal -- Read out Debug data.
            overTempCalDebugPrint(&mTask.over_temp_gyro_cal, rtc_time);
#endif  // OVERTEMPCAL_DBG_ENABLED
#endif  // OVERTEMPCAL_ENABLED
            print_flag = 1;
          }
#endif  // GYRO_CAL_DBG_ENABLED || OVERTEMPCAL_DBG_ENABLED
#endif  // GYRO_CAL_ENABLED
        }
    }

    if (mSensor->data_evt == NULL) {
        if (!allocateDataEvt(mSensor, rtc_time)) {
            return;
        }
    }

    if (mSensor->data_evt->samples[0].firstSample.numSamples >= MAX_NUM_COMMS_EVENT_SAMPLES) {
        ERROR_PRINT("BAD INDEX\n");
        return;
    }

#ifdef ACCEL_CAL_ENABLED
    // https://source.android.com/devices/sensors/sensor-types.html
    // "The bias and scale calibration must only be updated while the sensor is deactivated,
    // so as to avoid causing jumps in values during streaming." Note, this is now regulated
    // by the SensorHAL.
    if (mSensor->idx == ACC) {
        float accel_offset[3] = {0.0f, 0.0f, 0.0f};
        bool accelCalNewBiasAvailable = accelCalUpdateBias(
            &mTask.acc, &accel_offset[0], &accel_offset[1], &accel_offset[2]);
        if (accelCalNewBiasAvailable) {
            if (mSensor->data_evt->samples[0].firstSample.numSamples > 0) {
                // Flushes existing samples so the bias appears after them.
                (void)flushData(mSensor,
                          EVENT_TYPE_BIT_DISCARDABLE |
                          sensorGetMyEventType(mSensorInfo[ACC].sensorType));

                // Tries to allocate another data event and breaks if unsuccessful.
                if (!allocateDataEvt(mSensor, rtc_time)) {
                    return;
                }
            }
            mSensor->data_evt->samples[0].firstSample.biasCurrent = true;
            mSensor->data_evt->samples[0].firstSample.biasPresent = 1;
            mSensor->data_evt->samples[0].firstSample.biasSample =
                mSensor->data_evt->samples[0].firstSample.numSamples;
            sample = &mSensor->data_evt->
                samples[mSensor->data_evt->samples[0].firstSample.numSamples++];

            // Updates the accel offset in HAL.
            sample->x = accel_offset[0];
            sample->y = accel_offset[1];
            sample->z = accel_offset[2];

            (void)flushData(mSensor, sensorGetMyEventType(mSensorInfo[ACC].biasType));
            if (!allocateDataEvt(mSensor, rtc_time)) {
                return;
            }
        }
    }
#endif  // ACCEL_CAL_ENABLED
#ifdef GYRO_CAL_ENABLED
    if (mSensor->idx == GYR) {
      // GyroCal -- Checks for a new offset estimate update.
      float gyro_offset[3] = {0.0f, 0.0f, 0.0f};
      float gyro_offset_temperature_celsius = 0.0f;
      uint64_t calibration_time_nanos = 0;
      bool new_gyrocal_offset_update = gyroCalNewBiasAvailable(&mTask.gyro_cal);
      if (new_gyrocal_offset_update) {
        // GyroCal -- Gets the GyroCal offset estimate.
        gyroCalGetBias(&mTask.gyro_cal, &gyro_offset[0], &gyro_offset[1],
                       &gyro_offset[2], &gyro_offset_temperature_celsius,
                       &calibration_time_nanos);

#ifdef OVERTEMPCAL_ENABLED
        // OTC-Gyro Cal -- Sends a new GyroCal estimate to the OTC-Gyro.
        overTempCalUpdateSensorEstimate(&mTask.over_temp_gyro_cal, rtc_time,
                                        gyro_offset,
                                        gyro_offset_temperature_celsius);
#endif  // OVERTEMPCAL_ENABLED
      }

#ifdef OVERTEMPCAL_ENABLED
      // OTC-Gyro Cal --  Gets the latest OTC-Gyro temperature compensated
      // offset estimate.
      bool new_otc_offset_update =
          overTempCalNewOffsetAvailable(&mTask.over_temp_gyro_cal);
      overTempCalGetOffset(&mTask.over_temp_gyro_cal,
                           &gyro_offset_temperature_celsius, gyro_offset);

      // OTC-Gyro Cal --  Checks for a model update.
      bool new_otc_model_update =
          overTempCalNewModelUpdateAvailable(&mTask.over_temp_gyro_cal);

      if (new_otc_offset_update) {
#else   // OVERTEMPCAL_ENABLED
      if (new_gyrocal_offset_update) {
#endif  // OVERTEMPCAL_ENABLED
        if (mSensor->data_evt->samples[0].firstSample.numSamples > 0) {
          // flush existing samples so the bias appears after them.
          (void)flushData(mSensor,
                    EVENT_TYPE_BIT_DISCARDABLE |
                        sensorGetMyEventType(mSensorInfo[GYR].sensorType));
          if (!allocateDataEvt(mSensor, rtc_time)) {
            return;
          }
        }
        mSensor->data_evt->samples[0].firstSample.biasCurrent = true;
        mSensor->data_evt->samples[0].firstSample.biasPresent = 1;
        mSensor->data_evt->samples[0].firstSample.biasSample =
            mSensor->data_evt->samples[0].firstSample.numSamples;
        sample = &mSensor->data_evt->samples[mSensor->data_evt->samples[0]
                                                 .firstSample.numSamples++];
        // Updates the gyro offset in HAL.
        sample->x = gyro_offset[0];
        sample->y = gyro_offset[1];
        sample->z = gyro_offset[2];

        (void)flushData(mSensor, sensorGetMyEventType(mSensorInfo[GYR].biasType));
        if (!allocateDataEvt(mSensor, rtc_time)) {
          return;
        }
      }
#ifdef OVERTEMPCAL_ENABLED
      if (new_otc_model_update || new_otc_offset_update) {
        // Notify HAL to store new gyro OTC-Gyro data.
        T(otcGyroUpdateBuffer).sendToHostRequest = true;
      }
#endif  // OVERTEMPCAL_ENABLED
    }
#endif  // GYRO_CAL_ENABLED

    sample = &mSensor->data_evt->samples[mSensor->data_evt->samples[0].firstSample.numSamples++];

    // the first deltatime is for sample size
    if (mSensor->data_evt->samples[0].firstSample.numSamples > 1) {
        delta_time = rtc_time - mSensor->prev_rtc_time;
        delta_time = delta_time < 0 ? 0 : delta_time;
        sample->deltaTime = delta_time;
        mSensor->prev_rtc_time = rtc_time;
    }

    sample->x = x;
    sample->y = y;
    sample->z = z;

    //DEBUG_PRINT("bmi26x: x: %d, y: %d, z: %d\n", (int)(1000*x), (int)(1000*y), (int)(1000*z));

    //TODO: This was added to prevent to much data of the same type accumulate in internal buffer.
    //      It might no longer be necessary and can be removed.
    if (mSensor->data_evt->samples[0].firstSample.numSamples == MAX_NUM_COMMS_EVENT_SAMPLES) {
        flushAllData();
    }

}

static void dispatchData(void)
{
    size_t i = 2, j;
    size_t size = mTask.xferCnt;
    int fh_mode, fh_param;
    uint8_t *buf = mTask.dataBuffer;

    uint64_t min_delta = ULONG_LONG_MAX;
    uint32_t sensor_time24;
    uint64_t full_sensor_time;
    uint64_t frame_sensor_time = mTask.frame_sensortime;
    bool observed[NUM_CONT_SENSOR];
    uint64_t tmp_frame_time, tmp_time[NUM_CONT_SENSOR];
    bool frame_sensor_time_valid = mTask.frame_sensortime_valid;
    bool saved_pending_delta[NUM_CONT_SENSOR] = {0};
    uint64_t saved_time_delta[NUM_CONT_SENSOR] = {0};
#if TIMESTAMP_DBG
    int frame_num = -1;
#endif

    for (j = FIRST_CONT_SENSOR; j < NUM_CONT_SENSOR; j++)
        observed[j] = false;

    if (!mTask.frame_sensortime_valid) {
        // This is the first FIFO delivery after any sensor is enabled in
        // bmi26x. Sensor time reference is not establised until end of this
        // FIFO frame. Assume time start from zero and do a dry run to estimate
        // the time and then go through this FIFO again.
        frame_sensor_time = 0ull;

        // Save these states for future recovery by the end of dry run.
        for (j = FIRST_CONT_SENSOR; j < NUM_CONT_SENSOR; j++) {
            saved_pending_delta[j] = mTask.pending_delta[j];
            saved_time_delta[j] = mTask.time_delta[j];
        }
    }

    while (size > 0) {
        if (buf[i] == BMI26X_FRAME_HEADER_INVALID) {
            // reaching invalid header means no more data
            break;
        } else if (buf[i] == BMI26X_FRAME_HEADER_SKIP) {
            // manually injected skip header
            DEBUG_PRINT_IF(DBG_CHUNKED, "skip nop header");
            i++;
            size--;
            continue;
        }

        fh_mode = buf[i] >> 6;
        fh_param = (buf[i] >> 2) & 0xf;

        i++;
        size--;
#if TIMESTAMP_DBG
        ++frame_num;
#endif

        if (fh_mode == 1) {
            // control frame.
            if (fh_param == 0) {
                // skip frame, we skip it
                if (size >= 1) {
                    i++;
                    size--;
                } else {
                    size = 0;
                }
            } else if (fh_param == 1) {
                // sensortime frame
                if (size >= 3) {
                    // The active sensor with the highest odr/lowest delta is the one that
                    // determines the sensor time increments.
                    for (j = FIRST_CONT_SENSOR; j < NUM_CONT_SENSOR; j++) {
                        if (mTask.sensors[j].configed &&
                                mTask.sensors[j].latency != SENSOR_LATENCY_NODATA) {
                            min_delta = min_delta < mTask.time_delta[j] ? min_delta :
                                    mTask.time_delta[j];
                        }
                    }
                    sensor_time24 = buf[i + 2] << 16 | buf[i + 1] << 8 | buf[i];

                    // clear lower bits that measure time from taking the sample to reading the
                    // FIFO, something we're not interested in.
                    sensor_time24 &= ~(min_delta - 1);

                    full_sensor_time = parseSensortime(sensor_time24);

#if TIMESTAMP_DBG
                    if (frame_sensor_time == full_sensor_time) {
                        //DEBUG_PRINT("frame %d FrameTime 0x%08x\n",
                        //        frame_num - 1,
                        //        (unsigned int)frame_sensor_time);
                    } else if (frame_sensor_time_valid) {
                        DEBUG_PRINT("frame %d FrameTime 0x%08x != SensorTime 0x%08x, jumped %d msec\n",
                                frame_num - 1,
                                (unsigned int)frame_sensor_time,
                                (unsigned int)full_sensor_time,
                                (int)(5 * ((int64_t)(full_sensor_time - frame_sensor_time) >> 7)));
                    }
#endif


                    if (frame_sensor_time_valid) {
                        mTask.frame_sensortime = full_sensor_time;
                    } else {
                        // Dry run if frame_sensortime_valid == false,
                        // no sample is added this round.
                        // So let's time travel back to beginning of frame.
                        mTask.frame_sensortime_valid = true;
                        mTask.frame_sensortime = full_sensor_time - frame_sensor_time;

                        // recover states
                        for (j = FIRST_CONT_SENSOR; j < NUM_CONT_SENSOR; j++) {
                            // reset all prev_frame_time to invalid values
                            // they should be so anyway at the first FIFO
                            mTask.prev_frame_time[j] = ULONG_LONG_MAX;

                            // recover saved time_delta and pending_delta values
                            mTask.pending_delta[j] = saved_pending_delta[j];
                            mTask.time_delta[j] = saved_time_delta[j];
                        }

                        DEBUG_PRINT_IF(TIMESTAMP_DBG,
                                "sensortime invalid: full, frame, task = %llu, %llu, %llu\n",
                                full_sensor_time,
                                frame_sensor_time,
                                mTask.frame_sensortime);

                        // Parse again with known valid timing.
                        // This time the sensor events will be committed into event buffer.
                        dispatchData();
						return;
                    }

                    // Invalidate sensor timestamp that didn't get corrected by full_sensor_time,
                    // so it can't be used as a reference at next FIFO read.
                    // Use (ULONG_LONG_MAX - 1) to indicate this.
                    for (j = FIRST_CONT_SENSOR; j < NUM_CONT_SENSOR; j++) {
                        mTask.prev_frame_time[j] = observed[j] ? full_sensor_time : (ULONG_LONG_MAX - 1);

                        // sensor can be disabled in the middle of the FIFO, but wait till the FIFO
                        // end to invalidate prev_frame_time since it's still needed for parsing.
                        // Also invalidate pending delta just to be safe.
                        if (!mTask.sensors[j].configed ||
                                mTask.sensors[j].latency == SENSOR_LATENCY_NODATA) {
                            mTask.prev_frame_time[j] = ULONG_LONG_MAX;
                            mTask.pending_delta[j] = false;
                        }
                    }
                    i += 3;
                    size -= 3;
                } else {
                    size = 0;
                }
            } else if (fh_param == 2) {
                // fifo_input config frame
#if TIMESTAMP_DBG
                DEBUG_PRINT("frame %d config change 0x%02x\n", frame_num, buf[i]);
#endif
                if (size >= 4) {
                    for (j = FIRST_CONT_SENSOR; j < NUM_CONT_SENSOR; j++) {
                        if (buf[i] & (0x01 << (j << 1)) && mTask.pending_delta[j]) {
                            mTask.pending_delta[j] = false;
                            mTask.time_delta[j] = mTask.next_delta[j];
#if TIMESTAMP_DBG
                            DEBUG_PRINT("%s new delta %u\n", mSensorInfo[j].sensorName,
                                    (unsigned int)mTask.time_delta[j]);
#endif
                        }
                    }
                    i = i + 4;
                    size -= 4;
                } else {
                    size = 0;
                }
            } else {
                size = 0; // drop this batch
                ERROR_PRINT("Invalid fh_param in control frame\n");
            }
        } else if (fh_mode == 2) {
            // Calcutate candidate frame time (tmp_frame_time):
            // 1) When sensor is first enabled, reference from other sensors if possible.
            // Otherwise, add the smallest increment to the previous data frame time.
            // 2) The newly enabled sensor could only underestimate its
            // frame time without reference from other sensors.
            // 3) The underestimated frame time of a newly enabled sensor will be corrected
            // as soon as it shows up in the same frame with another sensor.
            // 4) (prev_frame_time == ULONG_LONG_MAX) means the sensor wasn't enabled.
            // 5) (prev_frame_time == ULONG_LONG_MAX -1) means the sensor didn't appear in the last
            // data frame of the previous fifo read.  So it won't be used as a frame time reference.

            tmp_frame_time = 0;
            for (j = FIRST_CONT_SENSOR; j < NUM_CONT_SENSOR; j++) {
                observed[j] = false; // reset at each data frame
                tmp_time[j] = 0;
                if ((mTask.prev_frame_time[j] < ULONG_LONG_MAX - 1) && (fh_param & (1 << j))) {
                    tmp_time[j] = mTask.prev_frame_time[j] + mTask.time_delta[j];
                    tmp_frame_time = (tmp_time[j] > tmp_frame_time) ? tmp_time[j] : tmp_frame_time;
                }
            }
            tmp_frame_time = (frame_sensor_time + kMinSensorTimeIncrement > tmp_frame_time)
                ? (frame_sensor_time + kMinSensorTimeIncrement) : tmp_frame_time;

            // regular frame, dispatch data to each sensor's own fifo
            if (fh_param & 4) { // have mag data
                if (size >= 8) {
                    if (frame_sensor_time_valid)
                    i += 8;
                    size -= 8;
                } else {
                    size = 0;
                }
            }
            if (fh_param & 2) { // have gyro data
                if (size >= 6) {
                    if (frame_sensor_time_valid) {
                        parseRawData(&mTask.sensors[GYR], &buf[i], kScale_gyr, tmp_frame_time);
#if TIMESTAMP_DBG
                        if (mTask.prev_frame_time[GYR] == ULONG_LONG_MAX) {
                            DEBUG_PRINT("gyr enabled: frame %d time 0x%08x\n",
                                    frame_num, (unsigned int)tmp_frame_time);
                        } else if ((tmp_frame_time != tmp_time[GYR]) && (tmp_time[GYR] != 0)) {
                            DEBUG_PRINT("frame %d gyr time: 0x%08x -> 0x%08x, jumped %d msec\n",
                                    frame_num,
                                    (unsigned int)tmp_time[GYR],
                                    (unsigned int)tmp_frame_time,
                                    (int)(5 * ((int64_t)(tmp_frame_time - tmp_time[GYR]) >> 7)));
                        }
#endif
                    }
                    mTask.prev_frame_time[GYR] = tmp_frame_time;
                    i += 6;
                    size -= 6;
                    observed[GYR] = true;
                } else {
                    size = 0;
                }
            }
            if (fh_param & 1) { // have accel data
                if (size >= 6) {
                    if (frame_sensor_time_valid) {
                        parseRawData(&mTask.sensors[ACC], &buf[i], kScale_acc, tmp_frame_time);
#if TIMESTAMP_DBG
                        if (mTask.prev_frame_time[ACC] == ULONG_LONG_MAX) {
                            DEBUG_PRINT("acc enabled: frame %d time 0x%08x\n",
                                    frame_num, (unsigned int)tmp_frame_time);
                        } else if ((tmp_frame_time != tmp_time[ACC]) && (tmp_time[ACC] != 0)) {
                            DEBUG_PRINT("frame %d gyr time: 0x%08x -> 0x%08x, jumped %d msec\n",
                                    frame_num,
                                    (unsigned int)tmp_time[ACC],
                                    (unsigned int)tmp_frame_time,
                                    (int)(5 * ((int64_t)(tmp_frame_time - tmp_time[ACC]) >> 7)));
                        }
#endif
                    }
                    mTask.prev_frame_time[ACC] = tmp_frame_time;
                    i += 6;
                    size -= 6;
                    observed[ACC] = true;
                } else {
                    size = 0;
                }
            }

            if (observed[ACC] || observed[GYR])
                frame_sensor_time = tmp_frame_time;
        } else {
            size = 0; // drop this batch
            ERROR_PRINT("Invalid fh_mode %d at 0x%x, data dump:\n", fh_mode, i);
            // dump (a) bytes back and (b) bytes forward.
            int a = i < 0x80 ? 0 : (i - 0x80) & ~0x0F;
            int b = ((i + 0x80 > mTask.xferCnt ? mTask.xferCnt : i + 0x80) + 0x0F) & ~0x0F;
            dumpBinary(mTask.dataBuffer, a, b - a);
        }
    }

    //flush data events.
    flushAllData();
}

/*
 * Read the interrupt type and send corresponding event
 * If it's anymo or double tap, also send a single uint32 to indicate which axies
 * is this interrupt triggered.
 * If it's flat, also send a bit to indicate flat/non-flat position.
 * If it's step detector, check if we need to send the total step count.
 */
static void int2Handling(void)
{
    TDECL();
#if !defined(NO_NEED_ANYMOTION_FOR_VIVO)
    union EmbeddedDataPoint trigger_axies;
    uint8_t int_status_0 = mTask.statusBuffer[2];
    if ((int_status_0 & INT_ANY_MOTION) && mTask.sensors[ANYMO].powered) {
        // bit [0:2] of INT_STATUS[2] is set when anymo is triggered by x, y or
        // z axies respectively. bit [3] indicates the slope.
        trigger_axies.idata = (mTask.statusBuffer[3] & 0x0f);
        DEBUG_PRINT("Detected any motion\n");
        (void)osEnqueueEvt(EVT_SENSOR_ANY_MOTION, trigger_axies.vptr, NULL);
    }
#endif

    return;
}

static void int2Evt(void)
{
    TDECL();
    if (trySwitchState(SENSOR_INT_2_HANDLING)) {
        // Read the interrupt reg value to determine what interrupts
        SPI_READ(BMI2_INT_STATUS_0_ADDR, 3, &mTask.statusBuffer);
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask, __FUNCTION__);
    } else {
        // even if we are still in SENSOR_INT_2_HANDLING, the SPI may already finished and we need
        // to issue another SPI read to get the latest status
        mTask.pending_int[1] = true;
    }
}

// bits[6:7] in OFFSET[6] to enable/disable gyro/accel offset.
// bits[0:5] in OFFSET[6] stores the most significant 2 bits of gyro offset at
// its x, y, z axies.
// Calculate the stored gyro offset and compose it with the intended
// enable/disable mode for gyro/accel offset to determine the value for
// OFFSET[6].
/*
static uint8_t offset6Mode(void)
{
    uint8_t mode = 0;
    if (mTask.sensors[GYR].offset_enable)
        mode |= 0x01 << 7;
    if (mTask.sensors[ACC].offset_enable)
        mode |= 0x01 << 6;
    mode |= (mTask.sensors[GYR].offset[2] & 0x0300) >> 4;
    mode |= (mTask.sensors[GYR].offset[1] & 0x0300) >> 6;
    mode |= (mTask.sensors[GYR].offset[0] & 0x0300) >> 8;
    DEBUG_PRINT("OFFSET_6_MODE is: %02x\n", mode);
    return mode;
}
*/
static bool saveCalibration()
{
    TDECL();
    if (trySwitchState(SENSOR_SAVE_CALIBRATION)) {
        if (mTask.sensors[ACC].offset_enable) {
           // SPI_WRITE(BMI2_ACC_OFF_COMP_0_ADDR, mTask.sensors[ACC].offset[0] & 0xFF, 450);
            //SPI_WRITE(BMI2_ACC_OFF_COMP_0_ADDR + 1, mTask.sensors[ACC].offset[1] & 0xFF, 450);
            //SPI_WRITE(BMI2_ACC_OFF_COMP_0_ADDR + 2, mTask.sensors[ACC].offset[2] & 0xFF, 450);
        }
        if (mTask.sensors[GYR].offset_enable) {
            //SPI_WRITE(BMI2_GYR_OFF_COMP_3_ADDR, mTask.sensors[GYR].offset[0] & 0xFF, 450);
            //SPI_WRITE(BMI2_GYR_OFF_COMP_3_ADDR + 1, mTask.sensors[GYR].offset[1] & 0xFF, 450);
            //SPI_WRITE(BMI2_GYR_OFF_COMP_3_ADDR + 2, mTask.sensors[GYR].offset[2] & 0xFF, 450);
        }
        //SPI_WRITE(BMI2_GYR_OFF_COMP_6_ADDR, offset6Mode(), 450);
        SPI_READ(BMI2_ACC_OFF_COMP_0_ADDR, 7, &mTask.dataBuffer);
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, NULL, __FUNCTION__);
        return true;
    } else {
        DEBUG_PRINT("%s, state != IDLE", __FUNCTION__);
        return false;
    }
}

static void sendCalibrationResult(uint8_t status, uint8_t sensorType,
        int32_t xBias, int32_t yBias, int32_t zBias) {
    struct CalibrationData *data = heapAlloc(sizeof(struct CalibrationData));
    if (!data) {
        osLog(LOG_WARN, "Couldn't alloc cal result pkt");
        return;
    }

    data->header.appId = BMI26X_APP_ID;
    data->header.dataLen = (sizeof(struct CalibrationData) - sizeof(struct HostHubRawPacket));
    data->data_header.msgId = SENSOR_APP_MSG_ID_CAL_RESULT;
    data->data_header.sensorType = sensorType;
    data->data_header.status = status;

    data->xBias = xBias;
    data->yBias = yBias;
    data->zBias = zBias;

    if (!osEnqueueEvtOrFree(EVT_APP_TO_HOST, data, heapFree))
        osLog(LOG_WARN, "Couldn't send cal result evt");
}

/*! @name Structure to store accelerometer data deviation from ideal value */
struct offset_delta
{
    /*! X axis */
    int16_t x;
    /*! Y axis */
    int16_t y;
    /*! Z axis */
    int16_t z;
};

/*! @name Structure to store accelerometer offset values */
struct accel_offset
{
    /*! offset X  data */
    uint8_t x;

    /*! offset Y  data */
    uint8_t y;

    /*! offset Z  data */
    uint8_t z;
};

/*!
 * @brief This internal API is used to calculate the power of a value.
 */
static int32_t power(int16_t base, uint8_t resolution)
{
    /* Initialize loop */
    uint8_t loop = 1;

    /* Initialize variable to store the power of 2 value */
    int32_t value = 1;

    for (; loop <= resolution; loop++)
    {
        value = (int32_t)(value * base);
    }

    return value;
}

/*!
 * @brief This internal API compensate the accelerometer data against gravity.
 */
static void comp_for_gravity(uint16_t lsb_per_g,
                             int32_t data[3],
                             struct offset_delta *comp_data)
{
    /* Array to store the accelerometer values in LSB */
    int16_t accel_value_lsb[3] = { 0 };

    /* Convert g-value to LSB */
    accel_value_lsb[BMI2_X_AXIS] = (int16_t)(lsb_per_g);
    accel_value_lsb[BMI2_Y_AXIS] = (int16_t)(lsb_per_g);
    accel_value_lsb[BMI2_Z_AXIS] = (int16_t)(lsb_per_g);

    /* Get the compensated values for X, Y and Z axis */
    comp_data->x = (data[0] - accel_value_lsb[BMI2_X_AXIS]);
    comp_data->y = (data[1] - accel_value_lsb[BMI2_Y_AXIS]);
    comp_data->z = (data[2] - accel_value_lsb[BMI2_Z_AXIS]);
}

/*!
 * @brief This internal API finds the bit position of 3.9mg according to given
 * range and resolution.
 */
static int8_t get_bit_pos_3_9mg(uint8_t range)
{
    /* Variable to store the bit position of 3.9mg resolution*/
    int8_t bit_pos_3_9mg;

    /* Variable to shift the bits according to the resolution  */
    uint32_t divisor = 1;

    /* Scaling factor to get the bit position of 3.9 mg resolution */
    int16_t scale_factor = -1;

    /* Variable to store temporary value */
    uint16_t temp;

    /* Shift left by the times of resolution */
    divisor = divisor << 16;

    /* Get the bit position to be shifted */
    temp = (uint16_t)(divisor / (range * 256));

    /* Get the scaling factor until bit position is shifted to last bit */
    while (temp != 1)
    {
        scale_factor++;
        temp = temp >> 1;
    }

    /* Scaling factor is the bit position of 3.9 mg resolution */
    bit_pos_3_9mg = (int8_t) scale_factor;

    return bit_pos_3_9mg;
}


/*!
 * @brief This internal API scales the compensated accelerometer data according
 * to the offset register resolution.
 *
 * @note The bit position is always greater than 0 since accelerometer data is
 * 16 bit wide.
 */
static void scale_accel_offset(uint8_t range, const struct offset_delta *comp_data, struct accel_offset *data)
{
    /* Variable to store the position of bit having 3.9mg resolution */
    int8_t bit_pos_3_9mg;

    /* Variable to store the position previous of bit having 3.9mg resolution */
    int8_t bit_pos_3_9mg_prev_bit;

    /* Variable to store the round-off value */
    uint8_t round_off;

    /* Find the bit position of 3.9mg */
    bit_pos_3_9mg = get_bit_pos_3_9mg(range);

    /* Round off, consider if the next bit is high */
    bit_pos_3_9mg_prev_bit = bit_pos_3_9mg - 1;
    round_off = (uint8_t)(power(2, ((uint8_t) bit_pos_3_9mg_prev_bit)));

    /* Scale according to offset register resolution */
    data->x = (uint8_t)((comp_data->x + round_off) / power(2, ((uint8_t) bit_pos_3_9mg)));
    data->y = (uint8_t)((comp_data->y + round_off) / power(2, ((uint8_t) bit_pos_3_9mg)));
    data->z = (uint8_t)((comp_data->z + round_off) / power(2, ((uint8_t) bit_pos_3_9mg)));
}

/*!
 * @brief This internal API inverts the accelerometer offset data.
 */
static void invert_accel_offset(struct accel_offset *offset_data)
{
    /* Get the offset data */
    offset_data->x = (uint8_t)((offset_data->x) * (-1));
    offset_data->y = (uint8_t)((offset_data->y) * (-1));
    offset_data->z = (uint8_t)((offset_data->z) * (-1));
}


static void accCalibrationHandling(void)
{
    TDECL();
    /* Variable to define LSB per g value */
    uint16_t lsb_per_g = 0;
    /* Variable to define range */
    uint8_t range = 8;
    uint8_t resolution = 16;
    /* Structure to store accelerometer data deviation from ideal value */
    struct offset_delta delta = { 0, 0, 0 };
    /* Structure to store accelerometer offset values */
    struct accel_offset offset = { 0, 0, 0 };
    int16_t x = 0;
    int16_t y = 0;
    int16_t z = 0;
    uint8_t reg59_val = 0;
    switch (mTask.calibration_state) {
    case CALIBRATION_START:
        T(mRetryLeft) = RETRY_CNT_CALIBRATION;
        // disable the fifo function
        SPI_WRITE(BMI2_FIFO_CONFIG_1_ADDR, 0x10, 450);/*enable the fifo header */
        /* Set accelerometer configurations to 50Hz, continuous mode, CIC mode */
        SPI_WRITE(BMI2_ACC_CONF_ADDR, 0xB7, 1000);
        /* set range for accel (+-8g)*/
        SPI_WRITE(BMI2_ACC_CONF_ADDR + 1, 0x02, 450);
        // turn ACC to NORMAL mode
        SPI_WRITE(BMI2_PWR_CTRL_ADDR, 0x04, 50000);
        SPI_READ(BMI2_ACC_OFF_COMP_0_ADDR, 4, &mTask.statusBuffer);
        mTask.calibration_state = CALIBRATION_FOC_PRE;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[ACC], __FUNCTION__);
        break;
     case CALIBRATION_FOC_PRE:
        ERROR_PRINT("dump acc offset, %x %x %x\n", mTask.statusBuffer[2], mTask.statusBuffer[3], mTask.statusBuffer[4]);
        mTask.calibration_state = CALIBRATION_FOC;
        SPI_READ(BMI2_ACC_OFF_COMP_0_ADDR, 4, &mTask.statusBuffer);
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[ACC], __FUNCTION__);
        break;
    case CALIBRATION_FOC:
        // poll get the acc data for 128 frame
        SPI_READ(BMI2_ACC_X_LSB_ADDR, 7, &mTask.statusBuffer, 50000);
        mTask.calibration_state = CALIBRATION_WAIT_FOC_DONE;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[ACC], __FUNCTION__);
        break;
    case CALIBRATION_WAIT_FOC_DONE:
        // if the STATUS REG has bit 3 set, it means calbration is done.
        // otherwise, check back in 50ms later.
        ERROR_PRINT("dump acc data read time = %d\n", T(acc_foc_read));
        if (T(acc_foc_read) < 128) {
            x = (mTask.statusBuffer[2] | mTask.statusBuffer[3]<<8);
            y = (mTask.statusBuffer[4] | mTask.statusBuffer[5]<<8);
            z = (mTask.statusBuffer[6] | mTask.statusBuffer[7]<<8);
            ERROR_PRINT("dump acc data, %d %d %d \n", x, y,z);
            /*acc x */
            mTask.accel_value[0] =x + mTask.accel_value[0];
            /*acc y */
            mTask.accel_value[1] =y + mTask.accel_value[1];
            /*acc x */
            mTask.accel_value[2] =z + mTask.accel_value[2];
            // poll get the acc data for 128 frame
            SPI_READ(BMI2_ACC_X_LSB_ADDR, BMI2_ACC_GYR_NUM_BYTES, &mTask.statusBuffer);
            mTask.calibration_state = CALIBRATION_FOC;
            T(acc_foc_read) =T(acc_foc_read) + 1;
            ERROR_PRINT("acc data, %ld %ld %ld read time = %d\n",
                mTask.accel_value[0], mTask.accel_value[1], mTask.accel_value[2], T(acc_foc_read));
        } else {
           T(acc_foc_read) = 0;
            // get nv config register
            SPI_READ(BMI2_NV_CONF_ADDR, 2, &mTask.statusBuffer);
            mTask.accel_avg[0] = mTask.accel_value[0] / 128;
            mTask.accel_avg[1] = mTask.accel_value[1] / 128;
            mTask.accel_avg[2] = mTask.accel_value[2] / 128;
            ERROR_PRINT("acc avg data, %ld %ld %ld \n", mTask.accel_avg[0],  mTask.accel_avg[1],  mTask.accel_avg[2]);
            mTask.accel_value[0] = mTask.accel_value[1] = mTask.accel_value[2] = 0;
            /* Get the smallest possible measurable acceleration level given the range and
             * resolution */
            lsb_per_g = (uint16_t)(power(2, resolution) / (2 * range));
            /* Compensate acceleration data against gravity */
            comp_for_gravity(lsb_per_g, mTask.accel_avg, &delta);
            ERROR_PRINT("acc delta data, %d %d %d \n", delta.x, delta.y, delta.z);
            /* Scale according to offset register resolution */
             scale_accel_offset(range, &delta, &offset);
            ERROR_PRINT("acc offset data, %x %x %x \n", offset.x, offset.y, offset.z);
            /* Invert the accelerometer offset data */
            invert_accel_offset(&offset);
            mTask.sensors[ACC].offset[0] = offset.x;
            mTask.sensors[ACC].offset[1] = offset.y;
            mTask.sensors[ACC].offset[2] = offset.z;
            /* read 0x59 value */
            SPI_READ(BMI2_INIT_CTRL_ADDR, 2, &mTask.reg59_val);
            mTask.calibration_state = CALIBRATION_SET_OFFSET;
            ERROR_PRINT("FOC set FINISHED!\n");
        }
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[ACC], __FUNCTION__);
        break;
    case CALIBRATION_SET_OFFSET:
        ERROR_PRINT("ACCELERATION OFFSET is %02x  %02x  %02x\n",
                (unsigned int)mTask.sensors[ACC].offset[0],
                (unsigned int)mTask.sensors[ACC].offset[1],
                (unsigned int)mTask.sensors[ACC].offset[2]);
        /* Offset values are written in the offset register */
        SPI_WRITE(BMI2_ACC_OFF_COMP_0_ADDR, mTask.sensors[ACC].offset[0], 1000);
        SPI_WRITE(BMI2_ACC_OFF_COMP_0_ADDR + 1, mTask.sensors[ACC].offset[1], 1000);
        SPI_WRITE(BMI2_ACC_OFF_COMP_0_ADDR + 2, mTask.sensors[ACC].offset[2], 1000);
        sendCalibrationResult(SENSOR_APP_EVT_STATUS_SUCCESS, SENS_TYPE_ACCEL,
                mTask.sensors[ACC].offset[0], mTask.sensors[ACC].offset[1],
                mTask.sensors[ACC].offset[2]);
        // Enable offset compensation for accel
        uint8_t mode = mTask.statusBuffer[2];
        mode = BMI2_SET_BITS(mode, BMI2_NV_ACC_OFFSET, 1);
        SPI_WRITE(BMI2_NV_CONF_ADDR, mode, 1000);
        reg59_val = mTask.reg59_val[2];
        mTask.saved_reg59_val = mTask.reg59_val[2];
        /* set 0x59 bit 0 = 0 */
        reg59_val = BST_CLR_VAL_BIT(reg59_val, 0);
        SPI_WRITE(BMI2_INIT_CTRL_ADDR, reg59_val, 1000);
        /* enable nvm */
        SPI_WRITE(BMI2_NVM_CONF_ADDR, 0x02, 1000);
        /* write cmd */
        SPI_WRITE(BMI2_CMD_REG_ADDR, BMI2_NVM_PROG_CMD, 2000);
         /* restore 0x59 */
        SPI_WRITE(BMI2_INIT_CTRL_ADDR, mTask.saved_reg59_val, 1000);
        SPI_READ(BMI2_ACC_OFF_COMP_0_ADDR, 4, &mTask.statusBuffer);
        mTask.calibration_state = CALIBRATION_CHECK;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[ACC], __FUNCTION__);
        break;
     case CALIBRATION_CHECK:
        ERROR_PRINT("ACCELERATION OFFSET is %02x  %02x  %02x\n",
                (unsigned int)mTask.statusBuffer[2],
                (unsigned int)mTask.statusBuffer[3],
                (unsigned int)mTask.statusBuffer[4]);
        mTask.calibration_state = CALIBRATION_DONE;
        // turn ACC to off mode
        SPI_WRITE(BMI2_PWR_CTRL_ADDR, 0x00, 5000);
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[ACC], __FUNCTION__);
        break;
    default:
        ERROR_PRINT("Invalid calibration state\n");
        break;
    }
}



static bool accCalibration(void *cookie)
{
    TDECL();
    if (!mTask.sensors[ACC].powered && trySwitchState(SENSOR_CALIBRATING)) {
        mTask.calibration_state = CALIBRATION_START;
        accCalibrationHandling();
        return true;
    } else {
        ERROR_PRINT("cannot calibrate accel because sensor is busy\n");
        sendCalibrationResult(SENSOR_APP_EVT_STATUS_BUSY, SENS_TYPE_ACCEL, 0, 0, 0);
        return false;
    }
}

static bool accCfgData(void *data, void *cookie)
{
    struct CfgData {
        int32_t hw[3];
        float sw[3];
    };
    struct CfgData *values = data;

    mTask.sensors[ACC].offset[0] = values->hw[0];
    mTask.sensors[ACC].offset[1] = values->hw[1];
    mTask.sensors[ACC].offset[2] = values->hw[2];
    mTask.sensors[ACC].offset_enable = true;

#ifdef ACCEL_CAL_ENABLED
    accelCalBiasSet(&mTask.acc, values->sw[0], values->sw[1], values->sw[2]);
#endif

    INFO_PRINT("accCfgData: data=%02lx, %02lx, %02lx\n",
            values->hw[0] & 0xFF, values->hw[1] & 0xFF, values->hw[2] & 0xFF);

    if (!saveCalibration()) {
        mTask.pending_calibration_save = true;
    }

    return true;
}

static void sendTestResult(uint8_t status, uint8_t sensorType) {
    struct TestResultData *data = heapAlloc(sizeof(struct TestResultData));
    if (!data) {
        osLog(LOG_WARN, "Couldn't alloc test result packet");
        return;
    }

    data->header.appId = BMI26X_APP_ID;
    data->header.dataLen = (sizeof(struct TestResultData) - sizeof(struct HostHubRawPacket));
    data->data_header.msgId = SENSOR_APP_MSG_ID_TEST_RESULT;
    data->data_header.sensorType = sensorType;
    data->data_header.status = status;

    if (!osEnqueueEvtOrFree(EVT_APP_TO_HOST, data, heapFree))
        osLog(LOG_WARN, "Couldn't send test result packet");
}

static void initRegisterVal(void)
{
    TDECL();
    /* disable accel, gyro and mag data in FIFO, enable header, enable fifo time */
    SPI_WRITE(BMI2_FIFO_CONFIG_1_ADDR, 0x10, 450);/*enable the fifo header */
    SPI_WRITE(BMI2_FIFO_CONFIG_0_ADDR, 0x02, 450);/*enable the fifo time */
    /* set the watermark to 13 byte */
    SPI_WRITE(BMI2_FIFO_WTM_0_ADDR, 0x18, 450);
    SPI_WRITE(BMI2_FIFO_WTM_1_ADDR, 0, 450);
    /* FIFO watermark and fifo_full interrupt enabled */
    SPI_WRITE(BMI2_INT_MAP_DATA_ADDR, 0x03, 450);
    /* INT1 enabled, high-level (push-pull) triggered */
    SPI_WRITE(BMI2_INT1_IO_CTRL_ADDR, 0x0a, 1000);
    SPI_WRITE(BMI2_INT2_IO_CTRL_ADDR, 0x0a, 1000);
    /* no Map all interrupts to INT1 and INT2 */
    SPI_WRITE(BMI2_INT1_MAP_FEAT_ADDR, 0x00, 450);
    SPI_WRITE(BMI2_INT2_MAP_FEAT_ADDR, 0x00, 450);
    /* initial range for accel (+-8g) and gyro (+-1000 degree) */
    SPI_WRITE(BMI2_ACC_CONF_ADDR + 1, 0x02, 450);
    SPI_WRITE(BMI2_GYR_CONF_ADDR + 1, 0x01, 450);
    /* disable the advance power config */
    SPI_WRITE(BMI2_PWR_CONF_ADDR, 0x00, 1000);
    /* disable the config loading */
    SPI_WRITE(BMI2_INIT_CTRL_ADDR, 0x00, 1000);
}
static void restoreRegisterVal(void)
{
    TDECL();
    /*restore the register value before selftest*/
    SPI_WRITE(BMI2_ACC_CONF_ADDR, mTask.reg_val[2], 450); //reg40
    SPI_WRITE((BMI2_ACC_CONF_ADDR + 1), mTask.reg_val[3], 450); //reg41
    SPI_WRITE(BMI2_GYR_CONF_ADDR, mTask.reg_val[4], 450); //reg42
    SPI_WRITE((BMI2_GYR_CONF_ADDR + 1), mTask.reg_val[5], 450); //reg43
    SPI_WRITE((BMI2_FIFO_DOWNS_ADDR), mTask.reg_val[7], 450); //reg45
    SPI_WRITE((BMI2_FIFO_WTM_0_ADDR), mTask.reg_val[8], 450); //reg46
    SPI_WRITE((BMI2_FIFO_WTM_1_ADDR), mTask.reg_val[9], 450); //reg47
    SPI_WRITE((BMI2_FIFO_CONFIG_0_ADDR), mTask.reg_val[10], 450); //reg48
    SPI_WRITE((BMI2_FIFO_CONFIG_1_ADDR), mTask.reg_val[11], 450); //reg49
    SPI_WRITE((BMI2_INT1_MAP_FEAT_ADDR), mTask.reg_val[12], 450); //reg56
    SPI_WRITE((BMI2_INT2_MAP_FEAT_ADDR), mTask.reg_val[13], 450); //reg57
    SPI_WRITE((BMI2_INT_MAP_DATA_ADDR), mTask.reg_val[14], 450); //reg58
    SPI_WRITE((BMI2_PWR_CTRL_ADDR), mTask.reg_val[16], 450); //reg7d
}
static void accTestHandling(void)
{
    static const int32_t kMinDifferenceX = 32768;
    static const int32_t kMinDifferenceY = 30720;
    static const int32_t kMinDifferenceZ =20480;

    int32_t tempTestX, tempTestY, tempTestZ;
    int32_t absDiffX, absDiffY, absDiffZ;
    uint16_t config_index = 0;
    uint8_t page = 0;
    uint8_t config_stream_status = 0;
    TDECL();

    switch (mTask.acc_test_state) {
    case ACC_TEST_START:
        ERROR_PRINT("ACC_TEST_START\n");
        /*backup the the all config*/
        SPI_READ(BMI2_ACC_CONF_ADDR, 11, &mTask.reg40_49);
        SPI_READ(BMI2_INT1_MAP_FEAT_ADDR, 4, &mTask.reg56_58);
        SPI_READ(BMI2_PWR_CONF_ADDR, 3, &mTask.reg7c_7d);
        /*disable the acc and gyro fifo enable*/
        SPI_WRITE(BMI2_FIFO_CONFIG_1_ADDR, 0x10, 5000);
        /* set acc power */
        SPI_WRITE(BMI2_PWR_CTRL_ADDR, 0x04, 5000);
        mTask.acc_test_state = ACC_TEST_CONFIG;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[ACC], __FUNCTION__);
        break;

    case ACC_TEST_CONFIG:
        ERROR_PRINT("ACC_TEST_CONFIG\n");
        ERROR_PRINT("dump reg addr:40-49, %x %x %x %x %x %x %x %x %x %x\n",
            mTask.reg40_49[2], mTask.reg40_49[3], mTask.reg40_49[4], mTask.reg40_49[5], mTask.reg40_49[6],
            mTask.reg40_49[7], mTask.reg40_49[8], mTask.reg40_49[9], mTask.reg40_49[10], mTask.reg40_49[11]);
        ERROR_PRINT("dump reg addr:56-58, 0x%x 0x%x 0x%x\n",
            mTask.reg56_58[2], mTask.reg56_58[3], mTask.reg56_58[4]);
        memcpy(&mTask.reg_val[2], &mTask.reg40_49[2], 10);
        ERROR_PRINT("reg40-49 %x %x %x %x %x %x %x %x %x %x\n",
        mTask.reg_val[2], mTask.reg_val[3], mTask.reg_val[4], mTask.reg_val[5], mTask.reg_val[6],
        mTask.reg_val[7], mTask.reg_val[8], mTask.reg_val[9], mTask.reg_val[10], mTask.reg_val[11]);
        memcpy(&mTask.reg_val[12], &mTask.reg56_58[2], 3);
        memcpy(&mTask.reg_val[15], &mTask.reg7c_7d[2], 2);
        ERROR_PRINT("reg56-58 7c7d %x %x %x %x %x\n",
        mTask.reg_val[12], mTask.reg_val[13], mTask.reg_val[14], mTask.reg_val[15], mTask.reg_val[16]);
         /*disable the adv power mode */
        SPI_WRITE(BMI2_PWR_CONF_ADDR, 0x00, 3000);
        /* set acc config */
        SPI_WRITE(BMI2_ACC_CONF_ADDR, 0xAC, 3000);
        /* set acc range */
        SPI_WRITE((BMI2_ACC_CONF_ADDR + 1), 0x03, 5000);
        // read stale accel data
        SPI_READ(BMI2_ACC_X_LSB_ADDR, 7, &mTask.dataBuffer);

        mTask.acc_test_state = ACC_TEST_RUN_0;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[ACC], __FUNCTION__);
        break;

    case ACC_TEST_RUN_0:
        ERROR_PRINT("ACC_TEST_RUN_0\n");
        // configure acc_self_test_amp=1, acc_self_test_sign=0, acc_self_test_enable=b01
        // wait 50ms for data to be available
        /* get positive data*/
         SPI_WRITE(BMI2_ACC_SELF_TEST_ADDR, 0x0D, 200000);/* delay 100ms */
        // read stale accel data
        SPI_READ(BMI2_ACC_X_LSB_ADDR, 7, &mTask.dataBuffer);
        mTask.acc_test_state = ACC_TEST_RUN_1;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[ACC], __FUNCTION__);
        break;

    case ACC_TEST_RUN_1:
        ERROR_PRINT("ACC_TEST_RUN_1\n");
        // save accel data
        mTask.accTestX = *(int16_t*)(mTask.dataBuffer+2);
        mTask.accTestY = *(int16_t*)(mTask.dataBuffer+4);
        mTask.accTestZ = *(int16_t*)(mTask.dataBuffer+6);

        /* get negative data*/
        SPI_WRITE(BMI2_ACC_SELF_TEST_ADDR, 0x09, 200000);/* delay 100ms */
        // read stale accel data
        SPI_READ(BMI2_ACC_X_LSB_ADDR, 7, &mTask.dataBuffer);
        mTask.acc_test_state = ACC_TEST_VERIFY;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[ACC], __FUNCTION__);
        break;

    case ACC_TEST_VERIFY:
        ERROR_PRINT("ACC_TEST_VERIFY\n");
        // save accel data
        tempTestX = *(int16_t*)(mTask.dataBuffer+2);
        tempTestY = *(int16_t*)(mTask.dataBuffer+4);
        tempTestZ = *(int16_t*)(mTask.dataBuffer+6);

        // calculate the differences between run 0 and run 1
        absDiffX = ABS((int32_t)mTask.accTestX - tempTestX);
        absDiffY = ABS((int32_t)mTask.accTestY - tempTestY);
        absDiffZ = ABS((int32_t)mTask.accTestZ - tempTestZ);

        ERROR_PRINT("accSelfTest diffs: X %d, Y %d, Z %d\n", (int)absDiffX, (int)absDiffY, (int)absDiffZ);

        // verify that the differences between run 0 and run 1 are within spec
        if (absDiffX >= kMinDifferenceX && absDiffY >= kMinDifferenceY && absDiffZ >= kMinDifferenceZ) {
            sendTestResult(SENSOR_APP_EVT_STATUS_SUCCESS, SENS_TYPE_ACCEL);
        } else {
            sendTestResult(SENSOR_APP_EVT_STATUS_ERROR, SENS_TYPE_ACCEL);
        }

#ifndef BMI26X_USE_I3C
        /* do software reset */
        SPI_WRITE(BMI2_CMD_REG_ADDR, 0xb6, 100000);
#endif
        /* dummy reads after soft reset for switch SPI bus, wait 100us */
        SPI_READ(BMI2_CHIP_ID_ADDR, 2, &mTask.dataBuffer, 2000);
        /* right now the bus is SPI bus, get chip id again */
        SPI_READ(BMI2_CHIP_ID_ADDR, 2, &mTask.dataBuffer, 2000);

        mTask.acc_test_state = ACC_TEST_INIT_REG;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[ACC], __FUNCTION__);
        break;
    case ACC_TEST_INIT_REG:
        ERROR_PRINT("ACC_TEST_INIT_REG\n");
        initRegisterVal();
        mTask.acc_test_state = ACC_TEST_INIT_LOAD_CONFIG0;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[ACC], __FUNCTION__);
        break;
     case ACC_TEST_INIT_LOAD_CONFIG0:
        ERROR_PRINT("ACC_TEST_INIT_LOAD_CONFIG0\n");
        config_index = 0;
        bmi2xyConfigLoadConfigFile(config_index);
        mTask.acc_test_state = ACC_TEST_INIT_LOAD_CONFIG1;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback,&mTask.sensors[ACC], __FUNCTION__);
        break;
    case ACC_TEST_INIT_LOAD_CONFIG1:
        ERROR_PRINT("ACC_TEST_INIT_LOAD_CONFIG1\n");
        config_index = 1020;
        bmi2xyConfigLoadConfigFile(config_index);
        mTask.acc_test_state = ACC_TEST_INIT_LOAD_CONFIG2;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[ACC], __FUNCTION__);
        break;
    case ACC_TEST_INIT_LOAD_CONFIG2:
        ERROR_PRINT("ACC_TEST_INIT_LOAD_CONFIG2\n");
        config_index = 2040;
        bmi2xyConfigLoadConfigFile(config_index);
        mTask.acc_test_state = ACC_TEST_INIT_LOAD_CONFIG3;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[ACC], __FUNCTION__);
        break;
    case ACC_TEST_INIT_LOAD_CONFIG3:
        ERROR_PRINT("ACC_TEST_INIT_LOAD_CONFIG3\n");
        config_index = 3060;
        bmi2xyConfigLoadConfigFile(config_index);
        mTask.acc_test_state = ACC_TEST_INIT_LOAD_CONFIG4;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[ACC], __FUNCTION__);
        break;
    case ACC_TEST_INIT_LOAD_CONFIG4:
        ERROR_PRINT("ACC_TEST_INIT_LOAD_CONFIG4\n");
        config_index = 4080;
        bmi2xyConfigLoadConfigFile(config_index);
        mTask.acc_test_state = ACC_TEST_INIT_LOAD_CONFIG5;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[ACC], __FUNCTION__);
        break;
    case ACC_TEST_INIT_LOAD_CONFIG5:
        ERROR_PRINT("ACC_TEST_INIT_LOAD_CONFIG5\n");
        config_index = 5100;
        bmi2xyConfigLoadConfigFile(config_index);
        mTask.acc_test_state = ACC_TEST_INIT_LOAD_CONFIG6;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[ACC], __FUNCTION__);
        break;
    case ACC_TEST_INIT_LOAD_CONFIG6:
        ERROR_PRINT("ACC_TEST_INIT_LOAD_CONFIG6\n");
        config_index = 6120;
        bmi2xyConfigLoadConfigFile(config_index);
        mTask.acc_test_state = ACC_TEST_INIT_LOAD_CONFIG7;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[ACC], __FUNCTION__);
        break;
    case ACC_TEST_INIT_LOAD_CONFIG7:
        ERROR_PRINT("ACC_TEST_INIT_LOAD_CONFIG7\n");
        config_index = 7140;
        bmi2xyConfigLoadConfigFile(config_index);
        mTask.acc_test_state = ACC_TEST_INIT_LOAD_CONFIG8;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[ACC], __FUNCTION__);
        break;
    case ACC_TEST_INIT_LOAD_CONFIG8:
        ERROR_PRINT("ACC_TEST_INIT_LOAD_CONFIG8\n");
        config_index = 8160;
        bmi2xyConfigLoadConfigFile(config_index);
        mTask.acc_test_state = ACC_TEST_INIT_LOAD_CONFIG_CHECk;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[ACC], __FUNCTION__);
        break;
    case ACC_TEST_INIT_LOAD_CONFIG_CHECk:
        ERROR_PRINT("ACC_TEST_INIT_LOAD_CONFIG_CHECk\n");
         /* Enable config loading and FIFO mode */
         SPI_WRITE(BMI2_INIT_CTRL_ADDR, 0x01, 150000);
        /* read config file status check if load successful */
         SPI_READ(BMI2_INTERNAL_STATUS_ADDR, 2, &mTask.config_stream_status);
#if defined(BMI260)
        page = BMI2_PAGE_1;
#endif
#if defined(BMI270)
        page = BMI2_PAGE_1;
#endif
#if defined(BMI263)
		page = BMI2_PAGE_2;
#endif
        /* Set the page */
        SPI_WRITE(BMI2_FEAT_PAGE_ADDR, page, 1000);
        /* get the anymotion from the page1 */
        SPI_READ(BMI2_FEATURES_REG_ADDR, 17, &mTask.dataBuffer);
        mTask.acc_test_state = ACC_TEST_INIT_LOAD_CONFIG_CHECk_DONE;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[ACC], __FUNCTION__);
         break;
    case ACC_TEST_INIT_LOAD_CONFIG_CHECk_DONE:
        ERROR_PRINT("dump reg addr:30, %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x\n",
            mTask.dataBuffer[2],mTask.dataBuffer[3],mTask.dataBuffer[4],mTask.dataBuffer[5],
            mTask.dataBuffer[6],mTask.dataBuffer[7],mTask.dataBuffer[8],mTask.dataBuffer[9],
            mTask.dataBuffer[10],mTask.dataBuffer[11],mTask.dataBuffer[12],mTask.dataBuffer[13],
            mTask.dataBuffer[14],mTask.dataBuffer[15],mTask.dataBuffer[16],mTask.dataBuffer[17]);

        config_stream_status = mTask.config_stream_status[2];
        if (config_stream_status != BMI2_CONFIG_LOAD_SUCCESS) {
          ERROR_PRINT("bmi2xyLoadConfigfile failed\n");
        }
        DEBUG_PRINT("config_stream_status = %x\n", mTask.config_stream_status[2]);
        /* enable the anymotion in feature */
        DEBUG_PRINT("enable anyMotionPower in feature\n");
#if defined(BMI260)
        page = BMI2_PAGE_1;
#endif
#if defined(BMI270)
        page = BMI2_PAGE_1;
#endif
#if defined(BMI263)
		page = BMI2_PAGE_2;
#endif
        /* Set the page */
        SPI_WRITE(BMI2_FEAT_PAGE_ADDR, page, 1000);
        /* enable the anymotion config */
#if defined(BMI260) || defined(BMI263)
        internel_write[0] = 0x30;
        memcpy(&internel_write[1], &mTask.dataBuffer[2], 16);
        DEBUG_PRINT("internel_write last  %x %x %x %x %x %x %x %x %x\n",
        internel_write[0], internel_write[1], internel_write[2], internel_write[3], internel_write[4], internel_write[9], internel_write[10], internel_write[15], internel_write[16]);
        /* enable the any-motion the bit out_conf = 0x07 and threshold is set to 83mg */
        internel_write[10] = 0xb8;
        DEBUG_PRINT("internel_write last  %x %x %x %x %x %x %x %x %x\n",
        internel_write[0], internel_write[1], internel_write[2], internel_write[3], internel_write[4], internel_write[9], internel_write[10], internel_write[15], internel_write[16]);
        /* Write to feature config registers */
        spiQueueWriteBlock(internel_write, (16 + 1), 1000);
#endif
#if defined(BMI270)
        internel_write[0] = 0x30;
        memcpy(&internel_write[1], &mTask.dataBuffer[2], 16);
        ERROR_PRINT("internel_write last  %x %x %x %x %x %x %x %x %x\n",
        internel_write[0], internel_write[1], internel_write[2], internel_write[3], internel_write[4], internel_write[5], internel_write[6], internel_write[15], internel_write[16]);
        /* enable the anymotion the bit */
        internel_write[16] = 0xb8;
        ERROR_PRINT("internel_write last  %x %x %x %x %x %x %x %x %x\n",
        internel_write[0], internel_write[1], internel_write[2], internel_write[3], internel_write[4], internel_write[5], internel_write[6], internel_write[15], internel_write[16]);
        /* Write to feature config registers */
        spiQueueWriteBlock(internel_write, (16 + 1), 1000);
#endif

        mTask.acc_test_state = ACC_TEST_RESTORE_REG;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[ACC], __FUNCTION__);
        break;
    case ACC_TEST_RESTORE_REG:
        ERROR_PRINT("ACC_TEST_RESTORE_REG\n");
        ERROR_PRINT("restore reg40-49 %x %x %x %x %x %x %x %x %x %x\n",
        mTask.reg_val[2], mTask.reg_val[3], mTask.reg_val[4], mTask.reg_val[5], mTask.reg_val[6],
        mTask.reg_val[7], mTask.reg_val[8], mTask.reg_val[9], mTask.reg_val[10], mTask.reg_val[11]);
        ERROR_PRINT("restore reg56-58 7c7d %x %x %x %x %x\n",
        mTask.reg_val[12], mTask.reg_val[13], mTask.reg_val[14], mTask.reg_val[15], mTask.reg_val[16]);
         restoreRegisterVal();
        mTask.acc_test_state = ACC_TEST_DONE;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[ACC], __FUNCTION__);
         break;
    default:
        ERROR_PRINT("Invalid accel test state\n");
        break;
    }
}



static bool accSelfTest(void *cookie)
{
    TDECL();
    ERROR_PRINT("accSelfTest\n");

    if (!mTask.sensors[ACC].powered && trySwitchState(SENSOR_TESTING)) {
        mTask.acc_test_state = ACC_TEST_START;
        accTestHandling();
        return true;
    } else {
        ERROR_PRINT("cannot test accel because sensor is busy\n");
        sendTestResult(SENSOR_APP_EVT_STATUS_BUSY, SENS_TYPE_ACCEL);
        return false;
    }
}
/*! @name Structure to define accelerometer and gyroscope sensor axes and
 * sensor time for virtual frames
 */
struct bmi2_sens_axes_data
{
    /*! Data in x-axis */
    int16_t x;

    /*! Data in y-axis */
    int16_t y;

    /*! Data in z-axis */
    int16_t z;

    /*! Sensor time for virtual frames */
    uint32_t virt_sens_time;
};
/*!
 * @brief This internal API saturates the gyroscope data value before writing to
 * to 10 bit offset register.
 */
static void saturate_gyro_data(struct bmi2_sens_axes_data *gyr_off)
{
    if (gyr_off->x > 511)
    {
        gyr_off->x = 511;
    }
    if (gyr_off->x < -512)
    {
        gyr_off->x = -512;
    }
    if (gyr_off->y > 511)
    {
        gyr_off->y = 511;
    }
    if (gyr_off->y < -512)
    {
        gyr_off->y = -512;
    }
    if (gyr_off->z > 511)
    {
        gyr_off->z = 511;
    }
    if (gyr_off->z < -512)
    {
        gyr_off->z = -512;
    }
}

/*!
 * @brief This internal API inverts the gyroscope offset data.
 */
static void invert_gyro_offset(struct bmi2_sens_axes_data *offset_data)
{
    /* Invert the values */
    offset_data->x = (int16_t)((offset_data->x) * (-1));
    offset_data->y = (int16_t)((offset_data->y) * (-1));
    offset_data->z = (int16_t)((offset_data->z) * (-1));
}

static void gyrCalibrationHandling(void)
{
    TDECL();
    uint8_t mode = 0;
    int16_t x = 0;
    int16_t y = 0;
    int16_t z = 0;
    /* Variable to store MSB value of offset compensation for x-axis */
    uint8_t gyr_off_msb_x = 0;
    /* Variable to store MSB value of offset compensation for y-axis */
    uint8_t gyr_off_msb_y = 0;
    /* Variable to store MSB value of offset compensation for z-axis */
    uint8_t gyr_off_msb_z = 0;
    uint8_t reg_data[4] = {0};
    uint8_t reg59_val = 0;
    /* Structure to store the offset values to be stored in the register */
    struct bmi2_sens_axes_data gyro_offset = { 0, 0, 0, 0 };
    switch (mTask.calibration_state) {
    case CALIBRATION_START:
        T(mRetryLeft) = RETRY_CNT_CALIBRATION;
         // disable the fifo function
         SPI_WRITE(BMI2_FIFO_CONFIG_1_ADDR, 0x10, 450);
       /* Set gyroscope configurations to 50Hz, continuous mode, CIC mode*/
        SPI_WRITE(BMI2_GYR_CONF_ADDR, 0xB7, 2000);
        /* Get the status of gyroscope offset enable */
        SPI_READ(BMI2_GYR_OFF_COMP_3_ADDR, 5, &mTask.statusBuffer);
                // turn GYR to NORMAL mode
        SPI_WRITE(BMI2_PWR_CTRL_ADDR, 0x0A, 80000);
        mTask.calibration_state = CALIBRATION_FOC_PRE;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[GYR], __FUNCTION__);
        break;
     case CALIBRATION_FOC_PRE:
        ERROR_PRINT("dump gyro offset, %x %x %x %x\n",
            mTask.statusBuffer[2], mTask.statusBuffer[3], mTask.statusBuffer[4], mTask.statusBuffer[5]);
         mode = mTask.statusBuffer[5];
         ERROR_PRINT("gyro work mode = %x\n",mode);
         mode = BMI2_SET_BITS(mode, BMI2_GYR_OFF_COMP_EN, 0);
         /* Enable/Disable gyroscope offset compensation */
         SPI_WRITE(BMI2_GYR_OFF_COMP_6_ADDR, mode);
        mTask.calibration_state = CALIBRATION_FOC;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[GYR], __FUNCTION__);
        break;
    case CALIBRATION_FOC:
        // poll get the acc data for 128 frame
        SPI_READ(BMI2_GYR_X_LSB_ADDR, 7, &mTask.statusBuffer, 50000);
        mTask.calibration_state = CALIBRATION_WAIT_FOC_DONE;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[GYR], __FUNCTION__);
        break;
    case CALIBRATION_WAIT_FOC_DONE:
        // if the STATUS REG has bit 3 set, it means calbration is done.
        // otherwise, check back in 50ms later.
        if (T(gyro_foc_read) < 128) {
            ERROR_PRINT("dump gyro data, %x %x %x %x %x %x read time = %d\n",
                mTask.statusBuffer[2], mTask.statusBuffer[3], mTask.statusBuffer[4], mTask.statusBuffer[5], mTask.statusBuffer[6],
                mTask.statusBuffer[7], T(gyro_foc_read));
            x = (mTask.statusBuffer[2] | mTask.statusBuffer[3]<<8);
            y = (mTask.statusBuffer[4] | mTask.statusBuffer[5]<<8);
            z = (mTask.statusBuffer[6] | mTask.statusBuffer[7]<<8);
            ERROR_PRINT("dump gyro data, %d %d %d \n", x, y,z);
            /*gyro x */
            mTask.gyro_value[0] =x + mTask.gyro_value[0];
            /*gyro y */
            mTask.gyro_value[1] =y + mTask.gyro_value[1];
            /*gyro x */
            mTask.gyro_value[2] =z + mTask.gyro_value[2];
            mTask.calibration_state = CALIBRATION_FOC;
            T(gyro_foc_read) = T(gyro_foc_read) + 1;
            // add one spi read function
            SPI_READ(BMI2_GYR_X_LSB_ADDR, 7, &mTask.statusBuffer);
            ERROR_PRINT("gyro data, %ld %ld %ld read time = %d\n",
                mTask.gyro_value[0], mTask.gyro_value[1], mTask.gyro_value[2], T(gyro_foc_read));
        } else {
           T(gyro_foc_read) = 0;
            // get nv config register
            //SPI_READ(BMI2_NV_CONF_ADDR, &mTask.statusBuffer, 1);
            /* Take average of x, y and z data for lesser
             * noise. It is same as offset data since lsb/dps
             * is same for both data and offset register
             */
            mTask.gyro_avg[0] = mTask.gyro_value[0] / 128;
            mTask.gyro_avg[1] = mTask.gyro_value[1] / 128;
            mTask.gyro_avg[2] = mTask.gyro_value[2] / 128;
            ERROR_PRINT("gyro avg data, %ld %ld %ld \n", mTask.gyro_avg[0],  mTask.gyro_avg[1],  mTask.gyro_avg[2]);
            mTask.gyro_value[0] = mTask.gyro_value[1] = mTask.gyro_value[2] = 0;
            /* Take average of x, y and z data for lesser
             * noise. It is same as offset data since lsb/dps
             * is same for both data and offset register
             */
            gyro_offset.x = (int16_t)mTask.gyro_avg[0];
            gyro_offset.y = (int16_t)mTask.gyro_avg[1];
            gyro_offset.z = (int16_t)mTask.gyro_avg[2];

            /* Saturate gyroscope data since the offset
             * registers are of 10 bit value where as the
             * gyroscope data is of 16 bit value
             */
            saturate_gyro_data(&gyro_offset);
            ERROR_PRINT("gyro_offset data 1, %x %x %x \n", gyro_offset.x, gyro_offset.y, gyro_offset.z);
            /* Invert the gyroscope offset  data */
            invert_gyro_offset(&gyro_offset);
            ERROR_PRINT("gyro_offset data 2, %x %x %x \n", gyro_offset.x, gyro_offset.y, gyro_offset.z);
            mTask.sensors[GYR].offset[0] = gyro_offset.x;
            mTask.sensors[GYR].offset[1] = gyro_offset.y;
            mTask.sensors[GYR].offset[2] = gyro_offset.z;
            ERROR_PRINT("GYRO OFFSET is %02x  %02x  %02x\n",
                (unsigned int)mTask.sensors[GYR].offset[0],
                (unsigned int)mTask.sensors[GYR].offset[1],
                (unsigned int)mTask.sensors[GYR].offset[2]);
            SPI_READ(BMI2_GYR_OFF_COMP_6_ADDR, 2, &mTask.statusBuffer);
            /* read 0x59 value */
            SPI_READ(BMI2_INIT_CTRL_ADDR, 2, &mTask.reg59_val);
            mTask.calibration_state = CALIBRATION_SET_OFFSET;
            DEBUG_PRINT("FOC set FINISHED!\n");
        }
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[GYR], __FUNCTION__);
        break;

    case CALIBRATION_SET_OFFSET:
        reg_data[3] = mTask.statusBuffer[2];
        reg59_val = mTask.reg59_val[2];
        mTask.saved_reg59_val = mTask.reg59_val[2];
        /* Get MSB value of x-axis from user-input */
        gyr_off_msb_x = (uint8_t)((mTask.sensors[GYR].offset[0] & GYR_OFF_COMP_MSB_MASK) >> 8);
        /* Get MSB value of y-axis from user-input */
        gyr_off_msb_y = (uint8_t)((mTask.sensors[GYR].offset[1] & GYR_OFF_COMP_MSB_MASK) >> 8);
        /* Get MSB value of z-axis from user-input */
        gyr_off_msb_z = (uint8_t)((mTask.sensors[GYR].offset[2] & GYR_OFF_COMP_MSB_MASK) >> 8);
        /* Get LSB value of x-axis from user-input */
        reg_data[0] = (uint8_t)(mTask.sensors[GYR].offset[0] & GYR_OFF_COMP_LSB_MASK);
        /* Get LSB value of y-axis from user-input */
        reg_data[1] = (uint8_t)(mTask.sensors[GYR].offset[1] & GYR_OFF_COMP_LSB_MASK);
        /* Get LSB value of z-axis from user-input */
        reg_data[2] = (uint8_t)(mTask.sensors[GYR].offset[2] & GYR_OFF_COMP_LSB_MASK);
        /* Get MSB value of x-axis to be set */
        reg_data[3] = BMI2_SET_BIT_POS0(reg_data[3], GYR_OFF_COMP_MSB_X, gyr_off_msb_x);
        /* Get MSB value of y-axis to be set */
        reg_data[3] = BMI2_SET_BITS(reg_data[3], GYR_OFF_COMP_MSB_Y, gyr_off_msb_y);
        /* Get MSB value of z-axis to be set */
        reg_data[3] = BMI2_SET_BITS(reg_data[3], GYR_OFF_COMP_MSB_Z, gyr_off_msb_z);
        /* Enable gyroscope offset compensation */
        reg_data[3] = BMI2_SET_BITS(reg_data[3], BMI2_GYR_OFF_COMP_EN, 1);
        mTask.sensors[GYR].offset[0] =  reg_data[0];
        mTask.sensors[GYR].offset[1] =  reg_data[1];
        mTask.sensors[GYR].offset[2] =  reg_data[2];
        mTask.sensors[GYR].offset_enable = true;
        DEBUG_PRINT("GYRO OFFSET is %02x  %02x  %02x\n",
                (unsigned int)mTask.sensors[GYR].offset[0],
                (unsigned int)mTask.sensors[GYR].offset[1],
                (unsigned int)mTask.sensors[GYR].offset[2]);

        sendCalibrationResult(SENSOR_APP_EVT_STATUS_SUCCESS, SENS_TYPE_GYRO,
                mTask.sensors[GYR].offset[0], mTask.sensors[GYR].offset[1],
                mTask.sensors[GYR].offset[2]);

        /* Offset values are written in the offset register */
        SPI_WRITE(BMI2_GYR_OFF_COMP_3_ADDR, mTask.sensors[GYR].offset[0], 1000);
        SPI_WRITE(BMI2_GYR_OFF_COMP_3_ADDR + 1, mTask.sensors[GYR].offset[1], 1000);
        SPI_WRITE(BMI2_GYR_OFF_COMP_3_ADDR + 2, mTask.sensors[GYR].offset[2], 1000);
        SPI_WRITE(BMI2_GYR_OFF_COMP_6_ADDR, reg_data[3], 1000);
        /* set 0x59 bit 0 = 0 */
        reg59_val = BST_CLR_VAL_BIT(reg59_val, 0);
        SPI_WRITE(BMI2_INIT_CTRL_ADDR, reg59_val, 1000);
        /* enable nvm */
        SPI_WRITE(BMI2_NVM_CONF_ADDR, 0x02, 1000);
        /* write cmd */
        SPI_WRITE(BMI2_CMD_REG_ADDR, BMI2_NVM_PROG_CMD, 2000);
         /* restore 0x59 */
        SPI_WRITE(BMI2_INIT_CTRL_ADDR, mTask.saved_reg59_val, 1000);
        SPI_READ(BMI2_GYR_OFF_COMP_3_ADDR, 5, &mTask.statusBuffer);

        mTask.calibration_state = CALIBRATION_CHECK;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[GYR], __FUNCTION__);
        break;
     case CALIBRATION_CHECK:
        ERROR_PRINT("gyro OFFSET is %02x  %02x  %02x  %02x\n",
                (unsigned int)mTask.statusBuffer[2],
                (unsigned int)mTask.statusBuffer[3],
                (unsigned int)mTask.statusBuffer[4],
                (unsigned int)mTask.statusBuffer[5]);
        mTask.calibration_state = CALIBRATION_DONE;
        // turn gyro to off mode
        SPI_WRITE(BMI2_PWR_CTRL_ADDR, 0x00, 5000);
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[ACC], __FUNCTION__);
        break;
    default:
        ERROR_PRINT("Invalid calibration state\n");
        break;
    }
}



static bool gyrCalibration(void *cookie)
{
    TDECL();
    if (!mTask.sensors[GYR].powered && trySwitchState(SENSOR_CALIBRATING)) {
        mTask.calibration_state = CALIBRATION_START;
        gyrCalibrationHandling();
        return true;
    } else {
        ERROR_PRINT("cannot calibrate gyro because sensor is busy\n");
        sendCalibrationResult(SENSOR_APP_EVT_STATUS_BUSY, SENS_TYPE_GYRO, 0, 0, 0);
        return false;
    }
}

static bool gyrCfgData(void *data, void *cookie)
{
    TDECL();
    const struct AppToSensorHalDataPayload *p = data;
    if (p->type == HALINTF_TYPE_GYRO_CAL_BIAS && p->size == sizeof(struct GyroCalBias)) {
        const struct GyroCalBias *bias = p->gyroCalBias;
        mTask.sensors[GYR].offset[0] = bias->hardwareBias[0];
        mTask.sensors[GYR].offset[1] = bias->hardwareBias[1];
        mTask.sensors[GYR].offset[2] = bias->hardwareBias[2];
        mTask.sensors[GYR].offset_enable = true;
        INFO_PRINT("gyrCfgData hw bias: data=%02lx, %02lx, %02lx\n",
                bias->hardwareBias[0] & 0xFF,
                bias->hardwareBias[1] & 0xFF,
                bias->hardwareBias[2] & 0xFF);

#ifdef GYRO_CAL_ENABLED
        const float dummy_temperature_celsius = 25.0f;
        gyroCalSetBias(&T(gyro_cal), bias->softwareBias[0],
                       bias->softwareBias[1], bias->softwareBias[2],
                       dummy_temperature_celsius,
                       sensorGetTime());
#endif  // GYRO_CAL_ENABLED
        if (!saveCalibration()) {
            T(pending_calibration_save) = true;
        }
#ifdef OVERTEMPCAL_ENABLED
    } else if (p->type == HALINTF_TYPE_GYRO_OTC_DATA && p->size == sizeof(struct GyroOtcData)) {
        handleOtcGyroConfig(data);
#endif // OVERTEMPCAL_ENABLED
    } else {
        ERROR_PRINT("Unknown gyro config data type 0x%04x, size %d\n", p->type, p->size);
    }
    return true;
}

static void gyroTestHandling(void)
{
    uint16_t config_index = 0;
    uint8_t page = 0;
    uint8_t config_stream_status = 0;

    TDECL();
    switch (mTask.gyro_test_state) {
    case GYRO_TEST_START:
        ERROR_PRINT("gyroTestHandling: state =GYRO_TEST_START\n");
        /*backup the the all config*/
        SPI_READ(BMI2_ACC_CONF_ADDR, 11, &mTask.reg40_49);
        SPI_READ(BMI2_INT1_MAP_FEAT_ADDR, 4, &mTask.reg56_58);
        SPI_READ(BMI2_PWR_CONF_ADDR, 3, &mTask.reg7c_7d);
        /*disable the acc and gyro fifo enable*/
        SPI_WRITE(BMI2_FIFO_CONFIG_1_ADDR, 0x10, 5000);
        /* disable the advance power config */
        SPI_WRITE(BMI2_PWR_CONF_ADDR, 0x00, 1000);
        /* enable the acc gyro power */
        SPI_WRITE(BMI2_PWR_CTRL_ADDR, 0x06, 50000);
        SPI_WRITE((BMI2_GYR_SELF_TEST_AXES_ADDR + 1), 0x01, 300000);
        mTask.gyro_test_state = GYRO_TEST_RUN;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[GYR], __FUNCTION__);
        break;
    case GYRO_TEST_RUN:
        ERROR_PRINT("GYRO_TEST_RUN\n");
        ERROR_PRINT("dump reg addr:40-49, %x %x %x %x %x %x %x %x %x %x\n",
            mTask.reg40_49[2], mTask.reg40_49[3], mTask.reg40_49[4], mTask.reg40_49[5], mTask.reg40_49[6],
            mTask.reg40_49[7], mTask.reg40_49[8], mTask.reg40_49[9], mTask.reg40_49[10], mTask.reg40_49[11]);
        ERROR_PRINT("dump reg addr:56-58, 0x%x 0x%x 0x%x\n",
            mTask.reg56_58[2], mTask.reg56_58[3], mTask.reg56_58[4]);
        memcpy(&mTask.reg_val[2], &mTask.reg40_49[2], 10);
        ERROR_PRINT("reg40-49 %x %x %x %x %x %x %x %x %x %x\n",
        mTask.reg_val[2], mTask.reg_val[3], mTask.reg_val[4], mTask.reg_val[5], mTask.reg_val[6],
        mTask.reg_val[7], mTask.reg_val[8], mTask.reg_val[9], mTask.reg_val[10], mTask.reg_val[11]);
        memcpy(&mTask.reg_val[12], &mTask.reg56_58[2], 3);
        memcpy(&mTask.reg_val[15], &mTask.reg7c_7d[2], 2);
        ERROR_PRINT("reg56-58 7c7d %x %x %x %x %x\n",
        mTask.reg_val[12], mTask.reg_val[13], mTask.reg_val[14], mTask.reg_val[15], mTask.reg_val[16]);

        SPI_READ((BMI2_GYR_SELF_TEST_AXES_ADDR + 1), 2, &mTask.statusBuffer);
        mTask.gyro_test_state = GYRO_TEST_VERIFY;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[GYR], __FUNCTION__);
        break;

    case GYRO_TEST_VERIFY:
        INFO_PRINT("GYRO_TEST_VERIFY statusBuffer = %x\n", mTask.statusBuffer[2]);
        if (mTask.statusBuffer[2] & 0x2) {
            INFO_PRINT("Gyro selftest PASS");
            sendTestResult(SENSOR_APP_EVT_STATUS_SUCCESS, SENS_TYPE_GYRO);
        } else {
            INFO_PRINT("Gyro selftest FAILURE");
            sendTestResult(SENSOR_APP_EVT_STATUS_ERROR, SENS_TYPE_GYRO);
        }
#ifndef BMI26X_USE_I3C
        // perform soft reset and wait for 100ms
        SPI_WRITE(BMI2_CMD_REG_ADDR, 0xb6, 100000);
#endif
        /* dummy reads after soft reset for switch SPI bus, wait 100us */
        SPI_READ(BMI2_CHIP_ID_ADDR, 2, &mTask.dataBuffer, 2000);
        /* right now the bus is SPI bus, get chip id again */
        SPI_READ(BMI2_CHIP_ID_ADDR, 2, &mTask.dataBuffer, 2000);

        mTask.gyro_test_state = GYRO_TEST_INIT_REG;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[GYR], __FUNCTION__);
        break;
    case GYRO_TEST_INIT_REG:
        ERROR_PRINT("GYRO_TEST_INIT_REG\n");
        initRegisterVal();
        mTask.gyro_test_state = GYRO_TEST_INIT_LOAD_CONFIG0;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[GYR], __FUNCTION__);
        break;
     case GYRO_TEST_INIT_LOAD_CONFIG0:
        config_index = 0;
        bmi2xyConfigLoadConfigFile(config_index);
        mTask.gyro_test_state = GYRO_TEST_INIT_LOAD_CONFIG1;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[GYR], __FUNCTION__);
        break;
    case GYRO_TEST_INIT_LOAD_CONFIG1:
        config_index = 1020;
        bmi2xyConfigLoadConfigFile(config_index);
        mTask.gyro_test_state = GYRO_TEST_INIT_LOAD_CONFIG2;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[GYR], __FUNCTION__);
        break;
    case GYRO_TEST_INIT_LOAD_CONFIG2:
        config_index = 2040;
        bmi2xyConfigLoadConfigFile(config_index);
        mTask.gyro_test_state = GYRO_TEST_INIT_LOAD_CONFIG3;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[GYR], __FUNCTION__);
        break;
    case GYRO_TEST_INIT_LOAD_CONFIG3:
        config_index = 3060;
        bmi2xyConfigLoadConfigFile(config_index);
        mTask.gyro_test_state = GYRO_TEST_INIT_LOAD_CONFIG4;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[GYR], __FUNCTION__);
        break;
    case GYRO_TEST_INIT_LOAD_CONFIG4:
        config_index = 4080;
        bmi2xyConfigLoadConfigFile(config_index);
        mTask.gyro_test_state = GYRO_TEST_INIT_LOAD_CONFIG5;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[GYR], __FUNCTION__);
        break;
    case GYRO_TEST_INIT_LOAD_CONFIG5:
        config_index = 5100;
        bmi2xyConfigLoadConfigFile(config_index);
        mTask.gyro_test_state = GYRO_TEST_INIT_LOAD_CONFIG6;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[GYR], __FUNCTION__);
        break;
    case GYRO_TEST_INIT_LOAD_CONFIG6:
        config_index = 6120;
        bmi2xyConfigLoadConfigFile(config_index);
        mTask.gyro_test_state = GYRO_TEST_INIT_LOAD_CONFIG7;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[GYR], __FUNCTION__);
        break;
    case GYRO_TEST_INIT_LOAD_CONFIG7:
        config_index = 7140;
        bmi2xyConfigLoadConfigFile(config_index);
        mTask.gyro_test_state = GYRO_TEST_INIT_LOAD_CONFIG8;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[GYR], __FUNCTION__);
        break;
    case GYRO_TEST_INIT_LOAD_CONFIG8:
        config_index = 8160;
        bmi2xyConfigLoadConfigFile(config_index);
        mTask.gyro_test_state = GYRO_TEST_INIT_LOAD_CONFIG_CHECk;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[GYR], __FUNCTION__);
        break;
    case GYRO_TEST_INIT_LOAD_CONFIG_CHECk:
         /* Enable config loading and FIFO mode */
         SPI_WRITE(BMI2_INIT_CTRL_ADDR, 0x01, 150000);
        /* read config file status check if load successful */
         SPI_READ(BMI2_INTERNAL_STATUS_ADDR, 2, &mTask.config_stream_status);
#if defined(BMI260)
        page = BMI2_PAGE_1;
#endif
#if defined(BMI270)
         page = BMI2_PAGE_1;
#endif
#if defined(BMI263)
         page = BMI2_PAGE_2;
#endif
        /* Set the page */
        SPI_WRITE(BMI2_FEAT_PAGE_ADDR, page, 1000);
        /* get the anymotion from the page1 */
        SPI_READ(BMI2_FEATURES_REG_ADDR, 17, &mTask.dataBuffer);
        mTask.gyro_test_state = GYRO_TEST_INIT_LOAD_CONFIG_CHECk_DONE;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[GYR], __FUNCTION__);
         break;
    case GYRO_TEST_INIT_LOAD_CONFIG_CHECk_DONE:
        ERROR_PRINT("dump reg addr:30, %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x\n",
            mTask.dataBuffer[2],mTask.dataBuffer[3],mTask.dataBuffer[4],mTask.dataBuffer[5],
            mTask.dataBuffer[6],mTask.dataBuffer[7],mTask.dataBuffer[8],mTask.dataBuffer[9],
            mTask.dataBuffer[10],mTask.dataBuffer[11],mTask.dataBuffer[12],mTask.dataBuffer[13],
            mTask.dataBuffer[14],mTask.dataBuffer[15],mTask.dataBuffer[16],mTask.dataBuffer[17]);

        config_stream_status = mTask.config_stream_status[2];
        if (config_stream_status != BMI2_CONFIG_LOAD_SUCCESS) {
          ERROR_PRINT("bmi2xyLoadConfigfile failed\n");
        }
        DEBUG_PRINT("config_stream_status = %x\n", mTask.config_stream_status[2]);
        /* enable the anymotion in feature */
        DEBUG_PRINT("enable anyMotionPower in feature\n");
#if defined(BMI260)
        page = BMI2_PAGE_1;
#endif
#if defined(BMI270)
        page = BMI2_PAGE_1;
#endif
#if defined(BMI263)
         page = BMI2_PAGE_2;
#endif
        /* Set the page */
        SPI_WRITE(BMI2_FEAT_PAGE_ADDR, page, 1000);
        /* enable the anymotion config */
#if defined(BMI260) || defined(BMI263)
        internel_write[0] = 0x30;
        memcpy(&internel_write[1], &mTask.dataBuffer[2], 16);
        ERROR_PRINT("internel_write last  %x %x %x %x %x %x %x %x %x\n",
        internel_write[0], internel_write[1], internel_write[2], internel_write[3], internel_write[4], internel_write[5], internel_write[6], internel_write[15], internel_write[16]);
        /* enable the anymotion the bit */
        internel_write[10] = 0xb8;
        ERROR_PRINT("internel_write last  %x %x %x %x %x %x %x %x %x\n",
        internel_write[0], internel_write[1], internel_write[2], internel_write[3], internel_write[4], internel_write[5], internel_write[6], internel_write[15], internel_write[16]);
        /* Write to feature config registers */
        spiQueueWriteBlock(internel_write, (16 + 1), 1000);
#endif
#if defined(BMI270)
        internel_write[0] = 0x30;
        memcpy(&internel_write[1], &mTask.dataBuffer[2], 16);
        ERROR_PRINT("internel_write last  %x %x %x %x %x %x %x %x %x\n",
        internel_write[0], internel_write[1], internel_write[2], internel_write[3], internel_write[4], internel_write[5], internel_write[6], internel_write[15], internel_write[16]);
        /* enable the anymotion the bit */
        internel_write[16] = 0xb8;
        ERROR_PRINT("internel_write last  %x %x %x %x %x %x %x %x %x\n",
        internel_write[0], internel_write[1], internel_write[2], internel_write[3], internel_write[4], internel_write[5], internel_write[6], internel_write[15], internel_write[16]);
        /* Write to feature config registers */
        spiQueueWriteBlock(internel_write, (16 + 1), 1000);
#endif

        mTask.gyro_test_state = GYRO_TEST_RESTORE_REG;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[GYR], __FUNCTION__);
        break;
    case GYRO_TEST_RESTORE_REG:
        ERROR_PRINT("restore reg40-49 %x %x %x %x %x %x %x %x %x %x\n",
        mTask.reg_val[2], mTask.reg_val[3], mTask.reg_val[4], mTask.reg_val[5], mTask.reg_val[6],
        mTask.reg_val[7], mTask.reg_val[8], mTask.reg_val[9], mTask.reg_val[10], mTask.reg_val[11]);
        ERROR_PRINT("restore reg56-58 7c7d %x %x %x %x %x\n",
        mTask.reg_val[12], mTask.reg_val[13], mTask.reg_val[14], mTask.reg_val[15], mTask.reg_val[16]);
        restoreRegisterVal();
        mTask.gyro_test_state = GYRO_TEST_DONE;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask.sensors[GYR], __FUNCTION__);
        break;

    default:
        ERROR_PRINT("Invalid gyro test state\n");
        break;
    }
}

static bool gyrSelfTest(void *cookie)
{
    TDECL();
    INFO_PRINT("gyrSelfTest\n");

    if (!mTask.sensors[GYR].powered && trySwitchState(SENSOR_TESTING)) {
        mTask.gyro_test_state = GYRO_TEST_START;
        gyroTestHandling();
        return true;
    } else {
        ERROR_PRINT("cannot test gyro because sensor is busy\n");
        sendTestResult(SENSOR_APP_EVT_STATUS_BUSY, SENS_TYPE_GYRO);
        return false;
    }
}
#define DEC_OPS(power, firmware, rate, flush) \
    .sensorPower = power, \
    .sensorFirmwareUpload = firmware, \
    .sensorSetRate = rate, \
    .sensorFlush = flush

#define DEC_OPS_SEND(power, firmware, rate, flush, send) \
    DEC_OPS(power, firmware, rate, flush), \
    .sensorSendOneDirectEvt = send

#define DEC_OPS_CAL_CFG_TEST(power, firmware, rate, flush, cal, cfg, test) \
    DEC_OPS(power, firmware, rate, flush), \
    .sensorCalibrate = cal, \
    .sensorCfgData = cfg, \
    .sensorSelfTest = test,

#define DEC_OPS_CFG(power, firmware, rate, flush, cfg) \
    DEC_OPS(power, firmware, rate, flush), \
    .sensorCfgData = cfg

static const struct SensorOps mSensorOps[NUM_OF_SENSOR] =
{
    { DEC_OPS_CAL_CFG_TEST(accPower, accFirmwareUpload, accSetRate, accFlush, accCalibration,
            accCfgData, accSelfTest) },
    { DEC_OPS_CAL_CFG_TEST(gyrPower, gyrFirmwareUpload, gyrSetRate, gyrFlush, gyrCalibration,
            gyrCfgData, gyrSelfTest) },
#if !defined(NO_NEED_ANYMOTION_FOR_VIVO)
    { DEC_OPS(anyMotionPower, anyMotionFirmwareUpload, anyMotionSetRate, anyMotionFlush) },
#endif
};

static void configEvent(struct BMI26XSensor *mSensor, struct ConfigStat *ConfigData)
{
    int i;

    for (i = 0; &mTask.sensors[i] != mSensor; i++) ;

    if (ConfigData->enable == 0 && mSensor->powered)
        (void)mSensorOps[i].sensorPower(false, (void *)i);
    else if (ConfigData->enable == 1 && !mSensor->powered)
        (void)mSensorOps[i].sensorPower(true, (void *)i);
    else
        (void)mSensorOps[i].sensorSetRate(ConfigData->rate, ConfigData->latency, (void *)i);
}

static void timeSyncEvt(uint32_t evtGeneration, bool evtDataValid)
{
    TDECL();
    // not processing pending events
    if (evtDataValid) {
        // stale event
        if (evtGeneration != mTask.poll_generation)
            return;

        mTask.active_poll_generation = mTask.poll_generation;
    }

    if (trySwitchState(SENSOR_TIME_SYNC)) {
        SPI_READ(BMI2_SENSOR_TIME_ADDR, 4, &mTask.sensorTimeBuffer);
        SPI_READ((BMI2_INTERNAL_STATUS_ADDR + 1), 5, &mTask.temperatureBuffer);
        if (mTask.need_fifo_clean == 1) {
            /*clean the fifo data*/
            SPI_WRITE(BMI2_CMD_REG_ADDR, 0xb0, 1000);
            mTask.need_fifo_clean = 0;
            /* get the register value */
            SPI_READ(BMI2_ACC_CONF_ADDR, 11, &mTask.reg40_49);
            SPI_READ(BMI2_INT1_MAP_FEAT_ADDR, 4, &mTask.reg56_58);
            SPI_READ(BMI2_PWR_CONF_ADDR, 3, &mTask.reg7c_7d);
            SPI_READ(BMI2_INT_STATUS_0_ADDR, 7, &mTask.reg1c_21);
            mTask.demp_reg = 1;
        }
        // sensorSpiCallback schedules a private event, which can be delayed
        // by other long-running tasks.
        // Take the rtc time now so it matches the current sensorTime register
        // reading.
        mTask.timesync_rtc_time = sensorGetTime();
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask, __FUNCTION__);
    } else {
        mTask.pending_time_sync = true;
    }
}

static void processPendingEvt(void)
{
    TDECL();
    enum SensorIndex i;
    if (mTask.pending_int[0]) {
        mTask.pending_int[0] = false;
        initiateFifoRead(false /*isInterruptContext*/);
        return;
    }
    if (mTask.pending_int[1]) {
        mTask.pending_int[1] = false;
        int2Evt();
        return;
    }
    if (mTask.pending_time_sync) {
        mTask.pending_time_sync = false;
        timeSyncEvt(0, false);
        return;
    }
    for (i = FIRST_CONT_SENSOR; i < NUM_OF_SENSOR; i++) {
        if (mTask.pending_config[i]) {
            mTask.pending_config[i] = false;
            configEvent(&mTask.sensors[i], &mTask.sensors[i].pConfig);
            return;
        }
    }
    if (mTask.pending_calibration_save) {
        mTask.pending_calibration_save = !saveCalibration();
        return;
    }

#ifdef OVERTEMPCAL_ENABLED
    // tasks that do not initiate SPI transaction
    if (T(otcGyroUpdateBuffer).sendToHostRequest) {
        sendOtcGyroUpdate();
    }
#endif
}

static void sensorInit(void)
{
    TDECL();
    uint16_t config_index = 0;
    uint8_t config_stream_status = 0;
    uint8_t page = BMI2_PAGE_0;
    #if defined(BMI260)
    page = BMI2_PAGE_1;
    #endif
    #if defined(BMI270)
    page = BMI2_PAGE_1;
    #endif
	#if defined(BMI263)
	page = BMI2_PAGE_2;
	#endif
    switch (mTask.init_state) {
    case RESET_BMI26X:
        DEBUG_PRINT("Performing soft reset\n");
        // perform soft reset and wait for 100ms
        SPI_WRITE(BMI2_CMD_REG_ADDR, 0xb6, 100000);
#ifndef BMI26X_USE_I3C
        /* dummy reads after soft reset for switch SPI bus, wait 100us */
        SPI_READ(BMI2_CHIP_ID_ADDR, 2, &mTask.dataBuffer, 2000);
        /* right now the bus is SPI bus, get chip id again */
        SPI_READ(BMI2_CHIP_ID_ADDR, 2, &mTask.dataBuffer, 2000);
#endif
        mTask.init_state = INIT_BMI26X;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask, "sensorInit RESET" );
        break;

    case INIT_BMI26X:
#ifdef BMI26X_USE_I3C
        i3cInit();
        mTask.i3cHandle = i3cMasterRequest(BMI26X_I3C_BUS_ID, &mI3cPid);
#endif

        /* disable accel, gyro and mag data in FIFO, enable header, enable fifo time */
        SPI_WRITE(BMI2_FIFO_CONFIG_1_ADDR, 0x10, 450);/*enable the fifo header */
        SPI_WRITE(BMI2_FIFO_CONFIG_0_ADDR, 0x02, 450);/*enable the fifo time */

        /* set the watermark to 13 byte */
        SPI_WRITE(BMI2_FIFO_WTM_0_ADDR, 0x18, 450);
        SPI_WRITE(BMI2_FIFO_WTM_1_ADDR, 0, 450);

        /* FIFO watermark and fifo_full interrupt enabled */
        SPI_WRITE(BMI2_INT_MAP_DATA_ADDR, 0x03, 450);

        /* INT1 enabled, high-level (push-pull) triggered */
        SPI_WRITE(BMI2_INT1_IO_CTRL_ADDR, 0x0a, 1000);
        SPI_WRITE(BMI2_INT2_IO_CTRL_ADDR, 0x0a, 1000);

        /* no Map all interrupts to INT1 and INT2 */
        SPI_WRITE(BMI2_INT1_MAP_FEAT_ADDR, 0x00, 450);
        SPI_WRITE(BMI2_INT2_MAP_FEAT_ADDR, 0x00, 450);

        /* initial range for accel (+-8g) and gyro (+-1000 degree) */
        SPI_WRITE(BMI2_ACC_CONF_ADDR + 1, 0x02, 450);
        SPI_WRITE(BMI2_GYR_CONF_ADDR + 1, 0x01, 450);

        /* disable the advance power config */
        SPI_WRITE(BMI2_PWR_CONF_ADDR, 0x00, 1000);
        /* disable the config loading */
        SPI_WRITE(BMI2_INIT_CTRL_ADDR, 0x00, 1000);

        // tell gyro and accel to NOT use the FOC offset.
        mTask.sensors[ACC].offset_enable = false;
        mTask.sensors[GYR].offset_enable = false;

        mTask.init_state = INIT_SENSOR_LOAD_CONFIG_FILE0;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask, "sensorInit INIT");
        break;

    case INIT_SENSOR_LOAD_CONFIG_FILE0:
        config_index = 0;
        bmi2xyConfigLoadConfigFile(config_index);
        mTask.init_state = INIT_SENSOR_LOAD_CONFIG_FILE1;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask, "sensorInit INIT_LOAD_CONFIG_FILE0");
        break;
    case INIT_SENSOR_LOAD_CONFIG_FILE1:
        config_index = 1020;
        bmi2xyConfigLoadConfigFile(config_index);
        mTask.init_state = INIT_SENSOR_LOAD_CONFIG_FILE2;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask, "sensorInit INIT_LOAD_CONFIG_FILE1");
        break;
    case INIT_SENSOR_LOAD_CONFIG_FILE2:
        config_index = 2040;
        bmi2xyConfigLoadConfigFile(config_index);
        mTask.init_state = INIT_SENSOR_LOAD_CONFIG_FILE3;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask, "sensorInit INIT_LOAD_CONFIG_FILE2");
        break;
    case INIT_SENSOR_LOAD_CONFIG_FILE3:
        config_index = 3060;
        bmi2xyConfigLoadConfigFile(config_index);
        mTask.init_state = INIT_SENSOR_LOAD_CONFIG_FILE4;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask, "sensorInit INIT_LOAD_CONFIG_FILE3");
        break;
    case INIT_SENSOR_LOAD_CONFIG_FILE4:
        config_index = 4080;
        bmi2xyConfigLoadConfigFile(config_index);
        mTask.init_state = INIT_SENSOR_LOAD_CONFIG_FILE5;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask, "sensorInit INIT_LOAD_CONFIG_FILE4");
        break;
    case INIT_SENSOR_LOAD_CONFIG_FILE5:
        config_index = 5100;
        bmi2xyConfigLoadConfigFile(config_index);
        mTask.init_state = INIT_SENSOR_LOAD_CONFIG_FILE6;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask, "sensorInit INIT_LOAD_CONFIG_FILE5");
        break;
    case INIT_SENSOR_LOAD_CONFIG_FILE6:
        config_index = 6120;
        bmi2xyConfigLoadConfigFile(config_index);
        mTask.init_state = INIT_SENSOR_LOAD_CONFIG_FILE7;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask, "sensorInit INIT_LOAD_CONFIG_FILE6");
        break;
    case INIT_SENSOR_LOAD_CONFIG_FILE7:
        config_index = 7140;
        bmi2xyConfigLoadConfigFile(config_index);
        mTask.init_state = INIT_SENSOR_LOAD_CONFIG_FILE8;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask, "sensorInit INIT_LOAD_CONFIG_FILE7");
        break;
    case INIT_SENSOR_LOAD_CONFIG_FILE8:
        config_index = 8160;
        bmi2xyConfigLoadConfigFile(config_index);
        mTask.init_state = INIT_SENSOR_LOAD_CONFIG_FILE_CHECK;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask, "sensorInit INIT_LOAD_CONFIG_FILE8");
        break;
    case INIT_SENSOR_LOAD_CONFIG_FILE_CHECK:
         /* Enable config loading and FIFO mode */
         SPI_WRITE(BMI2_INIT_CTRL_ADDR, 0x01, 150000);
        /* read config file status check if load successful */
         SPI_READ(BMI2_INTERNAL_STATUS_ADDR, 2, &mTask.config_stream_status);
#if defined(BMI260)
        page = BMI2_PAGE_1;
#endif
#if defined(BMI270)
        page = BMI2_PAGE_1;
#endif

#if defined(BMI263)
        page = BMI2_PAGE_2;
#endif
        /* Set the page */
        SPI_WRITE(BMI2_FEAT_PAGE_ADDR, page, 1000);
        /* get the anymotion from the page1 */
        SPI_READ(BMI2_FEATURES_REG_ADDR, 17, &mTask.dataBuffer);
        mTask.init_state = INIT_ON_CHANGE_SENSORS;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask, "sensorInit INIT_SENSOR_LOAD_CONFIG_FILE_CHECK");
         break;
    case INIT_ON_CHANGE_SENSORS:
        DEBUG_PRINT("dump reg addr:30, %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x\n",
            mTask.dataBuffer[2],mTask.dataBuffer[3],mTask.dataBuffer[4],mTask.dataBuffer[5],
            mTask.dataBuffer[6],mTask.dataBuffer[7],mTask.dataBuffer[8],mTask.dataBuffer[9],
            mTask.dataBuffer[10],mTask.dataBuffer[11],mTask.dataBuffer[12],mTask.dataBuffer[13],
            mTask.dataBuffer[14],mTask.dataBuffer[15],mTask.dataBuffer[16],mTask.dataBuffer[17]);

        config_stream_status = mTask.config_stream_status[2];
        if (config_stream_status != BMI2_CONFIG_LOAD_SUCCESS) {
          ERROR_PRINT("bmi2xyLoadConfigfile failed\n");
        }
        DEBUG_PRINT( "config_stream_status = %x\n", mTask.config_stream_status[2]);
        /* enable the anymotion in feature */
        DEBUG_PRINT("enable anyMotionPower in feature\n");
#if defined(BMI260)
        page = BMI2_PAGE_1;
#endif
#if defined(BMI270)
        page = BMI2_PAGE_1;
#endif

#if defined(BMI263)
        page = BMI2_PAGE_2;
#endif
        /* Set the page */
        SPI_WRITE(BMI2_FEAT_PAGE_ADDR, page, 1000);
        /* enable the anymotion config */
#if defined(BMI260) || defined(BMI263)
        internel_write[0] = 0x30;
        memcpy(&internel_write[1], &mTask.dataBuffer[2], 16);
        DEBUG_PRINT("internel_write last  %x %x %x %x %x %x %x %x %x\n",
        internel_write[0], internel_write[1], internel_write[2], internel_write[3], internel_write[4], internel_write[9], internel_write[10], internel_write[15], internel_write[16]);
        /* enable the any-motion the bit out_conf = 0x07 and threshold is set to 83mg */
        internel_write[10] = 0xb8;
        DEBUG_PRINT("internel_write last  %x %x %x %x %x %x %x %x %x\n",
        internel_write[0], internel_write[1], internel_write[2], internel_write[3], internel_write[4], internel_write[9], internel_write[10], internel_write[15], internel_write[16]);
        /* Write to feature config registers */
        spiQueueWriteBlock(internel_write, (16 + 1), 1000);
#endif
#if defined(BMI270)
        internel_write[0] = 0x30;
        memcpy(&internel_write[1], &mTask.dataBuffer[2], 16);
        ERROR_PRINT("internel_write last  %x %x %x %x %x %x %x %x %x\n",
        internel_write[0], internel_write[1], internel_write[2], internel_write[3], internel_write[4], internel_write[5], internel_write[6], internel_write[15], internel_write[16]);
        /* enable the anymotion the bit */
        internel_write[16] = 0xb8;
        ERROR_PRINT("internel_write last  %x %x %x %x %x %x %x %x %x\n",
        internel_write[0], internel_write[1], internel_write[2], internel_write[3], internel_write[4], internel_write[5], internel_write[6], internel_write[15], internel_write[16]);
        /* Write to feature config registers */
        spiQueueWriteBlock(internel_write, (16 + 1), 1000);
#endif

        mTask.init_state = INIT_DONE;
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask, "sensorInit INIT_ONC");
        break;

    default:
        INFO_PRINT("Invalid init_state.\n");
    }
}

static void handleSpiDoneEvt(const void* evtData)
{
    TDECL();
    struct BMI26XSensor *mSensor;
    uint64_t SensorTime;
    int16_t temperature16;
    uint16_t polling_length = 0;
    int i;
    bool returnIdle = false;
#if !defined(NO_NEED_ANYMOTION_FOR_VIVO)
    union EmbeddedDataPoint trigger_axies;
#endif

    uint8_t int_status_0 = 0;
    uint8_t int_status_1 = 0;
    uint16_t fifoLength = 0;

    switch (GET_STATE()) {
    case SENSOR_BOOT:
        SET_STATE(SENSOR_VERIFY_ID);
        /* dummy reads after soft reset for switch SPI bus, wait 100us */
        SPI_READ(BMI2_CHIP_ID_ADDR, 2, &mTask.dataBuffer, 100);
        /* right now the bus is SPI bus, get chip id again */
        SPI_READ(BMI2_CHIP_ID_ADDR, 2, &mTask.dataBuffer, 100);
        spiBatchTxRx(&mTask.mode, sensorSpiCallback, &mTask, "spiDone SENSOR_BOOT");
        break;

    case SENSOR_VERIFY_ID:
        DEBUG_PRINT("CID : %02x\n", mTask.dataBuffer[0]);
        DEBUG_PRINT("CID : %02x\n", mTask.dataBuffer[1]);
        INFO_PRINT("CID : %02x\n", mTask.dataBuffer[2]);
        DEBUG_PRINT("CID : %02x\n", mTask.dataBuffer[3]);

        if (mTask.dataBuffer[2] != BMI26X_ID) {
            ERROR_PRINT("chip id : %02x\n", mTask.dataBuffer[2]);
            T(mRetryLeft) --;
            ERROR_PRINT("failed id match: %02x\n", mTask.dataBuffer[2]);
            if (T(mRetryLeft) == 0)
                break;
            // For some reason the first ID read will fail to get the
            // correct value. need to retry a few times.
            SET_STATE(SENSOR_BOOT);
            if (timTimerSet(100000000, 100, 100, sensorTimerCallback, NULL, true) == 0)
                ERROR_PRINT("Couldn't get a timer to verify ID\n");
            break;
        } else {
            INFO_PRINT("BMI26X chip detected \n");
            SET_STATE(SENSOR_INITIALIZING);
            mTask.init_state = RESET_BMI26X;
            sensorInit();
            break;
        }
    case SENSOR_INITIALIZING:
        if (mTask.init_state == INIT_DONE) {
            INFO_PRINT("Done initialzing, system IDLE\n");
            for (i=0; i<NUM_OF_SENSOR; i++)
                (void)sensorRegisterInitComplete(mTask.sensors[i].handle);
            // In case other tasks have already requested us before we finish booting up.
            returnIdle = true;
        } else {
            sensorInit();
        }
        break;
    case SENSOR_POWERING_UP:
        mSensor = (struct BMI26XSensor *)evtData;
#if !defined(NO_NEED_ANYMOTION_FOR_VIVO)
        if (mSensor->idx >= FIRST_ONESHOT_SENSOR && ++mTask.active_oneshot_sensor_cnt == 1) {
            // if this is the first one-shot sensor to enable, we need
            // to request the accel at 50Hz.
            (void)sensorRequest(mTask.tid, mTask.sensors[ACC].handle, SENSOR_HZ(50), SENSOR_LATENCY_NODATA);
            //DEBUG_PRINT("oneshot on\n");
        }
#endif
        (void)sensorSignalInternalEvt(mSensor->handle, SENSOR_INTERNAL_EVT_POWER_STATE_CHG, 1, 0);
        returnIdle = true;
        break;
    case SENSOR_POWERING_DOWN:
        mSensor = (struct BMI26XSensor *)evtData;
#if !defined(NO_NEED_ANYMOTION_FOR_VIVO)
        if (mSensor->idx >= FIRST_ONESHOT_SENSOR && --mTask.active_oneshot_sensor_cnt == 0) {
            // if this is the last one-shot sensor to disable, we need to
            // release the accel.
            (void)sensorRelease(mTask.tid, mTask.sensors[ACC].handle);
            //DEBUG_PRINT("oneshot off\n");
        }
#endif
        (void)sensorSignalInternalEvt(mSensor->handle, SENSOR_INTERNAL_EVT_POWER_STATE_CHG, 0, 0);

        if (mTask.pending_dispatch) {
            mTask.pending_dispatch = false;
            dispatchData();
        }
        returnIdle = true;
        break;
    case SENSOR_INT_1_HANDLING:
    	int_status_0 = mTask.intStatus[2];
       	int_status_1 = mTask.intStatus[3];
    	fifoLength = (((mTask.intStatus[11] & 0x3F)<< 8) | (mTask.intStatus[10]));

        DEBUG_PRINT("status10x%x status2 0x%x fiflength %d\n", int_status_0, int_status_1, fifoLength);
#if !defined(NO_NEED_ANYMOTION_FOR_VIVO)
        /* check if the anymotion trigger */
        if ((int_status_0 & INT_ANY_MOTION) && mTask.sensors[ANYMO].powered) {
            trigger_axies.idata = 7;
            ERROR_PRINT("Detected any motion\n");
            (void)osEnqueueEvt(EVT_SENSOR_ANY_MOTION, trigger_axies.vptr, NULL);
        }
#endif
        /* check if the watermark trigger */
#if !defined(NO_NEED_ANYMOTION_FOR_VIVO)
        if ((int_status_1 & INT_FIFO_WM) || (fifoLength >= mTask.watermark)) {
#else
        if ((int_status_1 & INT_FIFO_WM) || (fifoLength > 0)) {
#endif
            initiateFifoRead(false /*isInterruptContext*/);
        }
        returnIdle = true;
        break;
    case SENSOR_INT_1_HANDLING_DATA:
        dispatchData();
        sendFlushEvt();
        returnIdle = true;
        break;
    case SENSOR_INT_2_HANDLING:
        int2Handling();
        returnIdle = true;
        break;
    case SENSOR_CONFIG_CHANGING:
        mSensor = (struct BMI26XSensor *)evtData;
        (void)sensorSignalInternalEvt(mSensor->handle,
                SENSOR_INTERNAL_EVT_RATE_CHG, mSensor->rate, mSensor->latency);

        if (mTask.pending_dispatch) {
            mTask.pending_dispatch = false;
            dispatchData();
        }

        returnIdle = true;
        break;
    case SENSOR_CALIBRATING:
        mSensor = (struct BMI26XSensor *)evtData;
        if (mTask.calibration_state == CALIBRATION_DONE) {
            DEBUG_PRINT("DONE calibration\n");
            returnIdle = true;
        } else if (mTask.calibration_state == CALIBRATION_TIMEOUT) {
            DEBUG_PRINT("Calibration TIMED OUT\n");
            sendCalibrationResult(SENSOR_APP_EVT_STATUS_ERROR,
                    (mSensor->idx == ACC) ? SENS_TYPE_ACCEL : SENS_TYPE_GYRO, 0, 0, 0);
            returnIdle = true;
        } else if (mSensor->idx == ACC) {
            accCalibrationHandling();
        } else if (mSensor->idx == GYR) {
            gyrCalibrationHandling();
        }
        break;
    case SENSOR_TESTING:
        mSensor = (struct BMI26XSensor *)evtData;
        if (mSensor->idx == ACC) {
            if (mTask.acc_test_state == ACC_TEST_DONE) {
                returnIdle = true;
            } else {
                accTestHandling();
            }
        } else if (mSensor->idx == GYR) {
            if (mTask.gyro_test_state == GYRO_TEST_DONE) {
                returnIdle = true;
            } else {
                gyroTestHandling();
            }
        }
        break;
    case SENSOR_TIME_SYNC:
        SensorTime = parseSensortime(mTask.sensorTimeBuffer[2] |
                (mTask.sensorTimeBuffer[3] << 8) | (mTask.sensorTimeBuffer[4] << 16));
        map_sensortime_to_rtc_time(SensorTime, mTask.timesync_rtc_time);

        temperature16 = (mTask.temperatureBuffer[2] | (mTask.temperatureBuffer[3] << 8));
        if (temperature16 == (int16_t)0x8000) {
            mTask.tempCelsius = kTempInvalid;
        } else {
            mTask.tempCelsius = 23.0f + temperature16 * kScale_temp;
            mTask.tempTime = sensorGetTime();
        }
        polling_length = (mTask.temperatureBuffer[4] | ((mTask.temperatureBuffer[5] & 0x3F) << 8));
#if defined(BM260_HEART_BEAT_TIMER)
        //heart beat timer
        if ((polling_length > 2000)) {
             osLog(LOG_ERROR, "datalength is %d, this have risk\n", polling_length);
            mTask.need_fifo_clean = 1;
        }
        if (mTask.demp_reg == 1) {
            osLog(LOG_ERROR, "dump reg addr:40-49, %x %x %x %x %x %x %x %x %x %x\n",
                mTask.reg40_49[2], mTask.reg40_49[3], mTask.reg40_49[4], mTask.reg40_49[5], mTask.reg40_49[6],
                mTask.reg40_49[7], mTask.reg40_49[8], mTask.reg40_49[9], mTask.reg40_49[10], mTask.reg40_49[11]);
            osLog(LOG_ERROR, "dump reg addr:56-58, 0x%x 0x%x 0x%x\n",
                mTask.reg56_58[2], mTask.reg56_58[3], mTask.reg56_58[4]);
            osLog(LOG_ERROR, "dump reg addr:7c-7d, 0x%x 0x%x\n",
                mTask.reg7c_7d[2], mTask.reg7c_7d[3]);
            osLog(LOG_ERROR, "dump reg addr: 1c 21 %x %x %x %x %x %x\n",
                (mTask.reg1c_21[2]), mTask.reg1c_21[3], mTask.reg1c_21[4], mTask.reg1c_21[5], mTask.reg1c_21[6], mTask.reg1c_21[7]);
            mTask.demp_reg = 0;
        }
#endif


        if (mTask.active_poll_generation == mTask.poll_generation) {
            // attach the generation number to event
            if (timTimerSet(kTimeSyncPeriodNs, 100, 100, timeSyncCallback,
                    (void *)mTask.poll_generation, true) == 0)
                ERROR_PRINT("Couldn't get a timer for time sync\n");
        }

        returnIdle = true;
        break;
    case SENSOR_SAVE_CALIBRATION:
        DEBUG_PRINT("SENSOR_SAVE_CALIBRATION: %02x %02x %02x %02x %02x %02x %02x\n",
                mTask.dataBuffer[1], mTask.dataBuffer[2], mTask.dataBuffer[3], mTask.dataBuffer[4],
                mTask.dataBuffer[5], mTask.dataBuffer[6], mTask.dataBuffer[7]);
        returnIdle = true;
        break;
    default:
        break;
    }

    if (returnIdle) {
        SET_STATE(SENSOR_IDLE);
        processPendingEvt();
    }
}

#ifdef BMI26X_USE_I2C
static void i2cCallback(void *cookie, size_t tx, size_t rx, int err);

/* delayed callback */
static void i2cDelayCallback(uint32_t timerId, void *data)
{
    i2cCallback(data, 0, 0, 0);
}

static void i2cCallback(void *cookie, size_t tx, size_t rx, int err)
{
    TDECL();
    uint8_t reg = T(cReg) - 1;
    uint32_t delay;

    if (err != 0) {
        ERROR_PRINT("i2c error (tx: %d, rx: %d, err: %d)\n", tx, rx, err);
    } else { /* delay callback if it is the case */
        delay = T(packets[reg]).delay;
        T(packets[reg]).delay = 0;
        if (delay > 0) {
            if (timTimerSet(delay, 0, 50, i2cDelayCallback, cookie, true))
                return;
            ERROR_PRINT("Cannot do delayed i2cCallback\n");
            err = -ENOMEM;
        }
    }
    i2cBatchTxRx(cookie, err);
}

static void i2cBatchTxRx(void *evtData, int err)
{
    TDECL();
    uint8_t *txBuf;
    uint8_t *rxBuf;
    uint16_t size;
    uint8_t reg;

    reg = T(cReg)++;
    if (err || (reg >= T(mRegCnt))) // No more packets
        goto i2c_batch_end;

    // Setup i2c op for next packet
    txBuf = (uint8_t *)T(packets[reg]).txBuf;

    size = T(packets[reg]).size;

    if (txBuf[0] & BMI26X_SPI_READ) { // Read op
    	/* Remove the SPI read command bit to get
    	 * the actual register address accessed */
    	txBuf[0] = txBuf[0] & (~BMI26X_SPI_READ);

#ifdef BMI263
    	rxBuf = (uint8_t *)T(packets[reg]).rxBuf;

#else
    	/* In General I2C doesnot have any dummy byte but the nano-app is made
    	 * to work in both SPI and I2C */
    	rxBuf = (uint8_t *)T(packets[reg]).rxBuf + 2;//TODO:CHECK 2is correct
    	size = size - 2;
#endif

#ifdef BMI26X_USE_I3C
        err = i3cMasterTxRx(BMI26X_I3C_BUS_ID, mTask.i3cHandle, txBuf, 1, rxBuf, size, i2cCallback, evtData);
    } else { /* Write transaction */
        err = i3cMasterTx(BMI26X_I3C_BUS_ID, mTask.i3cHandle, txBuf, size, i2cCallback, evtData);
    }
#else
        err = i2cMasterTxRx(BMI26X_I2C_BUS_ID, BMI26X_I2C_ADDR, txBuf, 1, rxBuf, size, i2cCallback, evtData);
    } else { /* Write transaction */
    	err = i2cMasterTx(BMI26X_I2C_BUS_ID, BMI26X_I2C_ADDR, txBuf, size, i2cCallback, evtData);
    }
#endif

    if (!err)
        return;
    ERROR_PRINT("%s: [0x%x] (err: %d)\n", __func__, txBuf[0], err);

i2c_batch_end:
    T(mRegCnt) = 0;
    if (T(sCallback))
        T(sCallback)((void *)evtData, err);
}
#endif

static void handleEvent(uint32_t evtType, const void* evtData)
{
    TDECL();
    uint64_t currTime;
    uint8_t *packet;
    float newMagBias;

    switch (evtType) {
    case EVT_APP_START:
        SET_STATE(SENSOR_BOOT);
        T(mRetryLeft) = RETRY_CNT_ID;
        (void)osEventUnsubscribe(mTask.tid, EVT_APP_START);
        // wait 100ms for sensor to boot
        currTime = timGetTime();
        if (currTime < 100000000ULL) {
            if (timTimerSet(100000000 - currTime, 100, 100, sensorTimerCallback, NULL, true) == 0)
                ERROR_PRINT("Couldn't get a timer for boot delay\n");

            break;
        }
        /* We have already been powered on long enough - fall through */
    case EVT_SPI_DONE:
        handleSpiDoneEvt(evtData);
        break;
    case EVT_APP_FROM_HOST:
        packet = (uint8_t*)evtData;
        if (packet[0] == sizeof(float)) {
            memcpy(&newMagBias, packet+1, sizeof(float));
            mTask.last_charging_bias_x = newMagBias;
            mTask.magBiasPosted = false;
        }
        break;
    case EVT_SENSOR_INTERRUPT_1:
        initiateFifoRead(false /*isInterruptContext*/);
        break;
    case EVT_SENSOR_INTERRUPT_2:
        int2Evt();
        break;
    case EVT_TIME_SYNC:
        timeSyncEvt((uint32_t)evtData, true);
		break;
    default:
        break;
    }
}

static void initSensorStruct(struct BMI26XSensor *sensor, enum SensorIndex idx)
{
    sensor->idx = idx;
    sensor->powered = false;
    sensor->configed = false;
    sensor->rate = 0;
    sensor->offset[0] = 0;
    sensor->offset[1] = 0;
    sensor->offset[2] = 0;
    sensor->latency = 0;
    sensor->data_evt = NULL;
    sensor->flush = 0;
    sensor->prev_rtc_time = 0;
}

static bool startTask(uint32_t task_id)
{
    TDECL();
    enum SensorIndex i;
    size_t slabSize;

/*lint -e572 */
	DEBUG_PRINT("BMI26X IMU v%d.%d.%d.%d TID: %lu\n", (BMI2XY_CONTEXT_HUB_DRIVER_VER & 0xF000) >> 12,
			((uint16_t)(BMI2XY_CONTEXT_HUB_DRIVER_VER & 0x0F00)) >> 8, ((uint16_t)(BMI2XY_CONTEXT_HUB_DRIVER_VER & 0x00F0)) >> 4,
			BMI2XY_CONTEXT_HUB_DRIVER_VER & 0x000F, task_id);
/*lint +e572 */

    time_init();
    T(tid) = task_id;
    T(Int1) = gpioRequest(BMI26X_INT1_PIN);
    T(Irq1) = BMI26X_INT1_IRQ;
    T(Isr1).func = bmi26xIsr1;
#if (SUPPORT_ACC_GYR_INT_PIN2)
    T(Int2) = gpioRequest(BMI26X_INT2_PIN);
    T(Irq2) = BMI26X_INT2_IRQ;
    T(Isr2).func = bmi26xIsr2;
#endif
    T(pending_int[0]) = false;
    T(pending_int[1]) = false;
    T(pending_step_cnt) = false;
    T(pending_dispatch) = false;
    T(frame_sensortime_valid) = false;
    T(poll_generation) = 0;
    T(tempCelsius) = kTempInvalid;
    T(tempTime) = 0;

    T(mode).speed = BMI26X_SPI_SPEED_HZ;
    T(mode).bitsPerWord = 8;
    T(mode).cpol = SPI_CPOL_IDLE_HI;
    T(mode).cpha = SPI_CPHA_TRAILING_EDGE;
    T(mode).nssChange = true;
    T(mode).format = SPI_FORMAT_MSB_FIRST;
#ifndef EXYNOS_CONTEXTHUB
    T(cs) = GPIO_PB(12);
#endif

    T(watermark) = 0;

#ifdef BMI26X_USE_I2C
    #ifdef BMI26X_USE_I3C
    mTask.i3cHandle = i3cMasterRequest(BMI26X_I3C_BUS_ID, &mI3cPid);
    #else
    i2cMasterRequest(BMI26X_I2C_BUS_ID, BMI26X_I2C_SPEED);
    DEBUG_PRINT("Requested I2C on bus #%d @%dHz, int1 on gpio#%d int2 on gpio#%d \n",
            BMI26X_I2C_BUS_ID, BMI26X_I2C_SPEED, BMI26X_INT1_PIN, BMI26X_INT2_PIN);
    #endif
#else
    (void)spiMasterRequest(BMI26X_SPI_BUS_ID, &T(spiDev));

#if (SUPPORT_ACC_GYR_INT_PIN2)
    DEBUG_PRINT("Requested SPI on bus #%d @%dHz, int1 on gpio#%d int2 on gpio#%d \n",
    		BMI26X_SPI_BUS_ID, BMI26X_SPI_SPEED_HZ, BMI26X_INT1_PIN, BMI26X_INT2_PIN);
#else
    DEBUG_PRINT("Requested SPI on bus #%d @%dHz, int1 on gpio#%d \n",
            BMI26X_SPI_BUS_ID, BMI26X_SPI_SPEED_HZ, BMI26X_INT1_PIN);
#endif

#endif


    for (i = FIRST_CONT_SENSOR; i < NUM_OF_SENSOR; i++) {
        initSensorStruct(&T(sensors[i]), i);
        T(sensors[i]).handle = sensorRegister(&mSensorInfo[i], &mSensorOps[i], NULL, false);
        T(pending_config[i]) = false;
    }

    (void)osEventSubscribe(mTask.tid, EVT_APP_START);

#ifdef ACCEL_CAL_ENABLED
    // Initializes the accelerometer offset calibration algorithm.
    const struct AccelCalParameters accel_cal_parameters = {
        MSEC_TO_NANOS(800),  // t0
        5,                   // n_s
        15,                  // fx
        15,                  // fxb
        15,                  // fy
        15,                  // fyb
        15,                  // fz
        15,                  // fzb
        15,                  // fle
        0.00025f             // th
    };
    accelCalInit(&mTask.acc, &accel_cal_parameters);
#endif  // ACCEL_CAL_ENABLED

#ifdef GYRO_CAL_ENABLED
    // Initializes the gyroscope offset calibration algorithm.
    const struct GyroCalParameters gyro_cal_parameters = {
        SEC_TO_NANOS(5),      // min_still_duration_nanos
        SEC_TO_NANOS(5.9f),   // max_still_duration_nanos [see, NOTE 1]
        0,                    // calibration_time_nanos
        SEC_TO_NANOS(1.5f),   // window_time_duration_nanos
        0,                    // bias_x
        0,                    // bias_y
        0,                    // bias_z
        0.95f,                // stillness_threshold
        MDEG_TO_RAD * 40.0f,  // stillness_mean_delta_limit [rad/sec]
        7.5e-5f,              // gyro_var_threshold [rad/sec]^2
        1.5e-5f,              // gyro_confidence_delta [rad/sec]^2
        4.5e-3f,              // accel_var_threshold [m/sec^2]^2
        9.0e-4f,              // accel_confidence_delta [m/sec^2]^2
        5.0f,                 // mag_var_threshold [uTesla]^2
        1.0f,                 // mag_confidence_delta [uTesla]^2
        1.5f,                 // temperature_delta_limit_celsius
        true                  // gyro_calibration_enable
    };
    // [NOTE 1]: 'max_still_duration_nanos' is set to 5.9 seconds to achieve a
    // max stillness period of 6.0 seconds and avoid buffer boundary conditions
    // that could push the max stillness to the next multiple of the analysis
    // window length (i.e., 7.5 seconds).
    gyroCalInit(&mTask.gyro_cal, &gyro_cal_parameters);

#ifdef OVERTEMPCAL_ENABLED
    // Initializes the gyroscope over-temperature offset compensation algorithm.
    const struct OverTempCalParameters gyro_otc_parameters = {
        MSEC_TO_NANOS(500),    // min_temp_update_period_nanos
        DAYS_TO_NANOS(2),      // age_limit_nanos
        0.75f,                 // delta_temp_per_bin
        40.0f * MDEG_TO_RAD,   // jump_tolerance
        50.0f * MDEG_TO_RAD,   // outlier_limit
        80.0f * MDEG_TO_RAD,   // temp_sensitivity_limit
        3.0e3f * MDEG_TO_RAD,  // sensor_intercept_limit
        0.1f * MDEG_TO_RAD,    // significant_offset_change
        5,                     // min_num_model_pts
        true                   // over_temp_enable
    };
    overTempCalInit(&mTask.over_temp_gyro_cal, &gyro_otc_parameters);

#endif  // OVERTEMPCAL_ENABLED
#endif  // GYRO_CAL_ENABLED


    slabSize = sizeof(struct TripleAxisDataEvent) +
               MAX_NUM_COMMS_EVENT_SAMPLES * sizeof(struct TripleAxisDataPoint);

    // each event has 15 samples, with 7 bytes per sample from the fifo.
    // the fifo size is 1K.
    // 20 slabs because some slabs may only hold 1-2 samples.
    // XXX: this consumes too much memeory, need to optimize
    T(mDataSlab) = slabAllocatorNew(slabSize, 4, 20);
    if (!T(mDataSlab)) {
        ERROR_PRINT("slabAllocatorNew() failed\n");
        return false;
    }
    T(mWbufCnt) = 0;
    T(mRegCnt) = 0;
#ifdef BMI26X_USE_I2C
    T(cReg) = 0;
#endif
    T(spiInUse) = false;

    T(interrupt_enable_0) = 0x00;
    T(interrupt_enable_2) = 0x00;

    // initialize the last bmi26x time to be ULONG_MAX, so that we know it's
    // not valid yet.
    T(last_sensortime) = 0;
    T(frame_sensortime) = ULONG_LONG_MAX;

#ifdef EXYNOS_CONTEXTHUB
    // it's ok to leave interrupt open all the time.
    (void)enableInterrupt(T(Int1), &T(Isr1));
    #if (SUPPORT_ACC_GYR_INT_PIN2)
    (void)enableInterrupt(T(Int2), &T(Isr2));
    #endif

    ipc_set_sensor_info(SENSOR_TYPE_GYROSCOPE, "BMI263 Gyro", VENDOR_NAME_BOSCH, 0, 0);
    ipc_set_sensor_info(SENSOR_TYPE_GYROSCOPE_UNCALIBRATED, "BMI263 Gyro(uncal)", VENDOR_NAME_BOSCH, 0, 0);
    ipc_set_sensor_info(SENSOR_TYPE_ACCELEROMETER, "BMI263 Accel", VENDOR_NAME_BOSCH, 0, 0);
    ipc_set_sensor_info(SENSOR_TYPE_ACCELEROMETER_UNCALIBRATED, "BMI263 Accel(uncal)", VENDOR_NAME_BOSCH, 0, 0);
#else
    (void)enableInterrupt(T(Int1), T(Irq1), &T(Isr1));
    (void)enableInterrupt(T(Int2), T(Irq2), &T(Isr2));
#endif

    return true;
}

static void endTask(void)
{
    TDECL();
#ifdef ACCEL_CAL_ENABLED
    accelCalDestroy(&mTask.acc);
#endif
    slabAllocatorDestroy(T(mDataSlab));
#ifndef BMI26X_USE_I2C
    (void)spiMasterRelease(mTask.spiDev);
#endif
    // disable and release interrupt.
    (void)disableInterrupt(mTask.Int1, mTask.Irq1, &mTask.Isr1);
#if (SUPPORT_ACC_GYR_INT_PIN2)
    (void)disableInterrupt(mTask.Int2, mTask.Irq2, &mTask.Isr2);
#endif
    gpioRelease(mTask.Int1);
    gpioRelease(mTask.Int2);
}

/**
 * Parse BMI26X FIFO frame without side effect.
 *
 * The major purpose of this function is to determine if FIFO content is received completely (start
 * to see invalid headers). If not, return the pointer to the beginning last incomplete frame so
 * additional read can use this pointer as start of read buffer.
 *
 * @param buf  buffer location
 * @param size size of data to be parsed
 *
 * @return NULL if the FIFO is received completely; or pointer to the beginning of last incomplete
 * frame for additional read.
 */
static uint8_t* shallowParseFrame(uint8_t * buf, int size) {
    int i = 0;
    int iLastFrame = 0; // last valid frame header index

    DEBUG_PRINT_IF(DBG_SHALLOW_PARSE, "spf start %p: %x %x %x\n", buf, buf[0], buf[1], buf[2]);
    while (size > 0) {
        int fh_mode, fh_param;
        iLastFrame = i;

        if (buf[i] == BMI26X_FRAME_HEADER_INVALID) {
            // no more data
            DEBUG_PRINT_IF(DBG_SHALLOW_PARSE, "spf:at%d=0x80\n", iLastFrame);
            return NULL;
        } else if (buf[i] == BMI26X_FRAME_HEADER_SKIP) {
            // artifically added nop frame header, skip
            DEBUG_PRINT_IF(DBG_SHALLOW_PARSE, "at %d, skip header\n", i);
            i++;
            size--;
            continue;
        }

        //++frame_num;

        fh_mode = buf[i] >> 6;
        fh_param = (buf[i] >> 2) & 0xf;

        i++;
        size--;

        if (fh_mode == 1) {
            // control frame.
            if (fh_param == 0) {
                // skip frame, we skip it (1 byte)
                i++;
                size--;
                DEBUG_PRINT_IF(DBG_SHALLOW_PARSE, "at %d, a skip frame\n", iLastFrame);
            } else if (fh_param == 1) {
                // sensortime frame  (3 bytes)
                i += 3;
                size -= 3;
                DEBUG_PRINT_IF(DBG_SHALLOW_PARSE, "at %d, a sensor_time frame\n", iLastFrame);
            } else if (fh_param == 2) {
                // fifo_input config frame (4byte)
                i = i + 4;
                size -=4;
                DEBUG_PRINT_IF(DBG_SHALLOW_PARSE, "at %d, a fifo cfg frame\n", iLastFrame);
            } else {
                size = 0; // drop this batch
                DEBUG_PRINT_IF(DBG_SHALLOW_PARSE, "Invalid fh_param in control frame!!\n");
                // mark invalid
                buf[iLastFrame] = BMI26X_FRAME_HEADER_INVALID;
                return NULL;
            }
        } else if (fh_mode == 2) {
            // regular frame, dispatch data to each sensor's own fifo
            if (fh_param & 4) { // have mag data
                i += 8;
                size -= 8;
            }
            if (fh_param & 2) { // have gyro data
                i += 6;
                size -= 6;
            }
            if (fh_param & 1) { // have accel data
                i += 6;
                size -= 6;
            }
            DEBUG_PRINT_IF(DBG_SHALLOW_PARSE, "at %d, a reg frame acc %d, gyro %d, mag %d\n",
                       iLastFrame, fh_param &1 ? 1:0, fh_param&2?1:0, fh_param&4?1:0);
        } else {
            size = 0; // drop the rest of batch
            DEBUG_PRINT_IF(DBG_SHALLOW_PARSE, "spf: Invalid fh_mode %d!!\n", fh_mode);
            //mark invalid
            buf[iLastFrame] = BMI26X_FRAME_HEADER_INVALID;
            return NULL;
        }
    }

    // there is a partial frame, return where to write next chunck of data
    DEBUG_PRINT_IF(DBG_SHALLOW_PARSE, "partial frame ends %p\n", buf + iLastFrame);
    return buf + iLastFrame;
}

/**
 * Intialize the first read of chunked SPI read sequence.
 *
 * @param read_index starting read_index of the txrxBuffer in which the data will be write into.
 */
static void chunkedReadInit_(TASK, int read_index, int size) {

    if (GET_STATE() != SENSOR_INT_1_HANDLING_DATA) {
        ERROR_PRINT("chunkedReadInit in wrong mode");
        return;
    }

    if (T(mRegCnt)) {
        //chunked read are always executed as a single command. This should never happen.
        ERROR_PRINT("SPI queue not empty at chunkedReadInit, regcnt = %d", T(mRegCnt));
        // In case it did happen, we do not want to write crap to BMI26X.
        T(mRegCnt) = 0;
    }

    T(mWbufCnt) = read_index;
    if (T(mWbufCnt) > FIFO_READ_SIZE) {
        // drop data to prevent bigger issue
        T(mWbufCnt) = 0;
    }
    T(chunkReadSize) = size > CHUNKED_READ_SIZE ? size : CHUNKED_READ_SIZE;

    DEBUG_PRINT_IF(DBG_CHUNKED, "crd %d>>%d\n", T(chunkReadSize), read_index);
    SPI_READ(BMI2_FIFO_DATA_ADDR, T(chunkReadSize), &T(dataBuffer));
    spiBatchTxRx(&T(mode), chunkedReadSpiCallback, _task, __FUNCTION__);
}

/**
 * Chunked SPI read callback.
 *
 * Handles the chunked read logic: issue additional read if necessary, or calls sensorSpiCallback()
 * if the entire FIFO is read.
 *
 * @param cookie extra data
 * @param err    error
 *
 * @see sensorSpiCallback()
 */
static void chunkedReadSpiCallback(void *cookie, int err) {
    TASK = (_Task*) cookie;

    T(spiInUse) = false;
    DEBUG_PRINT_IF(err !=0 || GET_STATE() != SENSOR_INT_1_HANDLING_DATA,
            "crcb,e:%d,s:%d", err, (int)GET_STATE());
    bool int1 = gpioGet(T(Int1));
    if (err != 0) {
        DEBUG_PRINT_IF(DBG_CHUNKED, "spi err, crd retry");
        // read full fifo length to be safe
        chunkedReadInit(0, FIFO_READ_SIZE);
        return;
    }

    *T(dataBuffer) = BMI26X_FRAME_HEADER_SKIP;/* fill the 0x00/0xff hole at the first byte */
    *(T(dataBuffer) + 1) = BMI26X_FRAME_HEADER_SKIP;/* fill the 0x00/0xff hole at the second byte */
    uint8_t* end = shallowParseFrame(T(dataBuffer), T(chunkReadSize));

    if (end == NULL) {
        // if interrupt is still set after read for some reason, set the pending interrupt
        // to handle it immediately after data is handled.
        T(pending_int[0]) = T(pending_int[0]) || int1;

        // recover the buffer and valid data size to make it looks like a single read so that
        // real frame parse works properly
        T(dataBuffer) = T(txrxBuffer);
        T(xferCnt) = FIFO_READ_SIZE;
        sensorSpiCallback(cookie, err);
    } else {
        DEBUG_PRINT_IF(DBG_CHUNKED, "crd cont");
        chunkedReadInit(end - T(txrxBuffer), CHUNKED_READ_SIZE);
    }
}

/**
 * Initiate read of sensor fifo.
 *
 * If task is in idle state, init chunked FIFO read; otherwise, submit an interrupt message or mark
 * the read pending depending if it is called in interrupt context.
 *
 * @param isInterruptContext true if called from interrupt context; false otherwise.
 *
 */
static void initiateFifoRead_(TASK, bool isInterruptContext) {
    if (trySwitchState(SENSOR_INT_1_HANDLING_DATA)) {
        // estimate first read size to be watermark + 1 more sample + some extra
        int firstReadSize = T(watermark) + 32; // 1+6+6+8+1+3 + extra = 25 + extra = 32
        if (firstReadSize < CHUNKED_READ_SIZE) {
            firstReadSize = CHUNKED_READ_SIZE;
        }
        chunkedReadInit(0, firstReadSize);
    } else {
        if (isInterruptContext) {
            // called from interrupt context, queue event
            if (!osEnqueuePrivateEvt(EVT_SENSOR_INTERRUPT_1, _task, NULL, T(tid)))
                ERROR_PRINT("initiateFifoRead_: osEnqueuePrivateEvt() failed\n");
        } else {
            // non-interrupt context, set pending flag, so next time it will be picked up after
            // switching back to idle.
            // Note: even if we are still in SENSOR_INT_1_HANDLING, the SPI may already finished and
            // we need to issue another SPI read to get the latest status.
            T(pending_int[0]) = true;
        }
    }
}

/**
 * Calculate fifo size using normalized input.
 *
 * @param iPeriod normalized period vector
 * @param iLatency normalized latency vector
 * @param factor vector that contains size factor for each sensor
 * @param n size of the vectors
 *
 * @return max size of FIFO to guarantee latency requirements of all sensors or SIZE_MAX if no
 * sensor is active.
 */
static size_t calcFifoSize(const int* iPeriod, const int* iLatency, const int* factor, int n) {
    int i;

    int minLatency = INT_MAX;
    for (i = 0; i < n; i++) {
        if (iLatency[i] > 0) {
            minLatency = iLatency[i] < minLatency ? iLatency[i] : minLatency;
        }
    }
    //minLatency = 1.4 for 0dr400HZ
    DEBUG_PRINT_IF(DBG_WM_CALC, "cfifo: min latency %d unit", minLatency);

    bool anyActive = false;
    size_t s = 0;
    size_t head = 0;
    for (i = 0; i < n; i++) {
        if (iPeriod[i] > 0) {
            anyActive = true;
            size_t t = minLatency / iPeriod[i];//1.4/1
            head = t > head ? t : head;//head =1
            s += t * factor[i];//s = 1*6 + 0 = 6;
            DEBUG_PRINT_IF(DBG_WM_CALC, "cfifo %d: s += %d * %d, head = %d", i, t, factor[i], head);
        }
    }

    return anyActive ? head + s : SIZE_MAX;
}

/**
 * Calculate the watermark setting from sensor registration information
 *
 * It is assumed  that all sensor period share a common denominator (true for BMI26X) and the
 * latency of sensor will be lower bounded by its sampling period.
 *
 * @return watermark register setting
 */
static uint16_t calcWatermark2_(TASK) {
    int period[] = {-1, -1};
    int latency[] = {-1, -1};
    int factor[] = {6, 6};
    int i;

    for (i = FIRST_CONT_SENSOR; i < NUM_CONT_SENSOR; ++i) {
        if (T(sensors[i]).configed && T(sensors[i]).latency != SENSOR_LATENCY_NODATA) {
            /*
            this is the command from framewark
            nanohub: accSetRate: rate=51200, latency=19999744, state=3
            nanohub: accSetRate: rate=409600, latency=2499584, state=3
            if only acc, and acc odr ==400HZ
            period[0] = 1;
            latency[0] = 1.4
         */
            period[i - ACC] = SENSOR_HZ((float)WATERMARK_MAX_SENSOR_RATE) / T(sensors[i]).rate;
            /*
            (2499584 + (1000000000/(400)/2)) / (1000000000/(400)
        */
/*lint -e506 */
            latency[i - ACC] = U64_DIV_BY_U64_CONSTANT(
                    T(sensors[i]).latency + WATERMARK_TIME_UNIT_NS/2, WATERMARK_TIME_UNIT_NS);
            DEBUG_PRINT_IF(DBG_WM_CALC, "cwm2 %d: f %dHz, l %dus => T %d unit, L %d unit",
                    i, (int) T(sensors[i]).rate/1024,
                    (int) U64_DIV_BY_U64_CONSTANT(T(sensors[i]).latency, 1000),
                    period[i-ACC], latency[i-ACC]);
/*lint +e506 */
            if ((T(sensors[i]).rate == 409600) && (latency[i - ACC] < 2)) {
                   factor[0] = 13;
                   factor[1] = 13;
            }
        }
    }

    size_t watermark = calcFifoSize(period, latency, factor, NUM_CONT_SENSOR);
    DEBUG_PRINT_IF(DBG_WM_CALC, "cwm2: wm = %d", watermark);
    watermark = watermark < WATERMARK_MIN ? WATERMARK_MIN : watermark;
    watermark = watermark > WATERMARK_MAX ? WATERMARK_MAX : watermark;

    return watermark;
}

static bool dumpBinaryPutC(void* p, char c) {
    *(*(char**)p)++ = c;
    return true;
}
/*lint -e530*/
static uint32_t cvprintf_ellipsis(printf_write_c writeF, void* writeD, const char* fmtStr, ...) {
    va_list vl;
    uint32_t ret;

    va_start(vl, fmtStr);
#ifdef CHRE_LATESTONE
    ret = cvprintf(writeF, 0, writeD, fmtStr, vl);
#else
    ret = cvprintf(writeF, writeD, fmtStr, vl);
#endif
    va_end(vl);

    return ret;
}
/*lint +e530*/
static void dumpBinary(void* buf, unsigned int address, size_t size) {
    size_t i, j;
    char buffer[5+16*3+1+2]; //5: address, 3:each byte+space, 1: middle space, 1: \n and \0
    char* p;

    for (i = 0; i < size; ) {
        p = buffer;
        (void)cvprintf_ellipsis(dumpBinaryPutC, &p, "%08x:", address);
        for (j = 0; j < 0x10 && i < size; ++i, ++j) {
            if (j == 0x8) {
                *p++ = ' ';
            }
            (void)cvprintf_ellipsis(dumpBinaryPutC, &p, " %02x", ((unsigned char *)buf)[i]);
        }
        *p = '\0';

        osLog(LOG_INFO, "%s\n", buffer);
        address += 0x10;
    }
}

#ifdef OVERTEMPCAL_ENABLED
static void handleOtcGyroConfig_(TASK, const struct AppToSensorHalDataPayload *data) {
    const struct GyroOtcData *d = data->gyroOtcData;

    INFO_PRINT("gyrCfgData otc-data: off %d %d %d, t %d, s %d %d %d, i %d %d %d",
            (int)(d->lastOffset[0]), (int)(d->lastOffset[1]), (int)(d->lastOffset[2]),
            (int)(d->lastTemperature),
            (int)(d->sensitivity[0]), (int)(d->sensitivity[1]), (int)(d->sensitivity[2]),
            (int)(d->intercept[0]), (int)(d->intercept[1]), (int)(d->intercept[2]));

    overTempCalSetModel(&T(over_temp_gyro_cal), d->lastOffset, d->lastTemperature,
                        sensorGetTime(), d->sensitivity, d->intercept, true /*jumpstart*/);
}

static bool sendOtcGyroUpdate_(TASK) {
    int step = 0;
    if (atomicCmpXchgByte(&T(otcGyroUpdateBuffer).lock, false, true)) {
        ++step;
        //fill HostIntfDataBuffer header
        struct HostIntfDataBuffer *p = (struct HostIntfDataBuffer *)(&T(otcGyroUpdateBuffer));
        p->sensType = SENS_TYPE_INVALID;
        p->length = sizeof(struct AppToSensorHalDataPayload) + sizeof(struct GyroOtcData);
        p->dataType = HOSTINTF_DATA_TYPE_APP_TO_SENSOR_HAL;
        p->interrupt = NANOHUB_INT_NONWAKEUP;

        //fill AppToSensorHalDataPayload header
        struct AppToSensorHalDataBuffer *q = (struct AppToSensorHalDataBuffer *)p;
        q->payload.size = sizeof(struct GyroOtcData);
        q->payload.type = HALINTF_TYPE_GYRO_OTC_DATA; // bit-or EVENT_TYPE_BIT_DISCARDABLE
                                                      // to make it discardable

        // fill payload data
        struct GyroOtcData *data = q->payload.gyroOtcData;
        uint64_t timestamp;
        overTempCalGetModel(&T(over_temp_gyro_cal), data->lastOffset, &data->lastTemperature,
                            &timestamp, data->sensitivity, data->intercept);
        if (osEnqueueEvtOrFree(EVT_APP_TO_SENSOR_HAL_DATA, // bit-or EVENT_TYPE_BIT_DISCARDABLE
                                                          // to make event discardable
                               p, unlockOtcGyroUpdateBuffer)) {
            T(otcGyroUpdateBuffer).sendToHostRequest = false;
            ++step;
        }
    }
    DEBUG_PRINT("otc gyro update, finished at step %d", step);
    return step == 2;
}

static void unlockOtcGyroUpdateBuffer(void *event) {
    atomicXchgByte(&(((struct OtcGyroUpdateBuffer*)(event))->lock), false);
}
#endif // OVERTEMPCAL_ENABLED

/*lint --e{528} */
/*lint -e485 */
INTERNAL_APP_INIT(BMI26X_APP_ID, BMI26X_APP_VERSION, startTask, endTask, handleEvent);
/*lint +e485 */
