/*
 * Wi-Fi Protected Setup - attribute processing
 * Copyright (c) 2008, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/includes.h"
#include "utils/common.h"
#include "crypto/rom/rom_wps_sha_256.h"
#include "wps_i.h"

//#ifdef CONFIG_WPS

int wps_process_authenticator(struct wps_data *wps, const u8 *authenticator,
							  const struct wpabuf *msg)
{
	u8 hash[SHA256_MAC_LEN];
	const u8 *addr[2];
	size_t len[2];
	//int i;

	if (authenticator == NULL) {
		wpa_printf(MSG_INFO, "WPS: No Authenticator attribute included");
		return -1;
	}


	if (wps->last_msg == NULL) {
		wpa_printf(MSG_INFO, "WPS: Last message not available for validating authenticator");
		return -1;
	}


	/* Authenticator = HMAC-SHA256_AuthKey(M_prev || M_curr*)
	 * (M_curr* is M_curr without the Authenticator attribute)
	 */
	wpa_printf(MSG_DEBUG, "Authenticator = HMAC-SHA256_AuthKey(M_prev || M_curr*)");
	wpa_printf(MSG_DEBUG, "(M_curr* is M_curr without the Authenticator attribute)");
	addr[0] = wpabuf_head(wps->last_msg);
	len[0] = wpabuf_len(wps->last_msg);
	addr[1] = wpabuf_head(msg);
	len[1] = wpabuf_len(msg) - 4 - WPS_AUTHENTICATOR_LEN;
	wpa_printf(MSG_DEBUG, "key = wps->authkey");
	rom_hmac_sha256_vector(wps->authkey, WPS_AUTHKEY_LEN, 2, addr, len, hash);

	if (os_memcmp(hash, authenticator, WPS_AUTHENTICATOR_LEN) == FALSE) {
		wpa_printf(MSG_INFO, "WPS: Incorrect Authenticator");
		return -1;
	}

	return 0;
}


int wps_process_key_wrap_auth(struct wps_data *wps, struct wpabuf *msg,
							  const u8 *key_wrap_auth)
{
	u8 hash[SHA256_MAC_LEN];
	const u8 *head;
	size_t len;
	//int i;

	if (key_wrap_auth == NULL) {
		wpa_printf(MSG_INFO, "WPS: No KWA in decrypted attribute");
		return -1;
	}

	head = wpabuf_head(msg);
	len = wpabuf_len(msg) - 4 - WPS_KWA_LEN;
	if (head + len != key_wrap_auth - 4) {
		wpa_printf(MSG_INFO, "WPS: KWA not in the end of the decrypted attribute");
		return -1;
	}

	rom_hmac_sha256(wps->authkey, WPS_AUTHKEY_LEN, head, len, hash);


	if (os_memcmp(hash, key_wrap_auth, WPS_KWA_LEN) == FALSE) {
		wpa_printf(MSG_INFO, "WPS: Invalid KWA");
		return -1;
	}

	return 0;
}


static int wps_process_cred_network_idx(struct wps_credential *cred,
										const u8 *idx)
{
	/* To avoid gcc warnings */
	(void) cred;

	if (idx == NULL) {
		wpa_printf(MSG_INFO, "WPS: Credential did not include Network Index");
		return -1;
	}
	wpa_printf(MSG_DEBUG, "WPS: Network Index: %d", *idx);

	return 0;
}


static int wps_process_cred_ssid(struct wps_credential *cred, const u8 *ssid,
								 size_t ssid_len)
{
	if (ssid == NULL) {
		wpa_printf(MSG_INFO, "WPS: Credential did not include SSID");
		return -1;
	}

	/* Remove zero-padding since some Registrar implementations seem to use
	 * hardcoded 32-octet length for this attribute */
	while (ssid_len > 0 && ssid[ssid_len - 1] == 0) {
		ssid_len--;
	}

	wpa_hexdump_ascii(MSG_DEBUG, "WPS: SSID", ssid, ssid_len);

	if (ssid_len <= sizeof(cred->ssid)) {
		os_memcpy(cred->ssid, ssid, ssid_len);
		cred->ssid_len = ssid_len;
	}

	return 0;
}


