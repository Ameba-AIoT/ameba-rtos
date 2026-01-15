/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "platform_autoconf.h"

#ifdef CONFIG_LWIP_LAYER
#if defined(CONFIG_ATCMD_MQTT) && (CONFIG_ATCMD_MQTT == 1)
#include "atcmd_service.h"
#include "atcmd_mqtt.h"

static const char *TAG = "AT-MQTT";

static void mqtt_message_arrived(MessageData *data, void *param);
static MQTT_RESULT_ENUM mqtt_string_copy(char **dest, char *src, size_t sz);

MQTT_CONTROL_BLOCK g_mqttCb[MQTT_MAX_CLIENT_NUM];
MQTTPacket_connectData mqtt_default_conn_data = MQTTPacket_connectData_initializer;

/* Process the received data.
 The res has different meaning with resultNo.
 The resultNo is used for AT command output. The res is used for set mqtt status. */
static void mqtt_clent_data_proc(MQTT_CONTROL_BLOCK *mqttCb, fd_set *read_fds)
{
	int packet_type = 0;
	int rc = 0, mqttStatus = 0, mqttRxEvent = 0;
	Timer timer;
	MQTT_RESULT_ENUM resultNo = MQTT_OK;

	if (NULL == mqttCb || NULL == read_fds) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "[%s] Invalid params\n", __func__);
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	mqttStatus = mqttCb->client.mqttstatus;
	mqttRxEvent = (0 <= mqttCb->client.ipstack->my_socket) ? FD_ISSET(mqttCb->client.ipstack->my_socket, read_fds) : 0;

	if (mqttRxEvent) {
		mqttCb->client.ipstack->m2m_rxevent = 0;
		TimerInit(&timer);
		TimerCountdownMS(&timer, 1000);
		packet_type = readPacket(&mqttCb->client, &timer);
		RTK_LOGS(TAG, RTK_LOG_INFO, "[%s] linkid:%d, Read packet %d\n", __func__, mqttCb->linkId, packet_type);
		if (packet_type < CONNECT || packet_type > DISCONNECT) {
			MQTTSetStatus(&mqttCb->client, MQTT_START);
			resultNo = MQTT_ARGS_ERROR;
			goto end;
		}
	}

	switch (mqttStatus) {
	/* MQTT_CONNECT status. */
	case MQTT_CONNECT:
		if (packet_type == CONNACK) {
			u8 connack_rc = 255, sessionPresent = 0;
			if (MQTTDeserialize_connack(&sessionPresent, &connack_rc, mqttCb->client.readbuf, mqttCb->client.readbuf_size) == 1) {
				rc = connack_rc;
				at_printf_indicate("[MQTT][EVENT]:linkid:%d, connected\r\n", mqttCb->linkId);
				RTK_LOGS(TAG, RTK_LOG_INFO, "[%s] linkid:%d, connected\n", __func__, mqttCb->linkId);
				mqttCb->client.isconnected = 1;
				MQTTSetStatus(&mqttCb->client, MQTT_RUNNING);
			} else {
				RTK_LOGS(TAG, RTK_LOG_INFO, "[%s] linkid:%d, deserialize CONNACK failed\n", __func__, mqttCb->linkId);
				rc = FAILURE;
			}
		} else if (TimerIsExpired(&mqttCb->client.cmd_timer)) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "[%s] linkid:%d, not received CONNACK\n", __func__, mqttCb->linkId);
			rc = FAILURE;
		}

		if (rc == FAILURE) {
			resultNo = MQTT_CONNECTION_ERROR;
			MQTTSetStatus(&mqttCb->client, MQTT_START);
		}
		break;

	/* MQTT_SUBTOPIC status. */
	case MQTT_SUBTOPIC:
		if (packet_type == SUBACK) {
			int count = 0, grantedQoS = -1;
			unsigned short mypacketid = 0;
			if (MQTTDeserialize_suback(&mypacketid, 1, &count, &grantedQoS, mqttCb->client.readbuf, mqttCb->client.readbuf_size) == 1) {
				/* It may be 0, 1, 2 or 0x80. */
				rc = grantedQoS;
				RTK_LOGS(TAG, RTK_LOG_INFO, "[%s] linkid:%d, grantedQoS = %d\n", __func__, mqttCb->linkId, rc);
			}
			if (rc != 0x80) {
				int i = 0;
				for (i = 0; i < MAX_MESSAGE_HANDLERS; i++) {
					if (NULL != mqttCb->subData[i].topic && mqttCb->client.messageHandlers[i].topicFilter != mqttCb->subData[i].topic) {
						mqttCb->client.messageHandlers[i].topicFilter = mqttCb->subData[i].topic;
						mqttCb->client.messageHandlers[i].fp = mqtt_message_arrived;
					}
				}
				at_printf_indicate("[MQTT][EVENT]:linkid:%d, subscribed\r\n", mqttCb->linkId);
				RTK_LOGS(TAG, RTK_LOG_INFO, "[%s] linkid:%d, subscribed\n", __func__, mqttCb->linkId);
				MQTTSetStatus(&mqttCb->client, MQTT_RUNNING);
			}
		} else if (TimerIsExpired(&mqttCb->client.cmd_timer)) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "[%s] linkid:%d, subscribe timeout\n", __func__, mqttCb->linkId);
			resultNo = MQTT_WAITACK_TIMEOUT_ERROR;
			MQTTSetStatus(&mqttCb->client, MQTT_START);
		}
		break;

	/* MQTT_RUNNING status. */
	case MQTT_RUNNING:
		if (packet_type >= CONNECT) {
			int len = 0;
			TimerInit(&timer);
			TimerCountdownMS(&timer, 10000);
			switch (packet_type) {
			case SUBACK: {
				int count = 0, grantedQoS = -1;
				unsigned short mypacketid = 0;
				if (MQTTDeserialize_suback(&mypacketid, 1, &count, &grantedQoS, mqttCb->client.readbuf, mqttCb->client.readbuf_size) == 1) {
					/* It may be 0, 1, 2 or 0x80. */
					rc = grantedQoS;
					RTK_LOGS(TAG, RTK_LOG_INFO, "[%s] linkid:%d, grantedQoS = %d\n", __func__, mqttCb->linkId, rc);
				}
				if (rc != 0x80) {
					int i = 0;
					for (i = 0; MAX_MESSAGE_HANDLERS > i; i++) {
						if (NULL != mqttCb->subData[i].topic && mqttCb->client.messageHandlers[i].topicFilter != mqttCb->subData[i].topic) {
							mqttCb->client.messageHandlers[i].topicFilter = mqttCb->subData[i].topic;
							mqttCb->client.messageHandlers[i].fp = mqtt_message_arrived;
						}
					}
					at_printf_indicate("[MQTT][EVENT]:linkid:%d, subscribed\r\n", mqttCb->linkId);
					RTK_LOGS(TAG, RTK_LOG_INFO, "[%s] linkid:%d, subscribed\n", __func__, mqttCb->linkId);
				}
			}
			break;

			case PUBACK: {
				u16 mypacketid = 0;
				u8 dup = 0, type = 0;
				if (MQTTDeserialize_ack(&type, &dup, &mypacketid, mqttCb->client.readbuf, mqttCb->client.readbuf_size) == 1) {
					at_printf_indicate("[MQTT][EVENT]:linkid:%d, published\r\n", mqttCb->linkId);
					RTK_LOGS(TAG, RTK_LOG_INFO, "[%s] linkid:%d, published\n", __func__, mqttCb->linkId);
				} else {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "[%s] linkid:%d, Deserialize_ack failed\n", __func__, mqttCb->linkId);
					resultNo = MQTT_PUBLISH_ERROR;
				}
			}
			break;

			case UNSUBACK:
				at_printf_indicate("[MQTT][EVENT]:linkid:%d, unsubscribed\r\n", mqttCb->linkId);
				RTK_LOGS(TAG, RTK_LOG_INFO, "[%s] linkid:%d, unsubscribed\n", __func__, mqttCb->linkId);
				break;

			case PUBLISH: {
				MQTTString topicName;
				MQTTMessage msg;
				int intQoS = 0;
				rc = MQTTDeserialize_publish(&msg.dup, &intQoS, &msg.retained, &msg.id, &topicName,
											 (unsigned char **)&msg.payload, (int *)&msg.payloadlen, mqttCb->client.readbuf, mqttCb->client.readbuf_size);
				if (rc != 1) {
					RTK_LOGS(TAG, RTK_LOG_INFO, "[%s] linkid:%d, Deserialize PUBLISH failed\n", __func__, mqttCb->linkId);
					resultNo = MQTT_PUBLISH_ERROR;
					goto end;
				}
				msg.qos = intQoS;
				deliverMessage(&mqttCb->client, &topicName, &msg);
				/* QOS0 has no ack. */
				if (msg.qos != QOS0) {
					if (msg.qos == QOS1) {
						len = MQTTSerialize_ack(mqttCb->client.buf, mqttCb->client.buf_size, PUBACK, 0, msg.id);
						RTK_LOGS(TAG, RTK_LOG_INFO, "[%s] linkid:%d, Send PUBACK\n", __func__, mqttCb->linkId);
					} else if (msg.qos == QOS2) {
						len = MQTTSerialize_ack(mqttCb->client.buf, mqttCb->client.buf_size, PUBREC, 0, msg.id);
						RTK_LOGS(TAG, RTK_LOG_INFO, "[%s] linkid:%d, Send PUBREC\n", __func__, mqttCb->linkId);
					} else {
						RTK_LOGS(TAG, RTK_LOG_INFO, "[%s] linkid:%d, Invalid QoS %d\n", __func__, mqttCb->linkId, msg.qos);
						resultNo = MQTT_PUBLISH_ERROR;
						goto end;
					}
					/* Send failed. */
					if (len <= 0) {
						RTK_LOGS(TAG, RTK_LOG_INFO, "[%s] linkid:%d, Serialize_ack failed\n", __func__, mqttCb->linkId);
						goto end;
					} else {
						rc = sendPacket(&mqttCb->client, len, &timer);
						if (rc == FAILURE) {
							RTK_LOGS(TAG, RTK_LOG_INFO, "[%s] linkid:%d, send packet failed, set to start\n", __func__, mqttCb->linkId);
							resultNo = MQTT_PUBLISH_ERROR;
							MQTTSetStatus(&mqttCb->client, MQTT_START);
						}
					}
				}
			}
			break;

			case PUBREC: {
				u16 mypacketid = 0;
				u8 dup = 0, type = 0;
				if (MQTTDeserialize_ack(&type, &dup, &mypacketid, mqttCb->client.readbuf, mqttCb->client.readbuf_size) != 1) {
					RTK_LOGS(TAG, RTK_LOG_INFO, "[%s] linkid:%d, Deserialize PUBREC failed\n", __func__, mqttCb->linkId);
					resultNo = MQTT_PUBLISH_ERROR;
				} else {
					len = MQTTSerialize_ack(mqttCb->client.buf, mqttCb->client.buf_size, PUBREL, 0, mypacketid);
					if (len <= 0) {
						RTK_LOGS(TAG, RTK_LOG_INFO, "[%s] linkid:%d, Serialize PUBREL failed\n", __func__, mqttCb->linkId);
						resultNo = MQTT_PUBLISH_ERROR;
					} else {
						rc = sendPacket(&mqttCb->client, len, &timer);
						if (rc == FAILURE) {
							RTK_LOGS(TAG, RTK_LOG_INFO, "[%s] linkid:%d, send packet failed\n", __func__, mqttCb->linkId);
							resultNo = MQTT_PUBLISH_ERROR;
							MQTTSetStatus(&mqttCb->client, MQTT_START);
						}
					}
				}
			}
			break;

			case PUBREL: {
				u16 mypacketid = 0;
				u8 dup = 0, type = 0;
				if (MQTTDeserialize_ack(&type, &dup, &mypacketid, mqttCb->client.readbuf, mqttCb->client.readbuf_size) != 1) {
					RTK_LOGS(TAG, RTK_LOG_INFO, "[%s] linkid:%d, Deserialize PUBREL failed\n", __func__, mqttCb->linkId);
					resultNo = MQTT_PUBLISH_ERROR;
				} else {
					len = MQTTSerialize_ack(mqttCb->client.buf, mqttCb->client.buf_size, PUBCOMP, 0, mypacketid);
					if (len <= 0) {
						RTK_LOGS(TAG, RTK_LOG_INFO, "[%s] linkid:%d, Serialize PUBCOMP failed\n", __func__, mqttCb->linkId);
						resultNo = MQTT_PUBLISH_ERROR;
					} else {
						rc = sendPacket(&mqttCb->client, len, &timer);
						if (rc == FAILURE) {
							RTK_LOGS(TAG, RTK_LOG_INFO, "[%s] linkid:%d, sent packet ERROR\n", __func__, mqttCb->linkId);
							resultNo = MQTT_PUBLISH_ERROR;
							MQTTSetStatus(&mqttCb->client, MQTT_START);
						}
					}
				}
			}
			break;
			case PUBCOMP:
				at_printf_indicate("[MQTT][EVENT]:linkid:%d, published\r\n", mqttCb->linkId);
				RTK_LOGS(TAG, RTK_LOG_INFO, "[%s] linkid:%d, published\n", __func__, mqttCb->linkId);
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
	if (resultNo != MQTT_OK) {
		at_printf_indicate("[MQTT][EVENT]:linkid:%d, ERROR:%d\r\n", mqttCb->linkId, resultNo);
	}
}

