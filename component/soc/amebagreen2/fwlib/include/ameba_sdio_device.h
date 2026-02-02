/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_SDIO_DEVICE_H_
#define _AMEBA_SDIO_DEVICE_H_

#include "ameba_inic.h"

#define SPDIO_DMA_ALIGN_4			4

#define SPDIO_TX_BD_BUF_SZ_UNIT		64 // the TX BD buffer size shall be an integer multiple of 64 bytes

#define SPDIO_RX_BD_FREE_TH			5
#define SPDIO_MIN_RX_BD_SEND_PKT	2
#define SPDIO_MAX_RX_BD_BUF_SIZE	16380	// the Maximum size for a RX_BD point to, make it 4-bytes aligned

#define SDIO_INIT_INT_MASK			(SDIO_WIFI_BIT_H2C_DMA_OK | SDIO_WIFI_BIT_C2H_DMA_OK | \
									SDIO_WIFI_BIT_H2C_BUS_RES_FAIL | SDIO_WIFI_BIT_RX_BD_FLAG_ERR_INT | SDIO_NOTIFY_TYPE_INT)

#define SDIO_NOTIFY_TYPE_INT		(SDIO_WIFI_BIT_H2C_MSG_INT | SDIO_WIFI_BIT_RPWM1_INT | \
									SDIO_WIFI_BIT_RPWM2_INT | SDIO_WIFI_BIT_HOST_WAKE_CPU_INT | SDIO_WIFI_BIT_H2C_SUS_REQ)

#define IS_SDIO_DEVICE(x)			(((x) == SDIO_WIFI) || ((x) == SDIO_BT))

/** @defgroup MBED_SPDIO_Enumeration_Type Enumeration Type
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

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

struct spdio_buf_t {
	void *priv; //priv data from user
	u32 buf_allocated; //The spdio buffer allocated address
	u16 size_allocated; //The actual allocated size
	u32 buf_addr; //The spdio buffer physical address, it must be 4-bytes aligned
	u16 buf_size;
	u8 type; //The type of the data which this buffer carries, spdio_rx_data_t and spdio_tx_data_t
	u8 reserved;
};

typedef struct {
	u32	Address;		/* The TX buffer physical address, it must be 4-bytes aligned */
} SPDIO_TX_BD;

/* the data structer to bind a TX_BD with a TX Packet */
typedef struct {
	SPDIO_TX_BD *pTXBD;		/* Point to the TX_BD buffer */
	struct spdio_buf_t *dev_rx_buf;
	u8 isPktEnd;			/* For a packet over 1 BD , this flag to indicate is this BD contains a packet end */
	u8 isFree;				/* is this TX BD free */
} SPDIO_TX_BD_HANDLE;

/* The RX Buffer Descriptor format */
typedef struct {
	u32 BuffSize: 14;		/* bit[13:0], RX Buffer Size, Maximum 16384-1 */
	u32 LS: 1;				/* bit[14], is the Last Segment ? */
	u32 FS: 1;				/* bit[15], is the First Segment ? */
	u32 Seq: 16;			/* bit[31:16], The sequence number, it's no use for now */
	u32 PhyAddr;			/* The RX buffer physical address, it must be 4-bytes aligned */
} SPDIO_RX_BD;

/* the data structer to bind a RX_BD with a RX Packet */
typedef struct {
	struct spdio_buf_t *dev_tx_buf;
	SPDIO_RX_BD *pRXBD;		/* Point to the RX_BD buffer */
	INIC_RX_DESC *pRXDESC;	/* point to the Rx Packet */
	u8 isPktEnd;			/*  For a packet over 1 BD , this flag to indicate is this BD contains a packet end */
	u8 isFree;				/* is this RX BD free (DMA done and its RX packet has been freed) */
} SPDIO_RX_BD_HANDLE;

typedef struct {
	void *spdio_priv;				/* Data from User */
	SPDIO_TX_BD_HANDLE *pTXBDHdl;	/* point to the allocated memory for TX_BD Handle array */
	SPDIO_TX_BD *pTXBDAddr;			/* The TX_BD start address, it must be 4-bytes aligned */
	u16 TXBDWPtr;					/* The SDIO TX(Host->Device) BD local write index, different with HW maintained write Index. */
	u16 TXBDRPtr;					/* The SDIO TX(Host->Device) BD read index */

	SPDIO_RX_BD_HANDLE *pRXBDHdl;	/* point to the allocated memory for RX_BD Handle array */
	SPDIO_RX_BD *pRXBDAddr;			/* The RX_BD start address, it must be 8-bytes aligned */
	INIC_RX_DESC *pRXDESCAddr;
	u16 RXBDWPtr;					/* The SDIO RX(Device->Host) BD write index */
	u16 RXBDRPtr;					/* The SDIO RX(Device->Host) BD local read index, different with HW maintained Read Index. */

	u16 host_rx_bd_num; //for spdio send data to host, 2 bd for one packet, so this value must be rounded to 2
	u16 host_tx_bd_num; //for spdio receive data from host
	u16 device_rx_bufsz; 			/* buffer size = desired packet length + 24(spdio header info), must be rounded to 64, max value is 16KB */

	u8 WaitForDeviceRxbuf;
	u8 TxOverFlow;
} SPDIO_ADAPTER, *PSPDIO_ADAPTER;

typedef s8(*spdio_device_tx_done_cb_ptr)(PSPDIO_ADAPTER pSPDIODev, struct spdio_buf_t *buf);
typedef s8(*spdio_device_rx_done_cb_ptr)(PSPDIO_ADAPTER pSPDIODev, struct spdio_buf_t *buf, u8 *pdata, u16 pktsize, u8 type);

void SDIO_TxBdHdl_Init(SPDIO_TX_BD_HANDLE *g_TXBDHdl, SPDIO_TX_BD *SPDIO_TXBDAddr, struct spdio_buf_t *spdio_dev_rx_buf, u16 host_tx_bd_num);
void SDIO_RxBdHdl_Init(SPDIO_RX_BD_HANDLE *g_RXBDHdl, SPDIO_RX_BD *SPDIO_RXBDAddr, INIC_RX_DESC *g_RXDESCAddr, u16 host_rx_bd_num);
void SPDIO_Notify_INT(SDIO_TypeDef *SDIO, u16 IntStatus);
void SPDIO_Recycle_Rx_BD(SDIO_TypeDef *SDIO, PSPDIO_ADAPTER pSPDIODev, spdio_device_tx_done_cb_ptr spdio_device_tx_done_cb);
u8 SPDIO_DeviceTx(SDIO_TypeDef *SDIO, PSPDIO_ADAPTER pSPDIODev, struct spdio_buf_t *pbuf);
void SPDIO_TxBd_DataReady_DeviceRx(SDIO_TypeDef *SDIO, PSPDIO_ADAPTER pSPDIODev, spdio_device_rx_done_cb_ptr spdio_device_rx_done_cb);
void SPDIO_Device_Init(SDIO_TypeDef *SDIO, PSPDIO_ADAPTER pSPDIODev);
void SPDIO_Device_DeInit(SDIO_TypeDef *SDIO);

#ifdef __cplusplus
}
#endif
#endif
