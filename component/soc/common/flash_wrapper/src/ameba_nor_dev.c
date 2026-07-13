/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "ameba_flash_dev.h"
#include "ameba_nor_ftl.h"

/* Private defines -----------------------------------------------------------*/

#define FLASH_CMD_BLOCK_ERASE_32K		0x52U
#define FLASH_CMD_BLOCK_ERASE_64K		0xD8U

#define NOR_SECTOR_SIZE				0x1000U		/* 4 KB  */
#define NOR_BLOCK_SIZE_32K			0x8000U		/* 32 KB */
#define NOR_BLOCK_SIZE_64K			0x10000U	/* 64 KB */
#define NOR_PROG_PAGE_SIZE			0x100U		/* 256 B program page */

/*
 * Leaf-primitive selection by environment:
 *
 *   CONFIG_NON_OS (floader / bare): ameba_flash_ram.c is NOT linked and the
 *   downloader runs single-core with XIP off, so the ROM primitives
 *   (FLASH_Erase/TxData/RxCmd/SetStatus) are used directly. Cache coherency is
 *   handled here with explicit DCache_Invalidate.
 *
 *   OS firmware (e.g. OTA): runs multi-core under XIP, so erase/program/status
 *   must go through the FLASH_*XIP / FLASH_*Stream helpers, which take the
 *   multi-core flash lock (FLASH_Write_Lock: irq off + gate other core + IPC
 *   sync + MMU flash->device) internally. This mirrors the legacy OTA path.
 */
#if defined(CONFIG_NON_OS)
#define NOR_ERASE_UNIT(type, addr)	FLASH_Erase((type), (addr))
#define NOR_RD_STATUS(cmd, val)		FLASH_RxCmd((cmd), 1, (val))
#define NOR_WR_STATUS(cmd, val)		FLASH_SetStatus((cmd), 1, (val))
#else
#define NOR_ERASE_UNIT(type, addr)	FLASH_EraseXIP((type), (addr))
#define NOR_RD_STATUS(cmd, val)		FLASH_RxCmdXIP((cmd), 1, (val))
#define NOR_WR_STATUS(cmd, val)		FLASH_SetStatusXIP((cmd), 1, (val))
#endif

/* Private functions ---------------------------------------------------------*/

static u8 nor_dev_read(struct flash_dev *dev, u32 offset, u8 *buf, u32 len)
{
	UNUSED(dev);
	/* NOR is XIP-mapped; reading from the memory-mapped region is safe in
	 * both environments (reads never conflict with the SPIC). */
	_memcpy(buf, (void *)(SPI_FLASH_BASE + offset), len);
	return ERR_NONE;
}

static u8 nor_dev_write(struct flash_dev *dev, u32 offset, const u8 *buf, u32 len)
{
	UNUSED(dev);

#if defined(CONFIG_NON_OS)
	/* Program in <=256B chunks; a program command must not cross a page. */
	u32 done = 0;
	u32 chunk;
	u32 space;

	while (done < len) {
		space = NOR_PROG_PAGE_SIZE - ((offset + done) & (NOR_PROG_PAGE_SIZE - 1U));
		chunk = len - done;
		if (chunk > space) {
			chunk = space;
		}
		FLASH_TxData(offset + done, chunk, (u8 *)(buf + done));
		done += chunk;
	}
	DCache_Invalidate(SPI_FLASH_BASE + offset, len);
	return ERR_NONE;
#else
	/* XIP-safe stream write: handles paging + multi-core flash lock. */
	return (FLASH_WriteStream(offset, len, (u8 *)buf) == 1) ? ERR_NONE : ERR_FAIL;
#endif
}