/****************************************************************
Function            mqtt_main
Brief                   The main function of mqtt AT task.
****************************************************************/
void mqtt_main(void *param)
{
	MQTT_CONTROL_BLOCK *mqttCb = (MQTT_CONTROL_BLOCK *)param;
	fd_set read_fds;
	fd_set except_fds;
	struct timeval timeout;

	if (NULL == mqttCb) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_main] Input invalid param\r\n");
		goto end;
	}

	while (1) {
		if (mqttCb->taskState != MQTT_TASK_START) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_main] linkid: %d, should stop now\r\n", mqttCb->linkId);
			break;
		}

		FD_ZERO(&read_fds);
		FD_ZERO(&except_fds);
		timeout.tv_sec = MQTT_SELECT_TIMEOUT;
		timeout.tv_usec = 0;

		if (mqttCb->network.my_socket >= 0) {
			FD_SET(mqttCb->network.my_socket, &read_fds);
			FD_SET(mqttCb->network.my_socket, &except_fds);
			FreeRTOS_Select(mqttCb->network.my_socket + 1, &read_fds, NULL, &except_fds, &timeout);
			/* The my_socket may be close */
			if (FD_ISSET(mqttCb->network.my_socket, &except_fds)) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_main] linkid: %d, except_fds is set\r\n", mqttCb->linkId);
				MQTTSetStatus(&mqttCb->client, MQTT_START);
			}
		} else {
			rtos_time_delay_ms(MQTT_SELECT_TIMEOUT * 1000);
		}
		/* Process the received data. */
		mqtt_clent_data_proc(mqttCb, &read_fds);
	}

