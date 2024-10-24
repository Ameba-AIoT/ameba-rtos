/******************************************************************************
 *
 * Copyright(c) 2007 - 2015 Realtek Corporation. All rights reserved.
 *
 *
 ******************************************************************************/
#include "platform_autoconf.h"
#include "lwip_netconf.h"
#include "wifi_intf_drv_to_upper.h"
#include "example_wifi_raw.h"
#include "os_wrapper.h"


u8 addr1[6] = {0xBC, 0x46, 0x99, 0x7B, 0x48, 0x74};
u8 addr2[6] = {0x00, 0xE0, 0x4C, 0x00, 0x02, 0xF0};
u8 addr3[6] = {0xBC, 0x46, 0x99, 0x7B, 0x48, 0x74};

void wifi_raw_tx(void *param)
{
	struct raw_frame_desc_t tx_raw_data_desc = {0};
	UNUSED(param);

	printf("%s start.\n", __func__);

	unsigned char tx_frame_buf[42] = {
		0x40, 0x00, 												// Frame Control field
		0x00, 0x00, 												// Duration/ID field
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 						// Address 1, should be the mac address of destination
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 						// Address 2, should be the mac address of myself as master
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 						// Address 3, shoule be the BSSID
		0x00, 0x00, 												// Sequence Control field
		0x00, 0x00,													// SSID field
		0x01, 0x08, 0x82, 0x84, 0x8b, 0x96, 0x8c, 0x12, 0x98, 0x24,	// Supported Rates field
		0x32, 0x04, 0xb0, 0x48, 0x60, 0x6c							// Extended Supported Rates field
	};

	memcpy(&tx_frame_buf[4], addr1, 6);
	memcpy(&tx_frame_buf[10], addr2, 6);
	memcpy(&tx_frame_buf[16], addr3, 6);

	tx_raw_data_desc.wlan_idx = STA_WLAN_INDEX;
	tx_raw_data_desc.buf = tx_frame_buf;
	tx_raw_data_desc.buf_len = 42;

	while (1) {
		if (wifi_send_raw_frame(&tx_raw_data_desc) < 0) {
			printf("%s failed!\n", __func__);
		}
		rtos_time_delay_ms(100);
	}

	rtos_task_delete(NULL);
}

static u8 promisc_callback(struct rx_pkt_info *pkt_info)
{
	u8 *buf = pkt_info->buf;
	u8 subtype;
	u32 buf_len = pkt_info->len;
	u32 i;

	subtype = GetFrameSubType(buf);

	if (subtype == WIFI_PROBERSP) {
		printf("Probe Rsp frame:\n");
		for (i = 0; i < buf_len; i++) {
			printf("%x ", buf[i]);
		}
		printf("\n\n");
		return 1;
	}

	return 0;
}
void wifi_raw_rx(void *param)
{
	UNUSED(param);
	struct _promisc_para_t promiscpara;

	printf("%s start.\n", __func__);

	memset(&promiscpara, 0, sizeof(struct _promisc_para_t));
	promiscpara.filter_mode = RCR_ALL_PKT;
	promiscpara.callback = promisc_callback;
	wifi_promisc_enable(ENABLE, &promiscpara);

	while (1) {
		rtos_time_delay_ms(10);
	}

	rtos_task_delete(NULL);
}


void wifi_raw_task(void *param)
{
	UNUSED(param);

	while (!((wifi_get_join_status() == RTW_JOINSTATUS_SUCCESS) && (*(u32 *)LwIP_GetIP(0) != IP_ADDR_INVALID))) {
		printf("Wait for WIFI connection ...\n");
		rtos_time_delay_ms(1000);
	}

	if (rtos_task_create(NULL, ((const char *)"wifi_raw_tx_task"), wifi_raw_tx, NULL, 1024 * 4, 4) != SUCCESS) {
		printf("\n\r[%s] Create wifi raw tx task failed", __FUNCTION__);
		goto exit;
	}

	if (rtos_task_create(NULL, ((const char *)"wifi_raw_rx_task"), wifi_raw_rx, NULL, 1024 * 4, 4) != SUCCESS) {
		printf("\n\r[%s] Create wifi raw rx task failed", __FUNCTION__);
		goto exit;
	}

exit:
	rtos_task_delete(NULL);
}


void example_wifi_raw(void)
{
	if (rtos_task_create(NULL, ((const char *)"wifi_raw_task"), wifi_raw_task, NULL, 1024 * 4, 1) != SUCCESS) {
		printf("\n\r[%s] Create wifi raw task failed", __FUNCTION__);
	}
}


