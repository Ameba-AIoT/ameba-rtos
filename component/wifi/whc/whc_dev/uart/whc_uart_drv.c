#include "whc_dev.h"

struct whc_uart_priv_t uart_priv = {0};
int whc_uart_idx;
void rtw_pending_q_resume(void);
//#define WHC_UART_DEBUG 1
#ifdef WHC_UART_DEBUG
static void dump_buf(char *info, uint8_t *buf, uint32_t len)
{
	DiagPrintf("%s", info);
	for (uint32_t i = 0; i < len; i++) {
		DiagPrintf("%s0x%02X%s", i % 16 == 0 ? "\n     " : ",",
				   buf[i], i == len - 1 ? "\n" : "");
	}
}
#endif

#if defined (CONFIG_AMEBALITE) || defined (CONFIG_AMEBADPLUS) || defined (CONFIG_AMEBAGREEN2)
const u8 UART_TX_FID[MAX_UART_INDEX] = {
	PINMUX_FUNCTION_UART0_TXD,
	PINMUX_FUNCTION_UART1_TXD,
	PINMUX_FUNCTION_UART2_TXD,
#if defined (CONFIG_AMEBALITE) || defined (CONFIG_AMEBAGREEN2)
	PINMUX_FUNCTION_UART3_TXD
#endif
};

const u8 UART_RX_FID[MAX_UART_INDEX] = {
	PINMUX_FUNCTION_UART0_RXD,
	PINMUX_FUNCTION_UART1_RXD,
	PINMUX_FUNCTION_UART2_RXD,
#if defined (CONFIG_AMEBALITE) || defined (CONFIG_AMEBAGREEN2)
	PINMUX_FUNCTION_UART3_RXD
#endif
};
#endif

#if defined (CONFIG_AMEBALITE) || defined (CONFIG_AMEBADPLUS) || defined (CONFIG_AMEBAGREEN2)
const u8 UART_CTS_FID[MAX_UART_INDEX] = {
	PINMUX_FUNCTION_UART0_CTS,
	NULL,
#if defined (CONFIG_AMEBALITE) || defined (CONFIG_AMEBAGREEN2)
	NULL,
	PINMUX_FUNCTION_UART3_CTS
#elif defined (CONFIG_AMEBADPLUS)
	PINMUX_FUNCTION_UART2_CTS
#endif
};

const u8 UART_RTS_FID[MAX_UART_INDEX] = {
	PINMUX_FUNCTION_UART0_RTS,
	NULL,
#if defined (CONFIG_AMEBALITE) || defined (CONFIG_AMEBAGREEN2)
	NULL,
	PINMUX_FUNCTION_UART3_RTS
#elif defined (CONFIG_AMEBADPLUS)
	PINMUX_FUNCTION_UART2_RTS
#endif
};
#endif

void whc_uart_irq_set(SerialIrq irq, u8 status)
{
	if (irq == RxIrq) {
		UART_INTConfig(WHC_UART_DEV, (RUART_BIT_ERBI | RUART_BIT_ELSI | RUART_BIT_ETOI), status); //RUART_BIT_ETOI RUART_BIT_ELSI
	}
}


