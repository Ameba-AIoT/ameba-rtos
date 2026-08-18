/**
 ******************************************************************************
 * @file    mcp_transport_http.c
 * @brief   MCP Streamable HTTP transport (stateless, single JSON-response).
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
 * Single POST endpoint, no session management, no SSE streaming.
 * Every request is processed independently:
 *   POST /mcp  →  initialize | ping | tools/list | tools/call
 *                 (notifications/initialized accepted, no response body)
 *
 * Transport stack: client → TCP:port → lwIP BSD socket → mcp_server_task →
 *                  handle_connection → mcp_protocol_handle → tool callbacks → response
 *
 * The transport owns its own TCP server socket and does not depend on the
 * httpd component, so it can coexist with user-started httpd instances on
 * different ports without any conflict.
 */

#include "mcp_server.h"
#include "mcp_protocol.h"
#include "mcp_tool_registry.h"
#include "lwip_netconf.h"
#include "os_wrapper.h"
#include "platform_stdlib.h"
#include "log.h"
#include <string.h>
#include <stdlib.h>

#define TAG "mcp_transport"

#define MCP_MAX_REQUEST_BODY  4096
#define MCP_MAX_ENDPOINT_LEN  64
#define MCP_LISTEN_BACKLOG    8
#define MCP_TASK_STACK_BYTES  4096
#define MCP_TASK_PRIORITY     1
#define MCP_HDR_BUF_SIZE      1024
#define MCP_RECV_TIMEOUT_SEC  5
#define MCP_ACCEPT_TIMEOUT_SEC 1

static struct {
	mcp_server_config_t config;
	mcp_tool_provider_t tool_provider;
	mcp_protocol_ctx_t  proto_ctx;
	char                endpoint_buf[MCP_MAX_ENDPOINT_LEN];
	rtos_mutex_t        lock;
	rtos_sema_t         task_done;
	int                 running;
	int                 server_fd;
} s = { .server_fd = -1 };

/* ── String helpers ───────────────────────────────────────────────────────── */

static int ci_char_eq(char a, char b)
{
	if (a >= 'A' && a <= 'Z') {
		a = (char)(a + 32);
	}
	if (b >= 'A' && b <= 'Z') {
		b = (char)(b + 32);
	}
	return a == b;
}

/* Returns 1 if 'line' starts with "name:" (case-insensitive), n = strlen(name). */
static int ci_field_match(const char *line, const char *name, size_t n)
{
	size_t i;

	for (i = 0; i < n; i++) {
		if (!ci_char_eq(line[i], name[i])) {
			return 0;
		}
	}
	return line[n] == ':';
}

/* Copy value of HTTP header 'name' from parsed header block into out[0..out_size-1].
 * hdr must be null-terminated and contain the full header block ending in \r\n\r\n.
 * Returns 0 on success, -1 if header not found. */
static int hdr_get(const char *hdr, const char *name, char *out, int out_size)
{
	const char *p;
	const char *v;
	size_t nlen = strlen(name);
	int i;

	/* skip the request line */
	p = strstr(hdr, "\r\n");
	if (!p) {
		return -1;
	}
	p += 2;

	/* p[0] == '\r' means we reached the empty line (end of headers) */
	while (p[0] != '\r') {
		if (ci_field_match(p, name, nlen)) {
			v = p + nlen + 1;
			while (*v == ' ') {
				v++;
			}
			i = 0;
			while (i < out_size - 1 && v[i] && v[i] != '\r' && v[i] != '\n') {
				out[i] = v[i];
				i++;
			}
			out[i] = '\0';
			return 0;
		}
		p = strstr(p, "\r\n");
		if (!p) {
			break;
		}
		p += 2;
	}
	return -1;
}

