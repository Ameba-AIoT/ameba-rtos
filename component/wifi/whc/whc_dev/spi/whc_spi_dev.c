#include "whc_dev.h"

struct whc_spi_priv_t spi_priv = {0};

void(*bt_inic_spi_recv_ptr)(uint8_t *buffer, uint16_t len);

void whc_spi_dev_dma_tx_done_cb(void *param)
{
	struct whc_spi_priv_t *spi_priv = (struct whc_spi_priv_t *) param;
	struct whc_txbuf_info_t *buf_info = container_of(spi_priv->txbuf_info, struct whc_txbuf_info_t, txbuf_info);
	GDMA_InitTypeDef *GDMA_InitStruct = &spi_priv->SSITxGdmaInitStruct;

	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, DISABLE);

	whc_dev_free_txbuf(buf_info);

	spi_priv->txbuf_info = NULL;
}

int whc_spi_dev_dma_rx_done_cb(void *param)
{
	struct whc_spi_priv_t *spi_priv = (struct whc_spi_priv_t *) param;
	GDMA_InitTypeDef *GDMA_InitStruct = &spi_priv->SSIRxGdmaInitStruct;
	struct sk_buff *new_skb = NULL, *rx_pkt = spi_priv->rx_skb;
	struct whc_msg_info *msg_info;
	u32 event;

	/* disable gdma channel */
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, DISABLE);

	DCache_Invalidate((u32)rx_pkt->data, SPI_BUFSZ);

	event = *(u32 *)(rx_pkt->data);

	/* receives XMIT_PKTS */
	if (event == WHC_WIFI_EVT_XIMT_PKTS) {
		/* reserved 3 skb for rx */
		if (((skbpriv.skb_buff_num - skbpriv.skb_buff_used) < 3) ||
			((new_skb = dev_alloc_skb(SPI_BUFSZ, SPI_SKB_RSVD_LEN)) == NULL)) {
			new_skb = spi_priv->rx_skb;
			goto drop_pkt;
		} else {
			spi_priv->rx_skb = new_skb;
		}

		/* process rx data */
		msg_info = (struct whc_msg_info *) rx_pkt->data;
		if (!wifi_is_running(msg_info->wlan_idx)) {
			/*free skb and return*/
			RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "Port %d is down, drop!\n", msg_info->wlan_idx);
			dev_kfree_skb_any(rx_pkt);
			goto drop_pkt;
		}
		skb_reserve(rx_pkt, sizeof(struct whc_msg_info) + msg_info->pad_len);
		skb_put(rx_pkt, msg_info->data_len);

		rx_pkt->dev = (void *)((u32)msg_info->wlan_idx);

		whc_dev_event_int_hdl((u8 *)msg_info, rx_pkt);

drop_pkt:
		/* set new dest addr for RXDMA */
		DCache_Invalidate((u32)new_skb->data, SPI_BUFSZ);
		GDMA_SetDstAddr(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, (u32)new_skb->data);
	} else if ((event >= WHC_BT_EVT_BASE) && (event < WHC_BT_EVT_MAX)) {

		/* copy by bt, skb no change */
		if (bt_inic_spi_recv_ptr) {
			bt_inic_spi_recv_ptr(spi_priv->rx_skb->data, SPI_BUFSZ);
		}
	} else if ((event == WHC_WIFI_EVT_CMD) || (event == WHC_WIFI_EVT_API_CALL) || (event == WHC_WIFI_EVT_API_RETURN)) {
		/* receives EVENTS */
		whc_dev_dispatch_event_copy(spi_priv->rx_skb->data, SPI_BUFSZ);
	} else {
		/* others, do nothing */
		//RTK_LOGD(TAG_WLAN_INIC, "RX dummy data, no process\n");
	}

	/* restart RX DMA */
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);

	return RTK_SUCCESS;
}

