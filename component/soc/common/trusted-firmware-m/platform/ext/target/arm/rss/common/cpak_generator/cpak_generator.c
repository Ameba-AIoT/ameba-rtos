/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "psa/crypto.h"
#include "mbedtls/aes.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

int load_bl1_2_hash(uint8_t *bl1_2_hash)
{
    FILE *bl1_2_hash_file;
    size_t read_bytes;

    bl1_2_hash_file = fopen(BL1_2_HASH_PATH, "rb");
    if (bl1_2_hash_file == NULL) {
        return 1;
    }

    read_bytes = fread(bl1_2_hash, 1, 32, bl1_2_hash_file);
    if (read_bytes != 32) {
        return 1;
    }

    fclose(bl1_2_hash_file);

    return 0;
}

int load_guk(uint8_t *guk)
{
    FILE *guk_file;
    size_t read_bytes;

    guk_file = fopen(GUK_PATH, "rb");
    if (guk_file == NULL) {
        return 1;
    }

    read_bytes = fread(guk, 1, 32, guk_file);
    if (read_bytes != 32) {
        return 1;
    }

    fclose(guk_file);

    return 0;
}


int generate_seed(uint8_t *bl1_2_hash, uint8_t *guk, uint8_t *seed_buf)
{
    uint8_t label[] = "BL1_CPAK_SEED_DERIVATION";
    uint8_t context[32 + sizeof(uint32_t) * 2] = {0};
    uint8_t state[sizeof(context) + sizeof(label) + sizeof(uint8_t)
                  + sizeof(uint32_t) * 2];
    uint8_t state_hash[PSA_HASH_LENGTH(PSA_ALG_SHA_256)];
    mbedtls_aes_context ctx;
    uint32_t reprovisioning_bits = 0;
    uint32_t lcs = 3;
    uint32_t seed_output_length = 32;
    uint32_t block_index = 1;
    size_t state_hash_len;
    psa_status_t status;
    int rc;

    memcpy(context, bl1_2_hash, 32);

    memcpy(context + 32, &lcs, sizeof(uint32_t));

    memcpy(context + 32 + sizeof(uint32_t), &reprovisioning_bits,
           sizeof(uint32_t));

    memcpy(state + sizeof(uint32_t), label, sizeof(label));
    memset(state + sizeof(uint32_t) + sizeof(label), 0, sizeof(uint8_t));
    memcpy(state + sizeof(uint32_t) + sizeof(label) + sizeof(uint8_t),
           context, sizeof(context));
    memcpy(state + sizeof(uint32_t) + sizeof(label) + sizeof(uint8_t) +
           sizeof(context), &seed_output_length, sizeof(uint32_t));

    status = psa_hash_compute(PSA_ALG_SHA_256, state, sizeof(state),
                              state_hash, sizeof(state_hash), &state_hash_len);
    if (status != PSA_SUCCESS) {
        return 1;
    }

    memcpy(state, &block_index, sizeof(uint32_t));

    status = psa_hash_compute(PSA_ALG_SHA_256, state, sizeof(state),
                              state_hash, sizeof(state_hash), &state_hash_len);
    if (status != PSA_SUCCESS) {
        return 1;
    }

    mbedtls_aes_init(&ctx);
    rc = mbedtls_aes_setkey_enc(&ctx, guk, 256);
    if (rc) {
        return rc;
    }

    rc = mbedtls_aes_crypt_ecb(&ctx, MBEDTLS_AES_ENCRYPT, state_hash, seed_buf);
    if (rc) {
        return rc;
    }

    rc = mbedtls_aes_crypt_ecb(&ctx, MBEDTLS_AES_ENCRYPT, state_hash + 16,
                               seed_buf + 16);
    if (rc) {
        return rc;
    }

    mbedtls_aes_free(&ctx);

    return 0;
}

