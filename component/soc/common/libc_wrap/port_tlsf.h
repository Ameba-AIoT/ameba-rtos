/*
 * SPDX-License-Identifier: LicenseRef-Realtek-Proprietary
 *
 * Copyright (c) 2023, Realtek Semiconductor Corp. All rights reserved.
 *
 * This software is a confidential and proprietary property of Realtek
 * Semiconductor Corp. Disclosure, reproduction, redistribution, in
 * whole or in part, of this work and its derivatives without express
 * permission is prohibited.
 *
 * Realtek Semiconductor Corp. reserves the right to update, modify, or
 * discontinue this software at any time without notice. This software is
 * provided "as is" and any express or implied warranties, including, but
 * not limited to, the implied warranties of merchantability and fitness for
 * a particular purpose are disclaimed. In no event shall Realtek
 * Semiconductor Corp. be liable for any direct, indirect, incidental,
 * special, exemplary, or consequential damages (including, but not limited
 * to, procurement of substitute goods or services; loss of use, data, or
 * profits; or business interruption) however caused and on any theory of
 * liability, whether in contract, strict liability, or tort (including
 * negligence or otherwise) arising in any way out of the use of this software,
 * even if advised of the possibility of such damage.
 */

#ifndef PORT_TLSF_H
#define PORT_TLSF_H

#include "tlsf.h"

/* Provide defaults for Kconfig macros when not defined by build system */
#ifndef CONFIG_HEAP_PROTECTOR
#define CONFIG_HEAP_PROTECTOR 0
#endif

#ifndef CONFIG_HEAP_CORRUPTION_DETECT_LITE
#define CONFIG_HEAP_CORRUPTION_DETECT_LITE 0
#endif

#ifndef CONFIG_HEAP_CORRUPTION_DETECT_COMPREHENSIVE
#define CONFIG_HEAP_CORRUPTION_DETECT_COMPREHENSIVE 0
#endif

#ifndef CONFIG_HEAP_TRACE
#define CONFIG_HEAP_TRACE 0
#endif

#ifndef _DEBUG
#define _DEBUG 0
#endif

/**
 * @brief   Initialize TLSF with heap regions (replaces vPortDefineHeapRegions for TLSF).
 */
void vPortDefineHeapRegions(const HeapRegion_t *pxHeapRegions);

/**
 * @brief   Freertos heap malloc operation.
 *
 * @param   xWantedSize: malloc size in bytes.
 */
void *pvPortMalloc(size_t xWantedSize);

/**
 * @brief   Freertos heap free operation.
 *
 * @param   pv: pointer to the free memory.
 */
void vPortFree(void *pv);

/**
 * @brief   Freertos heap realloc operation.
 *
 * @param   pv: pointer to the memory to reallocate.
 * @param   xWantedSize: new size in bytes.
 */
void *pvPortReAlloc(void *pv, size_t xWantedSize);

/**
 * @brief   Freertos heap malloc with base address hint.
 *
 * @param   xWantedSize: malloc size in bytes.
 * @param   startAddr: base address hint, 0 for any type.
 */
void *pvPortMallocBase(size_t xWantedSize, uint32_t startAddr);

/**
 * @brief   Freertos heap realloc with base address hint.
 *
 * @param   pv: pointer to the memory to reallocate.
 * @param   xWantedSize: new size in bytes.
 * @param   startAddr: base address hint, 0 for any type.
 */
void *pvPortReAllocBase(void *pv, size_t xWantedSize, uint32_t startAddr);

/**
 * @brief   Freertos heap malloc with cache-line alignment.
 *
 * @param   xWantedSize: malloc size in bytes.
 */
void *pvPortMallocCacheAligned(size_t xWantedSize);

#endif /* PORT_TLSF_H */
