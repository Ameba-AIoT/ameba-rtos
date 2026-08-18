/**
 ******************************************************************************
 * @file    mcp_protocol.c
 * @brief   MCP protocol dispatcher (stateless).
 ******************************************************************************
 * @attention
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2026, Realtek Semiconductor Corporation. All rights reserved.
 ******************************************************************************
 */

/*
 * Supported methods (MCP spec 2025-03-26, Streamable HTTP, no-SSE subset):
 *   initialize                  → serverInfo + capabilities
 *   notifications/initialized   → 202 (no body)
 *   ping                        → {}
 *   tools/list                  → tool descriptors from list_tools callback
 *   tools/call                  → dispatch to call_tool callback
 *
 * All methods are accepted without a prior initialize handshake (stateless).
 * Clients that send initialize before tool requests work correctly; clients
 * that skip the handshake also work correctly.
 */

#include "mcp_protocol.h"
#include "mcp_jsonrpc.h"
#include "mcp_server.h"
#include <string.h>
#include <stdlib.h>

#define MCP_PROTO_VER   "2025-03-26"
#define MCP_SERVER_VER  "1.0.0"

static cJSON *dup_id(const cJSON *id)
{
	return id ? cJSON_Duplicate(id, 1) : NULL;
}

/* Known MCP protocol versions, oldest first. */
static const char *const s_known_versions[] = {
	"2024-11-05",
	"2025-03-26",
	NULL
};

static int is_known_version(const char *v)
{
	const char *const *p;
	for (p = s_known_versions; *p; p++) {
		if (strcmp(v, *p) == 0) {
			return 1;
		}
	}
	return 0;
}

static cJSON *handle_initialize(const cJSON *params, const cJSON *id,
								const mcp_protocol_ctx_t *ctx)
{
	cJSON      *result, *info, *caps, *tools;
	cJSON      *jver;
	const char *resp_ver;

	if (!params || !cJSON_IsObject(params) ||
		!cJSON_IsString(cJSON_GetObjectItem(params, "protocolVersion"))) {
		return mcp_jsonrpc_error(dup_id(id), -32602, "protocolVersion required");
	}

	result = cJSON_CreateObject();
	info   = cJSON_CreateObject();
	caps   = cJSON_CreateObject();
	tools  = cJSON_CreateObject();

	if (!result || !info || !caps || !tools) {
		cJSON_Delete(result);
		cJSON_Delete(info);
		cJSON_Delete(caps);
		cJSON_Delete(tools);
		return mcp_jsonrpc_error(dup_id(id), -32603, "internal error");
	}

	/* Negotiate: echo the client's version if known, else fall back to ours. */
	jver     = cJSON_GetObjectItem(params, "protocolVersion");
	resp_ver = is_known_version(jver->valuestring) ? jver->valuestring : MCP_PROTO_VER;
	cJSON_AddStringToObject(result, "protocolVersion", resp_ver);
	cJSON_AddStringToObject(info, "name",
							ctx->server_name ? ctx->server_name : MCP_SERVER_DEFAULT_NAME);
	cJSON_AddStringToObject(info, "version", MCP_SERVER_VER);
	cJSON_AddItemToObject(result, "serverInfo", info);
	cJSON_AddBoolToObject(tools, "listChanged", false);
	cJSON_AddItemToObject(caps, "tools", tools);
	cJSON_AddItemToObject(result, "capabilities", caps);
	return mcp_jsonrpc_result(dup_id(id), result);
}

static cJSON *handle_tools_list(const cJSON *id, const mcp_protocol_ctx_t *ctx)
{
	cJSON *all_tools = ctx->tool_provider->list_tools(ctx->user_data);
	cJSON *result;

	if (!all_tools) {
		return mcp_jsonrpc_error(dup_id(id), -32603, "internal error");
	}
	result = cJSON_CreateObject();
	if (!result) {
		cJSON_Delete(all_tools);
		return mcp_jsonrpc_error(dup_id(id), -32603, "internal error");
	}
	cJSON_AddItemToObject(result, "tools", all_tools);
	return mcp_jsonrpc_result(dup_id(id), result);
}

