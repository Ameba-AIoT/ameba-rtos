/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "ameba_soc.h"
#include "bootloader_km4tz.h"
#include "ameba_dump_stack.h"
#include "ameba_secure_boot.h"
#include "boot_ota_km4tz.h"
#include "ameba_inic.h"

static const char *const TAG = "BOOT";

#define SPDIO_DMA_ALIGN_4			4



#define SPDIO_TX_BD_NUM	N_BYTE_ALIGMENT(4, 2)	// 2 bd for one transaction, must be rounded to 2
#define SPDIO_RX_BD_NUM	10
#define SPDIO_RX_BUFSZ	N_BYTE_ALIGMENT((2048 + 24), SPDIO_TX_BUF_SZ_UNIT) // desired packet length + 24(spdio header info), must be rounded to 64
#define SPDIO_TX_BUFSZ	N_BYTE_ALIGMENT((2048 + 24), 64) ////TODO: desired packet length + 24(spdio header info), must be rounded to 64


#define SPDIO_TX_BUF_SZ_UNIT		64
#define RX_BD_FREE_TH				5
#define MIN_RX_BD_SEND_PKT			2
#define MAX_RX_BD_BUF_SIZE			16380	// the Maximum size for a RX_BD point to, make it 4-bytes aligned

#define SDIO_INIT_INT_MASK			(SDIO_WIFI_BIT_H2C_DMA_OK | SDIO_WIFI_BIT_C2H_DMA_OK | \
									SDIO_WIFI_BIT_H2C_BUS_RES_FAIL | SDIO_WIFI_BIT_RX_BD_FLAG_ERR_INT | \
									SDIO_NOTIFY_TYPE_INT)

#define SDIO_NOTIFY_TYPE_INT		(SDIO_WIFI_BIT_H2C_MSG_INT | SDIO_WIFI_BIT_RPWM1_INT | \
									SDIO_WIFI_BIT_RPWM2_INT | SDIO_WIFI_BIT_HOST_WAKE_CPU_INT | \
									SDIO_WIFI_BIT_H2C_SUS_REQ)

#define SDIO_INCR_RING_IDX(idx, range)   \
    do {                          \
        (idx)++;                  \
        if ((idx) >= (range)) {   \
            (idx) -= (range);     \
        }                         \
    } while(0)

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

struct spdio_buf_t {
	//TODO: void *priv; //priv data from user
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
	void *priv;//TODO: rx_buf = INIC_TX_DESC + RxData
	u8 isPktEnd;//TODO:			/* For a packet over 1 BD , this flag to indicate is this BD contains a packet end */
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
	void *priv;//TODO: buf_t
	SPDIO_RX_BD *pRXBD;		/* Point to the RX_BD buffer */
	INIC_RX_DESC *pRXDESC;	/* point to the Rx Packet *///TODO: tx_buf = INIC_RX_DESC + TxData
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
	INIC_RX_DESC *pRXDESCAddrAligned; //TODO:
	SPDIO_RX_BD_HANDLE *pRXBDHdl;	/* point to the allocated memory for RX_BD Handle array */
	u16 RXBDWPtr;					/* The SDIO RX(Device->Host) BD write index */
	u16 RXBDRPtr;					/* The SDIO RX(Device->Host) BD local read index, different with HW maintained Read Index. */

	rtos_sema_t IrqSema;			/* Semaphore for SDIO RX, use to wakeup the SDIO RX task */
	rtos_task_t xSDIOIrqTaskHandle;	/* The handle of the SDIO Task special for RX, can be used to delete the task */

	u8 WaitForTxbuf;
} HAL_SPDIO_ADAPTER, *PHAL_SPDIO_ADAPTER;

