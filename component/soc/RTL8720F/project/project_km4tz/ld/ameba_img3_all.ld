ENTRY(Reset_Handler)

#include "../../ameba_layout.ld"
#include "project_km4tz/platform_autoconf.h"
#ifdef CONFIG_IMG3_PSRAM
	#define RAM_IMAGE3_TZ		KM4TZ_BD_PSRAM_TZ
#else
	#define RAM_IMAGE3_TZ		KM4TZ_BD_RAM_TZ
#endif

SECTIONS
{
	/* image3: security image start */
	.ram_image3.entry :
	{
		__image3_entry_func__ = .;
		KEEP(*(.image3.entry.data))
		KEEP(*(.image3.nsc_entry.text*))
	} > RAM_IMAGE3_TZ

	.gnu.sgstubs :
	{
		. = ALIGN(32);
		*(.gnu.sgstubs*)
	} > RAM_IMAGE3_TZ

	/* Use section-level symbols so they capture linker-generated stubs too */
	__ram_image3_nsc_start__ = ADDR(.gnu.sgstubs);
	__ram_image3_nsc_end__   = ADDR(.gnu.sgstubs) + SIZEOF(.gnu.sgstubs);

	.ram_image3.text :
	{
		__ram_image3_text_start__ = .;
		*(.text*)
		*(.rodata*)
	} > RAM_IMAGE3_TZ

	.ram_image3.data :
	{
		. = ALIGN(32);
		__ram_image3_data_start__ = .;
		*(.data*)
		__ram_image3_data_end__ = .;

		__ram_image3_text_end__ = .;
	} > RAM_IMAGE3_TZ

	.ram_image3.bss (NOLOAD):
	{
		__image3_bss_start__ = .;
		*(.bss*)
		*(COMMON)
		__image3_bss_end__ = .;
		end = .;
		. = ALIGN(32);
		__image3_heap_start__ = .;
	} > RAM_IMAGE3_TZ

	/* image2: normal image start */
	.ram_image2.entry :
	{
		__image2_entry_func__ = .;
	} > KM4TZ_BD_RAM
}

__image3_ram_start__ = ORIGIN(RAM_IMAGE3_TZ) - IMAGE_HEADER_LEN;
/* ORIGIN(=NSC_START+header) + LENGTH(=total-header) = NSC_START + total = MSP_NS_START.
   This is the exact physical end of the Secure TZ region. */
__image3_ram_end__ = ORIGIN(RAM_IMAGE3_TZ) + LENGTH(RAM_IMAGE3_TZ);
__image3_heap_size__ = __image3_ram_end__ - __image3_heap_start__;
