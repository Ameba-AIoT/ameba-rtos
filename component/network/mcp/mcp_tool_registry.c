/**
 ******************************************************************************
 * @file    mcp_tool_registry.c
 * @brief   Global tool registry: registration, schema generation, dispatch.
 ******************************************************************************
 * @attention
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2026, Realtek Semiconductor Corporation. All rights reserved.
 ******************************************************************************
 */

#include "mcp_tool_registry.h"
#include "log.h"
#include <string.h>
#include <stdlib.h>

#define TAG "mcp_registry"

/* ── Registry storage ─────────────────────────────────────────────────── */

typedef struct {
	const char             *name;
	const char             *description;
	const mcp_param_desc_t *params;       /* borrowed; caller keeps alive */
	size_t                  param_count;
	mcp_tool_handler_t      handler;
	void                   *user_data;
} registry_entry_t;

static registry_entry_t s_tools[MCP_MAX_TOOLS];
static size_t           s_tool_count;

/* ── Schema generation ────────────────────────────────────────────────── */

static const char *param_type_name(mcp_param_type_t t)
{
	switch (t) {
	case MCP_PARAM_STRING:
		return "string";
	case MCP_PARAM_INTEGER:
		return "integer";
	case MCP_PARAM_NUMBER:
		return "number";
	case MCP_PARAM_BOOLEAN:
		return "boolean";
	case MCP_PARAM_OBJECT:
		return "object";
	case MCP_PARAM_ARRAY:
		return "array";
	default:
		return "string";
	}
}

static cJSON *build_input_schema(const mcp_param_desc_t *params, size_t count)
{
	size_t i;
	cJSON *schema = cJSON_CreateObject();
	cJSON *props  = cJSON_CreateObject();
	cJSON *req    = cJSON_CreateArray();

	if (!schema || !props || !req) {
		goto oom;
	}

	if (!cJSON_AddStringToObject(schema, "type", "object")) {
		goto oom;
	}

	for (i = 0; i < count; i++) {
		cJSON *prop = cJSON_CreateObject();

		if (!prop) {
			goto oom;
		}
		if (!cJSON_AddStringToObject(prop, "type", param_type_name(params[i].type))) {
			cJSON_Delete(prop);
			goto oom;
		}
		if (params[i].description) {
			if (!cJSON_AddStringToObject(prop, "description", params[i].description)) {
				cJSON_Delete(prop);
				goto oom;
			}
		}
		cJSON_AddItemToObject(props, params[i].name, prop); /* ownership transferred */

		if (params[i].required) {
			cJSON *req_name = cJSON_CreateString(params[i].name);

			if (!req_name) {
				goto oom;
			}
			cJSON_AddItemToArray(req, req_name);
		}
	}

	cJSON_AddItemToObject(schema, "properties", props);
	props = NULL; /* schema now owns props; clear to avoid double-free in oom */
	if (!cJSON_AddFalseToObject(schema, "additionalProperties")) {
		goto oom;
	}
	if (cJSON_GetArraySize(req) > 0) {
		cJSON_AddItemToObject(schema, "required", req);
	} else {
		cJSON_Delete(req);
	}
	return schema;

oom:
	cJSON_Delete(schema);
	cJSON_Delete(props);
	cJSON_Delete(req);
	return NULL;
}

/* ── Public: mcp_server_add_tool ──────────────────────────────────────── */

int mcp_server_add_tool(const char             *name,
						const char             *description,
						const mcp_param_desc_t *params,
						size_t                  param_count,
						mcp_tool_handler_t      handler,
						void                   *user_data)
{
	registry_entry_t *e;

	size_t j;

	if (!name || !handler) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "add_tool: name and handler required\n");
		return -1;
	}
	for (j = 0; j < s_tool_count; j++) {
		if (strcmp(s_tools[j].name, name) == 0) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "add_tool: duplicate name '%s'\n", name);
			return -1;
		}
	}
	if (s_tool_count >= MCP_MAX_TOOLS) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "add_tool: registry full (max %d)\n",
				 MCP_MAX_TOOLS);
		return -1;
	}

	e               = &s_tools[s_tool_count];
	e->name         = name;
	e->description  = description ? description : "";
	e->params       = params;
	e->param_count  = param_count;
	e->handler      = handler;
	e->user_data    = user_data;
	s_tool_count++;
	return 0;
}

/* ── Internal callbacks ───────────────────────────────────────────────── */

cJSON *mcp_registry_list_tools(void *user_data)
{
	cJSON  *arr = cJSON_CreateArray();
	size_t  i;

	(void)user_data;

	if (!arr) {
		return NULL;
	}

	for (i = 0; i < s_tool_count; i++) {
		cJSON *tool   = cJSON_CreateObject();
		cJSON *schema = build_input_schema(s_tools[i].params,
										   s_tools[i].param_count);

		if (!tool || !schema) {
			cJSON_Delete(tool);
			cJSON_Delete(schema);
			cJSON_Delete(arr);
			return NULL;
		}

		if (!cJSON_AddStringToObject(tool, "name", s_tools[i].name) ||
			!cJSON_AddStringToObject(tool, "description", s_tools[i].description)) {
			cJSON_Delete(tool);
			cJSON_Delete(schema);
			cJSON_Delete(arr);
			return NULL;
		}
		cJSON_AddItemToObject(tool, "inputSchema", schema); /* schema ownership transferred */
		cJSON_AddItemToArray(arr, tool);
	}

	return arr;
}

char *mcp_registry_call_tool(const char  *tool_name,
							 const cJSON *params,
							 bool        *is_error,
							 void        *user_data)
{
	size_t i;

	(void)user_data;

	for (i = 0; i < s_tool_count; i++) {
		if (strcmp(s_tools[i].name, tool_name) == 0) {
			return s_tools[i].handler(params, is_error, s_tools[i].user_data);
		}
	}

	/* Should not be reached: mcp_protocol.c gates call on list_tools. */
	*is_error = true;
	return strdup("unknown tool");
}
