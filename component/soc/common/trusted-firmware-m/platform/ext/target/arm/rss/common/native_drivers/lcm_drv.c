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
 * \file lcm_drv.c
 * \brief Driver for Arm LCM.
 */

#include "lcm_drv.h"
#include <stddef.h>
#include <stdint.h>

static uint8_t dummy_key_value[32] = {0x01, 0x02, 0x03, 0x04,
                                      0x01, 0x02, 0x03, 0x04,
                                      0x01, 0x02, 0x03, 0x04,
                                      0x01, 0x02, 0x03, 0x04,
                                      0x01, 0x02, 0x03, 0x04,
                                      0x01, 0x02, 0x03, 0x04,
                                      0x01, 0x02, 0x03, 0x04,
                                      0x01, 0x02, 0x03, 0x04};

struct _lcm_reg_map_t {
    volatile uint32_t lcs_value;
                /*!< Offset: 0x000 (R/ ) LCM Lifecycle state Register */
    volatile uint32_t key_err;
                /*!< Offset: 0x004 (R/ ) LCM zero count error status Register */
    volatile uint32_t tp_mode;
                /*!< Offset: 0x008 (R/ ) LCM TP Mode (TCI/PCI) Register */
    volatile uint32_t fatal_err;
                /*!< Offset: 0x00C (R/W) LCM Fatal Error mode Enable Register */
    volatile uint32_t dm_rma_lock;
                /*!< Offset: 0x010 (R/W) LCM DRM RMA Flag lock enable */
    volatile uint32_t sp_enable;
                /*!< Offset: 0x014 (R/W) LCM Secure Provisioning enable
                 *                       Register */
    volatile uint32_t otp_addr_width;
                /*!< Offset: 0x018 (R/ ) LCM OTP Address Width Register  */
    volatile uint32_t otp_size_in_bytes;
                /*!< Offset: 0x01C (R/ ) LCM OTP Size in bytes Register */
    volatile uint32_t gppc;
                /*!< Offset: 0x020 (R/ ) LCM General Purpose Persistent
                 *                       Configuration Register */
    volatile uint32_t reserved_0[55];
                /*!< Offset: 0x024-0x0FC Reserved */
    volatile uint32_t dcu_en[4];
                /*!< Offset: 0x100 (R/W) LCM DCU enable Registers */
    volatile uint32_t dcu_lock[4];
                /*!< Offset: 0x110 (R/W) LCM DCU lock Registers */
    volatile uint32_t dcu_sp_disable_mask[4];
                /*!< Offset: 0x120 (R/ ) LCM DCU SP disable mask Registers */
    volatile uint32_t dcu_disable_mask[4];
                /*!< Offset: 0x130 (R/ ) LCM DCU disable mask Registers */
    volatile uint32_t reserved_1[932];
                /*!< Offset: 0x140-0xFCC Reserved */
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
    union {
        volatile uint32_t raw_otp[16384];
                /*!< Offset: 0x1000 (R/W) OTP direct access */
        struct lcm_otp_layout_t otp;
    };
};

static int is_pointer_word_aligned(void *ptr) {
    return !((uint32_t)ptr & (sizeof(uint32_t) - 1));
}

enum lcm_error_t lcm_init(struct lcm_dev_t *dev)
{
    struct _lcm_reg_map_t *p_lcm = (struct _lcm_reg_map_t *)dev->cfg->base;

    if (p_lcm->key_err) {
        return LCM_ERROR_INVALID_KEY;
    }

    return LCM_ERROR_NONE;
}

enum lcm_error_t lcm_get_tp_mode(struct lcm_dev_t *dev, enum lcm_tp_mode_t *mode)
{
    struct _lcm_reg_map_t *p_lcm = (struct _lcm_reg_map_t *)dev->cfg->base;

    *mode = (enum lcm_tp_mode_t)p_lcm->tp_mode;

    return LCM_ERROR_NONE;
}

