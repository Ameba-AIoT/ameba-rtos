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

#if defined(CONFIG_ATCMD_MQTT) && (CONFIG_ATCMD_MQTT == 1)

static const char *TAG = "AT-MQTT";

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

MQTT_AT_HANDLE mqtt_at_handle;
MQTTPacket_connectData mqtt_default_conn_data = MQTTPacket_connectData_initializer;

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
		RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_main] Input invalid param\r\n");
		goto end;
	}

	while (1) {
		if (MQTT_TASK_START != mqttCb->taskState) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_main] Should stop now\r\n");
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
				at_printf_indicate("ACK\r\n");
			} else {
				at_printf_indicate(ATCMD_ERROR_END_STR, resultNo);
			}
		}
	}

end:
	RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_main] Stop mqtt task\r\n");
	mqttCb->taskState = MQTT_TASK_NOT_CREATE;
	mqttCb->taskHandle = NULL;
	rtos_task_delete(mqttCb->taskHandle);
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
		RTK_LOGS(TAG, RTK_LOG_INFO, "\r\n[at_mqttsub] Usage : at_mqttsub=<tcpconnectid>,<topic>,<qos>");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	i = parse_param_advance(arg, argv);
	if (3 > i || 4 < i) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "\r\n[at_mqttsub] Usage : at_mqttsub=<tcpconnectid>,<msgid>,<topic>,<qos>");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	/* link ID. */
	res = -1;
	if (1 == strlen(argv[idIndex])
		&& '0' <= argv[idIndex][0] && '0' + MQTT_MAX_CLIENT_NUM > argv[idIndex][0]) {
		res = argv[idIndex][0] - '0';
	}
	if (0 > res || MQTT_MAX_CLIENT_ID_LEN <= res) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "\r\n[at_mqttsub] Invalid link ID");
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
		RTK_LOGS(TAG, RTK_LOG_INFO, "\r\n[at_mqttsub] Invalid topic");
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
		RTK_LOGS(TAG, RTK_LOG_INFO, "\r\n[at_mqttsub] Can not match.");
		resultNo = (-2 == found) ? MQTT_ALREADY_SUBSCRIBED_ERROR : MQTT_SUBSCRIPTION_ERROR;
		goto end;
	}
	j = (u8)found;
	resultNo = mqtt_string_copy(&mqttCb->topic[j], argv[topicIndex], _strlen(argv[topicIndex]));
	if (MQTT_OK != resultNo) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "\r\n[at_mqttsub] mqtt_string_copy failed.");
		goto end;
	}

	/* QoS. This is optional, if not including here, it should be QOS2. */
	if (4 == i) {
		res = (0 != strlen(argv[qosIndex])) ? atoi(argv[qosIndex]) : -1;
		if (QOS0 > res || QOS2 < res) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "\r\n[at_mqttsub] Invalid QoS");
			resultNo = MQTT_ARGS_ERROR;
			goto end;
		}
		qos = (u8)res;
	}

	/* Check status. */
	if (1 != mqttCb->client.isconnected || 0 > mqttCb->network.my_socket) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "\r\n[at_mqttsub] Not connected now (%d, %d)",
				 mqttCb->client.isconnected, mqttCb->network.my_socket);
		resultNo = MQTT_NOT_CONNECTED_ERROR;
		goto end;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "\r\n[at_mqttsub] Subscribe topic %s", mqttCb->topic[j]);

	TimerInit(&mqttCb->client.cmd_timer);
	TimerCountdownMS(&mqttCb->client.cmd_timer, mqttCb->client.command_timeout_ms);
	res = MQTTSubscribe(&mqttCb->client, mqttCb->topic[j], qos, mqtt_message_arrived);
	if (0 != res) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "\r\n[at_mqttsub] Subscribe ERROR");
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
		at_printf(ATCMD_ERROR_END_STR, resultNo);
	} else {
		at_printf(ATCMD_OK_END_STR);
	}
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
		RTK_LOGS(TAG, RTK_LOG_INFO, "\r\n[at_mqttunsub] Usage : at_mqttunsub=<tcpconnectid>,<topic>");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	i = parse_param_advance(arg, argv);
	if (3 != i) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "\r\n[at_mqttunsub] Usage : at_mqttunsub=<tcpconnectid>,<topic>");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	/* link ID. */
	res = -1;
	if (1 == strlen(argv[idIndex])
		&& '0' <= argv[idIndex][0] && '0' + MQTT_MAX_CLIENT_NUM > argv[idIndex][0]) {
		res = argv[idIndex][0] - '0';
	}
	if (0 > res || MQTT_MAX_CLIENT_ID_LEN <= res) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "\r\n[at_mqttunsub] Invalid link ID");
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
		RTK_LOGS(TAG, RTK_LOG_INFO, "\r\n[at_mqttunsub] Invalid topic");
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
		RTK_LOGS(TAG, RTK_LOG_INFO, "\r\n[at_mqttunsub] Invalid topic");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	/* Check status. */
	if (1 != mqttCb->client.isconnected || 0 > mqttCb->network.my_socket) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "\r\n[at_mqttunsub] Invalid status (%d, %d)",
				 mqttCb->client.isconnected, mqttCb->network.my_socket);
		resultNo = MQTT_UNSUBSCRIPTION_ERROR;
		goto end;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "\r\n[at_mqttunsub] Unsubscribe topic %s", mqttCb->topic[j]);

	TimerInit(&mqttCb->client.cmd_timer);
	TimerCountdownMS(&mqttCb->client.cmd_timer, mqttCb->client.command_timeout_ms);
	res = MQTTUnsubscribe(&mqttCb->client, mqttCb->topic[j]);
	if (0 != res) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "\r\n[at_mqttunsub] Unsubscribe ERROR");
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
		at_printf(ATCMD_ERROR_END_STR, resultNo);
	} else {
		at_printf(ATCMD_OK_END_STR);
	}
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
		RTK_LOGS(TAG, RTK_LOG_INFO, "\r\n[at_mqttpub] Usage : at_mqttpub=<tcpConnid>,<msgid>[,<qos>,<retain>],<topic>,<msg>");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	i = parse_param_advance(arg, argv);
	if (7 != i) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "\r\n[at_mqttpub] Usage : at_mqttpub=<tcpConnid>,<msgid>[,<qos>,<retain>],<topic>,<msg>");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	/* link ID. */
	res = -1;
	if (1 == strlen(argv[idIndex])
		&& '0' <= argv[idIndex][0] && '0' + MQTT_MAX_CLIENT_NUM > argv[idIndex][0]) {
		res = argv[idIndex][0] - '0';
	}
	if (0 > res || MQTT_MAX_CLIENT_ID_LEN <= res) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "\r\n[at_mqttpub] Invalid link ID");
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
		RTK_LOGS(TAG, RTK_LOG_INFO, "\r\n[at_mqttpub] Invalid msg ID");
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
			RTK_LOGS(TAG, RTK_LOG_INFO, "\r\n[at_mqttpub] Invalid QoS");
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
			RTK_LOGS(TAG, RTK_LOG_INFO, "\r\n[at_mqttpub] Invalid retain");
			resultNo = MQTT_ARGS_ERROR;
			goto end;
		}
		mqttCb->pubData.retain = (u8)res;
	}

	/* topic. */
	len = strlen(argv[topicIndex]);
	if (0 == len || MQTT_MAX_TOPIC_LEN < len) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "\r\n[at_mqttpub] Invalid topic");
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
		RTK_LOGS(TAG, RTK_LOG_INFO, "\r\n[at_mqttpub] Invalid msg");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}
	resultNo = mqtt_string_copy(&mqttCb->pubData.msg, argv[msgIndex], len);
	if (MQTT_OK != resultNo) {
		goto end;
	}

	/* send publish. */
	if (NULL == mqttCb->pubData.msg || NULL == mqttCb->pubData.topic) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "\r\n[at_mqttpub] Not prepared");
		resultNo = MQTT_COMMON_ERROR;
		goto end;
	}
	if (1 != mqttCb->client.isconnected) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "\r\n[at_mqttpub] Invalid status (%d)", mqttCb->client.isconnected);
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
		RTK_LOGS(TAG, RTK_LOG_INFO, "\r\n[at_mqttpub] Publish ERROR");
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
		at_printf(ATCMD_ERROR_END_STR, resultNo);
	} else {
		at_printf(ATCMD_OK_END_STR);
	}
}

