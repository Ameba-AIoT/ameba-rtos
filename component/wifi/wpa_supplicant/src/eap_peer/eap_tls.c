/*
 * EAP peer method: EAP-TLS (RFC 2716)
 * Copyright (c) 2004-2008, 2012, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

//#if CONFIG_TLS

#include "utils/includes.h"
#include "utils/common.h"
#include "crypto/tls.h"
#include "eap_i.h"
#include "eap_tls_common.h"
#include "eap_config.h"


static void eap_tls_deinit(struct eap_sm *sm, void *priv);


struct eap_tls_data {
	struct eap_ssl_data ssl;
	u8 *key_data;
	u8 *session_id;
	size_t id_len;
	void *ssl_ctx;
	u8 eap_type;
};


static void *eap_tls_init(struct eap_sm *sm)
{
	struct eap_tls_data *data;
	struct eap_peer_config *config = eap_get_config(sm);
	/*
	if (config == NULL ||
	    ((sm->init_phase2 ? config->private_key2 : config->private_key)
	     == NULL &&
	     (sm->init_phase2 ? config->engine2 : config->engine) == 0)) {
		wpa_printf(MSG_INFO, "EAP-TLS: Private key not configured");
		return NULL;
	}
	*/

	data = os_zalloc(sizeof(*data));
	if (data == NULL) {
		return NULL;
	}

	data->ssl_ctx = sm->init_phase2 && sm->ssl_ctx2 ? sm->ssl_ctx2 :
					sm->ssl_ctx;

	if (eap_peer_tls_ssl_init(sm, &data->ssl, config, EAP_TYPE_TLS)) {
		wpa_printf(MSG_INFO, "EAP-TLS: Failed to initialize SSL.");
		eap_tls_deinit(sm, data);
		if (config->engine) {
			wpa_printf(MSG_DEBUG, "EAP-TLS: Requesting Smartcard "
					   "PIN");
			eap_sm_request_pin(sm);
			sm->ignore = TRUE;
		} else if (config->private_key && !config->private_key_passwd) {
			wpa_printf(MSG_DEBUG, "EAP-TLS: Requesting private "
					   "key passphrase");
			eap_sm_request_passphrase(sm);
			sm->ignore = TRUE;
		}
		return NULL;
	}

	data->eap_type = EAP_TYPE_TLS;

	return data;
}


#ifdef EAP_UNAUTH_TLS
static void *eap_unauth_tls_init(struct eap_sm *sm)
{
	struct eap_tls_data *data;
	struct eap_peer_config *config = eap_get_config(sm);

	data = os_zalloc(sizeof(*data));
	if (data == NULL) {
		return NULL;
	}

	data->ssl_ctx = sm->init_phase2 && sm->ssl_ctx2 ? sm->ssl_ctx2 :
					sm->ssl_ctx;

	if (eap_peer_tls_ssl_init(sm, &data->ssl, config,
							  EAP_UNAUTH_TLS_TYPE)) {
		wpa_printf(MSG_INFO, "EAP-TLS: Failed to initialize SSL.");
		eap_tls_deinit(sm, data);
		return NULL;
	}

	data->eap_type = EAP_UNAUTH_TLS_TYPE;

	return data;
}
#endif /* EAP_UNAUTH_TLS */


#ifdef CONFIG_HS20
static void *eap_wfa_unauth_tls_init(struct eap_sm *sm)
{
	struct eap_tls_data *data;
	struct eap_peer_config *config = eap_get_config(sm);

	data = os_zalloc(sizeof(*data));
	if (data == NULL) {
		return NULL;
	}

	data->ssl_ctx = sm->init_phase2 && sm->ssl_ctx2 ? sm->ssl_ctx2 :
					sm->ssl_ctx;

	if (eap_peer_tls_ssl_init(sm, &data->ssl, config,
							  EAP_WFA_UNAUTH_TLS_TYPE)) {
		wpa_printf(MSG_INFO, "EAP-TLS: Failed to initialize SSL.");
		eap_tls_deinit(sm, data);
		return NULL;
	}

	data->eap_type = EAP_WFA_UNAUTH_TLS_TYPE;

	return data;
}
#endif /* CONFIG_HS20 */


