/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* #include "FreeRTOS.h"
#include "task.h"
#include "semphr.h" */

#include "platform_autoconf.h"
#include "os_wrapper.h"
#include "atcmd_service.h"
#include "atcmd_mqtt.h"
#ifdef CONFIG_LWIP_LAYER
#include <lwip_netconf.h>
#endif
#include <dhcp/dhcps.h>
#ifdef CONFIG_WLAN
#include "wifi_conf.h"
#endif

#ifndef CONFIG_MP_INCLUDED
static MQTT_RESULT_ENUM mqtt_get_handle_cb(u8 tcpConnId, MQTT_CONTROL_BLOCK **mqttCb, u8 needInit);
static void mqtt_del_handle_cb(MQTT_CONTROL_BLOCK **mqttCb);
static void mqtt_set_handle_cb(u8 tcpConnId, MQTT_CONTROL_BLOCK *mqttCb);
static MQTT_RESULT_ENUM mqtt_init_handle_cb(MQTT_CONTROL_BLOCK **pMqttCb);
static MQTT_RESULT_ENUM mqtt_init_client_buf(MQTTClient *client, Network *network, u32 timeout);
static void mqtt_del_client_buf(MQTTClient *client);
static void mqtt_message_arrived(MessageData *data, void *param);
static MQTT_RESULT_ENUM mqtt_string_copy(char **dest, char *src, size_t sz);
static void mqtt_clent_alive_fail(MQTT_CONTROL_BLOCK *mqttCb);
static MQTT_RESULT_ENUM mqtt_clent_data_proc(MQTT_CONTROL_BLOCK *mqttCb, fd_set *read_fds, u8 *needAtOutpput);
static u8 mqtt_config_get_index(char *cmdString);

MQTT_AT_HANDLE mqtt_at_handle;
MQTTPacket_connectData mqtt_default_conn_data = MQTTPacket_connectData_initializer;

const char *mqtt_config_string[MQTT_CONFIG_CMD_NUM] = {"version", "keepalive", "session", "timeout", "will", "ssl"};

/****************************************************************
Function            mqtt_main
Brief                   The main function of mqtt AT task.
****************************************************************/
void mqtt_main(void *param)
{
	int resultNo = 0;
	MQTT_CONTROL_BLOCK *mqttCb = (MQTT_CONTROL_BLOCK *)param;
	u8 needAtOutpput = 0;
	fd_set read_fds;
#if MQTT_EXCEPT_FDS
	fd_set except_fds;
#endif
	struct timeval timeout;

	if (NULL == mqttCb) {
		RTK_LOGI(NOTAG, "\r\n[mqtt_main] Input invalid param.");
		goto end;
	}

	while (1) {
		if (MQTT_TASK_START != mqttCb->taskState) {
			RTK_LOGI(NOTAG, "\r\n[mqtt_main] Should stop now");
			break;
		}

		FD_ZERO(&read_fds);
#if MQTT_EXCEPT_FDS
		FD_ZERO(&except_fds);
#endif
		timeout.tv_sec = MQTT_SELECT_TIMEOUT;
		timeout.tv_usec = 0;

		if (0 <= mqttCb->network.my_socket) {
			FD_SET(mqttCb->network.my_socket, &read_fds);
#if MQTT_EXCEPT_FDS
			FD_SET(mqttCb->network.my_socket, &except_fds);
			resultNo = FreeRTOS_Select(mqttCb->network.my_socket + 1, &read_fds, NULL, &except_fds, &timeout);
			/* The my_socket may be close, then will try reopen in mqtt_clent_data_proc if STATUS set to MQTT_START */
			if (FD_ISSET(mqttCb->network.my_socket, &except_fds)) {
				mqttCb->client.mqttstatus = MQTT_START;
			}
#else
			resultNo = FreeRTOS_Select(mqttCb->network.my_socket + 1, &read_fds, NULL, NULL, &timeout);
			if (FD_ISSET(mqttCb->network.my_socket, &read_fds)) {
				/* Do nothing here. The data will be processed in mqtt_clent_data_proc( ). */
			}
#endif
			/* Select timeout. */
			else if (0 == resultNo) {
				if (mqttCb->client.isconnected) {
					keepalive(&mqttCb->client);
					/* ping_outstanding++ when keepalive send ping req , ping_outstanding-- if receive ping rsp ;
					ping_outstandingis >= 2, it means we miss the ping rsp 2*keepalive intervals, maybe the link is error;
					server will disconnect if not receive ping req in 1.5*keepalive intervals. */
					if (2 < mqttCb->client.ping_outstanding) {
						mqtt_clent_alive_fail(mqttCb);
					}
				}
			}
		}
		/* Process the received data. */
		resultNo = mqtt_clent_data_proc(mqttCb, &read_fds, &needAtOutpput);
		if (1 == needAtOutpput) {
			if (MQTT_OK == resultNo) {
				at_printf("\r\nACK\r\n");
			} else {
				at_printf("\r\nERROR:%d\r\n", resultNo);
			}
		}
	}

end:
	RTK_LOGI(NOTAG, "\r\n[mqtt_main] Stop mqtt task");
	mqttCb->taskState = MQTT_TASK_NOT_CREATE;
	mqttCb->taskHandle = NULL;
	rtos_task_delete(mqttCb->taskHandle);
}

static void at_mqttopen_help(void)
{
	at_printf("\r\n");
	at_printf("AT+MQTTOPEN=<tcpConnId>,<hostname>[,<port>]\r\n");
	at_printf("\t<tcpConnId>:\t[0,3]\r\n");
	at_printf("\t<hostname>:\tA string of hostname\r\n");
	at_printf("\t<port>:\t[1,65535], if absent, use 1883 as default\r\n");
}

/****************************************************************
Function            at_mqttopen
Brief                   MQTT open, The command is used to open a network for MQTT client.
****************************************************************/
void at_mqttopen(void *arg)
{
	const u8 idIndex = 1, hostIndex = 2, portIndex = 3;
	s32 i = 0, res = 0, keep = 0;
	MQTT_RESULT_ENUM resultNo = MQTT_OK;
	char *argv[MAX_ARGC] = {0};
	MQTT_CONTROL_BLOCK *mqttCb = NULL;
	u8  tcpConnId = MQTT_MAX_CLIENT_NUM;
	char TaskName[6];
	char *hostName;
	u16 portId = MQTT_DEFAULT_PORT;
#if MQTT_OVER_SSL
	const u16 stacksize = 6144;
#else
	const u16 stacksize = 4096;
#endif

	/* Get the parameters of AT command. */
	if (!arg) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttopen] Usage : at_mqttopen=<tcpConnId>,<hostname>,<port>");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	/* parameters include tcpConnId, hostname, port. The port is optional. */
	i = parse_param(arg, argv);
	if (4 < i || 3 > i) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttopen] parameters include tcpConnId, hostname, port.");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	/* connect ID. */
	res = -1;
	if (1 == strlen(argv[idIndex])
		&& '0' <= argv[idIndex][0] && '0' + MQTT_MAX_CLIENT_NUM > argv[idIndex][0]) {
		res = argv[idIndex][0] - '0';
	}
	if (0 > res || MQTT_MAX_CLIENT_NUM <= res) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttopen] Invalid tcpConnId");
		resultNo = MQTT_CLIENTID_REJECTED_ERROR;
		goto end;
	}
	tcpConnId = (u8)res;
	/* For each connect ID, the task name should be unique. */
	TaskName[0] = 'M';
	TaskName[1] = 'Q';
	TaskName[2] = 'T';
	TaskName[3] = 'T';
	TaskName[4] = '0' + tcpConnId;
	TaskName[5] = '\0';

	if (NULL != mqtt_at_handle.mqttCb[tcpConnId]) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttopen] tcpConnId %d already exists", tcpConnId);
		keep = 1;
		resultNo = MQTT_ALREADY_EXIST_ID;
		goto end;
	}

	/* hostName. */
	if (0 == strlen(argv[hostIndex]) || MQTT_MAX_HOSTNAME_LEN < strlen(argv[hostIndex]) || 0 == strlen(argv[hostIndex])) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttopen] Invalid hostName.");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}
	hostName = argv[hostIndex];

	/* Port is optional. */
	if (4 == i && 0 != strlen(argv[portIndex])) {
		res = atoi(argv[portIndex]);
		if (0 >= res || 0xFFFF < res) {
			RTK_LOGI(NOTAG, "\r\n[at_mqttopen] Invalid port.");
			resultNo = MQTT_ARGS_ERROR;
			goto end;
		}
		portId = (u16)res;
	}

	/* Check whether the network is linked. */
	if (wifi_is_connected_to_ap() != RTW_SUCCESS) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttopen] The network is not ready.");
		resultNo = MQTT_NETWORK_LINK_ERROR;
		goto end;
	}

	/* Get the specific mqtt control block, and initialise. */
	resultNo = mqtt_get_handle_cb(tcpConnId, &mqttCb, 1);
	if (MQTT_OK != resultNo) {
		goto end;
	}

	/* Update the mqttCb information. */
	mqttCb->tcpConnectId = tcpConnId;
	mqttCb->port = portId;
	resultNo = mqtt_string_copy(&mqttCb->host, hostName, strlen(hostName));
	if (MQTT_OK != resultNo) {
		goto end;
	}
#if MQTT_OVER_SSL
	mqttCb->network.use_ssl = mqttCb->useSsl;
#endif

	RTK_LOGI(NOTAG, "\r\n[at_mqttopen] Open a new connection for %s", mqttCb->host);

#if (!MQTT_CONNECT_LATER)
	/* Try to connect the host. */
	res = NetworkConnect(&mqttCb->network, mqttCb->host, mqttCb->port);
	if (0 != res) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttopen] Can not connect %s", mqttCb->host);
		resultNo = MQTT_AUTH_ERROR;
		goto end;
	}
	mqttCb->networkConnect = 1;
#endif

	/* Create the task. */
	mqttCb->taskState = MQTT_TASK_START;
	if (SUCCESS != rtos_task_create(&mqttCb->taskHandle,
									TaskName,
									mqtt_main,
									(void *)mqttCb,
									stacksize,
									ATCMD_MQTT_TASK_PRIORITY)) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttopen] Create task failed.");
		mqttCb->taskState = MQTT_TASK_NOT_CREATE;
		resultNo = MQTT_THREAD_CREATE_ERROR;
		goto end;
	}

