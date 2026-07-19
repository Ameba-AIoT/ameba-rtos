/*
 * Wi-Fi Protected Setup - common functionality
 * Copyright (c) 2008-2012, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/includes.h"
#include "utils/common.h"

#include "crypto/crypto.h"
#include "crypto/dh_group5.h"
#include "crypto/rom/rom_wps_sha_256.h"
#include "crypto/rom/rom_wps_aes-cbc.h"
#include "crypto/random.h"
#include "wps/wps_i.h"

//#ifdef CONFIG_WPS
void wps_kdf(const u8 *key, const u8 *label_prefix, size_t label_prefix_len,
			 const char *label, u8 *res, size_t res_len)
{
	u8 i_buf[4], key_bits[4];
	const u8 *addr[4];
	size_t len[4];
	int i, iter;
	u8 hash[SHA256_MAC_LEN], *opos;
	size_t left;

	WPA_PUT_BE32(key_bits, res_len * 8);

	addr[0] = i_buf;
	len[0] = sizeof(i_buf);
	addr[1] = label_prefix;
	len[1] = label_prefix_len;
	addr[2] = (const u8 *) label;
	len[2] = os_strlen(label);
	addr[3] = key_bits;
	len[3] = sizeof(key_bits);

	iter = (res_len + SHA256_MAC_LEN - 1) / SHA256_MAC_LEN;
	opos = res;
	left = res_len;

	wpa_printf(MSG_DEBUG, "key = kdk");
	for (i = 1; i <= iter; i++) {
		WPA_PUT_BE32(i_buf, i);
		rom_hmac_sha256_vector(key, SHA256_MAC_LEN, 4, addr, len, hash);
		if (i < iter) {
			os_memcpy(opos, hash, SHA256_MAC_LEN);
			opos += SHA256_MAC_LEN;
			left -= SHA256_MAC_LEN;
		} else {
			os_memcpy(opos, hash, left);
		}
	}
}

int wps_derive_keys(struct wps_data *wps)
{
	struct wpabuf *pubkey, *dh_shared;
	u8 dhkey[SHA256_MAC_LEN], kdk[SHA256_MAC_LEN];
	const u8 *addr[3];
	size_t len[3];
	u8 keys[WPS_AUTHKEY_LEN + WPS_KEYWRAPKEY_LEN + WPS_EMSK_LEN];
//	int i;


	if (wps->dh_privkey == NULL) {
		wpa_printf(MSG_INFO, "WPS: Own DH private key not available");
		return -1;
	}
	wpa_printf(MSG_DEBUG, "wps->registrar = 0x%x", wps->registrar);
	pubkey = wps->registrar ? wps->dh_pubkey_e : wps->dh_pubkey_r;
	if (pubkey == NULL) {
		wpa_printf(MSG_INFO, "WPS: Peer DH public key not available");
		return -1;
	}

	//wpa_printf(MSG_DEBUG, "WPS: DH Private Key(STA)");
	//for (i = 0; i < 192; i+=16) {
	//	wpa_printf(MSG_DEBUG, "0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x",wps->dh_privkey->buf[i],wps->dh_privkey->buf[i+1],wps->dh_privkey->buf[i+2],wps->dh_privkey->buf[i+3],wps->dh_privkey->buf[i+4],wps->dh_privkey->buf[i+5],wps->dh_privkey->buf[i+6],wps->dh_privkey->buf[i+7],wps->dh_privkey->buf[i+8],wps->dh_privkey->buf[i+9],wps->dh_privkey->buf[i+10],wps->dh_privkey->buf[i+11],wps->dh_privkey->buf[i+12],wps->dh_privkey->buf[i+13],wps->dh_privkey->buf[i+14],wps->dh_privkey->buf[i+15]);
	//}

	//wpa_printf(MSG_DEBUG, "WPS: DH peer Public Key");
	//for (i = 0; i < 192; i+=16) {
	//	wpa_printf(MSG_DEBUG, "0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x",pubkey->buf[i],pubkey->buf[i+1],pubkey->buf[i+2],pubkey->buf[i+3],pubkey->buf[i+4],pubkey->buf[i+5],pubkey->buf[i+6],pubkey->buf[i+7],pubkey->buf[i+8],pubkey->buf[i+9],pubkey->buf[i+10],pubkey->buf[i+11],pubkey->buf[i+12],pubkey->buf[i+13],pubkey->buf[i+14],pubkey->buf[i+15]);
	//}

	wpa_hexdump_buf_key(MSG_MSGDUMP, "WPS: DH Private Key", wps->dh_privkey);
	wpa_hexdump_buf(MSG_MSGDUMP, "WPS: DH peer Public Key", pubkey);

	dh_shared = dh5_derive_shared(wps->dh_ctx, pubkey, wps->dh_privkey);
	dh5_free(wps->dh_ctx);
	wps->dh_ctx = NULL;
	dh_shared = wpabuf_zeropad(dh_shared, 192);
	if (dh_shared == NULL) {
		wpa_printf(MSG_DEBUG, "WPS: Failed to derive DH shared key");
		return -1;
	}

	/* Own DH private key is not needed anymore */
	wpabuf_free(wps->dh_privkey);
	wps->dh_privkey = NULL;

	wpa_hexdump_buf_key(MSG_MSGDUMP, "WPS: DH shared key", dh_shared);


	/* DHKey = SHA-256(g^AB mod p) */
	wpa_printf(MSG_DEBUG, "DHKey = SHA-256(g^AB mod p)");
	addr[0] = wpabuf_head(dh_shared);
	len[0] = wpabuf_len(dh_shared);
	wifi_rom_sha256_vector(1, addr, len, dhkey);
	wpa_hexdump_key(MSG_MSGDUMP, "WPS: DHKey", dhkey, sizeof(dhkey));

	wpabuf_free(dh_shared);
	/* KDK = HMAC-SHA-256_DHKey(N1 || EnrolleeMAC || N2) */
	wpa_printf(MSG_DEBUG, "KDK = HMAC-SHA-256_DHKey(N1 || EnrolleeMAC || N2)");
	addr[0] = wps->nonce_e;
	len[0] = WPS_NONCE_LEN;
	addr[1] = wps->mac_addr_e;
	len[1] = ETH_ALEN;
	addr[2] = wps->nonce_r;
	len[2] = WPS_NONCE_LEN;
	wpa_printf(MSG_DEBUG, "key = dhkey");
	rom_hmac_sha256_vector(dhkey, sizeof(dhkey), 3, addr, len, kdk);
	wpa_hexdump_key(MSG_MSGDUMP, "WPS: KDK", kdk, sizeof(kdk));
	//for (i = 0; i < 32; i+=16) {
	//	wpa_printf(MSG_DEBUG, "0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x",kdk[i],kdk[i+1],kdk[i+2],kdk[i+3],kdk[i+4],kdk[i+5],kdk[i+6],kdk[i+7],kdk[i+8],kdk[i+9],kdk[i+10],kdk[i+11],kdk[i+12],kdk[i+13],kdk[i+14],kdk[i+15]);
	//}
	wps_kdf(kdk, NULL, 0, "Wi-Fi Easy and Secure Key Derivation",
			keys, sizeof(keys));
	os_memcpy(wps->authkey, keys, WPS_AUTHKEY_LEN);
	os_memcpy(wps->keywrapkey, keys + WPS_AUTHKEY_LEN, WPS_KEYWRAPKEY_LEN);
	os_memcpy(wps->emsk, keys + WPS_AUTHKEY_LEN + WPS_KEYWRAPKEY_LEN,
			  WPS_EMSK_LEN);

	wpa_hexdump_key(MSG_MSGDUMP, "WPS: AuthKey",
					wps->authkey, WPS_AUTHKEY_LEN);
	//wpa_printf(MSG_DEBUG, "WPS: AuthKey");
	//for (i = 0; i < WPS_AUTHKEY_LEN; i+=16) {
	//	wpa_printf(MSG_DEBUG, "0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x",wps->authkey[i],wps->authkey[i+1],wps->authkey[i+2],wps->authkey[i+3],wps->authkey[i+4],wps->authkey[i+5],wps->authkey[i+6],wps->authkey[i+7],wps->authkey[i+8],wps->authkey[i+9],wps->authkey[i+10],wps->authkey[i+11],wps->authkey[i+12],wps->authkey[i+13],wps->authkey[i+14],wps->authkey[i+15]);
	//}
	wpa_hexdump_key(MSG_MSGDUMP, "WPS: KeyWrapKey",
					wps->keywrapkey, WPS_KEYWRAPKEY_LEN);
	//wpa_printf(MSG_DEBUG, "WPS: KeyWrapKey");
	//for (i = 0; i < WPS_KEYWRAPKEY_LEN; i+=16) {
	//	wpa_printf(MSG_DEBUG, "0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x",wps->keywrapkey[i],wps->keywrapkey[i+1],wps->keywrapkey[i+2],wps->keywrapkey[i+3],wps->keywrapkey[i+4],wps->keywrapkey[i+5],wps->keywrapkey[i+6],wps->keywrapkey[i+7],wps->keywrapkey[i+8],wps->keywrapkey[i+9],wps->keywrapkey[i+10],wps->keywrapkey[i+11],wps->keywrapkey[i+12],wps->keywrapkey[i+13],wps->keywrapkey[i+14],wps->keywrapkey[i+15]);
	//}
	wpa_hexdump_key(MSG_MSGDUMP, "WPS: EMSK", wps->emsk, WPS_EMSK_LEN);
	//wpa_printf(MSG_DEBUG, "WPS: EMSK");
	//for (i = 0; i < WPS_EMSK_LEN; i+=16) {
	//	wpa_printf(MSG_DEBUG, "0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x",wps->emsk[i],wps->emsk[i+1],wps->emsk[i+2],wps->emsk[i+3],wps->emsk[i+4],wps->emsk[i+5],wps->emsk[i+6],wps->emsk[i+7],wps->emsk[i+8],wps->emsk[i+9],wps->emsk[i+10],wps->emsk[i+11],wps->emsk[i+12],wps->emsk[i+13],wps->emsk[i+14],wps->emsk[i+15]);
	//}
	return 0;
}


