#include "whc_dev.h"
#include "lwip/sys.h"
#include "lwip_netconf.h"
#include "os_wrapper.h"
#include "whc_dev_rtw_cli_cmd_define.h"
#include "whc_dev_rtw_cli_cmd_parse.h"

void whc_rtw_cli_send_to_host(u32 cmd_category, u8 cmd_id,
							  u8 *user_data, u32 user_data_len)
{
	u8 *buf = NULL;
	u8 *buf_p = NULL;

	//user data + cmd
	buf = rtos_mem_zmalloc(user_data_len + 5);
	if (buf == NULL) {
		RTK_LOGI(TAG_WLAN_INIC, "%s, alloc whc_rtw_cli_send_to_host fail\n", __FUNCTION__);
		return;
	}

	buf_p = buf;
	*(u32 *)buf_p = cmd_category;
	buf_p += 4;
	*buf_p = cmd_id;
	buf_p += 1;

	if (user_data_len > 0) {
		memcpy(buf_p, user_data, user_data_len);
	}

	//user data + cmd
	whc_dev_api_send_to_host(buf, user_data_len + 5);

	if (buf != NULL) {
		rtos_mem_free((void *)buf);
	}

}

void whc_rtw_cli_join_status_hdl(u8 *evt_info)
{
	struct rtw_event_join_status_info *join_status_info = (struct rtw_event_join_status_info *)evt_info;
	u8 *p_event_status = NULL;
	size_t rem = 0;
	int n = 0;
	char *p = NULL;

	if (join_status_info == NULL) {
		RTK_LOGI(TAG_WLAN_INIC, "%s, Event join status err\n", __func__);
		return;
	}
	RTK_LOGI(TAG_WLAN_INIC, "%s, status: %d\n", __func__, join_status_info->status);


	//Event status + cmd
	p_event_status = rtos_mem_zmalloc(REPORT_EVENT_JOIN_STATUS_LEN);
	if (p_event_status == NULL) {
		RTK_LOGI(TAG_WLAN_INIC, "%s, alloc p_event_status fail\n", __FUNCTION__);
		return;
	}
	rem = REPORT_EVENT_JOIN_STATUS_LEN;
	p = (char *)p_event_status;

	const char *prefix = "stauts: %d, ";
	n = snprintf(p, rem, prefix, join_status_info->status);
	p += n;
	rem -= n;

	switch (join_status_info->status) {
	case RTW_JOINSTATUS_STARTING:
		n = snprintf(p, rem, "%s, ", "RTW_JOINSTATUS_STARTING");
		break;
	case RTW_JOINSTATUS_SCANNING:
		n = snprintf(p, rem, "%s, ", "RTW_JOINSTATUS_SCANNING");
		break;
	case RTW_JOINSTATUS_AUTHENTICATING:
		n = snprintf(p, rem, "%s, ", "RTW_JOINSTATUS_AUTHENTICATING");
		break;
	case RTW_JOINSTATUS_AUTHENTICATED:
		n = snprintf(p, rem, "%s, ", "RTW_JOINSTATUS_AUTHENTICATED");
		break;
	case RTW_JOINSTATUS_ASSOCIATING:
		n = snprintf(p, rem, "%s, ", "RTW_JOINSTATUS_ASSOCIATING");
		break;
	case RTW_JOINSTATUS_ASSOCIATED:
		n = snprintf(p, rem, "%s, ", "RTW_JOINSTATUS_ASSOCIATED");
		break;
	case RTW_JOINSTATUS_4WAY_HANDSHAKING:
		n = snprintf(p, rem, "%s, ", "RTW_JOINSTATUS_4WAY_HANDSHAKING");
		break;
	case RTW_JOINSTATUS_4WAY_HANDSHAKE_DONE:
		n = snprintf(p, rem, "%s, ", "RTW_JOINSTATUS_4WAY_HANDSHAKE_DONE");
		break;
	case RTW_JOINSTATUS_SUCCESS:
		n = snprintf(p, rem, "%s, ", "RTW_JOINSTATUS_SUCCESS");
		break;
	case RTW_JOINSTATUS_FAIL:
		n = snprintf(p, rem, "%s ", "RTW_JOINSTATUS_FAIL");
		p += n;
		rem -= n;
		struct rtw_event_join_fail *join_fail = &join_status_info->priv.fail;

		switch (join_fail->fail_reason) {
		case -RTK_ERR_WIFI_CONN_SCAN_FAIL:
			n = snprintf(p, rem, "%s, ", "(Can not found target AP)");
			break;
		case -RTK_ERR_WIFI_CONN_INVALID_KEY:
			n = snprintf(p, rem, "%s, ", "(Password format wrong)");
			break;
		case -RTK_ERR_WIFI_CONN_AUTH_PASSWORD_WRONG:
		case -RTK_ERR_WIFI_CONN_4WAY_PASSWORD_WRONG:
			n = snprintf(p, rem, "%s, ", "(Password may wrong)");
			break;
		case -RTK_ERR_WIFI_CONN_AUTH_FAIL:
			n = snprintf(p, rem, "%s, ", "(Auth fail)");
			break;
		case -RTK_ERR_WIFI_CONN_ASSOC_FAIL:
			n = snprintf(p, rem, "%s, ", "(Assoc fail)");
			break;
		case -RTK_ERR_WIFI_CONN_4WAY_HANDSHAKE_FAIL:
			n = snprintf(p, rem, "%s, ", "(4 way handshake fail)");
			break;
		default:
			n = snprintf(p, rem, "%s, ", "(unknow)");
			break;
		}

		p += n;
		rem -= n;

		/*Get more detail fail info*/
		if (join_fail->fail_reason == -RTK_ERR_WIFI_CONN_AUTH_FAIL ||
			join_fail->fail_reason == -RTK_ERR_WIFI_CONN_ASSOC_FAIL ||
			join_fail->fail_reason == -RTK_ERR_WIFI_CONN_AUTH_PASSWORD_WRONG) {
			/*status code is valid during auth and assoc*/
			const char *prefix = " status code = %d, ";
			n = snprintf(p, rem, prefix, join_fail->reason_or_status_code);

		} else if (join_fail->fail_reason == -RTK_ERR_WIFI_CONN_4WAY_HANDSHAKE_FAIL ||
				   join_fail->fail_reason == -RTK_ERR_WIFI_CONN_4WAY_PASSWORD_WRONG) {
			/*reason code is valid after assoc success*/
			const char *prefix = " reason code = %d, ";
			n = snprintf(p, rem, prefix, join_fail->reason_or_status_code);
		}

		break;
	case RTW_JOINSTATUS_DISCONNECT:
		n = snprintf(p, rem, "%s, ", "RTW_JOINSTATUS_DISCONNECT");

		p += n;
		rem -= n;

		struct rtw_event_disconnect *disconnect = &join_status_info->priv.disconnect;

		RTK_LOGI(TAG_WLAN_INIC, "Disconnect, reason = %d\n", disconnect->disconn_reason);
		const char *prefix = " reason = %d, ";
		n = snprintf(p, rem, prefix, disconnect->disconn_reason);

		p += n;
		rem -= n;

		/*Get more detail disconnect info*/
		if (disconnect->disconn_reason < RTW_DISCONN_RSN_DRV_BASE) {

		} else {
			switch (disconnect->disconn_reason) {
			case RTW_DISCONN_RSN_DRV_AP_LOSS:
				n = snprintf(p, rem, "%s", " Disconnect by Driver, detect AP loss");
				break;
			case RTW_DISCONN_RSN_DRV_AP_CHANGE:
				n = snprintf(p, rem, "%s", " Disconnect by Driver, detect AP change");
				break;
			case RTW_DISCONN_RSN_APP_DISCONN:
				n = snprintf(p, rem, "%s", " Disconnect by APP call disconnect API");
				break;
			case RTW_DISCONN_RSN_APP_CONN_WITHOUT_DISCONN:
				n = snprintf(p, rem, "%s", " Disconnect by APP call connect without calling disconnect first");
				break;
			}
		}

		break;
	default:
		break;
	}

	p += n;
	rem -= n;

	whc_rtw_cli_send_to_host(WHC_WPA_OPS_EVENT, WHC_WPA_OPS_EVENT_JOIN_STATUS,
							 p_event_status, REPORT_EVENT_JOIN_STATUS_LEN);

	if (p_event_status != NULL) {
		rtos_mem_free((void *)p_event_status);
	}
}


struct rtw_event_hdl_func_t event_external_hdl[1] = {
	{RTW_EVENT_JOIN_STATUS,			whc_rtw_cli_join_status_hdl},
};
u16 array_len_of_event_external_hdl = sizeof(event_external_hdl) / sizeof(struct rtw_event_hdl_func_t);
