#include "ameba_soc.h"
#include "spdio_api.h"
#include "ameba_inic.h"
static const char *TAG = "SPDIO";
/** @addtogroup Ameba_Mbed_API
  * @{
  */

/** @addtogroup MBED_SPDIO
 *  @brief      MBED_SPDIO driver modules.
 *  @{
 */

/** @defgroup MBED_SPDIO_Exported_Constants MBED_SPDIO Exported Constants
  * @{
  */

#define SPDIO_IRQ_PRIORITY			INT_PRI_MIDDLE
#define SPDIO_TX_BUF_SZ_UNIT		64
#define RX_BD_FREE_TH				5
#define MIN_RX_BD_SEND_PKT			2
#define MAX_RX_BD_BUF_SIZE			16380	// the Maximum size for a RX_BD point to, make it 4-bytes aligned

/** @}*/

/** @defgroup MBED_SPDIO_Exported_Types MBED_SPDIO Exported Types
  * @{
  */

/** @defgroup MBED_SPDIO_Structure_Type Structure Type
  * @{
  */

typedef struct {
	u32	Address;		/* The TX buffer physical address, it must be 4-bytes aligned */
} SPDIO_TX_BD;

/* The RX Buffer Descriptor format */
typedef struct {
	u32 BuffSize: 14;		/* bit[13:0], RX Buffer Size, Maximum 16384-1 */
	u32 LS: 1;				/* bit[14], is the Last Segment ? */
	u32 FS: 1;				/* bit[15], is the First Segment ? */
	u32 Seq: 16;			/* bit[31:16], The sequence number, it's no use for now */
	u32 PhyAddr;			/* The RX buffer physical address, it must be 4-bytes aligned */
} SPDIO_RX_BD;

/* the data structer to bind a TX_BD with a TX Packet */
typedef struct {
	SPDIO_TX_BD *pTXBD;		/* Point to the TX_BD buffer */
	void *priv;
	u8 isPktEnd;			/* For a packet over 1 BD , this flag to indicate is this BD contains a packet end */
	u8 isFree;				/* is this TX BD free */
} SPDIO_TX_BD_HANDLE;

/* the data structer to bind a RX_BD with a RX Packet */
typedef struct {
	void *priv;
	SPDIO_RX_BD *pRXBD;		/* Point to the RX_BD buffer */
	INIC_RX_DESC *pRXDESC;	/* point to the Rx Packet */
	u8 isPktEnd;			/*  For a packet over 1 BD , this flag to indicate is this BD contains a packet end */
	u8 isFree;				/* is this RX BD free (DMA done and its RX packet has been freed) */
} SPDIO_RX_BD_HANDLE;

typedef struct {
	void *spdio_priv;				/*Data from User*/
	u8 *pTXBDAddr;					/* The TX_BD start address */
	SPDIO_TX_BD *pTXBDAddrAligned;	/* The TX_BD start address, it must be 4-bytes aligned */

	SPDIO_TX_BD_HANDLE *pTXBDHdl;	/* point to the allocated memory for TX_BD Handle array */
	u16 TXBDWPtr;					/* The SDIO TX(Host->Device) BD local write index, different with HW maintained write Index. */
	u16 TXBDRPtr;					/* The SDIO TX(Host->Device) BD read index */
	u16 TXBDRPtrReg;				/* The SDIO TX(Host->Device) BD read index has been write to HW register */
	u8 TxOverFlow;

	u8 *pRXBDAddr;					/* The RX_BD start address */
	SPDIO_RX_BD *pRXBDAddrAligned;	/* The RX_BD start address, it must be 8-bytes aligned */

	u8 *pRXDESCAddr;
	INIC_RX_DESC *pRXDESCAddrAligned;
	SPDIO_RX_BD_HANDLE *pRXBDHdl;	/* point to the allocated memory for RX_BD Handle array */
	u16 RXBDWPtr;					/* The SDIO RX(Device->Host) BD write index */
	u16 RXBDRPtr;					/* The SDIO RX(Device->Host) BD local read index, different with HW maintained Read Index. */

	rtos_sema_t IrqSema;					/* Semaphore for SDIO RX, use to wakeup the SDIO RX task */
	rtos_task_t xSDIOIrqTaskHandle;	/* The handle of the SDIO Task speical for RX, can be used to delte the task */

	u8 WaitForTxbuf;
} HAL_SPDIO_ADAPTER, *PHAL_SPDIO_ADAPTER;

/**
  * @}
  */

/**
  * @}
  */

/** @defgroup MBED_SPDIO_Exported_Constants MBED_SPDIO Exported Constants
  * @{
  */

struct spdio_t *g_spdio_priv = NULL;
HAL_SPDIO_ADAPTER gSPDIODev;
PHAL_SPDIO_ADAPTER pgSPDIODev = NULL;

/**
  * @}
  */

/** @defgroup MBED_SPDIO_Exported_Functions MBED_SPDIO Exported Functions
  * @{
  */

