/**
 * *****************************************************************************
 * @file    rtk_psa_mac.h
 * @brief   PSA transparent driver entry points for the Realtek HMAC engine.
 * *****************************************************************************
 * @attention
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 * *****************************************************************************
 */

#ifndef RTK_PSA_MAC_H
#define RTK_PSA_MAC_H

#include "psa/crypto.h"
#include "ameba_soc.h"

psa_status_t rtk_transparent_mac_compute(
	const psa_key_attributes_t *attributes,
	const uint8_t *key_buffer, size_t key_buffer_size,
	psa_algorithm_t alg,
	const uint8_t *input, size_t input_length,
	uint8_t *mac, size_t mac_size, size_t *mac_length);

#endif /* RTK_PSA_MAC_H */
