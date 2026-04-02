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
 * @file whc_hal_rtl8721f.c
 * @brief HAL configuration for RTL8721F/AMEBAGREEN2
 *
 * This file contains IC-specific configurations for the RTL8721F chip
 * used in AmebaGreen2 series.
 */

#include "whc_hal.h"
#include "whc_host_xfer.h"

/* Memory region configuration for RTL8721F */
static const struct whc_mem_region_t rtl8721f_mem_regions[] = {
	{WHC_DEFINE_MEM_REGION(0x00000000, 0x00100000, WHC_MEM_TYPE_TCM)},
	{WHC_DEFINE_MEM_REGION(0x00200000, 0x20000000, WHC_MEM_TYPE_FLASH)},
	{WHC_DEFINE_MEM_REGION(0x20000000, 0x40000000, WHC_MEM_TYPE_SRAM)},
	{WHC_DEFINE_MEM_REGION(0x60000000, 0x80000000, WHC_MEM_TYPE_PSRAM)},
	{0}  /* Terminator */
};

/* Image configuration for RTL8721F */
static const struct whc_image_t rtl8721f_images[] = {
	{WHC_DEFINE_IMAGE("km4tz_fullmac_img_1.bin", 0x96969999, 0xFC66CC3F, 0, 0, WHC_IMAGE_TYPE_BOOTLOADER, WHC_POST_PROCESS_BOOT)},
	{WHC_DEFINE_IMAGE("km4tz_fullmac_img_2.bin", 0x35393138, 0x31313738, 0x08040000, 0x3007F000, WHC_IMAGE_TYPE_APPLICATION, WHC_POST_PROCESS_BOOT)},
	{0}  /* Terminator */
};

/* HAL configuration for RTL8721F */
static const struct whc_hal_config_t rtl8721f_config = {
	/* Chip identification */
	.chip_id = 0x8721,
	.chip_name = "RTL8721F",

	/* Protocol version range */
	.min_protocol_version = 0x0100,
	.max_protocol_version = 0x0000,

	/* XFER parameters */
	.hash_size = 32,
	.xfer_page_size = 2 * 1024,		/* 2KB */
	.read_buf_size = 16 * 1024,		/* 16KB (8 pages) */
	.manifest_pos = WHC_XFER_MANIFEST_POS_TAIL,

	/* Memory regions and images */
	.mem_regions = rtl8721f_mem_regions,
	.images = rtl8721f_images,

	/* Firmware paths */
	.image_dir = "/opt/images",
};

/**
 * @brief Get HAL configuration for RTL8721F
 * @return Pointer to RTL8721F HAL configuration
 */
const struct whc_hal_config_t *whc_hal_get_config(void)
{
	return &rtl8721f_config;
}