end:
	if (MQTT_OK != resultNo) {
		/* If the connect id is already created before, do not delete it. */
		if (0 == keep) {
			mqtt_del_handle_cb(&mqttCb);
			mqtt_set_handle_cb(tcpConnId, mqttCb);
		}
		at_printf("\r\n%sERROR:%d\r\n", "+MQTTOPEN:", resultNo);
		if (MQTT_ARGS_ERROR == resultNo) {
			at_mqttopen_help();
		}
	} else {
		at_printf("\r\n%sOK\r\n", "+MQTTOPEN:");
	}
}

static void at_mqttclose_help(void)
{
	at_printf("\r\n");
	at_printf("AT+MQTTCLOSE=<tcpConnId>\r\n");
	at_printf("\t<tcpConnId>:\t[0,3]\r\n");
}

/****************************************************************
Function            at_mqttclose
Brief                   MQTT close. The command is used to close a network for MQTT client.
****************************************************************/
void at_mqttclose(void *arg)
{
	const u8 idIndex = 1;
	s32  i = 0, res = 0;
	MQTT_RESULT_ENUM resultNo = MQTT_OK;
	char *argv[MAX_ARGC] = {0};
	MQTT_CONTROL_BLOCK *mqttCb = NULL;
	u8  tcpConnId = MQTT_MAX_CLIENT_NUM;

	/* Get the parameters of AT command. */
	if (!arg) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttclose] Usage : at_mqttclose=<tcpConnId>");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	/* parameters include tcpConnId. */
	i = parse_param(arg, argv);
	if (2 != i) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttclose] Input only connect ID");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	/* Connect ID. */
	res = -1;
	if (1 == strlen(argv[idIndex])
		&& '0' <= argv[idIndex][0] && '0' + MQTT_MAX_CLIENT_NUM > argv[idIndex][0]) {
		res = argv[idIndex][0] - '0';
	}
	if (0 > res || MQTT_MAX_CLIENT_NUM <= res) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttclose] Invalid tcpConnId");
		resultNo = MQTT_CLIENTID_REJECTED_ERROR;
		goto end;
	}
	tcpConnId = (u8)res;

	RTK_LOGI(NOTAG, "\r\n[at_mqttclose] Close %d", tcpConnId);
	resultNo = mqtt_get_handle_cb(tcpConnId, &mqttCb, 0);
	if (MQTT_OK != resultNo) {
		resultNo = MQTT_NOT_OPEN_ERROR;
		RTK_LOGI(NOTAG, "\r\n[at_mqttclose] Failed when getting mqttCb");
		goto end;
	}

	/* Disconnect client. */
	if (mqttCb->client.isconnected) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttclose] Still connected");
		res = MQTTDisconnect(&mqttCb->client);
		if (0 != res) {
			RTK_LOGI(NOTAG, "\r\n[at_mqttclose] Can not disconnect");
			resultNo = MQTT_ALREADY_CONNECTED_ERROR;
			goto end;
		}
		mqttCb->client.isconnected = 0;
	}

	/* Stop the task. */
	if (MQTT_TASK_START == mqttCb->taskState) {
		mqttCb->taskState = MQTT_TASK_STOP;
		/* Wait until the mqtt_main task stops. */
		while (MQTT_TASK_NOT_CREATE != mqttCb->taskState) {
			rtos_time_delay_ms(20);
		}
	}

	/* Disconnect host. */
	if (mqttCb->networkConnect) {
		if (NULL != mqttCb->network.disconnect) {
			RTK_LOGI(NOTAG, "\r\n[at_mqttclose] Disconnect from %s", mqttCb->host);
			mqttCb->network.disconnect(&mqttCb->network);
		}
		mqttCb->networkConnect = 0;
	}

	/* NetworkDeinit(&mqttCb->network); */
	mqttCb->network.my_socket = -1;
	mqttCb->network.mqttread = NULL;
	mqttCb->network.mqttwrite = NULL;
	mqttCb->network.disconnect = NULL;
#if MQTT_OVER_SSL
	mqttCb->network.use_ssl = 0;
	mqttCb->network.ssl = NULL;
	mqttCb->network.conf = NULL;
	mqttCb->network.rootCA = NULL;
	mqttCb->network.clientCA = NULL;
	mqttCb->network.private_key = NULL;
#endif
	mqttCb->network.ciphersuites = NULL;

end:
	/* Clean the Client Control block. */
	mqtt_del_handle_cb(&mqttCb);
	mqtt_set_handle_cb(tcpConnId, mqttCb);

	if (MQTT_OK != resultNo) {
		at_printf("\r\n%sERROR:%d\r\n", "+MQTTCLOSE:", resultNo);
		if (MQTT_ARGS_ERROR == resultNo) {
			at_mqttclose_help();
		}
	} else {
		at_printf("\r\n%sOK\r\n", "+MQTTCLOSE:");
	}
}

static void at_mqttconn_help(void)
{
	at_printf("\r\n");
	at_printf("AT+MQTTCONN=<tcpConnId>,<client>[,<username>,<password>]\r\n");
}

/****************************************************************
Function            at_mqttconn
Brief                   MQTT connect. The command is used to connect a cloud for MQTT client.
****************************************************************/
void at_mqttconn(void *arg)
{
	const u8 idIndex = 1, clientIndex = 2, usernameIndex = 3, passwordIndex = 4;
	s32 i = 0, len = 0, res = 0;
	MQTT_RESULT_ENUM resultNo = MQTT_OK;
	char *argv[MAX_ARGC] = {0};
	u8  tcpConnId = MQTT_MAX_CLIENT_NUM;
	MQTT_CONTROL_BLOCK *mqttCb = NULL;

	/* Get the parameters of AT command. */
	if (!arg) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttconn] Usage : at_mqttconn=<tcpConnId>,<client>[,<username>,<password>]");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	/* tcpconnectID, clientID are mandetory, username, password are optional. */
	i = parse_param(arg, argv);
	if (3 > i || 5 < i) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttconn] Usage : at_mqttconn=<tcpConnId>,<client>[,<username>,<password>]");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	/* Connect ID. */
	res = -1;
	if (1 == strlen(argv[idIndex])
		&& '0' <= argv[idIndex][0] && '0' + MQTT_MAX_CLIENT_NUM > argv[idIndex][0]) {
		res = argv[idIndex][0] - '0';
	}
	if (0 > res || MQTT_MAX_CLIENT_NUM <= res) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttconn] Invalid tcpConnId");
		resultNo = MQTT_CLIENTID_REJECTED_ERROR;
		goto end;
	}
	tcpConnId = (u8)res;

	resultNo = mqtt_get_handle_cb(tcpConnId, &mqttCb, 0);
	if (MQTT_OK != resultNo) {
		resultNo = MQTT_NOT_OPEN_ERROR;
		goto end;
	}

	/* Client ID. */
	len = strlen(argv[clientIndex]);
	if (0 == len || MQTT_MAX_CLIENT_ID_LEN < len) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttconn] Invalid client ID");
		resultNo = MQTT_CLIENTID_REJECTED_ERROR;
		goto end;
	}
	resultNo = mqtt_string_copy(&mqttCb->clientId, argv[clientIndex], len);
	if (MQTT_OK != resultNo) {
		goto end;
	}

	/* username. */
	if (usernameIndex < i) {
		len = strlen(argv[usernameIndex]);
		if (0 == len || MQTT_MAX_USERNAME_LEN < len) {
			RTK_LOGI(NOTAG, "\r\n[at_mqttconn] Invalid username");
			resultNo = MQTT_CLIENTID_REJECTED_ERROR;
			goto end;
		}
		resultNo = mqtt_string_copy(&mqttCb->userName, argv[usernameIndex], len);
		if (MQTT_OK != resultNo) {
			goto end;
		}
	}

	/* password. */
	if (passwordIndex < i) {
		len = strlen(argv[passwordIndex]);
		if (0 == len || MQTT_MAX_PASSWORD_LEN < len) {
			RTK_LOGI(NOTAG, "\r\n[at_mqttconn] Invalid password");
			resultNo = MQTT_CLIENTID_REJECTED_ERROR;
			goto end;
		}
		resultNo = mqtt_string_copy(&mqttCb->password, argv[passwordIndex], len);
		if (MQTT_OK != resultNo) {
			goto end;
		}
	}

	/* execute send. */
#if (!MQTT_CONNECT_LATER)
	if (0 > mqttCb->network.my_socket && NULL != mqttCb->host)
#endif
	{
		RTK_LOGI(NOTAG, "\r\n[at_mqttconn] Connect to host %s", mqttCb->host);
		res = NetworkConnect(&mqttCb->network, mqttCb->host, mqttCb->port);
		if (0 != res) {
			RTK_LOGI(NOTAG, "\r\n[at_mqttconn] Can not connect to host %s", mqttCb->host);
			mqttCb->client.mqttstatus = MQTT_START;
			resultNo = MQTT_CONNECTION_ERROR;
			goto end;
		}
		mqttCb->networkConnect = 1;
		mqttCb->client.isconnected = 0;
	}
	/* Check status. */
	if (mqttCb->client.isconnected) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttconn] Already connected");
		resultNo = MQTT_NOT_ATTACH_ERROR;
		goto end;
	} else {
		/* Reset when reconnect. */
		mqttCb->client.ping_outstanding = 0;
		mqttCb->client.next_packetid = 1;
	}
	if (NULL == mqttCb->clientId) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttconn] The clientid has not been set");
		resultNo = MQTT_COMMON_ERROR;
		goto end;
	}
	mqttCb->connectData.clientID.cstring = mqttCb->clientId;
	/* The connect may be anonymous, ie. without username and password. */
	if (NULL != mqttCb->userName) {
		mqttCb->connectData.username.cstring = mqttCb->userName;
	}
	if (NULL != mqttCb->password) {
		mqttCb->connectData.password.cstring = mqttCb->password;
	}
	/* Set timer. */
	TimerInit(&mqttCb->client.cmd_timer);
	TimerCountdownMS(&mqttCb->client.cmd_timer, mqttCb->client.command_timeout_ms);
	res = MQTTConnect(&mqttCb->client, &mqttCb->connectData);
	if (0 != res) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttconn] Connect ERROR");
		resultNo = MQTT_CONNECTION_ERROR;
		goto end;
	} else {
		/* AT output when receiving CONNACK. */
		RTK_LOGI(NOTAG, "\r\n[at_mqttconn] Sent connection request, waiting for ACK");
		mqttCb->client.mqttstatus = MQTT_CONNECT;
		mqttCb->initialConnect = 1;
	}

