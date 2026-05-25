#include "../../ameba_layout.ld"


/* Decide the location of the WLAN critical code here, because release lib section cannot be modified*/
#if defined(CONFIG_WLAN_CRITICAL_CODE_IN_RAM)
	#define WLAN_CRITICAL_TEXT
	#define WLAN_SRAMDRAM_ONLY_TEXT   *(.wlan.critical.text*)
#else
	#define WLAN_CRITICAL_TEXT        *(.wlan.critical.text*)
	#define WLAN_SRAMDRAM_ONLY_TEXT
#endif

#if defined(CONFIG_IMG2_FLASH)
	#define OUT_TEXT_SECTION				.xip_image2.text
	#define IMG2_TEXT_LMA_REGION			KM4TZ_IMG2_XIP
	#define FLASH_INPUT_TEXT_SECTION		*(.non.dram.text*)
	#ifdef CONFIG_DATA_HEAP_SRAM
		#define SRAM_INPUT_TEXT_SECTION		*(.sramdram.only.text*) WLAN_SRAMDRAM_ONLY_TEXT
		#define PSRAM_INPUT_TEXT_SECTION
	#else
		#define SRAM_INPUT_TEXT_SECTION
		#define PSRAM_INPUT_TEXT_SECTION	*(.sramdram.only.text*) WLAN_SRAMDRAM_ONLY_TEXT
	#endif
#elif defined(CONFIG_IMG2_SRAM)
	#define OUT_TEXT_SECTION				.sram_image2.text.data
	#define IMG2_TEXT_LMA_REGION			KM4TZ_BD_RAM
	#define FLASH_INPUT_TEXT_SECTION
	#define SRAM_INPUT_TEXT_SECTION			*(.non.dram.text*) *(.sramdram.only.text*) WLAN_SRAMDRAM_ONLY_TEXT
	#define PSRAM_INPUT_TEXT_SECTION

	#ifndef CONFIG_DATA_HEAP_SRAM
		#error "CONFIG_IMG2_SRAM requires CONFIG_DATA_HEAP_SRAM to be enabled"
	#endif
#else /*CONFIG_IMG2_PSRAM*/
	#define OUT_TEXT_SECTION				.psram_image2.text.data
	#define IMG2_TEXT_LMA_REGION			KM4TZ_BD_PSRAM
	#define FLASH_INPUT_TEXT_SECTION		/* Nand do not support XIP */
	#define SRAM_INPUT_TEXT_SECTION			*(.non.dram.text*)
	#define PSRAM_INPUT_TEXT_SECTION		*(.sramdram.only.text*) WLAN_SRAMDRAM_ONLY_TEXT
#endif

#ifdef CONFIG_IMG2_PSRAM
	#define ARM_EXCEPTION_SECTION		KM4TZ_BD_PSRAM
#else
	#define ARM_EXCEPTION_SECTION		KM4TZ_BD_RAM
#endif

#if defined(CONFIG_DATA_HEAP_SRAM)
	#define OUT_DATA_SECTION			.sram_image2.text.data
	#define IMG2_DATA_LMA_REGION		KM4TZ_BD_RAM
#else /* CONFIG_DATA_HEAP_PSRAM */
	#define OUT_DATA_SECTION			.psram_image2.text.data
	#define IMG2_DATA_LMA_REGION		KM4TZ_BD_PSRAM
#endif