u32 whc_spi_dev_rxdma_irq_handler(void *pData)
{
	GDMA_InitTypeDef *GDMA_InitStruct;
	u32 int_status, rx_evt;

	(void)pData;

	GDMA_InitStruct = &spi_priv.SSIRxGdmaInitStruct;

	/* check and clear RX DMA ISR */
	int_status = GDMA_ClearINT(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum);
	if ((int_status & (TransferType)))  {
		/* patch for SPI RXF interrupt status cleared by RXDMA or masked by cpsid.
			RXDMA would move data from RXFIFO to memory, which would make RXFIFO empty and clear RXF interrupt.*/
		set_dev_rdy_pin(DEV_BUSY);
		rtos_critical_enter(RTOS_CRITICAL_WIFI);
		spi_priv.dev_status |= DEV_STS_WAIT_RXDMA_DONE;
		rtos_critical_exit(RTOS_CRITICAL_WIFI);

		rx_evt = SPI_DMA_EVT_RX_DONE;
		rtos_queue_send(spi_priv.dma_irq_queue, &rx_evt, 0);
	}

	if (int_status & ErrType) {
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "rx dma err!\n");
	}

	return 0;
}

u32 whc_spi_dev_txdma_irq_handler(void *pData)
{
	GDMA_InitTypeDef *GDMA_InitStruct;
	u32 int_status, tx_evt;

	(void)pData;

	GDMA_InitStruct = &spi_priv.SSITxGdmaInitStruct;

	/* check and clear TX DMA ISR */
	int_status = GDMA_ClearINT(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum);
	if (int_status & (TransferType)) {
		set_dev_txreq_pin(DEV_TX_IDLE);
		rtos_critical_enter(RTOS_CRITICAL_WIFI);
		spi_priv.dev_status |= DEV_STS_WAIT_TXDMA_DONE;
		rtos_critical_exit(RTOS_CRITICAL_WIFI);

		tx_evt = SPI_DMA_EVT_TX_DONE;
		rtos_queue_send(spi_priv.dma_irq_queue, &tx_evt, 0);
	}

	if (int_status & ErrType) {
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "txdma err!\n");
	}

	return 0;
}

int whc_spi_dev_set_dev_status(struct whc_spi_priv_t *whc_spi_priv, u32 ops, u32 sts)
{
	rtos_critical_enter(RTOS_CRITICAL_WIFI);
	if ((ops == DISABLE) && (whc_spi_priv->dev_status & sts)) {
#ifdef SPI_DEBUG
		u32 pin = 0;

		switch (sts) {
		case DEV_STS_WAIT_TXDMA_DONE:
			pin = _PB_2;
			break;
		case DEV_STS_WAIT_RXDMA_DONE:
			pin = _PB_3;
			break;
		case DEV_STS_SPI_CS_LOW:
			pin = _PB_10;
			break;
		default:
			break;
		}

		GPIO_WriteBit(pin, 1);
#endif
		/* Clear status if exists */
		whc_spi_priv->dev_status &= ~sts;

		/* check if idle */
		if (whc_spi_priv->dev_status == DEV_STS_IDLE) {
			/* In case sclk is interfered, causing slave data to be sampled incorrectly, a reset is required.*/
			SSI_SlaveErrRecovery(WHC_SPI_DEV);

			/* disable spi recover timer */
			RTIM_Cmd(TIMx[WHC_RECOVER_TIM_IDX], DISABLE);

			/* re-enable SPI RXF interrupt */
			SSI_INTConfig(WHC_SPI_DEV, SPI_BIT_RXFIM, ENABLE);
			whc_spi_priv->tx_req = FALSE;

			if (!whc_spi_priv->wait_tx) {
				/* set DEV_RDY pin to idle */
				set_dev_rdy_pin(DEV_READY);
			}
		}
#ifdef SPI_DEBUG
		GPIO_WriteBit(pin, 0);
#endif
	} else if (ops == ENABLE) {
		/* Set status */
		set_dev_rdy_pin(DEV_BUSY);

		if (whc_spi_priv->tx_req) {
			/* Host initiate tx_req, set TX_REQ pin to idle */
			set_dev_txreq_pin(DEV_TX_IDLE);
		}
		whc_spi_priv->dev_status = sts;

		/* enable spi recover timer */
		RTIM_Reset(TIMx[WHC_RECOVER_TIM_IDX]);
		RTIM_Cmd(TIMx[WHC_RECOVER_TIM_IDX], ENABLE);
	}
	rtos_critical_exit(RTOS_CRITICAL_WIFI);

	if (whc_spi_priv->dev_status == DEV_STS_IDLE) {
		rtos_sema_give(whc_spi_priv->spi_transfer_done_sema);
	}

	return RTK_SUCCESS;
}

