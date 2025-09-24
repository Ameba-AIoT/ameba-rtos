/**
 * @brief  PKE hardware API for Mbedtls ECC caculation
 *
 * ECDSA:
 * mbedtls_ecdsa_genkey
 * mbedtls_ecdsa_sign
 * mbedtls_ecdsa_verify
 *
 * ECDH:
 * mbedtls_ecdh_gen_public
 * mbedtls_ecdh_compute_shared
 */

#include "ameba.h"
#include "mbedtls/ecdsa.h"
#include "mbedtls/error.h"
#include "mbedtls/asn1write.h"
#include "mbedtls/ecdh.h"
#include "mbedtls/bignum.h"
#include "mbedtls/platform.h"

/* define it in mbedtls_config_xxx.h */
#if defined(MBEDTLS_ECDH_GEN_PUBLIC_ALT) || defined(MBEDTLS_ECDH_COMPUTE_SHARED_ALT) || \
     defined(MBEDTLS_ECDSA_VERIFY_ALT) || defined(MBEDTLS_ECDSA_SIGN_ALT) || defined(MBEDTLS_ECDSA_GENKEY_ALT)

ecdsa_ecp_group_id get_curve_id_from_mbedtls(mbedtls_ecp_group *grp)
{
    ecdsa_ecp_group_id curve_id;

    if (grp->id == MBEDTLS_ECP_DP_SECP192R1) {
        curve_id = ECDSA_ECP_SECP192R1;
    } else if (grp->id == MBEDTLS_ECP_DP_SECP224R1) {
        curve_id = ECDSA_ECP_SECP224R1;
    } else if (grp->id == MBEDTLS_ECP_DP_SECP256R1) {
        curve_id = ECDSA_ECP_SECP256R1;
    } else if (grp->id == MBEDTLS_ECP_DP_BP256R1) {
        curve_id = ECDSA_ECP_BP256R1;
    } else if (grp->id == MBEDTLS_ECP_DP_CURVE25519) {
        curve_id = ECDSA_ECP_CURVE25519;
    } else if (grp->id == MBEDTLS_ECP_DP_SECP192K1) {
        curve_id = ECDSA_ECP_SECP192K1;
    } else if (grp->id == MBEDTLS_ECP_DP_SECP224K1) {
        curve_id = ECDSA_ECP_SECP224K1;
    } else if (grp->id == MBEDTLS_ECP_DP_SECP256K1) {
        curve_id = ECDSA_ECP_SECP256K1;
    } else {
        curve_id = ECDSA_ECP_NONE;
        mbedtls_printf("unsupport curve. %s \n", __func__);
        while(1);
    }
    return curve_id;
}

#endif

#if defined(MBEDTLS_ECDSA_SIGN_ALT)
int mbedtls_ecdsa_sign(mbedtls_ecp_group *grp, mbedtls_mpi *r, mbedtls_mpi *s,
                       const mbedtls_mpi *d, const unsigned char *buf, size_t blen,
                       int (*f_rng)(void *, unsigned char *, size_t), void *p_rng)
{
    UNUSED(f_rng);
    UNUSED(p_rng);

    u8 Q[64];
    u32 otpkey = 0;
    ECDSA_TypeDef *ECDSA = (ECDSA_TypeDef *)ECDSA_REG_BASE;

    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    // pke_ecp_group pke_grp;
    size_t precise_byte = (grp->pbits + 7 ) / 8;

    uint8_t *rand_buf = mbedtls_calloc(precise_byte, 1);
    uint8_t *res_sign_r_buf = mbedtls_calloc(precise_byte, 1);
    uint8_t *res_sign_s_buf = mbedtls_calloc(precise_byte, 1);

    /* check hash len */
    if (blen > 64) {
        mbedtls_printf("[%s] hash len unsupport \n", __func__);
        goto cleanup;
    }

    /* get rand buffer */
    TRNG_get_random_bytes(rand_buf, precise_byte - 1);
    rand_buf[precise_byte - 1] = 0;

    if ((ret = ECDSA_Sign(ECDSA,get_curve_id_from_mbedtls(grp),(uint8_t *)d->p,rand_buf,buf,blen,Q,Q+32,res_sign_r_buf,res_sign_s_buf,otpkey)) != 0) {
        mbedtls_printf("%s fail, ret %d\n", __func__, ret);
        goto cleanup;
    }

    /* read r/s */
    if ((ret = mbedtls_mpi_read_binary_le(r, res_sign_r_buf, precise_byte)) != 0) {
        mbedtls_printf("%s read r fail, ret %d\n", __func__, ret);
        goto cleanup;
    }

    if ((ret = mbedtls_mpi_read_binary_le(s, res_sign_s_buf, precise_byte)) != 0) {
        mbedtls_printf("%s read s fail, ret %d\n", __func__, ret);
        goto cleanup;
    }

cleanup:
    mbedtls_free(rand_buf);
    mbedtls_free(res_sign_r_buf);
    mbedtls_free(res_sign_s_buf);
    return ret;
}
#endif

#if defined(MBEDTLS_ECDSA_VERIFY_ALT)
int mbedtls_ecdsa_verify(mbedtls_ecp_group *grp,
                         const unsigned char *buf, size_t blen,
                         const mbedtls_ecp_point *Q,
                         const mbedtls_mpi *r,
                         const mbedtls_mpi *s)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;

    ECDSA_TypeDef *ECDSA = (ECDSA_TypeDef *)ECDSA_REG_BASE;

    if (blen > 64) {
        mbedtls_printf("[%s] hash len unsupport \n", __func__);
        goto cleanup;
    }

    ret = ECDSA_Verify(ECDSA,get_curve_id_from_mbedtls(grp),buf,blen,(uint8_t *)Q->X.p, (uint8_t *)Q->Y.p,(uint8_t *)r->p, (uint8_t *)s->p);

    if (ret != RTK_SUCCESS) {
        mbedtls_printf("%s fail, ret %d\n", __func__, ret);
    }

