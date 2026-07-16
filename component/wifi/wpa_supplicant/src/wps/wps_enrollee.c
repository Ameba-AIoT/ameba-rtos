/*
 * Wi-Fi Protected Setup - Enrollee
 * Copyright (c) 2008, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/includes.h"
#include "utils/common.h"

//#include "crypto/aes_wrap.h"
#include "crypto/crypto.h"

#include "crypto/rom/rom_wps_sha_256.h"
#include "crypto/random.h"
#include "wps/wps_i.h"
#include "wps_dev_attr.h"

//#ifdef CONFIG_WPS
#include "../wpa_supplicant/wps_protocol_handler.h"

//extern int wps_validate_m4_encr(const struct wpabuf *tlvs, int wps2);
//extern int wps_validate_m6_encr(const struct wpabuf *tlvs, int wps2);
//extern int wps_validate_m8_encr(const struct wpabuf *tlvs, int ap, int wps2);

extern xqueue_handle_t queue_for_credential;

#define PushButton 0x0004

static int wps_build_wps_state(struct wps_data *wps, struct wpabuf *msg)
{
	/* To avoid gcc warnings */
	(void) wps;

	u8 state;

	state = WPS_STATE_NOT_CONFIGURED;

	wpa_printf(MSG_DEBUG, "WPS:  * Wi-Fi Protected Setup State (%d)",
			   state);
	wpabuf_put_be16(msg, ATTR_WPS_STATE);
	wpabuf_put_be16(msg, 1);
	wpabuf_put_u8(msg, state);
	return 0;
}


static int wps_build_e_hash(struct wps_data *wps, struct wpabuf *msg)
{
	u8 *hash;
	//int i;
	const u8 *addr[4];
	size_t len[4];

	if (random_get_bytes(wps->snonce, 2 * WPS_SECRET_NONCE_LEN) < 0) {
		wpa_printf(MSG_WARNING, "random_get_bytes fail");
		return -1;
	}

	if (wps->dh_pubkey_e == NULL || wps->dh_pubkey_r == NULL) {
		wpa_printf(MSG_WARNING, "WPS: DH public keys not available for E-Hash derivation");
		return -1;
	}


	wpabuf_put_be16(msg, ATTR_E_HASH1);
	wpabuf_put_be16(msg, SHA256_MAC_LEN);
	hash = wpabuf_put(msg, SHA256_MAC_LEN);
	/* E-Hash1 = HMAC_AuthKey(E-S1 || PSK1 || PK_E || PK_R) */
	addr[0] = wps->snonce;
	len[0] = WPS_SECRET_NONCE_LEN;
	addr[1] = wps->psk1;
	len[1] = WPS_PSK_LEN;
	addr[2] = wpabuf_head(wps->dh_pubkey_e);
	len[2] = wpabuf_len(wps->dh_pubkey_e);
	addr[3] = wpabuf_head(wps->dh_pubkey_r);
	len[3] = wpabuf_len(wps->dh_pubkey_r);


	rom_hmac_sha256_vector(wps->authkey, WPS_AUTHKEY_LEN, 4, addr, len, hash);


	wpabuf_put_be16(msg, ATTR_E_HASH2);
	wpabuf_put_be16(msg, SHA256_MAC_LEN);
	hash = wpabuf_put(msg, SHA256_MAC_LEN);
	/* E-Hash2 = HMAC_AuthKey(E-S2 || PSK2 || PK_E || PK_R) */
	addr[0] = wps->snonce + WPS_SECRET_NONCE_LEN;
	addr[1] = wps->psk2;
	rom_hmac_sha256_vector(wps->authkey, WPS_AUTHKEY_LEN, 4, addr, len, hash);

	return 0;
}


static int wps_build_e_snonce1(struct wps_data *wps, struct wpabuf *msg)
{

	wpa_printf(MSG_DEBUG, "WPS:  * E-SNonce1");
	wpabuf_put_be16(msg, ATTR_E_SNONCE1);
	wpabuf_put_be16(msg, WPS_SECRET_NONCE_LEN);
	wpabuf_put_data(msg, wps->snonce, WPS_SECRET_NONCE_LEN);
	return 0;
}


static int wps_build_e_snonce2(struct wps_data *wps, struct wpabuf *msg)
{

	wpa_printf(MSG_DEBUG, "WPS:  * E-SNonce2");
	wpabuf_put_be16(msg, ATTR_E_SNONCE2);
	wpabuf_put_be16(msg, WPS_SECRET_NONCE_LEN);
	wpabuf_put_data(msg, wps->snonce + WPS_SECRET_NONCE_LEN,
					WPS_SECRET_NONCE_LEN);
	return 0;
}


struct wpabuf *wps_build_m1(struct wps_data *wps)
{
	struct wpabuf *msg;
	//int i;


	// config_methods=virtual_display(0x2008) virtual_push_button(0x0280)
	u16 config_methods = g_wps_context.wps->config_methods;	//0x2280;	// 0x2008|(0x0280)|0x0100

	if (random_get_bytes(wps->nonce_e, WPS_NONCE_LEN) < 0) {
		wpa_printf(MSG_WARNING, "random_get_bytes(wps->nonce_e, WPS_NONCE_LEN) < 0");
		return NULL;
	}


	wpa_printf(MSG_INFO, "WPS: Building Message M1");

	msg = wpabuf_alloc(512);
	if (msg == NULL) {
		wpa_printf(MSG_WARNING, "msg == NULL");
		return NULL;
	}

