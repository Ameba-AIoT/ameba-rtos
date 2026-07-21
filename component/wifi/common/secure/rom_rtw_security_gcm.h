/******************************************************************************
 *
 * Copyright(c) 2007 - 2014 Realtek Corporation. All rights reserved.
 *
 * This is ROM code section.
 *
 *
 ******************************************************************************/
#ifndef ROM_RTW_SECURITY_GCM_H
#define ROM_RTW_SECURITY_GCM_H

u32 gcmp_80211_encrypt(u8 *pframe, u32 wlan_hdr_len, u32 payload_len, u8 *key, u32 key_len);
u32 gcmp_80211_decrypt(u8 *pframe, u32 wlan_hdr_len, u32 frame_len, u8 *key, u32 key_len);
u8 bip_gcmp_protect(u8 *whdr_pos, size_t len, const u8 *key, size_t key_len, const u8 *data, size_t data_len, u8 *mic);

#endif