end:
	RTK_LOGS(TAG, RTK_LOG_INFO, "[mqtt_main] linkid: %d, stop mqtt task\r\n", mqttCb->linkId);
	mqttCb->taskState = MQTT_TASK_NOT_CREATE;
	mqttCb->taskHandle = NULL;
	rtos_task_delete(mqttCb->taskHandle);
}

/* Corresponding to mqtt_init_client_buf( ). */
static void mqtt_del_client_buf(MQTTClient *client)
{
	if (client == NULL) {
		return;
	}
	rtos_mem_free(client->buf);
	rtos_mem_free(client->readbuf);
	client->buf = NULL;
	client->buf_size = 0;
	client->readbuf = NULL;
	client->readbuf_size = 0;
}

/* Similar with MQTTClientInit( ), but the buf and readbuf can be malloced at first. */
static MQTT_RESULT_ENUM mqtt_init_client_buf(MQTTClient *client, Network *network, u32 timeout)
{
	MQTT_RESULT_ENUM resultNo = MQTT_OK;
	u8 *readbuf = NULL;
	u8 *sendbuf = NULL;

	/*  Create default buf, will be realloced when get larger msg && recover after being processed. */
	readbuf = (u8 *)rtos_mem_malloc(MQTT_DEFAULT_BUF_SIZE);
	if (readbuf == NULL) {
		resultNo = MQTT_MALLOC_FAILED;
		goto end;
	}
	sendbuf = (u8 *)rtos_mem_malloc(MQTT_DEFAULT_BUF_SIZE);
	if (sendbuf == NULL) {
		resultNo = MQTT_MALLOC_FAILED;
		goto end;
	}

	MQTTClientInit(client, network, timeout, sendbuf, MQTT_DEFAULT_BUF_SIZE, readbuf, MQTT_DEFAULT_BUF_SIZE);
end:
	if (resultNo != MQTT_OK) {
		rtos_mem_free(readbuf);
		rtos_mem_free(sendbuf);
	}
	return resultNo;
}

/* Free the malloced memories. */
static void mqtt_release_resource(MQTT_CONTROL_BLOCK *pMqttCb)
{
	if (NULL == pMqttCb) {
		return;
	}

	/* Delete memory malloced. */
	for (int i = 0; i < MAX_MESSAGE_HANDLERS; i++) {
		rtos_mem_free(pMqttCb->subData[i].topic);
		pMqttCb->subData[i].topic = NULL;
	}
	/* Connection information and open information. */
	rtos_mem_free(pMqttCb->clientId);
	rtos_mem_free(pMqttCb->userName);
	rtos_mem_free(pMqttCb->password);
	rtos_mem_free(pMqttCb->host);
	pMqttCb->clientId = NULL;
	pMqttCb->userName = NULL;
	pMqttCb->password = NULL;
	pMqttCb->host = NULL;

	if (pMqttCb->network.use_ssl) {
		rtos_mem_free(pMqttCb->network.rootCA);
		rtos_mem_free(pMqttCb->network.clientCA);
		rtos_mem_free(pMqttCb->network.private_key);
		pMqttCb->network.rootCA = NULL;
		pMqttCb->network.clientCA = NULL;
		pMqttCb->network.private_key = NULL;
		pMqttCb->network.use_ssl = 0;
	}

	if (pMqttCb->connectData.willFlag) {
		rtos_mem_free(pMqttCb->connectData.will.message.cstring);
		pMqttCb->connectData.will.message.cstring = NULL;
		rtos_mem_free(pMqttCb->connectData.will.topicName.cstring);
		pMqttCb->connectData.will.topicName.cstring = NULL;
		pMqttCb->connectData.willFlag = 0;
	}

	if (pMqttCb->initailClient) {
		mqtt_del_client_buf(&pMqttCb->client);
		pMqttCb->initailClient = 0;
	}
}

static void mqtt_message_arrived(MessageData *data, void *param)
{
	MQTT_CONTROL_BLOCK *mqttCb = (MQTT_CONTROL_BLOCK *)param;
	char *topicsrc = NULL, *topicdest = NULL;
	int topiclen = 0;

	if (mqttCb == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[%s] mqttCb is null\r\n", __func__);
		return;
	}

	if (data->topicName->cstring != NULL) {
		topicsrc = data->topicName->cstring;
		topiclen = strlen(topicsrc);
	} else {
		topicsrc = data->topicName->lenstring.data;
		topiclen = data->topicName->lenstring.len;
	}

	topicdest = (char *)rtos_mem_zmalloc(topiclen + 1);
	if (topicdest == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[%s] malloc failed\n", __func__);
		return;
	}
	strncpy(topicdest, topicsrc, topiclen);

	at_printf_lock();
	at_printf_indicate("[MQTT][DATA][%d][%s][%d][%d]:", mqttCb->linkId, topicdest, data->message->id, data->message->payloadlen);
	at_printf_data((char *)data->message->payload, (u32)data->message->payloadlen);
	at_printf("\r\n");
	at_printf_unlock();

	rtos_mem_free(topicdest);
}

static MQTT_RESULT_ENUM mqtt_string_copy(char **dest, char *src, size_t sz)
{
	if (src == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[mqtt_string_copy] ARGS error\r\n");
		return MQTT_ARGS_ERROR;
	}

	/* Delete the elder string. */
	if (*dest) {
		rtos_mem_free(*dest);
	}
	/* sz + 1, the 1 is used for '\0'. */
	*dest = (char *)rtos_mem_malloc(sz + 1);
	if (*dest == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[mqtt_string_copy] malloc failed\r\n");
		return MQTT_MALLOC_FAILED;
	}
	strcpy(*dest, src);
	(*dest)[sz] = '\0';

	return MQTT_OK;
}

static int mqtt_set_ssl_certificate(char **dest, CERT_TYPE cert_type, int index)
{
	char *cert_buffer = NULL;
	int cert_size = 0;
	int ret = 0;

	cert_size = atcmd_get_ssl_certificate_size(cert_type, index);
	if (cert_size <= 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "%d get size %d\r\n", cert_type, cert_size);
		return MQTT_CERTIFICATE_READ_ERROR;
	}
	cert_buffer = (char *)rtos_mem_zmalloc(cert_size);
	if (cert_buffer == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "cert_buffer malloc failed\r\n");
		return MQTT_MALLOC_FAILED;
	}
	ret = atcmd_get_ssl_certificate(cert_buffer, cert_type, index);
	if (ret <= 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "%d get failed\r\n", cert_type);
		rtos_mem_free(cert_buffer);
		return MQTT_CERTIFICATE_READ_ERROR;
	}

	*dest = cert_buffer;
	return MQTT_OK;
}

static void at_mqttsub_help(void)
{
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "AT+MQTTSUB=<link_id>,<topic>,<qos>\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<link_id>: \t0~3. link ID.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<topic>: \tthe topic that is subscribed to.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<qos>: \tthe QoS that is subscribed to, which can be set to 0, 1, or 2. Default 0.\r\n");
}