end:
	if (MQTT_OK != resultNo) {
		at_printf("\r\n%sERROR:%d\r\n", "+MQTTCONN:", resultNo);
		if (MQTT_ARGS_ERROR == resultNo) {
			at_mqttconn_help();
		}
	} else {
		at_printf("\r\n%sOK\r\n", "+MQTTCONN:");
	}
}

static void at_mqttdisconn_help(void)
{
	at_printf("\r\n");
	at_printf("AT+MQTTDISCONN=<tcpConnid>\r\n");
}

/****************************************************************
Function            at_mqttdisconn
Brief                   MQTT disconnect. The command is used to disconnect a cloud for MQTT client.
****************************************************************/
void at_mqttdisconn(void *arg)
{
	const u8 idIndex = 1;
	s32 i = 0, res = 0;
	MQTT_RESULT_ENUM resultNo = MQTT_OK;
	char *argv[MAX_ARGC] = {0};
	u8  tcpConnId = MQTT_MAX_CLIENT_NUM;
	MQTT_CONTROL_BLOCK *mqttCb = NULL;

	/* Get the parameters of AT command. */
	if (!arg) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttdisconn] Usage : at_mqttdisconn=<tcpConnId>");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	i = parse_param(arg, argv);
	if (2 != i) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttdisconn] Usage : at_mqttdisconn=<tcpConnId>");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	/* Connect ID. */
	res = -1;
	if (1 == strlen(argv[idIndex])
		&& '0' <= argv[idIndex][0] && '0' + MQTT_MAX_CLIENT_NUM > argv[idIndex][0]) {
		res = argv[idIndex][0] - '0';
	}
	if (0 > res || MQTT_MAX_CLIENT_NUM <= res) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttdisconn] Invalid connect ID");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}
	tcpConnId = (u8)res;

	RTK_LOGI(NOTAG, "\r\n[at_mqttdisconn] Disconnect connId %d", tcpConnId);
	resultNo = mqtt_get_handle_cb(tcpConnId, &mqttCb, 0);
	if (MQTT_OK != resultNo) {
		resultNo = MQTT_NOT_OPEN_ERROR;
		goto end;
	}

	if (!mqttCb->client.isconnected) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttdisconn] ALREADY disconnected");
		resultNo = MQTT_NOT_CONNECTED_ERROR;
		goto end;
	}

	res = MQTTDisconnect(&mqttCb->client);
	if (0 != res) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttdisconn] Disconnect ERROR");
		resultNo = MQTT_COMMON_ERROR;
		goto end;
	}

end:
	/* Delete the stored clientid, username, password. Set the flag back. */
	if (NULL != mqttCb) {
		rtos_mem_free(mqttCb->clientId);
		mqttCb->clientId = NULL;
		rtos_mem_free(mqttCb->userName);
		mqttCb->userName = NULL;
		rtos_mem_free(mqttCb->password);
		mqttCb->password = NULL;
		mqttCb->offline = 0;
		mqttCb->initialConnect = 0;
	}

	if (MQTT_OK != resultNo) {
		at_printf("\r\n%sERROR:%d\r\n", "+MQTTDISCONN:", resultNo);
		if (MQTT_ARGS_ERROR == resultNo) {
			at_mqttdisconn_help();
		}
	} else {
		at_printf("\r\n%sOK\r\n", "+MQTTDISCONN:");
	}
}

static void at_mqttsub_help(void)
{
	at_printf("\r\n");
	at_printf("AT+MQTTSUB=<tcpconnectid>,<topic>[,<qos>]\r\n");
	at_printf("\t<tcpconnectid>:\t[0,3]\r\n");
	at_printf("\t<topic>:\tThe topic string\r\n");
	at_printf("\t<qos>:\t[0,2], if absent, use 2 as default\r\n");
}

/****************************************************************
Function            at_mqttsub
Brief                   MQTT subscribe. The command is used to subscribe mqtt issue.
****************************************************************/
void at_mqttsub(void *arg)
{
	const u8 idIndex = 1, topicIndex = 2, qosIndex = 3;
	s32 i = 0, found = -1, res = 0;
	MQTT_RESULT_ENUM resultNo = MQTT_OK;
	char *argv[MAX_ARGC] = {0};
	u8  tcpConnId = MQTT_MAX_CLIENT_NUM;
	u8  qos = MQTT_DEFAULT_QOS;
	u8  j = 0;
	MQTT_CONTROL_BLOCK *mqttCb = NULL;

	/* Get the parameters of AT command. */
	if (!arg) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttsub] Usage : at_mqttsub=<tcpconnectid>,<topic>,<qos>");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	i = parse_param_advance(arg, argv);
	if (3 > i || 4 < i) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttsub] Usage : at_mqttsub=<tcpconnectid>,<msgid>,<topic>,<qos>");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	/* Connect ID. */
	res = -1;
	if (1 == strlen(argv[idIndex])
		&& '0' <= argv[idIndex][0] && '0' + MQTT_MAX_CLIENT_NUM > argv[idIndex][0]) {
		res = argv[idIndex][0] - '0';
	}
	if (0 > res || MQTT_MAX_CLIENT_ID_LEN <= res) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttsub] Invalid connect ID");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}
	tcpConnId = (u8)res;

	resultNo = mqtt_get_handle_cb(tcpConnId, &mqttCb, 0);
	if (MQTT_OK != resultNo) {
		resultNo = MQTT_NOT_OPEN_ERROR;
		goto end;
	}

	/* topic. */
	if (0 == strlen(argv[topicIndex]) || MQTT_MAX_TOPIC_LEN < strlen(argv[topicIndex])) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttsub] Invalid topic");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}
	for (j = 0; MAX_MESSAGE_HANDLERS > j; j++) {
		if (NULL == mqttCb->topic[j]) {
			if (0 > found) {
				found = (s32)j;
			}
		} else if (0 == strcmp(mqttCb->topic[j], argv[topicIndex])) {
			found = -2;
			break;
		}
	}
	if (0 > found) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttsub] Can not match.");
		resultNo = (-2 == found) ? MQTT_ALREADY_SUBSCRIBED_ERROR : MQTT_SUBSCRIPTION_ERROR;
		goto end;
	}
	j = (u8)found;
	resultNo = mqtt_string_copy(&mqttCb->topic[j], argv[topicIndex], _strlen(argv[topicIndex]));
	if (MQTT_OK != resultNo) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttsub] mqtt_string_copy failed.");
		goto end;
	}

	/* QoS. This is optional, if not including here, it should be QOS2. */
	if (4 == i) {
		res = (0 != strlen(argv[qosIndex])) ? atoi(argv[qosIndex]) : -1;
		if (QOS0 > res || QOS2 < res) {
			RTK_LOGI(NOTAG, "\r\n[at_mqttsub] Invalid QoS");
			resultNo = MQTT_ARGS_ERROR;
			goto end;
		}
		qos = (u8)res;
	}

	/* Check status. */
	if (1 != mqttCb->client.isconnected || 0 > mqttCb->network.my_socket) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttsub] Not connected now (%d, %d)",
				 mqttCb->client.isconnected, mqttCb->network.my_socket);
		resultNo = MQTT_NOT_CONNECTED_ERROR;
		goto end;
	}

	RTK_LOGI(NOTAG, "\r\n[at_mqttsub] Subscribe topic %s", mqttCb->topic[j]);

	TimerInit(&mqttCb->client.cmd_timer);
	TimerCountdownMS(&mqttCb->client.cmd_timer, mqttCb->client.command_timeout_ms);
	res = MQTTSubscribe(&mqttCb->client, mqttCb->topic[j], qos, mqtt_message_arrived);
	if (0 != res) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttsub] Subscribe ERROR");
		resultNo = MQTT_SUBSCRIPTION_ERROR;
		goto end;
	}
	mqttCb->client.mqttstatus = MQTT_SUBTOPIC;

end:
	if (MQTT_OK != resultNo) {
		if (0 <= found && NULL != mqttCb->topic[found]) {
			rtos_mem_free(mqttCb->topic[found]);
			mqttCb->topic[found] = NULL;
		}
		at_printf("\r\n%sERROR:%d\r\n", "+MQTTSUB:", resultNo);
		if (MQTT_ARGS_ERROR == resultNo) {
			at_mqttsub_help();
		}
	} else {
		at_printf("\r\n%sOK\r\n", "+MQTTSUB:");
	}
}

static void at_mqttunsub_help(void)
{
	at_printf("\r\n");
	at_printf("AT+MQTTUNSUB=<tcpConnid>,<topic>\r\n");
	at_printf("\t<tcpConnid>:\t[0,3]\r\n");
	at_printf("\t<topic>:\tThe topic string\r\n");
}

