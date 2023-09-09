/*!
 * @section LICENSE
 *
 * (C) Copyright 2011~2021 Bosch Sensortec GmbH All Rights Reserved
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
 *
 *------------------------------------------------------------------------------
 *  Disclaimer
 *
 * Common: Bosch Sensortec products are developed for the consumer goods
 * industry. They may only be used within the parameters of the respective valid
 * product data sheet.  Bosch Sensortec products are provided with the express
 * understanding that there is no warranty of fitness for a particular purpose.
 * They are not fit for use in life-sustaining, safety or security sensitive
 * systems or any system or device that may lead to bodily harm or property
 * damage if the system or device malfunctions. In addition, Bosch Sensortec
 * products are not fit for use in products which interact with motor vehicle
 * systems.  The resale and/or use of products are at the purchaser's own risk
 * and his own responsibility. The examination of fitness for the intended use
 * is the sole responsibility of the Purchaser.
 *
 * The purchaser shall indemnify Bosch Sensortec from all third party claims,
 * including any claims for incidental, or consequential damages, arising from
 * any product use not covered by the parameters of the respective valid product
 * data sheet or not approved by Bosch Sensortec and reimburse Bosch Sensortec
 * for all costs in connection with such claims.
 *
 * The purchaser must monitor the market for the purchased products,
 * particularly with regard to product safety and inform Bosch Sensortec without
 * delay of all security relevant incidents.
 *
 * Engineering Samples are marked with an asterisk (*) or (e). Samples may vary
 * from the valid technical specifications of the product series. They are
 * therefore not intended or fit for resale to third parties or for use in end
 * products. Their sole purpose is internal client testing. The testing of an
 * engineering sample may in no way replace the testing of a product series.
 * Bosch Sensortec assumes no liability for the use of engineering samples. By
 * accepting the engineering samples, the Purchaser agrees to indemnify Bosch
 * Sensortec from all claims arising from the use of engineering samples.
 *
 * Special: This software module (hereinafter called "Software") and any
 * information on application-sheets (hereinafter called "Information") is
 * provided free of charge for the sole purpose to support your application
 * work. The Software and Information is subject to the following terms and
 * conditions:
 *
 * The Software is specifically designed for the exclusive use for Bosch
 * Sensortec products by personnel who have special experience and training. Do
 * not use this Software if you do not have the proper experience or training.
 *
 * This Software package is provided `` as is `` and without any expressed or
 * implied warranties, including without limitation, the implied warranties of
 * merchantability and fitness for a particular purpose.
 *
 * Bosch Sensortec and their representatives and agents deny any liability for
 * the functional impairment of this Software in terms of fitness, performance
 * and safety. Bosch Sensortec and their representatives and agents shall not be
 * liable for any direct or indirect damages or injury, except as otherwise
 * stipulated in mandatory applicable law.
 *
 * The Information provided is believed to be accurate and reliable. Bosch
 * Sensortec assumes no responsibility for the consequences of use of such
 * Information nor for any infringement of patents or other rights of third
 * parties which may result from its use.
 *
 */


/*==============================================================================
   DESCRIPTION

   Impelements the driver for the Bosch BMI2XY

   EDIT HISTORY FOR FILE


when        who      what
[MM/DD/YYYY]
----------  ---      -----------------------------------------------------------
09/17/2019  MT  v0.0.0.1 :support the data streaming of acc and gyro
11/12/2019  MT  V0.0.0.2 : delete read the interrupt status
11/15/2019  MT  V0.0.0.3 : optimize the code for read the interrupts status
11/20/2019  MT  V0.0.0.4 : fix the issue when odr 400HZ for gyro factor[1] = 13
11/25/2019  MT  V0.0.0.5 : backup the register value before selftes and resotre it after selftest
11/28/2019  MT  V0.0.0.6 : 1 add the reg_val[17] use to save the register value and restore the register
                                             2 delete no use code
12/04/2019  MT  V0.0.0.7 : 1 fix the gyro selftest function
                           2 read the fifo data if the fifo length > 0
12/09/2019  MT  V0.0.0.8 : 1 reduce the delay time for gyro selft test
                                             2 add the heart beat timer function check if the fifo full, if FULL, need clear the fifo
12/17/2019  MT  V0.0.0.9 : 1 support the bmi270
12/20/2019  MT  V0.0.0.A : 1 fix the wrong state
                                             2 change the log tag, use the BMI26X
                                             3 add the dump register
12/20/2019  MT  V0.0.0.B : 1 optimize the risk for heart beat timer
12/30/2019  MT  V0.0.0.C : 1 delete the anymotion fucntion for VIVO refer NO_NEED_ANYMOTION_FOR_VIVO
04/23/2020  LS  V1.0.0.0 : PC-Lint Errors and warnings fixed
02/01/2021  LS  V1.0.0.1 : BMI263 sensor support added, I2C protocol support added and configurable based on BMI26X_I2C_BUS_ID
05/11/2021  MT  V1.0.0.2 : add the foc fucntion
============================================================================*/

#define BMI2XY_CONTEXT_HUB_DRIVER_VER UINT16_C(0x1002)