/****************************************************************
Function            at_mqttsub
Brief                   MQTT subscribe. The command is used to subscribe mqtt issue.
AT+MQTTSUB=<link_id>,<topic>,<qos>
****************************************************************/
void at_mqttsub(void *arg)
{
	char *argv[MAX_ARGC] = {0};
	int argc = 0, res = 0;
	MQTT_RESULT_ENUM resultNo = MQTT_OK;
	MQTT_CONTROL_BLOCK *mqttCb = NULL;
	int link_id = MQTT_MAX_CLIENT_NUM;
	int qos = 0;
	int topic_index = 0;
	u8 added = 0;

	/* Get the parameters of AT command. */
	if (!arg) {
		at_mqttsub_help();
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	argc = parse_param_advance(arg, argv);
	if (argc < 4) {
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	/* link_id. */
	link_id = atoi(argv[1]);
	if (link_id >= MQTT_MAX_CLIENT_NUM || link_id < 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTSUB] Invalid link_id\r\n");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	mqttCb = &g_mqttCb[link_id];
	if (!mqttCb->client.isconnected || !mqttCb->networkConnect) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTSUB] The link ID is not connected\r\n");
		resultNo = MQTT_NOT_CONNECTED_ERROR;
		goto end;
	}

	/* topic. */
	if (0 == strlen(argv[2]) || MQTT_MAX_TOPIC_LEN < strlen(argv[2])) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTSUB] Invalid topic\r\n");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	/* QoS. This is optional, if not including here, it should be QOS2. */
	qos = atoi(argv[3]);
	if (qos < QOS0 || qos > QOS2) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTSUB] Invalid qos\r\n");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	for (topic_index = 0; MAX_MESSAGE_HANDLERS > topic_index; topic_index++) {
		if (mqttCb->subData[topic_index].topic == NULL) {
			added = 1;
			break;
		} else if (strcmp(mqttCb->subData[topic_index].topic, argv[2]) == 0 &&
				   mqttCb->client.messageHandlers[topic_index].topicFilter != NULL) {
			resultNo = MQTT_ALREADY_SUBSCRIBED;
			goto end;
		}
	}
	if (topic_index == MAX_MESSAGE_HANDLERS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTSUB] Topic index overflow, already subcribed %d topics\r\n", MAX_MESSAGE_HANDLERS);
		resultNo = MQTT_SUBSCRIBE_ERROR;
		goto end;
	}

	resultNo = mqtt_string_copy(&mqttCb->subData[topic_index].topic, argv[2], strlen(argv[2]));
	if (MQTT_OK != resultNo) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTSUB] mqtt_string_copy failed\r\n");
		goto end;
	}

	mqttCb->subData[topic_index].qos = qos;

	RTK_LOGS(TAG, RTK_LOG_INFO, "[+MQTTSUB] Subscribe topic %s\r\n", mqttCb->subData[topic_index].topic);

	TimerInit(&mqttCb->client.cmd_timer);
	TimerCountdownMS(&mqttCb->client.cmd_timer, mqttCb->client.command_timeout_ms);
	res = MQTTSubscribe(&mqttCb->client, mqttCb->subData[topic_index].topic, qos, mqtt_message_arrived);
	if (0 != res) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTSUB] Subscribe ERROR\r\n");
		resultNo = MQTT_SUBSCRIBE_ERROR;
		goto end;
	}
	MQTTSetStatus(&mqttCb->client, MQTT_SUBTOPIC);

end:
	if (MQTT_OK != resultNo) {
		if (resultNo == MQTT_ALREADY_SUBSCRIBED) {
			at_printf(ATCMD_OK_END_STR);
			at_printf("\r\nALREADY SUBCRIBE\r\n");
		} else {
			if (added && NULL != mqttCb->subData[topic_index].topic) {
				rtos_mem_free(mqttCb->subData[topic_index].topic);
				mqttCb->subData[topic_index].topic = NULL;
				mqttCb->subData[topic_index].qos = 0;
			}
			at_printf(ATCMD_ERROR_END_STR, resultNo);
		}
	} else {
		at_printf(ATCMD_OK_END_STR);
	}
}

static void at_mqttunsub_help(void)
{
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "AT+MQTTUNSUB=<link_id>,<topic>\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<link_id>: \t0~3. link ID.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<topic>: \tthe topic that is subscribed.\r\n");
}

/****************************************************************
Function            at_mqttunsub
Brief                   MQTT unsubscribe. The command is used to unsubscribe mqtt issue.
AT+MQTTUNSUB=<link_id>,<topic>
****************************************************************/
void at_mqttunsub(void *arg)
{
	char *argv[MAX_ARGC] = {0};
	int argc = 0, res = 0;
	MQTT_RESULT_ENUM resultNo = MQTT_OK;
	MQTT_CONTROL_BLOCK *mqttCb = NULL;
	int link_id = MQTT_MAX_CLIENT_NUM;
	int topic_index = 0;
	u8 matched = 0;

	/* Get the parameters of AT command. */
	if (!arg) {
		at_mqttunsub_help();
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	argc = parse_param_advance(arg, argv);
	if (argc < 3) {
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	/* link_id. */
	link_id = atoi(argv[1]);
	if (link_id >= MQTT_MAX_CLIENT_NUM || link_id < 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTUNSUB] Invalid link_id\r\n");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	mqttCb = &g_mqttCb[link_id];
	if (!mqttCb->client.isconnected || !mqttCb->networkConnect) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTUNSUB] The link ID is not connected\r\n");
		resultNo = MQTT_NOT_CONNECTED_ERROR;
		goto end;
	}

	/* topic. */
	if (0 == strlen(argv[2]) || MQTT_MAX_TOPIC_LEN < strlen(argv[2])) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTUNSUB] Invalid topic\r\n");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	for (topic_index = 0; MAX_MESSAGE_HANDLERS > topic_index; topic_index++) {
		if (NULL != mqttCb->subData[topic_index].topic && 0 == strcmp(mqttCb->subData[topic_index].topic, argv[2])) {
			matched = 1;
			break;
		}
	}
	if (topic_index == MAX_MESSAGE_HANDLERS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTUNSUB] Invalid topic\r\n");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "[+MQTTUNSUB] Unsubscribe topic %s\r\n", mqttCb->subData[topic_index].topic);

	TimerInit(&mqttCb->client.cmd_timer);
	TimerCountdownMS(&mqttCb->client.cmd_timer, mqttCb->client.command_timeout_ms);
	res = MQTTUnsubscribe(&mqttCb->client, mqttCb->subData[topic_index].topic);
	if (0 != res) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTUNSUB] Unsubscribe ERROR\r\n");
		resultNo = MQTT_UNSUBSCRIBE_ERROR;
		goto end;
	}

end:
	if (matched && NULL != mqttCb->subData[topic_index].topic) {
		rtos_mem_free(mqttCb->subData[topic_index].topic);
		mqttCb->subData[topic_index].topic = NULL;
		mqttCb->subData[topic_index].qos = 0;
		mqttCb->client.messageHandlers[topic_index].topicFilter = 0;
		mqttCb->client.messageHandlers[topic_index].fp = NULL;
	}
	if (MQTT_OK != resultNo) {
		at_printf(ATCMD_ERROR_END_STR, resultNo);
	} else {
		at_printf(ATCMD_OK_END_STR);
	}
}

static void at_mqttpub_help(void)
{
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "AT+MQTTPUB=<link_id>,<msg_id>,<topic>,<msg>,<qos>,<retain>\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<link_id>: \t0~3. link ID.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<msg_id>: \t0~65535. Message ID.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<topic>: \tMQTT topic.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<msg>: \tMQTT message.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<qos>: \tQoS of message, which can be set to 0, 1, or 2. Default: 0.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<retain>: \tretain flag.\r\n");
}

