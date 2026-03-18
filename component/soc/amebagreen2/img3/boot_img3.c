/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* this file is build in no secure img, function in this file will call NSC functions */

#include "ameba_soc.h"
#include "ssl_rom_to_ram_map.h"

static const char *const TAG = "BOOT";

extern void vPortFree(void *pv);
extern void *pvPortMalloc(size_t xWantedSize);
extern int internal_secure_func(void);

void *img3_calloc(uint32_t elementNum, uint32_t elementSize)
{
	uint32_t sz = elementNum * elementSize;
	void *ptr = pvPortMalloc(sz);
	if (ptr) {
		_memset(ptr, 0, sz);
	}
	return ptr;
}

IMAGE3_ENTRY_SECTION
void app_mbedtls_image3_init(void)
{
	/* image3 heap use secure_heap.c */
	ssl_function_map.ssl_calloc = (void *(*)(unsigned int, unsigned int))img3_calloc;
	ssl_function_map.ssl_free = (void (*)(void *))vPortFree;
	ssl_function_map.ssl_printf = (long unsigned int (*)(const char *, ...))DiagPrintf;
	ssl_function_map.ssl_snprintf = (int (*)(char *s, size_t n, const char *format, ...))DiagSnPrintf;
}

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
	RTK_LOGS(TAG, RTK_LOG_INFO, "BOOT_IMG3: BSS [%08x~%08x] SEC: %x \n", __image3_bss_start__, __image3_bss_end__,
			 TrustZone_IsSecure());
	/* reset img3 bss */
	_memset((void *) __image3_bss_start__, 0, (__image3_bss_end__ - __image3_bss_start__));

	/* Initialize mbedtls for secure world, customers can enable it themselves as needed. */
	// app_mbedtls_image3_init();

	IMG3_NsStart((u32)Image2EntryFun->RamStartFun);
}

IMAGE3_BOOT_SECTION
RAM_START_FUNCTION Img3EntryFun0 = {
	.RamStartFun = BOOT_IMG3,
	.RamWakeupFun = IMG3_WakeFromPG,
};
