/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

static const char *const TAG = "SPDIO";

#define SDIO_INCR_RING_IDX(idx, range)   \
    do {                          \
        (idx)++;                  \
        if ((idx) >= (range)) {   \
            (idx) -= (range);     \
        }                         \
    } while(0)

void SDIO_TxBdHdl_Init(SPDIO_TX_BD_HANDLE *g_TXBDHdl, SPDIO_TX_BD *SPDIO_TXBDAddr, struct spdio_buf_t *spdio_dev_rx_buf, u16 host_tx_bd_num)
{
	SPDIO_TX_BD_HANDLE *pTxBdHdl;
	u32 i;

	for (i = 0; i < host_tx_bd_num; i++) {
		pTxBdHdl = &g_TXBDHdl[i];
		pTxBdHdl->pTXBD = &SPDIO_TXBDAddr[i];
		// Pre-allocate buffer by User
		pTxBdHdl->dev_rx_buf = &spdio_dev_rx_buf[i];
		pTxBdHdl->pTXBD->Address = spdio_dev_rx_buf[i].buf_addr;
		assert_param(spdio_dev_rx_buf[i].buf_addr != 0);
		assert_param(spdio_dev_rx_buf[i].buf_addr % SPDIO_DMA_ALIGN_4 == 0);
		DCache_CleanInvalidate(spdio_dev_rx_buf[i].buf_allocated, spdio_dev_rx_buf[i].size_allocated);

		pTxBdHdl->isFree = TRUE;
		RTK_LOGS(TAG, RTK_LOG_INFO, "TX_BD%d @ 0x%x 0x%x\n", i, pTxBdHdl, pTxBdHdl->pTXBD);
	}
	DCache_Clean((u32)SPDIO_TXBDAddr, host_tx_bd_num * sizeof(SPDIO_TX_BD));
}

void SDIO_RxBdHdl_Init(SPDIO_RX_BD_HANDLE *g_RXBDHdl, SPDIO_RX_BD *SPDIO_RXBDAddr, INIC_RX_DESC *g_RXDESCAddr, u16 host_rx_bd_num)
{
	SPDIO_RX_BD_HANDLE *pRxBdHdl;
	u32 i;

	_memset(g_RXDESCAddr, 0, sizeof(INIC_RX_DESC) * host_rx_bd_num);

	for (i = 0; i < host_rx_bd_num; i++) {
		pRxBdHdl = &g_RXBDHdl[i];
		pRxBdHdl->pRXBD = &SPDIO_RXBDAddr[i];
		pRxBdHdl->pRXDESC = &g_RXDESCAddr[i];
		pRxBdHdl->isFree = TRUE;
		pRxBdHdl->isPktEnd = FALSE;
	}
	DCache_Clean((u32)SPDIO_RXBDAddr, host_rx_bd_num * sizeof(SPDIO_RX_BD));
}

void SPDIO_Notify_INT(SDIO_TypeDef *SDIO, u16 IntStatus)
{
	assert_param(IS_SDIO_DEVICE(SDIO));

	if (SDIO_WIFI == SDIO) {
		if (IntStatus & SDIO_WIFI_BIT_RPWM1_INT) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "RPWM INT triggered, value is 0x%x\n", SDIO_RPWM1_Get(SDIO));
		}

		if (IntStatus & SDIO_WIFI_BIT_RPWM2_INT) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "RPWM2 INT triggered, value is 0x%x\n", SDIO_RPWM2_Get(SDIO));
		}

		if (IntStatus & SDIO_WIFI_BIT_H2C_MSG_INT) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "H2C MSG INT triggered, MSG and EXT MSG value: 0x%x, 0x%x\n", SDIO_H2C_MSG_Get(SDIO),  SDIO_H2C_MSG_EXT_Get(SDIO));
		}

		if (IntStatus & SDIO_WIFI_BIT_HOST_WAKE_CPU_INT) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "SDIO_WIFI_BIT_HOST_WAKE_CPU_INT triggered\n");
		}

		if (IntStatus & SDIO_WIFI_BIT_H2C_SUS_REQ) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "SDIO_WIFI_BIT_H2C_SUS_REQ triggered\n");
		}
	} else {
		if (IntStatus & SDIO_BT_BIT_SDIO_CS_RDY) {
			// RTK_LOGS(TAG, RTK_LOG_INFO, "######### [IRQ handler] BIT_SDIO_CS_RDY #########\n");
			/* g_sdio_status = SDIO_STATUS_CARD_RECG; */
		}

		if (IntStatus & SDIO_BT_BIT_SDIO_CS_RST) {
			// RTK_LOGS(TAG, RTK_LOG_INFO, "######### [IRQ handler] BIT_SDIO_CS_RST #########\n");
		}

		if (IntStatus & SDIO_BT_BIT_HOST_WAKE_CPU_INT) {
			// RTK_LOGS(TAG, RTK_LOG_INFO, "######### [IRQ handler] BIT_HOST_WAKE_CPU_INT #########\n");
		}

		if (IntStatus & SDIO_BT_BIT_H2C_SUS_REQ) {
			// RTK_LOGS(TAG, RTK_LOG_INFO, "######### [IRQ handler] BIT_H2C_SUS_REQ #########\n");
		}

		if (IntStatus & SDIO_BT_BIT_CCCR_IOE2_SET) {
			// RTK_LOGS(TAG, RTK_LOG_INFO, "######### [IRQ handler] BIT_CCCR_IOE2_SET #########\n");
		}

		if (IntStatus & SDIO_BT_BIT_CCCR_IOE2_CLR) {
			// RTK_LOGS(TAG, RTK_LOG_INFO, "######### [IRQ handler] BIT_CCCR_IOE2_CLR #########\n");
		}
	}
}

