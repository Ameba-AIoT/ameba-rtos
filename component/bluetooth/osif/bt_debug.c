#include <stdio.h>
#include <string.h>
#include <basic_types.h>
#include "ameba.h"
#include "ameba_vector.h"
#include "ameba_loguart.h"
#include <bt_debug.h>


#if defined(BT_LOG_USE_MUTEX) && BT_LOG_USE_MUTEX
void *bt_log_mtx = NULL;
#endif

void rtk_bt_log_init(void)
{
#if defined(BT_LOG_USE_MUTEX) && BT_LOG_USE_MUTEX
	if (bt_log_mtx == NULL) {
		osif_mutex_create(&bt_log_mtx);
	}
#endif
}

void rtk_bt_log_deinit(void)
{
#if defined(BT_LOG_USE_MUTEX) && BT_LOG_USE_MUTEX
	if (bt_log_mtx) {
		osif_mutex_delete(bt_log_mtx);
		bt_log_mtx = NULL;
	}
#endif
}

void rtk_bt_log_dump(uint8_t unit, const char *str, void *buf, uint16_t len)
{
	int i = 0;
	int num = 16 / unit;

	BT_LOG_MUTEX_TAKE
	RTK_LOGS(NOTAG, "%s\r\n", str);
	for (i = 0; i < len; i++) {
		if (unit == 4) {
			RTK_LOGS(NOTAG, "%08x ", *((uint32_t *)buf + i));
		} else if (unit == 2) {
			RTK_LOGS(NOTAG, "%04x ", *((uint16_t *)buf + i));
		} else {
			RTK_LOGS(NOTAG, "%02x ", *((uint8_t *)buf + i));
		}
		if ((i + 1) % num == 0 || (i + 1) == len) {
			RTK_LOGS(NOTAG, "\r\n");
		}
	}
	BT_LOG_MUTEX_GIVE
}

void rtk_bt_fw_log_open(void)
{
	LOGUART_Relay_InitTypeDef LOGUART_Relay_InitStruct;

	LOGUART_AGGCmd(LOGUART_DEV, ENABLE);
	LOGUART_AGGPathCmd(LOGUART_DEV, LOGUART_PATH_INDEX_5, ENABLE);

	LOGUART_Relay_StructInit(&LOGUART_Relay_InitStruct);
	LOGUART_Relay_ClearRxFifo(LOGUART_DEV);
	LOGUART_Relay_SetFormat(LOGUART_DEV, &LOGUART_Relay_InitStruct);
	LOGUART_Relay_SetBaud(LOGUART_DEV, 115200);
	LOGUART_Relay_RxCmd(LOGUART_DEV, ENABLE);
}

void rtk_bt_fw_log_close(void)
{
	LOGUART_Relay_RxCmd(LOGUART_DEV, DISABLE);
	LOGUART_WaitTxComplete();
	LOGUART_AGGPathCmd(LOGUART_DEV, LOGUART_PATH_INDEX_5, DISABLE);
}

void rtk_bt_trace_log_open(void)
{
	LOGUART_AGGCmd(LOGUART_DEV, ENABLE);
	LOGUART_AGGPathCmd(LOGUART_DEV, LOGUART_PATH_INDEX_3, ENABLE);
}

void rtk_bt_trace_log_close(void)
{
	LOGUART_WaitTxComplete();
	LOGUART_AGGPathCmd(LOGUART_DEV, LOGUART_PATH_INDEX_3, DISABLE);
}