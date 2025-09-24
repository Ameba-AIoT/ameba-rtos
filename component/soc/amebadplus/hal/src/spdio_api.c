#include "ameba_soc.h"
#include "spdio_api.h"
#include "ameba_inic.h"
#include "os_wrapper.h"

static const char *const TAG = "SPDIO";

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
#define SPDIO_TX_BD_BUF_SZ_UNIT		64
#define SPDIO_RX_BD_FREE_TH				5
#define SPDIO_MIN_RX_BD_SEND_PKT			2
#define SPDIO_MAX_RX_BD_BUF_SIZE			16380	// the Maximum size for a RX_BD point to, make it 4-bytes aligned

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

	u8 WaitForDeviceRxbuf;
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

static s8 spdio_device_rx_done_cb(void *padapter, void *data, u16 offset, u16 pktsize, u8 type)
{
	struct spdio_buf_t *buf = (struct spdio_buf_t *)data;
	struct spdio_t *obj = (struct spdio_t *)padapter;

	DCache_Invalidate((u32)buf->buf_addr, pktsize);

	if (obj) {
		return obj->device_rx_done_cb(obj, buf, (u8 *)(buf->buf_addr + offset), pktsize, type);
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "spdio rx done callback function is null!");
	}
	return RTK_SUCCESS;
}

static s8 spdio_device_tx_done_cb(void *padapter, u8 *data)
{
	struct spdio_t *obj = (struct spdio_t *)padapter;
	struct spdio_buf_t *buf = (struct spdio_buf_t *)data;
	if (obj) {
		return obj->device_tx_done_cb(obj, buf);
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "spdio tx done callback function is null!");
	}
	return RTK_SUCCESS;
}

static s8 spdio_rpwm_cb(void *padapter)
{
	struct spdio_t *obj = (struct spdio_t *)padapter;

	u16 rpwm2 = SDIO_RPWM2_Get(SDIO_WIFI);

	if (obj) {
		return obj->rpwm_cb(obj, rpwm2);
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "spdio rpwm callback function is null!");
	}
	return RTK_SUCCESS;
}

/**
 * @brief Spdio write function.
 * @param obj Pointer to a initialized spdio_t structure.
 * @param pbuf Pointer to a spdio_buf_t structure which carries the payload.
 * @retval RTK_SUCCESS or RTK_FAIL.
 */
