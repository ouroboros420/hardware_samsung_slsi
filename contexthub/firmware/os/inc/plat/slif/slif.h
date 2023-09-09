/*----------------------------------------------------------------------------
 *      Exynos SoC  -  Serial-LIF
 *----------------------------------------------------------------------------
 *      Name:    slif.h
 *      Purpose: To expose PWM APIs and define macros
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

#ifndef __SLIF_H__
#define __SLIF_H__

#include <csp_common.h>
#include <csp_assert.h>
#include <csp_printf.h>

#define SLIF_FIFO0_EMPTY     (0x1 << 0)
#define SLIF_FIFO1_EMPTY     (0x1 << 1)
#define SLIF_FIFO2_EMPTY     (0x1 << 2)
#define SLIF_FIFO3_EMPTY     (0x1 << 3)
#define SLIF_MSIF_FIFO_EMPTY (0x1 << 4)
#define SLIF_SSIF_FIFO_FULL  (0x1 << 5)
#define SLIF_AXI_WRITE_DONE  (0x1 << 6)
#define SLIF_AXI_READ_DONE   (0x1 << 7)
#define SLIF_AXI_WRITE_TRIG0 (0x1 << 8)
#define SLIF_AXI_WRITE_TRIG1 (0x1 << 9)
#define SLIF_AXI_WRITE_TRIG2 (0x1 << 10)
#define SLIF_AXI_WRITE_TRIG3 (0x1 << 11)
#define SLIF_AXI_READ_TRIG0  (0x1 << 12)
#define SLIF_AXI_READ_TRIG1  (0x1 << 13)
#define SLIF_AXI_READ_TRIG2  (0x1 << 14)
#define SLIF_AXI_READ_TRIG3  (0x1 << 15)
#define SLIF_AXI_WRITE_ERROR (0x1 << 16)
#define SLIF_AXI_READ_ERROR  (0x1 << 17)
#define SLIF_INTERRUPT_NUM   (18)

// CONFIG TX/RX
#define BCLK_RISING_EDGE     (0x0)
#define BCLK_FALLING_EDGE    (0x1)
#define WS_MODE_LRCK         (0x0)
#define WS_MODE_TDM          (0x1)
#define BCLK_INTERNAL        (0x0)
#define BCLK_EXTERNAL        (0x1)
#define WS_ACTIVE_HIGH       (0x0)
#define WS_ACTIVE_LOW        (0x0)
#define MODE_PACKED_16BIT    (0x3)
#define MODE_32BIT           (0x2)
#define MODE_16BIT           (0x1)
#define MODE_24BIT           (0x0)
#define USE_2CH              (0x1)
#define USE_4CH              (0x3)
#define USE_6CH              (0x5)
#define USE_8CH              (0x7)

#define SLIF_TX              (0)
#define SLIF_RX              (1)

int slifInit(uint32_t txBufAddr, uint32_t txBufSize, uint32_t rxBufAddr, uint32_t rxBufSize);
void slifEnableInterrupt(uint32_t irq);
void slifDisableInterrupt(uint32_t irq);
uint32_t slifGetPendingStatus(void);
void slifClearPending(uint32_t pend);
void slifEnableRx(void);
void slifDisableRx(void);
void slifEnalbeTx(void);
void slifDisableTX(void);
void slifEnableLoopback(void);
void slifDisableLoopback(void);
void slifSetBclkPolar(uint32_t dir, uint32_t polar);
void slifSetWSMode(uint32_t dir, uint32_t mode);
void slifSelectBclk(uint32_t dir, uint32_t bclk);
void slifSetWSPolar(uint32_t dir, uint32_t polar);
void slifSetOpMode(uint32_t dir, uint32_t mode);
void slifSetTxMode(uint32_t bclkpol, uint32_t wsmode, uint32_t bclk, uint32_t wspolar, uint32_t opmode);
void slifSetRxMode(uint32_t bclkpol, uint32_t wsmode, uint32_t bclk, uint32_t wspolar, uint32_t opmode);
void slifStartRx(void);
void slifStopRx(void);
void slifStartTx(void);
void slifStopTx(void);
void slifStartAll(void);
void slifStopAll(void);
int slifSetWriteArea(uint32_t start, uint32_t end);
int slifSetWriteTrigPoint(uint32_t no, uint32_t addr);
void slifEnableWriteAddrWrap(void);
void slifDisableWriteAddrWrap(void);
int slifSetReadArea(uint32_t start, uint32_t end);
int slifSetReadTrigPoint(uint32_t no, uint32_t addr);
void slifEnableReadAddrWrap(void);
void slifDisableReadAddrWrap(void);
void slifSetHandler(int irq, void (*handler)(void));
void slif_IRQHandler(void);

#endif

