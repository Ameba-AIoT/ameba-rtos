#include <whc_host_linux.h>

static bool rtw_sdio_claim_host_needed(struct sdio_func *func)
{
	struct whc_sdio *priv = sdio_get_drvdata(func);

	if (priv->sys_sdio_irq_thd && (priv->sys_sdio_irq_thd == current)) {
		return false;
	}

	return true;
}

static void sdio_get_cmdaddr(
	u8				DomainID,
	u32				Param,
	u32			*pCmdAddr
)
{
	switch (DomainID) {
	case SDIO_LOCAL_DOMAIN_ID:
		/* DeviceID[16]=0, DomainID[15:13]=0, Address/Len[12:0] is SDIO or SPDIO Control register */
		*pCmdAddr = ((SDIO_LOCAL_DOMAIN_ID << 13) | (Param & SDIO_LOCAL_MSK));
		break;

	case SDIO_TX_FIFO_DOMAIN_ID:
		/* DeviceID[16]=0, DomainID[15:13]=4, Address/Len[12:0] is Tx packet length, unit is 4-Byte */
		*pCmdAddr = ((SDIO_TX_FIFO_DOMAIN_ID << 13) | (Param & SDIO_TX_FIFO_MSK));
		break;

	case SDIO_RX_FIFO_DOMAIN_ID:
		/* DeviceID[16]=0, DomainID[15:13]=7, Address/Len[12:0] is stuff bit, shall be set to 0 */
		*pCmdAddr = ((SDIO_RX_FIFO_DOMAIN_ID << 13) | (Param & SDIO_RX_FIFO_MSK));
		break;

	default:
		break;
	}
}

u8 sdio_read8(struct whc_sdio *priv, u32 addr)
{
	struct sdio_func *func = priv->func;
	bool claim_needed;
	u32 ftaddr;
	u8 val;
	int err;

	sdio_get_cmdaddr(SDIO_LOCAL_DOMAIN_ID, addr & SDIO_LOCAL_MSK, &ftaddr);

	claim_needed = rtw_sdio_claim_host_needed(func);

	if (claim_needed) {
		sdio_claim_host(func);
	}
	val = sdio_readb(func, ftaddr, &err);
	if (claim_needed) {
		sdio_release_host(func);
	}

	if (err) {
		dev_err(&priv->func->dev, "%s: FAIL(%d)! ADDR=0x%x\n", __func__, err, addr);
		return 0;
	}

	return val;
}

u16 sdio_read16(struct whc_sdio *priv, u32 addr)
{
	struct sdio_func *func = priv->func;
	bool claim_needed;
	u32 ftaddr;
	u16 val;
	int err, i;

	sdio_get_cmdaddr(SDIO_LOCAL_DOMAIN_ID, addr & SDIO_LOCAL_MSK, &ftaddr);

	claim_needed = rtw_sdio_claim_host_needed(func);

	if (claim_needed) {
		sdio_claim_host(func);
	}

	/* Note: use CMD52 to read 16-bit reg, because SPDIO control register can only be accessed by CMD52 */
	for (i = 0; i < 2; i++) {
		*((u8 *)(&val) + i) = sdio_readb(func, ftaddr + i, &err);
		if (err) {
			dev_err(&priv->func->dev, "%s(): sdio_readb failed, err:%d!\n", __func__, err);
			break;
		}
	}

	if (claim_needed) {
		sdio_release_host(func);
	}

	if (err) {
		dev_err(&priv->func->dev, "%s: FAIL(%d)! ADDR=0x%x\n", __func__, err, addr);
		return 0;
	}

	val = le16_to_cpu(val);

	return val;
}