void wps_derive_psk(struct wps_data *wps, const u8 *dev_passwd,
					size_t dev_passwd_len)
{
	u8 hash[SHA256_MAC_LEN];
//	int i;

	rom_hmac_sha256(wps->authkey, WPS_AUTHKEY_LEN, dev_passwd,
					(dev_passwd_len + 1) / 2, hash);
	os_memcpy(wps->psk1, hash, WPS_PSK_LEN);
	rom_hmac_sha256(wps->authkey, WPS_AUTHKEY_LEN,
					dev_passwd + (dev_passwd_len + 1) / 2,
					dev_passwd_len / 2, hash);
	os_memcpy(wps->psk2, hash, WPS_PSK_LEN);

	wpa_hexdump_ascii_key(MSG_DEBUG, "WPS: Device Password",
						  dev_passwd, dev_passwd_len);
	//wpa_printf(MSG_DEBUG, "WPS: Device Password");
	//for (i = 0; i < dev_passwd_len; i+=8) {
	//	wpa_printf(MSG_DEBUG, "0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x",dev_passwd[i],dev_passwd[i+1],dev_passwd[i+2],dev_passwd[i+3],dev_passwd[i+4],dev_passwd[i+5],dev_passwd[i+6],dev_passwd[i+7]);
	//}
	wpa_hexdump_key(MSG_MSGDUMP, "WPS: PSK1", wps->psk1, WPS_PSK_LEN);

	wpa_hexdump_key(MSG_MSGDUMP, "WPS: PSK2", wps->psk2, WPS_PSK_LEN);

}

