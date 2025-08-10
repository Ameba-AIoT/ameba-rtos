#include "ameba_soc.h"
#include "osif.h"
#include "bt_inic_defs.h"
#include "hci_if_inic.h"
#include "platform_autoconf.h"
#include <stdio.h>
#include <string.h>
#include "bt_debug.h"
/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2014 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#define DEV_DMA_ALIGN               4

struct bt_inic_priv {
	void *rx_queue;
	u8 rx_req_flag;
#if !(defined(CONFIG_WHC_INTF_SPI) && CONFIG_WHC_INTF_SPI)
	u32 dev_status;
	void *dev_sts_lock;
	void *tx_lock;
	void *spi_transfer_done_sema;

	GDMA_InitTypeDef SSITxGdmaInitStruct;
	GDMA_InitTypeDef SSIRxGdmaInitStruct;

	u8 *rx_buf;
	u8 *tx_buf;

	u8 rx_req;
	u8 wait_tx;

	u8 ssris_pending: 1;
	u8 set_devsts_pending: 1;
#endif
};

struct bt_inic_spi_hdr {
	uint32_t event;
	uint32_t len : 24;
	uint8_t type;
} __attribute__((packed));

static struct bt_inic_priv spi_priv = {0};

enum {
	SPI_CLOSED = 0,  /* BT host uart is closed. */
	SPI_RUNNING,     /* BT host uart is running. */
	SPI_CLOSING,     /* BT host uart is closing. */
};

static void _spi_rx_thread(void *pData)
{
	(void)pData;
	uint8_t *buf;
	struct bt_inic_spi_hdr *hdr;

	while (osif_msg_recv(spi_priv.rx_queue, &buf, BT_TIMEOUT_FOREVER)) {
		if (spi_priv.rx_req_flag != SPI_RUNNING) {
			break;
		}

		if (buf) {
			hdr = (struct bt_inic_spi_hdr *)buf;
			bt_inic_recv_from_host(hdr->type, buf + sizeof(struct bt_inic_spi_hdr), hdr->len - sizeof(struct bt_inic_spi_hdr));
			osif_mem_free(buf);
		}
	}
	spi_priv.rx_req_flag = SPI_CLOSED;
	osif_task_delete(NULL);
}

static bool _spi_rx_queue_init(void)
{
	osif_msg_queue_create(&spi_priv.rx_queue, 32, sizeof(u8 *));

	spi_priv.rx_req_flag = SPI_RUNNING;
	if (!osif_task_create(NULL, "SPI_BT_RX_THREAD", _spi_rx_thread, (void *)(&spi_priv), 1024 * 4, 7)) {
		BT_LOGE("Create SPI_RX_THREAD Err!!\n");
		return FALSE;
	}

	return TRUE;
}

static void _spi_rx_queue_deinit(void)
{
	uint8_t *buf = NULL;

	spi_priv.rx_req_flag = SPI_CLOSING;
	osif_msg_send(spi_priv.rx_queue, &buf, BT_TIMEOUT_FOREVER);

	while (spi_priv.rx_req_flag != SPI_CLOSED) {
		osif_delay(5);
	}

	osif_msg_queue_delete(spi_priv.rx_queue);
	spi_priv.rx_queue = NULL;
}

#if defined(CONFIG_WHC_INTF_SPI) && CONFIG_WHC_INTF_SPI

void inic_dev_send_from_upper(u8 *buf, u8 *buf_alloc, u16 len);
extern void (*bt_inic_spi_recv_ptr)(uint8_t *buffer, uint16_t len);
void bt_inic_spi_recv_from_host(u8 *buf, uint16_t len);
void bt_inic_spi_init(void)
{
	bt_inic_spi_recv_ptr = bt_inic_spi_recv_from_host;
	_spi_rx_queue_init();
}

void bt_inic_spi_deinit(void)
{
	_spi_rx_queue_deinit();
}

