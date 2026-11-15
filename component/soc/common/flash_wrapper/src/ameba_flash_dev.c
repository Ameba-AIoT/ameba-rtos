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

/* Private defines -----------------------------------------------------------*/

/* NAND ready probe: 100 x 100us = 10ms, longer than the SPI NAND reset/power-up busy time */
#define NAND_PROBE_POLL_CNT		100U
#define NAND_PROBE_POLL_US		100U

/* Private variables ---------------------------------------------------------*/

static struct flash_dev nor_dev_inst;
#if defined(CONFIG_SUPPORT_NAND_FLASH)
static struct flash_dev nand_dev_inst;
#endif

/* Private functions ---------------------------------------------------------*/

#if defined(CONFIG_NAND_DOWNLOAD_EN) && defined(CONFIG_SUPPORT_NAND_FLASH)
/* Set up SPIC for NOR or NAND the way the ROM does it in BOOT_ROM_InitSpic()/
 * BOOT_ROM_InitFlash(), keeping the pinmux and the baudrate the ROM selected.
 * Flashloader only (CONFIG_NAND_DOWNLOAD_EN): application images do not switch the
 * flash type, and the SPIC ROM API used here differs between SoCs. */
static void flash_dev_reinit_spic(enum flash_media media)
{
	u8 baud_rate = flash_init_para.FLASH_baud_rate;
	u8 baud_boot = flash_init_para.FLASH_baud_boot;
	u8 pinmux = flash_init_para.FLASH_pinmux;

	if (media == FLASH_MEDIA_NAND) {
		NAND_StructInit(&flash_init_para);
		flash_init_para.FLASH_pinmux = pinmux;
		flash_init_para.FLASH_baud_rate = baud_rate;
		flash_init_para.FLASH_baud_boot = baud_boot;
		flash_init_para.FLASH_pagesize_main_bit_exp = (SYSCFG_OTP_BigPageNand() != 0U) ?
				NAND_PAGE_SIZE_MAIN_4K_BIT_EXP : NAND_PAGE_SIZE_MAIN_2K_BIT_EXP;
		NAND_Init(0); /* 1-bit SPI mode, same as ROM */
	} else {
		FLASH_StructInit(&flash_init_para);
		flash_init_para.FLASH_pinmux = pinmux;
		flash_init_para.FLASH_baud_rate = baud_rate;
		flash_init_para.FLASH_baud_boot = baud_boot;
		if (SYSCFG_OTP_SPICAddr4ByteEn() != 0U) {
			flash_init_para.FLASH_addr_phase_len = ADDR_4_BYTE;
		}
		/* NAND_Init() sets DEVICE_INFO for NAND and FLASH_Init() does not touch it: restore the
		 * reset value a NOR boot keeps (NOR flash, WR_PAGE_EN, 256B page) */
		SPIC->DEVICE_INFO = BIT_NOR_FLASH | BIT_WR_PAGE_EN | PAGE_SIZE(8);
		FLASH_Init(0); /* 1-bit SPI mode, same as ROM */
		/* The NOR may sit in deep power down: ROM releases it only when it booted from NOR */
		FLASH_DeepPowerDown(DISABLE);
		if (flash_init_para.FLASH_addr_phase_len == ADDR_4_BYTE) {
#if defined(CONFIG_AMEBAGREEN2)
			FLASH_Addr4ByteEn();
#else
			FLASH_WriteEn();
			FLASH_TxCmd(FLASH_CMD_ENT_ADDR4B, 0, NULL);
			FLASH_WaitBusy(WAIT_FLASH_BUSY);
#endif
		}
	}
}

/* Without a SPI NAND (e.g. a NOR board the trap pin booted as NAND) the status reads busy,
 * so every NAND_WaitBusy() in nand_ftl_init() would poll 0x200000 times and the floader
 * would not answer the host. Check the NAND gets ready within a bounded time first. */
static u8 flash_dev_nand_ready(void)
{
	u32 i;

	for (i = 0; i < NAND_PROBE_POLL_CNT; i++) {
		if ((NAND_GetStatus(NAND_REG_STATUS) & flash_init_para.FLASH_Busy_bit) == 0U) {
			return 1U;
		}
		DelayUs(NAND_PROBE_POLL_US);
	}

	return 0U;
}
#endif

static u8 flash_id_is_blank(u8 mid, u8 did)
{
	return ((mid == 0x00U) && (did == 0x00U)) || ((mid == 0xFFU) && (did == 0xFFU));
}

/* Exported functions --------------------------------------------------------*/

struct flash_dev *flash_dev_get(enum flash_media media)
{
	struct flash_dev *dev = NULL;

	if (media == FLASH_MEDIA_NAND) {
#if defined(CONFIG_SUPPORT_NAND_FLASH)
		dev = &nand_dev_inst;
		if (!dev->initialized) {
#if defined(CONFIG_NAND_DOWNLOAD_EN)
			if (!flash_dev_nand_ready()) {
				return NULL;
			}
#endif
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

u8 flash_dev_check_valid(const struct flash_dev *dev)
{
	const struct ameba_flash_info *info;

	if ((dev == NULL) || (!dev->initialized)) {
		return ERR_HARDWARE;
	}

	info = &dev->info;
	/* No device answers: the data lines float high or are pulled low */
	if (flash_id_is_blank(info->mid, info->did)) {
		return ERR_HARDWARE;
	}

	if (dev->media == FLASH_MEDIA_NAND) {
		/* nand_ftl_init() already rejected a bad ONFI parameter page; check the geometry */
		if ((info->page_size == 0U) || (info->pages_per_block == 0U) || (info->blocks_per_lun == 0U) ||
			(info->capacity == 0U)) {
			return ERR_HARDWARE;
		}
	} else {
		/* A SPI NAND answers the NOR RDID (9Fh) only after a dummy byte, so the MID read
		 * here is the undriven bus (0x00/0xFF). ID byte 3 is not checked: it is not a
		 * capacity code on every NOR (e.g. SST, Adesto), and 1IO download does not need it */
		if ((info->mid == 0x00U) || (info->mid == 0xFFU)) {
			return ERR_HARDWARE;
		}
	}

	return ERR_NONE;
}

struct flash_dev *flash_dev_reinit(enum flash_media media)
{
#if defined(CONFIG_NAND_DOWNLOAD_EN) && defined(CONFIG_SUPPORT_NAND_FLASH)
	struct flash_dev *dev = (media == FLASH_MEDIA_NAND) ? &nand_dev_inst : &nor_dev_inst;

	flash_dev_reinit_spic(media);

	dev->initialized = 0U;
	if (flash_dev_check_valid(flash_dev_get(media)) != ERR_NONE) {
		/* Do not leave a device with bogus ID/geometry marked usable */
		dev->initialized = 0U;
		return NULL;
	}

	return dev;
#else
	UNUSED(media);
	return NULL;
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
