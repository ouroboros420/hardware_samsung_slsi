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
#include <plat/spi/spi.h>
#include <i2c.h>
#include <plat/i2c/i2c.h>
#include <rtc.h>
#include <timer.h>
//#include <nanohubPacket.h>
#include <variant/sensType.h>
#include <variant/variant.h>
#include <platform.h>
#include <csp_common.h>
#include <ipc_chub.h>
#include <bl.h>
#include <variant/sensorManager.h>

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

enum SensorManageTypes
{
    TYPE_SENSOR_ACC_GYRO,
    TYPE_SENSOR_MAG,
    TYPE_SENSOR_ALS_PROX,
#ifdef USE_BARO
    TYPE_SENSOR_BARO,
#endif
    TYPE_SENSOR_MAX,
};

#define SENSOR_INFO_NUM     (TYPE_SENSOR_MAX * 2)

enum errorTypes
{
    NO_ERROR,
    PARAM_ERROR,
    OPERATION_ERROR,
    RESULT_ERROR,
};

#define I2C_TXRX_BUFFER_SIZE    10
#define MAX_REPEAT_TRIAL        3

#ifndef SPI_DUMMY_BYTES
    #define SPI_DUMMY_BYTES 0
#endif

#define SPI_READ                    (0x80)
#define SPI_BUFFER_ID_READ_SIZE     (4)
#define SPI_ID_READ_INDEX           (SPI_DUMMY_BYTES + 1)
#define SPI_ID_READ_SIZE            (SPI_ID_READ_INDEX+1)
#define NUM_MOUNTED_PHYSICAL_SENSOR (TYPE_SENSOR_MAX)
/* Sensor Priority Information */
#define PRIORITY_SHIFT_SIZE         (2)
#define PRI_POS_ACCEL_GYRO          (3)
#define PRI_POS_MAG                 (2)
#define PRI_POS_ALS_PROX            (1)
#ifdef USE_BARO
#define PRI_POS_BARO                (0)
#endif
#define SHIFT_TYPE_ACCEL            ((PRI_POS_ACCEL_GYRO) * (PRIORITY_SHIFT_SIZE))
#define SHIFT_TYPE_MAG              ((PRI_POS_MAG) * (PRIORITY_SHIFT_SIZE))
#define SHIFT_TYPE_ALS              ((PRI_POS_ALS_PROX) * (PRIORITY_SHIFT_SIZE))
#ifdef USE_BARO
#define SHIFT_TYPE_BARO             ((PRI_POS_BARO) * (PRIORITY_SHIFT_SIZE))
#endif
//#define SEC                         (2)
#define PRI                         (1)
#define NONE                        (0)
#define DEFAULT_ACCEL_GYRO          PRI
#define DEFAULT_MAG                 PRI
#define DEFAULT_ALS_PROX            PRI
#ifdef USE_BARO
#define DEFAULT_BARO                PRI
#endif
#define MAX_PHYSICAL_SENS_TYPE      (8)

static struct SpiPacket mPackets;
static struct SpiMode mSpiMode;
static uint8_t mSpiRxtxBuffer[SPI_BUFFER_ID_READ_SIZE];

static struct sensorIntfInfo mInfo[] =
{
//	sensorType, sensorInterface, busId, addr, regAddr,  speed, priority
    {TYPE_SENSOR_ACC_GYRO/*SENS_TYPE_ACCEL*/,   0x0/*SPI*/, ACC_GYR_SPI_BUS_ID, ACC_I2C_ADDR/*none for SPI*/, ACC_REG_ID/*BMI263_REG_ID*/, ACC_SPI_SPEED/*none for SPI*/, (PRI<<SHIFT_TYPE_ACCEL)},
    {TYPE_SENSOR_MAG/*SENS_TYPE_MAG*/,          0x1/*I2C*/, MAG_I2C_BUS_ID,     MAG_I2C_ADDR /*AK09918_I2C_ADDR*/, MAG_REG_ID/*AK09918_WIA1*/, MAG_I2C_SPEED/*A09918_I2C_SPEED*/, (PRI<<SHIFT_TYPE_MAG)},
    {TYPE_SENSOR_ALS_PROX/*SENS_TYPE_ALS*/,     0x1/*I2C*/, ALS_I2C_BUS_ID,     PROX_I2C_ADDR/*RPR0521_I2C_ADDR*/, PROX_REG_ID/*ROHM_RPR0521_REG_ID*/, PROX_I2C_SPEED/*RPR0521_I2C_SPEED*/, (PRI<<SHIFT_TYPE_ALS)},
#ifdef USE_BARO
    {TYPE_SENSOR_BARO/*SENS_TYPE_BARO*/,        0x1/*I2C*/, BARO_I2C_BUS_ID,    BARO_I2C_ADDR/*BMP380_I2C_ADDR*/, BARO_REG_ID/*BOSCH_BMP380_REG_ID*/, BARO_I2C_SPEED/*BMP380_I2C_SPEED*/, (PRI<<SHIFT_TYPE_BARO)},
#endif
};

