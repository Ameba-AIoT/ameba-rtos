/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "ameba_gspi.h"

static const char *const TAG = "GSPI";

/* One GSPI frame lives inside a single CS-low window and has three phases:
 *   register read : CMD 4B -> Status 8B -> Data 4B
 *   everything else: CMD 4B -> Data NB -> Status 8B
 * The data phase length must be a multiple of 4; an unaligned length still
 * delivers the packet but leaves the Status phase undriven.
 *
 * Command word: R/W bit31, Fun bit29 (mandatory, undocumented), domain
 * bits[28:24], address bits[23:8], byte enables bits[3:0]. For the FIFO domains
 * the low 16 bits carry the transfer length instead of an address. */
#define GSPI_CMD_FUN_BIT        ((u32)0x00000001 << 29)
#define GSPI_DOMAIN_CTRL        0x00
#define GSPI_DOMAIN_TXFIFO      0x0C
#define GSPI_DOMAIN_RXFIFO      0x1F

#define GSPI_ALIGN4(x)          (((x) + 3) & ~3u)

/* Activation retries, see GSPI_Configuration(). */
#define GSPI_ACTIVATE_RETRY     3
#define GSPI_ACTIVATE_RETRY_MS  50

/* ---------------------------------------------------------------- internals */

static u32 gspi_swap32(u32 v)
{
	return ((v & 0x000000FFu) << 24) | ((v & 0x0000FF00u) << 8) |
		   ((v & 0x00FF0000u) >> 8) | ((v & 0xFF000000u) >> 24);
}

static u32 gspi_build_cmd(GSPI_HandleTypeDef *hgspi, u8 rw, u8 domain, u32 addr_or_len, u8 byte_en)
{
	u32 cmd = ((u32)rw << 31) | GSPI_CMD_FUN_BIT | ((u32)domain << 24);

	if (domain == GSPI_DOMAIN_CTRL) {
		cmd |= ((addr_or_len & 0xFFFFu) << 8) | byte_en;
	} else {
		cmd |= (addr_or_len & 0xFFFFu);
	}

	return hgspi->swap ? gspi_swap32(cmd) : cmd;
}

static void gspi_cs(GSPI_HandleTypeDef *hgspi, u8 level)
{
	GPIO_WriteBit(hgspi->Init.GSPI_CsPin, level);
}

/* Polled full-duplex byte exchange, safe in place. */
static void gspi_xfer_poll(GSPI_HandleTypeDef *hgspi, u8 *buf, u32 len)
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
	GSPI_HandleTypeDef *hgspi = (GSPI_HandleTypeDef *)data;

	GDMA_ClearINT(hgspi->txgdma.GDMA_Index, hgspi->txgdma.GDMA_ChNum);
	GDMA_Cmd(hgspi->txgdma.GDMA_Index, hgspi->txgdma.GDMA_ChNum, DISABLE);
	SSI_SetDmaEnable(hgspi->spi_dev, DISABLE, SPI_BIT_TDMAE);
	GDMA_ChnlFree(hgspi->txgdma.GDMA_Index, hgspi->txgdma.GDMA_ChNum);
	hgspi->tx_dma_done = 1;

	return 0;
}

static u32 gspi_dma_rx_done(void *data)
{
	GSPI_HandleTypeDef *hgspi = (GSPI_HandleTypeDef *)data;

	GDMA_ClearINT(hgspi->rxgdma.GDMA_Index, hgspi->rxgdma.GDMA_ChNum);
	GDMA_Cmd(hgspi->rxgdma.GDMA_Index, hgspi->rxgdma.GDMA_ChNum, DISABLE);
	SSI_SetDmaEnable(hgspi->spi_dev, DISABLE, SPI_BIT_RDMAE);
	GDMA_ChnlFree(hgspi->rxgdma.GDMA_Index, hgspi->rxgdma.GDMA_ChNum);
	hgspi->rx_dma_done = 1;

	return 0;
}

static s32 gspi_wait_flag(GSPI_HandleTypeDef *hgspi, volatile u8 *flag)
{
	u32 waited = 0;

	while (*flag == 0) {
		if (waited >= hgspi->Init.GSPI_XferTimeoutUs) {
			return GSPI_ERR_TIMEOUT;
		}
		DelayUs(1);
		waited++;
	}

	return GSPI_OK;
}

/* Drain whatever the RX FIFO collected while a TX-only DMA phase ran. */
static void gspi_flush_rx(GSPI_HandleTypeDef *hgspi)
{
	while (SSI_Readable(hgspi->spi_dev)) {
		(void)SSI_ReadData(hgspi->spi_dev);
	}
}