void SPDIO_Recycle_Rx_BD(SDIO_TypeDef *SDIO, PSPDIO_ADAPTER pSPDIODev, spdio_device_tx_done_cb_ptr spdio_device_tx_done_cb)
{
	SPDIO_RX_BD_HANDLE *pRxBdHdl;
	SPDIO_RX_BD *pRXBD;
	u8 isPktEnd = FALSE;
	assert_param(IS_SDIO_DEVICE(SDIO));

	while (SDIO_RXBD_RPTR_Get(SDIO) != pSPDIODev->RXBDRPtr) {
		pRxBdHdl = pSPDIODev->pRXBDHdl + pSPDIODev->RXBDRPtr;
		pRXBD = pRxBdHdl->pRXBD;
		isPktEnd = FALSE;

		if (!pRxBdHdl->isFree) {
			if (pRxBdHdl->isPktEnd) {
				isPktEnd = TRUE;
			}
			pRxBdHdl->isPktEnd = FALSE;
			pRxBdHdl->isFree = TRUE;
			_memset((void *)(pRxBdHdl->pRXDESC), 0, sizeof(INIC_RX_DESC));
			_memset((void *)pRXBD, 0, sizeof(SPDIO_RX_BD));
		} else {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Recycle a Free RX_BD, RXBDRPtr=%d\n", pSPDIODev->RXBDRPtr);
			assert_param(FALSE);
		}

		SDIO_INCR_RING_IDX(pSPDIODev->RXBDRPtr, pSPDIODev->host_rx_bd_num);

		if (isPktEnd) {
			spdio_device_tx_done_cb(pSPDIODev, pRxBdHdl->dev_tx_buf);
		}
	}
}

/**
 * @brief Spdio write function.
 * @param obj Pointer to a initialized spdio_t structure.
 * @param pbuf Pointer to a spdio_buf_t structure which carries the payload.
 * @retval RTK_SUCCESS or RTK_FAIL.
 */