enum lcm_error_t lcm_set_tp_mode(struct lcm_dev_t *dev, enum lcm_tp_mode_t mode)
{
    enum lcm_tp_mode_t curr_mode;
    enum lcm_lcs_t lcs;
    uint32_t mode_reg_value;
    uint32_t readback_reg_value;
    enum lcm_bool_t fatal_err;
    enum lcm_error_t err;

    err = lcm_get_lcs(dev, &lcs);
    if (err != LCM_ERROR_NONE) {
        return err;
    }

    if (lcs != LCM_LCS_CM) {
        return LCM_ERROR_INVALID_LCS;
    }

    err = lcm_get_tp_mode(dev, &curr_mode);
    if (err != LCM_ERROR_NONE) {
        return err;
    }

    if(curr_mode != LCM_TP_MODE_VIRGIN) {
        return LCM_ERROR_INVALID_TRANSITION;
    }

    switch(mode) {
    case LCM_TP_MODE_TCI:
        /* High hamming-weight magic constant used to enable TCI mode */
        mode_reg_value = 0x0000FFFFu;
        break;
    case LCM_TP_MODE_PCI:
        /* High hamming-weight magic constant used to enable PCI mode */
        mode_reg_value = 0xFFFF0000u;
        break;
    default:
        return LCM_ERROR_INVALID_TRANSITION;
    }

    err = lcm_otp_write(dev, offsetof(struct lcm_otp_layout_t, tp_mode_config),
                        sizeof(uint32_t), (uint8_t *)&mode_reg_value);
    if (err != LCM_ERROR_NONE) {
        return err;
    }

    err = lcm_otp_read(dev, offsetof(struct lcm_otp_layout_t, tp_mode_config),
                       sizeof(uint32_t), (uint8_t *)&readback_reg_value);
    if (err != LCM_ERROR_NONE) {
        return err;
    }

    if (readback_reg_value != mode_reg_value) {
        return LCM_ERROR_INTERNAL_ERROR;
    }

    err = lcm_get_fatal_error(dev, &fatal_err);
    if (err != LCM_ERROR_NONE) {
        return err;
    }

    if (fatal_err == LCM_TRUE) {
        return LCM_ERROR_FATAL_ERR;
    }

    return LCM_ERROR_NONE;
}

enum lcm_error_t lcm_get_sp_enabled(struct lcm_dev_t *dev, enum lcm_bool_t *enabled)
{
    struct _lcm_reg_map_t *p_lcm = (struct _lcm_reg_map_t *)dev->cfg->base;

    *enabled = (enum lcm_bool_t)p_lcm->sp_enable;

    return LCM_ERROR_NONE;
}

enum lcm_error_t lcm_set_sp_enabled(struct lcm_dev_t *dev)
{
    struct _lcm_reg_map_t *p_lcm = (struct _lcm_reg_map_t *)dev->cfg->base;
    enum lcm_bool_t fatal_err;
    enum lcm_error_t err;

    /* High hamming-weight magic constant used to trigger secure provisioning
     * mode
     */
    p_lcm->sp_enable = 0x5EC10E1Eu;

    while(p_lcm->sp_enable != LCM_TRUE) {}

    err = lcm_get_fatal_error(dev, &fatal_err);
    if (err != LCM_ERROR_NONE) {
        return err;
    }

    if (fatal_err == LCM_TRUE) {
        return LCM_ERROR_FATAL_ERR;
    }

    return LCM_ERROR_NONE;
}

enum lcm_error_t lcm_get_fatal_error(struct lcm_dev_t *dev, enum lcm_bool_t *error)
{
    struct _lcm_reg_map_t *p_lcm = (struct _lcm_reg_map_t *)dev->cfg->base;

    *error = (enum lcm_bool_t)p_lcm->fatal_err;

    return LCM_ERROR_NONE;
}

enum lcm_error_t lcm_set_fatal_error(struct lcm_dev_t *dev)
{
    struct _lcm_reg_map_t *p_lcm = (struct _lcm_reg_map_t *)dev->cfg->base;

    /* High hamming-weight magic constant used to trigger fatal error state */
    p_lcm->fatal_err = 0xFA7A1EEEu;

