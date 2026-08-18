/**
 ******************************************************************************
 * @file    mcp_protocol.h
 * @brief   MCP protocol dispatcher, internal to the MCP component.
 ******************************************************************************
 * @attention
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2026, Realtek Semiconductor Corporation. All rights reserved.
 ******************************************************************************
 */

#ifndef _MCP_PROTOCOL_H_
#define _MCP_PROTOCOL_H_

#include <stddef.h>
#include <stdbool.h>
#include "cJSON.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Internal callback types — not part of the public API. */
typedef cJSON *(*mcp_list_tools_fn)(void *user_data);
typedef char  *(*mcp_call_tool_fn)(const char *tool_name,
								   const cJSON *params,
								   bool *is_error,
								   void *user_data);

typedef struct {
	mcp_list_tools_fn list_tools;
	mcp_call_tool_fn  call_tool;
} mcp_tool_provider_t;

typedef struct {
	const char                *server_name;
	const mcp_tool_provider_t *tool_provider;
	void                      *user_data;
} mcp_protocol_ctx_t;

/*
 * mcp_protocol_handle — stateless JSON-RPC request dispatcher.
 *
 * All methods (initialize, tools/list, tools/call, ping) are accepted without
 * any prior handshake requirement.  Returns NULL with *is_notification=1 for
 * fire-and-forget notifications (transport sends 202).  Returns NULL with
 * *is_notification=0 on OOM (transport sends 500).  Otherwise returns a
 * heap-allocated JSON string; the caller must free() it.
 */
char *mcp_protocol_handle(const char *body, size_t body_len,
						  const mcp_protocol_ctx_t *ctx,
						  int *is_notification);

#ifdef __cplusplus
}
#endif

#endif /* _MCP_PROTOCOL_H_ */
