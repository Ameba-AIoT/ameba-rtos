/**
 ******************************************************************************
 * @file    mcp_tool_registry.h
 * @brief   Global tool registry, internal to the MCP component.
 ******************************************************************************
 * @attention
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2026, Realtek Semiconductor Corporation. All rights reserved.
 ******************************************************************************
 */

#ifndef _MCP_TOOL_REGISTRY_H_
#define _MCP_TOOL_REGISTRY_H_

#include <stdbool.h>
#include "mcp_server.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * list_tools callback backed by the global registry.
 * Returns a cJSON* array owned by the caller (engine will cJSON_Delete it).
 */
cJSON *mcp_registry_list_tools(void *user_data);

/*
 * call_tool callback backed by the global registry.
 * Dispatches to the registered handler by name.
 */
char *mcp_registry_call_tool(const char *tool_name,
							 const cJSON *params,
							 bool *is_error,
							 void *user_data);

#ifdef __cplusplus
}
#endif

#endif /* _MCP_TOOL_REGISTRY_H_ */