static void _Init_SDIO_By_PinGrp(void)
{
	u8 idx;
	extern u8 SDIO_Pin_Grp;
	extern const u8 SDIO_PAD[7][6];

	assert_param(SDIO_Pin_Grp <= 0x6);
	RTK_LOGW(TAG, "Modify SDIO_Pin_Grp by Board...\n");

	/* Pinmux function and Pad control */
	for (idx = 0; idx < 6; idx++) {
		PAD_PullCtrl(SDIO_PAD[SDIO_Pin_Grp][idx], GPIO_PuPd_UP);
		Pinmux_Config(SDIO_PAD[SDIO_Pin_Grp][idx], PINMUX_FUNCTION_SDIO_SLV);
	}

	/* SDIO function enable and clock enable*/
	RCC_PeriphClockCmd(APBPeriph_SDD, APBPeriph_SDD_CLOCK, ENABLE);

	/* CCCR Autoload done */
	SDIO_WIFI->SPDIO_SYS_CPU_INDICATION |= SDIO_WIFI_BIT_READEE_DONE;

	/* SDIO pinmux function enable */
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_SDIO_CTRL, HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_SDIO_CTRL) | LSYS_BIT_SDD_PMUX_FEN);
}

void Boot_SDIO_Pinmux_init(void)
{
	u32 boot_src = SYSCFG_OTP_BOOTSEL();
	/* When not boot from sdio, sdio pinmux shall init early in bootloader.
	 * If done too late, it may cause host side SDIO card recognition timeout, resulting in sdio power supply failure. */
	if (boot_src != BOOT_FROM_SDIO) {
		_Init_SDIO_By_PinGrp();
	}
}

void Boot_Fullmac_Secure_Check(void)
{
	IMAGE_HEADER *ImgHdr = (IMAGE_HEADER *)((u32)__image2_entry_func__ - 0x20);
	SubImgInfo_TypeDef SubImgInfo;
	Manifest_TypeDef *Manifest;
	FIH_DECLARE(fih_rc, FIH_FAILURE);
	/* 1. get image info */
	SubImgInfo.Addr = ImgHdr->image_addr - IMAGE_HEADER_LEN;
	SubImgInfo.Len = ImgHdr->image_size + IMAGE_HEADER_LEN;
	assert_param(SubImgInfo.Addr == (u32)ImgHdr);

	/* 2. locate manifest */
	Manifest = (Manifest_TypeDef *)(SubImgInfo.Addr + SubImgInfo.Len);
	/* 3. verify signature */
	FIH_CALL(BOOT_Extract_SignatureCheck, fih_rc, Manifest, &SubImgInfo, 1);
	if (FIH_EQ(fih_rc, FIH_SUCCESS)) {
		return;
	}

	RTK_LOGE(TAG, "FULLMAC IMG2 SBOOT Check Fail!\n");
}

void Boot_Fullmac_LoadIMGAll(void)
{
	u32 boot_src = SYSCFG_OTP_BOOTSEL();
	if ((boot_src == BOOT_FROM_FLASH) || (boot_src == BOOT_FROM_FLASH1)) { // for test purpose, delete it later
		flash_highspeed_setup();

		// 3. Bootloader download img2 by fullmac interface

		Boot_Fullmac_LoadImage();
	} else {
		/* rom code init flash only when BOOT_FROM_FLASH */
		if (ChipInfo_MemoryType() != MEMORY_SINGLE_DIE) {
			void BOOT_ROM_InitFlash(void);
			BOOT_ROM_InitFlash();

			flash_highspeed_setup();

			// 3. Bootloader download img2 by fullmac interface

			Boot_Fullmac_LoadImage();
		} else {
			// Boot_SDIO_ImgDownload();
			Boot_Fullmac_Secure_Check();
		}
	}
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
	UNUSED(pData);
	InterruptDis(SDIO_WIFI_IRQ);

	return 0;
}


HAL_SPDIO_ADAPTER gSPDIODev;

// g_TXBDHdl/g_RXBDHdl shall be init to 0
SPDIO_TX_BD_HANDLE g_TXBDHdl[SPDIO_RX_BD_NUM];
SPDIO_RX_BD_HANDLE g_RXBDHdl[SPDIO_TX_BD_NUM];

