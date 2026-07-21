/**
 * \file bignum.h
 *
 * \brief Multi-precision integer library
 */
/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 *
 *  This file is provided under the Apache License 2.0, or the
 *  GNU General Public License v2.0 or later.
 *
 *  **********
 *  Apache License 2.0:
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  **********
 *
 *  **********
 *  GNU General Public License v2.0 or later:
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *  **********
 */
#ifndef ROM_BIGNUM_H
#define ROM_BIGNUM_H

#include <stddef.h>
#include <stdint.h>

#define ERR_MPI_FILE_IO_ERROR                     -0x0002  /**< An error occurred while reading from or writing to a file. */
#define ERR_MPI_BAD_INPUT_DATA                    -0x0004  /**< Bad input parameters to function. */
#define ERR_MPI_INVALID_CHARACTER                 -0x0006  /**< There is an invalid character in the digit string. */
#define ERR_MPI_BUFFER_TOO_SMALL                  -0x0008  /**< The buffer is too small to write to. */
#define ERR_MPI_NEGATIVE_VALUE                    -0x000A  /**< The input arguments are negative or result in illegal output. */
#define ERR_MPI_DIVISION_BY_ZERO                  -0x000C  /**< The input argument for division is zero, which is not allowed. */
#define ERR_MPI_NOT_ACCEPTABLE                    -0x000E  /**< The input arguments are not acceptable. */
#define ERR_MPI_ALLOC_FAILED                      -0x0010  /**< Memory allocation failed. */

#define MPI_CHK(f)       \
    do                           \
    {                            \
        if( ( ret = (f) ) != 0 ) \
            goto cleanup;        \
    } while( 0 )

/*
 * Maximum size MPIs are allowed to grow to in number of limbs.
 */
#define MPI_MAX_LIMBS                             10000

/*
 * Maximum window size used for modular exponentiation. Default: 6
 * Minimum value: 1. Maximum value: 6.
 *
 * Result is an array of ( 2 ** MPI_WINDOW_SIZE ) MPIs used
 * for the sliding window calculation. (So 64 by default)
 *
 * Reduction in size, reduces speed.
 */
#define MPI_WINDOW_SIZE                           6        /**< Maximum window size used. */

/*
 * Maximum size of MPIs allowed in bits and bytes for user-MPIs.
 * ( Default: 512 bytes => 4096 bits, Maximum tested: 2048 bytes => 16384 bits )
 *
 * Note: Calculations can temporarily result in larger MPIs. So the number
 * of limbs required (MPI_MAX_LIMBS) is higher.
 */
#define MPI_MAX_SIZE                              1024     /**< Maximum number of bytes for usable MPIs. */
#define MPI_MAX_BITS                              ( 8 * MPI_MAX_SIZE )    /**< Maximum number of bits for usable MPIs. */

/*
 * Define the base integer type, architecture-wise.
 */

/* Default to 32-bit compilation */
typedef  int32_t mpi_sint;
typedef uint32_t mpi_uint;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          MPI structure
 */
typedef struct mpi {
	int s;              /*!<  Sign: -1 if the mpi is negative, 1 otherwise */
	size_t n;           /*!<  total # of limbs  */
	mpi_uint *p;          /*!<  pointer to limbs  */
} mpi;

/**
 * \brief           Initialize an MPI context.
 *
 *                  This makes the MPI ready to be set or freed,
 *                  but does not define a value for the MPI.
 *
 * \param X         The MPI context to initialize. This must not be \c NULL.
 */
void wifi_rom_mpi_init(mpi *X);

/**
 * \brief          This function frees the components of an MPI context.
 *
 * \param X        The MPI context to be cleared. This may be \c NULL,
 *                 in which case this function is a no-op. If it is
 *                 not \c NULL, it must point to an initialized MPI.
 */
void wifi_rom_mpi_free(mpi *X);

