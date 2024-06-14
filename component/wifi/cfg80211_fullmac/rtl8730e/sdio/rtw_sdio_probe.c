#include <rtw_cfg80211_fullmac.h>

struct inic_sdio inic_sdio_priv = {0};

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

static u8 rtw_sdio_query_txbd_status(struct inic_sdio *priv)
{
#ifdef CALCULATE_FREE_TXBD
	u16 wptr;
	u16 rptr;

	if (priv->txbd_size == 0) {
		priv->txbd_size = rtw_read16(priv, SPDIO_REG_TXBD_NUM);
		dev_dbg(&priv->func->dev, "txbd_size: %x\n", priv->txbd_size);
	}

	wptr = rtw_read16(priv, SPDIO_REG_TXBD_WPTR);
	rptr = rtw_read16(priv, SPDIO_REG_TXBD_RPTR);

	if (wptr >= rptr) {
		priv->SdioTxBDFreeNum = priv->txbd_size + rptr - wptr - 1;
	} else {
		priv->SdioTxBDFreeNum = rptr - wptr - 1;
	}

	if (priv->SdioTxBDFreeNum > 19) {
		priv->SdioTxBDFreeNum =  0;
	}

	//dev_dbg(&priv->func->dev, "%s: Free page for TXBD(0x%x)\n", __FUNCTION__, priv->SdioTxBDFreeNum);
#else
	priv->SdioTxBDFreeNum = rtw_read16(priv, SDIO_REG_FREE_TXBD_NUM);
#endif
	return true;
}

void rtw_sdio_send_msg(u8 *buf, u32 len)
{
	u32 polling_num = 0, try_cnt = 0;
	struct inic_sdio *priv = &inic_sdio_priv;
	INIC_TX_DESC *ptxdesc;

	(void) polling_num;

	mutex_lock(&priv->lock);

	// check if hardware tx fifo page is enough
	while (priv->SdioTxBDFreeNum < 1) {
#ifdef CONFIG_SDIO_TX_ENABLE_AVAL_INT
		if (try_cnt ++ > 0) {
			priv->tx_avail_int_triggered = 0;
			if (!wait_event_timeout(priv->txbd_wq, priv->tx_avail_int_triggered == 1, msecs_to_jiffies(1000))) {
				dev_err(&priv->func->dev, "%s: TXBD unavailable, TX FAIL\n", __FUNCTION__);
				goto exit;
			}
		}
		rtw_sdio_query_txbd_status(priv);
#else
		polling_num++;
		if ((polling_num % 60) == 0) {
			msleep(1);
		}

		// Total number of TXBD is NOT available, so update current TXBD status
		rtw_sdio_query_txbd_status(priv);
#endif
	}

	if (len > priv->SdioTxMaxSZ) {
		dev_err(&priv->func->dev, "%s: PKT SIZE ERROR, total size: %d\n", __FUNCTION__, len);
		goto exit;
	}

	ptxdesc = (INIC_TX_DESC *)buf;
	ptxdesc->txpktsize = len - SIZE_TX_DESC;
	ptxdesc->offset = SIZE_TX_DESC;
	ptxdesc->type = TX_PACKET_802_3;
	ptxdesc->bus_agg_num = 1;

	//print_hex_dump_bytes("llhw_host_send: ", DUMP_PREFIX_NONE, data, data_len);

	rtw_write_port(priv, SDIO_TX_FIFO_DOMAIN_ID, len, buf);

	if (priv->SdioTxBDFreeNum > 0) {
		priv->SdioTxBDFreeNum -= 1;
	}

exit:
	mutex_unlock(&priv->lock);

	return;
}

void rtw_sdio_free_rxbuf(u8 *rx_payload)
{
	kfree(rx_payload - SIZE_RX_DESC);
}

#ifdef CONFIG_SDIO_RX_AGGREGATION
static struct recv_buf *rtw_sdio_recv_rxfifo(struct inic_sdio *priv, u32 size)
{
	u32 readsize, ret;
	u8 *preadbuf;
	struct recv_buf	*precvbuf;


	readsize = size;

	//3 1. alloc recvbuf
	precvbuf = rtw_dequeue_recvbuf(&shared_adapter.recvpriv.free_recv_buf_queue);
	if (precvbuf == NULL) {
		dev_err(&priv->func->dev, "%s: alloc recvbuf FAIL!\n", __FUNCTION__);
		return NULL;
	}

