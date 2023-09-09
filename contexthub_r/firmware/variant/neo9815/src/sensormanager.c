/*----------------------------------------------------------------------------
 *      Sensor - Sensor Manager
 *----------------------------------------------------------------------------
 *      Name:    sec_sensormanager.c
 *      Purpose: To implement checking connected sesor list
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
//#include <float.h>
//#include <eventnums.h>
#include <hostIntf.h>
#include <isr.h>
#include <slab.h>
#include <plat/gpio/gpio.h>
#include <sensors.h>
#include <spi.h>
#include <i2c.h>
#include <rtc.h>
#include <timer.h>
//#include <nanohubPacket.h>
#include <variant/sensType.h>
#include <variant/variant.h>
#include <platform.h>
#include <csp_common.h>
#include <ipc_chub.h>
#include <bl.h>

#define SENSORMANAGER_APP_ID      APP_ID_MAKE(NANOHUB_VENDOR_EXYNOS, 2)

static bool  enable_debug = true;
#define INFO_PRINT(fmt, ...) do { \
        osLog(LOG_INFO, " [SENSOR MANAGER] " fmt, ##__VA_ARGS__); \
    } while (0);

#define DEBUG_PRINTF(fmt, ...) do { \
        if (enable_debug) \
            osLog(LOG_DEBUG, " [SENSOR MANAGER] " fmt, ##__VA_ARGS__); \
    } while (0);

#define ERROR_PRINT(fmt, ...) do { \
        osLog(LOG_ERROR, " [SENSOR MANAGER] " fmt, ##__VA_ARGS__); \
    } while (0);

struct sensorIntfInfo {
    uint8_t sensorType;     //sensor type
    uint8_t sensorInterface; //SPI(0) or I2C(1)
    uint8_t busId;          //I2C Bus Id
    uint8_t addr;           //I2C_ADDR
    uint8_t regAddr;        //ID Reg ADDR
    uint32_t speed;         //SPI or I2C SPEED
    uint8_t priority;      //Sensor Priority, Primary(1), Secondary(2) so far
};

#define SENSOR_INFO_NUM     (10)
enum SensorManageTypes
{
    TYPE_SENSOR_ACC_GYRO,
    TYPE_SENSOR_MAG,
    TYPE_SENSOR_ALS_PROX,
    TYPE_SENSOR_BARO,
    TYPE_SENSOR_MAX,
};

enum errorTypes
{
    NO_ERROR,
    PARAM_ERROR,
    OPERATION_ERROR,
    RESULT_ERROR,
};

#define I2C_TXRX_BUFFER_SIZE    10
#define MAX_REPEAT_TRIAL        3

#define SPI_READ                    (0x80)
#define SPI_BUFFER_ID_READ_SIZE     (4)
#define SPI_ID_READ_INDEX           (1)
#define SPI_ID_READ_SIZE            (SPI_ID_READ_INDEX+1)
#define NUM_MOUNTED_PHYSICAL_SENSOR (4)
/* Sensor Priority Information */
#define PRIORITY_SHIFT_SIZE         (2)
#define PRI_POS_ACCEL_GYRO          (3)
#define PRI_POS_MAG                 (2)
#define PRI_POS_ALS_PROX            (1)
#define PRI_POS_BARO                (0)
#define SHIFT_TYPE_ACCEL            ((PRI_POS_ACCEL_GYRO) * (PRIORITY_SHIFT_SIZE))
#define SHIFT_TYPE_MAG              ((PRI_POS_MAG) * (PRIORITY_SHIFT_SIZE))
#define SHIFT_TYPE_ALS              ((PRI_POS_ALS_PROX) * (PRIORITY_SHIFT_SIZE))
#define SHIFT_TYPE_BARO             ((PRI_POS_BARO) * (PRIORITY_SHIFT_SIZE))
//#define SEC                         (2)
#define PRI                         (1)
#define NONE                        (0)
#define DEFAULT_ACCEL_GYRO          PRI
#define DEFAULT_MAG                 PRI
#define DEFAULT_ALS_PROX            PRI
#define DEFAULT_BARO                PRI
#define MAX_PHYSICAL_SENS_TYPE      (8)

struct phyInfo {
    uint8_t type;
    uint8_t id;
};