    return LCM_ERROR_NONE;
}

enum lcm_error_t lcm_get_gppc(struct lcm_dev_t *dev, uint32_t *gppc)
{
    struct _lcm_reg_map_t *p_lcm = (struct _lcm_reg_map_t *)dev->cfg->base;

    *gppc = p_lcm->gppc;

    return LCM_ERROR_NONE;
}

enum lcm_error_t lcm_get_otp_size(struct lcm_dev_t *dev, uint32_t *size)
{
    struct _lcm_reg_map_t *p_lcm = (struct _lcm_reg_map_t *)dev->cfg->base;

    *size = p_lcm->otp_size_in_bytes;

    return LCM_ERROR_NONE;
}

enum lcm_error_t lcm_get_lcs(struct lcm_dev_t *dev, enum lcm_lcs_t *lcs)
{
    struct _lcm_reg_map_t *p_lcm = (struct _lcm_reg_map_t *)dev->cfg->base;
    enum lcm_bool_t fatal_err;
    enum lcm_error_t err;

    err = lcm_get_fatal_error(dev, &fatal_err);
    if (err != LCM_ERROR_NONE) {
        return err;
    }

    if (fatal_err == LCM_TRUE) {
        return LCM_ERROR_FATAL_ERR;
    }


    *lcs = (enum lcm_lcs_t)p_lcm->lcs_value;

    if (*lcs == LCM_LCS_INVALID) {
        return LCM_ERROR_INVALID_LCS;
    }

    return LCM_ERROR_NONE;
}

/* Armclang attempts to inline this function, which causes huge code size
 * increases. It is marked as __attribute__((noinline)) explicitly to prevent
 * this.
 */
__attribute__((noinline))
static enum lcm_error_t count_otp_zero_bits(struct lcm_dev_t *dev,
                                            uint32_t offset, uint32_t len,
                                            uint32_t *zero_bits)
{
    enum lcm_error_t err;
    uint32_t idx;
    uint32_t word;
    uint32_t bit_index;

    *zero_bits = 0;

    for (idx = 0; idx < len; idx += sizeof(uint32_t)) {
        err = lcm_otp_read(dev, offset + idx, sizeof(uint32_t), (uint8_t *)&word);
        if (err != LCM_ERROR_NONE) {
            return err;
        }

        for (bit_index = 0; bit_index < sizeof(word) * 8; bit_index++) {
            *zero_bits += 1 - ((word >> bit_index) & 1);
        }
    }

    return LCM_ERROR_NONE;
}

static enum lcm_error_t cm_to_dm(struct lcm_dev_t *dev, uint16_t gppc_val)
{
    enum lcm_error_t err;
    uint32_t config_val;
    uint32_t zero_bits;

    config_val = LCM_TRUE;

    err = count_otp_zero_bits(dev, offsetof(struct lcm_otp_layout_t, huk), 32,
                              &zero_bits);
    if (err != LCM_ERROR_NONE) {
        return err;
    }
    if (zero_bits == 256) {
        err = lcm_otp_write(dev, offsetof(struct lcm_otp_layout_t, huk), 32,
                            dummy_key_value);
        if (err != LCM_ERROR_NONE) {
            return err;
        }
    }

    err = count_otp_zero_bits(dev, offsetof(struct lcm_otp_layout_t, guk), 32,
                              &zero_bits);
    if (err != LCM_ERROR_NONE) {
        return err;
    }
    if (zero_bits == 256) {
        err = lcm_otp_write(dev, offsetof(struct lcm_otp_layout_t, guk), 32,
                            dummy_key_value);
        if (err != LCM_ERROR_NONE) {
            return err;
        }
    }

    err = count_otp_zero_bits(dev, offsetof(struct lcm_otp_layout_t, kp_cm), 32,
                              &zero_bits);
    if (err != LCM_ERROR_NONE) {
        return err;
    }
    if (zero_bits == 256) {
        err = lcm_otp_write(dev, offsetof(struct lcm_otp_layout_t, kp_cm), 32,
                            dummy_key_value);
        if (err != LCM_ERROR_NONE) {
            return err;
        }
    }

