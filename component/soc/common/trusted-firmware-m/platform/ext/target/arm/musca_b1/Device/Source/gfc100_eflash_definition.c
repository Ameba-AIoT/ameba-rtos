/*
 * Copyright (c) 2023 Arm Limited. All rights reserved.
 *
 * Licensed under the Apache License Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "device_cfg.h"
#include "gfc100_eflash_drv.h"
#include "platform_base_address.h"

/* ======= Peripheral configuration structure definitions ======= */
/** GFC-100 eflash driver structures */
#ifdef GFC100_EFLASH0_S
static const struct gfc100_eflash_dev_cfg_t GFC100_EFLASH0_CFG_S = {
    .base = MUSCA_B1_EFLASH0_REG_MAP_S_BASE};
static struct gfc100_eflash_dev_data_t GFC100_EFLASH0_DATA_S = {
    .is_initialized = false,
    .flash_size = 0};
struct gfc100_eflash_dev_t GFC100_EFLASH0_DEV_S = {&(GFC100_EFLASH0_CFG_S),
                                                   &(GFC100_EFLASH0_DATA_S)};
#endif

#ifdef GFC100_EFLASH1_S
static const struct gfc100_eflash_dev_cfg_t GFC100_EFLASH1_CFG_S = {
    .base = MUSCA_B1_EFLASH1_REG_MAP_S_BASE};
static struct gfc100_eflash_dev_data_t GFC100_EFLASH1_DATA_S = {
    .is_initialized = false,
    .flash_size = 0};
struct gfc100_eflash_dev_t GFC100_EFLASH1_DEV_S = {&(GFC100_EFLASH1_CFG_S),
                                                   &(GFC100_EFLASH1_DATA_S)};
#endif
