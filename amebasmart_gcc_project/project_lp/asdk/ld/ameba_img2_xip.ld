ENTRY(Reset_Handler)

#include "../../../ameba_layout.ld"

SECTIONS
{
	/* xip part */
	.xip_image2.text :
	{
		__flash_text_start__ = .;

		__flash_text_end__ = .;
	} > KM0_IMG2_XIP

	/* sram part */
	.sram_image2.text.data ORIGIN(KM0_BD_RAM):
	{
		__sram_image2_start__ = .;

		/* add for bootloader, because bootloader and image2 build together now.*/
		KEEP(*(SORT(.image1.entry.data*)))

		__image2_entry_func__ = .;
		KEEP(*(SORT(.image2.entry.data*)))

		*(.timesensitive.text*)

		*(.sram.only.text*)
		*(.sramdram.only.text*)

		__sram_image2_end__ = .;
		. = ALIGN(32);
	} > KM0_BD_RAM

	/* psram part */
	.psram_image2.text.data ORIGIN(KM0_BD_DRAM):
	{
		__psram_image2_start__ = .;

		__psram_image2_end__ = .;
	} > KM0_BD_DRAM

	.xip_image2.text :
	{
		__image2_backtrace_start__ = .;

		/* add for bootloader, because bootloader and image2 build together now.*/
		*(.boot.ram.text*)
		*(.boot.rodata*)

		*(.non.dram.text*)

		*(.text*)
		*(.rodata*)

		. = ALIGN (4);
		__cmd_table_start__ = .;
		KEEP(*(.cmd.table.data*))
		__cmd_table_end__ = .;
		. = ALIGN (4);
		__ipc_table_start__ = .;
		KEEP(*(*.ipc.table.data*))
		__ipc_table_end__ = .;
		. = ALIGN (4);
		__git_ver_table_start__ = .;
		KEEP(*(.git.ver.data))
		__git_ver_table_end__ = .;

		__image2_backtrace_end__ = .;
		. = ALIGN (32);
	} > KM0_IMG2_XIP

	.sram_image2.text.data :
	{
		/* add for bootloader, because bootloader and image2 build together now.*/
		*(.timesensitive.data*)
		*(.sram.only.data*)

		*(.data*)
		*(.sramdram.only.data)
		. = ALIGN (32);
	} > KM0_BD_RAM

	.ram_image2.bss (NOLOAD):
	{
		__bss_start__ = .;

		*(.bss*)
		*(COMMON)
		__bss_end__ = .;
	} > KM0_BD_RAM

	.ram_image2.nocache.data (NOLOAD):
	{
		. = ALIGN (32);
		__ram_nocache_start__ = .;
		*(.nocache.data*)
		. = ALIGN (32);
		__ram_nocache_end__ = .;
	} > KM0_BD_RAM

	.ram_heap.data (NOLOAD):
	{
		__HeapBase = .;
		__end__ = .;
		end = __end__;
		. = . + 4096;	/* reserved for _sbrk */

		__HeapLimit = .;
	} > KM0_BD_RAM

	/* put in the end of sram part */
	.bdram_heap.start (NOLOAD):
	{
		__bdram_heap_buffer_start__ = .;
	} > KM0_BD_RAM

	/* put in the end of psram part */
	.psram_heap.start (NOLOAD):
	{
		__psram_heap_buffer_start__ = .;
	} > KM0_BD_DRAM

}

ASSERT( __flash_text_start__ == ORIGIN(KM0_IMG2_XIP), "__flash_text_start__ is changed.")
ASSERT(__sram_image2_start__ == ORIGIN(KM0_BD_RAM), "__sram_image2_start__ is changed.")
ASSERT( __psram_image2_start__ == ORIGIN(KM0_BD_DRAM), "__psram_image2_start__ is changed.")

__bdram_heap_buffer_size__ = ORIGIN(KM0_BD_RAM) + LENGTH(KM0_BD_RAM) - __bdram_heap_buffer_start__;
__psram_heap_buffer_size__ = 0;
