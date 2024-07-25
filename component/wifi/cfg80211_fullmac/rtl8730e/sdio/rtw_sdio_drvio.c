#include <rtw_cfg80211_fullmac.h>

/*
* Increase and check if the continual_io_error of this @param dvobjprive is larger than MAX_CONTINUAL_IO_ERR
* @return true:
* @return false:
*/
int rtw_inc_and_chk_continual_io_error(struct inic_sdio *priv)
{
	int ret = false;
	int value;
	if ((value = atomic_inc_return(&priv->continual_io_error)) > MAX_CONTINUAL_IO_ERR) {
		dev_err(&priv->func->dev, "[ERROR] continual_io_error:%d > %d\n", value, MAX_CONTINUAL_IO_ERR);
		ret = true;
	}

	return ret;
}

/*
* Set the continual_io_error of this @param dvobjprive to 0
*/
void rtw_reset_continual_io_error(struct inic_sdio *priv)
{
	atomic_set(&priv->continual_io_error, 0);
}

bool rtw_sdio_claim_host_needed(struct sdio_func *func)
{
	struct inic_sdio *priv = sdio_get_drvdata(func);

	if (priv->sys_sdio_irq_thd && (priv->sys_sdio_irq_thd == current)) {
		return false;
	}

	return true;
}

/*
 * Return:
 *	0		Success
 *	others	Fail
 */
s32 sd_cmd52_read(struct inic_sdio *priv, u32 addr, u32 cnt, u8 *pdata)
{

	int err = 0;
	struct sdio_func *func;
	bool claim_needed;
	u32 i;

	func = priv->func;
	claim_needed = rtw_sdio_claim_host_needed(func);

	if (claim_needed) {
		sdio_claim_host(func);
	}

	for (i = 0; i < cnt; i++) {
		pdata[i] = sdio_readb(func, addr + i, &err);
		if (err) {
			dev_err(&priv->func->dev, "%s(): sdio_readb failed, err:%d!\n", __func__, err);
			break;
		}
	}

	if (err) {
		int j;

		err = 0;
		for (j = 0; j < SD_IO_TRY_CNT; j++) {
			for (i = 0; i < cnt; i++) {
				pdata[i] = sdio_readb(func, addr + i, &err);
				if (err) {
					dev_err(&priv->func->dev, "%s(): sdio_readb failed, err:%d!\n", __func__, err);
					break;
				}
			}

			if (err == 0) {
				rtw_reset_continual_io_error(priv);
				break;
			} else {
				if ((-ESHUTDOWN == err) || (-ENODEV == err)) {
					priv->bSurpriseRemoved = true;
				}

				if (rtw_inc_and_chk_continual_io_error(priv) == true) {
					priv->bSurpriseRemoved = true;
#ifdef DBG_STACK_TRACE
					dump_stack();
#endif
					break;
				}
			}
		}

		if (j == SD_IO_TRY_CNT) {
			dev_err(&priv->func->dev, "%s: FAIL!(%d) addr=0x%05x, try_cnt=%d\n", __func__, err, addr, j);
		}

	}

	if (claim_needed) {
		sdio_release_host(func);
	}

	return err;
}

/*
 * Return:
 *	0		Success
 *	others	Fail
 */
s32 sd_cmd52_write(struct inic_sdio *priv, u32 addr, u32 cnt, u8 *pdata)
{
	int err = 0;
	struct sdio_func *func;
	bool claim_needed;
	u32 i;

	func = priv->func;
	claim_needed = rtw_sdio_claim_host_needed(func);

	if (claim_needed) {
		sdio_claim_host(func);
	}

	for (i = 0; i < cnt; i++) {
		sdio_writeb(func, pdata[i], addr + i, &err);
		if (err) {
			dev_err(&priv->func->dev, "%s: FAIL!(%d) addr=0x%05x val=0x%02x\n", __func__, err, addr + i, pdata[i]);
			break;
		}
	}

	if (err) {
		int j;

		err = 0;
		for (j = 0; j < SD_IO_TRY_CNT; j++) {
			for (i = 0; i < cnt; i++) {
				sdio_writeb(func, pdata[i], addr + i, &err);
				if (err) {
					dev_err(&priv->func->dev, "%s(): sdio_writeb failed!\n", __func__);
					break;
				}
			}

			if (err == 0) {
				rtw_reset_continual_io_error(priv);
				break;
			} else {
				if ((-ESHUTDOWN == err) || (-ENODEV == err)) {
					priv->bSurpriseRemoved = true;
				}

				if (rtw_inc_and_chk_continual_io_error(priv) == true) {
					priv->bSurpriseRemoved = true;
#ifdef DBG_STACK_TRACE
					dump_stack();
#endif
					break;
				}
			}
		}

		if (j == SD_IO_TRY_CNT) {
			dev_err(&priv->func->dev, "%s: FAIL!(%d) addr=0x%05x, try_cnt=%d\n", __func__, err, addr, j);
		}

	}

	if (claim_needed) {
		sdio_release_host(func);
	}

	return err;
}

