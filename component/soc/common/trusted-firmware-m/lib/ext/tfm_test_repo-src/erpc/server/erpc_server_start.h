/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __ERPC_SERVER_START_H__
#define __ERPC_SERVER_START_H__

#include "erpc_transport_setup.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief eRPC Server initialization.
 *
 * \param[in] transport         Transport to use.
 */
void erpc_server_start(erpc_transport_t transport);

#ifdef __cplusplus
}
#endif

#endif /* __ERPC_SERVER_START_H__ */
