/** mbed Microcontroller Library
  ******************************************************************************
  * @file    spdio_api.h
  * @author
  * @version V1.0.0
  * @brief   This file provides following mbed SPDIO API
  ******************************************************************************
  * @attention
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
#ifndef __SPDIO_API_H__
#define __SPDIO_API_H__

#include "basic_types.h"

/** @addtogroup Ameba_Mbed_API
 * @{
 */

/** @addtogroup MBED_SPDIO
 *  @brief MBED_SPDIO driver modules.
 *  @{
 */

/** @defgroup MBED_SPDIO_Exported_Constants MBED_SPDIO Exported Constants
 * @{
 */
#define SPDIO_DMA_ALIGN_4			4
#define SPDIO_RX_BUFSZ_ALIGN(x)	((((x-1)>>6)+1)<<6) //alignment to 64

#define SPDIO_RXDESC_SZ	24

/** @}*/

/** @defgroup MBED_SPDIO_Exported_Types MBED_SPDIO Exported Types
 * @{
 */

/** @defgroup MBED_SPDIO_Enumeration_Type Enumeration Type
 * @{
 */

/*Don't modify this enum table*/
enum spdio_rx_data_t {
	SPDIO_RX_DATA_NULL = 0x00,
	SPDIO_RX_DATA_ETH = 0x83, //an ethernet packet received
	SPDIO_RX_DATA_ATCMD = 0x11, //an AT command packet received
	SPDIO_RX_DATA_USER = 0x41, //defined by user
};

enum spdio_tx_data_t {
	SPDIO_TX_DATA_NULL = 0x00,
	SPDIO_TX_DATA_ETH = 0x82, //an ethernet packet sent
	SPDIO_TX_DATA_ATCMDRSP = 0x10, //an AT command response packet sent
	SPDIO_TX_DATA_USER = 0x40, // defined by user
};

/** @}*/

/** @defgroup MBED_SPDIO_Structure_Type Structure Type
 * @{
 */

struct spdio_buf_t {
	void *priv; //priv data from user
	u32 buf_allocated; //The spdio buffer allocated address
	u16 size_allocated; //The actual allocated size
	u32 buf_addr; //The spdio buffer physical address, it must be 4-bytes aligned
	u16 buf_size;
	u8 type; //The type of the data which this buffer carries, spdio_rx_data_t and spdio_tx_data_t
	u8 reserved;
};

struct spdio_t {
	void *priv; //not used by user
	u32 tx_bd_num; //for spdio send data to host, 2 bd for one packet, so this value must be rounded to 2
	u32 rx_bd_num; //for spdio receive data from host
	u32 rx_bd_bufsz; //buffer size = desired packet length + 24(spdio header info), must be rounded to 64
	struct spdio_buf_t *rx_buf; //buffer array for spdio receive assigned by user, rx_bd_bufsz * rx_bd_num

	/**
	 * @brief Callback function defined by user, called by spdio when one packet is received.
	 * @param priv Pointer to spdio_t structure which is used to initialize spdio interface.
	 * @param pbuf Pointer to spdio_buf_t structure which is spdio receive buffer.
	 * @param pdata Actual received packet payload.
	 * @param size Actual payload length.
	 * @param type Received packet type, which should be a value of @ref spdio_rx_data_t.
	 * @retval RTK_SUCCESS or RTK_FAIL.
	 */
	char (*rx_done_cb)(void *priv, void *pbuf, u8 *pdata, u16 size, u8 type);

	/**
	 * @brief Callback function defined by user, called by spdio when one packet is sent.
	 * @param priv Pointer to spdio_t structure which is used to initialize spdio interface.
	 * @param pbuf Pointer to spdio_buf_t structure which carries the transmit packet.
	 * @retval RTK_SUCCESS or RTK_FAIL.
	 */
	char (*tx_done_cb)(void *priv, void *pbuf);

	/**
	 * @brief Callback function defined by user.
	 * @param priv Pointer to spdio_t structure which is used to initialize spdio interface.
	 * @param value RPWM2 value.
	 * @retval RTK_SUCCESS or RTK_FAIL.
	 */
	char (*rpwm_cb)(void *priv, u16 value);
};

/** @}*/
/** @}*/
/** @}*/
/** @}*/

void spdio_structinit(struct spdio_t *obj);
void spdio_init(struct spdio_t *obj);
void spdio_deinit(struct spdio_t *obj);
s8 spdio_tx(struct spdio_t *obj, struct spdio_buf_t *pbuf);
void spdio_trigger_rx_handle(void);
void SPDIO_Board_Init(void);

/**
 * @brief An obj which will be used to initialize SDIO interface,
 * 		so it must be initialized before by calling HalSdioInit();
 */
extern struct spdio_t *g_spdio_priv;

#endif // #ifndef __SPDIO_API_H__