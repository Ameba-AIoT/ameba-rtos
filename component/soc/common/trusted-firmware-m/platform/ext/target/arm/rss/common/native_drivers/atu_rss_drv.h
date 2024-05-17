/*
 * Copyright (c) 2021-2022 Arm Limited. All rights reserved.
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

/**
 * \file atu_rss_drv.h
 * \brief Driver for Arm Address Translation Unit (ATU).
 */

#ifndef __ATU_RSS_DRV_H__
#define __ATU_RSS_DRV_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Arm ATU error enumeration types
 */
enum atu_error_t {
    ATU_ERR_NONE,
    ATU_ERR_INVALID_REGION,
    ATU_ERR_INVALID_ADDRESS,
    ATU_ERR_INVALID_ARG,
};

/**
 * \brief Allowed output bus attribute options for ATU region
 */
enum atu_roba_t {
    ATU_ROBA_PASSTHROUGH   = (0x0u),
    ATU_ROBA_RESERVED      = (0x1u),
    ATU_ROBA_SET_0         = (0x2u),
    ATU_ROBA_SET_1         = (0x3u),
};

/**
 * \brief Arm ATU device configuration structure
 */
struct atu_dev_cfg_t {
    const uint32_t base;                         /*!< ATU base address */
};

/**
 * \brief Arm ATU device structure
 */
struct atu_dev_t {
    const struct atu_dev_cfg_t* const cfg;       /*!< ATU configuration */
};

/**
 * \brief Gets the ATU page size.
 *
 * \param[in] dev                ATU device struct \ref atu_dev_t
 *
 * \return Returns page size in bytes
 *
 * \note This function doesn't check if dev is NULL.
 */
uint16_t get_page_size(struct atu_dev_t* dev);

/**
 * \brief Gets the number of ATU regions supported.
 *
 * \param[in] dev                ATU device struct \ref atu_dev_t
 *
 * \return Returns region count
 *
 * \note This function doesn't check if dev is NULL.
 */
uint8_t get_supported_region_count(struct atu_dev_t* dev);

/**
 * \brief Enables the ATU region.
 *
 * \param[in] dev                ATU device struct \ref atu_dev_t
 * \param[in] region             ATU region number to be enabled
 *
 * \return Returns error code as specified in \ref atu_error_t
 *
 * \note This function doesn't check if dev is NULL.
 */
enum atu_error_t enable_atu_region(struct atu_dev_t* dev, uint8_t region);

/**
 * \brief Disables the ATU region.
 *
 * \param[in] dev                ATU device struct \ref atu_dev_t
 * \param[in] region             ATU region number to be disabled
 *
 * \return Returns error code as specified in \ref atu_error_t
 *
 * \note This function doesn't check if dev is NULL.
 */
enum atu_error_t disable_atu_region(struct atu_dev_t* dev, uint8_t region);

/**
 * \brief Check if Mismatch Error(ME) interrupt is waiting
 *
 * \param[in] dev                ATU device struct \ref atu_dev_t
 *
 * \return Returns bool, true if ME interrupt is waiting, false otherwise
 *
 * \note This function doesn't check if dev is NULL.
 */
bool me_interrupt_is_waiting(struct atu_dev_t* dev);

/**
 * \brief Enables the ATU Mismatch Error(ME) Interrupt.
 *
 * \param[in] dev                ATU device struct \ref atu_dev_t
 *
 * \note This function doesn't check if dev is NULL.
 */
void enable_me_interrupt(struct atu_dev_t* dev);

/**
 * \brief Clears the ATU Mismatch Error(ME) Interrupt.
 *
 * \param[in] dev                ATU device struct \ref atu_dev_t
 *
 * \note This function doesn't check if dev is NULL.
 */
void clear_me_interrupt(struct atu_dev_t* dev);

/**
 * \brief Gets the ATU Mismatch address.
 *
 * \param[in] dev                ATU device struct \ref atu_dev_t
 *
 * \return Returns the ATU Mismatch address.
 *
 * \note This function doesn't check if dev is NULL.
 */
uint32_t get_mismatch_address(struct atu_dev_t* dev);

/**
 * \brief Sets the ATU logical address start.
 *
 * \param[in] dev                ATU device struct \ref atu_dev_t
 * \param[in] address            start address
 * \param[in] region             region number
 *
 * \return Returns error code as specified in \ref atu_error_t
 *
 * \note This function doesn't check if dev is NULL.
 */
enum atu_error_t set_start_logical_address(struct atu_dev_t* dev,
                    uint32_t address, uint8_t region);

/**
 * \brief Sets the ATU logical address end.
 *
 * \param[in] dev                ATU device struct \ref atu_dev_t
 * \param[in] address            end address
 * \param[in] region             region number
 *
 * \return Returns error code as specified in \ref atu_error_t
 *
 * \note This function doesn't check if dev is NULL.
 */
enum atu_error_t set_end_logical_address(struct atu_dev_t* dev,
                    uint32_t address, uint8_t region);

/**
 * \brief Sets the ATU add value for translation.
 *
 * \param[in] dev                ATU device struct \ref atu_dev_t
 * \param[in] offset_address     offset address to calculate the add value
 * \param[in] region             region number
 *
 * \return Returns error code as specified in \ref atu_error_t
 *
 * \note This function doesn't check if dev is NULL.
 */
enum atu_error_t set_add_value(struct atu_dev_t* dev,
                    uint64_t offset_address, uint8_t region);

