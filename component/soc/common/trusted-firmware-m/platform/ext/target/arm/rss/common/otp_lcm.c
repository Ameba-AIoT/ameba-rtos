/*
 * Copyright (c) 2021-2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_plat_otp.h"

#include "region_defs.h"
#include "cmsis_compiler.h"
#include "device_definition.h"
#include "lcm_drv.h"
#include "cmsis.h"
#include "uart_stdout.h"
#include "tfm_hal_platform.h"

__PACKED_STRUCT plat_user_area_layout_t {
    uint32_t boot_seed_zero_bits;
    uint32_t implementation_id_zero_bits;
    uint32_t cert_ref_zero_bits;
    uint32_t verification_service_url_zero_bits;
    uint32_t profile_definition_zero_bits;
    uint32_t iak_len_zero_bits;
    uint32_t iak_type_zero_bits;
    uint32_t iak_id_zero_bits;
    uint32_t bl2_rotpk_zero_bits[3];
    uint32_t bl2_encryption_key_zero_bits;
    uint32_t s_image_encryption_key_zero_bits;
    uint32_t ns_image_encryption_key_zero_bits;
    uint32_t bl1_2_image_hash_zero_bits;
    uint32_t bl2_image_hash_zero_bits;
    uint32_t bl1_rotpk_0_zero_bits;
    uint32_t secure_debug_pk_zero_bits;
    uint32_t host_rotpk_s_zero_bits;
    uint32_t host_rotpk_ns_zero_bits;
    uint32_t host_rotpk_cca_zero_bits;
    uint32_t cca_system_properties_zero_bits;

    uint32_t iak_len;
    uint32_t iak_type;
    uint32_t iak_id[8];

    uint32_t boot_seed[8];
    uint32_t implementation_id[8];
    uint32_t cert_ref[8];
    uint32_t verification_service_url[8];
    uint32_t profile_definition[8];

    uint32_t bl2_rotpk[3][8];
    uint32_t bl2_nv_counter[4][128];

    uint32_t bl2_encryption_key[8];
    uint32_t s_image_encryption_key[8];
    uint32_t ns_image_encryption_key[8];

    uint32_t bl1_2_image_hash[8];
    uint32_t bl2_image_hash[8];
    uint32_t bl1_nv_counter[128];

    uint32_t secure_debug_pk[8];

    uint32_t host_nv_counter[3][128];

    uint32_t bl1_rotpk_0[14];

    uint32_t host_rotpk_s[24];
    uint32_t host_rotpk_ns[24];
    uint32_t host_rotpk_cca[24];

    uint32_t cca_system_properties;

    uint32_t bl1_2_image[BL1_2_CODE_SIZE / sizeof(uint32_t)];
};

#ifdef NDEBUG
#define LOG(str)
#else
#define LOG(str) do { \
    stdio_output_string((const unsigned char *)str, sizeof(str) - 1); \
} while (0);
#endif /* NDEBUG */

#define OTP_OFFSET(x)       (offsetof(struct lcm_otp_layout_t, x))
#define OTP_SIZE(x)         (sizeof(((struct lcm_otp_layout_t *)0)->x))
#define USER_AREA_OFFSET(x) (OTP_OFFSET(user_data) + \
                             offsetof(struct plat_user_area_layout_t, x))
#define USER_AREA_SIZE(x)   (sizeof(((struct plat_user_area_layout_t *)0)->x))

static uint32_t count_buffer_zero_bits(const uint8_t* buf, size_t size)
{
    size_t byte_index;
    uint8_t byte;
    uint32_t one_count = 0;

    for (byte_index = 0; byte_index < size; byte_index++) {
        byte = buf[byte_index];
        for (int bit_index = 0; bit_index < 8; bit_index++) {
            one_count += (byte >> bit_index) & 1;
        }
    }

    return (size * 8) - one_count;
}

static enum tfm_plat_err_t otp_read(uint32_t offset, uint32_t len,
                                    uint32_t buf_len, uint8_t *buf)
{
    if (buf_len < len) {
        len = buf_len;
    }

    if (lcm_otp_read(&LCM_DEV_S, offset, len, buf) != LCM_ERROR_NONE) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    } else {
        return TFM_PLAT_ERR_SUCCESS;
    }
}

