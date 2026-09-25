/**
 * *****************************************************************************
 * @file    rtk_psa_ecdh.h
 * @brief   PSA transparent driver entry point for the Realtek PKE ECDH engine.
 * *****************************************************************************
 * @attention
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 * *****************************************************************************
 */

#ifndef RTK_PSA_ECDH_H
#define RTK_PSA_ECDH_H

#include "psa/crypto.h"

psa_status_t rtk_transparent_key_agreement(
	const psa_key_attributes_t *attributes,
	const uint8_t *key_buffer, size_t key_buffer_size,
	psa_algorithm_t alg,
	const uint8_t *peer_key, size_t peer_key_length,
	uint8_t *shared_secret, size_t shared_secret_size,
	size_t *shared_secret_length);

#endif /* RTK_PSA_ECDH_H */
