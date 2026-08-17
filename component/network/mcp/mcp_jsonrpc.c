/**
 ******************************************************************************
 * @file    mcp_jsonrpc.c
 * @brief   JSON-RPC 2.0 envelope helpers.
 ******************************************************************************
 * @attention
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2026, Realtek Semiconductor Corporation. All rights reserved.
 ******************************************************************************
 */

#include "mcp_jsonrpc.h"
#include <string.h>

cJSON *mcp_jsonrpc_result(cJSON *id, cJSON *result)
{
	cJSON *resp = cJSON_CreateObject();

	if (!resp) {
		cJSON_Delete(id);
		cJSON_Delete(result);
		return NULL;
	}
	if (!result) {
		/* Programming error: caller must not pass NULL result. Treat as OOM. */
		cJSON_Delete(id);
		cJSON_Delete(resp);
		return NULL;
	}
	cJSON_AddStringToObject(resp, "jsonrpc", "2.0");
	if (id) {
		cJSON_AddItemToObject(resp, "id", id);
	} else {
		cJSON_AddNullToObject(resp, "id");
	}
	cJSON_AddItemToObject(resp, "result", result);
	return resp;
}

cJSON *mcp_jsonrpc_error(cJSON *id, int code, const char *message)
{
	cJSON *resp  = cJSON_CreateObject();
	cJSON *error = cJSON_CreateObject();

	if (!resp || !error) {
		cJSON_Delete(resp);
		cJSON_Delete(error);
		cJSON_Delete(id);
		return NULL;
	}
	cJSON_AddStringToObject(resp, "jsonrpc", "2.0");
	if (id) {
		cJSON_AddItemToObject(resp, "id", id);
	} else {
		cJSON_AddNullToObject(resp, "id");
	}
	cJSON_AddNumberToObject(error, "code", (double)code);
	cJSON_AddStringToObject(error, "message", message ? message : "error");
	cJSON_AddItemToObject(resp, "error", error);
	return resp;
}

char *mcp_jsonrpc_print(cJSON *envelope)
{
	char *out = cJSON_PrintUnformatted(envelope);

	cJSON_Delete(envelope);
	return out;
}

int mcp_jsonrpc_validate(const cJSON *root, int *err_code)
{
	cJSON *jrpc;
	cJSON *method;

	if (!root || !cJSON_IsObject(root)) {
		if (err_code) {
			*err_code = -32600;
		}
		return -1;
	}
	jrpc = cJSON_GetObjectItem(root, "jsonrpc");
	if (!jrpc || !cJSON_IsString(jrpc) || strcmp(jrpc->valuestring, "2.0") != 0) {
		if (err_code) {
			*err_code = -32600;
		}
		return -1;
	}
	method = cJSON_GetObjectItem(root, "method");
	if (!method || !cJSON_IsString(method)) {
		if (err_code) {
			*err_code = -32600;
		}
		return -1;
	}
	/* JSON-RPC 2.0: when an id field is present it must be a string or number,
	 * never null.  A message with id:null is neither a valid request nor a
	 * notification (which omits the id field entirely). */
	{
		cJSON *id = cJSON_GetObjectItem(root, "id");
		if (id && cJSON_IsNull(id)) {
			if (err_code) {
				*err_code = -32600;
			}
			return -1;
		}
	}
	return 0;
}
