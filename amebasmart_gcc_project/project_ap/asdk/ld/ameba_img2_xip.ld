#include "../../../ameba_layout.ld"

/* Linker script to place sections and symbol values. Should be used together
 * with other linker script that defines memory regions FLASH and RAM.
 * It references following symbols, which must be defined in code:
 * _reset_init : Entry of reset handler
 *
 * It defines following symbols, which code can use without definition:
 * __exidx_start
 * __exidx_end
 * __etext
 * __data_start__
 * __preinit_array_start
 * __preinit_array_end
 * __init_array_start
 * __init_array_end
 * __fini_array_start
 * __fini_array_end
 * __data_end__
 * __bss_start__
 * __bss_end__
 * __end__
 * end
 * __HeapLimit
 * __StackLimit
 * __StackTop
 * __stack
 */
ENTRY(_vector_table)

SECTIONS
{
	/* xip part */
	.xip_image2.text ORIGIN(CA32_IMG2_XIP):
	{
		__flash_text_start__ = .;

		*(.text)		/* .text sections (code) */
		*(.text*)		/* .text* sections (code) */
		*(.rodata)		/* .rodata sections (constants, strings, etc.) */
		*(.rodata*)		/* .rodata* sections (constants, strings, etc.) */
		*(.glue_7)		/* glue arm to thumb code */
		*(.glue_7t)		/* glue thumb to arm code */
		*(.vfp11_veneer)
		*(.eh_frame)
		KEEP (*(.init))
		KEEP (*(.fini))
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
		. = ALIGN(4);
		 __etext = .;

		__flash_text_end__ = .;
	} > CA32_IMG2_XIP

	.code :
	{
		. = ALIGN(4096);
		__dram_text_start__ = .;

		. = ALIGN(0x4);
		*(.freertos_vectors)
		*(.vectors)
		*(.boot)
		*(.timesensitive.text*)
		*(.sramdram.only.text*)

		. = ALIGN(4096);
		__dram_text_end__ = .;
	} > CA32_BL3_DRAM_NS

	/* C++ .ARM.extab/.ARM.exidx address shall same as *(.text*) */
	.ARM.extab :
	{
		*(.ARM.extab* .gnu.linkonce.armextab.*)
	} > CA32_IMG2_XIP

	.ARM.exidx :
	{
		__exidx_start = .;
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
		__exidx_end = .;
	} > CA32_IMG2_XIP

	.ctors :
	{
		__CTOR_LIST__ = .;
		/* gcc uses crtbegin.o to find the start of
		   the constructors, so we make sure it is
		   first.  Because this is a wildcard, it
		   doesn't matter if the user does not
		   actually link against crtbegin.o; the
		   linker won't look for a file to match a
		   wildcard.  The wildcard also means that it
		   doesn't matter which directory crtbegin.o
		   is in.  */
		KEEP (*crtbegin.o(.ctors))
		KEEP (*crtbegin?.o(.ctors))
		/* We don't want to include the .ctor section from
		   from the crtend.o file until after the sorted ctors.
		   The .ctor section from the crtend file contains the
		   end of ctors marker and it must be last */
		KEEP (*(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors))
		KEEP (*(SORT(.ctors.*)))
		KEEP (*(.ctors))
		__CTOR_END__ = .;
	} > CA32_IMG2_XIP

	.dtors :
	{
		__DTOR_LIST__ = .;
		KEEP (*crtbegin.o(.dtors))
		KEEP (*crtbegin?.o(.dtors))
		KEEP (*(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors))
		KEEP (*(SORT(.dtors.*)))
		KEEP (*(.dtors))
		__DTOR_END__ = .;
	} > CA32_IMG2_XIP

	.preinit_array :
	{
		PROVIDE_HIDDEN (__preinit_array_start = .);
		KEEP (*(.preinit_array*))
		PROVIDE_HIDDEN (__preinit_array_end = .);
	} > CA32_IMG2_XIP

	.init_array :
	{
		PROVIDE_HIDDEN (__init_array_start = .);
		KEEP (*(SORT(.init_array.*)))
		KEEP (*(.init_array*))
		PROVIDE_HIDDEN (__init_array_end = .);
	} > CA32_IMG2_XIP

	.fini_array :
	{
		PROVIDE_HIDDEN (__fini_array_start = .);
		KEEP (*(SORT(.fini_array.*)))
		KEEP (*(.fini_array*))
		PROVIDE_HIDDEN (__fini_array_end = .);
	} > CA32_IMG2_XIP

	.data :
	{
		__data_start__ = .;
		. = ALIGN(4);
		*(.timesensitive.data*)

		*(.data*)
		*(.sramdram.only.data)
		. = ALIGN(4);

		/* All data end */
		__data_end__ = .;
	} > CA32_BL3_DRAM_NS

	.mmu_tbl (ALIGN(0x4000)) :
	{
		__mmu_tbl_start = .;
		*(.mmu_tbl)
		*(base_xlat_table)
		*(xlat_table)
		__mmu_tbl_end = .;
	} > CA32_BL3_DRAM_NS

	.bss (NOLOAD) :
	{
		__bss_start__ = .;
		*(.bss)
		*(.bss.*)
		*(COMMON)
		__bss_end__ = .;
	} > CA32_BL3_DRAM_NS

	.heap (NOLOAD) :
	{
		__end__ = .;
		end = __end__;
		__HeapBase = .;
		KEEP(*(.heap))
		. = . + 8192;
		__HeapLimit = .;
		__heap_limit = .;	/* Add for _sbrk */
		__HeapSize = __HeapLimit - __HeapBase;
	} > CA32_BL3_DRAM_NS

	.stack (NOLOAD) :
	{
		. = ALIGN(16);
		__StackLimit = .;
		KEEP(*(.stack))

		. = ALIGN(4096);
		__ram_nocache_start__ = .;
		KEEP(*(.nocache.data))
		. = ALIGN(4096);
		__ram_nocache_end__ = .;
	} > CA32_BL3_DRAM_NS

	/* put in the end of psram part */
	.psram_heap.start (NOLOAD):
	{
		. = ALIGN(64);
		__psram_heap_buffer_start__ = .;
	} > CA32_BL3_DRAM_NS

	.bluetooth_trace.text :
	{
		__btrace_start__ = .;
		*(.BTTRACE)
		__btrace_end__ = .;
	} > BTRACE

	.ARM.attributes 0 : { KEEP (*(.ARM.attributes)) KEEP (*(.gnu.attributes)) }

    /* Initialize the stack at the end of the memory block. */
    __StackTop = .;
    __stack = __StackTop;

    ASSERT(__StackLimit >= __HeapLimit, "region data overflowed with stack and heap")

    PROVIDE(__stack = __StackTop);

    _end = ALIGN(0x10);
}

ASSERT(__flash_text_start__ == ORIGIN(CA32_IMG2_XIP), "__flash_text_start__ is changed.")
ASSERT((__dram_text_start__ & 4095) == 0, "__dram_text_start__ is not 4KB aligned.")
ASSERT((__dram_text_end__ & 4095) == 0, "__dram_text_end__ is not 4KB aligned.")
ASSERT((__ram_nocache_start__ & 4095) == 0, "__ram_nocache_start__ is not 4KB aligned.")
ASSERT((__ram_nocache_end__ & 4095) == 0, "__ram_nocache_end__ is not 4KB aligned.")
__psram_heap_buffer_size__ = ORIGIN(CA32_BL3_DRAM_NS) + LENGTH(CA32_BL3_DRAM_NS) - __psram_heap_buffer_start__;

__dram_dynamic_app_text_start__ = ORIGIN(CA32_DYNAMIC_APP_LOAD_DRAM);
__dram_dynamic_app_text_end__ = ORIGIN(CA32_DYNAMIC_APP_LOAD_DRAM) + LENGTH(CA32_DYNAMIC_APP_LOAD_DRAM);
__dram_dynamic_app_text_size__ = LENGTH(CA32_DYNAMIC_APP_LOAD_DRAM);
