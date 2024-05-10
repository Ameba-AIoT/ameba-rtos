/*
 * Copyright (c) 2016-2021 Arm Limited. All rights reserved.
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
 * \file tgu_armv8_m_drv.h
 * \brief Generic driver for ARM TCM Gating Unit(TGU)
 */

#ifndef __TGU_ARMV8_M_DRV_H__
#define __TGU_ARMV8_M_DRV_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

enum tgu_armv8_m_error_t {
    TGU_ERR_NONE,          /*!< No error */
    TGU_INVALID_ARG,
    TGU_NOT_INIT,
    TGU_INTERN_ERR_NOT_IN_RANGE,
    TGU_INTERN_ERR_NOT_ALIGNED,
    TGU_INTERN_ERR_INVALID_RANGE,
    TGU_INTERN_ERR_BLK_IDX_TOO_HIGH,
    TGU_ERR_RANGE_SEC_ATTR_NON_COMPATIBLE
};

/* Security attribute used in various place of the API */
enum tgu_armv8_m_sec_attr_t {
    TGU_SEC_ATTR_SECURE,     /*!< Secure attribute */
    TGU_SEC_ATTR_NONSECURE,  /*!< Non-secure attribute */
    /*!< Used when getting the configuration of a memory range and some blocks
     *   are secure whereas some other are non secure
     */
    TGU_SEC_ATTR_MIXED,
};

/* Description of a memory range controlled by the TGU */
struct tgu_armv8_m_mem_range_t {
    const uint32_t base;   /*!< Base address (included in the range) */
    const uint32_t limit;  /*!< Limit address (included in the range) */
    const uint32_t range_offset;
    const enum tgu_armv8_m_sec_attr_t attr; /*!< Optional security attribute
                                            *   needed to be matched when
                                            *   accessing this range.
                                            *   For example, the non-secure
                                            *   alias of a memory region can not
                                            *   be accessed using secure access,
                                            *   and configuring the TGU to
                                            *   secure using that range will not
                                            *   be permitted by the driver.
                                            */
};

/* ARM TGU device configuration structure */
struct tgu_armv8_m_dev_cfg_t {
    const uint32_t base;  /*!< TGU CTRL base address */
};

/* ARM TGU device data structure */
struct tgu_armv8_m_dev_data_t {
    /*!< Array of pointers to memory ranges controlled by the TGU */
    const struct tgu_armv8_m_mem_range_t** range_list;
    uint8_t nbr_of_ranges;  /*!< Number of memory ranges in the list */
    bool is_initialized;

};

/* ARM TGU device structure */
struct tgu_armv8_m_dev_t {
    const struct tgu_armv8_m_dev_cfg_t* const cfg;  /*!< TGU configuration */
    struct tgu_armv8_m_dev_data_t* const data;      /*!< TGU data */
};

/* Initialize TGU */
enum tgu_armv8_m_error_t tgu_armv8_m_init(struct tgu_armv8_m_dev_t* dev,
                                          const struct tgu_armv8_m_mem_range_t** range_list,
                                          uint8_t nbr_of_ranges);

/* Get the block size in bytes*/
enum tgu_armv8_m_error_t tgu_armv8_m_get_block_size(struct tgu_armv8_m_dev_t* dev,
                                            uint32_t* blk_size);

/* Get number of programmable blocks*/
enum tgu_armv8_m_error_t tgu_armv8_m_get_number_of_prog_blocks(struct tgu_armv8_m_dev_t* dev,
                                                        uint32_t* num_blks);

/* Set the security configuration of the region */
enum tgu_armv8_m_error_t tgu_armv8_m_config_region(struct tgu_armv8_m_dev_t* dev,
                                           const uint32_t base,
                                           const uint32_t limit,
                                           enum tgu_armv8_m_sec_attr_t attr);

/* Get the security configuration of the region */
enum tgu_armv8_m_error_t tgu_armv8_m_get_region_config(struct tgu_armv8_m_dev_t* dev,
                                               uint32_t base,
                                               uint32_t limit,
                                               enum tgu_armv8_m_sec_attr_t* attr);

/* Get the control value from xTGU_CTRL */
enum tgu_armv8_m_error_t tgu_armv8_m_get_ctrl(struct tgu_armv8_m_dev_t* dev,
                                      uint32_t* ctrl_val);

/* Set the control val in xTGU_CTRL */
enum tgu_armv8_m_error_t tgu_armv8_m_set_ctrl(struct tgu_armv8_m_dev_t* dev,
                                      uint32_t tgu_ctrl);

/* Check if TGU is present */
enum tgu_armv8_m_error_t tgu_armv8_m_is_tgu_present(struct tgu_armv8_m_dev_t* dev,
                                               bool* tgu_present);

#endif /* __TGU_ARMV8_M_DRV_H__ */
