/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "rtw_gspi_drvio_v1.h"
#include "ameba_gspi_port.h"

static const char *const TAG = "GSPI";

/* Frame layout, command-word fields, domain IDs and the activation retry policy
 * are all in rtw_gspi_drvio_v1.h -- see the GSPI_Command_Word group there. */

/* ---------------------------------------------------------------- internals */

static u32 gspi_swap32(u32 v)
{
	return ((v & 0x000000FFu) << 24) | ((v & 0x0000FF00u) << 8) |
		   ((v & 0x00FF0000u) >> 8) | ((v & 0xFF000000u) >> 24);
}

static u32 gspi_build_cmd(struct whc_gspi_handle *hgspi, u8 rw, u8 domain, u32 addr_or_len, u8 byte_en)
{
	u32 cmd = ((u32)rw << 31) | GSPI_CMD_FUN_BIT | ((u32)domain << 24);

	if (domain == GSPI_DOMAIN_CTRL) {
		cmd |= ((addr_or_len & 0xFFFFu) << 8) | byte_en;
	} else {
		cmd |= (addr_or_len & 0xFFFFu);
	}

	return hgspi->swap ? gspi_swap32(cmd) : cmd;
}

static void gspi_cs(struct whc_gspi_handle *hgspi, u8 level)
{
	GPIO_WriteBit(hgspi->Init.GSPI_CsPin, level);
}

/* Polled full-duplex byte exchange, safe in place. */
static void gspi_xfer_poll(struct whc_gspi_handle *hgspi, u8 *buf, u32 len)
{
	SPI_TypeDef *spi_dev = hgspi->spi_dev;
	u32 i;

	for (i = 0; i < len; i++) {
		while (!SSI_Writeable(spi_dev));
		SSI_WriteData(spi_dev, buf[i]);
		while (!SSI_Readable(spi_dev));
		buf[i] = (u8)SSI_ReadData(spi_dev);
	}
}

static u32 gspi_dma_tx_done(void *data)
{
	struct whc_gspi_handle *hgspi = (struct whc_gspi_handle *)data;

	GDMA_ClearINT(hgspi->txgdma.GDMA_Index, hgspi->txgdma.GDMA_ChNum);
	GDMA_Cmd(hgspi->txgdma.GDMA_Index, hgspi->txgdma.GDMA_ChNum, DISABLE);
	SSI_SetDmaEnable(hgspi->spi_dev, DISABLE, SPI_BIT_TDMAE);
	GDMA_ChnlFree(hgspi->txgdma.GDMA_Index, hgspi->txgdma.GDMA_ChNum);
	hgspi->tx_dma_done = 1;

	return 0;
}

static u32 gspi_dma_rx_done(void *data)
{
	struct whc_gspi_handle *hgspi = (struct whc_gspi_handle *)data;

	GDMA_ClearINT(hgspi->rxgdma.GDMA_Index, hgspi->rxgdma.GDMA_ChNum);
	GDMA_Cmd(hgspi->rxgdma.GDMA_Index, hgspi->rxgdma.GDMA_ChNum, DISABLE);
	SSI_SetDmaEnable(hgspi->spi_dev, DISABLE, SPI_BIT_RDMAE);
	GDMA_ChnlFree(hgspi->rxgdma.GDMA_Index, hgspi->rxgdma.GDMA_ChNum);
	hgspi->rx_dma_done = 1;

	return 0;
}

static s32 gspi_wait_flag(struct whc_gspi_handle *hgspi, volatile u8 *flag)
{
	u32 waited = 0;

	while (*flag == 0) {
		if (waited >= hgspi->Init.GSPI_XferTimeoutUs) {
			return -RTK_ERR_TIMEOUT;
		}
		DelayUs(1);
		waited++;
	}

	return RTK_SUCCESS;
}

/* Drain whatever the RX FIFO collected while a TX-only DMA phase ran. */
static void gspi_flush_rx(struct whc_gspi_handle *hgspi)
{
	while (SSI_Readable(hgspi->spi_dev)) {
		(void)SSI_ReadData(hgspi->spi_dev);
	}
}

/* Data phase over GDMA. rx == NULL: transmit only, receive data discarded.
 * tx == NULL: receive only; the buffer doubles as the dummy TX source, because
 * MOSI content is ignored by the device during an RX data phase. */
