/**
  ******************************************************************************
  * @file    rtl8721d_otf_rom.c
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file contains all the functions prototypes for the flash run time decrypt firmware
  *          library.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#include "ameba_soc.h"

/**
  * @brief  init flash run time decrypt
  * @param  KeySize: set keysize for RSIP
  * @note RSIP_OTF_init is called in ROM code when boot
  */
__weak HAL_ROM_TEXT_SECTION
void RSIP_OTF_init(u32 KeySize)
{
	RSIP_REG_TypeDef *RSIP = ((RSIP_REG_TypeDef *) RSIP_REG_BASE);

	RSIP->RSIP_CR = RSIP_KEY_SIZE(KeySize);
}

/**
  * @brief  set iv
  * @param  index: OTF section num, should be 0-7
  * @param  IV: 8 bytes IV for Flash decrypt
  */
__weak HAL_ROM_TEXT_SECTION
void RSIP_IV_Set(u8 index, u8 *IV)
{
	RSIP_REG_TypeDef *RSIP = ((RSIP_REG_TypeDef *) RSIP_REG_BASE);

	/*set iv high 96 bits*/
	RSIP->OTF_IV[index].RSIP_IVxH = ((IV[0] << 24) | (IV[1] << 16) | (IV[2] << 8) | (IV[3]));
	RSIP->OTF_IV[index].RSIP_IVxL = ((IV[4] << 24) | (IV[5] << 16) | (IV[6] << 8) | (IV[7]));
}

/**
  * @brief  ENABLE flash run time decrypt
  * @param  NewStatus: ENABLE/DISABLE
  * @note called in ROM code when boot
  * @note this function can not be called by user.
  */
__weak HAL_ROM_TEXT_SECTION
void RSIP_OTF_Cmd(u32 NewStatus)
{
	RSIP_REG_TypeDef *RSIP = ((RSIP_REG_TypeDef *) RSIP_REG_BASE);

	if (NewStatus == ENABLE) {
		RSIP->RSIP_CR |= RSIP_BIT_AES_EN;
	} else {
		RSIP->RSIP_CR &= (~RSIP_BIT_AES_EN);
	}
}

/**
  * @brief  enable flash run time decrypt in some special FLASH area.
  * @param  EnIdx: 0-7
  * @param BaseAddr: FLASH area base address (should 32B alignment)
  * @param EndAddr: FLASH area end address (should 32B alignment)
  * @param NewStatus This parameter can be one of the following values
  *		 @arg DISABLE close this area run time decypt
  *		 @arg ENABLE enable this area run time decypt (this area will be decrypt when read)
  * @param IVNum: choose the iv for this region , can be 0-7
  * @param Mode: 1 ctr mode, 2 xts mode
  */
__weak HAL_ROM_TEXT_SECTION
void RSIP_OTF_Enable(u32 EnIdx, u32 BaseAddr, u32 EndAddr, u32 NewStatus, u32 IVNum, u32 Mode)
{
	RSIP_REG_TypeDef *RSIP = ((RSIP_REG_TypeDef *) RSIP_REG_BASE);

	if (NewStatus != DISABLE) {
		assert_param(IS_OTF_IVNUM(IVNum));
		assert_param(IS_OTF_MODE(Mode));
		/* disable region first */
		RSIP->OTF_REGION[EnIdx].RSIP_RxEAR &= ~RSIP_BIT_REGION_x_ENABLE;

		RSIP->OTF_REGION[EnIdx].RSIP_RxBAR = BaseAddr | RSIP_REGION_x_IV_NUM(IVNum);
		RSIP->OTF_REGION[EnIdx].RSIP_RxEAR = EndAddr | RSIP_REGION_x_MODE(Mode) | RSIP_BIT_REGION_x_ENABLE ;
	} else {
		RSIP->OTF_REGION[EnIdx].RSIP_RxEAR &= ~RSIP_BIT_REGION_x_ENABLE;
	}
}

/**
  * @brief  Config flash mmu in some special FLASH area.
  * @param  MMUIdx: 0~3
  * @param AddrStart: FLASH area logical address start (should 32B alignment)
  * @param AddrEnd: FLASH area logical address end (should 32B alignment)
  * @param AddrOffset: FLASH area physical address start (should 32B alignment)
  * @note output addr = Access address - AddrStart + AddrOffset.
  */
__weak HAL_ROM_TEXT_SECTION
void RSIP_MMU_Config(u32 MMUIdx, u32 AddrStart, u32 AddrEnd, u32 AddrOffset)
{

	RSIP_REG_TypeDef *RSIP = ((RSIP_REG_TypeDef *) RSIP_REG_BASE);

	/* disable the region first */
	RSIP->FLASH_MMU[MMUIdx].RSIP_REMAPxOR &= ~RSIP_BIT_REMAP_x_ENABLE;

	RSIP->FLASH_MMU[MMUIdx].RSIP_REMAPxBAR = ((AddrStart + 0x1f) >> 5) << 9;
	RSIP->FLASH_MMU[MMUIdx].RSIP_REMAPxEAR = ((AddrEnd + 0x1f) >> 5) << 9;
	RSIP->FLASH_MMU[MMUIdx].RSIP_REMAPxOR = ((AddrOffset + 0x1f) >> 5) << 9;
}

/**
  * @brief  ENABLE flash MMU Entry
  * @param  MMUIdx: 0~3
  * @param  NewStatus: ENABLE/DISABLE
  */
__weak HAL_ROM_TEXT_SECTION
void RSIP_MMU_Cmd(u32 MMUIdx, u32 NewStatus)
{
	RSIP_REG_TypeDef *RSIP = ((RSIP_REG_TypeDef *) RSIP_REG_BASE);

	if (NewStatus != DISABLE) {
		RSIP->FLASH_MMU[MMUIdx].RSIP_REMAPxOR |= RSIP_BIT_REMAP_x_ENABLE;
	} else {
		RSIP->FLASH_MMU[MMUIdx].RSIP_REMAPxOR &= ~RSIP_BIT_REMAP_x_ENABLE;
	}
}

/**
  * @brief Clean MMU Cache
  */
__weak HAL_ROM_TEXT_SECTION
void RSIP_MMU_Cache_Clean(void)
{
	RSIP_REG_TypeDef *RSIP = ((RSIP_REG_TypeDef *) RSIP_REG_BASE);

	RSIP->RSIP_MOR |= RSIP_BIT_CLEAN_CACHE_LINE;

}

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
