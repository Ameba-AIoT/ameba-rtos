/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_USERHEAPCFG_H_
#define _AMEBA_USERHEAPCFG_H_

#define configTOTAL_HEAP_SIZE					0 //Fixed to 0, delete later

//HeapRegion Addresses need to be sorted from smallest to largest
#if (!defined (CONFIG_WHC_INTF_IPC) && defined (CONFIG_WHC_DEV))
#define SRAM_HEAP0_START					__bdram_heap_buffer_start__
#define SRAM_HEAP0_SIZE						__bdram_heap_buffer_size__

#else

#if defined(CONFIG_ARM_CORE_CM4_KM4TZ)

#define SRAM_HEAP0_START					__bdram_heap_buffer_start__
#define SRAM_HEAP0_SIZE						__bdram_heap_buffer_size__

#else
#define SRAM_HEAP0_START					__bdram_heap_buffer_start__
#define SRAM_HEAP0_SIZE						__bdram_heap_buffer_size__
#endif

#if defined(CONFIG_ARM_CORE_CM4_KM4TZ)
/* AP(KM4TZ) core owns PSRAM heap, extend to runtime-measured top. When NP places
 * nothing in PSRAM the NP segment collapses to 0, so this spans the whole
 * non-secure PSRAM. */
#define PSRAM_HEAP0_START					__psram_heap_buffer_start__
#define PSRAM_HEAP0_SIZE					(ChipInfo_PsramHeapTop() - (u32)__psram_heap_buffer_start__)
#else
/* KM4NS (NP core): static region (0 unless IMG2/Data-Heap on PSRAM), no runtime extension */
#define PSRAM_HEAP0_START					__psram_heap_buffer_start__
#define PSRAM_HEAP0_SIZE					__psram_heap_buffer_size__
#endif
#endif

#endif

