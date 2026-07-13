/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "ameba_flash_dev.h"
#include "ameba_nand_ftl.h"

/* Private functions ---------------------------------------------------------*/

/*
 * Single-unit contract: each op processes exactly ONE page (read/write) or
 * ONE block (erase).  len must equal page_size or erase_size respectively.
 *
 * Rationale: multi-unit loops inside flash_dev would hide the address of a
 * failing page/block, making it impossible for the caller to perform bad-block
 * skip, retry, or mark-bad.  Per the design decision that bad-block handling
 * is the CALLER's responsibility, these ops intentionally expose the raw
 * single-unit result so the caller has full location information.
 *
 * Callers that need to iterate over a range must loop themselves, which lets
 * them decide per-unit what to do with errors.
 */
static u8 nand_dev_read(struct flash_dev *dev, u32 offset, u8 *buf, u32 len)
{
	u32 ps = dev->page_size;

	UNUSED(dev);
	if ((ps == 0U) || (len != ps) || ((offset % ps) != 0U)) {
		return ERR_INVALID;
	}

	return nand_ftl_read_page(offset, buf);
}

static u8 nand_dev_write(struct flash_dev *dev, u32 offset, const u8 *buf, u32 len)
{
	u32 ps = dev->page_size;

	UNUSED(dev);
	if ((ps == 0U) || (len != ps) || ((offset % ps) != 0U)) {
		return ERR_INVALID;
	}

	return nand_ftl_write_page(offset, buf);
}

static u8 nand_dev_erase(struct flash_dev *dev, u32 offset, u32 len)
{
	u32 bs = dev->erase_size;

	UNUSED(dev);
	if ((bs == 0U) || (len != bs) || ((offset % bs) != 0U)) {
		return ERR_INVALID;
	}

	/* force = 0: report bad block to caller; caller decides whether to skip */
	return nand_ftl_erase_block(offset, 0U);
}

static u8 nand_dev_is_bad(struct flash_dev *dev, u32 offset, u8 *bad)
{
	u8 value = 0;
	u8 ret;

	UNUSED(dev);
	if (bad == NULL) {
		return ERR_INVALID;
	}

	ret = nand_ftl_is_bad(offset, &value);
	*bad = (value != 0U) ? 1U : 0U;
	return ret;
}

static u8 nand_dev_mark_bad(struct flash_dev *dev, u32 offset)
{
	UNUSED(dev);
	return nand_ftl_mark_bad(offset);
}

static u8 nand_dev_get_status(struct flash_dev *dev, u8 cmd, u8 addr, u8 *value)
{
	UNUSED(dev);
	if (value == NULL) {
		return ERR_INVALID;
	}
	return nand_ftl_get_status(cmd, addr, value);
}

static u8 nand_dev_set_status(struct flash_dev *dev, u8 cmd, u8 addr, u8 value)
{
	UNUSED(dev);
	return nand_ftl_set_status(cmd, addr, value);
}

/* Private variables ---------------------------------------------------------*/

static const struct flash_ops nand_flash_ops = {
	.read = nand_dev_read,
	.write = nand_dev_write,
	.erase = nand_dev_erase,
	.is_bad = nand_dev_is_bad,
	.mark_bad = nand_dev_mark_bad,
	.get_status = nand_dev_get_status,
	.set_status = nand_dev_set_status,
};

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Initialize the NAND flash device instance.
  * @param  dev : device to fill in
  * @retval ERR_NONE on success, otherwise an enum ftl_errno error
  */
u8 nand_dev_init(struct flash_dev *dev)
{
	u8 ret;

	ret = nand_ftl_init(&dev->info);
	if (ret != ERR_NONE) {
		return ret;
	}

	dev->ops = &nand_flash_ops;
	dev->media = FLASH_MEDIA_NAND;
	dev->caps = FLASH_CAP_BADBLOCK | FLASH_CAP_ECC_STATUS;
	dev->page_size = dev->info.page_size;
	dev->erase_size = dev->info.page_size * dev->info.pages_per_block;
	dev->xip_base = 0U;	/* NAND is not memory-mapped */
	dev->initialized = 1U;

	return ERR_NONE;
}