/* Get the Client Control block for tcpConnId.
If the needInit == 1, we should initialise this Control block no matter it was initialised already. */
static MQTT_RESULT_ENUM mqtt_get_handle_cb(u8 tcpConnId, MQTT_CONTROL_BLOCK **mqttCb, u8 needInit)
{
	MQTT_RESULT_ENUM resultNo = MQTT_OK;
	MQTT_AT_HANDLE *mqttAtHandle = &mqtt_at_handle;

	if (NULL == mqttCb || MQTT_MAX_CLIENT_NUM <= tcpConnId) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_get_handle_cb] Invalid parameters\r\n");
		resultNo = MQTT_COMMON_ERROR;
		goto end;
	}

	/* Release older one. */
	if (1 == needInit && NULL != mqttAtHandle->mqttCb[tcpConnId]) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_get_handle_cb] Cover the older information for %d\r\n", tcpConnId);
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
			RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_get_handle_cb] NULL param\r\n");
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

	if (pMqttCb->network.use_ssl) {
		rtos_mem_free(pMqttCb->network.rootCA);
		rtos_mem_free(pMqttCb->network.clientCA);
		rtos_mem_free(pMqttCb->network.private_key);
		pMqttCb->network.use_ssl = 0;
	}

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
		RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_init_handle_cb] mqttCb malloc failed\r\n");
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
		RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_init_handle_cb] Init client failed\r\n");
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
		RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_message_arrived] Nothing to do\r\n");
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
		RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_message_arrived] Memory Failure for topicdest\r\n");
		goto end;
	}
	strncpy(topicdest, topicsrc, topiclen);

	data_shift = strlen("%s:%d,%d,%s,") + strlen("+MQTTSUBRECV") - 2	+ sprintf(num_buf, "%d", tcpconnectid) - 2 + sprintf(num_buf, "%d",
				 id) - 2 + strlen(topicdest) - 2;
	len_out = data_shift + data->message->payloadlen + 1;
	total_data_len = strlen(",%d,%d,%d,%d") + sprintf(num_buf, "%d", data->message->payloadlen) - 2 + sprintf(num_buf, "%d", data->message->qos) - 2
					 + sprintf(num_buf, "%d", data->message->dup) - 2 + sprintf(num_buf, "%d", data->message->retained) - 2 + len_out;

	uns_buff = (char *)rtos_mem_zmalloc(total_data_len);
	if (uns_buff == NULL) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_message_arrived] Memory Failure for uns_buff\r\n");
		goto end;
	}

	DiagSnPrintf(uns_buff, len_out, "%s:%d,%d,%s,", "+MQTTSUBRECV", tcpconnectid, id, topicdest);
	memcpy((uns_buff + data_shift), (char *)data->message->payload, (int16_t)(data->message->payloadlen));
	DiagSnPrintf(uns_buff + strlen(uns_buff), total_data_len - strlen(uns_buff), ",%d,%d,%d,%d", data->message->payloadlen, data->message->qos, data->message->dup,
				 data->message->retained);

	at_printf_indicate("%s\r\n", uns_buff);