/****************************************************************
Function            at_mqttunsub
Brief                   MQTT unsubscribe. The command is used to unsubscribe mqtt issue.
****************************************************************/
void at_mqttunsub(void *arg)
{
	const u8 idIndex = 1, topicIndex = 2;
	s32 i = 0, res = 0;
	MQTT_RESULT_ENUM resultNo = MQTT_OK;
	char *argv[MAX_ARGC] = {0};
	u8 j = 0, matched = 0, tcpConnId = MQTT_MAX_CLIENT_NUM;
	MQTT_CONTROL_BLOCK *mqttCb = NULL;

	/* Get the parameters of AT command. */
	if (!arg) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttunsub] Usage : at_mqttunsub=<tcpconnectid>,<topic>");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	i = parse_param_advance(arg, argv);
	if (3 != i) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttunsub] Usage : at_mqttunsub=<tcpconnectid>,<topic>");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	/* Connect ID. */
	res = -1;
	if (1 == strlen(argv[idIndex])
		&& '0' <= argv[idIndex][0] && '0' + MQTT_MAX_CLIENT_NUM > argv[idIndex][0]) {
		res = argv[idIndex][0] - '0';
	}
	if (0 > res || MQTT_MAX_CLIENT_ID_LEN <= res) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttunsub] Invalid connect ID");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}
	tcpConnId = (u8)res;

	resultNo = mqtt_get_handle_cb(tcpConnId, &mqttCb, 0);
	if (MQTT_OK != resultNo) {
		resultNo = MQTT_NOT_OPEN_ERROR;
		goto end;
	}

	/* topic. */
	if (0 == strlen(argv[topicIndex]) || MQTT_MAX_TOPIC_LEN < strlen(argv[topicIndex])) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttunsub] Invalid topic");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}
	for (j = 0; MAX_MESSAGE_HANDLERS > j; j++) {
		if (NULL != mqttCb->topic[j] && 0 == strcmp(mqttCb->topic[j], argv[topicIndex])) {
			matched = 1;
			break;
		}
	}
	if (0 == matched) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttunsub] Invalid topic");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	/* Check status. */
	if (1 != mqttCb->client.isconnected || 0 > mqttCb->network.my_socket) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttunsub] Invalid status (%d, %d)",
				 mqttCb->client.isconnected, mqttCb->network.my_socket);
		resultNo = MQTT_UNSUBSCRIPTION_ERROR;
		goto end;
	}

	RTK_LOGI(NOTAG, "\r\n[at_mqttunsub] Unsubscribe topic %s", mqttCb->topic[j]);

	TimerInit(&mqttCb->client.cmd_timer);
	TimerCountdownMS(&mqttCb->client.cmd_timer, mqttCb->client.command_timeout_ms);
	res = MQTTUnsubscribe(&mqttCb->client, mqttCb->topic[j]);
	if (0 != res) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttunsub] Unsubscribe ERROR");
		resultNo = MQTT_UNSUBSCRIPTION_ERROR;
		goto end;
	}

end:
	if (1 == matched && NULL != mqttCb->topic[j]) {
		rtos_mem_free(mqttCb->topic[j]);
		mqttCb->topic[j] = NULL;
		mqttCb->client.messageHandlers[j].topicFilter = 0;
		mqttCb->client.messageHandlers[j].fp = NULL;
	}
	if (MQTT_OK != resultNo) {
		at_printf("\r\n%sERROR:%d\r\n", "+MQTTUNSUB:", resultNo);
		if (MQTT_ARGS_ERROR == resultNo) {
			at_mqttunsub_help();
		}
	} else {
		at_printf("\r\n%sOK\r\n", "+MQTTUNSUB:");
	}
}

static void at_mqttpub_help(void)
{
	at_printf("\r\n");
	at_printf("AT+MQTTPUB=<tcpConnid>,<msgid>[,<qos>,<retain>],<topic>,<msg>\r\n");
}

/****************************************************************
Function            at_mqttpub
Brief                   MQTT publish. The command is used to publish mqtt issue.
****************************************************************/
void at_mqttpub(void *arg)
{
	const u8 idIndex = 1, msgidIndex = 2, qosIndex = 3, retainIndex = 4, topicIndex = 5, msgIndex = 6;
	s32 i = 0, len = 0, res = 0;
	MQTT_RESULT_ENUM resultNo = MQTT_OK;
	char *argv[MAX_ARGC] = {0};
	u8 tcpConnId = MQTT_MAX_CLIENT_NUM;
	u16 msgId = 0;
	MQTT_CONTROL_BLOCK *mqttCb = NULL;

	/* Get the parameters of AT command. */
	if (!arg) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttpub] Usage : at_mqttpub=<tcpConnid>,<msgid>[,<qos>,<retain>],<topic>,<msg>");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	i = parse_param_advance(arg, argv);
	if (7 != i) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttpub] Usage : at_mqttpub=<tcpConnid>,<msgid>[,<qos>,<retain>],<topic>,<msg>");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	/* Connect ID. */
	res = -1;
	if (1 == strlen(argv[idIndex])
		&& '0' <= argv[idIndex][0] && '0' + MQTT_MAX_CLIENT_NUM > argv[idIndex][0]) {
		res = argv[idIndex][0] - '0';
	}
	if (0 > res || MQTT_MAX_CLIENT_ID_LEN <= res) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttpub] Invalid connect ID");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}
	tcpConnId = (u8)res;

	resultNo = mqtt_get_handle_cb(tcpConnId, &mqttCb, 0);
	if (MQTT_OK != resultNo) {
		resultNo = MQTT_NOT_OPEN_ERROR;
		goto end;
	}

	/* msg ID. */
	res = (0 != strlen(argv[msgidIndex])) ? atoi(argv[msgidIndex]) : -1;
	if (0 >= res || 0xFFFF <= res) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttpub] Invalid msg ID");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}
	msgId = (u16)res;

	/* QoS. */
	mqttCb->pubData.qos = MQTT_DEFAULT_QOS;
	len = strlen(argv[qosIndex]);
	if (0 != len) {
		res = atoi(argv[qosIndex]);
		if (QOS0 > res || QOS2 < res) {
			RTK_LOGI(NOTAG, "\r\n[at_mqttpub] Invalid QoS");
			resultNo = MQTT_ARGS_ERROR;
			goto end;
		}
		mqttCb->pubData.qos = (u8)res;
	}

	/* retain. */
	mqttCb->pubData.retain = MQTT_DEFAULT_RETAIN;
	len = strlen(argv[retainIndex]);
	if (0 != len) {
		res = atoi(argv[retainIndex]);
		if (0 > res || 1 < res) {
			RTK_LOGI(NOTAG, "\r\n[at_mqttpub] Invalid retain");
			resultNo = MQTT_ARGS_ERROR;
			goto end;
		}
		mqttCb->pubData.retain = (u8)res;
	}

	/* topic. */
	len = strlen(argv[topicIndex]);
	if (0 == len || MQTT_MAX_TOPIC_LEN < len) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttpub] Invalid topic");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}
	resultNo = mqtt_string_copy(&mqttCb->pubData.topic, argv[topicIndex], len);
	if (MQTT_OK != resultNo) {
		goto end;
	}

	/* msg. */
	len = strlen(argv[msgIndex]);
	if (0 == len && MQTT_MAX_MSG_LEN < len) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttpub] Invalid msg");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}
	resultNo = mqtt_string_copy(&mqttCb->pubData.msg, argv[msgIndex], len);
	if (MQTT_OK != resultNo) {
		goto end;
	}

	/* send publish. */
	if (NULL == mqttCb->pubData.msg || NULL == mqttCb->pubData.topic) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttpub] Not prepared");
		resultNo = MQTT_COMMON_ERROR;
		goto end;
	}
	if (1 != mqttCb->client.isconnected) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttpub] Invalid status (%d)", mqttCb->client.isconnected);
		resultNo = MQTT_NOT_CONNECTED_ERROR;
		goto end;
	}
	MQTTMessage mqttMsg = {.qos = mqttCb->pubData.qos,
						   .retained = mqttCb->pubData.retain,
						   .dup = 0,
						   .id = msgId,
						   .payload = mqttCb->pubData.msg,
						   .payloadlen = strlen(mqttCb->pubData.msg)
						  };
	res = MQTTPublish(&mqttCb->client, mqttCb->pubData.topic, &mqttMsg);
	if (0 != res) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttpub] Publish ERROR");
		resultNo = MQTT_PUBLISH_ERROR;
		goto end;
	}

end:
	/* After sent, clean the msg and topic. */
	if (NULL != mqttCb) {
		rtos_mem_free(mqttCb->pubData.topic);
		mqttCb->pubData.topic = NULL;
		rtos_mem_free(mqttCb->pubData.msg);
		mqttCb->pubData.msg = NULL;
	}
	if (MQTT_OK != resultNo) {
		at_printf("\r\n%sERROR:%d\r\n", "+MQTTPUB:", resultNo);
		if (MQTT_ARGS_ERROR == resultNo) {
			at_mqttpub_help();
		}
	} else {
		at_printf("\r\n%sOK\r\n", "+MQTTPUB:");
	}
}

static void at_mqttcfg_help(void)
{
	at_printf("\r\n");
	at_printf("AT+MQTTCFG=<tcpConnid>,<option>[,<params>]");
	at_printf("\tGet the parameters:\r\n");
	at_printf("\tAT+MQTTCFG=1,?\r\n");
	at_printf("\tSet the parameters:\r\n");
	at_printf("\tAT+MQTTCFG=1,version,4\r\n");
	at_printf("\tAT+MQTTCFG=1,keepalive,0\r\n");
	at_printf("\tAT+MQTTCFG=1,session,0\r\n");
	at_printf("\tAT+MQTTCFG=1,timeout,30000\r\n");
	at_printf("\tAT+MQTTCFG=1,will,0\r\n");
	at_printf("\tWhen will is 1, the parameters are qos, retain, topic, message:\r\n");
	at_printf("\tAT+MQTTCFG=1,will,1,0,1,topic_string,message_string\r\n");
	at_printf("\tAT+MQTTCFG=1,ssl,0\r\n");
}