u8 sd_read8(struct inic_sdio *priv, u32 addr, s32 *err)
{
	u8 v = 0;
	struct sdio_func *func;
	bool claim_needed;

	func = priv->func;
	claim_needed = rtw_sdio_claim_host_needed(func);

	if (claim_needed) {
		sdio_claim_host(func);
	}
	v = sdio_readb(func, addr, err);
	if (claim_needed) {
		sdio_release_host(func);
	}
	if (err && *err) {
		dev_err(&priv->func->dev, "%s: FAIL!(%d) addr=0x%05x\n", __func__, *err, addr);
	}

	return v;
}

u8 sd_f0_read8(struct inic_sdio *priv, u32 addr, s32 *err)
{
	u8 v = 0;
	struct sdio_func *func;
	bool claim_needed;

	func = priv->func;
	claim_needed = rtw_sdio_claim_host_needed(func);

	if (claim_needed) {
		sdio_claim_host(func);
	}
	v = sdio_f0_readb(func, addr, err);
	if (claim_needed) {
		sdio_release_host(func);
	}
	if (err && *err) {
		dev_err(&priv->func->dev, "%s: FAIL!(%d) addr=0x%05x\n", __func__, *err, addr);
	}

	return v;
}

void sd_f0_write8(struct inic_sdio *priv, u32 addr, u8 v, s32 *err)
{
	struct sdio_func *func;
	bool claim_needed;

	func = priv->func;
	claim_needed = rtw_sdio_claim_host_needed(func);

	if (claim_needed) {
		sdio_claim_host(func);
	}
	sdio_f0_writeb(func, v, addr, err);
	if (claim_needed) {
		sdio_release_host(func);
	}
	if (err && *err) {
		dev_err(&priv->func->dev, "%s: FAIL!(%d) addr=0x%05x val=0x%02x\n", __func__, *err, addr, v);
	}

}

u16 sd_read16(struct inic_sdio *priv, u32 addr, s32 *err)
{
	u16 v = 0;
	struct sdio_func *func;
	bool claim_needed;

	func = priv->func;
	claim_needed = rtw_sdio_claim_host_needed(func);

	if (claim_needed) {
		sdio_claim_host(func);
	}
	v = sdio_readw(func, addr, err);
	if (claim_needed) {
		sdio_release_host(func);
	}

	if (err && *err) {
		int i;

		dev_err(&priv->func->dev, "%s: (%d) addr=0x%05x, val=0x%x\n", __func__, *err, addr, v);

		*err = 0;
		for (i = 0; i < SD_IO_TRY_CNT; i++) {
			if (claim_needed) {
				sdio_claim_host(func);
			}
			v = sdio_readw(func, addr, err);
			if (claim_needed) {
				sdio_release_host(func);
			}

			if (*err == 0) {
				rtw_reset_continual_io_error(priv);
				break;
			} else {
				dev_err(&priv->func->dev, "%s: (%d) addr=0x%05x, val=0x%x, try_cnt=%d\n", __func__, *err, addr, v, i);
				if ((-ESHUTDOWN == *err) || (-ENODEV == *err)) {
					priv->bSurpriseRemoved = true;
				}

				if (rtw_inc_and_chk_continual_io_error(priv) == true) {
					priv->bSurpriseRemoved = true;
					break;
				}
			}
		}

		if (i == SD_IO_TRY_CNT) {
			dev_err(&priv->func->dev, "%s: FAIL!(%d) addr=0x%05x, val=0x%x, try_cnt=%d\n", __func__, *err, addr, v, i);
		}
	}

	return  v;
}