    err = count_otp_zero_bits(dev, offsetof(struct lcm_otp_layout_t, kce_cm), 32,
                              &zero_bits);
    if (err != LCM_ERROR_NONE) {
        return err;
    }
    if (zero_bits == 256) {
        err = lcm_otp_write(dev, offsetof(struct lcm_otp_layout_t, kce_cm), 32,
                            dummy_key_value);
        if (err != LCM_ERROR_NONE) {
            return err;
        }
    }

    err = lcm_otp_write(dev, offsetof(struct lcm_otp_layout_t, cm_config_1),
                        sizeof(uint32_t), (uint8_t *)&config_val);
    /* This OTP field doesn't read-back as written, but that isn't an error */
    if (!(err == LCM_ERROR_NONE || err == LCM_ERROR_WRITE_VERIFY_FAIL)) {
        return err;
    }

    err = lcm_otp_read(dev, offsetof(struct lcm_otp_layout_t, cm_config_1),
                       sizeof(uint32_t), (uint8_t *)&config_val);
    if (err != LCM_ERROR_NONE) {
        return err;
    }

    if (config_val != 0) {
        return LCM_ERROR_WRITE_VERIFY_FAIL;
    }

    config_val = 0;

    err = count_otp_zero_bits(dev, offsetof(struct lcm_otp_layout_t, rotpk),
                              32, &zero_bits);
    if (err != LCM_ERROR_NONE) {
        return err;
    }

    config_val |= (zero_bits & 0xFF) << 0;
    config_val |= ((uint32_t)gppc_val) << 8;

    err = lcm_otp_write(dev, offsetof(struct lcm_otp_layout_t, cm_config_2),
                        sizeof(uint32_t), (uint8_t *)&config_val);
    if (err != LCM_ERROR_NONE) {
        return err;
    }

    return LCM_ERROR_NONE;
}

static enum lcm_error_t dm_to_se(struct lcm_dev_t *dev)
{
    enum lcm_error_t err;
    uint32_t config_val;
    uint32_t zero_bits;

    err = count_otp_zero_bits(dev, offsetof(struct lcm_otp_layout_t, kp_dm), 32,
                              &zero_bits);
    if (err != LCM_ERROR_NONE) {
        return err;
    }
    if (zero_bits == 256) {
        err = lcm_otp_write(dev, offsetof(struct lcm_otp_layout_t, kp_dm), 32,
                            dummy_key_value);
        if (err != LCM_ERROR_NONE) {
            return err;
        }
    }

    err = count_otp_zero_bits(dev, offsetof(struct lcm_otp_layout_t, kce_dm), 32,
                              &zero_bits);
    if (err != LCM_ERROR_NONE) {
        return err;
    }
    if (zero_bits == 256) {
        err = lcm_otp_write(dev, offsetof(struct lcm_otp_layout_t, kce_dm), 32,
                            dummy_key_value);
        if (err != LCM_ERROR_NONE) {
            return err;
        }
    }

    config_val = LCM_TRUE;

    /* This OTP field doesn't read-back as written, but that isn't an error */
    err = lcm_otp_write(dev, offsetof(struct lcm_otp_layout_t, dm_config),
                        sizeof(uint32_t), (uint8_t *)&config_val);
    if (!(err == LCM_ERROR_NONE || err == LCM_ERROR_WRITE_VERIFY_FAIL)) {
        return err;
    }

    /* Manually check that the readback value is what we expect (0x0 means no
     * key bit count errors).
     */
    err = lcm_otp_read(dev, offsetof(struct lcm_otp_layout_t, dm_config),
                       sizeof(uint32_t), (uint8_t *)&config_val);
    if (err != LCM_ERROR_NONE) {
        return err;
    }

    if (config_val != 0) {
        return LCM_ERROR_WRITE_VERIFY_FAIL;
    }

    return LCM_ERROR_NONE;
}

