/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "ameba_flash_dev.h"

/* Private function prototypes -----------------------------------------------*/

/* Backend setup, defined in ameba_nor_dev.c / ameba_nand_dev.c */
extern u8 nor_dev_init(struct flash_dev *dev);
#if defined(CONFIG_SUPPORT_NAND_FLASH)
extern u8 nand_dev_init(struct flash_dev *dev);
#endif

/* Private variables ---------------------------------------------------------*/

static struct flash_dev nor_dev_inst;
#if defined(CONFIG_SUPPORT_NAND_FLASH)
static struct flash_dev nand_dev_inst;
#endif

/* Exported functions --------------------------------------------------------*/

struct flash_dev *flash_dev_get(enum flash_media media)
{
	struct flash_dev *dev = NULL;

	if (media == FLASH_MEDIA_NAND) {
#if defined(CONFIG_SUPPORT_NAND_FLASH)
		dev = &nand_dev_inst;
		if (!dev->initialized) {
			if (nand_dev_init(dev) != ERR_NONE) {
				return NULL;
			}
		}
#else
		dev = NULL;
#endif
	} else {
		dev = &nor_dev_inst;
		if (!dev->initialized) {
			if (nor_dev_init(dev) != ERR_NONE) {
				return NULL;
			}
		}
	}

	return dev;
}

enum flash_media flash_dev_probe_media(void)
{
#if defined(CONFIG_SUPPORT_NAND_FLASH)
	/* OTP boot-select tells which media the chip actually booted from. */
	return (SYSCFG_OTP_BootFromNor() != 0U) ? FLASH_MEDIA_NOR : FLASH_MEDIA_NAND;
#else
	return FLASH_MEDIA_NOR;
#endif
}

u8 flash_dev_read(struct flash_dev *dev, u32 offset, u8 *buf, u32 len)
{
	if ((dev == NULL) || (dev->ops == NULL) || (dev->ops->read == NULL)) {
		return ERR_INVALID;
	}
	return dev->ops->read(dev, offset, buf, len);
}

u8 flash_dev_write(struct flash_dev *dev, u32 offset, const u8 *buf, u32 len)
{
	if ((dev == NULL) || (dev->ops == NULL) || (dev->ops->write == NULL)) {
		return ERR_INVALID;
	}
	return dev->ops->write(dev, offset, buf, len);
}

u8 flash_dev_erase(struct flash_dev *dev, u32 offset, u32 len)
{
	if ((dev == NULL) || (dev->ops == NULL) || (dev->ops->erase == NULL)) {
		return ERR_INVALID;
	}
	return dev->ops->erase(dev, offset, len);
}

u8 flash_dev_is_bad(struct flash_dev *dev, u32 offset, u8 *bad)
{
	if ((dev == NULL) || (dev->ops == NULL) || (dev->ops->is_bad == NULL)) {
		return ERR_INVALID;
	}
	return dev->ops->is_bad(dev, offset, bad);
}

u8 flash_dev_mark_bad(struct flash_dev *dev, u32 offset)
{
	if ((dev == NULL) || (dev->ops == NULL) || (dev->ops->mark_bad == NULL)) {
		return ERR_INVALID;
	}
	return dev->ops->mark_bad(dev, offset);
}

u8 flash_dev_get_status(struct flash_dev *dev, u8 cmd, u8 addr, u8 *value)
{
	if ((dev == NULL) || (dev->ops == NULL) || (dev->ops->get_status == NULL)) {
		return ERR_INVALID;
	}
	return dev->ops->get_status(dev, cmd, addr, value);
}

u8 flash_dev_set_status(struct flash_dev *dev, u8 cmd, u8 addr, u8 value)
{
	if ((dev == NULL) || (dev->ops == NULL) || (dev->ops->set_status == NULL)) {
		return ERR_INVALID;
	}
	return dev->ops->set_status(dev, cmd, addr, value);
}