u32 whc_spi_dev_recover(void *Data)
{
	struct whc_spi_priv_t *whc_spi_priv = (struct whc_spi_priv_t *) Data;
	u32 tx_evt = SPI_DMA_EVT_TX_DONE;

	RTIM_INTClear(TIMx[WHC_RECOVER_TIM_IDX]);

	/* check if error occurs or SPI transfer is still ongoing */
	if (SSI_Busy(WHC_SPI_DEV)) {
		return 0;
	}

	if (whc_spi_priv->dev_status & DEV_STS_SPI_CS_LOW) {
		RTK_LOGD(TAG_WLAN_INIC, "SSR interrupt lost\n");
		whc_spi_dev_set_dev_status(whc_spi_priv, DISABLE, DEV_STS_SPI_CS_LOW);

	} else if (whc_spi_priv->dev_status & DEV_STS_WAIT_TXDMA_DONE) {
		RTK_LOGD(TAG_WLAN_INIC, "TXDMA not done\n");
		rtos_queue_send(whc_spi_priv->dma_irq_queue, &tx_evt, 0);
	} else {
		/* disable spi recover timer */
		RTIM_Cmd(TIMx[WHC_RECOVER_TIM_IDX], DISABLE);
	}

	return 0;
}

void whc_spi_dev_dma_irq_task(void *pData)
{
	struct whc_spi_priv_t *whc_spi_priv = pData;
	u32 evt;

	for (;;) {
		rtos_queue_receive(whc_spi_priv->dma_irq_queue, &evt, RTOS_MAX_TIMEOUT);

		if (evt == SPI_DMA_EVT_TX_DONE) {
			if (whc_spi_priv->dev_status & DEV_STS_WAIT_TXDMA_DONE) {
				whc_spi_dev_dma_tx_done_cb(whc_spi_priv);
				whc_spi_dev_set_dev_status(whc_spi_priv, DISABLE, DEV_STS_WAIT_TXDMA_DONE);
			}
		} else if (evt == SPI_DMA_EVT_RX_DONE) {
			if (whc_spi_priv->dev_status & DEV_STS_WAIT_RXDMA_DONE) {
				whc_spi_dev_dma_rx_done_cb(whc_spi_priv);
				whc_spi_dev_set_dev_status(whc_spi_priv, DISABLE, DEV_STS_WAIT_RXDMA_DONE);
			}
		}
	}
}

u32 whc_spi_dev_interrupt_handler(void *param)
{
	struct whc_spi_priv_t *whc_spi_priv = (struct whc_spi_priv_t *)param;
	u32 interrupt_status = SSI_GetIsr(WHC_SPI_DEV);
	u32 status;

	SSI_SetIsrClean(WHC_SPI_DEV, interrupt_status);

	if (interrupt_status & SPI_BIT_RXFIS) {
		SSI_INTConfig(WHC_SPI_DEV, SPI_BIT_RXFIM, DISABLE);

		status = DEV_STS_SPI_CS_LOW | DEV_STS_WAIT_RXDMA_DONE;
		if (whc_spi_priv->tx_req) {
			status |= DEV_STS_WAIT_TXDMA_DONE;
		}
		whc_spi_dev_set_dev_status(whc_spi_priv, ENABLE, status);
	}

	if (interrupt_status & SPI_BIT_SSRIS) {
		if (whc_spi_priv->dev_status & DEV_STS_SPI_CS_LOW) {
			whc_spi_dev_set_dev_status(whc_spi_priv, DISABLE, DEV_STS_SPI_CS_LOW);
		}
	}

	return 0;
}

