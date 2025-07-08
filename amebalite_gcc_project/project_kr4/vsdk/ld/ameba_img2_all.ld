ENTRY(Reset_Handler)

#include "../../../ameba_layout.ld"

#if defined(CONFIG_IMG2_FLASH)
#define OUT_TEXT_SECTION			.xip_image2.text
#define IMG2_TEXT_LMA_REGION		KR4_IMG2_XIP
#else /* CONFIG_IMG2_PSRAM */
#define OUT_TEXT_SECTION			.psram_image2.text.data
#define IMG2_TEXT_LMA_REGION		KR4_BD_PSRAM
#endif

#if defined(CONFIG_DATA_HEAP_SRAM)
#define OUT_DATA_SECTION			.sram_image2.text.data
#define IMG2_DATA_LMA_REGION		KR4_BD_RAM
#define SRAM_INPUT_TEXT_SECTION		*(.sramdram.only.text*)
#define PSRAM_INPUT_TEXT_SECTION
#else /* CONFIG_DATA_HEAP_PSRAM */
#define OUT_DATA_SECTION			.psram_image2.text.data
#define IMG2_DATA_LMA_REGION		KR4_BD_PSRAM
#define SRAM_INPUT_TEXT_SECTION
#define PSRAM_INPUT_TEXT_SECTION	*(.sramdram.only.text*)
#endif

#if defined(CONFIG_DSP_EN) && defined(CONFIG_DATA_HEAP_SRAM)
#warning When DSP is turned on, DSP has exclusive access to SRAM by default.  Please modify the data and heap location of IMG2 in menuconfig or modify the default running location of DSP.
#endif

SECTIONS
{
	/* fixed region. do not add any code to this region */
	.boot.entry :
	{
		__boot_text_start__ = .;
		/* add for bootloader, because bootloader and image2 build together now.*/
		KEEP(*(SORT(.image1.entry.data*)))
		__boot_text_end__ = .;
		. = ALIGN(32);
	} > KR4_BOOT_RAM_BACKUP

	/* fixed region. do not add any code to this region */
	.sram_only.text.data :
	{
		__sram_only_start__ = .;
		*(.sram.only.text*)
		. = ALIGN(32);
		__image2_entry_func__ = .;
		KEEP(*(SORT(.image2.entry.data*)))
		. = ALIGN(32);
		*(.sram.only.data*)
	} > KR4_RAM_ONLY

	/* xip part */
	.xip_image2.text :
	{
		__flash_text_start__ = .;
		*(.non.dram.text*)
		__flash_text_end__ = .;
	} > KR4_IMG2_XIP

	/* space for TIMER and IDLE task stack: 4k */
	.sram_timer_idle_task_stack.bss (NOLOAD):
	{
		*port.o(*.uxIdleTaskStack*)
		*port.o(*.uxTimerTaskStack*)
	} > KR4_TIMER_IDLE_TASK_STACK

	/* sram part */
	.sram_image2.text.data ORIGIN(KR4_BD_RAM):
	{
		__sram_image2_start__ = .;

		SRAM_INPUT_TEXT_SECTION

		__sram_image2_end__ = .;
		. = ALIGN(32);
	} > KR4_BD_RAM

	/* psram part */
	.psram_image2.text.data ORIGIN(KR4_BD_PSRAM):
	{
		__psram_image2_start__ = .;

		PSRAM_INPUT_TEXT_SECTION

		__psram_image2_end__ = .;
	} > KR4_BD_PSRAM

	OUT_TEXT_SECTION :
	{
		__image2_backtrace_start__ = .;

		*(.timesensitive.text*)

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

	OUT_DATA_SECTION :
	{
		*(.timesensitive.data*)

		*(.data*)

		. = ALIGN (32);
	} > IMG2_DATA_LMA_REGION

	/* We want the small data sections together, so single-instruction offsets
		can access them all, and initialized data all before uninitialized, so
		we can shorten the on-disk segment size.  */
	OUT_DATA_SECTION :
	{
		__sdata_start__ = .;

		*(.srodata.cst16)
		*(.srodata.cst8)
		*(.srodata.cst4)
		*(.srodata.cst2)
		*(.srodata .srodata.*)
		*(.sdata .sdata.* .gnu.linkonce.s.*)

		__sdata_end__ = .;
		. = ALIGN(4);
	} > IMG2_DATA_LMA_REGION

	.ram_image2.sbss (NOLOAD):
	{
		__sbss_start__ = .;

		*(.dynsbss)
		*(.sbss)
		*(.sbss.*)
		*(.gnu.linkonce.sb.*)
		*(.scommon)

		__sbss_end__ = .;
	} > IMG2_DATA_LMA_REGION

	.ram_image2.bss (NOLOAD):
	{
		__bss_start__ = .;

		*(.bss*)
		*(COMMON)

		__bss_end__ = .;
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
	} > KR4_BD_RAM

	/* put in the end of psram part */
	.psram_heap.start (NOLOAD):
	{
		__psram_heap_buffer_start__ = .;
	} > KR4_BD_PSRAM

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

ASSERT( __boot_text_end__ == ORIGIN(KR4_BOOT_RAM_BACKUP) + LENGTH(KR4_BOOT_RAM_BACKUP), "__boot_text_end__ is changed.")
ASSERT( __sram_only_start__ == ORIGIN(KR4_RAM_ONLY), "__sram_only_start__ is changed.")
ASSERT( __flash_text_start__ == ORIGIN(KR4_IMG2_XIP), "__flash_text_start__ is changed.")
ASSERT(__sram_image2_start__ == ORIGIN(KR4_BD_RAM), "__sram_image2_start__ is changed.")

__global_pointer$ = MIN(__sdata_start__ + 0x800, __sbss_end__);

#if defined(CONFIG_DATA_HEAP_SRAM)
__bdram_heap_buffer_size__ = ORIGIN(KR4_BD_RAM) + LENGTH(KR4_BD_RAM) - __bdram_heap_buffer_start__;
#else /* CONFIG_DATA_HEAP_PSRAM */
__bdram_heap_buffer_size__ = 0;
#endif

ASSERT( __psram_image2_start__ == ORIGIN(KR4_BD_PSRAM), "__psram_image2_start__ is changed.")

#if defined(CONFIG_PSRAM_ALL_FOR_AP_HEAP)
__psram_heap_buffer_start__ = .;
__psram_heap_buffer_size__ = 0;

__psram_heap_extend_start__ = .;
__psram_heap_extend_size__ = 0;
#else
__psram_heap_buffer_size__ = ORIGIN(KR4_BD_PSRAM) + LENGTH(KR4_BD_PSRAM) - __psram_heap_buffer_start__;
__psram_heap_extend_start__ = ORIGIN(KR4_PSRAM_HEAP_EXT);
__psram_heap_extend_size__ = LENGTH(KR4_PSRAM_HEAP_EXT);
#endif

__kr4_sram_heap_extend_start__ = .;
__kr4_sram_heap_extend_size__ = 0;