end:
	rtos_mem_free(topicdest);
	rtos_mem_free(uns_buff);
}

static MQTT_RESULT_ENUM mqtt_string_copy(char **dest, char *src, size_t sz)
{
	MQTT_RESULT_ENUM resultNo = MQTT_OK;

	if (NULL == src) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_string_copy] ARGS error\r\n");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	/* Delete the elder string. */
	rtos_mem_free(*dest);
	/* sz + 1, the 1 is used for '\0'. */
	*dest = (char *)rtos_mem_malloc(sizeof(char) * (sz + 1));
	if (NULL == *dest) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_string_copy] Memory failure\r\n");
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
		RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_clent_alive_fail] Invalid param\r\n");
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
			RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_clent_alive_fail] Connect failed\r\n");
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
	RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_clent_alive_fail] Set to MQTT_START\r\n");
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
		RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_clent_data_proc] Invalid params\r\n");
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
				at_printf_indicate("%sOK\r\n", "+MQTTUNREACH:");
			} else {
				/* Do not use at_printf here. */
				RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_clent_data_proc] Lost connection\r\n");
			}
		}
		/* Try re-connect per second, except when initial-connect. */
#if 0
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
					// RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_clent_data_proc] Reconnect failed\r\n");
					resultNo = MQTT_CONNECTION_ERROR;
					goto end;
				}
				mqttCb->networkConnect = 1;
				TimerInit(&mqttCb->client.cmd_timer);
				TimerCountdownMS(&mqttCb->client.cmd_timer, mqttCb->client.command_timeout_ms);
				res = MQTTConnect(&mqttCb->client, &mqttCb->connectData);
				if (0 != res) {
					// RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_clent_data_proc] Reconnect MQTT failed\r\n");
					resultNo = MQTT_CONNECTION_ERROR;
					goto end;
				}
				mqttCb->client.mqttstatus = MQTT_CONNECT;
				mqttCb->offline = 0;
				RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_clent_data_proc] Reconnect the network OK\r\n");
				at_printf_indicate("\r\n%sOK\r\n", "+MQTTREACH:");
			}
		}