/**
 * \brief Sets the ATU AXNSC.
 *
 * \param[in] dev                ATU device struct \ref atu_dev_t
 * \param[in] val                value
 * \param[in] region             region number
 *
 * \return Returns error code as specified in \ref atu_error_t
 *
 * \note This function doesn't check if dev is NULL.
 */
enum atu_error_t set_axnsc(struct atu_dev_t* dev,
                    enum atu_roba_t val, uint8_t region);

/**
 * \brief Sets the ATU AXCACHE3.
 *
 * \param[in] dev                ATU device struct \ref atu_dev_t
 * \param[in] val                value
 * \param[in] region             region number
 *
 * \return Returns error code as specified in \ref atu_error_t
 *
 * \note This function doesn't check if dev is NULL.
 */
enum atu_error_t set_axcache3(struct atu_dev_t* dev,
                    enum atu_roba_t val, uint8_t region);

/**
 * \brief Sets the ATU AXCACHE2.
 *
 * \param[in] dev                ATU device struct \ref atu_dev_t
 * \param[in] val                value
 * \param[in] region             region number
 *
 * \return Returns error code as specified in \ref atu_error_t
 *
 * \note This function doesn't check if dev is NULL.
 */
enum atu_error_t set_axcache2(struct atu_dev_t* dev,
                    enum atu_roba_t val, uint8_t region);

/**
 * \brief Sets the ATU AXCACHE1.
 *
 * \param[in] dev                ATU device struct \ref atu_dev_t
 * \param[in] val                value
 * \param[in] region             region number
 *
 * \return Returns error code as specified in \ref atu_error_t
 *
 * \note This function doesn't check if dev is NULL.
 */
enum atu_error_t set_axcache1(struct atu_dev_t* dev,
                    enum atu_roba_t val, uint8_t region);

/**
 * \brief Sets the ATU AXCACHE0.
 *
 * \param[in] dev                ATU device struct \ref atu_dev_t
 * \param[in] val                value
 * \param[in] region             region number
 *
 * \return Returns error code as specified in \ref atu_error_t
 *
 * \note This function doesn't check if dev is NULL.
 */
enum atu_error_t set_axcache0(struct atu_dev_t* dev,
                    enum atu_roba_t val, uint8_t region);

/**
 * \brief Sets the ATU AXPROT2.
 *
 * \param[in] dev                ATU device struct \ref atu_dev_t
 * \param[in] val                value
 * \param[in] region             region number
 *
 * \return Returns error code as specified in \ref atu_error_t
 *
 * \note This function doesn't check if dev is NULL.
 */
enum atu_error_t set_axprot2(struct atu_dev_t* dev,
                    enum atu_roba_t val, uint8_t region);

/**
 * \brief Sets the ATU AXPROT1.
 *
 * \param[in] dev                ATU device struct \ref atu_dev_t
 * \param[in] val                value
 * \param[in] region             region number
 *
 * \return Returns error code as specified in \ref atu_error_t
 *
 * \note This function doesn't check if dev is NULL.
 */
enum atu_error_t set_axprot1(struct atu_dev_t* dev,
                    enum atu_roba_t val, uint8_t region);

/**
 * \brief Sets the ATU AXPROT0.
 *
 * \param[in] dev                ATU device struct \ref atu_dev_t
 * \param[in] val                value
 * \param[in] region             region number
 *
 * \return Returns error code as specified in \ref atu_error_t
 *
 * \note This function doesn't check if dev is NULL.
 */
enum atu_error_t set_axprot0(struct atu_dev_t* dev,
                    enum atu_roba_t val, uint8_t region);

/**
 * \brief Sets the ATU General Purpose Register.
 *
 * \param[in] dev                ATU device struct \ref atu_dev_t
 * \param[in] val                value
 * \param[in] region             region number
 *
 * \return Returns error code as specified in \ref atu_error_t
 *
 * \note This function doesn't check if dev is NULL.
 */
enum atu_error_t set_gp_value(struct atu_dev_t* dev,
                    uint8_t val, uint8_t region);

/**
 * \brief Gets the ATU General Purpose Register.
 *
 * \param[in] dev                ATU device struct \ref atu_dev_t
 * \param[in] region             region number
 *
 * \return Returns the ATU GP value.
 *
 * \note This function doesn't check if dev is NULL.
 */
uint8_t get_gp_value(struct atu_dev_t* dev, uint8_t region);

/**
 * \brief Creates and enables an ATU region.
 *
 * \param[in] dev                ATU device struct \ref atu_dev_t
 * \param[in] region             ATU region number to be initialized
 * \param[in] log_addr           Logical address
 * \param[in] phys_addr          Physical address
 * \param[in] size               Region size
 *
 * \return Returns error code as specified in \ref atu_error_t
 */
enum atu_error_t atu_initialize_region(struct atu_dev_t *dev, uint8_t region,
                                       uint32_t log_addr, uint64_t phys_addr,
                                       uint32_t size);

/**
 * \brief Uninitializes an ATU region.
 *
 * \param[in] dev                ATU device struct \ref atu_dev_t
 * \param[in] region             ATU region number to be uninitialized
 *
 * \return Returns error code as specified in \ref atu_error_t
 */
enum atu_error_t atu_uninitialize_region(struct atu_dev_t *dev, uint8_t region);

#ifdef __cplusplus
}
#endif

#endif /* __ATU_RSS_DRV_H__ */
