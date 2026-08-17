/**
 ******************************************************************************
 * @file    mcp_server.h
 * @author
 * @version
 * @brief   This file provides the public API for the MCP server SDK component.
 ******************************************************************************
 * @attention
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2026, Realtek Semiconductor Corporation. All rights reserved.
 ******************************************************************************
 */

#ifndef _MCP_SERVER_H_
#define _MCP_SERVER_H_

/** @addtogroup mcp       MCP
 *  @ingroup    network
 *  @brief      Model Context Protocol (MCP) server functions
 *  @{
 */

#include <stdbool.h>
#include <stdint.h>
#include "cJSON.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ── Registration API ─────────────────────────────────────────────────────
 *
 * Use mcp_server_add_tool() to register tools before calling mcp_server_start().
 * The engine maintains an internal registry, generates inputSchema automatically,
 * and dispatches calls without any application-side strcmp or JSON building.
 */

#define MCP_MAX_TOOLS  16  /*!< Maximum tools in the global registry */

/**
 * @brief  JSON Schema primitive types for tool parameters.
 */
typedef enum {
	MCP_PARAM_STRING  = 0, /*!< JSON string  */
	MCP_PARAM_INTEGER = 1, /*!< JSON integer (whole number) */
	MCP_PARAM_NUMBER  = 2, /*!< JSON number  (float allowed) */
	MCP_PARAM_BOOLEAN = 3, /*!< JSON boolean */
	MCP_PARAM_OBJECT  = 4, /*!< JSON object  */
	MCP_PARAM_ARRAY   = 5, /*!< JSON array   */
} mcp_param_type_t;

/**
 * @brief  Describes one parameter of a registered tool.
 */
typedef struct {
	const char      *name;        /*!< Parameter name */
	const char      *description; /*!< Human-readable description (may be NULL) */
	mcp_param_type_t type;        /*!< Expected JSON type */
	bool             required;    /*!< true = required, false = optional */
} mcp_param_desc_t;

/**
 * @brief  Per-tool handler invoked when a tools/call request arrives.
 *
 * @param[in]  params    cJSON* object of validated arguments; may be NULL if
 *                       the tool has no parameters.  Do NOT cJSON_Delete it.
 * @param[out] is_error  Set to true to signal a tool-level error
 *                       (isError=true in the MCP response).
 * @param[in]  user_data Context pointer supplied at registration.
 *
 * @return Heap-allocated result text; the SDK engine free()s it.
 *         Return NULL on internal error (engine responds with -32603).
 */
typedef char *(*mcp_tool_handler_t)(const cJSON *params,
									bool        *is_error,
									void        *user_data);

/**
 * @brief  Register a tool in the global registry.
 *
 * Must be called before mcp_server_start().
 * All pointer arguments must remain valid for the entire server lifetime
 * (the engine borrows them; it does not copy strings).
 *
 * @param[in] name        Tool name (must be unique).
 * @param[in] description Tool description.
 * @param[in] params      Array of param descriptors; NULL if no parameters.
 * @param[in] param_count Number of entries in params.
 * @param[in] handler     Handler function; must not be NULL.
 * @param[in] user_data   Passed through to handler on each call.
 *
 * @return 0 on success, -1 if registry is full or arguments are invalid.
 */
int mcp_server_add_tool(const char             *name,
						const char             *description,
						const mcp_param_desc_t *params,
						size_t                  param_count,
						mcp_tool_handler_t      handler,
						void                   *user_data);


/* ── Server configuration ─────────────────────────────────────────────── */

/**
 * @brief  MCP server configuration passed to mcp_server_start().
 */
typedef struct {
	uint16_t    port;        /*!< HTTP port; default MCP_SERVER_DEFAULT_PORT */
	const char *endpoint;    /*!< URL path; default MCP_SERVER_DEFAULT_ENDPOINT */
	const char *server_name; /*!< Reported in initialize response; default MCP_SERVER_DEFAULT_NAME */

	/** TLS certificate and private key in PEM format.  Both must be non-NULL
	 *  to enable HTTPS.  The pointers MUST remain valid for the full lifetime
	 *  of the server (the engine stores the pointers, it does NOT copy them).
	 *  Not yet enforced; reserved for future HTTPS support. */
	const char *tls_cert_pem;
	const char *tls_key_pem;

	/** Origin header whitelist for DNS rebinding defence.
	 *  NULL or "*" = accept any Origin (wildcard).
	 *  Otherwise: comma-separated list of exact allowed Origin values.
	 *  Requests that carry an Origin header not in the whitelist are rejected
	 *  with 403.  Requests without an Origin header (non-browser clients) are
	 *  passed through regardless of this setting. */
	const char *allowed_origins;
} mcp_server_config_t;

#define MCP_SERVER_DEFAULT_PORT      8080        /*!< Default MCP server port */
#define MCP_SERVER_DEFAULT_ENDPOINT  "/mcp"      /*!< Default MCP endpoint path */
#define MCP_SERVER_DEFAULT_NAME      "mcp-server" /*!< Default server name in initialize response */

/* ── Lifecycle ────────────────────────────────────────────────────────── */

/**
 * @brief     Start the MCP HTTP server.
 *
 * @details   Registers the MCP endpoint with the SDK httpd component and starts
 *            httpd on config->port.  Must be called after the network is up.
 *            The httpd instance is started exclusively by this module; it must
 *            not be started elsewhere in the same firmware image.
 *
 * @note      The very first call must occur in a single-task context (e.g.
 *            the network-ready callback) because the internal lifecycle mutex
 *            is created lazily on that first call.  Subsequent concurrent
 *            calls are safe once the mutex exists.
 *
 * @param[in] config  Server configuration.  Must not be NULL.
 *                    Tools must have been registered with mcp_server_add_tool()
 *                    before this call.
 *
 * @return    0   on success.
 * @return    -1  if config is invalid, httpd is already running, or httpd fails to start.
 */
int mcp_server_start(const mcp_server_config_t *config);

/**
 * @brief  Stop the MCP server and release resources.
 *
 * @return 0 always (idempotent; safe to call when not running).
 */
int mcp_server_stop(void);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* _MCP_SERVER_H_ */