	if (wps_build_version(msg) ||
		wps_build_msg_type(msg, WPS_M1) ||
		wps_build_uuid_e(msg, wps->uuid_e) ||
		wps_build_mac_addr(msg, wps->mac_addr_e) ||
		wps_build_enrollee_nonce(wps, msg) ||
		wps_build_public_key(wps, msg) ||
		wps_build_auth_type_flags(wps, msg) ||
		wps_build_encr_type_flags(wps, msg) ||
		wps_build_conn_type_flags(wps, msg) ||
		wps_build_config_methods(msg, config_methods) ||
		wps_build_wps_state(wps, msg) ||
		wps_build_device_attrs(&(wps->wps->dev), msg) ||
		wps_build_rf_bands(&wps->wps->dev, msg) ||
		wps_build_assoc_state(wps, msg) ||
		wps_build_dev_password_id(msg, wps->dev_pw_id) ||
		wps_build_config_error(msg, WPS_CFG_NO_ERROR) ||
		wps_build_os_version(NULL, msg) ||
		wps_build_wfa_ext(msg, 0, NULL, 0) ||
		wps_build_vendor_ext_m1(&(wps->wps->dev), msg)) {
		wpa_printf(MSG_WARNING, "build_m1 message error!!");
		wpabuf_free(msg);
		return NULL;
	}

	wps->state = RECV_M2;

	return msg;
}


static struct wpabuf *wps_build_m3(struct wps_data *wps)
{
	struct wpabuf *msg;

	wpa_printf(MSG_INFO, "WPS: Building Message M3");

	if (wps->dev_password == NULL) {
		wpa_printf(MSG_WARNING, "WPS: No Device Password available");
		return NULL;
	}
	//wps_fast_derive_psk(wps, wps->dev_password, wps->dev_password_len);
	wps_derive_psk(wps, wps->dev_password, wps->dev_password_len);

	msg = wpabuf_alloc(1000);
	if (msg == NULL) {
		return NULL;
	}
	if (wps_build_version(msg) ||
		wps_build_msg_type(msg, WPS_M3) ||
		wps_build_registrar_nonce(wps, msg) ||
		wps_build_e_hash(wps, msg) ||
		wps_build_wfa_ext(msg, 0, NULL, 0) ||
		wps_build_authenticator(wps, msg)) {
		wpabuf_free(msg);
		wpa_printf(MSG_WARNING, "build_m3 fail");
		return NULL;
	}

	wps->state = RECV_M4;
	return msg;
}


static struct wpabuf *wps_build_m5(struct wps_data *wps)
{
	struct wpabuf *msg, *plain;

	wpa_printf(MSG_INFO, "WPS: Building Message M5");
	plain = wpabuf_alloc(200);
	if (plain == NULL) {
		return NULL;
	}
	msg = wpabuf_alloc(1000);
	if (msg == NULL) {
		wpabuf_free(plain);
		return NULL;
	}

	if (wps_build_version(msg) ||
		wps_build_msg_type(msg, WPS_M5) ||
		wps_build_registrar_nonce(wps, msg) ||
		wps_build_e_snonce1(wps, plain) ||
		wps_build_key_wrap_auth(wps, plain) ||
		wps_build_encr_settings(wps, msg, plain) ||
		wps_build_wfa_ext(msg, 0, NULL, 0) ||
		wps_build_authenticator(wps, msg)) {
		wpabuf_free(plain);
		wpabuf_free(msg);
		return NULL;
	}
	wpabuf_free(plain);

	wps->state = RECV_M6;
	return msg;
}

#if 1
static int wps_build_cred_ssid(struct wps_data *wps, struct wpabuf *msg)
{
	wpa_printf(MSG_DEBUG, "WPS:  * SSID");
	wpabuf_put_be16(msg, ATTR_SSID);
	wpabuf_put_be16(msg, wps->wps->ssid_len);
	wpabuf_put_data(msg, wps->wps->ssid, wps->wps->ssid_len);
	return 0;
}


static int wps_build_cred_auth_type(struct wps_data *wps, struct wpabuf *msg)
{
	u16 auth_type = wps->wps->auth_types;

	/* Select the best authentication type */
	if (auth_type & WPS_AUTH_WPA2PSK) {
		auth_type = WPS_AUTH_WPA2PSK;
	} else if (auth_type & WPS_AUTH_WPAPSK) {
		auth_type = WPS_AUTH_WPAPSK;
	} else if (auth_type & WPS_AUTH_OPEN) {
		auth_type = WPS_AUTH_OPEN;
	} else if (auth_type & WPS_AUTH_SHARED) {
		auth_type = WPS_AUTH_SHARED;
	}

	wpa_printf(MSG_DEBUG, "WPS:  * Authentication Type (0x%x)", auth_type);
	wpabuf_put_be16(msg, ATTR_AUTH_TYPE);
	wpabuf_put_be16(msg, 2);
	wpabuf_put_be16(msg, auth_type);
	return 0;
}


static int wps_build_cred_encr_type(struct wps_data *wps, struct wpabuf *msg)
{
	u16 encr_type = wps->wps->encr_types;

	/* Select the best encryption type */
	if (wps->wps->auth_types & (WPS_AUTH_WPA2PSK | WPS_AUTH_WPAPSK)) {
		if (encr_type & WPS_ENCR_AES) {
			encr_type = WPS_ENCR_AES;
		} else if (encr_type & WPS_ENCR_TKIP) {
			encr_type = WPS_ENCR_TKIP;
		}
	} else {
		if (encr_type & WPS_ENCR_WEP) {
			encr_type = WPS_ENCR_WEP;
		} else if (encr_type & WPS_ENCR_NONE) {
			encr_type = WPS_ENCR_NONE;
		}
	}

	wpa_printf(MSG_DEBUG, "WPS:  * Encryption Type (0x%x)", encr_type);
	wpabuf_put_be16(msg, ATTR_ENCR_TYPE);
	wpabuf_put_be16(msg, 2);
	wpabuf_put_be16(msg, encr_type);
	return 0;
}


