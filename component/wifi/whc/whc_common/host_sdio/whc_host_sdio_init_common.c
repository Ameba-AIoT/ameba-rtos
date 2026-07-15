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
	TxUnitCnt = sdio_cmd52_read1byte_local(priv, SPDIO_REG_TXBUF_UNIT_SZ);
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
