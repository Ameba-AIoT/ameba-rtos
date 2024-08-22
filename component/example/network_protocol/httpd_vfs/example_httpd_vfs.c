#include "httpd/httpd.h"
#include "os_wrapper.h"
#include "vfs.h"
#include "example_httpd_vfs.h"
#include "lwipconf.h"
#include "dhcp/dhcps.h"

#define HTTPD_DEFAULT_CONFIG() {						\
		.port				= 80,						\
		.max_conn			= 10,						\
		.max_uri_handlers	= 8,						\
		.stack_bytes		= 3072,						\
		.thread_mode   		= HTTPD_THREAD_SINGLE,		\
		.secure         	= HTTPD_SECURE_NONE			\
}

#define MAX_PATH_LEN	200

extern httpd_handle_t httpd_handle;
extern void *httpd_malloc(size_t size);
extern int httpd_register_uri_handler(const httpd_uri_t *uri_handler);
extern int httpd_unregister_uri_handler(const httpd_uri_t *uri_handler);
extern int stat(const char *path, struct stat *buf);

int get_method_handler(struct httpd_conn *conn)
{
	char *user_agent = NULL;
	char *page_body = NULL;
	char *mime_type = MIME_TYPE_JS;
	char page_path[MAX_PATH_LEN] = {0};
	int  page_size;
	int  ret;
	char *index_page = "/index.html";
	int send_len = 2000;
	int i = 0;
	int remain_len;
	vfs_file *finfo;
	struct stat sbuf;
	char *prefix;
	char path[MAX_PATH_LEN * 2] = {0};

	//RTK_LOGS(NOTAG, "[%s] request.path = %s, request.path_len = %d\n", __func__, conn->request.path, conn->request.path_len);

	/*  Set mime type */
	if (conn->request.path_len >= 3 && strncmp(conn->request.path + conn->request.path_len - 3, ".js", 3) == 0) {
		mime_type = MIME_TYPE_JS;
	} else if (conn->request.path_len >= 4 && strncmp(conn->request.path + conn->request.path_len - 4, ".css", 4) == 0) {
		mime_type = MIME_TYPE_CSS;
	} else if ((conn->request.path_len >= 4 && strncmp(conn->request.path + conn->request.path_len - 4, ".jpg", 4) == 0) ||
			   (conn->request.path_len >= 5 && strncmp(conn->request.path + conn->request.path_len - 5, ".jpeg", 5) == 0)) {
		mime_type = MIME_TYPE_JPG;
	} else if (conn->request.path_len >= 4 && strncmp(conn->request.path + conn->request.path_len - 4, ".gif", 4) == 0) {
		mime_type = MIME_TYPE_GIF;
	} else if (conn->request.path_len >= 4 && strncmp(conn->request.path + conn->request.path_len - 4, ".png", 4) == 0) {
		mime_type = MIME_TYPE_PNG;
	} else if (conn->request.path_len >= 5 && strncmp(conn->request.path + conn->request.path_len - 5, ".html", 5) == 0) {
		mime_type = MIME_TYPE_HTML;
	} else {
		mime_type = MIME_TYPE_PLAIN;
	}

	strncpy(page_path, conn->request.path, (conn->request.path_len < MAX_PATH_LEN) ? conn->request.path_len : MAX_PATH_LEN);

	/* homepage / */
	if (conn->request.path_len == 1) {
		memset(page_path, 0, MAX_PATH_LEN);
		memcpy(page_path, index_page, strlen(index_page));
		mime_type = MIME_TYPE_HTML;
	}
	//RTK_LOGS(NOTAG, "[%s] page_path = %s\n", __func__, page_path);

	prefix = find_vfs_tag(VFS_REGION_1);

	DiagSnPrintf(path, MAX_PATH_LEN * 2, "%s:%s", prefix, page_path);

	finfo = (vfs_file *)fopen(path, "r");
	if (finfo == NULL) {
		RTK_LOGS(NOTAG, "[%s][%d] fopen failed: %s\r\n", __FUNCTION__, __LINE__, page_path);

#if(defined(CONFIG_ENABLE_CAPTIVE_PORTAL) && CONFIG_ENABLE_CAPTIVE_PORTAL)
		/* If enables captive portal, return homepage. */
		RTK_LOGS(NOTAG, "return homepage.\n");

		memset(page_path, 0, MAX_PATH_LEN);
		memcpy(page_path, index_page, strlen(index_page));
		mime_type = MIME_TYPE_HTML;

		sprintf(path, "%s:%s", prefix, page_path);
		finfo = (vfs_file *)fopen(path, "r");
		if (finfo == NULL) {
			RTK_LOGS(NOTAG, "[%s][%d] fopen failed: %s\r\n", __FUNCTION__, __LINE__, page_path);
			httpd_response_not_found(conn, NULL);
			httpd_conn_close(conn);
			return -1;
		}
#else
		httpd_response_not_found(conn, NULL);
		httpd_conn_close(conn);
		return -1;
#endif
	}

	if (stat(path, &sbuf) < 0) {
		RTK_LOGS(NOTAG, "%s stat fail %d\r\n", page_path, page_size);
		httpd_response_not_found(conn, NULL);
		goto EXIT;
	}
	page_size = sbuf.st_size;
	RTK_LOGS(NOTAG, "[%s] file:%s		size:%d\n", __func__, page_path, page_size);

	page_body = (char *)httpd_malloc(page_size);
	if (page_body == NULL) {
		RTK_LOGS(NOTAG, "[%s] malloc failed\n", __func__);
		goto EXIT;
	}
	memset(page_body, 0, page_size);

	// test log to show brief header parsing
	httpd_conn_dump_header(conn);

	// test log to show extra User-Agent header field
	if (httpd_request_get_header_field(conn, "User-Agent", &user_agent) != -1) {
		RTK_LOGS(NOTAG, "\nUser-Agent=[%s]\n", user_agent);
		httpd_free(user_agent);
	}

	ret = fread(page_body, page_size, 1, (FILE *)finfo);
	if (ret < 0) {
		RTK_LOGS(NOTAG, "[%s] fread failed\n", __func__);
		goto EXIT;
	}

	// write HTTP response
	httpd_response_write_header_start(conn, "200 OK", mime_type, page_size);
	httpd_response_write_header(conn, "Connection", "close");
	httpd_response_write_header_finish(conn);

	/* If it is greater than 6K, it may cause a hang */
	remain_len = page_size;
	while (remain_len > send_len) {
		httpd_response_write_data(conn, (uint8_t *)page_body + i * send_len, send_len);
		remain_len -= send_len;
		//RTK_LOGS(NOTAG, "i=%d, remain_len=%d\n", i, remain_len);
		i++;
	}

	if (remain_len > 0) {
		httpd_response_write_data(conn, (uint8_t *)page_body + i * send_len, remain_len);
	}

EXIT:
	fclose((FILE *)finfo);
	if (page_body) {
		httpd_free(page_body);
	}
	httpd_conn_close(conn);

	return 0;
}

