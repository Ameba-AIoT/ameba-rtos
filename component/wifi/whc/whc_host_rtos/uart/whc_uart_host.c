#include "whc_host.h"

struct whc_uart_host_priv_t uart_host_priv = {0};
int whc_host_init_done;

/* fix rx buf for gdma, should big enough */
extern struct event_priv_t event_priv;
//#define WHC_UART_DEBUG 1
#ifdef WHC_UART_DEBUG
static void dump_buf(char *info, uint8_t *buf, uint32_t len)
{
	printf("%s", info);
	for (uint32_t i = 0; i < len; i++) {
		printf("%s0x%02X%s", i % 16 == 0 ? "\n     " : ",",
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

// void whc_uart_set_txdma_len(u32 size, GDMA_InitTypeDef *GDMA_InitStruct)
// {
// 	GDMA_TypeDef *GDMA = ((GDMA_TypeDef *) GDMA_BASE);
// 	GDMA->CH[GDMA_InitStruct->GDMA_ChNum].GDMA_CTLx_H = size >> 2;
// }

bool whc_uart_host_txdma_init(u8 UartIndex, GDMA_InitTypeDef *GDMA_InitStruct, void *CallbackData,
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
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);

	return TRUE;
}

bool whc_uart_host_rxdma_init(u8 UartIndex, GDMA_InitTypeDef *GDMA_InitStruct, void *CallbackData,
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

void whc_uart_host_dma_tx_done_cb(void)
{
	struct whc_txbuf_info_t *whc_tx = container_of(uart_host_priv.txbuf_info, struct whc_txbuf_info_t, txbuf_info);
	GDMA_InitTypeDef *GDMA_InitStruct = &uart_host_priv.UARTTxGdmaInitStruct;

	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, DISABLE);

	/* Dev TX complete, free tx skb or buffer */
	if (whc_tx->is_skb) {
		char *buf = whc_tx->ptr;
		*buf = 0;
	} else {
		rtos_mem_free((u8 *)whc_tx->ptr);
	}
	rtos_mem_free((u8 *)whc_tx);

	uart_host_priv.txbuf_info = NULL;

	//todo free txlock here -> crash?
}

void whc_uart_host_txdma_irq_task(void *pData)
{
	(void)pData;
	for (;;) {
		/* Task blocked and wait the semaphore(events) here */
		rtos_sema_take(uart_host_priv.txirq_sema, RTOS_MAX_TIMEOUT);
		whc_uart_host_dma_tx_done_cb();
	}
}

u32 whc_uart_host_txdma_irq_handler(void *pData)
{
	GDMA_InitTypeDef *GDMA_InitStruct;
	u32 int_status;

	(void)pData;

	GDMA_InitStruct = &uart_host_priv.UARTTxGdmaInitStruct;

	/* check and clear TX DMA ISR */
	int_status = GDMA_ClearINT(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum);
	if (int_status & (TransferType)) {
		rtos_sema_give(uart_host_priv.txirq_sema);
	}

	if (int_status & ErrType) {
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "uart txdma err occurs!!\n");
	}

	return 0;
}

void whc_uart_host_rx_handler(u8 *buf)
{
	struct whc_msg_info *msg_info = (struct whc_msg_info *)buf;
	char *data = (char *)(buf + sizeof(struct whc_msg_info) + msg_info->pad_len);
	/* allocate pbuf to store ethernet data from IPC. */
	struct pbuf *p_buf = pbuf_alloc(PBUF_RAW, msg_info->data_len, PBUF_POOL);
	struct pbuf *temp_buf = 0;

	if (p_buf == NULL) {
		RTK_LOGE(TAG_WLAN_INIC,  "%s: Alloc skb rx buf Err\n", __func__);
		//just send rsp when pbuf alloc fail
		return;
	}

	/* cpoy data from skb(ipc data) to pbuf(ether net data) */
	temp_buf = p_buf;
	while (temp_buf) {
		/* If tot_len > PBUF_POOL_BUFSIZE_ALIGNED, the skb will be
		 * divided into several pbufs. Therefore, there is a while to
		 * use to assigne data to pbufs.
		 */

		_memcpy(temp_buf->payload, data, temp_buf->len);
		data = data + temp_buf->len;
		temp_buf = temp_buf->next;
	}

	LwIP_ethernetif_recv_inic(msg_info->wlan_idx, p_buf);
}

int whc_uart_host_recv_process(void)
{
	int ret = 0;
	u32 event = *(u32 *)(uart_host_priv.rx_buf);
	u8 *recv_msg = uart_host_priv.rx_buf;
#ifdef CONFIG_WHC_WIFI_API_PATH
	struct whc_api_info *ret_msg;
	u8 *buf = NULL;
	int counter = 0;
#endif

#ifdef UART_RX_DMA
	GDMA_InitTypeDef *GDMA_InitStruct = &(uart_host_priv.UARTRxGdmaInitStruct);
	/* disable gdma channel */
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, DISABLE);
#endif

	uart_host_priv.rx_buf = rtos_mem_zmalloc(UART_BUFSZ);

#ifdef UART_RX_DMA
	DCache_CleanInvalidate((u32)uart_host_priv.rx_buf, UART_BUFSZ);
	GDMA_SetDstAddr(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, (u32)uart_host_priv.rx_buf);
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);
#endif

