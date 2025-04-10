/*
 * Copyright (c) 2018-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include <string.h>
#include "array.h"
#include "tfm_boot_status.h"
#include "region_defs.h"
#include "tfm_api.h"
#include "psa_manifest/pid.h"
#include "internal_status_code.h"
#include "utilities.h"
#include "psa/service.h"
#include "thread.h"
#include "spm.h"
#include "load/partition_defs.h"
#include "tfm_hal_isolation.h"
#include "ameba_ipsec.h"
#include "ameba_syscfg.h"
#include "ameba_secure_boot.h"
#include "psa/crypto.h"
#include "tfm_boot_data.h"
#include "core_cache.h"
#include "ameba_cache.h"
#include "ameba_crypto_api.h"

/*!
 * \def BOOT_DATA_VALID
 *
 * \brief Indicates that shared data between bootloader and runtime firmware was
 *        passed the sanity check with success.
 */
#define BOOT_DATA_VALID (1u)

/*!
 * \def BOOT_DATA_INVALID
 *
 * \brief Indicates that shared data between bootloader and runtime firmware was
 *        failed on sanity check.
 */
#define BOOT_DATA_INVALID (0u)

/*!
 * \var is_boot_data_valid
 *
 * \brief Indicates the status of shared data between bootloader and runtime
 *        firmware
 */
static uint32_t is_boot_data_valid = BOOT_DATA_INVALID;

/*!
 * \struct boot_data_access_policy
 *
 * \brief Defines the access policy of secure partitions to data items in shared
 *        data area (between bootloader and runtime firmware).
 */
struct boot_data_access_policy {
    int32_t partition_id;
    uint32_t major_type;
};

/*!
 * \var access_policy_table
 *
 * \brief Contains the partition_id and major_type assignments. This describes
 *        which secure partition is allowed to access which data item
 *        (identified by major_type).
 */
static const struct boot_data_access_policy access_policy_table[] = {
    /*
     * IAR won't accept zero element array definition, so an invalid element
     * is always defined here.
     */
    {INVALID_PARTITION_ID, TLV_MAJOR_INVALID},
#ifdef TFM_PARTITION_INITIAL_ATTESTATION
    {TFM_SP_INITIAL_ATTESTATION, TLV_MAJOR_IAS},
#endif
#ifdef TFM_PARTITION_FIRMWARE_UPDATE
    {TFM_SP_FWU, TLV_MAJOR_FWU},
#endif
#ifdef TFM_PARTITION_MEASURED_BOOT
    {TFM_SP_MEASURED_BOOT, TLV_MAJOR_MBS},
#endif
};

/*!
 * \brief Verify the access right of the active secure partition to the
 *        specified data type in the shared data area.
 *
 * \param[in]  major_type  Data type identifier.
 *
 * \return  Returns 0 in case of success, otherwise -1.
 */
static int32_t tfm_core_check_boot_data_access_policy(uint8_t major_type)
{
    int32_t partition_id;
    uint32_t i;
    int32_t rc = -1;
    const uint32_t array_size = ARRAY_SIZE(access_policy_table);

    partition_id = tfm_spm_partition_get_running_partition_id();

    /*
     * The first element of the access_policy_table is an invalid element,
     * which isn't need to be checked, that's why the iteration
     * starts from i=1.
     */
    for (i = 1; i < array_size; ++i) {
        if (partition_id == access_policy_table[i].partition_id) {
            if (major_type == access_policy_table[i].major_type) {
                rc = 0;
                break;
            }
        }
    }

    return rc;
}

/* Compile time check to verify that shared data region is not overlapping with
 * non-secure data area.
 */
#if ((BOOT_TFM_SHARED_DATA_BASE  >= NS_DATA_START && \
      BOOT_TFM_SHARED_DATA_BASE  <= NS_DATA_LIMIT) || \
     (BOOT_TFM_SHARED_DATA_LIMIT >= NS_DATA_START && \
      BOOT_TFM_SHARED_DATA_LIMIT <= NS_DATA_LIMIT))
#error "Shared data area and non-secure data area is overlapping"
#endif

void tfm_core_validate_boot_data(void)
{
#ifdef BOOT_DATA_AVAILABLE
    struct tfm_boot_data *boot_data;

    boot_data = (struct tfm_boot_data *)BOOT_TFM_SHARED_DATA_BASE;

    if (boot_data->header.tlv_magic == SHARED_DATA_TLV_INFO_MAGIC) {
        is_boot_data_valid = BOOT_DATA_VALID;
    }
#else
    is_boot_data_valid = BOOT_DATA_VALID;
#endif /* BOOT_DATA_AVAILABLE */
}

