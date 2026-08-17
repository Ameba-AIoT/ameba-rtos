// SPDX-License-Identifier: GPL-2.0-only
/******************************************************************************
 *
 * Copyright(c) Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 *
 ******************************************************************************/

/**
 * @file whc_hal_rtl8720f.c
 * @brief HAL configuration for RTL8720F
 *
 * This file contains IC-specific configurations for the RTL8720F chip
 */

#include "whc_hal.h"
#include "whc_host_xfer.h"
#include <linux/module.h>

/* MP firmware select: mp=1 downloads *_MP.bin images */
static bool mp;
module_param(mp, bool, 0444);
MODULE_PARM_DESC(mp, "Download MP firmware images (*_MP.bin) when set to 1");

/* Memory region configuration for RTL8720F */
static const struct whc_mem_region_t rtl8720f_mem_regions[] = {
	{WHC_DEFINE_MEM_REGION(0x00000000, 0x00100000, WHC_MEM_TYPE_TCM)},    /* TCM: 64KB FULLMAC_TCM_RAM + ROM */
	{WHC_DEFINE_MEM_REGION(0x00200000, 0x20000000, WHC_MEM_TYPE_FLASH)},  /* Flash XIP */
	{WHC_DEFINE_MEM_REGION(0x20000000, 0x40000000, WHC_MEM_TYPE_SRAM)},   /* SRAM: 512KB */
	{WHC_DEFINE_MEM_REGION(0x60000000, 0x80000000, WHC_MEM_TYPE_PSRAM)},  /* PSRAM placeholder */
	{0}  /* Terminator */
};

/* RTL8720F only has SDIO; no USB variant */
#define WHC_FW_1_BASE "RTL8720F_FW_1"
#define WHC_FW_2_BASE "RTL8720F_FW_2"

#define WHC_FW_1_NAME WHC_FW_1_BASE ".bin"
#define WHC_FW_2_NAME WHC_FW_2_BASE ".bin"
#define WHC_FW_1_MP_NAME WHC_FW_1_BASE "_MP.bin"
#define WHC_FW_2_MP_NAME WHC_FW_2_BASE "_MP.bin"

/* Image configuration for RTL8720F (image_name rewritten to *_MP.bin when mp=1) */
static struct whc_image_t rtl8720f_images[] = {
	/* FW_1: ROM-mode bootloader download (signature 0x96969999/0xFC66CC3F) */
	{WHC_DEFINE_IMAGE(WHC_FW_1_NAME, 0x96969999, 0xFC66CC3F, 0x00000000, 0x00000000, WHC_IMAGE_TYPE_BOOTLOADER, WHC_POST_PROCESS_BOOT)},
	/* FW_2: Application image (km4tz + km4ns combined, APP signature) */
	{WHC_DEFINE_IMAGE(WHC_FW_2_NAME, 0x35393138, 0x31313738, 0x08001000, 0x3007F000, WHC_IMAGE_TYPE_APPLICATION, WHC_POST_PROCESS_BOOT)},
	{0}  /* Terminator */
};

/* HAL configuration for RTL8720F (images selected at runtime by whc_hal_get_config) */
static struct whc_hal_config_t rtl8720f_config = {
	/* Chip identification — chip_id must match HCI_CHIP_ID in hci_core.h */
	.chip_id = 0x8721,
	.chip_name = "RTL8720F",

	/* Protocol version (must match HCI_PROTOCOL_VERSION in hci_core.h) */
	.min_protocol_version = 0x0100,
	.max_protocol_version = 0x0000,

	/* XFER parameters */
	.hash_size = 32,
	.xfer_page_size = 2 * 1024,   /* 2KB */
	.read_buf_size = 16 * 1024,   /* 16KB (8 pages) */
	.manifest_pos = WHC_XFER_MANIFEST_POS_TAIL,

	/* Memory regions and images */
	.mem_regions = rtl8720f_mem_regions,
	.images = rtl8720f_images,

	/* Firmware paths */
	.image_dir = "/opt/images",
};

/**
 * @brief Get HAL configuration for RTL8720F
 * @return Pointer to RTL8720F HAL configuration
 */
const struct whc_hal_config_t *whc_hal_get_config(void)
{
	/* Rewrite image names to MP firmware when the mp module parameter is set */
	if (mp) {
		rtl8720f_images[0].image_name = WHC_FW_1_MP_NAME;
		rtl8720f_images[1].image_name = WHC_FW_2_MP_NAME;
	}
	return &rtl8720f_config;
}