s8 spdio_rx_done_cb(void *padapter, void *data, u16 offset, u16 pktsize, u8 type)
{
	struct spdio_buf_t *buf = (struct spdio_buf_t *)data;
	struct spdio_t *obj = (struct spdio_t *)padapter;

	DCache_Invalidate((u32) buf->buf_addr, pktsize);

	if (obj) {
		return obj->rx_done_cb(obj, buf, (u8 *)(buf->buf_addr + offset), pktsize, type);
	} else {
		RTK_LOGS_LVL(TAG, RTK_LOG_ERROR, "spdio rx done callback function is null!");
	}
	return SUCCESS;
}

s8 spdio_tx_done_cb(void *padapter, IN u8 *data)
{
	struct spdio_t *obj = (struct spdio_t *)padapter;
	struct spdio_buf_t *buf = (struct spdio_buf_t *)data;
	if (obj) {
		return obj->tx_done_cb(obj, buf);
	} else {
		RTK_LOGS(TAG, "spdio tx done callback function is null!");
	}
	return SUCCESS;
}

s8 spdio_tx(struct spdio_t *obj, struct spdio_buf_t *pbuf)
{
	DCache_CleanInvalidate((u32)pbuf->buf_allocated, pbuf->size_allocated);
	PHAL_SPDIO_ADAPTER pgSDIODev = obj->priv;
	INIC_RX_DESC *pRxDesc;
	SPDIO_RX_BD_HANDLE *pRxBdHdl;
	SPDIO_RX_BD *pRXBD;
	u32 Offset = 0;
	u16 RxBdWrite = 0;	// to count how much RX_BD used in a Transaction
	u16 RxBdRdPtr = pgSDIODev->RXBDRPtr;	// RX_BD read pointer
	u32 pkt_size;
#if defined(SDIO_RX_PKT_SIZE_OVER_16K) && SDIO_RX_PKT_SIZE_OVER_16K
	u8 needed_rxbd_num;
#endif
	/* check if RX_BD available */
#if defined(SDIO_RX_PKT_SIZE_OVER_16K) && SDIO_RX_PKT_SIZE_OVER_16K
	needed_rxbd_num = ((pbuf->buf_size - 1) / MAX_RX_BD_BUF_SIZE) + MIN_RX_BD_SEND_PKT;
#endif
	if (RxBdRdPtr != pgSDIODev->RXBDWPtr) {
		if (pgSDIODev->RXBDWPtr > RxBdRdPtr) {
#if defined(SDIO_RX_PKT_SIZE_OVER_16K) && SDIO_RX_PKT_SIZE_OVER_16K
			if ((pgSDIODev->RXBDWPtr - RxBdRdPtr) >= (u16)(obj->tx_bd_num - needed_rxbd_num))
#else
			if ((pgSDIODev->RXBDWPtr - RxBdRdPtr) >= (u16)(obj->tx_bd_num - (u32)MIN_RX_BD_SEND_PKT))
#endif
			{
				RTK_LOGW(TAG, "SDIO_Return_Rx_Data: No Available RX_BD, ReadPtr=%d WritePtr=%d\n", \
						 RxBdRdPtr, pgSDIODev->RXBDWPtr);
				return _FALSE;
			}
		} else {
#if defined(SDIO_RX_PKT_SIZE_OVER_16K) && SDIO_RX_PKT_SIZE_OVER_16K
			if ((RxBdRdPtr - pgSDIODev->RXBDWPtr) <= (u16)needed_rxbd_num)
#else
			if ((RxBdRdPtr - pgSDIODev->RXBDWPtr) <= (u16)MIN_RX_BD_SEND_PKT)
#endif
			{
				RTK_LOGW(TAG, "SDIO_Return_Rx_Data: No Available RX_BD, ReadPtr=%d WritePtr=%d\n", RxBdRdPtr, pgSDIODev->RXBDWPtr);
				return _FALSE;
			}
		}
	}

	// TODO: Add RX_DESC before the packet

	/* a SDIO RX packet will use at least 2 RX_BD, the 1st one is for RX_Desc,
	   other RX_BDs are for packet payload */
	/* Use a RX_BD to transmit RX_Desc */
	pRXBD = pgSDIODev->pRXBDAddrAligned + pgSDIODev->RXBDWPtr;	// get the RX_BD head
	pRxBdHdl = pgSDIODev->pRXBDHdl + pgSDIODev->RXBDWPtr;

	pRxDesc = pRxBdHdl->pRXDESC;
	pRxDesc->type = pbuf->type;
	pRxDesc->pkt_len = pbuf->buf_size;
	pRxDesc->offset = sizeof(INIC_RX_DESC);
	DCache_CleanInvalidate((u32)pRxDesc, sizeof(INIC_RX_DESC));

	if (!pRxBdHdl->isFree) {
		RTK_LOGE(TAG, "SDIO_Return_Rx_Data: Allocated a non-free RX_BD\n");
	}
	pRxBdHdl->isFree = 0;
	pRXBD->FS = 1;
	pRXBD->LS = 0;
	pRXBD->PhyAddr = (u32)((u8 *)pRxBdHdl->pRXDESC);
	pRXBD->BuffSize = sizeof(INIC_RX_DESC);
	DCache_CleanInvalidate((u32)pRXBD, sizeof(SPDIO_RX_BD));
	pRxBdHdl->isPktEnd = 0;

	pgSDIODev->RXBDWPtr += 1;
	if (pgSDIODev->RXBDWPtr >= obj->tx_bd_num) {
		pgSDIODev->RXBDWPtr -= obj->tx_bd_num;
	}

	RxBdWrite++;

	/* Take RX_BD to transmit packet payload */
	pkt_size = pbuf->buf_size;
	Offset = 0;
	do {
		pRXBD = pgSDIODev->pRXBDAddrAligned + pgSDIODev->RXBDWPtr;	// get the RX_BD head
		pRxBdHdl = pgSDIODev->pRXBDHdl + pgSDIODev->RXBDWPtr;
		pRxBdHdl->isFree = 0;
		pRXBD->FS = 0;
		pRXBD->PhyAddr = (u32)((u8 *)pbuf->buf_addr + Offset);
#if defined(SDIO_RX_PKT_SIZE_OVER_16K) && SDIO_RX_PKT_SIZE_OVER_16K
		if ((pkt_size - Offset) <= MAX_RX_BD_BUF_SIZE)	{
			pRXBD->BuffSize = pkt_size - Offset;
			pRxBdHdl->isPktEnd = 1;
		} else {
			pRXBD->BuffSize = MAX_RX_BD_BUF_SIZE;
			pRxBdHdl->isPktEnd = 0;
			RTK_LOGI(TAG, "SDIO_Return_Rx_Data: Split RX_BD, Offset=%d PktSize=%d\n", \
					 Offset, pkt_size);
		}
#else
		if (pkt_size > MAX_RX_BD_BUF_SIZE)	{
			// if come to here, please enable "SDIO_RX_PKT_SIZE_OVER_16K"
			RTK_LOGE(TAG, "SDIO_Return_Rx_Data: The Packet Size bigger than 16K\n");
			pkt_size = MAX_RX_BD_BUF_SIZE;
		}
		pRXBD->BuffSize = pkt_size;
		pRxBdHdl->priv = (void *)pbuf;
		pRxBdHdl->isPktEnd = 1;
#endif
		Offset += pRXBD->BuffSize;
		// Move the RX_BD Write pointer forward
		RxBdWrite++;
		pgSDIODev->RXBDWPtr += 1;
		if (pgSDIODev->RXBDWPtr >= obj->tx_bd_num) {
			pgSDIODev->RXBDWPtr -= obj->tx_bd_num;
		}

		if (Offset >= pkt_size) {
			pRXBD->LS = 1;
		}
		DCache_CleanInvalidate((u32)pRXBD, sizeof(SPDIO_RX_BD));
	} while (Offset < pkt_size);

	if (RxBdWrite > 0) {
		SDIO_RXBD_WPTR_Set(pgSDIODev->RXBDWPtr);
		SDIO_RxReq();
	}

	RTK_LOGI(TAG, "SDIO_Return_Rx_Data(%d)<==\n", RxBdWrite);
	return _TRUE;
}

