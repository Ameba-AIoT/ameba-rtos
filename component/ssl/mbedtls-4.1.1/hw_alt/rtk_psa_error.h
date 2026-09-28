/**
 * *****************************************************************************
 * @file    rtk_psa_error.h
 * @brief   Map Realtek crypto engine status codes to psa_status_t.
 *          4.x replacement for the mbedtls_alt_helper.h of the 3.6.5 port,
 *          which mapped to MBEDTLS_ERR_* instead.
 * *****************************************************************************
 * @attention
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 * *****************************************************************************
 */

#ifndef RTK_PSA_ERROR_H
#define RTK_PSA_ERROR_H

#include "psa/crypto.h"
#include "ameba_soc.h"

/*
 * Compatibility fallbacks for error codes present in RTL8720F/RLE1509 but
 * absent in amebagreen2 (which stops at _ERRNO_CRYPTO_SLAVE_ERROR = -22).
 * These values are never produced by amebagreen2 HAL calls; the corresponding
 * switch cases are therefore dead code on that platform and carry no runtime
 * overhead. They exist solely to keep this header self-contained.
 */
#ifndef _ERRNO_CRYPTO_AAD_LENGTH_OutRange
#define _ERRNO_CRYPTO_AAD_LENGTH_OutRange   -23
#endif
#ifndef _ERRNO_CRYPTO_DMA_TIMEOUT
#define _ERRNO_CRYPTO_DMA_TIMEOUT           -24
#endif
#ifndef _ERRNO_CRYPTO_MODE_ERR
#define _ERRNO_CRYPTO_MODE_ERR              -25
#endif
#ifndef _ERRNO_CRYPTO_BUFFER_TOO_SMALL
#define _ERRNO_CRYPTO_BUFFER_TOO_SMALL      -26
#endif
#ifndef _ERRNO_CRYPTO_AUTH_FAILED
#define _ERRNO_CRYPTO_AUTH_FAILED           -27
#endif

static inline psa_status_t rtk_hw_to_psa_error(int hw_ret)
{
	if (hw_ret == 0) {
		return PSA_SUCCESS;
	}

	switch (hw_ret) {
	/* Caller passed something the engine cannot accept. */
	case _ERRNO_CRYPTO_NULL_POINTER:
	case _ERRNO_CRYPTO_ADDR_NOT_4Byte_Aligned:
	case _ERRNO_CRYPTO_MSG_OutRange:
	case _ERRNO_CRYPTO_MESSAGE_LEN_ERR:
	case _ERRNO_CRYPTO_MODE_ERR:
	case _ERRNO_CRYPTO_IV_OutRange:
	case _ERRNO_CRYPTO_TAG_OutRange:
	case _ERRNO_CRYPTO_AAD_LENGTH_OutRange:
	case _ERRNO_CRYPTO_KEY_LENGTH_ERR:
	case _ERRNO_CRYPTO_KEY_OutRange:
	case _ERRNO_CRYPTO_KEY_IV_LEN_DIFF:
	case _ERRNO_CRYPTO_AUTH_TYPE_NOT_MATCH:
	case _ERRNO_CRYPTO_CIPHER_TYPE_NOT_MATCH:
	case _ERRNO_CRYPTO_DESC_NUM_SET_OutRange:
	case _ERRNO_CRYPTO_BURST_NUM_SET_OutRange:
		return PSA_ERROR_INVALID_ARGUMENT;

	case _ERRNO_CRYPTO_BUFFER_TOO_SMALL:
		return PSA_ERROR_BUFFER_TOO_SMALL;

	/* Operation sequence violated, or the engine was never brought up. */
	case _ERRNO_CRYPTO_ENGINE_NOT_INIT:
	case _ERRNO_CRYPTO_HASH_FINAL_NO_UPDATE:
		return PSA_ERROR_BAD_STATE;

	/* Tag/authentication mismatch is a signature failure, not a HW fault. */
	case _ERRNO_CRYPTO_GCM_TAG_NOT_MATCH:
	case _ERRNO_CRYPTO_AUTH_FAILED:
		return PSA_ERROR_INVALID_SIGNATURE;

	case _ERRNO_CRYPTO_KEY_SECURE_ERR:
	case _ERRNO_CRYPTO_KEY_LOAD_ERR:
		return PSA_ERROR_NOT_PERMITTED;

	/* DMA / slave-mode faults and anything unclassified. */
	default:
		return PSA_ERROR_HARDWARE_FAILURE;
	}
}

#endif /* RTK_PSA_ERROR_H */
