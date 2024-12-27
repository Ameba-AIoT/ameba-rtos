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

#define PKE_MONTGOMERY_DECODE_SCALAR_25519(k_list)  \
    do {                                            \
        uint8_t *list = (uint8_t *)k_list;          \
        (list)[0] &= 248;                           \
        (list)[31] &= 127;                          \
        (list)[31] |= 64;                           \
    } while (0)

#define PKE_MONTGOMERY_DECODE_SCALAR_448(k_list)    \
    do {                                            \
        uint8_t *list = (uint8_t *)k_list;          \
        (list)[0] &= 252;                           \
        (list)[55] |= 128;                          \
    } while (0)

/*X25519 (but not X448) MUST mask the most significant bit in the final byte.
  Here, the "bits" parameter should be set to 255 for X25519 and 448 for X448 */
#define PKE_MONTGOMERY_DECODE_U_COORDINATE(u_list, bits)   \
    do {                                                   \
        uint8_t *list = (uint8_t *)u_list;                 \
        int bytes = ((bits) + 7) / 8;                      \
        if ((bits) % 8) {                                  \
            (list)[bytes - 1] &= (1 << ((bits) % 8)) - 1;  \
        }                                                  \
    } while (0)

static pke_ecp_curve_id get_curve_id_from_mbedtls(mbedtls_ecp_group *grp)
{
	pke_ecp_curve_id curve_id;

	if (grp->id == MBEDTLS_ECP_DP_SECP192R1) {
		curve_id = PKE_ECP_CURVE_SECP192R1;
	} else if (grp->id == MBEDTLS_ECP_DP_SECP224R1) {
		curve_id = PKE_ECP_CURVE_SECP224R1;
	} else if (grp->id == MBEDTLS_ECP_DP_SECP256R1) {
		curve_id = PKE_ECP_CURVE_SECP256R1;
	} else if (grp->id == MBEDTLS_ECP_DP_SECP384R1) {
		curve_id = PKE_ECP_CURVE_SECP384R1;
	} else if (grp->id == MBEDTLS_ECP_DP_BP256R1) {
		curve_id = PKE_ECP_CURVE_BP256R1;
	} else if (grp->id == MBEDTLS_ECP_DP_BP384R1) {
		curve_id = PKE_ECP_CURVE_BP384R1;
	} else if (grp->id == MBEDTLS_ECP_DP_BP512R1) {
		curve_id = PKE_ECP_CURVE_BP512R1;
	} else if (grp->id == MBEDTLS_ECP_DP_CURVE25519) {
		curve_id = PKE_ECP_CURVE_CURVE25519;
	} else if (grp->id == MBEDTLS_ECP_DP_SECP192K1) {
		curve_id = PKE_ECP_CURVE_SECP192K1;
	} else if (grp->id == MBEDTLS_ECP_DP_SECP224K1) {
		curve_id = PKE_ECP_CURVE_SECP224K1;
	} else if (grp->id == MBEDTLS_ECP_DP_SECP256K1) {
		curve_id = PKE_ECP_CURVE_SECP256K1;
	} else if (grp->id == MBEDTLS_ECP_DP_CURVE448) {
		curve_id = PKE_ECP_CURVE_CURVE448;
	} else {
		curve_id = PKE_ECP_CURVE_NONE;
		mbedtls_printf("unsupport curve. %s \n", __func__);
		while(1);
	}
	return curve_id;
}

static int ecp_mul_restartable_internal(mbedtls_ecp_group *grp, mbedtls_ecp_point *R,
										const mbedtls_mpi *m, const mbedtls_ecp_point *P,
										int (*f_rng)(void *, unsigned char *, size_t), void *p_rng,
										mbedtls_ecp_restart_ctx *rs_ctx)
{
	UNUSED(f_rng);
	UNUSED(p_rng);
	UNUSED(rs_ctx);
	int ret = -1;

	uint8_t res_x[64];
	uint8_t res_y[64];
	uint8_t res_z = 1;

	// decode u and scalar
	if (grp->id == MBEDTLS_ECP_DP_CURVE25519) {
		PKE_MONTGOMERY_DECODE_SCALAR_25519(m->p);
		PKE_MONTGOMERY_DECODE_U_COORDINATE(P->X.p, 255);
	} else if (grp->id == MBEDTLS_ECP_DP_CURVE448) {
		PKE_MONTGOMERY_DECODE_SCALAR_448(m->p);
	}

	// pke ecp mul
	pke_ecp_group pke_grp;
	pke_ecp_point pke_R, pke_P;
	pke_ecp_group_init_in_rom(&pke_grp, get_curve_id_from_mbedtls(grp));
	pke_ecp_point_init_base_point(&pke_grp, &pke_P);
	pke_ecp_point_init(&pke_R);

	pke_P.X_size = mbedtls_mpi_size(&P->X);
	if (pke_P.X_size) {
		pke_P.X_p = (uint8_t *)P->X.p;
	}
	pke_P.Y_size = mbedtls_mpi_size(&P->Y);
	if (pke_P.Y_size) {
		pke_P.Y_p = (uint8_t *)P->Y.p;
	}
	pke_R.X_p = res_x;
	pke_R.Y_p = res_y;

	ret = pke_ecp_mul(&pke_grp, &pke_R, (uint8_t *)m->p, mbedtls_mpi_size(m),  &pke_P);

	// read result to mbedtls mpi
	mbedtls_mpi_read_binary_le(&R->Y, res_y, pke_grp.precise_bits / 8);
	mbedtls_mpi_read_binary_le(&R->X, res_x, pke_grp.precise_bits / 8);
	mbedtls_mpi_read_binary_le(&R->Z, &res_z, 1);
	return ret;
}
#endif

