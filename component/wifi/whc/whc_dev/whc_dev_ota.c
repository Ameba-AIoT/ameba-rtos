/******************************************************************************
 *
 * Copyright(c) 2020 - 2021 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/
#include "whc_dev.h"

#include "lwip_netconf.h"
#include "ameba_soc.h"
#include "ota_api.h"
#ifdef CONFIG_RMESH_OTA_EN
#include "wtn_app_ota.h"
extern struct rmesh_http_ota_param ota_param;
#endif
struct whc_dev_ota_info *whc_ota_info;

void whc_update_ota_task(void *param)
{
	int ret = -1;
	ota_context_t *ctx = NULL;
	u32 port;
	(void)param;

	if (!whc_ota_info) {
		RTK_LOGE(TAG_WLAN_INIC, "ota_info null \r\n");
		goto exit;
	}

	ctx = (ota_context_t *)rtos_mem_malloc(sizeof(ota_context_t));
	if (ctx == NULL) {
		RTK_LOGE(TAG_WLAN_INIC, "ota task mem err");
		goto exit;
	}

	memset(ctx, 0, sizeof(ota_context_t));

	port = whc_ota_info->port;

	ret = ota_init(ctx, (char *)whc_ota_info->host, port, (char *)whc_ota_info->resource, OTA_WHC);
	if (ret != 0) {
		RTK_LOGE(TAG_WLAN_INIC, "ota_init err");
		goto exit;
	}

	ret = ota_start(ctx);

	RTK_LOGE(TAG_WLAN_INIC, "ota exit");
	if (!ret) {
		RTK_LOGE(TAG_WLAN_INIC, "can reboot\n");
#ifdef todo
#endif
		// sys_reset();
	}

exit:
	if (ctx) {
		ota_deinit(ctx);
		rtos_mem_free(ctx);
	}

	if (whc_ota_info) {
		rtos_mem_free(whc_ota_info);
		whc_ota_info = NULL;
	}
	rtos_task_delete(NULL);
}

void whc_dev_api_start_ota(struct whc_dev_ota_info *ota_info)
{
	u8 ota_type = ota_info->ota_type;
	u32 len;
	(void)len;

	if (ota_type == OTA_FOR_NORMAL) {
		if (whc_ota_info == NULL) {
			whc_ota_info = rtos_mem_zmalloc(sizeof(struct whc_dev_ota_info));
			memcpy(whc_ota_info, ota_info, sizeof(struct whc_dev_ota_info));
		}

		if (rtos_task_create(NULL, ((const char *)"whc_update_ota_task"), whc_update_ota_task, NULL, 1024 * 5, 1) != RTK_SUCCESS) {
			RTK_LOGE(TAG_WLAN_INIC, "create ota task err\n");
		}
#ifdef CONFIG_RMESH_OTA_EN
	} else if (ota_type == OTA_FOR_RMESH) {
		memset(&ota_param, 0, sizeof(struct rmesh_http_ota_param));
		len = strlen(ota_info->host);
		ota_param.host = (char *)rtos_mem_zmalloc(len + 1);
		memcpy(ota_param.host, ota_info->host, len);
		memcpy(&ota_param.port, &(ota_info->port), 2);

		len = strlen(ota_info->resource);
		ota_param.resource = (char *)rtos_mem_zmalloc(len + 1);
		memcpy(ota_param.resource, ota_info->resource, len);
		ota_param.ota_type = ota_info->rmesh_ota_type;
		rmesh_ota_cmd_recv(&ota_param);
#endif
	}

}

void whc_dev_api_ota_write(u8 *buf)
{
	struct whc_dev_ota_hdr *ota_hdr = (struct whc_dev_ota_hdr *)buf;
	struct whc_dev_ota_readbuf *readbuf;
	int16_t payload_len = ota_hdr->len;
	if (whc_cmdpath_data.ret) {
		readbuf = (struct whc_dev_ota_readbuf *)whc_cmdpath_data.ret;
		memcpy(&readbuf->ota_hdr, ota_hdr, sizeof(struct whc_dev_ota_hdr));
		if (payload_len > 0) {
			memcpy(readbuf->buf, buf + sizeof(struct whc_dev_ota_hdr), payload_len);
		}
		rtos_sema_give(whc_cmdpath_data.whc_user_blocksend_sema);
	}
}

void whc_dev_api_ota_process(u8 *buf)
{
	u8 *ptr;
	struct whc_dev_ota_hdr *ota_hdr = (struct whc_dev_ota_hdr *)buf;
	struct whc_dev_ota_info *ota_info;
	ptr = buf + sizeof(struct whc_dev_ota_hdr);

	switch (ota_hdr->subtype) {
	case WHC_OTA_START:
		ota_info = (struct whc_dev_ota_info *)ptr;
		whc_dev_api_start_ota(ota_info);
		break;
	case WHC_OTA_WRITE:
		whc_dev_api_ota_write(buf);
		break;

	case WHC_OTA_END:
		break;
	default:
		break;
	}
}

int whc_dev_ota_read(u8 *data, int data_len)
{
	u32 buf_len = 4 + sizeof(struct whc_dev_ota_hdr);
	u8 *buf = rtos_mem_malloc(buf_len);
	u8 *ptr = buf;
	u32 read_len;
	struct whc_dev_ota_readbuf rbuf;

	struct whc_dev_ota_hdr *ota_hdr;

	rbuf.buf = data;

	*(u32 *)ptr = WHC_WIFI_TEST;
	ptr += 4;
	ota_hdr = (struct whc_dev_ota_hdr *)ptr;
	ota_hdr->type = WHC_WIFI_TEST_OTA;
	ota_hdr->subtype = WHC_OTA_READ;
	if (data_len > 1024) {
		ota_hdr->len = 1024;
	} else {
		ota_hdr->len = (u16)data_len;
	}

	/* len from host */
	whc_dev_api_send_to_host_block(buf, buf_len, (u8 *)&rbuf, ota_hdr->len);

	ota_hdr = &(rbuf.ota_hdr);
	read_len = (int16_t)ota_hdr->len;

	rtos_mem_free(buf);
	return read_len;
}

