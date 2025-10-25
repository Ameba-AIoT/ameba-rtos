/**
  ******************************************************************************
  * @file    wtn_app_rnat.h
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

#ifndef WTN_APP_OTA_H
#define WTN_APP_OTA_H

#define WTN_MULTI_NODE_OTA

#define RMESH_ALL_NODE_OTA 1
#define RMESH_PARTIAL_NODE_OTA 2

#define RMESH_OTA_DATA			0
#define RMESH_OTA_UPDATE_REQ	1
#define RMESH_OTA_STATUS_REQ	2
#define RMESH_OTA_STATUS_RSP	3
#define RMESH_OTA_PATTERN_LEN	8

#define RMESH_OTA_STS_ONGOING	0x1
#define RMESH_OTA_STS_FINISHED	0x2
#define RMESH_OTA_STS_RESET		0x3

#define RMESH_MAX_NODE_NUM	14 /*consider dplus rnat support max 14 client*/

#define RMESH_OTA_PACKET_SIZE 1024
#define RMESH_OTA_ROUND_MAX			50
#define	RMESH_OTA_ROUND_INTERVAL 1500
#define RMESH_OTA_VER_MAX_LEN	20

#define RMESH_OTA_PROGRESS_MAX		200

#define RMESH_OTA_HTTP_HOST_MAX_LEN 20

#define RMESH_OTA_UPDATE_REQ_WAIT_TO 55000
#define RMESH_OTA_HTTP_TO 100000
#define RMESH_OTA_RESET_WAIT_TO 2000
#define RMESH_OTA_WAIT_MORE_SLAVE_TO 500

enum rmesh_ota_filter_res {
	RMESH_OTA_SELF_PROCESS = 0,
	RMESH_OTA_DRIVER_RELAY = 1,
	RMESH_OTA_BOTH_SELF_AND_RELAY = 2,
	RMESH_OTA_NO_NEED_PROCESS = 3,
};

enum rmesh_ota_sts {
	RMESH_OTA_ONGOING = 1,
	RMESH_OTA_SUCCESS = 2,
	RMESH_OTA_RETRYING = 3,
};

enum rmesh_ota_method {
	RMESH_HTTP_OTA = 1,
	RMESH_INTER_OTA = 2,
};

struct rmesh_ota_info_to_flash {
	u8 ota_ver_len;
	u8 image_id;
	u8 ota_sts;
	u16 ota_time;
	u32 image_addr;
	u32 ota_firmware_size;
	u32 checksum;
	u32 http_port;
	u8 cur_ota_ver[RMESH_OTA_VER_MAX_LEN];
	u8 target_ota_ver[RMESH_OTA_VER_MAX_LEN];
	char http_host[RMESH_OTA_HTTP_HOST_MAX_LEN];
};

struct rmesh_ota_status {
	u8 current_state; /*ongoing, success, fail*/
	u8 percentage;
};

struct rmesh_http_ota_param {
	char *host;
	char *resource;
	u16 port;
	u8 ota_type;
};

//add for test
struct rmesh_role_info_to_flash {
	u8 role; /*1 for rnat, 2 for rnat client*/
};

/* -------------------------------- Includes -------------------------------- */
void rmesh_ota_cmd_recv(struct rmesh_http_ota_param *ota_param);
void rmesh_ota_init(void);
void rmesh_ota_get_status(struct rmesh_ota_status *ota_sts);
int wtn_on_ota_request(u8 *buf, int recv_len, int *forward_sock_fd);
#endif