u32 sdio_read32(struct whc_sdio *priv, u32 addr)
{
	struct sdio_func *func = priv->func;
	bool claim_needed;
	u32 ftaddr;
	u32 val;
	int err;

	if (addr & 0x3) {
		dev_err(&priv->func->dev, "%s: FAIL! addr should be 4-byte aligned\n", __func__);
		return 0;
	}

	sdio_get_cmdaddr(SDIO_LOCAL_DOMAIN_ID, addr & SDIO_LOCAL_MSK, &ftaddr);

	claim_needed = rtw_sdio_claim_host_needed(func);

	if (claim_needed) {
		sdio_claim_host(func);
	}
	val = sdio_readl(func, ftaddr, &err);
	if (claim_needed) {
		sdio_release_host(func);
	}

	if (err) {
		dev_err(&priv->func->dev, "%s: FAIL(%d)! ADDR=0x%x\n", __func__, err, addr);
		return 0;
	}

	return val;
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
	struct whc_sdio *priv,
	u32 addr,
	u32 cnt,
	u8 *mem)
{
	struct sdio_func *func = priv->func;
	bool claim_needed;
	s32 err;

	sdio_get_cmdaddr(addr, 0, &addr);

	cnt = _RND4(cnt);
	if (cnt > priv->block_transfer_len) {
		cnt = _RND(cnt,  priv->block_transfer_len);
	}

	claim_needed = rtw_sdio_claim_host_needed(func);

	if (claim_needed) {
		sdio_claim_host(func);
	}

	err = sdio_memcpy_fromio(func, mem, addr, cnt);

	if (claim_needed) {
		sdio_release_host(func);
	}

	if (err) {
		dev_err(&priv->func->dev, "%s: FAIL(%d)! ADDR=0x%x Size=%d\n", __func__, err, addr, cnt);
		return false;
	}
	return true;
}

s32 sdio_write8(struct whc_sdio *priv, u32 addr, u8 val)
{
	struct sdio_func *func = priv->func;
	bool claim_needed;
	u32 ftaddr;
	s32 err;

	sdio_get_cmdaddr(SDIO_LOCAL_DOMAIN_ID, addr & SDIO_LOCAL_MSK, &ftaddr);

	claim_needed = rtw_sdio_claim_host_needed(func);

	if (claim_needed) {
		sdio_claim_host(func);
	}
	sdio_writeb(func, val, ftaddr, &err);
	if (claim_needed) {
		sdio_release_host(func);
	}

	return err;
}

s32 sdio_write16(struct whc_sdio *priv, u32 addr, u16 val)
{
	struct sdio_func *func = priv->func;
	bool claim_needed;
	u32 ftaddr;
	s32 err, i;

	sdio_get_cmdaddr(SDIO_LOCAL_DOMAIN_ID, addr & SDIO_LOCAL_MSK, &ftaddr);
	val = cpu_to_le16(val);

	claim_needed = rtw_sdio_claim_host_needed(func);

	if (claim_needed) {
		sdio_claim_host(func);
	}

	/* Note: use CMD52 to read 16-bit reg, because SPDIO control register can only be accessed by CMD52 */
	for (i = 0; i < 2; i++) {
		sdio_writeb(func, *((u8 *)(&val) + i), ftaddr + i, &err);
		if (err) {
			dev_err(&priv->func->dev, "%s(): sdio_readb failed, err:%d!\n", __func__, err);
			break;
		}
	}

	if (claim_needed) {
		sdio_release_host(func);
	}

	return err;
}

s32 sdio_write32(struct whc_sdio *priv, u32 addr, u32 val)
{
	struct sdio_func *func = priv->func;
	bool claim_needed;
	u32 ftaddr;
	s32 err = 0;

	sdio_get_cmdaddr(SDIO_LOCAL_DOMAIN_ID, addr & SDIO_LOCAL_MSK, &ftaddr);
	val = cpu_to_le32(val);

	claim_needed = rtw_sdio_claim_host_needed(func);

	if (claim_needed) {
		sdio_claim_host(func);
	}
	sdio_writel(func, val, ftaddr, &err);
	if (claim_needed) {
		sdio_release_host(func);
	}

	return err;
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
	struct whc_sdio *priv,
	u32 addr,
	u32 cnt,
	u8 *mem)
{
	struct sdio_func *func = priv->func;
	bool claim_needed;
	s32 err = -EPERM;

	cnt = _RND4(cnt);

	sdio_get_cmdaddr(addr, cnt >> 2, &addr);

	if (cnt > priv->block_transfer_len) {
		cnt = _RND(cnt,  priv->block_transfer_len);
	}

	claim_needed = rtw_sdio_claim_host_needed(func);

	if (claim_needed) {
		sdio_claim_host(func);
	}

	err = sdio_memcpy_toio(func, addr, mem, cnt);

	if (claim_needed) {
		sdio_release_host(func);
	}

	if (err) {
		dev_err(&priv->func->dev, "%s: FAIL(%d)! ADDR=0x%x Size=%d\n", __func__, err, addr, cnt);
		return false;
	}
	return true;
}

