#include <rtw_cfg80211_fullmac.h>

static u8 rtw_sdio_get_tx_max_size(struct inic_sdio *priv)
{
	u8 TxUnitCnt = 0;
	TxUnitCnt = sdio_cmd52_read1byte_local(priv, SPDIO_REG_TXBUF_UNIT_SZ);
	if (!TxUnitCnt) {
		return false;
	}

	//num * unit_sz(64 bytes) -32(reserved for safety)
	priv->SdioTxMaxSZ = TxUnitCnt * 64 - 32;
	dev_dbg(&priv->func->dev, "%s: TX_UNIT_BUF_MAX_SIZE @ %d bytes\n", __FUNCTION__, priv->SdioTxMaxSZ);

	return true;
}

u8 rtw_sdio_query_txbd_status(struct inic_sdio *priv)
{
#ifdef CALCULATE_FREE_TXBD
	u16 wptr;
	u16 rptr;

	if (priv->txbd_size == 0) {
		priv->txbd_size = rtw_read16(priv, SPDIO_REG_TXBD_NUM);
		dev_dbg(&priv->func->dev, "txbd_size: %x\n", priv->txbd_size);
	}

	wptr = rtw_read8(priv, SPDIO_REG_TXBD_WPTR);
	rptr = rtw_read8(priv, SPDIO_REG_TXBD_RPTR);

	if (wptr >= rptr) {
		priv->SdioTxBDFreeNum = priv->txbd_size + rptr - wptr - 1;
	} else {
		priv->SdioTxBDFreeNum = rptr - wptr - 1;
	}

	//dev_dbg(&priv->func->dev, "%s: Free page for TXBD(0x%x)\n", __FUNCTION__, priv->SdioTxBDFreeNum);
#else
	priv->SdioTxBDFreeNum = rtw_read16(priv, SDIO_REG_FREE_TXBD_NUM);
#endif
	return true;
}

static void rtw_sdio_interrupt_handler(struct sdio_func *func)
{
	struct inic_sdio *priv;
	u8 data[4];
	u32 value, freepage;

	priv = (struct inic_sdio *) sdio_get_drvdata(func);

	//dev_dbg(&priv->func->dev, "%s: IRQ arrived!\n", __FUNCTION__);

	if (func == NULL) {
		dev_err(&priv->func->dev, "%s: func is NULL!\n", __FUNCTION__);
		return;
	}

	if (priv->bSurpriseRemoved == true) {
		return;
	}

	priv->sys_sdio_irq_thd = current;

	//read HISR
	sdio_local_read(priv, SDIO_REG_HISR, 4, data);
	priv->sdio_hisr = le32_to_cpu(*(u32 *)data);

	if (priv->sdio_hisr & priv->sdio_himr) {
		priv->sdio_hisr &= priv->sdio_himr;

		// clear HISR
		value = priv->sdio_hisr & MASK_SDIO_HISR_CLEAR;
		if (value) {
			value = cpu_to_le32(value);
			sdio_local_write(priv, SDIO_REG_HISR, 4, (u8 *)&value);
		}

#ifdef CONFIG_SDIO_TX_ENABLE_AVAL_INT
		if (priv->sdio_hisr & SDIO_HISR_AVAL_INT) {
			sdio_local_read(priv, SDIO_REG_FREE_TXBD_NUM, 4, (u8 *)&freepage);

			/* wakeup tx task if waiting */
			priv->tx_avail_int_triggered = 1;
			wake_up(&priv->txbd_wq);
		}
#endif
		if (priv->sdio_hisr & SDIO_HISR_RX_REQUEST) {
			priv->sdio_hisr ^= SDIO_HISR_RX_REQUEST;

			//schedule_work(&(priv->rx_work));
			//up(&priv->sdio_rx_sema);
			llhw_recv_notify();
		}

	} else {
		//dev_err(&priv->func->dev, "%s: HISR(0x%08x) and HIMR(0x%08x) not match!\n",
		//		__FUNCTION__, priv->sdio_hisr, priv->sdio_himr);
	}

	priv->sys_sdio_irq_thd = NULL;
}

int rtw_sdio_alloc_irq(struct inic_sdio *priv)
{
	struct sdio_func *func;
	int err = 0;

	func = priv->func;

	sdio_claim_host(func);

	err = sdio_claim_irq(func, &rtw_sdio_interrupt_handler);

	if (err) {
		dev_err(&priv->func->dev, "%s: sdio_claim_irq FAIL(%d)!\n", __func__, err);
	} else {
		priv->irq_alloc = 1;
	}

	sdio_release_host(func);

	return err ? false : true;
}

void rtw_sdio_init_txavailbd_threshold(struct inic_sdio *priv)
{
	u32 freeBDNum;
	u16 txBDTh_l;
	u16 txBDTh_h;

	freeBDNum = sdio_cmd53_read4byte_local(priv, SDIO_REG_FREE_TXBD_NUM);

	txBDTh_l = freeBDNum - 1;
	txBDTh_h = freeBDNum / 2;

	rtw_write16(priv, SDIO_REG_AVAI_BD_NUM_TH_L, txBDTh_l);
	rtw_write16(priv, SDIO_REG_AVAI_BD_NUM_TH_H, txBDTh_h);

	dev_dbg(&priv->func->dev, "%s: SDIO_REG_AVAI_BD_NUM_TH_L @ 0x%04x, SDIO_REG_AVAI_BD_NUM_TH_H @ 0x%04x\n", __FUNCTION__,
			rtw_read16(priv, SDIO_REG_AVAI_BD_NUM_TH_L),
			rtw_read16(priv, SDIO_REG_AVAI_BD_NUM_TH_H));
}