void bt_inic_send_to_host(u8 type, u8 *pbuf, u32 len)
{
	u8 *buf = NULL;
	struct bt_inic_spi_hdr *hdr;
	u16 spilen = len + sizeof(struct bt_inic_spi_hdr);

	buf = osif_mem_alloc(RAM_TYPE_DATA_ON, spilen + DEV_DMA_ALIGN);
	if (!buf) {
		return;
	}
	memset(buf, 0, spilen + DEV_DMA_ALIGN);
	hdr = (struct bt_inic_spi_hdr *)N_BYTE_ALIGMENT((u32)buf, DEV_DMA_ALIGN);

	hdr->event = INIC_BT_HOST_RX;
	hdr->len = spilen;
	hdr->type = type;

	memcpy((void *)(hdr + 1), pbuf, len);

	BT_LOGD("BT dev send type %d len %d\n", type, spilen);
	BT_DUMPA("Share TX:\r\n", buf, spilen);
	inic_dev_send_from_upper((u8 *)hdr, buf, spilen);

	return;
}

void bt_inic_spi_recv_from_host(u8 *buf, uint16_t len)
{
	uint8_t *rx;
	struct bt_inic_spi_hdr *hdr = (struct bt_inic_spi_hdr *)buf;

	if (hdr->event != INIC_BT_HOST_TX) {
		return;
	}

	if (hdr->len > len) {
		BT_LOGE("Error: SPI RX packet size is over rx buf size\r\n");
		return;
	}

	rx = osif_mem_alloc(RAM_TYPE_DATA_ON, hdr->len);
	if (rx == NULL) {
		BT_LOGE("%s, can't alloc buffer!!\n", __func__);
		return;
	}

	memcpy(rx, buf, hdr->len);

	BT_DUMPA("Share RX:\r\n", buf, hdr->len);
	osif_msg_send(spi_priv.rx_queue, &rx, BT_TIMEOUT_NONE);
}
#else /* #if defined(CONFIG_WHC_INTF_SPI) && CONFIG_WHC_INTF_SPI */

#if defined(CONFIG_AMEBASMART)
/* SPI0 */
/* both EV30EL0 and EV30EA0 support */
#define SPI0_MOSI  _PA_13
#define SPI0_MISO  _PA_14
#define SPI0_SCLK  _PA_15
#define SPI0_CS    _PA_16

/* SPI1 */
#define SPI1_MOSI  _PA_5
#define SPI1_MISO  _PA_4
#define SPI1_SCLK  _PA_3
#define SPI1_CS    _PA_2

#define CACHE_LINE_SIZE CACHE_L1_DCACHE_LINE_SIZE

#elif defined(CONFIG_AMEBALITE)
/* SPI1 */
#define SPI1_MOSI   _PB_3
#define SPI1_MISO   _PB_4
#define SPI1_SCLK   _PB_2
#define SPI1_CS     _PB_5
/* SPI0 */
#define SPI0_MOSI   _PA_29
#define SPI0_MISO   _PA_30
#define SPI0_SCLK   _PA_28
#define SPI0_CS     _PA_31

#elif defined(CONFIG_AMEBADPLUS)
/* SPI1 FID=8 & Fully PG */
#define SPI1_MOSI   _PB_19
#define SPI1_MISO   _PB_20
#define SPI1_SCLK   _PB_18
#define SPI1_CS     _PB_21

/* SPI0 FID=8 */
#define SPI0_MOSI   _PB_24
#define SPI0_MISO   _PB_25
#define SPI0_SCLK   _PB_23
#define SPI0_CS     _PB_26
#endif

#define DEV_READY_PIN               _PB_9
#define DEV_READY                   1
#define DEV_BUSY                    0

#define RX_REQ_PIN                  _PB_8
#define DEV_RX_REQ                  1
#define DEV_RX_IDLE                 0

#define INIC_SPI_DEV                SPI0_DEV

#define INIC_SPI_CLK_MHZ            20
#define INIC_RECOVER_TIM_IDX        0
#define INIC_RECOVER_TO_US          1000

#define SIZE_RX_DESC    0
#define SIZE_TX_DESC    0

