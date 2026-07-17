
/******************************************************************************
 *
 * Copyright(c) 2007 - 2011 Realtek Corporation. All rights reserved.
 *
 * This is ROM code section.
 *
 *
 ******************************************************************************/
#ifndef __ROM_RTW_SHA384_H_
#define __ROM_RTW_SHA384_H_
int wifi_rom_sha384_prf(u8 *key, size_t key_len, char *label, u8 *data, size_t data_len, u8 *buf, size_t buf_len);
int wifi_rom_hmac_sha384_vector(const u8 *key, size_t key_len, size_t num_elem, const u8 *addr[], const u32 *len, u8 *mac);

#endif