static s32 gspi_xfer_dma(struct whc_gspi_handle *hgspi, u8 *tx, u8 *rx, u32 len)
{
	s32 ret;

	hgspi->tx_dma_done = 0;
	hgspi->rx_dma_done = 0;

	if (rx) {
		if (SSI_RXGDMA_Init(hgspi->Init.GSPI_Index, &hgspi->rxgdma, hgspi,
							(IRQ_FUN)gspi_dma_rx_done, rx, len) != TRUE) {
			return RTK_FAIL;
		}
		SSI_SetDmaEnable(hgspi->spi_dev, ENABLE, SPI_BIT_RDMAE);
	} else {
		hgspi->rx_dma_done = 1;
	}

	if (SSI_TXGDMA_Init(hgspi->Init.GSPI_Index, &hgspi->txgdma, hgspi,
						(IRQ_FUN)gspi_dma_tx_done, tx ? tx : rx, len) != TRUE) {
		return RTK_FAIL;
	}
	SSI_SetDmaEnable(hgspi->spi_dev, ENABLE, SPI_BIT_TDMAE);

	ret = gspi_wait_flag(hgspi, &hgspi->tx_dma_done);
	if (ret == RTK_SUCCESS) {
		ret = gspi_wait_flag(hgspi, &hgspi->rx_dma_done);
	}
	if (ret != RTK_SUCCESS) {
		RTK_LOGE(TAG, "dma timeout tx=%d rx=%d len=%d\n",
				 hgspi->tx_dma_done, hgspi->rx_dma_done, (int)len);
		return ret;
	}

	/* The TX DMA only guarantees the bytes reached the SSI FIFO. */
	for (u32 waited = 0; SSI_Busy(hgspi->spi_dev); waited++) {
		if (waited >= hgspi->Init.GSPI_XferTimeoutUs) {
			RTK_LOGE(TAG, "ssi busy timeout\n");
			return -RTK_ERR_TIMEOUT;
		}
		DelayUs(1);
	}

	if (rx) {
		DCache_Invalidate((u32)rx, len);
	} else {
		gspi_flush_rx(hgspi);
	}

	return RTK_SUCCESS;
}

static u8 gspi_use_dma(struct whc_gspi_handle *hgspi, u32 len)
{
	return (hgspi->Init.GSPI_DmaEn && len >= hgspi->Init.GSPI_DmaThreshold) ? 1 : 0;
}

/* Decode the 8-byte Status phase sitting at sts_buf. */
static void gspi_fill_status(struct whc_gspi_status *sts, const u8 *sts_buf)
{
	u32 w0, w1;

	if (sts == NULL) {
		return;
	}

	_memcpy(&w0, sts_buf, 4);
	_memcpy(&w1, sts_buf + 4, 4);
	sts->word0 = w0;
	sts->word1 = w1;
	sts->valid = (w0 & GSPI_MASK_HISR_INVALID) ? 0 : 1;
}

/* ------------------------------------------------------------ register access */

/* Register frame: CMD 4B, then for a read Status 8B + Data 4B, for a write
 * Data 4B + Status 8B. Always 16 bytes, always polled. */
static u32 gspi_reg_xfer(struct whc_gspi_handle *hgspi, u32 cmd, u32 wdata, u8 is_write,
						 struct whc_gspi_status *sts)
{
	u8 *buf = hgspi->regbuf;
	u32 rdata = 0;

	_memset(buf, 0, 16);
	_memcpy(buf, &cmd, GSPI_CMD_LEN);
	if (is_write) {
		if (hgspi->swap) {
			wdata = gspi_swap32(wdata);
		}
		_memcpy(buf + GSPI_CMD_LEN, &wdata, 4);
	}

	gspi_cs(hgspi, 0);
	gspi_xfer_poll(hgspi, buf, 16);
	gspi_cs(hgspi, 1);

	if (is_write) {
		gspi_fill_status(sts, buf + GSPI_CMD_LEN + 4);
	} else {
		gspi_fill_status(sts, buf + GSPI_CMD_LEN);
		_memcpy(&rdata, buf + GSPI_CMD_LEN + GSPI_STATUS_LEN, 4);
		if (hgspi->swap) {
			rdata = gspi_swap32(rdata);
		}
	}

	return rdata;
}

static u32 gspi_read_reg(struct whc_gspi_handle *hgspi, u32 addr, u8 width, struct whc_gspi_status *sts)
{
	u32 aligned = addr & ~0x3u;
	u32 shift = (addr & 0x3u) * 8;
	u32 cmd = gspi_build_cmd(hgspi, 0, GSPI_DOMAIN_CTRL, aligned, 0);
	u32 data = gspi_reg_xfer(hgspi, cmd, 0, 0, sts);

	switch (width) {
	case 1:
		return (data >> shift) & 0xFFu;
	case 2:
		return (data >> shift) & 0xFFFFu;
	default:
		return data;
	}
}

