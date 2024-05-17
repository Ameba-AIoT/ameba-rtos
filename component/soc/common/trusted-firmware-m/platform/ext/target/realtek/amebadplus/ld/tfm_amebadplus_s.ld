/* auto gen, please change AmebaDplus_memory_layout.xlsx and exec gen_memory_layout.py */
MEMORY
{
	EMPTY_SECTION (rwx) :                   ORIGIN = 0x00000000, LENGTH = 0x00000000 - 0x00000000	/* This region shall be empty */

	/* KM4 ROM */
	KM4_IROM (rx) :                         ORIGIN = 0x00000000, LENGTH = 0x0001E000 - 0x00000000	/* KM4 IROM_S: 120k */
	KM4_IROM_NS (rx) :                      ORIGIN = 0x0001E000, LENGTH = 0x00033000 - 0x0001E000	/* KM4 IROM_NS: 84k */
	KM4_IROM_STDLIB_NS (rx) :               ORIGIN = 0x00033000, LENGTH = 0x00042000 - 0x00033000	/* KM4 STDLIB ROM: 60k */
	KM4_DROM_NS (rx) :                      ORIGIN = 0x00042000, LENGTH = 0x00044800 - 0x00042000	/* KM4 DROM_NS: 10k */
	KM4_DROM (rx) :                         ORIGIN = 0x00044800, LENGTH = 0x00048000 - 0x00044800	/* KM4 DROM_S: 14k */

	/* KM4 Secure RAM, BIT(28)=1 */
	KM4_ROMBSS_RAM_S (rw) :                 ORIGIN = 0x30007000, LENGTH = 0x30008000 - 0x30007000	/* KM4 ROM BSS RAM S: 4k */
	KM4_MSP_RAM_S (rw) :                    ORIGIN = 0x30008000, LENGTH = 0x3000A000 - 0x30008000	/* KM4 MSP_S RAM: 8k */
	KM4_BOOTLOADER_RAM_S (rwx) :            ORIGIN = 0x3000A020, LENGTH = 0x30010000 - 0x3000A020	/* KM4 BOOT Loader RAM: 24k */
	KM4_FLOADER_RAM_S (rwx) :               ORIGIN = 0x3000A020, LENGTH = 0x30020000 - 0x3000A020	/* KM4 ImgTool Flash Loader RAM: 88k */

	/* KM4 Non-secure RAM */
	KM4_MSP_RAM_NS (rw) :                   ORIGIN = 0x20003000, LENGTH = 0x20004000 - 0x20003000	/* KM4  MSP_NS RAM: 4k */
	KM4_ROMBSS_RAM_COM (rw) :               ORIGIN = 0x20004000, LENGTH = 0x20005000 - 0x20004000	/* KM4  ROM BSS COMMON(S & NS both used) RAM: 4k */
	KM4_ROMBSS_RAM_NS (rw) :                ORIGIN = 0x20005000, LENGTH = 0x20006000 - 0x20005000	/* KM4 ROM BSS NS RAM: 4k */
	KM4_STDLIB_HEAP_RAM_NS (rw) :           ORIGIN = 0x20006000, LENGTH = 0x20007000 - 0x20006000	/* KM4 ROM STDLIB HEAP: 4k */

	/* PMC RAM, fixed in memory layout, do not changed */
	KM0_IMG2_ENTRY (rwx) :                  ORIGIN = 0x20004D20, LENGTH = 0x20004D80 - 0x20004D20	/* KM0 IMG2 Entry(ROM Used): 128B */
	KM4_IMG2_ENTRY (rwx) :                  ORIGIN = 0x20004DA0, LENGTH = 0x20004DC0 - 0x20004DA0	/* KM4 IMG2 Entry: 64B */
	KM0_IPC_RAM (rwx) :                     ORIGIN = 0x20004E00, LENGTH = 0x20005000 - 0x20004E20	/* KM0 IPC RAM: 512B */

	/* IMG2 RAM */
	KM4_BD_RAM (rwx) :                      ORIGIN = 0x20010020, LENGTH = 0x20030000 - 0x20010020	/* KM4 BD RAM: 192k */
	KM0_BD_RAM (rwx) :                      ORIGIN = 0x20030020, LENGTH = 0x20050000 - 0x20030020	/* KM0 BD RAM: 192k */
	KM4_BD_RAM_TZ_NSC (rwx) :               ORIGIN = 0x20050020, LENGTH = 0x20050100 - 0x20050020	/* KM4 BD RAM NSC: 4k */
	KM4_BD_RAM_TZ_ENTRY (rwx) :             ORIGIN = 0x20050100, LENGTH = 0x20051000 - 0x20050100	/* KM4 BD RAM ENTRY: 16k */
	KM4_BD_RAM_TZ_S (rwx) :                 ORIGIN = 0x30051020, LENGTH = 0x30080000 - 0x30051020	/* KM4 BD RAM S: 44k */

	/* KM4 Non-secure PSRAM */
	KM4_BD_PSRAM (rwx) :                    ORIGIN = 0x60000020, LENGTH = 0x6025B000 - 0x60000020	/* KM4 BD PSRAM : 2412k */
	KM0_BD_PSRAM (rwx) :                    ORIGIN = 0x60280020, LENGTH = 0x60400000 - 0x60280020	/* KM0 BD PSRAM: 1536k */

	KM4_BD_PSRAM_TZ_NSC (rwx) :             ORIGIN = 0x6025B020, LENGTH = 0x6025C000 - 0x6025B020	/* KM4 BD RAM NSC: 4k */
	KM4_BD_PSRAM_TZ_ENTRY (rwx) :           ORIGIN = 0x6025C000, LENGTH = 0x60260000 - 0x6025C000	/* KM4 BD RAM ENTRY: 16k */
	KM4_BD_PSRAM_TZ_S (rwx) :               ORIGIN = 0x70260020, LENGTH = 0x70280000 - 0x70260020	/* KM4 BD RAM S: 128k */

	KM4_PSRAM_HEAP_EXT (rwx) :              ORIGIN = 0x6FFFFFFF, LENGTH = 0x6FFFFFFF - 0x6FFFFFFF	/* KM4 PSRAM HEAP EXT */
	KM0_PSRAM_HEAP_EXT (rwx) :              ORIGIN = 0x6FFFFFFF, LENGTH = 0x6FFFFFFF - 0x6FFFFFFF	/* KM0 PSRAM HEAP EXT */

	/* Retention RAM */
	RETENTION_RAM (rwx) :                   ORIGIN = 0x41008E00, LENGTH = 0x41009000 - 0x41008E00	/* KM0 Retention SRAM: 512B: 0k */

	/* KM0 ROM */
	KM0_IROM (rx) :                         ORIGIN = 0x00000000, LENGTH = 0x00006000 - 0x00000000	/* KM0 IROM: 24k */
	KM0_IROM_STDLIB (rx) :                  ORIGIN = 0x00006000, LENGTH = 0x0000A000 - 0x00006000	/* KM0 STDLIB ROM: 16k */
	KM0_DROM (rx) :                         ORIGIN = 0x0000A000, LENGTH = 0x0000B000 - 0x0000A000 	/* KM0 DROM: 4k */
	WIFI_ROM (rx) :                         ORIGIN = 0x0000B000, LENGTH = 0x00028000 - 0x0000B000 	/* KM0 DROM: 116k */

	/* KM0 RAM */
	KM0_ROMBSS_RAM (rw) :                   ORIGIN = 0x20000000, LENGTH = 0x20001000 - 0x20000000	/* KM0 ROM BSS RAM: 4k */
	KM0_MSP_RAM (rw) :                      ORIGIN = 0x20001000, LENGTH = 0x20002000 - 0x20001000	/* KM0 MSP RAM: 4k */
	KM0_STDLIB_HEAP_RAM (rw) :              ORIGIN = 0x20002000, LENGTH = 0x20003000 - 0x20002000	/* KM0 ROM STDLIB HEAP: 4k */

	/* Flash */
	KM4_BOOT_XIP (rx) :                     ORIGIN = 0x0F800020, LENGTH = 0x10000000 - 0x0F800020	/* KM4 Bootloader Virtual address: 8192k */
	KM0_IMG2_XIP (rx) :                     ORIGIN = 0x0C000020, LENGTH = 0x0E000000 - 0x0C000020	/* KM0 IMG2 Virtual address: 32768k */
	KM4_IMG2_XIP (rx) :                     ORIGIN = 0x0E000020, LENGTH = 0x0F800000 - 0x0E000020	/* KM4 IMG2 Virtual address: 24576k */
	BTRACE (rx) :                           ORIGIN = 0xC8800000, LENGTH = 0xC9000000 - 0xC8800000	/* BT LOG Virtual address: 8192k */
}

__km0_image2_entry_func__ = ORIGIN(KM0_BD_RAM);
__km0_ipc_memory_start__ = ORIGIN(KM0_IPC_RAM);
__km0_flash_text_start__ = ORIGIN(KM0_IMG2_XIP);
__km4_flash_text_start__ = ORIGIN(KM4_IMG2_XIP);
__km0_psram_text_start__ = ORIGIN(KM0_BD_PSRAM);
__km4_boot_text_start__ = ORIGIN(KM4_BOOT_XIP);

__km4_iram_ns_start__ = ORIGIN(KM4_IROM_NS);
__km4_dram_start__ = ORIGIN(KM4_DROM);
__km4_rombss_ram_s_start__ = ORIGIN(KM4_ROMBSS_RAM_S) - 0x10000000;
__km4_tz_nsc_start__ = ORIGIN(KM4_BD_RAM_TZ_ENTRY) - 0x1000;
__km4_tz_entry_start__ = ORIGIN(KM4_BD_RAM_TZ_ENTRY);
__km4_tz_s_start__ = ORIGIN(KM4_BD_RAM_TZ_ENTRY) + 0x4000;
__km4_tz_s_end__ = ORIGIN(KM4_BD_RAM_TZ_ENTRY) + 0xf000;
__km4_psram_tz_nsc_start__ = ORIGIN(KM4_BD_PSRAM_TZ_NSC) - 0x20;
__km4_psram_tz_entry_start__ = ORIGIN(KM4_BD_PSRAM_TZ_ENTRY) ;
__km4_psram_tz_s_start__ = ORIGIN(KM4_BD_PSRAM_TZ_S)- 0x10000020;
__km4_psram_tz_s_end__ = ORIGIN(KM4_BD_PSRAM_TZ_S) - 0x10000020 + 0x20000;
__km4_bd_ram_start__ = ORIGIN(KM4_BD_RAM) - 0x20;
;/*
; * Copyright (c) 2009-2023 Arm Limited
; * Copyright (c) 2022 Cypress Semiconductor Corporation (an Infineon company)
; * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
; *
; * Licensed under the Apache License, Version 2.0 (the "License");
; * you may not use this file except in compliance with the License.
; * You may obtain a copy of the License at
; *
; *     http://www.apache.org/licenses/LICENSE-2.0
; *
; * Unless required by applicable law or agreed to in writing, software
; * distributed under the License is distributed on an "AS IS" BASIS,
; * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
; * See the License for the specific language governing permissions and
; * limitations under the License.
; *
; *
; * This file is derivative of CMSIS V5.00 gcc_arm.ld
; */

