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

void app_mbedtls_image3_init(void)
{
#if defined(CONFIG_AMEBASMART)
	CRYPTO_Init(NULL);
	CRYPTO_SHA_Init(NULL);
	RCC_PeriphClockCmd(APBPeriph_RSA, APBPeriph_CLOCK_NULL, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_ECDSA, APBPeriph_ECDSA_CLOCK, ENABLE);
#endif
	/* image3 heap use secure_heap.c */
	ssl_function_map.ssl_calloc = (void *(*)(unsigned int, unsigned int))img3_calloc;
	ssl_function_map.ssl_free = (void (*)(void *))vPortFree;
	ssl_function_map.ssl_printf = (long unsigned int (*)(const char *, ...))DiagPrintf;
	ssl_function_map.ssl_snprintf = (int (*)(char *s, size_t n, const char *format, ...))DiagSnPrintf;
}

extern const SAU_CFG_TypeDef sau_config[];

__NO_RETURN void IMG3_NsStart(u32 Addr)
{
	nsfunc *fp = (nsfunc *)cmse_nsfptr_create(Addr);
	fp();

	/* avoid compiler to pop stack */
	while (1);
}

void IMG3_WakeFromPG(void)
{
	FIH_DECLARE(fih_rc, FIH_FAILURE);
	/* Re-apply SAU in case PG depth reset cleared core registers */
	FIH_CALL(BOOT_CPU_TZCfg, fih_rc, sau_config);
	if (FIH_NOT_EQ(fih_rc, FIH_SUCCESS)) {
		while (1);
	}

	PRAM_START_FUNCTION Image2EntryFun = (PRAM_START_FUNCTION)__image2_entry_func__;
	IMG3_NsStart((u32)Image2EntryFun->RamWakeupFun);
}

void BOOT_IMG3(void)
{
	FIH_DECLARE(fih_rc, FIH_FAILURE);
	PRAM_START_FUNCTION Image2EntryFun = (PRAM_START_FUNCTION)__image2_entry_func__;
	RTK_LOGS(TAG, RTK_LOG_INFO, "BOOT_IMG3: BSS [%08x~%08x] SEC: %x \n", __image3_bss_start__, __image3_bss_end__,
			 TrustZone_IsSecure());
	/* reset img3 bss */
	_memset((void *) __image3_bss_start__, 0, (__image3_bss_end__ - __image3_bss_start__));

	FIH_CALL(BOOT_CPU_TZCfg, fih_rc, sau_config);
	if (FIH_NOT_EQ(fih_rc, FIH_SUCCESS)) {
		/* SAU programming failed; halt before entering NS world */
		while (1);
	}

#ifdef CONFIG_TRUSTZONE_MBEDTLS
	app_mbedtls_image3_init();
#endif

	/* Before entering NS world, clean and invalidate the D-cache.
	 * Addresses in the range covered by SAU entry2 (e.g. 0x10000000~TZ_S_START-1,
	 * including NewVectorTable at 0x20005xxx) were written by the Secure bootloader
	 * before SAU was enabled, so their cache lines are tagged Secure. After SAU is
	 * enabled by BOOT_CPU_TZCfg above, those same addresses become NS-attributed.
	 * The tag-vs-SAU mismatch causes an imprecise BusFault (CFSR.IMPRECISERR) when
	 * NS app_start calls Cache_Enable() -> SCB_EnableDCache() -> NS DCISW, because
	 * NS cache maintenance is not permitted on Secure-tagged lines. With BFHFNMINS=1
	 * the fault escalates to NS HardFault; the NS vector table is not yet initialized
	 * at that point, resulting in a VECTTBL lockup. DCCISW (clean+invalidate) here
	 * writes back all dirty lines and clears their tags before NS takes over the cache.
	 */
	SCB_CleanInvalidateDCache();

	IMG3_NsStart((u32)Image2EntryFun->RamStartFun);
}

IMAGE3_BOOT_SECTION
RAM_START_FUNCTION Img3EntryFun0 = {
	.RamStartFun = BOOT_IMG3,
	.RamWakeupFun = IMG3_WakeFromPG,
};