/**
 * \brief          Enlarge an MPI to the specified number of limbs.
 *
 * \note           This function does nothing if the MPI is
 *                 already large enough.
 *
 * \param X        The MPI to grow. It must be initialized.
 * \param nblimbs  The target number of limbs.
 *
 * \return         \c 0 if successful.
 * \return         #ERR_MPI_ALLOC_FAILED if memory allocation failed.
 * \return         Another negative error code on other kinds of failure.
 */
int wifi_rom_mpi_grow(mpi *X, size_t nblimbs);

/**
 * \brief          This function resizes an MPI downwards, keeping at least the
 *                 specified number of limbs.
 *
 *                 If \c X is smaller than \c nblimbs, it is resized up
 *                 instead.
 *
 * \param X        The MPI to shrink. This must point to an initialized MPI.
 * \param nblimbs  The minimum number of limbs to keep.
 *
 * \return         \c 0 if successful.
 * \return         #ERR_MPI_ALLOC_FAILED if memory allocation failed
 *                 (this can only happen when resizing up).
 * \return         Another negative error code on other kinds of failure.
 */
int wifi_rom_mpi_shrink(mpi *X, size_t nblimbs);

/**
 * \brief          Make a copy of an MPI.
 *
 * \param X        The destination MPI. This must point to an initialized MPI.
 * \param Y        The source MPI. This must point to an initialized MPI.
 *
 * \note           The limb-buffer in the destination MPI is enlarged
 *                 if necessary to hold the value in the source MPI.
 *
 * \return         \c 0 if successful.
 * \return         #ERR_MPI_ALLOC_FAILED if memory allocation failed.
 * \return         Another negative error code on other kinds of failure.
 */
int wifi_rom_mpi_copy(mpi *X, const mpi *Y);

/**
 * \brief          Perform a safe conditional copy of MPI which doesn't
 *                 reveal whether the condition was true or not.
 *
 * \param X        The MPI to conditionally assign to. This must point
 *                 to an initialized MPI.
 * \param Y        The MPI to be assigned from. This must point to an
 *                 initialized MPI.
 * \param assign   The condition deciding whether to perform the
 *                 assignment or not. Possible values:
 *                 * \c 1: Perform the assignment `X = Y`.
 *                 * \c 0: Keep the original value of \p X.
 *
 * \note           This function is equivalent to
 *                      `if( assign ) wifi_rom_mpi_copy( X, Y );`
 *                 except that it avoids leaking any information about whether
 *                 the assignment was done or not (the above code may leak
 *                 information through branch prediction and/or memory access
 *                 patterns analysis).
 *
 * \return         \c 0 if successful.
 * \return         #ERR_MPI_ALLOC_FAILED if memory allocation failed.
 * \return         Another negative error code on other kinds of failure.
 */
int wifi_rom_mpi_safe_cond_assign(mpi *X, const mpi *Y, unsigned char assign);

/**
 * \brief          Store integer value in MPI.
 *
 * \param X        The MPI to set. This must be initialized.
 * \param z        The value to use.
 *
 * \return         \c 0 if successful.
 * \return         #ERR_MPI_ALLOC_FAILED if memory allocation failed.
 * \return         Another negative error code on other kinds of failure.
 */
int wifi_rom_mpi_lset(mpi *X, mpi_sint z);

/**
 * \brief          Get a specific bit from an MPI.
 *
 * \param X        The MPI to query. This must be initialized.
 * \param pos      Zero-based index of the bit to query.
 *
 * \return         \c 0 or \c 1 on success, depending on whether bit \c pos
 *                 of \c X is unset or set.
 * \return         A negative error code on failure.
 */
int wifi_rom_mpi_get_bit(const mpi *X, size_t pos);