#ifdef WHC_UART_DEBUG
	dump_buf("host rx", (u8 *)recv_msg, 32);
#endif

	switch (event) {
	case WHC_WIFI_EVT_RECV_PKTS:
		whc_uart_host_rx_handler(recv_msg);
		break;
#ifdef CONFIG_WHC_WIFI_API_PATH
	case WHC_WIFI_EVT_API_CALL:
		buf = rtos_mem_zmalloc(UART_BUFSZ);
		memcpy(buf, recv_msg, UART_BUFSZ);
		/* wating for last done */
		counter = 0;
		while (event_priv.rx_api_msg) {
			rtos_time_delay_ms(1);
			counter ++;
			if (counter == 500) {
				counter = 0;
				RTK_LOGE(TAG_WLAN_INIC,  "%s: waiting for last event \n", __func__);
			}
		};
		event_priv.rx_api_msg = buf;
		rtos_sema_give(event_priv.task_wake_sema);
		break;
	case WHC_WIFI_EVT_API_RETURN:
		if (event_priv.b_waiting_for_ret) {
			buf = rtos_mem_zmalloc(UART_BUFSZ);
			memcpy(buf, recv_msg, UART_BUFSZ);
			while (event_priv.rx_ret_msg) {
				rtos_time_delay_ms(1);
				counter ++;
				if (counter == 500) {
					counter = 0;
					RTK_LOGE(TAG_WLAN_INIC,  "%s: waiting for last event \n", __func__);
				}
			};
			event_priv.rx_ret_msg = buf;
			/* unblock API calling func */
			rtos_sema_give(event_priv.api_ret_sema);
		} else {
			ret_msg = (struct whc_api_info *)(recv_msg);
			RTK_LOGE(TAG_WLAN_INIC, "too late to receive API ret, ID: 0x%x!\n", ret_msg->api_id);
		}
		break;
#endif

#ifndef CONFIG_WHC_CMD_PATH
	default:
		RTK_LOGE(TAG_WLAN_INIC, "%s: unknown event:%d\n", __func__, event);
		break;
#else
#ifndef UART_TODO
#endif
	default:
		/* RX DESC first 16bits for size */
		//size = *(u16 *)pbuf;
		whc_host_pkt_rx_to_user(recv_msg, (u32)UART_BUFSZ);
		break;
#endif
	}

	rtos_mem_free(recv_msg);
	uart_host_priv.rx_state = WHC_UART_HOST_RX_DONE;
	whc_uart_irq_set(RxIrq, ENABLE);

	return ret;
}

#ifdef UART_RX_DMA

void whc_uart_host_rxdma_irq_task(void *pData)
{
	(void)pData;
	for (;;) {
		/* Task blocked and wait the semaphore(events) here */
		rtos_sema_take(uart_host_priv.rxirq_sema, RTOS_MAX_TIMEOUT);
		DCache_Invalidate((u32)uart_host_priv.rx_buf, UART_BUFSZ);
		whc_uart_host_recv_process();
	}
}
#else
void whc_uart_host_rx_irq_task(void *pData)
{
	(void)pData;
	for (;;) {
		/* Task blocked and wait the semaphore(events) here */
		rtos_sema_take(uart_host_priv.rxirq_sema, RTOS_MAX_TIMEOUT);
		whc_uart_host_recv_process();
	}
}
#endif