/****************************************************************
Function            at_mqttcfg
Brief                   MQTT configure. The command is used to configure mqtt parameters.
****************************************************************/
void at_mqttcfg(void *arg)
{
	const u8 idIndex = 1, paramIndex = 2, valueIndex = 3;
	u8  argc = 0;
	s32 res = 0;
	MQTT_RESULT_ENUM resultNo = MQTT_OK;
	char *argv[MAX_ARGC] = {0};
	u8  tcpConnId = MQTT_MAX_CLIENT_NUM, cmdIdx = MQTT_CONFIG_CMD_NUM;
	MQTT_CONTROL_BLOCK *mqttCb = NULL;

	/* Get the parameters of AT command. */
	if (!arg) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttcfg] Usage : at_mqttcfg=<id>,? or at_mqttcfg=<id>,<parameter>,<value>");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	argc = parse_param_advance(arg, argv);
	if (3 > argc) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttcfg] Input wrong parameter");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	/* Connect ID. */
	res = -1;
	if (1 == strlen(argv[idIndex])
		&& '0' <= argv[idIndex][0] && '0' + MQTT_MAX_CLIENT_NUM > argv[idIndex][0]) {
		res = argv[idIndex][0] - '0';
	}
	if (MQTT_MAX_CLIENT_NUM <= res || 0 > res) {
		RTK_LOGI(NOTAG, "\r\n[at_mqttcfg] Invalid tcpConnId");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}
	tcpConnId = (u8)res;

	resultNo = mqtt_get_handle_cb(tcpConnId, &mqttCb, 0);
	if (MQTT_OK != resultNo) {
		resultNo = MQTT_NOT_OPEN_ERROR;
		RTK_LOGI(NOTAG, "\r\n[at_mqttcfg] The connect ID does not exist");
		goto end;
	}

	/* Get command. */
	if ('?' == argv[paramIndex][0]) {
		at_printf("%sMQTTVersion %d\r\n", "+MQTTCFG:", mqttCb->connectData.MQTTVersion);
		at_printf("%skeepAliveInterval %d\r\n", "+MQTTCFG:", mqttCb->connectData.keepAliveInterval);
		at_printf("%scleansession %d\r\n", "+MQTTCFG:", mqttCb->connectData.cleansession);
		at_printf("%scommand_timeout_ms %d (ms)\r\n", "+MQTTCFG:", mqttCb->client.command_timeout_ms);
		at_printf("%swillFlag %d\r\n", "+MQTTCFG:", mqttCb->connectData.willFlag);
		if (mqttCb->connectData.willFlag) {
			MQTTPacket_willOptions *will = &mqttCb->connectData.will;
			at_printf("%sqos %d, retained %d, topicName %s, message %s\r\n",
					  "+MQTTCFG:", will->qos, will->retained, will->topicName.cstring, will->message.cstring);
		}
#if MQTT_OVER_SSL
		at_printf("%suseSsl %d\r\n", "+MQTTCFG:", mqttCb->useSsl);
#else
		at_printf("%suseSsl not support\r\n", "+MQTTCFG:");
#endif
	}

	/* Set command. */
	else {
		cmdIdx = mqtt_config_get_index(argv[paramIndex]);
		switch (cmdIdx) {
		/* "version" */
		case MQTT_CONFIG_VERSION:
			if (4 > argc) {
				RTK_LOGI(NOTAG, "\r\n[at_mqttcfg] No version number");
				resultNo = MQTT_ARGS_ERROR;
				goto end;
			}
			res = (0 != strlen(argv[valueIndex])) ? atoi(argv[valueIndex]) : -1;
			if (3 != res && 4 != res) {
				RTK_LOGI(NOTAG, "\r\n[at_mqttcfg] Invalid version number");
				resultNo = MQTT_ARGS_ERROR;
				goto end;
			}
			mqttCb->connectData.MQTTVersion = (u8)res;
			break;

		/* "keepalive" */
		case MQTT_CONFIG_KEEPALIVE:
			if (4 > argc) {
				RTK_LOGI(NOTAG, "\r\n[at_mqttcfg] No keepalive");
				resultNo = MQTT_ARGS_ERROR;
				goto end;
			}
			res = (0 != strlen(argv[valueIndex])) ? atoi(argv[valueIndex]) : -1;
			if (0 >= res || 3600 < res) {
				RTK_LOGI(NOTAG, "\r\n[at_mqttcfg] Invalid keepalive");
				resultNo = MQTT_ARGS_ERROR;
				goto end;
			}
			mqttCb->connectData.keepAliveInterval = (u16)res;
			break;

		/* "session" */
		case MQTT_CONFIG_SESSION:
			if (4 > argc) {
				RTK_LOGI(NOTAG, "\r\n[at_mqttcfg] No session");
				resultNo = MQTT_ARGS_ERROR;
				goto end;
			}
			res = (0 != strlen(argv[valueIndex])) ? atoi(argv[valueIndex]) : -1;
			if (0 != res && 1 != res) {
				RTK_LOGI(NOTAG, "\r\n[at_mqttcfg] Invalid session");
				resultNo = MQTT_ARGS_ERROR;
				goto end;
			}
			mqttCb->connectData.cleansession = (u8)res;
			break;

		/* "timeout" */
		case MQTT_CONFIG_TIMEOUT:
			if (4 > argc) {
				RTK_LOGI(NOTAG, "\r\n[at_mqttcfg] No timeout");
				resultNo = MQTT_ARGS_ERROR;
				goto end;
			}
			res = (0 != strlen(argv[valueIndex])) ? atoi(argv[valueIndex]) : -1;
			if (10000 > res || 60000 < res) {
				RTK_LOGI(NOTAG, "\r\n[at_mqttcfg] Invalid timeout");
				resultNo = MQTT_ARGS_ERROR;
				goto end;
			}
			mqttCb->client.command_timeout_ms = (u32)res;
			break;

		/* "will" */
		case MQTT_CONFIG_WILL:
			if (4 > argc) {
				RTK_LOGI(NOTAG, "\r\n[at_mqttcfg] No will value");
				resultNo = MQTT_ARGS_ERROR;
				goto end;
			}
			res = (0 != strlen(argv[valueIndex])) ? atoi(argv[valueIndex]) : -1;
			if (0 == res) {
				mqttCb->connectData.willFlag = 0;
			} else if (1 == res) {
				if (8 > argc) {
					RTK_LOGI(NOTAG, "\r\n[at_mqttcfg] No will value");
					resultNo = MQTT_ARGS_ERROR;
					goto end;
				}
				mqttCb->connectData.willFlag = 1;
				mqttCb->connectData.will.qos = (s8)atoi(argv[valueIndex + 1]);
				mqttCb->connectData.will.retained = (u8)atoi(argv[valueIndex + 2]);
				resultNo = mqtt_string_copy(&mqttCb->connectData.will.topicName.cstring, argv[valueIndex + 3], strlen(argv[valueIndex + 3]));
				if (MQTT_OK != resultNo) {
					goto end;
				}
				resultNo = mqtt_string_copy(&mqttCb->connectData.will.message.cstring, argv[valueIndex + 4], strlen(argv[valueIndex + 4]));
				if (MQTT_OK != resultNo) {
					goto end;
				}
			} else {
				RTK_LOGI(NOTAG, "\r\n[at_mqttcfg] Invalid will value");
				resultNo = MQTT_ARGS_ERROR;
				goto end;
			}
			break;

		/* "ssl" */
		case MQTT_CONFIG_SSL:
#if MQTT_OVER_SSL
			if (4 > argc) {
				RTK_LOGI(NOTAG, "\r\n[at_mqttcfg] No ssl");
				resultNo = MQTT_ARGS_ERROR;
				goto end;
			}
			res = (0 != strlen(argv[valueIndex])) ? atoi(argv[valueIndex]) : -1;
			if (0 != res && 1 != res) {
				RTK_LOGI(NOTAG, "\r\n[at_mqttcfg] Invalid ssl");
				resultNo = MQTT_ARGS_ERROR;
				goto end;
			}
			mqttCb->useSsl = (u8)res;
#else
			RTK_LOGI(NOTAG, "\r\n[at_mqttcfg] Not support");
			resultNo = MQTT_ARGS_ERROR;
			goto end;
#endif
			break;

		default:
			RTK_LOGI(NOTAG, "\r\n[at_mqttcfg] Input error parameters");
			resultNo = MQTT_ARGS_ERROR;
			goto end;
			break;
		}
	}

end:
	if (MQTT_OK == resultNo) {
		at_printf("\r\n%sOK\r\n", "+MQTTCFG:");
	} else {
		at_printf("\r\n%sERROR:%d\r\n", "+MQTTCFG:", resultNo);
		if (MQTT_ARGS_ERROR == resultNo) {
			at_mqttcfg_help();
		}
	}
	return;
}

/****************************************************************
Function            at_mqttreset
Brief                   MQTT reset, If you forget the procedure, or how many connections you have created, you can reset all.
****************************************************************/
void at_mqttreset(void *arg)
{
	int i = 0, res = 0;
	MQTT_CONTROL_BLOCK *mqttCb = NULL;
	MQTT_AT_HANDLE *mqttAtHandle = &mqtt_at_handle;

	UNUSED(arg);

	RTK_LOGI(NOTAG, "\r\n[at_mqttreset] reset all mqtt connections");
	/* No need any other parameters. */
	for (; MQTT_MAX_CLIENT_NUM > i; i++) {
		mqttCb = mqttAtHandle->mqttCb[i];
		if (NULL != mqttCb) {
			/* Disconnect client. */
			if (mqttCb->client.isconnected) {
				RTK_LOGI(NOTAG, "\r\n[at_mqttreset] Still connected");
				res = MQTTDisconnect(&mqttCb->client);
				if (0 != res) {
					RTK_LOGI(NOTAG, "\r\n[at_mqttreset] Can not disconnect");
					/* Continue to stop tasks, do not break here. */
				}
				mqttCb->client.isconnected = 0;
			}

			/* Stop the task. */
			if (MQTT_TASK_START == mqttCb->taskState) {
				mqttCb->taskState = MQTT_TASK_STOP;
				while (MQTT_TASK_NOT_CREATE != mqttCb->taskState) {
					rtos_time_delay_ms(20);
				}
			}

			/* Disconnect host. */
			if (mqttCb->networkConnect) {
				if (NULL != mqttCb->network.disconnect) {
					RTK_LOGI(NOTAG, "\r\n[at_mqttreset] Disconnect from %s", mqttCb->host);
					mqttCb->network.disconnect(&mqttCb->network);
				}
				mqttCb->networkConnect = 0;
			}

			// NetworkDeinit(&mqttCb->network);
			mqttCb->network.my_socket = -1;
			mqttCb->network.mqttread = NULL;
			mqttCb->network.mqttwrite = NULL;
			mqttCb->network.disconnect = NULL;
#if MQTT_OVER_SSL
			mqttCb->network.use_ssl = 0;
			mqttCb->network.ssl = NULL;
			mqttCb->network.conf = NULL;
			mqttCb->network.rootCA = NULL;
			mqttCb->network.clientCA = NULL;
			mqttCb->network.private_key = NULL;
#endif
			mqttCb->network.ciphersuites = NULL;

			/* Clean the Client Control block. */
			mqtt_del_handle_cb(&mqttCb);
			mqtt_set_handle_cb(i, mqttCb);
		}
	}

	/* This command only output "OK". */
	at_printf("\r\n%sOK\r\n", "+MQTTRESET:");
}