#define DEV_STS_IDLE                0
#define DEV_STS_SPI_CS_LOW          BIT(0)
#define DEV_STS_WAIT_RXDMA_DONE     BIT(1)
#define DEV_STS_WAIT_TXDMA_DONE     BIT(2)

#define HCI_MAXIMUM_BLE_PACKET_SIZE     256

#define SPI_DMA_ALIGN(x)    ((((x-1)>>5)+1)<<5) /* alignement to 32 */
#define BT_INIC_SPI_BUFSZ       SPI_DMA_ALIGN(HCI_MAXIMUM_BLE_PACKET_SIZE + HCI_SPI_PKT_HDR_LEN)

int inic_spi_set_dev_status(struct bt_inic_priv *inic_spi_priv, u32 ops, u32 sts);
bool inic_spi_txdma_init(u32 Index, PGDMA_InitTypeDef GDMA_InitStruct, IRQ_FUN CallbackFunc, u32 Length);
static inline void set_dev_rdy_pin(u8 status)
{
	GPIO_WriteBit(DEV_READY_PIN, status);
}

static inline void set_dev_rxreq_pin(u8 status)
{
	GPIO_WriteBit(RX_REQ_PIN, status);
}

void inic_spi_dma_tx_done_cb(void *param)
{
	struct bt_inic_priv *spi_priv = (struct bt_inic_priv *) param;
	u8 *tx_buf = spi_priv->tx_buf;
	GDMA_InitTypeDef *GDMA_InitStruct = &spi_priv->SSITxGdmaInitStruct;

	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, DISABLE);
	osif_mem_aligned_free(tx_buf);
	spi_priv->tx_buf = NULL;
}

int inic_spi_dma_rx_done_cb(struct bt_inic_priv *inic_spi_priv)
{
	GDMA_InitTypeDef *GDMA_InitStruct = &inic_spi_priv->SSIRxGdmaInitStruct;
	struct bt_inic_spi_hdr *hdr = (struct bt_inic_spi_hdr *)inic_spi_priv->rx_buf;
	u8 *rx = NULL;

	/* disable gdma channel */
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, DISABLE);
	DCache_Invalidate((u32)inic_spi_priv->rx_buf, BT_INIC_SPI_BUFSZ);

	if (hdr->event != INIC_BT_HOST_TX) {
		goto exit;
	}

	if (hdr->len > BT_INIC_SPI_BUFSZ) {
		BT_LOGE("Error: SPI RX packet length %d\r\n", hdr->len);
		goto exit;
	}

	rx = osif_mem_alloc(RAM_TYPE_DATA_ON, hdr->len);
	if (rx == NULL) {
		BT_LOGE("%s, Alloc fail!!\n", __func__);
		goto exit;
	}

	memcpy(rx, inic_spi_priv->rx_buf, hdr->len);
	/* BT_DUMPA("RX:\r\n", rx + sizeof(struct bt_inic_spi_hdr), hdr->len - sizeof(struct bt_inic_spi_hdr)); */
	osif_msg_send(spi_priv.rx_queue, &rx, BT_TIMEOUT_NONE);

exit:
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);
	return RTK_SUCCESS;
}

u32 inic_spi_rxdma_irq_handler(void *pData)
{
	GDMA_InitTypeDef *GDMA_InitStruct;
	u32 int_status;

	(void)pData;

	GDMA_InitStruct = &spi_priv.SSIRxGdmaInitStruct;

	/* check and clear RX DMA ISR */
	int_status = GDMA_ClearINT(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum);
	if ((int_status & (TransferType | BlockType))) {
		if (spi_priv.dev_status & DEV_STS_WAIT_RXDMA_DONE) {
			inic_spi_dma_rx_done_cb(&spi_priv);
			inic_spi_set_dev_status(&spi_priv, DISABLE, DEV_STS_WAIT_RXDMA_DONE);
		}
	}

	if (int_status & ErrType) {
		BT_LOGE("spi rxdma err occurs!!\n");
	}

	return 0;
}
/*TX DMA Interrupt handler*/
u32 inic_spi_txdma_irq_handler(void *pData)
{
	GDMA_InitTypeDef *GDMA_InitStruct;
	u32 int_status;

	(void)pData;

	GDMA_InitStruct = &spi_priv.SSITxGdmaInitStruct;

	/* check and clear TX DMA ISR */
	int_status = GDMA_ClearINT(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum);
	if (int_status & (TransferType | BlockType)) {
		if (spi_priv.dev_status & DEV_STS_WAIT_TXDMA_DONE) {
			inic_spi_dma_tx_done_cb(&spi_priv);
			inic_spi_set_dev_status(&spi_priv, DISABLE, DEV_STS_WAIT_TXDMA_DONE);
		}
	}

	if (int_status & ErrType) {
		BT_LOGE("spi txdma err occurs!!\n");
	}

	return 0;
}