uint32_t tfm_core_read_measurement(uint32_t *measurement_algo,
                                              uint8_t *signer_id,
                                              size_t signer_id_size,
                                              size_t *signer_id_len,
                                              uint8_t *version,
                                              size_t version_size,
                                              size_t *version_len)
{
    /*Already checked in tfm_core_get_measurement_value */
    Manifest_TypeDef *Manifest = (Manifest_TypeDef *)tfm_core_get_Manifest_Addr();

    u8 HashAlg = SYSCFG_OTP_SBootEn() ? Manifest->HashAlg : HASHID_SHA256;
    switch (HashAlg) {
    case HASHID_SHA256:
        *measurement_algo = PSA_ALG_SHA_256;
        break;
    case HASHID_SHA384:
        *measurement_algo = PSA_ALG_SHA_384;
        break;
    case HASHID_SHA512:
        *measurement_algo = PSA_ALG_SHA_512;
        break;
    case HASHID_HMAC256:
        *measurement_algo = PSA_ALG_HMAC(PSA_ALG_SHA_256);
        break;
    case HASHID_HMAC384:
        *measurement_algo = PSA_ALG_HMAC(PSA_ALG_SHA_384);
        break;
    case HASHID_HMAC512:
        *measurement_algo = PSA_ALG_HMAC(PSA_ALG_SHA_512);
        break;
    default:
        return TFM_ERROR_INVALID_PARAMETER;
    }

    u32 Img_Version = (Manifest->MajorImgVer << 16) | Manifest->MinorImgVer; // get 32-bit full version number;

    if(signer_id_size < sizeof(Img_Version)) {
        return TFM_ERROR_INVALID_PARAMETER;
    }
    (void)spm_memcpy(version, (const void *)&Img_Version, sizeof(Img_Version));
    *version_len = sizeof(Img_Version);

    ALIGNMTO(CACHE_LINE_SIZE) u8 CalcHash[CACHE_LINE_ALIGMENT(32)]; //Fixed for CRYPTO_ProcessAD CacheLine Assert
    u8 AuthAlg = SYSCFG_OTP_SBootEn() ? Manifest->AuthAlg : AUTHID_EDDSA_25519;
    u32 PubKeySize = 0;
    if (IS_EDDSA(AuthAlg)) {
        PubKeySize = 32;
    } else {
        return TFM_ERROR_INVALID_PARAMETER;
    }

    CRYPTO_SHA_Init(NULL);
    if (rtl_crypto_sha2(SHA2_256, Manifest->SBPubKey, PubKeySize, CalcHash)) {
        return TFM_ERROR_INVALID_PARAMETER;
    }

    if (signer_id_size < SHA2_256) {
        return TFM_ERROR_INVALID_PARAMETER;
    }
    (void)spm_memcpy(signer_id, (const void *)CalcHash, SHA2_256);
    *signer_id_len = SHA2_256;

    return (uint32_t)TFM_SUCCESS;
}

static inline uint32_t tfm_core_get_measurement_value(uint32_t *HashLen, uint32_t *ImgHash)
{
    struct partition_t *curr_partition = GET_CURRENT_COMPONENT();
    fih_int fih_rc = FIH_FAILURE;
    Manifest_TypeDef *Manifest = (Manifest_TypeDef *)tfm_core_get_Manifest_Addr();

    FIH_CALL(tfm_hal_memory_check, fih_rc,
             curr_partition->boundary, (uintptr_t)Manifest,
             sizeof(Manifest_TypeDef), TFM_HAL_ACCESS_READWRITE);
    if (fih_not_eq(fih_rc, fih_int_encode(PSA_SUCCESS))) {
        return TFM_ERROR_INVALID_PARAMETER;
    }

    u8 IsHmac, HashAlg = SYSCFG_OTP_SBootEn() ? Manifest->HashAlg : HASHID_SHA256;
    *HashLen = 0;
    fih_rc = SBOOT_GetMdType(HashAlg, (SHA2_TYPE *)HashLen, &IsHmac);
    if (fih_not_eq(fih_rc, fih_int_encode(PSA_SUCCESS))) {
        return TFM_ERROR_INVALID_PARAMETER;
    }

    *ImgHash = (uint32_t)Manifest->ImgHash;
    return (uint32_t)TFM_SUCCESS;
}