log_item_t at_mqtt_items[ ] = {
	{"+MQTTOPEN", at_mqttopen, {NULL, NULL}},  /* MQTT open. */
	{"+MQTTCLOSE", at_mqttclose, {NULL, NULL}},  /* MQTT close. */
	{"+MQTTCONN", at_mqttconn, {NULL, NULL}},  /* MQTT connect. */
	{"+MQTTDISCONN", at_mqttdisconn, {NULL, NULL}},  /* MQTT disconnect. */
	{"+MQTTSUB", at_mqttsub, {NULL, NULL}},  /* MQTT subscribe. */
	{"+MQTTUNSUB", at_mqttunsub, {NULL, NULL}},  /* MQTT unsubscribe. */
	{"+MQTTPUB", at_mqttpub, {NULL, NULL}},  /* MQTT publish. */
	{"+MQTTCFG", at_mqttcfg, {NULL, NULL}},   /* MQTT configure. */
	{"+MQTTRESET", at_mqttreset, {NULL, NULL}}   /* MQTT reset. */
};

/* Get the Client Control block for tcpConnId.
If the needInit == 1, we should initialise this Control block no matter it was initialised already. */
static MQTT_RESULT_ENUM mqtt_get_handle_cb(u8 tcpConnId, MQTT_CONTROL_BLOCK **mqttCb, u8 needInit)
{
	MQTT_RESULT_ENUM resultNo = MQTT_OK;
	MQTT_AT_HANDLE *mqttAtHandle = &mqtt_at_handle;

	if (NULL == mqttCb || MQTT_MAX_CLIENT_NUM <= tcpConnId) {
		RTK_LOGI(NOTAG, "\r\n[mqtt_get_handle_cb] Invalid parameters.");
		resultNo = MQTT_COMMON_ERROR;
		goto end;
	}

	/* Release older one. */
	if (1 == needInit && NULL != mqttAtHandle->mqttCb[tcpConnId]) {
		RTK_LOGI(NOTAG, "\r\n[mqtt_get_handle_cb] Cover the older information for %d", tcpConnId);
		mqtt_del_handle_cb(&mqttAtHandle->mqttCb[tcpConnId]);
	}

	if (NULL == mqttAtHandle->mqttCb[tcpConnId]) {
		/* Need to initialised. */
		if (1 == needInit) {
			resultNo = mqtt_init_handle_cb(mqttCb);
			if (MQTT_OK != resultNo) {
				goto end;
			}
			mqttAtHandle->mqttCb[tcpConnId] = *mqttCb;
		}
		/* If this is not the first time used. */
		else {
			RTK_LOGI(NOTAG, "\r\n[mqtt_get_handle_cb] NULL param.");
			resultNo = MQTT_MEMORY_ERROR;
			*mqttCb = NULL;
		}
	}
	/* Get it directly. */
	else {
		*mqttCb = mqttAtHandle->mqttCb[tcpConnId];
	}

end:
	return resultNo;
}

/* Remove the Client Control block, free the malloced memories. */
static void mqtt_del_handle_cb(MQTT_CONTROL_BLOCK **mqttCb)
{
	s8 i = 0;
	MQTT_CONTROL_BLOCK *pMqttCb = NULL;

	if (NULL == mqttCb || NULL == *mqttCb) {
		goto end;
	}
	pMqttCb = *mqttCb;

	mqtt_del_client_buf(&pMqttCb->client);

	/* Delete memory malloced. */
	rtos_mem_free(pMqttCb->connectData.will.message.cstring);
	pMqttCb->connectData.will.message.cstring = NULL;
	rtos_mem_free(pMqttCb->connectData.will.topicName.cstring);
	pMqttCb->connectData.will.topicName.cstring = NULL;

	/* Connection information and open information. */
	rtos_mem_free(pMqttCb->clientId);
	rtos_mem_free(pMqttCb->userName);
	rtos_mem_free(pMqttCb->password);
	rtos_mem_free(pMqttCb->host);
	for (i = 0; MAX_MESSAGE_HANDLERS > i; i++) {
		rtos_mem_free(pMqttCb->topic[i]);
	}
	rtos_mem_free(pMqttCb->pubData.topic);
	rtos_mem_free(pMqttCb->pubData.msg);
	rtos_mem_free(pMqttCb);
	*mqttCb = NULL;

end:
	return;
}

static void mqtt_set_handle_cb(u8 tcpConnId, MQTT_CONTROL_BLOCK *mqttCb)
{
	if (MQTT_MAX_CLIENT_NUM > tcpConnId) {
		mqtt_at_handle.mqttCb[tcpConnId] = mqttCb;
	}
}

static MQTT_RESULT_ENUM mqtt_init_handle_cb(MQTT_CONTROL_BLOCK **pMqttCb)
{
	MQTT_CONTROL_BLOCK *mqttCb = NULL;
	MQTT_RESULT_ENUM resultNo = MQTT_OK;

	/* Malloc memory for this block. */
	mqttCb = (MQTT_CONTROL_BLOCK *)rtos_mem_malloc(sizeof(MQTT_CONTROL_BLOCK));
	if (NULL == mqttCb) {
		RTK_LOGI(NOTAG, "\r\n[mqtt_init_handle_cb] Memory failure.");
		resultNo = MQTT_MEMORY_ERROR;
		goto end;
	}

	/* Initialise. */
	memset(mqttCb, 0, sizeof(MQTT_CONTROL_BLOCK));
	memcpy(&mqttCb->connectData, &mqtt_default_conn_data, sizeof(MQTTPacket_connectData));
	mqttCb->port = MQTT_DEFAULT_PORT;
	mqttCb->pubData.qos = MQTT_DEFAULT_QOS;
	mqttCb->pubData.retain = MQTT_DEFAULT_RETAIN;

	NetworkInit(&mqttCb->network);
	resultNo = mqtt_init_client_buf(&mqttCb->client, &mqttCb->network, MQTT_CMD_PKT_TIMEOUT_MS);
	if (MQTT_OK != resultNo) {
		RTK_LOGI(NOTAG, "\r\n[mqtt_init_handle_cb] Init client failure.");
		goto end;
	}
	mqttCb->client.cb = (void *)mqttCb;

end:
	if (MQTT_OK != resultNo) {
		rtos_mem_free(mqttCb);
		*pMqttCb = NULL;
	} else {
		*pMqttCb = mqttCb;
	}
	return resultNo;
}

/* Similar with MQTTClientInit( ), but the buf and readbuf can be malloced at first. */
static MQTT_RESULT_ENUM mqtt_init_client_buf(MQTTClient *client, Network *network, u32 timeout)
{
	u32 i = 0;
	MQTT_RESULT_ENUM resultNo = MQTT_OK;

	client->ipstack = network;

	for (; MAX_MESSAGE_HANDLERS > i; i++) {
		client->messageHandlers[i].topicFilter = 0;
	}
	client->defaultMessageHandler = mqtt_message_arrived;
	client->command_timeout_ms = timeout;

	/*  Create default buf, will be realloced when get larger msg && recover after being processed. */
	client->buf = (u8 *)rtos_mem_malloc(sizeof(u8) * MQTT_DEFAULT_SENDBUF_SIZE);
	if (NULL == client->buf) {
		resultNo = MQTT_MEMORY_ERROR;
		goto end;
	}
	client->buf_size = MQTT_DEFAULT_SENDBUF_SIZE;
	client->readbuf = (u8 *)rtos_mem_malloc(sizeof(u8) * MQTT_DEFAULT_SENDBUF_SIZE);
	if (NULL == client->readbuf) {
		resultNo = MQTT_MEMORY_ERROR;
		goto end;
	}
	client->readbuf_size = MQTT_DEFAULT_SENDBUF_SIZE;

	client->isconnected = 0;
	client->ping_outstanding = 0;
	client->next_packetid = 1;
	client->mqttstatus = MQTT_START;
	client->ipstack->m2m_rxevent = 0;
	TimerInit(&client->cmd_timer);
	TimerInit(&client->ping_timer);

end:
	if (MQTT_OK != resultNo) {
		rtos_mem_free(client->buf);
		client->buf = NULL;
		client->buf_size = 0;
		rtos_mem_free(client->readbuf);
		client->readbuf = NULL;
		client->readbuf_size = 0;
	}
	return resultNo;
}

/* Corresponding to mqtt_init_client_buf( ). */
static void mqtt_del_client_buf(MQTTClient *client)
{
	if (NULL != client) {
		rtos_mem_free(client->buf);
		client->buf = NULL;
		client->buf_size = 0;
		rtos_mem_free(client->readbuf);
		client->readbuf = NULL;
		client->readbuf_size = 0;
		memset(client, 0, sizeof(MQTTClient));
	}
}