static u32 whc_spi_dev_suspend(u32 expected_idle_time, void *param)
{
	UNUSED(expected_idle_time);
	UNUSED(param);
	if (pmu_get_sleep_type() == SLEEP_PG) {
		InterruptDis(SPIS_IRQ);
		InterruptUnRegister(SPIS_IRQ);
	}
	return TRUE;
}

static u32 whc_spi_dev_resume(u32 expected_idle_time, void *param)
{
	UNUSED(expected_idle_time);
	UNUSED(param);

	SSI_InitTypeDef SSI_InitStructSlave;
	GDMA_InitTypeDef *GDMA_InitStruct = &spi_priv.SSITxGdmaInitStruct;
	u32 index = (WHC_SPI_DEV == SPI0_DEV) ? 0 : 1;

	if (pmu_get_sleep_type() == SLEEP_PG) {
		set_dev_txreq_pin(DEV_TX_IDLE);

		/* Initialize SPI */
		PAD_PullCtrl(SPIS_CS, GPIO_PuPd_UP);  // pull-up, default 1
		PAD_PullCtrl(SPIS_SCLK, GPIO_PuPd_DOWN);

		SSI_SetRole(WHC_SPI_DEV, SSI_SLAVE);
		SSI_StructInit(&SSI_InitStructSlave);
		SSI_InitStructSlave.SPI_SclkPhase = SCPH_TOGGLES_IN_MIDDLE;
		SSI_InitStructSlave.SPI_SclkPolarity = SCPOL_INACTIVE_IS_LOW;
		SSI_InitStructSlave.SPI_DataFrameSize = DFS_8_BITS;
		SSI_InitStructSlave.SPI_Role = SSI_SLAVE;
		SSI_Init(WHC_SPI_DEV, &SSI_InitStructSlave);

		InterruptRegister((IRQ_FUN)whc_spi_dev_interrupt_handler, SPIS_IRQ, (u32)(&spi_priv), INT_PRI_MIDDLE);
		InterruptEn(SPIS_IRQ, INT_PRI_MIDDLE);

		/* Enable RX full interrupt */
		SSI_INTConfig(WHC_SPI_DEV, SPI_BIT_RXFIM | SPI_BIT_SSRIM, ENABLE);

		/* Configure RX DMA */
		SSI_RXGDMA_Init(index, &spi_priv.SSIRxGdmaInitStruct, (void *)WHC_SPI_RXDMA, whc_spi_dev_rxdma_irq_handler, spi_priv.rx_skb->data, SPI_BUFSZ);
		SSI_SetDmaEnable(WHC_SPI_DEV, ENABLE, SPI_BIT_RDMAE);

		set_dev_rdy_pin(DEV_READY);

		SSI_SetDmaEnable(WHC_SPI_DEV, ENABLE, SPI_BIT_TDMAE);
		/*  Enable GDMA for TX */
		GDMA_Init(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, GDMA_InitStruct);
	}

	whc_dev_ps_resume_cb();
	return TRUE;
}