#if defined(MBEDTLS_ECDSA_SIGN_ALT)
int mbedtls_ecdsa_sign(mbedtls_ecp_group *grp, mbedtls_mpi *r, mbedtls_mpi *s,
                       const mbedtls_mpi *d, const unsigned char *buf, size_t blen,
                       int (*f_rng)(void *, unsigned char *, size_t), void *p_rng)
{
	UNUSED(f_rng);
	UNUSED(p_rng);

	int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
	pke_ecp_group pke_grp;
	size_t precise_byte = (grp->pbits + 7 ) / 8;
	uint8_t *hash_buf = mbedtls_calloc(blen, 1);
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

	/* process hash buffer */
	pke_ecdsa_lalu_hash_process(hash_buf, (uint8_t *)buf, blen, grp->pbits);

	/* temp sign buffer */
	pke_ecp_group_init_in_rom(&pke_grp, get_curve_id_from_mbedtls(grp));
	if ((ret = pke_ecdsa_write_signature(&pke_grp, PKE_ECDSA_PRIV_KEY_SW, rand_buf, precise_byte,
										 (uint8_t *)d->p, mbedtls_mpi_size(d),
										 hash_buf, blen, res_sign_r_buf, res_sign_s_buf)) != 0) {
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
	mbedtls_free(hash_buf);
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
	pke_ecp_group pke_grp;

	uint8_t *hash_buf = mbedtls_calloc(blen, 1);

	if (blen > 64) {
		mbedtls_printf("[%s] hash len unsupport \n", __func__);
		goto cleanup;
	}

	pke_ecdsa_lalu_hash_process(hash_buf, (uint8_t *)buf, blen, grp->pbits);
	pke_ecp_group_init_in_rom(&pke_grp, get_curve_id_from_mbedtls(grp));
	ret = pke_ecdsa_read_signature(&pke_grp, (uint8_t *)Q->X.p, (uint8_t *)Q->Y.p,
										hash_buf, blen, (uint8_t *)r->p, (uint8_t *)s->p);
	if (ret != SUCCESS) {
		mbedtls_printf("%s fail, ret %d\n", __func__, ret);
	}

cleanup:
	mbedtls_free(hash_buf);
	return ret;
}
#endif

#if defined(MBEDTLS_ECDSA_GENKEY_ALT)
int mbedtls_ecdsa_genkey(mbedtls_ecdsa_context *ctx, mbedtls_ecp_group_id gid,
                         int (*f_rng)(void *, unsigned char *, size_t), void *p_rng)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    ret = mbedtls_ecp_group_load(&ctx->grp, gid);
    if (ret != 0) {
        goto cleanup;
    }

	MBEDTLS_MPI_CHK(mbedtls_ecp_gen_privkey(&ctx->grp, &ctx->d, f_rng, p_rng));
    MBEDTLS_MPI_CHK(ecp_mul_restartable_internal(&ctx->grp, &ctx->Q, &ctx->d, &ctx->grp.G, f_rng, p_rng, NULL));

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
	MBEDTLS_MPI_CHK(mbedtls_ecp_gen_privkey(grp, d, f_rng, p_rng));
	MBEDTLS_MPI_CHK(ecp_mul_restartable_internal(grp, Q, d, &grp->G,
					f_rng, p_rng, NULL));

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
	int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
	mbedtls_ecp_point P;

	mbedtls_ecp_point_init(&P);

	MBEDTLS_MPI_CHK(ecp_mul_restartable_internal(grp, &P, d, Q,
					f_rng, p_rng, NULL));
	if (mbedtls_ecp_is_zero(&P)) {
		ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
		goto cleanup;
	}

	MBEDTLS_MPI_CHK(mbedtls_mpi_copy(z, &P.X));

cleanup:
	mbedtls_ecp_point_free(&P);

	return ret;
}
#endif