int whc_dev_ota_close(void)
{
	u32 buf_len = 4 + sizeof(struct whc_dev_ota_hdr);
	u8 *buf = rtos_mem_malloc(buf_len);
	u8 *ptr = buf;
	struct whc_dev_ota_hdr *ota_hdr;

	*(u32 *)ptr = WHC_WIFI_TEST;
	ptr += 4;
	ota_hdr = (struct whc_dev_ota_hdr *)ptr;
	ota_hdr->type = WHC_WIFI_TEST_OTA;
	ota_hdr->subtype = WHC_OTA_END;

	whc_dev_api_send_to_host(buf, buf_len);

	return 0;
}

#ifdef CONFIG_RMESH_OTA_EN
void whc_dev_api_rmesh_ota_start(struct rmesh_http_ota_param *ota_param)
{
	u8 *buf = rtos_mem_zmalloc(1024);
	u8 *ptr = buf;
	struct whc_dev_ota_hdr *ota_hdr;
	struct whc_dev_ota_info *ota_info;
	u32 buf_len = 0;

	*(u32 *)ptr = WHC_WIFI_TEST;
	ptr += 4;

	ota_hdr = (struct whc_dev_ota_hdr *)ptr;
	ota_hdr->type = WHC_WIFI_TEST_OTA;
	ota_hdr->subtype = WHC_OTA_START;

	ptr += sizeof(struct whc_dev_ota_hdr);

	ota_info = (struct whc_dev_ota_info *)ptr;
	ota_info->port = (u32)ota_param->port;
	memcpy(ota_info->host, ota_param->host, strlen(ota_param->host));
	memcpy(ota_info->resource, ota_param->resource, strlen(ota_param->resource));

	buf_len = 4 + sizeof(struct whc_dev_ota_hdr) + sizeof(struct whc_dev_ota_info);

	whc_dev_api_send_to_host(buf, buf_len);

	rtos_mem_free(buf);
}
#endif