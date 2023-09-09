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

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <android/log.h>
#include <time.h>
#include <android/sensor.h>

#define PATH_TESTCASE "/data/vendor/log/chub/testcase.txt"

#undef TEST_RAND

struct SensorConfig {
    int listIndex;
    int type;
    int32_t rate;
    int64_t reportLatency;
    bool receivedEvent;
    int reporting_mode;
    unsigned long int last_timestamp;
    const char *name;
};

struct Sensor {
    int type;
    int rate;
};

struct SensorTestcase {
    int length; // seconds
    Sensor sensor[10];
};

//https://developer.android.com/reference/android/hardware/Sensor#TYPE_ACCELEROMETER

ASensorManager *mSensorManager;
ASensorList mSensorList;
int mNumSensors;
int numSensors;
bool mContinuousMode = true;
bool mTestAll = false;
SensorConfig mSensorConfigList[16];
int mNumSensorConfigs;
FILE * pFile;

int start, end;

SensorTestcase TC[20];

int get_file(void) {
    size_t len = 0;
    char *line = NULL;
    ssize_t nread;
    int i = 0;

    pFile = fopen(PATH_TESTCASE, "r");
    if (pFile != NULL) {
        while ((nread = getline(&line, &len, pFile)) != -1) {
            printf("[%d]: %s", i, line);
            //printf("%s", line);
            TC[i].length = atoi(line);
            auto linep = line;
            bool flag = true;
            int j = 0;
            while(*linep) {
                while (*linep >= '0' && *linep <= '9')
                    linep++;
                if (*linep == ' ') {
                    linep++;
                    if (flag)
                        TC[i].sensor[j].type = atoi(linep);
                    else {
                        TC[i].sensor[j].rate = atoi(linep);
                        j++;
                    }
                    flag = !flag;
                }
                else {
                    break;
                }
            }
            i++;
        }
        printf("start:");
        scanf("%d", &start);
        printf("end:");
        scanf("%d", &end);

        if (start < 0 || end < 0 || start >= i || end >= i) {
            printf("invalid number %d %d\n", start, end);
            return -1;
        }

        if (start > end) {
            int tmp = start ; start = end ; end = tmp;
        }

        printf("test start from %d to %d\n", start, end);
        free(line);
        fclose(pFile);
        return i;
    } else {
        printf("file not found, run random test\n");
        return 0;
    }
}

int findSensorTypeInSensorList(int type)
{
    for (int i = 0; i < mNumSensors; i++) {
        if (ASensor_getType(mSensorList[i]) == type) {
            return i;
        }
    }

    return -1;
}

int findSensorTypeInConfigList(int type)
{
    for (int i = 0; i < mNumSensorConfigs; i++) {
        if (mSensorConfigList[i].type == type) {
            return i;
        }
    }

    return -1;
}

void init_setting(Sensor *sensor) {
    mNumSensorConfigs = 0;
    for (int j = 0 ; sensor[j].type; j++) {
        for (int i = 0; i < mNumSensors; i++) {
            if (ASensor_getType(mSensorList[i]) == sensor[j].type) {
                mSensorConfigList[mNumSensorConfigs++] = (SensorConfig) {
                    .listIndex = i,
                    .type = ASensor_getType(mSensorList[i]),
                    .rate = sensor[j].rate ? (1000000 / sensor[j].rate) : (ASensor_getMinDelay(mSensorList[i]) ? ASensor_getMinDelay(mSensorList[i]) : 0), // fastest
                    .receivedEvent = false,
                    .reporting_mode = ASensor_getReportingMode(mSensorList[i]),
                    .name = ASensor_getName(mSensorList[i])
                };
                printf("sensor %s(%d) rate: %d mode:%d\n",
                        ASensor_getName(mSensorList[i]), ASensor_getType(mSensorList[i]),
                        1000000 / mSensorConfigList[mNumSensorConfigs-1].rate, ASensor_getReportingMode(mSensorList[i]));
                break;
            }
        }
    }
}

void init_setting_rand() {
    mNumSensorConfigs = 0;
    srand((unsigned int) time(NULL));
    for (int i = 0; i < mNumSensors; i++) {
        if (mTestAll || !(rand() % 3)) {
            mSensorConfigList[mNumSensorConfigs++] = (SensorConfig) {
                .listIndex = i,
                .type = ASensor_getType(mSensorList[i]),
                .rate = ASensor_getMinDelay(mSensorList[i]) > 0 ? ASensor_getMinDelay(mSensorList[i]) : 0, // fastest
                .receivedEvent = false,
                .reporting_mode = ASensor_getReportingMode(mSensorList[i]),
                .name = ASensor_getName(mSensorList[i])
            };
            printf("sensor %s(%d) delay: %d mode:%d\n",
                    ASensor_getName(mSensorList[i]), ASensor_getType(mSensorList[i]),
                    mSensorConfigList[mNumSensorConfigs-1].rate, ASensor_getReportingMode(mSensorList[i]));
        }
    }
}

