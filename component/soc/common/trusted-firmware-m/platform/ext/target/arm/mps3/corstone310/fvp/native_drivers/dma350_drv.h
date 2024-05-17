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

#ifndef __DMA350_DRV_H
#define __DMA350_DRV_H

#include "dma350_regdef.h"

/* For __STATIC_INLINE */
#include "cmsis_compiler.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DMA_INFO_IIDR(_IMPLEMENTER_, _REVISION_, _VARIANT_, _PRODUCT_ID_)      \
    (((_IMPLEMENTER_)&0xFFFUL) | (((_REVISION_)&0xFUL) << 12) |                \
     (((_VARIANT_)&0xFUL) << 16) | (((_PRODUCT_ID_)&0xFFFUL) << 20))

#define DMA_INFO_IIDR_IMPL_ARM 0x43B
#define DMA_INFO_IIDR_REVISION 0x0
#define DMA_INFO_IIDR_VARIANT  0x0
#define DMA_INFO_IIDR_PROD_ID  0x3A0

#define DMA_INFO_IIDR_SUPPORTED                                                \
    DMA_INFO_IIDR(DMA_INFO_IIDR_IMPL_ARM, DMA_INFO_IIDR_REVISION,              \
                  DMA_INFO_IIDR_VARIANT, DMA_INFO_IIDR_PROD_ID)

#define DMA_INFO_AIDR(_ARCH_MAJOR_, _ARCH_MINOR_)                              \
    (((_ARCH_MAJOR_)&0xFUL) | (((_ARCH_MINOR_)&0xFUL) << 4))

#define DMA_INFO_AIDR_SUPPORTED DMA_INFO_AIDR(0, 0)

#define DMA350_MAX_NUM_CH 16

/* DMA350 state definitions */
#define DMA350_INITIALIZED (1UL << 0)

/* DMA350 DMA Device error enumeration types */
enum dma350_error_t {
    DMA350_ERR_NONE = 0,       /*!< No error */
    DMA350_ERR_IIDR_MISMATCH,  /*!< Error: DMA350 driver does not support this
                                *   implementer of the hardware */
    DMA350_ERR_AIDR_MISMATCH,  /*!< Error: DMA350 driver does not support this
                                *   architecture revision */
    DMA350_ERR_NOT_INIT,       /*!< Error: DMA350 not initialized */
    DMA350_ERR_CANNOT_SET_NOW, /*!< Error: config cannot be set currently */
    DMA350_ERR_INVALID_PARAM,  /*!< Error: requested parameter is not valid */
};

/* ARM DMA350 DMA INFO register BUILDCFG0 field Type */
union dma350_dmainfo_buildcfg0_t {
    struct {
        uint32_t FRAMETYPE:3;    /*!< bit:  0.. 2 FRAMETYPE[ 2:0] */
        uint32_t RESERVED0:1;    /*!< bit:      3 RESERVED0 */
        uint32_t NUM_CHANNELS:6; /*!< bit:  4.. 9 NUM_CHANNELS[ 5:0] */
        uint32_t ADDR_WIDTH:6;   /*!< bit: 10..15 ADDR_WIDTH[ 5:0] */
        uint32_t DATA_WIDTH:3;   /*!< bit: 16..18 DATA_WIDTH[ 2:0] */
        uint32_t RESERVED1:1;    /*!< bit:     19 RESERVED1 */
        uint32_t CHID_WIDTH:5;   /*!< bit: 20..24 CHID_WIDTH[ 4:0] */
        uint32_t RESERVED2:7;    /*!< bit: 25..31 RESERVED2[ 6:0] */
    } b;                         /*!< Structure used for bit access */
    uint32_t w;                  /*!< Type used for word access */
};

/* ARM DMA350 DMA INFO register BUILDCFG1 field Type */
union dma350_dmainfo_buildcfg1_t {
    struct {
        uint32_t NUM_TRIGGER_IN:9;  /*!< bit:  0.. 8 NUM_TRIGGER_IN[ 8:0] */
        uint32_t NUM_TRIGGER_OUT:7; /*!< bit:  9..15 NUM_TRIGGER_OUT[ 6:0] */
        uint32_t HAS_TRIGSEL:1;     /*!< bit:     16 HAS_TRIGSEL */
        uint32_t RESERVED0:7;       /*!< bit: 17..23 RESERVED0[ 6:0] */
        uint32_t RESERVED1:1;       /*!< bit:     24 RESERVED1 */
        uint32_t RESERVED2:7;       /*!< bit: 25..31 RESERVED2[ 6:0] */
    } b;                            /*!< Structure used for bit access */
    uint32_t w;                     /*!< Type used for word access */
};