/* Linker script to configure memory regions. */
/* This file will be run trough the pre-processor. */
#include "region_defs.h"

/* Include file with definitions for section alignments.
 * Note: it should be included after region_defs.h to let platform define
 * default values if needed. */
#include "tfm_s_linker_alignments.h"

	__ram_image3_nsc_start__ = ORIGIN(KM4_BD_RAM_TZ_NSC);
	__ram_image3_text_start__ = ORIGIN(KM4_BD_RAM_TZ_S);

#ifndef TFM_LINKER_VENEERS_START
#define TFM_LINKER_VENEERS_START ALIGN(TFM_LINKER_VENEERS_ALIGNMENT)
#endif

#ifndef TFM_LINKER_VENEERS_END
#define TFM_LINKER_VENEERS_END ALIGN(TFM_LINKER_VENEERS_ALIGNMENT)
#endif

#define VENEERS() \
/* \
 * Place the CMSE Veneers (containing the SG instruction) after the code, in \
 * a separate at least 32 bytes aligned region so that the SAU can \
 * programmed to just set this region as Non-Secure Callable. \
 */ \
.gnu.sgstubs : TFM_LINKER_VENEERS_START \
{ \
    *(.gnu.sgstubs*) \
} > KM4_BD_RAM_TZ_NSC \
/* GCC always places veneers at the end of .gnu.sgstubs section, so the only \
 * way to align the end of .gnu.sgstubs section is to align start of the \
 * next section */ \
.sgstubs_end : TFM_LINKER_VENEERS_END \
{ \
} > KM4_BD_RAM_TZ_NSC

__msp_stack_size__ = S_MSP_STACK_SIZE;

/* Library configurations */
GROUP(libgcc.a libc.a libm.a libnosys.a libc_nano.a)

ENTRY(Reset_Handler)