bool whc_uart_dev_rxdma_init(u8 UartIndex, GDMA_InitTypeDef *GDMA_InitStruct, void *CallbackData,
							 IRQ_FUN CallbackFunc, u8 *pRxBuf, u32 RxCount)
{
	u8 GdmaChnl;
	UART_TypeDef *UARTx;

	assert_param(GDMA_InitStruct != NULL);

	DCache_CleanInvalidate((u32)pRxBuf, RxCount);

	GdmaChnl = GDMA_ChnlAlloc(0, (IRQ_FUN)CallbackFunc, (u32)CallbackData, INT_PRI_MIDDLE);
	if (GdmaChnl == 0xFF) {
		/* No Available DMA channel */
		return FALSE;
	}

	_memset((void *)GDMA_InitStruct, 0, sizeof(GDMA_InitTypeDef));

	UARTx = UART_DEV_TABLE[UartIndex].UARTx;
	if (RxCount == 0) {
		/*if length is 0, configure uart as the flow controller*/
		GDMA_InitStruct->GDMA_DIR = TTFCPeriToMem_PerCtrl;
		UARTx->MISCR |= RUART_BIT_RXDMA_OWNER;
	} else {
		/*if length isn`t 0, configure GDMA as the flow controller*/
		GDMA_InitStruct->GDMA_DIR = TTFCPeriToMem;
		UARTx->MISCR &= (~RUART_BIT_RXDMA_OWNER);
	}

	GDMA_InitStruct->GDMA_SrcHandshakeInterface = UART_DEV_TABLE[UartIndex].Rx_HandshakeInterface;
	GDMA_InitStruct->GDMA_SrcAddr = (u32)&UART_DEV_TABLE[UartIndex].UARTx->RBR_OR_UART_THR;
	GDMA_InitStruct->GDMA_DstAddr = (u32)(pRxBuf);
	GDMA_InitStruct->GDMA_Index   = 0;
	GDMA_InitStruct->GDMA_ChNum   = GdmaChnl;
	GDMA_InitStruct->GDMA_IsrType = (TransferType | ErrType);
	GDMA_InitStruct->GDMA_SrcMsize = MsizeOne;
	GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthOneByte;
	GDMA_InitStruct->GDMA_DstMsize = MsizeOne;
	GDMA_InitStruct->GDMA_DstDataWidth = TrWidthOneByte;
	GDMA_InitStruct->GDMA_SrcInc = NoChange;
	GDMA_InitStruct->GDMA_DstInc = IncType;
	GDMA_InitStruct->GDMA_BlockSize = RxCount;

	/* multi block close */
	GDMA_InitStruct->MuliBlockCunt  = 0;
	GDMA_InitStruct->GDMA_ReloadSrc = 0;
	GDMA_InitStruct->MaxMuliBlock   = 1;

	GDMA_Init(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, GDMA_InitStruct);
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);

	return TRUE;
}

bool whc_uart_dev_txdma_init(u8 UartIndex, GDMA_InitTypeDef *GDMA_InitStruct, void *CallbackData,
							 IRQ_FUN CallbackFunc, u8 *pTxBuf, u32 TxCount)
{
	u8 GdmaChnl;

	assert_param(GDMA_InitStruct != NULL);

	//DCache_CleanInvalidate((u32)pTxBuf, TxCount);

	GdmaChnl = GDMA_ChnlAlloc(0, (IRQ_FUN)CallbackFunc, (u32)CallbackData, INT_PRI_MIDDLE);
	if (GdmaChnl == 0xFF) {
		/*  No Available DMA channel */
		return FALSE;
	}

	_memset((void *)GDMA_InitStruct, 0, sizeof(GDMA_InitTypeDef));

	GDMA_InitStruct->MuliBlockCunt = 0;
	GDMA_InitStruct->MaxMuliBlock  = 1;

	GDMA_InitStruct->GDMA_DIR     = TTFCMemToPeri;
	GDMA_InitStruct->GDMA_DstHandshakeInterface = UART_DEV_TABLE[UartIndex].Tx_HandshakeInterface;
	GDMA_InitStruct->GDMA_DstAddr = (u32)&UART_DEV_TABLE[UartIndex].UARTx->RBR_OR_UART_THR;
	GDMA_InitStruct->GDMA_Index   = 0;
	GDMA_InitStruct->GDMA_ChNum   = GdmaChnl;
	GDMA_InitStruct->GDMA_IsrType = (TransferType | ErrType);

	/* set burst size 4B, due to src can't 8 */
	GDMA_InitStruct->GDMA_DstMsize = MsizeFour;
	GDMA_InitStruct->GDMA_DstDataWidth = TrWidthOneByte;
	GDMA_InitStruct->GDMA_DstInc = NoChange;
	GDMA_InitStruct->GDMA_SrcInc = IncType;

	if (((TxCount & 0x03) == 0) && (((u32)(pTxBuf) & 0x03) == 0)) {
		/* 4-bytes aligned, move 4 bytes each transfer */
		GDMA_InitStruct->GDMA_SrcMsize   = MsizeOne;
		GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthFourBytes;
		GDMA_InitStruct->GDMA_BlockSize = TxCount >> 2;
	} else {
		/* move 1 byte each transfer */
		GDMA_InitStruct->GDMA_SrcMsize   = MsizeFour;
		GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthOneByte;
		GDMA_InitStruct->GDMA_BlockSize = TxCount;
	}

	GDMA_InitStruct->GDMA_SrcAddr = (u32)(pTxBuf);

	GDMA_Init(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, GDMA_InitStruct);
	//GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);

	return TRUE;
}