u32 sd_read32(struct inic_sdio *priv, u32 addr, s32 *err)
{
	u32 v = 0;
	struct sdio_func *func;
	bool claim_needed;

	func = priv->func;
	claim_needed = rtw_sdio_claim_host_needed(func);

	if (claim_needed) {
		sdio_claim_host(func);
	}
	v = sdio_readl(func, addr, err);
	if (claim_needed) {
		sdio_release_host(func);
	}

	if (err && *err) {
		int i;

		dev_err(&priv->func->dev, "%s: (%d) addr=0x%05x, val=0x%x\n", __func__, *err, addr, v);

		*err = 0;
		for (i = 0; i < SD_IO_TRY_CNT; i++) {
			if (claim_needed) {
				sdio_claim_host(func);
			}
			v = sdio_readl(func, addr, err);
			if (claim_needed) {
				sdio_release_host(func);
			}

			if (*err == 0) {
				rtw_reset_continual_io_error(priv);
				break;
			} else {
				dev_err(&priv->func->dev, "%s: (%d) addr=0x%05x, val=0x%x, try_cnt=%d\n", __func__, *err, addr, v, i);
				if ((-ESHUTDOWN == *err) || (-ENODEV == *err)) {
					priv->bSurpriseRemoved = true;
				}

				if (rtw_inc_and_chk_continual_io_error(priv) == true) {
					priv->bSurpriseRemoved = true;
					break;
				}
			}
		}

		if (i == SD_IO_TRY_CNT) {
			dev_err(&priv->func->dev, "%s: FAIL!(%d) addr=0x%05x, val=0x%x, try_cnt=%d\n", __func__, *err, addr, v, i);
		}
	}

	return  v;
}

void sd_write8(struct inic_sdio *priv, u32 addr, u8 v, s32 *err)
{
	struct sdio_func *func;
	bool claim_needed;

	func = priv->func;
	claim_needed = rtw_sdio_claim_host_needed(func);

	if (claim_needed) {
		sdio_claim_host(func);
	}
	sdio_writeb(func, v, addr, err);
	if (claim_needed) {
		sdio_release_host(func);
	}

	if (err && *err) {
		int i;

		dev_err(&priv->func->dev, "%s: (%d) addr=0x%05x, val=0x%x\n", __func__, *err, addr, v);

		*err = 0;
		for (i = 0; i < SD_IO_TRY_CNT; i++) {
			if (claim_needed) {
				sdio_claim_host(func);
			}
			sdio_writeb(func, v, addr, err);
			if (claim_needed) {
				sdio_release_host(func);
			}

			if (*err == 0) {
				rtw_reset_continual_io_error(priv);
				break;
			} else {
				dev_err(&priv->func->dev, "%s: (%d) addr=0x%05x, val=0x%x, try_cnt=%d\n", __func__, *err, addr, v, i);
				if ((-ESHUTDOWN == *err) || (-ENODEV == *err)) {
					priv->bSurpriseRemoved = true;
				}

				if (rtw_inc_and_chk_continual_io_error(priv) == true) {
					priv->bSurpriseRemoved = true;
					break;
				}
			}
		}

		if (i == SD_IO_TRY_CNT) {
			dev_err(&priv->func->dev, "%s: FAIL!(%d) addr=0x%05x, val=0x%x, try_cnt=%d\n", __func__, *err, addr, v, i);
		}
	}
}

void sd_write16(struct inic_sdio *priv, u32 addr, u16 v, s32 *err)
{
	struct sdio_func *func;
	bool claim_needed;

	func = priv->func;
	claim_needed = rtw_sdio_claim_host_needed(func);

	if (claim_needed) {
		sdio_claim_host(func);
	}
	sdio_writew(func, v, addr, err);
	if (claim_needed) {
		sdio_release_host(func);
	}

	if (err && *err) {
		int i;

		dev_err(&priv->func->dev, "%s: (%d) addr=0x%05x, val=0x%x\n", __func__, *err, addr, v);

		*err = 0;
		for (i = 0; i < SD_IO_TRY_CNT; i++) {
			if (claim_needed) {
				sdio_claim_host(func);
			}
			sdio_writew(func, v, addr, err);
			if (claim_needed) {
				sdio_release_host(func);
			}

			if (*err == 0) {
				rtw_reset_continual_io_error(priv);
				break;
			} else {
				dev_err(&priv->func->dev, "%s: (%d) addr=0x%05x, val=0x%x, try_cnt=%d\n", __func__, *err, addr, v, i);
				if ((-ESHUTDOWN == *err) || (-ENODEV == *err)) {
					priv->bSurpriseRemoved = true;
				}

				if (rtw_inc_and_chk_continual_io_error(priv) == true) {
					priv->bSurpriseRemoved = true;
					break;
				}
			}
		}

		if (i == SD_IO_TRY_CNT) {
			dev_err(&priv->func->dev, "%s: FAIL!(%d) addr=0x%05x, val=0x%x, try_cnt=%d\n", __func__, *err, addr, v, i);
		}
	}
}

