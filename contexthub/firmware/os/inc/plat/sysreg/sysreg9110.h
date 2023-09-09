/*----------------------------------------------------------------------------
 *      Exynos SoC  -  SYSREG
 *----------------------------------------------------------------------------
 *      Name:    sysregDrv9110.h
 *      Purpose: To implement SYSREG driver functions for 9110
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

#ifndef __SYSREG_DRIVER_9110_H__
#define __SYSREG_DRIVER_9110_H__

// CHUB
#define    REG_SYSREG_BUS_COMPONENET_DRCG_EN       (SYSREG_BASE_ADDRESS + 0x104)
#define    REG_SYSREG_MEMCLK                       (SYSREG_BASE_ADDRESS + 0x108)
#define    REG_SYSREG_MEMCTRL_D                    (SYSREG_BASE_ADDRESS + 0x10C)
#define    REG_SYSREG_MEMCTRL_I                    (SYSREG_BASE_ADDRESS + 0x110)
#define    REG_SYSREG_MIF_REQ                      (SYSREG_BASE_ADDRESS + 0x200)
#define    REG_SYSREG_MIF_REQ_ACK                  (SYSREG_BASE_ADDRESS + 0x204)
#define    REG_SYSREG_APM_REQ                      (SYSREG_BASE_ADDRESS + 0x210)
#define    REG_SYSREG_APM_REQ_ACK                  (SYSREG_BASE_ADDRESS + 0x214)
#define    REG_SYSREG_PD_REQ                       (SYSREG_BASE_ADDRESS + 0x220)
#define    REG_SYSREG_EARLY_WAKEUP_WINDOW_REG      (SYSREG_BASE_ADDRESS + 0x230)
#define    REG_SYSREG_APM_UP_STATUS                (SYSREG_BASE_ADDRESS + 0x240)
#define    REG_SYSREG_CLEAR_VVALID                 (SYSREG_BASE_ADDRESS + 0x250)
#define    REG_SYSREG_HWACG_CM4_SATUS              (SYSREG_BASE_ADDRESS + 0x420)
#define    REG_SYSREG_HWACG_CM4_CONFIG             (SYSREG_BASE_ADDRESS + 0x424)
#define    REG_SYSREG_HWACG_CM4_CLKREQ             (SYSREG_BASE_ADDRESS + 0x428)
#define    REG_SYSREG_CHUB_VVALID_INTR             (SYSREG_BASE_ADDRESS + 0x600)
#define    REG_SYSREG_CHUB_VVALID_COUNTER          (SYSREG_BASE_ADDRESS + 0x604)
#define    REG_SYSREG_CHUB_SCALABLE_CTRL0          (SYSREG_BASE_ADDRESS + 0x700)
#define    REG_SYSREG_CHUB_SCALABLE_CTRL1          (SYSREG_BASE_ADDRESS + 0x704)
#define    REG_SYSREG_SRAM_CON_I_CHUB              (SYSREG_BASE_ADDRESS + 0x800)
#define    REG_SYSREG_SRAM_CON_D_CHUB              (SYSREG_BASE_ADDRESS + 0x804)
#define    REG_SYSREG_BPS_SEL_RESP                 (SYSREG_BASE_ADDRESS + 0x900)
#define    REG_SYSREG_USI_CHUB00_SW_CONF           (SYSREG_BASE_ADDRESS + 0x3000)
#define    REG_SYSREG_I2C_CHUB00_SW_CONF           (SYSREG_BASE_ADDRESS + 0x3004)
#define    REG_SYSREG_I2C_CHUB01_SW_CONF           (SYSREG_BASE_ADDRESS + 0x3008)

// CMGP -- need to check base address
#define    REG_SYSREG_USI_CMGP00_SW_CONF           (SYSREG_CMGP_BASE_ADDRESS + 0x2000)
#define    REG_SYSREG_I2C_CMGP00_SW_CONF           (SYSREG_CMGP_BASE_ADDRESS + 0x2004)
#define    REG_SYSREG_USI_CMGP01_SW_CONF           (SYSREG_CMGP_BASE_ADDRESS + 0x2010)
#define    REG_SYSREG_I2C_CMGP01_SW_CONF           (SYSREG_CMGP_BASE_ADDRESS + 0x2014)
#define    REG_SYSREG_USI_CMGP02_SW_CONF           (SYSREG_CMGP_BASE_ADDRESS + 0x2020)
#define    REG_SYSREG_I2C_CMGP02_SW_CONF           (SYSREG_CMGP_BASE_ADDRESS + 0x2024)
#define    REG_SYSREG_USI_CMGP03_SW_CONF           (SYSREG_CMGP_BASE_ADDRESS + 0x2030)
#define    REG_SYSREG_I2C_CMGP03_SW_CONF           (SYSREG_CMGP_BASE_ADDRESS + 0x2034)

#define    REG_SYSREG_USI_CMGP00_IPCLK             (SYSREG_CMGP_BASE_ADDRESS + 0x3000)
#define    REG_SYSREG_I2C_CMGP00_IPCLK             (SYSREG_CMGP_BASE_ADDRESS + 0x3004)
#define    REG_SYSREG_USI_CMGP01_IPCLK             (SYSREG_CMGP_BASE_ADDRESS + 0x3010)
#define    REG_SYSREG_I2C_CMGP01_IPCLK             (SYSREG_CMGP_BASE_ADDRESS + 0x3014)
#define    REG_SYSREG_USI_CMGP02_IPCLK             (SYSREG_CMGP_BASE_ADDRESS + 0x3020)
#define    REG_SYSREG_I2C_CMGP02_IPCLK             (SYSREG_CMGP_BASE_ADDRESS + 0x3024)
#define    REG_SYSREG_USI_CMGP03_IPCLK             (SYSREG_CMGP_BASE_ADDRESS + 0x3030)
#define    REG_SYSREG_I2C_CMGP03_IPCLK             (SYSREG_CMGP_BASE_ADDRESS + 0x3034)

#define    REG_SYSREG_ADC_AP_FLAG                  (SYSREG_CMGP_BASE_ADDRESS + 0x3100)
#define    REG_SYSREG_ADC_CHUB_FLAG                (SYSREG_CMGP_BASE_ADDRESS + 0x3104)
#define    REG_SYSREG_ADC_INIT_TURN                (SYSREG_CMGP_BASE_ADDRESS + 0x3108)

/* REG_SYSREG_MIF_REQ */
#define    MIF_REQ                                 (1<<0)