SECTIONS
{
#if defined(CONFIG_TFM_USE_TRUSTZONE) && !defined(TFM_LINKER_VENEERS_LOCATION_END)
    VENEERS()
#endif

    .ram_image3.entry :
    {
        *(SFN)
        . = ALIGN(32);
    } > KM4_BD_RAM_TZ_ENTRY

    Image$$VENEER_ENTRY$$Base = ADDR(.ram_image3.entry);
    Image$$VENEER_ENTRY$$Limit = ADDR(.ram_image3.entry) + SIZEOF(.ram_image3.entry);

#if defined(CONFIG_TFM_USE_TRUSTZONE)
    Image$$ER_VENEER$$Base = ADDR(.gnu.sgstubs);
    Image$$VENEER_ALIGN$$Limit = ADDR(.sgstubs_end);
#endif

    /*
     * MPU on Armv6-M/v7-M core in multi-core topology may require more strict
     * alignment that MPU region base address must align with the MPU region
     * size.
     * As a result, on Armv6-M/v7-M cores, to save memory resource and MPU
     * regions, unprivileged data sections and privileged data sections are
     * separated and gathered in unprivileged/privileged data area respectively.
     */
#if defined(__ARM_ARCH_8M_MAIN__) || defined(__ARM_ARCH_8M_BASE__) || \
    defined(__ARM_ARCH_8_1M_MAIN__)

    .msp_stack (NOLOAD): ALIGN(TFM_LINKER_MSP_STACK_ALIGNMENT)
    {
        . += __msp_stack_size__ - 0x8;
    } > KM4_MSP_RAM_S
    Image$$ARM_LIB_STACK$$ZI$$Base = ADDR(.msp_stack);
    Image$$ARM_LIB_STACK$$ZI$$Limit = ADDR(.msp_stack) + SIZEOF(.msp_stack);

    .msp_stack_seal_res (NOLOAD):
    {
        . += 0x8;
    } > KM4_MSP_RAM_S
    __StackSeal = ADDR(.msp_stack_seal_res);

#endif /* defined(__ARM_ARCH_8M_MAIN__) || defined(__ARM_ARCH_8M_BASE__) || \
        * defined(__ARM_ARCH_8_1M_MAIN__) */


    .TFM_VECTORS : ALIGN(4)
    {
        __vectors_start__ = .;
        KEEP(*(.vectors))
        . = ALIGN(4);
        __vectors_end__ = .;
    } > KM4_BD_RAM_TZ_S

    ASSERT(__vectors_start__ == ORIGIN(KM4_BD_RAM_TZ_S), "__vectors_start__ shall equal to __ram_image3_start__")

    .ER_TFM_CODE : ALIGN(4)
    {
        *startup*(.text*)
        *libplatform_s*:*(.text*)
        *libtfm_spm*:*(.text*)

        *libplatform_s*:*(.rodata*)
        *libtfm_spm*:*(.rodata*)
    } > KM4_BD_RAM_TZ_S



    .zero.table : ALIGN(4)
    {
        __tfm_zero_table_start__ = .;
        LONG (ADDR(.TFM_BSS))
        LONG (SIZEOF(.TFM_BSS) / 4)
        LONG (ADDR(.TFM_PSA_ROT_LINKER_BSS))
        LONG (SIZEOF(.TFM_PSA_ROT_LINKER_BSS) / 4)

        LONG (ADDR(.TFM_APP_ROT_LINKER_BSS))
        LONG (SIZEOF(.TFM_APP_ROT_LINKER_BSS) / 4)
#if defined(CONFIG_TFM_PARTITION_META)
        LONG (ADDR(.TFM_SP_META_PTR))
        LONG (SIZEOF(.TFM_SP_META_PTR) / 4)
#endif
        __tfm_zero_table_end__ = .;
    } > KM4_BD_RAM_TZ_S


    /**** Section for holding partition RO load data */
    /*
     * Sort the partition info by priority to guarantee the initing order.
     * The first loaded partition will be inited at last in SFN model.
     */
    .TFM_SP_LOAD_LIST : ALIGN(4)
    {
       KEEP(*(.part_load_priority_lowest))
       KEEP(*(.part_load_priority_low))
       KEEP(*(.part_load_priority_normal))
       KEEP(*(.part_load_priority_high))
    } > KM4_BD_RAM_TZ_S
    Image$$TFM_SP_LOAD_LIST$$RO$$Base = ADDR(.TFM_SP_LOAD_LIST);
    Image$$TFM_SP_LOAD_LIST$$RO$$Limit = ADDR(.TFM_SP_LOAD_LIST) + SIZEOF(.TFM_SP_LOAD_LIST);

    /**** PSA RoT RO part (CODE + RODATA) start here */
    . = ALIGN(TFM_LINKER_PSA_ROT_LINKER_CODE_ALIGNMENT);
    Image$$TFM_PSA_CODE_START$$Base = .;

    .TFM_PSA_ROT_LINKER : ALIGN(TFM_LINKER_PSA_ROT_LINKER_CODE_ALIGNMENT)
    {
        *tfm_psa_rot_partition*:*(.text*)
        *tfm_psa_rot_partition*:*(.rodata*)
        *(TFM_*_PSA-ROT_ATTR_FN)
        . = ALIGN(TFM_LINKER_PSA_ROT_LINKER_CODE_ALIGNMENT);
    } > KM4_BD_RAM_TZ_S

    Image$$TFM_PSA_ROT_LINKER$$RO$$Base = ADDR(.TFM_PSA_ROT_LINKER);
    Image$$TFM_PSA_ROT_LINKER$$RO$$Limit = ADDR(.TFM_PSA_ROT_LINKER) + SIZEOF(.TFM_PSA_ROT_LINKER);
    Image$$TFM_PSA_ROT_LINKER$$Base = ADDR(.TFM_PSA_ROT_LINKER);
    Image$$TFM_PSA_ROT_LINKER$$Limit = ADDR(.TFM_PSA_ROT_LINKER) + SIZEOF(.TFM_PSA_ROT_LINKER);

    /**** PSA RoT RO part (CODE + RODATA) end here */
    Image$$TFM_PSA_CODE_END$$Base = .;

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > KM4_BD_RAM_TZ_S

    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > KM4_BD_RAM_TZ_S
    __exidx_end = .;

#if defined(ENABLE_HEAP)
    __heap_size__ = S_HEAP_SIZE;
    .heap (NOLOAD): ALIGN(8)
    {
        __end__ = .;
        PROVIDE(end = .);
        __HeapBase = .;
        . += __heap_size__;
        __HeapLimit = .;
        __heap_limit = .; /* Add for _sbrk */
    } > KM4_BD_RAM_TZ_S
#endif

#if defined(CONFIG_TFM_PARTITION_META)
	/*------------UNPRIV------------*/
    .TFM_SP_META_PTR (NOLOAD): ALIGN(TFM_LINKER_SP_META_PTR_ALIGNMENT)
    {
        *(.bss.SP_META_PTR_SPRTL_INST)
        . = ALIGN(TFM_LINKER_SP_META_PTR_ALIGNMENT);
    } > KM4_BD_RAM_TZ_S
    Image$$TFM_SP_META_PTR$$ZI$$Base = ADDR(.TFM_SP_META_PTR);
    Image$$TFM_SP_META_PTR$$ZI$$Limit = ADDR(.TFM_SP_META_PTR) + SIZEOF(.TFM_SP_META_PTR);
#endif

    /**** APPLICATION RoT DATA start here */
	/*------------UNPRIV------------*/
    . = ALIGN(TFM_LINKER_APP_ROT_LINKER_DATA_ALIGNMENT);
    Image$$TFM_APP_RW_STACK_START$$Base = .;

    .TFM_APP_ROT_LINKER_DATA : ALIGN(TFM_LINKER_APP_ROT_LINKER_DATA_ALIGNMENT)
    {
        *tfm_app_rot_partition*:*(.data*)
        *(TFM_*_APP-ROT_ATTR_RW)
        . = ALIGN(4);
    } > KM4_BD_RAM_TZ_S 
    Image$$TFM_APP_ROT_LINKER_DATA$$RW$$Base = ADDR(.TFM_APP_ROT_LINKER_DATA);
    Image$$TFM_APP_ROT_LINKER_DATA$$RW$$Limit = ADDR(.TFM_APP_ROT_LINKER_DATA) + SIZEOF(.TFM_APP_ROT_LINKER_DATA);

    .TFM_APP_ROT_LINKER_BSS (NOLOAD): ALIGN(4)
    {
        start_of_TFM_APP_ROT_LINKER = .;
        *tfm_app_rot_partition*:*(.bss*)
        *tfm_app_rot_partition*:*(COMMON)
        *(TFM_*_APP-ROT_ATTR_ZI)
        . += (. - start_of_TFM_APP_ROT_LINKER) ? 0 : 4;
        . = ALIGN(TFM_LINKER_APP_ROT_LINKER_DATA_ALIGNMENT);
    } > KM4_BD_RAM_TZ_S 
    Image$$TFM_APP_ROT_LINKER_DATA$$ZI$$Base = ADDR(.TFM_APP_ROT_LINKER_BSS);
    Image$$TFM_APP_ROT_LINKER_DATA$$ZI$$Limit = ADDR(.TFM_APP_ROT_LINKER_BSS) + SIZEOF(.TFM_APP_ROT_LINKER_BSS);

    /**** APPLICATION RoT DATA end here */
    Image$$TFM_APP_RW_STACK_END$$Base = .;



    /**** PSA RoT DATA start here */

    Image$$TFM_PSA_RW_STACK_START$$Base = .;

    .TFM_PSA_ROT_LINKER_DATA : ALIGN(TFM_LINKER_PSA_ROT_LINKER_DATA_ALIGNMENT)
    {
        *tfm_psa_rot_partition*:*(.data*)
        *(TFM_*_PSA-ROT_ATTR_RW)
        . = ALIGN(4);
    } > KM4_BD_RAM_TZ_S 
    Image$$TFM_PSA_ROT_LINKER_DATA$$RW$$Base = ADDR(.TFM_PSA_ROT_LINKER_DATA);
    Image$$TFM_PSA_ROT_LINKER_DATA$$RW$$Limit = ADDR(.TFM_PSA_ROT_LINKER_DATA) + SIZEOF(.TFM_PSA_ROT_LINKER_DATA);

    .TFM_PSA_ROT_LINKER_BSS (NOLOAD): ALIGN(4)
    {
        start_of_TFM_PSA_ROT_LINKER = .;
        *tfm_psa_rot_partition*:*(.bss*)
        *tfm_psa_rot_partition*:*(COMMON)
        *(TFM_*_PSA-ROT_ATTR_ZI)
        . += (. - start_of_TFM_PSA_ROT_LINKER) ? 0 : 4;
        . = ALIGN(TFM_LINKER_PSA_ROT_LINKER_DATA_ALIGNMENT);
    } > KM4_BD_RAM_TZ_S


    Image$$TFM_PSA_ROT_LINKER_DATA$$ZI$$Base = ADDR(.TFM_PSA_ROT_LINKER_BSS);
    Image$$TFM_PSA_ROT_LINKER_DATA$$ZI$$Limit = ADDR(.TFM_PSA_ROT_LINKER_BSS) + SIZEOF(.TFM_PSA_ROT_LINKER_BSS);

    /**** PSA RoT DATA end here */
    Image$$TFM_PSA_RW_STACK_END$$Base = .;



	/*------------UNPRIV------------*/
    .TFM_UNPRIV_CODE : ALIGN(TFM_LINKER_UNPRIV_CODE_ALIGNMENT)
    {
        *(.text*)
        *(.rodata*)

        . = ALIGN(TFM_LINKER_UNPRIV_CODE_ALIGNMENT);
    } > KM4_BD_RAM_TZ_S
    Image$$TFM_UNPRIV_CODE$$RO$$Base = ADDR(.TFM_UNPRIV_CODE);
    Image$$TFM_UNPRIV_CODE$$RO$$Limit = ADDR(.TFM_UNPRIV_CODE) + SIZEOF(.TFM_UNPRIV_CODE);
    /**** APPLICATION RoT RO part (CODE + RODATA) start here */
	/*------------UNPRIV------------*/
    Image$$TFM_APP_CODE_START$$Base = .;

    .TFM_APP_ROT_LINKER Image$$TFM_UNPRIV_CODE$$RO$$Limit : ALIGN(TFM_LINKER_APP_ROT_LINKER_CODE_ALIGNMENT)
    {
        *tfm_app_rot_partition*:*(.text*)
        *tfm_app_rot_partition*:*(.rodata*)
        *(TFM_*_APP-ROT_ATTR_FN)
		
        . = ALIGN(TFM_LINKER_APP_ROT_LINKER_CODE_ALIGNMENT);
    } > KM4_BD_RAM_TZ_S
    Image$$TFM_APP_ROT_LINKER$$RO$$Base = ADDR(.TFM_APP_ROT_LINKER);
    Image$$TFM_APP_ROT_LINKER$$RO$$Limit = ADDR(.TFM_APP_ROT_LINKER) + SIZEOF(.TFM_APP_ROT_LINKER);
    Image$$TFM_APP_ROT_LINKER$$Base = ADDR(.TFM_APP_ROT_LINKER);
    Image$$TFM_APP_ROT_LINKER$$Limit = ADDR(.TFM_APP_ROT_LINKER) + SIZEOF(.TFM_APP_ROT_LINKER);

    /**** APPLICATION RoT RO part (CODE + RODATA) end here */
    Image$$TFM_APP_CODE_END$$Base = .;

    ASSERT(Image$$TFM_UNPRIV_CODE$$RO$$Limit + SIZEOF(.TFM_APP_ROT_LINKER) == Image$$TFM_APP_CODE_END$$Base, "TFM_UNPRIV_CODE must be continuous with TFM_APP_CODE_START")




    .TFM_DATA : ALIGN(4)
    {
        *(.data*)

        . = ALIGN(4);
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);

        . = ALIGN(4);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);

        KEEP(*(.jcr*))
        . = ALIGN(4);

    } > KM4_BD_RAM_TZ_S
    Image$$ER_TFM_DATA$$RW$$Base = ADDR(.TFM_DATA);
    Image$$ER_TFM_DATA$$RW$$Limit = ADDR(.TFM_DATA) + SIZEOF(.TFM_DATA);

    .TFM_BSS (NOLOAD): ALIGN(4)
    {
        __bss_start__ = .;

        /* The runtime partition placed order is same as load partition */
        __partition_runtime_start__ = .;
        KEEP(*(.bss.part_runtime_priority_lowest))
        KEEP(*(.bss.part_runtime_priority_low))
        KEEP(*(.bss.part_runtime_priority_normal))
        KEEP(*(.bss.part_runtime_priority_high))
        __partition_runtime_end__ = .;
        . = ALIGN(4);

        /* The runtime service placed order is same as load partition */
        __service_runtime_start__ = .;
        KEEP(*(.bss.serv_runtime_priority_lowest))
        KEEP(*(.bss.serv_runtime_priority_low))
        KEEP(*(.bss.serv_runtime_priority_normal))
        KEEP(*(.bss.serv_runtime_priority_high))
        __service_runtime_end__ = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
    } > KM4_BD_RAM_TZ_S
    Image$$ER_TFM_DATA$$ZI$$Base = ADDR(.TFM_BSS);
    Image$$ER_TFM_DATA$$ZI$$Limit = ADDR(.TFM_BSS) + SIZEOF(.TFM_BSS);
    Image$$ER_PART_RT_POOL$$ZI$$Base = __partition_runtime_start__;
    Image$$ER_PART_RT_POOL$$ZI$$Limit = __partition_runtime_end__;
    Image$$ER_SERV_RT_POOL$$ZI$$Base = __service_runtime_start__;
    Image$$ER_SERV_RT_POOL$$ZI$$Limit = __service_runtime_end__;

    Image$$ER_TFM_DATA$$Base = ADDR(.TFM_DATA);
    Image$$ER_TFM_DATA$$Limit = ADDR(.TFM_DATA) + SIZEOF(.TFM_DATA) + SIZEOF(.TFM_BSS);


    PROVIDE(__stack = Image$$ARM_LIB_STACK$$ZI$$Limit);
}
SECTIONS
{
    PROVIDE(__vectors_table = 0x0);
    PROVIDE(__rom_xo_start__ = 0x140);
    PROVIDE(CRYPTO_MemDump = 0x141);
    PROVIDE(CRYPTO_SetSrcDesc = 0x269);
    PROVIDE(CRYPTO_SetDstDesc = 0x2f9);
    PROVIDE(CRYPTO_CleanCmdOk = 0x389);
    PROVIDE(CRYPTO_ClearAllINT = 0x3d5);
    PROVIDE(CRYPTO_SetCmdBuf = 0x41d);
    PROVIDE(CRYPTO_SrcDescKeyIv = 0x6b1);
    PROVIDE(CRYPTO_Reset = 0xb41);
    PROVIDE(CRYPTO_SHA_Reset = 0xb8d);
    PROVIDE(CRYPTO_SetSecurityModeAD = 0xbdd);
    PROVIDE(CRYPTO_Init = 0xdad);
    PROVIDE(CRYPTO_SHA_Init = 0xdd5);
    PROVIDE(CRYPTO_OTPKey_Init = 0xdfd);
    PROVIDE(CRYPTO_OTPKey_SHA_Init = 0xe61);
    PROVIDE(CRYPTO_ProcessAD = 0xee1);
    PROVIDE(CRYPTO_SendSeqBuf = 0x1739);
    PROVIDE(CRYPTO_CipherInit = 0x1849);
    PROVIDE(CRYPTO_CipherEncryptAD = 0x186d);
    PROVIDE(CRYPTO_CipherDecryptAD = 0x18a1);
    PROVIDE(SBOOT_SwapData = 0x18d9);
    PROVIDE(SBOOT_GetMdType = 0x18f5);
    PROVIDE(SBOOT_EDDSA_Verify_SW = 0x1981);
    PROVIDE(SBOOT_Validate_Algorithm = 0x199d);
    PROVIDE(SBOOT_Validate_Signature = 0x19e1);
    PROVIDE(SBOOT_Validate_PubKey = 0x1a05);
    PROVIDE(SBOOT_Validate_ImgHash = 0x1a75);
    PROVIDE(SBOOT_SignatureVerify = 0x1bf1);
    PROVIDE(rtl_crypto_aes_cbc_init = 0x1c95);
    PROVIDE(rtl_crypto_aes_cbc_encrypt = 0x1cf5);
    PROVIDE(rtl_crypto_aes_cbc_decrypt = 0x1d49);
    PROVIDE(rtl_crypto_aes_ecb_init = 0x1d9d);
    PROVIDE(rtl_crypto_aes_ecb_encrypt = 0x1dfd);
    PROVIDE(rtl_crypto_aes_ecb_decrypt = 0x1e3d);
    PROVIDE(rtl_crypto_aes_ctr_init = 0x1e7d);
    PROVIDE(rtl_crypto_aes_ctr_encrypt = 0x1edd);
    PROVIDE(rtl_crypto_aes_ctr_decrypt = 0x1f29);
    PROVIDE(rtl_crypto_aes_cfb_init = 0x1f75);
    PROVIDE(rtl_crypto_aes_cfb_encrypt = 0x1fd5);
    PROVIDE(rtl_crypto_aes_cfb_decrypt = 0x2021);
    PROVIDE(rtl_crypto_aes_ofb_init = 0x206d);
    PROVIDE(rtl_crypto_aes_ofb_encrypt = 0x20cd);
    PROVIDE(rtl_crypto_aes_ofb_decrypt = 0x2119);
    PROVIDE(rtl_crypto_aes_gcm_init = 0x2165);
    PROVIDE(rtl_crypto_aes_gcm_encrypt = 0x21c5);
    PROVIDE(rtl_crypto_aes_gcm_decrypt = 0x2259);
    PROVIDE(hash_save = 0x22ed);
    PROVIDE(hash_restore = 0x2385);
    PROVIDE(rtl_crypto_sha1_init = 0x2431);
    PROVIDE(rtl_crypto_sha1_process = 0x24c9);
    PROVIDE(rtl_crypto_sha1_update_rom = 0x2509);
    PROVIDE(rtl_crypto_sha1_update = 0x2589);
    PROVIDE(rtl_crypto_sha1_final = 0x25f9);
    PROVIDE(rtl_crypto_sha1 = 0x2675);
    PROVIDE(rtl_crypto_sha2_init = 0x26a1);
    PROVIDE(rtl_crypto_sha2_process = 0x2969);
    PROVIDE(rtl_crypto_sha2_update_rom = 0x29ad);
    PROVIDE(rtl_crypto_sha2_update = 0x2a35);
    PROVIDE(rtl_crypto_sha2_final = 0x2aa5);
    PROVIDE(rtl_crypto_sha2 = 0x2b25);
    PROVIDE(rtl_crypto_hmac_sha1_init = 0x2b51);
    PROVIDE(rtl_crypto_hmac_sha1_process = 0x2c39);
    PROVIDE(rtl_crypto_hmac_sha1_update_rom = 0x2c7d);
    PROVIDE(rtl_crypto_hmac_sha1_update = 0x2d05);
    PROVIDE(rtl_crypto_hmac_sha1_final = 0x2d75);
    PROVIDE(rtl_crypto_hmac_sha1 = 0x2df5);
    PROVIDE(rtl_crypto_hmac_sha2_init = 0x2e25);
    PROVIDE(rtl_crypto_hmac_sha2_process = 0x3189);
    PROVIDE(rtl_crypto_hmac_sha2_update_rom = 0x31d1);
    PROVIDE(rtl_crypto_hmac_sha2_update = 0x325d);
    PROVIDE(rtl_crypto_hmac_sha2_final = 0x32cd);
    PROVIDE(rtl_crypto_hmac_sha2 = 0x3351);
    PROVIDE(md_hash_save = 0x3381);
    PROVIDE(md_hash_restore = 0x3419);
    PROVIDE(rtl_crypto_md5_init = 0x34c1);
    PROVIDE(rtl_crypto_md5_process = 0x3549);
    PROVIDE(rtl_crypto_md5_update_rom = 0x35ad);
    PROVIDE(rtl_crypto_md5_update = 0x362d);
    PROVIDE(rtl_crypto_md5_final = 0x369d);
    PROVIDE(rtl_crypto_md5 = 0x371d);
    PROVIDE(rtl_crypto_hmac_md5_init = 0x3749);
    PROVIDE(rtl_crypto_hmac_md5_process = 0x3821);
    PROVIDE(rtl_crypto_hmac_md5_update_rom = 0x3869);
    PROVIDE(rtl_crypto_hmac_md5_update = 0x38f5);
    PROVIDE(rtl_crypto_hmac_md5_final = 0x3965);
    PROVIDE(rtl_crypto_hmac_md5 = 0x39e9);
    PROVIDE(RSIP_OTF_Load_Key = 0x3a19);
    PROVIDE(RSIP_OTF_init = 0x3a31);
    PROVIDE(RSIP_IV_Set = 0x3a45);
    PROVIDE(RSIP_OTF_Cmd = 0x3a61);
    PROVIDE(RSIP_OTF_Enable = 0x3a7d);
    PROVIDE(RSIP_MMU_Config = 0x3b25);
    PROVIDE(RSIP_MMU_Cmd = 0x3b5d);
    PROVIDE(RSIP_MMU_Cache_Clean = 0x3b89);
    PROVIDE(boot_rom_ed25519_ge_double_scalarmult_vartime = 0x4e5b);
    PROVIDE(boot_rom_ed25519_ge_frombytes_negate_vartime = 0x522d);
    PROVIDE(boot_rom_ed25519_ge_tobytes = 0x56c7);
    PROVIDE(boot_rom_ed25519_verify_signature = 0x584b);
    PROVIDE(boot_rom_ed25519_crypto_sign_verify_detached = 0x584f);
    PROVIDE(boot_rom_ed25519_sc_reduce = 0x5a67);
    PROVIDE(rom_sha512_starts = 0x69d7);
    PROVIDE(rom_sha512_update = 0x69db);
    PROVIDE(rom_sha512_finish = 0x69df);
    PROVIDE(rom_sha512 = 0x69e3);
    PROVIDE(rom_sha512_hmac_starts = 0x69e7);
    PROVIDE(rom_sha512_hmac_update = 0x6a71);
    PROVIDE(rom_sha512_hmac_finish = 0x6a75);
    PROVIDE(rom_sha512_hmac_reset = 0x6ac9);
    PROVIDE(rom_sha512_hmac = 0x6ae5);
    PROVIDE(rom_sha512_hkdf = 0x6b21);
    PROVIDE(BOOT_ROM_OTFCheck = 0x6bf1);
    PROVIDE(BOOT_ROM_OTPCheck = 0x6cc1);
    PROVIDE(BOOT_ROM_PatchCmd = 0x6d21);
    PROVIDE(BOOT_ROM_PatchEntry = 0x6ded);
    PROVIDE(BOOT_ROM_SPUFlash = 0x6e31);
    PROVIDE(BOOT_ROM_InitSpic = 0x6e85);
    PROVIDE(BOOT_ROM_LoadImage = 0x6fa1);
    PROVIDE(BOOT_ROM_SignatureCheck = 0x7111);
    PROVIDE(BOOT_ROM_LoaderEntry = 0x71e1);
    PROVIDE(BOOT_ROM_KeyDeriveFunc = 0x7265);
    PROVIDE(BOOT_ROM_HUKDerive = 0x7459);
    PROVIDE(BOOT_ROM_SecurityConfig = 0x752d);
    PROVIDE(BOOT_ROM_BootldrVersion = 0x7715);
    PROVIDE(BOOT_ROM_BootldrSelect = 0x7765);
    PROVIDE(BOOT_ROM_InitFlash = 0x79e9);
    PROVIDE(BOOT_ROM_FromFlash = 0x7b0d);
    PROVIDE(BOOT_ROM_InitDebugFlg = 0x7bc9);
    PROVIDE(BOOT_ROM_OSC131_Enable = 0x7c09);
    PROVIDE(BOOT_ROM_ResetVsr = 0x7c21);
    PROVIDE(__rom_xo_end__ = 0x7e44);
    PROVIDE(Reset_Handler = 0x9001);
    PROVIDE(NMI_Handler = 0x9011);
    PROVIDE(HardFault_Handler = 0x9015);
    PROVIDE(MemManage_Handler = 0x9019);
    PROVIDE(BusFault_Handler = 0x901d);
    PROVIDE(UsageFault_Handler = 0x9021);
    PROVIDE(SecureFault_Handler = 0x9025);
    PROVIDE(io_assert_failed = 0x902d);
    PROVIDE(BKUP_Write = 0x904d);
    PROVIDE(BKUP_Read = 0x9079);
    PROVIDE(BKUP_Set = 0x90a1);
    PROVIDE(BKUP_Clear = 0x90c9);
    PROVIDE(EFUSE_GetPackageInfo = 0x90f5);
    PROVIDE(EFUSE_GetBDNum = 0x9119);
    PROVIDE(EFUSE_GetChipVersion = 0x9139);
    PROVIDE(EFUSE_GetESFlag = 0x915d);
    PROVIDE(EFUSE_GetUUID = 0x9181);
    PROVIDE(EFUSE_Get_Info = 0x91c5);
    PROVIDE(XTAL_ClkGet = 0x9255);
    PROVIDE(PLL_ClkGet = 0x9271);
    PROVIDE(_CPU_ClkSet = 0x92b1);
    PROVIDE(CPU_ClkGet = 0x93b5);
    PROVIDE(_CPU_ClkSrcSet = 0x9441);
    PROVIDE(DelayNop = 0x9499);
    PROVIDE(DelayUs = 0x94a5);
    PROVIDE(DelayMs = 0x951d);
    PROVIDE(DelayClkUpdate = 0x9531);
    PROVIDE(FLASH_Calibration_PLL_Open = 0x9549);
    PROVIDE(FLASH_Calibration_PLL_Close = 0x9575);
    PROVIDE(FLASH_CalibrationPLLPSCmd = 0x958d);
    PROVIDE(FLASH_CalibrationPLLPS_Shift = 0x95a5);
    PROVIDE(FLASH_PLLInit_ClockDiv = 0x95bd);
    PROVIDE(FLASH_CalibrationPhase = 0x9631);
    PROVIDE(FLASH_CalibrationPhaseIdx = 0x96c1);
    PROVIDE(FLASH_CalibrationNewCmd = 0x96cd);
    PROVIDE(FLASH_Read_HandShake_Cmd = 0x96f5);
    PROVIDE(FLASH_Read_DataIsRight = 0x9749);
    PROVIDE(FLASH_CalibrationNew = 0x97e1);
    PROVIDE(FLASH_Read_HandShake = 0x996d);
    PROVIDE(FLASH_UserMode_Enter = 0x99e5);
    PROVIDE(FLASH_UserMode_Exit = 0x99fd);
    PROVIDE(FLASH_Config = 0x9a0d);
    PROVIDE(FLASH_SetSpiMode = 0x9aa1);
    PROVIDE(FLASH_RxCmd_InUserMode = 0x9ba9);
    PROVIDE(FLASH_RxCmd = 0x9c25);
    PROVIDE(FLASH_WaitBusy_InUserMode = 0x9c41);
    PROVIDE(FLASH_RxData = 0x9cc9);
    PROVIDE(FLASH_TxCmd_InUserMode = 0x9df1);
    PROVIDE(FLASH_WriteEn_InUserMode = 0x9e49);
    PROVIDE(FLASH_TxData = 0x9e6d);
    PROVIDE(FLASH_SetStatus = 0x9fb9);
    PROVIDE(FLASH_Erase = 0x9fe5);
    PROVIDE(FLASH_Addr4ByteEn = 0xa081);
    PROVIDE(FLASH_Addr4ByteDis = 0xa0a5);
    PROVIDE(FLASH_DeepPowerDown = 0xa0c9);
    PROVIDE(FLASH_SetStatusBits = 0xa129);
    PROVIDE(FLASH_StructInit_Micron = 0xa201);
    PROVIDE(FLASH_StructInit_MXIC = 0xa26d);
    PROVIDE(FLASH_StructInit_GD = 0xa2d9);
    PROVIDE(FLASH_StructInit = 0xa34d);
    PROVIDE(FLASH_Init = 0xa3bd);
    PROVIDE(FLASH_Wait_Boot_Finish = 0xa3dd);
    PROVIDE(GPIO_INTMode = 0xa401);
    PROVIDE(GPIO_INTConfig = 0xa485);
    PROVIDE(GPIO_INTHandler = 0xa4c9);
    PROVIDE(GPIO_Direction = 0xa581);
    PROVIDE(GPIO_Init = 0xa5cd);
    PROVIDE(GPIO_DeInit = 0xa65d);
    PROVIDE(GPIO_ReadDataBit = 0xa6d1);
    PROVIDE(GPIO_WriteBit = 0xa709);
    PROVIDE(GPIO_PortDirection = 0xa755);
    PROVIDE(GPIO_PortRead = 0xa7a9);
    PROVIDE(GPIO_PortWrite = 0xa7ed);
    PROVIDE(GPIO_DebounceClock = 0xa839);
    PROVIDE(GPIO_LevelSync = 0xa889);
    PROVIDE(GPIO_UserRegIrq = 0xa8d5);
    PROVIDE(GPIO_ODInit = 0xa901);
    PROVIDE(GPIO_ODWriteBit = 0xa931);
    PROVIDE(LOGUART_StructInit = 0xa949);
    PROVIDE(LOGUART_SetBaud = 0xa981);
    PROVIDE(LOGUART_RxCmd = 0xaa2d);
    PROVIDE(LOGUART_INTCoreConfig = 0xaa41);
    PROVIDE(LOGUART_AGGCmd = 0xaa55);
    PROVIDE(LOGUART_AGGPathCmd = 0xaa69);
    PROVIDE(LOGUART_AGGSetTimeOut = 0xaae9);
    PROVIDE(LOGUART_AGGSetThreshold = 0xab05);
    PROVIDE(LOGUART_AGGSetSyncPattern = 0xab31);
    PROVIDE(LOGUART_AGGPathAllCmd = 0xab41);
    PROVIDE(LOGUART_WaitTx = 0xab65);
    PROVIDE(LOGUART_PutChar = 0xaba1);
    PROVIDE(LOGUART_GetChar = 0xabed);
    PROVIDE(LOGUART_Readable = 0xac09);
    PROVIDE(LOGUART_GetRxCount = 0xac19);
    PROVIDE(LOGUART_GetIMR = 0xac25);
    PROVIDE(LOGUART_SetIMR = 0xac31);
    PROVIDE(LOGUART_INTConfig = 0xac3d);
    PROVIDE(LOGUART_INTClear = 0xac4d);
    PROVIDE(LOGUART_GetStatus = 0xac55);
    PROVIDE(LOGUART_ClearRxFifo = 0xac59);
    PROVIDE(_LOGUART_Init = 0xac79);
    PROVIDE(LOGUART_Init = 0xadcd);
    PROVIDE(LOGUART_WaitBusy = 0xae31);
    PROVIDE(LOGUART_WaitTxComplete = 0xae71);
    PROVIDE(LOGUART_BT_SendData = 0xaea1);
    PROVIDE(LOGUART_Relay_StructInit = 0xaed9);
    PROVIDE(LOGUART_Relay_SetFormat = 0xaeed);
    PROVIDE(LOGUART_Relay_SetBaud = 0xaf3d);
    PROVIDE(LOGUART_Relay_ClearRxFifo = 0xaf95);
    PROVIDE(LOGUART_Relay_RxCmd = 0xafb9);
    PROVIDE(LOGUART_MonitorParaConfig = 0xafcd);
    PROVIDE(LOGUART_LPBaudSet = 0xb015);
    PROVIDE(LOGUART_Relay_LPSetBaud = 0xb0c1);
    PROVIDE(LOGUART_RxMonitorCmd = 0xb121);
    PROVIDE(LOGUART_RxMonBaudCtrlRegGet = 0xb135);
    PROVIDE(LOGUART_RxMonitorSatusGet = 0xb139);
    PROVIDE(LOGUART_DiagInit = 0xb13d);
    PROVIDE(OTP_PowerCmd = 0xb1cd);
    PROVIDE(OTP_GetPowerState = 0xb1e9);
    PROVIDE(OTP_AccessCmd = 0xb1f9);
    PROVIDE(OTPPowerSwitch = 0xb211);
    PROVIDE(OTPGetCRC = 0xb235);
    PROVIDE(OTP_Read8_Rom = 0xb275);
    PROVIDE(OTP_ProgramMarginRead8 = 0xb325);
    PROVIDE(_OTP_Write8 = 0xb3d9);
    PROVIDE(OTP_Write8_Rom = 0xb485);
    PROVIDE(OTP_Read32 = 0xb52d);
    PROVIDE(OTP_PG_Packet_Byte = 0xb565);
    PROVIDE(OTP_PG_Packet_Word = 0xb635);
    PROVIDE(OTP_LogicalMap_Read = 0xb769);
    PROVIDE(OTP_LogicalMap_Write = 0xb891);
    PROVIDE(otp_logical_remain = 0xbaa9);
    PROVIDE(PAD_DrvStrength = 0xbaf5);
    PROVIDE(PAD_SchmitCtrl = 0xbb19);
    PROVIDE(PAD_SlewRateCtrl = 0xbb39);
    PROVIDE(PAD_PullCtrl = 0xbb59);
    PROVIDE(PAD_SleepPullCtrl = 0xbc21);
    PROVIDE(PAD_ResistorCtrl = 0xbce9);
    PROVIDE(PAD_InputCtrl = 0xbd0d);
    PROVIDE(PAD_CMD = 0xbd2d);
    PROVIDE(Pinmux_Config = 0xbd4d);
    PROVIDE(Pinmux_ConfigGet = 0xbd65);
    PROVIDE(Pinmux_UartLogCtrl = 0xbd75);
    PROVIDE(PAD_SpicCtrl = 0xbdbd);
    PROVIDE(Pinmux_BootSpicCtrl = 0xbe91);
    PROVIDE(Pinmux_ComboSpicCtrl = 0xbe9d);
    PROVIDE(Pinmux_Swdoff = 0xbea9);
    PROVIDE(RCC_PeriphClockCmd = 0xbebd);
    PROVIDE(RCC_PeriphClockSource_UART = 0xbf5d);
    PROVIDE(RCC_PeriphClockSource_SPIC = 0xbfe5);
    PROVIDE(RCC_PeriphClockSource_PSRAM = 0xc001);
    PROVIDE(RCC_PeriphClockSource_CTC = 0xc01d);
    PROVIDE(RCC_PeriphClockSource_ADC = 0xc039);
    PROVIDE(RCC_PeriphClockSource_GPIO = 0xc055);
    PROVIDE(RCC_PeriphClockSource_SPORT = 0xc071);
    PROVIDE(RCC_PeriphClockSource_LOGUART = 0xc0b1);
    PROVIDE(IPC_SEMTake = 0xc109);
    PROVIDE(IPC_SEMFree = 0xc135);
    PROVIDE(SYSCFG_RLVersion = 0xc15d);
    PROVIDE(SYSCFG_GetRLNum = 0xc181);
    PROVIDE(SYSCFG_TRP_UARTImage = 0xc1a5);
    PROVIDE(SYSCFG_TRP_ICFG = 0xc1b5);
    PROVIDE(SYSCFG_TRP_TestMode = 0xc1cd);
    PROVIDE(SYSCFG_TRP_OTPBYP = 0xc1dd);
    PROVIDE(SYSCFG_ROMINFO_Get = 0xc1ed);
    PROVIDE(SYSCFG_ROMINFO_Set = 0xc1fd);
    PROVIDE(SYSCFG_CHIPType_Get = 0xc219);
    PROVIDE(SYSCFG_OTP_RDPEn = 0xc229);
    PROVIDE(SYSCFG_OTP_RSIPEn = 0xc249);
    PROVIDE(SYSCFG_OTP_SBootEn = 0xc269);
    PROVIDE(SYSCFG_OTP_LowBaudLog = 0xc289);
    PROVIDE(SYSCFG_OTP_DisBootLog = 0xc299);
    PROVIDE(SYSCFG_OTP_BootMCMFlash = 0xc2a9);
    PROVIDE(SYSCFG_OTP_BOOT_CNT_LMT_EN = 0xc2d1);
    PROVIDE(SYSCFG_OTP_BootFlashBaud = 0xc2e1);
    PROVIDE(SYSCFG_OTP_SPICBootSpeedUpDis = 0xc2f1);
    PROVIDE(SYSCFG_OTP_SPICAddr4ByteEn = 0xc301);
    PROVIDE(SYSCFG_OTP_FlashDSleepEn = 0xc311);
    PROVIDE(SYSCFG_OTP_UsbClockCalEn = 0xc321);
    PROVIDE(SYSCFG_OTP_UsbDownloadEn = 0xc331);
    PROVIDE(SYSCFG_OTP_UartDownloadDis = 0xc33d);
    PROVIDE(SYSCFG_OTP_AntiRollback = 0xc34d);
    PROVIDE(SYSCFG_OTP_FaultLogDis = 0xc361);
    PROVIDE(SYSCFG_OTP_RSIPMode = 0xc375);
    PROVIDE(SYSCFG_OTP_HUKDerivEn = 0xc38d);
    PROVIDE(SYSCFG_OTP_RomPatchEn = 0xc3a1);
    PROVIDE(SYSCFG_OTP_RandDelayEn = 0xc3c1);
    PROVIDE(SYSCFG_OTP_RMAMode = 0xc3d1);
    PROVIDE(SYSCFG_HWTrigDownload = 0xc3f5);
    PROVIDE(SYSCFG_SWTrigDownload = 0xc409);
    PROVIDE(SYSTIMER_Init = 0xc42d);
    PROVIDE(SYSTIMER_TickGet = 0xc479);
    PROVIDE(SYSTIMER_GetPassTick = 0xc489);
    PROVIDE(SYSTIMER_GetPassTime = 0xc4a9);
    PROVIDE(RTIM_TimeBaseStructInit = 0xc4dd);
    PROVIDE(RTIM_Cmd = 0xc4fd);
    PROVIDE(RTIM_GetCount = 0xc521);
    PROVIDE(RTIM_INTConfig = 0xc531);
    PROVIDE(RTIM_INTClear = 0xc575);
    PROVIDE(RTIM_TimeBaseInit = 0xc58d);
    PROVIDE(RTIM_DeInit = 0xc68d);
    PROVIDE(RTIM_INTClearPendingBit = 0xc6ad);
    PROVIDE(RTIM_GetFlagStatus = 0xc6ed);
    PROVIDE(RTIM_GetINTStatus = 0xc765);
    PROVIDE(TRNG_Random_Delay = 0xc7e9);
    PROVIDE(TRNG_Security_Only = 0xc85d);
    PROVIDE(TRNG_INTConfig = 0xc8a9);
    PROVIDE(TRNG_ClearINT = 0xc8dd);
    PROVIDE(TRNG_GetINT = 0xc905);
    PROVIDE(TRNG_Init = 0xc929);
    PROVIDE(UART_DeInit = 0xc92d);
    PROVIDE(UART_StructInit = 0xc935);
    PROVIDE(UART_BaudParaGetFull = 0xc951);
    PROVIDE(UART_BaudParaGet = 0xc991);
    PROVIDE(UART_SetBaud = 0xc9b9);
    PROVIDE(UART_SetBaudExt = 0xca69);
    PROVIDE(UART_SetRxLevel = 0xcaf9);
    PROVIDE(UART_RxCmd = 0xcb09);
    PROVIDE(UART_Writable = 0xcb1d);
    PROVIDE(UART_Readable = 0xcb25);
    PROVIDE(UART_CharPut = 0xcb2d);
    PROVIDE(UART_CharGet = 0xcb31);
    PROVIDE(UART_ReceiveData = 0xcb39);
    PROVIDE(UART_SendData = 0xcb61);
    PROVIDE(UART_ReceiveDataTO = 0xcb89);
    PROVIDE(UART_SendDataTO = 0xcbc5);
    PROVIDE(UART_RxByteCntClear = 0xcc01);
    PROVIDE(UART_RxByteCntGet = 0xcc0d);
    PROVIDE(UART_BreakCtl = 0xcc15);
    PROVIDE(UART_ClearRxFifo = 0xcc29);
    PROVIDE(UART_Init = 0xcc49);
    PROVIDE(UART_ClearTxFifo = 0xcce5);
    PROVIDE(UART_INTConfig = 0xccf1);
    PROVIDE(UART_INT_Clear = 0xcd01);
    PROVIDE(UART_IntStatus = 0xcd09);
    PROVIDE(UART_ModemStatusGet = 0xcd0d);
    PROVIDE(UART_LineStatusGet = 0xcd11);
    PROVIDE(UART_WaitBusy = 0xcd15);
    PROVIDE(INT_HardFault = 0xcd39);
    PROVIDE(INT_MemManage = 0xcd5d);
    PROVIDE(INT_BusFault = 0xcd81);
    PROVIDE(INT_UsageFault = 0xcda5);
    PROVIDE(INT_SecureFault = 0xcdc9);
    PROVIDE(INT_HardFault_C = 0xcded);
    PROVIDE(INT_NMI = 0xd23d);
    PROVIDE(irq_table_init = 0xd3c9);
    PROVIDE(irq_enable = 0xd641);
    PROVIDE(irq_disable = 0xd65d);
    PROVIDE(irq_set_priority = 0xd685);
    PROVIDE(irq_get_priority = 0xd6e5);
    PROVIDE(irq_set_pending = 0xd705);
    PROVIDE(irq_get_pending = 0xd725);
    PROVIDE(irq_clear_pending = 0xd749);
    PROVIDE(irq_register = 0xd769);
    PROVIDE(irq_unregister = 0xd795);
    PROVIDE(FaultPatch_register = 0xd7b9);
    PROVIDE(WDG_Wait_Busy = 0xd7c9);
    PROVIDE(WDG_StructInit = 0xd7ed);
    PROVIDE(WDG_Init = 0xd7fd);
    PROVIDE(WDG_Enable = 0xd879);
    PROVIDE(WDG_Timeout = 0xd8d1);
    PROVIDE(WDG_Refresh = 0xd939);
    PROVIDE(WDG_INTConfig = 0xd991);
    PROVIDE(WDG_ClearINT = 0xda01);
    PROVIDE(IWDG_LP_Enable = 0xda69);
    PROVIDE(DiagVSprintf = 0xdaa9);
    PROVIDE(DiagPrintf = 0xdd55);
    PROVIDE(DiagPrintfD = 0xdd89);
    PROVIDE(DiagSPrintf = 0xddbd);
    PROVIDE(DiagSnPrintf = 0xddf1);
    PROVIDE(cmd_rom_table = 0xe1ad);
    PROVIDE(_rand = 0xe1b9);
    PROVIDE(__wrap_rand = 0xe215);
    PROVIDE(RandBytes_Get = 0xe219);
    PROVIDE(shell_array_init = 0xe305);
    PROVIDE(shell_get_argc = 0xe313);
    PROVIDE(shell_get_argv = 0xe33f);
    PROVIDE(shell_cmd_chk = 0xe385);
    PROVIDE(shell_uart_irq_rom = 0xe441);
    PROVIDE(shell_init_rom = 0xe505);
    PROVIDE(shell_task_rom = 0xe575);
    PROVIDE(shell_rom = 0xe60d);
    PROVIDE(USBD_CDC_SetTxBuffer = 0xe8fd);
    PROVIDE(USBD_CDC_SetRxBuffer = 0xe915);
    PROVIDE(USBD_CDC_TransmitPacket = 0xe929);
    PROVIDE(USBD_CDC_ReceivePacket = 0xe961);
    PROVIDE(USBD_Core_Init = 0xe985);
    PROVIDE(USBD_Core_DeInit = 0xea01);
    PROVIDE(USBD_IRQHandler = 0xea39);
    PROVIDE(USBD_OpenEP = 0xea5d);
    PROVIDE(USBD_CloseEP = 0xea71);
    PROVIDE(USBD_Transmit = 0xea81);
    PROVIDE(USBD_PrepareReceive = 0xea91);
    PROVIDE(USBD_GetRxDataSize = 0xeaa1);
    PROVIDE(USBD_DataOutStage = 0xeaa9);
    PROVIDE(USBD_DataInStage = 0xeb2d);
    PROVIDE(USBD_Reset = 0xebf1);
    PROVIDE(USBD_SetSpeed = 0xec3d);
    PROVIDE(USBD_Suspend = 0xec45);
    PROVIDE(USBD_Resume = 0xec59);
    PROVIDE(USBD_SOF = 0xec69);
    PROVIDE(USBD_DevConnected = 0xec85);
    PROVIDE(USBD_DevDisconnected = 0xec89);
    PROVIDE(USBD_CtlSendData = 0xeca1);
    PROVIDE(USBD_CtlPrepareRx = 0xecbd);
    PROVIDE(USBD_CtlError = 0xecd9);
    PROVIDE(USBD_SetupStage = 0xecf5);
    PROVIDE(USBD_GetString = 0xf1d1);
    PROVIDE(USBD_DebugInit = 0xf225);
    PROVIDE(USBD_Debug = 0xf271);
    PROVIDE(USBD_Debug_Config = 0xf2b5);
    PROVIDE(USB_HalInit = 0xf2ed);
    PROVIDE(USB_SetTurnaroundTime = 0xf3bd);
    PROVIDE(USB_EnableGlobalInt = 0xf3e5);
    PROVIDE(USB_DisableGlobalInt = 0xf3f9);
    PROVIDE(USB_SetDeviceMode = 0xf40d);
    PROVIDE(USB_EPPredictNextEp = 0xf435);
    PROVIDE(USB_ResetLearnQueue = 0xf439);
    PROVIDE(USB_FlushTxFifo = 0xf461);
    PROVIDE(USB_FlushRxFifo = 0xf49d);
    PROVIDE(USB_SetDevSpeed = 0xf4d5);
    PROVIDE(USB_DevInit = 0xf4e5);
    PROVIDE(USB_GetDevSpeed = 0xf625);
    PROVIDE(USB_GetTxFifoNum = 0xf641);
    PROVIDE(USB_ActivateEndpoint = 0xf651);
    PROVIDE(USB_DeactivateEndpoint = 0xf6dd);
    PROVIDE(USB_EP0StartXfer = 0xf745);
    PROVIDE(USB_WritePacket = 0xf845);
    PROVIDE(USB_EPStartXfer = 0xf86d);
    PROVIDE(USB_ReadPacket = 0xfa01);
    PROVIDE(USB_EPSetStall = 0xfa21);
    PROVIDE(USB_EPClearStall = 0xfa5d);
    PROVIDE(USB_StopDevice = 0xfa99);
    PROVIDE(USB_SetDevAddress = 0xfae5);
    PROVIDE(USB_DevConnect = 0xfb05);
    PROVIDE(USB_DevDisconnect = 0xfb25);
    PROVIDE(USB_ReadInterrupts = 0xfb45);
    PROVIDE(USB_ReadDevAllOutEpInterrupt = 0xfb55);
    PROVIDE(USB_ReadDevAllInEpInterrupt = 0xfb65);
    PROVIDE(USB_ReadDevOutEPInterrupt = 0xfb75);
    PROVIDE(USB_ReadDevInEPInterrupt = 0xfb8d);
    PROVIDE(USB_ClearInterrupts = 0xfba5);
    PROVIDE(USB_GetMode = 0xfbb5);
    PROVIDE(USB_ActivateSetup = 0xfbc5);
    PROVIDE(USB_EP0_OutStart = 0xfbfd);
    PROVIDE(USBD_IsAttached = 0xff31);
    PROVIDE(USBD_Process = 0xff3d);
    PROVIDE(USBD_Init = 0xffb1);
    PROVIDE(USBD_DeInit = 0xffed);
    PROVIDE(USB_PCD_Init = 0xfff5);
    PROVIDE(USB_PCD_Start = 0x101ed);
    PROVIDE(USB_PCD_Stop = 0x10215);
    PROVIDE(USB_PCD_DeInit = 0x10251);
    PROVIDE(USB_PCD_DevConnect = 0x10291);
    PROVIDE(USB_PCD_DevDisconnect = 0x102b5);
    PROVIDE(USB_PCD_SetAddress = 0x102d9);
    PROVIDE(USB_PCD_EP_Open = 0x10301);
    PROVIDE(USB_PCD_EP_Close = 0x10379);
    PROVIDE(USB_PCD_EP_Receive = 0x103cd);
    PROVIDE(USB_PCD_EP_GetRxCount = 0x1041d);
    PROVIDE(USB_PCD_EP_Transmit = 0x1042d);
    PROVIDE(USB_PCD_EP_SetStall = 0x10481);
    PROVIDE(USB_PCD_EP_ClrStall = 0x104e1);
    PROVIDE(USB_PCD_EP_Flush = 0x1053d);
    PROVIDE(USB_PCD_GetState = 0x10579);
    PROVIDE(USB_PCD_SetTxFiFo = 0x10581);
    PROVIDE(USB_PCD_SetRxFiFo = 0x105d9);
    PROVIDE(USB_PCD_IRQHandler = 0x105e5);
    PROVIDE(xmodem_img_rxbuffer = 0x10d79);
    PROVIDE(xmodem_uart_init = 0x11535);
    PROVIDE(xmodem_uart_port_init = 0x115ad);
    PROVIDE(xmodem_uart_port_deinit = 0x115c5);
    PROVIDE(xmodem_uart_readable = 0x115dd);
    PROVIDE(xmodem_uart_writable = 0x115e5);
    PROVIDE(xmodem_uart_getc = 0x115f5);
    PROVIDE(xmodem_uart_putc = 0x11601);
    PROVIDE(xmodem_uart_putdata = 0x1160d);
    PROVIDE(xmodem_uart_getc_to = 0x11629);
    PROVIDE(xmodem_uart_clean_rx = 0x11655);
    PROVIDE(xmodem_uart_waitbusy = 0x11671);
    PROVIDE(xmodem_uart_setbaud = 0x11689);
    PROVIDE(xmodem_img_write = 0x116bd);
    PROVIDE(xmodem_vefiry_floader = 0x11749);
    PROVIDE(xmodem_pll_clkset = 0x11891);
    PROVIDE(xmodem_soc_clkset = 0x118e5);
    PROVIDE(xmodem_img_download = 0x11925);
    PROVIDE(_char2num = 0x11a19);
    PROVIDE(_2char2dec = 0x11a49);
    PROVIDE(_2char2hex = 0x11abd);
    PROVIDE(_memchr = 0x11b3d);
    PROVIDE(_memcmp = 0x11bbd);
    PROVIDE(_memcpy = 0x11c1d);
    PROVIDE(_memmove = 0x11ced);
    PROVIDE(_memset = 0x11de5);
    PROVIDE(_strcmp = 0x11e89);
    PROVIDE(_stricmp = 0x11f15);
    PROVIDE(_strlen = 0x11f69);
    PROVIDE(_strtoul = 0x11fc5);
    PROVIDE(_strupr = 0x120f1);
    PROVIDE(__wrap_memset = 0x1210d);
    PROVIDE(__wrap_memcpy = 0x12115);
    PROVIDE(__wrap_memcmp = 0x1211d);
    PROVIDE(__wrap_memchr = 0x12125);
    PROVIDE(__wrap_memmove = 0x1212d);
    PROVIDE(__wrap_strlen = 0x12135);
    PROVIDE(__wrap_strcmp = 0x1213d);
    PROVIDE(__wrap_stricmp = 0x12145);
    PROVIDE(__wrap_strupr = 0x1214d);
    PROVIDE(__wrap_strtoul = 0x12155);
    PROVIDE(mbedtls_asn1_get_len = 0x1215d);
    PROVIDE(mbedtls_asn1_get_tag = 0x121f1);
    PROVIDE(mbedtls_asn1_get_bool = 0x12219);
    PROVIDE(mbedtls_asn1_get_int = 0x12249);
    PROVIDE(mbedtls_asn1_get_mpi = 0x12291);
    PROVIDE(mbedtls_asn1_get_bitstring = 0x122b7);
    PROVIDE(mbedtls_asn1_get_bitstring_null = 0x122f7);
    PROVIDE(mbedtls_asn1_get_sequence_of = 0x1231f);
    PROVIDE(mbedtls_asn1_get_alg = 0x123a1);
    PROVIDE(mbedtls_asn1_get_alg_null = 0x12437);
    PROVIDE(mbedtls_asn1_free_named_data = 0x12467);
    PROVIDE(mbedtls_asn1_free_named_data_list = 0x12491);
    PROVIDE(mbedtls_asn1_find_named_data = 0x124b5);
    PROVIDE(mbedtls_asn1_write_len = 0x124dd);
    PROVIDE(mbedtls_asn1_write_tag = 0x125c5);
    PROVIDE(mbedtls_asn1_write_raw_buffer = 0x125e1);
    PROVIDE(mbedtls_asn1_write_mpi = 0x1260b);
    PROVIDE(mbedtls_asn1_write_null = 0x1268d);
    PROVIDE(mbedtls_asn1_write_oid = 0x126b3);
    PROVIDE(mbedtls_asn1_write_algorithm_identifier = 0x126e9);
    PROVIDE(mbedtls_asn1_write_bool = 0x1273b);
    PROVIDE(mbedtls_asn1_write_int = 0x12785);
    PROVIDE(mbedtls_asn1_write_tagged_string = 0x127eb);
    PROVIDE(mbedtls_asn1_write_utf8_string = 0x1282b);
    PROVIDE(mbedtls_asn1_write_printable_string = 0x1283d);
    PROVIDE(mbedtls_asn1_write_ia5_string = 0x1284f);
    PROVIDE(mbedtls_asn1_write_bitstring = 0x12861);
    PROVIDE(mbedtls_asn1_write_octet_string = 0x128eb);
    PROVIDE(mbedtls_asn1_store_named_data = 0x12921);
    PROVIDE(mbedtls_mpi_init = 0x13457);
    PROVIDE(mbedtls_mpi_free = 0x13463);
    PROVIDE(mbedtls_mpi_grow = 0x13491);
    PROVIDE(mbedtls_mpi_shrink = 0x134e9);
    PROVIDE(mbedtls_mpi_copy = 0x1355d);
    PROVIDE(mbedtls_mpi_swap = 0x135b5);
    PROVIDE(mbedtls_mpi_safe_cond_assign = 0x135e1);
    PROVIDE(mbedtls_mpi_safe_cond_swap = 0x1364b);
    PROVIDE(mbedtls_mpi_lset = 0x136cd);
    PROVIDE(mbedtls_mpi_get_bit = 0x13705);
    PROVIDE(mbedtls_mpi_set_bit = 0x13727);
    PROVIDE(mbedtls_mpi_lsb = 0x13779);
    PROVIDE(mbedtls_mpi_bitlen = 0x137ad);
    PROVIDE(mbedtls_mpi_size = 0x137e3);
    PROVIDE(mbedtls_mpi_read_binary = 0x137ef);
    PROVIDE(mbedtls_mpi_write_binary = 0x1385b);
    PROVIDE(mbedtls_mpi_shift_l = 0x138c7);
    PROVIDE(mbedtls_mpi_shift_r = 0x13967);
    PROVIDE(mbedtls_mpi_cmp_abs = 0x139db);
    PROVIDE(mbedtls_mpi_cmp_mpi = 0x13a41);
    PROVIDE(mbedtls_mpi_lt_mpi_ct = 0x13ac3);
    PROVIDE(mbedtls_mpi_cmp_int = 0x13b53);
    PROVIDE(mbedtls_mpi_add_abs = 0x13b7f);
    PROVIDE(mbedtls_mpi_sub_abs = 0x13c2f);
    PROVIDE(mbedtls_mpi_add_mpi = 0x13ccb);
    PROVIDE(mbedtls_mpi_sub_mpi = 0x13d0f);
    PROVIDE(mbedtls_mpi_add_int = 0x13d55);
    PROVIDE(mbedtls_mpi_sub_int = 0x13d81);
    PROVIDE(mbedtls_mpi_mul_mpi = 0x13dad);
    PROVIDE(mbedtls_mpi_mul_int = 0x13e75);
    PROVIDE(mbedtls_mpi_read_string = 0x13e8f);
    PROVIDE(mbedtls_mpi_div_mpi = 0x13fb5);
    PROVIDE(mbedtls_mpi_div_int = 0x143bb);
    PROVIDE(mbedtls_mpi_mod_mpi = 0x143e5);
    PROVIDE(mbedtls_mpi_mod_int = 0x14449);
    PROVIDE(mbedtls_mpi_write_string = 0x14483);
    PROVIDE(mbedtls_mpi_exp_mod = 0x14629);
    PROVIDE(mbedtls_mpi_gcd = 0x14a3f);
    PROVIDE(mbedtls_mpi_fill_random = 0x14b47);
    PROVIDE(mbedtls_mpi_inv_mod = 0x14d3f);
    PROVIDE(mbedtls_mpi_is_prime_ext = 0x14ff7);
    PROVIDE(mbedtls_mpi_is_prime = 0x15051);
    PROVIDE(mbedtls_mpi_gen_prime = 0x1505b);
    PROVIDE(mbedtls_ecdh_gen_public = 0x1526d);
    PROVIDE(mbedtls_ecdh_compute_shared = 0x152a1);
    PROVIDE(mbedtls_ecdh_init = 0x152f5);
    PROVIDE(mbedtls_ecdh_setup = 0x1533d);
    PROVIDE(mbedtls_ecdh_free = 0x15351);
    PROVIDE(mbedtls_ecdh_make_params = 0x15399);
    PROVIDE(mbedtls_ecdh_read_params = 0x15401);
    PROVIDE(mbedtls_ecdh_get_params = 0x15441);
    PROVIDE(mbedtls_ecdh_make_public = 0x154a1);
    PROVIDE(mbedtls_ecdh_read_public = 0x154ed);
    PROVIDE(mbedtls_ecdh_calc_secret = 0x15519);
    PROVIDE(mbedtls_ecdsa_sign = 0x159b1);
    PROVIDE(mbedtls_ecdsa_sign_det = 0x159d1);
    PROVIDE(mbedtls_ecdsa_sign_det_ext = 0x159f1);
    PROVIDE(mbedtls_ecdsa_verify = 0x159ff);
    PROVIDE(mbedtls_ecdsa_write_signature_restartable = 0x15a01);
    PROVIDE(mbedtls_ecdsa_write_signature = 0x15aa9);
    PROVIDE(mbedtls_ecdsa_write_signature_det = 0x15ac9);
    PROVIDE(mbedtls_ecdsa_read_signature_restartable = 0x15ae7);
    PROVIDE(mbedtls_ecdsa_read_signature = 0x15b85);
    PROVIDE(mbedtls_ecdsa_genkey = 0x15b97);
    PROVIDE(mbedtls_ecdsa_init = 0x15bbb);
    PROVIDE(mbedtls_ecdsa_free = 0x15bbf);
    PROVIDE(mbedtls_ecdsa_from_keypair = 0x15bc7);
    PROVIDE(mbedtls_ecp_group_load = 0x16919);
    PROVIDE(mbedtls_ecp_curve_list = 0x17795);
    PROVIDE(mbedtls_ecp_grp_id_list = 0x1779d);
    PROVIDE(mbedtls_ecp_curve_info_from_grp_id = 0x177d5);
    PROVIDE(mbedtls_ecp_curve_info_from_tls_id = 0x177ed);
    PROVIDE(mbedtls_ecp_curve_info_from_name = 0x17809);
    PROVIDE(mbedtls_ecp_point_init = 0x17835);
    PROVIDE(mbedtls_ecp_group_init = 0x17851);
    PROVIDE(mbedtls_ecp_keypair_init = 0x17893);
    PROVIDE(mbedtls_ecp_point_free = 0x178af);
    PROVIDE(mbedtls_ecp_group_free = 0x178b5);
    PROVIDE(mbedtls_ecp_keypair_free = 0x17921);
    PROVIDE(mbedtls_ecp_copy = 0x17941);
    PROVIDE(mbedtls_ecp_group_copy = 0x1796d);
    PROVIDE(mbedtls_ecp_set_zero = 0x17973);
    PROVIDE(mbedtls_ecp_is_zero = 0x181b1);
    PROVIDE(mbedtls_ecp_point_cmp = 0x181c3);
    PROVIDE(mbedtls_ecp_point_read_string = 0x181f9);
    PROVIDE(mbedtls_ecp_point_write_binary = 0x18225);
    PROVIDE(mbedtls_ecp_point_read_binary = 0x182c1);
    PROVIDE(mbedtls_ecp_tls_read_point = 0x18331);
    PROVIDE(mbedtls_ecp_tls_write_point = 0x18361);
    PROVIDE(mbedtls_ecp_tls_read_group_id = 0x18391);
    PROVIDE(mbedtls_ecp_tls_read_group = 0x183d5);
    PROVIDE(mbedtls_ecp_tls_write_group = 0x183f1);
    PROVIDE(mbedtls_ecp_check_pubkey = 0x18429);
    PROVIDE(mbedtls_ecp_check_privkey = 0x185b1);
    PROVIDE(mbedtls_ecp_mul_restartable = 0x18625);
    PROVIDE(mbedtls_ecp_mul = 0x18b8d);
    PROVIDE(mbedtls_ecp_muladd_restartable = 0x18c1f);
    PROVIDE(mbedtls_ecp_muladd = 0x18c99);
    PROVIDE(mbedtls_ecp_gen_privkey = 0x18caf);
    PROVIDE(mbedtls_ecp_gen_keypair_base = 0x18dc5);
    PROVIDE(mbedtls_ecp_gen_keypair = 0x18df9);
    PROVIDE(mbedtls_ecp_gen_key = 0x18e11);
    PROVIDE(mbedtls_ecp_check_pub_priv = 0x18e39);
    PROVIDE(mbedtls_hmac_drbg_init = 0x18eed);
    PROVIDE(mbedtls_hmac_drbg_update_ret = 0x18f01);
    PROVIDE(mbedtls_hmac_drbg_update = 0x1906f);
    PROVIDE(mbedtls_hmac_drbg_seed_buf = 0x19073);
    PROVIDE(mbedtls_hmac_drbg_reseed = 0x190bf);
    PROVIDE(mbedtls_hmac_drbg_seed = 0x190c3);
    PROVIDE(mbedtls_hmac_drbg_set_prediction_resistance = 0x1912d);
    PROVIDE(mbedtls_hmac_drbg_set_entropy_len = 0x19131);
    PROVIDE(mbedtls_hmac_drbg_set_reseed_interval = 0x19135);
    PROVIDE(mbedtls_hmac_drbg_random_with_add = 0x19139);
    PROVIDE(mbedtls_hmac_drbg_random = 0x191f9);
    PROVIDE(mbedtls_hmac_drbg_free = 0x19209);
    PROVIDE(mbedtls_md5_init = 0x19225);
    PROVIDE(mbedtls_md5_free = 0x1922d);
    PROVIDE(mbedtls_md5_clone = 0x19237);
    PROVIDE(mbedtls_md5_starts_ret = 0x19241);
    PROVIDE(mbedtls_md5_starts = 0x1926d);
    PROVIDE(mbedtls_internal_md5_process = 0x19271);
    PROVIDE(mbedtls_md5_process = 0x199a9);
    PROVIDE(mbedtls_md5_update_ret = 0x199ad);
    PROVIDE(mbedtls_md5_update = 0x19a27);
    PROVIDE(mbedtls_md5_finish_ret = 0x19a2b);
    PROVIDE(mbedtls_md5_finish = 0x19ad9);
    PROVIDE(mbedtls_md5_ret = 0x19add);
    PROVIDE(mbedtls_md5 = 0x19b39);
    PROVIDE(mbedtls_md_list = 0x19b3d);
    PROVIDE(mbedtls_md_info_from_string = 0x19b45);
    PROVIDE(mbedtls_md_info_from_type = 0x19bc1);
    PROVIDE(mbedtls_md_init = 0x19bfd);
    PROVIDE(mbedtls_md_free = 0x19c07);
    PROVIDE(mbedtls_md_clone = 0x19c45);
    PROVIDE(mbedtls_md_setup = 0x19c6d);
    PROVIDE(mbedtls_md_init_ctx = 0x19cb5);
    PROVIDE(mbedtls_md_starts = 0x19cbb);
    PROVIDE(mbedtls_md_update = 0x19cd1);
    PROVIDE(mbedtls_md_finish = 0x19ce5);
    PROVIDE(mbedtls_md = 0x19cf9);
    PROVIDE(mbedtls_md_hmac_starts = 0x19d19);
    PROVIDE(mbedtls_md_hmac_update = 0x19de1);
    PROVIDE(mbedtls_md_hmac_finish = 0x19e05);
    PROVIDE(mbedtls_md_hmac_reset = 0x19e65);
    PROVIDE(mbedtls_md_hmac = 0x19e95);
    PROVIDE(mbedtls_md_process = 0x19eed);
    PROVIDE(mbedtls_md_get_size = 0x19f01);
    PROVIDE(mbedtls_md_get_type = 0x19f07);
    PROVIDE(mbedtls_md_get_name = 0x19f0d);
    PROVIDE(mbedtls_platform_zeroize = 0x1a02d);
    PROVIDE(mbedtls_sha256_init = 0x1a03d);
    PROVIDE(mbedtls_sha256_free = 0x1a045);
    PROVIDE(mbedtls_sha256_clone = 0x1a04f);
    PROVIDE(mbedtls_sha256_starts_ret = 0x1a059);
    PROVIDE(mbedtls_sha256_starts = 0x1a0fd);
    PROVIDE(mbedtls_internal_sha256_process = 0x1a101);
    PROVIDE(mbedtls_sha256_process = 0x1a803);
    PROVIDE(mbedtls_sha256_update_ret = 0x1a807);
    PROVIDE(mbedtls_sha256_update = 0x1a881);
    PROVIDE(mbedtls_sha256_finish_ret = 0x1a885);
    PROVIDE(mbedtls_sha256_finish = 0x1a98b);
    PROVIDE(mbedtls_sha256_ret = 0x1a98f);
    PROVIDE(mbedtls_sha256 = 0x1a9d1);
    PROVIDE(mbedtls_sha512_init = 0x1a9d9);
    PROVIDE(mbedtls_sha512_free = 0x1a9e1);
    PROVIDE(mbedtls_sha512_clone = 0x1a9eb);
    PROVIDE(mbedtls_sha512_starts_ret = 0x1a9f5);
    PROVIDE(mbedtls_sha512_starts = 0x1ab29);
    PROVIDE(mbedtls_internal_sha512_process = 0x1ab2d);
    PROVIDE(mbedtls_sha512_process = 0x1b695);
    PROVIDE(mbedtls_sha512_update_ret = 0x1b699);
    PROVIDE(mbedtls_sha512_update = 0x1b72b);
    PROVIDE(mbedtls_sha512_finish_ret = 0x1b72f);
    PROVIDE(mbedtls_sha512_finish = 0x1b967);
    PROVIDE(mbedtls_sha512_ret = 0x1b96b);
    PROVIDE(mbedtls_sha512 = 0x1b9ad);
    PROVIDE(__rom_entry_ns_start__ = 0x42000);
    PROVIDE(SPIC_CALIB_PATTERN = 0x4484c);
    PROVIDE(GPIO_PORTx_S = 0x44854);
    PROVIDE(GPIO_PORTx = 0x4485c);
    PROVIDE(USBD_CDC = 0x44a6c);
    PROVIDE(BAUDRATE_TABLE = 0x44ac8);
    PROVIDE(mbedtls_sha512_info = 0x45ca0);
    PROVIDE(mbedtls_sha384_info = 0x45cd0);
    PROVIDE(mbedtls_sha256_info = 0x45d00);
    PROVIDE(mbedtls_sha224_info = 0x45d30);
    PROVIDE(mbedtls_md5_info = 0x45d60);
    PROVIDE(__nspe_phy_addr__ = 0x8014000);
    PROVIDE(__km4_image3_flash_start__ = 0x81e0000);
    PROVIDE(__km0_flash_text_start__ = 0xc000020);
    PROVIDE(__km4_flash_text_start__ = 0xe000020);
    PROVIDE(__km4_boot_text_start__ = 0xf800020);
    PROVIDE(__rom_bss_start__ = 0x20004000);
    PROVIDE(ConfigDebugClose = 0x20004000);
    PROVIDE(ConfigDebug = 0x20004004);
    PROVIDE(flash_init_para = 0x20004014);
    PROVIDE(shell_argv_array = 0x2000406c);
    PROVIDE(shell_rxbuf = 0x200040ac);
    PROVIDE(shell_ctl = 0x2000412c);
    PROVIDE(shell_buf = 0x20004144);
    PROVIDE(SHA_OTPKEY = 0x200041e0);
    PROVIDE(crypto_engine = 0x20004200);
    PROVIDE(crypto_sha_engine = 0x20004440);
    PROVIDE(RBSS_UDELAY_CLK = 0x200046dc);
    PROVIDE(PortA_IrqHandler = 0x200046e0);
    PROVIDE(PortA_IrqData = 0x20004760);
    PROVIDE(PortB_IrqHandler = 0x200047e0);
    PROVIDE(PortB_IrqData = 0x20004860);
    PROVIDE(__rom_bss_chk__ = 0x200048e0);
    PROVIDE(__rom_bss_end__ = 0x200048e0);
    PROVIDE(__km0_ipc_memory_start__ = 0x20004e00);
    PROVIDE(__km0_image2_entry_func__ = 0x20050020);
    PROVIDE(__rom_bss_start_s__ = 0x30007000);
    PROVIDE(NewVectorTable = 0x30007000);
    PROVIDE(FaultPatchHandler = 0x30007180);
    PROVIDE(UserIrqFunTable = 0x30007184);
    PROVIDE(UserIrqDataTable = 0x300072c4);
    PROVIDE(DerivedKey = 0x30007404);
    PROVIDE(USBD_Device = 0x30007898);
    PROVIDE(xmodem_ctrl = 0x30007984);
    PROVIDE(usbd_enabled = 0x30007990);
    PROVIDE(p_rom_ssl_ram_map = 0x30007998);
    PROVIDE(rom_ssl_ram_map = 0x3000799c);
    PROVIDE(__rom_bss_end_s__ = 0x300079a8);
    PROVIDE(STACK_TOP = 0x30009ffc);
    PROVIDE(__ram_start_table_start__ = 0x3000a020);
    PROVIDE(__retention_entry_func__ = 0x41008e00);
    PROVIDE(__km0_psram_text_start__ = 0x60180020);
}
