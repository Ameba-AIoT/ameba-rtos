#include "whc_host.h"

static rtos_sema_t sdio_whc_sema;

static SDIOHCFG_TypeDef sdioh_config = {
	.sdioh_bus_speed = SD_SPEED_HS,			 // SD_SPEED_DS or SD_SPEED_HS
	.sdioh_bus_width = SDIOH_BUS_WIDTH_4BIT, // SDIOH_BUS_WIDTH_1BIT or SDIOH_BUS_WIDTH_4BIT
	.sdioh_cd_pin = _PNC,					 // _PNC
	.sdioh_wp_pin = _PNC,					 // _PNC
};

extern struct whc_sdio whc_sdio_priv;
extern struct sdio_func sdio_func1;

void rtw_sdio_interrupt_handler(void)
{
	struct whc_sdio *priv = &whc_sdio_priv;
	uint8_t data[4];
	uint32_t value;
#ifdef CALCULATE_FREE_TXBD
	u32 freepage;
#endif

#ifdef SDIO_INT_MODE
	for (;;)  {
#ifdef CONFIG_SDIO_TX_ENABLE_AVAL_INT
		//SDIO->MASK |= SDIO_MASK_SDIOITIE;
		sdio_enable_data1_irq();
#endif
		rtos_sema_take(whc_sdio_priv.host_irq, MUTEX_WAIT_TIMEOUT);
#endif
		//read HISR
		sdio_local_read(priv, SDIO_REG_HISR, 4, data);
		priv->sdio_hisr = (*(u32 *)data);

		if (priv->sdio_hisr & priv->sdio_himr) {
			priv->sdio_hisr &= priv->sdio_himr;

			// clear HISR
			value = priv->sdio_hisr & MASK_SDIO_HISR_CLEAR;
			if (value) {
				sdio_local_write(priv, SDIO_REG_HISR, 4, (uint8_t *)&value);
			}

#ifdef CONFIG_SDIO_TX_ENABLE_AVAL_INT
			if (priv->sdio_hisr & SDIO_HISR_AVAL_INT) {
#ifdef CALCULATE_FREE_TXBD
				/* for DP bug, read txbd to clear aval int */
				sdio_local_read(priv, SDIO_REG_FREE_TXBD_NUM, 4, (u8 *)&freepage);

#else
				/* option set in dev, read txbd will never clr aval int */
				value = (SDIO_HISR_AVAL_INT);
				sdio_local_write(priv, SDIO_REG_HISR, 4, (u8 *)&value);
#endif
				/* wakeup tx task if waiting */
				if (priv->tx_avail_int_triggered == 1) {
					rtos_sema_give(priv->txbd_wq);
					priv->tx_avail_int_triggered = 0;
				}
			}
#endif
			if (priv->sdio_hisr & SDIO_HISR_RX_REQUEST) {
				priv->sdio_hisr ^= SDIO_HISR_RX_REQUEST;
				rtos_sema_give(whc_sdio_priv.host_recv_wake);
			}

		} else {

		}
#ifdef SDIO_INT_MODE
	}
#endif
}

void whc_sdio_irq_sema_give(void)
{
	struct whc_sdio *priv = &whc_sdio_priv;
#ifdef CONFIG_SDIO_TX_ENABLE_AVAL_INT
	sdio_disable_data1_irq();
#endif
	rtos_sema_give(priv->host_irq); /* ignore failure since there is nothing that can be done about it in a ISR */
}

static int sdio_give_sema(u32 timeout)
{
	UNUSED(timeout);
	return  rtos_sema_give(sdio_whc_sema);
}

static int sdio_take_sema(u32 timeout)
{
	return rtos_sema_take(sdio_whc_sema, timeout);
}

static uint32_t rtw_sdio_enable_func(struct whc_sdio *priv)
{
	rtos_sema_create(&sdio_whc_sema, 0, 1);
	SD_SetSema(sdio_take_sema, sdio_give_sema);

	if (SD_Init(&sdioh_config) != SD_OK) {
		return FALSE;
	}

	//TODO set block size SDIO_BLOCK_SIZE
#ifdef TODO
#endif
	//priv->func = &sdio_func1;
	priv->block_transfer_len = SDIO_BLOCK_SIZE;
	priv->tx_block_mode = 1;
	priv->rx_block_mode = 1;

	return TRUE;
}