static struct SpiPacket mPackets;
static struct SpiMode mSpiMode;
static uint8_t mSpiRxtxBuffer[SPI_BUFFER_ID_READ_SIZE];
static struct phyInfo mPhysicalSensorId[MAX_PHYSICAL_SENS_TYPE];
static int mSensCount;

static struct sensorIntfInfo mInfo[] =
{
//	sensorType, sensorInterface, busId, addr, regAddr,  speed, priority
    {TYPE_SENSOR_ACC_GYRO/*SENS_TYPE_ACCEL*/,   0x0/*SPI*/, ACC_GYR_SPI_BUS_ID, 0x00/*none for SPI*/,           0x00/*BMI160_REG_ID*/,             8000000/*none for SPI*/,          (PRI<<SHIFT_TYPE_ACCEL)},
    {TYPE_SENSOR_MAG/*SENS_TYPE_MAG*/,          0x1/*I2C*/, MAG_I2C_BUS_ID,     0x0c/*AK09918_I2C_ADDR*/,       0x00/*AK09918_WIA1*/,                  400000/*A09918_I2C_SPEED*/,       (PRI<<SHIFT_TYPE_MAG)},
    {TYPE_SENSOR_ALS_PROX/*SENS_TYPE_ALS*/,     0x1/*I2C*/, ALS_I2C_BUS_ID,     0x38/*RPR0521_I2C_ADDR*/,       0x92/*ROHM_RPR0521_REG_ID*/,        400000/*RPR0521_I2C_SPEED*/,       (PRI<<SHIFT_TYPE_ALS)},
    {TYPE_SENSOR_BARO/*SENS_TYPE_BARO*/,        0x1/*I2C*/, BARO_I2C_BUS_ID,    0x76/*BMP280_I2C_ADDR*/,        0xD0/*BOSCH_BMP280_REG_ID*/,        400000/*BMP280_I2C_SPEED*/,       (PRI<<SHIFT_TYPE_BARO)},
};

static uint8_t sensorCombination[][NUM_MOUNTED_PHYSICAL_SENSOR+1][2] = {
    /* sensor combiniation 1 */
    {
        {TYPE_SENSOR_ACC_GYRO/*SENS_TYPE_ACCEL*/, 0xd1/*BMI160_ID*/},
        {TYPE_SENSOR_MAG/*SENS_TYPE_MAG*/, 0x48/*AK09918_WIA*/},
        {TYPE_SENSOR_ALS_PROX/*SENS_TYPE_ALS*/, 0xe0/*ROHM_RPR0521_ID*/},
        {TYPE_SENSOR_BARO/* SENS_TYPE_BARO*/, 0x58/*BOSCH_BMP280_ID*/},
        {TYPE_SENSOR_MAX, ((PRI<<SHIFT_TYPE_ACCEL)|(PRI<<SHIFT_TYPE_MAG)|(PRI<<SHIFT_TYPE_ALS)|(PRI<<SHIFT_TYPE_BARO))}
    },
};

static uint8_t mCombinationNumber = (sizeof(sensorCombination)/(sizeof(uint8_t)*SENSOR_INFO_NUM));

/*
    mSensorInfo will keep the detected result of each sensor type.
*/
static uint8_t mSensorInfo[NUM_MOUNTED_PHYSICAL_SENSOR][3] = {0,};