static int wps_build_cred_network_key(struct wps_data *wps, struct wpabuf *msg)
{
	wpa_printf(MSG_DEBUG, "WPS:  * Network Key");
	wpabuf_put_be16(msg, ATTR_NETWORK_KEY);
	wpabuf_put_be16(msg, wps->wps->network_key_len);
	wpabuf_put_data(msg, wps->wps->network_key, wps->wps->network_key_len);
	return 0;
}


static int wps_build_cred_mac_addr(struct wps_data *wps, struct wpabuf *msg)
{
	wpa_printf(MSG_DEBUG, "WPS:  * MAC Address (AP BSSID)");
	wpabuf_put_be16(msg, ATTR_MAC_ADDR);
	wpabuf_put_be16(msg, ETH_ALEN);
	wpabuf_put_data(msg, wps->wps->dev.mac_addr, ETH_ALEN);
	return 0;
}
#endif
#if 1
static int wps_build_ap_settings(struct wps_data *wps, struct wpabuf *plain)
{

	return wps_build_cred_ssid(wps, plain) ||
		   wps_build_cred_mac_addr(wps, plain) ||
		   wps_build_cred_auth_type(wps, plain) ||
		   wps_build_cred_encr_type(wps, plain) ||
		   wps_build_cred_network_key(wps, plain);
}
#endif

static struct wpabuf *wps_build_m7(struct wps_data *wps)
{
	struct wpabuf *msg, *plain;

	wpa_printf(MSG_INFO, "WPS: Building Message M7");

	/*
	u8 hash[32];
	const u8 *addr[4];
	size_t len[4];
	int i;

	addr[0] = &wps->snonce[16];


	len[0] = 16;
	addr[1] = wps->psk2;


	len[1] = 16;
	addr[2] = wpabuf_head(wps->dh_pubkey_e);
	len[2] = 192;
	addr[3] = wpabuf_head(wps->dh_pubkey_r);
	len[3] = 192;
	*/

	/*
	rom_hmac_sha256_vector((u8 *)wps->authkey, 32, 4, addr, len, hash);

	*/



	plain = wpabuf_alloc(500 /*+ wps->wps->ap_settings_len*/);
	if (plain == NULL) {
		return NULL;
	}
	msg = wpabuf_alloc(1000 /*+ wps->wps->ap_settings_len*/);
	if (msg == NULL) {
		wpabuf_free(plain);

		return NULL;
	}

	if (wps_build_version(msg) ||
		wps_build_msg_type(msg, WPS_M7) ||
		wps_build_registrar_nonce(wps, msg) ||
		wps_build_e_snonce2(wps, plain) ||
		(wps->wps->ap && wps_build_ap_settings(wps, plain)) || //if AP is enrolle ,add this
		wps_build_key_wrap_auth(wps, plain) ||
		wps_build_encr_settings(wps, msg, plain) ||
		wps_build_wfa_ext(msg, 0, NULL, 0) ||
		wps_build_authenticator(wps, msg)) {


		wpabuf_free(plain);
		wpabuf_free(msg);
		return NULL;
	}
	wpabuf_free(plain);

	wps->state = RECV_M8;
	return msg;
}


static struct wpabuf *wps_build_wsc_done(struct wps_data *wps)
{
	struct wpabuf *msg;

	wpa_printf(MSG_INFO, "WPS: Building Message WSC_Done");

	msg = wpabuf_alloc(1000);
	if (msg == NULL) {
		return NULL;
	}

	if (wps_build_version(msg) ||
		wps_build_msg_type(msg, WPS_WSC_DONE) ||
		wps_build_enrollee_nonce(wps, msg) ||
		wps_build_registrar_nonce(wps, msg) ||
		wps_build_wfa_ext(msg, 0, NULL, 0)) {
		wpabuf_free(msg);
		return NULL;
	}

	wps->state = WPS_FINISHED;

	return msg;
}


struct wpabuf *wps_enrollee_get_msg(struct wps_data *wps,
									enum wsc_op_code *op_code)
{
	struct wpabuf *msg;

	wpa_printf(MSG_DEBUG, "wps->state = 0x%x", wps->state);

	switch (wps->state) {
	case SEND_M1: // 0
		wpa_printf(MSG_DEBUG, "wps_build_m1!!");
		msg = wps_build_m1(wps);
		*op_code = WSC_MSG;
		break;
	case SEND_M3: // 2
		wpa_printf(MSG_DEBUG, "wps_build_m3!!");
		msg = wps_build_m3(wps);
		*op_code = WSC_MSG;
		break;
	case SEND_M5: // 4
		wpa_printf(MSG_DEBUG, "wps_build_m5!!");
		msg = wps_build_m5(wps);
		*op_code = WSC_MSG;
		break;
	case SEND_M7: // 6
		wpa_printf(MSG_DEBUG, "wps_build_m7!!");
		msg = wps_build_m7(wps);
		*op_code = WSC_MSG;
		break;
	case RECEIVED_M2D: // 8
		wpa_printf(MSG_DEBUG, "RECEIVED_M2D!!");
		wpa_printf(MSG_DEBUG, "wps_build_wsc_ack!!");
		msg = wps_build_wsc_ack(wps);
		*op_code = WSC_ACK;
		if (msg) {
			/* Another M2/M2D may be received */
			wps->state = RECV_M2;
		}
		break;
	case SEND_WSC_NACK: // c
		wpa_printf(MSG_DEBUG, "wps_build_wsc_nack");
		msg = wps_build_wsc_nack(wps);
		*op_code = WSC_NACK;
		break;
	case WPS_MSG_DONE:  // 9
		msg = wps_build_wsc_done(wps);
		*op_code = WSC_Done;
		break;
	default:

		wpa_printf(MSG_WARNING, "WPS: Unsupported state %d for building a message", wps->state);
		msg = NULL;
		break;
	}

