/*----------------------------------------------------------------------------
 *      Exynos SoC  -  SPI UTC
 *----------------------------------------------------------------------------
 *      Name:    spi_test.c
 *      Purpose: To implement spi UTC codes
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

#if defined(UTC_REQUIRED)

#include <spi_test.h>
#include <plat/spi/spi.h>
#include <plat/spi/spiDrv.h>
#include <spi.h>
#if defined(SEOS)
    #include <spi_priv.h>
#endif

#define SPI_TEST_PORT SPI_CMGP2
#define DEVICE_ID    0xD1


void spi_test(void)
{
    uint8_t id = 0;
    int ret;

    CSP_PRINTF_INFO("exynos_utc:: spi_test\n");

    spiOpen(SPI_TEST_PORT, NULL);
    ret = spiRead(SPI_TEST_PORT, 0x0, &id, 1);
    //spiMasterTransfer(i, &txBuf[0], &rxBuf[0], 16, NULL);

    if (ret)
    {
        CSP_PRINTF_ERROR("SPI Test error..\n");
        return;
    }

    CSP_PRINTF_INFO("spi_test result : %s\n", id==DEVICE_ID?"PASS":"FAIL");
    CSP_PRINTF_INFO("  ID = 0x%X\n", id);
}

void spi_test_clean_up(void)
{
    // Do clean-up

    CSP_PRINTF_ERROR("%s is called\n", __func__);
}

#endif