void spdio_structinit(struct spdio_t *obj)
{
	obj->rx_bd_bufsz = SPDIO_RX_BUFSZ_ALIGN(2048 + 24); //extra 24 bytes for sdio header
	obj->rx_bd_num = 24;
	obj->tx_bd_num = 24;
	obj->priv = NULL;
	obj->rx_buf = NULL;
	obj->rx_done_cb = NULL;
	obj->tx_done_cb = NULL;
}

void spdio_trigger_rx_handle(void)
{
	if (pgSPDIODev->WaitForTxbuf) {
		rtos_sema_give(pgSPDIODev->IrqSema);
	}
}

/******************************************************************************
 * Function: SDIO_TX_FIFO_DataReady
 * Desc: Handle the SDIO FIFO data ready interrupt.
 *		1. Send those data to the target driver via callback fun., like WLan.
 *		2. Allocate a buffer for the TX BD
 *
 * Para:
 * 	 pSDIODev: The SDIO device data structor.
 ******************************************************************************/
void SPDIO_TX_FIFO_DataReady(IN PHAL_SPDIO_ADAPTER pSPDIODev)
{
	SPDIO_TX_BD_HANDLE *pTxBdHdl;
	PINIC_TX_DESC pTxDesc;
	u16 TxBDWPtr = 0;
	u16 TxBDRPtr = 0;
	u8 isForceBreak = 0;
	s8 ret = FAIL;
	u32 reg;
	SPDIO_TX_BD *pTXBD = NULL;
	struct spdio_t *obj = (struct spdio_t *)pSPDIODev->spdio_priv;
	u32 WPTR_FLAG, RPTR_FLAG;

	TxBDWPtr = (u16) SDIO_TXBD_WPTR_Get();
	WPTR_FLAG = BIT_SPDIO_TXBD_H2C_WPTR_WRAP & SDIO_TXBD_WPTR_Get();
	TxBDRPtr = (u16) SDIO_TXBD_RPTR_Get();
	RPTR_FLAG = BIT_SPDIO_TXBD_H2C_RPTR_WRAP & SDIO_TXBD_RPTR_Get();

	if ((TxBDWPtr == TxBDRPtr) && (WPTR_FLAG == RPTR_FLAG)) {
		if (unlikely(pSPDIODev->TxOverFlow != 0)) {
			pSPDIODev->TxOverFlow = 0;
			reg = SDIO_DMA_CTRL_Get();
			RTK_LOGS_LVL(TAG, RTK_LOG_WARN, "SDIO TX Overflow Case: Reg DMA_CTRL==0x%x %x %x %x\n", (reg >> 24) & 0xff, (reg >> 16) & 0xff, (reg >> 8) & 0xff,
						 (reg) & 0xff);
		} else {
			RTK_LOGS_LVL(TAG, RTK_LOG_WARN, "SDIO TX Data Read False Triggered!!, TXBDWPtr=0x%x\n", TxBDWPtr);
			goto exit;
		}
	}

	do {
		RTK_LOGS_LVL(TAG, RTK_LOG_DEBUG, "SDIO_TX_DataReady: TxBDWPtr=%d TxBDRPtr=%d\n", TxBDWPtr, pSPDIODev->TXBDRPtr);
		pTXBD = (SPDIO_TX_BD *)(pSPDIODev->pTXBDAddrAligned + pSPDIODev->TXBDRPtr);
		pTxBdHdl = pSPDIODev->pTXBDHdl + pSPDIODev->TXBDRPtr;
		pTxDesc = (PINIC_TX_DESC)(pTXBD->Address);
		DCache_Invalidate((u32)pTxDesc, sizeof(INIC_TX_DESC));

		RTK_LOGS_LVL(TAG, RTK_LOG_DEBUG, "SDIO_TX_DataReady: PktSz=%d Offset=%d\n", pTxDesc->txpktsize, pTxDesc->offset);

		if ((pTxDesc->txpktsize + pTxDesc->offset) <= obj->rx_bd_bufsz) {
			// use the callback function to fordward this packet to target(WLan) driver
			ret = spdio_rx_done_cb(obj, (u8 *)pTxBdHdl->priv, pTxDesc->offset, pTxDesc->txpktsize, pTxDesc->type);
			if (ret == FAIL) {
				pSPDIODev->WaitForTxbuf = _TRUE;
				RTK_LOGS_LVL(TAG, RTK_LOG_DEBUG, "spdio_rx_done_cb return fail!\n");
			} else {
				pSPDIODev->WaitForTxbuf = _FALSE;
			}
			pTXBD->Address =  obj->rx_buf[pSPDIODev->TXBDRPtr].buf_addr;
			DCache_CleanInvalidate((u32)pTXBD->Address, obj->rx_buf[pSPDIODev->TXBDRPtr].size_allocated);
			DCache_Clean((u32)pTXBD, sizeof(SPDIO_TX_BD));
		} else {
			// Invalid packet, Just drop it
			ret = SUCCESS;  // pretend we call the TX callback OK
		}

		if (SUCCESS != ret) {
			// may be is caused by TX queue is full, so we skip it and try again later
			isForceBreak = 1;
			break;  // break the while loop
		} else {
			pSPDIODev->TXBDRPtr++;
			if (pSPDIODev->TXBDRPtr >= obj->rx_bd_num) {
				RPTR_FLAG = BIT_SPDIO_TXBD_H2C_RPTR_WRAP & SDIO_TXBD_RPTR_Get();
				if (RPTR_FLAG) {
					RPTR_FLAG = 0;
				} else {
					RPTR_FLAG = BIT_SPDIO_TXBD_H2C_RPTR_WRAP;
				}
				pSPDIODev->TXBDRPtr = 0;
			}
			pSPDIODev->TXBDRPtrReg = pSPDIODev->TXBDRPtr;
			SDIO_TXBD_RPTR_Set(((u32)pSPDIODev->TXBDRPtrReg) | RPTR_FLAG);
		}
		TxBDWPtr = (u16) SDIO_TXBD_WPTR_Get();
		WPTR_FLAG = BIT_SPDIO_TXBD_H2C_WPTR_WRAP & SDIO_TXBD_WPTR_Get();
		if (isForceBreak) {
			break;	// break the TX FIFO DMA Done processing
		}
	} while (!((pSPDIODev->TXBDRPtr == TxBDWPtr) && (WPTR_FLAG == RPTR_FLAG)));

	// if not all TX data were processed, set an event to trigger SDIO_Task to process them later
	if (isForceBreak) {
		RTK_LOGS_LVL(TAG, RTK_LOG_DEBUG, "SDIO_TX Force Break: TXBDWP=0x%x TXBDRP=0x%x\n", TxBDWPtr, pSPDIODev->TXBDRPtr);
	}

exit:

	return;
}

