/**
  ******************************************************************************
  * @file    wtn_app_socket.c
  * @author
  * @version
  * @date
  * @brief
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#define __WTN_APP_SOCKET_C__


/* -------------------------------- Includes -------------------------------- */
#include "rtw_autoconf.h"
#ifdef CONFIG_WTN_SOCKET_APP
#include "os_wrapper.h"
#include "lwip_netconf.h"
#include "ameba_ota.h"
#include "dlist.h"
#include "rtw_queue.h"
#include <lwip/sockets.h>
#include "log.h"
#include "wifi_intf_drv_to_app_basic.h"
#include "build_info.h"
#include "wtn_app_socket.h"


extern void sys_reset(void);

#define WTN_BROADCAST_PORT 12345
#define WTN_OTA_REQUEST_PORT 54321

#define WTN_BUF_NUM 5
#define WTN_BCMC_REPORT 3
#define WTN_PC_IP_ENTRY 0x10
#define WTN_PC_PORT_ENTRY 0x11

#define WTN_UDP_SOCKET
//#define WTN_SINGLE_NODE_OTA
//#define WTN_COMPILE_TIME_INFO
struct wtn_buf_node {
	struct list_head	list;
	u8 *buf_addr;
	u32 buf_len;
	u8	is_used;
};

int wtn_client_fd = -1;
u32 wtn_server_port = 0;
u8 wtn_socket_need_close = 0;
u8 wtn_tool_restart = 0;
u8 wtn_tool_rand = 0;
u8 wtn_server_ip[4] = {0};
rtos_sema_t  wtn_socket_send_sema = NULL;
struct wtn_buf_node wtn_buf_pool[WTN_BUF_NUM] = {0};
struct __queue wtn_buf_queue;
struct wtn_http_ota_param ota_param;

struct wtn_buf_node *dequeue_wtn_buf(struct __queue *p_queue)
{
	struct wtn_buf_node *p_node;
	struct list_head *plist, *phead;

	rtos_critical_enter(RTOS_CRITICAL_WIFI);
	if (rtw_queue_empty(p_queue) == TRUE) {
		p_node = NULL;
	} else {
		phead = get_list_head(p_queue);
		plist = get_next(phead);
		p_node = LIST_CONTAINOR(plist, struct wtn_buf_node, list);
		rtw_list_delete(&(p_node->list));
	}
	rtos_critical_exit(RTOS_CRITICAL_WIFI);
	return p_node;
}

int wtn_socket_send(u8 *buf, u32 len)
{
	u8 *ip = 0;
	u8 invalid_ip[4] = {0};
	u8 *buf_copy = NULL;
	struct wtn_buf_node *pnode = NULL;
	int i;

	/* check if DHCP finished */
	ip = (u8 *)LwIP_GetIP(0);
	if (memcmp(ip, invalid_ip, 4) == 0 || wtn_client_fd < 0) {
		return FAIL;
	}

	/*fill IP*/
	buf_copy = (u8 *)rtos_mem_zmalloc(len);
	memcpy(buf_copy, buf, len);
	memcpy(buf_copy + 31, ip, 4);
	/*fill compile time*/
#ifdef WTN_COMPILE_TIME_INFO
	char *compiletime = NULL;
	compiletime = RTL_FW_COMPILE_TIME;
	memcpy(buf_copy + 48, compiletime,  strlen(compiletime));
#endif
	rtos_critical_enter(RTOS_CRITICAL_WIFI);
	for (i = 0; i < WTN_BUF_NUM; i++) {
		if (wtn_buf_pool[i].is_used == 0) {
			wtn_buf_pool[i].is_used = 1;
			pnode = &wtn_buf_pool[i];
			break;
		}
	}
	rtos_critical_exit(RTOS_CRITICAL_WIFI);
	if (pnode == NULL) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "wtn not enough buf node\n");
		return FAIL;
	}

	pnode->buf_addr = buf_copy;
	pnode->buf_len = len;
	rtos_critical_enter(RTOS_CRITICAL_WIFI);
	rtw_list_insert_tail(&(pnode->list), get_list_head(&wtn_buf_queue));
	rtos_critical_exit(RTOS_CRITICAL_WIFI);
	rtos_sema_give(wtn_socket_send_sema);
	return SUCCESS;
}

