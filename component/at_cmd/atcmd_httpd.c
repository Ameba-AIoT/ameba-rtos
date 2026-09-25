#include "platform_autoconf.h"

#ifdef CONFIG_LWIP_LAYER
#if defined(CONFIG_ATCMD_HTTPD) && (CONFIG_ATCMD_HTTPD == 1)

#include "lwip_netconf.h"
#include "atcmd_service.h"
#include "atcmd_httpd.h"
#include "httpd/httpd.h"
#include "os_wrapper.h"

#define AT_HTTPD_TAG "AT-HTTPD"

/*
 * Transparent HTTP server AT command (device passes requests to the host MCU).
 *
 * Data flow:
 *   client --HTTP--> httpd_conn_thread (per connection) --[$][HTTPD][REQ]/[BODY]--> host
 *   host --AT+HTTPDRSP--> at_httpdrsp() fills pending slot, wakes the conn thread
 *   conn thread --httpd_response_write_*--> client, then closes the connection
 *
 * The httpd server MUST run in HTTPD_THREAD_MULTIPLE mode so a handler can block
 * waiting for the host reply without stalling accept() or other connections.
 */

enum httpd_pending_state {
	HTTPD_SLOT_FREE = 0,
	HTTPD_SLOT_WAITING,     /* conn thread is blocked, waiting for host to start replying */
	HTTPD_SLOT_RESPONDING,  /* HTTPDRSPRAW is streaming the body straight to the client over TT */
	HTTPD_SLOT_READY,       /* inline HTTPDRSP stored a small body; conn thread will send it */
	HTTPD_SLOT_SENT,        /* HTTPDRSPRAW already streamed the full response; conn thread only closes */
};

struct httpd_pending {
	struct httpd_conn *conn;
	rtos_sema_t sema;             /* conn thread takes; at_httpdrsp gives */
	volatile u8 state;
	u32 seq;                      /* generation, bumped on each alloc; guards against slot reuse */
	int status;                   /* HTTP status code, e.g. 200 */
	char content_type[HTTPD_AT_CTYPE_LEN];
	u8 *body;
	u32 body_len;
};

/* Runtime config (set by AT+HTTPDCFG before AT+HTTPDSTART) */
static u8 httpd_cfg_max_conn = HTTPD_AT_DEFAULT_CONN;
static u32 httpd_cfg_stack = HTTPD_AT_DEFAULT_STACK;
static u32 httpd_cfg_timeout_ms = HTTPD_AT_DEFAULT_TIMEOUT_MS;

static u8 httpd_at_running = 0;
static struct httpd_pending g_pending[HTTPD_AT_MAX_CONN];
static rtos_mutex_t g_pending_lock = NULL;

extern httpd_handle_t httpd_handle;
extern int httpd_register_uri_handler(const httpd_uri_t *uri_handler);
extern int httpd_unregister_uri_handler(const httpd_uri_t *uri_handler);

/* -------------------------------------------------------------------------- */
/* pending table helpers                                                      */
/* -------------------------------------------------------------------------- */

/* Drain a binary semaphore so a stale give() from a previous request cannot
 * falsely wake the next taker. Must be called with the slot exclusively owned. */
static void httpd_sema_drain(rtos_sema_t sema)
{
	while (rtos_sema_take(sema, 0) == RTK_SUCCESS) {
		/* consume any pending signal */
	}
}

/* Allocate a free slot for a new request. On success returns req_id and writes
 * the slot's current generation to *seq_out. Returns -1 if the table is full. */
static int httpd_pending_alloc(struct httpd_conn *conn, u32 *seq_out)
{
	int id = -1;
	rtos_mutex_take(g_pending_lock, MUTEX_WAIT_TIMEOUT);
	for (int i = 0; i < httpd_cfg_max_conn; i++) {
		if (g_pending[i].state == HTTPD_SLOT_FREE && g_pending[i].conn == NULL) {
			httpd_sema_drain(g_pending[i].sema);
			g_pending[i].conn = conn;
			g_pending[i].state = HTTPD_SLOT_WAITING;
			g_pending[i].seq++;
			g_pending[i].status = 0;
			g_pending[i].content_type[0] = '\0';
			g_pending[i].body = NULL;
			g_pending[i].body_len = 0;
			if (seq_out) {
				*seq_out = g_pending[i].seq;
			}
			id = i;
			break;
		}
	}
	rtos_mutex_give(g_pending_lock);
	return id;
}

/* Release a slot and free any response body. Caller must NOT hold g_pending_lock. */
static void httpd_pending_free(int id)
{
	rtos_mutex_take(g_pending_lock, MUTEX_WAIT_TIMEOUT);
	if (g_pending[id].body) {
		rtos_mem_free(g_pending[id].body);
		g_pending[id].body = NULL;
	}
	g_pending[id].body_len = 0;
	g_pending[id].conn = NULL;
	g_pending[id].state = HTTPD_SLOT_FREE;
	rtos_mutex_give(g_pending_lock);
}

