#include "example_wifi_event.h"
#include "wifi_api.h"
#include "lwip_netconf.h"
#include "os_wrapper.h"

static const char *const TAG = "RTW_EVENT_EXAMPLE";

struct rtw_event_hdl_func_t event_external_hdl[1] = {
	{RTW_EVENT_JOIN_STATUS,			exampe_wifi_join_status_event_hdl},
};
u16 array_len_of_event_external_hdl = sizeof(event_external_hdl) / sizeof(struct rtw_event_hdl_func_t);

void exampe_wifi_join_status_event_hdl(u8 *buf, s32 buf_len, s32 flags)
{
	UNUSED(buf_len);
	u8 join_status = (u8)flags;
	struct rtw_event_join_status_info *evt_info = (struct rtw_event_join_status_info *)buf;
	struct rtw_event_join_fail *join_fail;
	struct rtw_event_disconnect *disconnect;

	if (join_status == RTW_JOINSTATUS_SUCCESS) {
		RTK_LOGI(TAG, "Join success\n");
		return;
	}

	/*Get join fail reason*/
	if (join_status == RTW_JOINSTATUS_FAIL) {/*Include 4 way handshake but not include DHCP*/
		join_fail = &evt_info->private.fail;
		RTK_LOGI(TAG, "Join fail, reason = %d ", join_fail->fail_reason);/*definition in enum int*/
		switch (join_fail->fail_reason) {
		case -RTK_ERR_WIFI_CONN_SCAN_FAIL:
			RTK_LOGI(NOTAG, "(Can not found target AP)\n");
			break;
		case -RTK_ERR_WIFI_CONN_INVALID_KEY:
			RTK_LOGI(NOTAG, "(Password format wrong)\n");
			break;
		case -RTK_ERR_WIFI_CONN_AUTH_PASSWORD_WRONG:
		case -RTK_ERR_WIFI_CONN_4WAY_PASSWORD_WRONG:
			RTK_LOGI(NOTAG, "(Password may wrong)\n");/*Not entirely accurate*/
			break;
		case -RTK_ERR_WIFI_CONN_AUTH_FAIL:
			RTK_LOGI(NOTAG, "(Auth fail)\n");
			break;
		case -RTK_ERR_WIFI_CONN_ASSOC_FAIL:
			RTK_LOGI(NOTAG, "(Assoc fail)\n");
			break;
		case -RTK_ERR_WIFI_CONN_4WAY_HANDSHAKE_FAIL:
			RTK_LOGI(NOTAG, "(4 way handshake fail)\n");
			break;
		default:
			RTK_LOGI(NOTAG, "(unknow)\n");
			break;
		}
		/*Get more detail fail info*/
		if (join_fail->fail_reason == -RTK_ERR_WIFI_CONN_AUTH_FAIL || join_fail->fail_reason == -RTK_ERR_WIFI_CONN_ASSOC_FAIL ||
			join_fail->fail_reason == -RTK_ERR_WIFI_CONN_AUTH_PASSWORD_WRONG) {
			/*status code is valid during auth and assoc*/
			RTK_LOGI(TAG, "status code from AP = %d\n", join_fail->reason_or_status_code);
		} else if (join_fail->fail_reason == -RTK_ERR_WIFI_CONN_4WAY_HANDSHAKE_FAIL || -RTK_ERR_WIFI_CONN_4WAY_PASSWORD_WRONG) {
			/*reason code is valid after assoc success*/
			RTK_LOGI(TAG, "reason code from AP = %d\n", join_fail->reason_or_status_code);
		}
		return;
	}

	/*Get disconnect reason*/
	if (join_status == RTW_JOINSTATUS_DISCONNECT) {
		disconnect = &evt_info->private.disconnect;
		RTK_LOGI(TAG, "Disconnect, reason = %d\n", disconnect->disconn_reason);
		/*Get more detail disconnect info*/
		if (disconnect->disconn_reason < RTW_DISCONN_RSN_DRV_BASE) {
			RTK_LOGI(TAG, "Disconnect by AP, reason code =%d\n", disconnect->disconn_reason);
		} else {
			switch (disconnect->disconn_reason) {
			case RTW_DISCONN_RSN_DRV_AP_LOSS:
				RTK_LOGI(TAG, "Disconnect by Driver, detect AP loss\n");
				break;
			case RTW_DISCONN_RSN_DRV_AP_CHANGE:
				RTK_LOGI(TAG, "Disconnect by Driver, detect AP change\n");
				break;
			case RTW_DISCONN_RSN_APP_DISCONN:
				RTK_LOGI(TAG, "Disconnect by APP call disconnect API\n");
				break;
			case RTW_DISCONN_RSN_APP_CONN_WITHOUT_DISCONN:
				RTK_LOGI(TAG, "Disconnect by APP call connect without calling disconnect first\n");
				break;
			}
		}
	}
}

static void example_main_task(void *param)
{
	(void) param;

	RTK_LOGI(TAG, "start\n");

	rtos_task_delete(NULL);
}

void example_wifi_event(void)
{
	if (rtos_task_create(NULL, ((const char *)"example_main_task"), example_main_task, NULL, 1024 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGI(TAG, "\n%s rtos_task_create failed\n", __FUNCTION__);
	}
}