#endif
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
		RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_clent_data_proc] Read packet %d\r\n", packet_type);
	}

	switch (mqttStatus) {
	/* MQTT_CONNECT status. */
	case MQTT_CONNECT:
		if (CONNACK == packet_type) {
			u8 connack_rc = 255, sessionPresent = 0;
			tmp = MQTTDeserialize_connack(&sessionPresent, &connack_rc, mqttCb->client.readbuf, mqttCb->client.readbuf_size);
			if (1 == tmp) {
				res = connack_rc;
				RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_clent_data_proc] MQTT connected OK\r\n");
			} else {
				RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_clent_data_proc] MQTT can not connect\r\n");
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
			RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_clent_data_proc] MQTT connected ERROR (%d)\r\n", mqttCb->client.isconnected);
			if (1 == mqttCb->client.isconnected) {
				res = MQTTDisconnect(&mqttCb->client);
				if (0 != res) {
					RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_clent_data_proc] Disconnect ERROR\r\n");
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
				RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_clent_data_proc] grantedQoS = %d\r\n", res);
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
			RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_clent_data_proc] Unsubscribe_end OK\r\n");
			mqttCb->client.mqttstatus = MQTT_CONNECT;
			*needAtOutpput = 1;
		} else if (TimerIsExpired(&mqttCb->client.cmd_timer)) {
			*needAtOutpput = 1;
			RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_clent_data_proc] MQTT subscribe timeout\r\n");
			resultNo = MQTT_WAITACK_TIMEOUT_ERROR;
			res = FAILURE;
		}
		if (FAILURE == res) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_clent_data_proc] MQTT subscribe ERROR\r\n");
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
					RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_clent_data_proc] grantedQoS = %d\r\n", res);
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
					RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_clent_data_proc] Deserialize_ack failed\r\n");
					res = FAILURE;
					resultNo = MQTT_PUBLISH_ERROR;
				}
				*needAtOutpput = 1;
			}
			break;

			case UNSUBACK: {
				u8 i = 0;
				RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_clent_data_proc] Unsubscribe OK\r\n");
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
					RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_clent_data_proc] Deserialize PUBLISH failed\r\n");
					goto end;
				}
				msg.qos = intQoS;
				deliverMessage(&mqttCb->client, &topicName, &msg);
				/* QOS0 has no ack. */
				if (QOS0 != msg.qos) {
					if (QOS1 == msg.qos) {
						len = MQTTSerialize_ack(mqttCb->client.buf, mqttCb->client.buf_size, PUBACK, 0, msg.id);
						RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_clent_data_proc] Send PUBACK\r\n");
					} else if (QOS2 == msg.qos) {
						len = MQTTSerialize_ack(mqttCb->client.buf, mqttCb->client.buf_size, PUBREC, 0, msg.id);
						RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_clent_data_proc] Send PUBREC\r\n");
					} else {
						RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_clent_data_proc] Invalid QoS %d\r\n", msg.qos);
						goto end;
					}
					/* Send failed. */
					if (0 >= len) {
						RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_clent_data_proc] Serialize_ack failed\r\n");
						goto end;
					}
					tmp = sendPacket(&mqttCb->client, len, &timer);
					if (FAILURE == tmp) {
						RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_clent_data_proc] send packet failed, set to start\r\n");
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
					RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_clent_data_proc] Deserialize PUBREC failed\r\n");
					resultNo = MQTT_PUBLISH_ERROR;
				} else {
					len = MQTTSerialize_ack(mqttCb->client.buf, mqttCb->client.buf_size, PUBREL, 0, mypacketid);
					if (0 >= len) {
						RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_clent_data_proc] Serialize PUBREL failed\r\n");
						resultNo = MQTT_PUBLISH_ERROR;
					} else {
						tmp = sendPacket(&mqttCb->client, len, &timer);
						if (FAILURE == tmp) {
							RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_clent_data_proc] send packet failed\r\n");
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
					RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_clent_data_proc] Deserialize PUBREL failed\r\n");
					res = FAILURE;
				} else {
					len = MQTTSerialize_ack(mqttCb->client.buf, mqttCb->client.buf_size, PUBCOMP, 0, mypacketid);
					if (0 >= len) {
						RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_clent_data_proc] Serialize PUBCOMP failed\r\n");
						res = FAILURE;
					} else {
						tmp = sendPacket(&mqttCb->client, len, &timer);
						if (FAILURE == tmp) {
							RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_clent_data_proc] sent packet ERROR\r\n");
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

static void at_mqttconn_help(void)
{
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "AT+MQTTCONN=<link_id>,<host>,<port>\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<link_id>:\t[0,3]\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<host>:\thostname\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<port>:\t[0,65535]\r\n");
}
/****************************************************************
Function            at_mqttconn
Brief                   MQTT connect. The command is used to connect a cloud for MQTT client.
AT+MQTTCONN=<link_id>,<host>,<port>
****************************************************************/
void at_mqttconn(void *arg)
{
	char *argv[MAX_ARGC] = {0};
	int argc = 0, res = 0;
	MQTT_RESULT_ENUM resultNo = MQTT_OK;
	MQTT_CONTROL_BLOCK *mqttCb = NULL;
	int link_id = MQTT_MAX_CLIENT_NUM;
	int port = MQTT_DEFAULT_PORT;
	char TaskName[6];
	u16 stacksize = 4096;

	/* Get the parameters of AT command. */
	if (!arg) {
		at_mqttconn_help();
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	/* parameters include link_id, host, port. */
	argc = parse_param_advance(arg, argv);
	if (argc < 4) {
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	/* link ID. */
	link_id = atoi(argv[1]);
	if (link_id < 0 || link_id >= MQTT_MAX_CLIENT_NUM) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTCONN] Invalid link_id\r\n");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	resultNo = mqtt_get_handle_cb(link_id, &mqttCb, 0);
	if (MQTT_OK != resultNo) {
		goto end;
	}

	/* hostName. */
	if (strlen(argv[2]) == 0 || strlen(argv[2]) > MQTT_MAX_HOSTNAME_LEN) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTCONN] Invalid hostName\r\n");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	resultNo = mqtt_string_copy(&mqttCb->host, argv[2], strlen(argv[2]));
	if (MQTT_OK != resultNo) {
		goto end;
	}

	/* Port. */
	port = atoi(argv[3]);
	if (port < 0 || port > 0xFFFF) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTCONN] Invalid port\r\n");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	/* Check whether the network is linked. */
	if (wifi_is_connected_to_ap() != RTW_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTCONN] The network is not ready\r\n");
		resultNo = MQTT_NETWORK_LINK_ERROR;
		goto end;
	}

	/* Update the mqttCb information. */
	mqttCb->port = port;
	mqttCb->network.use_ssl = mqttCb->useSsl;

	RTK_LOGS(TAG, RTK_LOG_INFO, "[+MQTTCONN] Open a new connection for %s\r\n", mqttCb->host);
	if (!mqttCb->networkConnect) {
		/* Try to connect the host. */
		res = NetworkConnect(&mqttCb->network, mqttCb->host, mqttCb->port);
		if (0 != res) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTCONN] Can not connect %s\r\n", mqttCb->host);
			mqttCb->client.mqttstatus = MQTT_START;
			resultNo = MQTT_AUTH_ERROR;
			goto end;
		}
		mqttCb->networkConnect = 1;
	}
	mqttCb->client.isconnected = 0;

	if (mqttCb->taskState == MQTT_TASK_NOT_CREATE) {
		/* Create the task. */
		mqttCb->taskState = MQTT_TASK_START;
		/* For each link ID, the task name should be unique. */
		TaskName[0] = 'M';
		TaskName[1] = 'Q';
		TaskName[2] = 'T';
		TaskName[3] = 'T';
		TaskName[4] = '0' + link_id;
		TaskName[5] = '\0';
		if (mqttCb->network.use_ssl) {
			stacksize = 6144;
		}
		if (SUCCESS != rtos_task_create(&mqttCb->taskHandle,
										TaskName,
										mqtt_main,
										(void *)mqttCb,
										stacksize,
										ATCMD_MQTT_TASK_PRIORITY)) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTCONN] Create task failed\r\n");
			mqttCb->taskState = MQTT_TASK_NOT_CREATE;
			resultNo = MQTT_THREAD_CREATE_ERROR;
			goto end;
		}
	}

	/* Check status. */
	if (mqttCb->client.isconnected) {
		RTK_LOGW(TAG, "[+MQTTCONN] Already connected\r\n");
		resultNo = MQTT_ALREADY_CONNECTED_ERROR;
		goto end;
	} else {
		/* Reset when reconnect. */
		mqttCb->client.ping_outstanding = 0;
		mqttCb->client.next_packetid = 1;
	}
	if (NULL == mqttCb->clientId) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTCONN] The clientid has not been set\r\n");
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
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTCONN] Connect ERROR\r\n");
		resultNo = MQTT_CONNECTION_ERROR;
		goto end;
	} else {
		/* AT output when receiving CONNACK. */
		RTK_LOGS(TAG, RTK_LOG_INFO, "[+MQTTCONN] Sent connection request, waiting for ACK\r\n");
		mqttCb->client.mqttstatus = MQTT_CONNECT;
		mqttCb->initialConnect = 1;
	}

