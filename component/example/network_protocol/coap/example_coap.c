#include "wifi_api.h"
#include "lwip_netconf.h"

#include "sn_coap_ameba_port.h"

#define SERVER_HOST     "coap.me"
#define URI_PATH        "/hello"
//#define SERVER_HOST		"californium.eclipseprojects.io"
//#define URI_PATH		"/obs"
#define SERVER_PORT     5683
#define BUF_LEN         1280 // Suggested is to keep packet size under 1280 bytes
#define MAX_AGE         60
#define TOKEN           "123"

struct coap_s          *coapHandle;
coap_version_e          coapVersion = COAP_VERSION_1;

uint8_t coap_tx_cb(uint8_t *a, uint16_t b, sn_nsdl_addr_s *c, void *d)
{
	/* To avoid gcc warnings */
	(void)a;
	(void)b;
	(void)c;
	(void)d;

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "coap tx cb\n");
	return 0;
}

int8_t coap_rx_cb(sn_coap_hdr_s *a, sn_nsdl_addr_s *b, void *c)
{
	/* To avoid gcc warnings */
	(void)a;
	(void)b;
	(void)c;

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "coap rx cb\n");
	return 0;
}

static void example_coap_thread(void *para)
{
	/* To avoid gcc warnings */
	(void)para;

	// Delay to check successful WiFi connection and obtain of an IP address
	LwIP_Check_Connectivity();

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\r\n====================Example: CoAP====================\r\n");

	// Initialize the CoAP protocol handle, pointing to local implementations on malloc/free/tx/rx functions
	coapHandle = coap_protocol_init(&coap_tx_cb, &coap_rx_cb);

	// See ns_coap_header.h
	sn_coap_hdr_s *coap_res_ptr = (sn_coap_hdr_s *)coap_calloc(1 * sizeof(sn_coap_hdr_s));


	coap_res_ptr->token_len = sizeof(TOKEN);
	coap_res_ptr->coap_status = COAP_STATUS_OK;
	coap_res_ptr->msg_code = COAP_MSG_CODE_REQUEST_GET;
	coap_res_ptr->msg_type = COAP_MSG_TYPE_CONFIRMABLE;
	coap_res_ptr->content_format = COAP_CT_TEXT_PLAIN;
	coap_res_ptr->msg_id = randLIB_get_16bit();
	coap_res_ptr->uri_path_len = strlen(URI_PATH);
	coap_res_ptr->payload_len = 0;
	coap_res_ptr->token_ptr = (uint8_t *)TOKEN;
	coap_res_ptr->uri_path_ptr = (uint8_t *)URI_PATH;
	coap_res_ptr->payload_ptr = NULL;
	//coap_res_ptr->options_list_ptr = NULL;

	coap_res_ptr->options_list_ptr = (sn_coap_options_list_s *)coap_calloc(1 * sizeof(sn_coap_options_list_s));

	/*Option list*/
	coap_res_ptr->options_list_ptr->observe = 0;
#if 0
	coap_res_ptr->options_list_ptr->uri_query_len = 0;
	coap_res_ptr->options_list_ptr->uri_query_ptr = NULL;
	coap_res_ptr->options_list_ptr->accept = COAP_CT_NONE;
	coap_res_ptr->options_list_ptr->block1 = -1;
	coap_res_ptr->options_list_ptr->block2 = -1;
	coap_res_ptr->options_list_ptr->etag_len = 0;
	coap_res_ptr->options_list_ptr->etag_ptr = NULL;
	coap_res_ptr->options_list_ptr->location_path_len = 0;
	coap_res_ptr->options_list_ptr->location_path_ptr = NULL;
	coap_res_ptr->options_list_ptr->location_query_len = 0;
	coap_res_ptr->options_list_ptr->location_query_ptr = NULL;
	coap_res_ptr->options_list_ptr->max_age = MAX_AGE;
	coap_res_ptr->options_list_ptr->observe = 0;
	coap_res_ptr->options_list_ptr->proxy_uri_len = 0;
	coap_res_ptr->options_list_ptr->proxy_uri_ptr = NULL;
	coap_res_ptr->options_list_ptr->size1 = 0;
	coap_res_ptr->options_list_ptr->size2 = 0;
	coap_res_ptr->options_list_ptr->uri_host_len = 0;
	coap_res_ptr->options_list_ptr->uri_host_ptr = NULL;
	coap_res_ptr->options_list_ptr->uri_port = -1;
#endif
	int socket = coap_sock_open();

	//send CoAP message
	if (coap_send(SERVER_HOST, SERVER_PORT, socket, coap_res_ptr) < 0) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\r\n coap_send() failed\r\n");
		goto exit;
	}

	//receive CoAP message
	struct sockaddr_in from_address;
	uint8_t *recv_buffer = (uint8_t *)coap_calloc(BUF_LEN);
	int ret;

	if ((ret = coap_recv(socket, &from_address, recv_buffer, BUF_LEN)) >= 0) {
		uint32_t ip = from_address.sin_addr.s_addr;
		uint8_t bytes[4];
		bytes[0] = ip & 0xFF;
		bytes[1] = (ip >> 8) & 0xFF;
		bytes[2] = (ip >> 16) & 0xFF;
		bytes[3] = (ip >> 24) & 0xFF;

		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\nReceived %d bytes from '%d.%d.%d.%d:%d'\n", ret, bytes[0], bytes[1], bytes[2], bytes[3], from_address.sin_port);

		sn_coap_hdr_s *parsed = sn_coap_parser(coapHandle, ret, recv_buffer, &coapVersion);

		if (parsed) {
			coap_print_hdr(parsed);
			sn_coap_parser_release_allocated_coap_msg_mem(coapHandle, parsed);
		}
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\r\n coap_recv() failed\r\n");
	}

	coap_free(recv_buffer);

exit:
	coap_sock_close(socket);

	coap_free(coap_res_ptr->options_list_ptr);
	coap_free(coap_res_ptr);

	coap_free(coapHandle);

	rtos_task_delete(NULL);
}

void example_coap(void)
{
	if (rtos_task_create(NULL, ((const char *)"example_coap_thread"), example_coap_thread, NULL, 2048 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r%s rtos_task_create(example_coap_thread) failed", __FUNCTION__);
	}
}