static void eap_tls_free_key(struct eap_tls_data *data)
{
	if (data->key_data) {
		bin_clear_free(data->key_data, EAP_TLS_KEY_LEN + EAP_EMSK_LEN);
		data->key_data = NULL;
	}
}


static void eap_tls_deinit(struct eap_sm *sm, void *priv)
{
	struct eap_tls_data *data = priv;
	if (data == NULL) {
		return;
	}
	eap_peer_tls_ssl_deinit(sm, &data->ssl);
	eap_tls_free_key(data);
	os_free(data->session_id, 0);
	os_free(data, 0);
}


static struct wpabuf *eap_tls_failure(struct eap_sm *sm,
									  struct eap_tls_data *data,
									  struct eap_method_ret *ret, int res,
									  struct wpabuf *resp, u8 id)
{
	(void)sm;
	(void)res;
	wpa_printf(MSG_DEBUG, "EAP-TLS: TLS processing failed");

	ret->methodState = METHOD_DONE;
	ret->decision = DECISION_FAIL;

	if (resp) {
		/*
		 * This is likely an alert message, so send it instead of just
		 * ACKing the error.
		 */
		return resp;
	}

	return eap_peer_tls_build_ack(id, (enum EapType)data->eap_type, 0);
}


static void eap_tls_success(struct eap_sm *sm, struct eap_tls_data *data,
							struct eap_method_ret *ret)
{
	wpa_printf(MSG_DEBUG, "EAP-TLS: Done");

	ret->methodState = METHOD_DONE;
	ret->decision = DECISION_UNCOND_SUCC;

	eap_tls_free_key(data);
	data->key_data = eap_peer_tls_derive_key(sm, &data->ssl,
					 "client EAP encryption",
					 EAP_TLS_KEY_LEN +
					 EAP_EMSK_LEN);
	if (data->key_data) {
		wpa_hexdump_key(MSG_DEBUG, "EAP-TLS: Derived key",
						data->key_data, EAP_TLS_KEY_LEN);
		wpa_hexdump_key(MSG_DEBUG, "EAP-TLS: Derived EMSK",
						data->key_data + EAP_TLS_KEY_LEN,
						EAP_EMSK_LEN);

		// added by Owen for setting 4-handshake PMK, moved to outside (eap_protocol_handler.c)
		//set_wpa_global_PSK(data->key_data);

	} else {
		wpa_printf(MSG_INFO, "EAP-TLS: Failed to derive key");
	}

	os_free(data->session_id, 0);
	data->session_id = eap_peer_tls_derive_session_id(sm, &data->ssl,
					   EAP_TYPE_TLS,
					   &data->id_len);
	if (data->session_id) {
		wpa_hexdump_key(MSG_DEBUG, "EAP-TLS: Derived Session-Id",
						data->session_id, data->id_len);
		wpa_hexdump(MSG_DEBUG, "EAP-TLS: Derived Session-Id",
					data->session_id, data->id_len);
	} else {
		wpa_printf(MSG_ERROR, "EAP-TLS: Failed to derive Session-Id");
	}
}


