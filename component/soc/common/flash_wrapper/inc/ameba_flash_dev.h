/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_FLASH_DEV_H_
#define _AMEBA_FLASH_DEV_H_

/* Includes ------------------------------------------------------------------*/

#include "ameba_ftl.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported defines ----------------------------------------------------------*/

/* Capability flags: callers test these to know which optional ops are valid */
#define FLASH_CAP_BADBLOCK		((u32)1U << 0)	/* is_bad/mark_bad are meaningful (NAND) */
#define FLASH_CAP_ECC_STATUS	((u32)1U << 1)	/* read() return code carries ECC bitflip status (NAND) */
#define FLASH_CAP_XIP			((u32)1U << 2)	/* device is memory-mapped, xip_base valid (NOR) */

/* Exported types ------------------------------------------------------------*/

enum flash_media {
	FLASH_MEDIA_NOR  = 0,
	FLASH_MEDIA_NAND = 1,
};

struct flash_dev;

/*
 * Unified raw-flash operation table.
 *
 * Conventions shared by NOR and NAND backends:
 *   - offset : physical byte offset from 0 (NOT the XIP absolute address)
 *   - len    : byte count
 *   - return : enum ftl_errno (ERR_NONE on success)
 *
 * This layer is a raw device abstraction only: it does NOT remap logical to
 * physical, does NOT skip bad blocks and does NOT do wear levelling. Bad-block
 * handling is the caller's responsibility (see FLASH_CAP_BADBLOCK).
 */
struct flash_ops {
	/* Mandatory: both NOR and NAND implement these. */
	u8(*read)(struct flash_dev *dev, u32 offset, u8 *buf, u32 len);
	u8(*write)(struct flash_dev *dev, u32 offset, const u8 *buf, u32 len);
	u8(*erase)(struct flash_dev *dev, u32 offset, u32 len);	/* rounds up to erase_size internally */

	/* Optional: NOR may report "always good" / "unsupported". */
	u8(*is_bad)(struct flash_dev *dev, u32 offset, u8 *bad);	/* bad: 1 = bad block, 0 = good */
	u8(*mark_bad)(struct flash_dev *dev, u32 offset);

	/* Status register access (NOR ignores addr; NAND uses it as feature addr). */
	u8(*get_status)(struct flash_dev *dev, u8 cmd, u8 addr, u8 *value);
	u8(*set_status)(struct flash_dev *dev, u8 cmd, u8 addr, u8 value);
};

struct flash_dev {
	const struct flash_ops *ops;
	struct ameba_flash_info info;	/* geometry filled at init (ONFI for NAND, ID for NOR) */
	enum flash_media media;
	u32 caps;			/* FLASH_CAP_* bitmask */
	u32 page_size;			/* program/read granularity in bytes */
	u32 erase_size;			/* minimum erase unit in bytes */
	u32 xip_base;			/* XIP base address, 0 if device is not memory-mapped */
	u8  initialized;
};

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Get the flash device instance for a given media type.
  * @param  media : FLASH_MEDIA_NOR or FLASH_MEDIA_NAND
  * @retval Pointer to the (lazily initialized) device, or NULL if unavailable.
  */
struct flash_dev *flash_dev_get(enum flash_media media);

/**
  * @brief  Probe the media the system actually booted from.
  * @retval FLASH_MEDIA_NOR or FLASH_MEDIA_NAND
  */
enum flash_media flash_dev_probe_media(void);

/**
  * @brief  Convert a device/absolute address to a physical offset.
  *         NAND uses xip_base = 0 (no-op); NOR strips the XIP base address.
  *         Lets callers feed both media through one expression with no branch.
  * @param  dev  : flash device
  * @param  addr : device address (NOR: XIP absolute; NAND: byte offset)
  * @retval physical offset suitable for the flash_dev_* / ops calls
  */
static inline u32 flash_dev_offset(const struct flash_dev *dev, u32 addr)
{
	/* Catch callers that pass a physical offset when an XIP address is required
	 * (or vice-versa).  On NAND xip_base == 0 so the check is always true. */
	assert_param(addr >= dev->xip_base);
	return addr - dev->xip_base;
}

/* Convenience wrappers (NULL-safe dispatch to dev->ops). */
u8 flash_dev_read(struct flash_dev *dev, u32 offset, u8 *buf, u32 len);
u8 flash_dev_write(struct flash_dev *dev, u32 offset, const u8 *buf, u32 len);
u8 flash_dev_erase(struct flash_dev *dev, u32 offset, u32 len);
u8 flash_dev_is_bad(struct flash_dev *dev, u32 offset, u8 *bad);
u8 flash_dev_mark_bad(struct flash_dev *dev, u32 offset);
u8 flash_dev_get_status(struct flash_dev *dev, u8 cmd, u8 addr, u8 *value);
u8 flash_dev_set_status(struct flash_dev *dev, u8 cmd, u8 addr, u8 value);

#ifdef __cplusplus
}
#endif

#endif // _AMEBA_FLASH_DEV_H_