s8 spdio_tx(struct spdio_t *obj, struct spdio_buf_t *pbuf)
{
	DCache_CleanInvalidate((u32)pbuf->buf_allocated, pbuf->size_allocated);
	PHAL_SPDIO_ADAPTER pgSDIODev = obj->priv;
	INIC_RX_DESC *pRxDesc;
	SPDIO_RX_BD_HANDLE *pRxBdHdl;
	SPDIO_RX_BD *pRXBD;
	u32 Offset = 0;
	u16 RxBdWrite = 0; // to count how much RX_BD used in a Transaction
	u16 RxBdRdPtr = pgSDIODev->RXBDRPtr; // RX_BD read pointer
	u32 pkt_size;
#if defined(SDIO_RX_PKT_SIZE_OVER_16K) && SDIO_RX_PKT_SIZE_OVER_16K
	u8 needed_rxbd_num;
#endif
	/* check if RX_BD available */
#if defined(SDIO_RX_PKT_SIZE_OVER_16K) && SDIO_RX_PKT_SIZE_OVER_16K
	needed_rxbd_num = ((pbuf->buf_size - 1) / SPDIO_MAX_RX_BD_BUF_SIZE) + SPDIO_MIN_RX_BD_SEND_PKT;
#endif
	if (RxBdRdPtr != pgSDIODev->RXBDWPtr) {
		if (pgSDIODev->RXBDWPtr > RxBdRdPtr) {
#if defined(SDIO_RX_PKT_SIZE_OVER_16K) && SDIO_RX_PKT_SIZE_OVER_16K
			if ((pgSDIODev->RXBDWPtr - RxBdRdPtr) >= (u16)(obj->host_rx_bd_num - needed_rxbd_num))
#else
			if ((pgSDIODev->RXBDWPtr - RxBdRdPtr) >= (u16)(obj->host_rx_bd_num - (u32)SPDIO_MIN_RX_BD_SEND_PKT))
#endif
			{
				RTK_LOGS(TAG, RTK_LOG_WARN, "SDIO_Return_Rx_Data: No Available RX_BD, ReadPtr=%d WritePtr=%d\n", \
						 RxBdRdPtr, pgSDIODev->RXBDWPtr);
				return FALSE;
			}
		} else {
#if defined(SDIO_RX_PKT_SIZE_OVER_16K) && SDIO_RX_PKT_SIZE_OVER_16K
			if ((RxBdRdPtr - pgSDIODev->RXBDWPtr) <= (u16)needed_rxbd_num)
#else
			if ((RxBdRdPtr - pgSDIODev->RXBDWPtr) <= (u16)SPDIO_MIN_RX_BD_SEND_PKT)
#endif
			{
				RTK_LOGS(TAG, RTK_LOG_WARN, "SDIO_Return_Rx_Data: No Available RX_BD, ReadPtr=%d WritePtr=%d\n", RxBdRdPtr, pgSDIODev->RXBDWPtr);
				return FALSE;
			}
		}
	}

	// TODO: Add RX_DESC before the packet

	/* a SDIO RX packet will use at least 2 RX_BD, the 1st one is for RX_Desc,
	   other RX_BDs are for packet payload */
	/* Use a RX_BD to transmit RX_Desc */
	pRXBD = pgSDIODev->pRXBDAddrAligned + pgSDIODev->RXBDWPtr; // get the RX_BD head
	pRxBdHdl = pgSDIODev->pRXBDHdl + pgSDIODev->RXBDWPtr;

	pRxDesc = pRxBdHdl->pRXDESC;
	pRxDesc->type = pbuf->type;
	pRxDesc->pkt_len = pbuf->buf_size;
	pRxDesc->offset = sizeof(INIC_RX_DESC);
	DCache_CleanInvalidate((u32)pRxDesc, sizeof(INIC_RX_DESC));

	if (!pRxBdHdl->isFree) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "SDIO_Return_Rx_Data: Allocated a non-free RX_BD\n");
	}
	pRxBdHdl->isFree = 0;
	pRXBD->FS = 1;
	pRXBD->LS = 0;
	pRXBD->PhyAddr = (u32)((u8 *)pRxBdHdl->pRXDESC);
	pRXBD->BuffSize = sizeof(INIC_RX_DESC);
	DCache_CleanInvalidate((u32)pRXBD, sizeof(SPDIO_RX_BD));
	pRxBdHdl->isPktEnd = 0;

	pgSDIODev->RXBDWPtr += 1;
	if (pgSDIODev->RXBDWPtr >= obj->host_rx_bd_num) {
		pgSDIODev->RXBDWPtr -= obj->host_rx_bd_num;
	}

	RxBdWrite++;

	/* Take RX_BD to transmit packet payload */
	pkt_size = pbuf->buf_size;
	Offset = 0;
	do {
		pRXBD = pgSDIODev->pRXBDAddrAligned + pgSDIODev->RXBDWPtr; // get the RX_BD head
		pRxBdHdl = pgSDIODev->pRXBDHdl + pgSDIODev->RXBDWPtr;
		pRxBdHdl->isFree = 0;
		pRXBD->FS = 0;
		pRXBD->PhyAddr = (u32)((u8 *)pbuf->buf_addr + Offset);
#if defined(SDIO_RX_PKT_SIZE_OVER_16K) && SDIO_RX_PKT_SIZE_OVER_16K
		if ((pkt_size - Offset) <= SPDIO_MAX_RX_BD_BUF_SIZE) {
			pRXBD->BuffSize = pkt_size - Offset;
			pRxBdHdl->isPktEnd = 1;
		} else {
			pRXBD->BuffSize = SPDIO_MAX_RX_BD_BUF_SIZE;
			pRxBdHdl->isPktEnd = 0;
			RTK_LOGS(TAG, RTK_LOG_INFO, "SDIO_Return_Rx_Data: Split RX_BD, Offset=%d PktSize=%d\n", \
					 Offset, pkt_size);
		}
#else
		if (pkt_size > SPDIO_MAX_RX_BD_BUF_SIZE) {
			// if come to here, please enable "SDIO_RX_PKT_SIZE_OVER_16K"
			RTK_LOGS(TAG, RTK_LOG_ERROR, "SDIO_Return_Rx_Data: The Packet Size bigger than 16K\n");
			pkt_size = SPDIO_MAX_RX_BD_BUF_SIZE;
		}
		pRXBD->BuffSize = pkt_size;
		pRxBdHdl->priv = (void *)pbuf;
		pRxBdHdl->isPktEnd = 1;
#endif
		Offset += pRXBD->BuffSize;
		// Move the RX_BD Write pointer forward
		RxBdWrite++;
		pgSDIODev->RXBDWPtr += 1;
		if (pgSDIODev->RXBDWPtr >= obj->host_rx_bd_num) {
			pgSDIODev->RXBDWPtr -= obj->host_rx_bd_num;
		}

		if (Offset >= pkt_size) {
			pRXBD->LS = 1;
		}
		DCache_CleanInvalidate((u32)pRXBD, sizeof(SPDIO_RX_BD));
	} while (Offset < pkt_size);

	if (RxBdWrite > 0) {
		SDIO_RXBD_WPTR_Set(SDIO_WIFI, pgSDIODev->RXBDWPtr);
		SDIO_RxReq(SDIO_WIFI);
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "SDIO_Return_Rx_Data(%d)<==\n", RxBdWrite);
	return TRUE;
}

