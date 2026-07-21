/*
 * Crypto wrapper for internal crypto implementation - modexp
 * Copyright (c) 2006-2009, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/includes.h"

#include "utils/common.h"
#include "tls/rom/rom_wps_big_num.h"
#include "crypto.h"

//#ifdef CONFIG_WPS

// dh_group5_generator[1] = { 0x02 }, 1, ?, 192, dh_group5_prime[192], 192, ?, ?
int crypto_mod_exp(const u8 *base, size_t base_len,
				   const u8 *power, size_t power_len,
				   const u8 *modulus, size_t modulus_len,
				   u8 *result, size_t *result_len)
{
	struct bignum *bn_base, *bn_exp, *bn_modulus, *bn_result;
	int ret = -1;
	//int i;

	bn_base = bignum_init();
	bn_exp = bignum_init();
	bn_modulus = bignum_init();
	bn_result = bignum_init();

	if (bn_base == NULL || bn_exp == NULL || bn_modulus == NULL || bn_result == NULL) {
		wpa_printf(MSG_WARNING, "bn_base || bn_exp || bn_modulus || bn_result == NULL");
		goto error;
	}

	if (bignum_set_unsigned_bin(bn_base, base, base_len) < 0 ||
		bignum_set_unsigned_bin(bn_exp, power, power_len) < 0 ||
		bignum_set_unsigned_bin(bn_modulus, modulus, modulus_len) < 0) {
		wpa_printf(MSG_WARNING, "bignum_set_unsigned_bin fail!!!");
		goto error;
	}

	if (bignum_exptmod(bn_base, bn_exp, bn_modulus, bn_result) < 0) {
		wpa_printf(MSG_WARNING, "bignum_exptmod fail!!!");
		goto error;
	}

	ret = bignum_get_unsigned_bin(bn_result, result, result_len);
error:
	bignum_deinit(bn_base);
	bignum_deinit(bn_exp);
	bignum_deinit(bn_modulus);
	bignum_deinit(bn_result);

	return ret;
}
//#endif