	if (*op_code == WSC_MSG && msg) {
		//rtos_time_delay_ms(1500);
		/* Save a copy of the last message for Authenticator derivation */
		wpabuf_free(wps->last_msg);
		wps->last_msg = wpabuf_dup(msg);
	}

	return msg;
}


static int wps_process_registrar_nonce(struct wps_data *wps, const u8 *r_nonce)
{
	if (r_nonce == NULL) {
		wpa_printf(MSG_WARNING, "WPS: No Registrar Nonce received!!");
		return -1;
	}

	os_memcpy(wps->nonce_r, r_nonce, WPS_NONCE_LEN);

	return 0;
}


static int wps_process_enrollee_nonce(struct wps_data *wps, const u8 *e_nonce)
{

	if (e_nonce == NULL) {
		wpa_printf(MSG_WARNING, "WPS: No Enrollee Nonce received");
		return -1;
	}

	if (os_memcmp(wps->nonce_e, e_nonce, WPS_NONCE_LEN) == FALSE) {
		wpa_printf(MSG_WARNING, "WPS: Invalid Enrollee Nonce received");
		return -1;
	}

	return 0;
}


static int wps_process_uuid_r(struct wps_data *wps, const u8 *uuid_r)
{
	//int i;

	if (uuid_r == NULL) {
		wpa_printf(MSG_WARNING, "WPS: No UUID-R received");
		return -1;
	}

	os_memcpy(wps->uuid_r, uuid_r, WPS_UUID_LEN);

	return 0;
}


static int wps_process_pubkey(struct wps_data *wps, const u8 *pk,
							  size_t pk_len)
{

	if (pk == NULL || pk_len == 0) {
		wpa_printf(MSG_WARNING, "WPS: No Public Key received");

		return -1;
	}
	wpabuf_free(wps->dh_pubkey_r);
	//get the registrar's public key(i.e PKr)
	wps->dh_pubkey_r = wpabuf_alloc_copy(pk, pk_len);
	if (wps->dh_pubkey_r == NULL) {
		wpa_printf(MSG_WARNING, "wps->dh_pubkey_r == NULL");
		return -1;
	}
	if (wps_derive_keys(wps) < 0) {
		wpa_printf(MSG_WARNING, "wps_derive_keys(wps) < 0");
		return -1;
	}

	return 0;
}


static int wps_process_r_hash1(struct wps_data *wps, const u8 *r_hash1)
{
	//int i;
	if (r_hash1 == NULL) {
		wpa_printf(MSG_WARNING, "WPS: No R-Hash1 received");
		return -1;
	}

	os_memcpy(wps->peer_hash1, r_hash1, WPS_HASH_LEN);

	return 0;
}


static int wps_process_r_hash2(struct wps_data *wps, const u8 *r_hash2)
{
	//int i;

	if (r_hash2 == NULL) {
		wpa_printf(MSG_WARNING, "WPS: No R-Hash2 received");

		return -1;
	}

	os_memcpy(wps->peer_hash2, r_hash2, WPS_HASH_LEN);

	return 0;
}


static int wps_process_r_snonce1(struct wps_data *wps, const u8 *r_snonce1)
{
	u8 hash[SHA256_MAC_LEN];
	const u8 *addr[4];
	size_t len[4];
	//int i;



	if (r_snonce1 == NULL) {
		wpa_printf(MSG_WARNING, "WPS: No R-SNonce1 received");
		return -1;
	}


	/* R-Hash1 = HMAC_AuthKey(R-S1 || PSK1 || PK_E || PK_R) */
	wpa_printf(MSG_DEBUG, "R-Hash1 = HMAC_AuthKey(R-S1 || PSK1 || PK_E || PK_R)");
	addr[0] = r_snonce1;
	len[0] = WPS_SECRET_NONCE_LEN;
	addr[1] = wps->psk1;
	len[1] = WPS_PSK_LEN;
	addr[2] = wpabuf_head(wps->dh_pubkey_e);
	len[2] = wpabuf_len(wps->dh_pubkey_e);
	addr[3] = wpabuf_head(wps->dh_pubkey_r);
	len[3] = wpabuf_len(wps->dh_pubkey_r);


	rom_hmac_sha256_vector(wps->authkey, WPS_AUTHKEY_LEN, 4, addr, len, hash);


	if (os_memcmp(wps->peer_hash1, hash, WPS_HASH_LEN) == FALSE) {
		wps->config_error = WPS_CFG_DEV_PASSWORD_AUTH_FAILURE;
		//wps_pwd_auth_fail_event(wps->wps, 1, 1);
		wpa_printf(MSG_WARNING, "WPS: R-Hash1 derived from R-S1 does not match with the pre-committed value");
		return -1;
	}


	wpa_printf(MSG_DEBUG, "WPS: Registrar proved knowledge of the first half of the device password");
	return 0;
}