// g_RXDESCAddr shall be init to 0
__ALIGNED(4) INIC_RX_DESC g_RXDESCAddr[SPDIO_TX_BD_NUM];

u32 spdio_dev_rx_buf_entry[SPDIO_RX_BD_NUM][SPDIO_RX_BUFSZ / sizeof(u32)];

struct spdio_buf_t spdio_dev_tx_buf[SPDIO_TX_BD_NUM];
u32 spdio_dev_tx_buf_entry[SPDIO_TX_BD_NUM][SPDIO_TX_BUFSZ / sizeof(u32)];

void Boot_SDIO_TxBdHdl_Init(SPDIO_TX_BD_HANDLE *g_TXBDHdl, SPDIO_TX_BD *SPDIO_TXBDAddr, struct spdio_buf_t *spdio_dev_rx_buf)
{
	SPDIO_TX_BD_HANDLE *pTxBdHdl;
	u32 i;

	for (i = 0; i < SPDIO_RX_BD_NUM; i++) {
		pTxBdHdl = &g_TXBDHdl[i];
		pTxBdHdl->pTXBD = &SPDIO_TXBDAddr[i];
		// Pre-allocate buffer by User
		pTxBdHdl->priv = (void *)&spdio_dev_rx_buf[i];
		pTxBdHdl->pTXBD->Address = spdio_dev_rx_buf[i].buf_addr;
		assert_param(spdio_dev_rx_buf[i].buf_addr != 0);
		assert_param(spdio_dev_rx_buf[i].buf_addr % SPDIO_DMA_ALIGN_4 == 0);
		DCache_CleanInvalidate(spdio_dev_rx_buf[i].buf_allocated, spdio_dev_rx_buf[i].size_allocated);

		pTxBdHdl->isFree = 1;
		RTK_LOGI(TAG, "TX_BD%lu @ %p %p\n", i, pTxBdHdl, pTxBdHdl->pTXBD);
	}
}

void Boot_SDIO_RxBdHdl_Init(SPDIO_RX_BD_HANDLE *g_RXBDHdl, SPDIO_RX_BD *SPDIO_RXBDAddr, INIC_RX_DESC *g_RXDESCAddr)
{
	SPDIO_RX_BD_HANDLE *pRxBdHdl;
	u32 i;

	for (i = 0; i < SPDIO_TX_BD_NUM; i++) {
		pRxBdHdl = &g_RXBDHdl[i];
		pRxBdHdl->pRXBD = &SPDIO_RXBDAddr[i];
		pRxBdHdl->pRXDESC = &g_RXDESCAddr[i];
		pRxBdHdl->isFree = 1;
		pRxBdHdl->isPktEnd = 0;
	}
}

void Boot_SDIO_Device_Init(SPDIO_TX_BD *SPDIO_TXBDAddr, SPDIO_RX_BD *SPDIO_RXBDAddr, PHAL_SPDIO_ADAPTER pSPDIODev)
{
	SDIO_InitTypeDef SDIO_InitStruct;
	SDIO_StructInit(&SDIO_InitStruct);
	SDIO_InitStruct.TXBD_BAR = (u32)SPDIO_TXBDAddr;
	SDIO_InitStruct.TXBD_RING_SIZE = SPDIO_RX_BD_NUM & 0xFFFF;
	SDIO_InitStruct.TX_BUFFER_SIZE = DIV_ROUND_UP(SPDIO_RX_BUFSZ, SPDIO_TX_BUF_SZ_UNIT) & 0xFF; // Round up to nearest 64 bytes
	SDIO_InitStruct.RXBD_BAR = (u32)SPDIO_RXBDAddr;
	SDIO_InitStruct.RXBD_RING_SIZE = SPDIO_TX_BD_NUM & 0xFFFF;
	SDIO_InitStruct.RXBD_FREE_TH = RX_BD_FREE_TH;
	SDIO_InitStruct.TX_AGG_DISPATCH = 1;
	SDIO_Init(SDIO_WIFI, &SDIO_InitStruct);

	DCache_Clean(SDIO_InitStruct.TXBD_BAR, SPDIO_RX_BD_NUM * sizeof(SPDIO_TX_BD));
	DCache_Clean(SDIO_InitStruct.RXBD_BAR, SPDIO_TX_BD_NUM * sizeof(SPDIO_RX_BD));

	/* enable the interrupt */
	InterruptRegister((IRQ_FUN)SPDIO_IRQ_Handler, SDIO_WIFI_IRQ, (u32)pSPDIODev, INT_PRI_MIDDLE);
	InterruptEn(SDIO_WIFI_IRQ, INT_PRI_MIDDLE);

	SDIO_INTClearAll(SDIO_WIFI);
	SDIO_INTConfig(SDIO_WIFI, SDIO_INIT_INT_MASK, ENABLE);

	// Update the power state indication
	SDIO_CPWM2_Set(SDIO_WIFI, CPWM2_ACT_BIT, ENABLE);

	/* Indicate the Host system that the TX/RX is ready */
	SDIO_SetReady(SDIO_WIFI, ENABLE);

	RTK_LOGI(TAG, "<==SDIO_Device_Init\n");
}

