
#include "../../../ameba_layout.ld"

SECTIONS
{
	/* xip part */
	.xip_image2.text :
	{
		__flash_text_start__ = .;

		__flash_text_end__ = .;
	} > KM4_IMG2_XIP

	/* sram part */
	.sram_image2.text.data ORIGIN(KM4_BD_RAM):
	{
		__sram_image2_start__ = .;

		*(.timesensitive.text*)
		*(.sramdram.only.text*)

		__sram_image2_end__ = .;
		. = ALIGN(32);
	} > KM4_BD_RAM

	/* image2: normal image start */
	.psram_image2.text.data ORIGIN(KM4_BD_DRAM):
	{
		__psram_image2_start__ = .;
		__image2_entry_func__ = .;
		KEEP(*(SORT(.image2.entry.data*)))

		__psram_image2_end__ = .;
	} > KM4_BD_DRAM

	/* C++ .ARM.extab/.ARM.exidx address shall same as *(.text*) */
	.ARM.extab :
	{
		*(.ARM.extab* .gnu.linkonce.armextab.*)

	} > KM4_IMG2_XIP

	.ARM.exidx :
	{
		__exidx_start = .;
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
		__exidx_end = .;
	} > KM4_IMG2_XIP

	.xip_image2.text :
	{
		__image2_backtrace_start__ = .;

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

		/* Add This for C++ support */
		. = ALIGN(4);
		__preinit_array_start = .;
		KEEP(*(.preinit_array))
		__preinit_array_end = .;
		. = ALIGN(4);
		__init_array_start = .;
		KEEP(*(SORT(.init_array.*)))
		KEEP(*(.init_array))
		__init_array_end = .;
		. = ALIGN(4);
		__fini_array_start = .;
		KEEP(*(SORT(.fini_array.*)))
		KEEP(*(.fini_array))
		__fini_array_end = .;

		/*-----------------*/
		__image2_backtrace_end__ = .;

		. = ALIGN(32);
	} > KM4_IMG2_XIP

	.psram_image2.text.data :
	{
		*(.timesensitive.data*)

		*(.data*)

		/*cache line size aligned in order to avoid MMU remap err*/
		. = ALIGN(32);
	} > KM4_BD_DRAM

	/*-----------------*/
	.ram_image2.bss (NOLOAD):
	{
		__bss_start__ = .;
		*(.bss*)
		*(COMMON)
		__bss_end__ = .;
		. = ALIGN(32);
	} > KM4_BD_DRAM

	.ram_image2.nocache.data (NOLOAD):
	{
		. = ALIGN(32);
		__ram_nocache_start__ = .;
		*(.nocache.data*)
		. = ALIGN(32);
		__ram_nocache_end__ = .;
	} > KM4_BD_DRAM

	.ram_heap.data (NOLOAD):
	{
		. = ALIGN(32);
		/*reserved heap to 8k for sbrk*/
		end = .;
		. = . + 8192;
	} > KM4_BD_DRAM

	/* put in the end of sram part */
	.bdram_heap.start (NOLOAD):
	{
		__bdram_heap_buffer_start__ = .;
	} > KM4_BD_RAM

	/* put in the end of psram part */
	.psram_heap.start (NOLOAD):
	{
		__psram_heap_buffer_start__ = .;
	} > KM4_BD_DRAM

	.bluetooth_trace.text :
	{
		__btrace_start__ = .;
		*(.BTTRACE)
		__btrace_end__ = .;
	} > BTRACE

	.coex_trace.text :
	{
		__ctrace_start__ = .;
		*(.COEXTRACE)
		__ctrace_end__ = .;
	} > CTRACE
}

ASSERT(__flash_text_start__ == ORIGIN(KM4_IMG2_XIP), "__flash_text_start__ is changed.")
ASSERT(__sram_image2_start__ == ORIGIN(KM4_BD_RAM), "__sram_image2_start__ is changed.")
ASSERT(__psram_image2_start__ == ORIGIN(KM4_BD_DRAM), "__psram_image2_start__ is changed.")
ASSERT(__image2_entry_func__ == ORIGIN(KM4_BD_DRAM), "__image2_entry_func__ is not KM4_BD_DRAM.")

__bdram_heap_buffer_size__ = ORIGIN(KM4_BD_RAM) + LENGTH(KM4_BD_RAM) - __bdram_heap_buffer_start__;
__psram_heap_buffer_size__ = ORIGIN(KM4_BD_DRAM) + LENGTH(KM4_BD_DRAM) - __psram_heap_buffer_start__;

/* KM4 DRAM HEAP Extend is not used */
__psram_heap_extend_start__ = ORIGIN(KM4_DRAM_HEAP_EXT);
__psram_heap_extend_size__ = 0;
