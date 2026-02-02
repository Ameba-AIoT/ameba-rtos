/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* this file is build in no secure img, function in this file will call NSC functions */

#include "ameba_soc.h"

static const char *const TAG = "BOOT-S";

extern void vPortFree(void *pv);
extern void *pvPortMalloc(size_t xWantedSize);

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
	CRYPTO_Init(NULL);
	CRYPTO_SHA_Init(NULL);
	RCC_PeriphClockCmd(APBPeriph_RSA, APBPeriph_CLOCK_NULL, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_ECDSA, APBPeriph_ECDSA_CLOCK, ENABLE);
	/* image3 heap use secure_heap.c */
	ssl_function_map.ssl_calloc = (void *(*)(unsigned int, unsigned int))img3_calloc;
	ssl_function_map.ssl_free = (void (*)(void *))vPortFree;
	ssl_function_map.ssl_printf = (long unsigned int (*)(const char *, ...))DiagPrintf;
	ssl_function_map.ssl_snprintf = (int (*)(char *s, size_t n, const char *format, ...))DiagSnPrintf;
}

IMAGE3_ENTRY_SECTION
void NS_ENTRY BOOT_IMG3(void)
{
	RTK_LOGI(TAG, "BOOT_IMG3: BSS [%08lX~%08lX] SEC: %lx \n", __image3_bss_start__, __image3_bss_end__,
			 TrustZone_IsSecure());

	/* reset img3 bss */
	_memset((void *) __image3_bss_start__, 0, (__image3_bss_end__ - __image3_bss_start__));

	/* Initialize mbedtls for secure world, customers can enable it themselves as needed. */
	// app_mbedtls_image3_init();
}