void SPDIO_Recycle_Rx_BD(IN PHAL_SPDIO_ADAPTER pgSPDIODev)
{
	SPDIO_RX_BD_HANDLE *pRxBdHdl;
	SPDIO_RX_BD *pRXBD;
	u32 FreeCnt = 0;		// for debugging
	struct spdio_t *obj = (struct spdio_t *)pgSPDIODev->spdio_priv;
	u8 isPktEnd = _FALSE;

	SDIO_INTConfig(BIT_C2H_DMA_OK, DISABLE);
	while (SDIO_RXBD_RPTR_Get() != pgSPDIODev->RXBDRPtr) {
		pRxBdHdl = pgSPDIODev->pRXBDHdl + pgSPDIODev->RXBDRPtr;
		pRXBD = pRxBdHdl->pRXBD;
		isPktEnd = _FALSE;

		if (!pRxBdHdl->isFree) {
			if (pRxBdHdl->isPktEnd) {
				isPktEnd = _TRUE;
			}
			pRxBdHdl->isPktEnd = 0;
			_memset((void *)(pRxBdHdl->pRXDESC), 0, sizeof(INIC_RX_DESC));
			_memset((void *)pRXBD, 0, sizeof(SPDIO_RX_BD));	// clean this RX_BD
			pRxBdHdl->isFree = 1;
		} else {
			RTK_LOGS_LVL(TAG, RTK_LOG_WARN, "SDIO_Recycle_Rx_BD: Warring, Recycle a Free RX_BD,RXBDRPtr=%d\n", pgSPDIODev->RXBDRPtr);
		}
		pgSPDIODev->RXBDRPtr++;
		if (pgSPDIODev->RXBDRPtr >= obj->tx_bd_num) {
			pgSPDIODev->RXBDRPtr -= obj->tx_bd_num;
		}

		if (isPktEnd) {
			spdio_tx_done_cb(obj, (u8 *)(pRxBdHdl->priv));
		}
	}
	SDIO_INTConfig(BIT_C2H_DMA_OK, ENABLE);
	RTK_LOGS_LVL(TAG, RTK_LOG_DEBUG, "<==SDIO_Recycle_Rx_BD(%u)\n", FreeCnt);

}

