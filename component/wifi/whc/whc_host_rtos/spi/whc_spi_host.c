#include "whc_host.h"

struct whc_spi_host_priv_t spi_host_priv = {0};

extern struct event_priv_t event_priv;
int whc_host_init_done;

extern void whc_host_api_task(void);
void(*bt_inic_spi_recv_host_ptr)(uint8_t *buffer, uint16_t len);

bool whc_spi_host_rxgdma_init(
	u8 Index,
	GDMA_InitTypeDef *GDMA_InitStruct,
	void *CallbackData,
	IRQ_FUN CallbackFunc,
	u8  *pRxData,
	u32 Length
)
{
	SPI_TypeDef *SPIx = SPI_DEV_TABLE[Index].SPIx;
	u32 DataFrameSize = SSI_GetDataFrameSize(SPIx);
	u8 GdmaChnl;

	assert_param(GDMA_InitStruct != NULL);

	DCache_CleanInvalidate((u32) pRxData, Length);

	GdmaChnl = GDMA_ChnlAlloc(0, CallbackFunc, (u32)CallbackData, INT_PRI_MIDDLE);
	if (GdmaChnl == 0xFF) {
		// No Available DMA channel
		return FALSE;
	}

	GDMA_StructInit(GDMA_InitStruct);
	GDMA_InitStruct->GDMA_DIR       = TTFCPeriToMem;
	GDMA_InitStruct->GDMA_ReloadSrc = 0;
	GDMA_InitStruct->GDMA_SrcHandshakeInterface    = SPI_DEV_TABLE[Index].Rx_HandshakeInterface;
	GDMA_InitStruct->GDMA_SrcAddr   = (u32)&SPI_DEV_TABLE[Index].SPIx->SPI_DRx;
	GDMA_InitStruct->GDMA_Index     = 0;
	GDMA_InitStruct->GDMA_ChNum     = GdmaChnl;
	GDMA_InitStruct->GDMA_IsrType   = (BlockType | TransferType | ErrType);

	GDMA_InitStruct->GDMA_SrcMsize = MsizeEight;
	GDMA_InitStruct->GDMA_DstMsize = MsizeFour;
	GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthTwoBytes;
	GDMA_InitStruct->GDMA_DstDataWidth = TrWidthFourBytes;
	GDMA_InitStruct->GDMA_DstInc = IncType;
	GDMA_InitStruct->GDMA_SrcInc = NoChange;

	/*  Cofigure GDMA transfer */
	if (DataFrameSize > 8) {
		/*  16~9 bits mode */
		GDMA_InitStruct->GDMA_SrcMsize = MsizeFour;
		GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthTwoBytes;
		GDMA_InitStruct->GDMA_BlockSize = Length >> 1;

		if (((Length & 0x03) == 0) && (((u32)(pRxData) & 0x03) == 0)) {
			/*  4-bytes aligned, move 4 bytes each transfer */
			GDMA_InitStruct->GDMA_DstMsize = MsizeFour;
			GDMA_InitStruct->GDMA_DstDataWidth = TrWidthFourBytes;
		} else if (((Length & 0x01) == 0) && (((u32)(pRxData) & 0x01) == 0)) {
			/*  2-bytes aligned, move 2 bytes each transfer */
			GDMA_InitStruct->GDMA_DstMsize = MsizeEight;
			GDMA_InitStruct->GDMA_DstDataWidth = TrWidthTwoBytes;
		} else {
			RTK_LOGE(TAG_WLAN_INIC, "SSI_RXGDMA_Init: Aligment Err: pTxData=%p,  Length=%lu\n", pRxData, Length);
			return FALSE;
		}
	} else {
		/*  8~4 bits mode */
		GDMA_InitStruct->GDMA_SrcMsize = MsizeFour;
		GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthOneByte;
		GDMA_InitStruct->GDMA_BlockSize = Length;
		if (((Length & 0x03) == 0) && (((u32)(pRxData) & 0x03) == 0)) {
			/*  4-bytes aligned, move 4 bytes each transfer */
			GDMA_InitStruct->GDMA_DstMsize = MsizeOne;
			GDMA_InitStruct->GDMA_DstDataWidth = TrWidthFourBytes;
		} else {
			GDMA_InitStruct->GDMA_DstMsize = MsizeFour;
			GDMA_InitStruct->GDMA_DstDataWidth = TrWidthOneByte;
		}
	}

	/*DMA block transfer size up to 65535 on Dplus, the value of this parameter must be no more than 65535*/

	GDMA_InitStruct->GDMA_DstAddr = (u32)pRxData;

	/*  Enable GDMA for RX */
	GDMA_Init(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, GDMA_InitStruct);
	//GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);

	return TRUE;
}

