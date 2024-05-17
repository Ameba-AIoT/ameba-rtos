/*
 * Copyright (c) 2019-2023 Arm Limited. All rights reserved.
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

#include "flash_device_definition.h"
#include "platform_base_address.h"

static const struct axi_qspi_dev_cfg_t AXI_QSPI_DEV_2_CFG_S = {
    .base = QSPI_CONFIG_BASE_S,
    .scc_base = FPGA_SCC_BASE_S
};
struct axi_qspi_dev_t AXI_QSPI_DEV_2_S = {
    .cfg = &AXI_QSPI_DEV_2_CFG_S,
    .is_initialized = false
};

/* Flash parameters will be configured by the CMSIS layer*/
struct spi_sst26vf064b_dev_t SPI_SST26VF064B_DEV = {
    .controller = &AXI_QSPI_DEV_2_S,
    .total_sector_cnt = 0,
    .page_size = 0,
    .sector_size = 0,
    .program_unit = 0,
    .is_initialized = false
};
