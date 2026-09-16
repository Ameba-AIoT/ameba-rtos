// SPDX-License-Identifier: GPL-2.0-only
/*
 * Realtek wireless local area network IC driver.
 *
 * Copyright(c) 2024 Realtek Corporation. All rights reserved.
 *
 * GSPI wire transport for the Linux host. The device runs its SPDIO IP in
 * SPI-slave mode; this file speaks the GSPI frame protocol over the Linux SPI
 * core. It is a direct port of the SoC-side reference implementation
 * component/soc/amebagreen2/fwlib/ram_common/ameba_gspi.c -- keep the two in
 * sync when the protocol changes.
 *
 * One frame lives inside a single CS-low window. The whole frame is one
 * contiguous buffer and is exchanged in a single full-duplex spi_transfer
 * (tx_buf == rx_buf == buf, len == whole frame), mirroring the in-place byte
 * exchange the SoC reference driver does. What MISO delivers overwrites the
 * buffer in place, so the phase contents end up at fixed offsets:
 *   register read  : send CMD 4B + dummy 12B, recv dummy 4B | Status 8B | Data 4B
 *   register write : send CMD 4B + Data 4B + dummy 8B, recv ... | Status 8B
 *   FIFO  (TX/RX)  : send CMD 4B + [Data NB] + dummy 8B, Status trails (N pad 4)
 * Because MOSI content during a dummy/read phase comes from the buffer itself,
 * the dummy region must be zeroed before the transfer (see the memset in the
 * read paths); the SPI core no longer supplies zeros for a NULL tx_buf.
 *
 * Only the CMD word and register data are byte-swapped when the device is in
 * little-endian mode; FIFO payload is an opaque byte stream and is never
 * swapped. We activate the device in big-endian-32 mode, so on a little-endian
 * host priv->swap ends up 0 and no swap is done -- see gspi_activate().
 */
#include <whc_host_linux.h>

/*
 * Full-duplex transfer buffer strategy:
 *   1 = shared:   tx_buf == rx_buf, MISO overwrites the frame in place. Saves an
 *                 allocation and a copy. Requires the SPI controller to support
 *                 in-place full-duplex (bcm2835 / Raspberry Pi does).
 *   0 = separate: rx goes into its own buffer, then the result is copied back.
 *                 Most portable; use if a controller/DMA engine rejects tx==rx.
 */
#ifndef GSPI_INPLACE_XFER
#define GSPI_INPLACE_XFER	1
#endif

struct gspi_status {
	u32 word0;
	u32 word1;
	u8  valid;	/* 0 = trailer not driven by device, words are bus idle level */
};

static u32 gspi_swap32(u32 v)
{
	return ((v & 0x000000FFu) << 24) | ((v & 0x0000FF00u) << 8) |
		   ((v & 0x00FF0000u) >> 8) | ((v & 0xFF000000u) >> 24);
}

/* Build the 4-byte command word. See GSPI_Command_Word in whc_gspi_host_reg.h. */
static u32 gspi_build_cmd(struct whc_gspi *priv, u8 rw, u8 domain, u32 addr_or_len, u8 byte_en)
{
	u32 cmd = ((u32)rw << 31) | GSPI_CMD_FUN_BIT | ((u32)domain << GSPI_CMD_DOMAIN_SHIFT);

	if (domain == GSPI_CTRL_DOMAIN_ID) {
		cmd |= ((addr_or_len & GSPI_CMD_ADDR_MSK) << GSPI_CMD_ADDR_SHIFT) | (byte_en & GSPI_CMD_BYTE_EN_MSK);
	} else {
		cmd |= (addr_or_len & GSPI_CMD_LEN_MSK);
	}

	return priv->swap ? gspi_swap32(cmd) : cmd;
}

static void gspi_fill_status(struct gspi_status *sts, const u8 *b)
{
	if (sts == NULL) {
		return;
	}

	memcpy(&sts->word0, b, 4);
	memcpy(&sts->word1, b + 4, 4);
	sts->valid = (sts->word0 & GSPI_MASK_HISR_INVALID) ? 0 : 1;
}

/*
 * Exchange one whole GSPI frame in a single full-duplex transfer. buf holds the
 * staged CMD/data and any dummy bytes to clock out; on return it holds the MISO
 * result at the same offsets. buf must be DMA-safe (kmalloc'd) and CS stays
 * asserted for the whole transfer, which is exactly one frame.
 *
 * GSPI_INPLACE_XFER selects tx==rx (in place) or a separate rx buffer that is
 * copied back into buf; both leave the caller with an identical result in buf.
 */