/**
 * \brief          Modify a specific bit in an MPI.
 *
 * \note           This function will grow the target MPI if necessary to set a
 *                 bit to \c 1 in a not yet existing limb. It will not grow if
 *                 the bit should be set to \c 0.
 *
 * \param X        The MPI to modify. This must be initialized.
 * \param pos      Zero-based index of the bit to modify.
 * \param val      The desired value of bit \c pos: \c 0 or \c 1.
 *
 * \return         \c 0 if successful.
 * \return         #ERR_MPI_ALLOC_FAILED if memory allocation failed.
 * \return         Another negative error code on other kinds of failure.
 */
int wifi_rom_mpi_set_bit(mpi *X, size_t pos, unsigned char val);

/**
 * \brief          Return the number of bits of value \c 0 before the
 *                 least significant bit of value \c 1.
 *
 * \note           This is the same as the zero-based index of
 *                 the least significant bit of value \c 1.
 *
 * \param X        The MPI to query.
 *
 * \return         The number of bits of value \c 0 before the least significant
 *                 bit of value \c 1 in \p X.
 */
size_t wifi_rom_mpi_lsb(const mpi *X);

/**
 * \brief          Return the number of bits up to and including the most
 *                 significant bit of value \c 1.
 *
 * * \note         This is same as the one-based index of the most
 *                 significant bit of value \c 1.
 *
 * \param X        The MPI to query. This must point to an initialized MPI.
 *
 * \return         The number of bits up to and including the most
 *                 significant bit of value \c 1.
 */
size_t wifi_rom_mpi_bitlen(const mpi *X);

/**
 * \brief          Return the total size of an MPI value in bytes.
 *
 * \param X        The MPI to use. This must point to an initialized MPI.
 *
 * \note           The value returned by this function may be less than
 *                 the number of bytes used to store \p X internally.
 *                 This happens if and only if there are trailing bytes
 *                 of value zero.
 *
 * \return         The least number of bytes capable of storing
 *                 the absolute value of \p X.
 */
size_t wifi_rom_mpi_size(const mpi *X);

/**
 * \brief          Import an MPI from unsigned big endian binary data.
 *
 * \param X        The destination MPI. This must point to an initialized MPI.
 * \param buf      The input buffer. This must be a readable buffer of length
 *                 \p buflen Bytes.
 * \param buflen   The length of the input buffer \p p in Bytes.
 *
 * \return         \c 0 if successful.
 * \return         #ERR_MPI_ALLOC_FAILED if memory allocation failed.
 * \return         Another negative error code on different kinds of failure.
 */
int wifi_rom_mpi_read_binary(mpi *X, const unsigned char *buf,
							 size_t buflen);

/**
 * \brief          Export an MPI into unsigned big endian binary data
 *                 of fixed size.
 *
 * \param X        The source MPI. This must point to an initialized MPI.
 * \param buf      The output buffer. This must be a writable buffer of length
 *                 \p buflen Bytes.
 * \param buflen   The size of the output buffer \p buf in Bytes.
 *
 * \return         \c 0 if successful.
 * \return         #ERR_MPI_BUFFER_TOO_SMALL if \p buf isn't
 *                 large enough to hold the value of \p X.
 * \return         Another negative error code on different kinds of failure.
 */
int wifi_rom_mpi_write_binary(const mpi *X, unsigned char *buf,
							  size_t buflen);

/**
 * \brief          Perform a left-shift on an MPI: X <<= count
 *
 * \param X        The MPI to shift. This must point to an initialized MPI.
 * \param count    The number of bits to shift by.
 *
 * \return         \c 0 if successful.
 * \return         #ERR_MPI_ALLOC_FAILED if a memory allocation failed.
 * \return         Another negative error code on different kinds of failure.
 */
int wifi_rom_mpi_shift_l(mpi *X, size_t count);

/**
 * \brief          Perform a right-shift on an MPI: X >>= count
 *
 * \param X        The MPI to shift. This must point to an initialized MPI.
 * \param count    The number of bits to shift by.
 *
 * \return         \c 0 if successful.
 * \return         #ERR_MPI_ALLOC_FAILED if a memory allocation failed.
 * \return         Another negative error code on different kinds of failure.
 */
