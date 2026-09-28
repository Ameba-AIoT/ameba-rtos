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
 * TLS / X.509 layer configuration only. The crypto layer is configured
 * separately through TF_PSA_CRYPTO_CONFIG_FILE (psa/crypto_config_rtk.h).
 */
#if defined(CONFIG_RLE1509)
#include "ameba.h"
#include "mbedtls/mbedtls_config_rtk_tls.h"
#elif defined(CONFIG_RTL8720F)
/* RTL8720F: temporary enable for P3 selftest validation (HAL is compatible).
 * NOT COMMITTED — see report/mbedtls4/P3_TEST_PLAN.md. */
#include "ameba.h"
#include "mbedtls/mbedtls_config_rtk_tls.h"
#elif defined(CONFIG_AMEBAGREEN2) || defined(CONFIG_AMEBAL2)
/* These SoCs are still on 3.6.5 (v_MBEDTLS_VER); porting is not validated yet.
 * Fail loudly instead of silently falling back to the upstream config. */
#error "mbedTLS 4.1.1 is only ported for RLE1509 so far"
#else
#include "mbedtls/mbedtls_config.h"
#endif

/*
 * enable the support for TLS 1.3.
 */
#if defined(CONFIG_MBEDTLS_SSL_PROTO_TLS1_3) && CONFIG_MBEDTLS_SSL_PROTO_TLS1_3
#define MBEDTLS_SSL_PROTO_TLS1_3
#else
#undef MBEDTLS_SSL_PROTO_TLS1_3
#endif

#endif