/**
 * @brief Get example settings for spdio obj.
 * @param obj Pointer to a spdio_t structure which will be initialized with an example settings.
 * @return None
 */
void spdio_structinit(struct spdio_t *obj)
{
	obj->device_rx_bufsz = SPDIO_RX_BUFSZ_ALIGN(2048 + 24); // extra 24 bytes for sdio header
	obj->host_tx_bd_num = 24;
	obj->host_rx_bd_num = 24;
	obj->priv = NULL;
	obj->rx_buf = NULL;
	obj->device_rx_done_cb = NULL;
	obj->device_tx_done_cb = NULL;
}

/**
 * @brief Trigger SDIO read packet when have free skb.
 */
void spdio_trigger_rx_handle(void)
{
	if (pgSPDIODev->WaitForDeviceRxbuf) {
		rtos_sema_give(pgSPDIODev->IrqSema);
	}
}

/**
 * @brief Handle the SDIO FIFO data ready interrupt, including
 * 		- Send those data to target driver via callback func, like WLan.
 * 		- Allocate a buffer for the TX BD
 * @param pSPDIODev Pointer to a SDIO device data structure.
 * @return None
 */
void SPDIO_TxBd_DataReady_DeviceRx(PHAL_SPDIO_ADAPTER pSPDIODev)
{
	SPDIO_TX_BD_HANDLE *pTxBdHdl;
	PINIC_TX_DESC pTxDesc;
	u16 TxBDWPtr = 0;
	u16 TxBDRPtr = 0;
	u8 isForceBreak = 0;
	s8 ret = RTK_FAIL;
	u32 reg;
	SPDIO_TX_BD *pTXBD = NULL;
	struct spdio_t *obj = (struct spdio_t *)pSPDIODev->spdio_priv;
	u32 WPTR_FLAG, RPTR_FLAG;

	TxBDWPtr = (u16)SDIO_TXBD_WPTR_Get(SDIO_WIFI);
	WPTR_FLAG = BIT_SPDIO_TXBD_H2C_WPTR_WRAP & SDIO_TXBD_WPTR_Get(SDIO_WIFI);
	TxBDRPtr = (u16)SDIO_TXBD_RPTR_Get(SDIO_WIFI);
	RPTR_FLAG = BIT_SPDIO_TXBD_H2C_RPTR_WRAP & SDIO_TXBD_RPTR_Get(SDIO_WIFI);

	if ((TxBDWPtr == TxBDRPtr) && (WPTR_FLAG == RPTR_FLAG)) {
		if (unlikely(pSPDIODev->TxOverFlow != 0)) {
			pSPDIODev->TxOverFlow = 0;
			reg = SDIO_DMA_CTRL_Get(SDIO_WIFI);
			RTK_LOGS(TAG, RTK_LOG_WARN, "SDIO TX Overflow Case: Reg DMA_CTRL==0x%x %x %x %x\n", (reg >> 24) & 0xff, (reg >> 16) & 0xff, (reg >> 8) & 0xff,
					 (reg) & 0xff);
		} else {
			//RTK_LOGS(TAG, RTK_LOG_INFO, "SDIO TX Data Read False Triggered!!, TXBDWPtr=0x%x\n", TxBDWPtr);
			goto exit;
		}
	}

	do {
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "SDIO_TX_DataReady: TxBDWPtr=%d TxBDRPtr=%d\n", TxBDWPtr, pSPDIODev->TXBDRPtr);
		pTXBD = (SPDIO_TX_BD *)(pSPDIODev->pTXBDAddrAligned + pSPDIODev->TXBDRPtr);
		pTxBdHdl = pSPDIODev->pTXBDHdl + pSPDIODev->TXBDRPtr;
		pTxDesc = (PINIC_TX_DESC)(pTXBD->Address);
		DCache_Invalidate((u32)pTxDesc, sizeof(INIC_TX_DESC));

		RTK_LOGS(TAG, RTK_LOG_DEBUG, "SDIO_TX_DataReady: PktSz=%d Offset=%d\n", pTxDesc->txpktsize, pTxDesc->offset);

		if ((pTxDesc->txpktsize + pTxDesc->offset) <= obj->device_rx_bufsz) {
			// use the callback function to fordward this packet to target(WLan) driver
			ret = spdio_device_rx_done_cb(obj, (u8 *)pTxBdHdl->priv, pTxDesc->offset, pTxDesc->txpktsize, pTxDesc->type);
			if (ret == RTK_FAIL) {
				pSPDIODev->WaitForDeviceRxbuf = TRUE;
				RTK_LOGS(TAG, RTK_LOG_DEBUG, "spdio_device_rx_done_cb return fail!\n");
			} else {
				pSPDIODev->WaitForDeviceRxbuf = FALSE;
			}
			pTXBD->Address = obj->rx_buf[pSPDIODev->TXBDRPtr].buf_addr;
			DCache_CleanInvalidate((u32)pTXBD->Address, obj->rx_buf[pSPDIODev->TXBDRPtr].size_allocated);
			DCache_Clean((u32)pTXBD, sizeof(SPDIO_TX_BD));
		} else {
			// Invalid packet, Just drop it
			ret = RTK_SUCCESS; // pretend we call the TX callback OK
		}

		if (RTK_SUCCESS != ret) {
			// may be is caused by TX queue is full, so we skip it and try again later
			isForceBreak = 1;
			break; // break the while loop
		} else {
			pSPDIODev->TXBDRPtr++;
			if (pSPDIODev->TXBDRPtr >= obj->host_tx_bd_num) {
				RPTR_FLAG = BIT_SPDIO_TXBD_H2C_RPTR_WRAP & SDIO_TXBD_RPTR_Get(SDIO_WIFI);
				if (RPTR_FLAG) {
					RPTR_FLAG = 0;
				} else {
					RPTR_FLAG = BIT_SPDIO_TXBD_H2C_RPTR_WRAP;
				}
				pSPDIODev->TXBDRPtr = 0;
			}
			pSPDIODev->TXBDRPtrReg = pSPDIODev->TXBDRPtr;
			SDIO_TXBD_RPTR_Set(SDIO_WIFI, ((u32)pSPDIODev->TXBDRPtrReg) | RPTR_FLAG);
		}
		TxBDWPtr = (u16)SDIO_TXBD_WPTR_Get(SDIO_WIFI);
		WPTR_FLAG = BIT_SPDIO_TXBD_H2C_WPTR_WRAP & SDIO_TXBD_WPTR_Get(SDIO_WIFI);
		if (isForceBreak) {
			break; // break the TX FIFO DMA Done processing
		}
	} while (!((pSPDIODev->TXBDRPtr == TxBDWPtr) && (WPTR_FLAG == RPTR_FLAG)));

	// if not all TX data were processed, set an event to trigger SDIO_Task to process them later
	if (isForceBreak) {
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "SDIO_TX Force Break: TXBDWP=0x%x TXBDRP=0x%x\n", TxBDWPtr, pSPDIODev->TXBDRPtr);
	}