void whc_spi_host_rx_handler(u8 *buf)
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

	if (p_buf != NULL) {
		LwIP_ethernetif_recv_inic(msg_info->wlan_idx, p_buf);
	}
}

int whc_spi_host_recv_process(void)
{
	int ret = 0;
	u32 event = *(u32 *)(spi_host_priv.rx_buf + SIZE_RX_DESC);
	GDMA_InitTypeDef *GDMA_InitStruct = &(spi_host_priv.SSIRxGdmaInitStruct);
	u8 *recv_msg = spi_host_priv.rx_buf;

#ifdef CONFIG_WHC_CMD_PATH
	struct whc_cmd_path_hdr *hdr = NULL;
#endif

#ifdef CONFIG_WHC_WIFI_API_PATH
	struct whc_api_info *ret_msg;
	u8 *buf = NULL;
	int counter = 0;
#endif

	/* disable gdma channel */
	spi_host_priv.rx_buf = rtos_mem_zmalloc(SPI_BUFSZ);
	DCache_CleanInvalidate((u32)spi_host_priv.rx_buf, SPI_BUFSZ);
	GDMA_SetDstAddr(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, (u32)spi_host_priv.rx_buf);
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);
	rtos_sema_give(spi_host_priv.host_recv_done);

	switch (event) {
	case WHC_WIFI_EVT_RECV_PKTS:
		whc_spi_host_rx_handler(recv_msg);
		break;
#ifdef CONFIG_WHC_WIFI_API_PATH
	case WHC_WIFI_EVT_API_CALL:
		buf = rtos_mem_zmalloc(SPI_BUFSZ);
		memcpy(buf, recv_msg, SPI_BUFSZ);
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
			buf = rtos_mem_zmalloc(SPI_BUFSZ);
			memcpy(buf, recv_msg, SPI_BUFSZ);
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
			ret_msg = (struct whc_api_info *)(recv_msg + SIZE_RX_DESC);
			RTK_LOGE(TAG_WLAN_INIC, "too late to receive API ret, ID: 0x%x!\n", ret_msg->api_id);
		}
		break;
	case WHC_CUST_EVT:
		whc_host_recv_cust_evt(spi_host_priv.rx_buf + SIZE_RX_DESC);
		break;
#endif

#ifdef CONFIG_WHC_CMD_PATH
	case WHC_WIFI_EVT_BRIDGE:
		hdr = (struct whc_cmd_path_hdr *)recv_msg;
		whc_host_pkt_rx_to_user((u8 *)(hdr + 1), hdr->len);
		break;
#endif

	default:
		if (event >= WHC_BT_EVT_BASE) {
			/* copy by bt, skb no change */
			if (bt_inic_spi_recv_host_ptr) {
				bt_inic_spi_recv_host_ptr(recv_msg + SIZE_RX_DESC, SPI_BUFSZ - SIZE_RX_DESC);
			}
		}
		RTK_LOGD(TAG_WLAN_INIC,  "%s: unknown event:%x\n", __func__, event);
		break;
	}

	rtos_mem_free(recv_msg);
	return ret;
}