/* Parse "METHOD /path HTTP/x.x\r\n" from the first line of hdr. */
static int parse_request_line(const char *hdr,
							  char *method, int msz,
							  char *path,   int psz)
{
	const char *sp1 = strchr(hdr, ' ');
	const char *sp2;
	const char *eol;
	int mlen, plen;

	if (!sp1) {
		return -1;
	}
	mlen = (int)(sp1 - hdr);
	if (mlen <= 0 || mlen >= msz) {
		return -1;
	}
	memcpy(method, hdr, (size_t)mlen);
	method[mlen] = '\0';

	sp2 = strchr(sp1 + 1, ' ');
	eol = strstr(sp1 + 1, "\r\n");
	if (!sp2 || !eol || sp2 > eol) {
		return -1;
	}
	plen = (int)(sp2 - sp1 - 1);
	if (plen <= 0 || plen >= psz) {
		return -1;
	}
	memcpy(path, sp1 + 1, (size_t)plen);
	path[plen] = '\0';
	return 0;
}

/* ── CORS helper ──────────────────────────────────────────────────────────── */

/* Check whether origin (not null-terminated, length origin_len) appears in the
 * comma-separated allowed list. */
static int origin_in_list(const char *origin, size_t origin_len, const char *allowed)
{
	const char *p = allowed;

	while (*p) {
		const char *end;
		size_t      item_len;

		while (*p == ' ' || *p == ',') {
			p++;
		}
		if (!*p) {
			break;
		}
		end = p;
		while (*end && *end != ',') {
			end++;
		}
		item_len = (size_t)(end - p);
		while (item_len > 0 && p[item_len - 1] == ' ') {
			item_len--;
		}

		if (item_len == origin_len && strncmp(p, origin, item_len) == 0) {
			return 1;
		}
		p = end;
	}
	return 0;
}

/* Determine the Access-Control-Allow-Origin value to send back, given the
 * request's Origin header value (null-terminated, may be "") and the server's
 * allowed_origins config.
 *
 * out_origin receives a pointer to the string to use (either "*", the echoed
 * origin from req_origin_buf, or NULL meaning no CORS header needed).
 * out_vary is set to 1 when Vary: Origin must be added. */
static void resolve_cors_origin(const char *req_origin,
								const char *allowed,
								const char **out_origin,
								int        *out_vary)
{
	*out_origin = NULL;
	*out_vary   = 0;

	if (!allowed || strcmp(allowed, "*") == 0) {
		*out_origin = "*";
		return;
	}
	if (!req_origin || req_origin[0] == '\0') {
		return;
	}
	if (origin_in_list(req_origin, strlen(req_origin), allowed)) {
		*out_origin = req_origin;
		*out_vary   = 1;
	}
}

/* ── Socket I/O helpers ───────────────────────────────────────────────────── */

/* Read bytes from fd into buf, stopping when \r\n\r\n is found or buf is full.
 * buf is null-terminated on return.
 * *hdr_len is set to the byte offset just after \r\n\r\n (start of any body
 * bytes that may have been pulled into buf).
 * Returns total bytes placed in buf, or -1 on error / headers too large. */
static int recv_headers(int fd, char *buf, int buf_size, int *hdr_len)
{
	int   total = 0;
	char *sep;

	while (total < buf_size - 1) {
		int r = recv(fd, buf + total, buf_size - 1 - total, 0);

		if (r <= 0) {
			return -1;
		}
		total += r;
		buf[total] = '\0';
		sep = strstr(buf, "\r\n\r\n");
		if (sep) {
			*hdr_len = (int)(sep + 4 - buf);
			return total;
		}
	}
	return -1; /* headers exceed HDR_BUF_SIZE */
}

/* Read exactly n bytes into buf. Returns 0 on success, -1 on error. */
static int recv_exact(int fd, char *buf, int n)
{
	int total = 0;

	while (total < n) {
		int r = recv(fd, buf + total, n - total, 0);

		if (r <= 0) {
			return -1;
		}
		total += r;
	}
	return 0;
}