u32 whc_uart_dev_irq(void *param)
{
	u32 uart_irq;
	(void)param;
	u32 ret = 0;
	uart_irq = UART_LineStatusGet(WHC_UART_DEV);
	u8 data_read;

	if (uart_irq & RUART_BIT_TIMEOUT_INT) {
		UART_INT_Clear(WHC_UART_DEV, RUART_BIT_TOICF);
	}

	if ((uart_irq & UART_ALL_RX_ERR)) {
		UART_INT_Clear(WHC_UART_DEV, RUART_BIT_RLSICF);
	}

	while (UART_Readable(WHC_UART_DEV)) {
		UART_CharGet(WHC_UART_DEV, &data_read);
		uart_priv.rx_size_done += 1;

		switch (uart_priv.rx_state) {
#if 0
		case WHC_UART_HOST_RX_DONE:
			uart_priv.rx_state = WHC_UART_HOST_RX_HEADER;
			uart_priv.rx_size_total = sizeof(struct whc_uart_hdr);
			uart_priv.rx_hdr[uart_priv.rx_size_total - 1] = data_read;
			break;
		case WHC_UART_HOST_RX_HEADER:
			uart_priv.rx_hdr[uart_priv.rx_size_total - 1] = data_read;
			if (uart_priv.rx_size_done == uart_uart_privize_total) {
				buf_hdr = (struct whc_uart_hdr *)uart_priv.rx_hdr;
				uart_priv.rx_state = WHC_UART_HOST_RX_PAYLOAD;
				uart_priv.rx_size_total = buf_hdr->buf_size;
				uart_priv.rx_size_done = 0;
			}
			break;
		case WHC_UART_HOST_RX_PAYLOAD:
			uart_priv.rx_buf[uart_priv.rx_size_done - 1] = data_read;
			if (uart_priv.rx_size_done == uart_priv.rx_size_total) {
				uart_priv.rx_state = WHC_UART_HOST_RX_END;
				uart_priv.rx_size_total = buf_hdr->buf_size;
				uart_priv.rx_size_done = 0;
				whc_uart_irq_set(RxIrq, DISABLE);
				rtos_sema_give(uart_priv.rxirq_sema);
			}
			break;
#else
		case WHC_UART_DEV_RX_DONE:
			uart_priv.rx_state = WHC_UART_DEV_RX_PAYLOAD;
			uart_priv.rx_size_total = UART_BUFSZ;
			uart_priv.rx_skb->data[uart_priv.rx_size_done - 1] = data_read;
			break;
		case WHC_UART_DEV_RX_PAYLOAD:
			uart_priv.rx_skb->data[uart_priv.rx_size_done - 1] = data_read;
			if (uart_priv.rx_size_done == uart_priv.rx_size_total) {
				uart_priv.rx_state = WHC_UART_DEV_RX_END;
				uart_priv.rx_size_done = 0;
				whc_uart_irq_set(RxIrq, DISABLE);
				rtos_sema_give(uart_priv.rxirq_sema);
			}
			break;
		case WHC_UART_DEV_RX_END:
			whc_uart_irq_set(RxIrq, DISABLE);
			break;
#endif
		default:
			DiagPrintf("do nothing %d \r\n", uart_priv.rx_state);
			/* do nothing  wait rx done */
			break;
		}
	}

	return ret;
}

void whc_uart_dev_trigger_rx_handle(void)
{
	if (uart_priv.wait_for_skb) {
		rtos_sema_give(uart_priv.free_skb_sema);
	}
}

u32 whc_uart_dev_dma_rx_done_cb(void *param)
{
	(void)param;
	struct sk_buff *new_skb = NULL, *rx_pkt = uart_priv.rx_skb;
	u32 event;
	struct whc_msg_info *msg_info;
	u8 *buf = NULL;
	u32 ret = 0;

#ifdef UART_RX_DMA
	GDMA_InitTypeDef *GDMA_InitStruct = &uart_priv.UARTRxGdmaInitStruct;

	/* disable gdma channel */
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, DISABLE);
	DCache_Invalidate((u32)rx_pkt->data, UART_BUFSZ);