u32 whc_uart_host_irq(void *param)
{
	u32 uart_irq;
	(void)param;
	u32 ret = 0;
	//struct whc_uart_hdr *buf_hdr;
	u8 data_read;

	uart_irq = UART_LineStatusGet(WHC_UART_DEV);
	if (uart_irq & RUART_BIT_TIMEOUT_INT) {
		UART_INT_Clear(WHC_UART_DEV, RUART_BIT_TOICF);
	}

	if ((uart_irq & UART_ALL_RX_ERR)) {
		UART_INT_Clear(WHC_UART_DEV, RUART_BIT_RLSICF);
	}

	while (UART_Readable(WHC_UART_DEV)) {
		switch (uart_host_priv.rx_state) {
#if 0
		case WHC_UART_HOST_RX_DONE:
			uart_host_priv.rx_state = WHC_UART_HOST_RX_HEADER;
			uart_host_priv.rx_size_total = sizeof(struct whc_uart_hdr);
			uart_host_priv.rx_hdr[uart_host_priv.rx_size_done - 1] = data_read;
			break;
		case WHC_UART_HOST_RX_HEADER:
			uart_host_priv.rx_hdr[uart_host_priv.rx_size_done - 1] = data_read;
			if (uart_host_priv.rx_size_done == uart_host_priv.rx_size_total) {
				buf_hdr = (struct whc_uart_hdr *)uart_host_priv.rx_hdr;
				uart_host_priv.rx_state = WHC_UART_HOST_RX_PAYLOAD;
				uart_host_priv.rx_size_total = buf_hdr->buf_size;
				uart_host_priv.rx_size_done = 0;
			}
			break;
		case WHC_UART_HOST_RX_PAYLOAD:
			uart_host_priv.rx_buf[uart_host_priv.rx_size_done - 1] = data_read;
			if (uart_host_priv.rx_size_done == uart_host_priv.rx_size_total) {
				uart_host_priv.rx_state = WHC_UART_HOST_RX_END;
				uart_host_priv.rx_size_total = buf_hdr->buf_size;
				uart_host_priv.rx_size_done = 0;
				whc_uart_irq_set(RxIrq, DISABLE);
				rtos_sema_give(uart_host_priv.rxirq_sema);
			}
			break;
#else
		case WHC_UART_HOST_RX_DONE:
			UART_CharGet(WHC_UART_DEV, &data_read);
			uart_host_priv.rx_size_done += 1;
			uart_host_priv.rx_state = WHC_UART_HOST_RX_PAYLOAD;
			uart_host_priv.rx_size_total = UART_BUFSZ;
			uart_host_priv.rx_buf[uart_host_priv.rx_size_done - 1] = data_read;
			break;
		case WHC_UART_HOST_RX_PAYLOAD:
			UART_CharGet(WHC_UART_DEV, &data_read);
			uart_host_priv.rx_size_done += 1;
			uart_host_priv.rx_buf[uart_host_priv.rx_size_done - 1] = data_read;
			if (uart_host_priv.rx_size_done == uart_host_priv.rx_size_total) {
				uart_host_priv.rx_state = WHC_UART_HOST_RX_END;
				//uart_host_priv.rx_size_total = buf_hdr->buf_size;
				uart_host_priv.rx_size_done = 0;
				whc_uart_irq_set(RxIrq, DISABLE);
				rtos_sema_give(uart_host_priv.rxirq_sema);
			}
			break;
		case WHC_UART_HOST_RX_END:
			whc_uart_irq_set(RxIrq, DISABLE);
			break;
#endif
		default:
			DiagPrintf("do nothing %d \r\n", uart_host_priv.rx_state);
			/* do nothing  wait rx done */
			break;
		}
	}

	return ret;
}

u32 whc_uart_host_rxdma_irq_handler(void *pData)
{
	GDMA_InitTypeDef *GDMA_InitStruct;
	u32 int_status;

	(void)pData;

	GDMA_InitStruct = &uart_host_priv.UARTRxGdmaInitStruct;
	/* check and clear RX DMA ISR */
	int_status = GDMA_ClearINT(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum);

	/* check and clear RX DMA ISR */
	if (int_status & (TransferType)) {
		//GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, DISABLE);
		rtos_sema_give(uart_host_priv.rxirq_sema);
	}

	if (int_status & ErrType) {
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "uart rxdma err occurs!!\n");
	}

	return 0;
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

