/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */

#include "ameba_soc.h"
#include "hci_core.h"

/* Private defines -----------------------------------------------------------*/

#define SPDIO_HOST_RX_BD_NUM	N_BYTE_ALIGMENT(4, 2)	// 2 bd for one transaction, must be rounded to 2
#define SPDIO_HOST_TX_BD_NUM	4
#define SPDIO_DEVICE_RX_BUFSZ	N_BYTE_ALIGMENT((HCI_RX_BUF_SIZE + 24), SPDIO_TX_BD_BUF_SZ_UNIT) // desired packet length + 24(spdio header info), must be rounded to 64

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/


/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "SPDIO";

/* Private functions ---------------------------------------------------------*/


/* Exported functions --------------------------------------------------------*/

SPDIO_TX_BD SPDIO_TXBDAddr[SPDIO_HOST_TX_BD_NUM];
SPDIO_TX_BD_HANDLE g_TXBDHdl[SPDIO_HOST_TX_BD_NUM];
struct spdio_buf_t spdio_dev_rx_buf[SPDIO_HOST_TX_BD_NUM];
u32 spdio_dev_rx_buf_entry[SPDIO_DEVICE_RX_BUFSZ / sizeof(u32)];

SPDIO_RX_BD SPDIO_RXBDAddr[SPDIO_HOST_RX_BD_NUM];
SPDIO_RX_BD_HANDLE g_RXBDHdl[SPDIO_HOST_RX_BD_NUM];
INIC_RX_DESC g_RXDESCAddr[SPDIO_HOST_RX_BD_NUM];
struct spdio_buf_t spdio_dev_tx_buf[SPDIO_HOST_RX_BD_NUM];
// u32 spdio_dev_tx_buf_entry[SPDIO_HOST_RX_BD_NUM][SPDIO_DEVICE_TX_BUFSZ / sizeof(u32)];


SPDIO_ADAPTER gSPDIODev;

static s8 spdio_device_rx_done_cb(PSPDIO_ADAPTER pSPDIODev, struct spdio_buf_t *buf, u8 *pdata, u16 pktsize, u8 type)
{
	UNUSED(buf);
	UNUSED(type);

	HCI_AdapterTypeDef *adapter = (HCI_AdapterTypeDef *)pSPDIODev->spdio_priv;
	_memcpy(adapter->RxBuffer, pdata, pktsize);
	assert_param(pktsize <= HCI_RX_BUF_SIZE);
	HCI_RxProcess(adapter, adapter->RxBuffer, pktsize);

	return RTK_SUCCESS;
}

static s8 spdio_device_tx_done_cb(PSPDIO_ADAPTER pSPDIODev, struct spdio_buf_t *buf)
{
	UNUSED(pSPDIODev);
	UNUSED(buf);

	return RTK_SUCCESS;
}

/**
 * @brief SPDIO device interrupt service routine, including
 * 		- Read and clean interrupt status.
 * 		- Wake up the SDIO task to handle the IRQ event.
 * @param pData Pointer to SDIO device data structure.
 * @retval 0
 */
u32 SPDIO_IRQ_Handler(void *pData)
{
	PSPDIO_ADAPTER pSPDIODev = pData;
	u16 IntStatus = SDIO_INTStatus(SDIO_WIFI);
	SDIO_INTClear(SDIO_WIFI, IntStatus); // clean the ISR

	if (IntStatus & SDIO_NOTIFY_TYPE_INT) {
		SPDIO_Notify_INT(SDIO_WIFI, IntStatus);
	}

	if (IntStatus & SDIO_WIFI_BIT_C2H_DMA_OK) {
		SPDIO_Recycle_Rx_BD(SDIO_WIFI, pSPDIODev, spdio_device_tx_done_cb);
	}

	if (IntStatus & SDIO_WIFI_BIT_TXBD_H2C_OVF) {
		pSPDIODev->TxOverFlow = 1;
	}

	if (IntStatus & SDIO_WIFI_BIT_H2C_DMA_OK || pSPDIODev->WaitForDeviceRxbuf) {
		SPDIO_TxBd_DataReady_DeviceRx(SDIO_WIFI, pSPDIODev, spdio_device_rx_done_cb);
	}

	return 0;
}

