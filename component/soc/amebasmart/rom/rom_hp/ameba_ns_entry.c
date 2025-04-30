/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include "ameba_soc.h"

HAL_ROM_TEXT_SECTION
void clear_ns_rom_bss(void)
{
	/* reset rom_ns bss */
	_memset((void *) __rom_bss_start_ns__, 0, (__rom_bss_end_ns__ - __rom_bss_start_ns__));
}

HAL_ROM_ENTRY_NS_SECTION
ROM_SECURE_CALL_NS_ENTRY rom_sec_call_ns_entry = {
	.clear_ns_rom_bss = clear_ns_rom_bss,
	.loguart_putchar = LOGUART_PutChar,
	.loguart_getchar = LOGUART_GetChar,
	.diagprintf = DiagPrintf,
};
