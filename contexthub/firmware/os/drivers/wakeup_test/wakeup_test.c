#include <nanohubPacket.h>
#include <hostIntf.h>
#include <seos.h>
#include <sensors.h>
#include <ipc_chub.h>
#include <rtc.h>
#include <rtc.h>

#define TEST_APP_VERSION 1
#define SENS_TYPE_TEST   11

#define EVT_FIRST EVT_APP_START + 1

struct testTask {
    uint32_t tid;
    uint32_t handle;
    uint32_t rate;
};

static struct testTask mTask;

static const uint32_t SupportedRates[] = {
    SENSOR_HZ(1),
    SENSOR_HZ(10),
    SENSOR_HZ(50),
    SENSOR_HZ(100),
    SENSOR_HZ(200),
    SENSOR_HZ(1000),
    0,
};

static const struct SensorInfo mSi = {
    .sensorName = "Wakeup Test",
    .supportedRates = SupportedRates,
    .sensorType = SENS_TYPE_TEST,
    .numAxis = NUM_AXIS_EMBEDDED,
    .interrupt = NANOHUB_INT_NONWAKEUP,
    .minSamples = 1
};

#define HEAVY_LOAD
#ifdef HEAVY_LOAD
static u64 calcu(void)
{
    u64 val = 0;
    u64 val2 = 0;
    u64 val3 = 0;
    u64 val4 = 0;
    u64 i;

	/* CSP_PRINTF_INFO("testStart: %s\n", __func__); */
    for (i = 0; i < 100000; i++) {
        val = (val / 10);
        val2 = i;
        val3 = (val2 + val) * 2;
        val4 += val3 / 2;
    }
    return val4;
}
#endif

static void test(void)
{
    union EmbeddedDataPoint sample;

    sample.idata = mTask.rate;

#ifdef HEAVY_LOAD
	if (mTask.rate / 1024 == 10) {
		sample.idata = calcu();
		sample.idata = calcu();
		sample.idata = calcu();
	}
#endif

    if (sample.idata == AP_WAKE) {
        osEnqueueEvt(sensorGetMyEventType(SENS_TYPE_TEST), sample.vptr, NULL);
    } else {
        osEnqueueEvt(sensorGetMyEventType(SENS_TYPE_TEST), sample.vptr, NULL);
    }
    return;
}

static void testCallback(void)
{
    osEnqueuePrivateEvt(EVT_FIRST, NULL, NULL, mTask.tid);
}

static bool setRtc(uint32_t rate)
{
    switch (rate) {
    case SENSOR_HZ(1):
        rtcSetTickTime(1, 0, 0, testCallback, 1);
        break;
    case SENSOR_HZ(10):
        rtcSetTickTime(0, 100, 0, testCallback, 1);
        break;
    case SENSOR_HZ(50):
        rtcSetTickTime(0, 20, 0, testCallback, 1);
        break;
    case SENSOR_HZ(100):
        rtcSetTickTime(0, 10, 0, testCallback, 1);
        break;
    case SENSOR_HZ(200):
        rtcSetTickTime(0, 5, 0, testCallback, 1);
        break;
    case SENSOR_HZ(1000):
        rtcSetTickTime(0, 1, 0, testCallback, 1);
        break;
    default:
        return false;
        break;
    }
    return true;
}

static bool testPower(bool on, void *cookie)
{
    CSP_PRINTF_INFO("%s %s\n", __func__, on ? "on" : "off");

    if (on)
        setRtc(mTask.rate);
    else
        rtcStopTick();

    return sensorSignalInternalEvt(mTask.handle,
                                   SENSOR_INTERNAL_EVT_POWER_STATE_CHG, on, 0);
}

static bool testSetRate(uint32_t rate, uint64_t latency, void *cookie)
{
    CSP_PRINTF_INFO("%s %lu\n", __func__, rate);
    rtcStopTick();
    mTask.rate = rate;
    setRtc(mTask.rate);
    return sensorSignalInternalEvt(mTask.handle, SENSOR_INTERNAL_EVT_RATE_CHG,
                                   rate, latency);
}

static bool testFlush(void *cookie)
{
    CSP_PRINTF_INFO("%s\n", __func__);
    return true;
}

static bool testFirmwareUpload(void *cookie)
{
    CSP_PRINTF_INFO("%s\n", __func__);
    return sensorSignalInternalEvt(mTask.handle,
                                   SENSOR_INTERNAL_EVT_FW_STATE_CHG, 1, 0);
}

static const struct SensorOps mSops = {
    .sensorPower = testPower,
    .sensorFirmwareUpload = testFirmwareUpload,
    .sensorSetRate = testSetRate,
    .sensorFlush = testFlush,
};

static bool testStart(uint32_t taskId)
{
    CSP_PRINTF_INFO("%s %lu\n", __func__, taskId);
    mTask.tid = taskId;
    mTask.handle = sensorRegister(&mSi, &mSops, NULL, true);
    return true;
}

static void testHandleEvent(uint32_t evtType, const void *evtData)
{
    //CSP_PRINTF_INFO("wake up test %lu\n", evtType);
    switch (evtType) {
    case EVT_FIRST:
        test();
        break;
    default:
        CSP_PRINTF_INFO("unknown evt %lu\n", evtType);
    }
}

static void testEnd()
{
    CSP_PRINTF_INFO("%s %lu\n", __func__, mTask.tid);
}

INTERNAL_APP_INIT(APP_ID_MAKE(NANOHUB_VENDOR_GOOGLE, 9),
                  TEST_APP_VERSION, testStart, testEnd, testHandleEvent);