static s8 spdio_tx_done_cb(PHAL_SPDIO_ADAPTER pSPDIODev, struct spdio_buf_t *buf)
{
	// struct spdio_t *obj = (struct spdio_t *)pSPDIODev->spdio_priv;
	// if (obj) {
	// 	return obj->tx_done_cb(obj, buf);
	// } else {
	// 	RTK_LOGS(TAG, RTK_LOG_ERROR, "spdio tx done callback function is null!");
	// }
	UNUSED(pSPDIODev);
	UNUSED(buf);
	return RTK_SUCCESS;
}

static s8 spdio_rx_done_cb(PHAL_SPDIO_ADAPTER pSPDIODev, struct spdio_buf_t *buf, u16 offset, u16 pktsize, u8 type)
{
	// struct spdio_t *obj = (struct spdio_t *)pSPDIODev->spdio_priv;

	DCache_Invalidate((u32)(buf->buf_addr + offset), pktsize);
	UNUSED(pSPDIODev);
	UNUSED(type);
	// if (obj) {
	// 	return obj->rx_done_cb(obj, buf, (u8 *)(buf->buf_addr + offset), pktsize, type);
	// } else {
	// 	RTK_LOGS(TAG, RTK_LOG_ERROR, "spdio rx done callback function is null!");
	// }
	return RTK_SUCCESS;
}

static void SPDIO_Notify_INT(u16 IntStatus)
{
	if (IntStatus & SDIO_WIFI_BIT_RPWM1_INT) {
		RTK_LOGI(NOTAG, "RPWM INT trigged, value is: 0x%x\n", SDIO_RPWM1_Get(SDIO_WIFI));
	}

	if (IntStatus & SDIO_WIFI_BIT_RPWM2_INT) {
		RTK_LOGI(NOTAG, "RPWM2 INT trigged, value is: 0x%x\n", SDIO_RPWM2_Get(SDIO_WIFI));
	}

	if (IntStatus & SDIO_WIFI_BIT_H2C_MSG_INT) {
		RTK_LOGI(NOTAG, "H2C MSG INT trigged, MSG and EXT MSG value: 0x%x, 0x%x\n",
				 (unsigned int)SDIO_H2C_MSG_Get(SDIO_WIFI), (unsigned int) SDIO_H2C_MSG_EXT_Get(SDIO_WIFI));
	}

	if (IntStatus & SDIO_WIFI_BIT_HOST_WAKE_CPU_INT) {
		RTK_LOGI(NOTAG, "SDIO_WIFI_BIT_HOST_WAKE_CPU_INT triggered\n");
	}

	if (IntStatus & SDIO_WIFI_BIT_H2C_SUS_REQ) {
		RTK_LOGI(NOTAG, "SDIO_WIFI_BIT_H2C_SUS_REQ triggered\n");
	}
}

