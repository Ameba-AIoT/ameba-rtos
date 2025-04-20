#include "example_wifi_user_reconnect.h"
#include "wifi_api.h"
#include "lwip_netconf.h"
#include "os_wrapper.h"

/********************************User configure**************************/
#define RECONNECT_LIMIT			8
#define RECONNECT_INTERVAL		5000/*ms*/
char *test_ssid = "xiaomi3000";
char *test_password = "123456789";
/***********************************End**********************************/
static const char *const TAG = "WIFI_RECONN_EXAMPLE";
u8 reconnect_cnt = 0;

int user_wifi_connect(void)
{
	int ret = 0;
	struct rtw_network_info connect_param = {0};

	/*Connect parameter set*/
	memcpy(connect_param.ssid.val, test_ssid, strlen(test_ssid));
	connect_param.ssid.len = strlen(test_ssid);
	connect_param.password = (unsigned char *)test_password;
	connect_param.password_len = strlen(test_password);

WIFI_CONNECT:
	/*Connect*/
	RTK_LOGI(TAG, "Wifi connect start, retry cnt = %d\n", reconnect_cnt);
	ret = wifi_connect(&connect_param, 1);
	if (ret != RTK_SUCCESS) {
		RTK_LOGI(TAG, "Reconnect Fail:%d", ret);
		if ((ret == -RTK_ERR_WIFI_CONN_INVALID_KEY)) {
			RTK_LOGI(TAG, "(password format wrong)\r\n");
		} else if (ret == -RTK_ERR_WIFI_CONN_SCAN_FAIL) {
			RTK_LOGI(TAG, "(not found AP)\r\n");
		} else if (ret == -RTK_ERR_BUSY) {
			RTK_LOGI(TAG, "(busy)\r\n");
		} else {
			RTK_LOGI(TAG, "(other)\r\n");
		}
	}

	/*DHCP*/
	if (ret == RTK_SUCCESS) {
		RTK_LOGI(TAG, "Wifi connect success, Start DHCP\n");
		ret = LwIP_DHCP(0, DHCP_START);
		if (ret == DHCP_ADDRESS_ASSIGNED) {
			RTK_LOGI(TAG, "DHCP Success\n");
			reconnect_cnt = 0;
			return RTK_SUCCESS;
		} else {
			RTK_LOGI(TAG, "DHCP Fail\n");
			wifi_disconnect();
		}
	}

	/*Reconnect when connect fail or DHCP fail*/
	reconnect_cnt++;
	if (reconnect_cnt >= RECONNECT_LIMIT) {
		RTK_LOGI(TAG, "Reconnect limit reach, Wifi connect fail\n");
		return RTK_FAIL;
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

void user_wifi_join_status_event_hdl(u8 *buf, s32 buf_len, s32 flags, void *userdata)
{
	(void) userdata;
	(void) buf_len;
	u8 join_status = (u8)flags;
	u16 disconn_reason = 0;

	/*Reconnect when disconnect after connected*/
	if (join_status == RTW_JOINSTATUS_DISCONNECT) {
		disconn_reason = ((struct rtw_event_info_joinstatus_disconn *)buf)->disconn_reason;
		/*Disconnect by APP no need do reconnect*/
		if (disconn_reason > RTW_DISCONN_RSN_APP_BASE && disconn_reason < RTW_DISCONN_RSN_APP_BASE_END) {
			return;
		}

		/*Creat a task to do wifi reconnect because call WIFI API in WIFI event is not safe*/
		if (rtos_task_create(NULL, ((const char *)"user_wifi_reconnect_task"), user_wifi_reconnect_task, NULL, 1024 * 4, 1) != RTK_SUCCESS) {
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
	wifi_set_autoreconnect(0);

	/* Register join status event, trigger reconnect when disconnect happen*/
	wifi_reg_event_handler(RTW_EVENT_JOIN_STATUS, user_wifi_join_status_event_hdl, NULL);

	/* Start connect */
	user_wifi_connect();

	rtos_task_delete(NULL);
}

void example_wifi_user_reconnect(void)
{
	/* Disable realtek fast reconnect */
	extern void wifi_fast_connect_enable(unsigned char enable);
	wifi_fast_connect_enable(0);

	if (rtos_task_create(NULL, ((const char *)"user_main_task"), user_main_task, NULL, 1024, 1) != RTK_SUCCESS) {
		RTK_LOGI(TAG, "\n%s rtos_task_create failed\n", __FUNCTION__);
	}
}

