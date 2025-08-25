ENTRY(Reset_Handler)

#include "../../../ameba_layout.ld"
#include "project_km4tz/platform_autoconf.h"
#if defined CONFIG_IMG3_PSRAM
	#define LMA_TZ_NSC        KM4TZ_BD_PSRAM_TZ_NSC
	#define LMA_TZ_ENTRY      KM4TZ_BD_PSRAM_TZ_ENTRY
	#define LMA_TZ_S_TEXT_RO  KM4TZ_BD_PSRAM_TZ_S
	#define LMA_TZ_S_BSS      KM4TZ_BD_PSRAM_TZ_S
#elif defined CONFIG_IMG3_FLASH
	#define LMA_TZ_NSC        KM4TZ_BD_RAM_TZ_NSC
	#define LMA_TZ_ENTRY      KM4TZ_BD_RAM_TZ_ENTRY
	#define LMA_TZ_S_TEXT_RO  KM4TZ_IMG3_XIP
	#define LMA_TZ_S_BSS      KM4TZ_BD_RAM_TZ_S
#else
	#define LMA_TZ_NSC        KM4TZ_BD_RAM_TZ_NSC
	#define LMA_TZ_ENTRY      KM4TZ_BD_RAM_TZ_ENTRY
	#define LMA_TZ_S_TEXT_RO  KM4TZ_BD_RAM_TZ_S
	#define LMA_TZ_S_BSS      KM4TZ_BD_RAM_TZ_S
#endif

SECTIONS
{
	/* image3: security image start */
	.gnu.sgstubs :
	{
		__ram_image3_nsc_start__ = .;
		. = ALIGN(32);
	} > LMA_TZ_NSC

	.ram_image3.entry :
	{
		KEEP(*(.image3.nsc_entry.text*))
		. = ALIGN(32);
	} > LMA_TZ_ENTRY

	.ram_image3.text :
	{
		__ram_image3_text_start__ = .;
		*(.text*)
		*(.rodata*)
		. = ALIGN(32);
	} > LMA_TZ_S_TEXT_RO

	.ram_image3.data :
	{
		__ram_image3_data_start__ = .;
		*(.data*)
		__ram_image3_data_end__ = .;

		. = ALIGN(32);
		__ram_image3_text_end__ = .;
	} > LMA_TZ_S_TEXT_RO

	.ram_image3.bss (NOLOAD):
	{
		__image3_bss_start__ = .;
		*(.bss*)
		*(COMMON)
		__image3_bss_end__ = .;
		end = .;
		. = . + 4096;
	} > LMA_TZ_S_BSS

}