/* -------------------------------------------------------------------------- */
/* request -> host                                                            */
/* -------------------------------------------------------------------------- */

/* Read the request body (Content-Length framed) and stream it to the host in
 * chunks. Returns 0 on success, -1 on read/alloc error. */
static int httpd_report_body(int req_id, struct httpd_conn *conn)
{
	u32 total = (u32)conn->request.content_len;
	u8 *buf;
	u32 done = 0;

	if (total == 0) {
		return 0;
	}

	buf = (u8 *)rtos_mem_malloc(HTTPD_AT_REPORT_CHUNK);
	if (buf == NULL) {
		RTK_LOGS(AT_HTTPD_TAG, RTK_LOG_ERROR, "body buf malloc failed\r\n");
		return -1;
	}

	while (done < total) {
		u32 want = (total - done) > HTTPD_AT_REPORT_CHUNK ? HTTPD_AT_REPORT_CHUNK : (total - done);
		int r = httpd_request_read_data(conn, buf, want);
		if (r <= 0) {
			RTK_LOGS(AT_HTTPD_TAG, RTK_LOG_ERROR, "read body failed r=%d\r\n", r);
			rtos_mem_free(buf);
			return -1;
		}
		at_printf_lock();
		at_printf_indicate("[HTTPD][BODY][%d][%d]:", req_id, r);
		at_printf_data((char *)buf, (u32)r);
		at_printf("\r\n");
		at_printf_unlock();
		done += (u32)r;
	}

	rtos_mem_free(buf);
	return 0;
}

/* Push the request metadata (method, path, query, content_len) to the host.
 * The httpd parser stores request.query starting at the '?' with query_len
 * counting the '?' too; we report the query WITHOUT the leading '?' so the
 * reported <query_len> matches the bytes actually emitted after the '?'. */
static void httpd_report_request(int req_id, struct httpd_conn *conn)
{
	struct http_request *req = &conn->request;
	char *query = req->query;
	int query_len = (int)req->query_len;

	/* drop the leading '?' that the httpd parser includes in query/query_len */
	if (query != NULL && query_len > 0 && query[0] == '?') {
		query++;
		query_len--;
	}

	at_printf_lock();
	at_printf_indicate("[HTTPD][REQ][%d][%s][%d][%d][%d]:",
					   req_id,
					   httpd_request_is_method(conn, "GET") ? "GET" :
					   (httpd_request_is_method(conn, "POST") ? "POST" :
						(httpd_request_is_method(conn, "PUT") ? "PUT" : "OTHER")),
					   (int)req->path_len, query_len, (int)req->content_len);
	if (req->path_len > 0) {
		at_printf_data(req->path, (u32)req->path_len);
	}
	if (query_len > 0) {
		at_printf("?");
		at_printf_data(query, (u32)query_len);
	}
	at_printf("\r\n");
	at_printf_unlock();
}

/* -------------------------------------------------------------------------- */
/* response -> client (runs on the connection thread after host replied)      */
/* -------------------------------------------------------------------------- */

static void httpd_send_response(int req_id, struct httpd_conn *conn)
{
	struct httpd_pending *p = &g_pending[req_id];
	char status_str[8];
	char *ctype = (p->content_type[0] != '\0') ? p->content_type : "text/plain";

	DiagSnPrintf(status_str, sizeof(status_str), "%d", p->status ? p->status : 200);

	httpd_response_write_header_start(conn, status_str, ctype, p->body_len);
	httpd_response_write_header(conn, "Connection", "close");
	httpd_response_write_header_finish(conn);
	if (p->body && p->body_len > 0) {
		httpd_response_write_data(conn, p->body, p->body_len);
	}
}

/* -------------------------------------------------------------------------- */
/* catch-all method handler (runs on the per-connection httpd thread)         */
/* -------------------------------------------------------------------------- */