int wifi_rom_mpi_shift_r(mpi *X, size_t count);

/**
 * \brief          Compare the absolute values of two MPIs.
 *
 * \param X        The left-hand MPI. This must point to an initialized MPI.
 * \param Y        The right-hand MPI. This must point to an initialized MPI.
 *
 * \return         \c 1 if `|X|` is greater than `|Y|`.
 * \return         \c -1 if `|X|` is lesser than `|Y|`.
 * \return         \c 0 if `|X|` is equal to `|Y|`.
 */
int wifi_rom_mpi_cmp_abs(const mpi *X, const mpi *Y);

/**
 * \brief          Compare two MPIs.
 *
 * \param X        The left-hand MPI. This must point to an initialized MPI.
 * \param Y        The right-hand MPI. This must point to an initialized MPI.
 *
 * \return         \c 1 if \p X is greater than \p Y.
 * \return         \c -1 if \p X is lesser than \p Y.
 * \return         \c 0 if \p X is equal to \p Y.
 */
int wifi_rom_mpi_cmp_mpi(const mpi *X, const mpi *Y);

/**
 * \brief          Compare an MPI with an integer.
 *
 * \param X        The left-hand MPI. This must point to an initialized MPI.
 * \param z        The integer value to compare \p X to.
 *
 * \return         \c 1 if \p X is greater than \p z.
 * \return         \c -1 if \p X is lesser than \p z.
 * \return         \c 0 if \p X is equal to \p z.
 */
int wifi_rom_mpi_cmp_int(const mpi *X, mpi_sint z);

/**
 * \brief          Perform an unsigned addition of MPIs: X = |A| + |B|
 *
 * \param X        The destination MPI. This must point to an initialized MPI.
 * \param A        The first summand. This must point to an initialized MPI.
 * \param B        The second summand. This must point to an initialized MPI.
 *
 * \return         \c 0 if successful.
 * \return         #ERR_MPI_ALLOC_FAILED if a memory allocation failed.
 * \return         Another negative error code on different kinds of failure.
 */
int wifi_rom_mpi_add_abs(mpi *X, const mpi *A,
						 const mpi *B);

/**
 * \brief          Perform an unsigned subtraction of MPIs: X = |A| - |B|
 *
 * \param X        The destination MPI. This must point to an initialized MPI.
 * \param A        The minuend. This must point to an initialized MPI.
 * \param B        The subtrahend. This must point to an initialized MPI.
 *
 * \return         \c 0 if successful.
 * \return         #ERR_MPI_NEGATIVE_VALUE if \p B is greater than \p A.
 * \return         Another negative error code on different kinds of failure.
 *
 */
int wifi_rom_mpi_sub_abs(mpi *X, const mpi *A,
						 const mpi *B);

/**
 * \brief          Perform a signed addition of MPIs: X = A + B
 *
 * \param X        The destination MPI. This must point to an initialized MPI.
 * \param A        The first summand. This must point to an initialized MPI.
 * \param B        The second summand. This must point to an initialized MPI.
 *
 * \return         \c 0 if successful.
 * \return         #ERR_MPI_ALLOC_FAILED if a memory allocation failed.
 * \return         Another negative error code on different kinds of failure.
 */
int wifi_rom_mpi_add_mpi(mpi *X, const mpi *A,
						 const mpi *B);

/**
 * \brief          Perform a signed subtraction of MPIs: X = A - B
 *
 * \param X        The destination MPI. This must point to an initialized MPI.
 * \param A        The minuend. This must point to an initialized MPI.
 * \param B        The subtrahend. This must point to an initialized MPI.
 *
 * \return         \c 0 if successful.
 * \return         #ERR_MPI_ALLOC_FAILED if a memory allocation failed.
 * \return         Another negative error code on different kinds of failure.
 */
int wifi_rom_mpi_sub_mpi(mpi *X, const mpi *A,
						 const mpi *B);