// wps = data->wps, msg = data->in_buf , attr(parsered msg)
struct wpabuf *wps_decrypt_encr_settings(struct wps_data *wps, const u8 *encr,
		size_t encr_len)
{
	struct wpabuf *decrypted;
	const size_t block_size = 16;
	size_t i;
//	int j;
	u8 pad;
	const u8 *pos;

	/* AES-128-CBC */
	if (encr == NULL || encr_len < 2 * block_size || encr_len % block_size) {
		wpa_printf(MSG_INFO, "WPS: No Encrypted Settings received");
		return NULL;
	}

	//			0x40 - 0x10
	decrypted = wpabuf_alloc(encr_len - block_size);
	if (decrypted == NULL) {
		wpa_printf(MSG_INFO, "allocate buf for decrypted fail");
		return NULL;
	}
	wpa_hexdump(MSG_MSGDUMP, "WPS: Encrypted Settings", encr, encr_len);

	//wpa_printf(MSG_DEBUG, "WPS: Encrypted Settings");
	//for (j = 0; j < encr_len; j+=16) {
	//	wpa_printf(MSG_DEBUG, "0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x", encr[j],encr[j+1],encr[j+2],encr[j+3],encr[j+4],encr[j+5],encr[j+6],encr[j+7],encr[j+8],encr[j+9],encr[j+10],encr[j+11],encr[j+12],encr[j+13],encr[j+14],encr[j+15]);
	//}
	//				encr >> 16 B	64 - 16 = 48
	wpabuf_put_data(decrypted, encr + block_size, encr_len - block_size);

	//wpa_printf(MSG_DEBUG, "wps->keywrapkey =");
	//for (j = 0; j < 16; j += 8 ) {
	//	wpa_printf(MSG_DEBUG, "0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x", wps->keywrapkey[j], wps->keywrapkey[j + 1], wps->keywrapkey[j + 2], wps->keywrapkey[j + 3], wps->keywrapkey[j + 4], wps->keywrapkey[j + 5], wps->keywrapkey[j + 6], wps->keywrapkey[j + 7]);
	//}

	if (rom_aes_128_cbc_decrypt(wps->keywrapkey, encr, wpabuf_mhead(decrypted),
								wpabuf_len(decrypted))) {
		wpabuf_free(decrypted);
		return NULL;
	}

	wpa_hexdump_buf_key(MSG_MSGDUMP, "WPS: Decrypted Encrypted Settings",
						decrypted);
	//wpa_printf(MSG_DEBUG, "WPS: Decrypted Encrypted Settings");
	//for (j = 0; j < decrypted->size; j+=16) {
	//	wpa_printf(MSG_DEBUG, "0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x",decrypted->buf[j],decrypted->buf[j+1],decrypted->buf[j+2],decrypted->buf[j+3],decrypted->buf[j+4],decrypted->buf[j+5],decrypted->buf[j+6],decrypted->buf[j+7],decrypted->buf[j+8],decrypted->buf[j+9],decrypted->buf[j+10],decrypted->buf[j+11],decrypted->buf[j+12],decrypted->buf[j+13],decrypted->buf[j+14],decrypted->buf[j+15]);
	//}

	pos = wpabuf_head_u8(decrypted) + wpabuf_len(decrypted) - 1;
	pad = *pos;

	if (pad > wpabuf_len(decrypted)) {
		wpabuf_free(decrypted);
		wpa_printf(MSG_DEBUG, "WPS: Invalid PKCS#5 v2.0 pad value");
		return NULL;
	}
	for (i = 0; i < pad; i++) {
		if (*pos-- != pad) {
			wpabuf_free(decrypted);
			wpa_printf(MSG_INFO, "WPS: Invalid PKCS#5 v2.0 pad string");
			return NULL;
		}
	}
	decrypted->used -= pad;

	return decrypted;
}


