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

#include "sic_drv.h"

#include "cmsis_compiler.h"
#include <stddef.h>

/**
 * \brief SIC register map structure
 */
struct _sic_reg_map_t {
    volatile uint32_t cbcr;
                /*!< Offset: 0x000 (R/ ) SIC Cache Build Configuration Register */
    volatile uint32_t ccr;
                /*!< Offset: 0x004 (R/W) SIC Cache Control Register */
    volatile uint32_t aecr;
                /*!< Offset: 0x008 (R/W) SIC Authentication Engine Control Register */
    volatile uint32_t arba;
                /*!< Offset: 0x00C (R/W) SIC Authentication Region Base Address Register */
    volatile uint32_t area;
                /*!< Offset: 0x010 (R/W) SIC Authentication Region End Address Register */
    volatile uint32_t aescr;
                /*!< Offset: 0x014 (R/W) SIC AES Configuration Register */
    volatile uint32_t aesrbgr;
                /*!< Offset: 0x018 (R/W) SIC AES Random Bit Generator seed Register */
    volatile uint32_t aesrbgcr;
                /*!< Offset: 0x01C (R/ ) SIC AES Random Bit Generator Counter Register */
    volatile uint32_t reserved_0[4];   /*!< Offset: 0x020-0x02F Reserved */
    volatile uint32_t istar;
                /*!< Offset: 0x030 (R/ ) SIC Interrupt Status Register */
    volatile uint32_t isclrr;
                /*!< Offset: 0x034 (R/W) SIC Interrupt Clear Register */
    volatile uint32_t einr;
                /*!< Offset: 0x038 (R/W) SIC Interrupt Enable Register */
    volatile uint32_t reserved_1[9]; /*!< Offset: 0x3C-0x5F Reserved */
    volatile uint32_t pmcr;
                /*!< Offset: 0x060 (R/W) SIC Performance Monitoring Control Register */
    volatile uint32_t pmphc;
                /*!< Offset: 0x064 (R/ ) SIC Performance Monitoring Page Hit Register */
    volatile uint32_t pmpmc;
                /*!< Offset: 0x068 (R/ ) SIC Performance Monitoring Page Miss Register */
    volatile uint32_t pmbc;
                /*!< Offset: 0x06C (R/ ) SIC Performance Monitoring Bypass Register */
    volatile uint32_t reserved_2[36]; /*!< Offset: 0x70-0xFF Reserved */
    __PACKED_STRUCT {
        volatile uint32_t drr;
                /*!< Offset: 0xX00 (R/W) SIC Decryption Region Register */
        volatile uint32_t reserved_0[3]; /*!< Offset: 0xX04-0xX0F Reserved */
        volatile uint32_t div;
                /*!< Offset: 0xX10 (R/W) SIC Decryption IV Register */
        volatile uint32_t dnw[2];
                /*!< Offset: 0xX14 (R/W) SIC Decryption Nonce Word Register */
        volatile uint32_t reserved_1[1]; /*!< Offset: 0xX1C-0xX1F Reserved */
        volatile uint32_t dkw[8];
                /*!< Offset: 0xX20 ( /W) SIC Decryption Key Word Register */
        volatile uint32_t phc;
                /*!< Offset: 0xX40 (R/ ) SIC Page Hit Counter Register */
        volatile uint32_t pmc;
                /*!< Offset: 0xX44 (R/ ) SIC Page Miss Counter Register */
        volatile uint32_t pbc;
                /*!< Offset: 0xX48 (R/ ) SIC Page Bypass Counter Register */
        volatile uint32_t reserved_2[45]; /*!< Offset: 0xX4C-0xXFF Reserved */
    } dr[4];
    volatile uint32_t reserved_3[640]; /*!< Offset: 0x500-0xEFF Reserved */
    volatile uint32_t di[3];
                /*!< Offset: 0xF00 (R/ ) SIC Debug Information Register */
    volatile uint32_t reserved_4[49]; /*!< Offset: 0x500-0xEFF Reserved */
    volatile uint32_t pidr4;
                /*!< Offset: 0xFD0 (R/ ) Peripheral ID 4 */
    volatile uint32_t reserved_5[3];   /*!< Offset: 0xFD4-0xFDC Reserved */
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
    volatile uint32_t htr[];
                /*!< Offset: 0x1000 (R/W) Hash Tag RAM */
};

