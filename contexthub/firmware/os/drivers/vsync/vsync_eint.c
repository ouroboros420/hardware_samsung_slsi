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
#include <plat/gpio/gpio.h>
#include <gpio9610.h>
#include <heap.h>
#include <hostIntf.h>
#include <isr.h>
#include <nanohubPacket.h>
#include <sensors.h>
#include <seos.h>
#include <slab.h>
#include <timer.h>
#include <variant/variant.h>
#include <csp_printf.h>
#if defined(EXYNOS_VSYNC)
#include <csisOS.h>
#endif

#define VSYNC_APP_ID      APP_ID_MAKE(NANOHUB_VENDOR_GOOGLE, 7)
#define VSYNC_APP_VERSION 2

// This defines how many vsync events we could handle being backed up in the
// queue. Use this to size our slab
#define MAX_VSYNC_EVENTS        4
#define MAX_VSYNC_INT_LATENCY   1000 /* in ns */

#if !defined(EXYNOS_VSYNC)
#ifndef VSYNC_PIN
#error "VSYNC_PIN is not defined; please define in variant.h"
#endif

#ifndef VSYNC_IRQ
#error "VSYNC_IRQ is not defined; please define in variant.h"
#endif
#endif

#define INFO_PRINT(fmt, ...) do { \
        osLog(LOG_INFO, "%s " fmt, "[VSYNC]", ##__VA_ARGS__); \
    } while (0);

#define ERROR_PRINT(fmt, ...) INFO_PRINT("%s" fmt, "ERROR: ", ##__VA_ARGS__); \

#define DEBUG_PRINT(fmt, ...) do { \
        if (enable_debug) {  \
            INFO_PRINT(fmt, ##__VA_ARGS__); \
        } \
    } while (0);

static const bool __attribute__((unused)) enable_debug = 0;

static struct SensorTask
{
    GpioEintNumType pin0;
	GpioEintNumType pin1;
    void (*isr0) (uint32_t);
    void (*isr1) (uint32_t);

    //struct Gpio *pin;
    //struct ChainedIsr isr;
    struct SlabAllocator *evtSlab;


    uint32_t id;
    uint32_t sensorHandle;

    bool on;
} mTask;

static bool vsyncAllocateEvt(struct SingleAxisDataEvent **evPtr, uint64_t time)
{
    struct SingleAxisDataEvent *ev;

    *evPtr = slabAllocatorAlloc(mTask.evtSlab);

    ev = *evPtr;
    if (!ev) {
        ERROR_PRINT("slabAllocatorAlloc() failed\n");
        return false;
    }

    memset(&ev->samples[0].firstSample, 0x00, sizeof(struct SensorFirstSample));
    ev->referenceTime = time;
    ev->samples[0].firstSample.numSamples = 1;
    ev->samples[0].idata = 1;

    return true;
}

static void vsyncFreeEvt(void *ptr)
{
    slabAllocatorFree(mTask.evtSlab, ptr);
}

// dummy function yo avoid build error in csisos.c
extern void vsyncIsr(uint32_t csis_no);
void vsyncIsr(uint32_t csis_no) {INFO_PRINT("vsyncIsr, %lld\n", sensorGetTime());}

extern void vsync0Isr(uint32_t csis_no);
extern void vsync1Isr(uint32_t csis_no);
void vsync0Isr(uint32_t csis_no)
{
    struct SingleAxisDataEvent *ev;
    if (mTask.on) {
        if (vsyncAllocateEvt(&ev, sensorGetTime())) {
            ev->samples[0].idata = 0; // csis0
            if (!osEnqueueEvtOrFree(sensorGetMyEventType(SENS_TYPE_VSYNC), ev, vsyncFreeEvt)) {
                ERROR_PRINT("osEnqueueEvtOrFree() failed\n");
            }
        }
    }
    gpioClearPending(mTask.pin0);
}

void vsync1Isr(uint32_t csis_no)
{
    struct SingleAxisDataEvent *ev;
    if (mTask.on) {
        if (vsyncAllocateEvt(&ev, sensorGetTime())) {
            ev->samples[0].idata = 1; // csis1
            if (!osEnqueueEvtOrFree(sensorGetMyEventType(SENS_TYPE_VSYNC), ev, vsyncFreeEvt)) {
                ERROR_PRINT("osEnqueueEvtOrFree() failed\n");
            }
        }
    }
    gpioClearPending(mTask.pin1);
}