static int wps_process_cred_auth_type(struct wps_credential *cred,
									  const u8 *auth_type)
{
	if (auth_type == NULL) {
		wpa_printf(MSG_INFO, "WPS: Credential did not include Authentication Type");
		return -1;
	}

	cred->auth_type = WPA_GET_BE16(auth_type);
	wpa_printf(MSG_DEBUG, "WPS: Authentication Type: 0x%x", cred->auth_type);

	return 0;
}


static int wps_process_cred_encr_type(struct wps_credential *cred,
									  const u8 *encr_type)
{
	if (encr_type == NULL) {
		wpa_printf(MSG_INFO, "WPS: Credential did not include Encryption Type");
		return -1;
	}

	cred->encr_type = WPA_GET_BE16(encr_type);
	wpa_printf(MSG_DEBUG, "WPS: Encryption Type: 0x%x", cred->encr_type);

	return 0;
}


static int wps_process_cred_network_key_idx(struct wps_credential *cred,
		const u8 *key_idx)
{
	if (key_idx == NULL) {
		wpa_printf(MSG_DEBUG, "key_idx == NULL,so it's optional attribute");
		return 0; /* optional attribute */
	}
	wpa_printf(MSG_DEBUG, "WPS: Network Key Index: %d", *key_idx);

	cred->key_idx = *key_idx;

	return 0;
}


static int wps_process_cred_network_key(struct wps_credential *cred,
										const u8 *key, size_t key_len)
{
	if (key == NULL) {
		wpa_printf(MSG_INFO, "WPS: Credential did not include Network Key");
		if (cred->auth_type == WPS_AUTH_OPEN && cred->encr_type == WPS_ENCR_NONE) {
			wpa_printf(MSG_INFO, "WPS: Workaround - Allow missing mandatory Network Key attribute for open network");
			return 0;
		}
		return -1;
	}

	wpa_hexdump_key(MSG_DEBUG, "WPS: Network Key", key, key_len);

	if (key_len <= sizeof(cred->key)) {
		os_memcpy(cred->key, key, key_len);
		cred->key_len = key_len;
	}

	return 0;
}


static int wps_process_cred_mac_addr(struct wps_credential *cred,
									 const u8 *mac_addr)
{
	if (mac_addr == NULL) {
		wpa_printf(MSG_INFO, "WPS: Credential did not include MAC Address");
		return -1;
	}
	wpa_printf(MSG_INFO, "WPS: MAC Address "MACSTR, MAC2STR(mac_addr));
	os_memcpy(cred->mac_addr, mac_addr, ETH_ALEN);

	return 0;
}


static int wps_process_cred_eap_type(struct wps_credential *cred,
									 const u8 *eap_type, size_t eap_type_len)
{
	/* To avoid gcc warnings */
	(void) cred;
	(void) eap_type_len;

	if (eap_type == NULL) {
		wpa_printf(MSG_DEBUG, "eap_type == NULL, optional attribute");
		return 0; /* optional attribute */
	}
	wpa_hexdump(MSG_DEBUG, "WPS: EAP Type", eap_type, eap_type_len);
	return 0;
}


static int wps_process_cred_eap_identity(struct wps_credential *cred,
		const u8 *identity,
		size_t identity_len)
{
	/* To avoid gcc warnings */
	(void) cred;
	(void) identity_len;

	if (identity == NULL) {
		wpa_printf(MSG_DEBUG, "identity == NULL, optional attribute");
		return 0; /* optional attribute */
	}
	wpa_hexdump_ascii(MSG_DEBUG, "WPS: EAP Identity",
					  identity, identity_len);

	return 0;
}


static int wps_process_cred_key_prov_auto(struct wps_credential *cred,
		const u8 *key_prov_auto)
{
	/* To avoid gcc warnings */
	(void) cred;

	if (key_prov_auto == NULL) {
		wpa_printf(MSG_DEBUG, "key_prov_auto == NULL, optional attribute");
		return 0; /* optional attribute */
	}
	wpa_printf(MSG_DEBUG, "WPS: Key Provided Automatically: %d", *key_prov_auto);
	return 0;
}


static int wps_process_cred_802_1x_enabled(struct wps_credential *cred,
		const u8 *dot1x_enabled)
{
	/* To avoid gcc warnings */
	(void) cred;

	if (dot1x_enabled == NULL) {
		wpa_printf(MSG_DEBUG, "dot1x_enabled == NULL, optional attribute");
		return 0; /* optional attribute */
	}
	wpa_printf(MSG_DEBUG, "WPS: 802.1X Enabled: %d", *dot1x_enabled);
	return 0;
}