static void whc_uart_host_drv_init(void)
{
	struct whc_uart_host_priv_t *whc_uart_priv = &uart_host_priv;
	UART_InitTypeDef *whc_uart_struct;
	u8 whc_uart_idx = uart_get_idx(WHC_UART_DEV);

	whc_uart_priv->uart_idx = whc_uart_idx;
	whc_uart_struct = &whc_uart_priv->UART_InitStruct;

	rtos_mutex_create(&whc_uart_priv->tx_lock);
	rtos_mutex_create(&whc_uart_priv->rx_lock);
	rtos_mutex_create(&whc_uart_priv->host_send);

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
	whc_uart_struct->RxFifoTrigLevel = UART_RX_FIFOTRIG_LEVEL_QUARTER;
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
	InterruptRegister((IRQ_FUN)whc_uart_host_irq, UART_DEV_TABLE[whc_uart_priv->uart_idx].IrqNum, (u32)&uart_host_priv, INT_PRI_MIDDLE);
	InterruptEn(UART_DEV_TABLE[whc_uart_priv->uart_idx].IrqNum, INT_PRI_MIDDLE);

	whc_uart_priv->rx_buf = rtos_mem_zmalloc(UART_BUFSZ);

#ifdef UART_RX_DMA
	DCache_CleanInvalidate((u32)whc_uart_priv->rx_buf, UART_BUFSZ);

	/* default enable rx dma */
	whc_uart_host_rxdma_init(whc_uart_idx, &whc_uart_priv->UARTRxGdmaInitStruct, WHC_UART_DEV, (IRQ_FUN)whc_uart_host_rxdma_irq_handler, whc_uart_priv->rx_buf,
							 UART_BUFSZ);

	/* Configure UART RX DMA burst size */
	UART_RXDMAConfig(WHC_UART_DEV, WHC_UART_RX_BURST_SIZE);
	/* Enable UART RX DMA */
	UART_RXDMACmd(WHC_UART_DEV, ENABLE);

	/* Create irq task */
	if (rtos_task_create(NULL, "UART_RXDMA_IRQ_TASK", whc_uart_host_rxdma_irq_task, (void *)whc_uart_priv, 1024 * 4, 7) != RTK_SUCCESS) {
		RTK_LOGE(TAG_WLAN_INIC, "Create UART_RXDMA_IRQ_TASK Err!!\n");
		return;
	}
#else

	if (rtos_task_create(NULL, "UART_RX_IRQ_TASK", whc_uart_host_rx_irq_task, (void *)whc_uart_priv, 1024 * 4, 7) != RTK_SUCCESS) {
		RTK_LOGE(TAG_WLAN_INIC, "Create UART_RXDMA_IRQ_TASK Err!!\n");
		return;
	}

#endif

	/* todo */
	//pmu_register_sleep_callback(PMU_WHC_WIFI, (PSM_HOOK_FUN)whc_uart_host_suspend, NULL, (PSM_HOOK_FUN)whc_uart_host_resume, NULL);

	if (rtos_task_create(NULL, "UART_TXDMA_IRQ_TASK", whc_uart_host_txdma_irq_task, (void *)whc_uart_priv, 1024 * 4, 7) != RTK_SUCCESS) {
		RTK_LOGE(TAG_WLAN_INIC, "Create UART_TXDMA_IRQ_TASK Err!!\n");
		return;
	}

#ifdef CONFIG_WHC_WIFI_API_PATH
	/* init event priv */
	rtos_sema_create(&(event_priv.task_wake_sema), 0, 0xFFFFFFFF);
	rtos_sema_create(&(event_priv.api_ret_sema), 0, 0xFFFFFFFF);
	rtos_sema_create(&(event_priv.send_mutex), 1, 1);
	rtos_sema_give(event_priv.send_mutex);

	/* Initialize the event task */
	if (RTK_SUCCESS != rtos_task_create(NULL, (const char *const)"whc_host_api_task", (rtos_task_function_t)whc_host_api_task, NULL,
										WIFI_STACK_SIZE_INIC_IPC_HST_API, 3)) {
		RTK_LOGE(TAG_WLAN_INIC, "Create api_host_task Err\n");
	}
#endif

	RTK_LOGI(TAG_WLAN_INIC, "UART init done!\n");
}

/**
 * @brief  to initialize the whc host.
 * @param  none.
 * @return none.
 */
void whc_uart_host_init(void)
{
	whc_uart_host_drv_init();

	whc_host_init_done = 1;
}

void whc_uart_host_send_data(struct whc_buf_info *pbuf)
{
	GDMA_InitTypeDef *GDMA_InitStruct = &uart_host_priv.UARTTxGdmaInitStruct;

	/* Call this function when receive pkt, call API or send API return value. Use locks to ensure exclusive execution. */
	rtos_mutex_take(uart_host_priv.tx_lock, MUTEX_WAIT_TIMEOUT);
	DCache_CleanInvalidate(pbuf->buf_addr, pbuf->buf_size);

#ifdef WHC_UART_DEBUG
	dump_buf("host send", (u8 *)pbuf->buf_addr, 32);
#endif
	/* Initialize or Restart TXDMA */
	if (!uart_host_priv.txdma_initialized) {
		/* setup tx dma */
		UART_TXDMAConfig(WHC_UART_DEV, WHC_UART_TX_BURST_SIZE);
		whc_uart_host_txdma_init(uart_host_priv.uart_idx, GDMA_InitStruct, WHC_UART_DEV, (IRQ_FUN)whc_uart_host_txdma_irq_handler, (u8 *)pbuf->buf_addr, UART_BUFSZ);
		UART_TXDMACmd(WHC_UART_DEV, ENABLE);
		uart_host_priv.txdma_initialized = 1;
	} else {
		GDMA_SetSrcAddr(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, pbuf->buf_addr);
		GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);
	}

	while (uart_host_priv.txbuf_info != NULL) {
		rtos_time_delay_ms(1);
	}

	uart_host_priv.txbuf_info = pbuf;
	rtos_mutex_give(uart_host_priv.tx_lock);
}