static int wps_process_r_snonce2(struct wps_data *wps, const u8 *r_snonce2)
{
	u8 hash[SHA256_MAC_LEN];
	const u8 *addr[4];
	size_t len[4];

	if (r_snonce2 == NULL) {
		return -1;
	}


	/* R-Hash2 = HMAC_AuthKey(R-S2 || PSK2 || PK_E || PK_R) */
	addr[0] = r_snonce2;
	len[0] = WPS_SECRET_NONCE_LEN;
	addr[1] = wps->psk2;
	len[1] = WPS_PSK_LEN;
	addr[2] = wpabuf_head(wps->dh_pubkey_e);
	len[2] = wpabuf_len(wps->dh_pubkey_e);
	addr[3] = wpabuf_head(wps->dh_pubkey_r);
	len[3] = wpabuf_len(wps->dh_pubkey_r);
	rom_hmac_sha256_vector(wps->authkey, WPS_AUTHKEY_LEN, 4, addr, len, hash);

	if (os_memcmp(wps->peer_hash2, hash, WPS_HASH_LEN) == FALSE) {
		wps->config_error = WPS_CFG_DEV_PASSWORD_AUTH_FAILURE;
		return -1;
	}


	return 0;
}


static int wps_process_cred_e(struct wps_data *wps, const u8 *cred,
							  size_t cred_len, int wps2)
{
	/* To avoid gcc warnings */
	(void) wps2;

	struct wps_parse_attr attr;
	struct wpabuf msg;
	int ret = 0;


#if 0
	os_memset(&wps->cred, 0, sizeof(wps->cred));

	/* put the cred data in msg buffer */
	wpabuf_set(&msg, cred, cred_len);

	if (wps_parse_msg(&msg, &attr) < 0 || wps_process_cred(&attr, &wps->cred)) {
		return -1;
	}
#else	// filter cred by rf_bands
	/* put the cred data in msg buffer */
	wpabuf_set(&msg, cred, cred_len);

	if (wps_parse_msg(&msg, &attr) < 0) {
		return -1;
	}
#ifndef SUPPORT_5G_CHANNEL
	if (attr.rf_bands && (*attr.rf_bands == WPS_RF_50GHZ)) {
		// skip cred
		return -2;
	}
#endif
	os_memset(&wps->cred, 0, sizeof(wps->cred));

	if (wps_process_cred(&attr, &wps->cred)) {
		return -1;
	}
#endif

#ifdef CONFIG_WPS2
	if (wps->cred.encr_type & WPS_ENCR_WEP) {
		wpa_printf(MSG_INFO, "WPS: Reject Credential "
				   "due to WEP configuration");

		return -2;
	}
#endif /* CONFIG_WPS2 */

	if (queue_for_credential != NULL) {
		os_xqueue_send(queue_for_credential, &wps->cred, 0);
	}

	return ret;
}

// wps, eattr.cred, eattr.cred_len, eattr.num_cred, attr->version2 != NULL)
static int wps_process_creds(struct wps_data *wps, const u8 *cred[],
							 size_t cred_len[], size_t num_cred, int wps2)
{
	size_t i;
	int ok = 0;


	if (num_cred == 0) {
		return -1;
	}

	for (i = 0; i < num_cred; i++) {
		int res;
		res = wps_process_cred_e(wps, cred[i], cred_len[i], wps2);
		if (res == 0) {
			ok++;
		} else {
			return -1;
		}
	}

	return 0;
}

static enum wps_process_res wps_process_m2(struct wps_data *wps,
		const struct wpabuf *msg,
		struct wps_parse_attr *attr)
{
	wpa_printf(MSG_INFO, "WPS: Received M2");

	if (wps->state != RECV_M2) {
		wpa_printf(MSG_WARNING, "WPS: Unexpected state (%d) for receiving M2", wps->state);
		wps->state = SEND_WSC_NACK;
		return WPS_CONTINUE;
	}

	if (wps_process_registrar_nonce(wps, attr->registrar_nonce) ||
		wps_process_enrollee_nonce(wps, attr->enrollee_nonce) ||
		wps_process_uuid_r(wps, attr->uuid_r)) {

		wps->state = SEND_WSC_NACK;
		return WPS_CONTINUE;
	}

	if (wps_process_pubkey(wps, attr->public_key, attr->public_key_len) ||
		wps_process_authenticator(wps, attr->authenticator, msg) ||
		wps_process_device_attrs(&wps->peer_dev, attr)) {

		wps->state = SEND_WSC_NACK;
		return WPS_CONTINUE;
	}

	wps->state = SEND_M3;

	return WPS_CONTINUE;
}


static enum wps_process_res wps_process_m2d(struct wps_data *wps,
		struct wps_parse_attr *attr)
{
	/* To avoid gcc warnings */
	(void) attr;

	wpa_printf(MSG_INFO, "WPS: Received M2D");

	if (wps->state != RECV_M2) {
		wpa_printf(MSG_DEBUG, "WPS: Unexpected state (%d) for "
				   "receiving M2D", wps->state);
		wps->state = SEND_WSC_NACK;
		return WPS_CONTINUE;
	}

	wpa_hexdump_ascii(MSG_DEBUG, "WPS: Manufacturer",
					  attr->manufacturer, attr->manufacturer_len);
	wpa_hexdump_ascii(MSG_DEBUG, "WPS: Model Name",
					  attr->model_name, attr->model_name_len);
	wpa_hexdump_ascii(MSG_DEBUG, "WPS: Model Number",
					  attr->model_number, attr->model_number_len);
	wpa_hexdump_ascii(MSG_DEBUG, "WPS: Serial Number",
					  attr->serial_number, attr->serial_number_len);
	wpa_hexdump_ascii(MSG_DEBUG, "WPS: Device Name",
					  attr->dev_name, attr->dev_name_len);



	wps->state = RECEIVED_M2D;
	return WPS_CONTINUE;
}


static enum wps_process_res wps_process_m4(struct wps_data *wps,
		const struct wpabuf *msg,
		struct wps_parse_attr *attr)
{
	struct wpabuf *decrypted;
	struct wps_parse_attr eattr;