static int wps_process_cred_ap_channel(struct wps_credential *cred,
									   const u8 *ap_channel)
{
	if (ap_channel == NULL) {
		wpa_printf(MSG_DEBUG, "ap_channel == NULL, optional attribute");
		return 0; /* optional attribute */
	}
	cred->ap_channel = WPA_GET_BE16(ap_channel);
	wpa_printf(MSG_DEBUG, "WPS: AP Channel: %u", cred->ap_channel);

	return 0;
}

static int has_ctrl_char(const u8 *data, size_t len)
{
	size_t i;

	for (i = 0; i < len; i++) {
		if (data[i] < 32 || data[i] == 127) {
			return 1;
		}
	}
	return 0;
}

static int wps_workaround_cred_key(struct wps_credential *cred)
{
	if (cred->auth_type & (WPS_AUTH_WPAPSK | WPS_AUTH_WPA2PSK) &&
		cred->key_len > 8 && cred->key_len < 64 &&
		cred->key[cred->key_len - 1] == 0) {
		/*
		* A deployed external registrar is known to encode ASCII
		* passphrases incorrectly. Remove the extra NULL termination
		* to fix the encoding.
		*/
		wpa_printf(MSG_INFO, "WPS: Workaround - remove NULL termination from ASCII passphrase");
		cred->key_len--;
	}

	if (cred->auth_type & (WPS_AUTH_WPAPSK | WPS_AUTH_WPA2PSK) &&
		(cred->key_len < 8 || has_ctrl_char(cred->key, cred->key_len))) {
		wpa_printf(MSG_INFO, "WPS: Reject credential with invalid WPA/WPA2-Personal passphrase");
		wpa_hexdump_ascii_key(MSG_INFO, "WPS: Network Key",
							  cred->key, cred->key_len);
		return -1;
	}

	return 0;
}


int wps_process_cred(struct wps_parse_attr *attr,
					 struct wps_credential *cred)
{
	wpa_printf(MSG_DEBUG, "WPS: Process Credential");

	/* TODO: support multiple Network Keys */
	if (wps_process_cred_network_idx(cred, attr->network_idx) ||
		wps_process_cred_ssid(cred, attr->ssid, attr->ssid_len) ||
		wps_process_cred_auth_type(cred, attr->auth_type) ||
		wps_process_cred_encr_type(cred, attr->encr_type) ||
		wps_process_cred_network_key_idx(cred, attr->network_key_idx) ||
		wps_process_cred_network_key(cred, attr->network_key,
									 attr->network_key_len) ||
		wps_process_cred_mac_addr(cred, attr->mac_addr) ||
		wps_process_cred_eap_type(cred, attr->eap_type,
								  attr->eap_type_len) ||
		wps_process_cred_eap_identity(cred, attr->eap_identity,
									  attr->eap_identity_len) ||
		wps_process_cred_key_prov_auto(cred, attr->key_prov_auto) ||
		wps_process_cred_802_1x_enabled(cred, attr->dot1x_enabled) ||
		wps_process_cred_ap_channel(cred, attr->ap_channel)) {

		wpa_printf(MSG_INFO, "WPS: Process Credential fail");
		return -1;
	}
	return wps_workaround_cred_key(cred);
}


int wps_process_ap_settings(struct wps_parse_attr *attr,
							struct wps_credential *cred)
{
	wpa_printf(MSG_DEBUG, "WPS: Processing AP Settings");
	os_memset(cred, 0, sizeof(*cred));
	/* TODO: optional attributes New Password and Device Password ID */
	if (wps_process_cred_ssid(cred, attr->ssid, attr->ssid_len) ||
		wps_process_cred_auth_type(cred, attr->auth_type) ||
		wps_process_cred_encr_type(cred, attr->encr_type) ||
		wps_process_cred_network_key_idx(cred, attr->network_key_idx) ||
		wps_process_cred_network_key(cred, attr->network_key,
									 attr->network_key_len) ||
		wps_process_cred_mac_addr(cred, attr->mac_addr)) {
		wpa_printf(MSG_INFO, "WPS: Processing AP Settings fail");
		return -1;
	}
	return wps_workaround_cred_key(cred);
}
//#endif
