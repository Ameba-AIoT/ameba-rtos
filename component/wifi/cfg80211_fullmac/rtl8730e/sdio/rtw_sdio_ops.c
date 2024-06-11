#include <rtw_cfg80211_fullmac.h>

//
// Description:
//	The following mapping is for SDIO host local register space.
static void sdio_get_cmdaddr(
	u8				DomainID,
	u32				Param,
	u32			*pCmdAddr
)
{
	switch (DomainID) {
	case SDIO_LOCAL_DOMAIN_ID:
		*pCmdAddr = ((SDIO_LOCAL_DOMAIN_ID << 13) | (Param & SDIO_LOCAL_MSK));
		break;

	case SDIO_TX_FIFO_DOMAIN_ID:
		*pCmdAddr = ((SDIO_TX_FIFO_DOMAIN_ID << 13) | (Param & SDIO_TX_FIFO_MSK));
		break;

	case SDIO_RX_FIFO_DOMAIN_ID:
		*pCmdAddr = ((SDIO_RX_FIFO_DOMAIN_ID << 13) | (Param & SDIO_RX_FIFO_MSK));
		break;

	default:
		break;
	}
}
static u8 sdio_get_domainid(u32 addr)
{
	u8 domainId;
	u16 pseudoId;


	pseudoId = (u16)(addr >> 16);
	switch (pseudoId) {
	case 0x1025:
		domainId = SDIO_LOCAL_DOMAIN_ID;
		break;

	case 0x1031:
		domainId = SDIO_TX_FIFO_DOMAIN_ID;
		break;

	case 0x1034:
		domainId = SDIO_RX_FIFO_DOMAIN_ID;
		break;

	default:
		domainId = SDIO_LOCAL_DOMAIN_ID;
		break;
	}

	return domainId;
}

static u32 sdio_convert_to_cmdaddr(const u32 addr, u8 *pdeviceId, u16 *poffset)
{
	u8 domainId;
	u16 offset;
	u32 ftaddr;


	domainId = sdio_get_domainid(addr);
	offset = 0;

	switch (domainId) {
	case SDIO_LOCAL_DOMAIN_ID:
		offset = addr & SDIO_LOCAL_MSK;
		break;

	case SDIO_TX_FIFO_DOMAIN_ID:
		offset = addr & SDIO_TX_FIFO_MSK;
		break;

	case SDIO_RX_FIFO_DOMAIN_ID:
		offset = addr & SDIO_RX_FIFO_MSK;
		break;

	default:
		domainId = SDIO_LOCAL_DOMAIN_ID;
		offset = addr & SDIO_LOCAL_MSK;
		break;
	}
	ftaddr = (domainId << 13) | offset;

	if (pdeviceId) {
		*pdeviceId = domainId;
	}
	if (poffset) {
		*poffset = offset;
	}

	return ftaddr;
}

u8 sdio_read8(struct inic_sdio *priv, u32 addr)
{
	u32 ftaddr;
	u8 val;

	ftaddr = sdio_convert_to_cmdaddr(addr, NULL, NULL);
	sd_cmd52_read(priv, ftaddr, 1, (u8 *)&val);

	return val;
}

u8 sdio_f0_read8(struct inic_sdio *priv, u32 addr)
{
	u8 val;

	val = sd_f0_read8(priv, addr, NULL);

	return val;
}

u16 sdio_read16(struct inic_sdio *priv, u32 addr)
{
	u32 ftaddr;
	u16 val;

	ftaddr = sdio_convert_to_cmdaddr(addr, NULL, NULL);
	sd_cmd52_read(priv, ftaddr, 2, (u8 *)&val);

	val = le16_to_cpu(val);

	return val;
}

u32 sdio_read32(struct inic_sdio *priv, u32 addr)
{
	u32 ftaddr;
	u32 val;
	u8 shift;
//	s32 err;

	ftaddr = sdio_convert_to_cmdaddr(addr, NULL, NULL);

	// 4 bytes alignment
	shift = ftaddr & 0x3;
	if (shift == 0) {
		val = sd_read32(priv, ftaddr, NULL);
	} else {
		u8 *ptmpbuf;

		ptmpbuf = (u8 *)kzalloc(8, GFP_KERNEL);
		if (NULL == ptmpbuf) {
			dev_err(&priv->func->dev, "%s: Allocate memory FAIL!(size=8) addr=0x%x\n", __func__, addr);
			return false;
		}

		ftaddr &= ~(u16)0x3;
		sd_read(priv, ftaddr, 8, ptmpbuf);
		memcpy(&val, ptmpbuf + shift, 4);
		val = le32_to_cpu(val);

		kfree(ptmpbuf);
	}

	return val;
}

