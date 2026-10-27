/**
 * \file sha3_alt.h
 * \brief SHA-3 hardware alternative (ALT) context for the LALU SHA3 engine.
 */
/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */
#ifndef MBEDTLS_SHA3_ALT_H
#define MBEDTLS_SHA3_ALT_H

#include "mbedtls/private_access.h"

#include "mbedtls/build_info.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(MBEDTLS_SHA3_ALT)
/**
 * \brief          The SHA-3 context structure of the hardware driver.
 *
 *                 Like the SHA-2 ALT, the context is the driver context itself, the Keccak state lives in the
 *                 context and the engine is only locked inside a driver call.
 */
#include "ameba.h"
typedef SHA3_context mbedtls_sha3_context;

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_SHA3_ALT */

#endif /* sha3_alt.h */
