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

#ifdef CONFIG_IMG3_SRAM
	#define LMA_TZ_NSC 		KM4_BD_RAM_TZ_NSC
	#define LMA_TZ_ENTRY 	KM4_BD_RAM_TZ_ENTRY
	#define LMA_TZ_S		KM4_BD_RAM_TZ_S
#else
	#define LMA_TZ_NSC 		KM4_BD_PSRAM_TZ_NSC
	#define LMA_TZ_ENTRY 	KM4_BD_PSRAM_TZ_ENTRY
	#define LMA_TZ_S		KM4_BD_PSRAM_TZ_S
#endif
/* Include file with definitions for section alignments.
 * Note: it should be included after region_defs.h to let platform define
 * default values if needed. */
#include "tfm_s_linker_alignments.h"

	__ram_image3_nsc_start__ = ORIGIN(LMA_TZ_NSC);
	__ram_image3_text_start__ = ORIGIN(LMA_TZ_S);

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
} > LMA_TZ_NSC \
/* GCC always places veneers at the end of .gnu.sgstubs section, so the only \
 * way to align the end of .gnu.sgstubs section is to align start of the \
 * next section */ \
.sgstubs_end : TFM_LINKER_VENEERS_END \
{ \
} > LMA_TZ_NSC

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
    } > LMA_TZ_ENTRY

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
    } > LMA_TZ_S

    ASSERT(__vectors_start__ == ORIGIN(LMA_TZ_S), "__vectors_start__ shall equal to __ram_image3_start__")

    /**** Section for holding partition RO load data */
    /*
     * Sort the partition info by priority to guarantee the initing order.
     * The first loaded partition will be inited at last in SFN model.
     */
    .TFM_SP_LOAD_LIST ALIGN(4) :
    {
       KEEP(*(.part_load_priority_lowest))
       KEEP(*(.part_load_priority_low))
       KEEP(*(.part_load_priority_normal))
       KEEP(*(.part_load_priority_high))
    } > LMA_TZ_S
    Image$$TFM_SP_LOAD_LIST$$RO$$Base = ADDR(.TFM_SP_LOAD_LIST);
    Image$$TFM_SP_LOAD_LIST$$RO$$Limit = ADDR(.TFM_SP_LOAD_LIST) + SIZEOF(.TFM_SP_LOAD_LIST);

    /**** PSA RoT RO part (CODE + RODATA) start here */
    . = ALIGN(TFM_LINKER_PSA_ROT_LINKER_CODE_ALIGNMENT);
    Image$$TFM_PSA_CODE_START$$Base = .;

    .TFM_PSA_ROT_LINKER ALIGN(TFM_LINKER_PSA_ROT_LINKER_CODE_ALIGNMENT) :
    {
        *tfm_psa_rot_partition*:(SORT_BY_ALIGNMENT(.text*))
        *tfm_psa_rot_partition*:(SORT_BY_ALIGNMENT(.rodata*))
        *(TFM_*_PSA-ROT_ATTR_FN)
        . = ALIGN(TFM_LINKER_PSA_ROT_LINKER_CODE_ALIGNMENT);
    } > LMA_TZ_S

    Image$$TFM_PSA_ROT_LINKER$$RO$$Base = ADDR(.TFM_PSA_ROT_LINKER);
    Image$$TFM_PSA_ROT_LINKER$$RO$$Limit = ADDR(.TFM_PSA_ROT_LINKER) + SIZEOF(.TFM_PSA_ROT_LINKER);
    Image$$TFM_PSA_ROT_LINKER$$Base = ADDR(.TFM_PSA_ROT_LINKER);
    Image$$TFM_PSA_ROT_LINKER$$Limit = ADDR(.TFM_PSA_ROT_LINKER) + SIZEOF(.TFM_PSA_ROT_LINKER);

    /**** PSA RoT RO part (CODE + RODATA) end here */
    Image$$TFM_PSA_CODE_END$$Base = .;

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > LMA_TZ_S

    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > LMA_TZ_S
    __exidx_end = .;

    .ER_TFM_CODE : ALIGN(4)
    {
        /* .zero.table */
        . = ALIGN(4);
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

        *startup*(.text*)
        *libplatform_s*:(SORT_BY_ALIGNMENT(.text*))
        *libtfm_spm*:(SORT_BY_ALIGNMENT(.text*))

        *libplatform_s*:*(.rodata*)
        *libtfm_spm*:*(.rodata*)
    } > LMA_TZ_S

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
    } > LMA_TZ_S