static int httpd_at_handler(struct httpd_conn *conn)
{
	int req_id;
	u32 seq = 0;
	u8 final_state = HTTPD_SLOT_FREE;

	req_id = httpd_pending_alloc(conn, &seq);
	if (req_id < 0) {
		httpd_response_too_many_requests(conn, NULL);
		httpd_conn_close(conn);
		return 0;
	}

	/* forward metadata + body to host */
	httpd_report_request(req_id, conn);
	if (httpd_report_body(req_id, conn) != 0) {
		httpd_pending_free(req_id);
		httpd_response_internal_server_error(conn, NULL);
		httpd_conn_close(conn);
		return 0;
	}

	/* Wait for the host to reply. The timeout only guards the "host has not even
	 * started replying" window (state still WAITING): once the host issues an
	 * HTTPDRSP(RAW) the slot leaves WAITING, and we keep waiting with no deadline
	 * while a large body streams over TT — otherwise a slow/large RSPRAW transfer
	 * would trip the timeout and drop the reply mid-flight.
	 *   READY      -> inline HTTPDRSP stored a small body; we send it, then close.
	 *   SENT       -> HTTPDRSPRAW already streamed the whole response; we only close.
	 *   RESPONDING -> RSPRAW mid-transfer; keep waiting.
	 *   WAITING    -> host silent past the deadline; time out with 500.
	 * Ownership is always resolved under the lock so a timeout can never race with
	 * a concurrent HTTPDRSP(RAW) on this slot. */
	while (1) {
		rtos_sema_take(g_pending[req_id].sema, httpd_cfg_timeout_ms);

		rtos_mutex_take(g_pending_lock, MUTEX_WAIT_TIMEOUT);
		if (g_pending[req_id].seq == seq &&
			(g_pending[req_id].state == HTTPD_SLOT_READY ||
			 g_pending[req_id].state == HTTPD_SLOT_SENT)) {
			final_state = g_pending[req_id].state;
			rtos_mutex_give(g_pending_lock);
			break;
		}
		if (g_pending[req_id].state == HTTPD_SLOT_RESPONDING && g_pending[req_id].seq == seq) {
			/* RSPRAW is streaming the body straight to the client; keep waiting */
			rtos_mutex_give(g_pending_lock);
			continue;
		}
		/* still WAITING (host silent) -> real timeout: abandon this request.
		 * Release the slot here, inside the lock, so a later HTTPDRSP sees it is
		 * gone. Any body is ours to drop: the host never delivered one, and a
		 * concurrent HTTPDRSP could only have attached one by first taking this
		 * slot out of WAITING. */
		if (g_pending[req_id].body) {
			rtos_mem_free(g_pending[req_id].body);
			g_pending[req_id].body = NULL;
		}
		g_pending[req_id].body_len = 0;
		g_pending[req_id].conn = NULL;
		g_pending[req_id].state = HTTPD_SLOT_FREE;
		rtos_mutex_give(g_pending_lock);

		/* The slot is already free and may be handed to another request at any
		 * moment, so do NOT fall through to httpd_pending_free(req_id) below: it
		 * indexes by req_id without re-checking seq and would clobber whichever
		 * request owns the slot by then. Finish this connection and return. */
		RTK_LOGS(AT_HTTPD_TAG, RTK_LOG_WARN, "req %d timeout\r\n", req_id);
		at_printf_indicate("[HTTPD][TIMEOUT][%d]\r\n", req_id);
		httpd_response_internal_server_error(conn, NULL);
		httpd_conn_close(conn);
		return 0;
	}

	/* Reaching here means the slot is still ours in READY or SENT state (never
	 * FREE), so httpd_pending_alloc cannot have reassigned req_id and freeing it
	 * by index is safe. */
	if (final_state == HTTPD_SLOT_READY) {
		/* inline HTTPDRSP filled the slot for us */
		httpd_send_response(req_id, conn);
	}
	/* else HTTPD_SLOT_SENT: HTTPDRSPRAW already streamed the whole response */

	httpd_pending_free(req_id);
	httpd_conn_close(conn);
	return 0;
}

static const httpd_uri_t httpd_at_get = { .method = HTTP_GET,  .handler = httpd_at_handler };
static const httpd_uri_t httpd_at_post = { .method = HTTP_POST, .handler = httpd_at_handler };
static const httpd_uri_t httpd_at_put = { .method = HTTP_PUT,  .handler = httpd_at_handler };

/* -------------------------------------------------------------------------- */
/* AT commands                                                                */
/* -------------------------------------------------------------------------- */

/* AT+HTTPDCFG=<max_conn>[,<stack>][,<timeout_ms>] */
void at_httpdcfg(u16 argc, char **argv)
{
	int error_no = 0;
	int v;

	if (httpd_at_running) {
		RTK_LOGS(AT_HTTPD_TAG, RTK_LOG_ERROR, "server running, stop before cfg\r\n");
		error_no = 2;
		goto end;
	}

	if (argc < 2 || argv[1] == NULL || strlen(argv[1]) == 0) {
		RTK_LOGS(AT_HTTPD_TAG, RTK_LOG_INFO, "AT+HTTPDCFG=<max_conn>[,<stack>][,<timeout_ms>]\r\n");
		error_no = 1;
		goto end;
	}

	v = atoi(argv[1]);
	if (v < 1 || v > HTTPD_AT_MAX_CONN) {
		RTK_LOGS(AT_HTTPD_TAG, RTK_LOG_ERROR, "max_conn must be 1~%d\r\n", HTTPD_AT_MAX_CONN);
		error_no = 1;
		goto end;
	}
	httpd_cfg_max_conn = (u8)v;

	if (argc > 2 && argv[2] != NULL && strlen(argv[2]) > 0) {
		v = atoi(argv[2]);
		if (v < 2048) {
			RTK_LOGS(AT_HTTPD_TAG, RTK_LOG_ERROR, "stack must be >= 2048\r\n");
			error_no = 1;
			goto end;
		}
		httpd_cfg_stack = (u32)v;
	}

	if (argc > 3 && argv[3] != NULL && strlen(argv[3]) > 0) {
		v = atoi(argv[3]);
		if (v < 1 || v > HTTPD_AT_MAX_TIMEOUT_MS) {
			RTK_LOGS(AT_HTTPD_TAG, RTK_LOG_ERROR, "timeout must be 1~%d\r\n", HTTPD_AT_MAX_TIMEOUT_MS);
			error_no = 1;
			goto end;
		}
		httpd_cfg_timeout_ms = (u32)v;
	}

end:
	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
}