static s32 gspi_write_reg(struct whc_gspi_handle *hgspi, u32 addr, u32 val, u8 width,
						  struct whc_gspi_status *sts)
{
	u32 aligned = addr & ~0x3u;
	u32 shift = (addr & 0x3u) * 8;
	u32 byte_en;
	u32 cmd;

	switch (width) {
	case 1:
		byte_en = 0x1u << (addr & 0x3u);
		val = (val & 0xFFu) << shift;
		break;
	case 2:
		byte_en = 0x3u << (addr & 0x3u);
		val = (val & 0xFFFFu) << shift;
		break;
	default:
		byte_en = 0xFu;
		break;
	}

	cmd = gspi_build_cmd(hgspi, 1, GSPI_DOMAIN_CTRL, aligned, (u8)byte_en);
	gspi_reg_xfer(hgspi, cmd, val, 1, sts);

	return RTK_SUCCESS;
}

u8 GSPI_WIFI_ReadReg8(struct whc_gspi_handle *hgspi, u32 addr, struct whc_gspi_status *sts)
{
	return (u8)gspi_read_reg(hgspi, addr, 1, sts);
}

u16 GSPI_WIFI_ReadReg16(struct whc_gspi_handle *hgspi, u32 addr, struct whc_gspi_status *sts)
{
	return (u16)gspi_read_reg(hgspi, addr, 2, sts);
}

u32 GSPI_WIFI_ReadReg32(struct whc_gspi_handle *hgspi, u32 addr, struct whc_gspi_status *sts)
{
	return gspi_read_reg(hgspi, addr, 4, sts);
}

s32 GSPI_WIFI_WriteReg8(struct whc_gspi_handle *hgspi, u32 addr, u8 val, struct whc_gspi_status *sts)
{
	return gspi_write_reg(hgspi, addr, val, 1, sts);
}

s32 GSPI_WIFI_WriteReg16(struct whc_gspi_handle *hgspi, u32 addr, u16 val, struct whc_gspi_status *sts)
{
	return gspi_write_reg(hgspi, addr, val, 2, sts);
}

s32 GSPI_WIFI_WriteReg32(struct whc_gspi_handle *hgspi, u32 addr, u32 val, struct whc_gspi_status *sts)
{
	return gspi_write_reg(hgspi, addr, val, 4, sts);
}

/* ---------------------------------------------------------------- FIFO access */

/**
  * @brief Write one packet into the device TX FIFO.
  * @param buf Frame buffer. buf[0..3] is the Command slot filled by this
  *        function, buf[4..4+len) holds TX descriptor plus payload, and the
  *        buffer must provide GSPI_FIFO_BUF_SIZE(len) bytes in total.
  * @param len Data bytes (descriptor + payload). Padded to 4 bytes internally.
  * @return RTK_SUCCESS, -RTK_ERR_BUSY when the device has no free TX BD, or an error.
  */
s32 GSPI_WIFI_WriteTxFifo(struct whc_gspi_handle *hgspi, u8 *buf, u32 len, struct whc_gspi_status *sts)
{
	u32 xlen = GSPI_ALIGN4(len);
	u32 cmd;
	s32 ret = RTK_SUCCESS;

	if ((hgspi == NULL) || (buf == NULL) || (len == 0)) {
		return -RTK_ERR_BADARG;
	}
	if (GSPI_WIFI_GetFreeTxBD(hgspi, NULL) == 0) {
		return -RTK_ERR_BUSY;
	}

	if (xlen > len) {
		_memset(buf + GSPI_CMD_LEN + len, 0, xlen - len);
	}
	_memset(buf + GSPI_CMD_LEN + xlen, 0, GSPI_STATUS_LEN);
	cmd = gspi_build_cmd(hgspi, 1, GSPI_DOMAIN_TXFIFO, xlen, 0);
	_memcpy(buf, &cmd, GSPI_CMD_LEN);

	gspi_cs(hgspi, 0);
	gspi_xfer_poll(hgspi, buf, GSPI_CMD_LEN);
	if (gspi_use_dma(hgspi, xlen)) {
		ret = gspi_xfer_dma(hgspi, buf + GSPI_CMD_LEN, NULL, xlen);
	} else {
		gspi_xfer_poll(hgspi, buf + GSPI_CMD_LEN, xlen);
	}
	if (ret == RTK_SUCCESS) {
		gspi_xfer_poll(hgspi, buf + GSPI_CMD_LEN + xlen, GSPI_STATUS_LEN);
	}
	gspi_cs(hgspi, 1);

	if (ret != RTK_SUCCESS) {
		return ret;
	}

	gspi_fill_status(sts, buf + GSPI_CMD_LEN + xlen);
	if (sts && sts->valid && (sts->word0 & GSPI_MASK_TX_ERR)) {
		RTK_LOGE(TAG, "tx rejected, status 0x%08x len %d\n", sts->word0, (int)len);
		return RTK_FAIL;
	}

	return RTK_SUCCESS;
}