/* Write an HTTP response. All pointer params except status may be NULL to omit. */
static void send_response(int         fd,
						  const char *status,
						  const char *content_type,
						  const char *body,       size_t body_len,
						  const char *cors_origin, int   vary_origin,
						  const char *cors_methods,
						  const char *cors_hdrs,
						  const char *cors_max_age)
{
	char hdr[512];
	int  pos = 0;
	/* Append to hdr[], stopping silently if the buffer is full. */
#define HAPP(fmt, ...) \
	do { int _r = (int)sizeof(hdr) - pos; \
	     if (_r > 1) pos += snprintf(hdr + pos, (size_t)_r, fmt, ##__VA_ARGS__); \
	} while (0)

	HAPP("HTTP/1.1 %s\r\n", status);

	if (content_type && body_len > 0) {
		HAPP("Content-Type: %s\r\n", content_type);
	}

	HAPP("Content-Length: %u\r\n", (unsigned)body_len);
	HAPP("Connection: close\r\n");

	if (cors_origin) {
		HAPP("Access-Control-Allow-Origin: %s\r\n", cors_origin);
	}
	if (vary_origin) {
		HAPP("Vary: Origin\r\n");
	}
	if (cors_methods) {
		HAPP("Access-Control-Allow-Methods: %s\r\n", cors_methods);
	}
	if (cors_hdrs) {
		HAPP("Access-Control-Allow-Headers: %s\r\n", cors_hdrs);
	}
	if (cors_max_age) {
		HAPP("Access-Control-Max-Age: %s\r\n", cors_max_age);
	}

	HAPP("\r\n");
#undef HAPP

	(void)send(fd, hdr, (size_t)pos, 0);
	if (body && body_len > 0) {
		(void)send(fd, body, body_len, 0);
	}
}

/* ── Connection handler ───────────────────────────────────────────────────── */

static void handle_connection(int fd)
{
	char  *hdr_buf = NULL;
	char  *body    = NULL;
	int    total_read;
	int    hdr_len;
	char   method[8];
	char   path[MCP_MAX_ENDPOINT_LEN + 1];
	char   cl_str[16];
	char  *endp;
	char   ct_str[64];
	char   origin_str[128];
	int    content_len;
	int    body_prefix;
	int    is_notification;
	char  *resp_body;
	const char *cors_origin;
	int    cors_vary;
	struct timeval tv;

	/* apply receive timeout so a silent client doesn't block the server task */
	tv.tv_sec  = MCP_RECV_TIMEOUT_SEC;
	tv.tv_usec = 0;
	setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	hdr_buf = (char *)malloc(MCP_HDR_BUF_SIZE);
	if (!hdr_buf) {
		goto exit;
	}

	total_read = recv_headers(fd, hdr_buf, MCP_HDR_BUF_SIZE, &hdr_len);
	if (total_read < 0) {
		goto exit;
	}

	if (parse_request_line(hdr_buf, method, sizeof(method), path, sizeof(path)) < 0) {
		send_response(fd, "400 Bad Request", "text/plain",
					  "bad request line", 16,
					  NULL, 0, NULL, NULL, NULL);
		goto exit;
	}

	/* Route: only the configured endpoint is handled */
	if (strcmp(path, s.config.endpoint) != 0) {
		send_response(fd, "404 Not Found", "text/plain",
					  "not found", 9,
					  NULL, 0, NULL, NULL, NULL);
		goto exit;
	}

	/* Resolve CORS for all responses */
	origin_str[0] = '\0';
	hdr_get(hdr_buf, "Origin", origin_str, sizeof(origin_str));
	resolve_cors_origin(origin_str, s.config.allowed_origins, &cors_origin, &cors_vary);

	/* CORS preflight */
	if (strcmp(method, "OPTIONS") == 0) {
		send_response(fd, "204 No Content", NULL, NULL, 0,
					  cors_origin, cors_vary,
					  "POST, OPTIONS",
					  "Content-Type, Authorization",
					  "86400");
		goto exit;
	}

	/* Origin policy check for non-browser clients: reject only when Origin is
	 * present AND not in the whitelist */
	if (origin_str[0] != '\0' && !cors_origin &&
		s.config.allowed_origins && strcmp(s.config.allowed_origins, "*") != 0) {
		send_response(fd, "403 Forbidden", "text/plain",
					  "origin not allowed", 18,
					  NULL, 0, NULL, NULL, NULL);
		goto exit;
	}

	if (strcmp(method, "POST") != 0) {
		send_response(fd, "405 Method Not Allowed", "text/plain",
					  "method not allowed", 18,
					  cors_origin, cors_vary, NULL, NULL, NULL);
		goto exit;
	}

	/* Content-Type check */
	ct_str[0] = '\0';
	hdr_get(hdr_buf, "Content-Type", ct_str, sizeof(ct_str));
	if (strncmp(ct_str, "application/json", 16) != 0) {
		send_response(fd, "400 Bad Request", "text/plain",
					  "Content-Type must be application/json", 37,
					  cors_origin, cors_vary, NULL, NULL, NULL);
		goto exit;
	}

	/* Content-Length check */
	cl_str[0] = '\0';
	if (hdr_get(hdr_buf, "Content-Length", cl_str, sizeof(cl_str)) < 0) {
		send_response(fd, "411 Length Required", "text/plain",
					  "Content-Length required", 23,
					  cors_origin, cors_vary, NULL, NULL, NULL);
		goto exit;
	}
	content_len = (int)strtol(cl_str, &endp, 10);
	if (endp == cl_str || content_len <= 0) {
		send_response(fd, "400 Bad Request", "text/plain",
					  "empty body", 10,
					  cors_origin, cors_vary, NULL, NULL, NULL);
		goto exit;
	}
	if (content_len > MCP_MAX_REQUEST_BODY) {
		send_response(fd, "413 Content Too Large", "text/plain",
					  "request too large", 17,
					  cors_origin, cors_vary, NULL, NULL, NULL);
		goto exit;
	}

	/* Read body — recv_headers may have pulled some body bytes into hdr_buf */
	body = (char *)malloc((size_t)content_len + 1);
	if (!body) {
		send_response(fd, "500 Internal Server Error", NULL, NULL, 0,
					  cors_origin, cors_vary, NULL, NULL, NULL);
		goto exit;
	}

	body_prefix = total_read - hdr_len;
	if (body_prefix < 0) {
		body_prefix = 0;
	}
	if (body_prefix > content_len) {
		body_prefix = content_len; /* truncate; shouldn't happen */
	}
	if (body_prefix > 0) {
		memcpy(body, hdr_buf + hdr_len, (size_t)body_prefix);
	}
	if (body_prefix < content_len) {
		if (recv_exact(fd, body + body_prefix, content_len - body_prefix) < 0) {
			goto exit;
		}
	}
	body[content_len] = '\0';

	/* Dispatch to MCP protocol layer */
	is_notification = 0;
	resp_body = mcp_protocol_handle(body, (size_t)content_len,
									&s.proto_ctx, &is_notification);

	if (!resp_body) {
		if (is_notification) {
			send_response(fd, "202 Accepted", NULL, NULL, 0,
						  cors_origin, cors_vary, NULL, NULL, NULL);
		} else {
			send_response(fd, "500 Internal Server Error", NULL, NULL, 0,
						  cors_origin, cors_vary, NULL, NULL, NULL);
		}
	} else {
		send_response(fd, "200 OK", "application/json",
					  resp_body, strlen(resp_body),
					  cors_origin, cors_vary, NULL, NULL, NULL);
		free(resp_body);
	}

exit:
	free(body);
	free(hdr_buf);
	close(fd);
}

/* ── Server task ──────────────────────────────────────────────────────────── */

static void mcp_server_task(void *arg)
{
	struct sockaddr_in client_addr;
	socklen_t          addr_len;
	fd_set             fds;
	struct timeval     tv;
	int                client_fd;

	(void)arg;

	RTK_LOGS(TAG, RTK_LOG_INFO, "server task started on port %d\n",
			 (int)s.config.port);

	while (s.running && s.server_fd >= 0) {
		FD_ZERO(&fds);
		FD_SET(s.server_fd, &fds);
		tv.tv_sec  = MCP_ACCEPT_TIMEOUT_SEC;
		tv.tv_usec = 0;

		if (select(s.server_fd + 1, &fds, NULL, NULL, &tv) <= 0) {
			continue;
		}
		if (!FD_ISSET(s.server_fd, &fds)) {
			continue;
		}

		addr_len  = sizeof(client_addr);
		client_fd = accept(s.server_fd, (struct sockaddr *)&client_addr, &addr_len);
		if (client_fd < 0) {
			continue;
		}

		handle_connection(client_fd);
		/* handle_connection() closes client_fd */
	}

	if (s.server_fd >= 0) {
		close(s.server_fd);
		s.server_fd = -1;
	}
	RTK_LOGS(TAG, RTK_LOG_INFO, "server task stopped\n");
	rtos_sema_give(s.task_done);
	rtos_task_delete(NULL);
}

/* ── Public API ───────────────────────────────────────────────────────────── */

int mcp_server_start(const mcp_server_config_t *config)
{
	uint16_t           port;
	const char        *endpoint;
	int                enable;
	struct sockaddr_in addr;

	if (!config) {
		return -1;
	}

	if (!s.lock) {
		if (rtos_mutex_create(&s.lock) != 0) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "failed to create MCP mutex\n");
			return -1;
		}
	}
	if (!s.task_done) {
		if (rtos_sema_create_binary(&s.task_done) != 0) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "failed to create MCP sema\n");
			return -1;
		}
	}

	rtos_mutex_take(s.lock, RTOS_MAX_DELAY);
	if (s.running) {
		rtos_mutex_give(s.lock);
		RTK_LOGS(TAG, RTK_LOG_INFO, "MCP server already running\n");
		return -1;
	}

	port     = config->port     ? config->port     : MCP_SERVER_DEFAULT_PORT;
	endpoint = config->endpoint ? config->endpoint : MCP_SERVER_DEFAULT_ENDPOINT;

	s.config      = *config;
	s.config.port = port;
	strncpy(s.endpoint_buf, endpoint, MCP_MAX_ENDPOINT_LEN - 1);
	s.endpoint_buf[MCP_MAX_ENDPOINT_LEN - 1] = '\0';
	s.config.endpoint = s.endpoint_buf;

	s.tool_provider.list_tools = mcp_registry_list_tools;
	s.tool_provider.call_tool  = mcp_registry_call_tool;
	s.proto_ctx.server_name    = config->server_name ? config->server_name
								 : MCP_SERVER_DEFAULT_NAME;
	s.proto_ctx.tool_provider  = &s.tool_provider;
	s.proto_ctx.user_data      = NULL;

	/* create TCP server socket */
	s.server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (s.server_fd < 0) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "socket() failed\n");
		rtos_mutex_give(s.lock);
		return -1;
	}

	enable = 1;
	setsockopt(s.server_fd, SOL_SOCKET, SO_REUSEADDR,
			   (const char *)&enable, sizeof(enable));

	memset(&addr, 0, sizeof(addr));
	addr.sin_family      = AF_INET;
	addr.sin_port        = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(s.server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "bind() failed on port %d\n", (int)port);
		close(s.server_fd);
		s.server_fd = -1;
		rtos_mutex_give(s.lock);
		return -1;
	}

	if (listen(s.server_fd, MCP_LISTEN_BACKLOG) < 0) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "listen() failed\n");
		close(s.server_fd);
		s.server_fd = -1;
		rtos_mutex_give(s.lock);
		return -1;
	}

	s.running = 1;

	if (rtos_task_create(NULL, "mcp_server", mcp_server_task, NULL,
						 MCP_TASK_STACK_BYTES, MCP_TASK_PRIORITY) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "task create failed\n");
		close(s.server_fd);
		s.server_fd = -1;
		s.running   = 0;
		rtos_mutex_give(s.lock);
		return -1;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "MCP server started on port %d at %s\n",
			 (int)port, s.endpoint_buf);

	rtos_mutex_give(s.lock);
	return 0;
}

int mcp_server_stop(void)
{
	if (!s.lock) {
		return 0;
	}

	rtos_mutex_take(s.lock, RTOS_MAX_DELAY);
	if (!s.running) {
		rtos_mutex_give(s.lock);
		return 0;
	}
	s.running = 0;
	if (s.server_fd >= 0) {
		close(s.server_fd);
		s.server_fd = -1;
	}
	rtos_mutex_give(s.lock);

	/* wait for server task to exit before returning, so a subsequent
	 * mcp_server_start() cannot race with the old task on s.server_fd */
	rtos_sema_take(s.task_done, RTOS_MAX_DELAY);

	RTK_LOGS(TAG, RTK_LOG_INFO, "MCP server stopped\n");
	return 0;
}
