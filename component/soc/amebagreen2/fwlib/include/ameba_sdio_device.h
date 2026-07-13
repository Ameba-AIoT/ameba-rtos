/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_SDIO_DEVICE_H_
#define _AMEBA_SDIO_DEVICE_H_

#include "ameba_inic.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup SDIO SDIO
  * @{
  */

/** @defgroup SDIO_Exported_Constants SDIO Exported Constants
  * @{
  */

#define SPDIO_DMA_ALIGN_4			4   /*!< DMA alignment requirement in bytes for SPDIO buffers. */

#define SPDIO_TX_BD_BUF_SZ_UNIT		64   /*!< TX BD buffer size unit; must be a multiple of 64 bytes. */

#define SPDIO_RX_BD_FREE_TH			5   /*!< Threshold of free RX BDs before notifying the host. */
#define SPDIO_MIN_RX_BD_SEND_PKT	2   /*!< Minimum number of RX BDs required to send a packet. */
#define SPDIO_MAX_RX_BD_BUF_SIZE	16380   /*!< Maximum buffer size a single RX BD can point to (4-byte aligned). */

/** @brief Initial interrupt mask for SDIO device operation. */
#define SDIO_INIT_INT_MASK			(SDIO_WIFI_BIT_H2C_DMA_OK | SDIO_WIFI_BIT_C2H_DMA_OK | \
									SDIO_WIFI_BIT_H2C_BUS_RES_FAIL | SDIO_WIFI_BIT_RX_BD_FLAG_ERR_INT | SDIO_NOTIFY_TYPE_INT)

/** @brief Interrupt bits for SDIO WiFi notification events. */
#define SDIO_NOTIFY_TYPE_INT		(SDIO_WIFI_BIT_H2C_MSG_INT | SDIO_WIFI_BIT_RPWM1_INT | \
									SDIO_WIFI_BIT_RPWM2_INT | SDIO_WIFI_BIT_HOST_WAKE_CPU_INT | SDIO_WIFI_BIT_H2C_SUS_REQ)

/** @brief Checks whether x is a valid SDIO device (WiFi or BT). */
#define IS_SDIO_DEVICE(x)			(((x) == SDIO_WIFI) || ((x) == SDIO_BT))

/** @} */

/** @defgroup SDIO_Enumeration_Type Enumeration Type
  * @{
  */

/*Don't modify this enum table*/
/** @brief RX data type identifiers for SPDIO receive buffers. */
enum spdio_rx_data_t {
	SPDIO_RX_DATA_NULL = 0x00,  /*!< No data type. */
	SPDIO_RX_DATA_ETH = 0x83,  /*!< Ethernet packet received. */
	SPDIO_RX_DATA_ATCMD = 0x11, /*!< AT command packet received. */
	SPDIO_RX_DATA_USER = 0x41,  /*!< User-defined data type. */
};

/** @brief TX data type identifiers for SPDIO transmit buffers. */
enum spdio_tx_data_t {
	SPDIO_TX_DATA_NULL = 0x00,      /*!< No data type. */
	SPDIO_TX_DATA_ETH = 0x82,       /*!< Ethernet packet sent. */
	SPDIO_TX_DATA_ATCMDRSP = 0x10,  /*!< AT command response packet sent. */
	SPDIO_TX_DATA_USER = 0x40,      /*!< User-defined data type. */
};

/** @}*/

/** @defgroup SDIO_Exported_Types SDIO Exported Types
  * @{
  */

/**
  * @brief SPDIO buffer structure used for TX and RX data transfer.
  */
struct spdio_buf_t {
	void *priv;             /*!< Private data from user. */
	u32 buf_allocated;      /*!< Allocated address of the SPDIO buffer. */
	u16 size_allocated;     /*!< Actual allocated size of the buffer. */
	u32 buf_addr;           /*!< Physical address of the SPDIO buffer, must be 4-byte aligned. */
	u16 buf_size;           /*!< Size of valid data in the buffer. */
	u8 type;                /*!< Data type carried by this buffer, refer to @ref spdio_rx_data_t and @ref spdio_tx_data_t. */
	u8 reserved;            /*!< Reserved. */
};

/**
  * @brief TX Buffer Descriptor structure.
  */
typedef struct {
	u32	Address;		/* The TX buffer physical address, it must be 4-byte aligned */
} SPDIO_TX_BD;

/**
  * @brief Handle structure binding a TX_BD with a TX packet.
  */
typedef struct {
	SPDIO_TX_BD *pTXBD;		/*!< Pointer to the TX BD buffer. */
	struct spdio_buf_t *dev_rx_buf;		/*!< Pointer to the device receive buffer associated with this TX BD. */
	u8 isPktEnd;			/*!< Indicates whether this BD is the last segment of a packet that spans more than one BD. */
	u8 isFree;				/*!< Indicates whether this TX BD is free. */
} SPDIO_TX_BD_HANDLE;

/**
  * @brief RX Buffer Descriptor structure.
  */
typedef struct {
	u32 BuffSize: 14;		/*!< bit[13:0]: RX buffer size, maximum 16384-1. */
	u32 LS: 1;				/*!< bit[14]: Last Segment flag. */
	u32 FS: 1;				/*!< bit[15]: First Segment flag. */
	u32 Seq: 16;			/*!< bit[31:16]: Sequence number, reserved and unused. */
	u32 PhyAddr;			/*!< Physical address of the RX buffer; must be 4-byte aligned. */
} SPDIO_RX_BD;

