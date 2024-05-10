/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
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
 * \file kmu_drv.c
 * \brief Driver for Arm KMU.
 */

#include "kmu_drv.h"

struct _kmu_reg_map_t {
    volatile uint32_t kmubc;
                /*!< Offset: 0x000 (R/ ) KMU Build Configuration Register */
    volatile uint32_t kmuis;
                /*!< Offset: 0x004 (R/ ) KMU Interrupt Status Register */
    volatile uint32_t kmuie;
                /*!< Offset: 0x008 (R/W) KMU Interrupt Enable Register */
    volatile uint32_t kmuic;
                /*!< Offset: 0x00C (R/W) KMU Interrupt Clear Register */
    volatile uint32_t kmuprbgsi;
                /*!< Offset: 0x010 (R/W) PRBG Seed Input Register */
    volatile uint32_t reserved_0[7];
                /*!< Offset: 0x14-0x30 Reserved */
    volatile uint32_t kmuksc[32];
                /*!< Offset: 0x030 (R/W) KMU Key Slot Configuration Register */
    volatile uint32_t kmudkpa[32];
                /*!< Offset: 0x0B0 (R/W) KMU Destination Key Port Address
                 *                       Register */
    volatile uint32_t kmuksk[32][8];
                /*!< Offset: 0x130 (R/W) KMU Key Slot Register */
    volatile uint32_t reserved_1[680];
                /*!< Offset: 0x530-0xFCC Reserved */
    volatile uint32_t pidr4;
                /*!< Offset: 0xFD0 (R/ ) Peripheral ID 4 */
    volatile uint32_t reserved_2[3];
                /*!< Offset: 0xFD4-0xFDC Reserved */
    volatile uint32_t pidr0;
                /*!< Offset: 0xFE0 (R/ ) Peripheral ID 0 */
    volatile uint32_t pidr1;
                /*!< Offset: 0xFE4 (R/ ) Peripheral ID 1 */
    volatile uint32_t pidr2;
                /*!< Offset: 0xFE8 (R/ ) Peripheral ID 2 */
    volatile uint32_t pidr3;
                /*!< Offset: 0xFEC (R/ ) Peripheral ID 3 */
    volatile uint32_t cidr0;
                /*!< Offset: 0xFF0 (R/ ) Component ID 0 */
    volatile uint32_t cidr1;
                /*!< Offset: 0xFF4 (R/ ) Component ID 1 */
    volatile uint32_t cidr2;
                /*!< Offset: 0xFF8 (R/ ) Component ID 2 */
    volatile uint32_t cidr3;
                /*!< Offset: 0xFFC (R/ ) Component ID 3 */
};

enum kmu_error_t kmu_init(struct kmu_dev_t *dev, uint8_t *prbg_seed)
{
    uint32_t *p_prgb_seed_word = (uint32_t *)prbg_seed;
    struct _kmu_reg_map_t* p_kmu = (struct _kmu_reg_map_t*)dev->cfg->base;
    uint32_t idx;

    if ((uint32_t)p_prgb_seed_word & (sizeof(uint32_t) - 1)) {
        return KMU_ERROR_INVALID_ALIGNMENT;
    }

    for (idx = 0; idx < KMU_PRBG_SEED_LEN / sizeof(uint32_t); idx++) {
        p_kmu->kmuprbgsi = p_prgb_seed_word[idx];
    }

    /* The lock can be done on any of the kmuksc registers, so we choose #0 */
    p_kmu->kmuksc[0] |= KMU_KMUKSC_L_KMUPRBGSI_MASK;

    /* TODO FIXME enable more selectively */
    p_kmu->kmuie = 0xFFFFFFFFu;

    return KMU_ERROR_NONE;
}

enum kmu_error_t kmu_key_get_export_config(struct kmu_dev_t *dev, uint32_t slot,
                                           struct kmu_key_export_config_t *config)
{
    struct _kmu_reg_map_t* p_kmu = (struct _kmu_reg_map_t*)dev->cfg->base;

    if (slot >= KMU_GET_NKS(p_kmu)) {
        return KMU_ERROR_INVALID_SLOT;
    }