int post_method_handler(struct httpd_conn *conn)
{
	char *msg = "[post] method is not supported!\n";
	RTK_LOGS(NOTAG, "%s", msg);
	httpd_response_method_not_allowed(conn, msg);

	return 0;
}

int put_method_handler(struct httpd_conn *conn)
{
	char *msg = "[put] method is not supported!\n";
	RTK_LOGS(NOTAG, "%s", msg);
	httpd_response_method_not_allowed(conn, msg);

	return 0;
}

static const httpd_uri_t put_callback = {
	.method  = HTTP_PUT,
	.handler = put_method_handler,
};

static const httpd_uri_t get_callback = {
	.method  = HTTP_GET,
	.handler = get_method_handler,
};

static const httpd_uri_t post_callback = {
	.method  = HTTP_POST,
	.handler = post_method_handler,
};


static void example_httpd_vfs_thread(void *param)
{
	/* To avoid gcc warnings */
	(void) param;

	httpd_config_t config = HTTPD_DEFAULT_CONFIG();

	rtos_time_delay_ms(5000);
	RTK_LOGS(NOTAG, "\nExample: httpd_vfs\n");

	struct httpd_data *hd = calloc(1, sizeof(struct httpd_data));
	if (!hd) {
		RTK_LOGS(NOTAG, "Failed to allocate memory for HTTP data");
		goto exit; ;
	}
	hd->hd_calls = calloc(config.max_uri_handlers, sizeof(httpd_uri_t *));
	if (!hd->hd_calls) {
		RTK_LOGS(NOTAG, "Failed to allocate memory for HTTP URI handlers");
		free(hd);
		goto exit;
	}

	hd->config = config;
	httpd_handle = (httpd_handle_t *)hd;

	httpd_register_uri_handler(&get_callback);
	httpd_register_uri_handler(&post_callback);
	httpd_register_uri_handler(&put_callback);

	if (httpd_start_with_callback(config.port, config.max_conn, config.stack_bytes, config.thread_mode, config.secure) != 0) {
		RTK_LOGS(NOTAG, "ERROR: example_httpd_littlefs_thread\n");
		httpd_unregister_uri_handler(&get_callback);
		httpd_unregister_uri_handler(&post_callback);
		httpd_unregister_uri_handler(&put_callback);
	}
exit:
	rtos_task_delete(NULL);
}

void example_httpd_vfs(void)
{
	rtos_task_t task;
	if (rtos_task_create(&task, ((const char *)"example_httpd_vfs_thread"), example_httpd_vfs_thread, NULL, 2048 * 4, 1) != SUCCESS) {
		RTK_LOGS(NOTAG, "\n\r[%s] Create example_httpd_vfs_thread task failed", __FUNCTION__);
	}
}
