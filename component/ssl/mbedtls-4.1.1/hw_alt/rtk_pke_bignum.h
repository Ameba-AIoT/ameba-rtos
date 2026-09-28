/**
 * *****************************************************************************
 * @file    rtk_pke_bignum.h
 * @brief   PKE-accelerated modular exponentiation for the builtin bignum layer.
 * *****************************************************************************
 * @attention
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 * *****************************************************************************
 */

#ifndef RTK_PKE_BIGNUM_H
#define RTK_PKE_BIGNUM_H

#include "mbedtls/private/bignum.h"

/**
 * @brief  Compute X = A^E mod N on the PKE engine.
 * @retval 0                                 Done in hardware.
 * @retval MBEDTLS_ERR_MPI_NOT_ACCEPTABLE    Operands out of the engine's range;
 *                                           the caller must run the software path.
 * @retval other MBEDTLS_ERR_MPI_*           Hard failure.
 */
int rtk_pke_mpi_exp_mod(mbedtls_mpi *X, const mbedtls_mpi *A,
						const mbedtls_mpi *E, const mbedtls_mpi *N);

#endif /* RTK_PKE_BIGNUM_H */
