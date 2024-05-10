/*
 * Copyright (c) 2022 Arm Limited. All rights reserved.
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

#include "dma350_drv.h"

#include <stddef.h>
#include <stdint.h>


enum dma350_error_t dma350_init(struct dma350_dev_t *dev)
{
    if (dev->cfg->dma_info->IIDR != DMA_INFO_IIDR_SUPPORTED) {
        return DMA350_ERR_IIDR_MISMATCH;
    }
    if (dev->cfg->dma_info->AIDR != DMA_INFO_AIDR_SUPPORTED) {
        return DMA350_ERR_AIDR_MISMATCH;
    }
    dev->data->state = DMA350_INITIALIZED;
    return DMA350_ERR_NONE;
}

void dma350_enable_secaccvio_irq(struct dma350_dev_t *dev)
{
    dev->cfg->dma_sec_cfg->SCFG_CTRL =
        (dev->cfg->dma_sec_cfg->SCFG_CTRL |
         DMA_SCFG_CTRL_INTREN_SECACCVIO_Msk) &
        ~(DMA_SCFG_INTRSTATUS_STAT_SECACCVIO_Msk);
}

void dma350_disable_secaccvio_irq(struct dma350_dev_t *dev)
{
    dev->cfg->dma_sec_cfg->SCFG_CTRL =
        (dev->cfg->dma_sec_cfg->SCFG_CTRL &
         ~(DMA_SCFG_CTRL_INTREN_SECACCVIO_Msk)) &
        ~(DMA_SCFG_INTRSTATUS_STAT_SECACCVIO_Msk);
}

void dma350_set_secaccvio_rsp_buserr(struct dma350_dev_t *dev)
{
    dev->cfg->dma_sec_cfg->SCFG_CTRL =
        (dev->cfg->dma_sec_cfg->SCFG_CTRL |
         DMA_SCFG_CTRL_RSPTYPE_SECACCVIO_Msk) &
        ~(DMA_SCFG_INTRSTATUS_STAT_SECACCVIO_Msk);
}

void dma350_set_secaccvio_rsp_razwi(struct dma350_dev_t *dev)
{
    dev->cfg->dma_sec_cfg->SCFG_CTRL =
        (dev->cfg->dma_sec_cfg->SCFG_CTRL &
         ~(DMA_SCFG_CTRL_RSPTYPE_SECACCVIO_Msk)) &
        ~(DMA_SCFG_INTRSTATUS_STAT_SECACCVIO_Msk);
}

void dma350_lock_security_cfg(struct dma350_dev_t *dev)
{
    dev->cfg->dma_sec_cfg->SCFG_CTRL =
        (dev->cfg->dma_sec_cfg->SCFG_CTRL | DMA_SCFG_CTRL_SEC_CFG_LCK_Msk) &
        ~(DMA_SCFG_INTRSTATUS_STAT_SECACCVIO_Msk);
}

uint8_t dma350_get_secaccvio_irq(struct dma350_dev_t *dev)
{
    return dev->cfg->dma_sec_cfg->SCFG_CTRL &
           DMA_SCFG_INTRSTATUS_INTR_SECACCVIO_Msk;
}

uint8_t dma350_get_secaccvio_status(struct dma350_dev_t *dev)
{
    return (uint8_t)(dev->cfg->dma_sec_cfg->SCFG_CTRL &
           DMA_SCFG_INTRSTATUS_STAT_SECACCVIO_Msk);
}

void dma350_clear_secaccvio_status(struct dma350_dev_t *dev)
{
    dev->cfg->dma_sec_cfg->SCFG_CTRL = dev->cfg->dma_sec_cfg->SCFG_CTRL |
                                       DMA_SCFG_INTRSTATUS_STAT_SECACCVIO_Msk;
}

enum dma350_error_t dma350_set_ch_secure(struct dma350_dev_t *dev,
                                         uint8_t channel)
{
    if (!dma350_is_init(dev)) {
        return DMA350_ERR_NOT_INIT;
    }

    dev->cfg->dma_sec_cfg->SCFG_CHSEC0 =
        dev->cfg->dma_sec_cfg->SCFG_CHSEC0 & ~(0x1UL << channel);
    if (dev->cfg->dma_sec_cfg->SCFG_CHSEC0 & (0x1UL << channel)) {
        return DMA350_ERR_CANNOT_SET_NOW;
    } else {
        return DMA350_ERR_NONE;
    }
}

enum dma350_error_t dma350_set_ch_nonsecure(struct dma350_dev_t *dev,
                                            uint8_t channel)
{
    if (!dma350_is_init(dev)) {
        return DMA350_ERR_NOT_INIT;
    }

    dev->cfg->dma_sec_cfg->SCFG_CHSEC0 =
        dev->cfg->dma_sec_cfg->SCFG_CHSEC0 | (0x1UL << channel);
    if (dev->cfg->dma_sec_cfg->SCFG_CHSEC0 & (0x1UL << channel)) {
        return DMA350_ERR_NONE;
    } else {
        return DMA350_ERR_CANNOT_SET_NOW;
    }
}

enum dma350_error_t dma350_set_ch_privileged(struct dma350_dev_t *dev,
                                             uint8_t channel)
{
    if (!dma350_is_init(dev)) {
        return DMA350_ERR_NOT_INIT;
    }
    if (dev->cfg->dma_sec_cfg->SCFG_CHSEC0 & (0x1UL << channel)) {
        /* Channel is Non-secure */
        dev->cfg->dma_nsec_ctrl->NSEC_CHPTR = channel;
        dev->cfg->dma_nsec_ctrl->NSEC_CHCFG =
            dev->cfg->dma_nsec_ctrl->NSEC_CHCFG | DMA_NSEC_CHCFG_CHPRIV_Msk;
        if (dev->cfg->dma_nsec_ctrl->NSEC_CHCFG & (DMA_NSEC_CHCFG_CHPRIV_Msk)) {
            return DMA350_ERR_NONE;
        } else {
            return DMA350_ERR_CANNOT_SET_NOW;
        }
    } else {
        /* Channel is Secure */
        dev->cfg->dma_sec_ctrl->SEC_CHPTR = channel;
        dev->cfg->dma_sec_ctrl->SEC_CHCFG =
            dev->cfg->dma_sec_ctrl->SEC_CHCFG | DMA_SEC_CHCFG_CHPRIV_Msk;
        if (dev->cfg->dma_sec_ctrl->SEC_CHCFG & (DMA_SEC_CHCFG_CHPRIV_Msk)) {
            return DMA350_ERR_NONE;
        } else {
            return DMA350_ERR_CANNOT_SET_NOW;
        }
    }
}