static void mqtt_message_arrived(MessageData *data, void *param)
{
	MQTT_CONTROL_BLOCK *mqttCb = (MQTT_CONTROL_BLOCK *)param;
	char *uns_buff = NULL;
	unsigned int len_out = 0, data_shift = 0, total_data_len = 0;
	char num_buf[8];
	unsigned short id = 0;
	u8 tcpconnectid = 0;
	char *topicdest = NULL, *topicsrc = NULL;
	int topiclen = 0;

	if (mqttCb == NULL) {
		RTK_LOGI(NOTAG, "\r\n[mqtt_message_arrived] Nothing to do.");
		goto end;
	}

	id = data->message->id;
	tcpconnectid = mqttCb->tcpConnectId;

	if (data->topicName->cstring != NULL) {
		topicsrc = data->topicName->cstring;
		topiclen = strlen(topicsrc);
	} else {
		topicsrc = data->topicName->lenstring.data;
		topiclen = data->topicName->lenstring.len;
	}

	topicdest = (char *)rtos_mem_zmalloc(topiclen + 1);
	if (topicdest == NULL) {
		RTK_LOGI(NOTAG, "\r\n[mqtt_message_arrived] Memory Failure for topicdest.");
		goto end;
	}
	strncpy(topicdest, topicsrc, topiclen);

	data_shift = strlen("%s:%d,%d,%s,") + strlen("+MQTTRECV") - 2	+ sprintf(num_buf, "%d", tcpconnectid) - 2 + sprintf(num_buf, "%d",
				 id) - 2 + strlen(topicdest) - 2;
	len_out = data_shift + data->message->payloadlen + 1;
	total_data_len = strlen(",%d,%d,%d,%d") + sprintf(num_buf, "%d", data->message->payloadlen) - 2 + sprintf(num_buf, "%d", data->message->qos) - 2
					 + sprintf(num_buf, "%d", data->message->dup) - 2 + sprintf(num_buf, "%d", data->message->retained) - 2 + len_out;

	uns_buff = (char *)rtos_mem_zmalloc(total_data_len);
	if (uns_buff == NULL) {
		RTK_LOGI(NOTAG, "\r\n[mqtt_message_arrived] Memory Failure for uns_buff.");
		goto end;
	}

	DiagSnPrintf(uns_buff, len_out, "%s:%d,%d,%s,", "+MQTTRECV", tcpconnectid, id, topicdest);
	memcpy((uns_buff + data_shift), (char *)data->message->payload, (int16_t)(data->message->payloadlen));
	DiagSnPrintf(uns_buff + strlen(uns_buff), total_data_len - strlen(uns_buff), ",%d,%d,%d,%d", data->message->payloadlen, data->message->qos, data->message->dup,
				 data->message->retained);

	at_printf("%s\r\n", uns_buff);

end:
	rtos_mem_free(topicdest);
	rtos_mem_free(uns_buff);
}