static enum lcm_error_t se_to_rma(struct lcm_dev_t *dev)
{
    enum lcm_error_t err;
    uint32_t rma_flag = LCM_TRUE;
    uint32_t idx;
    uint32_t otp_overwrite_val = 0xFFFFFFFFu;

    for (idx = 0; idx < offsetof(struct lcm_otp_layout_t, tp_mode_config);
         idx += sizeof(uint32_t)) {
        err = lcm_otp_write(dev, idx, sizeof(otp_overwrite_val),
                            (uint8_t *)&otp_overwrite_val);
        if (err != LCM_ERROR_NONE) {
            return err;
        }
    }

    err = lcm_otp_write(dev, offsetof(struct lcm_otp_layout_t, cm_rma_flag),
                        sizeof(uint32_t), (uint8_t *)&rma_flag);
    if (err != LCM_ERROR_NONE) {
        return err;
    }

    err = lcm_otp_write(dev, offsetof(struct lcm_otp_layout_t, dm_rma_flag),
                        sizeof(uint32_t), (uint8_t *)&rma_flag);
    if (err != LCM_ERROR_NONE) {
        return err;
    }

    return LCM_ERROR_NONE;
}

enum lcm_error_t lcm_set_lcs(struct lcm_dev_t *dev, enum lcm_lcs_t lcs,
                             uint16_t gppc_val)
{
    enum lcm_bool_t fatal_err;
    enum lcm_bool_t sp_enable;
    enum lcm_tp_mode_t tp_mode;
    enum lcm_error_t err;
    enum lcm_lcs_t curr_lcs;

    err = lcm_get_lcs(dev, &curr_lcs);
    if (err != LCM_ERROR_NONE) {
        return err;
    }
    if (lcs == curr_lcs) {
        return LCM_ERROR_NONE;
    }

    err = lcm_get_tp_mode(dev, &tp_mode);
    if (err != LCM_ERROR_NONE) {
        return err;
    }
    if (!(tp_mode == LCM_TP_MODE_PCI || tp_mode == LCM_TP_MODE_TCI)) {
        return LCM_ERROR_INVALID_TP_MODE;
    }

    err = lcm_get_sp_enabled(dev, &sp_enable);
    if (err != LCM_ERROR_NONE) {
        return err;
    }
    if (sp_enable != LCM_TRUE) {
        err = lcm_set_sp_enabled(dev);
        if (err != LCM_ERROR_NONE) {
            return err;
        }
    }

    do {
        err = lcm_get_sp_enabled(dev, &sp_enable);
        if (err != LCM_ERROR_NONE) {
            return err;
        }
        err = lcm_get_fatal_error(dev, &fatal_err);
        if (err != LCM_ERROR_NONE) {
            return err;
        }
    } while (sp_enable == LCM_FALSE && fatal_err == LCM_FALSE);

    if (fatal_err == LCM_TRUE) {
        return LCM_ERROR_FATAL_ERR;
    }

    switch (lcs) {
    case LCM_LCS_CM:
        /* There's no possible valid transition back to CM */
        return LCM_ERROR_INVALID_TRANSITION;
    case LCM_LCS_DM:
        if (curr_lcs != LCM_LCS_CM) {
            return LCM_ERROR_INVALID_TRANSITION;
        }

        return cm_to_dm(dev, gppc_val);

    case LCM_LCS_SE:
        if (curr_lcs != LCM_LCS_DM) {
            return LCM_ERROR_INVALID_TRANSITION;
        }

        return dm_to_se(dev);

    case LCM_LCS_RMA:
        if (curr_lcs != LCM_LCS_SE) {
            return LCM_ERROR_INVALID_TRANSITION;
        }

        return se_to_rma(dev);

    case LCM_LCS_INVALID:
        return LCM_ERROR_INVALID_LCS;
    }

    /* Should never get here */
    return LCM_ERROR_INTERNAL_ERROR;
}


/* Armclang attempts to inline this function, which causes huge code size
 * increases. It is marked as __attribute__((noinline)) explicitly to prevent
 * this.
 */
