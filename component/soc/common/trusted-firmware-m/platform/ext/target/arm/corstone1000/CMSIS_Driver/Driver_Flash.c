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

#include <string.h>
#include <stdint.h>
#include "Driver_Flash.h"
#include "RTE_Device.h"
#include "platform_base_address.h"
#include "flash_layout.h"
#include "cmsis_driver_config.h"

#if PLATFORM_IS_FVP
#include "Driver_Flash_Strata.h"
#else
#include "Driver_Flash_N25Q256A.h"
#include "Driver_Flash_SST26VF064B.h"
#endif

#if PLATFORM_IS_FVP

#if (RTE_FLASH0)
static ARM_FLASH_INFO ARM_FLASH0_DEV_DATA = {
    .sector_info    = NULL,     /* Uniform sector layout */
    .sector_count   = STRATA_NVM_FLASH_TOTAL_SIZE / STRATA_NVM_FLASH_SECTOR_SIZE,
    .sector_size    = STRATA_NVM_FLASH_SECTOR_SIZE,
    .page_size      = STRATA_NVM_FLASH_PAGE_SIZE,
    .program_unit   = STRATA_NVM_FLASH_PROGRAM_UNIT,
    .erased_value   = ARM_FLASH_DRV_ERASE_VALUE
};

static struct arm_strata_flash_dev_t ARM_FLASH0_DEV = {
    .dev    = &FLASH0_DEV,
    .data   = &ARM_FLASH0_DEV_DATA
};
#endif /* RTE_FLASH0 */

#if (RTE_FLASH1)
static ARM_FLASH_INFO ARM_FLASH1_DEV_DATA = {
    .sector_info    = NULL,     /* Uniform sector layout */
    .sector_count   = STRATA_SE_FLASH_TOTAL_SIZE / STRATA_SE_FLASH_SECTOR_SIZE,
    .sector_size    = STRATA_SE_FLASH_SECTOR_SIZE,
    .page_size      = STRATA_SE_FLASH_PAGE_SIZE,
    .program_unit   = STRATA_SE_FLASH_PROGRAM_UNIT,
    .erased_value   = ARM_FLASH_DRV_ERASE_VALUE
};

static struct arm_strata_flash_dev_t ARM_FLASH1_DEV = {
    .dev    = &FLASH1_DEV,
    .data   = &ARM_FLASH1_DEV_DATA
};
#endif /* RTE_FLASH1 */

#else /* PLATFORM_IS_FVP */

#if (RTE_FLASH0)
static struct arm_n25q256a_flash_dev_t ARM_FLASH0_DEV = {
    .dev    = &FLASH0_DEV,
    .data   = &N25Q256A_DEV_DATA
};
#endif /* RTE_FLASH0 */

#if (RTE_FLASH1)
static struct arm_flash_sst26vf064b_flash_dev_t ARM_FLASH1_DEV = {
    .dev    = &FLASH1_DEV,
    .data   = &SST26VF064B_DEV_DATA,
    .setup_qspi = NULL,
    .release_qspi = NULL,
    .memory_base_s = 0,
    .memory_base_ns = 0,
};
#endif /* RTE_FLASH1 */
#endif /* PLATFORM_IS_FVP */


/*
 * Functions
 */
#if PLATFORM_IS_FVP

#if (RTE_FLASH0)

int32_t Select_XIP_Mode_For_Shared_Flash(void)
{
    return ARM_DRIVER_OK;
}

int32_t Select_Write_Mode_For_Shared_Flash(void)
{
    return ARM_DRIVER_OK;
}

ARM_FLASH_STRATA(ARM_FLASH0_DEV, Driver_FLASH0);

#endif /* RTE_FLASH0 */

#if (RTE_FLASH1)

ARM_FLASH_STRATA(ARM_FLASH1_DEV, Driver_FLASH1);

#endif /* RTE_FLASH1 */

#else /* PLATFORM_IS_FVP */

#if (RTE_FLASH0)

int32_t Select_XIP_Mode_For_Shared_Flash(void)
{
    select_xip_mode(&AXI_QSPI_DEV_S);
    return ARM_DRIVER_OK;
}

int32_t Select_Write_Mode_For_Shared_Flash(void)
{
    select_qspi_mode(&AXI_QSPI_DEV_S);
    return ARM_DRIVER_OK;
}

ARM_FLASH_N25Q256A(ARM_FLASH0_DEV, Driver_FLASH0);

#endif /* RTE_FLASH0 */

#if (RTE_FLASH1)

ARM_FLASH_SST26VF064B(ARM_FLASH1_DEV, Driver_FLASH1);

#endif /* RTE_FLASH1 */

#endif /* PLATFORM_IS_FVP */
