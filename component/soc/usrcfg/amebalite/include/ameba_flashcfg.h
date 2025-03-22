/*
* Copyright (c) 2024 Realtek Semiconductor Corp.
*
* SPDX-License-Identifier: Apache-2.0
*/

#ifndef _AMEBA_FLASHCFG_H_
#define _AMEBA_FLASHCFG_H_

/** @addtogroup FLASH_Exported_Types FLASH Exported Types
  * @{
*/


/**
  * @brief Flash_Region_Type
  */
typedef enum _FLASH_REGION_TYPE_ {
	IMG_BOOT        = 0,
	IMG_BOOT_OTA2   = 1,
	IMG_APP_OTA1    = 2,
	IMG_APP_OTA2    = 3,
	IMG_DSP         = 4,
	FTL             = 5,
	VFS1            = 6,
	VFS2            = 7,
	USER            = 8,
#ifdef CONFIG_SOC_SOLO
	IMG_KR4_OTA1,
	IMG_KR4_OTA2,
#endif
} FLASH_REGION_TYPE, *PFLASH_REGION_TYPE;

/**
  * @brief  FLASH Layout Info Structure Definition
  */
typedef struct {
	u32 region_type;
	u32 start_addr;
	u32 end_addr;
} FlashLayoutInfo_TypeDef;
/** @} */

extern const u16 Flash_Speed;
extern const u16 Flash_ReadMode;

extern const FlashInfo_TypeDef Flash_AVL[];
extern FlashLayoutInfo_TypeDef Flash_Layout[];

//================functions=================
void flash_init_userdef(void);
void flash_get_layout_info(u32 type, u32 *start, u32 *end);

#endif
