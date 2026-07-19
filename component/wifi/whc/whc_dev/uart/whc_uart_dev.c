#include "whc_dev.h"

struct whc_uart_priv_t uart_priv = {0};

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

/* move to gdma api later*/
void whc_uart_set_dma_len(u32 size, GDMA_InitTypeDef *GDMA_InitStruct)
{
	GDMA_TypeDef *GDMA = ((GDMA_TypeDef *) GDMA_BASE);
	GDMA->CH[GDMA_InitStruct->GDMA_ChNum].GDMA_CTLx_H = size;
}

void whc_uart_irq_set(SerialIrq irq, u8 status)
{
	(void)irq;
	UART_INTConfig(WHC_UART_DEV, (RUART_BIT_ERBI | RUART_BIT_ELSI | RUART_BIT_ETOI), status); //RUART_BIT_ETOI RUART_BIT_ELSI
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

	return TRUE;
}

bool whc_uart_dev_txdma_init(u8 UartIndex, GDMA_InitTypeDef *GDMA_InitStruct, void *CallbackData,
							 IRQ_FUN CallbackFunc, u8 *pTxBuf, u32 TxCount)
{
	u8 GdmaChnl;

	assert_param(GDMA_InitStruct != NULL);
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

	/* move 1 byte each transfer */
	GDMA_InitStruct->GDMA_SrcMsize   = MsizeFour;
	GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthOneByte;
	GDMA_InitStruct->GDMA_BlockSize = TxCount;

	GDMA_InitStruct->GDMA_SrcAddr = (u32)(pTxBuf);

	GDMA_Init(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, GDMA_InitStruct);

	return TRUE;
}

void whc_uart_dev_send_ack(void)
{
	struct whc_uart_hdr txhdr = {0};
	u8 len = 0;
	u8 *ptr = (u8 *)&txhdr;
	txhdr.buf_size = sizeof(struct whc_uart_hdr);
	txhdr.subtype = WHC_UART_ACK;

	rtos_sema_take(uart_priv.tx_lock, RTOS_MAX_TIMEOUT);

	while (len < sizeof(struct whc_uart_hdr)) {
		if (UART_Writable(WHC_UART_DEV)) {
			UART_CharPut(WHC_UART_DEV, ptr[len]);
			len++;
		}
	}
	rtos_sema_give(uart_priv.tx_lock);
}

