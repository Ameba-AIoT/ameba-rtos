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

#ifndef __DEVICE_DEFINITION_H__
#define __DEVICE_DEFINITION_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "device_cfg.h"

/* ======= Defines peripheral configuration structures ======= */

/* SST26VF064B Flash driver structures */
#include "spi_sst26vf064b_flash_lib.h"
extern struct spi_sst26vf064b_dev_t SPI_SST26VF064B_DEV;

#include "xilinx_pg153_axi_qspi_controller_drv.h"
extern struct axi_qspi_dev_t AXI_QSPI_DEV_2_S;

#ifdef __cplusplus
}
#endif

#endif  /* __DEVICE_DEFINITION_H__ */