	wpa_printf(MSG_INFO, "WPS: Received M4");
	if (wps->state != RECV_M4) {
		wps->state = SEND_WSC_NACK;
		wpa_printf(MSG_WARNING, "WPS: Unexpected state (%d) for receiving M4", wps->state);
		return WPS_CONTINUE;
	}

	if (wps_process_enrollee_nonce(wps, attr->enrollee_nonce) ||
		wps_process_authenticator(wps, attr->authenticator, msg) ||
		wps_process_r_hash1(wps, attr->r_hash1) ||
		wps_process_r_hash2(wps, attr->r_hash2)) {
		wps->state = SEND_WSC_NACK;

		return WPS_CONTINUE;
	}

	wpa_printf(MSG_DEBUG, "attr->encr_settings_len = 0x%x", attr->encr_settings_len);

	decrypted = wps_decrypt_encr_settings(wps, attr->encr_settings,
										  attr->encr_settings_len);
	if (decrypted == NULL) {
		wpa_printf(MSG_WARNING, "WPS: Failed to decrypted Encrypted Settings attribute");
		wps->state = SEND_WSC_NACK;
		return WPS_CONTINUE;
	}

	if (wps_validate_m4_encr(decrypted, attr->version2 != NULL) < 0) {
		wpabuf_free(decrypted);
		wps->state = SEND_WSC_NACK;
		return WPS_CONTINUE;
	}


	wpa_printf(MSG_DEBUG, "WPS: Processing decrypted Encrypted Settings attribute");

	if (wps_parse_msg(decrypted, &eattr) < 0 ||
		wps_process_key_wrap_auth(wps, decrypted, eattr.key_wrap_auth) ||
		wps_process_r_snonce1(wps, eattr.r_snonce1)) {
		wpabuf_free(decrypted);
		wps->state = SEND_WSC_NACK;

		return WPS_CONTINUE;
	}
	wpabuf_free(decrypted);

	wps->state = SEND_M5;

	return WPS_CONTINUE;
}


static enum wps_process_res wps_process_m6(struct wps_data *wps,
		const struct wpabuf *msg,
		struct wps_parse_attr *attr)
{
	struct wpabuf *decrypted;
	struct wps_parse_attr eattr;

	wpa_printf(MSG_INFO, "WPS: Received M6");
	if (wps->state != RECV_M6) {

		wpa_printf(MSG_WARNING, "WPS: Unexpected state (%d) for receiving M6", wps->state);
		wps->state = SEND_WSC_NACK;
		return WPS_CONTINUE;
	}

	if (wps_process_enrollee_nonce(wps, attr->enrollee_nonce) ||
		wps_process_authenticator(wps, attr->authenticator, msg)) {
		wps->state = SEND_WSC_NACK;
		return WPS_CONTINUE;
	}

	decrypted = wps_decrypt_encr_settings(wps, attr->encr_settings,
										  attr->encr_settings_len);
	if (decrypted == NULL) {
		wpa_printf(MSG_DEBUG, "WPS: Failed to decrypted Encrypted "
				   "Settings attribute");
		wps->state = SEND_WSC_NACK;
		return WPS_CONTINUE;
	}

	if (wps_validate_m6_encr(decrypted, attr->version2 != NULL) < 0) {
		wpabuf_free(decrypted);
		wps->state = SEND_WSC_NACK;
		return WPS_CONTINUE;
	}

	wpa_printf(MSG_DEBUG, "WPS: Processing decrypted Encrypted Settings "
			   "attribute");
	if (wps_parse_msg(decrypted, &eattr) < 0 ||
		wps_process_key_wrap_auth(wps, decrypted, eattr.key_wrap_auth) ||
		wps_process_r_snonce2(wps, eattr.r_snonce2)) {
		wpabuf_free(decrypted);
		wps->state = SEND_WSC_NACK;
		return WPS_CONTINUE;
	}
	wpabuf_free(decrypted);

	wps->state = SEND_M7;
	return WPS_CONTINUE;
}

/* wps = data->wps, msg = data->in_buf , attr(parsered msg) */
static enum wps_process_res wps_process_m8(struct wps_data *wps,
		const struct wpabuf *msg,
		struct wps_parse_attr *attr)
{
	struct wpabuf *decrypted;
	struct wps_parse_attr eattr;

	wpa_printf(MSG_INFO, "WPS: Received M8");
	if (wps->state != RECV_M8) {
		wpa_printf(MSG_WARNING, "WPS: Unexpected state (%d) for receiving M8", wps->state);
		wps->state = SEND_WSC_NACK;
		return WPS_CONTINUE;
	}

	if (wps_process_enrollee_nonce(wps, attr->enrollee_nonce) ||
		wps_process_authenticator(wps, attr->authenticator, msg)) {
		wps->state = SEND_WSC_NACK;
		return WPS_CONTINUE;
	}

	decrypted = wps_decrypt_encr_settings(wps, attr->encr_settings,
										  attr->encr_settings_len);
	if (decrypted == NULL) {
		wpa_printf(MSG_WARNING, "WPS: Failed to decrypted Encrypted Settings attribute");
		wps->state = SEND_WSC_NACK;
		return WPS_CONTINUE;
	}

	if (wps_validate_m8_encr(decrypted, 0, attr->version2 != NULL) < 0) {
		wpabuf_free(decrypted);
		wps->state = SEND_WSC_NACK;
		return WPS_CONTINUE;
	}

	wpa_printf(MSG_DEBUG, "WPS: Processing decrypted Encrypted Settings attribute");