static struct wpabuf *eap_tls_process(struct eap_sm *sm, void *priv,
									  struct eap_method_ret *ret,
									  const struct wpabuf *reqData)
{
	size_t left;
	int res;
	struct wpabuf *resp;
	u8 flags, id;
	const u8 *pos;
	struct eap_tls_data *data = priv;
	struct wpabuf msg;

	if (ret == NULL) {
		ret = (struct eap_method_ret *)os_zalloc(sizeof(struct eap_method_ret));
	}

	pos = eap_peer_tls_process_init(sm, &data->ssl, (enum EapType)data->eap_type, ret,
									reqData, &left, &flags);
	if (pos == NULL) {
		return NULL;
	}
	id = eap_get_id(reqData);

	if (flags & EAP_TLS_FLAGS_START) {
		wpa_printf(MSG_DEBUG, "EAP-TLS: Start");
		left = 0; /* make sure that this frame is empty, even though it
			   * should always be, anyway */
	}

	resp = NULL;
	wpabuf_set(&msg, pos, left);
	res = eap_peer_tls_process_helper(sm, &data->ssl, (enum EapType)data->eap_type, 0,
									  id, &msg, &resp);

	if (res < 0) {
		return eap_tls_failure(sm, data, ret, res, resp, id);
	}

	if (tls_connection_established(data->ssl_ctx, data->ssl.conn)) {
		eap_tls_success(sm, data, ret);
	}

	if (res == 1) {
		wpabuf_free(resp);
		return eap_peer_tls_build_ack(id, (enum EapType)data->eap_type, 0);
	}

	if (ret) {
		os_free(ret, 0);
	}

	return resp;
}

#if defined(EAP_REMOVE_UNUSED_CODE) && (EAP_REMOVE_UNUSED_CODE == 0)
static Boolean eap_tls_has_reauth_data(struct eap_sm *sm, void *priv)
{
	struct eap_tls_data *data = priv;
	return (Boolean)tls_connection_established(data->ssl_ctx, data->ssl.conn);
}

static void eap_tls_deinit_for_reauth(struct eap_sm *sm, void *priv)
{
}

static void *eap_tls_init_for_reauth(struct eap_sm *sm, void *priv)
{
	struct eap_tls_data *data = priv;
	eap_tls_free_key(data);
	os_free(data->session_id, 0);
	data->session_id = NULL;
	if (eap_peer_tls_reauth_init(sm, &data->ssl)) {
		os_free(data, 0);
		return NULL;
	}
	return priv;
}

static int eap_tls_get_status(struct eap_sm *sm, void *priv, char *buf,
							  size_t buflen, int verbose)
{
	struct eap_tls_data *data = priv;
	return eap_peer_tls_status(sm, &data->ssl, buf, buflen, verbose);
}
#endif /* !EAP_REMOVE_UNUSED_CODE */

static Boolean eap_tls_isKeyAvailable(struct eap_sm *sm, void *priv)
{
	(void)sm;
	struct eap_tls_data *data = priv;
	return (Boolean)(data->key_data != NULL);
}

static u8 *eap_tls_getKey(struct eap_sm *sm, void *priv, size_t *len)
{
	(void)sm;
	struct eap_tls_data *data = priv;
	u8 *key;

	if (data->key_data == NULL) {
		return NULL;
	}

	key = os_malloc(EAP_TLS_KEY_LEN);
	if (key == NULL) {
		return NULL;
	}

	*len = EAP_TLS_KEY_LEN;
	os_memcpy(key, data->key_data, EAP_TLS_KEY_LEN);

	return key;
}

#if defined(EAP_REMOVE_UNUSED_CODE) && (EAP_REMOVE_UNUSED_CODE == 0)
static u8 *eap_tls_get_emsk(struct eap_sm *sm, void *priv, size_t *len)
{
	struct eap_tls_data *data = priv;
	u8 *key;

	if (data->key_data == NULL) {
		return NULL;
	}

	key = os_malloc(EAP_EMSK_LEN);
	if (key == NULL) {
		return NULL;
	}

	*len = EAP_EMSK_LEN;
	os_memcpy(key, data->key_data + EAP_TLS_KEY_LEN, EAP_EMSK_LEN);

	return key;
}

static u8 *eap_tls_get_session_id(struct eap_sm *sm, void *priv, size_t *len)
{
	struct eap_tls_data *data = priv;
	u8 *id;

	if (data->session_id == NULL) {
		return NULL;
	}

	id = os_malloc(data->id_len);
	if (id == NULL) {
		return NULL;
	}

	*len = data->id_len;
	os_memcpy(id, data->session_id, data->id_len);

	return id;
}
#endif /* !EAP_REMOVE_UNUSED_CODE */