void whc_spi_dev_init(void)
{
	struct whc_spi_priv_t *whc_spi_priv = &spi_priv;
	RTIM_TimeBaseInitTypeDef TIM_InitStruct;
	SSI_InitTypeDef SSI_InitStructSlave;
	GPIO_InitTypeDef GPIO_InitStruct;
	struct sk_buff *skb = NULL;
	u32 index;

	index = (WHC_SPI_DEV == SPI0_DEV) ? 0 : 1;

	rtos_mutex_create_static(&whc_spi_priv->tx_lock);
	rtos_queue_create(&whc_spi_priv->dma_irq_queue, 4, sizeof(u32));
	rtos_sema_create(&whc_spi_priv->spi_transfer_done_sema, 0, RTOS_SEMA_MAX_COUNT);
	rtos_sema_create(&whc_spi_priv->free_skb_sema, 0, RTOS_SEMA_MAX_COUNT);

	/* Initialize GPIO */
	GPIO_InitStruct.GPIO_Pin = DEV_TX_REQ_PIN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(&GPIO_InitStruct);
	set_dev_txreq_pin(DEV_TX_IDLE);

	GPIO_InitStruct.GPIO_Pin = DEV_READY_PIN;
	GPIO_Init(&GPIO_InitStruct);
#ifdef SPI_DEBUG
	GPIO_InitStruct.GPIO_Pin = _PB_2;
	GPIO_Init(&GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = _PB_3;
	GPIO_Init(&GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = _PB_10;
	GPIO_Init(&GPIO_InitStruct);
#endif

	/* Initialize Timer*/
#ifdef CONFIG_AMEBAGREEN2
	/* Switch clock source to 40M (default is 32768), default frequency division is 40, so acquire 1M timer (Dplus APBPeriph_HTIM0 is 1M).
	   Can use RCC_PeriphClockDividerSet(XTAL_LTIM0, 4) to set div4, acquire 10M timer. */
	RCC_PeriphClockSourceSet(LTIM0, XTAL);
#endif
	RCC_PeriphClockCmd(APBPeriph_TIMx[WHC_RECOVER_TIM_IDX], APBPeriph_TIMx_CLOCK[WHC_RECOVER_TIM_IDX], ENABLE);
	RTIM_TimeBaseStructInit(&TIM_InitStruct);
	TIM_InitStruct.TIM_Idx = WHC_RECOVER_TIM_IDX;
	TIM_InitStruct.TIM_Period = WHC_RECOVER_TO_US;

	RTIM_TimeBaseInit(TIMx[WHC_RECOVER_TIM_IDX], &TIM_InitStruct, TIMx_irq[WHC_RECOVER_TIM_IDX], (IRQ_FUN)whc_spi_dev_recover, (u32)whc_spi_priv);
	RTIM_INTConfig(TIMx[WHC_RECOVER_TIM_IDX], TIM_IT_Update, ENABLE);

	/* Initialize SPI */
	if (WHC_SPI_DEV == SPI0_DEV) {
		RCC_PeriphClockCmd(APBPeriph_SPI0, APBPeriph_SPI0_CLOCK, ENABLE);
	} else {
		RCC_PeriphClockCmd(APBPeriph_SPI1, APBPeriph_SPI1_CLOCK, ENABLE);
	}

	Pinmux_Config(SPIS_MOSI, PINMUX_FUNCTION_SPIS);//MOSI
	Pinmux_Config(SPIS_MISO, PINMUX_FUNCTION_SPIS);//MISO
	Pinmux_Config(SPIS_SCLK, PINMUX_FUNCTION_SPIS);//CLK
	Pinmux_Config(SPIS_CS, PINMUX_FUNCTION_SPIS);//CS
	PAD_PullCtrl(SPIS_CS, GPIO_PuPd_UP);  // pull-up, default 1
	PAD_PullCtrl(SPIS_SCLK, GPIO_PuPd_DOWN);

	SSI_SetRole(WHC_SPI_DEV, SSI_SLAVE);
	SSI_StructInit(&SSI_InitStructSlave);
	SSI_InitStructSlave.SPI_SclkPhase = SCPH_TOGGLES_IN_MIDDLE;
	SSI_InitStructSlave.SPI_SclkPolarity = SCPOL_INACTIVE_IS_LOW;
	SSI_InitStructSlave.SPI_DataFrameSize = DFS_8_BITS;
	SSI_InitStructSlave.SPI_Role = SSI_SLAVE;
	SSI_Init(WHC_SPI_DEV, &SSI_InitStructSlave);

	InterruptRegister((IRQ_FUN)whc_spi_dev_interrupt_handler, SPIS_IRQ, (u32)whc_spi_priv, INT_PRI_HIGH);
	InterruptEn(SPIS_IRQ, INT_PRI_HIGH);

	/* Enable RX full interrupt */
	SSI_INTConfig(WHC_SPI_DEV, SPI_BIT_RXFIM | SPI_BIT_SSRIM, ENABLE);

	/* Configure DMA and buffer */
	whc_spi_priv->txdma_initialized = 0;
	whc_spi_priv->txbuf_info = NULL;

	skb = dev_alloc_skb(SPI_BUFSZ, SPI_SKB_RSVD_LEN);
	if (skb == NULL || (((u32)skb->data) & 0x3) != 0) {
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "spi_init: skb err!\n");
		return;
	}
	whc_spi_priv->rx_skb = skb;

	/* Configure RX DMA */
	DCache_Invalidate((u32)skb->data, SPI_BUFSZ);
	SSI_RXGDMA_Init(index, &whc_spi_priv->SSIRxGdmaInitStruct, (void *)WHC_SPI_RXDMA, whc_spi_dev_rxdma_irq_handler, skb->data, SPI_BUFSZ);
	SSI_SetDmaEnable(WHC_SPI_DEV, ENABLE, SPI_BIT_RDMAE);

	pmu_register_sleep_callback(PMU_WHC_WIFI, (PSM_HOOK_FUN)whc_spi_dev_suspend, NULL, (PSM_HOOK_FUN)whc_spi_dev_resume, NULL);

	/* Create irq task */
	if (rtos_task_create(NULL, "SPI_DMA_IRQ_TASK", whc_spi_dev_dma_irq_task, (void *)whc_spi_priv, 450, 9) != RTK_SUCCESS) {
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "DMA TASK Err!\n");
		return;
	}

	/* Device ready */
	whc_spi_priv->dev_status = DEV_STS_IDLE;
	set_dev_rdy_pin(DEV_READY);

	if (WHC_WIFI_EVT_MAX > WHC_BT_EVT_BASE) {
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "check event id!\n");
	}

	RTK_LOGI(TAG_WLAN_INIC, "SPI device init done!\n");
}

