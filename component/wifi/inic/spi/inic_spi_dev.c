#include "inic_dev.h"

struct inic_spi_priv_t spi_priv = {0};

void inic_spi_dma_tx_done_cb(void *param)
{
	struct inic_spi_priv_t *spi_priv = (struct inic_spi_priv_t *) param;
	struct inic_txbuf_info_t *inic_tx = container_of(spi_priv->txbuf_info, struct inic_txbuf_info_t, txbuf_info);
	GDMA_InitTypeDef *GDMA_InitStruct = &spi_priv->SSITxGdmaInitStruct;

	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, DISABLE);

	/* Dev TX complete, free tx skb or buffer */
	if (inic_tx->is_skb) {
		dev_kfree_skb_any((struct sk_buff *) inic_tx->ptr);
	} else {
		rtos_mem_free((u8 *)inic_tx->ptr);
	}
	rtos_mem_free((u8 *)inic_tx);

	spi_priv->txbuf_info = NULL;
}

int inic_spi_dma_rx_done_cb(void *param)
{
	struct inic_spi_priv_t *spi_priv = (struct inic_spi_priv_t *) param;
	GDMA_InitTypeDef *GDMA_InitStruct = &spi_priv->SSIRxGdmaInitStruct;
	struct sk_buff *new_skb = NULL, *rx_pkt = spi_priv->rx_skb;
	struct inic_msg_info *msg_info;
	u8 *buf = NULL;
	u32 event;

	/* disable gdma channel */
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, DISABLE);

	DCache_Invalidate((u32)rx_pkt->data, SPI_BUFSZ);

	event = *(u32 *)(rx_pkt->data);

	/* receives XMIT_PKTS */
	if (event == INIC_WIFI_EVT_XIMT_PKTS) {
retry:
		/* alloc new skb, blocked if no skb */
		if (((skbpriv.skb_buff_num - skbpriv.skb_buff_used) < 3) ||
			((new_skb = dev_alloc_skb(SPI_BUFSZ, SPI_SKB_RSVD_LEN)) == NULL)) {
			spi_priv->wait_for_txbuf = TRUE;

			/* wait timeout to re-check skb, considering corner cases for wait_for_txbuf update */
			rtos_sema_take(spi_priv->free_skb_sema, 5);
			goto retry;
		} else {
			spi_priv->wait_for_txbuf = FALSE;
			spi_priv->rx_skb = new_skb;
		}

		/* process rx data */
		msg_info = (struct inic_msg_info *) rx_pkt->data;
		skb_reserve(rx_pkt, sizeof(struct inic_msg_info));
		skb_put(rx_pkt, msg_info->data_len);

		inic_dev_event_int_hdl((u8 *)msg_info, rx_pkt);

		/* set new dest addr for RXDMA */
		DCache_Invalidate((u32)new_skb->data, SPI_BUFSZ);
		GDMA_SetDstAddr(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, (u32)new_skb->data);

	} else if (event == INIC_WIFI_EVT_API_CALL || event == INIC_WIFI_EVT_API_RETURN) {
		/* receives EVENTS */
		buf = rtos_mem_zmalloc(SPI_BUFSZ);	//TODO: optimize
		if (buf == NULL) {
			RTK_LOGS_LVL(TAG_WLAN_INIC, RTK_LOG_ERROR, "%s, can't alloc buffer!!\n", __func__);
			goto exit;
		}

		memcpy(buf, spi_priv->rx_skb->data, SPI_BUFSZ);

		/* free buf later, no need to modify skb. */
		inic_dev_event_int_hdl(buf, NULL);

	} else {
		//RTK_LOGD(TAG_WLAN_INIC, "RX dummy data, no process\n");
	}

exit:
	/* restart RX DMA */
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);

	return SUCCESS;
}

u32 inic_spi_rxdma_irq_handler(void *pData)
{
	GDMA_InitTypeDef *GDMA_InitStruct;
	u32 int_status;

	(void)pData;

	GDMA_InitStruct = &spi_priv.SSIRxGdmaInitStruct;

	/* check and clear RX DMA ISR */
	int_status = GDMA_ClearINT(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum);
	if ((int_status & (TransferType | BlockType)))  {
		if (spi_priv.dev_status & DEV_STS_WAIT_RXDMA_DONE) {
			rtos_sema_give(spi_priv.rxirq_sema);
		}
	}

	if (int_status & ErrType) {
		RTK_LOGS_LVL(TAG_WLAN_INIC, RTK_LOG_ERROR, "spi rxdma err occurs!!\n");
	}

	return 0;
}

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
			rtos_sema_give(spi_priv.txirq_sema);
		}
	}

	if (int_status & ErrType) {
		RTK_LOGS_LVL(TAG_WLAN_INIC, RTK_LOG_ERROR, "spi txdma err occurs!!\n");
	}

	return 0;
}