int inic_spi_set_dev_status(struct bt_inic_priv *inic_spi_priv, u32 ops, u32 sts)
{
	do {
		if (!osif_mutex_take(inic_spi_priv->dev_sts_lock, BT_TIMEOUT_FOREVER)) {
			return RTK_FAIL;
		}

		if ((ops == DISABLE) && (inic_spi_priv->dev_status & sts)) {

			/* Clear status if exists */
			inic_spi_priv->dev_status &= ~sts;

			/* check if idle */
			if (inic_spi_priv->dev_status == DEV_STS_IDLE) {

				/* disable spi recover timer */
				RTIM_Cmd(TIMx[INIC_RECOVER_TIM_IDX], DISABLE);

				/* re-enable SPI RXF interrupt */
				SSI_INTConfig(INIC_SPI_DEV, SPI_BIT_RXFIM, ENABLE);
				inic_spi_priv->rx_req = FALSE;

				if (!inic_spi_priv->wait_tx) {
					/* set DEV_RDY pin to idle */
					set_dev_rdy_pin(DEV_READY);
				}

				osif_sem_give(inic_spi_priv->spi_transfer_done_sema);
			}

		} else if (ops == ENABLE) {
			/* Set status */
			set_dev_rdy_pin(DEV_BUSY);

			if (inic_spi_priv->rx_req) {
				/* Host initiate rx_req, set RX_REQ pin to idle */
				set_dev_rxreq_pin(DEV_RX_IDLE);
			}
			inic_spi_priv->dev_status = sts;

			/* enable spi recover timer */
			RTIM_Reset(TIMx[INIC_RECOVER_TIM_IDX]);
			RTIM_Cmd(TIMx[INIC_RECOVER_TIM_IDX], ENABLE);
		}

		osif_mutex_give(inic_spi_priv->dev_sts_lock);

		if (inic_spi_priv->ssris_pending) {
			inic_spi_priv->ssris_pending = FALSE;

			ops = DISABLE;
			sts = DEV_STS_SPI_CS_LOW;
		} else if (inic_spi_priv->set_devsts_pending) {
			inic_spi_priv->set_devsts_pending = FALSE;

			ops = ENABLE;
			sts = DEV_STS_SPI_CS_LOW | DEV_STS_WAIT_RXDMA_DONE;
			if (inic_spi_priv->rx_req) {
				sts |= DEV_STS_WAIT_TXDMA_DONE;
			}
		} else {
			break;
		}
	} while (1);

	return RTK_SUCCESS;
}

u32 inic_spi_recover(void *Data)
{
	struct bt_inic_priv *inic_spi_priv = (struct bt_inic_priv *) Data;

	RTIM_INTClear(TIM0);

	/* check if error occurs or SPI transfer is still ongoing */
	if (SSI_Busy(INIC_SPI_DEV)) {
		return 0;
	}

	if (inic_spi_priv->dev_status & DEV_STS_SPI_CS_LOW) {
		BT_LOGD("SSR interrupt lost\n");

		if (inic_spi_set_dev_status(inic_spi_priv, DISABLE, DEV_STS_SPI_CS_LOW) == RTK_FAIL) {
			inic_spi_priv->ssris_pending = TRUE;
		}
	} else if (inic_spi_priv->dev_status & DEV_STS_WAIT_TXDMA_DONE) {
		BT_LOGD("TXDMA not done\n");
		inic_spi_dma_tx_done_cb(inic_spi_priv);
		inic_spi_set_dev_status(inic_spi_priv, DISABLE, DEV_STS_WAIT_TXDMA_DONE);
	} else {
		/* disable spi recover timer */
		RTIM_Cmd(TIMx[INIC_RECOVER_TIM_IDX], DISABLE);
	}

	return 0;
}