#endif

	// /* get rx size */
	event = *(u32 *)(rx_pkt->data);

#ifdef WHC_UART_DEBUG
	dump_buf("dev rx", rx_pkt->data, 32);
#endif

	/* receives XMIT_PKTS */
	if (event == WHC_WIFI_EVT_XIMT_PKTS) {
retry:
		/* alloc new skb, blocked if no skb.
		block value <= wifi_user_config.skb_num_np - (rx_ring_buffer + wifi_user_config.rx_ampdu_num + 1 for rx_dma_buffer)
		e.g. 3 <= 14 - (4 + 4 + 1)*/
		if (((skbpriv.skb_buff_num - skbpriv.skb_buff_used) < 3) ||
			((new_skb = dev_alloc_skb(UART_BUFSZ, UART_SKB_RSVD_LEN)) == NULL)) {
			uart_priv.wait_for_skb = TRUE;

			/* resume pending queue to release skb */
			rtw_pending_q_resume();

			/* wait timeout to re-check skb, considering corner cases for wait_for_txbuf update */
			rtos_sema_take(uart_priv.free_skb_sema, 5);

			goto retry;
		} else {
			uart_priv.wait_for_skb = FALSE;
			uart_priv.rx_skb = new_skb;
		}
#ifdef UART_RX_DMA

		/* set new dest addr for RXDMA */
		DCache_Invalidate((u32)new_skb->data, UART_BUFSZ);
		GDMA_SetDstAddr(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, (u32)new_skb->data);

		/* restart RX DMA */
		GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);
#endif
		/* process rx data */
		msg_info = (struct whc_msg_info *) rx_pkt->data;
		skb_reserve(rx_pkt, sizeof(struct whc_msg_info) + msg_info->pad_len);
		skb_put(rx_pkt, msg_info->data_len);

		rx_pkt->dev = (void *)msg_info->wlan_idx;

		whc_uart_dev_event_int_hdl((u8 *)msg_info, rx_pkt);

	} else {
		/* receives EVENTS */
		buf = rtos_mem_zmalloc(UART_BUFSZ);	//TODO: optimize
		if (buf == NULL) {
			RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "%s, can't alloc buffer!!\n", __func__);
			goto exit;
		}


		memcpy(buf, uart_priv.rx_skb->data, UART_BUFSZ);
#ifdef UART_RX_DMA

		/* restart RX DMA */
		GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);
#endif
		/* free buf later, no need to modify skb. */
		whc_uart_dev_event_int_hdl(buf, NULL);
	}

exit:

	//whc_uart_irq_set(RxIrq, ENABLE);
	uart_priv.rx_state = WHC_UART_DEV_RX_DONE;
	whc_uart_irq_set(RxIrq, ENABLE);
	return ret;
}


void whc_uart_dev_dma_tx_done_cb(void *param)
{
	struct whc_uart_priv_t *whc_uart_priv = (struct whc_uart_priv_t *) param;
	struct whc_txbuf_info_t *whc_tx = container_of(whc_uart_priv->txbuf_info, struct whc_txbuf_info_t, txbuf_info);

	/* Dev TX complete, free tx skb or buffer */
	if (whc_tx->is_skb) {
		dev_kfree_skb_any((struct sk_buff *) whc_tx->ptr);
	} else {
		rtos_mem_free((u8 *)whc_tx->ptr);
	}
	rtos_mem_free((u8 *)whc_tx);

	whc_uart_priv->txbuf_info = NULL;

}

void whc_uart_dev_txdma_irq_task(void *pData)
{
	struct whc_uart_priv_t *whc_uart_priv = pData;

	for (;;) {
		/* Task blocked and wait the semaphore(events) here */
		rtos_sema_take(whc_uart_priv->txirq_sema, RTOS_MAX_TIMEOUT);
		whc_uart_dev_dma_tx_done_cb(whc_uart_priv);
	}
}