	if (wps_parse_msg(decrypted, &eattr) < 0 ||
		wps_process_key_wrap_auth(wps, decrypted, eattr.key_wrap_auth) ||
		wps_process_creds(wps, eattr.cred, eattr.cred_len,
						  eattr.num_cred, attr->version2 != NULL)) {
		wpabuf_free(decrypted);
		wps->state = SEND_WSC_NACK;
		return WPS_CONTINUE;
	}
	wpabuf_free(decrypted);

	wps->state = WPS_MSG_DONE;
	return WPS_CONTINUE;
}

/* wps = data->wps, msg = data->in_buf */
static enum wps_process_res wps_process_wsc_msg(struct wps_data *wps,
		const struct wpabuf *msg)
{
	struct wps_parse_attr *attr = rtos_mem_zmalloc(sizeof(struct wps_parse_attr)); /*realtek modify https://jira.realtek.com/browse/RSWLANDIOT-8148*/
	enum wps_process_res ret = WPS_CONTINUE;
	//int i;

	if (wps_parse_msg(msg, attr) < 0) {
		wpa_printf(MSG_WARNING, "wps_parse_msg fail!!");
		rtos_mem_free(attr);
		return WPS_FAILURE;
	}

	if (attr->enrollee_nonce == NULL ||
		os_memcmp(wps->nonce_e, attr->enrollee_nonce, WPS_NONCE_LEN) == FALSE) {
		wpa_printf(MSG_WARNING, "WPS: Mismatch in enrollee nonce");
		rtos_mem_free(attr);
		return WPS_FAILURE;
	}

	if (attr->msg_type == NULL) {
		wps->state = SEND_WSC_NACK;
		wpa_printf(MSG_WARNING, "WPS: No Message Type attribute");
		rtos_mem_free(attr);
		return WPS_CONTINUE;
	}

	switch (*(attr->msg_type)) {
	case WPS_M2:
		wpa_printf(MSG_INFO, "Receive WPS_M2");
		if (wps_validate_m2(msg) < 0) {
			wpa_printf(MSG_DEBUG, "wps_validate_m2 fail!!");
			rtos_mem_free(attr);
			return WPS_FAILURE;
		}
		ret = wps_process_m2(wps, msg, attr);
		break;
	case WPS_M2D:
		wpa_printf(MSG_INFO, "Receive WPS_M2D");
		if (wps_validate_m2d(msg) < 0) {
			wpa_printf(MSG_DEBUG, "wps_validate_m2d fail!!");
			rtos_mem_free(attr);
			return WPS_FAILURE;
		}
		ret = wps_process_m2d(wps, attr);
		break;
	case WPS_M4:
		wpa_printf(MSG_INFO, "Receive WPS_M4");
		if (wps_validate_m4(msg) < 0) {
			wpa_printf(MSG_DEBUG, "wps_validate_m4 fail!!");
			rtos_mem_free(attr);
			return WPS_FAILURE;
		}
		ret = wps_process_m4(wps, msg, attr);
		if (ret == WPS_FAILURE || wps->state == SEND_WSC_NACK) {
			wpa_printf(MSG_INFO, "WPS_FAILURE or State = SEND_WSC_NACK");
		}
		break;
	case WPS_M6:
		wpa_printf(MSG_INFO, "Receive WPS_M6");
		if (wps_validate_m6(msg) < 0) {
			wpa_printf(MSG_DEBUG, "wps_validate_m6 fail!!");
			rtos_mem_free(attr);
			return WPS_FAILURE;
		}
		ret = wps_process_m6(wps, msg, attr);
		if (ret == WPS_FAILURE || wps->state == SEND_WSC_NACK) {
			wpa_printf(MSG_INFO, "WPS_FAILURE or State = SEND_WSC_NACK");
		}
		break;
	case WPS_M8:
		wpa_printf(MSG_INFO, "Receive WPS_M8");
		if (wps_validate_m8(msg) < 0) {
			wpa_printf(MSG_DEBUG, "wps_validate_m8 fail!!");
			rtos_mem_free(attr);
			return WPS_FAILURE;
		}
		ret = wps_process_m8(wps, msg, attr);
		if (ret == WPS_FAILURE || wps->state == SEND_WSC_NACK) {
			wpa_printf(MSG_INFO, "WPS_FAILURE or State = SEND_WSC_NACK");
		}
		break;
	default:
		wpa_printf(MSG_DEBUG, "WPS: Unsupported Message Type %d", *(attr->msg_type));
		rtos_mem_free(attr);
		return WPS_FAILURE;
	}

	/*
	 * Save a copy of the last message for Authenticator derivation if we
	 * are continuing. However, skip M2D since it is not authenticated and
	 * neither is the ACK/NACK response frame. This allows the possibly
	 * following M2 to be processed correctly by using the previously sent
	 * M1 in Authenticator derivation.
	 */
	if (ret == WPS_CONTINUE && *(attr->msg_type) != WPS_M2D) {
		/* Save a copy of the last message for Authenticator derivation
		 */
		wpabuf_free(wps->last_msg);
		wps->last_msg = wpabuf_dup(msg);
	}
	rtos_mem_free(attr);
	return ret;
}


