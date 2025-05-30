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
 *    Ian Craggs - convert to FreeRTOS
 *******************************************************************************/

#include "MQTTFreertos.h"

#ifdef LWIP_IPV6
#undef LWIP_IPV6
#endif
#ifdef inet_ntop
#undef inet_ntop
#endif
#ifdef inet_pton
#undef inet_pton
#endif
#define LWIP_IPV6 0
#if LWIP_IPV6
#define inet_ntop(af,src,dst,size) \
    (((af) == AF_INET6) ? ip6addr_ntoa_r((src),(dst),(size)) \
     : (((af) == AF_INET) ? ipaddr_ntoa_r((src),(dst),(size)) : NULL))
#define inet_pton(af,src,dst) \
    (((af) == AF_INET6) ? inet6_aton((src),(dst)) \
     : (((af) == AF_INET) ? inet_aton((src),(dst)) : 0))
#else /* LWIP_IPV6 */
#define inet_ntop(af,src,dst,size) \
    (((af) == AF_INET) ? ipaddr_ntoa_r((src),(dst),(size)) : NULL)
#define inet_pton(af,src,dst) \
    (((af) == AF_INET) ? inet_aton((src),(dst)) : 0)
#endif /* LWIP_IPV6 */

int ThreadStart(Thread *thread, void (*fn)(void *), void *arg)
{
	int rc = 0;
	uint16_t usTaskStackSize = (RTOS_MINIMAL_STACK_SIZE * 5);
	uint32_t uxTaskPriority = rtos_task_priority_get(NULL); /* set the priority as the same as the calling task*/

	rc = rtos_task_create(&thread->task, "MQTTTask", fn, arg, usTaskStackSize * 4, uxTaskPriority);

	return rc;
}


void MutexInit(Mutex *mutex)
{
	rtos_mutex_create(&(mutex->sem));
}

int MutexLock(Mutex *mutex)
{
	return rtos_mutex_take(mutex->sem, RTOS_MAX_DELAY);
}

int MutexUnlock(Mutex *mutex)
{
	return rtos_mutex_give(mutex->sem);
}


void TimerCountdownMS(Timer *timer, unsigned int timeout_ms)
{
	timer->ms_to_wait = timeout_ms;
	rtos_task_set_time_out_state(&timer->xTimeOut); /* Record the time at which this function was entered. */
}


void TimerCountdown(Timer *timer, unsigned int timeout)
{
	TimerCountdownMS(timer, timeout * 1000);
}


int TimerLeftMS(Timer *timer)
{
	rtos_task_check_for_time_out(&timer->xTimeOut, &timer->ms_to_wait); /* updates ms_to_wait to the number left */
	return (timer->ms_to_wait);
}


char TimerIsExpired(Timer *timer)
{
	return rtos_task_check_for_time_out(&timer->xTimeOut, &timer->ms_to_wait) == TRUE;
}


void TimerInit(Timer *timer)
{
	timer->ms_to_wait = 0;
	memset(&timer->xTimeOut, '\0', sizeof(timer->xTimeOut));
}

int FreeRTOS_read(Network *n, unsigned char *buffer, int len, int timeout_ms)
{
	uint32_t ms_to_wait = timeout_ms; /* convert milliseconds to ticks */
	rtos_time_out_t xTimeOut;
	int recvLen = 0;

	rtos_task_set_time_out_state(&xTimeOut); /* Record the time at which this function was entered. */
	do {
		int rc = 0;
		struct timeval timeout;
		timeout.tv_sec  = ms_to_wait / 1000;
		timeout.tv_usec = (ms_to_wait % 1000) * 1000;
		setsockopt(n->my_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(struct timeval));
#if (MQTT_OVER_SSL)
		if (n->use_ssl) {
			rc = mbedtls_ssl_read(n->ssl, buffer + recvLen, len - recvLen);
		} else
#endif
			rc = recv(n->my_socket, buffer + recvLen, len - recvLen, 0);

		if (rc > 0) {
			recvLen += rc;
		} else if (rc < 0) {
			if (errno && (errno != EAGAIN)) {
				n->disconnect(n);
			}
			recvLen = rc;
			break;
		}
	} while (recvLen < len && rtos_task_check_for_time_out(&xTimeOut, &ms_to_wait) == FALSE);

	return recvLen;
}