static void SPDIO_Recycle_Rx_BD(IN PHAL_SPDIO_ADAPTER pSPDIODev)
{
	SPDIO_RX_BD_HANDLE *pRxBdHdl;
	SPDIO_RX_BD *pRXBD;
	u8 isPktEnd = FALSE;

	while (SDIO_RXBD_RPTR_Get(SDIO_WIFI) != pSPDIODev->RXBDRPtr) {
		pRxBdHdl = pSPDIODev->pRXBDHdl + pSPDIODev->RXBDRPtr;
		pRXBD = pRxBdHdl->pRXBD;
		isPktEnd = FALSE;

		if (!pRxBdHdl->isFree) {
			if (pRxBdHdl->isPktEnd) {
				isPktEnd = TRUE;
			}
			pRxBdHdl->isPktEnd = 0;
			pRxBdHdl->isFree = TRUE;
			_memset((void *)(pRxBdHdl->pRXDESC), 0, sizeof(INIC_RX_DESC));
			_memset((void *)pRXBD, 0, sizeof(SPDIO_RX_BD)); // clean this RX_BD // TODO:
		} else {
			RTK_LOGS(TAG, RTK_LOG_WARN, "SDIO_Recycle_Rx_BD: Warring, Recycle a Free RX_BD, RXBDRPtr=%d\n", pSPDIODev->RXBDRPtr);
		}

		SDIO_INCR_RING_IDX(pSPDIODev->RXBDRPtr, SPDIO_TX_BD_NUM);

		if (isPktEnd) {
			spdio_tx_done_cb(pSPDIODev, pRxBdHdl->priv);
		}
	}
}

/**
 * @brief Handle the SDIO FIFO data ready interrupt, including
 * 		- Send those data to target driver via callback func, like WLan.
 * 		- Allocate a buffer for the TX BD
 * @param pSPDIODev Pointer to a SDIO device data structure.
 * @return None
 */
void SPDIO_TX_FIFO_DataReady(IN PHAL_SPDIO_ADAPTER pSPDIODev)
{
	SPDIO_TX_BD_HANDLE *pTxBdHdl;
	SPDIO_TX_BD *pTXBD = NULL;
	PINIC_TX_DESC pTxDesc;
	volatile u16 TxBDWPtr = 0;
	s8 ret = RTK_FAIL;
	u32 reg;

	TxBDWPtr = SDIO_TXBD_WPTR_Get(SDIO_WIFI);
	if (TxBDWPtr == pSPDIODev->TXBDRPtr) {
		if (unlikely(pSPDIODev->TxOverFlow != 0)) {
			pSPDIODev->TxOverFlow = 0;
			reg = SDIO_DMA_CTRL_Get(SDIO_WIFI);
			RTK_LOGS(TAG, RTK_LOG_WARN, "SDIO TX Overflow Case: Reg DMA_CTRL=0x%x %x %x %x\n",
					 (reg >> 24) & 0xff, (reg >> 16) & 0xff, (reg >> 8) & 0xff, (reg) & 0xff);
		} else {
			//RTK_LOGS(TAG, RTK_LOG_INFO, "SDIO TX Data Read False Triggered!!, TXBDWPtr=0x%x\n", TxBDWPtr);
			return;
		}
	}

	do {
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "SDIO_TX_DataReady: TxBDWPtr=%d TxBDRPtr=%d\n", TxBDWPtr, pSPDIODev->TXBDRPtr);
		pTXBD = (SPDIO_TX_BD *)(pSPDIODev->pTXBDAddrAligned + pSPDIODev->TXBDRPtr);
		pTxBdHdl = pSPDIODev->pTXBDHdl + pSPDIODev->TXBDRPtr;
		pTxDesc = (PINIC_TX_DESC)(pTXBD->Address);
		DCache_Invalidate((u32)pTxDesc, sizeof(INIC_TX_DESC));

		RTK_LOGS(TAG, RTK_LOG_DEBUG, "SDIO_TX_DataReady: PktSz=%d Offset=%d\n", pTxDesc->txpktsize, pTxDesc->offset);

		if ((pTxDesc->txpktsize + pTxDesc->offset) <= SPDIO_RX_BUFSZ) {
			// use the callback function to fordward this packet to target(WLan) driver
			ret = spdio_rx_done_cb(pSPDIODev, pTxBdHdl->priv, pTxDesc->offset, pTxDesc->txpktsize, pTxDesc->type);
			if (ret == RTK_FAIL) {
				pSPDIODev->WaitForTxbuf = TRUE;
				RTK_LOGS(TAG, RTK_LOG_DEBUG, "spdio_rx_done_cb return fail!\n");
			} else {
				pSPDIODev->WaitForTxbuf = FALSE;
			}
		} else {
			// Invalid packet, Just drop it
			ret = RTK_SUCCESS; // pretend we call the TX callback OK
		}

		if (RTK_SUCCESS != ret) {
			// may be is caused by TX queue is full, so we skip it and try again later
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "SDIO_TX Force Break: TXBDWP=0x%x TXBDRP=0x%x\n", TxBDWPtr, pSPDIODev->TXBDRPtr);
			break; // break the while loop
		} else {
			SDIO_INCR_RING_IDX(pSPDIODev->TXBDRPtr, SPDIO_RX_BD_NUM);
			pSPDIODev->TXBDRPtrReg = pSPDIODev->TXBDRPtr;
			SDIO_TXBD_RPTR_Set(SDIO_WIFI, pSPDIODev->TXBDRPtrReg);
		}

		TxBDWPtr = SDIO_TXBD_WPTR_Get(SDIO_WIFI);
	} while (pSPDIODev->TXBDRPtr != TxBDWPtr);
}