end:
	if (MQTT_OK != resultNo) {
		at_printf(ATCMD_ERROR_END_STR, resultNo);
	} else {
		at_printf(ATCMD_OK_END_STR);
	}
}

static void at_mqttdisconn_help(void)
{
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "AT+MQTTDISCONN=<link_id>\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<link_id>: \t0~3. link ID.\r\n");
}

/****************************************************************
Function    at_mqttdisconn
Brief         MQTT disconnect. The command is used to disconnect a cloud for MQTT client and release the resources.
AT+MQTTDISCONN=<link_id>
****************************************************************/
void at_mqttdisconn(void *arg)
{
	char *argv[MAX_ARGC] = {0};
	int argc = 0, res = 0;
	MQTT_RESULT_ENUM resultNo = MQTT_OK;
	MQTT_CONTROL_BLOCK *mqttCb = NULL;
	int link_id = MQTT_MAX_CLIENT_NUM;

	/* Get the parameters of AT command. */
	if (!arg) {
		at_mqttdisconn_help();
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	argc = parse_param_advance(arg, argv);
	if (argc != 2) {
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	/* link ID. */
	link_id = atoi(argv[1]);
	if (link_id < 0 || link_id >= MQTT_MAX_CLIENT_NUM) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTDISCONN] Invalid link_id\r\n");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "[+MQTTDISCONN] Close %d\r\n", link_id);
	resultNo = mqtt_get_handle_cb((u8)link_id, &mqttCb, 0);
	if (MQTT_OK != resultNo) {
		resultNo = MQTT_ARGS_ERROR;
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTDISCONN] Failed when getting mqttCb\r\n");
		goto end;
	}

	/* Stop the task. */
	if (MQTT_TASK_START == mqttCb->taskState) {
		mqttCb->taskState = MQTT_TASK_STOP;
		/* Wait until the mqtt_main task stops. */
		while (MQTT_TASK_NOT_CREATE != mqttCb->taskState) {
			rtos_time_delay_ms(20);
		}
	}

	/* Disconnect client. */
	if (mqttCb->client.isconnected) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "[+MQTTDISCONN] Still connected\r\n");
		res = MQTTDisconnect(&mqttCb->client);
		if (0 != res) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTDISCONN] Can not disconnect\r\n");
			resultNo = MQTT_ALREADY_CONNECTED_ERROR;
			goto end;
		}
	}

	/* Disconnect host. */
	if (mqttCb->networkConnect) {
		if (NULL != mqttCb->network.disconnect) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "[+MQTTDISCONN] Disconnect from %s\r\n", mqttCb->host);
			mqttCb->network.disconnect(&mqttCb->network);
		}
		mqttCb->networkConnect = 0;
	}