static inline uint8_t get_dr_am(struct sic_dev_t *dev)
{
    struct _sic_reg_map_t* p_sic = (struct _sic_reg_map_t*)dev->cfg->base;

    return 1 << (p_sic->cbcr & 0xF);
}

static inline bool have_pmon(struct sic_dev_t *dev)
{
    struct _sic_reg_map_t* p_sic = (struct _sic_reg_map_t*)dev->cfg->base;

    return (p_sic->cbcr >> 4) & 0x1;
}

static inline uint8_t is_sic_enabled(struct sic_dev_t *dev)
{
    struct _sic_reg_map_t* p_sic = (struct _sic_reg_map_t*)dev->cfg->base;

    /* Return the SIC_ENABLE field */
    return p_sic->ccr & 0x1u;
}


size_t sic_page_size_get(struct sic_dev_t *dev)
{
    struct _sic_reg_map_t* p_sic = (struct _sic_reg_map_t*)dev->cfg->base;

    return 1 << (((p_sic->cbcr >> 8) & 0xF) + 7);
}

uint32_t sic_page_count_get(struct sic_dev_t *dev)
{
    struct _sic_reg_map_t* p_sic = (struct _sic_reg_map_t*)dev->cfg->base;

    return ((p_sic->cbcr >> 12) & 0x3FF) + 1;
}

enum sic_error_t sic_enable(struct sic_dev_t *dev)
{
    struct _sic_reg_map_t* p_sic = (struct _sic_reg_map_t*)dev->cfg->base;

    /* Set the SIC_ENABLE field */
    p_sic->ccr |= 0x1u;

    return SIC_ERROR_NONE;
}

enum sic_error_t sic_disable(struct sic_dev_t *dev)
{
    struct _sic_reg_map_t* p_sic = (struct _sic_reg_map_t*)dev->cfg->base;

    /* TODO FIXME re-enable this once the FVP sets the SIC_IDLE field */
    /* Wait for the SIC to be idle */
    /* while(!(p_sic->istar & (0x1u << 8))) { */
        /* Unset the SIC_ENABLE field */
        p_sic->ccr &= ~(0x1u);
    /* } */

    return SIC_ERROR_NONE;
}

enum sic_error_t sic_auth_init(struct sic_dev_t *dev,
                               enum sic_digest_size_t digest_size,
                               enum sic_digest_config_t digest_config,
                               uintptr_t base, size_t size)
{
    struct _sic_reg_map_t* p_sic = (struct _sic_reg_map_t*)dev->cfg->base;
    uintptr_t end;

    /* The CCR register cannot be written while the SIC is enabled */
    if (is_sic_enabled(dev)) {
        return SIC_ERROR_INVALID_OP_WHILE_ENABLED;
    }

    if (base & 0xFFFu) {
        return SIC_ERROR_INVALID_ALIGNMENT;
    }

    if (size & 0xFFFu) {
        return SIC_ERROR_INVALID_ALIGNMENT;
    }

    /* Auth region must not be larger than 8MiB */
    if (size == 0x0u || size >= 0x800000u) {
        return SIC_ERROR_INVALID_SIZE;
    }

    end = base + size;
    /* Prevent overflow */
    if (base > end) {
        return SIC_ERROR_INVALID_SIZE;
    }

    p_sic->arba = base;
    p_sic->area = end;

    /* Set the DIGEST_SIZE field */
    p_sic->ccr &= ~(0x3u << 5);
    p_sic->ccr |= (digest_size & 0x3u) << 5;

    if (digest_size == SIC_DIGEST_SIZE_128) {
        /* Set the DIGEST_COMPARISON_MODE field */
        p_sic->ccr &= ~(0x1u << 4);
        p_sic->ccr |= (digest_config & 0x1u) << 4;
    }

    return SIC_ERROR_NONE;
}

enum sic_error_t sic_auth_enable(struct sic_dev_t *dev)
{
    struct _sic_reg_map_t* p_sic = (struct _sic_reg_map_t*)dev->cfg->base;

    /* The CCR register cannot be written while the SIC is enabled */
    if (is_sic_enabled(dev)) {
        return SIC_ERROR_INVALID_OP_WHILE_ENABLED;
    }

    /* Set the AUTHENTICATION_ENABLE field */
    p_sic->ccr |= (0x1u << 1);

