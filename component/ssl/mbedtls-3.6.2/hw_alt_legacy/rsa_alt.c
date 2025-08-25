/**
 * @brief  PKE hardware API for Mbedtls RSA caculation
 * 
 * RSA:
 * mbedtls_rsa_gen_key
 * mbedtls_rsa_public
 * mbedtls_rsa_private
 */

#include "ameba_soc.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mbedtls/rsa.h"
#include "mbedtls/error.h"
#include "mbedtls/asn1write.h"
#include "mbedtls/bignum.h"
#include "mbedtls/platform.h"

// /* define it in mbedtls_config_xxx.h */
#if defined(RTK_LEGACY_MBEDTLS_RSA_CACULATION_ALT)

/* Only used for RSA encryption and decryption calculations in this file */
static int mbedtls_mpi_exp_mod_prime_n(mbedtls_mpi *X, const mbedtls_mpi *A,
		const mbedtls_mpi *E, const mbedtls_mpi *N, mbedtls_mpi *prec_RR)
{
	UNUSED(prec_RR);
	size_t N_size = mbedtls_mpi_size(N);
	char *X_mpi_le = malloc(N_size);
    char *A_le = NULL;
    char *E_le = NULL;
    char *N_le = NULL;
	int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;

    A_le = malloc(N_size);
    mbedtls_mpi_write_binary(A, (uint8_t *)A_le, N_size);
    reverse_bytes((uint8_t *)A_le, N_size);

    E_le = malloc(N_size);
    mbedtls_mpi_write_binary(E, (uint8_t *)E_le, N_size);
    reverse_bytes((uint8_t *)E_le, N_size);

    N_le = malloc(N_size);
    mbedtls_mpi_write_binary(N, (uint8_t *)N_le, N_size);
    reverse_bytes((uint8_t *)N_le, N_size);

	RSA_TypeDef *RSA = (RSA_TypeDef *)RSA_BASE;
	ret = RSA_HW_EXP_MOD(RSA, X_mpi_le, N_size, A_le, N_size, E_le, N_size, N_le, N_size);

	mbedtls_mpi_read_binary_le(X, (uint8_t *)X_mpi_le, N_size);

	free(X_mpi_le);
    free(A_le);
    free(E_le);
    free(N_le);
	return ret;
}

int mbedtls_rsa_public(mbedtls_rsa_context *ctx, const unsigned char *input, unsigned char *output)
{
	int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
	size_t olen;
	mbedtls_mpi T;

	mbedtls_mpi_init(&T);

#if defined(MBEDTLS_THREADING_C)
	if ((ret = mbedtls_mutex_lock(&ctx->mutex)) != 0) {
		return ret;
	}
#endif

	MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&T, input, ctx->len));

	if (mbedtls_mpi_cmp_mpi(&T, &ctx->N) >= 0) {
		ret = MBEDTLS_ERR_MPI_BAD_INPUT_DATA;
		goto cleanup;
	}

	olen = ctx->len;
	MBEDTLS_MPI_CHK(mbedtls_mpi_exp_mod_prime_n(&T, &T, &ctx->E, &ctx->N, &ctx->RN));
	MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&T, output, olen));

cleanup:
	mbedtls_mpi_free(&T);

#if defined(MBEDTLS_THREADING_C)
	if (mbedtls_mutex_unlock(&ctx->mutex) != 0) {
		return MBEDTLS_ERR_THREADING_MUTEX_ERROR;
	}
#endif

	if (ret != 0) {
		return MBEDTLS_ERROR_ADD(MBEDTLS_ERR_RSA_PUBLIC_FAILED, ret);
	}

	return 0;
}

int mbedtls_rsa_private(mbedtls_rsa_context *ctx, int (*f_rng)(void *, unsigned char *, size_t),
						void *p_rng, const unsigned char *input, unsigned char *output)
{
	UNUSED(f_rng);
	UNUSED(p_rng);

	int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
	size_t olen;

	/* Temporary holding the result */
	mbedtls_mpi T;

#if defined(MBEDTLS_THREADING_C)
	if ((ret = mbedtls_mutex_lock(&ctx->mutex)) != 0) {
		return ret;
	}
#endif

	/* MPI Initialization */
	mbedtls_mpi_init(&T);
	/* End of MPI initialization */

	MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&T, input, ctx->len));
	if (mbedtls_mpi_cmp_mpi(&T, &ctx->N) >= 0) {
		ret = MBEDTLS_ERR_MPI_BAD_INPUT_DATA;
		goto cleanup;
	}

	MBEDTLS_MPI_CHK(mbedtls_mpi_exp_mod_prime_n(&T, &T, &ctx->D, &ctx->N, &ctx->RN));

	olen = ctx->len;
	MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&T, output, olen));

cleanup:
	mbedtls_mpi_free(&T);

#if defined(MBEDTLS_THREADING_C)
	if (mbedtls_mutex_unlock(&ctx->mutex) != 0) {
		return MBEDTLS_ERR_THREADING_MUTEX_ERROR;
	}
#endif

	if (ret != 0 && ret >= -0x007f) {
		return MBEDTLS_ERROR_ADD(MBEDTLS_ERR_RSA_PRIVATE_FAILED, ret);
	}

	return ret;
}
#endif
