/*
 * Copyright (c) 2021-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "cc3xx_aes.h"

#include "cc3xx_dev.h"
#include "cc3xx_dma.h"
#include "cc3xx_lcs.h"
#include "cc3xx_engine_state.h"
#include "device_definition.h"

static cc3xx_err_t cc3xx_aes_dumpiv(cc3xx_aes_mode_t mode, uint8_t *iv)
{
    int32_t idx;

    switch (mode) {
    case CC3XX_AES_MODE_CTR:
        for (idx = 3; idx >= 0; idx--) {
            ((uint32_t*)iv)[idx] = P_CC3XX->aes.aes_ctr_0[idx];
        }
        break;
    case CC3XX_AES_MODE_ECB:
        return CC3XX_ERR_SUCCESS;
    default:
        return CC3XX_ERR_NOT_IMPLEMENTED;
    }

    return CC3XX_ERR_SUCCESS;
}

static cc3xx_err_t cc3xx_aes_loadiv(cc3xx_aes_mode_t mode, const uint8_t *iv)
{
    int32_t idx;

    switch (mode) {
    case CC3XX_AES_MODE_CTR:
        for (idx = 3; idx >= 0; idx--) {
            P_CC3XX->aes.aes_ctr_0[idx] = ((uint32_t*)iv)[idx];
        }
        break;
    case CC3XX_AES_MODE_ECB:
        return CC3XX_ERR_SUCCESS;
    default:
        return CC3XX_ERR_NOT_IMPLEMENTED;
    }

    return CC3XX_ERR_SUCCESS;
}

#ifdef KMU_S
static cc3xx_err_t cc3xx_aes_setkey(cc3xx_aes_key_id_t key_id,
                                    const uint8_t *key,
                                    cc3xx_aes_keysize_t key_size)
{
    enum kmu_error_t kmu_err;
    int32_t idx;

    if (P_CC3XX->ao.host_ao_lock_bits & 0x1U) {
        return CC3XX_ERR_INVALID_STATE;
    }

    if (key_id != CC3XX_AES_KEY_ID_USER_KEY) {
        switch (key_id) {
        case CC3XX_AES_KEY_ID_HUK:
            kmu_err = kmu_export_key(&KMU_DEV_S, KMU_HW_SLOT_HUK);
            break;
        case CC3XX_AES_KEY_ID_KRTL:
            kmu_err = kmu_export_key(&KMU_DEV_S, KMU_HW_SLOT_KRTL);
            break;
        case CC3XX_AES_KEY_ID_KCP:
            kmu_err = kmu_export_key(&KMU_DEV_S, KMU_HW_SLOT_KP_CM);
            break;
        case CC3XX_AES_KEY_ID_KCE:
            kmu_err = kmu_export_key(&KMU_DEV_S, KMU_HW_SLOT_KCE_CM);
            break;
        case CC3XX_AES_KEY_ID_KPICV:
            kmu_err = kmu_export_key(&KMU_DEV_S, KMU_HW_SLOT_KP_DM);
            break;
        case CC3XX_AES_KEY_ID_KCEICV:
            kmu_err = kmu_export_key(&KMU_DEV_S, KMU_HW_SLOT_KCE_DM);
            break;
        case CC3XX_AES_KEY_ID_GUK:
            kmu_err = kmu_export_key(&KMU_DEV_S, KMU_HW_SLOT_GUK);
            break;
        default:
            return CC3XX_ERR_NOT_IMPLEMENTED;
        }
        if (kmu_err != KMU_ERROR_NONE) {
            return CC3XX_ERR_KEY_IMPORT_FAILED;
        }
    } else {
        switch (key_size) {
        case CC3XX_AES_KEYSIZE_256:
            for (idx = 7; idx >= 6; idx--) {
                P_CC3XX->aes.aes_key_0[idx] = ((uint32_t*)key)[idx];
            }
        case CC3XX_AES_KEYSIZE_192:
            for (idx = 5; idx >= 4; idx--) {
                P_CC3XX->aes.aes_key_0[idx] = ((uint32_t*)key)[idx];
            }
        case CC3XX_AES_KEYSIZE_128:
            for (idx = 3; idx >= 0; idx--) {
                P_CC3XX->aes.aes_key_0[idx] = ((uint32_t*)key)[idx];
            }
            break;
        default:
            return CC3XX_ERR_NOT_IMPLEMENTED;
        }
    }

    /* Set key size */
    P_CC3XX->aes.aes_control &= ~(0b11U << 12);
    P_CC3XX->aes.aes_control |= (key_size & 0b11U) << 12;

    return CC3XX_ERR_SUCCESS;
}
#else
static cc3xx_err_t cc3xx_aes_check_key_lock(cc3xx_aes_key_id_t key_id)
{
    cc3xx_err_t err = CC3XX_ERR_SUCCESS;
    cc3xx_lcs_t lcs;

    switch (key_id) {
    case CC3XX_AES_KEY_ID_HUK:
        break;
    case CC3XX_AES_KEY_ID_KRTL:
        err = cc3xx_lcs_get(&lcs);
        if (err != CC3XX_ERR_SUCCESS) {
            return err;
        }
        /* The RTL key is only valid in certain states */
        if (! (lcs == (cc3xx_lcs_cm | cc3xx_lcs_dm))) {
            return CC3XX_ERR_INVALID_LCS;
        }
        break;
    case CC3XX_AES_KEY_ID_KCP:
        if (P_CC3XX->ao.host_ao_lock_bits & (0x1U << 3)) {
            return CC3XX_ERR_INVALID_STATE;
        }
        break;
    case CC3XX_AES_KEY_ID_KCE:
        if (P_CC3XX->ao.host_ao_lock_bits & (0x1U << 4)) {
            return CC3XX_ERR_INVALID_STATE;
        }
        break;
    case CC3XX_AES_KEY_ID_KPICV:
        if (P_CC3XX->ao.host_ao_lock_bits & (0x1U << 1)) {
            return CC3XX_ERR_INVALID_STATE;
        }
        break;
    case CC3XX_AES_KEY_ID_KCEICV:
        if (P_CC3XX->ao.host_ao_lock_bits & (0x1U << 2)) {
            return CC3XX_ERR_INVALID_STATE;
        }
        break;
    case CC3XX_AES_KEY_ID_USER_KEY:
        break;
    default:
        return CC3XX_ERR_SUCCESS;
    }

    return CC3XX_ERR_SUCCESS;
}