    config->destination_port_write_delay =
        (p_kmu->kmuksc[slot] & KMU_KMUKSC_DPWD_MASK) >> KMU_KMUKSC_DPWD_OFF;

    config->destination_port_address_increment =
        (p_kmu->kmuksc[slot] & KMU_KMUKSC_DPAI_MASK) >> KMU_KMUKSC_DPAI_OFF;

    config->destination_port_data_width_code =
        (p_kmu->kmuksc[slot] & KMU_KMUKSC_DPDW_MASK) >> KMU_KMUKSC_DPDW_OFF;

    config->destination_port_data_writes_code =
        (p_kmu->kmuksc[slot] & KMU_KMUKSC_NDPW_MASK) >> KMU_KMUKSC_NDPW_OFF;

    config->new_mask_for_next_key_writes =
        (p_kmu->kmuksc[slot] & KMU_KMUKSC_NMNKW_MASK) >> KMU_KMUKSC_NMNKW_OFF;

    config->write_mask_disable =
        (p_kmu->kmuksc[slot] & KMU_KMUKSC_WMD_MASK) >> KMU_KMUKSC_WMD_OFF;

    return KMU_ERROR_NONE;
}

enum kmu_error_t kmu_key_set_export_config(struct kmu_dev_t *dev, uint32_t slot,
                                           struct kmu_key_export_config_t *config)
{
    struct _kmu_reg_map_t* p_kmu = (struct _kmu_reg_map_t*)dev->cfg->base;

    if (slot >= KMU_GET_NKS(p_kmu)) {
        return KMU_ERROR_INVALID_SLOT;
    }

    p_kmu->kmuksc[slot] &= ~KMU_KMUKSC_DPWD_MASK;
    p_kmu->kmuksc[slot] |=
        ((config->destination_port_write_delay << KMU_KMUKSC_DPWD_OFF)
         & KMU_KMUKSC_DPWD_MASK);

    p_kmu->kmuksc[slot] &= ~KMU_KMUKSC_DPAI_MASK;
    p_kmu->kmuksc[slot] |=
        ((config->destination_port_address_increment << KMU_KMUKSC_DPAI_OFF)
         & KMU_KMUKSC_DPAI_MASK);

    p_kmu->kmuksc[slot] &= ~KMU_KMUKSC_DPDW_MASK;
    p_kmu->kmuksc[slot] |=
        ((config->destination_port_data_width_code << KMU_KMUKSC_DPDW_OFF)
         & KMU_KMUKSC_DPDW_MASK);

    p_kmu->kmuksc[slot] &= ~KMU_KMUKSC_NDPW_MASK;
    p_kmu->kmuksc[slot] |=
        ((config->destination_port_data_writes_code << KMU_KMUKSC_NDPW_OFF)
         & KMU_KMUKSC_NDPW_MASK);

    p_kmu->kmuksc[slot] &= ~KMU_KMUKSC_NMNKW_MASK;
    p_kmu->kmuksc[slot] |=
        ((config->new_mask_for_next_key_writes << KMU_KMUKSC_NMNKW_OFF)
         & KMU_KMUKSC_NMNKW_MASK);

    p_kmu->kmuksc[slot] &= ~KMU_KMUKSC_WMD_MASK;
    p_kmu->kmuksc[slot] |=
        ((config->write_mask_disable << KMU_KMUKSC_WMD_OFF)
         & KMU_KMUKSC_WMD_MASK);

    return KMU_ERROR_NONE;
}

enum kmu_error_t kmu_set_key_locked(struct kmu_dev_t *dev, uint32_t slot)
{
    struct _kmu_reg_map_t* p_kmu = (struct _kmu_reg_map_t*)dev->cfg->base;

    if (slot >= KMU_GET_NKS(p_kmu)) {
        return KMU_ERROR_INVALID_SLOT;
    }

    p_kmu->kmuksc[slot] |= KMU_KMUKSC_LKSKR_MASK;

    return KMU_ERROR_NONE;
}

