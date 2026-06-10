/**
 * *****************************************************************************
 * @file    gcm_alt.h
 * @author
 * @version V1.0.0
 * @date    2025-01-01
 * @brief   Mbedtls GCM Hardware alternative context
 * *****************************************************************************
 * @attention
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
 * *****************************************************************************
 */

#ifndef MBEDTLS_GCM_ALT_H
#define MBEDTLS_GCM_ALT_H

#if defined(MBEDTLS_GCM_ALT)

#include "crypto_aes_gcm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief  The GCM context-type definition for hardware acceleration.
 *         mbedtls_gcm_context is mapped to the hardware GCM context.
 */
typedef crypto_aes_gcm_context mbedtls_gcm_context;

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_GCM_ALT */

#endif /* MBEDTLS_GCM_ALT_H */