u32 whc_uart_dev_irq_handler(void *param)
{
	GDMA_InitTypeDef *GDMA_InitStruct = &uart_priv.UARTRxGdmaInitStruct;
	struct whc_uart_hdr *buf_hdr = &uart_priv.rx_hdr;
	u8 *hdr_ptr = (u8 *)(&uart_priv.rx_hdr);
	u32 uart_irq = UART_LineStatusGet(WHC_UART_DEV);
	u8 data_read;

	(void)param;

	if (uart_irq & RUART_BIT_TIMEOUT_INT) {
		UART_INT_Clear(WHC_UART_DEV, RUART_BIT_TOICF);
	}

	if ((uart_irq & UART_ALL_RX_ERR)) {
		UART_INT_Clear(WHC_UART_DEV, RUART_BIT_RLSICF);
	}

	while (UART_Readable(WHC_UART_DEV)) {
		switch (uart_priv.rx_state) {
		case WHC_UART_DEV_RX_IDLE:
			/* reset state and hdr */
			_memset(&(uart_priv.rx_hdr), 0, sizeof(struct whc_uart_hdr));
			uart_priv.rx_size_done  = 0;
			uart_priv.rx_state = WHC_UART_DEV_RX_HANDSHAKE;
			uart_priv.rx_size_total = sizeof(struct whc_uart_hdr);
			break;
		case WHC_UART_DEV_RX_HANDSHAKE:
			UART_CharGet(WHC_UART_DEV, &data_read);
			hdr_ptr[uart_priv.rx_size_done++] = data_read;

			if (uart_priv.rx_size_done == uart_priv.rx_size_total) {
				if ((uart_priv.rx_size_total) == 0 || (uart_priv.rx_size_total > UART_BUFSZ)) {
					RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "payload len err %d\r\n", uart_priv.rx_size_total);
				}
				if (buf_hdr->subtype == WHC_UART_HDR) {
					uart_priv.rx_state = WHC_UART_DEV_RX_HEADER;
					uart_priv.checksum = buf_hdr->checksum;
					uart_priv.payload_len = buf_hdr->buf_size;
					uart_priv.rx_size_done = 0;
					uart_priv.rx_size_total = 0;
					whc_uart_irq_set(RxIrq, DISABLE);
					rtos_sema_give(uart_priv.rx_wakeup_sema);
					goto exit;
				} else if (buf_hdr->subtype == WHC_UART_ACK) {
					uart_priv.rx_size_done = 0;
					uart_priv.rx_size_total = 0;
					uart_priv.rx_state = WHC_UART_DEV_RX_IDLE;
					uart_priv.tx_waiting_ack = 0;
					rtos_sema_give(uart_priv.hdr_reply);
				}
			}
			break;
		case WHC_UART_DEV_WAIT_PAYLOAD:
			if (uart_priv.tx_waiting_ack) {
				if (uart_priv.rx_size_done < sizeof(struct whc_uart_hdr)) {
					UART_CharGet(WHC_UART_DEV, &data_read);
					hdr_ptr[uart_priv.rx_size_done++] = data_read;

				} else if (uart_priv.rx_size_done == sizeof(struct whc_uart_hdr)) {
					uart_priv.rx_size_done = 0;
					if (buf_hdr->subtype != WHC_UART_ACK) {
						RTK_LOGE(TAG_WLAN_INIC, "err hdr\n");
					}
					uart_priv.tx_waiting_ack = 0;
					rtos_sema_give(uart_priv.hdr_reply);

					/* payload bytes already in flight; enable DMA immediately */
					whc_uart_set_dma_len(uart_priv.payload_len, GDMA_InitStruct);
					GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);
					uart_priv.rx_state = WHC_UART_DEV_RX_PAYLOAD;
					whc_uart_irq_set(RxIrq, DISABLE);
				}
			}
			break;
		default:
			goto exit;
			/* do nothing  wait rx done */
			break;
		}
	}
exit:
	UART_INT_Clear(WHC_UART_DEV, uart_irq);
	return 0;
}

void whc_uart_dev_trigger_rx_handle(void)
{
	if (uart_priv.wait_for_skb) {
		rtos_sema_give(uart_priv.free_skb_sema);
	}
}

u32 whc_uart_dev_rx_done_cb(void *param)
{
	(void)param;
	struct sk_buff *new_skb = NULL, *rx_pkt = uart_priv.rx_skb;
	u32 event;
	struct whc_msg_info *msg_info;
	u32 ret = 0;

	GDMA_InitTypeDef *GDMA_InitStruct = &uart_priv.UARTRxGdmaInitStruct;

	/* disable gdma channel */
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, DISABLE);
	DCache_Invalidate((u32)rx_pkt->data, UART_BUFSZ);

	event = *(u32 *)(rx_pkt->data);

#ifdef WHC_UART_DEBUG
	dump_buf("dev rx", rx_pkt->data, 16);
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

			/* wait timeout to re-check skb, considering corner cases for wait_for_txbuf update */
			rtos_sema_take(uart_priv.free_skb_sema, 5);

			goto retry;
		} else {
			uart_priv.wait_for_skb = FALSE;
			uart_priv.rx_skb = new_skb;
		}
		/* set new dest addr for RXDMA */
		DCache_Invalidate((u32)new_skb->data, UART_BUFSZ);
		GDMA_SetDstAddr(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, (u32)new_skb->data);
		/* process rx data */
		msg_info = (struct whc_msg_info *) rx_pkt->data;
		skb_reserve(rx_pkt, sizeof(struct whc_msg_info) + msg_info->pad_len);
		skb_put(rx_pkt, msg_info->data_len);

		rx_pkt->dev = (void *)((u32)msg_info->wlan_idx);

		whc_dev_event_int_hdl((u8 *)msg_info, rx_pkt);
	} else {
		/* receives EVENTS */
		whc_dev_dispatch_event_copy(uart_priv.rx_skb->data, uart_priv.payload_len);
	}

	return ret;
}