u32 inic_spi_interrupt_handler(void *param)
{
	struct bt_inic_priv *inic_spi_priv = (struct bt_inic_priv *)param;
	u32 interrupt_status = SSI_GetIsr(INIC_SPI_DEV);
	u32 status;

	SSI_SetIsrClean(INIC_SPI_DEV, interrupt_status);

	if (interrupt_status & SPI_BIT_RXFIS) {
		SSI_INTConfig(INIC_SPI_DEV, SPI_BIT_RXFIM, DISABLE);

		status = DEV_STS_SPI_CS_LOW | DEV_STS_WAIT_RXDMA_DONE;
		if (inic_spi_priv->rx_req) {
			status |= DEV_STS_WAIT_TXDMA_DONE;
		}

		if (inic_spi_set_dev_status(inic_spi_priv, ENABLE, status) == RTK_FAIL) {
			inic_spi_priv->set_devsts_pending = TRUE;
		}
	}

	if (interrupt_status & SPI_BIT_SSRIS) {
		if (inic_spi_priv->dev_status & DEV_STS_SPI_CS_LOW) {
			if (inic_spi_set_dev_status(inic_spi_priv, DISABLE, DEV_STS_SPI_CS_LOW) == RTK_FAIL) {
				inic_spi_priv->ssris_pending = TRUE;
			}
		}
	}

	return 0;
}