u32 whc_spi_host_rxdma_irq_handler(void *pData)
{
	GDMA_InitTypeDef *GDMA_InitStruct;
	u32 int_status;

	(void)pData;

	GDMA_InitStruct = &spi_host_priv.SSIRxGdmaInitStruct;

	/* check and clear RX DMA ISR */
	int_status = GDMA_ClearINT(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum);

	if (spi_host_priv.host_recv_state) {
		GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, DISABLE);
		SSI_SetDmaEnable(WHC_SPI_DEV, DISABLE, SPI_BIT_RDMAE);
		rtos_sema_give(spi_host_priv.rxirq_sema);
		spi_host_priv.host_recv_state = 0;
	} else {
		SSI_SetDmaEnable(WHC_SPI_DEV, DISABLE, SPI_BIT_RDMAE);
		rtos_sema_give(spi_host_priv.host_recv_done);
	}

	spi_host_priv.host_dma_waiting_status &= (~HOST_RX_DMA_CB_DONE);
	if (spi_host_priv.host_dma_waiting_status == 0) {
		set_host_rdy_pin(HOST_READY);
	}

	if (int_status & ErrType) {
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "spi rxdma err occurs!!\n");
	}

	return 0;
}



_OPTIMIZE_NONE_
void whc_spi_host_flush_rx_fifo(void)
{
	u32 rx_fifo_level;
	u32 i;
	u32 value;

	while ((WHC_SPI_DEV->SPI_SR & (SPI_BIT_RFNE))) {
		rx_fifo_level =  WHC_SPI_DEV->SPI_RXFLR & SPI_MASK_RXTFL;
		for (i = 0; i < rx_fifo_level; i++) {
			value = WHC_SPI_DEV->SPI_DRx[0];
		}
	}

	(void)value;
}

void whc_spi_host_rx_req_task(void)
{
	for (;;) {
		rtos_sema_take(spi_host_priv.host_recv_wake, MUTEX_WAIT_TIMEOUT);
retry:
		while (GPIO_ReadDataBit(DEV_READY_PIN) == DEV_BUSY) {
			/* wait for sema*/
			if (rtos_sema_take(spi_host_priv.dev_rdy_sema, 1000) == RTK_SUCCESS) {
				if (spi_host_priv.dev_state == DEV_BUSY) {
					//RTK_LOGE(TAG_WLAN_INIC, "%s: wait dev busy timeout, can't recv data %d \n\r", __func__, spi_host_priv.dev_state);
				}
			} else {
				RTK_LOGD(TAG_WLAN_INIC, "%s: down sema timeout, can't recv data\n\r", __func__);
			}
		}

		if (SSI_Busy(WHC_SPI_DEV)) {
			goto retry;
		}
		rtos_mutex_take(spi_host_priv.dev_lock, MUTEX_WAIT_TIMEOUT);

		while ((GPIO_ReadDataBit(HOST_READY_PIN) == HOST_BUSY) || (spi_host_priv.txbuf_info != NULL)) {
			rtos_time_delay_ms(1);
		}

		/* check RX_REQ level */
		if (GPIO_ReadDataBit(RX_REQ_PIN)) {
			rtos_sema_take(spi_host_priv.host_recv_done, MUTEX_WAIT_TIMEOUT);
			spi_host_priv.host_dma_waiting_status = HOST_RX_DMA_CB_DONE | HOST_TX_DMA_CB_DONE;

			set_host_rdy_pin(HOST_BUSY);

			whc_spi_host_flush_rx_fifo();
			spi_host_priv.host_recv_state = 1;
			SSI_SetDmaEnable(WHC_SPI_DEV, ENABLE, SPI_BIT_RDMAE);

			spi_host_priv.host_tx_state = 0;
			GDMA_SetSrcAddr(spi_host_priv.SSITxGdmaInitStruct.GDMA_Index, spi_host_priv.SSITxGdmaInitStruct.GDMA_ChNum, (u32)spi_host_priv.dummy_tx_buf);
			GDMA_Cmd(spi_host_priv.SSITxGdmaInitStruct.GDMA_Index, spi_host_priv.SSITxGdmaInitStruct.GDMA_ChNum, ENABLE);
			SSI_SetDmaEnable(WHC_SPI_DEV, ENABLE, SPI_BIT_TDMAE);
		} else {
			rtos_sema_give(spi_host_priv.dev_rdy_sema);
		}
		rtos_mutex_give(spi_host_priv.dev_lock);

	}

}