exit:

	return;
}

static void SPDIO_Recycle_Rx_BD(PHAL_SPDIO_ADAPTER pgSPDIODev)
{
	SPDIO_RX_BD_HANDLE *pRxBdHdl;
	SPDIO_RX_BD *pRXBD;
	u32 FreeCnt = 0; // for debugging
	struct spdio_t *obj = (struct spdio_t *)pgSPDIODev->spdio_priv;
	u8 isPktEnd = FALSE;

	while (SDIO_RXBD_RPTR_Get(SDIO_WIFI) != pgSPDIODev->RXBDRPtr) {
		pRxBdHdl = pgSPDIODev->pRXBDHdl + pgSPDIODev->RXBDRPtr;
		pRXBD = pRxBdHdl->pRXBD;
		isPktEnd = FALSE;

		if (!pRxBdHdl->isFree) {
			if (pRxBdHdl->isPktEnd) {
				isPktEnd = TRUE;
			}
			pRxBdHdl->isPktEnd = 0;
			_memset((void *)(pRxBdHdl->pRXDESC), 0, sizeof(INIC_RX_DESC));
			_memset((void *)pRXBD, 0, sizeof(SPDIO_RX_BD)); // clean this RX_BD
			pRxBdHdl->isFree = 1;
		} else {
			RTK_LOGS(TAG, RTK_LOG_WARN, "SDIO_Recycle_Rx_BD: Warring, Recycle a Free RX_BD,RXBDRPtr=%d\n", pgSPDIODev->RXBDRPtr);
		}
		pgSPDIODev->RXBDRPtr++;
		if (pgSPDIODev->RXBDRPtr >= obj->host_rx_bd_num) {
			pgSPDIODev->RXBDRPtr -= obj->host_rx_bd_num;
		}

		if (isPktEnd) {
			spdio_device_tx_done_cb(obj, (u8 *)(pRxBdHdl->priv));
		}
	}
	RTK_LOGS(TAG, RTK_LOG_DEBUG, "<==SDIO_Recycle_Rx_BD(%u)\n", FreeCnt);
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
	PHAL_SPDIO_ADAPTER pSPDIODev = pData;
	InterruptDis(SDIO_IRQ);
	rtos_sema_give(pSPDIODev->IrqSema);

	return 0;
}