int derive_key(uint8_t *seed_buf, psa_key_handle_t *cpak_handle)
{
    psa_status_t status;
    size_t output_key_len;
    psa_key_attributes_t seed_attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_attributes_t output_attr = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_handle_t seed_key = PSA_KEY_HANDLE_INIT;
    psa_key_derivation_operation_t op = PSA_KEY_DERIVATION_OPERATION_INIT;
    psa_set_key_type(&seed_attributes, PSA_KEY_TYPE_DERIVE);
    psa_set_key_algorithm(&seed_attributes, PSA_ALG_HKDF(PSA_ALG_SHA_256));
    psa_set_key_bits(&seed_attributes, 256);
    psa_set_key_usage_flags(&seed_attributes, PSA_KEY_USAGE_DERIVE);

    status = psa_import_key(&seed_attributes, seed_buf, 32, &seed_key);
    if (status != PSA_SUCCESS) {
        return 1;
    }

    status = psa_key_derivation_setup(&op, PSA_ALG_HKDF(PSA_ALG_SHA_256));
    if (status != PSA_SUCCESS) {
        goto err_release_seed_key;
    }

    status = psa_key_derivation_input_key(&op, PSA_KEY_DERIVATION_INPUT_SECRET,
                                          seed_key);
    if (status != PSA_SUCCESS) {
        goto err_release_op;
    }

    status = psa_key_derivation_input_bytes(&op, PSA_KEY_DERIVATION_INPUT_INFO,
                                            NULL, 0);
    if (status != PSA_SUCCESS) {
        goto err_release_op;
    }

    psa_set_key_type(&output_attr, PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1));
    psa_set_key_algorithm(&output_attr, PSA_ALG_ECDSA(PSA_ALG_SHA_384));
    psa_set_key_bits(&output_attr, 384);
    psa_set_key_usage_flags(&output_attr, PSA_KEY_USAGE_EXPORT);

    status = psa_key_derivation_output_key(&output_attr, &op, cpak_handle);
    if (status != PSA_SUCCESS) {
        goto err_release_op;
    }

    status = psa_key_derivation_abort(&op);
    if (status != PSA_SUCCESS) {
        goto err_release_seed_key;
    }

    status = psa_destroy_key(seed_key);
    if (status != PSA_SUCCESS) {
        return 1;
    }

    return 0;

err_release_output_key:
    (void)psa_destroy_key(*cpak_handle);

err_release_op:
    (void)psa_key_derivation_abort(&op);

err_release_seed_key:
    (void)psa_destroy_key(seed_key);

    return 1;
}

int export_pubkey(psa_key_handle_t cpak_handle)
{
    psa_status_t status;
    uint8_t cpak_buf[128];
    size_t cpak_size;
    FILE *cpak_output_file;
    size_t written_bytes;

    status = psa_export_public_key(cpak_handle, cpak_buf, sizeof(cpak_buf),
                                   &cpak_size);
    if (status != PSA_SUCCESS) {
        return 1;
    }

    cpak_output_file = fopen(CPAK_OUTPUT_PATH, "wb");
    if (cpak_output_file == NULL) {
        return 1;
    }

    written_bytes = fwrite(cpak_buf, 1, cpak_size, cpak_output_file);
    if (written_bytes != cpak_size) {
        return 1;
    }

    return 0;
}

int main (int argc, char *argv[])
{
    int rc;
    uint8_t bl1_2_hash[32];
    uint8_t guk[32];
    uint8_t seed_buf[32];
    psa_key_handle_t cpak_handle;

    rc = psa_crypto_init();
    if (rc) {
        printf("psa init failed\r\n");
        return rc;
    }

    rc = load_bl1_2_hash(bl1_2_hash);
    if (rc) {
        printf("bl1_2_hash load failed\r\n");
        return rc;
    }

    rc = load_guk(guk);
    if (rc) {
        printf("GUK load failed\r\n");
        return rc;
    }

    rc = generate_seed(bl1_2_hash, guk, seed_buf);
    if (rc) {
        printf("cpak seed generation failed\r\n");
        return rc;
    }

    rc = derive_key(seed_buf, &cpak_handle);
    if (rc) {
        printf("cpak derivation failed\r\n");
        return rc;
    }

    rc = export_pubkey(cpak_handle);
    if (rc) {
        printf("cpak output failed\r\n");
    }

    return 0;
}