bool whc_spi_dev_txdma_init(
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
		return FALSE;
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
			RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "txdma init err:Data=%x,  LEN=%d\n", pTxData, Length);
			return FALSE;
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

	return TRUE;
}

void whc_spi_dev_flowctrl(u8 *status, u8 send_cmd)
{
	u8 status_change = 0;

	if (skbpriv.skb_buff_num - skbpriv.skb_buff_used < SPI_FLOWCTRL_LOW_THRESHOLD) {
		if (!spi_priv.flowctrl_en) {
			spi_priv.flowctrl_en = 1;
			status_change = 1;
		}
	} else if (skbpriv.skb_buff_num - skbpriv.skb_buff_used > SPI_FLOWCTRL_HIGH_THRESHOLD) {
		if (spi_priv.flowctrl_en) {
			spi_priv.flowctrl_en = 0;
			status_change = 1;
		}
	}

	if (status) {
		*status = spi_priv.flowctrl_en;
	}

	if (send_cmd && status_change) {
		whc_dev_send_flowctrl_cmd(spi_priv.flowctrl_en);
	}
}

static s8 whc_spi_wait_dev_idle(void)
{
	s8 ret = 0;

	/* Wait for last SPI transaction done, including stages:
		1) trigger TX_REQ to host, wait for host to initiate SPI transfer (spi_priv.tx_req=TRUE)
		2) host initiates SPI transfer ~ device respond to RXF interrupt (SSI_Busy)
		3) device respond to RXF interrupt ~ device TRXDMA done (spi_priv.dev_status != DEV_STS_IDLE)*/
	while (spi_priv.tx_req || spi_priv.dev_status != DEV_STS_IDLE || SSI_Busy(WHC_SPI_DEV)) {
		spi_priv.wait_tx = TRUE;
		if (rtos_sema_take(spi_priv.spi_transfer_done_sema, WHC_DEV_SPI_TRANSFER_TIMEOUT) == RTK_FAIL) {
			RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "sema to, sts:%d, txreq:%d, spi:%d\n",
					 spi_priv.dev_status, spi_priv.tx_req, SSI_Busy(WHC_SPI_DEV));
#ifdef CONFIG_WHC_DEV_TCPIP_KEEPALIVE
			whc_dev_api_set_host_state(WHC_HOST_UNREADY);
#endif
			ret = -1;
			break;
		}
	}

	spi_priv.wait_tx = FALSE;
	return ret;
}