/* Read one certificate (cert_type, cert_index) from flash into a freshly
 * malloc'd, null-terminated buffer. Returns the buffer (caller frees) or NULL. */
static char *httpd_load_cert(CERT_TYPE cert_type, int cert_index)
{
	int size = atcmd_get_ssl_certificate_size(cert_type, cert_index);
	char *buf;

	if (size <= 0) {
		RTK_LOGS(AT_HTTPD_TAG, RTK_LOG_ERROR, "cert type %d idx %d size=%d\r\n", (int)cert_type, cert_index, size);
		return NULL;
	}
	buf = (char *)rtos_mem_malloc(size + 1);
	if (buf == NULL) {
		return NULL;
	}
	if (atcmd_get_ssl_certificate(buf, cert_type, cert_index) <= 0) {
		RTK_LOGS(AT_HTTPD_TAG, RTK_LOG_ERROR, "read cert type %d idx %d failed\r\n", (int)cert_type, cert_index);
		rtos_mem_free(buf);
		return NULL;
	}
	return buf;
}

/* Load server cert/key/CA for conn_type and hand them to the httpd TLS layer.
 * All three are loaded for every TLS tier: the httpd TLS layer parses ca_certs
 * unconditionally (httpd_tls.c), so a NULL CA would crash even when not verifying
 * the client — the websocket server loads all three for the same reason.
 * Returns 0 and sets *secure_out on success, else error_no. */
static int httpd_setup_tls(int conn_type, int cert_index, u8 *secure_out)
{
	char *cert = NULL, *key = NULL, *ca = NULL;
	int error_no = 0;

	cert = httpd_load_cert(SERVER_CERT, cert_index);
	key = httpd_load_cert(SERVER_KEY, cert_index);
	ca = httpd_load_cert(SERVER_CA, cert_index);
	if (cert == NULL || key == NULL || ca == NULL) {
		error_no = 3;
		goto out;
	}

	*secure_out = (conn_type == HTTPD_AT_OVER_TLS_VERIFY_CLIENT) ?
				  HTTPD_SECURE_TLS_VERIFY : HTTPD_SECURE_TLS;

	if (httpd_setup_cert(cert, key, ca) != 0) {
		RTK_LOGS(AT_HTTPD_TAG, RTK_LOG_ERROR, "httpd_setup_cert failed\r\n");
		error_no = 5;
		goto out;
	}

out:
	/* httpd_setup_cert copies the PEM strings, so free our local copies either way */
	if (cert) {
		rtos_mem_free(cert);
	}
	if (key) {
		rtos_mem_free(key);
	}
	if (ca) {
		rtos_mem_free(ca);
	}
	return error_no;
}

/* Free the httpd handle we allocated in at_httpdstart. The httpd library never
 * frees httpd_handle / hd_calls itself (it only frees the per-method slots via
 * httpd_unregister_uri_handler and httpd_connections in httpd_deinit), so on a
 * start/stop cycle these two blocks leak unless we free them here.
 * Call only after the server thread has fully exited (httpd_is_running()==0);
 * the running server dereferences httpd_handle on every accept. */
static void httpd_free_handle(void)
{
	struct httpd_data *hd = (struct httpd_data *)httpd_handle;

	if (hd == NULL) {
		return;
	}
	if (hd->hd_calls) {
		rtos_mem_free(hd->hd_calls);
		hd->hd_calls = NULL;
	}
	rtos_mem_free(hd);
	httpd_handle = NULL;
}

/* AT+HTTPDSTART=<port>,<conn_type>[,<cert_index>]
 *   <conn_type>: 1=TCP(HTTP), 2=TLS(HTTPS), 3=TLS+verify client
 *   <cert_index>: certificate set index in flash, required for conn_type 2/3 */
