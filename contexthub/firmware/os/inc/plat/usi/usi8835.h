/*----------------------------------------------------------------------------
 *      Exynos SoC  -  USI
 *----------------------------------------------------------------------------
 *      Name:    usi8835.h
 *      Purpose: To expose USI driver functions
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
void usi00_IRQHandler(void); //USI_CHUB0
void usi01_IRQHandler(void); //USI_CHUB1
void usi02_IRQHandler(void); //I2C_CHUB1
void usi03_IRQHandler(void); //USI_CHUB2
void usi04_IRQHandler(void); //USI_CHUB3
void usi05_IRQHandler(void); //I2C_CHUB3
void usi06_IRQHandler(void); //USI_CMGP0
void usi07_IRQHandler(void); //I2C_CMGP0
void usi08_IRQHandler(void); //USI_CMGP1
void usi09_IRQHandler(void); //I2C_CMGP1
void usi10_IRQHandler(void); //USI_CMGP2
void usi11_IRQHandler(void); //I2C_CMGP2
void usi12_IRQHandler(void); //USI_CMGP3
void usi13_IRQHandler(void); //I2C_CMGP3
void usi14_IRQHandler(void); //USI_CMGP4
void usi15_IRQHandler(void); //I2C_CMGP4
void usi16_IRQHandler(void); //USI_CMGP5
void usi17_IRQHandler(void); //I2C_CMGP5
void usi18_IRQHandler(void); //USI_CMGP6
void usi19_IRQHandler(void); //I2C_CMGP6