/****************************************************************
Function            at_mqttpub
Brief                   MQTT publish. The command is used to publish mqtt issue.
AT+MQTTPUB=<link_id>,<msg_id>,<topic>,<msg>,<qos>,<retain>
****************************************************************/
void at_mqttpub(void *arg)
{
	char *argv[MAX_ARGC] = {0};
	int argc = 0, res = 0;
	MQTT_RESULT_ENUM resultNo = MQTT_OK;
	MQTT_CONTROL_BLOCK *mqttCb = NULL;
	int link_id = MQTT_MAX_CLIENT_NUM;
	int msg_id = 0, qos = 0, retain = 0;

	/* Get the parameters of AT command. */
	if (!arg) {
		at_mqttpub_help();
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	argc = parse_param_advance(arg, argv);
	if (argc < 7) {
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	/* link_id. */
	link_id = atoi(argv[1]);
	if (link_id >= MQTT_MAX_CLIENT_NUM || link_id < 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTPUB] Invalid link_id\r\n");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	mqttCb = &g_mqttCb[link_id];
	if (!mqttCb->client.isconnected || !mqttCb->networkConnect) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTPUB] The link ID is not connected\r\n");
		resultNo = MQTT_NOT_CONNECTED_ERROR;
		goto end;
	}

	/* msg ID. */
	msg_id = atoi(argv[2]);
	if (msg_id < 0 || msg_id > 0xFFFF) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTPUB] Invalid msg ID\r\n");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	/* topic. */
	if (0 == strlen(argv[3]) || MQTT_MAX_TOPIC_LEN < strlen(argv[3])) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTPUB] Invalid topic\r\n");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	/* msg. */
	if (0 == strlen(argv[4]) || MQTT_MAX_MSG_LEN < strlen(argv[4])) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTPUB] Invalid msg\r\n");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	/* QoS. */
	qos = atoi(argv[5]);
	if (qos < QOS0 || qos > QOS2) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTPUB] Invalid QoS\r\n");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	/* retain. */
	retain = atoi(argv[6]);
	if (retain != 0 && retain != 1) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTPUB] Invalid retain\r\n");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	mqttCb->pubData.topic = argv[3];
	mqttCb->pubData.msg = argv[4];
	mqttCb->pubData.qos = (u8)qos;
	mqttCb->pubData.retain = (u8)retain;

	/* send publish. */
	MQTTMessage mqttMsg = {.qos = mqttCb->pubData.qos,
						   .retained = mqttCb->pubData.retain,
						   .dup = 0,
						   .id = msg_id,
						   .payload = mqttCb->pubData.msg,
						   .payloadlen = strlen(mqttCb->pubData.msg)
						  };
	res = MQTTPublish(&mqttCb->client, mqttCb->pubData.topic, &mqttMsg);
	if (0 != res) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTPUB] Publish ERROR\r\n");
		resultNo = MQTT_PUBLISH_ERROR;
		goto end;
	}

end:
	if (MQTT_OK != resultNo) {
		at_printf(ATCMD_ERROR_END_STR, resultNo);
	} else {
		at_printf(ATCMD_OK_END_STR);
	}
}

static void at_mqttpubraw_help(void)
{
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "AT+MQTTPUBRAW=<link_id>,<msg_id>,<topic>,<length>,<qos>,<retain>\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<link_id>: \t0~3. link ID.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<msg_id>: \t0~65535. Message ID.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<topic>: \tMQTT topic.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<length>: \tMQTT message length.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<qos>: \tQoS of message, which can be set to 0, 1, or 2. Default: 0.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<retain>: \tretain flag.\r\n");
}

/****************************************************************
Function            at_mqttpubraw
Brief                   MQTT publish with long message. The command is used to publish mqtt issue.
AT+MQTTPUBRAW=<link_id>,<msg_id>,<topic>,<length>,<qos>,<retain>
****************************************************************/
void at_mqttpubraw(void *arg)
{
	char *argv[MAX_ARGC] = {0};
	int argc = 0, res = 0;
	MQTT_RESULT_ENUM resultNo = MQTT_OK;
	MQTT_CONTROL_BLOCK *mqttCb = NULL;
	int link_id = MQTT_MAX_CLIENT_NUM;
	int msg_id = 0, length = 0, qos = 0, retain = 0;
	int tt_get_len = 0;
	u8 *msg = NULL;

	/* Get the parameters of AT command. */
	if (!arg) {
		at_mqttpubraw_help();
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	argc = parse_param_advance(arg, argv);
	if (argc < 7) {
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	/* link_id. */
	link_id = atoi(argv[1]);
	if (link_id >= MQTT_MAX_CLIENT_NUM || link_id < 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTPUBRAW] Invalid link_id\r\n");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	mqttCb = &g_mqttCb[link_id];
	if (!mqttCb->client.isconnected || !mqttCb->networkConnect) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTPUBRAW] The link ID is not connected\r\n");
		resultNo = MQTT_NOT_CONNECTED_ERROR;
		goto end;
	}

	/* msg ID. */
	msg_id = atoi(argv[2]);
	if (msg_id < 0 || msg_id > 0xFFFF) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTPUBRAW] Invalid msg ID\r\n");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	/* topic. */
	if (0 == strlen(argv[3]) || MQTT_MAX_TOPIC_LEN < strlen(argv[3])) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTPUBRAW] Invalid topic\r\n");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	/* msg length. */
	length = atoi(argv[4]);
	if (length <= 0 || length > MAX_TT_BUF_LEN) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTPUBRAW] Invalid length\r\n");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	/* QoS. */
	qos = atoi(argv[5]);
	if (qos < QOS0 || qos > QOS2) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTPUBRAW] Invalid QoS\r\n");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	/* retain. */
	retain = atoi(argv[6]);
	if (retain != 0 && retain != 1) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTPUBRAW] Invalid retain\r\n");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	msg = (u8 *)rtos_mem_zmalloc(length);
	if (msg == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTPUBRAW] Msg malloc failed\r\n");
		resultNo = MQTT_MALLOC_FAILED;
		goto end;
	}

	/* tt mode */
	res = atcmd_tt_mode_start(length);
	if (res < 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTPUBRAW] atcmd_tt_mode_start failed\r\n");
		resultNo = MQTT_TT_MODE_ERROR;
		goto end;
	}

	tt_get_len = atcmd_tt_mode_get(msg, length);
	if (tt_get_len == 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTPUBRAW] host stops tt mode\r\n");
		resultNo = MQTT_TT_MODE_ERROR;
		atcmd_tt_mode_end();
		goto end;
	}
	atcmd_tt_mode_end();

	mqttCb->pubData.topic = argv[3];
	mqttCb->pubData.msg = (char *)msg;
	mqttCb->pubData.qos = (u8)qos;
	mqttCb->pubData.retain = (u8)retain;

	/* send publish. */
	MQTTMessage mqttMsg = {.qos = mqttCb->pubData.qos,
						   .retained = mqttCb->pubData.retain,
						   .dup = 0,
						   .id = msg_id,
						   .payload = mqttCb->pubData.msg,
						   .payloadlen = tt_get_len
						  };
	res = MQTTPublish(&mqttCb->client, mqttCb->pubData.topic, &mqttMsg);
	if (0 != res) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTPUBRAW] Publish ERROR\r\n");
		resultNo = MQTT_PUBLISH_ERROR;
		goto end;
	}