/**
 * \brief          Perform a signed addition of an MPI and an integer: X = A + b
 *
 * \param X        The destination MPI. This must point to an initialized MPI.
 * \param A        The first summand. This must point to an initialized MPI.
 * \param b        The second summand.
 *
 * \return         \c 0 if successful.
 * \return         #ERR_MPI_ALLOC_FAILED if a memory allocation failed.
 * \return         Another negative error code on different kinds of failure.
 */
int wifi_rom_mpi_add_int(mpi *X, const mpi *A,
						 mpi_sint b);

/**
 * \brief          Perform a signed subtraction of an MPI and an integer:
 *                 X = A - b
 *
 * \param X        The destination MPI. This must point to an initialized MPI.
 * \param A        The minuend. This must point to an initialized MPI.
 * \param b        The subtrahend.
 *
 * \return         \c 0 if successful.
 * \return         #ERR_MPI_ALLOC_FAILED if a memory allocation failed.
 * \return         Another negative error code on different kinds of failure.
 */
int wifi_rom_mpi_sub_int(mpi *X, const mpi *A,
						 mpi_sint b);

/**
 * \brief          Perform a multiplication of two MPIs: X = A * B
 *
 * \param X        The destination MPI. This must point to an initialized MPI.
 * \param A        The first factor. This must point to an initialized MPI.
 * \param B        The second factor. This must point to an initialized MPI.
 *
 * \return         \c 0 if successful.
 * \return         #ERR_MPI_ALLOC_FAILED if a memory allocation failed.
 * \return         Another negative error code on different kinds of failure.
 *
 */
int wifi_rom_mpi_mul_mpi(mpi *X, const mpi *A,
						 const mpi *B);

/**
 * \brief          Perform a multiplication of an MPI with an unsigned integer:
 *                 X = A * b
 *
 * \param X        The destination MPI. This must point to an initialized MPI.
 * \param A        The first factor. This must point to an initialized MPI.
 * \param b        The second factor.
 *
 * \return         \c 0 if successful.
 * \return         #ERR_MPI_ALLOC_FAILED if a memory allocation failed.
 * \return         Another negative error code on different kinds of failure.
 *
 */
int wifi_rom_mpi_mul_int(mpi *X, const mpi *A,
						 mpi_uint b);

/**
 * \brief          Perform a division with remainder of two MPIs:
 *                 A = Q * B + R
 *
 * \param Q        The destination MPI for the quotient.
 *                 This may be \c NULL if the value of the
 *                 quotient is not needed.
 * \param R        The destination MPI for the remainder value.
 *                 This may be \c NULL if the value of the
 *                 remainder is not needed.
 * \param A        The dividend. This must point to an initialized MPi.
 * \param B        The divisor. This must point to an initialized MPI.
 *
 * \return         \c 0 if successful.
 * \return         #ERR_MPI_ALLOC_FAILED if memory allocation failed.
 * \return         #ERR_MPI_DIVISION_BY_ZERO if \p B equals zero.
 * \return         Another negative error code on different kinds of failure.
 */
int wifi_rom_mpi_div_mpi(mpi *Q, mpi *R, const mpi *A,
						 const mpi *B);

/**
 * \brief          Perform a modular reduction. R = A mod B
 *
 * \param R        The destination MPI for the residue value.
 *                 This must point to an initialized MPI.
 * \param A        The MPI to compute the residue of.
 *                 This must point to an initialized MPI.
 * \param B        The base of the modular reduction.
 *                 This must point to an initialized MPI.
 *
 * \return         \c 0 if successful.
 * \return         #ERR_MPI_ALLOC_FAILED if a memory allocation failed.
 * \return         #ERR_MPI_DIVISION_BY_ZERO if \p B equals zero.
 * \return         #ERR_MPI_NEGATIVE_VALUE if \p B is negative.
 * \return         Another negative error code on different kinds of failure.
 *
 */
int wifi_rom_mpi_mod_mpi(mpi *R, const mpi *A,
						 const mpi *B);

