/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "erpc_mbf_setup.h"
#include "erpc_client_setup.h"
#include "erpc_client_start.h"

void erpc_client_start(erpc_transport_t transport)
{
    erpc_mbf_t message_buffer_factory;

    message_buffer_factory = erpc_mbf_dynamic_init();
    erpc_client_init(transport, message_buffer_factory);
}