static int whc_spi_host_devrdy_handler(int irq, void *context)
{
	(void)irq;

	(void)context;

	if (GPIO_ReadDataBit(DEV_READY_PIN)) {
		spi_host_priv.dev_state = DEV_READY;
		/* wakeup wait task */
		rtos_sema_give(spi_host_priv.dev_rdy_sema);
	} else {
		spi_host_priv.dev_state = DEV_BUSY;
	}
	return 1; //IRQ_HANDLED;
}

static int whc_spi_host_rx_req_handler(int irq, void *context)
{
	(void)irq;
	(void)context;
	rtos_sema_give(spi_host_priv.host_recv_wake);

	return 1;//IRQ_HANDLED;
}

static void whc_spi_host_setup_gpio(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	InterruptRegister(GPIO_INTHandler, GPIOB_IRQ, (u32)GPIOB_BASE, 6);
	InterruptEn(GPIOB_IRQ, 6);

	/* Initialize GPIO */
	/* rx req only need rising */
	GPIO_InitStruct.GPIO_Pin = RX_REQ_PIN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_INT;
	GPIO_InitStruct.GPIO_ITTrigger = GPIO_INT_Trigger_EDGE;
	GPIO_InitStruct.GPIO_ITPolarity = GPIO_INT_POLARITY_ACTIVE_HIGH;
	GPIO_Init(&GPIO_InitStruct);
	GPIO_UserRegIrq(GPIO_InitStruct.GPIO_Pin, whc_spi_host_rx_req_handler, &GPIO_InitStruct);
	GPIO_INTConfig(GPIO_InitStruct.GPIO_Pin, ENABLE);

	/* dev ready need both edge */
	GPIO_InitStruct.GPIO_Pin = DEV_READY_PIN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_INT;
	GPIO_InitStruct.GPIO_ITTrigger = GPIO_INT_Trigger_BOTHEDGE;
	GPIO_Init(&GPIO_InitStruct);
	GPIO_UserRegIrq(GPIO_InitStruct.GPIO_Pin, whc_spi_host_devrdy_handler, &GPIO_InitStruct);
	GPIO_INTConfig(GPIO_InitStruct.GPIO_Pin, ENABLE);

	//	Pinmux_Config(HOST_READY_PIN, PINMUX_FUNCTION_SPIM);//CS
	//	PAD_PullCtrl(HOST_READY_PIN, GPIO_PuPd_UP);
	GPIO_InitStruct.GPIO_Pin = HOST_READY_PIN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(&GPIO_InitStruct);
	set_host_rdy_pin(HOST_READY);

#ifdef SPI_DEBUG
	GPIO_InitStruct.GPIO_Pin = _PB_20;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(&GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = _PB_6;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(&GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = _PA_24;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(&GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = _PA_25;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(&GPIO_InitStruct);
#endif

}

u32 whc_spi_host_txdma_irq_handler(void *pData)
{
	GDMA_InitTypeDef *GDMA_InitStruct;
	u32 int_status;

	(void)pData;

	GDMA_InitStruct = &spi_host_priv.SSITxGdmaInitStruct;

	/* check and clear TX DMA ISR */
	int_status = GDMA_ClearINT(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum);

	if (spi_host_priv.host_tx_state) {
		rtos_sema_give(spi_host_priv.txirq_sema);
		spi_host_priv.host_tx_state = 0;
	} else {
		GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, DISABLE);
		SSI_SetDmaEnable(WHC_SPI_DEV, DISABLE, SPI_BIT_TDMAE);
	}

	spi_host_priv.host_dma_waiting_status &= (~HOST_TX_DMA_CB_DONE);
	if (spi_host_priv.host_dma_waiting_status == 0) {
		set_host_rdy_pin(HOST_READY);
	}

	if (int_status & ErrType) {
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "spi txdma err occurs!!\n");
	}

	return 0;
}

void whc_spi_host_dma_tx_done_cb(void)
{
	struct whc_txbuf_info_t *inic_tx = container_of(spi_host_priv.txbuf_info, struct whc_txbuf_info_t, txbuf_info);
	GDMA_InitTypeDef *GDMA_InitStruct = &spi_host_priv.SSITxGdmaInitStruct;

	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, DISABLE);
	SSI_SetDmaEnable(WHC_SPI_DEV, DISABLE, SPI_BIT_TDMAE);

	/* Dev TX complete, free tx skb or buffer */
	if (inic_tx->is_skb) {
		char *buf = inic_tx->ptr;
		*buf = 0;
	} else {
		rtos_mem_free((u8 *)inic_tx->ptr);
	}
	rtos_mem_free((u8 *)inic_tx);

	spi_host_priv.txbuf_info = NULL;
}