/**
  * @brief Read one packet out of the device RX FIFO.
  * @param buf Frame buffer, same layout as GSPI_WIFI_WriteTxFifo: the payload lands
  *        at buf[4], and the buffer must provide GSPI_FIFO_BUF_SIZE(len) bytes.
  * @param len Byte count to read, normally taken from GSPI_WIFI_GetRxLen().
  * @note Whether the payload starts with an RX descriptor is decided by the
  *       device application, so the data is returned as opaque bytes.
  */
s32 GSPI_WIFI_ReadRxFifo(struct whc_gspi_handle *hgspi, u8 *buf, u32 len, struct whc_gspi_status *sts)
{
	u32 xlen = GSPI_ALIGN4(len);
	u32 cmd;
	s32 ret = RTK_SUCCESS;

	if ((hgspi == NULL) || (buf == NULL) || (len == 0)) {
		return -RTK_ERR_BADARG;
	}

	_memset(buf + GSPI_CMD_LEN, 0, xlen + GSPI_STATUS_LEN);
	cmd = gspi_build_cmd(hgspi, 0, GSPI_DOMAIN_RXFIFO, xlen, 0);
	_memcpy(buf, &cmd, GSPI_CMD_LEN);

	gspi_cs(hgspi, 0);
	gspi_xfer_poll(hgspi, buf, GSPI_CMD_LEN);
	if (gspi_use_dma(hgspi, xlen)) {
		ret = gspi_xfer_dma(hgspi, NULL, buf + GSPI_CMD_LEN, xlen);
	} else {
		gspi_xfer_poll(hgspi, buf + GSPI_CMD_LEN, xlen);
	}
	if (ret == RTK_SUCCESS) {
		gspi_xfer_poll(hgspi, buf + GSPI_CMD_LEN + xlen, GSPI_STATUS_LEN);
	}
	gspi_cs(hgspi, 1);

	if (ret != RTK_SUCCESS) {
		return ret;
	}

	gspi_fill_status(sts, buf + GSPI_CMD_LEN + xlen);

	return RTK_SUCCESS;
}

/* --------------------------------------------------------------- convenience */

/**
  * @brief Return the pending RX byte count, or 0 when nothing is ready.
  * @note RDY can be observed set while the length field still reads 0, so both
  *       have to be valid before reporting data.
  */
u32 GSPI_WIFI_GetRxLen(struct whc_gspi_handle *hgspi, struct whc_gspi_status *sts)
{
	u32 rx0 = GSPI_WIFI_ReadReg32(hgspi, GSPI_REG_RX0_REQ_LEN, sts);

	if ((rx0 & GSPI_BIT_SYNC_RX_LEN_READY) == 0) {
		return 0;
	}

	return rx0 & GSPI_MASK_RX_LENGTH_LAT;
}

u32 GSPI_WIFI_GetFreeTxBD(struct whc_gspi_handle *hgspi, struct whc_gspi_status *sts)
{
	return GSPI_WIFI_ReadReg32(hgspi, GSPI_REG_FREE_TX_BD_NUM, sts) & 0xFFFFu;
}

/**
  * @brief Unmask or mask HISR bits. HIMR cannot be read back on this SoC, so the
  *        value is kept in the handle and rewritten in full each time.
  */
void GSPI_WIFI_INTConfig(struct whc_gspi_handle *hgspi, u32 mask, u32 newstate)
{
	if (newstate == ENABLE) {
		hgspi->himr_shadow |= mask;
	} else {
		hgspi->himr_shadow &= ~mask;
	}

	GSPI_WIFI_WriteReg32(hgspi, GSPI_REG_HIMR, hgspi->himr_shadow, NULL);
}

