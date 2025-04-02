ENTRY(Reset_Handler)

#include "../../../amebaDplus_layout.ld"


#define OUTPUT_TEXT_SECTION 		.xip_image2.text
#define IMG2_LMA_REGION 			KM0_BD_RAM_MP
#if defined(CONFIG_PSRAM_AS_HEAP)
	#define SRAM_INPUT_TEXT_SECTION
	#define PSRAM_INPUT_TEXT_SECTION	*(.sramdram.only.text*)
#else
	#define SRAM_INPUT_TEXT_SECTION		*(.sramdram.only.text*)
	#define PSRAM_INPUT_TEXT_SECTION
#endif


SECTIONS
{
	/* space for TIMER and IDLE task stack: 4k */
	.sram_timer_idle_task_stack.bss (NOLOAD):
	{
		*port.o(*.uxIdleTaskStack*)
		*port.o(*.uxTimerTaskStack*)
	} > KM0_TIMER_IDLE_TASK_STACK

	.ram_image2.entry :
	{
		/* KM0_IMG2_ENTRY move to 0x20004D20 before enable KM0 */
		__img2_entry_start__ = .;
		/* add for bootloader, because bootloader and image2 build together now.*/
		KEEP(*(SORT(.image1.entry.data*)))
		. = ALIGN(32);

		__image2_entry_func__ = .;
		KEEP(*(SORT(.image2.entry.data*)))

		__img2_entry_end__ = .;
		. = ALIGN(32);
	} > KM0_IMG2_ENTRY

	/* sram part */
	.sram_image2.text.data ORIGIN(KM0_BD_RAM_MP):
	{
		__sram_image2_start__ = .;
		SRAM_INPUT_TEXT_SECTION
		*(.sram.only.text*)
		__sram_image2_end__ = .;
		. = ALIGN(32);
		
		__flash_text_start__ = .;
		*(.non.dram.text*)
		__flash_text_end__ = .;

		*os_wrapper_static_functions.o(*.mutex_pool)
		*os_wrapper_static_functions.o(*.sema_pool)
		*os_wrapper_static_functions.o(*.timer_pool)

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
		__ipc_table_start__ = .;
		KEEP(*(*.ipc.table.data*))
		__ipc_table_end__ = .;
		. = ALIGN (4);
		__git_ver_table_start__ = .;
		KEEP(*(.git.ver.data))
		__git_ver_table_end__ = .;

		__image2_backtrace_end__ = .;
		. = ALIGN (32);
		
		
	} > KM0_BD_RAM_MP


	/* C++ .ARM.extab/.ARM.exidx address shall same as *(.text*) */
	.ARM.extab  : 
	{
		*(.ARM.extab* .gnu.linkonce.armextab.*) 

	} > KM0_BD_RAM_MP

	.ARM.exidx :
	{
		__exidx_start = .;
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
		__exidx_end = .;
	} > KM0_BD_RAM_MP

	.sram_image2.text.data :
	{
		/* add for bootloader, because bootloader and image2 build together now.*/
		*(.timesensitive.data*)
		*(.sram.only.data*)

		*(.data*)
		. = ALIGN (32);
	} > KM0_BD_RAM_MP

	.ram_image2.bss (NOLOAD):
	{
		__bss_start__ = .;

		/* add for bootloader, because bootloader and image2 build together now.*/
		*(.bss*)
		*(COMMON)

		__bss_end__ = .;
	} > KM0_BD_RAM_MP

	.ram_image2.bd.data (NOLOAD):
	{
		. = ALIGN (32);
		__ram_bd_start__ = .;
		*(.bdsram.data*)
		. = ALIGN (32);
		__ram_bd_end__ = .;
	} > KM0_BD_RAM_MP

	.ram_image2.nocache.data (NOLOAD):
	{
		. = ALIGN (32);
		__ram_nocache_start__ = .;
		*(.nocache.data*)
		. = ALIGN (32);
		__ram_nocache_end__ = .;
	} > KM0_BD_RAM_MP

	/* put in the end of sram part */
	.bdram_heap.start (NOLOAD):
	{
		__bdram_heap_buffer_start__ = .;
	} > KM0_BD_RAM_MP

	/* put in the end of psram part */
	.psram_heap.start (NOLOAD):
	{
		__psram_heap_buffer_start__ = .;
	} > KM0_BD_RAM_MP

	.ram_image2_fill :
	{
		BYTE(0x00)
		. = ALIGN(32);
	} > KM0_BD_RAM_MP_END

	.coex_trace.text :
	{
		__ctrace_start__ = .;
		*(.COEXTRACE)
		__ctrace_end__ = .;
	} > CTRACE
}


__bdram_heap_buffer_size__ = ORIGIN(KM0_BD_RAM_MP) + LENGTH(KM0_BD_RAM_MP) - __bdram_heap_buffer_start__;

__sram_bootloader_mp_start__ = ORIGIN(KM4_FLOADER_RAM_MP) + LENGTH(KM4_FLOADER_RAM_MP) - 0x10000000;
__sram_bootloader_mp_size__ = ORIGIN(KM0_BD_RAM_MP) - __sram_bootloader_mp_start__ - IMAGE_HEADER_LEN * 2;

__psram_image2_start__ = .;

__psram_heap_buffer_start__ = .;
__psram_heap_buffer_size__ = 0;

__psram_heap_extend_start__ = .;
__psram_heap_extend_size__ = 0;