u32 rtw_sdio_init_agg_setting(struct inic_sdio *priv)
{
	u8	valueTimeout;
	u8	valueBDCount;

	//DMA timeout
	valueTimeout = 0x06;
	//AGG BD threshold
	valueBDCount = MAX_RX_AGG_NUM * 2; //SDIO needs 2 BD to send one rx packet

	//get rx bd max num
	if (priv->rxbd_num == 0) {
		priv->rxbd_num = rtw_read16(priv, SPDIO_REG_RXBD_NUM);
	}

	if (valueBDCount > priv->rxbd_num) {
		dev_err(&priv->func->dev, "%s: HALINIT FAILED! RX_AGG_NUM(%d) > RXBD_NUM(%d)\n", __FUNCTION__, valueBDCount, priv->rxbd_num);
		return false;
	}

	rtw_write8(priv, SDIO_REG_RX_AGG_CFG + 1, valueTimeout);
	rtw_write8(priv, SDIO_REG_RX_AGG_CFG, valueBDCount);

	//enable rx aggregation
	rtw_write16(priv, SDIO_REG_RX_AGG_CFG, rtw_read16(priv, SDIO_REG_RX_AGG_CFG) | SDIO_RX_AGG_EN);

	dev_dbg(&priv->func->dev, "%s: SDIO_REG_RX_AGG_CFG @ 0x%04x\n", __FUNCTION__, rtw_read16(priv, SDIO_REG_RX_AGG_CFG));

	return true;
}

static void rtw_sdio_init_interrupt(struct inic_sdio *priv)
{
	u32 himr;

	//HISR write one to clear
	rtw_write32(priv, SDIO_REG_HISR, 0xFFFFFFFF);

	// HIMR - turn all off
	rtw_write32(priv, SDIO_REG_HIMR, 0);

	// Initialize SDIO Host Interrupt Mask configuration
	priv->sdio_himr = (u32)(\
							SDIO_HIMR_RX_REQUEST_MSK |
#ifdef CONFIG_SDIO_TX_ENABLE_AVAL_INT
							SDIO_HIMR_AVAL_MSK |
#endif
							//SDIO_HIMR_CPU_NOT_RDY_MSK |
							SDIO_HIMR_CPWM1_MSK |
							0);

	// Register IRQ handler
	rtw_sdio_alloc_irq(priv);

	// Enable interrupt
	himr = cpu_to_le32(priv->sdio_himr);
	sdio_local_write(priv, SDIO_REG_HIMR, 4, (u8 *)&himr);

}

static u32 rtw_sdio_enable_func(struct inic_sdio *priv)
{
	struct sdio_func *func = priv->func;
	int err;

	sdio_claim_host(func);

	err = sdio_enable_func(func);
	if (err) {
		dev_err(&priv->func->dev, "%s: sdio_enable_func FAIL(%d)!\n", __func__, err);
		sdio_release_host(func);
		return false;
	}

	err = sdio_set_block_size(func, SDIO_BLOCK_SIZE);
	if (err) {
		dev_err(&priv->func->dev, "%s: sdio_set_block_size FAIL(%d)!\n", __func__, err);
		sdio_release_host(func);
		return false;
	}
	sdio_release_host(func);

	priv->block_transfer_len = SDIO_BLOCK_SIZE;
	priv->tx_block_mode = 1;
	priv->rx_block_mode = 1;

	return true;
}

u32 rtw_sdio_init(struct inic_sdio *priv)
{
	struct sdio_func *func = priv->func;
	u8 fw_ready;
	u32 i;
	u8 value;

	/* enable func and set block size */
	if (rtw_sdio_enable_func(priv) == false) {
		return false;
	}

	/* wait for device TRX ready */
	for (i = 0; i < 100; i++) {
		fw_ready = rtw_read8(priv, SDIO_REG_CPU_IND);
		if (fw_ready & SDIO_SYSTEM_TRX_RDY_IND) {
			break;
		}
		msleep(10);
	}
	if (i == 100) {
		dev_err(&func->dev, "%s: Wait Device Firmware Ready Timeout!!SDIO_REG_CPU_IND @ 0x%04x\n", __FUNCTION__, fw_ready);
		return false;
	}

	value = rtw_read8(priv, SDIO_REG_TX_CTRL) | SDIO_EN_HISR_MASK_TIMER;
	rtw_write8(priv, SDIO_REG_TX_CTRL, value);

	rtw_write16(priv, SDIO_REG_STATIS_RECOVERY_TIMOUT, 0x10); //500us

#ifdef CONFIG_SDIO_TX_ENABLE_AVAL_INT
	rtw_sdio_init_txavailbd_threshold(priv);
#endif

#ifdef CONFIG_SDIO_RX_AGGREGATION
	if (rtw_sdio_init_agg_setting(priv) == false) {
		return false;
	}
#endif
	rtw_sdio_query_txbd_status(priv);

	if (rtw_sdio_get_tx_max_size(priv) == false) {
		return false;
	}

	rtw_sdio_init_interrupt(priv);

	return true;
}

void rtw_sdio_deinit(struct inic_sdio *priv)
{
	struct sdio_func *func;
	int err;

	func = priv->func;

	if (func) {
		sdio_claim_host(func);
		err = sdio_disable_func(func);
		if (err) {
			dev_err(&priv->func->dev, "%s: sdio_disable_func(%d)\n", __func__, err);
		}

		if (priv->irq_alloc) {
			err = sdio_release_irq(func);
			if (err) {
				dev_err(&priv->func->dev, "%s: sdio_release_irq(%d)\n", __func__, err);
			}
		}

		sdio_release_host(func);
	}
}

