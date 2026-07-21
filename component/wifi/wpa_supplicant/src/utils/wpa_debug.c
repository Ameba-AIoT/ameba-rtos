/*
 * wpa_supplicant/hostapd / Debug prints
 * Copyright (c) 2002-2007, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */
#include "utils/includes.h"

#ifndef CONFIG_NO_STDOUT_DEBUG
//#include <stdarg.h>

#include "utils/common.h"
/* Debugging function - conditional DiagPrintf and hex dump. Driver wrappers can
 * use these for debugging purposes. */

//static inline int wpa_debug_reopen_file(void)
//{
//	return 0;
//}

int wpa_debug_level = MSG_INFO;
void wpa_hexdump_key(int level, const char *title, const void *buf, size_t len)
{
	size_t i;
	u8 *pbuf = (u8 *)buf;

	if (level < wpa_debug_level) {
		return;
	}

	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "%s - hexdump(len=%d):", title, len);
	if (buf == NULL) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, " [NULL]");
	} else {
		for (i = 0; i < len; i++) {
			if (i % 16 == 0) {
				RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\r\n");
			}
			RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, " %02x", pbuf[i]);
		}
	}
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\n\r");
}

void wpa_hexdump(int level, const char *title, const void *buf, size_t len)
{
	wpa_hexdump_key(level, title, buf, len);
}

void wpa_hexdump_buf(int level, const char *title,
					 const struct wpabuf *buf)
{
	wpa_hexdump_key(level, title, buf ? wpabuf_head(buf) : NULL,
					buf ? wpabuf_len(buf) : 0);
}

#ifndef isprint
#define in_range(c, lo, up)  ((u8)c >= lo && (u8)c <= up)
#define isprint(c)           in_range(c, 0x20, 0x7f)
#endif

void wpa_hexdump_buf_key(int level, const char *title,
						 const struct wpabuf *buf)
{
	wpa_hexdump_key(level, title, buf ? wpabuf_head(buf) : NULL,
					buf ? wpabuf_len(buf) : 0);
}

void wpa_hexdump_ascii(int level, const char *title, const void *buf,
					   size_t len)
{
	size_t i, llen;
	const u8 *pos = buf;
	const size_t line_len = 16;

	if (level < wpa_debug_level) {
		return;
	}

	if (buf == NULL) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "%s - hexdump_ascii(len=%d): [NULL]\n",
				 title, len);
		return;
	}
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "%s - hexdump_ascii(len=%d):\n", title, len);
	while (len) {
		llen = len > line_len ? line_len : len;
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "    ");
		for (i = 0; i < llen; i++) {
			RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, " %02x", pos[i]);
		}
		for (i = llen; i < line_len; i++) {
			RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "   ");
		}
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "   ");
		for (i = 0; i < llen; i++) {
			if (isprint(pos[i])) {
				RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "%c", pos[i]);
			} else {
				RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "_");
			}
		}
		for (i = llen; i < line_len; i++) {
			RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, " ");
		}
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\n");
		pos += llen;
		len -= llen;
	}
}
#endif