end:
	/* Clean the Client Control block. */
	mqtt_del_handle_cb(&mqttCb);
	mqtt_set_handle_cb(link_id, NULL);

	if (MQTT_OK != resultNo) {
		at_printf(ATCMD_ERROR_END_STR, resultNo);
	} else {
		at_printf(ATCMD_OK_END_STR);
	}
}

static int mqttusercfg_set_ssl_certificate(char **dest, CERT_TYPE cert_type, int index)
{
	char *cert_buffer = NULL;
	int cert_size = 0;
	int ret = 0;

	cert_size = atcmd_get_ssl_certificate_size(cert_type, index);
	if (cert_size <= 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTUSERCFG] %d get size %d\r\n", cert_type, cert_size);
		return MQTT_ARGS_ERROR;
	}
	cert_buffer = (char *)rtos_mem_zmalloc(cert_size);
	if (cert_buffer == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTUSERCFG] cert_buffer malloc failed\r\n");
		return MQTT_ARGS_ERROR;
	}
	ret = atcmd_get_ssl_certificate(cert_buffer, cert_type, index);
	if (ret <= 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTUSERCFG] %d get failed\r\n", cert_type);
		rtos_mem_free(cert_buffer);
		return MQTT_ARGS_ERROR;
	}

	ret = mqtt_string_copy(dest, cert_buffer, cert_size);
	if (ret != MQTT_OK) {
		rtos_mem_free(cert_buffer);
		return ret;
	}
	rtos_mem_free(cert_buffer);
	return MQTT_OK;
}

static void at_mqttusercfg_help(void)
{
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "AT+MQTTUSERCFG=<link_id>,<conn_type>,<client_id>,<username>,<password>,<certificate_index>\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<link_id>:\t0~3. link ID.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<conn_type>:\t1~5. Connection type.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t\t1:\tMQTT over TCP.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t\t2:\tMQTT over TLS (no certificate verify).\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t\t3:\tMQTT over TLS (verify server certificate).\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t\t4:\tMQTT over TLS (provide client certificate).\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t\t5:\tMQTT over TLS (verify server certificate and provide client certificate).\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<client_id>:\tstring, MQTT client ID.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<username>:\tstring, the username to login to the MQTT broker.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<password>:\tstring, the password to login to the MQTT broker.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<certificate_index>:\tcertificate index.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "e.g.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\tAT+MQTTUSERCFG=0,1,publisher,rs,12345678,0\r\n");
}

/****************************************************************
Function            at_mqttusercfg
Brief                   MQTT user parameters configure. The command is used to configure mqtt user parameters.
AT+MQTTUSERCFG=<link_id>,<conn_type>,<client_id>,<username>,<password>,<certificate_index>
****************************************************************/
void at_mqttusercfg(void *arg)
{
	char *argv[MAX_ARGC] = {0};
	int argc = 0;
	MQTT_RESULT_ENUM resultNo = MQTT_OK;
	MQTT_CONTROL_BLOCK *mqttCb = NULL;
	int link_id = MQTT_MAX_CLIENT_NUM;
	int conn_type = 0;
	int cert_index = 0;

	/* Get the parameters of AT command. */
	if (!arg) {
		at_mqttusercfg_help();
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	argc = parse_param_advance(arg, argv);
	if (argc < 7) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTUSERCFG] Input wrong parameter\r\n");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	/* link_id. */
	link_id = atoi(argv[1]);
	if (link_id >= MQTT_MAX_CLIENT_NUM || link_id < 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTUSERCFG] Invalid link_id\r\n");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	if (NULL == mqtt_at_handle.mqttCb[link_id]) {
		/* Get the specific mqtt control block, and initialize. */
		resultNo = mqtt_get_handle_cb((u8)link_id, &mqttCb, 1);
		if (MQTT_OK != resultNo) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTUSERCFG] The link ID init failed\r\n");
			resultNo = MQTT_ARGS_ERROR;
			goto end;
		}
		mqttCb->tcpConnectId = (u8)link_id;
	} else {
		RTK_LOGS(TAG, RTK_LOG_INFO, "[+MQTTUSERCFG] link_id %d already exists\r\n", link_id);
		mqtt_get_handle_cb((u8)link_id, &mqttCb, 0);
		if (mqttCb->client.mqttstatus == MQTT_CONNECT) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTUSERCFG] The link ID is already connected\r\n");
			resultNo = MQTT_ALREADY_CONNECTED_ERROR;
			goto end;
		}
	}

	/* conn_type. */
	conn_type = atoi(argv[2]);
	if (conn_type <= MQTT_OVER_NONE || conn_type >= MQTT_OVER_MAX) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTUSERCFG] Invalid conn_type\r\n");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	/* client_id. */
	if (strlen(argv[3]) == 0 || strlen(argv[3]) > MQTT_MAX_CLIENT_ID_LEN) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTUSERCFG] Invalid client_id\r\n");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}
	resultNo = mqtt_string_copy(&mqttCb->clientId, argv[3], strlen(argv[3]));
	if (MQTT_OK != resultNo) {
		goto end;
	}

	/* username. */
	if (strlen(argv[4]) == 0 || strlen(argv[4]) > MQTT_MAX_USERNAME_LEN) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTUSERCFG] Invalid username\r\n");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}
	resultNo = mqtt_string_copy(&mqttCb->userName, argv[4], strlen(argv[4]));
	if (MQTT_OK != resultNo) {
		goto end;
	}

	/* password. */
	if (strlen(argv[5]) == 0 || strlen(argv[5]) > MQTT_MAX_PASSWORD_LEN) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTUSERCFG] Invalid password\r\n");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}
	resultNo = mqtt_string_copy(&mqttCb->password, argv[5], strlen(argv[5]));
	if (MQTT_OK != resultNo) {
		goto end;
	}

	/* certificate_index. */
	cert_index = atoi(argv[6]);
	if (conn_type >= MQTT_OVER_TLS_VERIFY_NONE) {
		if (cert_index < 0) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTUSERCFG] Invalid certificate_index\r\n");
			resultNo = MQTT_ARGS_ERROR;
			goto end;
		}
		if (conn_type == MQTT_OVER_TLS_VERIFY_SERVER || conn_type == MQTT_OVER_TLS_VERIFY_BOTH) {
			resultNo = mqttusercfg_set_ssl_certificate(&mqttCb->network.rootCA, CLIENT_CA, cert_index);
			if (MQTT_OK != resultNo) {
				goto end;
			}
		}

		if (conn_type == MQTT_OVER_TLS_VERIFY_CLIENT || conn_type == MQTT_OVER_TLS_VERIFY_BOTH) {
			resultNo = mqttusercfg_set_ssl_certificate(&mqttCb->network.clientCA, CLIENT_CERT, cert_index);
			if (MQTT_OK != resultNo) {
				goto end;
			}

			resultNo = mqttusercfg_set_ssl_certificate(&mqttCb->network.private_key, CLIENT_KEY, cert_index);
			if (MQTT_OK != resultNo) {
				goto end;
			}
		}
		mqttCb->useSsl = 1;
	}

