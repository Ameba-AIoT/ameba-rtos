/*
 *******************************************************************************
 * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include "osif.h"
#include "hci/hci_if_inic.h"
#include "hci/hci_process.h"
#include "hci/hci_transport.h"
#include "hci_uart.h"
#include "hci_platform.h"
#include "bt_debug.h"
#include "hci_common.h"

#define H4_HDR_LEN          (1)

static u8 b_hci_drv_initialized = 0;

#if 0
static bool bt_inic_get_buf(hci_rx_t *info, uint32_t timeout)
{
	(void)timeout;
	u8 *buf = NULL;

	buf = (u8 *)osif_mem_aligned_alloc(RAM_TYPE_DATA_ON, info->len, 4);
	memset(buf, 0, info->len);

	info->data = buf;
	info->arg = buf;
	return true;
}

static bt_inic_free_buf(hci_rx_t *info)
{
	osif_mem_aligned_free((u8 *)info->arg);
}

static uint8_t hci_if_recv(hci_rx_t *info)
{
	u8 *buf = (u8 *)info->arg;

	bt_inic_send_pkt(info->type, buf, info->len);
	osif_mem_aligned_free(buf);

	return HCI_SUCCESS;
}

bool hci_if_open(void)
{
	if (b_hci_drv_initialized) {
		BT_LOGE("hci drv has been initialized! \r\n");
		return true;
	}

	/* BT Board Init */
	if (HCI_FAIL == hci_platform_init()) {
		BT_LOGE("hci_platform_init fail!\r\n");
		return false;
	}

	/* HCI Transport */
	if (HCI_FAIL == hci_transport_open()) {
		BT_LOGE("hci_transport_open fail!\r\n");
		return false;
	}

	/* HCI Transport Bridge to StandAlone */
	hci_transport_set_buf_ops(hci_sa_recv_get_buf, NULL);
	hci_transport_set_recv(hci_sa_recv);

	/* HCI UART Bridge to Transport */
	hci_uart_set_rx_ind(hci_transport_recv_ind);

	if (HCI_FAIL == hci_process()) {
		BT_LOGE("hci_process fail!\r\n");
		return false;
	}

	/* HCI Transport Bridge to RTK Stack
	 * (Stop and Start rx_ind for this Moment)
	 */
	hci_uart_set_rx_ind(NULL);
	hci_transport_set_buf_ops(bt_inic_get_buf, bt_inic_free_buf);
	hci_transport_set_recv(hci_if_recv);
	hci_uart_set_rx_ind(hci_transport_recv_ind);

	b_hci_drv_initialized = 1;
	rtk_bt_post_enable();

	return true;
}

bool hci_if_close(void)
{
	if (!b_hci_drv_initialized) {
		return true;
	}

	if (HCI_FAIL == hci_platform_deinit() || /* Platform Deinit First */
		HCI_FAIL == hci_transport_close()) { /* HCI Transport Close */
		return false;
	}

	b_hci_drv_initialized = 0;

	return true;
}

void hci_if_send(u8 type, u8 *buf, u32 len)
{
	hci_transport_send(type, buf, len, 1);
}

#else
bool hci_if_open(void)
{
	return false;
}

bool hci_if_close(void)
{
	return true;
}

void hci_if_send(u8 type, u8 *buf, u32 len)
{
	(void)type;
	(void)buf;
	(void)len;
}


#endif
