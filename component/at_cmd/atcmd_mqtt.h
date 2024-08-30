/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ATCMD_MQTT_H__
#define __ATCMD_MQTT_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include "main.h"
#ifdef CONFIG_LWIP_LAYER
#include <lwip_netconf.h>
#endif
#include "MQTTClient.h"
#include "MQTTPublish.h"
#include "MQTTFreertos.h"
#include "MQTTConnect.h"
#include "os_wrapper.h"

#ifndef CONFIG_MP_INCLUDED

/* Remain 30 bytes for command-header and other parameters. */
#define MQTT_SINGLE_STR_LEN         (UART_LOG_CMD_BUFLEN - 30)

/* There are 4 connection IDs at most. */
#define MQTT_MAX_CLIENT_NUM         4

#define MQTT_DEFAULT_PORT           1883

#define MQTT_DEFAULT_QOS            2    /* QOS2 */

#define MQTT_DEFAULT_RETAIN         0

#define MQTT_DEFAULT_SENDBUF_SIZE   MQTT_SINGLE_STR_LEN

#define MQTT_SELECT_TIMEOUT         1

#define MQTT_EXCEPT_FDS             1

#define MQTT_CONNECT_LATER          1

/* Host name's length (excluding '\0'). */
#define MQTT_MAX_HOSTNAME_LEN       MQTT_SINGLE_STR_LEN

/* Client ID's length  (excluding '\0').*/
#define MQTT_MAX_CLIENT_ID_LEN      MQTT_SINGLE_STR_LEN

/* username's length  (excluding '\0'). */
#define MQTT_MAX_USERNAME_LEN       MQTT_SINGLE_STR_LEN

/* password's length  (excluding '\0'). */
#define MQTT_MAX_PASSWORD_LEN       MQTT_SINGLE_STR_LEN

/* topic's length  (excluding '\0'). */
#define MQTT_MAX_TOPIC_LEN          MQTT_SINGLE_STR_LEN

/* msg's length  (excluding '\0'). */
#define MQTT_MAX_MSG_LEN            MQTT_DEFAULT_SENDBUF_SIZE

#define ATCMD_MQTT_TASK_PRIORITY    4

/* Default timeout == 60s. */
#define MQTT_CMD_PKT_TIMEOUT_MS     60000

typedef enum MQTT_CONFIG_CMD_TYPES_e {
	MQTT_CONFIG_VERSION = 0,
	MQTT_CONFIG_KEEPALIVE,
	MQTT_CONFIG_SESSION,
	MQTT_CONFIG_TIMEOUT,
	MQTT_CONFIG_WILL,
	MQTT_CONFIG_SSL,
	MQTT_CONFIG_CMD_NUM
}
MQTT_CONFIG_CMD_TYPES;

/* This enum may be modified later. */
typedef enum MQTT_RESULT_ENUM_e {
	MQTT_OK = 0,
	MQTT_COMMON_ERROR,
	MQTT_ARGS_ERROR,
	MQTT_ALREADY_EXIST_ID,
	MQTT_MEMORY_ERROR,
	MQTT_NOT_ATTACH_ERROR,
	MQTT_NOT_OPEN_ERROR,
	MQTT_CONNECTION_ERROR,
	MQTT_AUTH_ERROR,
	MQTT_CLIENTID_REJECTED_ERROR,
	MQTT_NOT_CONNECTED_ERROR,
	MQTT_ALREADY_CONNECTED_ERROR,
	MQTT_PUBLISH_ERROR,
	MQTT_SUBSCRIPTION_ERROR,
	MQTT_ALREADY_SUBSCRIBED_ERROR,
	MQTT_NOT_SUBSCRIBED_ERROR,
	MQTT_UNSUBSCRIPTION_ERROR,
	MQTT_WAITACK_TIMEOUT_ERROR,
	MQTT_THREAD_CREATE_ERROR,
	MQTT_NETWORK_LINK_ERROR
}
MQTT_RESULT_ENUM;

typedef enum MQTT_CLIENT_TASK_e {
	MQTT_TASK_NOT_CREATE = 0,
	MQTT_TASK_START,
	MQTT_TASK_STOP
}
MQTT_CLIENT_TASK;

typedef struct MQTT_PUB_DATA_t {
	u8 qos;
	u8 retain;
	char *topic;
	char *msg;
}
MQTT_PUB_DATA;

typedef struct MQTT_CONTROL_BLOCK_t {
	u8          tcpConnectId;   /* 0 ~ 3 */
	char        *host;
	u16         port;           /* 1 ~ 65535 */
#if MQTT_OVER_SSL
	u8          useSsl;
#endif
	char        *clientId;
	char        *userName;
	char        *password;
	/* topic[i] corresponds to client.messageHandlers[i]. */
	char        *topic[MAX_MESSAGE_HANDLERS];
	MQTT_PUB_DATA   pubData;
	u8          networkConnect;
	u8          offline;        /* Set to 1 when offline during connecting status. */
	u8          initialConnect; /* Set to 1 between MQTTCONN and CONN_ACK. */
	Network         network;
	MQTTClient  client;
	MQTTPacket_connectData  connectData;
	rtos_task_t         taskHandle;
	MQTT_CLIENT_TASK    taskState;
}
MQTT_CONTROL_BLOCK;

typedef struct MQTT_AT_HANDLE_t {
	MQTT_CONTROL_BLOCK  *mqttCb[MQTT_MAX_CLIENT_NUM];
}
MQTT_AT_HANDLE;

void mqtt_main(void *param);
void print_mqtt_at(void);
void at_mqtt_init(void);

extern MQTT_AT_HANDLE mqtt_at_handle;
extern MQTTPacket_connectData mqtt_default_conn_data;

extern int keepalive(MQTTClient *c);
extern int sendPacket(MQTTClient *c, int length, Timer *timer);
extern int readPacket(MQTTClient *c, Timer *timer);
extern int deliverMessage(MQTTClient *c, MQTTString *topicName, MQTTMessage *message);
#endif /* CONFIG_MP_INCLUDED */

#if defined(__cplusplus)
}
#endif

#endif