#ifdef WTN_SINGLE_NODE_OTA
void wtn_http_ota_task(void *param)
{
	struct wtn_http_ota_param *ota_param = (struct wtn_http_ota_param *)param;
	int ret = -1;
	ota_context *ctx = NULL;

	ctx = (ota_context *)rtos_mem_malloc(sizeof(ota_context));
	if (ctx == NULL) {
		goto exit;
	}

	memset(ctx, 0, sizeof(ota_context));

	ret = ota_update_init(ctx, (char *)ota_param->host, ota_param->port, (char *)ota_param->resource, OTA_HTTP);
	if (ret != 0) {
		goto exit;
	}

	ret = ota_update_start(ctx);

	if (!ret) {
		/*send ota status to server*/

		rtos_time_delay_ms(20);
		sys_reset();
	}

exit:
	ota_update_deinit(ctx);
	if (ctx) {
		rtos_mem_free(ctx);
	}
	rtos_mem_free(ota_param->host);
	rtos_mem_free(ota_param->resource);
	rtos_task_delete(NULL);
}

int wtn_ota_socket_process(int *ota_socket_fd, u8 *buf)
{
	struct sockaddr_in ota_server_addr;
	struct sockaddr_in ota_client_addr;
	u8 ota_packet_pattern[4] = {0xAC, 0xBD, 0xCE, 0xDF};
	//u8 invalid_ip[4] = {0};
	u8 ota_request_seq = 0;
	int client_len;
	int ret;
	u8 httpiplen = 0;
	u8 http_resourcelen = 0;
	struct timeval timeout;

	if ((*ota_socket_fd == -1)) {/*create socket first*/ //&& (memcmp(wtn_server_ip, invalid_ip, 4) != 0)
		*ota_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
		if (*ota_socket_fd < 0) {
			return FAIL;
		}
		memset(&ota_server_addr, 0, sizeof(ota_server_addr));
		ota_server_addr.sin_family = AF_INET;
		//ota_server_addr.sin_addr.s_addr = (u32)(wtn_server_ip[0] | wtn_server_ip[1] << 8 | wtn_server_ip[2] << 16 | wtn_server_ip[3] << 24);
		ota_server_addr.sin_addr.s_addr = INADDR_ANY;
		ota_server_addr.sin_port = htons(WTN_OTA_REQUEST_PORT);
		if (bind(*ota_socket_fd, (struct sockaddr *)&ota_server_addr, sizeof(ota_server_addr)) < 0) {
			RTK_LOGS(NOTAG, RTK_LOG_ERROR, "wtn bind ota rx socket fail\n");
			closesocket(*ota_socket_fd);
			return FAIL;
		}

		timeout.tv_sec = 3;
		timeout.tv_usec = 0;
		lwip_setsockopt(*ota_socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
	}

	if (*ota_socket_fd >= 0) {/*if socket created, try to receive ota request from server*/
		memset(buf, 0, 200);
		client_len = sizeof(ota_client_addr);
		ret = lwip_recvfrom(*ota_socket_fd, buf, 50, 0, (struct sockaddr *)&ota_client_addr, (u32_t *)&client_len);
		if (ret < 0) {
			if (errno == EWOULDBLOCK || errno == EAGAIN) {
				return FAIL;
			}
		} else if (((u32)ret) >= 5) {
			if (memcmp(buf, ota_packet_pattern, 4) == 0) {
				if (buf[4] != ota_request_seq) {
					ota_request_seq = buf[4];
					/*get host and resource*/
					httpiplen = buf[5];
					ota_param.host = (char *)rtos_mem_zmalloc(httpiplen + 1);
					memcpy(ota_param.host, buf + 6, httpiplen);
					http_resourcelen = buf[4 + 1 + 1 + httpiplen];
					ota_param.resource = (char *)rtos_mem_zmalloc(http_resourcelen + 1);
					memcpy(ota_param.resource, buf + 7 + buf[5], http_resourcelen);
					memcpy(&ota_param.port, buf + 4 + 1 + 1 + httpiplen + 1 + http_resourcelen, 4);
					ota_param.port = htonl(ota_param.port);
					/*create task to do http ota*/
					if (rtos_task_create(NULL, ((const char *)"update_ota_task"), wtn_http_ota_task, &ota_param, 8000, 1) != SUCCESS) {
						RTK_LOGS(NOTAG, RTK_LOG_ERROR, "wtn ota task create fail\n");
					}
				}
			}
		}
	}
	return SUCCESS;
}
#endif

void wtn_bcmc_socket_handler(void *param)
{
	(void) param;
	u8 *ip = 0;
	u8 invalid_ip[4] = {0};
	int bcmc_socket_fd = -1;
	int softap_bcmc_socket_fd = -1;
	int ota_socket_fd = -1;
	int broadcast_enable = 1;
	int client_len;
	struct sockaddr_in bcmc_server_addr;
	struct sockaddr_in bcmc_client_addr;
	struct sockaddr_in softap_bcmc_addr;
	struct timeval timeout;
	u8 *buf = NULL;
	int opt = 1;
	int ret;

	/*check whether DHCP finished*/
	ip = (u8 *)LwIP_GetIP(0);
	while (memcmp(ip, invalid_ip, 4) == 0) {
		rtos_time_delay_ms(1000);
		if (wtn_socket_need_close) {/*if connect fail after assoc success, since IP has not gotten, just exit*/
			goto exit;
		}
		ip = (u8 *)LwIP_GetIP(0);
	}

	/*when rnat enabled, check whether softap ap has started*/
	if (wifi_user_config.wtn_rnat_en) {
		ip = (u8 *)LwIP_GetIP(1);
		while (memcmp(ip, invalid_ip, 4) == 0) {
			rtos_time_delay_ms(1000);
			ip = (u8 *)LwIP_GetIP(1);
		}
		/*create the softap bcmc socket for forwarding bcmc socket pkt*/
		softap_bcmc_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
		if (softap_bcmc_socket_fd < 0) {
			goto exit;
		}

		setsockopt(softap_bcmc_socket_fd, SOL_SOCKET, SO_REUSEADDR, (const char *) &opt, sizeof(opt));

		if (setsockopt(softap_bcmc_socket_fd, SOL_SOCKET, SO_BROADCAST, &broadcast_enable, sizeof(broadcast_enable)) < 0) {
			closesocket(softap_bcmc_socket_fd);
			goto exit;
		}

		memset(&softap_bcmc_addr, 0, sizeof(softap_bcmc_addr));
		softap_bcmc_addr.sin_family = AF_INET;
		softap_bcmc_addr.sin_addr.s_addr = (u32)(ip[0] | ip[1] << 8 | ip[2] << 16 | (0xFF) << 24);
		softap_bcmc_addr.sin_port = htons(WTN_BROADCAST_PORT);
	}

	/*bind broadcast socket*/
	bcmc_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (bcmc_socket_fd < 0) {
		goto exit;
	}
	if (setsockopt(bcmc_socket_fd, SOL_SOCKET, SO_BROADCAST, &broadcast_enable, sizeof(broadcast_enable)) < 0) {
		closesocket(bcmc_socket_fd);
		goto exit;
	}

	setsockopt(bcmc_socket_fd, SOL_SOCKET, SO_REUSEADDR, (const char *) &opt, sizeof(opt));

	memset(&bcmc_server_addr, 0, sizeof(bcmc_server_addr));
	bcmc_server_addr.sin_family = AF_INET;
	bcmc_server_addr.sin_addr.s_addr = INADDR_ANY;
	bcmc_server_addr.sin_port = htons(WTN_BROADCAST_PORT);

	if (bind(bcmc_socket_fd, (struct sockaddr *)&bcmc_server_addr, sizeof(bcmc_server_addr)) < 0) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "wtn bind rx socket fail\n");
		closesocket(bcmc_socket_fd);
		goto exit;
	}

	timeout.tv_sec = 3;
	timeout.tv_usec = 0;
	lwip_setsockopt(bcmc_socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

	buf = rtos_mem_zmalloc(200);
	if (!buf) {
		goto exit;
	}

	while (1) {
		if (wtn_socket_need_close) {
			goto exit;
		}
		memset(buf, 0, 200);
		client_len = sizeof(bcmc_client_addr);
		ret = lwip_recvfrom(bcmc_socket_fd, buf, 200, 0, (struct sockaddr *)&bcmc_client_addr, (u32_t *)&client_len);
		if (ret < 0) {
			if (errno == EWOULDBLOCK || errno == EAGAIN) {
#ifdef WTN_SINGLE_NODE_OTA
				goto ota_process;
#else
				continue;
#endif
			} else {
				RTK_LOGS(NOTAG, RTK_LOG_ERROR, "wtn rx bcmc fail\n");
				goto exit;
			}
		} else {
			/*parsing content to get server ip and port*/
			if (((u32)ret) >= 13) {
				/*1 byte random, when tool restarted, random will change*/
				if (wtn_tool_rand == 0) {
					wtn_tool_rand = buf[3];
				} else {
					if (wtn_tool_rand != buf[3]) {
						wtn_tool_rand = buf[3];
						wtn_tool_restart = 1;
					}
				}
				if (buf[4] == WTN_PC_IP_ENTRY) {
					memcpy(wtn_server_ip, buf + 5, 4);
				}
				if (buf[9] == WTN_PC_PORT_ENTRY) {
					memcpy(&wtn_server_port, buf + 10, 4);
					wtn_server_port = htonl(wtn_server_port);
				}
				if (wifi_user_config.wtn_rnat_en) {
					/*forward this packet by softap port*/
					sendto(softap_bcmc_socket_fd, buf, ret, 0, (struct sockaddr *)&softap_bcmc_addr, sizeof(softap_bcmc_addr));
				}
			}
		}
#ifdef WTN_SINGLE_NODE_OTA
ota_process:
		if (wtn_ota_socket_process(&ota_socket_fd, buf) == FAIL) {
			continue;
		}
#endif
	}
exit:
	if (buf) {
		rtos_mem_free(buf);
	}
	if (bcmc_socket_fd >= 0) {
		closesocket(bcmc_socket_fd);
	}
	if (softap_bcmc_socket_fd >= 0) {
		closesocket(softap_bcmc_socket_fd);
	}
	if (ota_socket_fd >= 0) {
		closesocket(ota_socket_fd);
	}
	rtos_task_delete(NULL);
}