void at_httpdstart(u16 argc, char **argv)
{
	int error_no = 0;
	int port;
	int conn_type = HTTPD_AT_OVER_TCP;
	int cert_index = 0;
	u8 secure = HTTPD_SECURE_NONE;
	struct httpd_data *hd = NULL;

	if (argc < 3 || argv[1] == NULL || strlen(argv[1]) == 0 || argv[2] == NULL || strlen(argv[2]) == 0) {
		RTK_LOGS(AT_HTTPD_TAG, RTK_LOG_INFO, "AT+HTTPDSTART=<port>,<conn_type>[,<cert_index>]\r\n");
		error_no = 1;
		goto end;
	}

	if (httpd_at_running) {
		RTK_LOGS(AT_HTTPD_TAG, RTK_LOG_ERROR, "server already running\r\n");
		error_no = 2;
		goto end;
	}

	port = atoi(argv[1]);
	if (port < 1 || port > 65535) {
		RTK_LOGS(AT_HTTPD_TAG, RTK_LOG_ERROR, "port must be 1~65535\r\n");
		error_no = 1;
		goto end;
	}

	conn_type = atoi(argv[2]);
	if (conn_type < HTTPD_AT_OVER_TCP || conn_type > HTTPD_AT_OVER_TLS_VERIFY_CLIENT) {
		RTK_LOGS(AT_HTTPD_TAG, RTK_LOG_ERROR, "conn_type must be %d~%d\r\n", HTTPD_AT_OVER_TCP, HTTPD_AT_OVER_TLS_VERIFY_CLIENT);
		error_no = 1;
		goto end;
	}

	/* HTTPS tiers need a cert set index and must load certs before start */
	if (conn_type != HTTPD_AT_OVER_TCP) {
		if (argc < 4 || argv[3] == NULL || strlen(argv[3]) == 0) {
			RTK_LOGS(AT_HTTPD_TAG, RTK_LOG_ERROR, "cert_index required for HTTPS\r\n");
			error_no = 1;
			goto end;
		}
		cert_index = atoi(argv[3]);
		if (cert_index <= 0) {
			RTK_LOGS(AT_HTTPD_TAG, RTK_LOG_ERROR, "cert_index must be > 0\r\n");
			error_no = 1;
			goto end;
		}
		error_no = httpd_setup_tls(conn_type, cert_index, &secure);
		if (error_no != 0) {
			goto end;
		}
	}

	/* allocate httpd handle + uri handler slots (3 methods) */
	hd = (struct httpd_data *)rtos_mem_zmalloc(sizeof(struct httpd_data));
	if (hd == NULL) {
		error_no = 3;
		goto end;
	}
	hd->config.max_uri_handlers = 3;
	hd->hd_calls = (httpd_uri_t **)rtos_mem_zmalloc(hd->config.max_uri_handlers * sizeof(httpd_uri_t *));
	if (hd->hd_calls == NULL) {
		rtos_mem_free(hd);
		error_no = 3;
		goto end;
	}
	httpd_handle = (httpd_handle_t)hd;

	httpd_register_uri_handler(&httpd_at_get);
	httpd_register_uri_handler(&httpd_at_post);
	httpd_register_uri_handler(&httpd_at_put);

	/* Shorten the server-thread select() period so STOP releases the listen
	 * socket quickly (see HTTPD_AT_IDLE_TIMEOUT_S). Must be set before start. */
	httpd_setup_idle_timeout(HTTPD_AT_IDLE_TIMEOUT_S);

	if (httpd_start_with_callback((u16)port, httpd_cfg_max_conn, httpd_cfg_stack,
								  HTTPD_THREAD_MULTIPLE, secure) != 0) {
		RTK_LOGS(AT_HTTPD_TAG, RTK_LOG_ERROR, "httpd_start failed\r\n");
		httpd_unregister_uri_handler(&httpd_at_get);
		httpd_unregister_uri_handler(&httpd_at_post);
		httpd_unregister_uri_handler(&httpd_at_put);
		/* start failed => server thread not running, safe to free the handle now */
		httpd_free_handle();
		error_no = 4;
		goto end;
	}

	httpd_at_running = 1;
	at_printf("[HTTPD][EVENT]:start\r\n");

end:
	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
}

/* AT+HTTPDSTOP */
void at_httpdstop(u16 argc, char **argv)
{
	UNUSED(argc);
	UNUSED(argv);

	if (!httpd_at_running) {
		at_printf(ATCMD_OK_END_STR);
		return;
	}

	/* httpd_stop() is asynchronous (only sets a flag); the per-connection threads
	 * stay valid, so waking a blocked handler lets it bail out and close its own
	 * conn. Each woken handler resolves ownership under the lock (state still
	 * WAITING with matching seq -> it wins, returns 500, closes conn). */
	httpd_stop();
	httpd_unregister_uri_handler(&httpd_at_get);
	httpd_unregister_uri_handler(&httpd_at_post);
	httpd_unregister_uri_handler(&httpd_at_put);

	/* wake any waiting connection threads so they bail out */
	rtos_mutex_take(g_pending_lock, MUTEX_WAIT_TIMEOUT);
	for (int i = 0; i < HTTPD_AT_MAX_CONN; i++) {
		if (g_pending[i].state == HTTPD_SLOT_WAITING) {
			rtos_sema_give(g_pending[i].sema);
		}
	}
	rtos_mutex_give(g_pending_lock);

	/* httpd_stop() only sets a flag; the server thread closes the listen socket
	 * lazily on its next select() wakeup (up to the idle timeout). Wait for it to
	 * actually exit here, otherwise a quick STOP->START races the still-open
	 * listen socket and fails with "ERROR: listen" (EADDRINUSE). */
	{
		u32 waited = 0;
		while (httpd_is_running() && waited < HTTPD_AT_STOP_WAIT_MS) {
			rtos_time_delay_ms(HTTPD_AT_STOP_POLL_MS);
			waited += HTTPD_AT_STOP_POLL_MS;
		}
		if (httpd_is_running()) {
			/* Thread did not exit in time: do NOT free the handle, it still
			 * dereferences httpd_handle on accept. Leak one handle over
			 * losing the box to a use-after-free. */
			RTK_LOGS(AT_HTTPD_TAG, RTK_LOG_WARN, "server thread still running after %d ms\r\n", (int)waited);
		} else {
			/* Server thread is gone; free the handle the library never frees. */
			httpd_free_handle();
		}
	}

	httpd_at_running = 0;
	at_printf("[HTTPD][EVENT]:stop\r\n");
	at_printf(ATCMD_OK_END_STR);
}

