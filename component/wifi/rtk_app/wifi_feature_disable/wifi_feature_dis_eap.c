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
#define __WIFI_FEATURE_DIS_ENTERPRISE_C__

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

#endif  /* CONFIG_EAP */