void wtn_tx_socket_handler(void *param)
{
	(void) param;
	u8 *ip = 0;
	u8 invalid_ip[4] = {0};
	struct sockaddr_in server_addr;
	struct wtn_buf_node *pnode;

create_socket:
	/*check whether DHCP finished and PC IP is obtained*/
	ip = (u8 *)LwIP_GetIP(0);
	while (memcmp(ip, invalid_ip, 4) == 0 || memcmp(wtn_server_ip, invalid_ip, 4) == 0) {
		rtos_time_delay_ms(1000);
		if (wtn_socket_need_close) {/*if connect fail after assoc success, since IP has not gotten, just exit*/
			rtos_task_delete(NULL);
		}
		ip = (u8 *)LwIP_GetIP(0);
	}

#ifdef WTN_UDP_SOCKET
	wtn_client_fd = socket(AF_INET, SOCK_DGRAM, 0);
#else
	wtn_client_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
#endif
	if (wtn_client_fd < 0) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "wtn tx socket create fail\n");
		goto exit;
	}

	/* initialize value in dest */
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(wtn_server_port);
	server_addr.sin_addr.s_addr = wtn_server_ip[3] << 24 | wtn_server_ip[2] << 16 | wtn_server_ip[1] << 8 | wtn_server_ip[0];