/*
*/
static uint8_t checkCombinationIndex(uint8_t *number)
{
    uint8_t ret = NO_ERROR;
    uint8_t i;
    uint8_t combinationInfo=0;

    mSensCount = 0;
#if 1/* pick the default sensor*/
    INFO_PRINT("---> %s <---\n", __func__);
    for(i=0;i<NUM_MOUNTED_PHYSICAL_SENSOR;i++){
        if(mSensorInfo[i][0] == false){
            if(i == TYPE_SENSOR_ACC_GYRO){
                mSensorInfo[i][2] = DEFAULT_ACCEL_GYRO<<SHIFT_TYPE_ACCEL;
                mPhysicalSensorId[mSensCount].type = SENSOR_TYPE_ACCELEROMETER;
                mPhysicalSensorId[mSensCount++].id = sensor_list_no_active;
                mPhysicalSensorId[mSensCount].type = SENSOR_TYPE_ACCELEROMETER_UNCALIBRATED;
                mPhysicalSensorId[mSensCount++].id = sensor_list_no_active;
                mPhysicalSensorId[mSensCount].type = SENSOR_TYPE_GYROSCOPE;
                mPhysicalSensorId[mSensCount++].id = sensor_list_no_active;
                mPhysicalSensorId[mSensCount].type = SENSOR_TYPE_GYROSCOPE_UNCALIBRATED;
                mPhysicalSensorId[mSensCount++].id = sensor_list_no_active;
            }
            else if(i == TYPE_SENSOR_MAG){
                mSensorInfo[i][2] = DEFAULT_MAG<<SHIFT_TYPE_MAG;
                mPhysicalSensorId[mSensCount].type = SENSOR_TYPE_MAGNETIC_FIELD;
                mPhysicalSensorId[mSensCount++].id = sensor_list_no_active;
                mPhysicalSensorId[mSensCount].type = SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED;
                mPhysicalSensorId[mSensCount++].id = sensor_list_no_active;
            }
            else if(i == TYPE_SENSOR_ALS_PROX){
                mSensorInfo[i][2] = DEFAULT_ALS_PROX<<SHIFT_TYPE_ALS;
                mPhysicalSensorId[mSensCount].type = SENSOR_TYPE_LIGHT;
                mPhysicalSensorId[mSensCount++].id = sensor_list_no_active;
                mPhysicalSensorId[mSensCount].type = SENSOR_TYPE_PROXIMITY;
                mPhysicalSensorId[mSensCount++].id = sensor_list_no_active;
            }
            else if(i == TYPE_SENSOR_BARO){
                mSensorInfo[i][2] = DEFAULT_BARO<<SHIFT_TYPE_BARO;
                mPhysicalSensorId[mSensCount].type = SENSOR_TYPE_PRESSURE;
                mPhysicalSensorId[mSensCount++].id = sensor_list_no_active;
            }
        } else {
            if(i == TYPE_SENSOR_ACC_GYRO){
                mPhysicalSensorId[mSensCount].type = SENSOR_TYPE_ACCELEROMETER;
                mPhysicalSensorId[mSensCount++].id = sensor_list_bmi160_accel;
                mPhysicalSensorId[mSensCount].type = SENSOR_TYPE_ACCELEROMETER_UNCALIBRATED;
                mPhysicalSensorId[mSensCount++].id = sensor_list_bmi160_accel_ucal;
                mPhysicalSensorId[mSensCount].type = SENSOR_TYPE_GYROSCOPE;
                mPhysicalSensorId[mSensCount++].id = sensor_list_bmi160_gyro;
                mPhysicalSensorId[mSensCount].type = SENSOR_TYPE_GYROSCOPE_UNCALIBRATED;
                mPhysicalSensorId[mSensCount++].id = sensor_list_bmi160_gyro_ucal;
            }
            else if(i == TYPE_SENSOR_MAG){
                mPhysicalSensorId[mSensCount].type = SENSOR_TYPE_MAGNETIC_FIELD;
                mPhysicalSensorId[mSensCount++].id = sensor_list_stlis3mdl_mag;
                mPhysicalSensorId[mSensCount].type = SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED;
                mPhysicalSensorId[mSensCount++].id = sensor_list_stlis3mdl_mag_ucal;
            }
            else if(i == TYPE_SENSOR_ALS_PROX){
                mPhysicalSensorId[mSensCount].type = SENSOR_TYPE_LIGHT;
                mPhysicalSensorId[mSensCount++].id = sensor_list_rpr0521_ligth;
                mPhysicalSensorId[mSensCount].type = SENSOR_TYPE_PROXIMITY;
                mPhysicalSensorId[mSensCount++].id = sensor_list_rpr0521_prox;
            }
            else if(i == TYPE_SENSOR_BARO){
                mPhysicalSensorId[mSensCount].type = SENSOR_TYPE_PRESSURE;
                mPhysicalSensorId[mSensCount++].id = sensor_list_bmp280_press;
            }
        }
    }

    if(!number)
        return (ret=RESULT_ERROR);

    for(i=0;i<NUM_MOUNTED_PHYSICAL_SENSOR;i++)
        combinationInfo |= mSensorInfo[i][2];

    INFO_PRINT("detected combination: 0x%x\n", (int)combinationInfo);

    for(i=0;i<mCombinationNumber;i++){
        if(sensorCombination[i][TYPE_SENSOR_MAX][1] == combinationInfo){
            *number = i;
            INFO_PRINT("Found the matched combination(%d)\n",(int)(*number));
            return (ret = NO_ERROR);
        }
    }
    INFO_PRINT("Failed to find any matched combination(%d)\n",(int)(*number));
    return (ret = RESULT_ERROR);
#else
    for(i=0;i<NUM_MOUNTED_PHYSICAL_SENSOR;i++){
        if(mSensorInfo[i][0] == false)
            return (ret = RESULT_ERROR);
    }

    for(i=0;i<mCombinationNumber;i++){
        if((sensorCombination[i][TYPE_SENSOR_ACC_GYRO][1] == mSensorInfo[TYPE_SENSOR_ACC_GYRO][1]) && \
            (sensorCombination[i][TYPE_SENSOR_MAG][1] == mSensorInfo[TYPE_SENSOR_MAG][1]) && \
            (sensorCombination[i][TYPE_SENSOR_ALS][1] == mSensorInfo[TYPE_SENSOR_ALS][1]) && \
            (sensorCombination[i][TYPE_SENSOR_PROX][1] == mSensorInfo[TYPE_SENSOR_PROX][1])){
            *number = i;
            INFO_PRINT("---> %s <---\n", __func__);
            INFO_PRINT("Found the matched combination(%d)\n",(int)(*number));
            return (ret = NO_ERROR);
        }
    }

    INFO_PRINT("---> %s <---\n", __func__);
    INFO_PRINT("Failed to find any matched combination(%d)\n",(int)(*number));
    return (ret = RESULT_ERROR);
#endif
}
/*
    the setSpiMode function configures the spi mode for each sensor which uses the SPI for the interface between AP.
*/
static void setSpiMode(void)
{
    mSpiMode.speed = 8000000/*BMI160_SPI_SPEED_HZ*/;
    mSpiMode.bitsPerWord = 8;
    mSpiMode.cpol = SPI_CPOL_IDLE_HI;
    mSpiMode.cpha = SPI_CPHA_TRAILING_EDGE;
    mSpiMode.nssChange = true;
    mSpiMode.format = SPI_FORMAT_MSB_FIRST;
}

