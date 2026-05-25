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

#ifdef CONFIG_SOLO
#define IMG_APP_OTA1_SIZE	(512 * 1024 - 1)
#define IMG_APP_OTA2_SIZE	(512 * 1024 - 1)
#else
#define IMG_APP_OTA1_SIZE	(1968 * 1024 - 1)
#define IMG_APP_OTA2_SIZE	(1828 * 1024 - 1)
#endif

extern const u16 Flash_ReadMode;
extern const u16 Data_Flash_ReadMode;

extern FlashInfo_TypeDef Flash_AVL[];

extern const FlashLayoutInfo_TypeDef Flash_Layout[];

extern void flash_init_userdef(void);

extern u32 flash_get_readmode(u32 ReadMode);

extern FlashInfo_TypeDef *flash_get_chip_info(u32 flash_id);
extern FlashInfo_TypeDef *data_flash_get_chip_info(u32 flash_id);

void flash_get_layout_info(u32 type, u32 *start, u32 *end);

#ifdef __cplusplus
}
#endif

#endif
