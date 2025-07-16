/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#define __WIFI_FEATURE_DIS_EAP_C__

#include "wifi_api_types.h"

#ifdef CONFIG_EAP

int wifi_rom_sha384_prf(u8 *key, size_t key_len, char *label, u8 *data, size_t data_len, u8 *buf, size_t buf_len)
{
	UNUSED(key);
	UNUSED(key_len);
	UNUSED(label);
	UNUSED(data);
	UNUSED(data_len);
	UNUSED(buf);
	UNUSED(buf_len);
	return -1;
}

int wifi_rom_hmac_sha384_vector(const u8 *key, size_t key_len, size_t num_elem, const u8 *addr[], const u32 *len, u8 *mac)
{
	UNUSED(key);
	UNUSED(key_len);
	UNUSED(num_elem);
	UNUSED(addr);
	UNUSED(len);
	UNUSED(mac);
	return -1;
}

u32 gcmp_80211_encrypt(u8 *pframe, u32 wlan_hdr_len, u32 payload_len, u8 *key, u32 key_len)
{
	UNUSED(pframe);
	UNUSED(wlan_hdr_len);
	UNUSED(payload_len);
	UNUSED(key);
	UNUSED(key_len);
	return 0;
}

u32 gcmp_80211_decrypt(u8 *pframe, u32 wlan_hdr_len, u32 frame_len, u8 *key, u32 key_len)
{
	UNUSED(pframe);
	UNUSED(wlan_hdr_len);
	UNUSED(frame_len);
	UNUSED(key);
	UNUSED(key_len);
	return 0;
}

u8 bip_gcmp_protect(u8 *whdr_pos, size_t len, const u8 *key, size_t key_len, const u8 *data, size_t data_len, u8 *mic)
{
	UNUSED(whdr_pos);
	UNUSED(len);
	UNUSED(key);
	UNUSED(key_len);
	UNUSED(data);
	UNUSED(data_len);
	UNUSED(mic);
	return 0;
}

int eap_start(char *method)
{
	UNUSED(method);
	return -1;
}

void eap_autoreconnect_hdl(u8 method_id)
{
	UNUSED(method_id);
}

void eap_disconnected_hdl(void)
{
}

int get_eap_phase(void)
{
	return 0;
}

void eap_eapol_start_hdl(u8 *buf, s32 buf_len)
{
	UNUSED(buf);
	UNUSED(buf_len);
}

void eap_eapol_recvd_hdl(u8 *buf, s32 buf_len)
{
	UNUSED(buf);
	UNUSED(buf_len);
}


#endif  /* CONFIG_EAP */
