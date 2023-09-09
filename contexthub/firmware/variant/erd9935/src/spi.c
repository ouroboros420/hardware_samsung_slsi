/*----------------------------------------------------------------------------
 *      ERD  -  SPI
 *----------------------------------------------------------------------------
 *      Name:    spi.c
 *      Purpose: Initialize SPI ports
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


#include <csp_common.h>
#include <spiBoard.h>
#include <plat/spi/spi.h>
#include <spiDrv.h>
#include <usi.h>
#include <cmu.h>

static const spi_device_t mSpiDevConf[] = {
    [0] = {
        .m_portId=SPI_CHUB0,
        .m_EnAutoCS = 0,
        .m_IRQ = USI_CHUB0_IRQn,
        .m_OpMode = SPI_MASTER,
        .m_TargetSPICLK = 8000000,
        .m_OpClock=8000000,
        .m_ChWidth=SPI_BYTE,
        .m_BusWidth=SPI_BYTE,
#ifdef DEF_IF_INTERRUPT_MODE
        .m_RxTransferMode=SPI_INTERRUPT_MODE,
        .m_TxTransferMode=SPI_INTERRUPT_MODE,
#else
       .m_RxTransferMode=SPI_POLLING_MODE,
       .m_TxTransferMode=SPI_POLLING_MODE,
#endif
        .m_loopback=0,     // self loopback mode
        .m_FBClkSel = SPI_PHASE_DELAY90,
        .m_CPOL = SPI_ACTIVE_HIGH,//Active high
        .m_CPHA = SPI_FORMAT_A,//Format A
    },
    [1] = {
        .m_portId=SPI_CHUB1,
        .m_EnAutoCS = 0,
        .m_IRQ = USI_CHUB1_IRQn,
        .m_OpMode = SPI_MASTER,
        .m_TargetSPICLK = 8000000,
        .m_OpClock=8000000,
        .m_ChWidth=SPI_BYTE,
        .m_BusWidth=SPI_BYTE,
#ifdef DEF_IF_INTERRUPT_MODE
        .m_RxTransferMode=SPI_INTERRUPT_MODE,
        .m_TxTransferMode=SPI_INTERRUPT_MODE,
#else
       .m_RxTransferMode=SPI_POLLING_MODE,
       .m_TxTransferMode=SPI_POLLING_MODE,
#endif
        .m_loopback=0,     // self loopback mode
        .m_FBClkSel = SPI_PHASE_DELAY90,
        .m_CPOL = SPI_ACTIVE_HIGH,//Active high
        .m_CPHA = SPI_FORMAT_A,//Format A
    },
};

void spiBoardInit(void)
{
    int i, num;
    uint32_t portNum;
    uint32_t inputClock, clockDivider;
    CmuIpType   spiType;

    num = sizeof(mSpiDevConf)/sizeof(spi_device_t);
    for(i=0; i<num; i++){
        portNum = mSpiDevConf[i].m_portId;
        spiSetDeviceInfo(portNum, &mSpiDevConf[i]);

        /* Set divider value of USI based on SPI operation clock */
        if      (portNum == SPI_CHUB0)  spiType=CMU_CLK_CHUB_SPI0;
        else if (portNum == SPI_CHUB1)  spiType=CMU_CLK_CHUB_SPI1;
        else if (portNum == SPI_CHUB2)  spiType=CMU_CLK_CHUB_SPI2;
        else if (portNum == SPI_CHUB3)  spiType=CMU_CLK_CHUB_SPI3;
        else    spiType = CMU_MAX_IP_TYPE;

        inputClock = cmuGetSpeed(spiType);
        clockDivider = (inputClock + (mSpiDevConf[i].m_OpClock * 4) - 1) / (mSpiDevConf[i].m_OpClock * 4);
        cmuSetDivider(spiType, clockDivider);
    }
}