end:
	if (MQTT_OK == resultNo) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		/* Clean the Client Control block. */
		if (mqttCb != NULL && resultNo != MQTT_ALREADY_CONNECTED_ERROR) {
			mqtt_del_handle_cb(&mqttCb);
			mqtt_set_handle_cb(link_id, NULL);
		}
		at_printf(ATCMD_ERROR_END_STR, resultNo);
	}
	return;
}

static void at_mqttconncfg_help(void)
{
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "AT+MQTTCONNCFG=<link_id>,<keepalive>,<timeout>,<disable_clean_session>,<lwt_topic>,<lwt_msg>,<lwt_qos>,<lwt_retain>\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<link_id>:\t0~3. link ID.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<keepalive>:\ttimeout of MQTT ping. Unit: ms.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<timeout>:\tsocket send/recv timeout. Unit: ms.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<disable_clean_session>:\tset MQTT clean session.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t\t0:\tenable clean session.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t\t1:\tdisable clean session.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<lwt_topic>:\tLWT (Last Will and Testament) message topic. Maximum length: 128 bytes.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<lwt_msg>:\tLWT message. Maximum length: 128 bytes.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<lwt_qos>:\tLWT QoS, which can be set to 0, 1, or 2. Default: 0.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<lwt_retain>:\tLWT retain, which can be set to 0 or 1. Default: 0.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "e.g.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\tAT+MQTTCONNCFG=0,10000,1000,0,topic/lwt,This is a lwt message,0,0\r\n");
}