/* ARM DMA350 DMA device configuration structure */
struct dma350_dev_cfg_t {
    DMASECCFG_TypeDef *dma_sec_cfg;     /*!< DMA350 secure config */
    DMASECCTRL_TypeDef *dma_sec_ctrl;   /*!< DMA350 secure control */
    DMANSECCTRL_TypeDef *dma_nsec_ctrl; /*!< DMA350 non-secure control */
    DMAINFO_TypeDef *dma_info;          /*!< DMA350 info */
};

/* ARM DMA350 DMA device data structure */
struct dma350_dev_data_t {
    uint32_t state; /*!< Indicates if the DMA350 driver
                     *   is initialized and enabled */
};

/* ARM DMA350 DMA device structure */
struct dma350_dev_t {
    const struct dma350_dev_cfg_t *const cfg; /*!< DMA350 DMA configuration */
    struct dma350_dev_data_t *const data;     /*!< DMA350 DMA data */
};

/**
 * \brief Initializes DMA350 DMA
 *
 * \param[in] dev         DMA350 device struct \ref dma350_dev_t
 *
 * \return Returns error code as specified in \ref dma350_error_t
 *
 * \note This function doesn't check if dev is NULL.
 */
enum dma350_error_t dma350_init(struct dma350_dev_t *dev);

/**
 * \brief Checks if DMA350 device is initialised
 *
 * \param[in] dev         DMA350 device struct \ref dma350_dev_t
 *
 * \return Returns true if device is initialized
 *
 * \note This function doesn't check if dev is NULL.
 */
__STATIC_INLINE
uint8_t dma350_is_init(const struct dma350_dev_t *dev);

/**
 * \brief Gets number of channels available in DMA350 device
 *
 * \param[in] dev         DMA350 device struct \ref dma350_dev_t
 *
 * \return Returns number of channels
 *
 * \note This function doesn't check if dev is NULL or if it has been init.
 */
__STATIC_INLINE
uint8_t dma350_get_num_ch(const struct dma350_dev_t *dev);

/**
 * \brief Enables Security Access Violation Interrupt
 *
 * \param[in] dev         DMA350 device struct \ref dma350_dev_t
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL.
 */
void dma350_enable_secaccvio_irq(struct dma350_dev_t *dev);

/**
 * \brief Disables Security Access Violation Interrupt
 *
 * \param[in] dev         DMA350 device struct \ref dma350_dev_t
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL.
 */
void dma350_disable_secaccvio_irq(struct dma350_dev_t *dev);

/**
 * \brief Set Security Access Violation response to bus error
 *
 * \param[in] dev         DMA350 device struct \ref dma350_dev_t
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL.
 */
void dma350_set_secaccvio_rsp_buserr(struct dma350_dev_t *dev);

/**
 * \brief Set Security Access Violation response to RAZ/WI
 *
 * \param[in] dev         DMA350 device struct \ref dma350_dev_t
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL.
 */
void dma350_set_secaccvio_rsp_razwi(struct dma350_dev_t *dev);

/**
 * \brief Set Security Access Violation response to RAZ/WI
 *        Once locked, SCFGCHSEC, SCFGTRIGSEC and SCFGCTRL cannot be modified.
 *        Can only be unlocked by reset.
 *
 * \param[in] dev         DMA350 device struct \ref dma350_dev_t
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL.
 */
void dma350_lock_security_cfg(struct dma350_dev_t *dev);

/**
 * \brief Gets Security Access Violation Interrupt status
 *
 * \param[in] dev         DMA350 device struct \ref dma350_dev_t
 *
 * \return Returns true if secaccvio irq is enabled and active,
 *         false otherwise.
 *
 * \note This function doesn't check if dev is NULL.
 *       Cleared when secaccvio status is cleared.
 */
uint8_t dma350_get_secaccvio_irq(struct dma350_dev_t *dev);

/**
 * \brief Gets Security Access Violation status
 *
 * \param[in] dev         DMA350 device struct \ref dma350_dev_t
 *
 * \return Returns true if secaccvio status active, false otherwise.
 *
 * \note This function doesn't check if dev is NULL.
 */
uint8_t dma350_get_secaccvio_status(struct dma350_dev_t *dev);

/**
 * \brief Clears Security Access Violation Status
 *
 * \param[in] dev         DMA350 device struct \ref dma350_dev_t
 *
 * \return void
 *
 * \note This function doesn't check if dev is NULL.
 */
void dma350_clear_secaccvio_status(struct dma350_dev_t *dev);

/**
 * \brief Set DMA350 DMA Channel to secure.
 *
 * \param[in] dev         DMA350 device struct \ref dma350_dev_t
 * \param[in] channel     Id of the channel to be updated
 *
 * \return Returns error code as specified in \ref dma350_error_t
 *
 * \note This function doesn't check if dev is NULL.
 *       Operation will fail if channel is enabled.
 */
