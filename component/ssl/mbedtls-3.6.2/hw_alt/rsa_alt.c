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

/* define it in mbedtls_config_xxx.h */
#if defined(RTK_LALU_MBEDTLS_RSA_CACULATION_ALT)

extern int mpi_check_small_factors(const mbedtls_mpi *X);
extern int mbedtls_rsa_deduce_crt(const mbedtls_mpi *P, const mbedtls_mpi *Q,
                           const mbedtls_mpi *D, mbedtls_mpi *DP,
                           mbedtls_mpi *DQ, mbedtls_mpi *QP);

static int pke_rsa_prime_generate(size_t nbits, uint8_t *num_buf, int flags)
{
	uint32_t max_loop = 10000;
	uint32_t loops = max_loop;
	uint8_t a_buf[256];
	size_t rounds = 0;
	size_t prime_bytes = nbits / 8;

	if ((flags & MBEDTLS_MPI_GEN_PRIME_FLAG_LOW_ERR) == 0) {
		/*
		 * 2^-80 error probability, number of rounds chosen per HAC, table 4.4
		 */
		rounds = ((nbits >= 1300) ?  2 : (nbits >=  850) ?  3 :
				  (nbits >=  650) ?  4 : (nbits >=  350) ?  8 :
				  (nbits >=  250) ? 12 : (nbits >=  150) ? 18 : 27);
	} else {
		/*
		 * 2^-100 error probability, number of rounds computed based on HAC,
		 * fact 4.48
		 */
		rounds = ((nbits >= 1450) ?  4 : (nbits >=  1150) ?  5 :
				  (nbits >= 1000) ?  6 : (nbits >=   850) ?  7 :
				  (nbits >=  750) ?  8 : (nbits >=   500) ? 13 :
				  (nbits >=  250) ? 28 : (nbits >=   150) ? 40 : 51);
	}

	mbedtls_mpi N;
	mbedtls_mpi_init(&N);

	while (loops > 0) {
		/* generate _rand n: The highest four bits are not zero,
			which facilitates the subsequent generation of a.
			And the lowest bit cannot be 0 */
		while (1) {
			TRNG_get_random_bytes(num_buf, prime_bytes);
			if ((num_buf[0] & 0x1) && (num_buf[prime_bytes - 1] & 0xf0)) {
				break;
			}
		}

		size_t round_t = rounds;
		while (round_t > 0) {
			/* generate _rand a: 2 <= a <= n - 2 */
			while (1) {
				TRNG_get_random_bytes(a_buf, prime_bytes);
				if (a_buf[prime_bytes - 1] < num_buf[prime_bytes - 1]) {
					break;
				}
			}

			/* check small factors */
			mbedtls_mpi_read_binary_le(&N, num_buf, nbits / 8);
			if (mpi_check_small_factors(&N) == MBEDTLS_ERR_MPI_NOT_ACCEPTABLE) {
				break;
			}

			/* witness(a, n) */
			if (pke_rsa_witness(a_buf, nbits, num_buf, nbits) != SUCCESS) {
				break;
			}
			round_t -= 1;
		}

		if (round_t == 0) {
			/* gen prime success */
			break;
		}
		loops -= 1;
	}
	mbedtls_mpi_free(&N);

	// printf("[%s] try loop: %d\n", __func__, max_loop - loops);

	if (loops > 0) {
		return SUCCESS;
	} else {
		return FAIL;
	}
}

/* PKE generate prime number */
static int mbedtls_mpi_gen_prime_pke(mbedtls_mpi *X, size_t nbits, int flags,
							  int (*f_rng)(void *, unsigned char *, size_t), void *p_rng)
{
	UNUSED(f_rng);
	UNUSED(p_rng);

	int ret = 0;
	// uint32_t t0, t1;
	// t0 = rtos_time_get_current_system_time_ms();

	// for X < 128 bits as PKE does not support it
	if (nbits < 128) {
        return MBEDTLS_ERR_MPI_BAD_INPUT_DATA;
	}

	if (nbits % 8 != 0 || nbits > 4096) {
		return MBEDTLS_ERR_MPI_BAD_INPUT_DATA;
	}

	uint8_t *num_buf = malloc(nbits / 8);
	if (SUCCESS != pke_rsa_prime_generate(nbits, num_buf, flags)) {
		ret = FAIL;
		goto cleanup;
	}
	mbedtls_mpi_read_binary_le(X, num_buf, nbits / 8);
	// t1 = rtos_time_get_current_system_time_ms();
	// printf("mbedtls_mpi_gen_prime_pke time:%d ms\n", t1 - t0);
cleanup:
	free(num_buf);
	return ret;
}