void bt_inic_spi_init(void)
{
	struct bt_inic_priv *inic_spi_priv = &spi_priv;
	RTIM_TimeBaseInitTypeDef TIM_InitStruct;
	SSI_InitTypeDef SSI_InitStructSlave;
	GPIO_InitTypeDef GPIO_InitStruct;
	u8 *rx_buf = NULL;
	u32 index;

	index = (INIC_SPI_DEV == SPI0_DEV) ? 0 : 1;

	osif_mutex_create(&inic_spi_priv->dev_sts_lock);
	osif_mutex_create(&inic_spi_priv->tx_lock);
	osif_sem_create(&inic_spi_priv->spi_transfer_done_sema, 0, 1);

	/* Initialize GPIO */
	GPIO_InitStruct.GPIO_Pin = RX_REQ_PIN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(&GPIO_InitStruct);
	set_dev_rxreq_pin(DEV_RX_IDLE);

	GPIO_InitStruct.GPIO_Pin = DEV_READY_PIN;
	GPIO_Init(&GPIO_InitStruct);

#ifdef SPI_DEBUG
	GPIO_InitStruct.GPIO_Pin = _PB_20;
	GPIO_Init(&GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = _PB_6;
	GPIO_Init(&GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = _PA_24;
	GPIO_Init(&GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = _PA_25;
	GPIO_Init(&GPIO_InitStruct);
#endif

	/* Initialize Timer*/
	RCC_PeriphClockCmd(APBPeriph_LTIM0, APBPeriph_LTIM0_CLOCK, ENABLE);
	RTIM_TimeBaseStructInit(&TIM_InitStruct);
	TIM_InitStruct.TIM_Idx = INIC_RECOVER_TIM_IDX;
	TIM_InitStruct.TIM_Period = INIC_RECOVER_TO_US / (1000000 / 32768);

	RTIM_TimeBaseInit(TIMx[INIC_RECOVER_TIM_IDX], &TIM_InitStruct, TIMx_irq[INIC_RECOVER_TIM_IDX], (IRQ_FUN)inic_spi_recover, (u32)inic_spi_priv);
	RTIM_INTConfig(TIMx[INIC_RECOVER_TIM_IDX], TIM_IT_Update, ENABLE);

	/* Initialize SPI */
	RCC_PeriphClockCmd(APBPeriph_SPI0, APBPeriph_SPI0_CLOCK, ENABLE);
	Pinmux_Config(SPI0_MOSI, PINMUX_FUNCTION_SPI); /* MOSI */
	Pinmux_Config(SPI0_MISO, PINMUX_FUNCTION_SPI); /* MISO */
	Pinmux_Config(SPI0_SCLK, PINMUX_FUNCTION_SPI); /* SCLK */
	Pinmux_Config(SPI0_CS, PINMUX_FUNCTION_SPI); /* CS */
	PAD_PullCtrl(SPI0_CS, GPIO_PuPd_UP); /* CS Default Active low */
	PAD_PullCtrl((u32)SPI0_SCLK, GPIO_PuPd_DOWN);

	SSI_SetRole(INIC_SPI_DEV, SSI_SLAVE);
	SSI_StructInit(&SSI_InitStructSlave);
	SSI_InitStructSlave.SPI_SclkPhase = SCPH_TOGGLES_IN_MIDDLE;
	SSI_InitStructSlave.SPI_SclkPolarity = SCPOL_INACTIVE_IS_LOW;
	SSI_InitStructSlave.SPI_DataFrameSize = DFS_8_BITS;
	SSI_InitStructSlave.SPI_Role = SSI_SLAVE;
	SSI_Init(INIC_SPI_DEV, &SSI_InitStructSlave);

	InterruptRegister((IRQ_FUN)inic_spi_interrupt_handler, SPI0_IRQ, (u32)inic_spi_priv, INT_PRI_MIDDLE);
	InterruptEn(SPI0_IRQ, INT_PRI_MIDDLE);

	/* Enable RX full interrupt */
	SSI_INTConfig(INIC_SPI_DEV, SPI_BIT_RXFIM | SPI_BIT_SSRIM, ENABLE);

	/* Configure DMA and buffer */
	rx_buf = (u8 *)osif_mem_alloc(RAM_TYPE_DATA_ON, BT_INIC_SPI_BUFSZ);
	if (rx_buf == NULL) {
		BT_LOGE("%s: alloc RX buf fail!\n", __func__);
		return;
	}
	inic_spi_priv->rx_buf = (void *)rx_buf;

	/* Configure RX DMA */
	SSI_SetDmaEnable(INIC_SPI_DEV, ENABLE, SPI_BIT_RDMAE);
	SSI_RXGDMA_Init(index, &inic_spi_priv->SSIRxGdmaInitStruct, NULL, inic_spi_rxdma_irq_handler, rx_buf, BT_INIC_SPI_BUFSZ);

	SSI_SetDmaEnable(INIC_SPI_DEV, ENABLE, SPI_BIT_TDMAE);
	inic_spi_txdma_init(index, &spi_priv.SSITxGdmaInitStruct, inic_spi_txdma_irq_handler, BT_INIC_SPI_BUFSZ);

	if (_spi_rx_queue_init() != TRUE) {
		return;
	}

	/* Device ready */
	inic_spi_priv->dev_status = DEV_STS_IDLE;
	set_dev_rdy_pin(DEV_READY);

	BT_LOGA("SPI init done!\n");
}

/* porting from SSI_TXGDMA_Init, just init, not enable */
bool inic_spi_txdma_init(u32 Index, PGDMA_InitTypeDef GDMA_InitStruct, IRQ_FUN CallbackFunc, u32 Length)
{
	SPI_TypeDef *SPIx = SPI_DEV_TABLE[Index].SPIx;
	u32 DataFrameSize = SSI_GetDataFrameSize(SPIx);
	u8 GdmaChnl;

	if (Length & 0x3) {
		BT_LOGE("%s: length %d is not aligned.", Length);
		return FALSE;
	}

	GdmaChnl = GDMA_ChnlAlloc(0, CallbackFunc, NULL, INT_PRI_MIDDLE);
	if (GdmaChnl == 0xFF) {
		return FALSE;
	}

	GDMA_StructInit(GDMA_InitStruct);
	GDMA_InitStruct->GDMA_DIR     = TTFCMemToPeri;
	GDMA_InitStruct->GDMA_DstHandshakeInterface = SPI_DEV_TABLE[Index].Tx_HandshakeInterface;
	GDMA_InitStruct->GDMA_DstAddr = (u32)&SPI_DEV_TABLE[Index].SPIx->SPI_DRx;
	GDMA_InitStruct->GDMA_Index   = 0;
	GDMA_InitStruct->GDMA_ChNum   = GdmaChnl;
	GDMA_InitStruct->GDMA_IsrType = (BlockType | TransferType | ErrType);

	GDMA_InitStruct->GDMA_DstInc = NoChange;
	GDMA_InitStruct->GDMA_SrcInc = IncType;

	/*  Cofigure GDMA transfer */
	if (DataFrameSize > 8) {
		/*  16~9 bits mode */
		/*  4-bytes aligned, move 4 bytes each transfer */
		GDMA_InitStruct->GDMA_SrcMsize = MsizeFour;
		GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthFourBytes;
		GDMA_InitStruct->GDMA_DstMsize = MsizeEight;
		GDMA_InitStruct->GDMA_DstDataWidth = TrWidthTwoBytes;
		GDMA_InitStruct->GDMA_BlockSize = Length >> 2;
	} else {
		/*  8~4 bits mode */
		/*  4-bytes aligned, move 4 bytes each transfer */
		GDMA_InitStruct->GDMA_SrcMsize = MsizeOne;
		GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthFourBytes;
		GDMA_InitStruct->GDMA_DstMsize = MsizeFour;
		GDMA_InitStruct->GDMA_DstDataWidth = TrWidthOneByte;
		GDMA_InitStruct->GDMA_BlockSize = Length >> 2;
	}

	GDMA_Init(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, GDMA_InitStruct);

	return TRUE;
}

void inic_dev_wait_dev_idle(void)
{
	/* Wait for last SPI transaction done, including stages:
	    1) trigger RX_REQ to host, wait for host to initiate SPI transfer (spi_priv.rx_req=TRUE)
	    2) host initiates SPI transfer ~ device respond to RXF interrupt (SSI_Busy)
	    3) device respond to RXF interrupt ~ device TRXDMA done (spi_priv.dev_status != DEV_STS_IDLE)*/
	while (spi_priv.rx_req || spi_priv.dev_status != DEV_STS_IDLE || SSI_Busy(INIC_SPI_DEV)) {
		spi_priv.wait_tx = TRUE;
		osif_sem_take(spi_priv.spi_transfer_done_sema, BT_TIMEOUT_FOREVER);
	}

	spi_priv.wait_tx = FALSE;
}

void inic_dev_send(u8 *tx_buf)
{
	GDMA_InitTypeDef *GDMA_InitStruct = &spi_priv.SSITxGdmaInitStruct;

	/* Call this function when receive pkt, call API or send API return value. Use locks to ensure exclusive execution. */
	osif_mutex_take(spi_priv.tx_lock, BT_TIMEOUT_FOREVER);

	DCache_CleanInvalidate((u32)tx_buf, BT_INIC_SPI_BUFSZ);

retry:
	/* check dev is idle */
	inic_dev_wait_dev_idle();

	spi_priv.tx_buf = tx_buf;
	GDMA_SetSrcAddr(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, (u32)tx_buf);

	/* protected by critical section to prevent interrupted by INTERRUPTS*/
	u32 flags = osif_lock();

	/* RXF interrupt would occur after inic_dev_wait_dev_idle(). This case would increase time, during which Host would start SPI transfer.
	 So double check SPI is not busy, then start TXDMA */
	if (SSI_Busy(INIC_SPI_DEV)) {
		osif_unlock(flags);
		goto retry;
	}

	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);

	spi_priv.rx_req = TRUE;

	/* Send rx request signal to host */
	set_dev_rxreq_pin(DEV_RX_REQ);
	set_dev_rdy_pin(DEV_READY);

	osif_unlock(flags);

	osif_mutex_give(spi_priv.tx_lock);
	return;
}

void bt_inic_send_to_host(u8 type, u8 *pbuf, u32 len)
{
	struct bt_inic_spi_hdr *hdr;
	u16 spilen = len + sizeof(struct bt_inic_spi_hdr);
	u8 *spi_txdata = osif_mem_aligned_alloc(RAM_TYPE_DATA_ON, spilen, DEV_DMA_ALIGN); /* DMA need 4 byte aligned. */

	if (!(spi_txdata)) {
		BT_LOGE("ERROR NULL\n");
		return;
	}
	memset(spi_txdata, 0, spilen);
	hdr = (struct bt_inic_spi_hdr *)spi_txdata;
	hdr->event = INIC_BT_HOST_RX;
	hdr->len = spilen;
	hdr->type = type;

	memcpy(spi_txdata + sizeof(struct bt_inic_spi_hdr), pbuf, len); /* BLE Packet data */
	/* BT_DUMPA("SPI TX:\r\n", spi_txdata, spilen); */
	inic_dev_send(spi_txdata);
	return;
}

void bt_inic_spi_deinit(void)
{
	InterruptDis(SPI0_IRQ);
	InterruptUnRegister(SPI0_IRQ);

	osif_mutex_delete(spi_priv.dev_sts_lock);
	spi_priv.dev_sts_lock = NULL;
	osif_mutex_delete(spi_priv.tx_lock);
	spi_priv.tx_lock = NULL;
	osif_sem_delete(spi_priv.spi_transfer_done_sema);
	spi_priv.spi_transfer_done_sema = NULL;

	if (spi_priv.rx_buf) {
		osif_mem_free(spi_priv.rx_buf);
		spi_priv.rx_buf = NULL;
	}

	_spi_rx_queue_deinit();

	PGDMA_InitTypeDef GDMA_Rx = &(spi_priv.SSIRxGdmaInitStruct);

	/* Set SSI Rx DMA Disable */
	/* SSI_SetDmaEnable(INIC_SPI_DEV, DISABLE, SPI_BIT_RDMAE); */

	/* Clear Pending ISR */
	GDMA_ClearINT(GDMA_Rx->GDMA_Index, GDMA_Rx->GDMA_ChNum);
	/* GDMA_ChCleanAutoReload(GDMA_Rx->GDMA_Index, GDMA_Rx->GDMA_ChNum, CLEAN_RELOAD_SRC_DST); */
	/* GDMA_Cmd(GDMA_Rx->GDMA_Index, GDMA_Rx->GDMA_ChNum, DISABLE); */
	GDMA_Abort(GDMA_Rx->GDMA_Index, GDMA_Rx->GDMA_ChNum);
	GDMA_ChnlFree(GDMA_Rx->GDMA_Index, GDMA_Rx->GDMA_ChNum);

	PGDMA_InitTypeDef GDMA_Tx = &(spi_priv.SSITxGdmaInitStruct);

	/* Set SSI Tx DMA Disable */
	/* SSI_SetDmaEnable(INIC_SPI_DEV, DISABLE, SPI_BIT_TDMAE); */

	/* Clear Pending ISR */
	GDMA_ClearINT(GDMA_Tx->GDMA_Index, GDMA_Tx->GDMA_ChNum);
	/* GDMA_ChCleanAutoReload(GDMA_Tx->GDMA_Index, GDMA_Tx->GDMA_ChNum, CLEAN_RELOAD_SRC_DST); */
	/* GDMA_Cmd(GDMA_Tx->GDMA_Index, GDMA_Tx->GDMA_ChNum, DISABLE); */
	GDMA_Abort(GDMA_Tx->GDMA_Index, GDMA_Tx->GDMA_ChNum);
	GDMA_ChnlFree(GDMA_Tx->GDMA_Index, GDMA_Tx->GDMA_ChNum);
	/* SSI_Cmd(INIC_SPI_DEV, DISABLE); */
}

#endif /* #if defined(CONFIG_WHC_INTF_SPI) && CONFIG_WHC_INTF_SPI */