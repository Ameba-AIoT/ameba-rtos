/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_FLASHCFG_H_
#define _AMEBA_FLASHCFG_H_

#ifdef __cplusplus
extern "C" {
#endif

extern const u16 Flash_ReadMode;
extern const u16 Data_Flash_ReadMode;

extern const FlashInfo_TypeDef Flash_AVL[];

extern FlashLayoutInfo_TypeDef Flash_Layout[];

extern void flash_init_userdef(void);

extern u32 flash_get_readmode(u32 ReadMode);

extern FlashInfo_TypeDef *flash_get_chip_info(u32 flash_id);
extern FlashInfo_TypeDef *data_flash_get_chip_info(u32 flash_id);

void flash_get_layout_info_by_addr(u32 addr, u32 *start, u32 *end);
void flash_get_layout_info(u32 type, u32 *start, u32 *end);

#ifdef __cplusplus
}
#endif

#endif