u8 SPDIO_DeviceTx(SDIO_TypeDef *SDIO, PSPDIO_ADAPTER pSPDIODev, struct spdio_buf_t *pbuf)
{
	SPDIO_RX_BD_HANDLE *pRxBdHdl;
	SPDIO_RX_BD *pRXBD;
	INIC_RX_DESC *pRxDesc;
	u16 RxBdRdPtr = pSPDIODev->RXBDRPtr; // RX_BD read pointer
	u16 pkt_size = pbuf->buf_size;
	u16 pkt_offset = 0;
	u8 needed_rxbd_num;

	assert_param(IS_SDIO_DEVICE(SDIO));

	/* check if RX_BD available */
	if (SDIO_WIFI == SDIO) {
#if defined(SDIO_RX_PKT_SIZE_OVER_16K) && SDIO_RX_PKT_SIZE_OVER_16K
		needed_rxbd_num = ((pkt_size - 1) / SPDIO_MAX_RX_BD_BUF_SIZE) + SPDIO_MIN_RX_BD_SEND_PKT;
#else
		needed_rxbd_num = SPDIO_MIN_RX_BD_SEND_PKT;
#endif
	} else {
		needed_rxbd_num = 0x1;
	}

	if (RxBdRdPtr == pSPDIODev->RXBDWPtr) {
		/* Idle */
	} else if (pSPDIODev->RXBDWPtr > RxBdRdPtr) {
		if ((pSPDIODev->RXBDWPtr - RxBdRdPtr) >= (pSPDIODev->host_rx_bd_num - needed_rxbd_num)) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "No Available RX_BD, ReadPtr=%d WritePtr=%d\n", RxBdRdPtr, pSPDIODev->RXBDWPtr);
			return FALSE;
		}
	} else {
		if ((RxBdRdPtr - pSPDIODev->RXBDWPtr) <= needed_rxbd_num) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "No Available RX_BD, ReadPtr=%d WritePtr=%d\n", RxBdRdPtr, pSPDIODev->RXBDWPtr);
			return FALSE;
		}
	}

	if (SDIO_WIFI == SDIO) {
		/* a SDIO RX packet will use at least 2 RX_BD, the 1st one is for RX_Desc, other RX_BDs are for packet payload */
		pRxBdHdl = pSPDIODev->pRXBDHdl + pSPDIODev->RXBDWPtr;
		pRXBD = pRxBdHdl->pRXBD;
		if (!pRxBdHdl->isFree) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Allocated a non-free RX_BD\n");
			assert_param(FALSE);
		}

		pRxDesc = pRxBdHdl->pRXDESC;
		pRxDesc->type = pbuf->type;
		pRxDesc->pkt_len = pkt_size;
		pRxDesc->offset = sizeof(INIC_RX_DESC);
		DCache_CleanInvalidate((u32)pRxDesc, sizeof(INIC_RX_DESC));

		pRxBdHdl->isFree = FALSE;
		pRXBD->FS = 1;
		pRXBD->LS = 0;
		pRXBD->PhyAddr = (u32)pRxDesc;
		pRXBD->BuffSize = sizeof(INIC_RX_DESC);
		DCache_CleanInvalidate((u32)pRXBD, sizeof(SPDIO_RX_BD));
		pRxBdHdl->isPktEnd = 0;
		SDIO_INCR_RING_IDX(pSPDIODev->RXBDWPtr, pSPDIODev->host_rx_bd_num);
	}

	assert_param(pbuf->buf_addr % SPDIO_DMA_ALIGN_4 == 0);
	DCache_CleanInvalidate((u32)pbuf->buf_addr, pbuf->buf_size);
	/* Take RX_BD to transmit packet payload */
	do {
		pRxBdHdl = pSPDIODev->pRXBDHdl + pSPDIODev->RXBDWPtr;
		pRXBD = pRxBdHdl->pRXBD;
		pRxBdHdl->isFree = FALSE;

		if (SDIO_WIFI == SDIO) {
			pRXBD->FS = 0;
		} else {
			pRXBD->FS = 1;
		}

		pRXBD->PhyAddr = (u32)((u8 *)pbuf->buf_addr + pkt_offset);
#if defined(SDIO_RX_PKT_SIZE_OVER_16K) && SDIO_RX_PKT_SIZE_OVER_16K
		if ((pkt_size - pkt_offset) <= SPDIO_MAX_RX_BD_BUF_SIZE) {
			pRXBD->BuffSize = pkt_size - pkt_offset;
			pRxBdHdl->isPktEnd = 1;
		} else {
			pRXBD->BuffSize = SPDIO_MAX_RX_BD_BUF_SIZE;
			pRxBdHdl->isPktEnd = 0;
		}
#else
		if (pkt_size > SPDIO_MAX_RX_BD_BUF_SIZE) {
			// if come to here, please enable "SDIO_RX_PKT_SIZE_OVER_16K"
			RTK_LOGS(TAG, RTK_LOG_ERROR, "The Packet Size bigger than 16K\n");
			pkt_size = SPDIO_MAX_RX_BD_BUF_SIZE;
		}
		pRXBD->BuffSize = pkt_size;
		pRxBdHdl->isPktEnd = 1;
#endif
		pkt_offset += pRXBD->BuffSize;
		if (pkt_offset >= pkt_size) {
			pRxBdHdl->dev_tx_buf = pbuf;
			pRXBD->LS = 1;
		}
		DCache_CleanInvalidate((u32)pRXBD, sizeof(SPDIO_RX_BD));
		SDIO_INCR_RING_IDX(pSPDIODev->RXBDWPtr, pSPDIODev->host_rx_bd_num);
	} while (pkt_offset < pkt_size);

	SDIO_RXBD_WPTR_Set(SDIO, pSPDIODev->RXBDWPtr);
	SDIO_RxReq(SDIO);

	return TRUE;
}