static void SPDIO_IRQ_Handler_BH(void *pData)
{
	PHAL_SPDIO_ADAPTER pgSPDIODev = pData;
	u16 IntStatus;

	for (;;) {
		/* Task blocked and wait the semaphore(events) here */
		rtos_sema_take(pgSPDIODev->IrqSema, RTOS_MAX_TIMEOUT);

		IntStatus = SDIO_INTStatus(SDIO_WIFI);
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "%s:ISRStatus=0x%x\n", __FUNCTION__, IntStatus);

		SDIO_INTClear(SDIO_WIFI, IntStatus); // clean the ISR
		InterruptEn(SDIO_IRQ, SPDIO_IRQ_PRIORITY);

		if (IntStatus & BIT_C2H_DMA_OK) {
			SPDIO_Recycle_Rx_BD(pgSPDIODev);
		}

		if (IntStatus & BIT_H2C_MSG_INT) {
			SDIO_H2C_MSG_Get(SDIO_WIFI);
		}

		if (IntStatus & BIT_RPWM2_INT) {
			spdio_rpwm_cb((struct spdio_t *)pgSPDIODev->spdio_priv);
		}

		if (IntStatus & BIT_H2C_DMA_OK || pgSPDIODev->WaitForDeviceRxbuf) {
			SPDIO_TxBd_DataReady_DeviceRx(pgSPDIODev);
		}
		if (IntStatus & BIT_TXFIFO_H2C_OVF) {
			pgSPDIODev->TxOverFlow = 1;
		}

		RTK_LOGS(TAG, RTK_LOG_DEBUG, "%s @2 IntStatus=0x%x\n", __FUNCTION__, IntStatus);
	}
}