enum dma350_error_t dma350_set_ch_secure(struct dma350_dev_t *dev,
                                         uint8_t channel);

/**
 * \brief Set DMA350 DMA Channel to non-secure.
 *
 * \param[in] dev         DMA350 device struct \ref dma350_dev_t
 * \param[in] channel     Id of the channel to be updated
 *
 * \return Returns error code as specified in \ref dma350_error_t
 *
 * \note This function doesn't check if dev is NULL.
 *       Operation will fail if channel is enabled.
 */
enum dma350_error_t dma350_set_ch_nonsecure(struct dma350_dev_t *dev,
                                            uint8_t channel);

/**
 * \brief Set DMA350 DMA Channel to privileged.
 *
 * \param[in] dev         DMA350 device struct \ref dma350_dev_t
 * \param[in] channel     Id of the channel to be updated
 *
 * \return Returns error code as specified in \ref dma350_error_t
 *
 * \note This function doesn't check if dev is NULL.
 *       Operation will fail if channel is enabled.
 *
 * \note This function sets the channel privilege according to the current
 *       channel security. When changing channel security, this setting is
 *       not transferred to the other domain. So, it is recommended that after
 *       the security of a channel is changed, the privilege level should
 *       also be set.
 *
 */
enum dma350_error_t dma350_set_ch_privileged(struct dma350_dev_t *dev,
                                             uint8_t channel);

/**
 * \brief Set DMA350 DMA Channel to unprivileged.
 *
 * \param[in] dev         DMA350 device struct \ref dma350_dev_t
 * \param[in] channel     Id of the channel to be updated
 *
 * \return Returns error code as specified in \ref dma350_error_t
 *
 * \note This function doesn't check if dev is NULL.
 *       Operation will fail if channel is enabled.
 */
enum dma350_error_t dma350_set_ch_unprivileged(struct dma350_dev_t *dev,
                                               uint8_t channel);

/**
 * \brief Set DMA350 DMA Trigger input to secure.
 *
 * \param[in] dev         DMA350 device struct \ref dma350_dev_t
 * \param[in] trigger     Trigger input number to be updated
 *
 * \return Returns error code as specified in \ref dma350_error_t
 *
 * \note This function doesn't check if dev is NULL.
 *       Operation will fail if channel is enabled.
 */
enum dma350_error_t dma350_set_trigin_secure(struct dma350_dev_t *dev,
                                             uint8_t trigger);

/**
 * \brief Set DMA350 DMA Trigger input to non-secure.
 *
 * \param[in] dev         DMA350 device struct \ref dma350_dev_t
 * \param[in] trigger     Trigger input number to be updated
 *
 * \return Returns error code as specified in \ref dma350_error_t
 *
 * \note This function doesn't check if dev is NULL.
 *       Operation will fail if channel is enabled.
 */
enum dma350_error_t dma350_set_trigin_nonsecure(struct dma350_dev_t *dev,
                                                uint8_t trigger);

/**
 * \brief Set DMA350 DMA Trigger output to secure.
 *
 * \param[in] dev         DMA350 device struct \ref dma350_dev_t
 * \param[in] trigger     Trigger output number to be updated
 *
 * \return Returns error code as specified in \ref dma350_error_t
 *
 * \note This function doesn't check if dev is NULL.
 *       Operation will fail if channel is enabled.
 */
enum dma350_error_t dma350_set_trigout_secure(struct dma350_dev_t *dev,
                                              uint8_t trigger);

/**
 * \brief Set DMA350 DMA Trigger output to non-secure.
 *
 * \param[in] dev         DMA350 device struct \ref dma350_dev_t
 * \param[in] trigger     Trigger output number to be updated
 *
 * \return Returns error code as specified in \ref dma350_error_t
 *
 * \note This function doesn't check if dev is NULL.
 *       Operation will fail if channel is enabled.
 */
enum dma350_error_t dma350_set_trigout_nonsecure(struct dma350_dev_t *dev,
                                                 uint8_t trigger);

__STATIC_INLINE
uint8_t dma350_get_num_ch(const struct dma350_dev_t *dev)
{
    return (union dma350_dmainfo_buildcfg0_t) {
        .w = dev->cfg->dma_info->DMA_BUILDCFG0
    }.b.NUM_CHANNELS;
}

__STATIC_INLINE
uint8_t dma350_is_init(const struct dma350_dev_t *dev)
{
    return dev->data->state == DMA350_INITIALIZED;
}

#ifdef __cplusplus
}
#endif
#endif /* __DMA350_DRV_H */