end:
	/* After sent, clean the msg. */
	if (msg) {
		rtos_mem_free(msg);
	}
	if (MQTT_OK != resultNo) {
		at_printf(ATCMD_ERROR_END_STR, resultNo);
	} else {
		at_printf(ATCMD_OK_END_STR);
	}
}

static void at_mqttconn_help(void)
{
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "AT+MQTTCONN=<link_id>,<host>,<port>,<client_id>,<conn_type>[,<cert_index>,<username>,<password>]\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<link_id>:\t0~%d.\r\n", MQTT_MAX_CLIENT_NUM - 1);
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<host>:\thostname\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<port>:\t0~65535. \r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<client_id>:\tstring, MQTT client ID.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<conn_type>:\t1~5. Connection type.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t\t1:\tMQTT over TCP.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t\t2:\tMQTT over TLS (no certificate verify).\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t\t3:\tMQTT over TLS (verify server certificate).\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t\t4:\tMQTT over TLS (provide client certificate).\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t\t5:\tMQTT over TLS (verify server certificate and provide client certificate).\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<cert_index>:\tcertificate index.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<username>:\tstring, the username to login to the MQTT broker.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<password>:\tstring, the password to login to the MQTT broker.\r\n");
}
/****************************************************************
Function            at_mqttconn
Brief                   MQTT connect. The command is used to connect a cloud for MQTT client.
AT+MQTTCONN=<link_id>,<host>,<port>,<client_id>,<conn_type>[,<cert_index>,<username>,<password>]
****************************************************************/
void at_mqttconn(void *arg)
{
	char *argv[MAX_ARGC] = {0};
	int argc = 0, res = 0;
	MQTT_RESULT_ENUM resultNo = MQTT_OK;
	MQTT_CONTROL_BLOCK *mqttCb = NULL;
	int link_id = MQTT_MAX_CLIENT_NUM;
	char TaskName[6];
	u16 stacksize = 4096;
	int conn_type = 0, cert_index = 0, port = 0;

	/* Get the parameters of AT command. */
	if (!arg) {
		at_mqttconn_help();
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	/* parameters include link_id, host, port. */
	argc = parse_param_advance(arg, argv);
	if (argc < 6) {
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

	mqttCb = &g_mqttCb[link_id];
	if (mqttCb->client.isconnected) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTCONN] The link ID is already connected\r\n");
		resultNo = MQTT_ALREADY_CONNECTED;
		goto end;
	}

	mqttCb->linkId = (u8)link_id;
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
	if (strlen(argv[3]) > 0) {
		port = atoi(argv[3]);
		if (port < 0 || port > 0xFFFF) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTCONN] Invalid port\r\n");
			resultNo = MQTT_ARGS_ERROR;
			goto end;
		}
	}

	/* client_id. */
	if (strlen(argv[4]) == 0 || strlen(argv[4]) > MQTT_MAX_CLIENT_ID_LEN) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTCONN] Invalid client_id\r\n");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}
	resultNo = mqtt_string_copy(&mqttCb->clientId, argv[4], strlen(argv[4]));
	if (MQTT_OK != resultNo) {
		goto end;
	}

	/* conn_type. */
	conn_type = atoi(argv[5]);
	if (conn_type <= MQTT_OVER_NONE || conn_type >= MQTT_OVER_MAX) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTCONN] Invalid conn_type\r\n");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	/* certificate_index. (Optional) */
	if (argc >= 7) {
		cert_index = atoi(argv[6]);
		if (conn_type >= MQTT_OVER_TLS_VERIFY_NONE) {
			if (cert_index < 0) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTCONN] Invalid certificate_index\r\n");
				resultNo = MQTT_ARGS_ERROR;
				goto end;
			}
			if (conn_type == MQTT_OVER_TLS_VERIFY_SERVER || conn_type == MQTT_OVER_TLS_VERIFY_BOTH) {
				resultNo = mqtt_set_ssl_certificate(&mqttCb->network.rootCA, CLIENT_CA, cert_index);
				if (MQTT_OK != resultNo) {
					goto end;
				}
			}

			if (conn_type == MQTT_OVER_TLS_VERIFY_CLIENT || conn_type == MQTT_OVER_TLS_VERIFY_BOTH) {
				resultNo = mqtt_set_ssl_certificate(&mqttCb->network.clientCA, CLIENT_CERT, cert_index);
				if (MQTT_OK != resultNo) {
					goto end;
				}

				resultNo = mqtt_set_ssl_certificate(&mqttCb->network.private_key, CLIENT_KEY, cert_index);
				if (MQTT_OK != resultNo) {
					goto end;
				}
			}
			mqttCb->useSsl = conn_type;
		}
	}

	/* username. (Optional) */
	if (argc >= 8) {
		if (strlen(argv[7]) != 0 && strlen(argv[7]) > MQTT_MAX_USERNAME_LEN) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTCONN] Invalid username\r\n");
			resultNo = MQTT_ARGS_ERROR;
			goto end;
		}
		if (strlen(argv[7]) != 0) {
			resultNo = mqtt_string_copy(&mqttCb->userName, argv[7], strlen(argv[7]));
			if (MQTT_OK != resultNo) {
				resultNo = MQTT_ARGS_ERROR;
				goto end;
			}
		}
	}

	/* password. (Optional) */
	if (argc == 9) {
		if (strlen(argv[8]) != 0 && strlen(argv[8]) > MQTT_MAX_PASSWORD_LEN) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTCONN] Invalid password\r\n");
			resultNo = MQTT_ARGS_ERROR;
			goto end;
		}
		if (strlen(argv[8]) != 0) {
			resultNo = mqtt_string_copy(&mqttCb->password, argv[8], strlen(argv[8]));
			if (MQTT_OK != resultNo) {
				resultNo = MQTT_ARGS_ERROR;
				goto end;
			}
		}
	}

	/* Update the mqttCb information. */
	if (port == 0) {
		if (mqttCb->useSsl) {
			mqttCb->port = MQTT_DEFAULT_PORT_TLS;
		} else {
			mqttCb->port = MQTT_DEFAULT_PORT;
		}
	} else {
		mqttCb->port = port;
	}
	mqttCb->network.use_ssl = mqttCb->useSsl;
	mqttCb->connectData.clientID.cstring = mqttCb->clientId;
	/* The connect may be anonymous, ie. without username and password. */
	if (mqttCb->userName != NULL) {
		mqttCb->connectData.username.cstring = mqttCb->userName;
	}
	if (mqttCb->password != NULL) {
		mqttCb->connectData.password.cstring = mqttCb->password;
	}

	if (!mqttCb->initailClient) {
		resultNo = mqtt_init_client_buf(&mqttCb->client, &mqttCb->network, MQTT_CMD_PKT_TIMEOUT_MS);
		if (resultNo != MQTT_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTCONN] Init client fail\r\n");
			goto end;
		}
		mqttCb->client.cb = (void *)mqttCb;
		mqttCb->initailClient = 1;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "[+MQTTCONN] Open a new connection for %s(%d)\r\n", mqttCb->host, mqttCb->port);
	if (!mqttCb->networkConnect) {
		/* Try to connect the host. */
		res = NetworkConnect(&mqttCb->network, mqttCb->host, mqttCb->port);
		if (res != 0) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTCONN] Can not connect %s\r\n", mqttCb->host);
			MQTTSetStatus(&mqttCb->client, MQTT_START);
			resultNo = MQTT_CONNECTION_ERROR;
			goto end;
		}
		mqttCb->networkConnect = 1;
	}

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
		if (RTK_SUCCESS != rtos_task_create(&mqttCb->taskHandle, TaskName, mqtt_main, (void *)mqttCb, stacksize, ATCMD_MQTT_TASK_PRIORITY)) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTCONN] Create task failed\r\n");
			mqttCb->taskState = MQTT_TASK_NOT_CREATE;
			resultNo = MQTT_THREAD_CREATE_ERROR;
			goto end;
		}
	}

	/* Set timer. */
	TimerCountdownMS(&mqttCb->client.cmd_timer, mqttCb->client.command_timeout_ms);
	res = MQTTConnect(&mqttCb->client, &mqttCb->connectData);
	if (res != 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTCONN] Connect ERROR\r\n");
		resultNo = MQTT_CONNECTION_ERROR;
		goto end;
	} else {
		/* AT output when receiving CONNACK. */
		RTK_LOGS(TAG, RTK_LOG_INFO, "[+MQTTCONN] Sent connection request, waiting for ACK\r\n");
		mqttCb->client.isconnected = 0;
		MQTTSetStatus(&mqttCb->client, MQTT_CONNECT);
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
	mqttCb = &g_mqttCb[link_id];

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
			resultNo = MQTT_DISCONNECTION_ERROR;
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
	mqtt_release_resource(mqttCb);

	if (MQTT_OK != resultNo) {
		at_printf(ATCMD_ERROR_END_STR, resultNo);
	} else {
		at_printf(ATCMD_OK_END_STR);
	}
}