void SPDIO_Board_Init(void)
{
	u8 idx;
	/* Be consistent with order of SDIO_PAD[][]. */
	char *sdio_pin[] = {"CLK", "CMD", "D0", "D1", "D2", "D3"};

	assert_param(SDIO_Pin_Grp <= 0x4);

	/* Pinmux function and Pad control */
	for (idx = 0; idx < 6; idx++) {
		PAD_PullCtrl(SDIO_PAD[SDIO_Pin_Grp][idx], GPIO_PuPd_UP);
		Pinmux_Config(SDIO_PAD[SDIO_Pin_Grp][idx], PINMUX_FUNCTION_SDIO);
		RTK_LOGI(TAG, "SDIO_%s --> P%c%d\n", sdio_pin[idx], 'A' + PORT_NUM(SDIO_PAD[SDIO_Pin_Grp][idx]), PIN_NUM(SDIO_PAD[SDIO_Pin_Grp][idx]));
	}
}

/**
 * @brief Initilaize SDIO device, including
 * 		- Allocate SDIO TX BD, RX BD and RX Desc.
 * 		- Allocate SDIO RX Buffer Descriptor and RX Buffer. Initialize RX related registers.
 * 		- Register the Interrupt function.
 * @param obj Pointer to a spdio_t structure.
 * @return Initialization result, which can be RTK_SUCCESS(0) or RTK_FAIL(-1).
 */
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
		return RTK_FAIL;
	}

	RTK_LOGI(TAG, "SDIO_Device_Init==>\n");

	pgSPDIODev = &gSPDIODev;
	pgSPDIODev->spdio_priv = (void *)obj;
	obj->priv = (void *)pgSPDIODev;

	// initial TX BD and RX BD
	pgSPDIODev->pTXBDAddr = rtos_mem_malloc((obj->host_tx_bd_num * sizeof(SPDIO_TX_BD)) + 3);
	if (NULL == pgSPDIODev->pTXBDAddr) {
		RTK_LOGE(TAG, "SDIO_Device_Init: Malloc for TX_BD Err!!\n");
		goto SDIO_INIT_ERR;
	}
	pgSPDIODev->pTXBDAddrAligned = (SPDIO_TX_BD *)(((((u32)pgSPDIODev->pTXBDAddr - 1) >> 2) + 1) << 2); // Make it 4-bytes aligned

	pgSPDIODev->pRXBDAddr = rtos_mem_malloc((obj->host_rx_bd_num * sizeof(SPDIO_RX_BD)) + 7);
	if (NULL == pgSPDIODev->pRXBDAddr) {
		RTK_LOGE(TAG, "SDIO_Device_Init: Malloc for RX_BD Err!!\n");
		goto SDIO_INIT_ERR;
	}
	pgSPDIODev->pRXBDAddrAligned = (SPDIO_RX_BD *)(((((u32)pgSPDIODev->pRXBDAddr - 1) >> 3) + 1) << 3); // Make it 8-bytes aligned

	pgSPDIODev->pRXDESCAddr = rtos_mem_zmalloc((obj->host_rx_bd_num * sizeof(INIC_RX_DESC)) + 3);
	if (NULL == pgSPDIODev->pRXDESCAddr) {
		RTK_LOGE(TAG, "SDIO_Device_Init: Malloc for RX_DESC Err!!\n");
		goto SDIO_INIT_ERR;
	}
	pgSPDIODev->pRXDESCAddrAligned = (INIC_RX_DESC *)(((((u32)pgSPDIODev->pRXDESCAddr - 1) >> 2) + 1) << 2); // Make it 4-bytes aligned

	SPDIO_Board_Init();

	/* SDIO function enable and clock enable*/
	RCC_PeriphClockCmd(APBPeriph_SDIO, APBPeriph_SDIO_CLOCK, ENABLE);

	SDIO_StructInit(&SDIO_InitStruct);
	SDIO_InitStruct.TXBD_BAR = (u32)pgSPDIODev->pTXBDAddrAligned;
	SDIO_InitStruct.TXBD_RING_SIZE = obj->host_tx_bd_num; // SDIO_TX_BD_NUM;
	SDIO_InitStruct.TX_BUFFER_SIZE = ((((obj->device_rx_bufsz - 1) / SPDIO_TX_BD_BUF_SZ_UNIT) + 1) & 0xff);
	SDIO_InitStruct.RXBD_BAR = (u32)pgSPDIODev->pRXBDAddrAligned;
	SDIO_InitStruct.RXBD_RING_SIZE = obj->host_rx_bd_num;
	SDIO_InitStruct.RXBD_FREE_TH = SPDIO_RX_BD_FREE_TH;

	SDIO_Init(SDIO_WIFI, (&SDIO_InitStruct));

	pgSPDIODev->TXBDWPtr = SDIO_TXBD_WPTR_Get(SDIO_WIFI);
	pgSPDIODev->TXBDRPtr = pgSPDIODev->TXBDWPtr;
	pgSPDIODev->TXBDRPtrReg = pgSPDIODev->TXBDWPtr;

	pgSPDIODev->RXBDWPtr = pgSPDIODev->RXBDRPtr = SDIO_RXBD_RPTR_Get(SDIO_WIFI);

	RTK_LOGI(TAG, "TXBDWPtr=0x%x TXBDRPtr=0x%x\n", pgSPDIODev->TXBDWPtr, pgSPDIODev->TXBDRPtr);

	pgSPDIODev->pTXBDHdl = (SPDIO_TX_BD_HANDLE *)rtos_mem_zmalloc(obj->host_tx_bd_num * sizeof(SPDIO_TX_BD_HANDLE));
	if (NULL == pgSPDIODev->pTXBDHdl) {
		RTK_LOGE(TAG, "SDIO_Device_Init: Malloc for TX_BD Handle Err!!\n");
		goto SDIO_INIT_ERR;
	}

	for (i = 0; i < obj->host_tx_bd_num; i++) {
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

	pgSPDIODev->pRXBDHdl = (SPDIO_RX_BD_HANDLE *)rtos_mem_zmalloc(obj->host_rx_bd_num * sizeof(SPDIO_RX_BD_HANDLE));
	if (NULL == pgSPDIODev->pRXBDHdl) {
		RTK_LOGE(TAG, "SDIO_Device_Init: Malloc for RX_BD Handle Err!!\n");
		goto SDIO_INIT_ERR;
	}

	for (i = 0; i < obj->host_rx_bd_num; i++) {
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
						   1024 * 4, 1);
	if (RTK_SUCCESS != ret) {
		RTK_LOGE(TAG, "SDIO_Device_Init: Create IRQ Task Err(%d)!!\n", ret);
		goto SDIO_INIT_ERR;
	}

	//pgSPDIODev->CRPWM = SDIO_RPWM1_Get();
	//pgSPDIODev->CRPWM2 = SDIO_RPWM2_Get(SDIO_WIFI);

	// Indicate Host this is a iNIC FW
	//SDIO_CPWM2_Set(SDIO_WIFI, CPWM2_INIC_FW_RDY_BIT, ENABLE);

	/* enable the interrupt */
	InterruptRegister((IRQ_FUN) SPDIO_IRQ_Handler, SDIO_IRQ, (u32) pgSPDIODev, SPDIO_IRQ_PRIORITY);
	InterruptEn(SDIO_IRQ, SPDIO_IRQ_PRIORITY);

	SDIO_INTClearAll(SDIO_WIFI);
	SDIO_INTConfig(SDIO_WIFI, SDIO_INIT_INT_MASK, ENABLE);

	// Update the power state indication
	SDIO_CPWM2_Set(SDIO_WIFI, CPWM2_ACT_BIT, ENABLE);

	/* Indicate the Host system that the TX/RX is ready */
	SDIO_SetReady(SDIO_WIFI, ENABLE);

	RTK_LOGI(TAG, "<==SDIO_Device_Init\n");

	return RTK_SUCCESS;

SDIO_INIT_ERR:

	if (pgSPDIODev->pRXBDHdl) {
		rtos_mem_free((u8 *)pgSPDIODev->pRXBDHdl);
		pgSPDIODev->pRXBDHdl = NULL;
	}

	if ((pgSPDIODev->pTXBDHdl)) {
		for (i = 0; i < obj->host_tx_bd_num; i++) {
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
	return RTK_FAIL;
}

/**
 * @brief Free SDIO device, including freeing TX FIFO buffer.
 * @return None
 * @note This function should be called in a task.
 */
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
	SDIO_CPWM2_Set(SDIO_WIFI, CPWM2_ACT_BIT, DISABLE);

	if (pgSPDIODev->pRXBDHdl) {
		rtos_mem_free((u8 *)pgSPDIODev->pRXBDHdl);
		pgSPDIODev->pRXBDHdl = NULL;
	}

	/* Free TX FIFO Buffer */
	for (i = 0; i < obj->host_tx_bd_num; i++) {
		pTXBD = (SPDIO_TX_BD *)(pgSPDIODev->pTXBDAddrAligned + i);
		pTxBdHdl = pgSPDIODev->pTXBDHdl + i;
		if (pTXBD->Address) {
			pTXBD->Address = (u32)NULL;
		}
	}

	if ((pgSPDIODev->pTXBDHdl)) {
		for (i = 0; i < obj->host_tx_bd_num; i++) {
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

	SDIO_INTConfig(SDIO_WIFI, 0xffff, DISABLE);
	SDIO_INTClearAll(SDIO_WIFI);
	InterruptDis(SDIO_IRQ);
	InterruptUnRegister(SDIO_IRQ);

	rtos_task_delete(pgSPDIODev->xSDIOIrqTaskHandle);

	if (pgSPDIODev->IrqSema) {
		rtos_sema_delete(pgSPDIODev->IrqSema);
		pgSPDIODev->IrqSema = NULL;
	}

	// Reset SDIO DMA
	SDIO_DMA_Reset(SDIO_WIFI);
}

/**
 * @brief Initialize spdio interface.
 * @param obj Pointer to a spdio_t structure which should be initialized by user,
 * 		and which will be used to initialize spdio interface.
 * 		- obj->host_rx_bd_num: spdio write bd number, needs 2 bd for one transaction.
 * 		- obj->host_tx_bd_num: spdio read bd number.
 * 		- obj->device_rx_bufsz: spdio read buffer size.
 * 		- obj->rx_buf: spdio read buffer array.
 * @return None
 */
void spdio_init(struct spdio_t *obj)
{
	if (obj == NULL) {
		RTK_LOGE(TAG, "spdio obj is NULL, spdio init failed!\n");
		return;
	}

	if ((obj->host_tx_bd_num == 0) || (obj->device_rx_bufsz == 0) || (obj->device_rx_bufsz % 64)
		|| (obj->host_rx_bd_num == 0) || (obj->host_rx_bd_num % 2) || (obj->rx_buf == NULL)) {
		RTK_LOGE(TAG, "spdio obj resource isn't correctly inited, spdio init failed!\n");
		return;
	}

	g_spdio_priv = obj;
	SPDIO_Device_Init(obj);
}

/**
 * @brief Deinitialize spdio interface.
 * @param obj Pointer to a spdio_t structure which is already initialized
 * @return None
 */
void spdio_deinit(struct spdio_t *obj)
{
	if (obj == NULL) {
		RTK_LOGW(TAG, "spdio obj is NULL, spdio deinit failed\n");
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
