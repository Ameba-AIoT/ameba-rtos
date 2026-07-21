#include "rtw_whc_common.h"

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

uint8_t rtw_sdio_get_tx_max_size(struct whc_sdio *priv)
{
	uint8_t TxUnitCnt = 0;
	TxUnitCnt = rtw_read8(priv, SPDIO_REG_TXBUF_UNIT_SZ);
	if (!TxUnitCnt) {
		return FALSE;
	}

	//num * unit_sz(64 bytes) -32(reserved for safety)
	priv->SdioTxMaxSZ = TxUnitCnt * 64 - 32;
	printf("%s: TX_UNIT_BUF_MAX_SIZE @ %d bytes\n", __FUNCTION__, priv->SdioTxMaxSZ);
	return TRUE;
}

void rtw_sdio_init_interrupt(struct whc_sdio *priv)
{
	uint32_t himr;

	rtw_write32(priv, SDIO_REG_HISR, 0xFFFFFFFF);
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

	whc_host_sdio_alloc_irq(priv);

	himr = priv->sdio_himr;
	rtw_write32(priv, SDIO_REG_HIMR, himr);
}

void rtw_sdio_init_txavailbd_threshold(struct whc_sdio *priv)
{
	uint16_t txBDTh_l;
	uint16_t txBDTh_h;
	uint32_t freeBDNum;
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

	/* wait for device TRX ready */
	for (i = 0; i < 100; i++) {
		fw_ready = rtw_read8(priv, SDIO_REG_CPU_IND);
		if (fw_ready & SDIO_SYSTEM_TRX_RDY_IND) {
			break;
		}
		whc_msleep(10);
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