static void printSensorCombination(void)
{
    uint8_t i;
    INFO_PRINT("---> %s <---\n", __func__);

    for(i=0; i<mCombinationNumber;i++)
    {
        INFO_PRINT("--- Combination #%d\n", (int)i+1);
        for(uint8_t j=0; j<TYPE_SENSOR_MAX+1;j++)
        {
#if 0
            if(sensorCombination[i][j][1]==0x0)
            {
                ERROR_PRINT("Sensor ID not matched(0x%x)\n", (int)sensorCombination[i][j]);
                continue;
            }
#endif
            if(sensorCombination[i][j][0] == TYPE_SENSOR_ACC_GYRO)
            {
                INFO_PRINT("ACCEL&GYRO: 0x%x\n", sensorCombination[i][j][1]);
            }
            else if(sensorCombination[i][j][0] == TYPE_SENSOR_MAG)
            {
                INFO_PRINT("MAG: 0x%x\n", sensorCombination[i][j][1]);
            }
            else if(sensorCombination[i][j][0] == TYPE_SENSOR_ALS_PROX)
            {
                INFO_PRINT("ALS&PROX: 0x%x\n", sensorCombination[i][j][1]);
            }
            else if(sensorCombination[i][j][0] == TYPE_SENSOR_BARO)
            {
                INFO_PRINT("BARO: 0x%x\n", sensorCombination[i][j][1]);
            }
            else if(sensorCombination[i][j][0] == TYPE_SENSOR_MAX)
            {
                INFO_PRINT("combination code: 0x%x\n", sensorCombination[i][j][1]);
            }
            else
            {
                ERROR_PRINT("Sensor type not matched(0x%x)", (int)sensorCombination[i][j][0]);
            }
        }
    }
    INFO_PRINT("-------------------------\n");
}

