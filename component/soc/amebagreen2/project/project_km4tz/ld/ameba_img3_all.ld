ENTRY(Reset_Handler)

#include "../../ameba_layout.ld"
#include "project_km4tz/platform_autoconf.h"

#if defined(CONFIG_IMG3_PSRAM)
#define RAM_IMAGE3_TZ     KM4TZ_BD_PSRAM_TZ
#define LMA_TZ_S_TEXT_RO  KM4TZ_BD_PSRAM_TZ
#elif defined(CONFIG_IMG3_FLASH)
#define RAM_IMAGE3_TZ     KM4TZ_BD_RAM_TZ
#define LMA_TZ_S_TEXT_RO  KM4TZ_IMG3_XIP
#else
#define RAM_IMAGE3_TZ     KM4TZ_BD_RAM_TZ
#define LMA_TZ_S_TEXT_RO  KM4TZ_BD_RAM_TZ
#endif


SECTIONS
{

	.ram_image3.entry :
	{
		__image3_entry_func__ = .;
		KEEP(*(.image3.entry.data))
		KEEP(*(.image3.nsc_entry.text*))
		__ram_tz_end__ = .;
	} > LMA_TZ_S_TEXT_RO

	.gnu.sgstubs :
	{
		. = ALIGN(32);
		__ram_image3_nsc_start__ = .;
		*(.gnu.sgstubs*)
		. = ALIGN(32);
	} > LMA_TZ_S_TEXT_RO

	__ram_image3_nsc_end__ = ADDR(.gnu.sgstubs) + SIZEOF(.gnu.sgstubs);

	.ram_image3.text :
	{
		__ram_image3_text_start__ = .;
		*(.text*)
		*(.rodata*)
		__ram_image3_text_end__ = .;
	} > LMA_TZ_S_TEXT_RO

  #if !defined(CONFIG_IMG3_FLASH)
        /* Reserve space for the ram_3_s sub-image header that lands here
         * when the monolithic image3_all.bin is copied to RAM as a single blob. */
        . = . + IMAGE_HEADER_LEN;
  #endif

	.ram_image3.data : ALIGN(32)
	{
		__ram_image3_data_start__ = .;
		*(.data*)
		__ram_image3_data_end__ = .;
	} > RAM_IMAGE3_TZ

	.ram_image3.bss (NOLOAD):
	{
		__image3_bss_start__ = .;
		*(.bss*)
		*(COMMON)
		__image3_bss_end__ = .;
		end = .;
		__image3_heap_start__ = .;
	} > RAM_IMAGE3_TZ

	/* image2: normal image start */
	.ram_image2.entry :
	{
		__image2_entry_func__ = .;
	} > KM4TZ_BD_RAM
}

__image3_ram_start__ = ORIGIN(RAM_IMAGE3_TZ) - IMAGE_HEADER_LEN;
__image3_ram_end__ = __image3_ram_start__ + LENGTH(RAM_IMAGE3_TZ) + IMAGE_HEADER_LEN;
__image3_heap_size__ = __image3_ram_end__ - __image3_heap_start__;
