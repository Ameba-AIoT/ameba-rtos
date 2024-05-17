/*
 * Copyright (c) 2022-2023 Arm Limited. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "utilities.h"
#include "platform_s_device_definition.h"
#include "platform_base_address.h"
#include "dma350_drv.h"
#include "common_target_cfg.h"

struct platform_data_t tfm_peripheral_dma0_ch0 = {
        DMA_350_BASE_S + 0x1000,
        DMA_350_BASE_S + 0x10FF,
        PPC_SP_DO_NOT_CONFIGURE,
        0
};

struct platform_data_t tfm_peripheral_dma0_ch1 = {
        DMA_350_BASE_S + 0x1100,
        DMA_350_BASE_S + 0x11FF,
        PPC_SP_DO_NOT_CONFIGURE,
        0
};

/*------------------- DMA configuration functions -------------------------*/
enum tfm_plat_err_t dma_init_cfg(void)
{
    enum dma350_error_t dma_err;
    uint32_t i = 0;

    dma_err = dma350_init(&DMA350_DMA0_DEV_S);
    if(dma_err != DMA350_ERR_NONE) {
        ERROR_MSG("DMA350_DMA0_DEV_S init failed!");
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    /* Configure Channel 0 */
    dma_err = dma350_set_ch_secure(&DMA350_DMA0_DEV_S, 0);
    if(dma_err != DMA350_ERR_NONE)
    {
        ERROR_MSG("Failed to set DMA350_DMA0_DEV_S, channel 0 secure!");
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }
    dma_err = dma350_set_ch_privileged(&DMA350_DMA0_DEV_S, 0);
    if(dma_err != DMA350_ERR_NONE)
    {
        ERROR_MSG("Failed to set DMA350_DMA0_DEV_S, channel 0 privileged!");
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    /* Configure Channel 1 */
    dma_err = dma350_set_ch_nonsecure(&DMA350_DMA0_DEV_S, 1);
    if(dma_err != DMA350_ERR_NONE)
    {
        ERROR_MSG("Failed to set DMA350_DMA0_DEV_S, channel 1 nonsecure!");
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }
    dma_err = dma350_set_ch_privileged(&DMA350_DMA0_DEV_S, 1);
    if(dma_err != DMA350_ERR_NONE)
    {
        ERROR_MSG("Failed to set DMA350_DMA0_DEV_S, channel 1 privileged!");
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    /* Configure every Trigger input to NS by default */
    for(i = 0; i < DMA350_TRIGIN_NUMBER; i++)
    {
        dma_err = dma350_set_trigin_nonsecure(&DMA350_DMA0_DEV_S, i);
        if(dma_err != DMA350_ERR_NONE)
        {
            SPMLOG_ERRMSGVAL("Failed to set the following Trigger input of DMA350_DMA0_DEV_S to NS: ", i);
            return TFM_PLAT_ERR_SYSTEM_ERR;
        }
    }

    return TFM_PLAT_ERR_SUCCESS;
}
