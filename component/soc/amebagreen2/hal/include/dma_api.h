/** mbed Microcontroller Library
  ******************************************************************************
  * @file    dma_api.h
  * @author
  * @version V1.0.0
  * @brief   This file provides mbed GDMA API
  ******************************************************************************
  * @attention
  *
  * Copyright(c) 2006 - 2022 Realtek Corporation. All rights reserved.
  *
  * SPDX-License-Identifier: Apache-2.0
  *
  * Licensed under the Apache License, Version 2.0 (the License); you may
  * not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  * http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an AS IS BASIS, WITHOUT
  * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */
#ifndef MBED_GDMA_API_H
#define MBED_GDMA_API_H

#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Ameba_Mbed_API
  * @{
  */

/** @addtogroup MBED_GDMA
 *  @brief      MBED_GDMA driver modules.
 *  @{
 */

/* reserved region for storage enum and structure, used by Doxygen */

/** @addtogroup MBED_GDMA_Exported_Types MBED_GDMA Exported Types
  * @{
  */
/** @defgroup MBED_GDMA_Structure_Type Structure Type
  * @{
  */
/**
  * @brief typedef struct gdma_s to gdma_t
  */
typedef struct gdma_s gdma_t;
/** @} */
/** @} */
/** @} */
/** @} */

/**
  * @brief typedef function pointer to point interrupt handler
  */
typedef u32(*dma_irq_handler)(void *id);

void dma_memcpy_init(gdma_t *dma_obj, dma_irq_handler handler, uint32_t id);
void dma_memcpy_deinit(gdma_t *dma_obj);
void dma_memcpy(gdma_t *dma_obj, void *dst, void *src, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif  // end of "#define MBED_GDMA_API_H"