int inic_spi_set_dev_status(struct inic_spi_priv_t *inic_spi_priv, u32 ops, u32 sts)
{
	do {
		if (rtos_mutex_take(inic_spi_priv->dev_sts_lock, MUTEX_WAIT_TIMEOUT) == FAIL) {
			return FAIL;
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

				rtos_sema_give(inic_spi_priv->spi_transfer_done_sema);
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

		rtos_mutex_give(inic_spi_priv->dev_sts_lock);

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

	return SUCCESS;
}

u32 inic_spi_recover(void *Data)
{
	struct inic_spi_priv_t *inic_spi_priv = (struct inic_spi_priv_t *) Data;

	RTIM_INTClear(TIM0);

	/* check if error occurs or SPI transfer is still ongoing */
	if (SSI_Busy(INIC_SPI_DEV)) {
		return 0;
	}

	if (inic_spi_priv->dev_status & DEV_STS_SPI_CS_LOW) {
		RTK_LOGD(TAG_WLAN_INIC, "SSR interrupt lost\n");

		if (inic_spi_set_dev_status(inic_spi_priv, DISABLE, DEV_STS_SPI_CS_LOW) == FAIL) {
			inic_spi_priv->ssris_pending = TRUE;
		}
	} else if (inic_spi_priv->dev_status & DEV_STS_WAIT_TXDMA_DONE) {
		RTK_LOGD(TAG_WLAN_INIC, "TXDMA not done\n");
		rtos_sema_give(inic_spi_priv->txirq_sema);
	} else {
		/* disable spi recover timer */
		RTIM_Cmd(TIMx[INIC_RECOVER_TIM_IDX], DISABLE);
	}

	return 0;
}

void inic_spi_txdma_irq_task(void *pData)
{
	struct inic_spi_priv_t *inic_spi_priv = pData;

	for (;;) {
		/* Task blocked and wait the semaphore(events) here */
		rtos_sema_take(inic_spi_priv->txirq_sema, RTOS_MAX_TIMEOUT);

		if (inic_spi_priv->dev_status & DEV_STS_WAIT_TXDMA_DONE) {
			inic_spi_dma_tx_done_cb(inic_spi_priv);
			inic_spi_set_dev_status(inic_spi_priv, DISABLE, DEV_STS_WAIT_TXDMA_DONE);
		}
	}
}

void inic_spi_rxdma_irq_task(void *pData)
{
	struct inic_spi_priv_t *inic_spi_priv = pData;

	for (;;) {
		/* Task blocked and wait the semaphore(events) here */
		rtos_sema_take(inic_spi_priv->rxirq_sema, RTOS_MAX_TIMEOUT);

		if (inic_spi_priv->dev_status & DEV_STS_WAIT_RXDMA_DONE) {
			inic_spi_dma_rx_done_cb(inic_spi_priv);
			inic_spi_set_dev_status(inic_spi_priv, DISABLE, DEV_STS_WAIT_RXDMA_DONE);
		}
	}
}

u32 inic_spi_interrupt_handler(void *param)
{
	struct inic_spi_priv_t *inic_spi_priv = (struct inic_spi_priv_t *)param;
	u32 interrupt_status = SSI_GetIsr(INIC_SPI_DEV);
	u32 status;

	SSI_SetIsrClean(INIC_SPI_DEV, interrupt_status);

	if (interrupt_status & SPI_BIT_RXFIS) {
		SSI_INTConfig(INIC_SPI_DEV, SPI_BIT_RXFIM, DISABLE);

		status = DEV_STS_SPI_CS_LOW | DEV_STS_WAIT_RXDMA_DONE;
		if (inic_spi_priv->rx_req) {
			status |= DEV_STS_WAIT_TXDMA_DONE;
		}

		if (inic_spi_set_dev_status(inic_spi_priv, ENABLE, status) == FAIL) {
			inic_spi_priv->set_devsts_pending = TRUE;
		}
	}

	if (interrupt_status & SPI_BIT_SSRIS) {
		if (inic_spi_priv->dev_status & DEV_STS_SPI_CS_LOW) {
			if (inic_spi_set_dev_status(inic_spi_priv, DISABLE, DEV_STS_SPI_CS_LOW) == FAIL) {
				inic_spi_priv->ssris_pending = TRUE;
			}
		}
	}

	return 0;
}

static void inic_spi_init(void)
{
	struct inic_spi_priv_t *inic_spi_priv = &spi_priv;
	RTIM_TimeBaseInitTypeDef TIM_InitStruct;
	SSI_InitTypeDef SSI_InitStructSlave;
	GPIO_InitTypeDef GPIO_InitStruct;
	struct sk_buff *skb = NULL;
	u32 index;

	index = (INIC_SPI_DEV == SPI0_DEV) ? 0 : 1;

	rtos_mutex_create_static(&inic_spi_priv->dev_sts_lock);
	rtos_mutex_create_static(&inic_spi_priv->tx_lock);
	rtos_sema_create(&inic_spi_priv->txirq_sema, 0, RTOS_SEMA_MAX_COUNT);
	rtos_sema_create(&inic_spi_priv->rxirq_sema, 0, RTOS_SEMA_MAX_COUNT);
	rtos_sema_create(&inic_spi_priv->spi_transfer_done_sema, 0, RTOS_SEMA_MAX_COUNT);
	rtos_sema_create(&inic_spi_priv->free_skb_sema, 0, RTOS_SEMA_MAX_COUNT);

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
	Pinmux_Config(_PB_24, PINMUX_FUNCTION_SPI);//MOSI
	Pinmux_Config(_PB_25, PINMUX_FUNCTION_SPI);//MISO
	Pinmux_Config(_PB_23, PINMUX_FUNCTION_SPI);//CLK
	Pinmux_Config(_PB_26, PINMUX_FUNCTION_SPI);//CS
	PAD_PullCtrl(_PB_26, GPIO_PuPd_UP);
	PAD_PullCtrl(_PB_23, GPIO_PuPd_DOWN);

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
	inic_spi_priv->txdma_initialized = 0;
	inic_spi_priv->txbuf_info = NULL;

	skb = dev_alloc_skb(SPI_BUFSZ, SPI_SKB_RSVD_LEN);
	if (skb == NULL || (((u32)skb->data) & 0x3) != 0) {
		RTK_LOGE(TAG_WLAN_INIC, "%s: alloc skb fail!\n", __func__);
		return;
	}
	inic_spi_priv->rx_skb = skb;

	/* Configure RX DMA */
	DCache_Invalidate((u32)skb->data, SPI_BUFSZ);
	SSI_RXGDMA_Init(index, &inic_spi_priv->SSIRxGdmaInitStruct, (void *)INIC_SPI_RXDMA, inic_spi_rxdma_irq_handler, skb->data, SPI_BUFSZ);
	SSI_SetDmaEnable(INIC_SPI_DEV, ENABLE, SPI_BIT_RDMAE);

	/* Create irq task */
	if (rtos_task_create(NULL, "SPI_RXDMA_IRQ_TASK", inic_spi_rxdma_irq_task, (void *)inic_spi_priv, 1024 * 4, 7) != SUCCESS) {
		RTK_LOGE(TAG_WLAN_INIC, "Create SPI_RXDMA_IRQ_TASK Err!!\n");
		return;
	}

	if (rtos_task_create(NULL, "SPI_TXDMA_IRQ_TASK", inic_spi_txdma_irq_task, (void *)inic_spi_priv, 1024 * 4, 7) != SUCCESS) {
		RTK_LOGE(TAG_WLAN_INIC, "Create SPI_TXDMA_IRQ_TASK Err!!\n");
		return;
	}

	/* Device ready */
	inic_spi_priv->dev_status = DEV_STS_IDLE;
	set_dev_rdy_pin(DEV_READY);

	RTK_LOGI(TAG_WLAN_INIC, "SPI device init done!\n");
}

bool inic_spi_txdma_init(
	u32 Index,
	PGDMA_InitTypeDef GDMA_InitStruct,
	void *CallbackData,
	IRQ_FUN CallbackFunc,
	u8 *pTxData,
	u32 Length
)
{
	SPI_TypeDef *SPIx = SPI_DEV_TABLE[Index].SPIx;
	u32 DataFrameSize = SSI_GetDataFrameSize(SPIx);
	u8 GdmaChnl;

	assert_param(GDMA_InitStruct != NULL);

	DCache_CleanInvalidate((u32) pTxData, Length);

	GdmaChnl = GDMA_ChnlAlloc(0, CallbackFunc, (u32)CallbackData, INT_PRI_MIDDLE);
	if (GdmaChnl == 0xFF) {
		return _FALSE;
	}

	GDMA_StructInit(GDMA_InitStruct);
	GDMA_InitStruct->GDMA_DIR     = TTFCMemToPeri;
	GDMA_InitStruct->GDMA_DstHandshakeInterface = SPI_DEV_TABLE[Index].Tx_HandshakeInterface;
	GDMA_InitStruct->GDMA_DstAddr = (u32)&SPI_DEV_TABLE[Index].SPIx->SPI_DRx;
	GDMA_InitStruct->GDMA_Index   = 0;
	GDMA_InitStruct->GDMA_ChNum   = GdmaChnl;
	GDMA_InitStruct->GDMA_IsrType = (BlockType | TransferType | ErrType);

	GDMA_InitStruct->GDMA_SrcMsize = MsizeOne;
	GDMA_InitStruct->GDMA_DstMsize = MsizeFour;
	GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthFourBytes;
	GDMA_InitStruct->GDMA_DstDataWidth = TrWidthOneByte;
	GDMA_InitStruct->GDMA_DstInc = NoChange;
	GDMA_InitStruct->GDMA_SrcInc = IncType;

	/*  Cofigure GDMA transfer */
	if (DataFrameSize > 8) {
		/*  16~9 bits mode */
		if (((Length & 0x03) == 0) && (((u32)(pTxData) & 0x03) == 0)) {
			/*  4-bytes aligned, move 4 bytes each transfer */
			GDMA_InitStruct->GDMA_SrcMsize = MsizeFour;
			GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthFourBytes;
			GDMA_InitStruct->GDMA_DstMsize = MsizeEight;
			GDMA_InitStruct->GDMA_DstDataWidth = TrWidthTwoBytes;
			GDMA_InitStruct->GDMA_BlockSize = Length >> 2;
		} else if (((Length & 0x01) == 0) && (((u32)(pTxData) & 0x01) == 0)) {
			/*  2-bytes aligned, move 2 bytes each transfer */
			GDMA_InitStruct->GDMA_SrcMsize = MsizeEight;
			GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthTwoBytes;
			GDMA_InitStruct->GDMA_DstMsize = MsizeEight;
			GDMA_InitStruct->GDMA_DstDataWidth = TrWidthTwoBytes;
			GDMA_InitStruct->GDMA_BlockSize = Length >> 1;
		} else {
			RTK_LOGE(TAG_WLAN_INIC, "SSI_TXGDMA_Init: Aligment Err: pTxData=%p,  Length=%lu\n", pTxData, Length);
			return _FALSE;
		}
	} else {
		/*  8~4 bits mode */
		if (((Length & 0x03) == 0) && (((u32)(pTxData) & 0x03) == 0)) {
			/*  4-bytes aligned, move 4 bytes each transfer */
			GDMA_InitStruct->GDMA_SrcMsize = MsizeOne;
			GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthFourBytes;
			GDMA_InitStruct->GDMA_BlockSize = Length >> 2;
		} else {
			GDMA_InitStruct->GDMA_SrcMsize = MsizeFour;
			GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthOneByte;
			GDMA_InitStruct->GDMA_BlockSize = Length;
		}
		GDMA_InitStruct->GDMA_DstMsize = MsizeFour;
		GDMA_InitStruct->GDMA_DstDataWidth = TrWidthOneByte;
	}

	GDMA_InitStruct->GDMA_SrcAddr = (u32)pTxData;

	/*  Enable GDMA for TX */
	GDMA_Init(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, GDMA_InitStruct);
	//GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);

	return _TRUE;
}

void inic_dev_trigger_rx_handle(void)
{
	if (spi_priv.wait_for_txbuf) {
		rtos_sema_give(spi_priv.free_skb_sema);
	}
}

u8 inic_dev_tx_path_avail(void)
{
	u32 delay_cnt = 0;
	u8 ret = FALSE;

	while (delay_cnt++ < 100) {
		/* wait for skb allocatable */
		if ((skbpriv.skb_buff_num - skbpriv.skb_buff_used) > 3) {
			ret = TRUE;
			break;
		} else {
			rtos_time_delay_ms(1);
		}
	}

	return ret;
}

/**
 * @brief  to haddle the inic message interrupt. If the message queue is
 * 	initialized, it will enqueue the message and wake up the message
 * 	task to haddle the message. If last send message cannot be done, I will
 * 	set pending for next sending message.
 * @param  rxbuf: rx data.
 * @return none.
 */
void inic_dev_event_int_hdl(u8 *rxbuf, struct sk_buff *skb)
{
	u32 event = *(u32 *)rxbuf;
	struct inic_api_info *ret_msg;

	switch (event) {
	case INIC_WIFI_EVT_API_CALL:
		event_priv.rx_api_msg = rxbuf;
		rtos_sema_give(event_priv.task_wake_sema);

		break;
	case INIC_WIFI_EVT_API_RETURN:
		if (event_priv.b_waiting_for_ret) {
			event_priv.rx_ret_msg = rxbuf;
			rtos_sema_give(event_priv.api_ret_sema);
		} else {
			ret_msg = (struct inic_api_info *)rxbuf;
			RTK_LOGS_LVL(TAG_WLAN_INIC, RTK_LOG_WARN, "too late to receive API ret, ID: 0x%x!\n", ret_msg->api_id);

			/* free rx buffer */
			rtos_mem_free((u8 *)ret_msg);
		}

		break;
	case INIC_WIFI_EVT_XIMT_PKTS:
		/* put the inic message to the queue */
		if (inic_msg_enqueue(skb, &dev_xmit_priv.xmit_queue) == FAIL) {
			break;
		}
		/* wakeup task */
		rtos_sema_give(dev_xmit_priv.xmit_sema);

		break;
	default:
		RTK_LOGS_LVL(TAG_WLAN_INIC, RTK_LOG_ERROR, "Event(%x) unknown!\n", event);
		break;
	}

}

void inic_dev_wait_dev_idle(void)
{
	/* Wait for last SPI transaction done, including stages:
		1) trigger RX_REQ to host, wait for host to initiate SPI transfer (spi_priv.rx_req=TRUE)
		2) host initiates SPI transfer ~ device respond to RXF interrupt (SSI_Busy)
		3) device respond to RXF interrupt ~ device TRXDMA done (spi_priv.dev_status != DEV_STS_IDLE)*/
	while (spi_priv.rx_req || spi_priv.dev_status != DEV_STS_IDLE || SSI_Busy(INIC_SPI_DEV)) {
		spi_priv.wait_tx = TRUE;
		rtos_sema_take(spi_priv.spi_transfer_done_sema, 0xFFFFFFFF);
	}

	spi_priv.wait_tx = FALSE;
}

void inic_dev_send(struct inic_buf_info *pbuf)
{
	GDMA_InitTypeDef *GDMA_InitStruct = &spi_priv.SSITxGdmaInitStruct;
	u32 index = (INIC_SPI_DEV == SPI0_DEV) ? 0 : 1;

	/* Call this function when receive pkt, call API or send API return value. Use locks to ensure exclusive execution. */
	rtos_mutex_take(spi_priv.tx_lock, MUTEX_WAIT_TIMEOUT);

	DCache_CleanInvalidate(pbuf->buf_addr, SPI_BUFSZ);

retry:
	/* check dev is idle */
	inic_dev_wait_dev_idle();

	/* Initialize or Restart TXDMA */
	if (!spi_priv.txdma_initialized) {
		SSI_SetDmaEnable(INIC_SPI_DEV, ENABLE, SPI_BIT_TDMAE);
		inic_spi_txdma_init(index, &spi_priv.SSITxGdmaInitStruct, INIC_SPI_TXDMA, inic_spi_txdma_irq_handler, (u8 *)(pbuf->buf_addr), SPI_BUFSZ);
		spi_priv.txdma_initialized = 1;
	} else {
		GDMA_SetSrcAddr(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, pbuf->buf_addr);
	}
	spi_priv.txbuf_info = pbuf;

	/* protected by critical section to prevent interrupted by INTERRUPTS*/
	rtos_critical_enter();

	/* RXF interrupt would occur after inic_dev_wait_dev_idle(). This case would increase time, during which Host would start SPI transfer.
	 So double check SPI is not busy, then start TXDMA */
	if (SSI_Busy(INIC_SPI_DEV)) {
		rtos_critical_exit();
		goto retry;
	}

	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);

	spi_priv.rx_req = TRUE;

	/* Send rx request signal to host */
	set_dev_rxreq_pin(DEV_RX_REQ);
	set_dev_rdy_pin(DEV_READY);

	rtos_critical_exit();

	rtos_mutex_give(spi_priv.tx_lock);

	return;
}

/**
 * @brief  to initialize the inic device.
 * @param  none.
 * @return none.
 */
void inic_dev_init(void)
{
	rtk_log_level_set(TAG_WLAN_INIC, RTK_LOG_DEBUG);

	wifi_set_user_config();
	init_skb_pool(wifi_user_config.skb_num_np, wifi_user_config.skb_buf_size ? wifi_user_config.skb_buf_size : MAX_SKB_BUF_SIZE, SKB_CACHE_SZ);

	inic_spi_init();

	/* initialize the dev priv */
	inic_dev_init_priv();

	inic_api_init_dev();
}

