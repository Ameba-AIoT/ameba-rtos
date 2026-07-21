/*
 * Diffie-Hellman group 5 operations
 * Copyright (c) 2009, 2012, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/includes.h"
#include "utils/common.h"
#include "crypto/dh_group5.h"
#include "dh_groups.h"

//#ifdef CONFIG_WPS

// priv = &wps->dh_privkey, publ = &pubkey
void *dh5_init(struct wpabuf **priv, struct wpabuf **publ)
{
	*publ = dh_init(dh_groups_get(5), priv);
	if (*publ == NULL) {
		wpa_printf(MSG_WARNING, "*publ == NULL!!!");
		return NULL;
	}
	return (void *) 1;
}


void *dh5_init_fixed(const struct wpabuf *priv, const struct wpabuf *publ)
{
	/* To avoid gcc warnings */
	(void) priv;
	(void) publ;

	return (void *) 1;
}


struct wpabuf *dh5_derive_shared(void *ctx, const struct wpabuf *peer_public,
								 const struct wpabuf *own_private)
{
	/* To avoid gcc warnings */
	(void) ctx;

	return dh_derive_shared(peer_public, own_private, dh_groups_get(5));
}


void dh5_free(void *ctx)
{
	/* To avoid gcc warnings */
	(void) ctx;
#if 0
	if (ctx != NULL) {
		os_free(ctx, 0);
		ctx = NULL;
	}
#endif
}
//#endif