void sd_write32(struct inic_sdio *priv, u32 addr, u32 v, s32 *err)
{
	struct sdio_func *func;
	bool claim_needed;

	func = priv->func;
	claim_needed = rtw_sdio_claim_host_needed(func);

	if (claim_needed) {
		sdio_claim_host(func);
	}
	sdio_writel(func, v, addr, err);
	if (claim_needed) {
		sdio_release_host(func);
	}

	if (err && *err) {
		int i;

		dev_err(&priv->func->dev, "%s: (%d) addr=0x%05x val=0x%08x\n", __func__, *err, addr, v);

		*err = 0;
		for (i = 0; i < SD_IO_TRY_CNT; i++) {
			if (claim_needed) {
				sdio_claim_host(func);
			}
			sdio_writel(func, v, addr, err);
			if (claim_needed) {
				sdio_release_host(func);
			}
			if (*err == 0) {
				rtw_reset_continual_io_error(priv);
				break;
			} else {
				dev_err(&priv->func->dev, "%s: (%d) addr=0x%05x, val=0x%x, try_cnt=%d\n", __func__, *err, addr, v, i);
				if ((-ESHUTDOWN == *err) || (-ENODEV == *err)) {
					priv->bSurpriseRemoved = true;
				}

				if (rtw_inc_and_chk_continual_io_error(priv) == true) {
					priv->bSurpriseRemoved = true;
					break;
				}
			}
		}

		if (i == SD_IO_TRY_CNT) {
			dev_err(&priv->func->dev, "%s: FAIL!(%d) addr=0x%05x val=0x%08x, try_cnt=%d\n", __func__, *err, addr, v, i);
		} else {
			dev_err(&priv->func->dev, "%s: (%d) addr=0x%05x val=0x%08x, try_cnt=%d\n", __func__, *err, addr, v, i);
		}
	}
}

/*
 * Use CMD53 to read data from SDIO device.
 *
 * Parameters:
 *	psdio	pointer of SDIO_DATA
 *	addr	address to read
 *	cnt		amount to read
 *	pdata	pointer to put data, this should be a "DMA:able scratch buffer"!
 *
 * Return:
 *	0		Success
 *	others	Fail
 */
s32 sd_read(struct inic_sdio *priv, u32 addr, u32 cnt, void *pdata)
{
	struct sdio_func *func;
	bool claim_needed;
	s32 err = -EPERM;

	func = priv->func;
	claim_needed = rtw_sdio_claim_host_needed(func);

	if (claim_needed) {
		sdio_claim_host(func);
	}

	if (unlikely((cnt == 1) || (cnt == 2))) {
		u32 i;
		u8 *pbuf = (u8 *)pdata;

		for (i = 0; i < cnt; i++) {
			*(pbuf + i) = sdio_readb(func, addr + i, &err);

			if (err) {
				dev_err(&priv->func->dev, "%s: FAIL!(%d) addr=0x%05x\n", __func__, err, addr);
				break;
			}
		}
		return err;
	}

	err = sdio_memcpy_fromio(func, pdata, addr, cnt);
	if (err) {
		dev_err(&priv->func->dev, "%s: FAIL(%d)! ADDR=0x%x Size=%d\n", __func__, err, addr, cnt);
#ifdef DBG_STACK_TRACE
		dump_stack();
#endif
	} else {
		//dev_dbg(&priv->func->dev, "%s: SUCCESS(%d)! ADDR=0x%x Size=%d\n", __func__, err, addr, cnt);
	}

	if (claim_needed) {
		sdio_release_host(func);
	}

	return err;
}

/*
 * Use CMD53 to write data to SDIO device.
 *
 * Parameters:
 *  psdio	pointer of SDIO_DATA
 *  addr	address to write
 *  cnt		amount to write
 *  pdata	data pointer, this should be a "DMA:able scratch buffer"!
 *
 * Return:
 *  0		Success
 *  others	Fail
 */
s32 sd_write(struct inic_sdio *priv, u32 addr, u32 cnt, void *pdata)
{
	struct sdio_func *func;
	bool claim_needed;
	s32 err = -EPERM;

	func = priv->func;
	claim_needed = rtw_sdio_claim_host_needed(func);

	if (claim_needed) {
		sdio_claim_host(func);
	}

	if (unlikely((cnt == 1) || (cnt == 2))) {
		u32 i;
		u8 *pbuf = (u8 *)pdata;

		for (i = 0; i < cnt; i++) {
			sdio_writeb(func, *(pbuf + i), addr + i, &err);
			if (err) {
				dev_err(&priv->func->dev, "%s: FAIL!(%d) addr=0x%05x val=0x%02x\n", __func__, err, addr, *(pbuf + i));
				break;
			}
		}

		return err;
	}

	err = sdio_memcpy_toio(func, addr, pdata, cnt);
	if (err) {
		dev_err(&priv->func->dev, "%s: FAIL(%d)! ADDR=0x%x Size=%d(%d)\n", __func__, err, addr, cnt, cnt);
	}

	if (claim_needed) {
		sdio_release_host(func);
	}

	return err;
}

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