#ifndef WTN_UDP_SOCKET
	/* Connecting to server */
	if (connect(wtn_client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "wtn tx socket connect fail\n");
		goto exit;
	}
#endif

	wtn_tool_restart = 0;
	do {
		rtos_sema_take(wtn_socket_send_sema, RTOS_MAX_TIMEOUT);

		while (1) {
			pnode = dequeue_wtn_buf(&wtn_buf_queue);
			if (pnode == NULL) {
				break;
			}

#ifdef WTN_UDP_SOCKET
			if (sendto(wtn_client_fd, pnode->buf_addr, pnode->buf_len, 0, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) <= 0) {
				//goto exit;
			}
#else
			if (send(wtn_client_fd, pnode->buf_addr, pnode->buf_len, 0) <= 0) {
				//RTK_LOGS(NOTAG, RTK_LOG_ERROR, "wtn tx socket send data error\n");
				//goto exit;
			}
#endif
			rtos_mem_free(pnode->buf_addr);
			rtos_critical_enter(RTOS_CRITICAL_WIFI);
			pnode->is_used = 0;
			rtos_critical_exit(RTOS_CRITICAL_WIFI);
		}
		if (wtn_socket_need_close) {
			goto exit;
		}
		if (wtn_tool_restart) {
			RTK_LOGS(NOTAG, RTK_LOG_ERROR, "wtn tool restart, reconnect to server\n");
			closesocket(wtn_client_fd);
			while (1) {/*release current buf node before recreate socket*/
				pnode = dequeue_wtn_buf(&wtn_buf_queue);
				if (pnode == NULL) {
					break;
				}
				rtos_mem_free(pnode->buf_addr);
				rtos_critical_enter(RTOS_CRITICAL_WIFI);
				pnode->is_used = 0;
				rtos_critical_exit(RTOS_CRITICAL_WIFI);
			}
			goto create_socket;
		}
	} while (1);

