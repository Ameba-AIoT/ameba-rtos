/******************************************************************************
 *
 * Copyright(c) 2007 - 2011 Realtek Corporation. All rights reserved.
 *
 * This is ROM code section.
 *
 *
 ******************************************************************************/
#ifndef __ROM_RTW_SHA256_H_
#define __ROM_RTW_SHA256_H_

int wifi_rom_sha256_vector(size_t num_elem, const u8 *addr[], const size_t *len, u8 *mac);
void wifi_rom_hmac_sha256_vector(const u8 *key, size_t key_len, size_t num_elem, const u8 *addr[], const u32 *len, u8 *mac);
int wifi_rom_hmac_sha256_kdf(const u8 *secret, size_t secret_len, const char *label, const u8 *seed, size_t seed_len, u8 *out, size_t outlen);
int wifi_rom_sha256_prf(u8 *key, size_t key_len, char *label, u8 *data, size_t data_len, u8 *buf, size_t buf_len);
int wifi_rom_sha256_prf_bits(u8 *key, size_t key_len, char *label, u8 *data, size_t data_len, u8 *buf, size_t buf_len_bits);

#endif