int eap_peer_tls_register(void)
{
	struct eap_method *eap;
	int ret;

	eap = eap_peer_method_alloc(EAP_PEER_METHOD_INTERFACE_VERSION,
								EAP_VENDOR_IETF, EAP_TYPE_TLS, "TLS");
	if (eap == NULL) {
		return -1;
	}

	eap->init = eap_tls_init;
	eap->deinit = eap_tls_deinit;
	eap->process = eap_tls_process;
	eap->getKey = eap_tls_getKey;
	eap->isKeyAvailable = eap_tls_isKeyAvailable;

#if defined(EAP_REMOVE_UNUSED_CODE) && (EAP_REMOVE_UNUSED_CODE == 0)
	eap->getSessionId = eap_tls_get_session_id;
	eap->get_status = eap_tls_get_status;
	eap->has_reauth_data = eap_tls_has_reauth_data;
	eap->deinit_for_reauth = eap_tls_deinit_for_reauth;
	eap->init_for_reauth = eap_tls_init_for_reauth;
	eap->get_emsk = eap_tls_get_emsk;
#endif /* !EAP_REMOVE_UNUSED_CODE */

	ret = eap_peer_method_register(eap);
	if (ret) {
		eap_peer_method_free(eap);
	}
	return ret;
}


#ifdef EAP_UNAUTH_TLS
int eap_peer_unauth_tls_register(void)
{
	struct eap_method *eap;
	int ret;

	eap = eap_peer_method_alloc(EAP_PEER_METHOD_INTERFACE_VERSION,
								EAP_VENDOR_UNAUTH_TLS,
								EAP_VENDOR_TYPE_UNAUTH_TLS, "UNAUTH-TLS");
	if (eap == NULL) {
		return -1;
	}

	eap->init = eap_unauth_tls_init;
	eap->deinit = eap_tls_deinit;
	eap->process = eap_tls_process;
	eap->isKeyAvailable = eap_tls_isKeyAvailable;
	eap->getKey = eap_tls_getKey;
	eap->get_status = eap_tls_get_status;
	eap->has_reauth_data = eap_tls_has_reauth_data;
	eap->deinit_for_reauth = eap_tls_deinit_for_reauth;
	eap->init_for_reauth = eap_tls_init_for_reauth;
	eap->get_emsk = eap_tls_get_emsk;

	ret = eap_peer_method_register(eap);
	if (ret) {
		eap_peer_method_free(eap);
	}
	return ret;
}
#endif /* EAP_UNAUTH_TLS */


#ifdef CONFIG_HS20
int eap_peer_wfa_unauth_tls_register(void)
{
	struct eap_method *eap;
	int ret;

	eap = eap_peer_method_alloc(EAP_PEER_METHOD_INTERFACE_VERSION,
								EAP_VENDOR_WFA_NEW,
								EAP_VENDOR_WFA_UNAUTH_TLS,
								"WFA-UNAUTH-TLS");
	if (eap == NULL) {
		return -1;
	}

	eap->init = eap_wfa_unauth_tls_init;
	eap->deinit = eap_tls_deinit;
	eap->process = eap_tls_process;
	eap->isKeyAvailable = eap_tls_isKeyAvailable;
	eap->getKey = eap_tls_getKey;
	eap->get_status = eap_tls_get_status;
	eap->has_reauth_data = eap_tls_has_reauth_data;
	eap->deinit_for_reauth = eap_tls_deinit_for_reauth;
	eap->init_for_reauth = eap_tls_init_for_reauth;
	eap->get_emsk = eap_tls_get_emsk;

	ret = eap_peer_method_register(eap);
	if (ret) {
		eap_peer_method_free(eap);
	}
	return ret;
}
#endif /* CONFIG_HS20 */

//#endif
/* CONFIG_TLS */