    /* Set the AENG_CLOCK_GATING_ENABLE field */
    p_sic->ccr |= (0x1u << 8);

    /* Unset the PAGE_INVALIDATE field */
    p_sic->aecr &= ~(0x1u);

    return SIC_ERROR_NONE;
}

enum sic_error_t sic_auth_disable(struct sic_dev_t *dev)
{
    struct _sic_reg_map_t* p_sic = (struct _sic_reg_map_t*)dev->cfg->base;
    enum sic_error_t err;

    /* The CCR register cannot be written while the SIC is enabled */
    if (is_sic_enabled(dev)) {
        return SIC_ERROR_INVALID_OP_WHILE_ENABLED;
    }

    /* Unset the AUTHENTICATION_ENABLE field */
    p_sic->ccr &= ~(0x1u << 1);

    /* Unset the AENG_CLOCK_GATING_ENABLE field */
    p_sic->ccr &= ~(0x1u << 8);

    err = sic_auth_invalidate_pages(dev);

    return err;
}

enum sic_error_t sic_auth_invalidate_pages(struct sic_dev_t *dev)
{
    struct _sic_reg_map_t* p_sic = (struct _sic_reg_map_t*)dev->cfg->base;

    /* The AECR register cannot be written while the SIC is enabled */
    if (is_sic_enabled(dev)) {
        return SIC_ERROR_INVALID_OP_WHILE_ENABLED;
    }

    /* Set the PAGE_INVALIDATE field */
    p_sic->aecr |= 0x1u;

    return SIC_ERROR_NONE;
}

enum sic_error_t sic_auth_axim_cfg_set(struct sic_dev_t *dev,
                                       const struct sic_auth_axim_config_t *cfg)
{
    struct _sic_reg_map_t* p_sic = (struct _sic_reg_map_t*)dev->cfg->base;

    /* The AECR register cannot be written while the SIC is enabled */
    if (is_sic_enabled(dev)) {
        return SIC_ERROR_INVALID_OP_WHILE_ENABLED;
    }


    /* Set the AXIM_ARPROT0_CONTROL field */
    p_sic->aecr &= ~(0x3u << 18);
    p_sic->aecr |= (cfg->priv & 0x3u) << 18;

    /* Set the AXIM_ARPROT1_CONTROL field */
    p_sic->aecr &= ~(0x3u << 20);
    p_sic->aecr |= (cfg->secure & 0x3u) << 20;

    /* Set the AXIM_ARPROT2_CONTROL field */
    p_sic->aecr &= ~(0x3u << 22);
    p_sic->aecr |= (cfg->dinst & 0x3u) << 22;

    /* Set the AXIM_ARCACHE0_CONTROL field */
    p_sic->aecr &= ~(0x3u << 24);
    p_sic->aecr |= (cfg->bufferable & 0x3u) << 24;

    /* Set the AXIM_ARCACHE1_CONTROL field */
    p_sic->aecr &= ~(0x3u << 26);
    p_sic->aecr |= (cfg->modifiable & 0x3u) << 26;

    /* Set the AXIM_ARCACHE2_CONTROL field */
    p_sic->aecr &= ~(0x3u << 28);
    p_sic->aecr |= (cfg->allocate & 0x3u) << 28;

    /* Set the AXIM_ARCACHE3_CONTROL field */
    p_sic->aecr &= ~(0x3u << 30);
    p_sic->aecr |= (cfg->other_allocate & 0x3u) << 30;

    return SIC_ERROR_NONE;
}

enum sic_error_t sic_auth_axim_cfg_get(struct sic_dev_t *dev,
                                       struct sic_auth_axim_config_t *cfg)
{
    struct _sic_reg_map_t* p_sic = (struct _sic_reg_map_t*)dev->cfg->base;

    /* Get the AXIM_ARPROT0_CONTROL field */
    cfg->priv = (p_sic->aecr >> 18) & 0x3u;

    /* Get the AXIM_ARPROT1_CONTROL field */
    cfg->secure = (p_sic->aecr >> 20) & 0x3u;

    /* Get the AXIM_ARPROT2_CONTROL field */
    cfg->dinst = (p_sic->aecr >> 22) & 0x3u;

    /* Get the AXIM_ARCACHE0_CONTROL field */
    cfg->bufferable = (p_sic->aecr >> 24) & 0x3u;