s32 sdio_readN(struct inic_sdio *priv, u32 addr, u32 cnt, u8 *pbuf)
{

	u8 deviceId;
	u16 offset;
	u32 ftaddr;
	u8 shift;
	s32 err = 0;

	ftaddr = sdio_convert_to_cmdaddr(addr, &deviceId, &offset);

	// 4 bytes alignment
	shift = ftaddr & 0x3;
	if (shift == 0) {
		err = sd_read(priv, ftaddr, cnt, pbuf);
	} else {
		u8 *ptmpbuf;
		u32 n;

		ftaddr &= ~(u16)0x3;
		n = cnt + shift;
		ptmpbuf = (u8 *)kzalloc(n, GFP_KERNEL);
		if (NULL == ptmpbuf) {
			return -1;
		}
		err = sd_read(priv, ftaddr, n, ptmpbuf);
		if (!err) {
			memcpy(pbuf, ptmpbuf + shift, cnt);
		}
		kfree(ptmpbuf);
	}

	return err;
}

void sdio_read_mem(struct inic_sdio *priv, u32 addr, u32 cnt, u8 *rmem)
{
	s32 err;

	err = sdio_readN(priv, addr, cnt, rmem);
}

/*
 * Description:
 *	Read from RX FIFO
 *	Round read size to block size,
 *	and make sure data transfer will be done in one command.
 *
 * Parameters:
 *	func		a pointer of sdio func
 *	addr		port ID
 *	cnt			size to read
 *	rmem		address to put data
 *
 * Return:
 *	true(1)		Success
 *	false(0)		Fail
 */

u32 sdio_read_port(
	struct inic_sdio *priv,
	u32 addr,
	u32 cnt,
	u8 *mem)
{
	s32 err;

	sdio_get_cmdaddr(addr, priv->SdioRxFIFOCnt++, &addr);

	cnt = _RND4(cnt);

	if (cnt >  priv->block_transfer_len) {
		cnt = _RND(cnt,  priv->block_transfer_len);
	}

	err = sd_read(priv, addr, cnt, mem);
	if (err) {
		return false;
	}
	return true;
}


s32 sdio_write8(struct inic_sdio *priv, u32 addr, u8 val)
{
	u32 ftaddr;
	s32 err;

	ftaddr = sdio_convert_to_cmdaddr(addr, NULL, NULL);
	sd_write8(priv, ftaddr, val, &err);

	return err;
}

s32 sdio_write16(struct inic_sdio *priv, u32 addr, u16 val)
{
	u32 ftaddr;
	s32 err;

	ftaddr = sdio_convert_to_cmdaddr(addr, NULL, NULL);
	val = cpu_to_le16(val);
	err = sd_cmd52_write(priv, ftaddr, 2, (u8 *)&val);

	return err;
}

s32 sdio_write32(struct inic_sdio *priv, u32 addr, u32 val)
{
	u8 deviceId;
	u16 offset;
	u32 ftaddr;
	u8 shift;
	s32 err = 0;

	ftaddr = sdio_convert_to_cmdaddr(addr, &deviceId, &offset);

	// 4 bytes alignment
	shift = ftaddr & 0x3;

	if (shift == 0) {
		sd_write32(priv, ftaddr, val, &err);
	} else {
		val = cpu_to_le32(val);
		err = sd_cmd52_write(priv, ftaddr, 4, (u8 *)&val);
	}

	return err;
}

s32 sdio_writeN(struct inic_sdio *priv, u32 addr, u32 cnt, u8 *pbuf)
{

	u8 deviceId;
	u16 offset;
	u32 ftaddr;
	u8 shift;
	s32 err = 0;

	ftaddr = sdio_convert_to_cmdaddr(addr, &deviceId, &offset);

	shift = ftaddr & 0x3;
	if (shift == 0) {
		err = sd_write(priv, ftaddr, cnt, pbuf);
	} else {
		u8 *ptmpbuf;
		u32 n;

		ftaddr &= ~(u16)0x3;
		n = cnt + shift;
		ptmpbuf = (u8 *)kzalloc(n, GFP_KERNEL);
		if (NULL == ptmpbuf) {
			return -1;
		}
		err = sd_read(priv, ftaddr, 4, ptmpbuf);
		if (err) {
			kfree(ptmpbuf);
			return err;
		}
		memcpy(ptmpbuf + shift, pbuf, cnt);
		err = sd_write(priv, ftaddr, n, ptmpbuf);
		kfree(ptmpbuf);
	}

	return err;
}

void sdio_write_mem(struct inic_sdio *priv, u32 addr, u32 cnt, u8 *wmem)
{
	sdio_writeN(priv, addr, cnt, wmem);
}

/*
 * Description:
 *	Write to TX FIFO
 *	Align write size block size,
 *	and make sure data could be written in one command.
 *
 * Parameters:
 *	pintfhdl	a pointer of intf_hdl
 *	addr		port ID
 *	cnt			size to write
 *	wmem		data pointer to write
 *
 * Return:
 *	true(1)		Success
 *	false(0)		Fail
 */