__attribute__((noinline))
enum lcm_error_t lcm_otp_write(struct lcm_dev_t *dev, uint32_t offset, uint32_t len,
                               const uint8_t *buf)
{
    enum lcm_error_t err;
    struct _lcm_reg_map_t *p_lcm = (struct _lcm_reg_map_t *)dev->cfg->base;
    uint32_t *p_buf_word = (uint32_t *)buf;
    uint32_t otp_size;
    uint32_t idx;

    if (!is_pointer_word_aligned(p_buf_word)) {
        return LCM_ERROR_INVALID_ALIGNMENT;
    }

    if (offset & (sizeof(uint32_t) - 1)) {
        return LCM_ERROR_INVALID_OFFSET;
    }

    if (len & (sizeof(uint32_t) - 1)) {
        return LCM_ERROR_INVALID_LENGTH;
    }

    err = lcm_get_otp_size(dev, &otp_size);
    if (err) {
        return err;
    }

    if (otp_size <= (offset + len)) {
        return LCM_ERROR_INVALID_OFFSET;
    }

    /* Reject the write if the word is already written and we're not trying to
     * write the same word
     */
    for (idx = 0; idx < len / sizeof(uint32_t); idx++) {
        if (p_lcm->raw_otp[(offset / sizeof(uint32_t)) + idx] &&
           p_lcm->raw_otp[(offset / sizeof(uint32_t)) + idx] ^ p_buf_word[idx]) {
            return LCM_ERROR_INVALID_WRITE;
        }
    }

    for (idx = 0; idx < len / sizeof(uint32_t); idx++) {
        p_lcm->raw_otp[(offset / sizeof(uint32_t)) + idx] = p_buf_word[idx];
    }

    for (idx = 0; idx < len / sizeof(uint32_t); idx++) {
        if (p_buf_word[idx] != p_lcm->raw_otp[(offset / sizeof(uint32_t)) + idx]) {
            return LCM_ERROR_WRITE_VERIFY_FAIL;
        }
    }

    return LCM_ERROR_NONE;
}


/* Armclang attempts to inline this function, which causes huge code size
 * increases. It is marked as __attribute__((noinline)) explicitly to prevent
 * this.
 */
__attribute__((noinline))
enum lcm_error_t lcm_otp_read(struct lcm_dev_t *dev, uint32_t offset,
                              uint32_t len, uint8_t *buf)
{
    enum lcm_error_t err;
    struct _lcm_reg_map_t *p_lcm = (struct _lcm_reg_map_t *)dev->cfg->base;
    uint32_t *p_buf_word = (uint32_t *)buf;
    uint32_t otp_size;
    uint32_t idx;

    if (!is_pointer_word_aligned(p_buf_word)) {
        return LCM_ERROR_INVALID_ALIGNMENT;
    }

    if (offset & (sizeof(uint32_t) - 1)) {
        return LCM_ERROR_INVALID_OFFSET;
    }

    if (len & (sizeof(uint32_t) - 1)) {
        return LCM_ERROR_INVALID_LENGTH;
    }

    err = lcm_get_otp_size(dev, &otp_size);
    if (err) {
        return err;
    }

    if (otp_size <= (offset + len)) {
        return LCM_ERROR_INVALID_OFFSET;
    }

    for (idx = 0; idx < len / sizeof(uint32_t); idx++) {
        p_buf_word[idx] = p_lcm->raw_otp[(offset / sizeof(uint32_t)) + idx];
    }

    return LCM_ERROR_NONE;
}

enum lcm_error_t lcm_dcu_get_enabled(struct lcm_dev_t *dev, uint8_t *val)
{
    struct _lcm_reg_map_t *p_lcm = (struct _lcm_reg_map_t *)dev->cfg->base;
    uint32_t *p_val_word = (uint32_t *)val;
    uint32_t idx;

    if (!is_pointer_word_aligned(p_val_word)) {
        return LCM_ERROR_INVALID_ALIGNMENT;
    }

    for (idx = 0; idx < LCM_DCU_WIDTH_IN_BYTES / sizeof(uint32_t); idx++) {
        p_val_word[idx] = p_lcm->dcu_en[idx];
    }