/* Validate the reply header params common to HTTPDRSP / HTTPDRSPRAW and snapshot
 * the target slot's generation. On success fills the out params (req_id, seq,
 * status, body_len) plus the caller-provided ctype buffer of HTTPD_AT_CTYPE_LEN
 * bytes, and returns 0. Returns an error_no (>0) otherwise. */
static int httpdrsp_parse_header(u16 argc, char **argv, int *req_id_out, u32 *seq_out,
								 int *status_out, char *ctype, int *body_len_out)
{
	int req_id, status, body_len;
	struct httpd_pending *p;

	if (argc < 5) {
		return 1;
	}

	req_id = atoi(argv[1]);
	if (req_id < 0 || req_id >= HTTPD_AT_MAX_CONN) {
		RTK_LOGS(AT_HTTPD_TAG, RTK_LOG_ERROR, "req_id out of range\r\n");
		return 1;
	}
	p = &g_pending[req_id];

	status = atoi(argv[2]);
	if (status < 100 || status > 599) {
		RTK_LOGS(AT_HTTPD_TAG, RTK_LOG_ERROR, "bad status\r\n");
		return 1;
	}

	body_len = atoi(argv[4]);
	if (body_len < 0) {
		return 1;
	}

	/* Snapshot the generation under the lock; the request may already have timed
	 * out (state != WAITING), in which case there is nothing to reply to. */
	rtos_mutex_take(g_pending_lock, MUTEX_WAIT_TIMEOUT);
	if (p->state != HTTPD_SLOT_WAITING || p->conn == NULL) {
		rtos_mutex_give(g_pending_lock);
		RTK_LOGS(AT_HTTPD_TAG, RTK_LOG_ERROR, "req_id %d not waiting\r\n", req_id);
		return 2;
	}
	*seq_out = p->seq;
	rtos_mutex_give(g_pending_lock);

	strncpy(ctype, argv[3], HTTPD_AT_CTYPE_LEN - 1);
	ctype[HTTPD_AT_CTYPE_LEN - 1] = '\0';
	*req_id_out = req_id;
	*status_out = status;
	*body_len_out = body_len;
	return 0;
}

/* Claim a slot for a streaming (HTTPDRSPRAW) reply: flip WAITING -> RESPONDING
 * and hand back the connection so this thread can write straight to the client.
 * While RESPONDING the connection thread never touches conn (it only waits), so
 * the pointer is safe to use here until we finish. Returns 0 and sets *conn_out
 * on success, else error_no 2 (request already timed out). */
static int httpdrsp_begin(int req_id, u32 seq, struct httpd_conn **conn_out)
{
	struct httpd_pending *p = &g_pending[req_id];
	int error_no = 0;

	rtos_mutex_take(g_pending_lock, MUTEX_WAIT_TIMEOUT);
	if (p->state == HTTPD_SLOT_WAITING && p->seq == seq && p->conn != NULL) {
		p->state = HTTPD_SLOT_RESPONDING;
		*conn_out = p->conn;
	} else {
		error_no = 2;                       /* request already gone (timed out) */
	}
	rtos_mutex_give(g_pending_lock);
	return error_no;
}

/* Finish a streaming reply: flip RESPONDING -> SENT and wake the connection
 * thread, which then only closes the connection (the response is already on the
 * wire, in full or partially on error). Called after the TT transfer completes. */
static void httpdrsp_finish_sent(int req_id, u32 seq)
{
	struct httpd_pending *p = &g_pending[req_id];

	rtos_mutex_take(g_pending_lock, MUTEX_WAIT_TIMEOUT);
	if (p->state == HTTPD_SLOT_RESPONDING && p->seq == seq) {
		p->state = HTTPD_SLOT_SENT;
		rtos_sema_give(p->sema);
	}
	rtos_mutex_give(g_pending_lock);
}