enum kmu_error_t kmu_get_key_locked(struct kmu_dev_t *dev, uint32_t slot)
{
    struct _kmu_reg_map_t* p_kmu = (struct _kmu_reg_map_t*)dev->cfg->base;

    if (slot >= KMU_GET_NKS(p_kmu)) {
        return KMU_ERROR_INVALID_SLOT;
    }

    if (p_kmu->kmuksc[slot] & KMU_KMUKSC_LKSKR_MASK) {
        return KMU_ERROR_SLOT_LOCKED;
    } else {
        return KMU_ERROR_NONE;
    }
}

enum kmu_error_t kmu_set_key_export_config_locked(struct kmu_dev_t *dev,
                                                  uint32_t slot)
{
    struct _kmu_reg_map_t* p_kmu = (struct _kmu_reg_map_t*)dev->cfg->base;

    if (slot >= KMU_GET_NKS(p_kmu)) {
        return KMU_ERROR_INVALID_SLOT;
    }

    p_kmu->kmuksc[slot] |= KMU_KMUKSC_LKS_MASK;

    return KMU_ERROR_NONE;
}

enum kmu_error_t kmu_get_key_export_config_locked(struct kmu_dev_t *dev,
                                                  uint32_t slot)
{
    struct _kmu_reg_map_t* p_kmu = (struct _kmu_reg_map_t*)dev->cfg->base;

    if (slot >= KMU_GET_NKS(p_kmu)) {
        return KMU_ERROR_INVALID_SLOT;
    }

    if (p_kmu->kmuksc[slot] & KMU_KMUKSC_LKS_MASK) {
        return KMU_ERROR_SLOT_LOCKED;
    } else {
        return KMU_ERROR_NONE;
    }
}

enum kmu_error_t kmu_set_slot_invalid(struct kmu_dev_t *dev, uint32_t slot)
{
    struct _kmu_reg_map_t* p_kmu = (struct _kmu_reg_map_t*)dev->cfg->base;
    enum kmu_error_t err;

    if (slot >= KMU_GET_NKS(p_kmu)) {
        return KMU_ERROR_INVALID_SLOT;
    }

    p_kmu->kmuksc[slot] |= KMU_KMUKSC_IKS_MASK;

    if (p_kmu->kmuis & KMU_KMISR_AIKSWE_MASK) {
        err = KMU_ERROR_INTERNAL_ERROR;
    } else {
        err = KMU_ERROR_NONE;
    }

    p_kmu->kmuic = 0xFFFFFFFFu;
    return err;
}

enum kmu_error_t kmu_get_slot_invalid(struct kmu_dev_t *dev, uint32_t slot)
{
    struct _kmu_reg_map_t* p_kmu = (struct _kmu_reg_map_t*)dev->cfg->base;

    if (slot >= KMU_GET_NKS(p_kmu)) {
        return KMU_ERROR_INVALID_SLOT;
    }

    if (p_kmu->kmuksc[slot] | KMU_KMUKSC_KSIP_MASK){
        return KMU_ERROR_SLOT_INVALIDATED;
    } else {
        return KMU_ERROR_NONE;
    }
}

enum kmu_error_t kmu_set_key(struct kmu_dev_t *dev, uint32_t slot, uint8_t *key,
                             size_t key_len)
{
    enum kmu_error_t err;
    struct _kmu_reg_map_t* p_kmu = (struct _kmu_reg_map_t*)dev->cfg->base;
    uint32_t* p_key_word = (uint32_t*)key;
    size_t idx;

    if ((uint32_t)key & (sizeof(uint32_t) - 1)) {
        return KMU_ERROR_INVALID_ALIGNMENT;
    }

    if (key_len & (sizeof(uint32_t) - 1) || key_len > 32) {
        return KMU_ERROR_INVALID_LENGTH;
    }

    if (slot >= KMU_GET_NKS(p_kmu)) {
        return KMU_ERROR_INVALID_SLOT;
    }

    err = kmu_get_key_locked(dev, slot);
    if (err != KMU_ERROR_NONE) {
        return err;
    }

    for (idx = 0; idx < key_len / sizeof(uint32_t); idx++) {
        p_kmu->kmuksk[slot][idx] = p_key_word[idx];
        if (p_kmu->kmuis & KMU_KMISR_MWKSW_MASK) {
            p_kmu->kmuis &= ~KMU_KMISR_MWKSW_MASK;
            return KMU_ERROR_SLOT_ALREADY_WRITTEN;
        }
        if (p_kmu->kmuksk[slot][idx] != p_key_word[idx]) {
            return KMU_ERROR_INTERNAL_ERROR;
        }
    }

    return KMU_ERROR_NONE;
}