    /* Get the AXIM_ARCACHE1_CONTROL field */
    cfg->modifiable = (p_sic->aecr >> 26) & 0x3u;

    /* Get the AXIM_ARCACHE2_CONTROL field */
    cfg->allocate = (p_sic->aecr >> 28) & 0x3u;

    /* Get the AXIM_ARCACHE3_CONTROL field */
    cfg->other_allocate = (p_sic->aecr >> 30) & 0x3u;

    return SIC_ERROR_NONE;
}

enum sic_error_t sic_auth_table_set(struct sic_dev_t *dev, uint32_t *data,
                                    size_t data_len_bytes, size_t table_offset)
{
    struct _sic_reg_map_t* p_sic = (struct _sic_reg_map_t*)dev->cfg->base;
    size_t idx;

    /* The tables cannot be written while the SIC is enabled */
    if (is_sic_enabled(dev)) {
        return SIC_ERROR_INVALID_OP_WHILE_ENABLED;
    }

    if (table_offset + data_len_bytes >= (sic_page_count_get(dev) * 32)) {
        return SIC_ERROR_INVALID_ADDRESS;
    }

    for (idx = 0; idx < data_len_bytes / 4; idx++) {
        p_sic->htr[(table_offset / 4) + idx] = data[idx];
    }

    return SIC_ERROR_NONE;
}

enum sic_error_t sic_decrypt_init(struct sic_dev_t *dev,
                                  enum sic_decrypt_keysize_t decrypt_keysize,
                                  bool decrypt_padding_enable)
{
    struct _sic_reg_map_t* p_sic = (struct _sic_reg_map_t*)dev->cfg->base;

    /* The CCR register cannot be written while the SIC is enabled */
    if (is_sic_enabled(dev)) {
        return SIC_ERROR_INVALID_OP_WHILE_ENABLED;
    }

    /* Set the DECRYPTION_KEY_SIZE field */
    p_sic->ccr &= ~(0x1u << 3);
    p_sic->ccr |= (decrypt_keysize & 0x1u) << 3;

    /* Set the AENG_PADDING_ENABLE field */
    p_sic->ccr &= ~(0x1u << 12);
    p_sic->ccr |= (decrypt_padding_enable & 0x1u) << 12;

    /* Set the Decryption Enable bit */
    p_sic->ccr |= 0x1u << 2;

    return SIC_ERROR_NONE;
}

static enum sic_error_t check_region_overlap(struct sic_dev_t *dev,
                                             uint8_t region_idx, uintptr_t base,
                                             size_t size)
{
    struct _sic_reg_map_t* p_sic = (struct _sic_reg_map_t*)dev->cfg->base;
    uintptr_t region_base;
    uintptr_t region_size;
    size_t idx;

    for (idx = 0; idx < get_dr_am(dev); idx++) {
        /* Skip the current region, in case we're reenabling a region that was
         * previously enabled */
        if (idx == region_idx) {
            continue;
        }

        region_base = (p_sic->dr[idx].drr & 0xFFFFFu) << 12;
        region_size = ((p_sic->dr[idx].drr >> 20) & 0x7FF) << 12;

        /* Skip regions that have not been configured */
        if (region_size == 0) {
            continue;
        }

        if ((base > region_base && base < region_base + region_size)
         || (region_base > base && region_base < base + size)) {
            return SIC_ERROR_INVALID_ADDRESS;
        }
    }

    return SIC_ERROR_NONE;
}

