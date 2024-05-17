/*******************************************************************************
 * Copyright (c) 2014, 2015 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Allan Stockdill-Mander - initial API and implementation and/or initial documentation
 *******************************************************************************/

#if !defined(MQTTFreeRTOS_H)
#define MQTTFreeRTOS_H


#include "lwipconf.h"
#include "os_wrapper.h"
#include "ameba.h"
#include "FreeRTOS.h"
#include "task.h"

#define MQTT_OVER_SSL (1)
#if (MQTT_OVER_SSL)
#include "mbedtls/config.h"
#include "mbedtls/platform.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl.h"
#include "mbedtls/error.h"
#include "mbedtls/debug.h"
#include "mbedtls/version.h"
#if (MBEDTLS_VERSION_NUMBER >= 0x03000000)
#include "mbedtls/psa_util.h"
#endif
#endif

enum {
	MQTT_EXCESSIVE, MQTT_MSGDUMP, MQTT_DEBUG, MQTT_INFO, MQTT_ALWAYS, MQTT_WARNING, MQTT_ERROR
};

#define FreeRTOS_Select select

#define mqtt_printf(level, fmt, arg...)     \
	do {\
		if (level >= MQTT_DEBUG) {\
			{\
				RTK_LOGA("MQTT", "[%d]mqtt:"fmt"\n\r", (int)rtos_time_get_current_system_time_ms(), ##arg);\
			} \
		}\
	}while(0)

typedef struct Timer {
	uint32_t xTicksToWait;
	TimeOut_t xTimeOut;
} Timer;

typedef struct Network Network;

struct Network {
	int my_socket;
	int (*mqttread)(Network *, unsigned char *, int, int);
	int (*mqttwrite)(Network *, unsigned char *, int, int);
	void (*disconnect)(Network *);
	int m2m_rxevent;

#if (MQTT_OVER_SSL)
	unsigned char use_ssl;
	mbedtls_ssl_context *ssl;
	mbedtls_ssl_config *conf;
	char *rootCA;
	char *clientCA;
	char *private_key;
#endif
	int *ciphersuites;
};

void TimerInit(Timer *);
char TimerIsExpired(Timer *);
void TimerCountdownMS(Timer *, unsigned int);
void TimerCountdown(Timer *, unsigned int);
int TimerLeftMS(Timer *);

typedef struct Mutex {
	rtos_sema_t sem;
} Mutex;

void MutexInit(Mutex *);
int MutexLock(Mutex *);
int MutexUnlock(Mutex *);

typedef struct Thread {
	rtos_task_t task;
} Thread;

int ThreadStart(Thread *, void (*fn)(void *), void *arg);

int FreeRTOS_read(Network *, unsigned char *, int, int);
int FreeRTOS_write(Network *, unsigned char *, int, int);
void FreeRTOS_disconnect(Network *);

void NetworkInit(Network *);
int NetworkConnect(Network *, char *, int);
/*int NetworkConnectTLS(Network*, char*, int, SlSockSecureFiles_t*, unsigned char, unsigned int, char);*/

#endif