/* Commit an inline (HTTPDRSP) reply to the slot and wake the connection thread.
 * Only hands off if the slot is still WAITING for this request (seq match);
 * otherwise the request timed out and *body is freed here. Takes ownership of *body.
 * Returns 0 on success, or error_no 2 if the request is gone. */
static int httpdrsp_commit(int req_id, u32 seq, int status, const char *ctype,
						   u8 *body, int body_len)
{
	struct httpd_pending *p = &g_pending[req_id];
	int error_no = 0;

	rtos_mutex_take(g_pending_lock, MUTEX_WAIT_TIMEOUT);
	if (p->state == HTTPD_SLOT_WAITING && p->seq == seq && p->conn != NULL) {
		p->status = status;
		strncpy(p->content_type, ctype, sizeof(p->content_type) - 1);
		p->content_type[sizeof(p->content_type) - 1] = '\0';
		p->body = body;
		p->body_len = (u32)body_len;
		p->state = HTTPD_SLOT_READY;
		body = NULL;                    /* ownership transferred to the slot */
		rtos_sema_give(p->sema);
	} else {
		error_no = 2;                   /* request gone (timed out); reply discarded */
	}
	rtos_mutex_give(g_pending_lock);

	if (body) {
		rtos_mem_free(body);
	}
	return error_no;
}

/* AT+HTTPDRSP=<req_id>,<status>,<content_type>,<body_len>[,<body>]
 * Inline reply: the body (if any) is carried on the command line. For small
 * responses only; the body must not contain ',' or CR/LF. Mirrors AT+SKTSEND.
 * Use AT+HTTPDRSPRAW for large or binary bodies. */
void at_httpdrsp(u16 argc, char **argv)
{
	int error_no = 0;
	int req_id = 0, status = 0, body_len = 0;
	u32 seq = 0;
	u8 *body = NULL;
	char ctype[HTTPD_AT_CTYPE_LEN];

	error_no = httpdrsp_parse_header(argc, argv, &req_id, &seq, &status, ctype, &body_len);
	if (error_no != 0) {
		if (error_no == 1) {
			RTK_LOGS(AT_HTTPD_TAG, RTK_LOG_INFO,
					 "AT+HTTPDRSP=<req_id>,<status>,<content_type>,<body_len>[,<body>]\r\n");
		}
		goto end;
	}

	if (body_len > 0) {
		if (argc < 6 || argv[5] == NULL || (int)strlen(argv[5]) < body_len) {
			RTK_LOGS(AT_HTTPD_TAG, RTK_LOG_ERROR, "inline body shorter than body_len\r\n");
			error_no = 1;
			goto end;
		}
		body = (u8 *)rtos_mem_zmalloc((u32)body_len + 1);
		if (body == NULL) {
			error_no = 3;
			goto end;
		}
		_memcpy(body, argv[5], (u32)body_len);
	}

	error_no = httpdrsp_commit(req_id, seq, status, ctype, body, body_len);
	body = NULL;                        /* commit took ownership (freed it on failure) */

end:
	if (body) {
		rtos_mem_free(body);
	}
	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
}

/* AT+HTTPDRSPRAW=<req_id>,<status>,<content_type>,<body_len>
 * Transparent reply: device emits ">>>" and the host streams body_len raw bytes
 * over TT mode. For large or binary bodies. Mirrors AT+SKTSENDRAW. */
