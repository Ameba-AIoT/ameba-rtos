#include "example_wifi_user_reconnect.h"
#include "wifi_conf.h"
#include "lwip_netconf.h"
#include "os_wrapper.h"


static char *test_ssid = "Test_ap";
static char *test_password = "12345678";
static rtos_sema_t sta_disconnect_sema = NULL;

void wifi_join_status_callback(rtw_join_status_t join_status)
{
	if (join_status == RTW_JOINSTATUS_DISCONNECT) {
		printf("\nSTA disconnected\n");
		rtos_sema_give(sta_disconnect_sema);
	}
}

static void user_wifi_reconnect_task(void *param)
{
	int ret = 0;

	rtw_network_info_t connect_param = {0};
	while(1) {
		rtos_sema_take(sta_disconnect_sema, RTOS_SEMA_MAX_COUNT);
		printf("Start reconnect\n");
		memcpy(connect_param.ssid.val, test_ssid, strlen(test_ssid));
		connect_param.ssid.len = strlen(test_ssid);
		connect_param.password = (unsigned char *)test_password;
		connect_param.password_len = strlen(test_password);
		connect_param.security_type = RTW_SECURITY_WPA2_AES_PSK;
		connect_param.joinstatus_user_callback = wifi_join_status_callback;
		ret = wifi_connect(&connect_param, 1);

		if (ret == RTW_SUCCESS) {
			ret = LwIP_DHCP(0, DHCP_START);
			if (ret == DHCP_ADDRESS_ASSIGNED) {
				printf("Reconnect success\n");
			} else {
				printf("DHCP failed\n");
			}
		} else {
			printf("Can not reconnect to AP\n");
		}
	}

	rtos_task_delete(NULL);
}

static void user_main_task(void *param)
{
	int ret = 0;
	printf("\nExample: user defined wifi recnnect\n");

	/* Wait wifi init finish */
	while (!(wifi_is_running(STA_WLAN_INDEX))) {
		rtos_time_delay_ms(1000);
	}

	/* Disable auto-reconnect in driver layer */
	wifi_config_autoreconnect(0);
	
	printf("Connect to test ap\n");
	rtw_network_info_t connect_param = {0};
	memcpy(connect_param.ssid.val, test_ssid, strlen(test_ssid));
	connect_param.ssid.len = strlen(test_ssid);
	connect_param.password = (unsigned char *)test_password;
	connect_param.password_len = strlen(test_password);
	connect_param.security_type = RTW_SECURITY_WPA2_AES_PSK;
	connect_param.joinstatus_user_callback = wifi_join_status_callback;
	ret = wifi_connect(&connect_param, 1);

	if (ret == RTW_SUCCESS) {
		ret = LwIP_DHCP(0, DHCP_START);
		if (ret == DHCP_ADDRESS_ASSIGNED) {
			printf("Connect success\n");

			/* Create user defined reconnect task */
			ret = rtos_sema_create_binary(&sta_disconnect_sema);
			if (ret != SUCCESS || sta_disconnect_sema == NULL) {
				printf("Create disconnect sema failed\n");
				goto exit;
			}
			if (rtos_task_create(NULL, ((const char *)"user_wifi_reconnect_task"), user_wifi_reconnect_task, NULL, 1024 * 4, 1) != SUCCESS) {
				rtos_sema_delete(sta_disconnect_sema);
				printf("Create reconnect task failed\n");
	}
		} else {
			printf("DHCP failed\n");
		}
	} else {
		printf("Can not connect to AP\n");
	}

exit:
	rtos_task_delete(NULL);
}

void example_wifi_user_reconnect(void)
{
	if (rtos_task_create(NULL, ((const char *)"user_main_task"), user_main_task, NULL, 1024 * 4, 1) != SUCCESS) {
		printf("\n%s rtos_task_create failed\n", __FUNCTION__);
	}
}

