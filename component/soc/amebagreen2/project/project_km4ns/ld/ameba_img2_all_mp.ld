ENTRY(Reset_Handler)

#include "../../ameba_layout.ld"

#define OUT_TEXT_SECTION			.xip_image2.text
#define IMG2_TEXT_LMA_REGION	 	KM4NS_BD_RAM_MP
#define FLASH_INPUT_TEXT_SECTION	*(.non.dram.text*)

/* Decide the location of the WLAN critical code here, because release lib section cannot be modified*/
#if defined(CONFIG_WLAN_CRITICAL_CODE_IN_RAM)
	#define WLAN_CRITICAL_TEXT
	#define WLAN_SRAMDRAM_ONLY_TEXT   *(.wlan.critical.text*)
#else
	#define WLAN_CRITICAL_TEXT        *(.wlan.critical.text*)
	#define WLAN_SRAMDRAM_ONLY_TEXT
#endif

#define OUT_DATA_SECTION			.sram_image2.text.data
#define IMG2_DATA_LMA_REGION	 	KM4NS_BD_RAM_MP
#define SRAM_INPUT_TEXT_SECTION		*(.sramdram.only.text*) WLAN_SRAMDRAM_ONLY_TEXT
#define PSRAM_INPUT_TEXT_SECTION

SECTIONS
{
	/* xip part */
	.xip_image2.text :
	{
		__flash_text_start__ = .;
		__flash_text_end__ = .;
	} > KM4NS_IMG2_XIP

	/* sram part */
	.sram_image2.text.data ORIGIN(KM4NS_BD_RAM_MP):
	{
		__sram_image2_start__ = .;
		/* add for bootloader, because bootloader and image2 build together now.*/
		KEEP(*(SORT(.image1.entry.data*)))
		. = ALIGN(32);
		__image2_entry_func__ = .;
		KEEP(*(SORT(.image2.entry.data*)))
		. = ALIGN(32);

		SRAM_INPUT_TEXT_SECTION
		*(.sram.only.text*)
		*(.sram.only.data*)
		__sram_image2_end__ = .;
		. = ALIGN(32);
		FLASH_INPUT_TEXT_SECTION
	} > KM4NS_BD_RAM_MP

	/* psram part */
	.psram_image2.text.data ORIGIN(KM4NS_BD_PSRAM):
	{
		__psram_image2_start__ = .;
		__psram_image2_end__ = .;
	} > KM4NS_BD_PSRAM

	.sram_image2.text.data :
	{
		__image2_backtrace_start__ = .;

		*(.img2_custom_signature*)
		PSRAM_INPUT_TEXT_SECTION
		WLAN_CRITICAL_TEXT

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

		*(.gcc_except_table*)
		/*-----------------*/

		__image2_backtrace_end__ = .;
		. = ALIGN (32);
	} > IMG2_TEXT_LMA_REGION
	/* C++ .ARM.extab/.ARM.exidx address shall same as *(.text*) */
	.ARM.extab  :
	{
		*(.ARM.extab* .gnu.linkonce.armextab.*)

	} > IMG2_TEXT_LMA_REGION

	.ARM.exidx :
	{
		__exidx_start = .;
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
		__exidx_end = .;
	} > IMG2_TEXT_LMA_REGION

	OUT_DATA_SECTION :
	{

		*(.data*)
		. = ALIGN (32);
	} > IMG2_DATA_LMA_REGION

	.ram_image2.bss (NOLOAD):
	{
		__bss_start__ = .;

		*(.bss*)
		*(COMMON)

		__bss_end__ = .;
	} > IMG2_DATA_LMA_REGION

	.ram_image2.bd.data (NOLOAD):
	{
		. = ALIGN (32);
		__ram_bd_start__ = .;
		*(.bdsram.data*)
		. = ALIGN (32);
		__ram_bd_end__ = .;
	} > IMG2_DATA_LMA_REGION

	.ram_image2.nocache.data (NOLOAD):
	{
		. = ALIGN (32);
		__ram_nocache_start__ = .;
		*(.nocache.data*)
		. = ALIGN (32);
		__ram_nocache_end__ = .;
	} > IMG2_DATA_LMA_REGION


	/* put in the end of sram part */
	.bdram_heap.start (NOLOAD):
	{
		__bdram_heap_buffer_start__ = .;
	} > KM4NS_BD_RAM_MP


	/* put in the end of psram part */
	.psram_heap.start (NOLOAD):
	{
		__psram_heap_buffer_start__ = .;
	} > KM4NS_BD_PSRAM


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

ASSERT( __flash_text_start__ == ORIGIN(KM4NS_IMG2_XIP), "__flash_text_start__ is changed.")
ASSERT(__sram_image2_start__ == ORIGIN(KM4NS_BD_RAM_MP), "__sram_image2_start__ is changed.")
ASSERT(__psram_image2_start__ == ORIGIN(KM4NS_BD_PSRAM), "__psram_image2_start__ is changed.")

__bdram_heap_buffer_size__ = ORIGIN(KM4NS_BD_RAM_MP) + LENGTH(KM4NS_BD_RAM_MP) - __bdram_heap_buffer_start__;

__psram_heap_buffer_start__ = .;
__psram_heap_buffer_size__ = 0;

__psram_heap_extend_start__ = .;
__psram_heap_extend_size__ = 0;
