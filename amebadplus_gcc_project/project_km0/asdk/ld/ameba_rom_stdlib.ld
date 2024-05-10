/* ENTRY(Reset_Handler) */

INCLUDE "../../amebaDplus_layout.ld"

/* just build stdlib_link.c to link all stdlib funtions like printf/scanf/.... */
SECTIONS
{
	/*__test_symbol__ = ORIGIN(DROM_NS);*/
	.rom.rodata :
	{
		__rom_entry_ns_start__ = .;

	} > KM0_DROM

	.rom.stdlib.data :
	{
		__rom_stdlib_data_start__ = .;
		*(.data*)
		__rom_stdlib_data_end__ = .;
		. = ALIGN(32);
	} > KM0_STDLIB_HEAP_RAM

	.rom.stdlib.bss :
	{
		__rom_stdlib_bss_start__ = .;
		*(.bss*)
		*(COMMON)
		__rom_stdlib_bss_end__ = .;
		. = ALIGN(32);
	} > KM0_STDLIB_HEAP_RAM

	.rom.stdlib.heap :
	{
		__rom_stdlib_heap_start__ = .;
		end = .;
	} > KM0_STDLIB_HEAP_RAM

	.rom.stdlib.text :
	{
		__rom_stdlib_text_start__ = .;
		KEEP(*(SORT(.stdlib.entry.table*))) /* called before call stdlib */
		*(*.text*)
		*(*.rodata*)
		. = ALIGN(32);
		__rom_stdlib_text_end__ = .;
	} > KM0_IROM_STDLIB
}

ASSERT( __rom_stdlib_data_end__ - __rom_stdlib_data_start__ + __rom_stdlib_text_end__ < ORIGIN(KM0_IROM_STDLIB) + LENGTH(KM0_IROM_STDLIB), "STDLIB size overflow.")