void whc_spi_host_txdma_irq_task(void *pData)
{
	(void)pData;
	for (;;) {
		/* Task blocked and wait the semaphore(events) here */
		rtos_sema_take(spi_host_priv.txirq_sema, RTOS_MAX_TIMEOUT);
		whc_spi_host_dma_tx_done_cb();
	}
}

void whc_spi_host_rxdma_irq_task(void *pData)
{
	(void)pData;
	for (;;) {
		/* Task blocked and wait the semaphore(events) here */
		rtos_sema_take(spi_host_priv.rxirq_sema, RTOS_MAX_TIMEOUT);
		DCache_Invalidate((u32)spi_host_priv.rx_buf, SPI_BUFSZ);
		whc_spi_host_recv_process();
	}
}

static void whc_spi_host_spi_init(void)
{
	SSI_InitTypeDef SSI_InitStructMaster;

	u8 index = (WHC_SPI_DEV == SPI0_DEV) ? 0 : 1;

	if (WHC_SPI_DEV == SPI0_DEV) {
		RCC_PeriphClockCmd(APBPeriph_SPI0, APBPeriph_SPI0_CLOCK, ENABLE);
	} else {
		RCC_PeriphClockCmd(APBPeriph_SPI1, APBPeriph_SPI1_CLOCK, ENABLE);
	}

	Pinmux_Config(SPIM_MOSI, PINMUX_FUNCTION_SPIM);//MOSI
	Pinmux_Config(SPIM_MISO, PINMUX_FUNCTION_SPIM);//MISO
	Pinmux_Config(SPIM_SCLK, PINMUX_FUNCTION_SPIM);//CLK
	//Pinmux_Config(SPIM_CS, PINMUX_FUNCTION_SPIM);//CS
	//PAD_PullCtrl(SPIM_CS, GPIO_PuPd_UP);  // pull-up, default 1
	PAD_PullCtrl(SPIM_SCLK, GPIO_PuPd_DOWN);

	SSI_SetRole(WHC_SPI_DEV, SSI_MASTER);
	SSI_StructInit(&SSI_InitStructMaster);
	SSI_InitStructMaster.SPI_SclkPhase = SCPH_TOGGLES_IN_MIDDLE;
	SSI_InitStructMaster.SPI_SclkPolarity = SCPOL_INACTIVE_IS_LOW;
	SSI_InitStructMaster.SPI_DataFrameSize = DFS_8_BITS;
	SSI_InitStructMaster.SPI_Role = SSI_MASTER;
	/* for stable now */
#ifndef SPI_TOOD
#endif
	SSI_InitStructMaster.SPI_ClockDivider = SPI_CLOCK_DIVIDER;
	SSI_Init(WHC_SPI_DEV, &SSI_InitStructMaster);

	spi_host_priv.rx_buf = rtos_mem_zmalloc(SPI_BUFSZ);
	DCache_CleanInvalidate((u32)spi_host_priv.rx_buf, SPI_BUFSZ);
	whc_spi_host_rxgdma_init(index, &(spi_host_priv.SSIRxGdmaInitStruct), (void *)WHC_SPI_RXDMA, (IRQ_FUN) whc_spi_host_rxdma_irq_handler, spi_host_priv.rx_buf,
							 SPI_BUFSZ);

	//SSI_SetDmaEnable(WHC_SPI_DEV, ENABLE, SPI_BIT_RDMAE);
	GDMA_Cmd(spi_host_priv.SSIRxGdmaInitStruct.GDMA_Index, spi_host_priv.SSIRxGdmaInitStruct.GDMA_ChNum, ENABLE);

	/* Configure DMA and buffer */
	spi_host_priv.txdma_initialized = 0;
	spi_host_priv.txbuf_info = NULL;

	/* fix for dummy tx when rx */
	spi_host_priv.dummy_tx_buf = rtos_mem_zmalloc(SPI_BUFSZ);
	DCache_CleanInvalidate((u32)spi_host_priv.dummy_tx_buf, SPI_BUFSZ);

	if (rtos_task_create(NULL, "SPI_RXDMA_IRQ_TASK", whc_spi_host_rxdma_irq_task, (void *)&spi_host_priv, WIFI_STACK_WHC_SPI_HOST_RXDMA_IRQ_TASK,
						 7) != RTK_SUCCESS) {
		RTK_LOGE(TAG_WLAN_INIC, "Create SPI_RXDMA_IRQ_TASK Err!!\n");
		return;
	}

	if (rtos_task_create(NULL, "SPI_TXDMA_IRQ_TASK", whc_spi_host_txdma_irq_task, (void *)&spi_host_priv, WIFI_STACK_WHC_SPI_HOST_TXDMA_IRQ_TASK,
						 7) != RTK_SUCCESS) {
		RTK_LOGE(TAG_WLAN_INIC, "Create SPI_TXDMA_IRQ_TASK Err!!\n");
		return;
	}

	RTK_LOGI(TAG_WLAN_INIC, "SPI Host init done! \n");
}

