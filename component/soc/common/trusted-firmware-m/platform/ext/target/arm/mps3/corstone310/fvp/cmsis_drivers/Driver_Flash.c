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
#include "Driver_Flash_Emulated.h"
#include "RTE_Device.h"
#include "platform_base_address.h"

#define FLASH0_BASE_S         QSPI_SRAM_BASE_S
#define FLASH0_BASE_NS        QSPI_SRAM_BASE_NS
#define FLASH0_SIZE           QSPI_SRAM_SIZE
#define FLASH0_SECTOR_SIZE    0x00010000 /* 64 kB */
#define FLASH0_PAGE_SIZE      0x00001000 /* 4 kB */
#define FLASH0_PROGRAM_UNIT   0x1        /* Minimum write size */

#if (RTE_FLASH0)
static ARM_FLASH_INFO ARM_FLASH0_DEV_DATA = {
    .sector_info  = NULL,                  /* Uniform sector layout */
    .sector_count = FLASH0_SIZE / FLASH0_SECTOR_SIZE,
    .sector_size  = FLASH0_SECTOR_SIZE,
    .page_size    = FLASH0_PAGE_SIZE,
    .program_unit = FLASH0_PROGRAM_UNIT,
    .erased_value = ARM_FLASH_DRV_ERASE_VALUE
};

static struct emulated_flash_dev_t ARM_FLASH0_DEV = {
#if (defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U))
    .memory_base_s = FLASH0_BASE_S,
    .memory_base_ns = FLASH0_BASE_NS,
#else
    .memory_base_ns = FLASH0_BASE_NS,
#endif /* __ARM_FEATURE_CMSE == 1 */
    .data        = &ARM_FLASH0_DEV_DATA
};

ARM_FLASH_EMULATED(ARM_FLASH0_DEV, Driver_FLASH0);

#endif /* RTE_FLASH0 */
