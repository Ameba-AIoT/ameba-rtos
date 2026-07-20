/*
 * Diffie-Hellman groups
 * Copyright (c) 2007, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/includes.h"
#include "utils/common.h"
#include "crypto.h"
#include "random.h"
#include "dh_groups.h"

//#ifdef CONFIG_WPS


/**
 * dh_init - Initialize Diffie-Hellman handshake
 * @dh: Selected Diffie-Hellman group
 * @priv: Pointer for returning Diffie-Hellman private key
 * Returns: Diffie-Hellman public value
 */
/* 5, dh_group5_generator, sizeof(dh_group5_generator), dh_group5_prime, sizeof(dh_group5_prime) */
// dh = &dh_groups[0], priv = &wps->dh_privkey
struct wpabuf *dh_init(const struct dh_group *dh, struct wpabuf **priv)
{
	struct wpabuf *pv;
	size_t pv_len;
	//int i;

	if (dh == NULL) {
		wpa_printf(MSG_WARNING, "dh == NULL");
		return NULL;
	}
	wpabuf_free(*priv);
	// prime_len = sizeof(dh_group5_prime)
	*priv = wpabuf_alloc(dh->prime_len);
	if (*priv == NULL) {
		wpa_printf(MSG_WARNING, "*priv == NULL");
		return NULL;
	}
	if (random_get_bytes(wpabuf_put(*priv, dh->prime_len), dh->prime_len)) {
		wpabuf_free(*priv);
		*priv = NULL;
		wpa_printf(MSG_WARNING, "random_get_bytes fail");
		return NULL;
	}

	//  dh->prime = dh_group5_prime, dh->prime_len = sizeof(dh_group5_prime)
	if (/*os_memcmp*/memcmp(wpabuf_head(*priv), dh->prime, dh->prime_len) > 0) {
		/* Make sure private value is smaller than prime */
		*(wpabuf_mhead_u8(*priv)) = 0;
	}

	//wpa_hexdump_buf_key(MSG_DEBUG, "DH: private value", *priv);

	pv_len = dh->prime_len;

	pv = wpabuf_alloc(pv_len);

	if (pv == NULL) {
		wpa_printf(MSG_WARNING, " pv == NULL");
		return NULL;
	}

	// dh_group5_generator = {0x02}, dh->generator_len = 1, ?, 192, dh_group5_prime[192] = {...}, 192, ?, ?
	if (crypto_mod_exp(dh->generator, dh->generator_len,
					   wpabuf_head(*priv), wpabuf_len(*priv),
					   dh->prime, dh->prime_len, wpabuf_mhead(pv),
					   &pv_len) < 0) {
		wpabuf_free(pv);
		wpa_printf(MSG_INFO, "DH: crypto_mod_exp failed");
		return NULL;
	}
	wpabuf_put(pv, pv_len);
	wpa_hexdump_buf(MSG_DEBUG, "DH: public value", pv);

	return pv;
}


/**
 * dh_derive_shared - Derive shared Diffie-Hellman key
 * @peer_public: Diffie-Hellman public value from peer
 * @own_private: Diffie-Hellman private key from dh_init()
 * @dh: Selected Diffie-Hellman group
 * Returns: Diffie-Hellman shared key
 */
struct wpabuf *dh_derive_shared(const struct wpabuf *peer_public,
								const struct wpabuf *own_private,
								const struct dh_group *dh)
{
	struct wpabuf *shared;
	size_t shared_len;
	//int i;

	wpa_printf(MSG_DEBUG, "===>dh_derive_shared()");

	if (dh == NULL || peer_public == NULL || own_private == NULL) {
		wpa_printf(MSG_WARNING, "dh || peer_public || own_private == NULL ");
		wpa_printf(MSG_WARNING, "<===dh_derive_shared(),return NULL");
		return NULL;
	}
	shared_len = dh->prime_len;
	shared = wpabuf_alloc(shared_len);
	if (shared == NULL) {
		wpa_printf(MSG_WARNING, "<===dh_derive_shared(),return NULL");
		return NULL;
	}
	if (crypto_mod_exp(wpabuf_head(peer_public), wpabuf_len(peer_public),
					   wpabuf_head(own_private), wpabuf_len(own_private),
					   dh->prime, dh->prime_len,
					   wpabuf_mhead(shared), &shared_len) < 0) {
		wpabuf_free(shared);
		wpa_printf(MSG_WARNING, "DH: crypto_mod_exp failed");
		wpa_printf(MSG_WARNING, "<===dh_derive_shared(),return NULL");
		return NULL;
	}
	wpabuf_put(shared, shared_len);
	wpa_hexdump_buf_key(MSG_DEBUG, "DH: shared key", shared);
	return shared;
}
//#endif