bool hasReceivedAllEvents()
{
    for (int i = 0; i < mNumSensorConfigs; i++) {
        if (!mSensorConfigList[i].receivedEvent) {
            return false;
        }
    }

    return true;
};

int main(int argc, char **argv) {
    printf("sensor random test\n");

    int numSensorEvents;
    ASensorEvent sensorEvents[16];
    int configListIndex;
    timespec startTime, curTime;
    mSensorManager = ASensorManager_getInstanceForPackage("");
    mNumSensors = ASensorManager_getSensorList(mSensorManager, &mSensorList);
    int period;
    int ret;

    if (argc >= 2) {
        mTestAll = !strcmp(argv[1], "all");
    }
    printf("%s test!\n", mTestAll ? "All sensor for 10 sec" : "random sensor");
    ALooper *mLooper = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);
    ASensorEventQueue *sensorEventQueue = ASensorManager_createEventQueue(mSensorManager, mLooper, 0, NULL, NULL);

    ret = get_file();

    if (ret < 0)
        return -1;

    int i = start;

    while(1) {
        if (ret) {
            SensorTestcase t = TC[i];
            period = t.length;
            if (!period) {
                printf("done!");
                break;
            }
            init_setting(t.sensor);
        } else {
            init_setting_rand();
            period = ((time(nullptr) % 6) + 6) * 10;
        }

        if (!mNumSensorConfigs || (start && end && i > end))
            break;
	if (mTestAll)
		period = 10;
        clock_gettime(CLOCK_REALTIME, &startTime);
        printf("[%d] test with %d sensors for %d seconds\n", i++, mNumSensorConfigs, period);

        for (int i = 0; i < mNumSensorConfigs; i++) {
            if (ASensorEventQueue_registerSensor(sensorEventQueue, mSensorList[mSensorConfigList[i].listIndex],
                                                mSensorConfigList[i].rate, mSensorConfigList[i].reportLatency) < 0) {
                printf("Unable to register sensor %s[%d] with rate %d and report latency %" PRId64 "\n",
                    mSensorConfigList[i].name, mSensorConfigList[i].listIndex, mSensorConfigList[i].rate,
                    mSensorConfigList[i].reportLatency);
            }
        }

        while (mContinuousMode || !hasReceivedAllEvents()) {
            if ((numSensorEvents = ASensorEventQueue_getEvents(sensorEventQueue, sensorEvents, 16)) < 0) {
                printf("An error occurred while polling for events\n");
                break;
            } else if (numSensorEvents > 0) {
                for (int i = 0; i < numSensorEvents; i++) {
                    if ((configListIndex = findSensorTypeInConfigList(sensorEvents[i].type)) < 0) {
                        printf("Received unexpected event for type %s[%d]\n",
                                ASensor_getStringType(mSensorList[findSensorTypeInSensorList(sensorEvents[i].type)]),
                                sensorEvents[i].type);
                        break;
                    }

                    if (mContinuousMode || !mSensorConfigList[configListIndex].receivedEvent) {
#ifdef DEBUG
                        printf("[%d] = %f, %f, %f @ %" PRId64 "\n", sensorEvents[i].type,
                            sensorEvents[i].data[0], sensorEvents[i].data[1],
                            sensorEvents[i].data[2], sensorEvents[i].timestamp);
#endif
                        if (mSensorConfigList[configListIndex].reportLatency == 0 &&
                            (mSensorConfigList[configListIndex].reporting_mode == 0)) {
                            unsigned long int now = time(NULL);
                            if (mSensorConfigList[configListIndex].last_timestamp &&
                                 (now - mSensorConfigList[configListIndex].last_timestamp) > 30) {
                                printf("@@@@@@@timestamp error in %s %ld @@@@@@@\n", mSensorConfigList[configListIndex].name,
                                        now - mSensorConfigList[configListIndex].last_timestamp);
                                system("echo 1 > sys/devices/platform/11020000.contexthub_reset");
                                }
                            mSensorConfigList[configListIndex].last_timestamp = now;
                        }

                        mSensorConfigList[configListIndex].receivedEvent = true;

                        if (!mContinuousMode) {
                            ASensorEventQueue_disableSensor(sensorEventQueue, mSensorList[mSensorConfigList[configListIndex].listIndex]);
                        }
                    }
                }
            }

            fflush(stdout);

            clock_gettime(CLOCK_REALTIME, &curTime);
            if (curTime.tv_sec - startTime.tv_sec > period)
                break;
        }
        for (int i = 0 ; i < mNumSensorConfigs ; i++)
            ASensorEventQueue_disableSensor(sensorEventQueue, mSensorList[mSensorConfigList[i].listIndex]);
        memset(mSensorConfigList, 0, sizeof(mSensorConfigList));
	if (mTestAll)
		break;
    }
    ASensorManager_destroyEventQueue(mSensorManager, sensorEventQueue);

    return 0;
}