static void at_mqttcfg_help(void)
{
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "AT+MQTTCFG=<link_id>[,<keepalive>][,<cmd_timeout>][,<clean_session>][,<lwt_topic>,<lwt_msg>][,<lwt_qos>][,<lwt_retain>]\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<link_id>:\t0~3. link ID.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<keepalive>:\ttimeout of MQTT ping. Unit: second.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<cmd_timeout>:\tmqtt connect and subcribe timeout. Unit: second.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<clean_session>:\tmqtt clean session option.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t\t0:\tdisable clean session.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t\t1:\tenable clean session.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<lwt_topic>:\tLWT (Last Will and Testament) message topic. Maximum length: 128 bytes.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<lwt_msg>:\tLWT message. Maximum length: 128 bytes.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<lwt_qos>:\tLWT QoS, which can be set to 0, 1, or 2. Default: 0.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<lwt_retain>:\tLWT retain, which can be set to 0 or 1. Default: 0.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "e.g.\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\tAT+MQTTCFG=0,20,60,0,topic/lwt,This is a lwt message,0,0\r\n");
}

/****************************************************************
Function            at_mqttcfg
Brief                   MQTT connection parameters configure. The command is used to configure mqtt connection parameters.
AT+MQTTCFG=<link_id>[,<keepalive>][,<cmd_timeout>][,<clean_session>][,<lwt_topic>,<lwt_msg>][,<lwt_qos>][,<lwt_retain>]
****************************************************************/
void at_mqttcfg(void *arg)
{
	char *argv[MAX_ARGC] = {0};
	int argc = 0;
	MQTT_RESULT_ENUM resultNo = MQTT_OK;
	MQTT_CONTROL_BLOCK *mqttCb = NULL;
	int link_id = MQTT_MAX_CLIENT_NUM;
	int keepalive = 60, timeout = 60, qos = 0, retain = 0, clean_session = 1;

	/* Get the parameters of AT command. */
	if (!arg) {
		at_mqttcfg_help();
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	argc = parse_param_advance(arg, argv);
	if (argc < 2) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTCFG] Input wrong parameter\r\n");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	/* link_id. */
	link_id = atoi(argv[1]);
	if (link_id >= MQTT_MAX_CLIENT_NUM || link_id < 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTCFG] Invalid link_id\r\n");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	mqttCb = &g_mqttCb[link_id];
	if (mqttCb->client.isconnected) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTCFG] The link ID is already connected\r\n");
		resultNo = MQTT_ALREADY_CONNECTED;
		goto end;
	}

	if (!mqttCb->initailClient) {
		resultNo = mqtt_init_client_buf(&mqttCb->client, &mqttCb->network, MQTT_CMD_PKT_TIMEOUT_MS);
		if (resultNo != MQTT_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTCONN] Init client fail\r\n");
			goto end;
		}
		mqttCb->initailClient = 1;
		mqttCb->client.cb = (void *)mqttCb;
	}

	/* keepalive. */
	if (argv[2] != NULL && (strlen(argv[2]) > 0)) {
		keepalive = atoi(argv[2]);
		if (keepalive <= 0) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTCFG] Invalid keepalive\r\n");
			resultNo = MQTT_ARGS_ERROR;
			goto end;
		}
	}

	/* timeout. */
	if (argv[3] != NULL && (strlen(argv[3]) > 0)) {
		timeout = atoi(argv[3]);
		if (timeout < 0) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTCFG] Invalid cmd_timeout\r\n");
			resultNo = MQTT_ARGS_ERROR;
			goto end;
		}
	}

	/* clean_session. */
	if (argv[4] != NULL && (strlen(argv[4]) > 0)) {
		clean_session = atoi(argv[4]);
		if (clean_session != 0 && clean_session != 1) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTCFG] Invalid clean_session\r\n");
			resultNo = MQTT_ARGS_ERROR;
			goto end;
		}
	}

	mqttCb->connectData.keepAliveInterval = (u16)keepalive;
	mqttCb->client.command_timeout_ms = (u32)(timeout * 1000);
	mqttCb->connectData.cleansession = (u8)clean_session;
	mqttCb->connectData.willFlag = 0;

	/* Last Will and Testament. (Optional) */
	if (argv[5] != NULL && (strlen(argv[5]) > 0)) {
		resultNo = mqtt_string_copy(&mqttCb->connectData.will.topicName.cstring, argv[5], strlen(argv[5]));
		if (MQTT_OK != resultNo) {
			goto end;
		}

		if (argv[6] == NULL || strlen(argv[6]) == 0) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTCFG] Invalid lwt_msg\r\n");
			resultNo = MQTT_ARGS_ERROR;
			goto end;
		}

		resultNo = mqtt_string_copy(&mqttCb->connectData.will.message.cstring, argv[6], strlen(argv[6]));
		if (MQTT_OK != resultNo) {
			goto end;
		}

		mqttCb->connectData.willFlag = 1;
	}

	if (argv[7] != NULL && (strlen(argv[7]) > 0)) {
		qos = atoi(argv[7]);
		if (qos < QOS0 || qos > QOS2) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTCFG] Invalid lwt_qos\r\n");
			resultNo = MQTT_ARGS_ERROR;
			goto end;
		}
	}

	if (argv[7] != NULL && (strlen(argv[7]) > 0)) {
		retain = atoi(argv[8]);
		if (retain != 0 && retain != 1) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTCFG] Invalid lwt_retain\r\n");
			resultNo = MQTT_ARGS_ERROR;
			goto end;
		}
	}
	mqttCb->connectData.will.qos = (char)qos;
	mqttCb->connectData.will.retained = (u8)retain;

