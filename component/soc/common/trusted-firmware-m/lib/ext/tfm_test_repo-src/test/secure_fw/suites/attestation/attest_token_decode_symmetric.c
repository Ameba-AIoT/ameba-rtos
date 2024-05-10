/*
 * attest_symmetric_iat_decode.c
 *
 * Copyright (c) 2019, Laurence Lundblade.
 * Copyright (c) 2020-2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * See BSD-3-Clause license in README.md
 */

#include "attest_token_decode.h"
#include "attest.h"
#include "psa/crypto.h"
#include "q_useful_buf.h"
#include "qcbor_util.h"
#include "t_cose_common.h"
#include "t_cose_mac0_verify.h"
#include "tfm_crypto_defs.h"

/* Only support HMAC as MAC algorithm in COSE_Mac0 so far */
#define SYMMETRIC_IAK_MAX_SIZE        PSA_MAC_MAX_SIZE

#if DOMAIN_NS == 1U
/*
 * Public function. See attest_token_decode.h
 * It is not allowed to let NS side fetch the symmetric IAK and perform the MAC
 * verification.
 */
enum attest_token_err_t
attest_token_decode_validate_token(struct attest_token_decode_context *me,
                                   struct q_useful_buf_c               token)
{
    enum t_cose_err_t              t_cose_error;
    enum attest_token_err_t        return_value;
    /* Decode only without signature verification */
    int32_t                        t_cose_options = T_COSE_OPT_DECODE_ONLY;
    struct t_cose_mac0_verify_ctx  verify_ctx;
    struct t_cose_key              attest_key = T_COSE_NULL_KEY;

    t_cose_mac0_verify_init(&verify_ctx, t_cose_options);

    t_cose_mac0_set_verify_key(&verify_ctx, attest_key);

    t_cose_error = t_cose_mac0_verify(&verify_ctx,
                                      token, /* COSE to verify */
                                      &me->payload, /* Payload from token */
                                      NULL
                                     );

    return_value = map_t_cose_errors(t_cose_error);
    me->last_error = return_value;

    return return_value;
}

#else /* DOMAIN_NS == 1U */

/*
 * Public function. See attest_token_decode.h
 * Decode the received COSE_Mac0 structure and verify the tag. Authentication
 * tag verification in tests is for debug purpose only. The symmetric Initial
 * Attestation key (IAK) should not be able to be used by anything other than
 * the Attestation partition in real products.
 */
enum attest_token_err_t
attest_token_decode_validate_token(struct attest_token_decode_context *me,
                                   struct q_useful_buf_c               token)
{
    enum t_cose_err_t              t_cose_error;
    enum attest_token_err_t        return_value;
    int32_t                        t_cose_options = 0;
    struct t_cose_mac0_verify_ctx  verify_ctx;
    struct t_cose_key              attest_key;
    psa_key_handle_t               key_handle = TFM_BUILTIN_KEY_ID_IAK;

    if (me->options & TOKEN_OPT_SHORT_CIRCUIT_SIGN) {
        t_cose_options |= T_COSE_OPT_ALLOW_SHORT_CIRCUIT;
    }

    t_cose_mac0_verify_init(&verify_ctx, t_cose_options);

    attest_key.crypto_lib = T_COSE_CRYPTO_LIB_PSA;
    attest_key.k.key_handle = (uint64_t)key_handle;
    t_cose_mac0_set_verify_key(&verify_ctx, attest_key);

    t_cose_error = t_cose_mac0_verify(&verify_ctx,
                                      token, /* COSE to verify */
                                      &me->payload, /* Payload from token */
                                      NULL);

    return_value = map_t_cose_errors(t_cose_error);
    me->last_error = return_value;

    return return_value;
}
#endif /* DOMAIN_NS == 1U */