/*
    readSpiSensorId
    The readSpiSensorId read the specific SPI port repeately. MAX_REPEAT_TRIAL defines how many time to read a SPI.
    And readSpiSensorId returns the ID of a Accel&Gyro sensor if the ID is matched with pre-defined sensorCombination.
*/
static uint8_t readSpiSensorId(uint8_t index, uint8_t *id)
{
    spi_cs_t cs={0,};
    struct SpiDevice *spiDev;
    uint8_t ret = RESULT_ERROR;
    uint8_t i, j, tmpData;
    //uint8_t txrxBuf[I2C_TXRX_BUFFER_SIZE];

    if(id == NULL){
        ERROR_PRINT("%s parameter error\n",__func__);
        return (ret=PARAM_ERROR);
    }

    if((mInfo[index].sensorInterface)||(mInfo[index].busId != ACC_GYR_SPI_BUS_ID)){
        return (ret = PARAM_ERROR);
    }

    setSpiMode();

    if(spiMasterRequest(mInfo[index].busId, &spiDev)){
        DEBUG_PRINTF("ERROR: spiMasterRequest\n");
        return (ret=OPERATION_ERROR);
    }
    else{
        for(j=0;j<MAX_REPEAT_TRIAL;j++){
            mSpiRxtxBuffer[0]= SPI_READ | mInfo[index].regAddr;

            mPackets.rxBuf = mSpiRxtxBuffer;
            mPackets.txBuf = mSpiRxtxBuffer;
            mPackets.size = SPI_ID_READ_SIZE;

            INFO_PRINT("read sensor ID(RegAddr: 0x%x) throubh SPI(trial: %d)\n", (int)mSpiRxtxBuffer[0], (int)j+1);
            ret = spiMasterRxTx(spiDev, cs, &mPackets, (size_t)1, &mSpiMode, NULL, NULL);
            tmpData = mSpiRxtxBuffer[SPI_ID_READ_INDEX];

            for(i=0;i<mCombinationNumber;i++){
                //INFO_PRINT("sensorCombination[%d][0][1]: 0x%x\n", (int)i, (int)sensorCombination[i][TYPE_SENSOR_ACC_GYRO][1]);
                if(tmpData == sensorCombination[i][TYPE_SENSOR_ACC_GYRO][1]){
                    INFO_PRINT("Detected ACCEL&Gyro Sensor ID: 0x%x\n", tmpData);
                    *id = tmpData;

                    if(spiMasterRelease(spiDev)){
                        DEBUG_PRINTF("ERROR: spiMasterRequest\n");
                        return (ret=OPERATION_ERROR);
                    }

                    return (ret=NO_ERROR);
                }
            }
        }
    }

    if(spiMasterRelease(spiDev)){
        DEBUG_PRINTF("ERROR: spiMasterRequest\n");
        INFO_PRINT("ACCEL&GYRO sensor is not detected \n");
        return (ret=OPERATION_ERROR);
    }

    INFO_PRINT("ACCEL&GYRO sensor is not detected \n");
    for(j=0;j<SPI_BUFFER_ID_READ_SIZE;j++)
        INFO_PRINT("mSpiRxtxBuffer[%d]: 0x%x\n",(int)j,(int)mSpiRxtxBuffer[j]);

    return (ret = RESULT_ERROR);
}

