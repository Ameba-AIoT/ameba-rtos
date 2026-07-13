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

#include "ameba_soc.h"

#ifdef __cplusplus
extern "C" {
#endif

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
#define SPDIO_DMA_ALIGN_4			4   /*!< DMA alignment requirement in bytes for SPDIO buffers. */
#define SPDIO_RX_BUFSZ_ALIGN(x)	((((x-1)>>6)+1)<<6)   /*!< Align RX buffer size x to the next multiple of 64 bytes. */

#define SPDIO_RXDESC_SZ	24   /*!< Size of the SPDIO receive descriptor in bytes. */

/** @}*/

/** @defgroup MBED_SPDIO_Exported_Types MBED_SPDIO Exported Types
 * @{
 */

/** @defgroup MBED_SPDIO_Enumeration_Type Enumeration Type
 * @{
 */

/*Don't modify this enum table*/
/** @brief Data type identifiers for SPDIO receive buffers. */
enum spdio_rx_data_t {
	SPDIO_RX_DATA_NULL = 0x00,    /*!< No data type. */
	SPDIO_RX_DATA_ETH = 0x83,     /*!< Ethernet packet received. */
	SPDIO_RX_DATA_ATCMD = 0x11,   /*!< AT command packet received. */
	SPDIO_RX_DATA_USER = 0x41,    /*!< User-defined data type. */
};

/** @brief Data type identifiers for SPDIO transmit buffers. */
enum spdio_tx_data_t {
	SPDIO_TX_DATA_NULL = 0x00,      /*!< No data type. */
	SPDIO_TX_DATA_ETH = 0x82,       /*!< Ethernet packet sent. */
	SPDIO_TX_DATA_ATCMDRSP = 0x10,  /*!< AT command response packet sent. */
	SPDIO_TX_DATA_USER = 0x40,      /*!< User-defined data type. */
};

/** @}*/

/** @defgroup MBED_SPDIO_Structure_Type Structure Type
 * @{
 */

/** @brief SPDIO buffer structure for TX and RX data transfer. */
struct spdio_buf_t {
	void *priv;           /*!< Private data from user. */
	u32 buf_allocated;    /*!< Allocated address of the SPDIO buffer. */
	u16 size_allocated;   /*!< Actual allocated size of the buffer. */
	u32 buf_addr;         /*!< Physical address of the SPDIO buffer; must be 4-byte aligned. */
	u16 buf_size;         /*!< Size of valid data in the buffer. */
	u8 type;              /*!< Data type of the buffer payload; see @ref spdio_rx_data_t and @ref spdio_tx_data_t. */
	u8 reserved;          /*!< Reserved. */
};

/** @brief MBED SPDIO driver configuration and callback structure. */
struct spdio_t {
	SDIO_TypeDef *pSDIO;        /*!< Pointer to SDIO hardware base address (SDIO_WIFI only). */
	void *priv;                 /*!< Reserved for internal use; not set by user. */
	u32 host_rx_bd_num;         /*!< Number of host RX BDs for sending data to host; 2 BDs per packet, must be even. */
	u32 host_tx_bd_num;         /*!< Number of host TX BDs for receiving data from host. */
	u32 device_rx_bufsz;        /*!< RX buffer size: desired packet length + 24-byte SPDIO header; must be a multiple of 64. */
	struct spdio_buf_t *rx_buf; /*!< RX buffer array pre-allocated by user; total size = device_rx_bufsz * host_tx_bd_num. */

	/**
	 * @brief Callback function defined by user, called by SPDIO when one packet is received.
	 * @param priv Pointer to @ref spdio_t structure which is used to initialize the SPDIO interface.
	 * @param pbuf Pointer to @ref spdio_buf_t structure which is the SPDIO receive buffer.
	 * @param pdata Actual received packet payload.
	 * @param size Actual payload length.
	 * @param type Received packet type, which should be a value of @ref spdio_rx_data_t.
	 * @return RTK_SUCCESS or RTK_FAIL.
	 */
	char (*device_rx_done_cb)(void *priv, void *pbuf, u8 *pdata, u16 size, u8 type);

	/**
	 * @brief Callback function defined by user, called by SPDIO when one packet is sent.
	 * @param priv Pointer to @ref spdio_t structure which is used to initialize the SPDIO interface.
	 * @param pbuf Pointer to @ref spdio_buf_t structure which carries the transmit packet.
	 * @return RTK_SUCCESS or RTK_FAIL.
	 */
	char (*device_tx_done_cb)(void *priv, void *pbuf);

	/**
	 * @brief Callback function defined by user to respond to RPWM from host.
	 * @param priv Pointer to @ref spdio_t structure which is used to initialize the SPDIO interface.
	 * @param value RPWM2 value.
	 * @return RTK_SUCCESS or RTK_FAIL.
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
/// @cond
void SPDIO_Board_Init(void);
bool SPDIO_Device_Init(struct spdio_t *obj);
void SPDIO_Device_DeInit(void);
/// @endcond

/**
 * @brief An @ref spdio_t object that must be initialized before calling @ref spdio_init().
 */
extern struct spdio_t *g_spdio_priv;

#ifdef __cplusplus
}
#endif

#endif // #ifndef __SPDIO_API_H__