void at_httpdrspraw(u16 argc, char **argv)
{
	int error_no = 0;
	int req_id = 0, status = 0, body_len = 0;
	u32 seq = 0;
	u8 *chunk = NULL;               /* fixed-size relay buffer; NOT the whole body */
	char ctype[HTTPD_AT_CTYPE_LEN];
	char status_str[8];
	struct httpd_conn *conn = NULL;
	int responding = 0;             /* slot is RESPONDING; must abort/finish on the way out */
	int tt_started = 0;             /* TT ring active; must drain+end before returning */
	int write_failed = 0;
	u32 done = 0;

	error_no = httpdrsp_parse_header(argc, argv, &req_id, &seq, &status, ctype, &body_len);
	if (error_no != 0) {
		if (error_no == 1) {
			RTK_LOGS(AT_HTTPD_TAG, RTK_LOG_INFO,
					 "AT+HTTPDRSPRAW=<req_id>,<status>,<content_type>,<body_len>\r\n");
		}
		goto end;
	}
	if (body_len == 0) {
		RTK_LOGS(AT_HTTPD_TAG, RTK_LOG_ERROR, "body_len must be > 0 for RAW; use HTTPDRSP\r\n");
		error_no = 1;
		goto end;
	}

	/* One fixed relay buffer, independent of body_len — this is what lets a body
	 * of tens of MB stream through a few KB of heap. */
	chunk = (u8 *)rtos_mem_malloc(MAX_TT_BUF_LEN);
	if (chunk == NULL) {
		error_no = 3;
		goto end;
	}

	/* Claim the slot for streaming and grab the connection. While RESPONDING the
	 * connection thread only waits, so conn is ours to write until finish. */
	error_no = httpdrsp_begin(req_id, seq, &conn);
	if (error_no != 0) {
		goto end;
	}
	responding = 1;

	/* Write the response header now (Content-Length is known from body_len). */
	DiagSnPrintf(status_str, sizeof(status_str), "%d", status);
	httpd_response_write_header_start(conn, status_str,
									  (ctype[0] != '\0') ? ctype : "application/octet-stream",
									  (size_t)body_len);
	httpd_response_write_header(conn, "Connection", "close");
	if (httpd_response_write_header_finish(conn) < 0) {
		/* header failed to reach the client; still must drain TT below */
		write_failed = 1;
	}

	/* TT mode: device prints ">>>", host streams body_len bytes. We relay each
	 * chunk straight to the client. On a client write error we stop writing but
	 * keep draining TT to completion — the device declared body_len to the host,
	 * so the ring must be fully consumed before atcmd_tt_mode_end(), or the next
	 * TT command inherits leftover bytes. */
	if (atcmd_tt_mode_start((u32)body_len) < 0) {
		RTK_LOGS(AT_HTTPD_TAG, RTK_LOG_ERROR, "tt_mode_start failed\r\n");
		error_no = 4;
		goto end;
	}
	tt_started = 1;

	while (done < (u32)body_len) {
		u32 want = ((u32)body_len - done) > MAX_TT_BUF_LEN ? MAX_TT_BUF_LEN : ((u32)body_len - done);
		int got = atcmd_tt_mode_get(chunk, want);
		if (got <= 0) {
			RTK_LOGS(AT_HTTPD_TAG, RTK_LOG_ERROR, "tt_mode_get failed at %u/%u\r\n", (unsigned int)done, (unsigned int)body_len);
			error_no = 4;
			break;                  /* TT itself broke; cannot recover the stream */
		}
		if (!write_failed) {
			if (httpd_response_write_data(conn, chunk, (size_t)got) < 0) {
				RTK_LOGS(AT_HTTPD_TAG, RTK_LOG_WARN, "client write failed at %u; draining rest\r\n", (unsigned int)done);
				write_failed = 1;   /* keep draining, stop sending */
			}
		}
		done += (u32)got;
	}

end:
	if (tt_started) {
		atcmd_tt_mode_end();
	}
	if (responding) {
		/* The response header (200 + Content-Length) went out right after begin(),
		 * so we can never cleanly fall back to a 500 here — that would layer an
		 * error status on top of an already-sent success line. Whatever happened to
		 * the body (complete, client-dropped, or TT-broken mid-stream), the only
		 * correct move is to hand the slot to SENT so the conn thread just closes;
		 * a short/truncated body vs the declared Content-Length is the client's cue
		 * that the response failed. */
		httpdrsp_finish_sent(req_id, seq);
	}
	if (chunk) {
		rtos_mem_free(chunk);
	}
	if (error_no == 0 && !write_failed) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no ? error_no : 2);
	}
}

/* AT+HTTPDQUERY */
void at_httpdquery(u16 argc, char **argv)
{
	UNUSED(argc);
	UNUSED(argv);

	at_printf("running: %d\r\n", httpd_at_running);
	at_printf("max_conn: %d\r\n", httpd_cfg_max_conn);
	at_printf("stack: %d\r\n", (int)httpd_cfg_stack);
	at_printf("timeout_ms: %d\r\n", (int)httpd_cfg_timeout_ms);
	for (int i = 0; i < httpd_cfg_max_conn; i++) {
		if (g_pending[i].state != HTTPD_SLOT_FREE) {
			at_printf("req_id %d: state %d\r\n", i, g_pending[i].state);
		}
	}
	at_printf(ATCMD_OK_END_STR);
}

/* -------------------------------------------------------------------------- */
/* registration                                                               */
/* -------------------------------------------------------------------------- */

ATCMD_APONLY_TABLE_DATA_SECTION
const log_item_t at_httpd_items[] = {
	{"+HTTPDCFG", at_httpdcfg},
	{"+HTTPDSTART", at_httpdstart},
	{"+HTTPDSTOP", at_httpdstop},
	{"+HTTPDRSP", at_httpdrsp},
	{"+HTTPDRSPRAW", at_httpdrspraw},
	{"+HTTPDQUERY", at_httpdquery},
};

void at_httpd_init(void)
{
	if (g_pending_lock == NULL) {
		rtos_mutex_create(&g_pending_lock);
	}
	for (int i = 0; i < HTTPD_AT_MAX_CONN; i++) {
		g_pending[i].state = HTTPD_SLOT_FREE;
		g_pending[i].conn = NULL;
		if (g_pending[i].sema == NULL) {
			rtos_sema_create_binary(&g_pending[i].sema);
		}
	}
}

#endif /* CONFIG_ATCMD_HTTPD */
#endif /* CONFIG_LWIP_LAYER */
