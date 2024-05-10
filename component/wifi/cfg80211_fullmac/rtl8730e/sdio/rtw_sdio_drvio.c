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