/**
 * \brief          Perform a sliding-window exponentiation: X = A^E mod N
 *
 * \param X        The destination MPI. This must point to an initialized MPI.
 * \param A        The base of the exponentiation.
 *                 This must point to an initialized MPI.
 * \param E        The exponent MPI. This must point to an initialized MPI.
 * \param N        The base for the modular reduction. This must point to an
 *                 initialized MPI.
 * \param _RR      A helper MPI depending solely on \p N which can be used to
 *                 speed-up multiple modular exponentiations for the same value
 *                 of \p N. This may be \c NULL. If it is not \c NULL, it must
 *                 point to an initialized MPI. If it hasn't been used after
 *                 the call to wifi_rom_mpi_init(), this function will compute
 *                 the helper value and store it in \p _RR for reuse on
 *                 subsequent calls to this function. Otherwise, the function
 *                 will assume that \p _RR holds the helper value set by a
 *                 previous call to wifi_rom_mpi_exp_mod(), and reuse it.
 *
 * \return         \c 0 if successful.
 * \return         #ERR_MPI_ALLOC_FAILED if a memory allocation failed.
 * \return         #ERR_MPI_BAD_INPUT_DATA if \c N is negative or
 *                 even, or if \c E is negative.
 * \return         Another negative error code on different kinds of failures.
 *
 */
int wifi_rom_mpi_exp_mod(mpi *X, const mpi *A,
						 const mpi *E, const mpi *N,
						 mpi *_RR);

/**
 * \brief          Fill an MPI with a number of random bytes.
 *
 * \param X        The destination MPI. This must point to an initialized MPI.
 * \param size     The number of random bytes to generate.
 * \param f_rng    The RNG function to use. This must not be \c NULL.
 * \param p_rng    The RNG parameter to be passed to \p f_rng. This may be
 *                 \c NULL if \p f_rng doesn't need a context argument.
 *
 * \return         \c 0 if successful.
 * \return         #ERR_MPI_ALLOC_FAILED if a memory allocation failed.
 * \return         Another negative error code on failure.
 *
 * \note           The bytes obtained from the RNG are interpreted
 *                 as a big-endian representation of an MPI; this can
 *                 be relevant in applications like deterministic ECDSA.
 */
int wifi_rom_mpi_fill_random(mpi *X, size_t size,
							 int (*f_rng)(void *, unsigned char *, size_t),
							 void *p_rng);

/**
 * \brief          Compute the greatest common divisor: G = gcd(A, B)
 *
 * \param G        The destination MPI. This must point to an initialized MPI.
 * \param A        The first operand. This must point to an initialized MPI.
 * \param B        The second operand. This must point to an initialized MPI.
 *
 * \return         \c 0 if successful.
 * \return         #ERR_MPI_ALLOC_FAILED if a memory allocation failed.
 * \return         Another negative error code on different kinds of failure.
 */
int wifi_rom_mpi_gcd(mpi *G, const mpi *A,
					 const mpi *B);

/**
 * \brief          Compute the modular inverse: X = A^-1 mod N
 *
 * \param X        The destination MPI. This must point to an initialized MPI.
 * \param A        The MPI to calculate the modular inverse of. This must point
 *                 to an initialized MPI.
 * \param N        The base of the modular inversion. This must point to an
 *                 initialized MPI.
 *
 * \return         \c 0 if successful.
 * \return         #ERR_MPI_ALLOC_FAILED if a memory allocation failed.
 * \return         #ERR_MPI_BAD_INPUT_DATA if \p N is less than
 *                 or equal to one.
 * \return         #ERR_MPI_NOT_ACCEPTABLE if \p has no modular inverse
 *                 with respect to \p N.
 */
int wifi_rom_mpi_inv_mod(mpi *X, const mpi *A,
						 const mpi *N);

#ifdef __cplusplus
}
#endif

#endif /* rom_bignum.h */
