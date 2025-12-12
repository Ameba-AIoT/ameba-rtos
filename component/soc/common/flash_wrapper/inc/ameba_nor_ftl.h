/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_NOR_FTL_H_
#define _AMEBA_NOR_FTL_H_

/* Includes ------------------------------------------------------------------*/

#include "ameba_ftl.h"

/* Exported defines ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

void nor_ftl_init(struct ameba_flash_info *info);
void nor_ftl_read_page(u32 addr, u8 *buf);
void nor_ftl_erase_block(u32 addr, u32 size);
void nor_ftl_erase(u32 addr, u32 size);
void nor_ftl_write_page(u32 addr, const u8 *buf);
u16 nor_ftl_get_status(u8 cmd, u8 size);
void nor_ftl_set_status(u8 cmd, u8 size, u8 *status);

#endif // _AMEBA_NOR_FTL_H_

