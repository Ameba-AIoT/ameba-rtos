/*
 * Copyright (c) 2013-2020, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <common/bl_common.ld.h>
#include <lib/xlat_tables/xlat_tables_defs.h>

OUTPUT_FORMAT(PLATFORM_LINKER_FORMAT)
OUTPUT_ARCH(PLATFORM_LINKER_ARCH)
ENTRY(bl2_entrypoint)

MEMORY {
#ifdef CONFIG_CP_TEST_CA32
    ROM (rwx): ORIGIN = BL2_RO_BASE, LENGTH = BL2_RO_LIMIT - BL2_RO_BASE
    RAM (rwx): ORIGIN = BL2_RW_BASE, LENGTH = BL2_RW_LIMIT - BL2_RW_BASE
#else
    RAM (rwx): ORIGIN = BL2_BASE, LENGTH = BL2_LIMIT - BL2_BASE
#endif
}

#ifdef CONFIG_CP_TEST_CA32
#define BL2_ROM ROM
#define BL2_RAM RAM
#else
#define BL2_ROM RAM
#define BL2_RAM RAM
#endif

SECTIONS
{
    . = ORIGIN(BL2_ROM);
    ASSERT(. == ALIGN(PAGE_SIZE),
           "BL2_BASE address is not aligned on a page boundary.")

#if SEPARATE_CODE_AND_RODATA
    .text . : {
        __TEXT_START__ = .;
        *bl2_entrypoint.o(.text*)
        *(SORT_BY_ALIGNMENT(.text*))
        *(.vectors)
        . = ALIGN(PAGE_SIZE);
        __TEXT_END__ = .;
     } >BL2_ROM

     /* .ARM.extab and .ARM.exidx are only added because Clang need them */
     .ARM.extab . : {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
     } >BL2_ROM

     .ARM.exidx . : {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
     } >BL2_ROM

    .rodata . : {
        __RODATA_START__ = .;
        *(SORT_BY_ALIGNMENT(.rodata*))

	RODATA_COMMON

        . = ALIGN(PAGE_SIZE);
        __RODATA_END__ = .;
    } >BL2_ROM
#else
    ro . : {
        __RO_START__ = .;
        *bl2_entrypoint.o(.text*)
        *(SORT_BY_ALIGNMENT(.text*))
        *(SORT_BY_ALIGNMENT(.rodata*))

	RODATA_COMMON

        *(.vectors)
        __RO_END_UNALIGNED__ = .;
        /*
         * Memory page(s) mapped to this section will be marked as
         * read-only, executable.  No RW data from the next section must
         * creep in.  Ensure the rest of the current memory page is unused.
         */
        . = ALIGN(PAGE_SIZE);
        __RO_END__ = .;
    } >BL2_ROM
#endif

#ifdef CONFIG_CP_TEST_CA32
    . = BL2_RW_BASE;
    ASSERT(BL2_RW_BASE == ALIGN(PAGE_SIZE),
           "BL2_RW_BASE address is not aligned on a page boundary.")
#endif
    /*
     * Define a linker symbol to mark start of the RW memory area for this
     * image.
     */
    __RW_START__ = . ;

    /*
     * .data must be placed at a lower address than the stacks if the stack
     * protector is enabled. Alternatively, the .data.stack_protector_canary
     * section can be placed independently of the main .data section.
     */
    .data . : {
        __DATA_RAM_START__ = .;
        *(SORT_BY_ALIGNMENT(.data*))
        __DATA_RAM_END__ = .;
#ifdef CONFIG_CP_TEST_CA32
    } >RAM AT>ROM

    __DATA_ROM_START__ = LOADADDR(.data);
    __DATA_SIZE__ = SIZEOF(.data);
    /*
     * The .data section is the last PROGBITS section so its end marks the end
     * of BL2's actual content in Trusted ROM.
     */
    __BL2_ROM_END__ =  __DATA_ROM_START__ + __DATA_SIZE__;
    ASSERT(__BL2_ROM_END__ <= BL2_RO_LIMIT,
           "BL2's ROM content has exceeded its limit.")
#else
    } >RAM
#endif /* CONFIG_CP_TEST_CA32 */

    stacks (NOLOAD) : {
        __STACKS_START__ = .;
        *(tzfw_normal_stacks)
        __STACKS_END__ = .;
    } >RAM

    BSS_SECTION >RAM
    XLAT_TABLE_SECTION >RAM

#if USE_COHERENT_MEM
    /*
     * The base address of the coherent memory section must be page-aligned (4K)
     * to guarantee that the coherent data are stored on their own pages and
     * are not mixed with normal data.  This is required to set up the correct
     * memory attributes for the coherent data page tables.
     */
    coherent_ram (NOLOAD) : ALIGN(PAGE_SIZE) {
        __COHERENT_RAM_START__ = .;
        *(tzfw_coherent_mem)
        __COHERENT_RAM_END_UNALIGNED__ = .;
        /*
         * Memory page(s) mapped to this section will be marked
         * as device memory.  No other unexpected data must creep in.
         * Ensure the rest of the current memory page is unused.
         */
        . = ALIGN(PAGE_SIZE);
        __COHERENT_RAM_END__ = .;
    } >RAM
#endif

    /*
     * Define a linker symbol to mark end of the RW memory area for this
     * image.
     */
    __RW_END__ = .;
    __BL2_END__ = .;

    __BSS_SIZE__ = SIZEOF(.bss);

#if USE_COHERENT_MEM
    __COHERENT_RAM_UNALIGNED_SIZE__ =
        __COHERENT_RAM_END_UNALIGNED__ - __COHERENT_RAM_START__;
#endif

    ASSERT(. <= BL2_LIMIT, "BL2 image has exceeded its limit.")
}
