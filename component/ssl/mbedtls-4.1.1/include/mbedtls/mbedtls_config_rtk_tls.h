/**
 * *****************************************************************************
 * @file    mbedtls_config_rtk_tls.h
 * @brief   RTK TLS / X.509 layer configuration for mbedTLS 4.x.
 * *****************************************************************************
 * @attention
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 * *****************************************************************************
 */

#ifndef MBEDTLS_CONFIG_RTK_TLS_H
#define MBEDTLS_CONFIG_RTK_TLS_H

#define MBEDTLS_CONFIG_VERSION 0x04000000

/* Key exchanges. 4.x removed all the non-(EC)DHE ones. */
#define MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
#define MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED
#define MBEDTLS_KEY_EXCHANGE_ECDHE_PSK_ENABLED
#define MBEDTLS_KEY_EXCHANGE_PSK_ENABLED

/* Protocol versions. TLS 1.3 is switched on by config.h from Kconfig. */
#define MBEDTLS_SSL_PROTO_TLS1_2
#define MBEDTLS_SSL_PROTO_DTLS
#define MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE
#define MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
#define MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED
#define MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
#define MBEDTLS_SSL_TLS1_3_TICKET_AGE_TOLERANCE          6000
#define MBEDTLS_SSL_TLS1_3_TICKET_NONCE_LENGTH           32
#define MBEDTLS_SSL_TLS1_3_DEFAULT_NEW_SESSION_TICKETS   1

/* SSL modules and features. */
#define MBEDTLS_SSL_TLS_C
#define MBEDTLS_SSL_CLI_C
#define MBEDTLS_SSL_SRV_C
#define MBEDTLS_SSL_CACHE_C
#define MBEDTLS_SSL_COOKIE_C
#define MBEDTLS_SSL_TICKET_C
#define MBEDTLS_SSL_ALL_ALERT_MESSAGES
#define MBEDTLS_SSL_ALPN
#define MBEDTLS_SSL_CONTEXT_SERIALIZATION
#define MBEDTLS_SSL_DTLS_ANTI_REPLAY
#define MBEDTLS_SSL_DTLS_CONNECTION_ID
#define MBEDTLS_SSL_DTLS_HELLO_VERIFY
#define MBEDTLS_SSL_ENCRYPT_THEN_MAC
#define MBEDTLS_SSL_EXTENDED_MASTER_SECRET
#define MBEDTLS_SSL_KEEP_PEER_CERTIFICATE
#define MBEDTLS_SSL_KEYING_MATERIAL_EXPORT
#define MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
#define MBEDTLS_SSL_RENEGOTIATION
#define MBEDTLS_SSL_SERVER_NAME_INDICATION
#define MBEDTLS_SSL_SESSION_TICKETS
#define MBEDTLS_SSL_IN_CONTENT_LEN  CONFIG_MBEDTLS_SSL_IN_CONTENT_LEN
#define MBEDTLS_SSL_OUT_CONTENT_LEN CONFIG_MBEDTLS_SSL_OUT_CONTENT_LEN

/* X.509 and PKCS#7. */
#define MBEDTLS_X509_USE_C
#define MBEDTLS_X509_CREATE_C
#define MBEDTLS_X509_CRT_PARSE_C
#define MBEDTLS_X509_CRL_PARSE_C
#define MBEDTLS_X509_CSR_PARSE_C
#define MBEDTLS_X509_CRT_WRITE_C
#define MBEDTLS_X509_CSR_WRITE_C
#define MBEDTLS_X509_RSASSA_PSS_SUPPORT
#define MBEDTLS_PKCS7_C

/* Misc TLS-layer helpers. */
#define MBEDTLS_NET_C
#define MBEDTLS_TIMING_ALT
#define MBEDTLS_DEBUG_C
#define MBEDTLS_ERROR_C
#define MBEDTLS_ERROR_STRERROR_DUMMY
#define MBEDTLS_VERSION_C
#define MBEDTLS_VERSION_FEATURES

#endif /* MBEDTLS_CONFIG_RTK_TLS_H */
