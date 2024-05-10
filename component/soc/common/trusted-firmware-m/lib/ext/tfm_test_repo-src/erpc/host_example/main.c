/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "erpc_port.h"
#include "erpc_client_start.h"
#include "tfm_erpc_psa_client_api.h"
#include "tfm_crypto_defs.h"
#include "psa/client.h"
#include "psa/crypto.h"

static void test_call(void)
{
    psa_status_t status;
    uint8_t hash[PSA_HASH_LENGTH(PSA_ALG_SHA_256)] = {0};
    size_t hash_size = sizeof(hash);
    const uint8_t *msg = "test";

    status = psa_hash_compute(PSA_ALG_SHA_256, msg, strlen(msg), hash, hash_size, &hash_size);

    printf("psa_hash_compute: %d\r\n", status);
    printf("> hash_size = %zu\r\n", hash_size);
    printf("> hash = ");
    for (size_t i = 0; i < sizeof(hash); ++i) {
        printf("%02hhX", hash[i]);
    }
    printf("\r\n");
}

int main(int argc, char *argv[])
{
    erpc_transport_t transport;

#ifdef ERPC_TRANSPORT_UART
    transport = erpc_transport_serial_init(PORT_NAME, 115200);
#elif defined(ERPC_TRANSPORT_TCP)
    transport = erpc_transport_tcp_init(ERPC_HOST, ERPC_PORT, false);
#else
#error "No valid transportation layer selected."
#endif

    if (!transport) {
        printf("eRPC transport init failed!\r\n");
        return 1;
    }

    erpc_client_start(transport);

    printf("psa_framework_version: %d\r\n", psa_framework_version());

    test_call();

    return 0;
}
