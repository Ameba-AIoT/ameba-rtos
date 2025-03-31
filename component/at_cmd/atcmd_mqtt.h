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

#ifdef CONFIG_LWIP_LAYER
#include <lwip_netconf.h>
#endif
#ifdef CONFIG_WLAN
#include "wifi_api.h"
#endif
#include "MQTTClient.h"
#include "MQTTPublish.h"
#include "MQTTFreertos.h"
#include "MQTTConnect.h"
#include "os_wrapper.h"
#include "atcmd_service.h"

/* There are 4 connection IDs at most. */
#define MQTT_MAX_CLIENT_NUM         4

#define MQTT_DEFAULT_PORT           1883
#define MQTT_DEFAULT_PORT_TLS       8883

#define MQTT_DEFAULT_QOS            2    /* QOS2 */

#define MQTT_DEFAULT_RETAIN         0

#define MQTT_SELECT_TIMEOUT         1

#define MQTT_EXCEPT_FDS             1

#define MQTT_CONNECT_LATER          1

#define MQTT_DEFAULT_BUF_SIZE       MAX_TT_BUF_LEN

/* Host name's length (excluding '\0'). */
#define MQTT_MAX_HOSTNAME_LEN       128

/* Client ID's length  (excluding '\0').*/
#define MQTT_MAX_CLIENT_ID_LEN      256

/* username's length  (excluding '\0'). */
#define MQTT_MAX_USERNAME_LEN       64

/* password's length  (excluding '\0'). */
#define MQTT_MAX_PASSWORD_LEN       64

/* topic's length  (excluding '\0'). */
#define MQTT_MAX_TOPIC_LEN          128

/* msg's length  (excluding '\0'). */
#define MQTT_MAX_MSG_LEN            512

#define ATCMD_MQTT_TASK_PRIORITY    4

/* Default timeout == 60s. */
#define MQTT_CMD_PKT_TIMEOUT_MS     60000

typedef enum MQTT_CONFIG_CONN_TYPES_e {
	MQTT_OVER_NONE = 0,
	MQTT_OVER_TCP,
	MQTT_OVER_TLS_VERIFY_NONE,
	MQTT_OVER_TLS_VERIFY_SERVER,
	MQTT_OVER_TLS_VERIFY_CLIENT,
	MQTT_OVER_TLS_VERIFY_BOTH,
	MQTT_OVER_MAX,
}
MQTT_CONFIG_CONN_TYPES;

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
	MQTT_ARGS_ERROR,
	MQTT_MALLOC_FAILED,
	MQTT_CONNECTION_ERROR,
	MQTT_DISCONNECTION_ERROR,
	MQTT_TT_MODE_ERROR,
	MQTT_CERTIFICATE_READ_ERROR,
	MQTT_NOT_CONNECTED_ERROR,
	MQTT_ALREADY_CONNECTED,
	MQTT_PUBLISH_ERROR,
	MQTT_SUBSCRIBE_ERROR,
	MQTT_UNSUBSCRIBE_ERROR,
	MQTT_ALREADY_SUBSCRIBED,
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

typedef struct MQTT_SUB_DATA_t {
	u8 qos;
	char *topic;
}
MQTT_SUB_DATA;

typedef struct MQTT_CONTROL_BLOCK_t {
	u8          linkId;   /* 0 ~ 3 */
	char        *host;
	u16         port;           /* 1 ~ 65535 */
#if MQTT_OVER_SSL
	u8          useSsl;
#endif
	char        *clientId;
	char        *userName;
	char        *password;
	/* topic[i] corresponds to client.messageHandlers[i]. */
	MQTT_SUB_DATA   subData[MAX_MESSAGE_HANDLERS];
	MQTT_PUB_DATA   pubData;
	u8          networkConnect;
	u8          initailClient;
	Network         network;
	MQTTClient  client;
	MQTTPacket_connectData  connectData;
	rtos_task_t         taskHandle;
	MQTT_CLIENT_TASK    taskState;
}
MQTT_CONTROL_BLOCK;

void mqtt_main(void *param);
void print_mqtt_at(void);
void at_mqtt_init(void);

extern MQTTPacket_connectData mqtt_default_conn_data;

extern int keepalive(MQTTClient *c);
extern int sendPacket(MQTTClient *c, int length, Timer *timer);
extern int readPacket(MQTTClient *c, Timer *timer);
extern int deliverMessage(MQTTClient *c, MQTTString *topicName, MQTTMessage *message);

#if defined(__cplusplus)
}
#endif

#endif