static enum tfm_plat_err_t otp_write(uint32_t offset, uint32_t len,
                                     uint32_t buf_len, const uint8_t *buf,
                                     uint32_t zero_count_offset)
{
    uint32_t zero_count;
    enum lcm_error_t err;

    if (buf_len > len) {
        return TFM_PLAT_ERR_INVALID_INPUT;
    }

    err = lcm_otp_write(&LCM_DEV_S, offset, buf_len, buf);
    if (err != LCM_ERROR_NONE) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    if (zero_count_offset != 0) {
        zero_count = count_buffer_zero_bits(buf, buf_len);

        err = lcm_otp_write(&LCM_DEV_S, zero_count_offset, sizeof(zero_count),
                            (uint8_t *)&zero_count);
        if (err != LCM_ERROR_NONE) {
            return TFM_PLAT_ERR_SYSTEM_ERR;
        }
    }

    return TFM_PLAT_ERR_SUCCESS;
}

static uint32_t count_otp_zero_bits(uint32_t offset, uint32_t len)
{
    uint8_t buf[len];

    otp_read(offset, len, len, buf);
    return count_buffer_zero_bits(buf, len);
}

static enum tfm_plat_err_t verify_zero_bits_count(uint32_t offset,
                                                  uint32_t len,
                                                  uint32_t zero_count_offset)
{
    enum lcm_error_t lcm_err;
    uint32_t zero_count;

    lcm_err = lcm_otp_read(&LCM_DEV_S, zero_count_offset, sizeof(zero_count),
                           (uint8_t*)&zero_count);
    if (lcm_err != LCM_ERROR_NONE) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    if (zero_count != count_otp_zero_bits(offset, len)) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    return TFM_PLAT_ERR_SUCCESS;
}

static enum tfm_plat_err_t check_keys_for_tampering(void)
{
    size_t idx;
    uint32_t zero_count;
    enum tfm_plat_err_t err;

    err = verify_zero_bits_count(USER_AREA_OFFSET(boot_seed),
                                 USER_AREA_SIZE(boot_seed),
                                 USER_AREA_OFFSET(boot_seed_zero_bits));
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    err = verify_zero_bits_count(USER_AREA_OFFSET(implementation_id),
                                 USER_AREA_SIZE(implementation_id),
                                 USER_AREA_OFFSET(implementation_id_zero_bits));
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    err = verify_zero_bits_count(USER_AREA_OFFSET(cert_ref),
                                 USER_AREA_SIZE(cert_ref),
                                 USER_AREA_OFFSET(cert_ref_zero_bits));
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    err = verify_zero_bits_count(USER_AREA_OFFSET(verification_service_url),
                                 USER_AREA_SIZE(verification_service_url),
                                 USER_AREA_OFFSET(verification_service_url_zero_bits));
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    err = verify_zero_bits_count(USER_AREA_OFFSET(profile_definition),
                                 USER_AREA_SIZE(profile_definition),
                                 USER_AREA_OFFSET(profile_definition_zero_bits));
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    /* The rotpk (used as the ROTPK for the RSS rutime) is special as it's zero
     * count is stored in the cm_config_2 field, but it's not checked so we
     * still need to do it manually
     */
    otp_read(OTP_OFFSET(cm_config_2), OTP_SIZE(cm_config_2), sizeof(zero_count),
             (uint8_t *)&zero_count);

    zero_count &= 0xff;

    if (zero_count != count_otp_zero_bits(OTP_OFFSET(rotpk), OTP_SIZE(rotpk))) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    /* First bl2 ROTPK is stored in the ROTPK slot, validate the others */
    for (idx = 0; idx < MCUBOOT_IMAGE_NUMBER - 1; idx++) {
        err = verify_zero_bits_count(USER_AREA_OFFSET(bl2_rotpk[idx]),
                                     USER_AREA_SIZE(bl2_rotpk[idx]),
                                     USER_AREA_OFFSET(bl2_rotpk_zero_bits[idx]));
        if (err != TFM_PLAT_ERR_SUCCESS) {
            return err;
        }
    }

    err = verify_zero_bits_count(USER_AREA_OFFSET(s_image_encryption_key),
                                 USER_AREA_SIZE(s_image_encryption_key),
                                 USER_AREA_OFFSET(s_image_encryption_key_zero_bits));
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }
    err = verify_zero_bits_count(USER_AREA_OFFSET(ns_image_encryption_key),
                                 USER_AREA_SIZE(ns_image_encryption_key),
                                 USER_AREA_OFFSET(ns_image_encryption_key_zero_bits));
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

#ifdef BL1
    err = verify_zero_bits_count(USER_AREA_OFFSET(bl2_encryption_key),
                                 USER_AREA_SIZE(bl2_encryption_key),
                                 USER_AREA_OFFSET(bl2_encryption_key_zero_bits));
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