void tfm_core_get_boot_data_handler(uint32_t args[])
{
    uint8_t  tlv_major = (uint8_t)args[0];
    uint8_t *buf_start = (uint8_t *)args[1];
    uint16_t buf_size  = (uint16_t)args[2];
    struct tfm_boot_data *boot_data;
#ifdef BOOT_DATA_AVAILABLE
    uint8_t *ptr;
    struct shared_data_tlv_entry tlv_entry;
    uintptr_t tlv_end, offset;
    size_t next_tlv_offset;
#endif /* BOOT_DATA_AVAILABLE */
    struct partition_t *curr_partition = GET_CURRENT_COMPONENT();
    fih_int fih_rc = FIH_FAILURE;

    FIH_CALL(tfm_hal_memory_check, fih_rc,
             curr_partition->boundary, (uintptr_t)buf_start,
             buf_size, TFM_HAL_ACCESS_READWRITE);
    if (fih_not_eq(fih_rc, fih_int_encode(PSA_SUCCESS))) {
        args[0] = (uint32_t)TFM_ERROR_INVALID_PARAMETER;
        return;
    }

    if (is_boot_data_valid != BOOT_DATA_VALID) {
        args[0] = (uint32_t)TFM_ERROR_INVALID_PARAMETER;
        return;
    }

    /* Check whether caller has access right to given tlv_major_type */
    if (tfm_core_check_boot_data_access_policy(tlv_major)) {
        args[0] = (uint32_t)TFM_ERROR_INVALID_PARAMETER;
        return;
    }

#ifdef BOOT_DATA_AVAILABLE
    /* Get the boundaries of TLV section */
    boot_data = (struct tfm_boot_data *)BOOT_TFM_SHARED_DATA_BASE;
    tlv_end = BOOT_TFM_SHARED_DATA_BASE + boot_data->header.tlv_tot_len;
    offset  = BOOT_TFM_SHARED_DATA_BASE + SHARED_DATA_HEADER_SIZE;
#endif /* BOOT_DATA_AVAILABLE */

    /* Add header to output buffer as well */
    if (buf_size < SHARED_DATA_HEADER_SIZE) {
        args[0] = (uint32_t)TFM_ERROR_INVALID_PARAMETER;
        return;
    } else {
        boot_data = (struct tfm_boot_data *)buf_start;
        boot_data->header.tlv_magic   = SHARED_DATA_TLV_INFO_MAGIC;
        boot_data->header.tlv_tot_len = SHARED_DATA_HEADER_SIZE;
    }

#ifdef BOOT_DATA_AVAILABLE
    ptr = boot_data->data;
    /* Iterates over the TLV section and copy TLVs with requested major
     * type to the provided buffer.
     */
    for (; offset < tlv_end; offset += next_tlv_offset) {
        /* Create local copy to avoid unaligned access */
        (void)spm_memcpy(&tlv_entry, (const void *)offset,
                         SHARED_DATA_ENTRY_HEADER_SIZE);

        next_tlv_offset = SHARED_DATA_ENTRY_HEADER_SIZE + tlv_entry.tlv_len;

        if (GET_MAJOR(tlv_entry.tlv_type) == tlv_major) {
            /* Check buffer overflow */
            if (((ptr - buf_start) + next_tlv_offset) > buf_size) {
                args[0] = (uint32_t)TFM_ERROR_INVALID_PARAMETER;
                return;
            }

            (void)spm_memcpy(ptr, (const void *)offset, next_tlv_offset);
            ptr += next_tlv_offset;
            boot_data->header.tlv_tot_len += next_tlv_offset;
        }
    }
#endif /* BOOT_DATA_AVAILABLE */

#ifdef BOOT_DATA_AVAILABLE
#error BOOT_DATA_AVAILABLE Not defined by AMEBADPLUS
#endif
    uint8_t *ptr;
    struct shared_data_tlv_entry tlv_entry;
    size_t next_tlv_offset;

    uint32_t HashLen, ImgHash;
    if (tfm_core_get_measurement_value(&HashLen, &ImgHash)) {
        args[0] = (uint32_t)TFM_ERROR_INVALID_PARAMETER;
        return;
    }

    ptr = boot_data->data;
    tlv_entry.tlv_type = SET_TLV_TYPE(TLV_MAJOR_IAS, SET_IAS_MINOR(SW_GENERAL, SW_BOOT_RECORD));
    tlv_entry.tlv_len = HashLen;
    next_tlv_offset = SHARED_DATA_ENTRY_HEADER_SIZE + tlv_entry.tlv_len;

    if (GET_MAJOR(tlv_entry.tlv_type) == tlv_major) {
        /* Check buffer overflow */
        if (((ptr - buf_start) + next_tlv_offset) > buf_size) {
            args[0] = (uint32_t)TFM_ERROR_INVALID_PARAMETER;
            return;
        }

        (void)spm_memcpy(ptr, (const void *)&tlv_entry, SHARED_DATA_ENTRY_HEADER_SIZE);
        ptr += SHARED_DATA_ENTRY_HEADER_SIZE;
        (void)spm_memcpy(ptr, (const void *)ImgHash, tlv_entry.tlv_len);
        ptr += tlv_entry.tlv_len;
        boot_data->header.tlv_tot_len += next_tlv_offset;
    }

    args[0] = (uint32_t)TFM_SUCCESS;
    return;
}