exit:

	if (wtn_client_fd >= 0) {
		closesocket(wtn_client_fd);
		wtn_client_fd = -1;
	}
	rtos_sema_delete_static(wtn_socket_send_sema);
	rtos_task_delete(NULL);
}

int wtn_socket_init(u8 enable)
{
	if (enable) {
		rtos_sema_create_static(&wtn_socket_send_sema, 0, 0xFFFFFFFF);
		rtw_init_queue(&wtn_buf_queue);
		wtn_socket_need_close = 0;
		wtn_tool_restart = 0;
		wtn_tool_rand = 0;
		wtn_client_fd = -1;
		wtn_server_port = 0;
		memset(wtn_server_ip, 0, 4);
		memset(wtn_buf_pool, 0, WTN_BUF_NUM * sizeof(struct wtn_buf_node));
		if (rtos_task_create(NULL, "wtn_bcmc_socket_handler", wtn_bcmc_socket_handler, NULL, 2048, 2) != SUCCESS) {
			return FAIL;
		}

		if (rtos_task_create(NULL, "wtn_tx_socket_handler", wtn_tx_socket_handler, NULL, 2048, 2) != SUCCESS) {
			return FAIL;
		}
	} else {
		wtn_socket_need_close = 1;
		rtos_sema_give(wtn_socket_send_sema);/*trigger socket tx task to end*/
	}

	return SUCCESS;
}
#endif