#ifdef PLATFORM_PSA_ADAC_SECURE_DEBUG
    err = verify_zero_bits_count(USER_AREA_OFFSET(secure_debug_pk),
                                 USER_AREA_SIZE(secure_debug_pk),
                                 USER_AREA_OFFSET(secure_debug_pk_zero_bits));
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }
#endif

    err = verify_zero_bits_count(USER_AREA_OFFSET(bl1_2_image_hash),
                                 USER_AREA_SIZE(bl1_2_image_hash),
                                 USER_AREA_OFFSET(bl1_2_image_hash_zero_bits));
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    err = verify_zero_bits_count(USER_AREA_OFFSET(bl2_image_hash),
                                 USER_AREA_SIZE(bl2_image_hash),
                                 USER_AREA_OFFSET(bl2_image_hash_zero_bits));
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    err = verify_zero_bits_count(USER_AREA_OFFSET(bl1_rotpk_0),
                                 USER_AREA_SIZE(bl1_rotpk_0),
                                 USER_AREA_OFFSET(bl1_rotpk_0_zero_bits));
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }
#endif /* BL1 */

    err = verify_zero_bits_count(USER_AREA_OFFSET(host_rotpk_s),
                                 USER_AREA_SIZE(host_rotpk_s),
                                 USER_AREA_OFFSET(host_rotpk_s_zero_bits));
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    err = verify_zero_bits_count(USER_AREA_OFFSET(host_rotpk_ns),
                                 USER_AREA_SIZE(host_rotpk_ns),
                                 USER_AREA_OFFSET(host_rotpk_ns_zero_bits));
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    err = verify_zero_bits_count(USER_AREA_OFFSET(host_rotpk_cca),
                                 USER_AREA_SIZE(host_rotpk_cca),
                                 USER_AREA_OFFSET(host_rotpk_cca_zero_bits));
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    err = verify_zero_bits_count(USER_AREA_OFFSET(cca_system_properties),
                                 USER_AREA_SIZE(cca_system_properties),
                                 USER_AREA_OFFSET(cca_system_properties_zero_bits));
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    return TFM_PLAT_ERR_SUCCESS;
}

static enum lcm_lcs_t map_otp_lcs_to_lcm_lcs(enum plat_otp_lcs_t lcs)
{
    switch (lcs) {
    case PLAT_OTP_LCS_ASSEMBLY_AND_TEST:
        return LCM_LCS_CM;
    case PLAT_OTP_LCS_PSA_ROT_PROVISIONING:
        return LCM_LCS_DM;
    case PLAT_OTP_LCS_SECURED:
        return LCM_LCS_SE;
    case PLAT_OTP_LCS_DECOMMISSIONED:
        return LCM_LCS_RMA;
    default:
        return LCM_LCS_INVALID;
    }
}

static enum plat_otp_lcs_t map_lcm_lcs_to_otp_lcs(enum lcm_lcs_t lcs)
{
    switch (lcs) {
    case LCM_LCS_CM:
        return PLAT_OTP_LCS_ASSEMBLY_AND_TEST;
    case LCM_LCS_DM:
        return PLAT_OTP_LCS_PSA_ROT_PROVISIONING;
    case LCM_LCS_SE:
        return PLAT_OTP_LCS_SECURED;
    case LCM_LCS_RMA:
        return PLAT_OTP_LCS_DECOMMISSIONED;
    default:
        return PLAT_OTP_LCS_UNKNOWN;
    }
}

static enum tfm_plat_err_t otp_read_lcs(size_t out_len, uint8_t *out)
{
    enum lcm_lcs_t lcm_lcs;
    enum plat_otp_lcs_t *lcs = (enum plat_otp_lcs_t*) out;

    if (lcm_get_lcs(&LCM_DEV_S, &lcm_lcs)) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    if (out_len != sizeof(uint32_t)) {
        return TFM_PLAT_ERR_INVALID_INPUT;
    }

