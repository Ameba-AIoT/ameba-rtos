#include "../../ameba_layout.ld"

/* Decide the location of the WLAN critical code here, because release lib section cannot be modified*/
#if defined(CONFIG_WLAN_CRITICAL_CODE_IN_RAM)
	#define WLAN_CRITICAL_TEXT
	#define WLAN_SRAMDRAM_ONLY_TEXT   *(.wlan.critical.text*) *(.lwip.cache.efficient.wlan.critical.text*)
#else
	#define WLAN_CRITICAL_TEXT        *(.wlan.critical.text*) *(.lwip.cache.efficient.wlan.critical.text*)
	#define WLAN_SRAMDRAM_ONLY_TEXT
#endif

#define OUT_TEXT_SECTION				.sram_image2.text.data
#define IMG2_TEXT_LMA_REGION		 	KM4TZ_BD_RAM_MP
#define FLASH_INPUT_TEXT_SECTION		*(.non.dram.text*)
#ifdef CONFIG_DATA_HEAP_SRAM
	#define SRAM_INPUT_TEXT_SECTION		*(.sramdram.only.text*) WLAN_SRAMDRAM_ONLY_TEXT
	#define PSRAM_INPUT_TEXT_SECTION
#else
	#define SRAM_INPUT_TEXT_SECTION
	#define PSRAM_INPUT_TEXT_SECTION	*(.sramdram.only.text*) WLAN_SRAMDRAM_ONLY_TEXT
#endif

#define ARM_EXCEPTION_SECTION		 KM4TZ_BD_RAM_MP

#define OUT_DATA_SECTION				.sram_image2.text.data
#define IMG2_DATA_LMA_REGION		 KM4TZ_BD_RAM_MP

SECTIONS
{
	/* xip part */
	.xip_image2.text :
	{
		__flash_text_start__ = .;
		__flash_text_end__ = .;
	} > KM4TZ_IMG2_XIP

	.sram_image2.entry ORIGIN(KM4TZ_BD_RAM):
	{
		/* fixed location. do not move to other location */
		__image2_entry_func__ = .;
		KEEP(*(SORT(.image2.entry.data*)))
		. = ALIGN(32);
	} > KM4TZ_BD_RAM

	/* sram part */
	.sram_image2.text.data ORIGIN(KM4TZ_BD_RAM_MP):
	{
		__sram_image2_start__ = .;
		__image2_validate_code__ = .;
		KEEP(*(.image2.validate.rodata*))
		. = ALIGN(32);

		SRAM_INPUT_TEXT_SECTION
		*(.sram.only.text*)
		*(.sram.only.data*)
		__sram_image2_end__ = .;
		. = ALIGN(32);
		
		FLASH_INPUT_TEXT_SECTION
		*(.xip.only.text*)
		*(.xip.only.rodata*)
		. = ALIGN(32);
	} > KM4TZ_BD_RAM_MP

	/* image2: normal image start */
	.psram_image2.text.data ORIGIN(KM4TZ_BD_PSRAM):
	{
		__psram_image2_start__ = .;

		PSRAM_INPUT_TEXT_SECTION

		__psram_image2_end__ = .;
	} > KM4TZ_BD_PSRAM

	/* text section */
	OUT_TEXT_SECTION :
	{
		__image2_backtrace_start__ = .;

		*(.img2_custom_signature*)
		*(.image2.net.ram.text*)

		/* fix lwip text section to a high cache hit area, start at 0xXXXXX900, reduce cache conflicts between LwIP and ROM code*/
		. = ALIGN (0x100) + 0x200;

		*(.lwip.cache.efficient.text*)
		*(.lwip.cache.efficient.rodata*)

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
		. = ALIGN (4);
		__unity_table_start__ = .;
		KEEP(*(.unity.table.rodata*))
		__unity_table_end__ = .;

		/* Add This for C++ support */
		. = ALIGN(4);
		__c_plus_plus_support_start__ = .;
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
		__c_plus_plus_support_end__ = .;
		/*-----------------*/
		__image2_backtrace_end__ = .;

		. = ALIGN (32);
	} > IMG2_TEXT_LMA_REGION

	/* C++ .ARM.extab/.ARM.exidx address shall same as *(.text*) */
	.ARM.extab  :
	{
		*(.ARM.extab* .gnu.linkonce.armextab.*)

	} > ARM_EXCEPTION_SECTION

	.ARM.exidx :
	{
		__exidx_start = .;
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
		__exidx_end = .;
	} > ARM_EXCEPTION_SECTION

	OUT_DATA_SECTION :
	{
		*(.lwip.cache.efficient.data*)
		*(.data*)

		/*cache line size aligned in order to avoid MMU remap err*/
		. = ALIGN(32);
	} > IMG2_DATA_LMA_REGION

	/*-----------------*/
	.ram_image2.bss (NOLOAD):
	{
		__bss_start__ = .;
		*(.lwip.cache.efficient.bss*)
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
	} > KM4TZ_BD_RAM_MP

	/* put in the end of psram part */
	.psram_heap.start (NOLOAD):
	{
		__psram_heap_buffer_start__ = .;
	} > KM4TZ_BD_PSRAM

	.ram_image2_fill :
	{
		BYTE(0x00)
		. = ALIGN(32);
	} > KM4TZ_BD_RAM_MP_END

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

ASSERT(__image2_entry_func__ == ORIGIN(KM4TZ_BD_RAM), "__image2_entry_func__ is not KM4TZ_BD_RAM.")
ASSERT(__sram_image2_start__ == ORIGIN(KM4TZ_BD_RAM_MP), "__sram_image2_start__ is changed.")
ASSERT(__psram_image2_start__ == ORIGIN(KM4TZ_BD_PSRAM), "__psram_image2_start__ is changed.")

//ASSERT(__ipc_table_start__ == __ipc_table_end__, "fullmac does not require ipc table!")
//ASSERT(__c_plus_plus_support_start__ == __c_plus_plus_support_end__, "fullmac does not support C++!")

__bdram_heap_buffer_size__ = SRAM_END - __bdram_heap_buffer_start__;
__tcm_heap_buffer_start__ = ORIGIN(FULLMAC_TCM_RAM);
__tcm_heap_buffer_size__ = LENGTH(FULLMAC_TCM_RAM);
// remember heap: [__retrieve_ap_buffer_start__, __retrieve_ap_buffer_size__)


__psram_heap_buffer_start__ = .;
__psram_heap_buffer_size__ = 0;