static uint8_t readI2cSensorId(uint8_t index, uint8_t *type, uint8_t *id)
{
    uint8_t ret = RESULT_ERROR;
    uint8_t i,j;
    uint8_t txrxBuf[I2C_TXRX_BUFFER_SIZE];

    if((type == NULL) || (id == NULL)){
        ERROR_PRINT("%s parameter error\n",__func__);
        return (ret=PARAM_ERROR);
    }

    ret = (uint32_t)i2cMasterRequest(mInfo[index].busId, mInfo[index].speed);
    if(ret){
        ERROR_PRINT("ERROR: i2cMasterRequest\n");
        return (ret=OPERATION_ERROR);
    }
    else{
        DEBUG_PRINTF("i2cMasterRequest done!\n");
        for(j=0;j<MAX_REPEAT_TRIAL;j++){
            txrxBuf[0] = mInfo[index].regAddr;
            ret = i2cMasterTxRx(mInfo[index].busId, mInfo[index].addr, txrxBuf, 1, txrxBuf, 1, NULL, NULL);
            if(ret){
                ERROR_PRINT("ERROR: i2cMasterTxRx(I2C_ADDR(0x%x),Bus_id(0x%x) REG_ADDR(0x%x))  (trial: %d)\n",mInfo[index].addr,mInfo[index].busId, mInfo[index].regAddr, (int)j);
                //return (ret=OPERATION_ERROR);
            }
            else{
                DEBUG_PRINTF("i2cMasterTxRx(I2C_ADDR(0x%x),Bus_id(0x%x) REG_ADDR(0x%x)) done!\n",mInfo[index].addr,mInfo[index].busId, mInfo[index].regAddr);
                if(txrxBuf[0] != 0){
                    INFO_PRINT("Detected Sensor ID: 0x%x (trial: %d)\n",txrxBuf[0], (int)j);
#if 1
                    for(i=0;i<mCombinationNumber;i++){
                        //INFO_PRINT("sensorCombination[%d][0][1]: 0x%x\n", (int)i, (int)sensorCombination[i][TYPE_SENSOR_ACC_GYRO][1]);
                        if(txrxBuf[0] == sensorCombination[i][*type][1]){
                            INFO_PRINT("Detected Sensor ID: 0x%x\n", txrxBuf[0]);
                            *id = txrxBuf[0];
                            if(i2cMasterRelease(mInfo[index].busId)){
                                ERROR_PRINT("ERROR: i2cMasterRelease\n");
                                return (ret = OPERATION_ERROR);
                            }
                            return (ret = NO_ERROR);
                        }
                    }
#else
                    *id = txrxBuf[0];
                    if(i2cMasterRelease(mInfo[index].busId)){
                        ERROR_PRINT("ERROR: i2cMasterRelease\n");
                        return (ret = OPERATION_ERROR);
                    }
                    return (ret = NO_ERROR);
#endif
                }
                else{
                    DEBUG_PRINTF("read data is 0 \n");
                }
            }
            mSleep(1);
        }
    }

    if(i2cMasterRelease(mInfo[index].busId)){
        DEBUG_PRINTF("ERROR: i2cMasterRelease\n");
        return (ret = OPERATION_ERROR);
    }

    return (ret = RESULT_ERROR);
}