    *lcs = map_lcm_lcs_to_otp_lcs(lcm_lcs);

    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t tfm_plat_otp_init(void)
{
    uint32_t otp_size;
    enum lcm_error_t err;
    enum lcm_lcs_t lcs;
    enum tfm_plat_err_t plat_err;
    enum lcm_bool_t sp_enabled;
    enum lcm_tp_mode_t tp_mode;

    err = lcm_get_otp_size(&LCM_DEV_S, &otp_size);
    if (err != LCM_ERROR_NONE) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    if (otp_size < OTP_OFFSET(user_data) +
                   sizeof(struct plat_user_area_layout_t)) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    err = lcm_get_tp_mode(&LCM_DEV_S, &tp_mode);
    if (err != LCM_ERROR_NONE) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }
    if (tp_mode == LCM_TP_MODE_VIRGIN) {
        err = lcm_set_tp_mode(&LCM_DEV_S, LCM_TP_MODE_TCI);
        if (err != LCM_ERROR_NONE) {
            return TFM_PLAT_ERR_SYSTEM_ERR;
        }
        LOG("TP mode set complete, reset now.\r\n");
        tfm_hal_system_reset();
    } else if (!(tp_mode == LCM_TP_MODE_TCI || tp_mode == LCM_TP_MODE_PCI)) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    err = lcm_get_lcs(&LCM_DEV_S, &lcs);
    if (err != LCM_ERROR_NONE) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }
    if (lcs == LCM_LCS_CM || lcs == LCM_LCS_DM) {
        err = lcm_get_sp_enabled(&LCM_DEV_S, &sp_enabled);
        if (err != LCM_ERROR_NONE) {
            return TFM_PLAT_ERR_SYSTEM_ERR;
        }

        if (sp_enabled != LCM_TRUE) {
            LOG("Enabling secure provisioning mode\r\n");
            lcm_set_sp_enabled(&LCM_DEV_S);
        }
    } else if (lcs == LCM_LCS_SE) {
        /* If we are in SE LCS, check keys for tampering. Only applies to keys
         * in the user storage area, since the others are checked for tampering
         * by HW.
         */
        plat_err = check_keys_for_tampering();
        if (plat_err != TFM_PLAT_ERR_SUCCESS) {
            return plat_err;
        }
    }

    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t tfm_plat_otp_read(enum tfm_otp_element_id_t id,
                                      size_t out_len, uint8_t *out)
{
    switch (id) {
    case PLAT_OTP_ID_HUK:
        return otp_read(OTP_OFFSET(huk), OTP_SIZE(huk), out_len, out);
    case PLAT_OTP_ID_GUK:
        return otp_read(OTP_OFFSET(guk), OTP_SIZE(guk), out_len, out);

    case PLAT_OTP_ID_BOOT_SEED:
        return otp_read(USER_AREA_OFFSET(boot_seed), USER_AREA_SIZE(boot_seed),
                        out_len, out);
    case PLAT_OTP_ID_LCS:
        return otp_read_lcs(out_len, out);
    case PLAT_OTP_ID_IMPLEMENTATION_ID:
        return otp_read(USER_AREA_OFFSET(implementation_id),
                        USER_AREA_SIZE(implementation_id), out_len, out);
    case PLAT_OTP_ID_CERT_REF:
        return otp_read(USER_AREA_OFFSET(cert_ref),
                        USER_AREA_SIZE(cert_ref), out_len, out);
    case PLAT_OTP_ID_VERIFICATION_SERVICE_URL:
        return otp_read(USER_AREA_OFFSET(verification_service_url),
                        USER_AREA_SIZE(verification_service_url), out_len,
                        out);
    case PLAT_OTP_ID_PROFILE_DEFINITION:
        return otp_read(USER_AREA_OFFSET(profile_definition),
                        USER_AREA_SIZE(profile_definition), out_len, out);

    case PLAT_OTP_ID_BL2_ROTPK_0:
        return otp_read(OTP_OFFSET(rotpk), OTP_SIZE(rotpk), out_len, out);
    case PLAT_OTP_ID_NV_COUNTER_BL2_0:
        return otp_read(USER_AREA_OFFSET(bl2_nv_counter[0]),
                        USER_AREA_SIZE(bl2_nv_counter[0]), out_len, out);

    case PLAT_OTP_ID_BL2_ROTPK_1:
        return otp_read(USER_AREA_OFFSET(bl2_rotpk[0]),
                        USER_AREA_SIZE(bl2_rotpk[0]), out_len, out);
    case PLAT_OTP_ID_NV_COUNTER_BL2_1:
        return otp_read(USER_AREA_OFFSET(bl2_nv_counter[1]),
                        USER_AREA_SIZE(bl2_nv_counter[1]), out_len, out);

    case PLAT_OTP_ID_BL2_ROTPK_2:
        return otp_read(USER_AREA_OFFSET(bl2_rotpk[1]),
                        USER_AREA_SIZE(bl2_rotpk[1]), out_len, out);
    case PLAT_OTP_ID_NV_COUNTER_BL2_2:
        return otp_read(USER_AREA_OFFSET(bl2_nv_counter[2]),
                        USER_AREA_SIZE(bl2_nv_counter[2]), out_len, out);

    case PLAT_OTP_ID_BL2_ROTPK_3:
        return otp_read(USER_AREA_OFFSET(bl2_rotpk[2]),
                        USER_AREA_SIZE(bl2_rotpk[2]), out_len, out);
    case PLAT_OTP_ID_NV_COUNTER_BL2_3:
        return otp_read(USER_AREA_OFFSET(bl2_nv_counter[3]),
                        USER_AREA_SIZE(bl2_nv_counter[3]), out_len, out);

    case PLAT_OTP_ID_NV_COUNTER_NS_0:
        return otp_read(USER_AREA_OFFSET(host_nv_counter[0]),
                        USER_AREA_SIZE(host_nv_counter[0]), out_len, out);
    case PLAT_OTP_ID_NV_COUNTER_NS_1:
        return otp_read(USER_AREA_OFFSET(host_nv_counter[1]),
                        USER_AREA_SIZE(host_nv_counter[1]), out_len, out);
    case PLAT_OTP_ID_NV_COUNTER_NS_2:
        return otp_read(USER_AREA_OFFSET(host_nv_counter[2]),
                        USER_AREA_SIZE(host_nv_counter[2]), out_len, out);

    case PLAT_OTP_ID_KEY_SECURE_ENCRYPTION:
        return otp_read(USER_AREA_OFFSET(s_image_encryption_key),
                        USER_AREA_SIZE(s_image_encryption_key), out_len, out);
    case PLAT_OTP_ID_KEY_NON_SECURE_ENCRYPTION:
        return otp_read(USER_AREA_OFFSET(ns_image_encryption_key),
                        USER_AREA_SIZE(ns_image_encryption_key), out_len, out);
#ifdef BL1
    case PLAT_OTP_ID_KEY_BL2_ENCRYPTION:
        return otp_read(USER_AREA_OFFSET(bl2_encryption_key),
                        USER_AREA_SIZE(bl2_encryption_key), out_len, out);
    case PLAT_OTP_ID_BL1_2_IMAGE_HASH:
        return otp_read(USER_AREA_OFFSET(bl1_2_image_hash),
                        USER_AREA_SIZE(bl1_2_image_hash), out_len, out);
    case PLAT_OTP_ID_BL2_IMAGE_HASH:
        return otp_read(USER_AREA_OFFSET(bl2_image_hash),
                        USER_AREA_SIZE(bl2_image_hash), out_len, out);
    case PLAT_OTP_ID_NV_COUNTER_BL1_0:
        return otp_read(USER_AREA_OFFSET(bl1_nv_counter),
                        USER_AREA_SIZE(bl1_nv_counter), out_len, out);
    case PLAT_OTP_ID_BL1_ROTPK_0:
        return otp_read(USER_AREA_OFFSET(bl1_rotpk_0),
                        USER_AREA_SIZE(bl1_rotpk_0), out_len, out);
    case PLAT_OTP_ID_BL1_2_IMAGE:
        return otp_read(USER_AREA_OFFSET(bl1_2_image),
                        USER_AREA_SIZE(bl1_2_image), out_len, out);
#endif /* BL1 */

    case PLAT_OTP_ID_ENTROPY_SEED:
        return TFM_PLAT_ERR_UNSUPPORTED;

    case PLAT_OTP_ID_SECURE_DEBUG_PK:
        return otp_read(USER_AREA_OFFSET(secure_debug_pk),
                        USER_AREA_SIZE(secure_debug_pk), out_len, out);

    case PLAT_OTP_ID_HOST_ROTPK_S:
        return otp_read(USER_AREA_OFFSET(host_rotpk_s),
                        USER_AREA_SIZE(host_rotpk_s), out_len, out);
    case PLAT_OTP_ID_HOST_ROTPK_NS:
        return otp_read(USER_AREA_OFFSET(host_rotpk_ns),
                        USER_AREA_SIZE(host_rotpk_ns), out_len, out);
    case PLAT_OTP_ID_HOST_ROTPK_CCA:
        return otp_read(USER_AREA_OFFSET(host_rotpk_cca),
                        USER_AREA_SIZE(host_rotpk_cca), out_len, out);

    case PLAT_OTP_ID_CCA_SYSTEM_PROPERTIES:
        return otp_read(USER_AREA_OFFSET(cca_system_properties),
                        USER_AREA_SIZE(cca_system_properties), out_len, out);

    default:
        return TFM_PLAT_ERR_UNSUPPORTED;
    }
}

