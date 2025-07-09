/*
* Copyright (c) 2024 Realtek Semiconductor Corp.
*
* SPDX-License-Identifier: Apache-2.0
*/

#ifndef _AMEBA_USERHEAPCFG_H_
#define _AMEBA_USERHEAPCFG_H_

#define configTOTAL_HEAP_SIZE					0 //Fixed to 0, delete later

//HeapRegion Addresses need to be sorted from smallest to largest
#if defined(CONFIG_WIFI_HOST_CONTROL)
#define SRAM_HEAP0_START					__bdram_heap_buffer_start__
#define SRAM_HEAP0_SIZE						__bdram_heap_buffer_size__

#define SRAM_HEAP1_START					__retrieve_ap_buffer_start__
#define SRAM_HEAP1_SIZE						__retrieve_ap_buffer_size__

#else

#if defined(CONFIG_ARM_CORE_CM4_KM4TZ)
#define SRAM_HEAP0_START					__bdram_heap_buffer_start__
#define SRAM_HEAP0_SIZE						__bdram_heap_buffer_size__
#else
#define SRAM_HEAP0_START					__bdram_heap_buffer_start__
#define SRAM_HEAP0_SIZE						__bdram_heap_buffer_size__
#endif

#define PSRAM_HEAP0_START					__psram_heap_buffer_start__
#define PSRAM_HEAP0_SIZE					__psram_heap_buffer_size__

#define PSRAM_HEAP1_START					__psram_heap_extend_start__
#define PSRAM_HEAP1_SIZE					__psram_heap_extend_size__
#endif

#endif