void whc_uart_dev_rxdma_irq_task(void *pData)
{
	struct whc_uart_priv_t *whc_uart_priv = pData;

	for (;;) {
		/* Task blocked and wait the semaphore(events) here */
		rtos_sema_take(whc_uart_priv->rxirq_sema, RTOS_MAX_TIMEOUT);
		whc_uart_dev_dma_rx_done_cb(whc_uart_priv);
	}
}

static u32 uart_get_idx(UART_TypeDef *Uartx)
{
	u32 i;

	for (i = 0; i < MAX_UART_INDEX; i++) {
		if (Uartx == UART_DEV_TABLE[i].UARTx) {
			return i;
		}
	}

	return 0xFF;
}

u32 whc_uart_dev_txdma_irq_handler(void *pData)
{
	GDMA_InitTypeDef *GDMA_InitStruct;
	u32 int_status;

	(void)pData;

	GDMA_InitStruct = &uart_priv.UARTTxGdmaInitStruct;

	/* check and clear TX DMA ISR */
	int_status = GDMA_ClearINT(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum);
	if (int_status & (TransferType)) {
		GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, DISABLE);
		rtos_sema_give(uart_priv.txirq_sema);
	}

	if (int_status & ErrType) {
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "uart txdma err occurs!!\n");
	}

	return 0;
}

u32 whc_uart_dev_rxdma_irq_handler(void *pData)
{
	GDMA_InitTypeDef *GDMA_InitStruct;
	u32 int_status;

	(void)pData;

	GDMA_InitStruct = &uart_priv.UARTRxGdmaInitStruct;

	/* check and clear RX DMA ISR */
	int_status = GDMA_ClearINT(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum);

	if (int_status & (TransferType)) {
		rtos_sema_give(uart_priv.rxirq_sema);
	}

	if (int_status & ErrType) {
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "uart rxdma err occurs!!\n");
	}

	return 0;
}

void whc_uart_dev_device_init(void)
{
	struct whc_uart_priv_t *whc_uart_priv = &uart_priv;
	UART_InitTypeDef *whc_uart_struct;
	u8 whc_uart_idx = uart_get_idx(WHC_UART_DEV);
	struct sk_buff *skb = NULL;

	whc_uart_priv->uart_idx = whc_uart_idx;
	whc_uart_struct = &whc_uart_priv->UART_InitStruct;


	rtos_mutex_create(&whc_uart_priv->tx_lock);
	rtos_mutex_create(&whc_uart_priv->rx_lock);

	rtos_sema_create(&whc_uart_priv->txirq_sema, 0, RTOS_SEMA_MAX_COUNT);
	rtos_sema_create(&whc_uart_priv->rxirq_sema, 0, RTOS_SEMA_MAX_COUNT);
	rtos_sema_create(&whc_uart_priv->free_skb_sema, 0, RTOS_SEMA_MAX_COUNT);


	/* enable uart clock and function */
	RCC_PeriphClockCmd(APBPeriph_UARTx[whc_uart_idx], APBPeriph_UARTx_CLOCK[whc_uart_idx], ENABLE);

#if defined(CONFIG_AMEBASMART)
	/* Configure UART TX and RX pin */
	Pinmux_Config(UART_TX, PINMUX_FUNCTION_UART);
	Pinmux_Config(UART_RX, PINMUX_FUNCTION_UART);
	Pinmux_Config(UART_CTS, PINMUX_FUNCTION_UART_RTSCTS);
	Pinmux_Config(UART_RTS, PINMUX_FUNCTION_UART_RTSCTS);
#elif defined(CONFIG_AMEBALITE) || defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBAGREEN2)
	/* Configure UART TX and RX pin */
	Pinmux_Config(UART_TX, UART_TX_FID[whc_uart_idx]);
	Pinmux_Config(UART_RX, UART_RX_FID[whc_uart_idx]);
	Pinmux_Config(UART_CTS, UART_CTS_FID[whc_uart_idx]);
	Pinmux_Config(UART_RTS, UART_RTS_FID[whc_uart_idx]);
#elif defined(CONFIG_AMEBAL2)
	RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "%s, need to check !!\n", __func__);
#endif

	PAD_PullCtrl(UART_TX, GPIO_PuPd_UP); // pull up Tx/Rx pin
	PAD_PullCtrl(UART_RX, GPIO_PuPd_UP); // pull up Tx/Rx pin

	UART_StructInit(whc_uart_struct);
	whc_uart_struct->Parity = RUART_PARITY_DISABLE;