/******************************************************************************
 * Function: SPDIO_IRQ_Handler
 * Desc: SPDIO device interrupt service routine
 *		1. Read & clean the interrupt status
 *		2. Wake up the SDIO task to handle the IRQ event
 *
 * Para:
 * 	 pSDIODev: The SDIO device data structor.
 ******************************************************************************/
u32 SPDIO_IRQ_Handler(void *pData)
{
	PHAL_SPDIO_ADAPTER pSPDIODev = pData;
	InterruptDis(SDIO_IRQ);
	rtos_sema_give(pSPDIODev->IrqSema);

	return 0;
}

void SPDIO_IRQ_Handler_BH(void *pData)
{
	PHAL_SPDIO_ADAPTER pgSPDIODev = pData;
	u16 IntStatus;

	for (;;) {
		/* Task blocked and wait the semaphore(events) here */
		rtos_sema_take(pgSPDIODev->IrqSema, RTOS_MAX_TIMEOUT);

		IntStatus = SDIO_INTStatus();
		RTK_LOGS_LVL(TAG, RTK_LOG_DEBUG, "%s:ISRStatus=0x%x\n", __FUNCTION__, IntStatus);

		SDIO_INTClear(IntStatus);	// clean the ISR
		InterruptEn(SDIO_IRQ, SPDIO_IRQ_PRIORITY);

		if (IntStatus & BIT_C2H_DMA_OK) {
			SPDIO_Recycle_Rx_BD(pgSPDIODev);
		}

		if (IntStatus & BIT_H2C_MSG_INT) {
			SDIO_H2C_MSG_Get();
		}

		if (IntStatus & BIT_H2C_DMA_OK || pgSPDIODev->WaitForTxbuf) {
			SDIO_INTConfig(BIT_H2C_DMA_OK, DISABLE);
			SPDIO_TX_FIFO_DataReady(pgSPDIODev);
			SDIO_INTConfig(BIT_H2C_DMA_OK, ENABLE);
		}
		if (IntStatus & BIT_TXFIFO_H2C_OVF) {
			pgSPDIODev->TxOverFlow = 1;
		}

		RTK_LOGS_LVL(TAG, RTK_LOG_DEBUG, "%s @2 IntStatus=0x%x\n", __FUNCTION__, IntStatus);
	}
}