u32 sdio_write_port(
	struct inic_sdio *priv,
	u32 addr,
	u32 cnt,
	u8 *mem)
{
	s32 err;
	cnt = _RND4(cnt);

	sdio_get_cmdaddr(addr, cnt >> 2, &addr);

	if (cnt >  priv->block_transfer_len) {
		cnt = _RND(cnt,  priv->block_transfer_len);
	}

	err = sd_write(priv, addr, cnt, mem);

	if (err) {
		dev_err(&priv->func->dev, "%s, error=%d\n", __func__, err);
		return false;
	}
	return true;
}

/*
 * align address to 4 bytes in future.
 */
s32 sdio_local_read(
	struct inic_sdio *priv,
	u32		addr,
	u32		cnt,
	u8		*pbuf)
{

	s32 err;
	u8 *ptmpbuf;
	u32 n, retry = 0;
	sdio_get_cmdaddr(SDIO_LOCAL_DOMAIN_ID, addr, &addr);

	n = RND4(cnt);
	ptmpbuf = (u8 *)kzalloc(n, GFP_KERNEL);
	if (!ptmpbuf) {
		return (-1);
	}

	while (1) {
		err = sd_read(priv, addr, n, ptmpbuf);
		if (!err) {
			memcpy(pbuf, ptmpbuf, cnt);
			break;
		} else {
			/* retry to reduce impact of bus err */
			if (retry ++ > 10) {
				break;
			}
		}
	}

	if (ptmpbuf) {
		kfree(ptmpbuf);
	}

	return err;
}

/*
 * align address to 4 bytes in future.
 */
s32 sdio_local_write(
	struct inic_sdio *priv,
	u32		addr,
	u32		cnt,
	u8		*pbuf)
{
	s32 err;
	u8 *ptmpbuf;
	if (addr & 0x3) {
		dev_err(&priv->func->dev, "%s, address must be 4 bytes alignment\n", __FUNCTION__);
	}

	if (cnt  & 0x3) {
		dev_err(&priv->func->dev, "%s, size must be the multiple of 4 \n", __FUNCTION__);
	}

	sdio_get_cmdaddr(SDIO_LOCAL_DOMAIN_ID, addr, &addr);

	ptmpbuf = (u8 *)kzalloc(cnt, GFP_KERNEL);
	if (!ptmpbuf) {
		return (-1);
	}

	memcpy(ptmpbuf, pbuf, cnt);

	err = sd_write(priv, addr, cnt, ptmpbuf);

	if (ptmpbuf) {
		kfree(ptmpbuf);
	}

	return err;
}


u8 sdio_cmd52_read1byte_local(struct inic_sdio *priv, u32 addr)
{

	u8 val = 0;

	sdio_get_cmdaddr(SDIO_LOCAL_DOMAIN_ID, addr, &addr);
	sd_cmd52_read(priv, addr, 1, &val);

	return val;
}

u16 sdio_cmd52_read2byte_local(struct inic_sdio *priv, u32 addr)
{

	u16 val = 0;

	sdio_get_cmdaddr(SDIO_LOCAL_DOMAIN_ID, addr, &addr);
	sd_cmd52_read(priv, addr, 2, (u8 *)&val);

	val = le16_to_cpu(val);

	return val;
}

u32 sdio_cmd52_read4byte_local(struct inic_sdio *priv, u32 addr)
{
	u32 val = 0;

	sdio_get_cmdaddr(SDIO_LOCAL_DOMAIN_ID, addr, &addr);
	sd_cmd52_read(priv, addr, 4, (u8 *)&val);

	val = le32_to_cpu(val);

	return val;
}

u32 sdio_cmd53_read4byte_local(struct inic_sdio *priv, u32 addr)
{
	u32 val = 0;

	sdio_get_cmdaddr(SDIO_LOCAL_DOMAIN_ID, addr, &addr);

	val = sd_read32(priv, addr, NULL);

	return val;
}

void sdio_cmd52_write1byte_local(struct inic_sdio *priv, u32 addr, u8 v)
{
	sdio_get_cmdaddr(SDIO_LOCAL_DOMAIN_ID, addr, &addr);
	sd_cmd52_write(priv, addr, 1, &v);
}

void sdio_cmd52_write2byte_local(struct inic_sdio *priv, u32 addr, u16 v)
{
	sdio_get_cmdaddr(SDIO_LOCAL_DOMAIN_ID, addr, &addr);
	v = cpu_to_le16(v);
	sd_cmd52_write(priv, addr, 2, (u8 *)&v);
}

void sdio_cmd52_write4byte_local(struct inic_sdio *priv, u32 addr, u32 v)
{
	sdio_get_cmdaddr(SDIO_LOCAL_DOMAIN_ID, addr, &addr);
	v = cpu_to_le32(v);
	sd_cmd52_write(priv, addr, 4, (u8 *)&v);
}

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

			/* wakeup tx task if waiting in llhw_host_send() */
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

