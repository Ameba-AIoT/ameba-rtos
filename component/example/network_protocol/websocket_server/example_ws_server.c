#include "platform_stdlib.h"
#include "basic_types.h"
#include "websocket/wsserver_api.h"
#include "wifi_conf.h"
#include "wifi_ind.h"
#include "websocket/ws_server_msg.h"

#define IP_ADDR_INVALID 0
extern uint8_t *LwIP_GetIP(uint8_t idx);

#define USE_WSS    0

#if USE_WSS
// use test_srv_crt, test_srv_key, test_ca_list in PolarSSL certs.c
#include <mbedtls/certs.h>
#endif

void echo_data(ws_conn *conn, int data_len, enum opcode_type opcode)
{
	RTK_LOGS(NOTAG, "\r\n>>>>>> Receiving: %s with length: %d from %d\r\n", conn->receivedData, (int)data_len, conn->sock);
	if (opcode == TEXT_FRAME) {
		ws_server_sendText((char *)conn->receivedData, (size_t)data_len, 1, conn);
	} else if (opcode == BINARY_FRAME) {
		ws_server_sendBinary(conn->receivedData, (size_t)data_len, 1, conn);
	}
}

static void example_wsserver_thread(void *param)
{
	(void)param;
	RTK_LOGS(NOTAG, "\r\n\r\n\r\n>>>>>>>>>>>>>>>websocket server example<<<<<<<<<<<<<<<<<\r\n\r\n\r\n");

	while (!((wifi_get_join_status() == RTW_JOINSTATUS_SUCCESS) && (*(u32 *)LwIP_GetIP(0) != IP_ADDR_INVALID))) {
		RTK_LOGS(NOTAG, "\r\n\r\n\r\n>>>>>>>>>>>>>>Wifi is disconnected!!Please connect!!<<<<<<<<<<<<<<<<<\r\n\r\n\r\n");
		rtos_time_delay_ms(2000);
	}
#if USE_WSS
	if (ws_server_setup_cert(mbedtls_test_srv_crt, mbedtls_test_srv_key, mbedtls_test_ca_crt) != 0) {
		RTK_LOGS(NOTAG, "\r\nERROR: ws_server_setup_cert\r\n");
		goto exit;
	}
#endif
	ws_server_dispatch(echo_data);

#if USE_WSS
	if (ws_server_start(443, 5, 4096, WS_SERVER_SECURE_TLS) != 0) {
#else
	if (ws_server_start(80, 5, 4096, WS_SERVER_SECURE_NONE) != 0) {
#endif
		RTK_LOGS(NOTAG, "\r\nERROR: ws_server_start\r\n");
	}
#if USE_WSS
exit:
#endif
	rtos_task_delete(NULL);
}

void example_wsserver(void)
{
	if (rtos_task_create(NULL, ((const char *)"example_wsserver_thread"), example_wsserver_thread, NULL, 2048 * 4, 1) != SUCCESS) {
		RTK_LOGS(NOTAG, "\r\n%s rtos_task_create(example_wsserver_thread) failed\r\n", __FUNCTION__);
	}
}

