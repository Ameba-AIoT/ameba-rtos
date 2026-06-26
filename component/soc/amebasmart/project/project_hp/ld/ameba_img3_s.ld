ENTRY(Reset_Handler)

#include "../../ameba_layout.ld"

SECTIONS
{
	/* image3: security image start */
	.xip_image3.entry :
	{
		__image3_xip_entry_start__ = .;
	} > KM4_BD_DRAM_TZ

	/* image3 entry table + NSC veneers at the start of the TZ region */
	.ram_image3.entry :
	{
		__image3_ram_entry_start__ = .;
		KEEP(*(.image3.entry.data))
		KEEP(*(.image3.nsc_entry.text*))
		__ram_tz_end__ = .;
	} > KM4_BD_DRAM_TZ

	.gnu.sgstubs :
	{
		. = ALIGN(32);
		__ram_image3_nsc_start__ = .;
		*(.gnu.sgstubs*)
		. = ALIGN(32);
	} > KM4_BD_DRAM_TZ

	__ram_image3_nsc_end__ = ADDR(.gnu.sgstubs) + SIZEOF(.gnu.sgstubs);

	.ram_image3.text :
	{
		__ram_image3_text_start__ = .;
		*(.text*)
		*(.sramdram.only.text*)
		*(.rodata*)
		__ram_image3_text_end__ = .;
	} > KM4_BD_DRAM_TZ

	.ram_image3.data :
	{
		. = ALIGN(32);
		__ram_image3_data_start__ = .;
		*(.data*)
		*(.sramdram.only.data)
		__ram_image3_data_end__ = .;
	} > KM4_BD_DRAM_TZ

	.ram_image3.bss :
	{
		__image3_bss_start__ = .;
		*(.bss*)
		*(COMMON)
		__image3_bss_end__ = .;
		end = .;
		__image3_heap_start__ = .;
	} > KM4_BD_DRAM_TZ

	/* image2 NS entry: image3 uses this to jump into NS world.
	 * The symbol is defined in image2's linker script at ORIGIN(KM4_BD_DRAM).
	 * Provide the address here so image3 can reference it. */
	.ram_image2.entry (NOLOAD) :
	{
		__image2_entry_func__ = .;
	} > KM4_BD_DRAM

}

/* TZ region bounds (used by SAU config and post-build tools) */
__image3_ram_start__ = ORIGIN(KM4_BD_DRAM_TZ) - IMAGE_HEADER_LEN;
__image3_ram_end__   = __image3_ram_start__ + LENGTH(KM4_BD_DRAM_TZ) + IMAGE_HEADER_LEN;
__image3_heap_size__ = __image3_ram_end__ - __image3_heap_start__;
