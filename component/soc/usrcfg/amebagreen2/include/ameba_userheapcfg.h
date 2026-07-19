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

#define SRAM_HEAP1_START					__retrieve_ap_buffer_start__
#define SRAM_HEAP1_SIZE						__retrieve_ap_buffer_size__

#else

#define SRAM_HEAP0_START					__bdram_heap_buffer_start__
#define SRAM_HEAP0_SIZE						__bdram_heap_buffer_size__

#if defined(CONFIG_ARM_CORE_CM4_KM4TZ)
/* AP(TZ) core owns the last PSRAM segment plus all capacity up to the
 * runtime-measured boundary. Register the whole [image_end, PsramHeapTop) as
 * one region so it extends beyond the PSRAM_END window. The static
 * __psram_heap_buffer_size__ is used only as a "this core has PSRAM heap" flag
 * (0 only when PSRAM is disabled). */
#define PSRAM_HEAP0_START					__psram_heap_buffer_start__
#define PSRAM_HEAP0_SIZE					((u32)__psram_heap_buffer_size__ ? (ChipInfo_PsramHeapTop() - (u32)__psram_heap_buffer_start__) : 0)
#else
/* NP core: fixed image region, static tail only, no runtime extension. */
#define PSRAM_HEAP0_START					__psram_heap_buffer_start__
#define PSRAM_HEAP0_SIZE					__psram_heap_buffer_size__
#endif
#endif

#endif

