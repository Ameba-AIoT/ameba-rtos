/*
 * OS specific functions for UNIX/POSIX systems
 * Copyright (c) 2005-2009, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

//#ifdef CONFIG_WPS
#include "utils/os.h"

WLAN_ROM_TEXT_SECTION
u8 *WPS_realloc(u8 *old_buf, u32 old_sz, u32 new_sz)
{
	u8 *new_buf = NULL;
	new_buf = os_malloc(new_sz);
	if (new_buf) {
		memset(new_buf, 0, new_sz);
		memcpy((void *)new_buf, (void *)old_buf, old_sz);
	}
	os_free(old_buf, old_sz);
	return new_buf;
}

WLAN_ROM_TEXT_SECTION
void *os_zalloc(size_t size)
{
	void *ptr = (void *)os_malloc(size);
	if (ptr) {
		memset(ptr, 0, size);
	}
	return ptr;
}

//#endif