end:
	if (MQTT_OK != resultNo) {
		at_printf(ATCMD_ERROR_END_STR, resultNo);
	} else {
		at_printf(ATCMD_OK_END_STR);
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
	UNUSED(arg);

	RTK_LOGS(TAG, RTK_LOG_INFO, "[+MQTTRESET] reset all mqtt connections\r\n");
	/* No need any other parameters. */
	for (; MQTT_MAX_CLIENT_NUM > i; i++) {
		mqttCb = &g_mqttCb[i];

		/* Stop the task. */
		if (MQTT_TASK_START == mqttCb->taskState) {
			mqttCb->taskState = MQTT_TASK_STOP;
			while (MQTT_TASK_NOT_CREATE != mqttCb->taskState) {
				rtos_time_delay_ms(20);
			}
		}

		/* Disconnect client. */
		if (mqttCb->client.isconnected) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "[+MQTTRESET] linkid: %d, still connected\r\n", mqttCb->linkId);
			res = MQTTDisconnect(&mqttCb->client);
			if (0 != res) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "[+MQTTRESET] linkid: %d, can not disconnect\r\n", mqttCb->linkId);
			}
			mqttCb->client.isconnected = 0;
		}

		/* Disconnect host. */
		if (mqttCb->networkConnect) {
			if (NULL != mqttCb->network.disconnect) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "[+MQTTRESET] linkid: %d, disconnect from %s\r\n", mqttCb->linkId, mqttCb->host);
				mqttCb->network.disconnect(&mqttCb->network);
			}
			mqttCb->networkConnect = 0;
		}

		/* Clean the Client Control block. */
		mqtt_release_resource(mqttCb);
	}

	/* This command only output "OK". */
	at_printf(ATCMD_OK_END_STR);
}

void at_mqttquery(void *arg)
{
	char *argv[MAX_ARGC] = {0};
	int argc = 0;
	MQTT_RESULT_ENUM resultNo = MQTT_OK;
	MQTT_CONTROL_BLOCK *mqttCb = NULL;
	int link_id = MQTT_MAX_CLIENT_NUM;

	/* Get the parameters of AT command. */
	if (!arg) {
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	argc = parse_param_advance(arg, argv);
	if (argc < 2) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTQUERY] Input wrong parameter\r\n");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	/* link_id. */
	link_id = atoi(argv[1]);
	if (link_id >= MQTT_MAX_CLIENT_NUM || link_id < 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTQUERY] Invalid link_id\r\n");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}
	mqttCb = &g_mqttCb[link_id];

	at_printf("\r\n");
	at_printf("link_id: %d\r\n", link_id);
	at_printf("state: %d\r\n", mqttCb->client.isconnected);
	if (mqttCb->useSsl) {
		at_printf("conn_type: %d\r\n", mqttCb->useSsl);
	} else {
		at_printf("conn_type: %d\r\n", MQTT_OVER_TCP);
	}
	at_printf("host: %s\r\n", mqttCb->host ? mqttCb->host : "NULL");
	at_printf("port: %d\r\n", mqttCb->port);
	at_printf("client_id: %s\r\n", mqttCb->clientId ? mqttCb->clientId : "NULL");
	at_printf("username: %s\r\n", mqttCb->userName ? mqttCb->userName : "NULL");
	at_printf("password: %s\r\n", mqttCb->password ? mqttCb->password : "NULL");
	at_printf("cmd_timeout: %d\r\n", mqttCb->client.command_timeout_ms / 1000);
	at_printf("keepalive: %d\r\n", mqttCb->connectData.keepAliveInterval);
	at_printf("clean_session: %d\r\n", mqttCb->connectData.cleansession);
	at_printf("LWT: %d\r\n", mqttCb->connectData.willFlag);
	if (mqttCb->connectData.willFlag) {
		MQTTPacket_willOptions *will = &mqttCb->connectData.will;
		at_printf("lwt_qos: %d\r\n", will->qos);
		at_printf("lwt_retain: %d\r\n", will->retained);
		at_printf("lwt_topic: %s\r\n", will->topicName.cstring);
		at_printf("lwt_msg: %s\r\n", will->message.cstring);
	}
end:
	if (MQTT_OK == resultNo) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, resultNo);
	}
}

void at_mqttsubquery(void *arg)
{
	char *argv[MAX_ARGC] = {0};
	int argc = 0;
	MQTT_RESULT_ENUM resultNo = MQTT_OK;
	MQTT_CONTROL_BLOCK *mqttCb = NULL;
	int link_id = MQTT_MAX_CLIENT_NUM;

	/* Get the parameters of AT command. */
	if (!arg) {
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	argc = parse_param_advance(arg, argv);
	if (argc < 2) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTSUBQUERY] Input wrong parameter\r\n");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	/* link_id. */
	link_id = atoi(argv[1]);
	if (link_id >= MQTT_MAX_CLIENT_NUM || link_id < 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[+MQTTSUBQUERY] Invalid link_id\r\n");
		resultNo = MQTT_ARGS_ERROR;
		goto end;
	}

	mqttCb = &g_mqttCb[link_id];

	at_printf("\r\n");
	at_printf("link_id: %d\r\n", link_id);
	at_printf("state: %d\r\n", mqttCb->client.isconnected);
	for (int i = 0; i < MAX_MESSAGE_HANDLERS; i++) {
		if (mqttCb->subData[i].topic) {
			at_printf("topic: %s, qos: %d\r\n", mqttCb->subData[i].topic, mqttCb->subData[i].qos);
		}
	}
end:
	if (MQTT_OK == resultNo) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, resultNo);
	}
}

/* There are 4 connection IDs at most. */
void mqtt_init_info(void)
{
	for (int i = 0; i < MQTT_MAX_CLIENT_NUM; i++) {
		_memset(&g_mqttCb[i], 0, sizeof(MQTT_CONTROL_BLOCK));
		_memcpy(&g_mqttCb[i].connectData, &mqtt_default_conn_data, sizeof(MQTTPacket_connectData));
		g_mqttCb[i].port = MQTT_DEFAULT_PORT;
		g_mqttCb[i].pubData.qos = MQTT_DEFAULT_QOS;
		g_mqttCb[i].pubData.retain = MQTT_DEFAULT_RETAIN;
		g_mqttCb[i].client.command_timeout_ms = MQTT_CMD_PKT_TIMEOUT_MS;
		NetworkInit(&g_mqttCb[i].network);
	}
}

ATCMD_TABLE_DATA_SECTION
const log_item_t at_mqtt_items[ ] = {
	{"+MQTTCFG", at_mqttcfg, {NULL, NULL}},   /* MQTT connection configure. */
	{"+MQTTCONN", at_mqttconn, {NULL, NULL}},  /* MQTT connect. */
	{"+MQTTDISCONN", at_mqttdisconn, {NULL, NULL}},  /* MQTT disconnect. */
	{"+MQTTSUB", at_mqttsub, {NULL, NULL}},  /* MQTT subscribe. */
	{"+MQTTUNSUB", at_mqttunsub, {NULL, NULL}},  /* MQTT unsubscribe. */
	{"+MQTTPUB", at_mqttpub, {NULL, NULL}},  /* MQTT publish. */
	{"+MQTTPUBRAW", at_mqttpubraw, {NULL, NULL}}, /* MQTT publish raw. */
	{"+MQTTRESET", at_mqttreset, {NULL, NULL}},   /* MQTT reset. */
	{"+MQTTQUERY", at_mqttquery, {NULL, NULL}},    /* MQTT connection status query */
	{"+MQTTSUBQUERY", at_mqttsubquery, {NULL, NULL}} /* MQTT subcribe query */
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

	atcmd_service_add_table((log_item_t *)at_mqtt_items, sizeof(at_mqtt_items) / sizeof(at_mqtt_items[0]));
}

#endif /* CONFIG_ATCMD_MQTT */
#endif /* CONFIG_LWIP_LAYER */