static enum wps_process_res wps_process_wsc_ack(struct wps_data *wps,
		const struct wpabuf *msg)
{
	struct wps_parse_attr attr;

	wpa_printf(MSG_DEBUG, "WPS: Received WSC_ACK");

	if (wps_parse_msg(msg, &attr) < 0) {
		return WPS_FAILURE;
	}

	if (attr.msg_type == NULL) {
		wpa_printf(MSG_DEBUG, "WPS: No Message Type attribute");
		return WPS_FAILURE;
	}

	if (*attr.msg_type != WPS_WSC_ACK) {
		wpa_printf(MSG_DEBUG, "WPS: Invalid Message Type %d",
				   *attr.msg_type);
		return WPS_FAILURE;
	}

	if (attr.registrar_nonce == NULL ||
		os_memcmp(wps->nonce_r, attr.registrar_nonce, WPS_NONCE_LEN) == FALSE) {
		wpa_printf(MSG_DEBUG, "WPS: Mismatch in registrar nonce");
		return WPS_FAILURE;
	}

	if (attr.enrollee_nonce == NULL ||
		os_memcmp(wps->nonce_e, attr.enrollee_nonce, WPS_NONCE_LEN) == FALSE) {
		wpa_printf(MSG_DEBUG, "WPS: Mismatch in enrollee nonce");
		return WPS_FAILURE;
	}

	if (wps->state == RECV_ACK && 0/*wps->wps->ap*/) {
		wpa_printf(MSG_DEBUG, "WPS: External Registrar registration "
				   "completed successfully");

		wps->state = WPS_FINISHED;
		return WPS_DONE;
	}

	return WPS_FAILURE;
}


static enum wps_process_res wps_process_wsc_nack(struct wps_data *wps,
		const struct wpabuf *msg)
{
	struct wps_parse_attr attr;
	//u16 config_error;

	wpa_printf(MSG_INFO, "WPS: Received WSC_NACK");
	if (wps_parse_msg(msg, &attr) < 0) {
		return WPS_FAILURE;
	}

	if (attr.msg_type == NULL) {
		wpa_printf(MSG_DEBUG, "WPS: No Message Type attribute");
		return WPS_FAILURE;
	}

	if (*attr.msg_type != WPS_WSC_NACK) {
		wpa_printf(MSG_DEBUG, "WPS: Invalid Message Type %d",
				   *attr.msg_type);
		return WPS_FAILURE;
	}

	if (attr.registrar_nonce == NULL ||
		os_memcmp(wps->nonce_r, attr.registrar_nonce, WPS_NONCE_LEN) == FALSE/*!= 0*/) {
		wpa_printf(MSG_DEBUG, "WPS: Mismatch in registrar nonce");
		wpa_hexdump(MSG_DEBUG, "WPS: Received Registrar Nonce",
					attr.registrar_nonce, WPS_NONCE_LEN);
		wpa_hexdump(MSG_DEBUG, "WPS: Expected Registrar Nonce",
					wps->nonce_r, WPS_NONCE_LEN);
		return WPS_FAILURE;
	}

	if (attr.enrollee_nonce == NULL ||
		os_memcmp(wps->nonce_e, attr.enrollee_nonce, WPS_NONCE_LEN) == FALSE) {
		wpa_printf(MSG_DEBUG, "WPS: Mismatch in enrollee nonce");
		wpa_hexdump(MSG_DEBUG, "WPS: Received Enrollee Nonce",
					attr.enrollee_nonce, WPS_NONCE_LEN);
		wpa_hexdump(MSG_DEBUG, "WPS: Expected Enrollee Nonce",
					wps->nonce_e, WPS_NONCE_LEN);
		return WPS_FAILURE;
	}

	if (attr.config_error == NULL) {
		wpa_printf(MSG_INFO, "WPS: No Configuration Error attribute in WSC_NACK");
		return WPS_FAILURE;
	}

	//config_error = WPA_GET_BE16(attr.config_error);
	wpa_printf(MSG_DEBUG, "WPS: Registrar terminated negotiation with Configuration Error %d", WPA_GET_BE16(attr.config_error)/*config_error*/);

	switch (wps->state) {
	case RECV_M4:
		//wps_fail_event(wps->wps, WPS_M3, config_error,
		//wps->error_indication);
		break;
	case RECV_M6:
		//wps_fail_event(wps->wps, WPS_M5, config_error,
		//wps->error_indication);
		break;
	case RECV_M8:
		//wps_fail_event(wps->wps, WPS_M7, config_error,
		//wps->error_indication);
		break;
	default:
		break;
	}

	/* Followed by NACK if Enrollee is Supplicant or EAP-Failure if
	 * Enrollee is Authenticator */
	wps->state = SEND_WSC_NACK;

	return WPS_FAILURE;
}

// wps = data->wps,     msg = data->in_buf
enum wps_process_res wps_enrollee_process_msg(struct wps_data *wps,
		enum wsc_op_code op_code, const struct wpabuf *msg)
{

	if (op_code == WSC_UPnP) {
		/* Determine the OpCode based on message type attribute */
		struct wps_parse_attr *attr = rtos_mem_zmalloc(sizeof(struct wps_parse_attr));/*realtek modify https://jira.realtek.com/browse/RSWLANDIOT-8148*/

		if (wps_parse_msg(msg, attr) == 0 && attr->msg_type) {
			if (*(attr->msg_type) == WPS_WSC_ACK) {
				op_code = WSC_ACK;
			} else if (*(attr->msg_type) == WPS_WSC_NACK) {
				op_code = WSC_NACK;
			}
		}
		rtos_mem_free(attr);
	}

	switch (op_code) {
	case WSC_MSG:
	case WSC_UPnP:
		return wps_process_wsc_msg(wps, msg);
	case WSC_ACK:
		if (wps_validate_wsc_ack(msg) < 0) {
			return WPS_FAILURE;
		}
		return wps_process_wsc_ack(wps, msg);
	case WSC_NACK:
		if (wps_validate_wsc_nack(msg) < 0) {
			return WPS_FAILURE;
		}
		return wps_process_wsc_nack(wps, msg);
	default:
		return WPS_FAILURE;
	}
}
//#endif