	//3 2. alloc skb
#ifdef PLATFORM_ECOS
	// can't allocate skb with size larger than 2K, use malloc for precvbuf->pbuf instead
#else
	if (precvbuf->pskb == NULL) {
		SIZE_PTR tmpaddr = 0;
		SIZE_PTR alignment = 0;

		precvbuf->pskb = rtw_skb_alloc(MAX_RECVBUF_SZ + RECVBUFF_ALIGN_SZ);

		if (precvbuf->pskb) {
			precvbuf->pskb->dev = rtw_get_netdev(get_iface_type(padapter));

			tmpaddr = (SIZE_PTR)precvbuf->pskb->data;
			alignment = tmpaddr & (RECVBUFF_ALIGN_SZ - 1);
			skb_reserve(precvbuf->pskb, (RECVBUFF_ALIGN_SZ - alignment));

			precvbuf->phead = precvbuf->pskb->head;
			precvbuf->pdata = precvbuf->pskb->data;
			precvbuf->ptail = skb_tail_pointer(precvbuf->pskb);
			precvbuf->pend = skb_end_pointer(precvbuf->pskb);
			precvbuf->len = 0;
		}

		if (precvbuf->pskb == NULL) {
			dev_err(&priv->func->dev, "%s: alloc_skb fail! read=%d\n", __FUNCTION__, readsize);
			return NULL;
		}
	}
#endif
	//3 3. read data from rxfifo
	preadbuf = precvbuf->pdata;

#if 0//def CONFIG_POWER_SAVING
#ifdef CONFIG_PS_DYNAMIC_CHK
	rtw_ps_deny(padapter, PS_DENY_DRV_RXDATA);
	rtw_set_ps_mode(PS_MODE_ACTIVE);
#endif
#endif

	ret = sdio_read_port(padapter, SDIO_RX_FIFO_DOMAIN_ID, readsize, preadbuf);

#if 0//def CONFIG_POWER_SAVING
#ifdef CONFIG_PS_DYNAMIC_CHK
	rtw_ps_deny_cancel(padapter, PS_DENY_DRV_RXDATA);
	rtw_set_ps_mode(PS_MODE_SLEEP);
#endif
#endif
	if (ret == false) {
		dev_err(&priv->func->dev, "%s: read port FAIL!\n", __FUNCTION__);
		return NULL;
	}


	//3 4. init recvbuf
	precvbuf->len = readsize;

	return precvbuf;
}
#else
static u8 *rtw_sdio_recv_rxfifo(struct inic_sdio *priv, u32 size)
{
	u8 *rx_buf = NULL;
	u32 allocsize, ret;
	u32 retry = 0;

	allocsize = _RND(size, priv->func->cur_blksize);

	rx_buf = (u8 *)kzalloc(allocsize, GFP_KERNEL);
	if (rx_buf == NULL) {
		return NULL;
	}

	while (1) {
		ret = sdio_read_port(priv, SDIO_RX_FIFO_DOMAIN_ID, size, rx_buf);
		if (ret == true) {
			break;
		} else {
			/* retry to reduce impact of bus err */
			if (retry++ > 10) {
				kfree(rx_buf);
				dev_err(&priv->func->dev, "%s: read port FAIL!\n", __FUNCTION__);
				return NULL;
			};
		}
	}

	//print_hex_dump_bytes("rtw_sdio_recv_rxfifo: ", DUMP_PREFIX_NONE, rx_buf, size);

	return rx_buf;
}

#endif

void rtw_sdio_isr_dispatch(struct inic_sdio *priv)
{
	u8 data[4];
	u32 rx_len_rdy;
	u8 *rx_buf;
	u32 freepage;
	u16 SdioRxFIFOSize;
	u8 retry = 0;

#ifdef CONFIG_SDIO_TX_ENABLE_AVAL_INT
	if (priv->sdio_hisr & SDIO_HISR_AVAL_INT) {
		sdio_local_read(priv, SDIO_REG_FREE_TXBD_NUM, 4, (u8 *)&freepage);

		/* wakeup tx task if waiting in llhw_host_send() */
		priv->tx_avail_int_triggered = 1;
		wake_up(&priv->txbd_wq);
	}
#endif
	if (priv->sdio_hisr & SDIO_HISR_RX_REQUEST) {
		priv->sdio_hisr ^= SDIO_HISR_RX_REQUEST;

		if (priv->dev_state == PWR_STATE_SLEEP) {
			dev_dbg(&priv->func->dev, "%s: wakeup device", __func__);
			if (rtw_resume_common(priv)) {
				dev_err(&priv->func->dev, "%s: fail to wakeup device, stop read rxfifo", __func__);
				return;
			}
		}

		do {
			/* validate RX_LEN_RDY before reading RX0_REQ_LEN */
			rx_len_rdy = sdio_read8(priv, SDIO_REG_RX0_REQ_LEN + 3) & BIT(7);

			if (rx_len_rdy) {
				sdio_local_read(priv, SDIO_REG_RX0_REQ_LEN, 4, data);
				SdioRxFIFOSize = le16_to_cpu(*(u16 *)data);

				if (SdioRxFIFOSize == 0) {
					if (retry ++ < 3) {
						continue;
					} else {
						break;
					}
				} else {
					retry = 0;
					rx_buf = rtw_sdio_recv_rxfifo(priv, SdioRxFIFOSize);
					if (rx_buf) {
						/* skip RX_DESC */
						llhw_recv_handler(rx_buf  + SIZE_RX_DESC);
					} else {
						break;
					}
				}
			} else {
				break;
			}
		} while (1);
	}
}