static void SPDIO_IRQ_Handler_BH(void *pData)
{
	PHAL_SPDIO_ADAPTER pSPDIODev = pData;
	u16 IntStatus;

	for (;;) {
		/* Task blocked and wait the semaphore(events) here */
		// rtos_sema_take(pSPDIODev->IrqSema, RTOS_MAX_TIMEOUT);

		IntStatus = SDIO_INTStatus(SDIO_WIFI);
		SDIO_INTClear(SDIO_WIFI, IntStatus); // clean the ISR

		if (IntStatus & SDIO_NOTIFY_TYPE_INT) {
			SPDIO_Notify_INT(IntStatus);
		}

		if (IntStatus & SDIO_WIFI_BIT_C2H_DMA_OK) {
			SPDIO_Recycle_Rx_BD(pSPDIODev);
		}

		if (IntStatus & SDIO_WIFI_BIT_TXBD_H2C_OVF) {
			pSPDIODev->TxOverFlow = 1;
		}

		if (IntStatus & SDIO_WIFI_BIT_H2C_DMA_OK || pSPDIODev->WaitForTxbuf) {
			SPDIO_TX_FIFO_DataReady(pSPDIODev);
		}

		InterruptEn(SDIO_WIFI_IRQ, INT_PRI_MIDDLE);
	}
}

