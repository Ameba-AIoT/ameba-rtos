/*
 * Copyright (c) 2019-2023 Arm Limited. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

#ifndef __RTE_DEVICE_H
#define __RTE_DEVICE_H

// <q> USART (Universal synchronous - asynchronous receiver transmitter) [Driver_USART0]
// <i> Configuration settings for Driver_USART0 in component ::Drivers:USART
#define   RTE_USART0                     1

// <q> MPC (Memory Protection Controller) [Driver_VM0_MPC]
// <i> Configuration settings for Driver_VM0_MPC in component ::Drivers:MPC
#define   RTE_VM0_MPC                 1

// <q> MPC (Memory Protection Controller) [Driver_VM1_MPC]
// <i> Configuration settings for Driver_VM1_MPC in component ::Drivers:MPC
#define   RTE_VM1_MPC                 1

// <q> MPC (Memory Protection Controller) [Driver_SIC_MPC]
// <i> Configuration settings for Driver_SIC_MPC in component ::Drivers:MPC
#ifdef RSS_XIP
#define   RTE_SIC_MPC                 1
#endif /* RSS_XIP */

// <q> PPC (Peripheral Protection Controller) [PPC_RSS_MAIN0]
// <i> Configuration settings for Driver_PPC_RSS_MAIN0 in component ::Drivers:PPC
#define   RTE_PPC_RSS_MAIN0             1

// <q> PPC (Peripheral Protection Controller) [PPC_RSS_MAIN_EXP0]
// <i> Configuration settings for Driver_PPC_RSS_MAIN_EXP0 in component ::Drivers:PPC
#define   RTE_PPC_RSS_MAIN_EXP0             1

// <q> PPC (Peripheral Protection Controller) [PPC_RSS_MAIN_EXP1]
// <i> Configuration settings for Driver_PPC_RSS_MAIN_EXP1 in component ::Drivers:PPC
#define   RTE_PPC_RSS_MAIN_EXP1             1

// <q> PPC (Peripheral Protection Controller) [PPC_RSS_MAIN_EXP2]
// <i> Configuration settings for Driver_PPC_RSS_MAIN_EXP2 in component ::Drivers:PPC
#define   RTE_PPC_RSS_MAIN_EXP2             1

// <q> PPC (Peripheral Protection Controller) [PPC_RSS_MAIN_EXP3]
// <i> Configuration settings for Driver_PPC_RSS_MAIN_EXP3 in component ::Drivers:PPC
#define   RTE_PPC_RSS_MAIN_EXP3             1

// <q> PPC (Peripheral Protection Controller) [PPC_RSS_PERIPH0]
// <i> Configuration settings for Driver_PPC_RSS_PERIPH0 in component ::Drivers:PPC
#define   RTE_PPC_RSS_PERIPH0             1

// <q> PPC (Peripheral Protection Controller) [PPC_RSS_PERIPH1]
// <i> Configuration settings for Driver_PPC_RSS_PERIPH1 in component ::Drivers:PPC
#define   RTE_PPC_RSS_PERIPH1             1

// <q> PPC (Peripheral Protection Controller) [PPC_RSS_PERIPH_EXP0]
// <i> Configuration settings for Driver_PPC_RSS_PERIPH_EXP0 in component ::Drivers:PPC
#define   RTE_PPC_RSS_PERIPH_EXP0             1

// <q> PPC (Peripheral Protection Controller) [PPC_RSS_PERIPH_EXP1]
// <i> Configuration settings for Driver_PPC_RSS_PERIPH_EXP1 in component ::Drivers:PPC
#define   RTE_PPC_RSS_PERIPH_EXP1             1

// <q> PPC (Peripheral Protection Controller) [PPC_RSS_PERIPH_EXP2]
// <i> Configuration settings for Driver_PPC_RSS_PERIPH_EXP2 in component ::Drivers:PPC
#define   RTE_PPC_RSS_PERIPH_EXP2             1

// <q> PPC (Peripheral Protection Controller) [PPC_RSS_PERIPH_EXP3]
// <i> Configuration settings for Driver_PPC_RSS_PERIPH_EXP3 in component ::Drivers:PPC
#define   RTE_PPC_RSS_PERIPH_EXP3             1

// <q> Flash device emulated by SRAM [Driver_Flash0]
// <i> Configuration settings for Driver_Flash0 in component ::Drivers:Flash
#define   RTE_FLASH0                     1

#endif  /* __RTE_DEVICE_H */
