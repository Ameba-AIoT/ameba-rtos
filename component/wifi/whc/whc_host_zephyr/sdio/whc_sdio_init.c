#include "rtw_whc_common.h"
extern struct whc_sdio whc_sdio_priv;
extern struct sdio_func sdio_func1;

void rtw_sdio_polling_timer_expiry(struct k_timer *dummy);

//timer for sdio int polling
K_SEM_DEFINE(sdio_polling_sem, 0, SEMA_MAX_COUNT);
K_TIMER_DEFINE(sdio_polling_timer, rtw_sdio_polling_timer_expiry, NULL);

//TODO check real stack size
#define SDIO_POLLING_STACK_SIZE 1024
K_THREAD_STACK_DEFINE(sdio_polling_task_stack, SDIO_POLLING_STACK_SIZE);
struct k_thread sdio_polling_thread;

static void rtw_sdio_interrupt_handler(void)
{
	struct whc_sdio *priv = &whc_sdio_priv;

	whc_host_sdio_isr_process(priv);
}

static uint32_t rtw_sdio_enable_func(struct whc_sdio *priv)
{
	//int err = 0;

	//TODO set block size SDIO_BLOCK_SIZE

	priv->func = &sdio_func1;
	priv->block_transfer_len = SDIO_BLOCK_SIZE;
	priv->tx_block_mode = 1;
	priv->rx_block_mode = 1;

	return TRUE;
}

uint8_t rtw_sdio_query_txbd_status(struct whc_sdio *priv)
{
#ifdef CALCULATE_FREE_TXBD
	uint16_t wptr = priv->txbd_wptr;
	uint16_t rptr;

	if (priv->txbd_size == 0) {
		priv->txbd_size = rtw_read16(priv, SPDIO_REG_TXBD_NUM);
		printf("txbd_size: %x\n", priv->txbd_size);
	}

	//wptr = rtw_read8(priv, SPDIO_REG_TXBD_WPTR);
	rptr = rtw_read8(priv, SPDIO_REG_TXBD_RPTR);

	if (wptr >= rptr) {
		priv->SdioTxBDFreeNum = priv->txbd_size + rptr - wptr - 1;
	} else {
		priv->SdioTxBDFreeNum = rptr - wptr - 1;
	}

#else
	priv->SdioTxBDFreeNum = rtw_read16(priv, SDIO_REG_FREE_TXBD_NUM);
#endif
	return TRUE;
}

static uint8_t rtw_sdio_get_tx_max_size(struct whc_sdio *priv)
{
	uint8_t TxUnitCnt = 0;
	TxUnitCnt = sdio_cmd52_read1byte_local(priv, SPDIO_REG_TXBUF_UNIT_SZ);
	if (!TxUnitCnt) {
		return FALSE;
	}

	//num * unit_sz(64 bytes) -32(reserved for safety)
	priv->SdioTxMaxSZ = TxUnitCnt * 64 - 32;
	printf("%s: TX_UNIT_BUF_MAX_SIZE @ %d bytes\n", __FUNCTION__, priv->SdioTxMaxSZ);
	return TRUE;
}

#ifdef SDIO_INT_MODE
static void rtw_sdio_init_interrupt(struct whc_sdio *priv)
{
	uint32_t himr;

	//HISR write one to clear
	rtw_write32(priv, SDIO_REG_HISR, 0xFFFFFFFF);

	// HIMR - turn all off
	rtw_write32(priv, SDIO_REG_HIMR, 0);

	// Initialize SDIO Host Interrupt Mask configuration
	priv->sdio_himr = (uint32_t)(\
								 SDIO_HIMR_RX_REQUEST_MSK |
#ifdef CONFIG_SDIO_TX_ENABLE_AVAL_INT
								 SDIO_HIMR_AVAL_MSK |
#endif
								 //SDIO_HIMR_CPU_NOT_RDY_MSK |
								 SDIO_HIMR_CPWM1_MSK |
								 0);

	WHC_HOST_SDIO_ALLOC_IRQ(priv);

	// Enable interrupt
	himr = priv->sdio_himr;
	rtw_write32(priv, SDIO_REG_HIMR, himr);

}
#endif


void rtw_sdio_polling_task(void *arg1, void *arg2, void *arg3)
{
	(void)arg1;
	(void)arg2;
	(void)arg3;

	while (1) {
		// wait for sema from timer
		k_sem_take(&sdio_polling_sem, K_FOREVER);
		rtw_sdio_interrupt_handler();
	}
}

void rtw_sdio_polling_timer_expiry(struct k_timer *dummy)
{
	(void)dummy;
	k_sem_give(&sdio_polling_sem);
}

void rtw_sdio_polling_init(void)
{
	u32 Interval = 10;
	printf("sdio polling every %dms \n", Interval);

	k_thread_create(&sdio_polling_thread,
					sdio_polling_task_stack, SDIO_POLLING_STACK_SIZE,
					rtw_sdio_polling_task,
					NULL, NULL, NULL,
					K_PRIO_PREEMPT(7), 0, K_NO_WAIT);

	//TODO check period to achieve best tp
	k_timer_start(&sdio_polling_timer, K_MSEC(Interval), K_MSEC(Interval));
}

uint32_t rtw_sdio_init(struct whc_sdio *priv)
{
	uint8_t fw_ready;
	uint32_t i;
	uint8_t value;

	/* enable func and set block size */
	if (rtw_sdio_enable_func(priv) == FALSE) {
		return FALSE;
	}

	k_mutex_init(&(priv->lock));

	/* wait for device TRX ready */
	for (i = 0; i < 100; i++) {
		fw_ready = rtw_read8(priv, SDIO_REG_CPU_IND);
		if (fw_ready & SDIO_SYSTEM_TRX_RDY_IND) {
			break;
		}
		WHC_MSLEEP(10);
	}
	if (i == 100) {
		printf("%s: Wait Device Firmware Ready Timeout!!SDIO_REG_CPU_IND @ 0x%04x\n", __FUNCTION__, fw_ready);
		return FALSE;
	}

	//TODO read slave reg
	value = rtw_read8(priv, SDIO_REG_TX_CTRL) | SDIO_EN_HISR_MASK_TIMER;
	rtw_write8(priv, SDIO_REG_TX_CTRL, value);

	rtw_write16(priv, SDIO_REG_STATIS_RECOVERY_TIMOUT, 0x10); //500us

#ifdef CONFIG_SDIO_TX_ENABLE_AVAL_INT
	rtw_sdio_init_txavailbd_threshold(priv);
#endif

	priv->txbd_wptr = (uint16_t)rtw_read8(priv, SPDIO_REG_TXBD_WPTR);
	rtw_sdio_query_txbd_status(priv);

	if (rtw_sdio_get_tx_max_size(priv) == FALSE) {
		return FALSE;
	}

#ifdef SDIO_INT_MODE
	rtw_sdio_init_interrupt(priv);
#else
	rtw_sdio_polling_init();
#endif

	return TRUE;
}

