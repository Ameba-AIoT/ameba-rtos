#include "whc_dev.h"
#include "lwip/sys.h"
#include "lwip_netconf.h"
#include "os_wrapper.h"
#include "whc_dev_rtw_cli_cmd_define.h"
#include "whc_dev_rtw_cli_cmd_parse.h"

#ifdef CONFIG_WPA_LITE
#include "whc_wpa_lite_app.h"
#endif


extern rtos_sema_t whc_user_rx_sema;
extern u8 *whc_rx_msg;

__weak int whc_wpa_ops_disconnect(char *ptr, u8 *buf)
{
	(void)ptr;
	(void)buf;

	return 0;
}

__weak int whc_wpa_ops_select_network(char *ptr, u8 *buf)
{
	(void)ptr;
	(void)buf;

	return 0;
}

__weak int whc_wpa_ops_list_network(char *ptr, u8 *buf)
{
	(void)ptr;
	(void)buf;

	return 0;
}

__weak int whc_wpa_ops_set_network(char *ptr, u8 *buf, int msg_len)
{
	(void)ptr;
	(void)buf;
	(void)msg_len;

	return 0;
}

__weak int whc_wpa_ops_get_macaddr(u8 *ptr, u8 *buf)
{
	(void)ptr;
	(void)buf;

	return 0;
}

__weak int whc_wpa_ops_do_scan(u8 *ptr, u8 *buf)
{
	(void)ptr;
	(void)buf;

	return 0;
}

__weak int whc_wpa_ops_init_wpas_std(u8 *ptr, u8 *buf)
{
	(void)ptr;
	(void)buf;

	return 0;
}

__weak int whc_wpa_ops_wifi_on(u8 *ptr, u8 *buf)
{
	(void)ptr;
	(void)buf;

#ifdef CONFIG_WHC_DUAL_TCPIP
	whc_dev_api_set_host_state(WHC_HOST_READY);
#endif
	wifi_on(RTW_MODE_STA);

	return 0;
}


int whc_wpa_ops_cli_cmd_parse(char *ptr, u8 *buf)
{
	int msg_len = 0;
	char *cmd = NULL;
	char *params = NULL;

	msg_len = *ptr;
	ptr += 4;

	cmd = strtok(ptr, " ");
	params = strtok(NULL, "");

	RTK_LOGI(TAG_WLAN_INIC, "%s, cmd: %s, params: %s, msg_len: %d\n", __func__, cmd, params, msg_len);

	if (!cmd) {
		RTK_LOGI(TAG_WLAN_INIC, "%s, No command\n", __func__);
		return 0;
	}

	if (strcmp(cmd, "set_network") == 0) {
		if (params) {
			whc_wpa_ops_set_network(params, buf, msg_len - strlen("set_network") - 1);
		} else {
			RTK_LOGI(TAG_WLAN_INIC, "%s, missing parameter\n", __func__);
		}
	} else if (strcmp(cmd, "disconnect") == 0) {
		whc_wpa_ops_disconnect(params, buf);
	} else if (strcmp(cmd, "list_networks") == 0) {
		whc_wpa_ops_list_network(params, buf);
	} else if (strcmp(cmd, "select_network") == 0) {
		whc_wpa_ops_select_network(params, buf);
	} else if (strcmp(cmd, "status") == 0) {
		whc_wpa_ops_get_status(params, buf);
	} else {
		RTK_LOGI(TAG_WLAN_INIC, "%s, Unknown command\n", __func__);
	}


	return 0;
}


void whc_dev_pkt_rx_to_user_task(void)
{
	u8 *ptr = NULL;
	u32 event = 0;
	u8 *buf = NULL;

	while (1) {
		rtos_sema_take(whc_user_rx_sema, RTOS_MAX_TIMEOUT);
		if (whc_rx_msg) {
			ptr = whc_rx_msg;
			event = *(u32 *)whc_rx_msg;
			ptr += 4;

			buf = rtos_mem_malloc(BRIDGE_WPA_OPS_BUF_SIZE);
			if (!buf) {
				RTK_LOGE(TAG_WLAN_INIC, "%s, can't alloc buffer!!\n", __func__);
				return;
			}

			if (event == WHC_WPA_OPS_UTIL) {
				if (*ptr == WHC_WPA_OPS_UTIL_GET_MAC_ADDR) {
					ptr += 1;
					whc_wpa_ops_get_macaddr(ptr, buf);

				} else if (*ptr == WHC_WPA_OPS_UTIL_SET_NETWORK) {
					ptr += 1;

					whc_wpa_ops_cli_cmd_parse((char *)ptr, buf);
				}
			} else if (event == WHC_WPA_OPS_CUSTOM_API) {
				if (*ptr == WHC_WPA_OPS_CUSTOM_API_SCAN) {
					ptr += 1;
					whc_wpa_ops_do_scan(ptr, buf);

				} else if (*ptr == WHC_WPA_OPS_CUSTOM_API_INIT_WPAS_STD) {
					ptr += 1;

					whc_wpa_ops_init_wpas_std(ptr, buf);

				} else if (*ptr == WHC_WPA_OPS_CUSTOM_API_WIFION) {
					ptr += 1;

					whc_wpa_ops_wifi_on(ptr, buf);
				}
			}

			if (buf != NULL) {
				rtos_mem_free(buf);
			}

			whc_rx_msg = NULL;
		}
	}

}