/**
  * @brief Handle structure binding a RX_BD with a RX packet.
  */
typedef struct {
	struct spdio_buf_t *dev_tx_buf;		/*!< Pointer to the device transmit buffer associated with this RX BD. */
	SPDIO_RX_BD *pRXBD;		/*!< Pointer to the RX BD buffer. */
	INIC_RX_DESC *pRXDESC;	/*!< Pointer to the RX packet descriptor. */
	u8 isPktEnd;			/*!< Indicates whether this BD is the last segment of a packet that spans more than one BD. */
	u8 isFree;				/*!< Indicates whether this RX BD is free (DMA complete and the associated packet has been freed). */
} SPDIO_RX_BD_HANDLE;

/**
  * @brief SPDIO adapter structure containing all SDIO device state.
  */
typedef struct {
	void *spdio_priv;				/*!< Private data from the user. */
	SPDIO_TX_BD_HANDLE *pTXBDHdl;	/*!< Pointer to the allocated memory for the TX BD handle array. */
	SPDIO_TX_BD *pTXBDAddr;			/*!< Start address of the TX BD array; must be 4-byte aligned. */
	u16 TXBDWPtr;					/*!< Local write index of the SDIO TX (Host-to-Device) BD ring, distinct from the HW-maintained write index. */
	u16 TXBDRPtr;					/*!< Read index of the SDIO TX (Host-to-Device) BD ring. */

	SPDIO_RX_BD_HANDLE *pRXBDHdl;	/*!< Pointer to the allocated memory for the RX BD handle array. */
	SPDIO_RX_BD *pRXBDAddr;			/*!< Start address of the RX BD array; must be 8-byte aligned. */
	INIC_RX_DESC *pRXDESCAddr;			/*!< Pointer to the RX descriptor array. */
	u16 RXBDWPtr;					/*!< Write index of the SDIO RX (Device-to-Host) BD ring. */
	u16 RXBDRPtr;					/*!< Local read index of the SDIO RX (Device-to-Host) BD ring, distinct from the HW-maintained read index. */

	u16 host_rx_bd_num;				/*!< Number of RX BDs on the host side; for WiFi, 2 BDs are required per packet, so this value must be a multiple of 2. */
	u16 host_tx_bd_num;				/*!< Number of TX BDs on the host side (data received from host). */
	u16 device_rx_bufsz;			/*!< RX buffer size per BD: equals desired packet length plus 24 bytes (SPDIO header), must be a multiple of 64, maximum 16 KB. */

	u8 WaitForDeviceRxbuf;				/*!< Flag indicating the device is waiting for an available RX buffer. */
	u8 TxOverFlow;						/*!< Flag indicating a TX buffer overflow has occurred. */
} SPDIO_ADAPTER, *PSPDIO_ADAPTER;

/** @brief Callback function type invoked when SDIO device TX transfer to host completes. */
typedef s8(*spdio_device_tx_done_cb_ptr)(PSPDIO_ADAPTER pSPDIODev, struct spdio_buf_t *buf);
/** @brief Callback function type invoked when SDIO device RX transfer from host completes. */
typedef s8(*spdio_device_rx_done_cb_ptr)(PSPDIO_ADAPTER pSPDIODev, struct spdio_buf_t *buf, u8 *pdata, u16 pktsize, u8 type);

/** @} */

/* Exported functions --------------------------------------------------------*/
/** @defgroup SDIO_Exported_Functions SDIO Exported Functions
  * @{
  */

void SDIO_TxBdHdl_Init(SPDIO_TX_BD_HANDLE *g_TXBDHdl, SPDIO_TX_BD *SPDIO_TXBDAddr, struct spdio_buf_t *spdio_dev_rx_buf, u16 host_tx_bd_num);
void SDIO_RxBdHdl_Init(SPDIO_RX_BD_HANDLE *g_RXBDHdl, SPDIO_RX_BD *SPDIO_RXBDAddr, INIC_RX_DESC *g_RXDESCAddr, u16 host_rx_bd_num);
void SPDIO_Notify_INT(SDIO_TypeDef *SDIO, u16 IntStatus);
void SPDIO_Recycle_Rx_BD(SDIO_TypeDef *SDIO, PSPDIO_ADAPTER pSPDIODev, spdio_device_tx_done_cb_ptr spdio_device_tx_done_cb);
u8 SPDIO_DeviceTx(SDIO_TypeDef *SDIO, PSPDIO_ADAPTER pSPDIODev, struct spdio_buf_t *pbuf);
/// @cond
void SPDIO_TxBd_DataReady_DeviceRx(SDIO_TypeDef *SDIO, PSPDIO_ADAPTER pSPDIODev, spdio_device_rx_done_cb_ptr spdio_device_rx_done_cb);
void SPDIO_Device_Init(SDIO_TypeDef *SDIO, PSPDIO_ADAPTER pSPDIODev);
void SPDIO_Device_DeInit(SDIO_TypeDef *SDIO);
/// @endcond
/**
  * @}
  */


/** @} */

/** @} */

#ifdef __cplusplus
}
#endif
#endif