/******************************************************************************
 * Function: SPDIO_Device_Init
 * Desc: SDIO mbed device driver initialization.
 *		1. Allocate SDIO TX BD and RX BD adn RX Desc.
 *		2. Allocate SDIO RX Buffer Descriptor and RX Buffer. Initial RX related
 *			register.
 *		3. Register the Interrupt function.
 *
 ******************************************************************************/
bool SPDIO_Device_Init(struct spdio_t *obj)
{
	u32 i;
	SPDIO_TX_BD_HANDLE *pTxBdHdl;
	SPDIO_RX_BD_HANDLE *pRxBdHdl;
	int ret;
	//SPDIO_TX_BD *pTXBD = NULL;
	SDIO_InitTypeDef SDIO_InitStruct;

	if (obj == NULL) {
		RTK_LOGE(TAG, "struct spdio_t must be inited\n");
		return FAIL;
	}

	RTK_LOGI(TAG, "SDIO_Device_Init==>\n");

	pgSPDIODev = &gSPDIODev;
	pgSPDIODev->spdio_priv = (void *)obj;
	obj->priv = (void *)pgSPDIODev;

// initial TX BD and RX BD
	pgSPDIODev->pTXBDAddr = rtos_mem_malloc((obj->rx_bd_num * sizeof(SPDIO_TX_BD)) + 3);
	if (NULL == pgSPDIODev->pTXBDAddr) {
		RTK_LOGE(TAG, "SDIO_Device_Init: Malloc for TX_BD Err!!\n");
		goto SDIO_INIT_ERR;
	}
	pgSPDIODev->pTXBDAddrAligned = (SPDIO_TX_BD *)(((((u32)pgSPDIODev->pTXBDAddr - 1) >> 2) + 1) << 2);	// Make it 4-bytes aligned

	pgSPDIODev->pRXBDAddr = rtos_mem_malloc((obj->tx_bd_num * sizeof(SPDIO_RX_BD)) + 7);
	if (NULL == pgSPDIODev->pRXBDAddr) {
		RTK_LOGE(TAG, "SDIO_Device_Init: Malloc for RX_BD Err!!\n");
		goto SDIO_INIT_ERR;
	}
	pgSPDIODev->pRXBDAddrAligned = (SPDIO_RX_BD *)(((((u32)pgSPDIODev->pRXBDAddr - 1) >> 3) + 1) << 3);	// Make it 8-bytes aligned

	pgSPDIODev->pRXDESCAddr = rtos_mem_zmalloc((obj->tx_bd_num * sizeof(INIC_RX_DESC)) + 3);
	if (NULL == pgSPDIODev->pRXDESCAddr) {
		RTK_LOGE(TAG, "SDIO_Device_Init: Malloc for RX_DESC Err!!\n");
		goto SDIO_INIT_ERR;
	}
	pgSPDIODev->pRXDESCAddrAligned = (INIC_RX_DESC *)(((((u32)pgSPDIODev->pRXDESCAddr - 1) >> 2) + 1) << 2); //Make it 4-bytes aligned

	SDIO_StructInit(&SDIO_InitStruct);
	SDIO_InitStruct.TXBD_BAR = (u32)pgSPDIODev->pTXBDAddrAligned;
	SDIO_InitStruct.TXBD_RING_SIZE = obj->rx_bd_num; //SDIO_TX_BD_NUM;
	SDIO_InitStruct.TX_BUFFER_SIZE = ((((obj->rx_bd_bufsz - 1) / SPDIO_TX_BUF_SZ_UNIT) + 1) & 0xff);
	SDIO_InitStruct.RXBD_BAR = (u32)pgSPDIODev->pRXBDAddrAligned;
	SDIO_InitStruct.RXBD_RING_SIZE = obj->tx_bd_num;
	SDIO_InitStruct.RXBD_FREE_TH = RX_BD_FREE_TH;

	SDIO_Init((&SDIO_InitStruct));

	pgSPDIODev->TXBDWPtr = SDIO_TXBD_WPTR_Get();
	pgSPDIODev->TXBDRPtr = pgSPDIODev->TXBDWPtr;
	pgSPDIODev->TXBDRPtrReg = pgSPDIODev->TXBDWPtr;

	pgSPDIODev->RXBDWPtr = pgSPDIODev->RXBDRPtr = SDIO_RXBD_RPTR_Get();

	RTK_LOGI(TAG, "TXBDWPtr=0x%x TXBDRPtr=0x%x\n", pgSPDIODev->TXBDWPtr, pgSPDIODev->TXBDRPtr);

	pgSPDIODev->pTXBDHdl = (SPDIO_TX_BD_HANDLE *)rtos_mem_zmalloc(obj->rx_bd_num * sizeof(SPDIO_TX_BD_HANDLE));
	if (NULL == pgSPDIODev->pTXBDHdl) {
		RTK_LOGE(TAG, "SDIO_Device_Init: Malloc for TX_BD Handle Err!!\n");
		goto SDIO_INIT_ERR;
	}

	for (i = 0; i < obj->rx_bd_num; i++) {
		pTxBdHdl = pgSPDIODev->pTXBDHdl + i;
		pTxBdHdl->pTXBD = pgSPDIODev->pTXBDAddrAligned + i;
		// Pre-allocate buffer by User
		pTxBdHdl->priv = (void *)&obj->rx_buf[i];
		pTxBdHdl->pTXBD->Address = (u32)obj->rx_buf[i].buf_addr;
		DCache_CleanInvalidate(pTxBdHdl->pTXBD->Address, obj->rx_buf[i].size_allocated);
		if (pTxBdHdl->pTXBD->Address % 4) {
			RTK_LOGE(TAG, "buffer address must be aligned to 4!!\n");
			goto SDIO_INIT_ERR;
		}

		if (NULL == (u32 *)(pTxBdHdl->pTXBD->Address)) {
			RTK_LOGE(TAG, "SDIO_Device_Init: Malloc buffer for TX_BD Err!!\n");
			goto SDIO_INIT_ERR;
		}
		pTxBdHdl->isFree = 1;
		RTK_LOGI(TAG, "TX_BD%lu @ %p %p\n", i, pTxBdHdl, pTxBdHdl->pTXBD);
	}

	pgSPDIODev->pRXBDHdl = (SPDIO_RX_BD_HANDLE *)rtos_mem_zmalloc(obj->tx_bd_num * sizeof(SPDIO_RX_BD_HANDLE));
	if (NULL == pgSPDIODev->pRXBDHdl) {
		RTK_LOGE(TAG, "SDIO_Device_Init: Malloc for RX_BD Handle Err!!\n");
		goto SDIO_INIT_ERR;
	}

	for (i = 0; i < obj->tx_bd_num; i++) {
		pRxBdHdl = pgSPDIODev->pRXBDHdl + i;
		pRxBdHdl->pRXBD = pgSPDIODev->pRXBDAddrAligned + i;
		pRxBdHdl->pRXDESC = pgSPDIODev->pRXDESCAddrAligned + i;
		pRxBdHdl->isFree = 1;
	}

	DCache_Clean(SDIO_InitStruct.TXBD_BAR, SDIO_InitStruct.TXBD_RING_SIZE * sizeof(SPDIO_TX_BD));
	DCache_Clean(SDIO_InitStruct.RXBD_BAR, SDIO_InitStruct.RXBD_RING_SIZE * sizeof(SPDIO_RX_BD));

	rtos_sema_create(&(pgSPDIODev->IrqSema), 0, RTOS_SEMA_MAX_COUNT);
	if (NULL == pgSPDIODev->IrqSema) {
		RTK_LOGE(TAG, "SDIO_Device_Init Create IRQ Semaphore Err!!\n");
		goto SDIO_INIT_ERR;
	}

	ret = rtos_task_create((rtos_task_t *)&pgSPDIODev->xSDIOIrqTaskHandle, "SPDIO_IRQ_TASK", SPDIO_IRQ_Handler_BH, (void *)pgSPDIODev,
						   1024 * 4, 1 + 4);
	if (SUCCESS != ret) {
		RTK_LOGE(TAG, "SDIO_Device_Init: Create IRQ Task Err(%d)!!\n", ret);
		goto SDIO_INIT_ERR;
	}

	//pgSPDIODev->CRPWM = SDIO_RPWM1_Get();
	//pgSPDIODev->CRPWM2 = SDIO_RPWM2_Get();

	// Indicate Host this is a iNIC FW
	//SDIO_CPWM2_Set(CPWM2_INIC_FW_RDY_BIT, ENABLE);

	/* enable the interrupt */
	InterruptRegister((IRQ_FUN) SPDIO_IRQ_Handler, SDIO_IRQ, (u32) pgSPDIODev, SPDIO_IRQ_PRIORITY);
	InterruptEn(SDIO_IRQ, SPDIO_IRQ_PRIORITY);

	SDIO_INTClearAll();
	SDIO_INTConfig(SDIO_INIT_INT_MASK, ENABLE);

	// Update the power state indication
	SDIO_CPWM2_Set(CPWM2_ACT_BIT, ENABLE);

	/* Indicate the Host system that the TX/RX is ready */
	SDIO_SetReady(ENABLE);

	RTK_LOGI(TAG, "<==SDIO_Device_Init\n");

	return SUCCESS;

SDIO_INIT_ERR:

	if (pgSPDIODev->pRXBDHdl) {
		rtos_mem_free((u8 *)pgSPDIODev->pRXBDHdl);
		pgSPDIODev->pRXBDHdl = NULL;
	}

	if ((pgSPDIODev->pTXBDHdl)) {
		for (i = 0; i < obj->rx_bd_num; i++) {
			pTxBdHdl = pgSPDIODev->pTXBDHdl + i;
			if (pTxBdHdl->pTXBD->Address) {
				pTxBdHdl->pTXBD->Address = (u32)NULL;
			}
		}
		rtos_mem_free((u8 *)pgSPDIODev->pTXBDHdl);
		pgSPDIODev->pTXBDHdl = NULL;
	}

	if (pgSPDIODev->pRXBDAddr) {
		rtos_mem_free((u8 *)pgSPDIODev->pRXBDAddr);
		pgSPDIODev->pRXBDAddr = NULL;
	}

	if (pgSPDIODev->pTXBDAddr) {
		rtos_mem_free(pgSPDIODev->pTXBDAddr);
		pgSPDIODev->pTXBDAddr = NULL;
		pgSPDIODev->pTXBDAddrAligned = NULL;
	}

	if (pgSPDIODev->pRXDESCAddr) {
		rtos_mem_free(pgSPDIODev->pRXDESCAddr);
		pgSPDIODev->pRXDESCAddr = NULL;
		pgSPDIODev->pRXDESCAddrAligned = NULL;
	}
	return FAIL;
}

