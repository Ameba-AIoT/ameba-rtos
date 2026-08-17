/**
 ******************************************************************************
 * @file    mcp_jsonrpc.h
 * @brief   JSON-RPC 2.0 envelope helpers, internal to the MCP component.
 ******************************************************************************
 * @attention
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2026, Realtek Semiconductor Corporation. All rights reserved.
 ******************************************************************************
 */

#ifndef _MCP_JSONRPC_H_
#define _MCP_JSONRPC_H_

#include <stddef.h>
#include "cJSON.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Build a JSON-RPC 2.0 result envelope.
 *
 * Takes ownership of both `id` and `result`; the returned cJSON* is a new
 * object that owns them.  Caller must cJSON_Delete the return value.
 */
cJSON *mcp_jsonrpc_result(cJSON *id, cJSON *result);

/*
 * Build a JSON-RPC 2.0 error envelope.
 *
 * Takes ownership of `id`.  Standard error codes:
 *   -32700  Parse error
 *   -32600  Invalid Request
 *   -32601  Method not found
 *   -32602  Invalid params
 *   -32603  Internal error
 */
cJSON *mcp_jsonrpc_error(cJSON *id, int code, const char *message);

/*
 * Serialize a cJSON envelope to a heap-allocated string.
 * Caller must free() the result.  Returns NULL on OOM.
 * Deletes the envelope after printing.
 */
char *mcp_jsonrpc_print(cJSON *envelope);

/*
 * Validate that `root` is a well-formed JSON-RPC 2.0 request object.
 *
 * Returns  0 if valid.
 * Returns -1 if invalid; *err_code is set to the appropriate JSON-RPC
 * error code (-32700 parse, -32600 invalid request).
 */
int mcp_jsonrpc_validate(const cJSON *root, int *err_code);

#ifdef __cplusplus
}
#endif

#endif /* _MCP_JSONRPC_H_ */