/* Data phase over GDMA. rx == NULL: transmit only, receive data discarded.
 * tx == NULL: receive only; the buffer doubles as the dummy TX source, because
 * MOSI content is ignored by the device during an RX data phase. */
static s32 gspi_xfer_dma(GSPI_HandleTypeDef *hgspi, u8 *tx, u8 *rx, u32 len)
{
	s32 ret;

	hgspi->tx_dma_done = 0;
	hgspi->rx_dma_done = 0;

	if (rx) {
		if (SSI_RXGDMA_Init(hgspi->Init.GSPI_Index, &hgspi->rxgdma, hgspi,
							(IRQ_FUN)gspi_dma_rx_done, rx, len) != TRUE) {
			return GSPI_ERR_STATE;
		}
		SSI_SetDmaEnable(hgspi->spi_dev, ENABLE, SPI_BIT_RDMAE);
	} else {
		hgspi->rx_dma_done = 1;
	}

	if (SSI_TXGDMA_Init(hgspi->Init.GSPI_Index, &hgspi->txgdma, hgspi,
						(IRQ_FUN)gspi_dma_tx_done, tx ? tx : rx, len) != TRUE) {
		return GSPI_ERR_STATE;
	}
	SSI_SetDmaEnable(hgspi->spi_dev, ENABLE, SPI_BIT_TDMAE);

	ret = gspi_wait_flag(hgspi, &hgspi->tx_dma_done);
	if (ret == GSPI_OK) {
		ret = gspi_wait_flag(hgspi, &hgspi->rx_dma_done);
	}
	if (ret != GSPI_OK) {
		RTK_LOGE(TAG, "dma timeout tx=%d rx=%d len=%d\n",
				 hgspi->tx_dma_done, hgspi->rx_dma_done, (int)len);
		return ret;
	}

	/* The TX DMA only guarantees the bytes reached the SSI FIFO. */
	while (SSI_Busy(hgspi->spi_dev));

	if (rx) {
		DCache_Invalidate((u32)rx, len);
	} else {
		gspi_flush_rx(hgspi);
	}

	return GSPI_OK;
}

static u8 gspi_use_dma(GSPI_HandleTypeDef *hgspi, u32 len)
{
	return (hgspi->Init.GSPI_DmaEn && len >= hgspi->Init.GSPI_DmaThreshold) ? 1 : 0;
}

/* Decode the 8-byte Status phase sitting at sts_buf. */
static void gspi_fill_status(GSPI_StatusTypeDef *sts, const u8 *sts_buf)
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
static u32 gspi_reg_xfer(GSPI_HandleTypeDef *hgspi, u32 cmd, u32 wdata, u8 is_write,
						 GSPI_StatusTypeDef *sts)
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

static u32 gspi_read_reg(GSPI_HandleTypeDef *hgspi, u32 addr, u8 width, GSPI_StatusTypeDef *sts)
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

static s32 gspi_write_reg(GSPI_HandleTypeDef *hgspi, u32 addr, u32 val, u8 width,
						  GSPI_StatusTypeDef *sts)
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

	return GSPI_OK;
}

u8 GSPI_ReadReg8(GSPI_HandleTypeDef *hgspi, u32 addr, GSPI_StatusTypeDef *sts)
{
	return (u8)gspi_read_reg(hgspi, addr, 1, sts);
}

u16 GSPI_ReadReg16(GSPI_HandleTypeDef *hgspi, u32 addr, GSPI_StatusTypeDef *sts)
{
	return (u16)gspi_read_reg(hgspi, addr, 2, sts);
}

u32 GSPI_ReadReg32(GSPI_HandleTypeDef *hgspi, u32 addr, GSPI_StatusTypeDef *sts)
{
	return gspi_read_reg(hgspi, addr, 4, sts);
}

s32 GSPI_WriteReg8(GSPI_HandleTypeDef *hgspi, u32 addr, u8 val, GSPI_StatusTypeDef *sts)
{
	return gspi_write_reg(hgspi, addr, val, 1, sts);
}

s32 GSPI_WriteReg16(GSPI_HandleTypeDef *hgspi, u32 addr, u16 val, GSPI_StatusTypeDef *sts)
{
	return gspi_write_reg(hgspi, addr, val, 2, sts);
}

s32 GSPI_WriteReg32(GSPI_HandleTypeDef *hgspi, u32 addr, u32 val, GSPI_StatusTypeDef *sts)
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
  * @return GSPI_OK, GSPI_NO_TXBD when the device has no free TX BD, or an error.
  */