/**
 * @brief Handle the SDIO FIFO data ready interrupt, including
 * 		- Send those data to target driver via callback func, like WLan.
 * 		- Allocate a buffer for the TX BD
 * @param pSPDIODev Pointer to a SDIO device data structure.
 * @return None
 */
void SPDIO_TxBd_DataReady_DeviceRx(SDIO_TypeDef *SDIO, PSPDIO_ADAPTER pSPDIODev, spdio_device_rx_done_cb_ptr spdio_device_rx_done_cb)
{
	SPDIO_TX_BD_HANDLE *pTxBdHdl;
	SPDIO_TX_BD *pTXBD = NULL;
	PINIC_TX_DESC pTxDesc;
	volatile u16 TxBDWPtr = SDIO_TXBD_WPTR_Get(SDIO);
	s8 ret = RTK_FAIL;
	u32 RxDataAddr;
	u8 *read_buf = NULL;
	u32 pkt_len = 0;

	assert_param(IS_SDIO_DEVICE(SDIO));

	if (unlikely(pSPDIODev->TxOverFlow != 0)) {
		pSPDIODev->TxOverFlow = 0;
		RTK_LOGS(TAG, RTK_LOG_ERROR, "SDIO TXBD Overflow Case: DMA_CTRL_REG=0x%x\n", SDIO_DMA_CTRL_Get(SDIO));
		assert_param(FALSE);
	}

	while (pSPDIODev->TXBDRPtr != TxBDWPtr) {
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "TxBDWPtr=%d TxBDRPtr=%d\n", TxBDWPtr, pSPDIODev->TXBDRPtr);
		pTxBdHdl = pSPDIODev->pTXBDHdl + pSPDIODev->TXBDRPtr;
		pTXBD = pTxBdHdl->pTXBD;

		if (SDIO_WIFI == SDIO) {
			pTxDesc = (PINIC_TX_DESC)(pTXBD->Address);
			DCache_Invalidate((u32)pTxDesc, sizeof(INIC_TX_DESC));

			RTK_LOGS(TAG, RTK_LOG_DEBUG, "SPDIO_DeviceRx: PktSz=%d Offset=%d\n", pTxDesc->txpktsize, pTxDesc->offset);

			if ((pTxDesc->txpktsize + pTxDesc->offset) <= pSPDIODev->device_rx_bufsz) {
				// use the callback function to fordward this packet to target(WLan) driver
				RxDataAddr = pTXBD->Address + pTxDesc->offset;
				DCache_Invalidate(RxDataAddr, pTxDesc->txpktsize);
				ret = spdio_device_rx_done_cb(pSPDIODev, pTxBdHdl->dev_rx_buf, (u8 *)(RxDataAddr), pTxDesc->txpktsize, pTxDesc->type);
				if (ret == RTK_FAIL) {
					pSPDIODev->WaitForDeviceRxbuf = TRUE;
					RTK_LOGS(TAG, RTK_LOG_WARN, "spdio_device_rx_done_cb return fail!\n");
				} else {
					pSPDIODev->WaitForDeviceRxbuf = FALSE;
					// No need to change the device rx buffer address.
					DCache_CleanInvalidate(pTxBdHdl->dev_rx_buf->buf_addr, pTxBdHdl->dev_rx_buf->buf_size);
					pTXBD->Address = pTxBdHdl->dev_rx_buf->buf_addr;
					DCache_Clean((u32)pTXBD, sizeof(SPDIO_TX_BD));
				}
			} else {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid TxDesc packet, Just drop it\n");
				ret = RTK_SUCCESS; // pretend we call the TX callback OK
			}
		} else {
			read_buf = (u8 *)(pTXBD->Address);
			DCache_Invalidate((u32)read_buf, pSPDIODev->device_rx_bufsz);
			pkt_len = *((u16 *)read_buf);

			if (pkt_len > pSPDIODev->device_rx_bufsz) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "SDIO TX packet size is over tx buf size\n");
			}

			ret = spdio_device_rx_done_cb(pSPDIODev, NULL, read_buf, pkt_len, NULL);
			if (ret == RTK_FAIL) {
				pSPDIODev->WaitForDeviceRxbuf = TRUE;
				RTK_LOGS(TAG, RTK_LOG_WARN, "spdio_device_rx_done_cb return fail!\n");
			} else {
				pSPDIODev->WaitForDeviceRxbuf = FALSE;
				// No need to change the device rx buffer address.
				DCache_CleanInvalidate(pTxBdHdl->dev_rx_buf->buf_addr, pTxBdHdl->dev_rx_buf->buf_size);
				pTXBD->Address = pTxBdHdl->dev_rx_buf->buf_addr;
				DCache_Clean((u32)pTXBD, sizeof(SPDIO_TX_BD));
			}
		}

		if (RTK_SUCCESS != ret) {
			// may be is caused by TX queue is full, so we skip it and try again later
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "SDIO_TX Force Break: TXBDWP=0x%x TXBDRP=0x%x\n", TxBDWPtr, pSPDIODev->TXBDRPtr);
			break; // break the while loop
		} else {
			SDIO_INCR_RING_IDX(pSPDIODev->TXBDRPtr, pSPDIODev->host_tx_bd_num);
			SDIO_TXBD_RPTR_Set(SDIO, pSPDIODev->TXBDRPtr);
		}

		TxBDWPtr = SDIO_TXBD_WPTR_Get(SDIO);
	}
}

