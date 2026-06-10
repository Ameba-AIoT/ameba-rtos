/**
 * *****************************************************************************
 * @file    ccm_alt.h
 * @author
 * @version V1.0.0
 * @date    2025-01-01
 * @brief   Mbedtls CCM Hardware alternative context
 * *****************************************************************************
 * @attention
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
 * *****************************************************************************
 */

#ifndef MBEDTLS_CCM_ALT_H
#define MBEDTLS_CCM_ALT_H

#if defined(MBEDTLS_CCM_ALT)

#include "crypto_aes_ccm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief  The CCM context-type definition for hardware acceleration.
 *         mbedtls_ccm_context is mapped to the hardware CCM context.
 */
typedef crypto_aes_ccm_context mbedtls_ccm_context;

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_CCM_ALT */

#endif /* MBEDTLS_CCM_ALT_H */