#if defined(MBEDTLS_GENPRIME)
int mbedtls_rsa_gen_key(mbedtls_rsa_context *ctx,
                        int (*f_rng)(void *, unsigned char *, size_t),
                        void *p_rng,
                        unsigned int nbits, int exponent)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    mbedtls_mpi H, G, L;
    int prime_quality = 0;

    /*
     * If the modulus is 1024 bit long or shorter, then the security strength of
     * the RSA algorithm is less than or equal to 80 bits and therefore an error
     * rate of 2^-80 is sufficient.
     */
    if (nbits > 1024) {
        prime_quality = MBEDTLS_MPI_GEN_PRIME_FLAG_LOW_ERR;
    }

    mbedtls_mpi_init(&H);
    mbedtls_mpi_init(&G);
    mbedtls_mpi_init(&L);

    if (nbits < 128 || exponent < 3 || nbits % 2 != 0) {
        ret = MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
        goto cleanup;
    }

    /*
     * find primes P and Q with Q < P so that:
     * 1.  |P-Q| > 2^( nbits / 2 - 100 )
     * 2.  GCD( E, (P-1)*(Q-1) ) == 1
     * 3.  E^-1 mod LCM(P-1, Q-1) > 2^( nbits / 2 )
     */
    MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&ctx->E, exponent));

    do {
        MBEDTLS_MPI_CHK(mbedtls_mpi_gen_prime_pke(&ctx->P, nbits >> 1,
                                              prime_quality, f_rng, p_rng));

        MBEDTLS_MPI_CHK(mbedtls_mpi_gen_prime_pke(&ctx->Q, nbits >> 1,
                                              prime_quality, f_rng, p_rng));

        /* make sure the difference between p and q is not too small (FIPS 186-4 §B.3.3 step 5.4) */
        MBEDTLS_MPI_CHK(mbedtls_mpi_sub_mpi(&H, &ctx->P, &ctx->Q));
        if (mbedtls_mpi_bitlen(&H) <= ((nbits >= 200) ? ((nbits >> 1) - 99) : 0)) {
            continue;
        }

        /* not required by any standards, but some users rely on the fact that P > Q */
        if (H.s < 0) {
            mbedtls_mpi_swap(&ctx->P, &ctx->Q);
        }

        /* Temporarily replace P,Q by P-1, Q-1 */
        MBEDTLS_MPI_CHK(mbedtls_mpi_sub_int(&ctx->P, &ctx->P, 1));
        MBEDTLS_MPI_CHK(mbedtls_mpi_sub_int(&ctx->Q, &ctx->Q, 1));
        MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(&H, &ctx->P, &ctx->Q));

        /* check GCD( E, (P-1)*(Q-1) ) == 1 (FIPS 186-4 §B.3.1 criterion 2(a)) */
        MBEDTLS_MPI_CHK(mbedtls_mpi_gcd(&G, &ctx->E, &H));
        if (mbedtls_mpi_cmp_int(&G, 1) != 0) {
            continue;
        }

        /* compute smallest possible D = E^-1 mod LCM(P-1, Q-1) (FIPS 186-4 §B.3.1 criterion 3(b)) */
        MBEDTLS_MPI_CHK(mbedtls_mpi_gcd(&G, &ctx->P, &ctx->Q));
        MBEDTLS_MPI_CHK(mbedtls_mpi_div_mpi(&L, NULL, &H, &G));
        MBEDTLS_MPI_CHK(mbedtls_mpi_inv_mod(&ctx->D, &ctx->E, &L));

        if (mbedtls_mpi_bitlen(&ctx->D) <= ((nbits + 1) / 2)) {      // (FIPS 186-4 §B.3.1 criterion 3(a))
            continue;
        }

        break;
    } while (1);

    /* Restore P,Q */
    MBEDTLS_MPI_CHK(mbedtls_mpi_add_int(&ctx->P,  &ctx->P, 1));
    MBEDTLS_MPI_CHK(mbedtls_mpi_add_int(&ctx->Q,  &ctx->Q, 1));

    MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(&ctx->N, &ctx->P, &ctx->Q));

    ctx->len = mbedtls_mpi_size(&ctx->N);

#if !defined(MBEDTLS_RSA_NO_CRT)
    /*
     * DP = D mod (P - 1)
     * DQ = D mod (Q - 1)
     * QP = Q^-1 mod P
     */
    MBEDTLS_MPI_CHK(mbedtls_rsa_deduce_crt(&ctx->P, &ctx->Q, &ctx->D,
                                           &ctx->DP, &ctx->DQ, &ctx->QP));
#endif /* MBEDTLS_RSA_NO_CRT */

    /* Double-check */
    MBEDTLS_MPI_CHK(mbedtls_rsa_check_privkey(ctx));

cleanup:

    mbedtls_mpi_free(&H);
    mbedtls_mpi_free(&G);
    mbedtls_mpi_free(&L);

    if (ret != 0) {
        mbedtls_rsa_free(ctx);

        if ((-ret & ~0x7f) == 0) {
            ret = MBEDTLS_ERROR_ADD(MBEDTLS_ERR_RSA_KEY_GEN_FAILED, ret);
        }
        return ret;
    }

    return 0;
}
#endif /* MBEDTLS_GENPRIME */

/* Only used for RSA encryption and decryption calculations in this file */
static int mbedtls_mpi_exp_mod_prime_n(mbedtls_mpi *X, const mbedtls_mpi *A,
		const mbedtls_mpi *E, const mbedtls_mpi *N, mbedtls_mpi *prec_RR)
{
	UNUSED(prec_RR);
	size_t N_size = mbedtls_mpi_size(N);
	uint8_t *X_mpi_le = malloc(N_size);

	pke_rsa_exp_mod(X_mpi_le, N_size, (uint8_t *)A->p, mbedtls_mpi_size(A), (uint8_t *)E->p, mbedtls_mpi_size(E), (uint8_t *)N->p, N_size);
	mbedtls_mpi_read_binary_le(X, X_mpi_le, N_size);

	free(X_mpi_le);
	return SUCCESS;
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