void SPDIO_Device_Init(SDIO_TypeDef *SDIO, PSPDIO_ADAPTER pSPDIODev)
{
	SDIO_InitTypeDef SDIO_InitStruct;
	assert_param(pSPDIODev->device_rx_bufsz % SPDIO_TX_BD_BUF_SZ_UNIT == 0);
	assert_param(pSPDIODev->device_rx_bufsz / SPDIO_TX_BD_BUF_SZ_UNIT <= 0xFF);
	assert_param(IS_SDIO_DEVICE(SDIO));

	pSPDIODev->TXBDWPtr = SDIO_TXBD_WPTR_Get(SDIO);
	pSPDIODev->TXBDRPtr = pSPDIODev->TXBDWPtr;

	pSPDIODev->RXBDWPtr = SDIO_RXBD_RPTR_Get(SDIO);
	pSPDIODev->RXBDRPtr = pSPDIODev->RXBDWPtr;

	SDIO_StructInit(&SDIO_InitStruct);
	SDIO_InitStruct.TXBD_BAR = (u32)pSPDIODev->pTXBDAddr;
	SDIO_InitStruct.TXBD_RING_SIZE = pSPDIODev->host_tx_bd_num & 0xFFFF;
	SDIO_InitStruct.TX_BUFFER_SIZE = DIV_ROUND_UP(pSPDIODev->device_rx_bufsz, SPDIO_TX_BD_BUF_SZ_UNIT) & 0xFF; // Round up to nearest 64 bytes
	SDIO_InitStruct.RXBD_BAR = (u32)pSPDIODev->pRXBDAddr;
	SDIO_InitStruct.RXBD_RING_SIZE = pSPDIODev->host_rx_bd_num & 0xFFFF;
	SDIO_InitStruct.RXBD_FREE_TH = SPDIO_RX_BD_FREE_TH;
	SDIO_InitStruct.TX_AGG_DISPATCH = 1;
	SDIO_Init(SDIO, &SDIO_InitStruct);

	SDIO_INTClearAll(SDIO);

	if (SDIO_WIFI == SDIO) {
		SDIO_INTConfig(SDIO, SDIO_INIT_INT_MASK, ENABLE);
	} else if (SDIO_BT == SDIO) {
		SDIO_INTConfig(SDIO, 0xFFFF, ENABLE);
	}

	/* Indicate the Host system that the TX/RX is ready */
	SDIO_SetReady(SDIO, ENABLE);

	RTK_LOGS(TAG, RTK_LOG_INFO, "<==SDIO_Device_Init\n");
}

/**
 * @brief Free SDIO device, including freeing TX FIFO buffer.
 * @return None
 * @note This function should be called in a task.
 */
void SPDIO_Device_DeInit(SDIO_TypeDef *SDIO)
{
	assert_param(IS_SDIO_DEVICE(SDIO));

	SDIO_INTConfig(SDIO, 0xffff, DISABLE);
	SDIO_INTClearAll(SDIO);

	// Reset SDIO DMA
	SDIO_DMA_Reset(SDIO);
}
