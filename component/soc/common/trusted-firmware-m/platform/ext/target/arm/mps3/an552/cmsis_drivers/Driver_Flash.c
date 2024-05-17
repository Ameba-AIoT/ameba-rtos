/*
 * Copyright (c) 2013-2023 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <string.h>
#include <stdint.h>
#include "Driver_Flash.h"
#include "RTE_Device.h"
#include "platform_base_address.h"
#include "emulated_flash_drv.h"
#include "Driver_Flash_Emulated.h"

#define FLASH0_BASE_S         SRAM_BASE_S
#define FLASH0_BASE_NS        SRAM_BASE_NS
#define FLASH0_SIZE           SRAM_SIZE
#define FLASH0_SECTOR_SIZE    0x00001000 /* 4 kB */
#define FLASH0_PAGE_SIZE      0x00001000 /* 4 kB */
#define FLASH0_PROGRAM_UNIT   0x1        /* Minimum write size */

#define FLASH1_BASE_S         ISRAM1_BASE_S
#define FLASH1_BASE_NS        ISRAM1_BASE_NS
#define FLASH1_SIZE           ISRAM1_SIZE
#define FLASH1_SECTOR_SIZE    0x00001000 /* 4 kB */
#define FLASH1_PAGE_SIZE      0x00001000 /* 4 kB */
#define FLASH1_PROGRAM_UNIT   0x1        /* Minimum write size */


#if (defined (RTE_SRAM) && (RTE_SRAM == 1)) || \
    (defined (RTE_ISRAM1) && (RTE_ISRAM1 == 1))

static ARM_FLASH_INFO ARM_FLASH0_DEV_INFO = {
    .sector_info  = NULL,                  /* Uniform sector layout */
    .sector_count = FLASH0_SIZE / FLASH0_SECTOR_SIZE,
    .sector_size  = FLASH0_SECTOR_SIZE,
    .page_size    = FLASH0_PAGE_SIZE,
    .program_unit = FLASH0_PROGRAM_UNIT,
    .erased_value = EMULATED_FLASH_DRV_ERASE_VALUE
};

static struct emulated_flash_dev_t ARM_FLASH0_DEV = {
#if (defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U))
    .memory_base_s = FLASH0_BASE_S,
    .memory_base_ns = FLASH0_BASE_NS,
#else
    .memory_base_ns = FLASH0_BASE_NS,
#endif /* __ARM_FEATURE_CMSE == 1 */
    .data        = &ARM_FLASH0_DEV_INFO
};

ARM_FLASH_EMULATED(ARM_FLASH0_DEV, Driver_FLASH0);

#endif /* RTE_SRAM */

#if (defined (RTE_ISRAM1) && (RTE_ISRAM1 == 1))

static ARM_FLASH_INFO ARM_FLASH1_DEV_INFO = {
    .sector_info  = NULL,                  /* Uniform sector layout */
    .sector_count = FLASH1_SIZE / FLASH1_SECTOR_SIZE,
    .sector_size  = FLASH1_SECTOR_SIZE,
    .page_size    = FLASH1_PAGE_SIZE,
    .program_unit = FLASH1_PROGRAM_UNIT,
    .erased_value = EMULATED_FLASH_DRV_ERASE_VALUE
};

static struct emulated_flash_dev_t ARM_FLASH1_DEV = {
#if (defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U))
    .memory_base_s = FLASH1_BASE_S,
    .memory_base_ns = FLASH1_BASE_NS,
#else
    .memory_base_ns = FLASH1_BASE_NS,
#endif /* __ARM_FEATURE_CMSE == 1 */
    .data        = &ARM_FLASH1_DEV_INFO
};

ARM_FLASH_EMULATED(ARM_FLASH1_DEV, Driver_FLASH1);

#endif /* RTE_ISRAM1 */
