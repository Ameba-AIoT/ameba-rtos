/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "erpc_server_start.h"

#include "erpc_mbf_setup.h"
#include "erpc_server_setup.h"
#include "tfm_erpc_psa_client_api_server.h"
#include "tfm_erpc_psa_connection_api_server.h"

void erpc_server_start(erpc_transport_t transport)
{
    erpc_server_init(transport, erpc_mbf_dynamic_init());
    erpc_add_service_to_server(create_psa_client_api_service());
    erpc_add_service_to_server(create_psa_connection_api_service());

    erpc_server_run();

    return;
}