static void rtw_sdio_interrupt_handler(struct sdio_func *func)
{
	struct inic_sdio *priv;
	u8 data[4];
	u32 value;

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
		rtw_sdio_isr_dispatch(priv);

	} else {
		dev_err(&priv->func->dev, "%s: HISR(0x%08x) and HIMR(0x%08x) not match!\n",
				__FUNCTION__, priv->sdio_hisr, priv->sdio_himr);
	}

	priv->sys_sdio_irq_thd = NULL;
}

static int rtw_sdio_alloc_irq(struct inic_sdio *priv)
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

static void rtw_sdio_init_txavailbd_threshold(struct inic_sdio *priv)
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

	err = sdio_set_block_size(func, 512);
	if (err) {
		dev_err(&priv->func->dev, "%s: sdio_set_block_size FAIL(%d)!\n", __func__, err);
		sdio_release_host(func);
		return false;
	}
	sdio_release_host(func);

	priv->block_transfer_len = 512;
	priv->tx_block_mode = 1;
	priv->rx_block_mode = 1;

	return true;
}

static u32 rtw_sdio_init(struct inic_sdio *priv)
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

static void rtw_sdio_deinit(struct inic_sdio *priv)
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

static int rtw_sdio_probe(struct sdio_func *func, const struct sdio_device_id *id)
{
	struct inic_sdio *priv = &inic_sdio_priv;
	int status = false;

	dev_info(&func->dev, "rtw_sdio_probe: vendor=0x%04x device=0x%04x class=0x%02x\n", func->vendor, func->device, func->class);

	mutex_init(&(priv->lock));
	init_waitqueue_head(&priv->txbd_wq);

	priv->func = func;
	atomic_set(&priv->continual_io_error, 0);

	sdio_set_drvdata(func, (void *)priv);

	if (rtw_sdio_init(priv) != true) {
		dev_err(&priv->func->dev, "%s: initialize SDIO Failed!\n", __FUNCTION__);
		goto exit;
	}

	rtw_netdev_probe(&func->dev);

	status = true;

exit:
	if (status != true) {
		sdio_set_drvdata(func, NULL);
		rtw_sdio_deinit(priv);
	}

	return status == true ? 0 : -ENODEV;
}

static void rtw_sdio_remove(struct sdio_func *func)
{
	int err;
	struct inic_sdio *priv = (struct inic_sdio *) sdio_get_drvdata(func);

	dev_info(&func->dev, "rtw_sdio_remove: vendor=0x%04x device=0x%04x class=0x%02x\n", func->vendor, func->device, func->class);

	rtw_netdev_remove(&func->dev);

	if (priv->bSurpriseRemoved == false) {

		/* test surprise remove */
		sdio_claim_host(func);
		sdio_readb(func, 0, &err);
		sdio_release_host(func);
		if (err == -ENOMEDIUM) {
			priv->bSurpriseRemoved = true;
			dev_info(&priv->func->dev, "%s: device had been removed!\n", __func__);
		}
	}

	sdio_set_drvdata(func, NULL);
	rtw_sdio_deinit(priv);
}