/* REG_SYSREG_MIF_REQ_ACK */
#define    INTREQ_MIF_REQ_ACK                      (1<<8)
#define    MIF_REQ_ACK_STATUS                      (1<<4)
#define    MIF_REQ_INT_CLEAR                       (1<<0)

/* REG_SYSREG_APM_REQ */
#define    APM_REQ                                 (1<<0)

/* REG_SYSREG_APM_REQ_ACK */
#define    INTREQ_APM_REQ_ACK                      (1<<8)
#define    APM_REQ_ACK_STATUS                      (1<<4)
#define    APM_REQ_INT_CLEAR                       (1<<0)

/* REG_SYSREG_PD_REQ */
#define    PD_REQ                                  (1<<0)

/* REG_SYSREG_EARLY_WAKEUP_WINDOW_REG */
#define    EARLY_WAKEUP_WINDOW_REQ                 (1<<0)

/* REG_SYSREG_APM_UP_STATUS */
#define    APM_UP_STATUS                           (1<<0)



void sysregSetPdReq(IN bool en);
uint32_t sysregGetPdReq(void);
void sysregSetMaskIrq(IN uint64_t irq);
uint64_t sysregGetMaskIrq(void);
void sysregSetDRCG(IN uint32_t enable);
void sysregSetEarlyWakeupWindowReq(IN bool en);
uint32_t sysregGetEarlyWakeupWindowReq(void);
void sysregSetVvalidIntr(IN bool negEdge, IN bool intMask);
void sysregSetMIFReq(IN bool enable);
bool sysregEnableMIF(void);
uint32_t sysregGetMIFAck(void);
void sysregClrMIFReqAck(void);
void sysregSetAPMReq(IN bool enable);
uint32_t sysregGetAPMAck(void);
void sysregClrAPMAck(void);
bool sysregEnableAPM(void);
uint32_t sysregGetAPMUpStatus(void);
void sysregSaveState(void);
void sysregRestoreState(void);
void sysregApmReqAck_IRQHandler(void);

#endif  //  #ifndef __SYSREG_DRIVER_9110_H__
