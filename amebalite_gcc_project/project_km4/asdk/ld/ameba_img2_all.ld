#include "../../../amebalite_layout.ld"

#if defined(CONFIG_CODE_XIP_DATA_SRAM)
#define OUT_TEXT_SECTION			.xip_image2.text
#define OUT_DATA_SECTION			.sram_image2.text.data
#define IMG2_TEXT_LMA_REGION		KM4_IMG2_XIP
#define IMG2_DATA_LMA_REGION		KM4_BD_RAM
#define SRAM_INPUT_TEXT_SECTION		*(.sramdram.only.text*)	
#define PSRAM_INPUT_TEXT_SECTION
#elif defined(CONFIG_CODE_XIP_DATA_PSRAM)
#define OUT_TEXT_SECTION			.xip_image2.text
#define OUT_DATA_SECTION			.psram_image2.text.data
#define IMG2_TEXT_LMA_REGION		KM4_IMG2_XIP
#define IMG2_DATA_LMA_REGION		KM4_BD_PSRAM
#define SRAM_INPUT_TEXT_SECTION		
#define PSRAM_INPUT_TEXT_SECTION	*(.sramdram.only.text*)	
#elif defined(CONFIG_CODE_PSRAM_DATA_SRAM)
#define OUT_TEXT_SECTION			.psram_image2.text.data
#define OUT_DATA_SECTION			.sram_image2.text.data
#define IMG2_TEXT_LMA_REGION		KM4_BD_PSRAM
#define IMG2_DATA_LMA_REGION		KM4_BD_RAM
#define SRAM_INPUT_TEXT_SECTION		
#define PSRAM_INPUT_TEXT_SECTION	*(.sramdram.only.text*)
#else
#define OUT_TEXT_SECTION			.psram_image2.text.data
#define OUT_DATA_SECTION			.psram_image2.text.data
#define IMG2_TEXT_LMA_REGION		KM4_BD_PSRAM
#define IMG2_DATA_LMA_REGION		KM4_BD_PSRAM
#define SRAM_INPUT_TEXT_SECTION
#define PSRAM_INPUT_TEXT_SECTION	*(.sramdram.only.text*)
#endif

#if defined(CONFIG_DSP_EN) && (defined(CONFIG_CODE_XIP_DATA_SRAM) || defined(CONFIG_CODE_PSRAM_DATA_SRAM))
#error When DSP is turned on, DSP has exclusive access to SRAM by default. Please modify the running location of IMG2 in menuconfig or modify the default running location of DSP.
#endif

SECTIONS
{
	/* fixed region. do not add any code to this region */
	.sram_only.text.data :
	{
		__sram_only_start__ = .;
		*(.sram.only.text*)
		. = ALIGN(32);

		*(.sram.only.data*)
	} > KM4_RAM_ONLY

	/* fixed region. do not add any code to this region */
	.boot.entry :
	{
		__boot_text_start__ = .;
		__image2_entry_func__ = .;
		KEEP(*(SORT(.image2.entry.data*)))
		__boot_text_end__ = .;
		. = ALIGN(32);
	} > KM4_BOOT_RAM

	/* xip part */
	.xip_image2.text :
	{
		__flash_text_start__ = .;
		*(.non.dram.text*)
		__flash_text_end__ = .;
	} > KM4_IMG2_XIP

	/* space for TIMER and IDLE task stack: 4k */
	.sram_timer_idle_task_stack.bss (NOLOAD):
	{
		*port.o(*.uxIdleTaskStack*)
		*port.o(*.uxTimerTaskStack*)
	} > KM4_TIMER_IDLE_TASK_STACK

	/* sram part */
	.sram_image2.text.data ORIGIN(KM4_BD_RAM):
	{
		__sram_image2_start__ = .;
		SRAM_INPUT_TEXT_SECTION
		__sram_image2_end__ = .;
		. = ALIGN(32);
	} > KM4_BD_RAM

	/* image2: normal image start */
	.psram_image2.text.data ORIGIN(KM4_BD_PSRAM):
	{
		__psram_image2_start__ = .;

		PSRAM_INPUT_TEXT_SECTION

		__psram_image2_end__ = .;
	} > KM4_BD_PSRAM

	/* text section */
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

	/* sram/psram part */
	OUT_DATA_SECTION :
	{
		*(.timesensitive.data*)
		*(.data*)

		/*cache line size aligned in order to avoid MMU remap err*/
		. = ALIGN(32);
	} > IMG2_DATA_LMA_REGION

	/*-----------------*/
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
		*(.ncsram.data*)
		. = ALIGN (32);
		__ram_nocache_end__ = .;
	} > IMG2_DATA_LMA_REGION

	/* put in the end of sram part */
	.bdram_heap.start (NOLOAD):
	{
		__bdram_heap_buffer_start__ = .;
	} > KM4_BD_RAM

	/* put in the end of psram part */
	.psram_heap.start (NOLOAD):
	{
		__psram_heap_buffer_start__ = .;
	} > KM4_BD_PSRAM

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