static int gspi_frame_xfer(struct whc_gspi *priv, u8 *buf, u32 len)
{
	struct spi_transfer xfer;
	struct spi_message msg;

#if GSPI_INPLACE_XFER
	memset(&xfer, 0, sizeof(xfer));
	xfer.tx_buf = buf;
	xfer.rx_buf = buf;
	xfer.len = len;
	/* todo */
	xfer.speed_hz = 10000000;
	spi_message_init(&msg);
	spi_message_add_tail(&xfer, &msg);

	return spi_sync(priv->spi_dev, &msg);
#else
	u8 *rxbuf;
	int rc;

	rxbuf = kzalloc(len, GFP_KERNEL);
	if (rxbuf == NULL) {
		return -ENOMEM;
	}

	memset(&xfer, 0, sizeof(xfer));
	xfer.tx_buf = buf;
	xfer.rx_buf = rxbuf;
	xfer.len = len;

	spi_message_init(&msg);
	spi_message_add_tail(&xfer, &msg);

	rc = spi_sync(priv->spi_dev, &msg);
	if (rc == 0) {
		memcpy(buf, rxbuf, len);
	}

	kfree(rxbuf);
	return rc;
#endif
}

/* ------------------------------------------------------------ register access */

/*
 * Read the aligned 32-bit word at aligned_addr. The frame buffer is kmalloc'd
 * per call so it is both DMA-safe and private to this transaction -- concurrent
 * register access from the IRQ thread and the TX/RX paths therefore needs no
 * extra lock; spi_sync already serializes the bus.
 */
static u32 gspi_reg_read_word(struct whc_gspi *priv, u32 aligned_addr, struct gspi_status *sts)
{
	u8 *buf;	/* 16 bytes: [cmd 4][status 8][data 4] */
	u32 cmd = gspi_build_cmd(priv, 0, GSPI_CTRL_DOMAIN_ID, aligned_addr, 0);
	u32 rdata = 0;

	buf = kzalloc(GSPI_REG_FRAME_LEN, GFP_KERNEL);
	if (buf == NULL) {
		return 0;
	}
	memcpy(buf, &cmd, GSPI_CMD_PHASE_LEN);

	/* Send CMD 4B + dummy 12B (buf is kzalloc'd, dummy already 0);
	 * recv dummy 4B | Status 8B | Data 4B in place. */
	gspi_frame_xfer(priv, buf, GSPI_REG_FRAME_LEN);

	gspi_fill_status(sts, buf + 4);
	memcpy(&rdata, buf + 12, 4);
	kfree(buf);

	if (priv->swap) {
		rdata = gspi_swap32(rdata);
	}

	return rdata;
}

/* Write val (already positioned within the word) to the aligned word. */
static s32 gspi_reg_write_word(struct whc_gspi *priv, u32 aligned_addr, u32 val,
							   u8 byte_en, struct gspi_status *sts)
{
	u8 *buf;	/* 16 bytes: [cmd 4][data 4][status 8] */
	u32 cmd = gspi_build_cmd(priv, 1, GSPI_CTRL_DOMAIN_ID, aligned_addr, byte_en);
	int rc;

	if (priv->swap) {
		val = gspi_swap32(val);
	}

	buf = kzalloc(GSPI_REG_FRAME_LEN, GFP_KERNEL);
	if (buf == NULL) {
		return -ENOMEM;
	}
	memcpy(buf, &cmd, GSPI_CMD_PHASE_LEN);
	memcpy(buf + 4, &val, 4);

	/* Send CMD 4B + Data 4B + dummy 8B (dummy already 0); Status trails. */
	rc = gspi_frame_xfer(priv, buf, GSPI_REG_FRAME_LEN);

	gspi_fill_status(sts, buf + 8);
	kfree(buf);

	return rc ? rc : 0;
}

u32 gspi_read32(struct whc_gspi *priv, u32 addr)
{
	if (addr & 0x3) {
		dev_err(&priv->spi_dev->dev, "%s: addr 0x%x not 4-byte aligned\n", __func__, addr);
		return 0;
	}
	return gspi_reg_read_word(priv, addr, NULL);
}

u16 gspi_read16(struct whc_gspi *priv, u32 addr)
{
	u32 w = gspi_reg_read_word(priv, addr & ~0x3u, NULL);

	return (u16)((w >> ((addr & 0x3u) * 8)) & 0xFFFFu);
}

u8 gspi_read8(struct whc_gspi *priv, u32 addr)
{
	u32 w = gspi_reg_read_word(priv, addr & ~0x3u, NULL);

	return (u8)((w >> ((addr & 0x3u) * 8)) & 0xFFu);
}

