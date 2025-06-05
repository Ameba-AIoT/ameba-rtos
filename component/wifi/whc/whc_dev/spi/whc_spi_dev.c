#include "whc_dev.h"

struct whc_spi_priv_t spi_priv = {0};

void rtw_pending_q_resume(void);
void(*bt_inic_spi_recv_ptr)(uint8_t *buffer, uint16_t len);

void whc_spi_dev_dma_tx_done_cb(void *param)
{
	struct whc_spi_priv_t *spi_priv = (struct whc_spi_priv_t *) param;
	struct whc_txbuf_info_t *inic_tx = container_of(spi_priv->txbuf_info, struct whc_txbuf_info_t, txbuf_info);
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

int whc_spi_dev_dma_rx_done_cb(void *param)
{
	struct whc_spi_priv_t *spi_priv = (struct whc_spi_priv_t *) param;
	GDMA_InitTypeDef *GDMA_InitStruct = &spi_priv->SSIRxGdmaInitStruct;
	struct sk_buff *new_skb = NULL, *rx_pkt = spi_priv->rx_skb;
	struct whc_msg_info *msg_info;
	u8 *buf = NULL;
	u32 event;

	/* disable gdma channel */
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, DISABLE);

	DCache_Invalidate((u32)rx_pkt->data, SPI_BUFSZ);

	event = *(u32 *)(rx_pkt->data);

	/* receives XMIT_PKTS */
	if (event == WHC_WIFI_EVT_XIMT_PKTS) {
retry:
		/* alloc new skb, blocked if no skb */
		if (((skbpriv.skb_buff_num - skbpriv.skb_buff_used) < 3) ||
			((new_skb = dev_alloc_skb(SPI_BUFSZ, SPI_SKB_RSVD_LEN)) == NULL)) {
			spi_priv->wait_for_txbuf = TRUE;

			/* resume pending queue to release skb */
			rtw_pending_q_resume();

			/* wait timeout to re-check skb, considering corner cases for wait_for_txbuf update */
			rtos_sema_take(spi_priv->free_skb_sema, 5);

			goto retry;
		} else {
			spi_priv->wait_for_txbuf = FALSE;
			spi_priv->rx_skb = new_skb;
		}

		/* process rx data */
		msg_info = (struct whc_msg_info *) rx_pkt->data;
		skb_reserve(rx_pkt, sizeof(struct whc_msg_info) + msg_info->pad_len);
		skb_put(rx_pkt, msg_info->data_len);

		rx_pkt->dev = (void *)msg_info->wlan_idx;

		whc_spi_dev_event_int_hdl((u8 *)msg_info, rx_pkt);

		/* set new dest addr for RXDMA */
		DCache_Invalidate((u32)new_skb->data, SPI_BUFSZ);
		GDMA_SetDstAddr(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, (u32)new_skb->data);
	} else if ((event >= WHC_BT_EVT_BASE) && (event < WHC_BT_EVT_MAX)) {

		/* copy by bt, skb no change */
		if (bt_inic_spi_recv_ptr) {
			bt_inic_spi_recv_ptr(spi_priv->rx_skb->data, SPI_BUFSZ);
		}
	} else if ((event == WHC_WIFI_EVT_BRIDGE) || (event == WHC_WIFI_EVT_API_CALL) || (event == WHC_WIFI_EVT_API_RETURN)) {
		/* receives EVENTS */
		buf = rtos_mem_zmalloc(SPI_BUFSZ);	//TODO: optimize
		if (buf == NULL) {
			RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "%s, can't alloc buffer!!\n", __func__);
			goto exit;
		}

		memcpy(buf, spi_priv->rx_skb->data, SPI_BUFSZ);

		/* free buf later, no need to modify skb. */
		whc_spi_dev_event_int_hdl(buf, NULL);
	} else {
		/* others, do nothing */
		//RTK_LOGD(TAG_WLAN_INIC, "RX dummy data, no process\n");
	}

exit:
	/* restart RX DMA */
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);

	return RTK_SUCCESS;
}

u32 whc_spi_dev_rxdma_irq_handler(void *pData)
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
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "spi rxdma err occurs!!\n");
	}

	return 0;
}