#if defined(CONFIG_AMEBASMART)
	whc_uart_struct->RxFifoTrigLevel = UART_RX_FIFOTRIG_LEVEL_32BYTES;
#else
	whc_uart_struct->RxFifoTrigLevel = UART_RX_FIFOTRIG_LEVEL_1BYTES;
#endif
	/* uart as flow controller */
	whc_uart_struct->FlowControl = ENABLE;
	// whc_uart_struct->RxTimeOutCnt = 10;

	UART_Init(WHC_UART_DEV, whc_uart_struct);
	UART_SetBaud(WHC_UART_DEV, UART_BAUD);
	UART_RxCmd(WHC_UART_DEV, ENABLE);

	/* enable uart irq */
	whc_uart_irq_set(RxIrq, ENABLE);

	/*arrange Uart IRQ Number and handler*/
	InterruptRegister((IRQ_FUN)whc_uart_dev_irq, UART_DEV_TABLE[whc_uart_priv->uart_idx].IrqNum, (u32)&uart_priv, INT_PRI_MIDDLE);
	InterruptEn(UART_DEV_TABLE[whc_uart_priv->uart_idx].IrqNum, INT_PRI_MIDDLE);

	skb = dev_alloc_skb(UART_BUFSZ, UART_SKB_RSVD_LEN);
	if (skb == NULL || (((u32)skb->data) & 0x3) != 0) {
		RTK_LOGE(TAG_WLAN_INIC, "%s: alloc skb fail!\n", __func__);
		return;
	}

	whc_uart_priv->rx_skb = skb;
#ifdef UART_RX_DMA

	DCache_Invalidate((u32)skb->data, UART_BUFSZ);

	/* RxCount = 0, set UART as dma flow controller */
	/* default enable rx dma */
	whc_uart_dev_rxdma_init(whc_uart_idx, &whc_uart_priv->UARTRxGdmaInitStruct, WHC_UART_DEV, (IRQ_FUN)whc_uart_dev_rxdma_irq_handler, whc_uart_priv->rx_skb->data,
							UART_BUFSZ);
	/* Configure UART RX DMA burst size */
	UART_RXDMAConfig(WHC_UART_DEV, WHC_UART_RX_BURST_SIZE);

	/* Enable UART RX DMA */
	UART_RXDMACmd(WHC_UART_DEV, ENABLE);
	UART_ClearRxFifo(WHC_UART_DEV);
#endif

	/* setup tx dma */
	UART_TXDMAConfig(WHC_UART_DEV, WHC_UART_TX_BURST_SIZE);

	/* todo */
	//pmu_register_sleep_callback(PMU_WHC_WIFI, (PSM_HOOK_FUN)whc_uart_dev_suspend, NULL, (PSM_HOOK_FUN)whc_uart_dev_resume, NULL);

	/* Create irq task */
	if (rtos_task_create(NULL, "UART_RXDMA_IRQ_TASK", whc_uart_dev_rxdma_irq_task, (void *)whc_uart_priv, 1024 * 4, 7) != RTK_SUCCESS) {
		RTK_LOGE(TAG_WLAN_INIC, "Create UART_RXDMA_IRQ_TASK Err!!\n");
		return;
	}

	if (rtos_task_create(NULL, "UART_TXDMA_IRQ_TASK", whc_uart_dev_txdma_irq_task, (void *)whc_uart_priv, 1024 * 4, 7) != RTK_SUCCESS) {
		RTK_LOGE(TAG_WLAN_INIC, "Create UART_TXDMA_IRQ_TASK Err!!\n");
		return;
	}

	RTK_LOGI(TAG_WLAN_INIC, "UART init done!\n");
}