void whc_uart_dev_dma_tx_done_cb(void *param)
{
	struct whc_uart_priv_t *whc_uart_priv = (struct whc_uart_priv_t *) param;
	struct whc_txbuf_info_t *whc_tx = container_of(whc_uart_priv->txbuf_info, struct whc_txbuf_info_t, txbuf_info);

	whc_dev_free_txbuf(whc_tx);

	whc_uart_priv->txbuf_info = NULL;
	rtos_sema_give(uart_priv.tx_lock);
	rtos_sema_give(uart_priv.session_lock);
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

void whc_uart_dev_rx_process_task(void *pData)
{
	struct whc_uart_priv_t *whc_uart_priv = pData;
	GDMA_InitTypeDef *GDMA_InitStruct = &uart_priv.UARTRxGdmaInitStruct;

	for (;;) {
		/* Task blocked and wait the semaphore(events) here */
		rtos_sema_take(whc_uart_priv->rx_wakeup_sema, RTOS_MAX_TIMEOUT);
		switch (uart_priv.rx_state) {
		case WHC_UART_DEV_RX_HEADER:
			whc_uart_dev_send_ack();
			if (uart_priv.tx_waiting_ack == 0) {
				whc_uart_set_dma_len(uart_priv.payload_len, GDMA_InitStruct);
				GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);
				uart_priv.rx_state = WHC_UART_DEV_RX_PAYLOAD;
			} else {
				uart_priv.rx_state = WHC_UART_DEV_WAIT_PAYLOAD;
			}
			break;
		default:
			whc_uart_dev_rx_done_cb(whc_uart_priv);
			uart_priv.rx_state = WHC_UART_DEV_RX_IDLE;
			break;
		}
		if (uart_priv.rx_state != WHC_UART_DEV_RX_PAYLOAD) {
			whc_uart_irq_set(RxIrq, ENABLE);
		}
	}
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
		rtos_sema_give(uart_priv.rx_wakeup_sema);
	}

	if (int_status & ErrType) {
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "uart rxdma err occurs!!\n");
	}

	return 0;
}