void Boot_SDIO_ImgDownload(void)
{
	u32 i;

	struct spdio_buf_t spdio_dev_rx_buf[SPDIO_RX_BD_NUM];
	for (i = 0; i < SPDIO_RX_BD_NUM; i++) {
		spdio_dev_rx_buf[i].buf_allocated = (u32)spdio_dev_rx_buf_entry[i];
		spdio_dev_rx_buf[i].size_allocated = SPDIO_RX_BUFSZ;
		// this buffer must be 4 byte alignment
		spdio_dev_rx_buf[i].buf_addr = N_BYTE_ALIGMENT(spdio_dev_rx_buf[i].buf_allocated, SPDIO_DMA_ALIGN_4);
	}

	SPDIO_TX_BD SPDIO_TXBDAddr[SPDIO_RX_BD_NUM];
	__ALIGNED(8) SPDIO_RX_BD SPDIO_RXBDAddr[SPDIO_TX_BD_NUM];

	PHAL_SPDIO_ADAPTER pSPDIODev = &gSPDIODev;
	pSPDIODev->TXBDWPtr = SDIO_TXBD_WPTR_Get(SDIO_WIFI);
	pSPDIODev->TXBDRPtr = pSPDIODev->TXBDWPtr;
	pSPDIODev->TXBDRPtrReg = pSPDIODev->TXBDWPtr;

	pSPDIODev->RXBDWPtr = pSPDIODev->RXBDRPtr = SDIO_RXBD_RPTR_Get(SDIO_WIFI);
	RTK_LOGI(TAG, "TXBDWPtr=0x%x TXBDRPtr=0x%x\n", pSPDIODev->TXBDWPtr, pSPDIODev->TXBDRPtr);

	pSPDIODev->pTXBDAddrAligned = SPDIO_TXBDAddr;
	pSPDIODev->pRXBDAddrAligned = SPDIO_RXBDAddr;
	pSPDIODev->pRXDESCAddrAligned = g_RXDESCAddr;
	pSPDIODev->pTXBDHdl = g_TXBDHdl;
	pSPDIODev->pRXBDHdl = g_RXBDHdl;

	Boot_SDIO_TxBdHdl_Init(g_TXBDHdl, SPDIO_TXBDAddr, spdio_dev_rx_buf);
	Boot_SDIO_RxBdHdl_Init(g_RXBDHdl, SPDIO_RXBDAddr, g_RXDESCAddr);

	Boot_SDIO_Device_Init(SPDIO_TXBDAddr, SPDIO_RXBDAddr, pSPDIODev);

	SPDIO_IRQ_Handler_BH(pSPDIODev);
}

/**
 * @brief Spdio write function.
 * @param obj Pointer to a initialized spdio_t structure.
 * @param pbuf Pointer to a spdio_buf_t structure which carries the payload.
 * @retval RTK_SUCCESS or RTK_FAIL.
 */