bool whc_spi_host_txdma_init(
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
			RTK_LOGE(TAG_WLAN_INIC, "SSI_TXGDMA_Init: Aligment Err: pTxData=%p,  Length=%lu\n", pTxData, Length);
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

/**
* @brief  send buf to dev, used by IP without struct whc_buf_info.
* @param  buf: data buf to be sent, must 4B aligned.
* @param  buf: data len to be sent.
* @param  buf_alloc: real buf address, to be freed after sent.
* @return none.
*/
void whc_spi_host_send_to_dev_internal(u8 *buf, u8 *buf_alloc, u16 len)
{
	struct whc_txbuf_info_t *inic_tx;

	/* construct struct whc_buf_info & whc_buf_info_t */
	inic_tx = (struct whc_txbuf_info_t *)rtos_mem_zmalloc(sizeof(struct whc_txbuf_info_t));

	inic_tx->txbuf_info.buf_allocated = inic_tx->txbuf_info.buf_addr = (u32)buf;
	inic_tx->txbuf_info.size_allocated = inic_tx->txbuf_info.buf_size = len;

	inic_tx->ptr = buf_alloc;
	inic_tx->is_skb = 0;

	/* send ret_msg + ret_val(buf, len) */
	whc_spi_host_send_data(&inic_tx->txbuf_info);
}

void whc_spi_host_send_data(struct whc_buf_info *pbuf)
{
	GDMA_InitTypeDef *GDMA_InitStruct = &(spi_host_priv.SSITxGdmaInitStruct);
	u32 index = (WHC_SPI_DEV == SPI0_DEV) ? 0 : 1;

	DCache_CleanInvalidate(pbuf->buf_addr, SPI_BUFSZ);

	if (pbuf->buf_size > SPI_BUFSZ) {
		RTK_LOGE(TAG_WLAN_INIC, "%s: len(%d) > SPI_BUFSZ\n\r", __func__, pbuf->buf_size);
	}

retry:
	while (GPIO_ReadDataBit(DEV_READY_PIN) == DEV_BUSY) {
		/* wait for sema*/
		if (rtos_sema_take(spi_host_priv.dev_rdy_sema, 1000) == RTK_SUCCESS) {
			if (spi_host_priv.dev_state == DEV_BUSY) {
				RTK_LOGD(TAG_WLAN_INIC, "%s: wait dev busy timeout, can't send data %d \n\r", __func__, spi_host_priv.dev_state);
			}
		} else {
			RTK_LOGD(TAG_WLAN_INIC, "%s: down sema timeout, can't send data\n\r", __func__);
		}
	}

	rtos_mutex_take(spi_host_priv.dev_lock, MUTEX_WAIT_TIMEOUT);

	if (SSI_Busy(WHC_SPI_DEV)) {
		rtos_mutex_give(spi_host_priv.dev_lock);
		goto retry;
	}

	while ((GPIO_ReadDataBit(HOST_READY_PIN) == HOST_BUSY) || (spi_host_priv.txbuf_info != NULL)) {
		rtos_time_delay_ms(1);
	}

	set_host_rdy_pin(HOST_BUSY);

	/* initiate spi transaction */
	if (!spi_host_priv.txdma_initialized) {
		whc_spi_host_txdma_init(index, &(spi_host_priv.SSITxGdmaInitStruct), WHC_SPI_TXDMA, whc_spi_host_txdma_irq_handler, (u8 *)(pbuf->buf_addr), SPI_BUFSZ);
		spi_host_priv.txdma_initialized = 1;
	} else {
		GDMA_SetSrcAddr(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, pbuf->buf_addr);
	}

	/* take without block */
	rtos_sema_take(spi_host_priv.host_recv_done, MUTEX_WAIT_TIMEOUT);

	whc_spi_host_flush_rx_fifo();
	spi_host_priv.host_recv_state = 1;
	spi_host_priv.host_tx_state = 1;
	spi_host_priv.host_dma_waiting_status = HOST_RX_DMA_CB_DONE | HOST_TX_DMA_CB_DONE;

	rtos_critical_enter(RTOS_CRITICAL_WIFI);
	SSI_SetDmaEnable(WHC_SPI_DEV, ENABLE, SPI_BIT_RDMAE);

	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);
	SSI_SetDmaEnable(WHC_SPI_DEV, ENABLE, SPI_BIT_TDMAE);

	spi_host_priv.txbuf_info = pbuf;

	rtos_critical_exit(RTOS_CRITICAL_WIFI);

	rtos_mutex_give(spi_host_priv.dev_lock);
}

