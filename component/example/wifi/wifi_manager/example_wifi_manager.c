/******************************************************************************
 *
 * Copyright(c) 2007 - 2015 Realtek Corporation. All rights reserved.
 *
 *
 ******************************************************************************/
#include "platform_autoconf.h"
#include "example_wifi_manager.h"
#include "platform_stdlib.h"
#include "basic_types.h"
#include "wifi_conf.h"
#include "rtw_autoconf.h"

#define MAX_RETRY_TIMEOUT 60000
#define RETRY_NUM 5

extern int wifi_do_fast_connect();

rtos_sema_t wm_assoc_sema, wm_discon_sema;

int retry_timeout[RETRY_NUM] = {1000, 5000, 10000, 30000, MAX_RETRY_TIMEOUT};

//For this example, must disable auto-reconnect in driver layer because we do reconnect in up layer.
//#define CONFIG_AUTO_RECONNECT 0 , in rtw_autoconf.h
static void wifi_manager_disconnect_hdl(void)
{
	static int retry_count = 0;

	if (retry_count >= RETRY_NUM) {
		rtos_time_delay_ms(retry_timeout[RETRY_NUM - 1]);
	} else {
		rtos_time_delay_ms(retry_timeout[retry_count]);
	}
	retry_count++;

	printf("\n%s : Reconnect to AP (%d)\n", __func__, retry_count);
	//Do fast connection to reconnect
	wifi_do_fast_connect();

	return;
}

static void wifi_manager_thread(void *param)
{
	//Register wifi event callback function
	wifi_reg_event_handler(WIFI_EVENT_CONNECT, example_wifi_manager_connect_cb, NULL);
	wifi_reg_event_handler(WIFI_EVENT_DISCONNECT, example_wifi_manager_disconnect_cb, NULL);
	printf("\nExample: wifi_manager \n");
	//Can use semaphore to wait wifi event happen
	//The following example shows that we wait for wifi association
	rtos_sema_create(&wm_assoc_sema, 0, RTOS_SEMA_MAX_COUNT);
	if (!wm_assoc_sema) {
		printf("\nInit wm_assoc_sema failed\n");
	}

	rtos_sema_create(&wm_discon_sema, 0, RTOS_SEMA_MAX_COUNT);
	if (!wm_discon_sema) {
		printf("\nInit wm_discon_sema failed\n");
	}

	if (rtos_sema_take(wm_assoc_sema, WM_ASSOC_SEMA_TIMEOUT) == FALSE) {
		rtos_sema_delete(wm_assoc_sema);
	}

	int condition = 1;
	while (condition != 0) {
		//Example: When wifi disconnect, call disconnect handle function.
		if (rtos_sema_take(wm_discon_sema, RTOS_MAX_DELAY) == FALSE) {
			rtos_sema_delete(wm_discon_sema);
		}
		wifi_manager_disconnect_hdl();
	}

	//Unregister wifi event callback function
	wifi_unreg_event_handler(WIFI_EVENT_CONNECT, example_wifi_manager_connect_cb);
	wifi_unreg_event_handler(WIFI_EVENT_DISCONNECT, example_wifi_manager_disconnect_cb);

	if (wm_assoc_sema) {
		rtos_sema_delete(wm_assoc_sema);
	}
	if (wm_discon_sema) {
		rtos_sema_delete(wm_discon_sema);
	}

	rtos_task_delete(NULL);
}

//Wifi association done callback
void example_wifi_manager_connect_cb(char *buf, int buf_len, int flags, void *userdata)
{
	rtos_sema_give(&wm_assoc_sema);
	printf("\n\rWifi association done!!\n");
	return;
}

//Wifi disconnect callback
void example_wifi_manager_disconnect_cb(char *buf, int buf_len, int flags, void *userdata)
{
	rtos_sema_give(&wm_discon_sema);
	printf("\n\rWifi disconnect!!\n");
	return;
}

void example_wifi_manager(void)
{
	if (rtos_task_create(NULL, ((const char *)"wifi_manager_thread"), wifi_manager_thread, NULL, 1024 * 4, 0) != SUCCESS) {
		printf("\n\r%s rtos_task_create(wifi_manager_thread) failed", __FUNCTION__);
	}

	return;
}

