#include "rtw_whc_common.h"

extern struct whc_sdio whc_sdio_priv;
extern struct sdio_func sdio_func1;

void rtw_sdio_interrupt_handler(void)
{
	struct whc_sdio *priv = &whc_sdio_priv;

#ifdef SDIO_INT_MODE
	for (;;)  {
#ifdef CONFIG_SDIO_TX_ENABLE_AVAL_INT
		//SDIO->MASK |= SDIO_MASK_SDIOITIE;
		sdio_enable_data1_irq();
#endif
		rtos_sema_take(whc_sdio_priv.host_irq, MUTEX_WAIT_TIMEOUT);
#endif
		whc_host_sdio_isr_process(priv);
#ifdef SDIO_INT_MODE
	}
#endif
}

void whc_sdio_irq_sema_give(void)
{
	struct whc_sdio *priv = &whc_sdio_priv;
#ifdef CONFIG_SDIO_TX_ENABLE_AVAL_INT
	//SDIO->MASK &= ~SDIO_MASK_SDIOITIE;
	sdio_disable_data1_irq();
#endif
	rtos_sema_give(priv->host_irq); /* ignore failure since there is nothing that can be done about it in a ISR */
}

static uint32_t rtw_sdio_enable_func(struct whc_sdio *priv)
{
	//TODO set block size SDIO_BLOCK_SIZE
	priv->block_transfer_len = SDIO_BLOCK_SIZE;
	priv->tx_block_mode = 1;
	priv->rx_block_mode = 1;

	return TRUE;
}


void sdio_polling_task(void *arg1, void *arg2, void *arg3)
{
	u32 Interval = 100;
	printf("sdio polling every %dms \n", Interval);

	while (1) {
		// polling int reg
		rtw_sdio_interrupt_handler();

		vTaskDelay(Interval);
	}
}

void rtw_sdio_init_txavailbd_threshold(struct whc_sdio *priv)
{
	u16 txBDTh_l;
	u16 txBDTh_h;
	u32 freeBDNum;
	(void)freeBDNum;

#ifdef CONFIG_AMEBAGREEN2
	freeBDNum = rtw_read32(priv, SDIO_REG_FREE_TXBD_NUM);

	/* The value of SDIO_REG_FREE_TXBD_NUM = actual FREE TXBD NUM-1.
	When this value changes from "< txBDTh_l" to ">= txBDTh_h", TXBD_AVAIL interrupt triggers.
	Because driver would keep at least 1 TXBD available, so this value would >= 0*/
	txBDTh_l = 1;
	txBDTh_h = (freeBDNum + 1) / 2;
#else
	/* When actual FREE TXBD NUM changes from "< txBDTh_l" to ">= txBDTh_h", TXBD_AVAIL interrupt triggers.
	Because driver would keep at least 1 TXBD available, so the actual FREE TXBD NUM would >= 1*/
	txBDTh_l = 2;
	txBDTh_h = 3;
#endif

	rtw_write16(priv, SDIO_REG_AVAI_BD_NUM_TH_L, txBDTh_l);
	rtw_write16(priv, SDIO_REG_AVAI_BD_NUM_TH_H, txBDTh_h);

	printf("%s: SDIO_REG_AVAI_BD_NUM_TH_L @ 0x%04x, SDIO_REG_AVAI_BD_NUM_TH_H @ 0x%04x\n", __FUNCTION__,
		   rtw_read16(priv, SDIO_REG_AVAI_BD_NUM_TH_L),
		   rtw_read16(priv, SDIO_REG_AVAI_BD_NUM_TH_H));
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

	rtos_mutex_create(&(priv->lock));

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

	rtw_sdio_init_interrupt(priv);

	return TRUE;
}

