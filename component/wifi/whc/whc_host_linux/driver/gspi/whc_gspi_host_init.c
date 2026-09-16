// SPDX-License-Identifier: GPL-2.0-only
/*
 * Realtek wireless local area network IC driver.
 *
 * Copyright(c) 2024 Realtek Corporation. All rights reserved.
 *
 * GSPI bring-up: activate the GSPI protocol, wait for device CPU ready, then
 * program the interrupt mask and TX-BD-available thresholds. Ported from
 * GSPI_Configuration()/init in ameba_gspi.c. The DEV_INT GPIO interrupt itself
 * is requested in whc_gspi_host_probe.c, not here.
 */
#include <whc_host_linux.h>

/*
 * Query how many bytes the device has queued for the host to read. RX_LEN_RDY
 * can read set while the length still reads 0, so retry a few times.
 */
u32 rtw_gspi_get_rx_len(struct whc_gspi *priv)
{
	u8 retry = 0;
	u32 rx0;
	u32 rx_len = 0;

	do {
		rx0 = gspi_read32(priv, GSPI_REG_RX0_REQ_LEN);
		if ((rx0 & GSPI_RX_REQ_LEN_RDY) == 0) {
			break;
		}

		rx_len = rx0 & GSPI_RX_REQ_LEN_MSK;
		if ((rx_len == 0) && (retry++ < 3)) {
			continue;
		}
		break;
	} while (1);

	return rx_len;
}


/*
 * Activate GSPI mode by writing SPI_CFG, which must be the first bus
 * transaction after the device boots or it latches SDIO mode. Retried because a
 * frame sent while the device is still in its own SPDIO init is simply not seen.
 * We select big-endian-32, so on a little-endian host no per-word swap is
 * needed afterwards (priv->swap becomes 0).
 */
static bool gspi_activate(struct whc_gspi *priv, u8 spi_cfg)
{
	u8 readback;
	u32 attempt;

	for (attempt = 1; attempt <= GSPI_ACTIVATE_RETRY; attempt++) {
		/* the activation frame is always encoded for the device reset state */
		priv->swap = 1;
		rtw_write8(priv, GSPI_REG_SPI_CFG, spi_cfg);

		/* from here on the device uses the new setting */
		priv->swap = (spi_cfg == GSPI_BIG_ENDIAN_32) ? 0 : 1;

		readback = gspi_read8(priv, GSPI_REG_SPI_CFG);
		if (readback == spi_cfg) {
			if (attempt > 1) {
				dev_info(&priv->spi_dev->dev, "%s: activated on attempt %d\n", __func__, attempt);
			}
			return true;
		}

		dev_warn(&priv->spi_dev->dev, "%s: attempt %d, SPI_CFG reads 0x%02x\n",
				 __func__, attempt, readback);
		priv->swap = 1;
		msleep(GSPI_ACTIVATE_RETRY_MS);
	}

	dev_err(&priv->spi_dev->dev, "%s: activation failed after %d attempts\n",
			__func__, GSPI_ACTIVATE_RETRY);

	return false;
}

static void rtw_gspi_init_txavailbd_threshold(struct whc_gspi *priv)
{
	u32 freeBDNum;
	u16 txBDTh_l;
	u16 txBDTh_h;

	freeBDNum = rtw_read32(priv, GSPI_REG_FREE_TXBD_NUM) & GSPI_FREE_TXBD_NUM_MSK;

	/* When the free BD count crosses from "< L" up to ">= H", TXBD_AVAL fires.
	 * The driver always keeps at least 1 BD free, hence L = 1. */
	txBDTh_l = 1;
	txBDTh_h = (freeBDNum + 1) / 2;
	if (txBDTh_h < txBDTh_l) {
		txBDTh_h = txBDTh_l;
	}

	rtw_write32(priv, GSPI_REG_AVAI_PGTH_L, txBDTh_l);
	rtw_write32(priv, GSPI_REG_AVAI_PGTH_H, txBDTh_h);
}

