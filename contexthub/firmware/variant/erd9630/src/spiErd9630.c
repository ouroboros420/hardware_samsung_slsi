/*----------------------------------------------------------------------------
 *      ERD9630  -  SPI
 *----------------------------------------------------------------------------
 *      Name:    spiErd9630.c
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

#include <spiErd9630.h>
#include <csp_common9630.h>
#include <plat/spi/spi.h>
#include <spiDrv.h>
#include <usi.h>

static const spi_device_t mSpiDevConf[] = {
    [0] = {
        .m_portId=0,
        .m_EnAutoCS = 0,
        .m_OpMode = SPI_MASTER,
        .m_TargetSPICLK = 8000000,
        .m_OpClock=8000000,
        .m_ChWidth=SPI_BYTE,
        .m_BusWidth=SPI_BYTE,
        .m_RxTransferMode=SPI_POLLING_MODE,
        .m_TxTransferMode=SPI_POLLING_MODE,
        .m_loopback=0,     // self loopback mode
        .m_FBClkSel = SPI_PHASE_DELAY90,
        .m_CPOL = SPI_ACTIVE_LOW,//Active low
        .m_CPHA = SPI_FORMAT_B,//Format B
    },
};

void spiBoardInit(void)
{
    int i, num;
    uint32_t portNum;

    num = sizeof(mSpiDevConf)/sizeof(spi_device_t);
    for(i=0; i<num; i++){
        portNum = mSpiDevConf[i].m_portId;
        if(usiOpen(portNum, USI_PROTOCOL_SPI, CMGP_DEFAULT_HWACG))
            CSP_PRINTF_ERROR("\nERROR spiBoardInit!!!\n");
        spiSetDeviceInfo(i, &mSpiDevConf[i]);
    }
}
