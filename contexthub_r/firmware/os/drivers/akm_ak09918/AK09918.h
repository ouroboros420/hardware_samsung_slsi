
#ifndef _AK09918C_H_
#define	_AK09918C_H_

#define AK09918_APP_ID                          APP_ID_MAKE(NANOHUB_VENDOR_GOOGLE, 16)

#define AK09918_REG_WIA1			            (0x00)
#define AK09918_REG_WIA2			            (0x01)
#define AK09918_REG_INFO1			            (0x02)
#define AK09918_REG_INFO2			            (0x03)
#define AK09918_REG_ST1				            (0x10)
#define AK09918_REG_HXL				            (0x11)
#define AK09918_REG_HXH				            (0x12)
#define AK09918_REG_HYL				            (0x13)
#define AK09918_REG_HYH				            (0x14)
#define AK09918_REG_HZL				            (0x15)
#define AK09918_REG_HZH				            (0x16)
#define AK09918_REG_TMPS			            (0x17)
#define AK09918_REG_ST2				            (0x18)
#define AK09918_REG_CNTL1			            (0x30)
#define AK09918_REG_CNTL2			            (0x31)
#define AK09918_REG_CNTL3			            (0x32)

#define AK09918_MODE_SNG_MEASURE	            (0x01)
#define AK09918_MODE_CONT_MEASURE_MODE1         (0x02)
#define AK09918_MODE_CONT_MEASURE_MODE2         (0x04)
#define AK09918_MODE_CONT_MEASURE_MODE3         (0x06)
#define AK09918_MODE_CONT_MEASURE_MODE4         (0x08)
#define AK09918_MODE_SELF_TEST		            (0x10)
#define AK09911_MODE_FUSE_ACCESS	            (0x1F)
#define AK09918_MODE_POWERDOWN		            (0x00)
#define AK09918_RESET_DATA			            (0x01)

#define AKM_REG_MODE			                AK09918_REG_CNTL2
#define AKM_REG_RESET			                AK09918_REG_CNTL3
#define AKM_REG_STATUS			                AK09918_REG_ST1

#define AKM_WIA_VALE			                (0x48)
#define AKM_MEASURE_TIME_US		                (10000)
#define AKM_DRDY_IS_HIGH(x)		                ((x) & 0x01)
#define AKM_SENSOR_INFO_SIZE	                (2)
#define AKM_SENSOR_CONF_SIZE	                (3)
#define AKM_SENSOR_DATA_SIZE	                (9)

#define AKM_YPR_DATA_SIZE		                (16)
#define AKM_RWBUF_SIZE			                (16)

#define AKM_MODE_SNG_MEASURE	                AK09918_MODE_SNG_MEASURE
#define AKM_MODE_CONT_MEASURE1	                AK09918_MODE_CONT_MEASURE_MODE1
#define AKM_MODE_CONT_MEASURE2	                AK09918_MODE_CONT_MEASURE_MODE2
#define AKM_MODE_CONT_MEASURE3	                AK09918_MODE_CONT_MEASURE_MODE3
#define AKM_MODE_CONT_MEASURE4	                AK09918_MODE_CONT_MEASURE_MODE4
#define AKM_MODE_SELF_TEST		                AK09918_MODE_SELF_TEST
#define AKM_MODE_POWERDOWN		                AK09918_MODE_POWERDOWN
#define AKM_RESET_DATA			                AK09918_RESET_DATA

#define TLMIT_NO_CNT_ITR                        (3)

#define ACC_DATA_FLAG		                    (0)
#define MAG_DATA_FLAG		                    (1)
#define FUSION_DATA_FLAG	                    (2)
#define AKM_NUM_SENSORS		                    (3)

#define ACC_DATA_READY		                    (1<<(ACC_DATA_FLAG))
#define MAG_DATA_READY		                    (1<<(MAG_DATA_FLAG))
#define FUSION_DATA_READY	                    (1<<(FUSION_DATA_FLAG))

#define AK09918_ADDRESS				            (0x18)

#define  GEOMAGNETIC_ADDRESS					(AK09918_ADDRESS)
#define  GEOMAGNETIC_WHOAMI_RESPONSE			(AKM_WIA_VALE)

#define AKM_LOGGING_DATA_SIZE                   (21)

#define I2C_ADDR                                (0x0C)

#define I2C_BUS_ID                              MAG_I2C_BUS_ID
#define I2C_SPEED                               (400000)


#define I2C_MULTIBYTE_FLAG                      (0x80)

#define AK09918_DEFAULT_DELAY                   (100000000ULL)
#define AK09918_DATA_RATE_10HZ                  (4)
#define AK09918_DEFAULT_RATE                    AK09918_DATA_RATE_10HZ

#define MAX_NUM_COMMS_EVENT_SAMPLES             (4)
#define AKM09918_SENSITIVITY                    ((float)0.15)
#define MIN_DELTA_TIME                          (1000000)

 
#endif //_GEOMAGNETIC_AK09918C_H_