static cc3xx_err_t cc3xx_aes_setkey(cc3xx_aes_key_id_t key_id,
                                    const uint8_t *key,
                                    cc3xx_aes_keysize_t key_size)
{
    cc3xx_err_t err = CC3XX_ERR_SUCCESS;
    int32_t idx;

    if (key_id != CC3XX_AES_KEY_ID_USER_KEY) {
        /* Check if the HOST_FATAL_ERROR mode is enabled */
        if (P_CC3XX->ao.host_ao_lock_bits & 0x1U) {
            return CC3XX_ERR_INVALID_STATE;
        }

        /* If the KMU is not integrated, there are limited keys */
        if (key_id > CC3XX_AES_KEY_ID_KCEICV) {
            return CC3XX_ERR_NOT_IMPLEMENTED;
        }

        /* Check if the key is masked / locked */
        err = cc3xx_aes_check_key_lock(key_id);
        if (err != CC3XX_ERR_SUCCESS) {
            return err;
        }

        /* Select the required key */
        P_CC3XX->host_rgf.host_cryptokey_sel = key_id;

        /* Trigger the load into the key registers */
        P_CC3XX->aes.aes_sk = 0b1U;
    } else {
        switch (key_size) {
        case CC3XX_AES_KEYSIZE_256:
            for (idx = 7; idx >= 6; idx--) {
                P_CC3XX->aes.aes_key_0[idx] = ((uint32_t*)key)[idx];
            }
        case CC3XX_AES_KEYSIZE_192:
            for (idx = 5; idx >= 4; idx--) {
                P_CC3XX->aes.aes_key_0[idx] = ((uint32_t*)key)[idx];
            }
        case CC3XX_AES_KEYSIZE_128:
            for (idx = 3; idx >= 0; idx--) {
                P_CC3XX->aes.aes_key_0[idx] = ((uint32_t*)key)[idx];
            }
            break;
        default:
            return CC3XX_ERR_NOT_IMPLEMENTED;
        }
    }

    /* Set key size */
    P_CC3XX->aes.aes_control &= ~(0b11U << 12);
    P_CC3XX->aes.aes_control |= (key_size & 0b11U) << 12;

    return CC3XX_ERR_SUCCESS;
}
#endif /* KMU_S */

cc3xx_err_t cc3xx_aes(cc3xx_aes_key_id_t key_id, const uint8_t *key,
                      cc3xx_aes_keysize_t key_size, const uint8_t* in,
                      size_t in_len, uint8_t* iv, uint8_t *out,
                      cc3xx_aes_direction_t direction, cc3xx_aes_mode_t mode)
{
    cc3xx_err_t err = CC3XX_ERR_SUCCESS;

    if (cc3xx_engine_in_use) {
        /* Since the AES operation isn't restartable, just check that the engine
         * isn't in use when we begin.
         */
        return CC3XX_ERR_ENGINE_IN_USE;
    }

    /* Enable the aes engine clock */
    P_CC3XX->misc.aes_clk_enable = 0x1U;

    /* Enable the DMA clock */
    P_CC3XX->misc.dma_clk_enable = 0x1U;

    /* Wait for the crypto engine to be ready */
    while (P_CC3XX->cc_ctl.crypto_busy) {}

    /* Set the crypto engine to the AES engine */
    P_CC3XX->cc_ctl.crypto_ctl = 0b00001U;

    /* Clear number of remaining bytes */
    P_CC3XX->aes.aes_remaining_bytes = 0x0U;

    /* Set direction field of AES control register */
    P_CC3XX->aes.aes_control &= ~0b1U;
    P_CC3XX->aes.aes_control |= (direction & 0b1U);

    /* Set mode field of AES control register */
    P_CC3XX->aes.aes_control &= ~(0b111U << 2);
    P_CC3XX->aes.aes_control |= (mode & 0b111U) << 2;

    /* Set up the key */
    err = cc3xx_aes_setkey(key_id, key, key_size);
    if (err != CC3XX_ERR_SUCCESS) {
        goto out;
    }

    /* Set up the IV */
    err = cc3xx_aes_loadiv(mode, iv);
    if (err != CC3XX_ERR_SUCCESS) {
        goto out;
    }

    /* Set up output */
    cc3xx_dma_set_output(out, in_len);

    /* Use DMA to input the data. This completes the encryption */
    err = cc3xx_dma_input_data(in, in_len);

    /* Dump the IV so multipart works */
    err = cc3xx_aes_dumpiv(mode, iv);
    if (err != CC3XX_ERR_SUCCESS) {
        goto out;
    }

out:
    /* Set the crypto engine back to the default PASSTHROUGH engine */
    P_CC3XX->cc_ctl.crypto_ctl = 0x0;

    /* Disable the hash engine clock */
    P_CC3XX->misc.aes_clk_enable = 0x0U;

    /* Disable the DMA clock */
    P_CC3XX->misc.dma_clk_enable = 0x0U;

    return err;
}
