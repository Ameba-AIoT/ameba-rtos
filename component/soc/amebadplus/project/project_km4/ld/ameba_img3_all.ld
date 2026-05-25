ENTRY(Reset_Handler)

#include "../../ameba_layout.ld"
#include "project_km4/platform_autoconf.h"

#ifdef CONFIG_IMG3_SRAM
#define RAM_IMAGE3_TZ	KM4_BD_RAM_TZ
#else
#define RAM_IMAGE3_TZ	KM4_BD_PSRAM_TZ
#endif


SECTIONS
{
	.xip_image3.entry :
	{
		__image3_xip_entry_start__ = .;
	} > RAM_IMAGE3_TZ

	.ram_image3.entry :
	{
		__image3_ram_entry_start__ = .;
		KEEP(*(.image3.entry.data))
		KEEP(*(.image3.nsc_entry.text*))
		__ram_tz_end__ = .;
	} > RAM_IMAGE3_TZ

	.gnu.sgstubs :
	{
		. = ALIGN(32);
		__ram_image3_nsc_start__ = .;
		*(.gnu.sgstubs*)
		. = ALIGN(32);
	} > RAM_IMAGE3_TZ

	__ram_image3_nsc_end__ = ADDR(.gnu.sgstubs) + SIZEOF(.gnu.sgstubs);

	.ram_image3.text :
	{
		__ram_image3_text_start__ = .;
		*(.text*)
		*(.rodata*)
		__ram_image3_text_end__ = .;
	} > RAM_IMAGE3_TZ

	.ram_image3.data :
	{
		. = ALIGN(32);
		__ram_image3_data_start__ = .;
		*(.data*)
		__ram_image3_data_end__ = .;
	} > RAM_IMAGE3_TZ

	.ram_image3.bss :
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
	} > KM4_IMG2_ENTRY
}

/* Calculate remaining space for secure heap */
__image3_ram_start__ = ORIGIN(RAM_IMAGE3_TZ) - IMAGE_HEADER_LEN;
__image3_ram_end__ = __image3_ram_start__ + LENGTH(RAM_IMAGE3_TZ);
__image3_heap_size__ = __image3_ram_end__ - __image3_heap_start__;
