/**
 * *****************************************************************************
 * @file    config.h
 * @author
 * @version V1.0.0
 * @date    2023-09-06
 * @brief
 * *****************************************************************************
 * @attention
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
 * *****************************************************************************
 */

#ifndef SSL_CONFIG_H
#define SSL_CONFIG_H

#include "platform_autoconf.h"
#include "platform_stdlib.h"

/*
 * we select different Mbedtls configuration files according to different target.
 */
#if defined(CONFIG_802154_THREAD_EN) && CONFIG_802154_THREAD_EN
#include "mbedtls/config_thread.h"
#else

#if defined(CONFIG_AMEBAGREEN2) || defined(CONFIG_AMEBAL2)
#include "ameba.h"
#include "mbedtls/mbedtls_config_green2.h"
#else
#include "mbedtls/mbedtls_config.h"
#endif
/*
 * enable the support for TLS 1.3.
 */
#if defined(CONFIG_MBEDTLS_SSL_PROTO_TLS1_3) && CONFIG_MBEDTLS_SSL_PROTO_TLS1_3
#define MBEDTLS_SSL_PROTO_TLS1_3
#ifndef MBEDTLS_PSA_CRYPTO_C
#define MBEDTLS_PSA_CRYPTO_C
#endif
#else
#undef MBEDTLS_SSL_PROTO_TLS1_3
#endif
#endif
#endif