void SPDIO_HAL_Init(void *adapter)
{
	u32 i;
	PSPDIO_ADAPTER pSPDIODev = &gSPDIODev;
	pSPDIODev->spdio_priv = adapter;

	for (i = 0; i < SPDIO_HOST_TX_BD_NUM; i++) {
		spdio_dev_rx_buf[i].buf_allocated = (u32)spdio_dev_rx_buf_entry;
		spdio_dev_rx_buf[i].size_allocated = SPDIO_DEVICE_RX_BUFSZ;
		// this buffer must be 4 byte alignment
		spdio_dev_rx_buf[i].buf_addr = (u32)spdio_dev_rx_buf_entry;
	}

	pSPDIODev->host_rx_bd_num = SPDIO_HOST_RX_BD_NUM;
	pSPDIODev->host_tx_bd_num = SPDIO_HOST_TX_BD_NUM;
	pSPDIODev->device_rx_bufsz = SPDIO_DEVICE_RX_BUFSZ;

	pSPDIODev->pTXBDHdl = g_TXBDHdl;
	pSPDIODev->pTXBDAddr = SPDIO_TXBDAddr;
	SDIO_TxBdHdl_Init(g_TXBDHdl, SPDIO_TXBDAddr, spdio_dev_rx_buf, SPDIO_HOST_TX_BD_NUM);
	RTK_LOGI(TAG, "TXBDWPtr=0x%x TXBDRPtr=0x%x\n", pSPDIODev->TXBDWPtr, pSPDIODev->TXBDRPtr);

	pSPDIODev->pRXBDHdl = g_RXBDHdl;
	pSPDIODev->pRXBDAddr = SPDIO_RXBDAddr;
	pSPDIODev->pRXDESCAddr = g_RXDESCAddr;
	SDIO_RxBdHdl_Init(g_RXBDHdl, SPDIO_RXBDAddr, g_RXDESCAddr, pSPDIODev->host_rx_bd_num);

	SPDIO_Device_Init(SDIO_WIFI, pSPDIODev);

	SDIO_CPWM2_Set(SDIO_WIFI, SPDIO_CCPWM2_SEND_CHECKSUM, DISABLE);
	SDIO_CPWM2_Set(SDIO_WIFI, CPWM2_IMG1_BIT, ENABLE);

	/* enable the interrupt */
	InterruptRegister((IRQ_FUN)SPDIO_IRQ_Handler, SDIO_WIFI_IRQ, (u32)pSPDIODev, INT_PRI_MIDDLE);
	InterruptEn(SDIO_WIFI_IRQ, INT_PRI_MIDDLE);
}

void SPDIO_HAL_DeInit(void)
{
	PSPDIO_ADAPTER pSPDIODev = &gSPDIODev;
	pSPDIODev->spdio_priv = NULL;

	SDIO_CPWM2_Set(SDIO_WIFI, CPWM2_IMG1_BIT, DISABLE);
	SPDIO_Device_DeInit(SDIO_WIFI);

	InterruptDis(SDIO_WIFI_IRQ);
	InterruptUnRegister(SDIO_WIFI_IRQ);
}

char SPDIO_HAL_Transmit(u8 *pdata, u16 size)
{
	int ret;
	PSPDIO_ADAPTER pSPDIODev = &gSPDIODev;
	struct spdio_buf_t *tx_buf = &spdio_dev_tx_buf[pSPDIODev->RXBDWPtr];

	tx_buf->buf_allocated = (u32)pdata;
	tx_buf->size_allocated = size;
	tx_buf->buf_addr = (u32)pdata;
	tx_buf->buf_size = size;
	tx_buf->type = SPDIO_RX_DATA_USER; // you can define your own data type in spdio_rx_data_t and spdio_tx_data_t

	ret = SPDIO_DeviceTx(SDIO_WIFI, pSPDIODev, tx_buf);
	if (ret == FALSE) {
		/* free buffer, otherwise free in tx callback */
	}

	return RTK_SUCCESS;
}