    return LCM_ERROR_NONE;
}

enum lcm_error_t lcm_dcu_set_enabled(struct lcm_dev_t *dev, uint8_t *val)
{
    struct _lcm_reg_map_t *p_lcm = (struct _lcm_reg_map_t *)dev->cfg->base;
    uint32_t *p_val_word = (uint32_t *)val;
    uint32_t idx;

    if (!is_pointer_word_aligned(p_val_word)) {
        return LCM_ERROR_INVALID_ALIGNMENT;
    }

    for (idx = 0; idx < LCM_DCU_WIDTH_IN_BYTES / sizeof(uint32_t); idx++) {
        p_lcm->dcu_en[idx] = p_val_word[idx];
    }

    for (idx = 0; idx < LCM_DCU_WIDTH_IN_BYTES / sizeof(uint32_t); idx++) {
        if (p_lcm->dcu_en[idx] != p_val_word[idx]) {
            return LCM_ERROR_WRITE_VERIFY_FAIL;
        }
    }

    return LCM_ERROR_NONE;
}

enum lcm_error_t lcm_dcu_get_locked(struct lcm_dev_t *dev, uint8_t *val)
{
    struct _lcm_reg_map_t *p_lcm = (struct _lcm_reg_map_t *)dev->cfg->base;
    uint32_t *p_val_word = (uint32_t *)val;
    uint32_t idx;

    if (!is_pointer_word_aligned(p_val_word)) {
        return LCM_ERROR_INVALID_ALIGNMENT;
    }

    for (idx = 0; idx < LCM_DCU_WIDTH_IN_BYTES / sizeof(uint32_t); idx++) {
        p_val_word[idx] = p_lcm->dcu_lock[idx];
    }

    return LCM_ERROR_NONE;
}

enum lcm_error_t lcm_dcu_set_locked(struct lcm_dev_t *dev, uint8_t *val)
{
    struct _lcm_reg_map_t *p_lcm = (struct _lcm_reg_map_t *)dev->cfg->base;
    uint32_t *p_val_word = (uint32_t *)val;
    uint32_t idx;

    if (!is_pointer_word_aligned(p_val_word)) {
        return LCM_ERROR_INVALID_ALIGNMENT;
    }

    for (idx = 0; idx < LCM_DCU_WIDTH_IN_BYTES / sizeof(uint32_t); idx++) {
        p_lcm->dcu_lock[idx] = p_val_word[idx];
    }

    return LCM_ERROR_NONE;
}

enum lcm_error_t lcm_dcu_get_sp_disable_mask(struct lcm_dev_t *dev, uint8_t *val)
{
    struct _lcm_reg_map_t *p_lcm = (struct _lcm_reg_map_t *)dev->cfg->base;
    uint32_t *p_val_word = (uint32_t *)val;
    uint32_t idx;

    if (!is_pointer_word_aligned(p_val_word)) {
        return LCM_ERROR_INVALID_ALIGNMENT;
    }

    for (idx = 0; idx < LCM_DCU_WIDTH_IN_BYTES / sizeof(uint32_t); idx++) {
        p_val_word[idx] = p_lcm->dcu_sp_disable_mask[idx];
    }

    return LCM_ERROR_NONE;
}

enum lcm_error_t lcm_dcu_get_disable_mask(struct lcm_dev_t *dev, uint8_t *val)
{
    struct _lcm_reg_map_t *p_lcm = (struct _lcm_reg_map_t *)dev->cfg->base;
    uint32_t *p_val_word = (uint32_t *)val;
    uint32_t idx;

    if (!is_pointer_word_aligned(p_val_word)) {
        return LCM_ERROR_INVALID_ALIGNMENT;
    }

    for (idx = 0; idx < LCM_DCU_WIDTH_IN_BYTES / sizeof(uint32_t); idx++) {
         p_val_word[idx] = p_lcm->dcu_disable_mask[idx];
    }

    return LCM_ERROR_NONE;
}