static enum tfm_plat_err_t otp_write_lcs(size_t in_len, const uint8_t *in)
{
    uint32_t lcs;
    enum plat_otp_lcs_t new_lcs = *(uint32_t*)in;
    enum lcm_error_t lcm_err;
    uint16_t gppc_val = 0;

    if (in_len != sizeof(lcs)) {
        return TFM_PLAT_ERR_INVALID_INPUT;
    }

    lcm_err = lcm_set_lcs(&LCM_DEV_S, map_otp_lcs_to_lcm_lcs(new_lcs), gppc_val);
    if (lcm_err != LCM_ERROR_NONE) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    LOG("LCS transition complete, resetting now.\r\n");

    tfm_hal_system_reset();

    /* This should never happen */
    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t tfm_plat_otp_write(enum tfm_otp_element_id_t id,
                                       size_t in_len, const uint8_t *in)
{
    switch (id) {
    case PLAT_OTP_ID_HUK:
        return otp_write(OTP_OFFSET(huk), OTP_SIZE(huk), in_len, in,
                         0);
    case PLAT_OTP_ID_GUK:
        return otp_write(OTP_OFFSET(guk), OTP_SIZE(guk), in_len, in,
                         0);

    case PLAT_OTP_ID_BOOT_SEED:
        return otp_write(USER_AREA_OFFSET(boot_seed), USER_AREA_SIZE(boot_seed),
                         in_len, in, USER_AREA_OFFSET(boot_seed_zero_bits));
    case PLAT_OTP_ID_LCS:
        return otp_write_lcs(in_len, in);
    case PLAT_OTP_ID_IMPLEMENTATION_ID:
        return otp_write(USER_AREA_OFFSET(implementation_id),
                         USER_AREA_SIZE(implementation_id), in_len, in,
                         USER_AREA_OFFSET(implementation_id_zero_bits));
    case PLAT_OTP_ID_CERT_REF:
        return otp_write(USER_AREA_OFFSET(cert_ref),
                         USER_AREA_SIZE(cert_ref), in_len, in,
                         USER_AREA_OFFSET(cert_ref_zero_bits));
    case PLAT_OTP_ID_VERIFICATION_SERVICE_URL:
        return otp_write(USER_AREA_OFFSET(verification_service_url),
                         USER_AREA_SIZE(verification_service_url), in_len, in,
                         USER_AREA_OFFSET(verification_service_url_zero_bits));
    case PLAT_OTP_ID_PROFILE_DEFINITION:
        return otp_write(USER_AREA_OFFSET(profile_definition),
                         USER_AREA_SIZE(profile_definition), in_len,
                         in, USER_AREA_OFFSET(profile_definition_zero_bits));

    case PLAT_OTP_ID_BL2_ROTPK_0:
        return otp_write(OTP_OFFSET(rotpk), OTP_SIZE(rotpk), in_len, in, 0);
    case PLAT_OTP_ID_NV_COUNTER_BL2_0:
        return otp_write(USER_AREA_OFFSET(bl2_nv_counter[0]),
                         USER_AREA_SIZE(bl2_nv_counter[0]), in_len, in, 0);

    case PLAT_OTP_ID_BL2_ROTPK_1:
        return otp_write(USER_AREA_OFFSET(bl2_rotpk[0]),
                         USER_AREA_SIZE(bl2_rotpk[0]), in_len, in,
                         USER_AREA_OFFSET(bl2_rotpk_zero_bits[0]));
    case PLAT_OTP_ID_NV_COUNTER_BL2_1:
        return otp_write(USER_AREA_OFFSET(bl2_nv_counter[1]),
                         USER_AREA_SIZE(bl2_nv_counter[1]), in_len, in, 0);

    case PLAT_OTP_ID_BL2_ROTPK_2:
        return otp_write(USER_AREA_OFFSET(bl2_rotpk[1]),
                         USER_AREA_SIZE(bl2_rotpk[1]), in_len, in,
                         USER_AREA_OFFSET(bl2_rotpk_zero_bits[1]));
    case PLAT_OTP_ID_NV_COUNTER_BL2_2:
        return otp_write(USER_AREA_OFFSET(bl2_nv_counter[2]),
                         USER_AREA_SIZE(bl2_nv_counter[2]), in_len, in, 0);

    case PLAT_OTP_ID_BL2_ROTPK_3:
        return otp_write(USER_AREA_OFFSET(bl2_rotpk[2]),
                         USER_AREA_SIZE(bl2_rotpk[2]), in_len, in,
                         USER_AREA_OFFSET(bl2_rotpk_zero_bits[2]));
    case PLAT_OTP_ID_NV_COUNTER_BL2_3:
        return otp_write(USER_AREA_OFFSET(bl2_nv_counter[3]),
                         USER_AREA_SIZE(bl2_nv_counter[3]), in_len, in, 0);

    case PLAT_OTP_ID_NV_COUNTER_NS_0:
        return otp_write(USER_AREA_OFFSET(host_nv_counter[0]),
                         USER_AREA_SIZE(host_nv_counter[0]), in_len, in, 0);
    case PLAT_OTP_ID_NV_COUNTER_NS_1:
        return otp_write(USER_AREA_OFFSET(host_nv_counter[1]),
                         USER_AREA_SIZE(host_nv_counter[1]), in_len, in, 0);
    case PLAT_OTP_ID_NV_COUNTER_NS_2:
        return otp_write(USER_AREA_OFFSET(host_nv_counter[2]),
                         USER_AREA_SIZE(host_nv_counter[2]), in_len, in, 0);

    case PLAT_OTP_ID_KEY_SECURE_ENCRYPTION:
        return otp_write(USER_AREA_OFFSET(s_image_encryption_key),
                         USER_AREA_SIZE(s_image_encryption_key), in_len, in,
                         USER_AREA_OFFSET(s_image_encryption_key_zero_bits));
    case PLAT_OTP_ID_KEY_NON_SECURE_ENCRYPTION:
        return otp_write(USER_AREA_OFFSET(ns_image_encryption_key),
                         USER_AREA_SIZE(ns_image_encryption_key), in_len, in,
                         USER_AREA_OFFSET(ns_image_encryption_key_zero_bits));
#ifdef BL1
    case PLAT_OTP_ID_KEY_BL2_ENCRYPTION:
        return otp_write(USER_AREA_OFFSET(bl2_encryption_key),
                         USER_AREA_SIZE(bl2_encryption_key), in_len, in,
                         USER_AREA_OFFSET(bl2_encryption_key_zero_bits));
    case PLAT_OTP_ID_BL1_2_IMAGE_HASH:
        return otp_write(USER_AREA_OFFSET(bl1_2_image_hash),
                         USER_AREA_SIZE(bl1_2_image_hash), in_len, in,
                         USER_AREA_OFFSET(bl1_2_image_hash_zero_bits));
    case PLAT_OTP_ID_BL2_IMAGE_HASH:
        return otp_write(USER_AREA_OFFSET(bl2_image_hash),
                         USER_AREA_SIZE(bl2_image_hash), in_len, in,
                         USER_AREA_OFFSET(bl2_image_hash_zero_bits));
    case PLAT_OTP_ID_NV_COUNTER_BL1_0:
        return otp_write(USER_AREA_OFFSET(bl1_nv_counter),
                         USER_AREA_SIZE(bl1_nv_counter), in_len, in, 0);
    case PLAT_OTP_ID_BL1_ROTPK_0:
        return otp_write(USER_AREA_OFFSET(bl1_rotpk_0),
                         USER_AREA_SIZE(bl1_rotpk_0), in_len, in,
                         USER_AREA_OFFSET(bl1_rotpk_0_zero_bits));
    case PLAT_OTP_ID_BL1_2_IMAGE:
        return otp_write(USER_AREA_OFFSET(bl1_2_image),
                         USER_AREA_SIZE(bl1_2_image), in_len, in, 0);
#endif /* BL1 */

    case PLAT_OTP_ID_ENTROPY_SEED:
        return TFM_PLAT_ERR_UNSUPPORTED;

    case PLAT_OTP_ID_SECURE_DEBUG_PK:
        return otp_write(USER_AREA_OFFSET(secure_debug_pk),
                         USER_AREA_SIZE(secure_debug_pk), in_len, in,
                         USER_AREA_OFFSET(secure_debug_pk_zero_bits));

    case PLAT_OTP_ID_HOST_ROTPK_S:
        return otp_write(USER_AREA_OFFSET(host_rotpk_s),
                         USER_AREA_SIZE(host_rotpk_s), in_len, in,
                         USER_AREA_OFFSET(host_rotpk_s_zero_bits));
    case PLAT_OTP_ID_HOST_ROTPK_NS:
        return otp_write(USER_AREA_OFFSET(host_rotpk_ns),
                         USER_AREA_SIZE(host_rotpk_ns), in_len, in,
                         USER_AREA_OFFSET(host_rotpk_ns_zero_bits));
    case PLAT_OTP_ID_HOST_ROTPK_CCA:
        return otp_write(USER_AREA_OFFSET(host_rotpk_cca),
                         USER_AREA_SIZE(host_rotpk_cca), in_len, in,
                         USER_AREA_OFFSET(host_rotpk_cca_zero_bits));

    case PLAT_OTP_ID_CCA_SYSTEM_PROPERTIES:
        return otp_write(USER_AREA_OFFSET(cca_system_properties),
                         USER_AREA_SIZE(cca_system_properties), in_len, in,
                         USER_AREA_OFFSET(cca_system_properties_zero_bits));

    default:
        return TFM_PLAT_ERR_UNSUPPORTED;
    }
}


enum tfm_plat_err_t tfm_plat_otp_get_size(enum tfm_otp_element_id_t id,
                                          size_t *size)
{
    switch (id) {
    case PLAT_OTP_ID_HUK:
        *size = OTP_SIZE(huk);
        break;
    case PLAT_OTP_ID_GUK:
        *size = OTP_SIZE(guk);
        break;

    case PLAT_OTP_ID_BOOT_SEED:
        *size = USER_AREA_SIZE(boot_seed);
        break;
    case PLAT_OTP_ID_LCS:
        *size = sizeof(uint32_t);
        break;
    case PLAT_OTP_ID_IMPLEMENTATION_ID:
        *size = USER_AREA_SIZE(implementation_id);
        break;
    case PLAT_OTP_ID_CERT_REF:
        *size = USER_AREA_SIZE(cert_ref);
        break;
    case PLAT_OTP_ID_VERIFICATION_SERVICE_URL:
        *size = USER_AREA_SIZE(verification_service_url);
        break;
    case PLAT_OTP_ID_PROFILE_DEFINITION:
        *size = USER_AREA_SIZE(profile_definition);
        break;

    case PLAT_OTP_ID_BL2_ROTPK_0:
        *size = OTP_SIZE(rotpk);
        break;
    case PLAT_OTP_ID_NV_COUNTER_BL2_0:
        *size = USER_AREA_SIZE(bl2_nv_counter[0]);
        break;

    case PLAT_OTP_ID_BL2_ROTPK_1:
        *size = USER_AREA_SIZE(bl2_rotpk[0]);
        break;
    case PLAT_OTP_ID_NV_COUNTER_BL2_1:
        *size = USER_AREA_SIZE(bl2_nv_counter[1]);
        break;

    case PLAT_OTP_ID_BL2_ROTPK_2:
        *size = USER_AREA_SIZE(bl2_rotpk[1]);
        break;
    case PLAT_OTP_ID_NV_COUNTER_BL2_2:
        *size = USER_AREA_SIZE(bl2_nv_counter[2]);
        break;

    case PLAT_OTP_ID_BL2_ROTPK_3:
        *size = USER_AREA_SIZE(bl2_rotpk[2]);
        break;
    case PLAT_OTP_ID_NV_COUNTER_BL2_3:
        *size = USER_AREA_SIZE(bl2_nv_counter[3]);
        break;

    case PLAT_OTP_ID_NV_COUNTER_NS_0:
        *size = USER_AREA_SIZE(host_nv_counter[0]);
        break;
    case PLAT_OTP_ID_NV_COUNTER_NS_1:
        *size = USER_AREA_SIZE(host_nv_counter[1]);
        break;
    case PLAT_OTP_ID_NV_COUNTER_NS_2:
        *size = USER_AREA_SIZE(host_nv_counter[2]);
        break;

    case PLAT_OTP_ID_KEY_SECURE_ENCRYPTION:
        *size = USER_AREA_SIZE(s_image_encryption_key);
        break;
    case PLAT_OTP_ID_KEY_NON_SECURE_ENCRYPTION:
        *size = USER_AREA_SIZE(ns_image_encryption_key);
        break;
#ifdef BL1
    case PLAT_OTP_ID_KEY_BL2_ENCRYPTION:
        *size = USER_AREA_SIZE(bl2_encryption_key);
        break;
    case PLAT_OTP_ID_BL1_2_IMAGE_HASH:
        *size = USER_AREA_SIZE(bl1_2_image_hash);
        break;
    case PLAT_OTP_ID_BL2_IMAGE_HASH:
        *size = USER_AREA_SIZE(bl2_image_hash);
        break;
    case PLAT_OTP_ID_NV_COUNTER_BL1_0:
        *size = USER_AREA_SIZE(bl1_nv_counter);
        break;
    case PLAT_OTP_ID_BL1_ROTPK_0:
        *size = USER_AREA_SIZE(bl1_rotpk_0);
        break;
    case PLAT_OTP_ID_BL1_2_IMAGE:
        *size = USER_AREA_SIZE(bl1_2_image);
        break;
#endif

    case PLAT_OTP_ID_ENTROPY_SEED:
        return TFM_PLAT_ERR_UNSUPPORTED;

    case PLAT_OTP_ID_SECURE_DEBUG_PK:
        *size = USER_AREA_SIZE(secure_debug_pk);
        break;

    case PLAT_OTP_ID_HOST_ROTPK_S:
        *size = USER_AREA_SIZE(host_rotpk_s);
        break;
    case PLAT_OTP_ID_HOST_ROTPK_NS:
        *size = USER_AREA_SIZE(host_rotpk_ns);
        break;
    case PLAT_OTP_ID_HOST_ROTPK_CCA:
        *size = USER_AREA_SIZE(host_rotpk_cca);
        break;

    case PLAT_OTP_ID_CCA_SYSTEM_PROPERTIES:
        *size = USER_AREA_SIZE(cca_system_properties);
        break;

    default:
        return TFM_PLAT_ERR_UNSUPPORTED;
    }

    return TFM_PLAT_ERR_SUCCESS;
}
