#include "example_wifi_tsf_sync.h"

#include "wifi_api.h"
#include "tsf_sync_dev.h"

static const char *const TAG = "TSF_SYNC";

/* Runs on the wifi driver thread, so it must not block. Both ASSOCIATED and
 * SUCCESS are accepted since not every config branch posts both. */
static void tsf_sync_join_status_hdl(u8 *evt_info)
{
	struct rtw_event_join_status_info *info = (struct rtw_event_join_status_info *)evt_info;

	switch (info->status) {
	case RTW_JOINSTATUS_ASSOCIATED:
	case RTW_JOINSTATUS_SUCCESS:
		tsf_sync_dev_start();
		break;

	case RTW_JOINSTATUS_DISCONNECT:
	case RTW_JOINSTATUS_FAIL:
		tsf_sync_dev_stop();
		break;

	default:
		break;
	}
}

/* Overrides the __weak definition in usrcfg/amebagreen2/ameba_wificfg.c. */
struct rtw_event_hdl_func_t event_external_hdl[1] = {
	{RTW_EVENT_JOIN_STATUS, tsf_sync_join_status_hdl},
};
u16 array_len_of_event_external_hdl = sizeof(event_external_hdl) / sizeof(struct rtw_event_hdl_func_t);

void example_wifi_tsf_sync(void)
{
	/* both hooks are compile-time overrides; this log proves the example built in */
	RTK_LOGI(TAG, "tsf_sync example loaded, waiting for STA association\n");
}