s8 spdio_tx(struct spdio_buf_t *pbuf)
{
	DCache_CleanInvalidate((u32)pbuf->buf_allocated, pbuf->size_allocated);// TODO:
	PHAL_SPDIO_ADAPTER pSPDIODev = &gSPDIODev;
	SPDIO_RX_BD_HANDLE *pRxBdHdl;
	SPDIO_RX_BD *pRXBD;
	INIC_RX_DESC *pRxDesc;
	u32 Offset = 0;
	u16 RxBdRdPtr = pSPDIODev->RXBDRPtr; // RX_BD read pointer
	u32 pkt_size = pbuf->buf_size;
	/* check if RX_BD available */
#if defined(SDIO_RX_PKT_SIZE_OVER_16K) && SDIO_RX_PKT_SIZE_OVER_16K
	u8 needed_rxbd_num = ((pbuf->buf_size - 1) / MAX_RX_BD_BUF_SIZE) + MIN_RX_BD_SEND_PKT;
#else
	u8 needed_rxbd_num = MIN_RX_BD_SEND_PKT;
#endif

	if (RxBdRdPtr != pSPDIODev->RXBDWPtr) {
		if (pSPDIODev->RXBDWPtr > RxBdRdPtr) {
			if ((pSPDIODev->RXBDWPtr - RxBdRdPtr) >= (SPDIO_TX_BD_NUM - needed_rxbd_num)) {
				RTK_LOGW(TAG, "SDIO_Return_Rx_Data: No Available RX_BD, ReadPtr=%d WritePtr=%d\n", RxBdRdPtr, pSPDIODev->RXBDWPtr);
				return FALSE;
			}
		} else {
			if ((RxBdRdPtr - pSPDIODev->RXBDWPtr) <= needed_rxbd_num) {
				RTK_LOGW(TAG, "SDIO_Return_Rx_Data: No Available RX_BD, ReadPtr=%d WritePtr=%d\n", RxBdRdPtr, pSPDIODev->RXBDWPtr);
				return FALSE;
			}
		}
	}

	// TODO: Add RX_DESC before the packet

	/* a SDIO RX packet will use at least 2 RX_BD, the 1st one is for RX_Desc, other RX_BDs are for packet payload */
	/* Use a RX_BD to transmit RX_Desc */
	pRXBD = pSPDIODev->pRXBDAddrAligned + pSPDIODev->RXBDWPtr; // get the RX_BD head
	pRxBdHdl = pSPDIODev->pRXBDHdl + pSPDIODev->RXBDWPtr;

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
	SDIO_INCR_RING_IDX(pSPDIODev->RXBDWPtr, SPDIO_TX_BD_NUM);

	/* Take RX_BD to transmit packet payload */
	do {
		pRXBD = pSPDIODev->pRXBDAddrAligned + pSPDIODev->RXBDWPtr; // get the RX_BD head
		pRxBdHdl = pSPDIODev->pRXBDHdl + pSPDIODev->RXBDWPtr;
		pRxBdHdl->isFree = 0;
		pRXBD->FS = 0;
		pRXBD->PhyAddr = (u32)((u8 *)pbuf->buf_addr + Offset);
#if defined(SDIO_RX_PKT_SIZE_OVER_16K) && SDIO_RX_PKT_SIZE_OVER_16K
		if ((pkt_size - Offset) <= MAX_RX_BD_BUF_SIZE) {
			pRXBD->BuffSize = pkt_size - Offset;
			pRxBdHdl->isPktEnd = 1;
		} else {
			pRXBD->BuffSize = MAX_RX_BD_BUF_SIZE;
			pRxBdHdl->isPktEnd = 0;
		}
#else
		if (pkt_size > MAX_RX_BD_BUF_SIZE) {
			// if come to here, please enable "SDIO_RX_PKT_SIZE_OVER_16K"
			RTK_LOGE(TAG, "SDIO_Return_Rx_Data: The Packet Size bigger than 16K\n");
			pkt_size = MAX_RX_BD_BUF_SIZE;
		}
		pRXBD->BuffSize = pkt_size;
		pRxBdHdl->isPktEnd = 1;
#endif
		Offset += pRXBD->BuffSize;
		if (Offset >= pkt_size) {
			pRxBdHdl->priv = (void *)pbuf;
			pRXBD->LS = 1;
		}
		DCache_CleanInvalidate((u32)pRXBD, sizeof(SPDIO_RX_BD));
		SDIO_INCR_RING_IDX(pSPDIODev->RXBDWPtr, SPDIO_TX_BD_NUM);
	} while (Offset < pkt_size);

	SDIO_RXBD_WPTR_Set(SDIO_WIFI, pSPDIODev->RXBDWPtr);
	SDIO_RxReq(SDIO_WIFI);

	return TRUE;
}

char ex_spdio_tx(u8 *pdata, u16 size, u8 type)
{
	UNUSED(type);
	int ret;
	PHAL_SPDIO_ADAPTER pSPDIODev = &gSPDIODev;
	struct spdio_buf_t *tx_buf = &spdio_dev_tx_buf[pSPDIODev->RXBDWPtr];

	tx_buf->buf_allocated = (u32)&spdio_dev_tx_buf_entry[pSPDIODev->RXBDWPtr];
	tx_buf->size_allocated = SPDIO_TX_BUFSZ;
	tx_buf->buf_addr = (u32)N_BYTE_ALIGMENT((u32)(tx_buf->buf_allocated), SPDIO_DMA_ALIGN_4);

	// copy data
	_memcpy((void *)tx_buf->buf_addr, pdata, size);
	tx_buf->buf_size = size;
	tx_buf->type = SPDIO_RX_DATA_USER; // you can define your own data type in spdio_rx_data_t and spdio_tx_data_t

	ret = spdio_tx(tx_buf);
	if (ret == FALSE) {
		// rtos_mem_free((u8 *)tx_buf->buf_allocated);
		// rtos_mem_free((u8 *)tx_buf);
	}

	return RTK_SUCCESS;
}
