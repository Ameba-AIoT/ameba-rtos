/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* this file is build in no secure img, function in this file will call NSC functions */

#include "ameba_soc.h"

static const char *const TAG = "BOOT";

u32 RandSeedTZ = 0x12345;

__NO_RETURN void IMG3_NsStart(u32 Addr)
{
	nsfunc *fp = (nsfunc *)cmse_nsfptr_create(Addr);
	fp();

	/* avoid compiler to pop stack */
	while (1);
}

void IMG3_WakeFromPG(void)
{
	PRAM_START_FUNCTION Image2EntryFun = (PRAM_START_FUNCTION)__image2_entry_func__;
	IMG3_NsStart((u32)Image2EntryFun->RamWakeupFun);
}

void BOOT_IMG3(void)
{
	PRAM_START_FUNCTION Image2EntryFun = (PRAM_START_FUNCTION)__image2_entry_func__;
	RTK_LOGI(TAG,  "BOOT_IMG3: BSS [%08x~%08x] SEC: %x \n", __image3_bss_start__, __image3_bss_end__,
			 TrustZone_IsSecure());
	/* reset img3 bss */
	_memset((void *) __image3_bss_start__, 0, (__image3_bss_end__ - __image3_bss_start__));
	/* TODO: generate random seed in trustzone */

	IMG3_NsStart((u32)Image2EntryFun->RamStartFun);
}

IMAGE3_BOOT_SECTION
RAM_START_FUNCTION Img3EntryFun0 = {
	.RamStartFun = BOOT_IMG3,
	.RamWakeupFun = IMG3_WakeFromPG,
};