void whc_uart_dev_init(void)
{
	struct whc_uart_priv_t *whc_uart_priv = &uart_priv;
	UART_InitTypeDef *whc_uart_struct;
	u8 whc_uart_idx = uart_get_idx(WHC_UART_DEV);
	struct sk_buff *skb = NULL;

	whc_uart_priv->uart_idx = whc_uart_idx;
	whc_uart_struct = &whc_uart_priv->UART_InitStruct;

	rtos_sema_create(&whc_uart_priv->tx_lock, 1, 1);
	rtos_sema_create(&whc_uart_priv->rx_lock, 1, 1);
	rtos_sema_create(&whc_uart_priv->hdr_reply, 0, 1);
	rtos_sema_create(&whc_uart_priv->session_lock, 1, 1);

	rtos_sema_create(&whc_uart_priv->txirq_sema, 0, RTOS_SEMA_MAX_COUNT);
	rtos_sema_create(&whc_uart_priv->rx_wakeup_sema, 0, RTOS_SEMA_MAX_COUNT);
	rtos_sema_create(&whc_uart_priv->free_skb_sema, 0, RTOS_SEMA_MAX_COUNT);

	/* enable uart clock and function */
	RCC_PeriphClockCmd(APBPeriph_UARTx[whc_uart_idx], APBPeriph_UARTx_CLOCK[whc_uart_idx], ENABLE);

#if defined(CONFIG_AMEBASMART)
	Pinmux_Config(UART_TX, PINMUX_FUNCTION_UART);
	Pinmux_Config(UART_RX, PINMUX_FUNCTION_UART);
#elif defined(CONFIG_AMEBALITE) || defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBAGREEN2)
	Pinmux_Config(UART_TX, UART_TX_FID[whc_uart_idx]);
	Pinmux_Config(UART_RX, UART_RX_FID[whc_uart_idx]);
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

	UART_Init(WHC_UART_DEV, whc_uart_struct);
	UART_SetBaud(WHC_UART_DEV, UART_BAUD);
	UART_RxCmd(WHC_UART_DEV, ENABLE);

	/* enable uart irq */
	whc_uart_irq_set(RxIrq, ENABLE);

	/*arrange Uart IRQ Number and handler*/
	InterruptRegister((IRQ_FUN)whc_uart_dev_irq_handler, UART_DEV_TABLE[whc_uart_priv->uart_idx].IrqNum, (u32)&uart_priv, INT_PRI_MIDDLE);
	InterruptEn(UART_DEV_TABLE[whc_uart_priv->uart_idx].IrqNum, INT_PRI_MIDDLE);

	skb = dev_alloc_skb(UART_BUFSZ, UART_SKB_RSVD_LEN);
	if (skb == NULL || (((u32)skb->data) & 0x3) != 0) {
		RTK_LOGE(TAG_WLAN_INIC, "%s: alloc skb fail!\n", __func__);
		return;
	}
	whc_uart_priv->rx_skb = skb;

	DCache_Invalidate((u32)skb->data, UART_BUFSZ);

	/* default enable rx dma */
	whc_uart_dev_rxdma_init(whc_uart_idx, &whc_uart_priv->UARTRxGdmaInitStruct, WHC_UART_DEV, (IRQ_FUN)whc_uart_dev_rxdma_irq_handler, whc_uart_priv->rx_skb->data,
							UART_BUFSZ);
	/* Configure UART RX DMA burst size */
	UART_RXDMAConfig(WHC_UART_DEV, WHC_UART_RX_BURST_SIZE);

	/* Enable UART RX DMA */
	UART_RXDMACmd(WHC_UART_DEV, ENABLE);
	UART_ClearRxFifo(WHC_UART_DEV);

	/* todo */
	//pmu_register_sleep_callback(PMU_WHC_WIFI, (PSM_HOOK_FUN)whc_uart_dev_suspend, NULL, (PSM_HOOK_FUN)whc_uart_dev_resume, NULL);

	/* Create irq task */
	if (rtos_task_create(NULL, "UART_RXDMA_IRQ_TASK", whc_uart_dev_rx_process_task, (void *)whc_uart_priv, 460, 7) != RTK_SUCCESS) {
		RTK_LOGE(TAG_WLAN_INIC, "Create UART_RXDMA_IRQ_TASK Err!!\n");
		return;
	}

	if (rtos_task_create(NULL, "UART_TXDMA_IRQ_TASK", whc_uart_dev_txdma_irq_task, (void *)whc_uart_priv, 220, 6) != RTK_SUCCESS) {
		RTK_LOGE(TAG_WLAN_INIC, "Create UART_TXDMA_IRQ_TASK Err!!\n");
		return;
	}

	RTK_LOGI(TAG_WLAN_INIC, "UART init done!\n");
}