static bool enableInterrupt(GpioEintNumType pin, void (*isr)(uint32_t))
{
    gpioSetExtInterrupt(pin, Rising_Edge, 0, 0, isr); // need to check rising
    return true;
}

static bool disableInterrupt(GpioEintNumType pin)
{
	gpioUnsetExtInterrupt(pin);
    return true;
}

static const uint32_t supportedRates[] =
{
    SENSOR_RATE_ONCHANGE,
    0
};

static const struct SensorInfo mSensorInfo =
{
    .sensorName = "Camera Vsync",
    .sensorType = SENS_TYPE_VSYNC,
    .numAxis = NUM_AXIS_ONE,
    .interrupt = NANOHUB_INT_NONWAKEUP,
    .minSamples = 20,
    //.supportedRates = supportedRates, // TODO: add rate additionally..
};

static bool vsyncPower(bool on, void *cookie)
{
    INFO_PRINT("power %d\n", on);

    if (on) {
        gpioClearPending(mTask.pin0);
        gpioClearPending(mTask.pin1);
        enableInterrupt(mTask.pin0, mTask.isr0);
        enableInterrupt(mTask.pin1, mTask.isr1);
    } else {
        disableInterrupt(mTask.pin0);
        disableInterrupt(mTask.pin1);
        gpioClearPending(mTask.pin0);
        gpioClearPending(mTask.pin1);
    }

    mTask.on = on;
    sensorSignalInternalEvt(mTask.sensorHandle, SENSOR_INTERNAL_EVT_POWER_STATE_CHG, on, 0);
    return true;
}

static bool vsyncFirmwareUpload(void *cookie)
{
    return sensorSignalInternalEvt(mTask.sensorHandle, SENSOR_INTERNAL_EVT_FW_STATE_CHG, 1, 0);
}

static bool vsyncSetRate(uint32_t rate, uint64_t latency, void *cookie)
{
    INFO_PRINT("setRate\n");
    return sensorSignalInternalEvt(mTask.sensorHandle, SENSOR_INTERNAL_EVT_RATE_CHG, rate, latency);
}

static bool vsyncFlush(void *cookie)
{
    INFO_PRINT("flush\n");
    return osEnqueueEvt(sensorGetMyEventType(SENS_TYPE_VSYNC), SENSOR_DATA_EVENT_FLUSH, NULL);
}

static const struct SensorOps mSensorOps =
{
    .sensorPower = vsyncPower,
    .sensorFirmwareUpload = vsyncFirmwareUpload,
    .sensorSetRate = vsyncSetRate,
    .sensorFlush = vsyncFlush,
};

static void handleEvent(uint32_t evtType, const void* evtData)
{
}

static bool startTask(uint32_t taskId)
{
    CSP_PRINTF_INFO("Vsync starts\n");

    mTask.id = taskId;
    mTask.sensorHandle = sensorRegister(&mSensorInfo, &mSensorOps, NULL, true);
    mTask.pin0 = VSYNC0_PIN;
    mTask.isr0 = vsync0Isr;
    mTask.pin1 = VSYNC1_PIN;
    mTask.isr1 = vsync1Isr;

    mTask.evtSlab = slabAllocatorNew(sizeof(struct SingleAxisDataEvent) + sizeof(struct SingleAxisDataPoint), 4, MAX_VSYNC_EVENTS);
    if (!mTask.evtSlab) {
        ERROR_PRINT("slabAllocatorNew() failed\n");
        return false;
    }

    return true;
}

static void endTask(void)
{
    disableInterrupt(mTask.pin0);
    disableInterrupt(mTask.pin1);
    gpioClearPending(mTask.pin0);
    gpioClearPending(mTask.pin1);
    sensorUnregister(mTask.sensorHandle);
}

INTERNAL_APP_INIT(VSYNC_APP_ID, VSYNC_APP_VERSION, startTask, endTask, handleEvent);
