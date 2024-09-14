#include "example_wifi_user_reconnect.h"
#include "wifi_conf.h"
#include "wifi_ind.h"
#include "lwip_netconf.h"
#include "os_wrapper.h"

/********************************User configure**************************/
#define RECONNECT_LIMIT			8
#define RECONNECT_INTERVAL		5000/*ms*/
char *test_ssid = "xiaomi3000";
char *test_password = "123456789";
rtw_security_t secure_type = RTW_SECURITY_WPA2_AES_PSK;/*Just distinguish between WEP, TKIP/WPA/WPA2/WPA3 can use the same secure_type*/
/***********************************End**********************************/
static const char *TAG = "WIFI_RECONN_EXAMPLE";
u8 reconnect_cnt = 0;

int user_wifi_connect(void)
{
	int ret = 0;
	rtw_network_info_t connect_param = {0};

	/*Connect parameter set*/
	memcpy(connect_param.ssid.val, test_ssid, strlen(test_ssid));
	connect_param.ssid.len = strlen(test_ssid);
	connect_param.password = (unsigned char *)test_password;
	connect_param.password_len = strlen(test_password);
	connect_param.security_type = secure_type;

WIFI_CONNECT:
	/*Connect*/
	RTK_LOGI(TAG, "Wifi connect start, retey cnt = %d\n", reconnect_cnt);
	ret = wifi_connect(&connect_param, 1);
	if (ret != RTW_SUCCESS) {
		RTK_LOGI(TAG, "Reconnect Fail:%d", ret);
		if ((ret == RTW_CONNECT_INVALID_KEY)) {
			RTK_LOGI(TAG, "(password format wrong)\r\n");
		} else if (ret == RTW_CONNECT_SCAN_FAIL) {
			RTK_LOGI(TAG, "(not found AP)\r\n");
		} else if (ret == RTW_BUSY) {
			RTK_LOGI(TAG, "(busy)\r\n");
		} else {
			RTK_LOGI(TAG, "(other)\r\n");
		}
	}

	/*DHCP*/
	if (ret == RTW_SUCCESS) {
		RTK_LOGI(TAG, "Wifi connect success, Start DHCP\n");
		ret = LwIP_DHCP(0, DHCP_START);
		if (ret == DHCP_ADDRESS_ASSIGNED) {
			RTK_LOGI(TAG, "DHCP Success\n");
			reconnect_cnt = 0;
			return RTW_SUCCESS;
		} else {
			RTK_LOGI(TAG, "DHCP Fail\n");
			wifi_disconnect();
		}
	}

	/*Reconnect when connect fail or DHCP fail*/
	reconnect_cnt++;
	if (reconnect_cnt >= RECONNECT_LIMIT) {
		RTK_LOGI(TAG, "Reconnect limit reach, Wifi connect fail\n");
		return RTW_ERROR;
	} else {
		rtos_time_delay_ms(RECONNECT_INTERVAL);
		goto WIFI_CONNECT;
	}
}

static void user_wifi_reconnect_task(void *param)
{
	(void) param;

	rtos_time_delay_ms(RECONNECT_INTERVAL);
	user_wifi_connect();
	rtos_task_delete(NULL);
}

void user_wifi_join_status_event_hdl(char *buf, int buf_len, int flags, void *userdata)
{
	(void) userdata;
	(void) buf_len;
	enum rtw_join_status_type join_status = (enum rtw_join_status_type)flags;
	u16 disconn_reason = 0;

	/*Reconnect when disconnect after connected*/
	if (join_status == RTW_JOINSTATUS_DISCONNECT) {
		disconn_reason = ((struct rtw_event_disconn_info_t *)buf)->disconn_reason;
		/*Disconnect by APP no need do reconnect*/
		if (disconn_reason > WLAN_REASON_APP_BASE && disconn_reason < WLAN_REASON_APP_BASE_END) {
			return;
		}

		/*Creat a task to do wifi reconnect because call WIFI API in WIFI event is not safe*/
		if (rtos_task_create(NULL, ((const char *)"user_wifi_reconnect_task"), user_wifi_reconnect_task, NULL, 1024 * 4, 1) != SUCCESS) {
			RTK_LOGI(TAG, "Create reconnect task failed\n");
		}
	}
}

static void user_main_task(void *param)
{
	(void) param;
	RTK_LOGI(TAG, "start\n");

	/* Wait wifi init finish */
	while (!(wifi_is_running(STA_WLAN_INDEX))) {
		rtos_time_delay_ms(1000);
	}

	/* Disable realtek auto reconnect */
	wifi_config_autoreconnect(0);

	/* Register join status event, trigger reconnect when disconnect happen*/
	wifi_reg_event_handler(WIFI_EVENT_JOIN_STATUS, user_wifi_join_status_event_hdl, NULL);

	/* Start connect */
	user_wifi_connect();

	rtos_task_delete(NULL);
}

void example_wifi_user_reconnect(void)
{
	/* Disable realtek fast reconnect */
	wifi_fast_connect_enable(0);

	if (rtos_task_create(NULL, ((const char *)"user_main_task"), user_main_task, NULL, 1024, 1) != SUCCESS) {
		RTK_LOGI(TAG, "\n%s rtos_task_create failed\n", __FUNCTION__);
	}
}