/**
 * wps_pin_checksum - Compute PIN checksum
 * @pin: Seven digit PIN (i.e., eight digit PIN without the checksum digit)
 * Returns: Checksum digit
 */
unsigned int wps_pin_checksum(unsigned int pin)
{
	unsigned int accum = 0;
	while (pin) {
		accum += 3 * (pin % 10);
		pin /= 10;
		accum += pin % 10;
		pin /= 10;
	}

	return (10 - accum % 10) % 10;
}


/**
 * wps_pin_valid - Check whether a PIN has a valid checksum
 * @pin: Eight digit PIN (i.e., including the checksum digit)
 * Returns: 1 if checksum digit is valid, or 0 if not
 */
unsigned int wps_pin_valid(unsigned int pin)
{
	return wps_pin_checksum(pin / 10) == (pin % 10);
}


/**
 * wps_generate_pin - Generate a random PIN
 * Returns: Eight digit PIN (i.e., including the checksum digit)
 */
unsigned int wps_generate_pin(void)
{
	unsigned int val;

	/* Generate seven random digits for the PIN */
#if 0
	/* Generate seven random digits for the PIN */
	if (random_get_bytes((unsigned char *) &val, sizeof(val)) < 0) {
		struct os_time now;
		os_get_time(&now);
		val = os_random() ^ now.sec ^ now.usec;
	}
#else
	os_get_random_bytes(&val, sizeof(val));
#endif
	val %= 10000000;

	/* if val is only six digits */
	if (!(val / 1000000)) {
		val += 3000000;
	}

	/* Append checksum digit */
	return val * 10 + wps_pin_checksum(val);
}


struct wpabuf *wps_build_wsc_ack(struct wps_data *wps)
{
	struct wpabuf *msg;

	wpa_printf(MSG_DEBUG, "WPS: Building Message WSC_ACK");

	msg = wpabuf_alloc(1000);
	if (msg == NULL) {
		return NULL;
	}

	if (wps_build_version(msg) ||
		wps_build_msg_type(msg, WPS_WSC_ACK) ||
		wps_build_enrollee_nonce(wps, msg) ||
		wps_build_registrar_nonce(wps, msg) ||
		wps_build_wfa_ext(msg, 0, NULL, 0)) {
		wpabuf_free(msg);
		return NULL;
	}

	return msg;
}


struct wpabuf *wps_build_wsc_nack(struct wps_data *wps)
{
	struct wpabuf *msg;

	wpa_printf(MSG_DEBUG, "WPS: Building Message WSC_NACK");

	msg = wpabuf_alloc(1000);
	if (msg == NULL) {
		return NULL;
	}

	if (wps_build_version(msg) ||
		wps_build_msg_type(msg, WPS_WSC_NACK) ||
		wps_build_enrollee_nonce(wps, msg) ||
		wps_build_registrar_nonce(wps, msg) ||
		wps_build_config_error(msg, wps->config_error) ||
		wps_build_wfa_ext(msg, 0, NULL, 0)) {
		wpabuf_free(msg);
		return NULL;
	}

	return msg;
}

char *wps_dev_type_bin2str(const u8 dev_type[WPS_DEV_TYPE_LEN], char *buf,
						   size_t buf_len)
{
	int ret;

	ret = os_snprintf(buf, buf_len, "%u-%08X-%u",
					  WPA_GET_BE16(dev_type), (unsigned int)WPA_GET_BE32(&dev_type[2]),
					  WPA_GET_BE16(&dev_type[6]));
	if (ret < 0 || (unsigned int) ret >= buf_len) {
		return NULL;
	}

	return buf;
}

//#endif
