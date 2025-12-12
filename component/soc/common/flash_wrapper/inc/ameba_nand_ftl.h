/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_NAND_FTL_H_
#define _AMEBA_NAND_FTL_H_

/* Includes ------------------------------------------------------------------*/

#include "ameba_ftl.h"

/* Exported defines ----------------------------------------------------------*/

#define NF_BAD_BLOCK				0x00U
#define NF_GOOD_BLOCK				0xFFU

#define NF_SMALL_PAGE_SIZE  2048U
#define NF_BIG_PAGE_SIZE    4096U

/* Exported types ------------------------------------------------------------*/

struct nand_ftl_device {
	struct ameba_flash_info *flash_info;
	void *mfg_ops;
	__IO u8 cur_target;
	__IO u8 initialized;
	__IO u32 last_erased_block_address;
};

struct nand_ftl_mfg_ops {
	u8(* init)(struct nand_ftl_device *nand);
	u8(* select_target)(struct nand_ftl_device *nand, u8 target);
	u8(* get_ecc_status)(struct nand_ftl_device *nand, u8 status);
};

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

u8 nand_ftl_init(struct ameba_flash_info *info);
u8 nand_ftl_read_page(u32 addr, u8 *buf);
u8 nand_ftl_read_page_status(u32 addr, u8 *buf);
u8 nand_ftl_read_block_status(u32 addr, u8 *buf, u8 *block_status, u32 *page_status);
u8 nand_ftl_erase_block(u32 addr, u8 force);
u8 nand_ftl_write_page(u32 addr, const u8 *buf);
u8 nand_ftl_is_bad(u32 addr, u8 *value);
u8 nand_ftl_mark_bad(u32 addr);
u8 nand_ftl_get_status(u8 cmd, u8 addr, u8 *value);
u8 nand_ftl_set_status(u8 cmd, u8 addr, u8 value);

#endif // _AMEBA_NAND_FTL_H_