enum sic_error_t sic_decrypt_region_enable(struct sic_dev_t *dev,
                                           uint8_t region_idx,
                                           uintptr_t base, size_t size,
                                           uint32_t fw_revision,
                                           uint32_t *nonce, uint32_t *key)
{
    struct _sic_reg_map_t* p_sic = (struct _sic_reg_map_t*)dev->cfg->base;
    enum sic_error_t err;
    uint32_t idx;
    size_t key_len;

    /* Size must be 4KiB aligned */
    if (size & 0xFFFu) {
        return SIC_ERROR_INVALID_ALIGNMENT;
    }

    /* decrypt region must not be larger than 8MiB - 4 KiB */
    if (size == 0x0u || size >= (0x800000u - 4096u)) {
        return SIC_ERROR_INVALID_SIZE;
    }

    if (region_idx >= get_dr_am(dev)) {
        return SIC_ERROR_INVALID_REGION;
    }

    /* It's not valid to have a region overlap another region */
    err = check_region_overlap(dev, region_idx, base, size);
    if (err != SIC_ERROR_NONE) {
        return err;
    }

    /* Address is right-shifted by 12 */
    p_sic->dr[region_idx].drr &= ~0xFFFFFu;
    p_sic->dr[region_idx].drr |= (base >> 12);

    /* Size is in the uppermost 12 bits. It's measured in 4k pages, so
     * right-shift the size by 12 to get the page amount. Bit 31 _must not_ be
     * set
     */
    p_sic->dr[region_idx].drr &= ~0xFFF00000u;
    p_sic->dr[region_idx].drr |= ((size >> 12) & 0x7FF) << 20;

    /* The FW revision is used as the dr-specific portion of the IV */
    p_sic->dr[region_idx].div = fw_revision;

    /* The Nonce is 2 words */
    for (idx = 0; idx < 2; idx++) {
        p_sic->dr[region_idx].dnw[idx] = nonce[idx];
    }

    /* The key is 4 words, plus another 4 if the 256-bit key bit is set in the
     * CCR
     */
    key_len = 4 + (((p_sic->ccr >> 3) & 0x1) * 4);
    for (idx = 0; idx < key_len; idx++) {
        p_sic->dr[region_idx].dkw[idx] = key[idx];
    }

    return SIC_ERROR_NONE;
}

enum sic_error_t sic_decrypt_region_disable(struct sic_dev_t *dev,
                                            uint8_t region_idx)
{
    struct _sic_reg_map_t* p_sic = (struct _sic_reg_map_t*)dev->cfg->base;

    if (region_idx >= get_dr_am(dev)) {
        return SIC_ERROR_INVALID_REGION;
    }

    /* Address is right-shifted by 12 */
    p_sic->dr[region_idx].drr = 0x0u;

    return SIC_ERROR_NONE;
}

enum sic_error_t sic_decrypt_mitigations_set(struct sic_dev_t *dev,
                                             const struct sic_decrypt_mitigations_config_t *cfg)
{
    struct _sic_reg_map_t* p_sic = (struct _sic_reg_map_t*)dev->cfg->base;

    /* The AESCR register cannot be written while the SIC is enabled */
    if (is_sic_enabled(dev)) {
        return SIC_ERROR_INVALID_OP_WHILE_ENABLED;
    }

    /* Set the AES_DFA_EN field */
    p_sic->aescr &= ~(0x1u << 0);
    p_sic->aescr |= (cfg->aes_dfa_enable & 0x1u) << 0;

    /* Set the AES_DUMMY_EN field */
    p_sic->aescr &= ~(0x1u << 1);
    p_sic->aescr |= (cfg->aes_dummy_enable & 0x1u) << 1;

    /* Set the AES_DUMMY_VALID_PHASE_EN field */
    p_sic->aescr &= ~(0x1u << 2);
    p_sic->aescr |= (cfg->aes_dummy_valid_phase_enable & 0x1u) << 2;

    /* Set the AES_DUMMY_VALID_KEY_EN field */
    p_sic->aescr &= ~(0x1u << 3);
    p_sic->aescr |= (cfg->aes_dummy_valid_key_enable & 0x1u) << 3;

    /* Set the AES_DR_PRE_MAX_ROUNDS field */
    p_sic->aescr &= ~(0x7u << 4);
    p_sic->aescr |= (cfg->aes_dr_pre_rounds_max & 0x7u) << 4;

    /* Set the AES_DR_POST_MAX_ROUNDS field */
    p_sic->aescr &= ~(0x7u << 7);
    p_sic->aescr |= (cfg->aes_dr_post_rounds_max & 0x7u) << 7;

    /* Set the AES_DR_VALID_MAX_ROUNDS field */
    p_sic->aescr &= ~(0x3u << 10);
    p_sic->aescr |= (cfg->aes_dr_post_rounds_max & 0x3u) << 10;

    return SIC_ERROR_NONE;
}

enum sic_error_t sic_decrypt_mitigations_get(struct sic_dev_t *dev,
                                             struct sic_decrypt_mitigations_config_t *cfg)
{
    struct _sic_reg_map_t* p_sic = (struct _sic_reg_map_t*)dev->cfg->base;

    /* Get the AES_DFA_EN field */
    cfg->aes_dfa_enable = (p_sic->aescr >> 0) & 0x1u;

