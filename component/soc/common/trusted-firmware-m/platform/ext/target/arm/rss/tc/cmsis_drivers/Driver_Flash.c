/*
 * Copyright (c) 2013-2023 Arm Limited. All rights reserved.
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
 */

#include "Driver_Flash_Strata.h"
#include "cmsis_driver_config.h"
#include "RTE_Device.h"

#if (RTE_FLASH0)
static ARM_FLASH_INFO ARM_FLASH0_DEV_DATA = {
    .sector_info    = NULL,     /* Uniform sector layout */
    .sector_count   = HOST_ACCESS_SIZE / 0x1000,
    .sector_size    = 0x1000,
    .page_size      = 256U,
    .program_unit   = 1U,
    .erased_value   = ARM_FLASH_DRV_ERASE_VALUE
};

static struct arm_strata_flash_dev_t ARM_FLASH0_DEV = {
    .dev    = &FLASH0_DEV,
    .data   = &ARM_FLASH0_DEV_DATA
};

ARM_FLASH_STRATA(ARM_FLASH0_DEV, Driver_FLASH0);
#endif /* RTE_FLASH0 */