u8 whc_spi_dev_bus_is_idle(void)
{
	if (spi_priv.tx_req || spi_priv.dev_status != DEV_STS_IDLE || SSI_Busy(WHC_SPI_DEV)) {
		return FALSE;
	}
	return TRUE;
}

/**
* @brief  send the data in the buffer to host.
* @param  buf: data buf to be sent, must 4B aligned.
* @param  len: data len to be sent.
* @param  buf_alloc: real buf address, to be freed after sent.
* @param  is_skb: whether the buf is skb or not.
* @return none.
*/
void whc_spi_dev_send(u8 *buf, u16 len, void *buf_alloc, u8 is_skb)
{
	s8 ret = 0;
	struct whc_txbuf_info_t *buf_info = NULL;
	GDMA_InitTypeDef *GDMA_InitStruct = &spi_priv.SSITxGdmaInitStruct;
	u32 index = (WHC_SPI_DEV == SPI0_DEV) ? 0 : 1;
	struct whc_buf_info *pbuf;

	if ((u32)buf & (DEV_DMA_ALIGN - 1)) {
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "Txbuf align Err!\n");
		goto drop;
	}

	buf_info = whc_dev_alloc_buf_info(buf, len, buf_alloc, is_skb);
	if (!buf_info) {
		goto drop;
	}
	pbuf = &buf_info->txbuf_info;

	/* Call this function when receive pkt, call API or send API return value. Use locks to ensure exclusive execution. */
	rtos_mutex_take(spi_priv.tx_lock, MUTEX_WAIT_TIMEOUT);

	DCache_CleanInvalidate(pbuf->buf_addr, SPI_BUFSZ);

retry:
	/* check dev is idle */
	ret = whc_spi_wait_dev_idle();
	if (ret < 0) {
		goto drop;
	}

	/* Initialize or Restart TXDMA */
	if (!spi_priv.txdma_initialized) {
		SSI_SetDmaEnable(WHC_SPI_DEV, ENABLE, SPI_BIT_TDMAE);
		whc_spi_dev_txdma_init(index, &spi_priv.SSITxGdmaInitStruct, WHC_SPI_TXDMA, whc_spi_dev_txdma_irq_handler, (u8 *)(pbuf->buf_addr), SPI_BUFSZ);
		spi_priv.txdma_initialized = 1;
	} else {
		GDMA_SetSrcAddr(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, pbuf->buf_addr);
	}
	spi_priv.txbuf_info = pbuf;

	/* protected by critical section to prevent interrupted by INTERRUPTS*/
	rtos_critical_enter(RTOS_CRITICAL_WIFI);

	/* RXF interrupt would occur after whc_spi_wait_dev_idle(). This case would increase time, during which Host would start SPI transfer.
	 So double check SPI is not busy, then start TXDMA */
	if (SSI_Busy(WHC_SPI_DEV) || (spi_priv.dev_status != DEV_STS_IDLE)) {
		rtos_critical_exit(RTOS_CRITICAL_WIFI);
		goto retry;
	}

	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);

	spi_priv.tx_req = TRUE;

	/* Send tx request signal to host */
	set_dev_txreq_pin(DEV_TX_REQ);
	set_dev_rdy_pin(DEV_READY);

	rtos_critical_exit(RTOS_CRITICAL_WIFI);

	rtos_mutex_give(spi_priv.tx_lock);

	return;

drop:
	/* Dev drop tx, free tx skb or buffer */
	if (is_skb) {
		dev_kfree_skb_any((struct sk_buff *) buf_alloc);
	} else {
		rtos_mem_free((u8 *)buf_alloc);
	}
	rtos_mem_free((u8 *)buf_info);

	rtos_mutex_give(spi_priv.tx_lock);
}