u8 rtw_sdio_rpwm_notify(struct inic_sdio *priv, enum RPWM2_EVENT event)
{
	u16 rpwm2;
	u32 i = 0;
	u8 target_cpu_rdy_bit;
	u8 old_state = priv->dev_state;
	u8 ret = true, fw_ready;

	rpwm2 = rtw_read16(priv, SDIO_REG_HRPWM2) & RPWM2_TOGGLE_BIT;	// get the toggle bit
	switch (event) {
	case RPWM2_PWR_SUSPEND:
		if (priv->dev_state == PWR_STATE_SLEEP) {
			dev_dbg(&priv->func->dev, "Device is already in SLEEP state!\n");
			goto exit;
		} else {
			rpwm2 |= RPWM2_CG_BIT;
			priv->dev_state = PWR_STATE_SLEEP;
			target_cpu_rdy_bit = 0;
			dev_dbg(&priv->func->dev, "Device is in ACTIVE state, suspend\n");
		}
		break;

	case RPWM2_PWR_RESUME:
		if (priv->dev_state == PWR_STATE_ACTIVE) {
			dev_dbg(&priv->func->dev, "Device is already in ACTIVE state!\n");
			goto exit;
		} else {
			rpwm2 |= RPWM2_ACT_BIT;
			priv->dev_state = PWR_STATE_ACTIVE;
			target_cpu_rdy_bit = SDIO_SYSTEM_TRX_RDY_IND;
			dev_dbg(&priv->func->dev, "Device is in SLEEP state, resume\n");
		}
		break;

	default:
		dev_err(&priv->func->dev, "unknown rpwm event: %d\n", event);
		ret = false;
		goto exit;
	}

	/* inverse toggle bit */
	rpwm2 = rpwm2 ^ RPWM2_TOGGLE_BIT;
	rtw_write16(priv, SDIO_REG_HRPWM2, rpwm2);

	/* wait for device response */
	for (i = 0; i < 100; i++) {
		fw_ready = rtw_read8(priv, SDIO_REG_CPU_IND);
		if ((fw_ready & SDIO_SYSTEM_TRX_RDY_IND) == target_cpu_rdy_bit) {
			break;
		}
		msleep(1);
	}
	if (i == 100) {
		priv->dev_state = old_state;
		dev_err(&priv->func->dev, "wait for device response timeout, restore dev state to %s\n", old_state ? "SLEEP" : "ACTIVE");
		ret = false;
		goto exit;
	}

exit:
	return ret;
}

int rtw_suspend_common(struct inic_sdio *priv)
{
	/* staion mode */
	if (llhw_wifi_is_connected_to_ap() == 0) {
		/* update ip address success */
		if (llhw_wifi_update_ip_addr_in_wowlan()) {
			return -EPERM;
		}
	}

	/* set wowlan_state, stop schedule rx/tx work */
	global_idev.wowlan_state = 1;
	netif_tx_stop_all_queues(global_idev.pndev[0]);

	/* suspend device */
	if (!rtw_sdio_rpwm_notify(priv, RPWM2_PWR_SUSPEND)) {
		return -EPERM;
	}

	return 0;
}

int rtw_sdio_suspend(struct device *dev)
{
	struct sdio_func *func = container_of(dev, struct sdio_func, dev);
	struct inic_sdio *priv = &inic_sdio_priv;

	dev_dbg(dev, "%s", __func__);

	if (rtw_netdev_priv_is_on(global_idev.pndev[1])) {
		/* AP is up, stop to suspend */
		return -EPERM;
	}

	if (rtw_suspend_common(priv)) {
		goto FAIL;
	}

	if (func) {
		mmc_pm_flag_t pm_flag = sdio_get_host_pm_caps(func);
		dev_dbg(dev, "pm_flag:0x%x\n", pm_flag);
		if (!(pm_flag & MMC_PM_KEEP_POWER)) {
			dev_dbg(dev, "%s: can't remain alive while host is suspended\n", sdio_func_id(func));
			return -ENOSYS;
		} else {
			sdio_set_host_pm_flags(func,  MMC_PM_WAKE_SDIO_IRQ); //MMC_PM_KEEP_POWER
			dev_dbg(dev, "suspend with MMC_PM_WAKE_SDIO_IRQ\n");
		}
#ifndef SDIO_HOST_FAKE_SLEEP
		/* release irq */
		if (priv->irq_alloc) {
			sdio_claim_host(func);
			if (sdio_release_irq(func)) {
				dev_err(dev, "%s: sdio_release_irq fail\n", __func__);
			} else {
				priv->irq_alloc = 0;
			}
			sdio_release_host(func);
		}
#endif
	}

	return 0;

FAIL:
	netif_tx_start_all_queues(global_idev.pndev[0]);
	netif_tx_wake_all_queues(global_idev.pndev[0]);
	global_idev.wowlan_state = 0;

	/* wakeup recv and xmit work */
	if (!work_pending(&(global_idev.msg_priv.msg_work))) {
		schedule_work(&(global_idev.msg_priv.msg_work));
	}
	if (!work_pending(&(global_idev.xmit_priv.tx_msg_priv.msg_work))) {
		schedule_work(&(global_idev.xmit_priv.tx_msg_priv.msg_work));
	}

	return -EPERM;
}