cleanup:
    return ret;
}
#endif

#if defined(MBEDTLS_ECDSA_GENKEY_ALT)
int mbedtls_ecdsa_genkey(mbedtls_ecdsa_context *ctx,
                         mbedtls_ecp_group_id gid,
                         int (*f_rng)(void *, unsigned char *, size_t),
                         void *p_rng)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    ECDSA_TypeDef *ECDSA = (ECDSA_TypeDef *)ECDSA_REG_BASE;
    size_t key_len;

    uint8_t d_buf[32] = {0};
    uint8_t Q_x[32] = {0};
    uint8_t Q_y[32] = {0};

    if (ctx == NULL || f_rng == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    MBEDTLS_MPI_CHK(mbedtls_ecp_group_load(&ctx->grp, gid));

    key_len = (ctx->grp.pbits + 7) / 8;

    MBEDTLS_MPI_CHK(mbedtls_ecp_gen_privkey(&ctx->grp, &ctx->d, f_rng, p_rng));

    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&ctx->d, d_buf, key_len));
    reverse_bytes(d_buf, key_len);

    const int curve_id = get_curve_id_from_mbedtls(&ctx->grp);

    if ((ret = ECDSA_KeyGen(ECDSA, curve_id, d_buf, Q_x, Q_y, 0)) != 0) {
        printf("ECDSA_KeyGen failed: 0x%X\n", ret);
        goto cleanup;
    }

    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary_le(&ctx->Q.X, Q_x, key_len));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary_le(&ctx->Q.Y, Q_y, key_len));
    MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&ctx->Q.Z, 1));

cleanup:
    return ret;
}
#endif


#if defined(MBEDTLS_ECDH_GEN_PUBLIC_ALT)
int mbedtls_ecdh_gen_public(mbedtls_ecp_group *grp, mbedtls_mpi *d, mbedtls_ecp_point *Q,
                            int (*f_rng)(void *, unsigned char *, size_t),
                            void *p_rng)
{
#if defined(MBEDTLS_ECP_RESTARTABLE)
#error "MBEDTLS_ECDH_GEN_PUBLIC_ALT do not support MBEDTLS_ECP_RESTARTABLE"
#endif

    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    u32 otpkey = 0;
    ECDSA_TypeDef *ECDSA = (ECDSA_TypeDef *)ECDSA_REG_BASE;
    size_t key_len;

    uint8_t d_buf[32] = {0};
    uint8_t Q_x[32] = {0};
    uint8_t Q_y[32] = {0};

    if (grp == NULL || d == NULL || Q == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    mbedtls_ecp_point_init(Q);
    MBEDTLS_MPI_CHK(mbedtls_ecp_gen_privkey(grp, d, f_rng, p_rng));
    key_len = (grp->pbits + 7) / 8;;

    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(d, d_buf, key_len));

    reverse_bytes(d_buf, key_len);

    ret = ECDSA_KeyGen(ECDSA, get_curve_id_from_mbedtls(grp), d_buf, Q_x, Q_y, otpkey);
    if (ret != 0) {
        mbedtls_printf("ECDSA_KeyGen failed: %d\n", ret);
        goto cleanup;
    }

    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary_le(&Q->X, Q_x, key_len));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary_le(&Q->Y, Q_y,  key_len));
    MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&Q->Z, 1));

cleanup:
    return ret;
}
#endif

#if defined(MBEDTLS_ECDH_COMPUTE_SHARED_ALT)
int mbedtls_ecdh_compute_shared(mbedtls_ecp_group *grp, mbedtls_mpi *z,
                                const mbedtls_ecp_point *Q, const mbedtls_mpi *d,
                                int (*f_rng)(void *, unsigned char *, size_t),
                                void *p_rng)
{
    UNUSED(f_rng);
    UNUSED(p_rng);

    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    u32 otpkey = 0;
    ECDSA_TypeDef *ECDSA = (ECDSA_TypeDef *)ECDSA_REG_BASE;
    uint8_t Q_x[32] = {0};
    uint8_t Q_y[32] = {0};
    uint8_t d_buf[32] = {0};
    uint8_t Z[64] = {0};

    mbedtls_mpi_init(z);

    if (grp == NULL || z == NULL || Q == NULL || d == NULL) {
        ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
        mbedtls_printf("MBEDTLS_ERR_ECP_BAD_INPUT_DATA\n");
        goto cleanup;
    }

    size_t priv_len =(grp->pbits + 7) / 8;;

    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&Q->X, Q_x, priv_len));
    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&Q->Y, Q_y, priv_len));
    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(d, d_buf, priv_len));

    reverse_bytes(Q_x, priv_len);
    reverse_bytes(Q_y, priv_len);
    reverse_bytes(d_buf, priv_len);

    ret = ECDSA_ECDH_Compute_Shared(ECDSA, get_curve_id_from_mbedtls(grp), Z, Z+priv_len, Q_x, Q_y, d_buf, otpkey);
    if (ret != 0) {
        mbedtls_printf("ECDSA_ECDH_Compute_Shared failed: %d\n", ret);
        goto cleanup;
    }

    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary_le(z, Z, priv_len));
    z->s = 1;

cleanup:
    return ret;
}
#endif