int FreeRTOS_write(Network *n, unsigned char *buffer, int len, int timeout_ms)
{
	uint32_t ms_to_wait = timeout_ms; /* convert milliseconds to ticks */
	rtos_time_out_t xTimeOut;
	int sentLen = 0;

	rtos_task_set_time_out_state(&xTimeOut); /* Record the time at which this function was entered. */
	do {
		int rc = 0;
		struct timeval timeout;
		timeout.tv_sec  = ms_to_wait / 1000;
		timeout.tv_usec = (ms_to_wait % 1000) * 1000;
		setsockopt(n->my_socket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(struct timeval));
#if (MQTT_OVER_SSL)
		if (n->use_ssl) {
			rc = mbedtls_ssl_write(n->ssl, buffer + sentLen, len - sentLen);
		} else
#endif
			rc = send(n->my_socket, buffer + sentLen, len - sentLen, 0);

		if (rc > 0) {
			sentLen += rc;
		} else if (rc < 0) {
			if (errno && (errno != EAGAIN)) {
				n->disconnect(n);
			}
			sentLen = rc;
			break;
		}
	} while (sentLen < len && rtos_task_check_for_time_out(&xTimeOut, &ms_to_wait) == FALSE);

	return sentLen;
}


void FreeRTOS_disconnect(Network *n)
{
	if (n->my_socket >= 0) {
		shutdown(n->my_socket, SHUT_RDWR);
		close(n->my_socket);
		n->my_socket = -1;

#if (MQTT_OVER_SSL)
		if (n->use_ssl) {
			mbedtls_ssl_free(n->ssl);
			mbedtls_ssl_config_free(n->conf);
			free(n->ssl);
			free(n->conf);
			n->ssl = NULL;
			n->conf = NULL;
		}
#endif
	}
}


void NetworkInit(Network *n)
{
	n->my_socket = -1;
	n->mqttread = FreeRTOS_read;
	n->mqttwrite = FreeRTOS_write;
	n->disconnect = FreeRTOS_disconnect;

#if (MQTT_OVER_SSL)
	n->use_ssl = 0;
	n->ssl = NULL;
	n->conf = NULL;
	n->rootCA = NULL;
	n->clientCA = NULL;
	n->private_key = NULL;
	n->ciphersuites = NULL;
#endif
}


#if (MQTT_OVER_SSL)
static int mqtt_tls_verify(void *data, mbedtls_x509_crt *crt, int depth, int *flags)
{
	/* To avoid gcc warnings */
	(void) data;

	char buf[1024];

	mqtt_printf(MQTT_DEBUG, "\nVerify requested for (Depth %d):\n", depth);
	mbedtls_x509_crt_info(buf, sizeof(buf) - 1, "", crt);
	mqtt_printf(MQTT_DEBUG, "%s", buf);

	if (((*flags) & MBEDTLS_X509_BADCERT_EXPIRED) != 0) {
		mqtt_printf(MQTT_DEBUG, "  ! server certificate has expired\n");
	}

	if (((*flags) & MBEDTLS_X509_BADCERT_REVOKED) != 0) {
		mqtt_printf(MQTT_DEBUG, "  ! server certificate has been revoked\n");
	}

	if (((*flags) & MBEDTLS_X509_BADCERT_CN_MISMATCH) != 0) {
		mqtt_printf(MQTT_DEBUG, "  ! CN mismatch\n");
	}

	if (((*flags) & MBEDTLS_X509_BADCERT_NOT_TRUSTED) != 0) {
		mqtt_printf(MQTT_DEBUG, "  ! self-signed or not signed by a trusted CA\n");
	}

	if (((*flags) & MBEDTLS_X509_BADCRL_NOT_TRUSTED) != 0) {
		mqtt_printf(MQTT_DEBUG, "  ! CRL not trusted\n");
	}

	if (((*flags) & MBEDTLS_X509_BADCRL_EXPIRED) != 0) {
		mqtt_printf(MQTT_DEBUG, "  ! CRL expired\n");
	}

	if (((*flags) & MBEDTLS_X509_BADCERT_OTHER) != 0) {
		mqtt_printf(MQTT_DEBUG, "  ! other (unknown) flag\n");
	}

	if ((*flags) == 0) {
		mqtt_printf(MQTT_DEBUG, "  This certificate has no flags\n");
	}

	return (0);
}

static int my_random(void *p_rng, unsigned char *output, size_t output_len)
{
	/* To avoid gcc warnings */
	(void) p_rng;
	TRNG_get_random_bytes(output, output_len);
	return 0;
}
#endif // #if (MQTT_OVER_SSL)