/******************************************************************************
 * Function: SPDIO_Device_DeInit
 * Desc: SDIO device driver free resource. This function should be called in
 *			a task.
 *		1. Free TX FIFO buffer
 *
 * Para:
 *		pSDIODev: The SDIO device data structor.
 ******************************************************************************/
//TODO: Call this function in a task
void SPDIO_Device_DeInit(void)
{
	u32 i = 0;
	SPDIO_TX_BD_HANDLE *pTxBdHdl;
	SPDIO_TX_BD *pTXBD = NULL;
	struct spdio_t *obj;

	if (NULL == pgSPDIODev) {
		return;
	}

	obj = pgSPDIODev->spdio_priv;
	// Indicate the Host that Ameba is InActived
	SDIO_CPWM2_Set(CPWM2_ACT_BIT, DISABLE);

	if (pgSPDIODev->pRXBDHdl) {
		rtos_mem_free((u8 *)pgSPDIODev->pRXBDHdl);
		pgSPDIODev->pRXBDHdl = NULL;
	}

	/* Free TX FIFO Buffer */
	for (i = 0; i < obj->rx_bd_num; i++) {
		pTXBD = (SPDIO_TX_BD *)(pgSPDIODev->pTXBDAddrAligned + i);
		pTxBdHdl = pgSPDIODev->pTXBDHdl + i;
		if (pTXBD->Address) {
			pTXBD->Address = (u32)NULL;
		}
	}

	if ((pgSPDIODev->pTXBDHdl)) {
		for (i = 0; i < obj->rx_bd_num; i++) {
			pTxBdHdl = pgSPDIODev->pTXBDHdl + i;
			if (pTxBdHdl->pTXBD->Address) {
				pTxBdHdl->pTXBD->Address = (u32)NULL;
			}
		}
		rtos_mem_free((u8 *)pgSPDIODev->pTXBDHdl);
		pgSPDIODev->pTXBDHdl = NULL;
	}

	if (pgSPDIODev->pRXBDAddr) {
		rtos_mem_free((u8 *)pgSPDIODev->pRXBDAddr);
		pgSPDIODev->pRXBDAddr = NULL;
	}

	if (pgSPDIODev->pTXBDAddr) {
		rtos_mem_free(pgSPDIODev->pTXBDAddr);
		pgSPDIODev->pTXBDAddr = NULL;
		pgSPDIODev->pTXBDAddrAligned = NULL;
	}

	if (pgSPDIODev->pRXDESCAddr) {
		rtos_mem_free(pgSPDIODev->pRXDESCAddr);
		pgSPDIODev->pRXDESCAddr = NULL;
		pgSPDIODev->pRXDESCAddrAligned = NULL;
	}

	SDIO_INTConfig(0xffff, DISABLE);
	SDIO_INTClearAll();
	InterruptDis(SDIO_IRQ);
	InterruptUnRegister(SDIO_IRQ);

	if (pgSPDIODev->IrqSema) {
		rtos_sema_delete(pgSPDIODev->IrqSema);
		pgSPDIODev->IrqSema = NULL;
	}

	// Reset SDIO DMA
	SDIO_DMA_Reset();
}

void spdio_init(struct spdio_t *obj)
{
	if (obj == NULL) {
		RTK_LOGE(TAG, "spdio obj is NULL, spdio init failed!\n");
		return;
	}

	if ((obj->rx_bd_num == 0) || (obj->rx_bd_bufsz == 0) || (obj->rx_bd_bufsz % 64)
		|| (obj->tx_bd_num == 0) || (obj->tx_bd_num % 2) || (obj->rx_buf == NULL)) {
		RTK_LOGE(TAG, "spdio obj resource isn't correctly inited, spdio init failed!\n");
		return;
	}

	g_spdio_priv = obj;
	SPDIO_Device_Init(obj);
}

void spdio_deinit(struct spdio_t *obj)
{
	if (obj == NULL) {
		SPDIO_API_PRINTK("spdio obj is NULL, spdio deinit failed");
		return;
	}
	SPDIO_Device_DeInit();
	g_spdio_priv = NULL;
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