#endif

#if defined(CONFIG_TFM_PARTITION_META)
	/*------------UNPRIV------------*/
    .TFM_SP_META_PTR (NOLOAD): ALIGN(TFM_LINKER_SP_META_PTR_ALIGNMENT)
    {
        *(.bss.SP_META_PTR_SPRTL_INST)
        . = ALIGN(TFM_LINKER_SP_META_PTR_ALIGNMENT);
    } > LMA_TZ_S
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
    } > LMA_TZ_S 
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
    } > LMA_TZ_S 
    Image$$TFM_APP_ROT_LINKER_DATA$$ZI$$Base = ADDR(.TFM_APP_ROT_LINKER_BSS);
    Image$$TFM_APP_ROT_LINKER_DATA$$ZI$$Limit = ADDR(.TFM_APP_ROT_LINKER_BSS) + SIZEOF(.TFM_APP_ROT_LINKER_BSS);

    /**** APPLICATION RoT DATA end here */
    Image$$TFM_APP_RW_STACK_END$$Base = .;


    /**** PSA RoT DATA start here */

    Image$$TFM_PSA_RW_STACK_START$$Base = .;

    .TFM_PSA_ROT_LINKER_DATA ALIGN(TFM_LINKER_PSA_ROT_LINKER_DATA_ALIGNMENT) :
    {
        *tfm_psa_rot_partition*:(SORT_BY_ALIGNMENT(.data*))
        *(TFM_*_PSA-ROT_ATTR_RW)
        . = ALIGN(4);
    } > LMA_TZ_S 
    Image$$TFM_PSA_ROT_LINKER_DATA$$RW$$Base = ADDR(.TFM_PSA_ROT_LINKER_DATA);
    Image$$TFM_PSA_ROT_LINKER_DATA$$RW$$Limit = ADDR(.TFM_PSA_ROT_LINKER_DATA) + SIZEOF(.TFM_PSA_ROT_LINKER_DATA);

    .TFM_PSA_ROT_LINKER_BSS ALIGN(4) (NOLOAD) :
    {
        start_of_TFM_PSA_ROT_LINKER = .;
        *tfm_psa_rot_partition*:(SORT_BY_ALIGNMENT(.bss*))
        *tfm_psa_rot_partition*:*(COMMON)
        *(TFM_*_PSA-ROT_ATTR_ZI)
        . += (. - start_of_TFM_PSA_ROT_LINKER) ? 0 : 4;
        . = ALIGN(TFM_LINKER_PSA_ROT_LINKER_DATA_ALIGNMENT);
    } > LMA_TZ_S


    Image$$TFM_PSA_ROT_LINKER_DATA$$ZI$$Base = ADDR(.TFM_PSA_ROT_LINKER_BSS);
    Image$$TFM_PSA_ROT_LINKER_DATA$$ZI$$Limit = ADDR(.TFM_PSA_ROT_LINKER_BSS) + SIZEOF(.TFM_PSA_ROT_LINKER_BSS);

    /**** PSA RoT DATA end here */
    Image$$TFM_PSA_RW_STACK_END$$Base = .;



	/*------------UNPRIV------------*/
    .TFM_UNPRIV_CODE : ALIGN(TFM_LINKER_UNPRIV_CODE_ALIGNMENT)
    {
        *(SORT_BY_ALIGNMENT(.text*))
        *(SORT_BY_ALIGNMENT(.rodata*))

        KEEP(*(.eh_frame*))
        . = ALIGN(TFM_LINKER_UNPRIV_CODE_ALIGNMENT);
    } > LMA_TZ_S
    Image$$TFM_UNPRIV_CODE$$RO$$Base = ADDR(.TFM_UNPRIV_CODE);
    Image$$TFM_UNPRIV_CODE$$RO$$Limit = ADDR(.TFM_UNPRIV_CODE) + SIZEOF(.TFM_UNPRIV_CODE);
    /**** APPLICATION RoT RO part (CODE + RODATA) start here */
	/*------------UNPRIV------------*/
    Image$$TFM_APP_CODE_START$$Base = .;

    .TFM_APP_ROT_LINKER Image$$TFM_UNPRIV_CODE$$RO$$Limit : ALIGN(TFM_LINKER_APP_ROT_LINKER_CODE_ALIGNMENT)
    {
        *tfm_app_rot_partition*:(SORT_BY_ALIGNMENT(.text*))
        *tfm_app_rot_partition*:(SORT_BY_ALIGNMENT(.rodata*))
        *(TFM_*_APP-ROT_ATTR_FN)
		
        . = ALIGN(TFM_LINKER_APP_ROT_LINKER_CODE_ALIGNMENT);
    } > LMA_TZ_S
    Image$$TFM_APP_ROT_LINKER$$RO$$Base = ADDR(.TFM_APP_ROT_LINKER);
    Image$$TFM_APP_ROT_LINKER$$RO$$Limit = ADDR(.TFM_APP_ROT_LINKER) + SIZEOF(.TFM_APP_ROT_LINKER);
    Image$$TFM_APP_ROT_LINKER$$Base = ADDR(.TFM_APP_ROT_LINKER);
    Image$$TFM_APP_ROT_LINKER$$Limit = ADDR(.TFM_APP_ROT_LINKER) + SIZEOF(.TFM_APP_ROT_LINKER);

    /**** APPLICATION RoT RO part (CODE + RODATA) end here */
    Image$$TFM_APP_CODE_END$$Base = .;

    ASSERT(Image$$TFM_UNPRIV_CODE$$RO$$Limit + SIZEOF(.TFM_APP_ROT_LINKER) == Image$$TFM_APP_CODE_END$$Base, "TFM_UNPRIV_CODE must be continuous with TFM_APP_CODE_START")

    .TFM_DATA : ALIGN(4)
    {
        *(SORT_BY_ALIGNMENT(.data*))

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

    } > LMA_TZ_S
    Image$$ER_TFM_DATA$$RW$$Base = ADDR(.TFM_DATA);
    Image$$ER_TFM_DATA$$RW$$Limit = ADDR(.TFM_DATA) + SIZEOF(.TFM_DATA);

    .TFM_BSS ALIGN(4) (NOLOAD) :
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
        *(SORT_BY_ALIGNMENT(.bss*))
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
    } > LMA_TZ_S
    Image$$ER_TFM_DATA$$ZI$$Base = ADDR(.TFM_BSS);
    Image$$ER_TFM_DATA$$ZI$$Limit = ADDR(.TFM_BSS) + SIZEOF(.TFM_BSS);
    Image$$ER_PART_RT_POOL$$ZI$$Base = __partition_runtime_start__;
    Image$$ER_PART_RT_POOL$$ZI$$Limit = __partition_runtime_end__;
    Image$$ER_SERV_RT_POOL$$ZI$$Base = __service_runtime_start__;
    Image$$ER_SERV_RT_POOL$$ZI$$Limit = __service_runtime_end__;

    Image$$ER_TFM_DATA$$Base = ADDR(.TFM_DATA);
    Image$$ER_TFM_DATA$$Limit = ADDR(.TFM_DATA) + SIZEOF(.TFM_DATA) + SIZEOF(.TFM_BSS);


    PROVIDE(__stack = Image$$ARM_LIB_STACK$$ZI$$Limit);

    .BSS_UNINIT (NOLOAD): ALIGN(4)
	{
		*(.psa_bss_un_init*)
    } > LMA_TZ_S
}
