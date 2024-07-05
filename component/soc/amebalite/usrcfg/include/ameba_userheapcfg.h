/*
* Copyright (c) 2024 Realtek Semiconductor Corp.
*
* SPDX-License-Identifier: Apache-2.0
*/

#ifndef _AMEBA_USERHEAPCFG_H_
#define _AMEBA_USERHEAPCFG_H_

#define configTOTAL_HEAP_SIZE					0 //Fixed to 0, delete later

//HeapRegion Addresses need to be sorted from smallest to largest

#if (defined ARM_CORE_CM4)

#ifdef CONFIG_MP_SHRINK
/* Recycle floader mem for km4 */
#define SRAM_HEAP2_START					__sram_floader_mp_start__
#define SRAM_HEAP2_SIZE					    __sram_floader_mp_size__
#endif

#define SRAM_HEAP0_START					__bdram_heap_buffer_start__
#define SRAM_HEAP0_SIZE						__bdram_heap_buffer_size__

#define PSRAM_HEAP0_START					__psram_heap_buffer_start__
#define PSRAM_HEAP0_SIZE					__psram_heap_buffer_size__

#define PSRAM_HEAP1_START					__psram_heap_extend_start__
#define PSRAM_HEAP1_SIZE					__psram_heap_extend_size__

#elif (defined RSICV_CORE_KR4)

#ifdef CONFIG_MP_SHRINK
/* Recycle bootloader mem for kr4 */
#define SRAM_HEAP2_START					__sram_bootloader_mp_start__
#define SRAM_HEAP2_SIZE					    __sram_bootloader_mp_size__
#endif

#define SRAM_HEAP0_START					__kr4_sram_heap_extend_start__
#define SRAM_HEAP0_SIZE						__kr4_sram_heap_extend_size__

#define SRAM_HEAP1_START					__bdram_heap_buffer_start__
#define SRAM_HEAP1_SIZE						__bdram_heap_buffer_size__

#define PSRAM_HEAP0_START					__psram_heap_buffer_start__
#define PSRAM_HEAP0_SIZE					__psram_heap_buffer_size__

#define PSRAM_HEAP1_START					__psram_heap_extend_start__
#define PSRAM_HEAP1_SIZE					__psram_heap_extend_size__
#endif

#endif