static bool sensormanager_initApp(uint32_t taskId)
{
    uint32_t ret=false;
    uint8_t numInfo=0, i;//, j;
    uint8_t tmp[22]={0,};
    uint8_t numOsBinary = 0;
    struct sensor_map *ipc_sensor_map;

    INFO_PRINT("Sensor Manager init \n");

    numInfo = sizeof(mInfo)/sizeof(struct sensorIntfInfo);
    DEBUG_PRINTF(" %s numInfo = %d\n",__func__,(int)numInfo);

   printSensorCombination();

	/*
		Check sensor id
		- interface type: 12c or spi
		- sensor type: ACCEL&GYRO, MAG, ALS, PROX
	*/
    for(i=0;i<numInfo;i++){
        DEBUG_PRINTF("------> numInfo: %d(sensorType: %d) \n",i,mInfo[i].sensorType);
        if(mInfo[i].sensorType == TYPE_SENSOR_ACC_GYRO/*SENS_TYPE_ACCEL*/){
            DEBUG_PRINTF("Check ACCEL&GYRO sensor id! \n");
            if(mSensorInfo[TYPE_SENSOR_ACC_GYRO][0]) {
                INFO_PRINT("ACCEL&GYRO sensor(0x%x) already detected\n", (int)mSensorInfo[TYPE_SENSOR_ACC_GYRO]);
            }
            else{
                tmp[0] = mInfo[i].sensorType;
                ret=readSpiSensorId(i, &tmp[1]);
                if(!ret){
                    INFO_PRINT("Detected ACCEL_GYRO Sensor ID: 0x%x\n",tmp[1]);
                    mSensorInfo[TYPE_SENSOR_ACC_GYRO][0] = true;
                    mSensorInfo[TYPE_SENSOR_ACC_GYRO][1] = tmp[1];
                    //mSensorInfo[TYPE_SENSOR_ACC_GYRO][2] = mInfo[i].priority<<SHIFT_TYPE_ACCEL;
                    mSensorInfo[TYPE_SENSOR_ACC_GYRO][2] = mInfo[i].priority;
                    DEBUG_PRINTF("Detected Sensor Priority: 0x%x \n",mSensorInfo[TYPE_SENSOR_ACC_GYRO][2]);
                }
            }
        }
#if 0
        esle if((mInfo[i].sensorTye > TYPE_SENSOR_ACC_GYRO) || (mInfo[i].sensorTye < TYPE_SENSOR_MAX)){
            DEBUG_PRINTF("Check %s sensor id! \n", (tmp[0] == TYPE_SENSOR_MAG)?"MAG":(tmp[0] == TYPE_SENSOR_ALS)?"ALS":"PROX");
            tmp[0] = mInfo[i].sensorType;
            if(mSensorInfo[tmp[0]][0]) {
                INFO_PRINT("%s sensor(0x%x) already detected\n", (tmp[0] == TYPE_SENSOR_MAG)?"MAG":(tmp[0] == TYPE_SENSOR_ALS)?"ALS":"PROX", (int)mSensorInfo[TYPE_SENSOR_MAG]);
            }
            else{
                //tmp[0] = mInfo[i].sensorType;
                ret=readI2cSensorId(i, &tmp[0], &tmp[1]);
                if(ret){
                    ERROR_PRINT("ERROR: readI2cSensorId(%d) \n", (int)ret);
                }
                else{
                    INFO_PRINT("Detected %s Sensor ID: 0x%x\n",tmp[1]);
                    mSensorInfo[tmp[0]][0] = true;
                    mSensorInfo[tmp[0]][1] = tmp[1];
                    mSensorInfo[tmp[0]][2] = mInfo[i].priority;
                }
            }
        }
        else{
           ERROR_PRINT("sensor type(%s) is not matched(index: %d)!! \n",(tmp[0] == TYPE_SENSOR_MAG)?"MAG":(tmp[0] == TYPE_SENSOR_ALS)?"ALS":"PROX", i);
        }
#else
        else if(mInfo[i].sensorType == TYPE_SENSOR_MAG/*SENS_TYPE_MAG*/){
            DEBUG_PRINTF("Check MAG sensor id! \n");

            if(mSensorInfo[TYPE_SENSOR_MAG][0]) {
                INFO_PRINT("MAG sensor(0x%x) already detected\n", (int)mSensorInfo[TYPE_SENSOR_MAG]);
            }
            else{
                tmp[0] = mInfo[i].sensorType;
                ret=readI2cSensorId(i, &tmp[0], &tmp[1]);
                mSleep(1);

                if(!ret){
                    INFO_PRINT("Detected MAG Sensor ID: 0x%x\n",tmp[1]);
                    mSensorInfo[TYPE_SENSOR_MAG][0] = true;
                    mSensorInfo[TYPE_SENSOR_MAG][1] = tmp[1];
                    //mSensorInfo[TYPE_SENSOR_MAG][2] = mInfo[i].priority<<SHIFT_TYPE_MAG;
                    mSensorInfo[TYPE_SENSOR_MAG][2] = mInfo[i].priority;
                    DEBUG_PRINTF("Detected Sensor Priority: 0x%x \n",mSensorInfo[TYPE_SENSOR_MAG][2]);
                }
                else{
                    ERROR_PRINT("ERROR: readI2cSensorId(%d) \n", (int)ret);
                }
            }
        }
        else if(mInfo[i].sensorType == TYPE_SENSOR_ALS_PROX/*SENS_TYPE_ALS*/){
            DEBUG_PRINTF("Check ALS&PROX sensor id! \n");

            if(mSensorInfo[TYPE_SENSOR_ALS_PROX][0]) {
                INFO_PRINT("ALS PROX sensor(0x%x) already detected\n", (int)mSensorInfo[TYPE_SENSOR_ALS_PROX][1]);
            }
            else{
                tmp[0] = mInfo[i].sensorType;
                ret=readI2cSensorId(i, &tmp[0], &tmp[1]);
                if(!ret){
                    INFO_PRINT("Detected ALS PROX Sensor ID: 0x%x\n",tmp[1]);
                    mSensorInfo[TYPE_SENSOR_ALS_PROX][0] = true;
                    mSensorInfo[TYPE_SENSOR_ALS_PROX][1] = tmp[1];
                    //mSensorInfo[TYPE_SENSOR_ALS][2] = mInfo[i].priority<<SHIFT_TYPE_ALS;
                    mSensorInfo[TYPE_SENSOR_ALS_PROX][2] = mInfo[i].priority;
                    DEBUG_PRINTF("Detected Sensor Priority: 0x%x \n",mSensorInfo[TYPE_SENSOR_ALS_PROX][2]);
                }
                else{
                    ERROR_PRINT("ERROR: readI2cSensorId(%d) \n", (int)ret);
                }
            }
        }
        else if(mInfo[i].sensorType == TYPE_SENSOR_BARO/*SENS_TYPE_PROX*/){
           DEBUG_PRINTF("Check BARO sensor id! \n");
            if(mSensorInfo[TYPE_SENSOR_BARO][0]) {
                INFO_PRINT("PROX sensor(0x%x) already detected\n", (int)mSensorInfo[TYPE_SENSOR_BARO][1]);
            }
            else{
                tmp[0] = mInfo[i].sensorType;
                ret=readI2cSensorId(i, &tmp[0], &tmp[1]);
                if(!ret){
                    INFO_PRINT("Detected BARO Sensor ID: 0x%x\n",tmp[1]);
                    mSensorInfo[TYPE_SENSOR_BARO][0] = true;
                    mSensorInfo[TYPE_SENSOR_BARO][1] = tmp[1];
                    //mSensorInfo[TYPE_SENSOR_PROX][2] = mInfo[i].priority<<SHIFT_TYPE_PROX;
                    mSensorInfo[TYPE_SENSOR_BARO][2] = mInfo[i].priority;
                    DEBUG_PRINTF("Detected Sensor Priority: 0x%x \n",mSensorInfo[TYPE_SENSOR_BARO][2]);
                }
                else{
                    ERROR_PRINT("ERROR: readI2cSensorId(%d) \n", (int)ret);
                }
            }
        }
        else{
           ERROR_PRINT("sensor type(%d) is not matched!! \n", mInfo[i].sensorType);
        }
#endif
        DEBUG_PRINTF("<------ numInfo: %d \n\n",i);

    }

    INFO_PRINT("---> result <---\n");
    for(i=0;i<NUM_MOUNTED_PHYSICAL_SENSOR;i++){
        INFO_PRINT("mSensorInfo[%d][0]: %d, mSensorInfo[%d][1]: 0x%x\n",(int)i,(int) mSensorInfo[i][0],(int)i,(int)mSensorInfo[i][1]);
    }
	/*
		Check sensor combination: combination according to ID of each sensor and save it in SRAM
	*/
    DEBUG_PRINTF("Check sensor combination: %d \n", numOsBinary);

    if(checkCombinationIndex(&numOsBinary) == NO_ERROR){
        ipc_init();
        ipc_set_owner(AP, (void*)MAILBOX_AP_BASE_ADDRESS, IPC_DST);
        ipc_write_val(AP,(numOsBinary+1));
        ERROR_PRINT("Save the number of os binary[%d] \n",(int)(numOsBinary+1));
    }

    ipc_sensor_map = ipc_get_base(IPC_REG_IPC_SENSORINFO);
    if (ipc_have_sensor_info(ipc_sensor_map)) {
        struct phyInfo *p = mPhysicalSensorId;
        for (i = 0 ; i < MAX_PHYSICAL_SENS_TYPE ; i++)
            ipc_set_sensor_id(mPhysicalSensorId[i].type, mPhysicalSensorId[i].id);
        DEBUG_PRINTF("SensorInfo : %d(%d), %d(%d), %d(%d), %d(%d), %d(%d), %d(%d), %d(%d), %d(%d)\n",
            p[0].type, p[0].id, p[1].type, p[1].id,
            p[2].type, p[2].id, p[3].type, p[3].id,
            p[4].type, p[4].id, p[5].type, p[5].id,
            p[6].type, p[6].id, p[7].type, p[7].id);
#if 0
            for (i = 0; i < SENSOR_TYPE_MAX; i++)
                if (ipc_sensor_map->active_sensor_list[i])
                    DEBUG_PRINTF("sensorlist: type:%d/id:%d\n", i, ipc_sensor_map->active_sensor_list[i]);
#endif
    } else {
        DEBUG_PRINTF("%d comb: fail to set sensorinfo: %p, %s\n",numOsBinary, ipc_sensor_map, ipc_sensor_map ? ipc_sensor_map->magic : NULL);
    }
	/*
		Jump to BL
	*/
    DEBUG_PRINTF("Jump to BL(%X)\n", (unsigned int)BL.blNotifyOS);
    BL.blNotifyOS();

    return true;
}

static void sensormanager_endApp(void)
{
}
static void sensormanager_handleEvent(uint32_t evtType, const void* evtData)
{

}

INTERNAL_APP_INIT(SENSORMANAGER_APP_ID, 1, sensormanager_initApp, sensormanager_endApp, sensormanager_handleEvent);