u32 GSPI_GetINTMask(struct whc_gspi_handle *hgspi)
{
	return hgspi->himr_shadow;
}

u32 GSPI_WIFI_INTStatus(struct whc_gspi_handle *hgspi, struct whc_gspi_status *sts)
{
	return GSPI_WIFI_ReadReg32(hgspi, GSPI_REG_HISR, sts);
}

void GSPI_WIFI_INTClear(struct whc_gspi_handle *hgspi, u32 bits)
{
	GSPI_WIFI_WriteReg32(hgspi, GSPI_REG_HISR, bits, NULL);
}

void GSPI_DmaCmd(struct whc_gspi_handle *hgspi, u32 newstate)
{
	hgspi->Init.GSPI_DmaEn = (newstate == ENABLE) ? 1 : 0;
}

u8 GSPI_DmaEnabled(struct whc_gspi_handle *hgspi)
{
	return hgspi->Init.GSPI_DmaEn;
}

/* ------------------------------------------------------------- init / config */

void GSPI_StructInit(struct whc_gspi_init *init)
{
	init->GSPI_Index = 0;
	init->GSPI_ClkPin = _PNC;
	init->GSPI_MosiPin = _PNC;
	init->GSPI_MisoPin = _PNC;
	init->GSPI_CsPin = _PNC;
	init->GSPI_ClkFreq = 10000000;
	init->GSPI_ClkFreqInit = 1000000;
	init->GSPI_DmaEn = 1;
	init->GSPI_DmaThreshold = 64;
	init->GSPI_XferTimeoutUs = 100000;
}

void GSPI_SetFreq(struct whc_gspi_handle *hgspi, u32 freq_hz)
{
	u32 ipclk = HPERI_ClkGet();
	u32 div;

	if (freq_hz == 0) {
		return;
	}

	div = (ipclk / freq_hz / 2) * 2;
	if (div == 0) {
		div = 2;
	}
	if ((ipclk / div) > freq_hz) {
		div += 2;
	}
	if (div >= 0xFFFF) {
		div = 0xFFFE;
	}

	SSI_SetBaudDiv(hgspi->spi_dev, div);
}

/**
  * @brief Configure SPI master, CS GPIO and pinmux. The GSPI protocol itself is
  *        not touched here; call GSPI_WIFI_Configuration() next to activate it.
  * @note CS is latched high before its output is enabled: a low or floating CS
  *       during boot looks like an asserted chip select to the device, which
  *       would make it fall back to SDIO mode.
  */
s32 GSPI_Init(struct whc_gspi_handle *hgspi, struct whc_gspi_init *init)
{
	SSI_InitTypeDef ssi_init;
	GPIO_InitTypeDef gpio_init;
	u8 clk_func, mosi_func, miso_func;

	if ((hgspi == NULL) || (init == NULL)) {
		return -RTK_ERR_BADARG;
	}

	_memset(hgspi, 0, sizeof(struct whc_gspi_handle));
	hgspi->Init = *init;
	hgspi->swap = 1;   /* the device boots in little-endian mode */

	if (init->GSPI_Index == 0) {
		hgspi->spi_dev = GSPI_SPI0_DEV;
		clk_func = GSPI_SPI0_PINMUX_CLK;
		mosi_func = GSPI_SPI0_PINMUX_MOSI;
		miso_func = GSPI_SPI0_PINMUX_MISO;
		RCC_PeriphClockCmd(GSPI_SPI0_APB, GSPI_SPI0_APB_CLOCK, ENABLE);
	} else {
		hgspi->spi_dev = GSPI_SPI1_DEV;
		clk_func = GSPI_SPI1_PINMUX_CLK;
		mosi_func = GSPI_SPI1_PINMUX_MOSI;
		miso_func = GSPI_SPI1_PINMUX_MISO;
		RCC_PeriphClockCmd(GSPI_SPI1_APB, GSPI_SPI1_APB_CLOCK, ENABLE);
	}

	/* CS first, and the level before the direction: the pad must never actively
	 * drive low, because the device would take it for a chip select and fall
	 * back to SDIO mode. */
	Pinmux_Config(init->GSPI_CsPin, PINMUX_FUNCTION_GPIO);
	gpio_init.GPIO_Pin = init->GSPI_CsPin;
	gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gpio_init.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(&gpio_init);
	GPIO_WriteBit(init->GSPI_CsPin, 1);
	gpio_init.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(&gpio_init);

	/* Configure the controller BEFORE routing its signals to the pads. The
	 * SSI's idle SCLK level depends on the polarity setting, so programming it
	 * with SCLK already connected puts an edge on the wire, which the device
	 * may take for bus activity and answer by latching SDIO mode. */
	SSI_StructInit(&ssi_init);
	ssi_init.SPI_Role = SSI_MASTER;
	ssi_init.SPI_DataFrameSize = DFS_8_BITS;
	ssi_init.SPI_SclkPolarity = SCPOL_INACTIVE_IS_LOW;   /* SPI mode 0 */
	ssi_init.SPI_SclkPhase = SCPH_TOGGLES_IN_MIDDLE;
	ssi_init.SPI_SlaveSelectEnable = 1;                  /* SS0, pad stays GPIO */
	SSI_SetRole(hgspi->spi_dev, SSI_MASTER);
	SSI_Init(hgspi->spi_dev, &ssi_init);
	GSPI_SetFreq(hgspi, init->GSPI_ClkFreqInit);

	/* Per-signal extended function IDs: the generic SPI0/SPI1 IDs only address
	 * the SoC's one fixed pin group and are invalid on other pads. */
	Pinmux_Config(init->GSPI_ClkPin, clk_func);
	Pinmux_Config(init->GSPI_MosiPin, mosi_func);
	Pinmux_Config(init->GSPI_MisoPin, miso_func);

	hgspi->initialized = 1;

	return RTK_SUCCESS;
}

