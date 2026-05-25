#ifndef SDN_COEX_DBG_H
#define SDN_COEX_DBG_H

#define TAG_SDN_COEX    "SDN_COEX"

#define SDN_COEX_DEBUG_ON   (1)
#define SDN_COEX_DEBUG_OFF  (0)

#define SDN_COEX_DEBUG_EN   SDN_COEX_DEBUG_OFF

#if (SDN_COEX_DEBUG_EN == SDN_COEX_DEBUG_ON)
#define SDN_COEX_LOGA(format, ...) RTK_LOGS( TAG_SDN_COEX, RTK_LOG_ALWAYS, format, ##__VA_ARGS__ );
#define SDN_COEX_LOGD(format, ...) RTK_LOGS( TAG_SDN_COEX, RTK_LOG_DEBUG, format, ##__VA_ARGS__ );
#define SDN_COEX_LOGW(format, ...) RTK_LOGS( TAG_SDN_COEX, RTK_LOG_WARN, format, ##__VA_ARGS__ );
#define SDN_COEX_LOGE(format, ...) RTK_LOGS( TAG_SDN_COEX, RTK_LOG_ERROR, format, ##__VA_ARGS__ );
#define SDN_COEX_DUMPA(str, buf, len)   do {            \
	int i = 0;                                          \
	if (!buf || !len) {	break;}                         \
	RTK_LOGS(TAG_SDN_COEX, RTK_LOG_ALWAYS, str);        \
	for (i = 0; i < len; i++) {                         \
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "%02x ", *(((uint8_t *)buf) + i));  \
		if ((i + 1) % 16 == 0 || (i + 1) == len) {      \
			RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\r\n");    \
		}   \
	}   \
}while(0);
#else
#define SDN_COEX_LOGA(format, ...)
#define SDN_COEX_LOGD(format, ...)
#define SDN_COEX_LOGW(format, ...)
#define SDN_COEX_LOGE(format, ...)
#define SDN_COEX_DUMPA(str, buf, len)
#endif

#endif