void whc_uart_dev_send_hdr(u16 size, u32 checksum)
{
	struct whc_uart_hdr txhdr = {0};
	u8 len;
	u8 *ptr = (u8 *)&txhdr;
	int ret;

	txhdr.buf_size = size;
	txhdr.subtype = WHC_UART_HDR;
	txhdr.checksum = checksum;

	len = 0;
retry:
	while ((uart_priv.rx_state == WHC_UART_DEV_WAIT_PAYLOAD) || (uart_priv.rx_state == WHC_UART_DEV_RX_HEADER)) {
		rtos_time_delay_ms(1);
	}
	rtos_sema_take(uart_priv.tx_lock, RTOS_MAX_TIMEOUT);
	if ((uart_priv.rx_state == WHC_UART_DEV_WAIT_PAYLOAD) || (uart_priv.rx_state == WHC_UART_DEV_RX_HEADER)) {
		rtos_sema_give(uart_priv.tx_lock);
		goto retry;
	}

	while (len < sizeof(struct whc_uart_hdr)) {
		if (UART_Writable(WHC_UART_DEV)) {
			UART_CharPut(WHC_UART_DEV, ptr[len]);
			len++;
		}
	}
	uart_priv.tx_waiting_ack = 1;
	rtos_sema_give(uart_priv.tx_lock);

again:
	/* block until rx header reply */
	ret = rtos_sema_take(uart_priv.hdr_reply, 5000);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, 	"fail to get hdr lock\n");
		goto again;
	}

}

void whc_uart_dev_send(u8 *buf, u16 len, void *buf_alloc, u8 is_skb)
{
	GDMA_InitTypeDef *GDMA_InitStruct = &uart_priv.UARTTxGdmaInitStruct;
	struct whc_txbuf_info_t *buf_info = NULL;
	struct whc_buf_info *pbuf;
	u32 i, checksum = 0;
	u32 *data = (u32 *)buf;

	if ((u32)buf & (DEV_DMA_ALIGN - 1)) {
		RTK_LOGE(TAG_WLAN_INIC, "Send Error, Data buf unaligned!");
		goto drop;
	}

	buf_info = whc_dev_alloc_buf_info(buf, len, buf_alloc, is_skb);
	if (!buf_info) {
		goto drop;
	}
	pbuf = &buf_info->txbuf_info;

	for (i = 0; i < pbuf->buf_size / sizeof(u32); i++) {
		checksum += data[i];
	}
	/* Serialize the full HDR→payload session; tx_lock is acquired only during actual byte transmission. */
	rtos_sema_take(uart_priv.session_lock, RTOS_MAX_TIMEOUT);
	DCache_CleanInvalidate(pbuf->buf_addr, pbuf->buf_size);

	/* send_hdr: takes tx_lock, sends HDR bytes, releases tx_lock, then waits for ACK_REPLY */
	whc_uart_dev_send_hdr(pbuf->buf_size, checksum);

	/* ACK received — take tx_lock for payload DMA */
	rtos_sema_take(uart_priv.tx_lock, RTOS_MAX_TIMEOUT);

	/* Initialize or Restart TXDMA */
	if (!uart_priv.txdma_initialized) {
		UART_TXDMAConfig(WHC_UART_DEV, WHC_UART_TX_BURST_SIZE);
		whc_uart_dev_txdma_init(uart_priv.uart_idx, GDMA_InitStruct, WHC_UART_DEV, (IRQ_FUN)whc_uart_dev_txdma_irq_handler, (u8 *)pbuf->buf_addr, UART_BUFSZ);
		UART_TXDMACmd(WHC_UART_DEV, ENABLE);
		uart_priv.txdma_initialized = 1;
	} else {
		GDMA_SetSrcAddr(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, pbuf->buf_addr);
	}
	whc_uart_set_dma_len(pbuf->buf_size, GDMA_InitStruct);
	uart_priv.txbuf_info = pbuf;
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);
#ifdef WHC_UART_DEBUG
	dump_buf("dev tx", (u8 *)pbuf->buf_addr, 32);
#endif

	return;

drop:
	/* Dev drop tx, free tx skb or buffer */
	if (is_skb) {
		dev_kfree_skb_any((struct sk_buff *) buf_alloc);
	} else {
		rtos_mem_free((u8 *)buf_alloc);
	}
}

u8 whc_uart_dev_bus_is_idle(void)
{
	/*Not yet implemented*/
	return TRUE;
}
