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
#include "dlist.h"
#include "rtw_queue.h"
#include <lwip/sockets.h>
#include "lwip_netconf.h"
#include "log.h"

#define WTN_SERVER_PORT 12346
#define WTN_BROADCAST_PORT 12345

#define WTN_BUF_NUM 5
#define WTN_BCMC_REPORT 3
#define WTN_PC_IP_ENTRY 0x10
#define WTN_PC_PORT_ENTRY 0x11

#define WTN_UDP_SOCKET
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

struct wtn_buf_node *dequeue_wtn_buf(struct __queue *p_queue)
{
	struct wtn_buf_node *p_node;
	struct list_head *plist, *phead;

	rtos_critical_enter();
	if (rtw_queue_empty(p_queue) == TRUE) {
		p_node = NULL;
	} else {
		phead = get_list_head(p_queue);
		plist = get_next(phead);
		p_node = LIST_CONTAINOR(plist, struct wtn_buf_node, list);
		rtw_list_delete(&(p_node->list));
	}
	rtos_critical_exit();
	return p_node;
}

int wtn_socket_send(u8 *buf, u32 len)
{
	/*buf get from NP, need dcache invalidate first*/
	DCache_Invalidate((u32)buf, len);
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
	rtos_critical_enter();
	for (i = 0; i < WTN_BUF_NUM; i++) {
		if (wtn_buf_pool[i].is_used == 0) {
			wtn_buf_pool[i].is_used = 1;
			pnode = &wtn_buf_pool[i];
			break;
		}
	}
	rtos_critical_exit();
	if (pnode == NULL) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "wtn not enough buf node\n");
		return FAIL;
	}

	pnode->buf_addr = buf_copy;
	pnode->buf_len = len;
	rtos_critical_enter();
	rtw_list_insert_tail(&(pnode->list), get_list_head(&wtn_buf_queue));
	rtos_critical_exit();
	rtos_sema_give(wtn_socket_send_sema);
	return SUCCESS;
}

void wtn_bcmc_socket_handler(void *param)
{
	(void) param;
	u8 *ip = 0;
	u8 invalid_ip[4] = {0};
	int bcmc_socket_fd = -1;
	int broadcast_enable = 1;
	int client_len;
	struct sockaddr_in bcmc_server_addr;
	struct sockaddr_in bcmc_client_addr;
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
		return;
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
				continue;
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
			}
		}
	}
exit:
	if (buf) {
		rtos_mem_free(buf);
	}
	if (bcmc_socket_fd >= 0) {
		closesocket(bcmc_socket_fd);
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
			rtos_critical_enter();
			pnode->is_used = 0;
			rtos_critical_exit();
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
				rtos_critical_enter();
				pnode->is_used = 0;
				rtos_critical_exit();
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