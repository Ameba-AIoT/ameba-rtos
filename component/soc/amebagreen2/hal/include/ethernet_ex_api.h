/* mbed Microcontroller Library
  ******************************************************************************
 *
  * Copyright(c) 2006 - 2022 Realtek Corporation. All rights reserved.
  *
  * SPDX-License-Identifier: Apache-2.0
  *
  * Licensed under the Apache License, Version 2.0 (the License); you may
  * not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  * http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an AS IS BASIS, WITHOUT
  * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
 */
#ifndef ETHERNET_EX_API_H
#define ETHERNET_EX_API_H

#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif


/// the size of the packet buffer
#define ETH_PKT_BUF_SIZE	1600

/**
  \brief  Defines the link speed.
*/
typedef enum {
	Eth_Spd_Auto    = -1,
	Eth_Spd_10M     = 0,
	Eth_Spd_100M    = 1
} EthSpeedSel;

/**
  \brief  Defines the duplex mode.
*/
typedef enum {
	Eth_Duplex_Auto = -1,
	Eth_Duplex_Half = 0,
	Eth_Duplex_Full = 1
} EthDuplexMode;

/**
  \brief  The function type of the Ethernet MAC controller interrupt callback function.
*/
typedef void (*ethernet_callback)(uint32_t event, uint32_t data);

/**
 *  @brief To hook a callback function for Ethernet MAC controller interrupt.
 *
 *  @param[in]  callback The callback function.
 *
 *  @returns    void
 */
void ethernet_irq_hook(ethernet_callback callback);

/**
 *  @brief To set the Tx/Rx descriptor number.
 *
 *  @param[in]  txdescCnt The specified Tx descriptor number.
 *  @param[in]  rxdescCnt The specified Rx descriptor number.
 *
 *  @returns    void.
 */
void ethernet_set_descnum(uint8_t txdescCnt, uint8_t rxdescCnt);

/**
 *  @brief To set the start address of Tx/Rx descriptor ring and Tx/Rx packet buffer.
 *
 *  @param[in]  TxDescAddr The start address of Tx descriptor ring.
 *  @param[in]  RxDescAddr The start address of Rx descriptor ring.
 *  @param[in]  pTxPktBuf The start address of Tx packet buffer.
 *  @param[in]  pRxPktBuf The start address of Rx packet buffer.
 *
 *  @returns    void.
 */
void ethernet_trx_pre_setting(uint8_t *TxDescAddr, uint8_t *RxDescAddr, uint8_t *pTxPktBuf, uint8_t *pRxPktBuf);

#ifdef __cplusplus
}
#endif

#endif  // #ifndef ETHERNET_EX_API_H