ASSERT(__sram_only_start__ == ORIGIN(KM4_RAM_ONLY), "__sram_only_start__ is changed.")
ASSERT(__flash_text_start__ == ORIGIN(KM4_IMG2_XIP), "__flash_text_start__ is changed.")
ASSERT(__sram_image2_start__ == ORIGIN(KM4_BD_RAM), "__sram_image2_start__ is changed.")
ASSERT(__image2_entry_func__ == ORIGIN(KM4_BOOT_RAM), "__image2_entry_func__ is not KM4_BOOT_RAM.")


#if defined(CONFIG_CODE_XIP_DATA_SRAM)
__bdram_heap_buffer_size__ = ORIGIN(KM4_BD_RAM) + LENGTH(KM4_BD_RAM) - __bdram_heap_buffer_start__;

__psram_image2_start__ = .;

__psram_heap_buffer_start__ = .;
__psram_heap_buffer_size__ = 0;

__psram_heap_extend_start__ = .;
__psram_heap_extend_size__ = 0;
#elif defined(CONFIG_CODE_XIP_DATA_PSRAM)
__km4_sram_heap_extend_size__ = 0;
__bdram_heap_buffer_size__ = 0;
ASSERT(__psram_image2_start__ == ORIGIN(KM4_BD_PSRAM), "__psram_image2_start__ is changed.")
__psram_heap_buffer_size__ = ORIGIN(KM4_BD_PSRAM) + LENGTH(KM4_BD_PSRAM) - __psram_heap_buffer_start__;

__psram_heap_extend_start__ = ORIGIN(KM4_PSRAM_HEAP_EXT);
__psram_heap_extend_size__ = LENGTH(KM4_PSRAM_HEAP_EXT);

#elif defined(CONFIG_CODE_PSRAM_DATA_SRAM)
__bdram_heap_buffer_size__ = ORIGIN(KM4_BD_RAM) + LENGTH(KM4_BD_RAM) - __bdram_heap_buffer_start__;

ASSERT(__psram_image2_start__ == ORIGIN(KM4_BD_PSRAM), "__psram_image2_start__ is changed.")
__psram_heap_buffer_size__ = ORIGIN(KM4_BD_PSRAM) + LENGTH(KM4_BD_PSRAM) - __psram_heap_buffer_start__;

__psram_heap_extend_start__ = ORIGIN(KM4_PSRAM_HEAP_EXT);
__psram_heap_extend_size__ = LENGTH(KM4_PSRAM_HEAP_EXT);

#else
__km4_sram_heap_extend_size__ = 0;
__bdram_heap_buffer_size__ = 0;

ASSERT(__psram_image2_start__ == ORIGIN(KM4_BD_PSRAM), "__psram_image2_start__ is changed.")
__psram_heap_buffer_size__ = ORIGIN(KM4_BD_PSRAM) + LENGTH(KM4_BD_PSRAM) - __psram_heap_buffer_start__;

__psram_heap_extend_start__ = ORIGIN(KM4_PSRAM_HEAP_EXT);
__psram_heap_extend_size__ = LENGTH(KM4_PSRAM_HEAP_EXT);
#endif