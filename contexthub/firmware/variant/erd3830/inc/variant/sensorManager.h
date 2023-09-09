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

#ifndef _SENS_MANAGER_H_
#define _SENS_MANAGER_H_

/* Accel BMI160 */
#define BMI160_I2C_ADDR (0x00) /* SPI use */
#define BMI160_REG_ID (0x00)
#define BMI160_CHIP_ID (0xd1)
#define BMI160_SPI_SPEED 800000

/* Accel LSM6DSM */
#define LSM6DSM_I2C_ADDR (0x00) /* SPI use */
#define LSM6DSM_REG_ID (0x0f)
#define LSM6DSM_CHIP_ID (0x6a)
#define LSM6DSM_SPI_SPEED 800000

/* Accel BMI26X */
#define BMI26X_I2C_ADDR (0x00) /* SPI use */
#define BMI26X_REG_ID (0x00)
#define BMI26X_CHIP_ID (0xA0)
#define BMI26X_SPI_SPEED 800000

/* ALS & PROX */
#define RPR0521_I2C_ADDR (0x38)
#define RPR0521_REG_ID (0x92)
#define RPR0521_CHIP_ID (0xe0)
#define RPR0521_I2C_SPEED 400000

/* ALS & PROX */
#define TMD3725_I2C_ADDR (0x39)
#define TMD3725_REG_ID (0x92)
#define TMD3725_CHIP_ID (0xe4)
#define TMD3725_I2C_SPEED 400000

/* Mag LIS3MDL */
#define LIS3MDL_I2C_ADDR (0x1E)
#define REG_LIS3MDL_WHO_AM_I (0x0F)
#define LIS3MDL_CHIP_ID (0x3d)
#define LIS3MDL_I2C_SPEED 400000

/* Mag AK099 */
#define AK09918_I2C_ADDR (0x0c)
#define AK09918_WIA1_REG_ID (0x00)
#define A09918_I2C_SPEED 400000
#define AK09918_CHIP_ID (0x48)

/* BARO BMP280*/
#define BMP280_I2C_ADDR (0x76)
#define BMP280_I2C_SPEED 400000
#define BMP280_REG_ID (0xD0)
#define BMP280_CHIP_ID (0x58)

/* BARO BMP380*/
#define BMP380_I2C_ADDR (0x76)
#define BMP380_I2C_SPEED 400000
#define BMP380_REG_ID (0x00)
#define BMP380_CHIP_ID (0x50)

/* Define Sensor Info for Sensor manager */
#define USE_BARO

#define SPI_DUMMY_BYTES 1

/* i2c addr */
#define ACC_I2C_ADDR    BMI160_I2C_ADDR
#ifdef USE_BARO
#define PROX_I2C_ADDR   RPR0521_I2C_ADDR
#endif
#define MAG_I2C_ADDR    AK09918_I2C_ADDR
#ifdef USE_BARO
    #define BARO_I2C_ADDR   BMP380_I2C_ADDR
#endif

/* i2c/spi speed */
#define ACC_SPI_SPEED   BMI160_SPI_SPEED
#define PROX_I2C_SPEED  RPR0521_I2C_SPEED
#define MAG_I2C_SPEED   A09918_I2C_SPEED
#ifdef USE_BARO
    #define BARO_I2C_SPEED BMP380_I2C_SPEED
#endif

/* reg id */
#define ACC_REG_ID  BMI160_REG_ID
#define PROX_REG_ID RPR0521_REG_ID
#define MAG_REG_ID  AK09918_WIA1_REG_ID
#ifdef USE_BARO
    #define BARO_REG_ID BMP380_REG_ID
#endif

/* chip id */
#define ACC_CHIP_ID     BMI160_CHIP_ID
#define PROX_CHIP_ID    RPR0521_CHIP_ID
#define MAG_CHIP_ID     AK09918_CHIP_ID
#ifdef USE_BARO
    #define BARO_CHIP_ID BMP380_CHIP_ID
#endif

#endif