enum dma350_error_t dma350_set_ch_unprivileged(struct dma350_dev_t *dev,
                                               uint8_t channel)
{
    if (!dma350_is_init(dev)) {
        return DMA350_ERR_NOT_INIT;
    }
    if (dev->cfg->dma_sec_cfg->SCFG_CHSEC0 & (0x1UL << channel)) {
        /* Channel is Non-secure */
        dev->cfg->dma_nsec_ctrl->NSEC_CHPTR = channel;
        dev->cfg->dma_nsec_ctrl->NSEC_CHCFG =
            dev->cfg->dma_nsec_ctrl->NSEC_CHCFG & ~(DMA_NSEC_CHCFG_CHPRIV_Msk);
        if (dev->cfg->dma_nsec_ctrl->NSEC_CHCFG & (DMA_NSEC_CHCFG_CHPRIV_Msk)) {
            return DMA350_ERR_CANNOT_SET_NOW;
        } else {
            return DMA350_ERR_NONE;
        }
    } else {
        /* Channel is Secure */
        dev->cfg->dma_sec_ctrl->SEC_CHPTR = channel;
        dev->cfg->dma_sec_ctrl->SEC_CHCFG =
            dev->cfg->dma_sec_ctrl->SEC_CHCFG & ~(DMA_SEC_CHCFG_CHPRIV_Msk);
        if (dev->cfg->dma_sec_ctrl->SEC_CHCFG & (DMA_SEC_CHCFG_CHPRIV_Msk)) {
            return DMA350_ERR_CANNOT_SET_NOW;
        } else {
            return DMA350_ERR_NONE;
        }
    }
}

enum dma350_error_t dma350_set_trigin_secure(struct dma350_dev_t *dev,
                                             uint8_t trigger)
{
    if (!dma350_is_init(dev)) {
        return DMA350_ERR_NOT_INIT;
    }

    if(trigger >= 32) {
        return DMA350_ERR_INVALID_PARAM;
    }

    dev->cfg->dma_sec_cfg->SCFG_TRIGINSEC0 =
        dev->cfg->dma_sec_cfg->SCFG_TRIGINSEC0 & ~(0x1UL << trigger);
    if (dev->cfg->dma_sec_cfg->SCFG_TRIGINSEC0 & (0x1UL << trigger)) {
        return DMA350_ERR_CANNOT_SET_NOW;
    } else {
        return DMA350_ERR_NONE;
    }
}

enum dma350_error_t dma350_set_trigin_nonsecure(struct dma350_dev_t *dev,
                                                uint8_t trigger)
{
    if (!dma350_is_init(dev)) {
        return DMA350_ERR_NOT_INIT;
    }

    if(trigger >= 32) {
        return DMA350_ERR_INVALID_PARAM;
    }

    dev->cfg->dma_sec_cfg->SCFG_TRIGINSEC0 =
        dev->cfg->dma_sec_cfg->SCFG_TRIGINSEC0 | (0x1UL << trigger);
    if (dev->cfg->dma_sec_cfg->SCFG_TRIGINSEC0 & (0x1UL << trigger)) {
        return DMA350_ERR_NONE;
    } else {
        return DMA350_ERR_CANNOT_SET_NOW;
    }
}

enum dma350_error_t dma350_set_trigout_secure(struct dma350_dev_t *dev,
                                              uint8_t trigger)
{
    if (!dma350_is_init(dev)) {
        return DMA350_ERR_NOT_INIT;
    }

    if(trigger >= 32) {
        return DMA350_ERR_INVALID_PARAM;
    }

    dev->cfg->dma_sec_cfg->SCFG_TRIGOUTSEC0 =
        dev->cfg->dma_sec_cfg->SCFG_TRIGOUTSEC0 & ~(0x1UL << trigger);
    if (dev->cfg->dma_sec_cfg->SCFG_TRIGOUTSEC0 & (0x1UL << trigger)) {
        return DMA350_ERR_CANNOT_SET_NOW;
    } else {
        return DMA350_ERR_NONE;
    }
}

enum dma350_error_t dma350_set_trigout_nonsecure(struct dma350_dev_t *dev,
                                                 uint8_t trigger)
{
    if (!dma350_is_init(dev)) {
        return DMA350_ERR_NOT_INIT;
    }

    if(trigger >= 32) {
        return DMA350_ERR_INVALID_PARAM;
    }

    dev->cfg->dma_sec_cfg->SCFG_TRIGOUTSEC0 =
        dev->cfg->dma_sec_cfg->SCFG_TRIGOUTSEC0 | (0x1UL << trigger);
    if (dev->cfg->dma_sec_cfg->SCFG_TRIGOUTSEC0 & (0x1UL << trigger)) {
        return DMA350_ERR_NONE;
    } else {
        return DMA350_ERR_CANNOT_SET_NOW;
    }
}