s32 gspi_write32(struct whc_gspi *priv, u32 addr, u32 val)
{
	if (addr & 0x3) {
		dev_err(&priv->spi_dev->dev, "%s: addr 0x%x not 4-byte aligned\n", __func__, addr);
		return -EINVAL;
	}
	return gspi_reg_write_word(priv, addr, val, GSPI_CMD_BYTE_EN_MSK, NULL);
}

s32 gspi_write16(struct whc_gspi *priv, u32 addr, u16 val)
{
	u32 shift = (addr & 0x3u) * 8;
	u8 byte_en = (u8)(0x3u << (addr & 0x3u));

	return gspi_reg_write_word(priv, addr & ~0x3u, ((u32)val & 0xFFFFu) << shift, byte_en, NULL);
}

s32 gspi_write8(struct whc_gspi *priv, u32 addr, u8 val)
{
	u32 shift = (addr & 0x3u) * 8;
	u8 byte_en = (u8)(0x1u << (addr & 0x3u));

	return gspi_reg_write_word(priv, addr & ~0x3u, ((u32)val & 0xFFu) << shift, byte_en, NULL);
}

/* ---------------------------------------------------------------- FIFO access */

/*
 * Write one packet to the device TX FIFO. src holds [INIC_TX_DESC | payload].
 * Frame buffer layout: [CMD 4][data, 4-byte aligned][Status 8].
 */
u32 gspi_write_port(struct whc_gspi *priv, u8 *src, u32 cnt)
{
	u32 xlen = GSPI_ALIGN4(cnt);
	u32 framelen = GSPI_CMD_PHASE_LEN + xlen + GSPI_STATUS_PHASE_LEN;
	struct gspi_status sts;
	u32 cmd;
	u8 *fb;
	int rc;

	fb = kmalloc(framelen, GFP_KERNEL);
	if (fb == NULL) {
		return false;
	}

	cmd = gspi_build_cmd(priv, 1, GSPI_TX_FIFO_DOMAIN_ID, xlen, 0);
	memcpy(fb, &cmd, GSPI_CMD_PHASE_LEN);
	memcpy(fb + GSPI_CMD_PHASE_LEN, src, cnt);
	if (xlen > cnt) {
		memset(fb + GSPI_CMD_PHASE_LEN + cnt, 0, xlen - cnt);
	}
	/* zero the status dummy so it clocks out 0 during the trailing phase */
	memset(fb + GSPI_CMD_PHASE_LEN + xlen, 0, GSPI_STATUS_PHASE_LEN);

	/* Send CMD 4B + Data xlen + dummy 8B in one frame; Status overwrites the
	 * trailing 8B in place. */
	rc = gspi_frame_xfer(priv, fb, framelen);

	gspi_fill_status(&sts, fb + GSPI_CMD_PHASE_LEN + xlen);
	kfree(fb);

	if (rc) {
		dev_err(&priv->spi_dev->dev, "%s: spi_sync FAIL(%d) len=%d\n", __func__, rc, cnt);
		return false;
	}
	if (sts.valid && (sts.word0 & GSPI_MASK_HISR_TX_ERR)) {
		dev_err(&priv->spi_dev->dev, "%s: TX rejected, status 0x%08x len=%d\n",
				__func__, sts.word0, cnt);
		return false;
	}

	return true;
}

/*
 * Read one packet from the device RX FIFO into fb. fb must hold
 * GSPI_FIFO_FRAME_LEN(cnt) bytes; on return the payload is at fb + CMD phase.
 */
u32 gspi_read_port(struct whc_gspi *priv, u8 *fb, u32 cnt)
{
	u32 xlen = GSPI_ALIGN4(cnt);
	u32 framelen = GSPI_CMD_PHASE_LEN + xlen + GSPI_STATUS_PHASE_LEN;
	struct gspi_status sts;
	u32 cmd;
	int rc;

	cmd = gspi_build_cmd(priv, 0, GSPI_RX_FIFO_DOMAIN_ID, xlen, 0);
	memcpy(fb, &cmd, GSPI_CMD_PHASE_LEN);
	/* zero the data + status region so it clocks out 0 while MISO fills it in */
	memset(fb + GSPI_CMD_PHASE_LEN, 0, xlen + GSPI_STATUS_PHASE_LEN);

	/* Send CMD 4B + dummy (xlen + 8B) in one frame; MISO overwrites the data
	 * and Status region in place. */
	rc = gspi_frame_xfer(priv, fb, framelen);

	if (rc) {
		dev_err(&priv->spi_dev->dev, "%s: spi_sync FAIL(%d) len=%d\n", __func__, rc, cnt);
		return false;
	}

	gspi_fill_status(&sts, fb + GSPI_CMD_PHASE_LEN + xlen);
	(void)sts;

	return true;
}
