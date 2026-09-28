/**
 * *****************************************************************************
 * @file    rtk_pke_bignum.c
 * @brief   PKE-accelerated modular exponentiation.
 * *****************************************************************************
 * @attention
 *
 * @note    4.x removed every *_ALT hook, and RSA has no PSA transparent-driver
 *          entry point of its own: psa_asymmetric_encrypt/decrypt and
 *          sign/verify_hash for RSA all end up in mbedtls_rsa_public() /
 *          mbedtls_rsa_private(). Both of those reach the engine through a
 *          single chokepoint, mbedtls_mpi_exp_mod_optionally_safe(), which is
 *          therefore where the hardware call is spliced in (see the RTK hunk in
 *          tf-psa-crypto/drivers/builtin/src/bignum.c).
 *
 * @note    Endianness: the PKE engine reads and writes little-endian byte
 *          buffers, which is exactly the in-memory layout of mbedtls_mpi limbs
 *          on this little-endian core, so limb arrays are handed over directly.
 *
 * @note    Constant-time: the engine is not constant time with respect to the
 *          exponent, so this path is only taken for RSA-sized operands where
 *          the software replacement would be prohibitively slow. Callers that
 *          need a secret-independent exponent must not enable this.
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 * *****************************************************************************
 */

/* mbedtls_mpi_read_binary_le is guarded by MBEDTLS_DECLARE_PRIVATE_IDENTIFIERS. */
#define MBEDTLS_ALLOW_PRIVATE_ACCESS

#include "rtk_pke_bignum.h"

#include "ameba_soc.h"
#include "mbedtls/platform.h"

/* The engine's RSA parameter window is PKE_RSA_PARAMETER_SIZE bytes wide. */
#define RTK_PKE_RSA_MAX_BYTES PKE_RSA_PARAMETER_SIZE
/* Below this size the software path is faster than the engine hand-off. */
#define RTK_PKE_RSA_MIN_BYTES 64

int rtk_pke_mpi_exp_mod(mbedtls_mpi *X, const mbedtls_mpi *A,
					   const mbedtls_mpi *E, const mbedtls_mpi *N)
{
	size_t n_size = mbedtls_mpi_size(N);
	size_t a_size = mbedtls_mpi_size(A);
	size_t e_size = mbedtls_mpi_size(E);
	uint8_t *res_le;
	int ret;

	if (n_size < RTK_PKE_RSA_MIN_BYTES || n_size > RTK_PKE_RSA_MAX_BYTES ||
		a_size == 0 || a_size > n_size ||
		e_size == 0 || e_size > RTK_PKE_RSA_MAX_BYTES) {
		return MBEDTLS_ERR_MPI_NOT_ACCEPTABLE;
	}

	/* The engine needs an odd modulus, a non-negative base and A < N; it does
	 * not reduce the base itself. */
	if ((N->p[0] & 1) == 0 || A->s < 0 || mbedtls_mpi_cmp_mpi(A, N) >= 0) {
		return MBEDTLS_ERR_MPI_NOT_ACCEPTABLE;
	}

	res_le = mbedtls_calloc(n_size, 1);
	if (res_le == NULL) {
		return MBEDTLS_ERR_MPI_ALLOC_FAILED;
	}

	ret = pke_rsa_exp_mod(res_le, n_size,
						  (uint8_t *) A->p, a_size,
						  (uint8_t *) E->p, e_size,
						  (uint8_t *) N->p, n_size);
	if (ret != RTK_SUCCESS) {
		ret = MBEDTLS_ERR_MPI_NOT_ACCEPTABLE;
		goto cleanup;
	}

	ret = mbedtls_mpi_read_binary_le(X, res_le, n_size);

cleanup:
	mbedtls_platform_zeroize(res_le, n_size);
	mbedtls_free(res_le);
	return ret;
}