/****************************************************************
Function            at_mqttconncfg
Brief                   MQTT connection parameters configure. The command is used to configure mqtt connection parameters.
AT+MQTTCONNCFG=<link_id>,<keepalive>,<timeout>,<disable_clean_session>,<lwt_topic>,<lwt_msg>,<lwt_qos>,<lwt_retain>
****************************************************************/
void at_mqttconncfg(void *arg)
{
	char *argv[MAX_ARGC] = {0};
	int argc = 0;
	MQTT_RESULT_ENUM resultNo = MQTT_OK;
	MQTT_CONTROL_BLOCK *mqttCb = NULL;
	int link_id = MQTT_MAX_CLIENT_NUM;
	int keepalive = 0, timeout = 0, qos = 0, retain = 0, disable_clean_session = 0;

	/* Get the parameters of AT command. */
	if (!arg) {
		at_mqttconncfg_help();
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	argc = parse_param_advance(arg, argv);
	if (argc < 5) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "\r\n[+MQTTCONNCFG] Input wrong parameter");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	/* link_id. */
	link_id = atoi(argv[1]);
	if (link_id >= MQTT_MAX_CLIENT_NUM || link_id < 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "\r\n[+MQTTCONNCFG] Invalid link_id");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	resultNo = mqtt_get_handle_cb((u8)link_id, &mqttCb, 0);
	if (MQTT_OK != resultNo) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "\r\n[+MQTTCONNCFG] The link ID does not exist");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	/* keepalive. */
	keepalive = atoi(argv[2]);
	if (keepalive <= 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "\r\n[+MQTTCONNCFG] Invalid keepalive");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	/* timeout. */
	timeout = atoi(argv[3]);
	if (timeout < 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "\r\n[+MQTTCONNCFG] Invalid timeout");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	/* disable_clean_session. */
	disable_clean_session = atoi(argv[4]);
	if (disable_clean_session != 0 && disable_clean_session != 1) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "\r\n[+MQTTCONNCFG] Invalid disable_clean_session");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	mqttCb->connectData.keepAliveInterval = (u16)keepalive;
	mqttCb->client.command_timeout_ms = (u32)timeout;
	mqttCb->connectData.cleansession = (u8)disable_clean_session;

	/* Last Will and Testament. */
	if (argc == 9) {
		if (strlen(argv[5]) == 0) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "\r\n[+MQTTCONNCFG] Invalid lwt_topic");
			resultNo = MQTT_ARGS_ERROR;
			goto end;
		}
		resultNo = mqtt_string_copy(&mqttCb->connectData.will.topicName.cstring, argv[5], strlen(argv[5]));
		if (MQTT_OK != resultNo) {
			goto end;
		}
		if (strlen(argv[6]) == 0) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "\r\n[+MQTTCONNCFG] Invalid lwt_msg");
			resultNo = MQTT_ARGS_ERROR;
			goto end;
		}
		resultNo = mqtt_string_copy(&mqttCb->connectData.will.message.cstring, argv[6], strlen(argv[6]));
		if (MQTT_OK != resultNo) {
			goto end;
		}

		qos = atoi(argv[7]);
		if (qos < 0 || qos > 2) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "\r\n[+MQTTCONNCFG] Invalid lwt_qos");
			resultNo = MQTT_ARGS_ERROR;
			goto end;
		}

		retain = atoi(argv[8]);
		if (retain != 0 && retain != 1) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "\r\n[+MQTTCONNCFG] Invalid lwt_retain");
			resultNo = MQTT_ARGS_ERROR;
			goto end;
		}
		mqttCb->connectData.willFlag = 1;
		mqttCb->connectData.will.qos = (char)qos;
		mqttCb->connectData.will.retained = (u8)retain;
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "\r\n[+MQTTCONNCFG] No LWT value");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

end:
	if (MQTT_OK == resultNo) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, resultNo);
	}
	return;
}
/****************************************************************
Function            at_mqttreset
Brief                   MQTT reset, If you forget the procedure, or how many connections you have created, you can reset all.
AT+MQTTRESET
****************************************************************/
void at_mqttreset(void *arg)
{
	int i = 0, res = 0;
	MQTT_CONTROL_BLOCK *mqttCb = NULL;
	MQTT_AT_HANDLE *mqttAtHandle = &mqtt_at_handle;

	UNUSED(arg);

	RTK_LOGS(TAG, RTK_LOG_INFO, "\r\n[+MQTTRESET] reset all mqtt connections");
	/* No need any other parameters. */
	for (; MQTT_MAX_CLIENT_NUM > i; i++) {
		mqttCb = mqttAtHandle->mqttCb[i];
		if (NULL != mqttCb) {
			/* Disconnect client. */
			if (mqttCb->client.isconnected) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "\r\n[+MQTTRESET] Still connected");
				res = MQTTDisconnect(&mqttCb->client);
				if (0 != res) {
					RTK_LOGS(TAG, RTK_LOG_INFO, "\r\n[+MQTTRESET] Can not disconnect");
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
					RTK_LOGS(TAG, RTK_LOG_INFO, "\r\n[+MQTTRESET] Disconnect from %s", mqttCb->host);
					mqttCb->network.disconnect(&mqttCb->network);
				}
				mqttCb->networkConnect = 0;
			}

			/* Clean the Client Control block. */
			mqtt_del_handle_cb(&mqttCb);
			mqtt_set_handle_cb(i, NULL);
		}
	}

	/* This command only output "OK". */
	at_printf(ATCMD_OK_END_STR);
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

log_item_t at_mqtt_items[ ] = {
	{"+MQTTUSERCFG", at_mqttusercfg, {NULL, NULL}},   /* MQTT user configure. */
	{"+MQTTCONNCFG", at_mqttconncfg, {NULL, NULL}},   /* MQTT connection configure. */
	{"+MQTTCONN", at_mqttconn, {NULL, NULL}},  /* MQTT connect. */
	{"+MQTTDISCONN", at_mqttdisconn, {NULL, NULL}},  /* MQTT disconnect. */
	{"+MQTTSUB", at_mqttsub, {NULL, NULL}},  /* MQTT subscribe. */
	{"+MQTTUNSUB", at_mqttunsub, {NULL, NULL}},  /* MQTT unsubscribe. */
	{"+MQTTPUB", at_mqttpub, {NULL, NULL}},  /* MQTT publish. */
	{"+MQTTRESET", at_mqttreset, {NULL, NULL}}   /* MQTT reset. */
};

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

#endif /* CONFIG_ATCMD_MQTT */
#endif /* CONFIG_MP_INCLUDED */
