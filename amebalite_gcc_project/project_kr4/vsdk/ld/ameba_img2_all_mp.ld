ENTRY(Reset_Handler)

#include "../../../ameba_layout.ld"

#define OUT_TEXT_SECTION			.xip_image2.text
#define OUT_DATA_SECTION			.sram_image2.text.data
#define IMG2_TEXT_LMA_REGION		KR4_BD_RAM_MP
#define IMG2_DATA_LMA_REGION		KR4_BD_RAM_MP
#define SRAM_INPUT_TEXT_SECTION		*(.sramdram.only.text*)
#define PSRAM_INPUT_TEXT_SECTION

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

		__image2_entry_func__ = .;
		KEEP(*(SORT(.image2.entry.data*)))
		. = ALIGN(32);
	} > KR4_RAM_ONLY

	/* space for TIMER and IDLE task stack: 4k */
	.sram_timer_idle_task_stack.bss (NOLOAD):
	{
		*port.o(*.uxIdleTaskStack*)
		*port.o(*.uxTimerTaskStack*)
	} > KR4_TIMER_IDLE_TASK_STACK

	/* sram part */
	.sram_image2.text.data :
	{
		__sram_image2_start__ = .;

		SRAM_INPUT_TEXT_SECTION

		__sram_image2_end__ = .;
		. = ALIGN(32);

		*(.sram.only.text*)
		. = ALIGN(32);
		*(.sram.only.data*)
		. = ALIGN(32);

		__flash_text_start__ = .;
		*(.non.dram.text*)
		__flash_text_end__ = .;

		__psram_image2_start__ = .;

		PSRAM_INPUT_TEXT_SECTION

		__psram_image2_end__ = .;

		__image2_backtrace_start__ = .;

		*(.timesensitive.text*)

		*(.text*)
		*(.rodata*)

		. = ALIGN (4);
		__cmd_table_start__ = .;
		KEEP(*(.cmd.table.data*))
		__cmd_table_end__ = .;
		. = ALIGN (4);
		__atcmd_table_start__ = .;
		KEEP(*(.atcmd.table.rodata*))
		__atcmd_table_end__ = .;
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
	} > KR4_BD_RAM_MP

	/* put in the end of psram part */
	.psram_heap.start (NOLOAD):
	{
		__psram_heap_buffer_start__ = .;
	} > KR4_BD_RAM_MP

	.ram_image2_fill :
	{
		BYTE(0x00)
		. = ALIGN(32);
	} > KR4_BD_RAM_MP_END

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

__global_pointer$ = MIN(__sdata_start__ + 0x800, __sbss_end__);

__bdram_heap_buffer_size__ = ORIGIN(KR4_BD_RAM_MP) + LENGTH(KR4_BD_RAM_MP) - __bdram_heap_buffer_start__;

/* cert + mainfest = 8k, and secure to ns addr */
__sram_bootloader_mp_start__ = ORIGIN(KM4_FLOADER_RAM_MP) + LENGTH(KM4_FLOADER_RAM_MP) - 0x10000000;
__sram_bootloader_mp_size__ = ORIGIN(KR4_BD_RAM_MP) - __sram_bootloader_mp_start__ - IMAGE_HEADER_LEN * 3;

__psram_image2_start__ = .;

__psram_heap_buffer_start__ = .;
__psram_heap_buffer_size__ = 0;

__psram_heap_extend_start__ = .;
__psram_heap_extend_size__ = 0;

__kr4_sram_heap_extend_start__ = .;
__kr4_sram_heap_extend_size__ = 0;