static void whc_spi_host_drv_init(void)
{
	/* init host priv */
	rtos_mutex_create(&(spi_host_priv.dev_lock));
	rtos_sema_create(&(spi_host_priv.dev_rdy_sema), 0, 1);
	rtos_sema_create(&(spi_host_priv.host_send), 0, RTOS_SEMA_MAX_COUNT);
	rtos_sema_create(&(spi_host_priv.host_send_api), 0, RTOS_SEMA_MAX_COUNT);
	rtos_sema_create(&(spi_host_priv.host_recv_wake), 0, RTOS_SEMA_MAX_COUNT);
	rtos_sema_create(&(spi_host_priv.host_recv_done), 1, 1);

	rtos_sema_create(&(spi_host_priv.rxirq_sema), 0, RTOS_SEMA_MAX_COUNT);
	rtos_sema_create(&(spi_host_priv.txirq_sema), 0, RTOS_SEMA_MAX_COUNT);
	spi_host_priv.rx_buf = NULL;
	spi_host_priv.host_dma_waiting_status = 0;

	rtos_sema_give(spi_host_priv.host_send);

	if (rtos_task_create(NULL, (const char *const)"SPI_RX_REQ_TASK", (rtos_task_function_t)whc_spi_host_rx_req_task, NULL, WIFI_STACK_SIZE_INIC_RX_REQ_TASK,
						 6) != RTK_SUCCESS) {
		RTK_LOGE(TAG_WLAN_INIC, "Create SPI_RXDMA_IRQ_TASK Err!!\n");
		return;
	}

	if (GPIO_ReadDataBit(DEV_READY_PIN)) {
		spi_host_priv.dev_state = DEV_READY;
		RTK_LOGD(TAG_WLAN_INIC, "dev ready\n");
	} else {
		spi_host_priv.dev_state = DEV_BUSY;
		RTK_LOGE(TAG_WLAN_INIC, "!!!dev busy\n");
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

}

/**
 * @brief  to initialize the inic device.
 * @param  none.
 * @return none.
 */
void whc_spi_host_init(void)
{

	whc_spi_host_drv_init();

	/* dev rdy and req pin */
	whc_spi_host_setup_gpio();

	/* init spi */
	whc_spi_host_spi_init();

	whc_host_init_done = 1;
}


