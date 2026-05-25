/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "rom/ameba_dump_stack.h"
#include "rom/ameba_v8m_crashdump.h"
#include "boot_ota_km4ns_solo.h"



#ifdef CONFIG_SOLO
u32 BOOT_LoadImages(void)
{
	/* Load from OTA and ECC check for Certificate and IMG2*/
	BOOT_OTA_IMG();

	return TRUE;
}

/* load SRAM bin for itself */
void BOOT_CopySRAMImg(void)
{
	u32 LogAddr = (u32)__km4ns_flash_text_start__ - IMAGE_HEADER_LEN;
	IMAGE_HEADER ImgHdr;

	/* xip image header */
	_memcpy((void *)&ImgHdr, (void *)LogAddr, IMAGE_HEADER_LEN);

	LogAddr += (ImgHdr.image_size + IMAGE_HEADER_LEN);

	/* sram image header */
	_memcpy((void *)&ImgHdr, (void *)LogAddr, IMAGE_HEADER_LEN);

	/* sram image */
	if ((!IS_FLASH_ADDR(ImgHdr.image_addr)) && (ImgHdr.image_size > IMAGE_HEADER_LEN)) {
		_memcpy((void *)ImgHdr.image_addr, (void *)(LogAddr + IMAGE_HEADER_LEN), ImgHdr.image_size);
		DCache_CleanInvalidate((u32)ImgHdr.image_addr, ImgHdr.image_size);
	}
}
#endif

//3 Image 1
void BOOT_Start(void)
{
	PRAM_START_FUNCTION Image2EntryFun = (PRAM_START_FUNCTION)__image2_entry_func__;

#ifdef CONFIG_SOLO
	BOOT_CopySRAMImg();

	BOOT_LoadImages();
#endif

	/* goto IMG2 */
	Image2EntryFun->RamStartFun();

	return;
}