static uint8_t sensorCombination[][NUM_MOUNTED_PHYSICAL_SENSOR+1][2] = {
    /* sensor combiniation 1 */
    {
        {TYPE_SENSOR_ACC_GYRO/*SENS_TYPE_ACCEL*/, ACC_CHIP_ID/*LSM6DSM_ID*/},
        {TYPE_SENSOR_MAG/*SENS_TYPE_MAG*/, MAG_CHIP_ID /*AK09918_WIA*/},
        {TYPE_SENSOR_ALS_PROX/*SENS_TYPE_ALS*/, PROX_CHIP_ID/*ROHM_RPR0521_ID*/},
#ifdef USE_BARO
        {TYPE_SENSOR_BARO/* SENS_TYPE_BARO*/, BARO_CHIP_ID/*BOSCH_BMP380_ID*/},
        {TYPE_SENSOR_MAX, ((PRI<<SHIFT_TYPE_ACCEL)|(PRI<<SHIFT_TYPE_MAG)|(PRI<<SHIFT_TYPE_ALS)|(PRI<<SHIFT_TYPE_BARO))}
#else
        {TYPE_SENSOR_MAX, ((PRI<<SHIFT_TYPE_ACCEL)|(PRI<<SHIFT_TYPE_MAG)|(PRI<<SHIFT_TYPE_ALS))}
#endif
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

    INFO_PRINT("---> %s <---\n", __func__);
    for(i=0;i<NUM_MOUNTED_PHYSICAL_SENSOR;i++){
        if(mSensorInfo[i][0] == false){
            if(i == TYPE_SENSOR_ACC_GYRO){
                mSensorInfo[i][2] = DEFAULT_ACCEL_GYRO<<SHIFT_TYPE_ACCEL;
            }
            else if(i == TYPE_SENSOR_MAG){
                mSensorInfo[i][2] = DEFAULT_MAG<<SHIFT_TYPE_MAG;
            }
            else if(i == TYPE_SENSOR_ALS_PROX){
                mSensorInfo[i][2] = DEFAULT_ALS_PROX<<SHIFT_TYPE_ALS;
            }
#ifdef USE_BARO
            else if(i == TYPE_SENSOR_BARO){
                mSensorInfo[i][2] = DEFAULT_BARO<<SHIFT_TYPE_BARO;
            }
#endif
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
}
/*
    the setSpiMode function configures the spi mode for each sensor which uses the SPI for the interface between AP.
*/
static void setSpiMode(void)
{
    mSpiMode.speed = 8000000/*LSM6DSM_SPI_SPEED_HZ*/;
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
#ifdef USE_BARO
            else if(sensorCombination[i][j][0] == TYPE_SENSOR_BARO)
            {
                INFO_PRINT("BARO: 0x%x\n", sensorCombination[i][j][1]);
            }
#endif
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
    spiSetTransferMode(mInfo[index].busId, SPI_POLLING_MODE);

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

    i2cSetOperationMode(mInfo[index].busId, I2C_MODE_POLLING);
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
#ifdef USE_BARO
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
#endif
        else{
           ERROR_PRINT("sensor type(%d) is not matched!! \n", mInfo[i].sensorType);
        }
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
        ipc_write_hw_value(IPC_VAL_HW_DEBUG, (numOsBinary+1));
        ERROR_PRINT("Save the number of os binary[%d] \n",(int)(numOsBinary+1));
    }

    ipc_hw_gen_interrupt((void *)MAILBOX_AP_BASE_ADDRESS, IPC_SRC_MB0, IRQ_NUM_CHUB_ALIVE);

	/*
		Jump to BL
	*/
    DEBUG_PRINTF("Jump to BL(%X)\n", (unsigned int)BL.blNotifyOS);
    //BL.blNotifyOS();
    while(1) {
        asm volatile ("wfi\n" "nop":::"memory");
    }

    return true;
}

static void sensormanager_endApp(void)
{
}
static void sensormanager_handleEvent(uint32_t evtType, const void* evtData)
{

}

INTERNAL_APP_INIT(SENSORMANAGER_APP_ID, 1, sensormanager_initApp, sensormanager_endApp, sensormanager_handleEvent);

