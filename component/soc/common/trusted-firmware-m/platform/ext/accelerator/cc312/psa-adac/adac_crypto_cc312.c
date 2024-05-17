/*
 * Copyright (c) 2021-2023 Arm Limited. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "psa_adac_debug.h"
#include "adac_crypto_cc312.h"

#include "psa/crypto_types.h"

psa_status_t psa_adac_crypto_init() {
    PSA_ADAC_LOG_INFO("cc312", "CryptoCell Initialization function\r\n");
    return PSA_SUCCESS;
}