enum kmu_error_t kmu_get_key(struct kmu_dev_t *dev, uint32_t slot, uint8_t *buf,
                    size_t buf_len)
{
    enum kmu_error_t err;
    struct _kmu_reg_map_t* p_kmu = (struct _kmu_reg_map_t*)dev->cfg->base;
    uint32_t* p_buf_word = (uint32_t*)buf;
    size_t idx;

    if ((uint32_t)buf & (sizeof(uint32_t) - 1)) {
        return KMU_ERROR_INVALID_ALIGNMENT;
    }

    if ((buf_len & (sizeof(uint32_t) - 1)) || buf_len > 32) {
        return KMU_ERROR_INVALID_LENGTH;
    }

    if (slot >= KMU_GET_NKS(p_kmu)) {
        return KMU_ERROR_INVALID_SLOT;
    }

    err = kmu_get_key_locked(dev, slot);
    if (err != KMU_ERROR_NONE) {
        return err;
    }

    for (idx = 0; idx < buf_len / sizeof(uint32_t); idx++) {
        p_buf_word[idx] = p_kmu->kmuksk[slot][idx];
    }

    return KMU_ERROR_NONE;
}

enum kmu_error_t kmu_export_key(struct kmu_dev_t *dev, uint32_t slot)
{
    enum kmu_error_t err;
    struct _kmu_reg_map_t* p_kmu = (struct _kmu_reg_map_t*)dev->cfg->base;

    if (slot >= KMU_GET_NKS(p_kmu)) {
        return KMU_ERROR_INVALID_SLOT;
    }

    /* Trigger the key ready operation */
    p_kmu->kmuksc[slot] |= KMU_KMUKSC_VKS_MASK;

    /* Wait for the ready operation to complete */
    while (p_kmu->kmuksc[slot] & KMU_KMUKSC_VKS_MASK) {}

    /* Check that key readying succeeded, if not return the right error */
    if (!(p_kmu->kmuksc[slot] & KMU_KMUKSC_KSR_MASK)) {
        if (p_kmu->kmuis & KMU_KMISR_KSNL_MASK) {
            err = KMU_ERROR_SLOT_NOT_LOCKED;
            goto out;
        } else if (p_kmu->kmuis & KMU_KMISR_KSKRSM_MASK) {
            err = KMU_ERROR_INVALID_LENGTH;
            goto out;
        } else if (p_kmu->kmuis & KMU_KMISR_KSDPANS_MASK) {
            err = KMU_ERROR_INVALID_EXPORT_ADDR;
            goto out;
        }

        /* Shouldn't ever happen, all errors should be one of the three above */
        err = KMU_ERROR_INTERNAL_ERROR;
        goto out;
    }

    /* Trigger the key export operation */
    p_kmu->kmuksc[slot] |= KMU_KMUKSC_EK_MASK;

    /* Wait for the key export to complete */
    while (!(p_kmu->kmuis & KMU_KMISR_KEC_MASK)) {}

    if (p_kmu->kmuis & KMU_KMISR_WTE_MASK) {
        err = KMU_ERROR_INTERNAL_ERROR;
        goto out;
    } else if (p_kmu->kmuis & KMU_KMISR_INPPE_MASK) {
        err = KMU_ERROR_INTERNAL_ERROR;
        goto out;
    } else if (p_kmu->kmuis & KMU_KMISR_AWBE_MASK) {
        err = KMU_ERROR_INTERNAL_ERROR;
        goto out;
    }

    err = KMU_ERROR_NONE;
out:
    p_kmu->kmuic = 0xFFFFFFFFu;
    return err;
}