u32 whc_spi_dev_txdma_irq_handler(void *pData)
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
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "spi txdma err occurs!!\n");
	}

	return 0;
}

int whc_spi_dev_set_dev_status(struct whc_spi_priv_t *whc_spi_priv, u32 ops, u32 sts)
{
	do {
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

				/* disable spi recover timer */
				RTIM_Cmd(TIMx[WHC_RECOVER_TIM_IDX], DISABLE);

				/* re-enable SPI RXF interrupt */
				SSI_INTConfig(WHC_SPI_DEV, SPI_BIT_RXFIM, ENABLE);
				whc_spi_priv->rx_req = FALSE;

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

			if (whc_spi_priv->rx_req) {
				/* Host initiate rx_req, set RX_REQ pin to idle */
				set_dev_rxreq_pin(DEV_RX_IDLE);
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
		if (whc_spi_priv->ssris_pending) {
			whc_spi_priv->ssris_pending = FALSE;

			ops = DISABLE;
			sts = DEV_STS_SPI_CS_LOW;
		} else if (whc_spi_priv->set_devsts_pending) {
			whc_spi_priv->set_devsts_pending = FALSE;

			ops = ENABLE;
			sts = DEV_STS_SPI_CS_LOW | DEV_STS_WAIT_RXDMA_DONE;
			if (whc_spi_priv->rx_req) {
				sts |= DEV_STS_WAIT_TXDMA_DONE;
			}
		} else {
			break;
		}
	} while (1);

	return RTK_SUCCESS;
}

u32 whc_spi_dev_recover(void *Data)
{
	struct whc_spi_priv_t *whc_spi_priv = (struct whc_spi_priv_t *) Data;

	RTIM_INTClear(TIMx[WHC_RECOVER_TIM_IDX]);

	/* check if error occurs or SPI transfer is still ongoing */
	if (SSI_Busy(WHC_SPI_DEV)) {
		return 0;
	}

	if (whc_spi_priv->dev_status & DEV_STS_SPI_CS_LOW) {
		RTK_LOGD(TAG_WLAN_INIC, "SSR interrupt lost\n");

		if (whc_spi_dev_set_dev_status(whc_spi_priv, DISABLE, DEV_STS_SPI_CS_LOW) == RTK_FAIL) {
			whc_spi_priv->ssris_pending = TRUE;
		}
	} else if (whc_spi_priv->dev_status & DEV_STS_WAIT_TXDMA_DONE) {
		RTK_LOGD(TAG_WLAN_INIC, "TXDMA not done\n");
		rtos_sema_give(whc_spi_priv->txirq_sema);
	} else {
		/* disable spi recover timer */
		RTIM_Cmd(TIMx[WHC_RECOVER_TIM_IDX], DISABLE);
	}

	return 0;
}

void whc_spi_dev_txdma_irq_task(void *pData)
{
	struct whc_spi_priv_t *whc_spi_priv = pData;

	for (;;) {
		/* Task blocked and wait the semaphore(events) here */
		rtos_sema_take(whc_spi_priv->txirq_sema, RTOS_MAX_TIMEOUT);

		if (whc_spi_priv->dev_status & DEV_STS_WAIT_TXDMA_DONE) {
			whc_spi_dev_dma_tx_done_cb(whc_spi_priv);
			whc_spi_dev_set_dev_status(whc_spi_priv, DISABLE, DEV_STS_WAIT_TXDMA_DONE);
		}
	}
}

void whc_spi_dev_rxdma_irq_task(void *pData)
{
	struct whc_spi_priv_t *whc_spi_priv = pData;

	for (;;) {
		/* Task blocked and wait the semaphore(events) here */
		rtos_sema_take(whc_spi_priv->rxirq_sema, RTOS_MAX_TIMEOUT);

		if (whc_spi_priv->dev_status & DEV_STS_WAIT_RXDMA_DONE) {
			whc_spi_dev_dma_rx_done_cb(whc_spi_priv);
			whc_spi_dev_set_dev_status(whc_spi_priv, DISABLE, DEV_STS_WAIT_RXDMA_DONE);
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
		if (whc_spi_priv->rx_req) {
			status |= DEV_STS_WAIT_TXDMA_DONE;
		}

		if (whc_spi_dev_set_dev_status(whc_spi_priv, ENABLE, status) == RTK_FAIL) {
			whc_spi_priv->set_devsts_pending = TRUE;
		}
	}

	if (interrupt_status & SPI_BIT_SSRIS) {
		if (whc_spi_priv->dev_status & DEV_STS_SPI_CS_LOW) {
			if (whc_spi_dev_set_dev_status(whc_spi_priv, DISABLE, DEV_STS_SPI_CS_LOW) == RTK_FAIL) {
				whc_spi_priv->ssris_pending = TRUE;
			}
		}
	}

	return 0;
}

static u32 whc_spi_dev_suspend(u32 expected_idle_time, void *param)
{
	UNUSED(expected_idle_time);
	UNUSED(param);
	if (pmu_get_sleep_type() == SLEEP_PG) {
		InterruptDis(SPI0_IRQ);
		InterruptUnRegister(SPI0_IRQ);
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
		set_dev_rxreq_pin(DEV_RX_IDLE);

		/* Initialize SPI */
		PAD_PullCtrl(_PB_26, GPIO_PuPd_UP);
		PAD_PullCtrl(_PB_23, GPIO_PuPd_DOWN);

		SSI_SetRole(WHC_SPI_DEV, SSI_SLAVE);
		SSI_StructInit(&SSI_InitStructSlave);
		SSI_InitStructSlave.SPI_SclkPhase = SCPH_TOGGLES_IN_MIDDLE;
		SSI_InitStructSlave.SPI_SclkPolarity = SCPOL_INACTIVE_IS_LOW;
		SSI_InitStructSlave.SPI_DataFrameSize = DFS_8_BITS;
		SSI_InitStructSlave.SPI_Role = SSI_SLAVE;
		SSI_Init(WHC_SPI_DEV, &SSI_InitStructSlave);

		InterruptRegister((IRQ_FUN)whc_spi_dev_interrupt_handler, SPI0_IRQ, (u32)(&spi_priv), INT_PRI_MIDDLE);
		InterruptEn(SPI0_IRQ, INT_PRI_MIDDLE);

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
	rtos_sema_create(&whc_spi_priv->txirq_sema, 0, RTOS_SEMA_MAX_COUNT);
	rtos_sema_create(&whc_spi_priv->rxirq_sema, 0, RTOS_SEMA_MAX_COUNT);
	rtos_sema_create(&whc_spi_priv->spi_transfer_done_sema, 0, RTOS_SEMA_MAX_COUNT);
	rtos_sema_create(&whc_spi_priv->free_skb_sema, 0, RTOS_SEMA_MAX_COUNT);

	/* Initialize GPIO */
	GPIO_InitStruct.GPIO_Pin = RX_REQ_PIN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(&GPIO_InitStruct);
	set_dev_rxreq_pin(DEV_RX_IDLE);

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
	RCC_PeriphClockCmd(APBPeriph_TIMx[WHC_RECOVER_TIM_IDX], APBPeriph_TIMx_CLOCK[WHC_RECOVER_TIM_IDX], ENABLE);
	RTIM_TimeBaseStructInit(&TIM_InitStruct);
	TIM_InitStruct.TIM_Idx = WHC_RECOVER_TIM_IDX;
	TIM_InitStruct.TIM_Period = WHC_RECOVER_TO_US;

	RTIM_TimeBaseInit(TIMx[WHC_RECOVER_TIM_IDX], &TIM_InitStruct, TIMx_irq[WHC_RECOVER_TIM_IDX], (IRQ_FUN)whc_spi_dev_recover, (u32)whc_spi_priv);
	RTIM_INTConfig(TIMx[WHC_RECOVER_TIM_IDX], TIM_IT_Update, ENABLE);

	/* Initialize SPI */
	RCC_PeriphClockCmd(APBPeriph_SPI0, APBPeriph_SPI0_CLOCK, ENABLE);
	Pinmux_Config(_PB_24, PINMUX_FUNCTION_SPI);//MOSI
	Pinmux_Config(_PB_25, PINMUX_FUNCTION_SPI);//MISO
	Pinmux_Config(_PB_23, PINMUX_FUNCTION_SPI);//CLK
	Pinmux_Config(_PB_26, PINMUX_FUNCTION_SPI);//CS
	PAD_PullCtrl(_PB_26, GPIO_PuPd_UP);
	PAD_PullCtrl(_PB_23, GPIO_PuPd_DOWN);

	SSI_SetRole(WHC_SPI_DEV, SSI_SLAVE);
	SSI_StructInit(&SSI_InitStructSlave);
	SSI_InitStructSlave.SPI_SclkPhase = SCPH_TOGGLES_IN_MIDDLE;
	SSI_InitStructSlave.SPI_SclkPolarity = SCPOL_INACTIVE_IS_LOW;
	SSI_InitStructSlave.SPI_DataFrameSize = DFS_8_BITS;
	SSI_InitStructSlave.SPI_Role = SSI_SLAVE;
	SSI_Init(WHC_SPI_DEV, &SSI_InitStructSlave);

	InterruptRegister((IRQ_FUN)whc_spi_dev_interrupt_handler, SPI0_IRQ, (u32)whc_spi_priv, INT_PRI_MIDDLE);
	InterruptEn(SPI0_IRQ, INT_PRI_MIDDLE);

	/* Enable RX full interrupt */
	SSI_INTConfig(WHC_SPI_DEV, SPI_BIT_RXFIM | SPI_BIT_SSRIM, ENABLE);

	/* Configure DMA and buffer */
	whc_spi_priv->txdma_initialized = 0;
	whc_spi_priv->txbuf_info = NULL;

	skb = dev_alloc_skb(SPI_BUFSZ, SPI_SKB_RSVD_LEN);
	if (skb == NULL || (((u32)skb->data) & 0x3) != 0) {
		RTK_LOGE(TAG_WLAN_INIC, "%s: alloc skb fail!\n", __func__);
		return;
	}
	whc_spi_priv->rx_skb = skb;

	/* Configure RX DMA */
	DCache_Invalidate((u32)skb->data, SPI_BUFSZ);
	SSI_RXGDMA_Init(index, &whc_spi_priv->SSIRxGdmaInitStruct, (void *)WHC_SPI_RXDMA, whc_spi_dev_rxdma_irq_handler, skb->data, SPI_BUFSZ);
	SSI_SetDmaEnable(WHC_SPI_DEV, ENABLE, SPI_BIT_RDMAE);

	pmu_register_sleep_callback(PMU_FULLMAC_WIFI, (PSM_HOOK_FUN)whc_spi_dev_suspend, NULL, (PSM_HOOK_FUN)whc_spi_dev_resume, NULL);

	/* Create irq task */
	if (rtos_task_create(NULL, "SPI_RXDMA_IRQ_TASK", whc_spi_dev_rxdma_irq_task, (void *)whc_spi_priv, 1024 * 4, 7) != RTK_SUCCESS) {
		RTK_LOGE(TAG_WLAN_INIC, "Create SPI_RXDMA_IRQ_TASK Err!!\n");
		return;
	}

	if (rtos_task_create(NULL, "SPI_TXDMA_IRQ_TASK", whc_spi_dev_txdma_irq_task, (void *)whc_spi_priv, 1024 * 4, 7) != RTK_SUCCESS) {
		RTK_LOGE(TAG_WLAN_INIC, "Create SPI_TXDMA_IRQ_TASK Err!!\n");
		return;
	}

	/* Device ready */
	whc_spi_priv->dev_status = DEV_STS_IDLE;
	set_dev_rdy_pin(DEV_READY);

	if (WHC_WIFI_EVT_MAX > WHC_BT_EVT_BASE) {
		RTK_LOGE(TAG_WLAN_INIC, "SPI ID may conflict!\n");
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
			RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "SSI_TXGDMA_Init: Aligment Err: pTxData=%p,  Length=%lu\n", pTxData, Length);
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

void whc_spi_dev_trigger_rx_handle(void)
{
	if (spi_priv.wait_for_txbuf) {
		rtos_sema_give(spi_priv.free_skb_sema);
	}
}

u8 whc_spi_dev_tx_path_avail(void)
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

void whc_dev_spi_wait_dev_idle(void)
{
	/* Wait for last SPI transaction done, including stages:
		1) trigger RX_REQ to host, wait for host to initiate SPI transfer (spi_priv.rx_req=TRUE)
		2) host initiates SPI transfer ~ device respond to RXF interrupt (SSI_Busy)
		3) device respond to RXF interrupt ~ device TRXDMA done (spi_priv.dev_status != DEV_STS_IDLE)*/
	while (spi_priv.rx_req || spi_priv.dev_status != DEV_STS_IDLE || SSI_Busy(WHC_SPI_DEV)) {
		spi_priv.wait_tx = TRUE;
		rtos_sema_take(spi_priv.spi_transfer_done_sema, 0xFFFFFFFF);
	}

	spi_priv.wait_tx = FALSE;
}

/**
* @brief  send buf from upper, used by BT without struct whc_buf_info.
* @param  buf: data buf to be sent, must 4B aligned.
* @param  buf: data len to be sent.
* @param  buf_alloc: real buf address, to be freed after sent.
* @return none.
*/
void whc_spi_dev_send_to_host(u8 *buf, u8 *buf_alloc, u16 len)
{
	struct whc_txbuf_info_t *inic_tx;

	/* construct struct whc_buf_info & whc_buf_info_t */
	inic_tx = (struct whc_txbuf_info_t *)rtos_mem_zmalloc(sizeof(struct whc_txbuf_info_t));

	inic_tx->txbuf_info.buf_allocated = inic_tx->txbuf_info.buf_addr = (u32)buf;
	inic_tx->txbuf_info.size_allocated = inic_tx->txbuf_info.buf_size = len;

	inic_tx->ptr = buf_alloc;
	inic_tx->is_skb = 0;

	/* send ret_msg + ret_val(buf, len) */
	whc_spi_dev_send(&inic_tx->txbuf_info);
}

/**
* @brief  send buf from upper, used by BT without struct whc_buf_info.
* @param  buf: data buf to be sent, must 4B aligned.
* @param  len: data len to be sent.
* @return none.
*/
void whc_spi_dev_send_data(u8 *buf, u32 len)
{
	struct whc_txbuf_info_t *inic_tx;

	if ((u32)buf & (DEV_DMA_ALIGN - 1)) {
		RTK_LOGE(TAG_WLAN_INIC, "Send Error, Data buf unaligned!");
		return;
	}

	/* construct struct whc_buf_info & whc_buf_info_t */
	inic_tx = (struct whc_txbuf_info_t *)rtos_mem_zmalloc(sizeof(struct whc_txbuf_info_t));

	inic_tx->txbuf_info.buf_allocated = inic_tx->txbuf_info.buf_addr = (u32)buf;
	inic_tx->txbuf_info.size_allocated = inic_tx->txbuf_info.buf_size = len;

	inic_tx->ptr = buf;
	inic_tx->is_skb = 0;

	/* send ret_msg + ret_val(buf, len) */
	whc_spi_dev_send(&inic_tx->txbuf_info);
}

void whc_spi_dev_send(struct whc_buf_info *pbuf)
{
	GDMA_InitTypeDef *GDMA_InitStruct = &spi_priv.SSITxGdmaInitStruct;
	u32 index = (WHC_SPI_DEV == SPI0_DEV) ? 0 : 1;

	/* Call this function when receive pkt, call API or send API return value. Use locks to ensure exclusive execution. */
	rtos_mutex_take(spi_priv.tx_lock, MUTEX_WAIT_TIMEOUT);

	DCache_CleanInvalidate(pbuf->buf_addr, SPI_BUFSZ);

retry:
	/* check dev is idle */
	whc_dev_spi_wait_dev_idle();

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

	/* RXF interrupt would occur after whc_dev_spi_wait_dev_idle(). This case would increase time, during which Host would start SPI transfer.
	 So double check SPI is not busy, then start TXDMA */
	if (SSI_Busy(WHC_SPI_DEV)) {
		rtos_critical_exit(RTOS_CRITICAL_WIFI);
		goto retry;
	}

	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);

	spi_priv.rx_req = TRUE;

	/* Send rx request signal to host */
	set_dev_rxreq_pin(DEV_RX_REQ);
	set_dev_rdy_pin(DEV_READY);

	rtos_critical_exit(RTOS_CRITICAL_WIFI);

	rtos_mutex_give(spi_priv.tx_lock);

	return;
}