uint8_t rtw_sdio_query_txbd_status(struct whc_sdio *priv)
{
#ifdef CALCULATE_FREE_TXBD
	uint16_t wptr;
	uint16_t rptr;

	if (priv->txbd_size == 0) {
		priv->txbd_size = rtw_read16(priv, SPDIO_REG_TXBD_NUM);
		RTK_LOGE(TAG_WLAN_INIC, "txbd_size: %x\n", priv->txbd_size);
	}

	wptr = rtw_read8(priv, SPDIO_REG_TXBD_WPTR);
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
	RTK_LOGE(TAG_WLAN_INIC, "%s: TX_UNIT_BUF_MAX_SIZE @ %d bytes\n", __FUNCTION__, priv->SdioTxMaxSZ);
	return TRUE;
}

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

	// Enable interrupt
	himr = priv->sdio_himr;
	sdio_local_write(priv, SDIO_REG_HIMR, 4, (uint8_t *)&himr);

}

//TODO check real stack size
#define SDIO_POLLING_STACK_SIZE 1024

void sdio_polling_task(void *arg1, void *arg2, void *arg3)
{
	(void)arg1;
	(void)arg2;
	(void)arg3;

	u32 Interval = 100;
	RTK_LOGE(TAG_WLAN_INIC, "sdio polling every %dms \n", Interval);

	while (1) {
		// polling int reg
		rtw_sdio_interrupt_handler();

		rtos_time_delay_ms(Interval);
	}
}

void rtw_sdio_init_txavailbd_threshold(struct whc_sdio *priv)
{
	u16 txBDTh_l;
	u16 txBDTh_h;
	u32 freeBDNum;
	(void)freeBDNum;

#ifdef CONFIG_AMEBAGREEN2
	freeBDNum = sdio_cmd53_read4byte_local(priv, SDIO_REG_FREE_TXBD_NUM);

	/* The value of SDIO_REG_FREE_TXBD_NUM = actual FREE TXBD NUM-1.
	When this value changes from "< txBDTh_l" to ">= txBDTh_h", TXBD_AVAIL interrupt triggers.
	Because driver would keep at least 1 TXBD available, so this value would >= 0*/
	txBDTh_l = 2;//1;
	txBDTh_h = 3;//(freeBDNum + 1) / 2;
#else
	/* When actual FREE TXBD NUM changes from "< txBDTh_l" to ">= txBDTh_h", TXBD_AVAIL interrupt triggers.
	Because driver would keep at least 1 TXBD available, so the actual FREE TXBD NUM would >= 1*/
	txBDTh_l = 2;
	txBDTh_h = 3;
#endif

	rtw_write16(priv, SDIO_REG_AVAI_BD_NUM_TH_L, txBDTh_l);
	rtw_write16(priv, SDIO_REG_AVAI_BD_NUM_TH_H, txBDTh_h);

	RTK_LOGE(TAG_WLAN_INIC, "%s: SDIO_REG_AVAI_BD_NUM_TH_L @ 0x%04x, SDIO_REG_AVAI_BD_NUM_TH_H @ 0x%04x\n", __FUNCTION__,
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

	priv->sdio_himr = (uint32_t)(\
								 SDIO_HIMR_RX_REQUEST_MSK |
#ifdef CONFIG_SDIO_TX_ENABLE_AVAL_INT
								 SDIO_HIMR_AVAL_MSK |
#endif
								 //SDIO_HIMR_CPU_NOT_RDY_MSK |
								 SDIO_HIMR_CPWM1_MSK |
								 0);

	/* wait for device TRX ready */
	for (i = 0; i < 100; i++) {
		fw_ready = rtw_read8(priv, SDIO_REG_CPU_IND);
		if (fw_ready & SDIO_SYSTEM_TRX_RDY_IND) {
			break;
		}
		rtos_time_delay_ms(10);
	}

	if (i == 100) {
		RTK_LOGE(TAG_WLAN_INIC, "%s: Wait Device Firmware Ready Timeout!!SDIO_REG_CPU_IND @ 0x%04x\n", __FUNCTION__, fw_ready);
		return FALSE;
	}

	//TODO read slave reg
	value = rtw_read8(priv, SDIO_REG_TX_CTRL) | SDIO_EN_HISR_MASK_TIMER;
	rtw_write8(priv, SDIO_REG_TX_CTRL, value);

	rtw_write16(priv, SDIO_REG_STATIS_RECOVERY_TIMOUT, 0x10); //500us

#ifdef CONFIG_SDIO_TX_ENABLE_AVAL_INT
	rtw_sdio_init_txavailbd_threshold(priv);
#endif

	rtw_sdio_query_txbd_status(priv);

	if (rtw_sdio_get_tx_max_size(priv) == FALSE) {
		return FALSE;
	}

	rtw_sdio_init_interrupt(priv);


	return TRUE;
}

