#include "platform_stdlib.h"
#include "basic_types.h"
#include "wifi_conf.h"
#include "wifi_ind.h"
#include "websocket/libwsclient.h"
#include "websocket/wsclient_api.h"
#include <stdio.h>
#include "example_wsclient.h"
#include "os_wrapper.h"

#define IP_ADDR_INVALID 0
extern uint8_t *LwIP_GetIP(uint8_t idx);

void handle_message(wsclient_context **wsclient, int data_len)
{
	wsclient_context *wsc = *wsclient;
	RTK_LOGS(NOTAG, "\r\n>>>>>> Receiving: %s with length: %d\n", wsc->receivedData, data_len);

	if (strcmp((char const *)wsc->receivedData, "hello") == 0) {
		ws_send("world", strlen("world"), 1, wsc);
	} else if (strcmp((char const *)wsc->receivedData, "world") == 0) {
		ws_close(wsclient);
	}
}

static void example_wsclient_thread(void *param)
{
	/* To avoid gcc warnings */
	(void) param;
#if defined(configENABLE_TRUSTZONE) && (configENABLE_TRUSTZONE == 1)
	rtos_create_secure_context(configMINIMAL_SECURE_STACK_SIZE);
#endif

	RTK_LOGS(NOTAG, "\r\n\r\n\r\n>>>>>>>>>>>>>>>wsclient example<<<<<<<<<<<<<<<<<\r\n\r\n\r\n");

	while (!((wifi_get_join_status() == RTW_JOINSTATUS_SUCCESS) && (*(u32 *)LwIP_GetIP(0) != IP_ADDR_INVALID))) {
		RTK_LOGS(NOTAG, "\r\n\r\n\r\n>>>>>>>>>>>>>>Wifi is disconnected!!Please connect!!<<<<<<<<<<<<<<<<<\r\n\r\n\r\n");
		rtos_time_delay_ms(2000);
	}
	int ret;

	//Please set MBEDTLS_SSL_MAX_CONTENT_LEN to 16384
	//wsclient_context *wsclient = create_wsclient("wss://websocket-echo.com", 0, NULL, NULL, 1500, 3);
	wsclient_context *wsclient = create_wsclient("ws://websocket-echo.com", 0, NULL, NULL, 1500, 3);

	if (wsclient != NULL) {

		if (wsclient->use_ssl == 1) {
#ifndef USING_SSL
			RTK_LOGS(NOTAG, "\r\nNot Support the wss server!\r\n");
			rtos_task_delete(NULL);
#endif
		}
		ret = ws_connect_url(wsclient);
		if (ret >= 0) {
			ws_send("hello", strlen("hello"), 1, wsclient);
			ws_dispatch(handle_message);
			while (wsclient->readyState != CLOSED) {

				ws_poll(0, &wsclient);
			}
		} else {
			RTK_LOGS(NOTAG, "\r\nConnect to websocket server failed!\r\n");
		}

		if (wsclient) {
			ws_free(wsclient);
			wsclient = NULL;
		}
	} else {
		RTK_LOGS(NOTAG, "\r\nCreat websocket context failed!\r\n");
	}

	rtos_task_delete(NULL);
}

void example_wsclient(void)
{
	if (rtos_task_create(NULL, ((const char *)"example_wsclient_thread"), example_wsclient_thread, NULL, 1024 * 4, 1) != SUCCESS) {
		RTK_LOGS(NOTAG, "\n\r%s rtos_task_create(init_thread) failed", __FUNCTION__);
	}
}