int NetworkConnect(Network *n, char *addr, int port)
{
	struct sockaddr_in sAddr;
	int retVal = -1;
	struct hostent *hptr;
	char   **pptr;
	char   str[32];
	int	keepalive_enable = 1;
	int keep_idle = 30;

	if (n->my_socket >= 0) {
		n->disconnect(n);
	}
	if ((hptr = gethostbyname(addr)) == 0) {
		mqtt_printf(MQTT_DEBUG, "gethostbyname failed!");
		goto exit;
	}
	pptr = hptr->h_addr_list;

	for (; *pptr != NULL; pptr++) {
		inet_ntop(hptr->h_addrtype, (const ip_addr_t *)*pptr, str, sizeof(str));
	}
	inet_ntop(hptr->h_addrtype, (const ip_addr_t *)hptr->h_addr, str, sizeof(str));
	sAddr.sin_family = AF_INET;
	sAddr.sin_port = htons(port);
	sAddr.sin_addr.s_addr = inet_addr(str);
	mqtt_printf(MQTT_DEBUG, "addr = %s", str);
	if ((n->my_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		goto exit;
	}
	setsockopt(n->my_socket, SOL_SOCKET, SO_KEEPALIVE,
			   (const char *) &keepalive_enable, sizeof(keepalive_enable));
	setsockopt(n->my_socket, IPPROTO_TCP, TCP_KEEPIDLE,
			   (const char *) &keep_idle, sizeof(keep_idle));
	if ((retVal = connect(n->my_socket, (struct sockaddr *)&sAddr, sizeof(sAddr))) < 0) {
		close(n->my_socket);
		n->my_socket = -1;
		mqtt_printf(MQTT_DEBUG, "Connect failed!!");
		goto exit;
	}

#if (MQTT_OVER_SSL)
	mbedtls_x509_crt *root_crt;
	mbedtls_x509_crt *client_crt;
	mbedtls_pk_context *client_rsa;

	root_crt = NULL;
	client_crt = NULL;
	client_rsa = NULL;

	if (n->use_ssl != 0) {
		n->ssl = (mbedtls_ssl_context *) malloc(sizeof(mbedtls_ssl_context));
		n->conf = (mbedtls_ssl_config *) malloc(sizeof(mbedtls_ssl_config));
		if ((n->ssl == NULL) || (n->conf == NULL)) {
			mqtt_printf(MQTT_DEBUG, "malloc ssl failed!");
			goto err;
		}

		mbedtls_ssl_init(n->ssl);
		mbedtls_ssl_config_init(n->conf);

		if ((mbedtls_ssl_config_defaults(n->conf,
										 MBEDTLS_SSL_IS_CLIENT,
										 MBEDTLS_SSL_TRANSPORT_STREAM,
										 MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
			mqtt_printf(MQTT_DEBUG, "ssl config defaults failed!");
			goto err;
		}

		if (n->ciphersuites) {
			mbedtls_ssl_conf_ciphersuites(n->conf, n->ciphersuites);
		}


		mbedtls_ssl_set_bio(n->ssl, &n->my_socket, mbedtls_net_send, mbedtls_net_recv, NULL);
		mbedtls_ssl_conf_rng(n->conf, my_random, NULL);

		if ((mbedtls_ssl_setup(n->ssl, n->conf)) != 0) {
			mqtt_printf(MQTT_DEBUG, "mbedtls_ssl_setup failed!");
			goto err;
		}

		if (n->rootCA != NULL) {
			root_crt = (mbedtls_x509_crt *) mbedtls_calloc(sizeof(mbedtls_x509_crt), 1);
			if (root_crt == NULL) {
				mqtt_printf(MQTT_DEBUG, "malloc root_crt failed!");
				goto err;
			}

			mbedtls_x509_crt_init(root_crt);

			if (mbedtls_x509_crt_parse(root_crt, (const unsigned char *)n->rootCA, strlen(n->rootCA) + 1) != 0) {
				mqtt_printf(MQTT_DEBUG, "parse root_crt failed!");
				goto err;
			}
			mbedtls_ssl_conf_ca_chain(n->conf, root_crt, NULL);
			mbedtls_ssl_conf_authmode(n->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
			mbedtls_ssl_conf_verify(n->conf, (int (*)(void *, mbedtls_x509_crt *, int, uint32_t *))mqtt_tls_verify, NULL);
			mqtt_printf(MQTT_DEBUG, "root_crt parse done");
		} else {
			mbedtls_ssl_conf_authmode(n->conf, MBEDTLS_SSL_VERIFY_NONE);
		}

		if (n->clientCA != NULL && n->private_key != NULL) {
			client_crt = (mbedtls_x509_crt *) mbedtls_calloc(sizeof(mbedtls_x509_crt), 1);
			if (client_crt == NULL) {
				mqtt_printf(MQTT_DEBUG, "malloc client_crt failed!");
				goto err;
			}
			mbedtls_x509_crt_init(client_crt);

			client_rsa = (mbedtls_pk_context *) mbedtls_calloc(sizeof(mbedtls_pk_context), 1);
			if (client_rsa == NULL) {
				mqtt_printf(MQTT_DEBUG, "malloc client_rsa failed!");
				goto err;
			}
			mbedtls_pk_init(client_rsa);

			if (mbedtls_x509_crt_parse(client_crt, (const unsigned char *)n->clientCA, strlen(n->clientCA) + 1) != 0) {
				mqtt_printf(MQTT_DEBUG, "parse client_crt failed!");
				goto err;
			}

			if (mbedtls_pk_parse_key(client_rsa, (const unsigned char *)n->private_key, strlen(n->private_key) + 1, NULL, 0, NULL, NULL) != 0) {
				mqtt_printf(MQTT_DEBUG, "parse client_rsa failed!");
				goto err;
			}

			mbedtls_ssl_conf_own_cert(n->conf, client_crt, client_rsa);
		}

		retVal = mbedtls_ssl_handshake(n->ssl);
		if (retVal < 0) {
			mqtt_printf(MQTT_DEBUG, "ssl handshake failed err:-0x%04X", -retVal);
			goto err;
		} else {
			mqtt_printf(MQTT_DEBUG, "ssl handshake success");
		}
	}

	if (client_rsa) {
		mbedtls_pk_free(client_rsa);
		mbedtls_free(client_rsa);
	}
	if (client_crt) {
		mbedtls_x509_crt_free(client_crt);
		mbedtls_free(client_crt);
	}
	if (root_crt) {
		mbedtls_x509_crt_free(root_crt);
		mbedtls_free(root_crt);
	}
	goto exit;

err:
	if (client_rsa) {
		mbedtls_pk_free(client_rsa);
		mbedtls_free(client_rsa);
	}
	if (client_crt) {
		mbedtls_x509_crt_free(client_crt);
		mbedtls_free(client_crt);
	}
	if (root_crt) {
		mbedtls_x509_crt_free(root_crt);
		mbedtls_free(root_crt);
	}
	mbedtls_net_free((mbedtls_net_context *)&n->my_socket);
	mbedtls_ssl_free(n->ssl);
	mbedtls_ssl_config_free(n->conf);
	free(n->ssl);
	free(n->conf);
	retVal = -1;
#endif // #if (MQTT_OVER_SSL)

exit:
	return retVal;
}

#if 0
int NetworkConnectTLS(Network *n, char *addr, int port, SlSockSecureFiles_t *certificates, unsigned char sec_method, unsigned int cipher, char server_verify)
{
	SlSockAddrIn_t sAddr;
	int addrSize;
	int retVal;
	unsigned long ipAddress;

	retVal = sl_NetAppDnsGetHostByName(addr, strlen(addr), &ipAddress, AF_INET);
	if (retVal < 0) {
		return -1;
	}

	sAddr.sin_family = AF_INET;
	sAddr.sin_port = sl_Htons((unsigned short)port);
	sAddr.sin_addr.s_addr = sl_Htonl(ipAddress);

	addrSize = sizeof(SlSockAddrIn_t);

	n->my_socket = sl_Socket(SL_AF_INET, SL_SOCK_STREAM, SL_SEC_SOCKET);
	if (n->my_socket < 0) {
		return -1;
	}

	SlSockSecureMethod method;
	method.secureMethod = sec_method;
	retVal = sl_SetSockOpt(n->my_socket, SL_SOL_SOCKET, SL_SO_SECMETHOD, &method, sizeof(method));
	if (retVal < 0) {
		return retVal;
	}

	SlSockSecureMask mask;
	mask.secureMask = cipher;
	retVal = sl_SetSockOpt(n->my_socket, SL_SOL_SOCKET, SL_SO_SECURE_MASK, &mask, sizeof(mask));
	if (retVal < 0) {
		return retVal;
	}

	if (certificates != NULL) {
		retVal = sl_SetSockOpt(n->my_socket, SL_SOL_SOCKET, SL_SO_SECURE_FILES, certificates->secureFiles, sizeof(SlSockSecureFiles_t));
		if (retVal < 0) {
			return retVal;
		}
	}

	retVal = sl_Connect(n->my_socket, (SlSockAddr_t *)&sAddr, addrSize);
	if (retVal < 0) {
		if (server_verify || retVal != -453) {
			sl_Close(n->my_socket);
			return retVal;
		}
	}

	SysTickIntRegister(SysTickIntHandler);
	SysTickPeriodSet(80000);
	SysTickEnable();

	return retVal;
}
#endif
