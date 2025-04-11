#ifndef MBEDTLS_CONFIG_THREAD_H
#define MBEDTLS_CONFIG_THREAD_H

#if 1 //use sdk default mbedtls config, add openthread need

//#include "mbedtls/mbedtls_config.h" //has include in config.h
#ifndef MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED
#define MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED
#endif

#if 0
#ifndef MBEDTLS_NO_DEFAULT_ENTROPY_SOURCES
#define MBEDTLS_NO_DEFAULT_ENTROPY_SOURCES
#endif

#ifndef MBEDTLS_SHA256_SMALLER
#define MBEDTLS_SHA256_SMALLER
#endif
#endif

#ifndef MBEDTLS_SSL_DTLS_HELLO_VERIFY
#define MBEDTLS_SSL_DTLS_HELLO_VERIFY
#endif

#ifndef MBEDTLS_SSL_EXPORT_KEYS
#define MBEDTLS_SSL_EXPORT_KEYS
#endif

#else //use openthread mbedtls config

#ifndef OPENTHREAD_CONFIG_CORE_USER_CONFIG_HEADER_ENABLE
#define OPENTHREAD_CONFIG_CORE_USER_CONFIG_HEADER_ENABLE
#endif

#include "mbedtls-config.h"

#ifndef MBEDTLS_SSL_DTLS_CONNECTION_ID_COMPAT
#define MBEDTLS_SSL_DTLS_CONNECTION_ID_COMPAT 0
#endif

#ifndef MBEDTLS_SSL_DTLS_CONNECTION_ID
#define MBEDTLS_SSL_DTLS_CONNECTION_ID
#endif

#ifndef MBEDTLS_NET_C
#define MBEDTLS_NET_C
#endif

#endif

#endif