static cJSON *handle_tools_call(const cJSON *params, const cJSON *id,
								const mcp_protocol_ctx_t *ctx)
{
	const char *name;
	cJSON      *jname, *jargs;
	bool        is_error = false;
	char       *result_text;
	cJSON      *content_arr, *text_item, *result;

	if (!params) {
		return mcp_jsonrpc_error(dup_id(id), -32602, "params required");
	}
	jname = cJSON_GetObjectItem(params, "name");
	if (!jname || !cJSON_IsString(jname)) {
		return mcp_jsonrpc_error(dup_id(id), -32602, "name required");
	}
	name  = jname->valuestring;
	jargs = cJSON_GetObjectItem(params, "arguments");
	if (jargs && !cJSON_IsObject(jargs)) {
		return mcp_jsonrpc_error(dup_id(id), -32602, "arguments must be object");
	}

	result_text = ctx->tool_provider->call_tool(name, jargs, &is_error,
				  ctx->user_data);
	if (!result_text) {
		return mcp_jsonrpc_error(dup_id(id), -32603, "internal error");
	}

	content_arr = cJSON_CreateArray();
	text_item   = cJSON_CreateObject();
	result      = cJSON_CreateObject();
	if (!content_arr || !text_item || !result) {
		free(result_text);
		cJSON_Delete(content_arr);
		cJSON_Delete(text_item);
		cJSON_Delete(result);
		return mcp_jsonrpc_error(dup_id(id), -32603, "internal error");
	}

	cJSON_AddStringToObject(text_item, "type", "text");
	cJSON_AddStringToObject(text_item, "text", result_text);
	free(result_text);

	cJSON_AddItemToArray(content_arr, text_item);
	cJSON_AddItemToObject(result, "content", content_arr);
	cJSON_AddBoolToObject(result, "isError", is_error ? true : false);
	return mcp_jsonrpc_result(dup_id(id), result);
}

char *mcp_protocol_handle(const char *body, size_t body_len,
						  const mcp_protocol_ctx_t *ctx,
						  int *is_notification)
{
	cJSON      *root, *jmethod, *jid, *jparams;
	const char *method;
	int         err_code = 0;
	cJSON      *resp = NULL;
	char       *out;

	*is_notification = 0;

	root = cJSON_ParseWithLength(body, body_len);
	if (!root) {
		return mcp_jsonrpc_print(
				   mcp_jsonrpc_error(NULL, -32700, "parse error"));
	}

	if (mcp_jsonrpc_validate(root, &err_code) != 0) {
		cJSON *err_resp = mcp_jsonrpc_error(
							  cJSON_Duplicate(cJSON_GetObjectItem(root, "id"), 1),
							  err_code, "invalid request");
		cJSON_Delete(root);
		return mcp_jsonrpc_print(err_resp);
	}

	jmethod = cJSON_GetObjectItem(root, "method");
	jid     = cJSON_GetObjectItem(root, "id");
	jparams = cJSON_GetObjectItem(root, "params");
	method  = jmethod->valuestring;

	/* JSON-RPC notifications have no id field — no response required. */
	if (!jid) {
		*is_notification = 1;
		cJSON_Delete(root);
		return NULL;
	}

	if (strcmp(method, "initialize") == 0) {
		resp = handle_initialize(jparams, jid, ctx);
	} else if (strcmp(method, "ping") == 0) {
		cJSON *empty = cJSON_CreateObject();
		resp = empty ? mcp_jsonrpc_result(dup_id(jid), empty)
			   : mcp_jsonrpc_error(dup_id(jid), -32603, "internal error");
	} else if (strcmp(method, "tools/list") == 0) {
		resp = handle_tools_list(jid, ctx);
	} else if (strcmp(method, "tools/call") == 0) {
		resp = handle_tools_call(jparams, jid, ctx);
	} else {
		resp = mcp_jsonrpc_error(dup_id(jid), -32601, "method not found");
	}

	cJSON_Delete(root);
	if (!resp) {
		return NULL;
	}
	out = mcp_jsonrpc_print(resp);
	return out;
}
