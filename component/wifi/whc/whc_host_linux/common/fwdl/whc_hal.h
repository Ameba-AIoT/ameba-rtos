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

#ifndef __WHC_HAL_H__
#define __WHC_HAL_H__

#include <linux/types.h>

/**
 * HAL Configuration Structure
 * Each IC should provide its own configuration via whc_hal_get_config()
 */

/* Forward declarations */
struct whc_mem_region_t;
struct whc_image_t;

/**
 * @brief HAL configuration structure
 * Contains all IC-specific parameters for XFER protocol
 */
struct whc_hal_config_t {
	/* Chip identification */
	u16 chip_id;
	const char *chip_name;

	/* Protocol version range */
	u16 min_protocol_version;
	u16 max_protocol_version;

	/* XFER parameters */
	u32 hash_size;
	u32 xfer_page_size;
	u32 read_buf_size;
	u32 manifest_pos;		/* WHC_XFER_MANIFEST_POS_HEAD or WHC_XFER_MANIFEST_POS_TAIL */

	/* Memory regions and images */
	const struct whc_mem_region_t *mem_regions;
	const struct whc_image_t *images;

	/* Firmware paths */
	const char *image_dir;
};

/**
 * @brief Get HAL configuration for the current IC
 * @return Pointer to HAL configuration structure
 *
 * Note: This function must be implemented by each IC-specific HAL file.
 * The build system selects which HAL file to compile based on the target IC.
 */
const struct whc_hal_config_t *whc_hal_get_config(void);

/**
 * @brief Get chip ID
 * @return Chip ID value
 */
static inline u16 whc_hal_get_chip_id(void)
{
	return whc_hal_get_config()->chip_id;
}

/**
 * @brief Get chip name
 * @return Chip name string
 */
static inline const char *whc_hal_get_chip_name(void)
{
	return whc_hal_get_config()->chip_name;
}

/**
 * @brief Get memory regions configuration
 * @return Pointer to memory regions array (NULL terminated)
 */
static inline const struct whc_mem_region_t *whc_hal_get_mem_regions(void)
{
	return whc_hal_get_config()->mem_regions;
}

/**
 * @brief Get images configuration
 * @return Pointer to images array (NULL terminated)
 */
static inline const struct whc_image_t *whc_hal_get_images(void)
{
	return whc_hal_get_config()->images;
}

/**
 * @brief Get firmware image directory
 * @return Image directory path
 */
static inline const char *whc_hal_get_image_dir(void)
{
	return whc_hal_get_config()->image_dir;
}

#endif /* __WHC_HAL_H__ */