static u8 nor_dev_erase(struct flash_dev *dev, u32 offset, u32 len)
{
	u32 addr;
	u32 end;
#if defined(CONFIG_NON_OS)
	u32 erased_start;
#endif

	UNUSED(dev);

	/*
	 * Whole-chip erase sentinel: offset 0 with len 0xFFFFFFFF (matches the
	 * floader full-erase request). len 0 is NOT chip erase: it falls through
	 * to the loop below and erases nothing.
	 */
	if ((offset == 0U) && (len == 0xFFFFFFFFU)) {
		NOR_ERASE_UNIT(EraseChip, 0);
#if defined(CONFIG_NON_OS)
		DCache_Invalidate(SPI_FLASH_BASE, 0x10000000U - SPI_FLASH_BASE);
#endif
		return ERR_NONE;
	}

	addr = offset & ~(NOR_SECTOR_SIZE - 1U);	/* round down to 4 KB */
	end = offset + len;
#if defined(CONFIG_NON_OS)
	erased_start = addr;
#endif

	while (addr < end) {
		if (((addr & (NOR_BLOCK_SIZE_64K - 1U)) == 0U) && ((end - addr) >= NOR_BLOCK_SIZE_64K)) {
			flash_init_para.FLASH_cmd_block_e = FLASH_CMD_BLOCK_ERASE_64K;
			NOR_ERASE_UNIT(EraseBlock, addr);
			addr += NOR_BLOCK_SIZE_64K;
		} else if (((addr & (NOR_BLOCK_SIZE_32K - 1U)) == 0U) && ((end - addr) >= NOR_BLOCK_SIZE_32K)) {
			flash_init_para.FLASH_cmd_block_e = FLASH_CMD_BLOCK_ERASE_32K;
			NOR_ERASE_UNIT(EraseBlock, addr);
			addr += NOR_BLOCK_SIZE_32K;
		} else {
			NOR_ERASE_UNIT(EraseSector, addr);
			addr += NOR_SECTOR_SIZE;
		}
	}

#if defined(CONFIG_NON_OS)
	/* Firmware path invalidates per unit inside FLASH_EraseXIP; floader must
	 * invalidate the whole erased range itself. */
	DCache_Invalidate(SPI_FLASH_BASE + erased_start, addr - erased_start);
#endif

	return ERR_NONE;
}

static u8 nor_dev_is_bad(struct flash_dev *dev, u32 offset, u8 *bad)
{
	UNUSED(dev);
	UNUSED(offset);
	if (bad == NULL) {
		return ERR_INVALID;
	}
	*bad = 0U;	/* NOR has no bad-block concept */
	return ERR_NONE;
}

static u8 nor_dev_mark_bad(struct flash_dev *dev, u32 offset)
{
	UNUSED(dev);
	UNUSED(offset);
	return ERR_INVALID;	/* not supported on NOR */
}

static u8 nor_dev_get_status(struct flash_dev *dev, u8 cmd, u8 addr, u8 *value)
{
	UNUSED(dev);
	UNUSED(addr);
	if (value == NULL) {
		return ERR_INVALID;
	}
	NOR_RD_STATUS(cmd, value);
	return ERR_NONE;
}

static u8 nor_dev_set_status(struct flash_dev *dev, u8 cmd, u8 addr, u8 value)
{
	u8 v = value;
	UNUSED(dev);
	UNUSED(addr);
	NOR_WR_STATUS(cmd, &v);
	return ERR_NONE;
}

/* Private variables ---------------------------------------------------------*/

static const struct flash_ops nor_flash_ops = {
	.read = nor_dev_read,
	.write = nor_dev_write,
	.erase = nor_dev_erase,
	.is_bad = nor_dev_is_bad,
	.mark_bad = nor_dev_mark_bad,
	.get_status = nor_dev_get_status,
	.set_status = nor_dev_set_status,
};

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Initialize the NOR flash device instance.
  * @param  dev : device to fill in
  * @retval ERR_NONE on success
  */
u8 nor_dev_init(struct flash_dev *dev)
{
	nor_ftl_init(&dev->info);

	dev->ops = &nor_flash_ops;
	dev->media = FLASH_MEDIA_NOR;
	dev->caps = FLASH_CAP_XIP;
	dev->page_size = NOR_PROG_PAGE_SIZE;
	dev->erase_size = NOR_SECTOR_SIZE;
	dev->xip_base = SPI_FLASH_BASE;
	dev->initialized = 1U;

	return ERR_NONE;
}