SECTIONS
{
	/* xip part */
	.xip_image2.text :
	{
		__flash_text_start__ = .;
		FLASH_INPUT_TEXT_SECTION
		*(.flash.only.text*)
		__flash_text_end__ = .;
	} > KM4TZ_IMG2_XIP

	/* rom lib part */
	.romlib.bss :
	{
		/* the first 0x70 byte are used to store rom_s/rom_ns common bss */
		. = 0x3c;
		__romlib_bss_start__ = .;
		KEEP(*log.o(.bss.rtk_log_tag_array))
		KEEP(*log.o(.bss.rtk_log_entry_count))
		KEEP(*log.o(.bss.rtk_log_default_level))
		KEEP(*ameba_shell_rom.o(.bss.shell_ctl))
		KEEP(*ameba_shell_rom.o(.bss.shell_argv_array))
		KEEP(*ameba_shell_rom.o(.bss.shell_buf))
		KEEP(*ameba_shell_rom.o(.bss.shell_buf_array))
		KEEP(*ameba_shell_rom.o(.bss.shell_rxbuf))
		KEEP(*ameba_shell_rom.o(.bss.shell_rxbuf_array))
		KEEP(*ameba_dump_stack.o(.bss.crash_dump_depth))
		__romlib_bss_end__ = .;
	} >  KM4TZ_ROMBSS_RAM_NS

	/* sram part */
	.sram_image2.text.data ORIGIN(KM4TZ_BD_RAM):
	{
		__sram_image2_start__ = .;
		/* fixed location. do not move to other location */
		__image2_entry_func__ = .;
		KEEP(*(SORT(.image2.entry.data*)))
		__image2_validate_code__ = .;
		KEEP(*(.image2.validate.rodata*))
		. = ALIGN(32);

		SRAM_INPUT_TEXT_SECTION
		*(.sram.only.text*)
		*(.sram.only.data*)
		__sram_image2_end__ = .;
	} > KM4TZ_BD_RAM

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

		*(.image2.net.ram.text*)
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

#if !(!defined (CONFIG_WHC_INTF_IPC) && defined (CONFIG_WHC_DEV))
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
#endif
		__image2_backtrace_end__ = .;

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
		. = ALIGN(32);
		*(.data*)

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
		*(.nocache.data*)
		. = ALIGN (32);
		__ram_nocache_end__ = .;
	} > IMG2_DATA_LMA_REGION

	/* put in the end of sram part */
	.bdram_heap.start (NOLOAD):
	{
		__bdram_heap_buffer_start__ = .;
	} > KM4TZ_BD_RAM


	/* put in the end of psram part */
	.psram_heap.start (NOLOAD):
	{
		__psram_heap_buffer_start__ = .;
	} > KM4TZ_BD_PSRAM

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

ASSERT(__image2_entry_func__ == ORIGIN(KM4TZ_BD_RAM), "__image2_entry_func__ is not KM4TZ_BD_RAM or FULLMAC_BD_RAM.")
ASSERT(__sram_image2_start__ == ORIGIN(KM4TZ_BD_RAM), "__sram_image2_start__ is changed.")
ASSERT(__flash_text_start__ == ORIGIN(KM4TZ_IMG2_XIP), "__flash_text_start__ is changed.")
ASSERT(__psram_image2_start__ == ORIGIN(KM4TZ_BD_PSRAM), "__psram_image2_start__ is changed.")
#if defined(CONFIG_TRUSTZONE_EN)
ASSERT(__romlib_bss_end__ == 0x20001220, "__romlib_bss_end__ is changed. Common bss of rom_lib and rom_tz may be modified illegally.")
#else
ASSERT(__romlib_bss_end__ == 0x2000103c, "__romlib_bss_end__ is changed. Common bss of rom_lib and rom_tz may be modified illegally.")
#endif



#if defined(CONFIG_DATA_HEAP_SRAM)
	#if (!defined (CONFIG_WHC_INTF_IPC) && defined (CONFIG_WHC_DEV))
		//ASSERT(__ipc_table_start__ == __ipc_table_end__, "fullmac does not require ipc table!")

		__bdram_heap_buffer_size__ = SRAM_END - __bdram_heap_buffer_start__;
		__tcm_heap_buffer_start__ = ORIGIN(FULLMAC_TCM_RAM);
		__tcm_heap_buffer_size__ = LENGTH(FULLMAC_TCM_RAM);
	#else
		__bdram_heap_buffer_size__ = ORIGIN(KM4TZ_BD_RAM) + LENGTH(KM4TZ_BD_RAM) - __bdram_heap_buffer_start__;
	#endif
#else /* CONFIG_DATA_HEAP_PSRAM */
	__bdram_heap_buffer_size__ = 0;
#endif

#if defined(CONFIG_PSRAM_ALL_FOR_AP_HEAP)
__psram_heap_buffer_start__ = .;
__psram_heap_buffer_size__ = 0;

__psram_heap_extend_start__ = .;
__psram_heap_extend_size__ = 0;

#else
__psram_heap_buffer_size__ = ORIGIN(KM4TZ_BD_PSRAM) + LENGTH(KM4TZ_BD_PSRAM) - __psram_heap_buffer_start__;

__psram_heap_extend_start__ = ORIGIN(KM4TZ_PSRAM_HEAP_EXT);
__psram_heap_extend_size__ = LENGTH(KM4TZ_PSRAM_HEAP_EXT);
#endif