int rtw_resume_common(struct inic_sdio *priv)
{
	/* wakeup device */
	if (!rtw_sdio_rpwm_notify(priv, RPWM2_PWR_RESUME)) {
		dev_err(&priv->func->dev, "%s: wakeup device FAIL!\n", __func__);
		return -EPERM;
	}

	netif_tx_start_all_queues(global_idev.pndev[0]);
	netif_tx_wake_all_queues(global_idev.pndev[0]);

	global_idev.wowlan_state = 0;

	/* wakeup recv and xmit work */
	if (!work_pending(&(global_idev.msg_priv.msg_work))) {
		schedule_work(&(global_idev.msg_priv.msg_work));
	}
	if (!work_pending(&(global_idev.xmit_priv.tx_msg_priv.msg_work))) {
		schedule_work(&(global_idev.xmit_priv.tx_msg_priv.msg_work));
	}
	return 0;
}

int rtw_sdio_resume(struct device *dev)
{
	int ret;
	struct sdio_func *func = container_of(dev, struct sdio_func, dev);
	struct inic_sdio *priv = &inic_sdio_priv;
	u32 himr;
	u8 value;

	dev_dbg(dev, "%s", __func__);

	/* some sdio local registers and CCCR would be reset after kernel resume from suspend */
	sdio_claim_host(func);
	ret = sdio_set_block_size(func, 512);
	if (ret) {
		dev_err(dev, "%s: sdio_set_block_size FAIL(%d)!\n", __func__, ret);
		sdio_release_host(func);
		return -EPERM;
	}
	sdio_release_host(func);

	value = rtw_read8(priv, SDIO_REG_TX_CTRL) | SDIO_EN_HISR_MASK_TIMER;
	rtw_write8(priv, SDIO_REG_TX_CTRL, value);

	rtw_write16(priv, SDIO_REG_STATIS_RECOVERY_TIMOUT, 0x10); //500us

#ifdef CONFIG_SDIO_TX_ENABLE_AVAL_INT
	rtw_sdio_init_txavailbd_threshold(priv);
#endif

#ifdef CONFIG_SDIO_RX_AGGREGATION
	if (rtw_sdio_init_agg_setting(priv) == false) {
		return -EPERM;
	}
#endif
	rtw_sdio_query_txbd_status(priv);

	/* alloc irq */
	rtw_sdio_alloc_irq(priv);

	/* Enable interrupt */
	himr = cpu_to_le32(priv->sdio_himr);
	sdio_local_write(priv, SDIO_REG_HIMR, 4, (u8 *)&himr);

	/* Wakeup device */
	if (rtw_resume_common(priv)) {
		return -EPERM;
	}

	return 0;
}

struct dev_pm_ops sdio_pm_ops = {
	.suspend = rtw_sdio_suspend,
	.resume = rtw_sdio_resume,
};

static const struct sdio_device_id sdio_ids[] = {
	{ SDIO_DEVICE(0x024c, 0x8722), .driver_data = 0},
};

static struct sdio_driver inic_sdio_driver = {
	.probe	= rtw_sdio_probe,
	.remove	= rtw_sdio_remove,
	.name	= "INIC_SDIO",
	.id_table	= sdio_ids,
	.drv = {
		.pm = &sdio_pm_ops,
	},
};

static int __init rtw_sdio_init_module(void)
{
	int ret = 0;

	printk("%s\n", __func__);

	ret = sdio_register_driver(&inic_sdio_driver);
	if (ret != 0) {
		printk("sdio register driver Failed!\n");
	}

	rtw_inetaddr_notifier_register();

	return ret;
}

static void __exit rtw_sdio_cleanup_module(void)
{
	rtw_inetaddr_notifier_unregister();

	sdio_unregister_driver(&inic_sdio_driver);

	printk("%s\n", __func__);
}

module_init(rtw_sdio_init_module);
module_exit(rtw_sdio_cleanup_module);

MODULE_AUTHOR("Realtek");
MODULE_DESCRIPTION("RealTek iNIC Fullmac");
MODULE_LICENSE("GPL");
MODULE_VERSION("rtl8721da");