void whc_uart_dev_pkt_rx(u8 *rxbuf, struct sk_buff *skb)
{
	u32 event = *(u32 *)rxbuf;
	struct whc_api_info *ret_msg;
	(void) ret_msg;

	switch (event) {
	case WHC_WIFI_EVT_XIMT_PKTS:
		/* put the inic message to the queue */
		if (whc_msg_enqueue(skb, &dev_xmit_priv.xmit_queue) == RTK_FAIL) {
			break;
		}
		/* wakeup task */
		rtos_sema_give(dev_xmit_priv.xmit_sema);

		break;
#ifdef CONFIG_WHC_WIFI_API_PATH
	case WHC_WIFI_EVT_API_CALL:
		event_priv.rx_api_msg = rxbuf;
		rtos_sema_give(event_priv.task_wake_sema);

		break;
	case WHC_WIFI_EVT_API_RETURN:
		if (event_priv.b_waiting_for_ret) {
			event_priv.rx_ret_msg = rxbuf;
			rtos_sema_give(event_priv.api_ret_sema);
		} else {
			ret_msg = (struct whc_api_info *)rxbuf;
			RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_WARN, "too late to receive API ret, ID: 0x%x!\n", ret_msg->api_id);

			/* free rx buffer */
			rtos_mem_free((u8 *)ret_msg);
		}

		break;
#endif
	default:
#ifdef CONFIG_WHC_CMD_PATH
		whc_dev_pkt_rx_to_user(rxbuf, rxbuf, UART_BUFSZ);
#else
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "Event(%x) unknown!\n", event);
#endif
		break;
	}

}

u8 whc_uart_dev_tx_path_avail(void)
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

void whc_uart_set_txdma_len(u32 size, GDMA_InitTypeDef *GDMA_InitStruct)
{
	GDMA_TypeDef *GDMA = ((GDMA_TypeDef *) GDMA_BASE);
	GDMA->CH[GDMA_InitStruct->GDMA_ChNum].GDMA_CTLx_H = size >> 2;
}

void whc_uart_dev_send(struct whc_buf_info *pbuf)
{
	GDMA_InitTypeDef *GDMA_InitStruct = &uart_priv.UARTTxGdmaInitStruct;
	/* Call this function when receive pkt, call API or send API return value. Use locks to ensure exclusive execution. */
	rtos_mutex_take(uart_priv.tx_lock, MUTEX_WAIT_TIMEOUT);

	DCache_CleanInvalidate(pbuf->buf_addr, UART_BUFSZ);

	/* Initialize or Restart TXDMA */
	if (!uart_priv.txdma_initialized) {
		UART_TXDMACmd(WHC_UART_DEV, ENABLE);
		whc_uart_dev_txdma_init(uart_priv.uart_idx, GDMA_InitStruct, WHC_UART_DEV, (IRQ_FUN)whc_uart_dev_txdma_irq_handler, (u8 *)pbuf->buf_addr, UART_BUFSZ);
		uart_priv.txdma_initialized = 1;
	} else {
		GDMA_SetSrcAddr(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, pbuf->buf_addr);
	}

	uart_priv.txbuf_info = pbuf;
#ifdef WHC_UART_DEBUG
	dump_buf("dev tx", (u8 *)pbuf->buf_addr, 32);
#endif
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);

	while (uart_priv.txbuf_info != NULL) {
		rtos_time_delay_ms(1);
	}

	uart_priv.txbuf_info = pbuf;
	rtos_mutex_give(uart_priv.tx_lock);
}

u8 whc_uart_dev_bus_is_idle(void)
{
	/*Not yet implemented*/
	return TRUE;
}

/**
* @brief  send buf from upper
* @param  buf: data buf to be sent, must 4B aligned.
* @param  len: data len to be sent.
* @return none.
*/
void whc_uart_dev_send_data(u8 *buf, u32 len)
{
	struct whc_txbuf_info_t *whc_tx;

	if ((u32)buf & (DEV_DMA_ALIGN - 1)) {
		RTK_LOGE(TAG_WLAN_INIC, "Send Error, Data buf unaligned!");
		return;
	}

	/* construct struct whc_buf_info & whc_buf_info_t */
	whc_tx = (struct whc_txbuf_info_t *)rtos_mem_zmalloc(sizeof(struct whc_txbuf_info_t));

	whc_tx->txbuf_info.buf_allocated = whc_tx->txbuf_info.buf_addr = (u32)buf;
	whc_tx->txbuf_info.size_allocated = whc_tx->txbuf_info.buf_size = len;

	whc_tx->ptr = buf;
	whc_tx->is_skb = 0;

	/* send ret_msg + ret_val(buf, len) */
	whc_uart_dev_send(&whc_tx->txbuf_info);
}