/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "ameba_ota.h"

u32 IMG_ADDR[OTA_IMGID_MAX][2] = {0}; /* IMG Flash Physical Address use for OTA */

/**
  * @brief  get current image2 location
  * @param  image index
  * @retval  The retval can be one of the followings:
  *              OTA_INDEX_1: current images located in OTA1 address space
  *              OTA_INDEX_2: current images located in OTA2 address space
  */
u8 ota_get_cur_index(u8 img_id)
{
	u32 PhyAddr;

	RSIP_REG_TypeDef *RSIP = ((RSIP_REG_TypeDef *) RSIP_REG_BASE);
	u32 AddrStart = RSIP->FLASH_MMU[img_id].RSIP_REMAPxOR;

	if (IMG_ADDR[img_id][OTA_INDEX_1] == 0) {
		flash_get_layout_info(IMG_BOOT, &IMG_ADDR[OTA_IMGID_BOOT][OTA_INDEX_1], NULL);
		flash_get_layout_info(IMG_BOOT_OTA2, &IMG_ADDR[OTA_IMGID_BOOT][OTA_INDEX_2], NULL);
		flash_get_layout_info(IMG_APP_OTA1, &IMG_ADDR[OTA_IMGID_APP][OTA_INDEX_1], NULL);
		flash_get_layout_info(IMG_APP_OTA2, &IMG_ADDR[OTA_IMGID_APP][OTA_INDEX_2], NULL);
		flash_get_layout_info(IMG_DSP, &IMG_ADDR[OTA_IMGID_DSP][OTA_INDEX_1], NULL);
		flash_get_layout_info(IMG_DSP, &IMG_ADDR[OTA_IMGID_DSP][OTA_INDEX_2], NULL);
	}

	if (AddrStart & RSIP_BIT_REMAP_x_ENABLE) {
		PhyAddr = (AddrStart >> 9) << 5;

		if (img_id == OTA_IMGID_APP) {
			/*add certificate and manifest*/
			PhyAddr = PhyAddr - 0x2000;
		} else {
			/*add manifest*/
			PhyAddr = PhyAddr - 0x1000;
		}
		if (PhyAddr == IMG_ADDR[img_id][OTA_INDEX_1]) {
			return OTA_INDEX_1;
		} else {
			return OTA_INDEX_2;
		}
	}

	return OTA_INDEX_1;
}

/**
  * @brief  check if bootloader OTA2 address is valid
  * @param  none
  * The retval can be one of the followings:
  *              0: OTA2 address is valid
  *              1: OTA2 address is invalid
  */
u8 ota_check_bootloader_ota2(void)
{
	u16 tmp;
	u32 ImgAddr;

	tmp = HAL_READ16(OTPC_REG_BASE, SEC_OTA_ADDR);

	/* Take default OTP value as invalid address */
	if (tmp != 0xFFFF) {
		ImgAddr = tmp << 12;
	} else {
		ImgAddr = 0;
	}

	/* check if OTA2 addr is valid. The OTA2 addr in OTP should be a multiples of 4KB. */
	return ((ImgAddr == IMG_ADDR[OTA_IMGID_BOOT][OTA_INDEX_2]) ? 0 : 1);
}