void GSPI_DeInit(struct whc_gspi_handle *hgspi)
{
	if ((hgspi == NULL) || (hgspi->initialized == 0)) {
		return;
	}

	SSI_Cmd(hgspi->spi_dev, DISABLE);
	if (hgspi->Init.GSPI_Index == 0) {
		RCC_PeriphClockCmd(GSPI_SPI0_APB, GSPI_SPI0_APB_CLOCK, DISABLE);
	} else {
		RCC_PeriphClockCmd(GSPI_SPI1_APB, GSPI_SPI1_APB_CLOCK, DISABLE);
	}
	hgspi->initialized = 0;
	hgspi->activated = 0;
}

/**
  * @brief Activate GSPI mode by writing SPI_CFG, then switch to the working bus
  *        clock. This must be the very first bus transaction after the device
  *        boots, otherwise the device latches SDIO mode.
  * @param spi_cfg GSPI_BIG_ENDIAN_32 or GSPI_LITTLE_ENDIAN_32.
  * @note The activation frame is always encoded for the device's reset state
  *       (little endian), whatever the target setting is.
  */
s32 GSPI_WIFI_Configuration(struct whc_gspi_handle *hgspi, u8 spi_cfg)
{
	u8 readback;
	u32 attempt;

	if ((hgspi == NULL) || (hgspi->initialized == 0)) {
		return RTK_FAIL;
	}
	if ((spi_cfg != GSPI_BIG_ENDIAN_32) && (spi_cfg != GSPI_LITTLE_ENDIAN_32)) {
		return -RTK_ERR_BADARG;
	}

	/* Retried, because a frame sent while the device is still running its own
	 * SPDIO init is simply not seen: in that case the next attempt is the first
	 * transaction the device observes and activation succeeds. */
	for (attempt = 1; attempt <= GSPI_ACTIVATE_RETRY; attempt++) {
		hgspi->swap = 1;
		GSPI_WIFI_WriteReg8(hgspi, GSPI_REG_SPI_CFG, spi_cfg, NULL);

		/* From here on the device uses the new setting: a little-endian host
		 * needs no swap for BIG_ENDIAN_32, a full 4-byte swap otherwise. */
		hgspi->swap = (spi_cfg == GSPI_BIG_ENDIAN_32) ? 0 : 1;

		readback = GSPI_WIFI_ReadReg8(hgspi, GSPI_REG_SPI_CFG, NULL);
		if (readback == spi_cfg) {
			GSPI_SetFreq(hgspi, hgspi->Init.GSPI_ClkFreq);
			hgspi->activated = 1;
			if (attempt > 1) {
				RTK_LOGW(TAG, "activated on attempt %d\n", (int)attempt);
			}
			return RTK_SUCCESS;
		}

		RTK_LOGW(TAG, "activation attempt %d: SPI_CFG reads 0x%02x\n", (int)attempt, readback);
		hgspi->swap = 1;
		DelayMs(GSPI_ACTIVATE_RETRY_MS);
	}

	RTK_LOGE(TAG, "activation failed after %d attempts\n", GSPI_ACTIVATE_RETRY);

	return RTK_FAIL;
}
