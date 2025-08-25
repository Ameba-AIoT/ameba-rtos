/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_USERHEAPCFG_H_
#define _AMEBA_USERHEAPCFG_H_

#define configTOTAL_HEAP_SIZE					0 //Fixed to 0, delete later

//HeapRegion Addresses need to be sorted from smallest to largest
#define SRAM_HEAP0_START					__bdram_heap_buffer_start__
#define SRAM_HEAP0_SIZE						__bdram_heap_buffer_size__

#define PSRAM_HEAP0_START					__psram_heap_buffer_start__
#define PSRAM_HEAP0_SIZE					__psram_heap_buffer_size__

#define PSRAM_HEAP1_START					__psram_heap_extend_start__
#define PSRAM_HEAP1_SIZE					__psram_heap_extend_size__

#if (defined CONFIG_ARM_CORE_CM4)

#ifdef CONFIG_MP_SHRINK
#define SRAM_HEAP2_START					__sram_floader_mp_start__
#define SRAM_HEAP2_SIZE					    __sram_floader_mp_size__
#endif

#elif (defined CONFIG_ARM_CORE_CM0) // #if (defined CONFIG_ARM_CORE_CM4)

#ifdef CONFIG_MP_SHRINK
#define SRAM_HEAP2_START					__sram_bootloader_mp_start__
#define SRAM_HEAP2_SIZE					    __sram_bootloader_mp_size__
#endif

#endif // #if (defined CONFIG_ARM_CORE_CM4)

#endif