    /* Get the AES_DUMMY_EN field */
    cfg->aes_dummy_enable = (p_sic->aescr >> 1) & 0x1u;

    /* Get the AES_DUMMY_VALID_PHASE_EN field */
    cfg->aes_dummy_valid_phase_enable = (p_sic->aescr >> 2) & 0x1u;

    /* Get the AES_DUMMY_VALID_KEY_EN field */
    cfg->aes_dummy_valid_key_enable = (p_sic->aescr >> 3) & 0x1u;

    /* Get the AES_DR_PRE_MAX_ROUNDS field */
    cfg->aes_dr_pre_rounds_max = (p_sic->aescr >> 4) & 0x7u;

    /* Get the AES_DR_POST_MAX_ROUNDS field */
    cfg->aes_dr_post_rounds_max = (p_sic->aescr >> 7) & 0x7u;

    /* Get the AES_DR_VALID_MAX_ROUNDS field */
    cfg->aes_dr_valid_rounds_max = (p_sic->aescr >> 10) & 0x3u;

    return SIC_ERROR_NONE;
}

enum sic_error_t sic_decrypt_rbg_seed_set(struct sic_dev_t *dev,
                                          const uint8_t *seed,
                                          size_t seed_len)
{
    struct _sic_reg_map_t* p_sic = (struct _sic_reg_map_t*)dev->cfg->base;
    uint32_t seed_start;
    uint32_t idx;

    /* The AESRBGRC register cannot be written while the SIC is enabled */
    if (is_sic_enabled(dev)) {
        return SIC_ERROR_INVALID_OP_WHILE_ENABLED;
    }

    if (p_sic->aesrbgcr == 16) {
        return SIC_ERROR_RBG_SEED_ALREADY_SET;
    }

    seed_start = p_sic->aesrbgcr;

    for (idx = seed_start; idx < 16 && idx < seed_start + seed_len; idx++) {
        p_sic->aesrbgr = seed[idx];

        while(p_sic->aesrbgcr == idx) {}
    }

    return SIC_ERROR_NONE;
}

enum sic_error_t sic_pmon_enable(struct sic_dev_t *dev,
                                 enum sic_pmon_counting_mode_t counting_mode,
                                 bool timer_enable, uint32_t timer_val)
{
    struct _sic_reg_map_t* p_sic = (struct _sic_reg_map_t*)dev->cfg->base;

    if (!have_pmon(dev)) {
        return SIC_ERROR_NO_HW_SUPPORT;
    }

    /* Set the PMON_TIMER_ENABLE field */
    p_sic->pmcr &= ~(0x1u << 1);
    p_sic->pmcr |= (timer_enable & 0x1u) << 1;

    /* Set the PMON_COUNTING_MODE field */
    p_sic->pmcr &= ~(0x1u << 2);
    p_sic->pmcr |= (counting_mode & 0x1u) << 2;

    /* Set the PMON_TIMER field */
    p_sic->pmcr &= ~(0xFFFFFFu << 8);
    if (timer_enable) {
        p_sic->pmcr |= (counting_mode & 0xFFFFFFu) << 8;
    }

    /* Set the PMON_ENABLE field */
    p_sic->pmcr |= 0x1u;

    return SIC_ERROR_NONE;
}

enum sic_error_t sic_pmon_disable(struct sic_dev_t *dev)
{
    struct _sic_reg_map_t* p_sic = (struct _sic_reg_map_t*)dev->cfg->base;

    if (!have_pmon(dev)) {
        return SIC_ERROR_NO_HW_SUPPORT;
    }

    /* Set the PMON_ENABLE field */
    p_sic->pmcr &= ~(0x1u);

    return SIC_ERROR_NONE;
}

enum sic_error_t sic_pmon_get_stats(struct sic_dev_t *dev,
                                    struct sic_pmon_counters_t *counters)
{
    struct _sic_reg_map_t* p_sic = (struct _sic_reg_map_t*)dev->cfg->base;

    if (!have_pmon(dev)) {
        return SIC_ERROR_NO_HW_SUPPORT;
    }

    counters->page_hit_counter  = p_sic->pmphc;
    counters->page_miss_counter = p_sic->pmpmc;
    counters->bypass_counter    = p_sic->pmbc;

    return SIC_ERROR_NONE;
}