static MQTT_RESULT_ENUM mqtt_string_copy(char **dest, char *src, size_t sz)
{
	MQTT_RESULT_ENUM resultNo = MQTT_OK;

	if (NULL == src) {
		RTK_LOGI(NOTAG, "\r\n[mqtt_string_copy] ARGS error.");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	/* Delete the elder string. */
	rtos_mem_free(*dest);
	/* sz + 1, the 1 is used for '\0'. */
	*dest = (char *)rtos_mem_malloc(sizeof(char) * (sz + 1));
	if (NULL == *dest) {
		RTK_LOGI(NOTAG, "\r\n[mqtt_string_copy] Memory failure.");
		resultNo = MQTT_MEMORY_ERROR;
		goto end;
	}
	strcpy(*dest, src);
	(*dest)[sz] = '\0';

end:
	if (MQTT_OK != resultNo) {
		rtos_mem_free(*dest);
		*dest = NULL;
	}
	return resultNo;
}

static void mqtt_clent_alive_fail(MQTT_CONTROL_BLOCK *mqttCb)
{
	int i = 0;

	if (NULL == mqttCb) {
		RTK_LOGI(NOTAG, "\r\n[mqtt_clent_alive_fail] Invalid param");
		return;
	}

	if (1 == mqttCb->client.isconnected) {
		mqttCb->client.isconnected = 0;
		mqttCb->offline = 1;
		for (i = 0; MAX_MESSAGE_HANDLERS > i; i++) {
			rtos_mem_free(mqttCb->topic[i]);
			mqttCb->topic[i] = NULL;
		}
		if (0 == mqttCb->initialConnect) {
			at_printf("\r\n%sOK\r\n", "+MQTTUNREACH:");
		} else {
			/* Do not use at_printf here. */
			RTK_LOGI(NOTAG, "\r\n[mqtt_clent_alive_fail] Connect failed.");
		}
	}
	mqttCb->client.ping_outstanding = 0;
	mqttCb->client.next_packetid = 1;

	if (mqttCb->networkConnect) {
		if (NULL != mqttCb->network.disconnect) {
			mqttCb->network.disconnect(&mqttCb->network);
		}
		mqttCb->networkConnect = 0;
	}
	RTK_LOGI(NOTAG, "\r\n[mqtt_clent_alive_fail] Set to MQTT_START");
	mqttCb->client.mqttstatus = MQTT_START;
}

/* Process the received data.
 The res has different meaning with resultNo.
 The resultNo is used for AT command output. The res is used for set mqtt status. */
static MQTT_RESULT_ENUM mqtt_clent_data_proc(MQTT_CONTROL_BLOCK *mqttCb, fd_set *read_fds, u8 *needAtOutpput)
{
	s16 packet_type = 0;
	s32 res = 0, mqttStatus = 0, mqttRxEvent = 0, tmp = 0;
	Timer timer;
	MQTT_RESULT_ENUM resultNo = MQTT_OK;

	if (NULL == mqttCb || NULL == read_fds) {
		RTK_LOGI(NOTAG, "\r\n[mqtt_clent_data_proc] Invalid params");
		*needAtOutpput = 1;
		resultNo = MQTT_COMMON_ERROR;
		goto end;
	}

	*needAtOutpput = 0;
	mqttStatus = mqttCb->client.mqttstatus;
	mqttRxEvent = (0 <= mqttCb->client.ipstack->my_socket) ? FD_ISSET(mqttCb->client.ipstack->my_socket, read_fds) : 0;

	if (MQTT_START == mqttStatus) {
		/* Once disconnected, set offline = 1, and release each topic. */
		if (mqttCb->client.isconnected) {
			mqttCb->client.isconnected = 0;
			mqttCb->offline = 1;
			for (int i = 0; MAX_MESSAGE_HANDLERS > i; i++) {
				rtos_mem_free(mqttCb->topic[i]);
				mqttCb->topic[i] = NULL;
			}
			if (0 == mqttCb->initialConnect) {
				at_printf("\r\n%sOK\r\n", "+MQTTUNREACH:");
			} else {
				/* Do not use at_printf here. */
				RTK_LOGI(NOTAG, "\r\n[mqtt_clent_data_proc] Lost connection.");
			}
		}
		/* Try re-connect per second, except when initial-connect. */
		if (0 == mqttCb->initialConnect && 1 == mqttCb->offline && NULL != mqttCb->host && NULL != mqttCb->clientId) {
			rtos_time_delay_ms(1000);
			if (wifi_is_connected_to_ap() == RTW_SUCCESS) {
				if (0 >= mqttCb->network.my_socket && 1 == mqttCb->networkConnect && NULL != mqttCb->network.disconnect) {
					mqttCb->network.disconnect(&mqttCb->network);
					NetworkInit(&mqttCb->network);
					mqttCb->networkConnect = 0;
				}
				res = NetworkConnect(&mqttCb->network, mqttCb->host, mqttCb->port);
				if (0 != res) {
					// RTK_LOGI(NOTAG, "\r\n[mqtt_clent_data_proc] Reconnect failed");
					resultNo = MQTT_CONNECTION_ERROR;
					goto end;
				}
				mqttCb->networkConnect = 1;
				TimerInit(&mqttCb->client.cmd_timer);
				TimerCountdownMS(&mqttCb->client.cmd_timer, mqttCb->client.command_timeout_ms);
				res = MQTTConnect(&mqttCb->client, &mqttCb->connectData);
				if (0 != res) {
					// RTK_LOGI(NOTAG, "\r\n[mqtt_clent_data_proc] Reconnect MQTT failed");
					resultNo = MQTT_CONNECTION_ERROR;
					goto end;
				}
				mqttCb->client.mqttstatus = MQTT_CONNECT;
				mqttCb->offline = 0;
				RTK_LOGI(NOTAG, "\r\n[mqtt_clent_data_proc] Reconnect the network OK");
				at_printf("\r\n%sOK\r\n", "+MQTTREACH:");
			}
		}
		goto end;
	}

	if (mqttRxEvent) {
		mqttCb->client.ipstack->m2m_rxevent = 0;
		TimerInit(&timer);
		TimerCountdownMS(&timer, 1000);
		packet_type = readPacket(&mqttCb->client, &timer);
		if (CONNECT > packet_type || DISCONNECT < packet_type) {
			goto end;
		}
		RTK_LOGI(NOTAG, "\r\n[mqtt_clent_data_proc] Read packet %d", packet_type);
	}

	switch (mqttStatus) {
	/* MQTT_CONNECT status. */
	case MQTT_CONNECT:
		if (CONNACK == packet_type) {
			u8 connack_rc = 255, sessionPresent = 0;
			tmp = MQTTDeserialize_connack(&sessionPresent, &connack_rc, mqttCb->client.readbuf, mqttCb->client.readbuf_size);
			if (1 == tmp) {
				res = connack_rc;
				RTK_LOGI(NOTAG, "\r\n[mqtt_clent_data_proc] MQTT connected OK");
			} else {
				RTK_LOGI(NOTAG, "\r\n[mqtt_clent_data_proc] MQTT can not connect");
				res = FAILURE;
			}
			mqttCb->initialConnect = 0;
			*needAtOutpput = 1;
		} else if (PINGRESP == packet_type) {
			mqttCb->client.ping_outstanding = 0;
		} else if (TimerIsExpired(&mqttCb->client.cmd_timer)) {
			keepalive(&mqttCb->client);
		}
		if (FAILURE == res) {
			RTK_LOGI(NOTAG, "\r\n[mqtt_clent_data_proc] MQTT connected ERROR (%d)", mqttCb->client.isconnected);
			if (1 == mqttCb->client.isconnected) {
				res = MQTTDisconnect(&mqttCb->client);
				if (0 != res) {
					RTK_LOGI(NOTAG, "\r\n[mqtt_clent_data_proc] Disconnect ERROR");
				}
				mqttCb->client.isconnected = 0;
			}
			mqttCb->client.mqttstatus = MQTT_START;
		}
		break;

	/* MQTT_SUBTOPIC status. */
	case MQTT_SUBTOPIC:
		if (SUBACK == packet_type) {
			int count = 0, grantedQoS = -1;
			int i = 0;
			unsigned short mypacketid = 0;
			tmp = MQTTDeserialize_suback(&mypacketid, 1, &count, &grantedQoS, mqttCb->client.readbuf, mqttCb->client.readbuf_size);
			if (1 == tmp) {
				/* It may be 0, 1, 2 or 0x80. */
				res = grantedQoS;
				RTK_LOGI(NOTAG, "\r\n[mqtt_clent_data_proc] grantedQoS = %d", res);
			}
			if (0x80 != res) {
				for (i = 0; MAX_MESSAGE_HANDLERS > i; i++) {
					if (NULL != mqttCb->topic[i] && mqttCb->client.messageHandlers[i].topicFilter != mqttCb->topic[i]) {
						mqttCb->client.messageHandlers[i].topicFilter = mqttCb->topic[i];
						mqttCb->client.messageHandlers[i].fp = mqtt_message_arrived;
					}
				}
				res = 0;
				*needAtOutpput = 1;
				mqttCb->client.mqttstatus = MQTT_RUNNING;
			}
		} else if (UNSUBACK == packet_type) {
			RTK_LOGI(NOTAG, "\r\n[mqtt_clent_data_proc] Unsubscribe_end OK");
			mqttCb->client.mqttstatus = MQTT_CONNECT;
			*needAtOutpput = 1;
		} else if (TimerIsExpired(&mqttCb->client.cmd_timer)) {
			*needAtOutpput = 1;
			RTK_LOGI(NOTAG, "\r\n[mqtt_clent_data_proc] MQTT subscribe timeout");
			resultNo = MQTT_WAITACK_TIMEOUT_ERROR;
			res = FAILURE;
		}
		if (FAILURE == res) {
			RTK_LOGI(NOTAG, "\r\n[mqtt_clent_data_proc] MQTT subscribe ERROR");
			mqttCb->client.mqttstatus = MQTT_START;
		}
		break;

	/* MQTT_RUNNING status. */
	case MQTT_RUNNING:
		if (CONNECT <= packet_type) {
			s32 len = 0;
			TimerInit(&timer);
			TimerCountdownMS(&timer, 10000);
			switch (packet_type) {
			case SUBACK: {
				int count = 0, grantedQoS = -1;
				int i = 0;
				unsigned short mypacketid = 0;
				tmp = MQTTDeserialize_suback(&mypacketid, 1, &count, &grantedQoS, mqttCb->client.readbuf, mqttCb->client.readbuf_size);
				if (1 == tmp) {
					/* It may be 0, 1, 2 or 0x80. */
					res = grantedQoS;
					RTK_LOGI(NOTAG, "\r\n[mqtt_clent_data_proc] grantedQoS = %d", res);
				}
				if (0x80 != res) {
					for (i = 0; MAX_MESSAGE_HANDLERS > i; i++) {
						if (NULL != mqttCb->topic[i] && mqttCb->client.messageHandlers[i].topicFilter != mqttCb->topic[i]) {
							mqttCb->client.messageHandlers[i].topicFilter = mqttCb->topic[i];
							mqttCb->client.messageHandlers[i].fp = mqtt_message_arrived;
						}
					}
					res = 0;
					*needAtOutpput = 1;
					// mqttCb->client.mqttstatus = MQTT_RUNNING;
				}
			}
			break;

			case PUBACK: {
				u16 mypacketid = 0;
				u8 dup = 0, type = 0;
				tmp = MQTTDeserialize_ack(&type, &dup, &mypacketid, mqttCb->client.readbuf, mqttCb->client.readbuf_size);
				if (1 != tmp) {
					RTK_LOGI(NOTAG, "\r\n[mqtt_clent_data_proc] Deserialize_ack failed");
					res = FAILURE;
					resultNo = MQTT_PUBLISH_ERROR;
				}
				*needAtOutpput = 1;
			}
			break;

			case UNSUBACK: {
				u8 i = 0;
				RTK_LOGI(NOTAG, "\r\n[mqtt_clent_data_proc] Unsubscribe OK");
				for (; MAX_MESSAGE_HANDLERS > i && NULL == mqttCb->topic[i]; i++) {
					;
				}
				/* If there is(are) any topic(s) remained, stay in MQTT_RUNNING. */
				mqttCb->client.mqttstatus = (MAX_MESSAGE_HANDLERS == i) ? MQTT_CONNECT : MQTT_RUNNING;
				*needAtOutpput = 1;
			}
			break;

			case PUBLISH: {
				MQTTString topicName;
				MQTTMessage msg;
				int intQoS = 0;
				tmp = MQTTDeserialize_publish(&msg.dup, &intQoS, &msg.retained, &msg.id, &topicName,
											  (unsigned char **)&msg.payload, (int *)&msg.payloadlen, mqttCb->client.readbuf, mqttCb->client.readbuf_size);
				if (1 != tmp) {
					RTK_LOGI(NOTAG, "\r\n[mqtt_clent_data_proc] Deserialize PUBLISH failed");
					goto end;
				}
				msg.qos = intQoS;
				deliverMessage(&mqttCb->client, &topicName, &msg);
				/* QOS0 has no ack. */
				if (QOS0 != msg.qos) {
					if (QOS1 == msg.qos) {
						len = MQTTSerialize_ack(mqttCb->client.buf, mqttCb->client.buf_size, PUBACK, 0, msg.id);
						RTK_LOGI(NOTAG, "\r\n[mqtt_clent_data_proc] Send PUBACK");
					} else if (QOS2 == msg.qos) {
						len = MQTTSerialize_ack(mqttCb->client.buf, mqttCb->client.buf_size, PUBREC, 0, msg.id);
						RTK_LOGI(NOTAG, "\r\n[mqtt_clent_data_proc] Send PUBREC");
					} else {
						RTK_LOGI(NOTAG, "\r\n[mqtt_clent_data_proc] Invalid QoS %d", msg.qos);
						goto end;
					}
					/* Send failed. */
					if (0 >= len) {
						RTK_LOGI(NOTAG, "\r\n[mqtt_clent_data_proc] Serialize_ack failed");
						goto end;
					}
					tmp = sendPacket(&mqttCb->client, len, &timer);
					if (FAILURE == tmp) {
						RTK_LOGI(NOTAG, "\r\n[mqtt_clent_data_proc] send packet failed, set to start");
						mqttCb->client.mqttstatus = MQTT_START;
						goto end;
					}
				}
			}
			break;

			case PUBREC: {
				u16 mypacketid = 0;
				u8 dup = 0, type = 0;
				*needAtOutpput = 1;
				tmp = MQTTDeserialize_ack(&type, &dup, &mypacketid, mqttCb->client.readbuf, mqttCb->client.readbuf_size);
				if (1 != tmp) {
					RTK_LOGI(NOTAG, "\r\n[mqtt_clent_data_proc] Deserialize PUBREC failed");
					resultNo = MQTT_PUBLISH_ERROR;
				} else {
					len = MQTTSerialize_ack(mqttCb->client.buf, mqttCb->client.buf_size, PUBREL, 0, mypacketid);
					if (0 >= len) {
						RTK_LOGI(NOTAG, "\r\n[mqtt_clent_data_proc] Serialize PUBREL failed");
						resultNo = MQTT_PUBLISH_ERROR;
					} else {
						tmp = sendPacket(&mqttCb->client, len, &timer);
						if (FAILURE == tmp) {
							RTK_LOGI(NOTAG, "\r\n[mqtt_clent_data_proc] send packet failed");
							resultNo = MQTT_PUBLISH_ERROR;
							mqttCb->client.mqttstatus = MQTT_START;
						}
					}
				}
			}
			break;

			case PUBREL: {
				u16 mypacketid = 0;
				u8 dup = 0, type = 0;
				tmp = MQTTDeserialize_ack(&type, &dup, &mypacketid, mqttCb->client.readbuf, mqttCb->client.readbuf_size);
				if (1 != tmp) {
					RTK_LOGI(NOTAG, "\r\n[mqtt_clent_data_proc] Deserialize PUBREL failed");
					res = FAILURE;
				} else {
					len = MQTTSerialize_ack(mqttCb->client.buf, mqttCb->client.buf_size, PUBCOMP, 0, mypacketid);
					if (0 >= len) {
						RTK_LOGI(NOTAG, "\r\n[mqtt_clent_data_proc] Serialize PUBCOMP failed");
						res = FAILURE;
					} else {
						tmp = sendPacket(&mqttCb->client, len, &timer);
						if (FAILURE == tmp) {
							RTK_LOGI(NOTAG, "\r\n[mqtt_clent_data_proc] sent packet ERROR");
							res = FAILURE;
							mqttCb->client.mqttstatus = MQTT_START;
						}
					}
				}
			}
			break;

			case PINGRESP:
				mqttCb->client.ping_outstanding = 0;
				break;

			/* Ignore the other types. */
			default:
				break;
			}
		}
		keepalive(&mqttCb->client);
		break;

	/* The other status, ignored. */
	default:
		break;
	}

end:
	return resultNo;
}

static u8 mqtt_config_get_index(char *cmdString)
{
	u8 i = 0;

	for (; MQTT_CONFIG_CMD_NUM > i; i++) {
		if (0 == strcmp(mqtt_config_string[i], cmdString)) {
			return i;
		}
	}

	return i;
}

/* There are 4 connection IDs at most. */
void mqtt_init_info(void)
{
	u8 i = 0;
	MQTT_AT_HANDLE *mqttAtHandle = &mqtt_at_handle;

	for (; MQTT_MAX_CLIENT_NUM > i; i++) {
		mqttAtHandle->mqttCb[i] = NULL;
	}
}

void print_mqtt_at(void)
{
	int i, cmdSize;

	cmdSize = sizeof(at_mqtt_items) / sizeof(log_item_t);
	for (i = 0; cmdSize > i; i++) {
		at_printf("AT%s\r\n", at_mqtt_items[i].log_cmd);
	}
}

void at_mqtt_init(void)
{
	mqtt_init_info();

	atcmd_service_add_table(at_mqtt_items, sizeof(at_mqtt_items) / sizeof(at_mqtt_items[0]));
}

#endif /* CONFIG_MP_INCLUDED */