#ifdef WHC_RX_AGG
void rtw_gspi_enable_rx_agg(struct whc_gspi *priv)
{
	u16 cfg;

	cfg = GSPI_RX_AGG_EN
		  | ((GSPI_RX_AGG_TO & GSPI_RX_AGG_TIMEOUT_MSK) << GSPI_RX_AGG_TIMEOUT_SHIFT)
		  | (GSPI_RX_AGG_BD_CNT_TH & GSPI_RX_AGG_BDCNT_MSK);

	rtw_write16(priv, GSPI_REG_RX_AGG, cfg);
}
#endif /* WHC_RX_AGG */

u8 rtw_gspi_query_txbd_status(struct whc_gspi *priv)
{
#ifdef CALCULATE_FREE_TXBD
	u8 wptr = priv->txbd_wptr;
	u8 rptr;

	rptr = rtw_read8(priv, SPDIO_REG_TXBD_RPTR);

	if (wptr >= rptr) {
		priv->GspiTxBDFreeNum = priv->txbd_size + rptr - wptr - 1;
	} else {
		priv->GspiTxBDFreeNum = rptr - wptr - 1;
	}
#else
	priv->GspiTxBDFreeNum = rtw_read16(priv, GSPI_REG_FREE_TXBD_NUM);
#endif
	return true;
}

static void rtw_gspi_init_interrupt(struct whc_gspi *priv)
{
	u32 himr;

	//HISR write one to clear
	rtw_write32(priv, GSPI_REG_HISR, 0xFFFFFFFF);

	// HIMR - turn all off
	rtw_write32(priv, GSPI_REG_HIMR, 0);

	// Initialize SDIO Host Interrupt Mask configuration
	priv->gspi_himr = (u32)(\
							GSPI_HIMR_RX_REQUEST_MSK |
#ifdef CONFIG_GSPI_TX_ENABLE_AVAL_INT
							GSPI_HIMR_TXFIFO_AVAL_MSK |
#endif
							GSPI_HIMR_CPWM1_MSK |
							0);

	// Enable interrupt
	himr = cpu_to_le32(priv->gspi_himr);
	rtw_write32(priv, GSPI_REG_HIMR, himr);
}

u32 rtw_gspi_init_common(struct whc_gspi *priv)
{
	u32 value;

	/* todo, check if need: https://jira.realtek.com/browse/RSWLANDIOT-7721  */
	value = GSPI_EN_HISR_MASK_TIMER | (0x10 << GSPI_INT_TIMEOUT_SHIFT);
	rtw_write32(priv, GSPI_REG_SPI_INT, value);

#ifdef CONFIG_GSPI_TX_ENABLE_AVAL_INT
	rtw_gspi_init_txavailbd_threshold(priv);
#endif

#ifdef WHC_RX_AGG
	rtw_gspi_enable_rx_agg(priv);
#endif

	priv->txbd_wptr = (u16)rtw_read8(priv, SPDIO_REG_TXBD_WPTR);
	priv->txbd_size = rtw_read16(priv, SPDIO_REG_TXBD_NUM);
	rtw_gspi_query_txbd_status(priv);

	dev_info(&priv->spi_dev->dev, "txbd size %d\n", priv->txbd_size);
	priv->GspiTxMaxSZ = CONFIG_MAX_TXAGG_SZ;

#ifdef WHC_TX_AGG
	/* coalescing buffer for tx aggregation, sized to one bus transfer */
	priv->agg_buf = kmalloc(priv->GspiTxMaxSZ, GFP_KERNEL);
	if (priv->agg_buf == NULL) {
		dev_err(&priv->spi_dev->dev, "%s: alloc tx-agg buf FAIL!\n", __func__);
		return false;
	}
#endif
	rtw_gspi_init_interrupt(priv);

	return true;
}

u32 rtw_gspi_init(struct whc_gspi *priv)
{
	//todo if need check gspi ready bit
	/* GSPI activation must be the very first transaction. */
	if (gspi_activate(priv, GSPI_BIG_ENDIAN_32) == false) {
		return false;
	}

	priv->rx_recv_notify = whc_host_recv_notify;
	priv->dev_state = PWR_STATE_ACTIVE;

	return rtw_gspi_init_common(priv);
}

void rtw_gspi_deinit(struct whc_gspi *priv)
{
	/* mask all interrupts */
	priv->gspi_himr = GSPI_HIMR_DISABLED;
	gspi_write32(priv, GSPI_REG_HIMR, priv->gspi_himr);
}