s32 GSPI_WriteTxFifo(GSPI_HandleTypeDef *hgspi, u8 *buf, u32 len, GSPI_StatusTypeDef *sts)
{
	u32 xlen = GSPI_ALIGN4(len);
	u32 cmd;
	s32 ret = GSPI_OK;

	if ((hgspi == NULL) || (buf == NULL) || (len == 0)) {
		return GSPI_ERR_PARAM;
	}
	if (GSPI_GetFreeTxBD(hgspi, NULL) == 0) {
		return GSPI_NO_TXBD;
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
	if (ret == GSPI_OK) {
		gspi_xfer_poll(hgspi, buf + GSPI_CMD_LEN + xlen, GSPI_STATUS_LEN);
	}
	gspi_cs(hgspi, 1);

	if (ret != GSPI_OK) {
		return ret;
	}

	gspi_fill_status(sts, buf + GSPI_CMD_LEN + xlen);
	if (sts && sts->valid && (sts->word0 & GSPI_MASK_TX_ERR)) {
		RTK_LOGE(TAG, "tx rejected, status 0x%08x len %d\n", sts->word0, (int)len);
		return GSPI_ERR_STATUS;
	}

	return GSPI_OK;
}

/**
  * @brief Read one packet out of the device RX FIFO.
  * @param buf Frame buffer, same layout as GSPI_WriteTxFifo: the payload lands
  *        at buf[4], and the buffer must provide GSPI_FIFO_BUF_SIZE(len) bytes.
  * @param len Byte count to read, normally taken from GSPI_GetRxLen().
  * @note Whether the payload starts with an RX descriptor is decided by the
  *       device application, so the data is returned as opaque bytes.
  */
s32 GSPI_ReadRxFifo(GSPI_HandleTypeDef *hgspi, u8 *buf, u32 len, GSPI_StatusTypeDef *sts)
{
	u32 xlen = GSPI_ALIGN4(len);
	u32 cmd;
	s32 ret = GSPI_OK;

	if ((hgspi == NULL) || (buf == NULL) || (len == 0)) {
		return GSPI_ERR_PARAM;
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
	if (ret == GSPI_OK) {
		gspi_xfer_poll(hgspi, buf + GSPI_CMD_LEN + xlen, GSPI_STATUS_LEN);
	}
	gspi_cs(hgspi, 1);

	if (ret != GSPI_OK) {
		return ret;
	}

	gspi_fill_status(sts, buf + GSPI_CMD_LEN + xlen);

	return GSPI_OK;
}

/* --------------------------------------------------------------- convenience */

/**
  * @brief Return the pending RX byte count, or 0 when nothing is ready.
  * @note RDY can be observed set while the length field still reads 0, so both
  *       have to be valid before reporting data.
  */
u32 GSPI_GetRxLen(GSPI_HandleTypeDef *hgspi, GSPI_StatusTypeDef *sts)
{
	u32 rx0 = GSPI_ReadReg32(hgspi, GSPI_REG_RX0_REQ_LEN, sts);

	if ((rx0 & GSPI_BIT_RX0_RDY) == 0) {
		return 0;
	}

	return rx0 & GSPI_MASK_RX0_LEN;
}

u32 GSPI_GetFreeTxBD(GSPI_HandleTypeDef *hgspi, GSPI_StatusTypeDef *sts)
{
	return GSPI_ReadReg32(hgspi, GSPI_REG_FREE_TX_BD_NUM, sts) & 0xFFFFu;
}

/**
  * @brief Unmask or mask HISR bits. HIMR cannot be read back on this SoC, so the
  *        value is kept in the handle and rewritten in full each time.
  */
void GSPI_INTConfig(GSPI_HandleTypeDef *hgspi, u32 mask, u32 newstate)
{
	if (newstate == ENABLE) {
		hgspi->himr_shadow |= mask;
	} else {
		hgspi->himr_shadow &= ~mask;
	}

	GSPI_WriteReg32(hgspi, GSPI_REG_HIMR, hgspi->himr_shadow, NULL);
}

u32 GSPI_GetINTMask(GSPI_HandleTypeDef *hgspi)
{
	return hgspi->himr_shadow;
}

u32 GSPI_INTStatus(GSPI_HandleTypeDef *hgspi, GSPI_StatusTypeDef *sts)
{
	return GSPI_ReadReg32(hgspi, GSPI_REG_HISR, sts);
}

void GSPI_INTClear(GSPI_HandleTypeDef *hgspi, u32 bits)
{
	GSPI_WriteReg32(hgspi, GSPI_REG_HISR, bits, NULL);
}

void GSPI_DmaCmd(GSPI_HandleTypeDef *hgspi, u32 newstate)
{
	hgspi->Init.GSPI_DmaEn = (newstate == ENABLE) ? 1 : 0;
}

u8 GSPI_DmaEnabled(GSPI_HandleTypeDef *hgspi)
{
	return hgspi->Init.GSPI_DmaEn;
}

/* ------------------------------------------------------------- init / config */

void GSPI_StructInit(GSPI_InitTypeDef *init)
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

void GSPI_SetFreq(GSPI_HandleTypeDef *hgspi, u32 freq_hz)
{
	u32 ipclk = HPERI_ClkGet();
	u32 div = (ipclk / freq_hz / 2) * 2;

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
  *        not touched here; call GSPI_Configuration() next to activate it.
  * @note CS is latched high before its output is enabled: a low or floating CS
  *       during boot looks like an asserted chip select to the device, which
  *       would make it fall back to SDIO mode.
  */
s32 GSPI_Init(GSPI_HandleTypeDef *hgspi, GSPI_InitTypeDef *init)
{
	SSI_InitTypeDef ssi_init;
	GPIO_InitTypeDef gpio_init;
	u8 clk_func, mosi_func, miso_func;

	if ((hgspi == NULL) || (init == NULL)) {
		return GSPI_ERR_PARAM;
	}

	_memset(hgspi, 0, sizeof(GSPI_HandleTypeDef));
	hgspi->Init = *init;
	hgspi->swap = 1;   /* the device boots in little-endian mode */

	if (init->GSPI_Index == 0) {
		hgspi->spi_dev = SPI0_DEV;
		clk_func = PINMUX_FUNCTION_SPI0_CLK;
		mosi_func = PINMUX_FUNCTION_SPI0_MOSI;
		miso_func = PINMUX_FUNCTION_SPI0_MISO;
		RCC_PeriphClockCmd(APBPeriph_SPI0, APBPeriph_SPI0_CLOCK, ENABLE);
	} else {
		hgspi->spi_dev = SPI1_DEV;
		clk_func = PINMUX_FUNCTION_SPI1_CLK;
		mosi_func = PINMUX_FUNCTION_SPI1_MOSI;
		miso_func = PINMUX_FUNCTION_SPI1_MISO;
		RCC_PeriphClockCmd(APBPeriph_SPI1, APBPeriph_SPI1_CLOCK, ENABLE);
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

	return GSPI_OK;
}

void GSPI_DeInit(GSPI_HandleTypeDef *hgspi)
{
	if ((hgspi == NULL) || (hgspi->initialized == 0)) {
		return;
	}

	SSI_Cmd(hgspi->spi_dev, DISABLE);
	if (hgspi->Init.GSPI_Index == 0) {
		RCC_PeriphClockCmd(APBPeriph_SPI0, APBPeriph_SPI0_CLOCK, DISABLE);
	} else {
		RCC_PeriphClockCmd(APBPeriph_SPI1, APBPeriph_SPI1_CLOCK, DISABLE);
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
s32 GSPI_Configuration(GSPI_HandleTypeDef *hgspi, u8 spi_cfg)
{
	u8 readback;
	u32 attempt;

	if ((hgspi == NULL) || (hgspi->initialized == 0)) {
		return GSPI_ERR_STATE;
	}
	if ((spi_cfg != GSPI_BIG_ENDIAN_32) && (spi_cfg != GSPI_LITTLE_ENDIAN_32)) {
		return GSPI_ERR_PARAM;
	}

	/* Retried, because a frame sent while the device is still running its own
	 * SPDIO init is simply not seen: in that case the next attempt is the first
	 * transaction the device observes and activation succeeds. */
	for (attempt = 1; attempt <= GSPI_ACTIVATE_RETRY; attempt++) {
		hgspi->swap = 1;
		GSPI_WriteReg8(hgspi, GSPI_REG_SPI_CFG, spi_cfg, NULL);

		/* From here on the device uses the new setting: a little-endian host
		 * needs no swap for BIG_ENDIAN_32, a full 4-byte swap otherwise. */
		hgspi->swap = (spi_cfg == GSPI_BIG_ENDIAN_32) ? 0 : 1;

		readback = GSPI_ReadReg8(hgspi, GSPI_REG_SPI_CFG, NULL);
		if (readback == spi_cfg) {
			GSPI_SetFreq(hgspi, hgspi->Init.GSPI_ClkFreq);
			hgspi->activated = 1;
			if (attempt > 1) {
				RTK_LOGW(TAG, "activated on attempt %d\n", (int)attempt);
			}
			return GSPI_OK;
		}

		RTK_LOGW(TAG, "activation attempt %d: SPI_CFG reads 0x%02x\n", (int)attempt, readback);
		hgspi->swap = 1;
		DelayMs(GSPI_ACTIVATE_RETRY_MS);
	}

	RTK_LOGE(TAG, "activation failed after %d attempts\n", GSPI_ACTIVATE_RETRY);

	return GSPI_ERR_STATUS;
}
