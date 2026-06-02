/*
 *******************************************************************************
 * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */
#include <stdbool.h>
#include "osif.h"
#include "bt_debug.h"
#include "hci_common.h"
#include "hci_platform.h"
#include <sdn_host.h>
#include <bt_eqc_common.h>

extern void *eqc_event_sem;
extern void *eqc_evt_queue_handle;  /* Event queue handle */
static uint8_t *hci_eqc_buf;
void *eqc_recv_sem = NULL;

static bool g_bt_controller_open = false;

static int eqc_recv(uint8_t type, uint8_t *pdata, uint16_t len)
{
	E_EVENT_MSG event_msg;

	if (!pdata) {
		BT_LOGA("pdata is NULL \n\r");
		return -1;
	}

	if (HCI_EVT == type && (BT_HCI_EVT_CMD_COMPLETE == (*pdata) || BT_HCI_EVT_CMD_STATUS == (*pdata))) {  /* command complete && command status event */
		memcpy(hci_eqc_buf, pdata, len);
		osif_sem_give(eqc_recv_sem);
	} else {
		if (eqc_evt_queue_handle) {
			uint8_t *pbuf = (uint8_t *)osif_mem_alloc(0, len);
			if (!pbuf) {
				BT_LOGE("alloc pbuf fail\n\r");
				return -1;
			}
			memset(pbuf, 0, len);
			memcpy(pbuf, pdata, len);
			event_msg.buf = (void *)pbuf;
			event_msg.len = len;

			if (false == osif_msg_send(eqc_evt_queue_handle, &event_msg, 0)) {
				BT_LOGE("send event queue fail!\n\r");
				osif_mem_free(pbuf);
			}
		} else {
			BT_LOGE("evt_queue_handle is NULL\n\r");
		}
	}
	return 0;
}

void hci_eqc_open(void)
{
	if (g_bt_controller_open) {
		BT_LOGA("EQC:bt controller has already open \n\r");
		return;
	}
	hci_set_mp(true);
	rtk_bt_pre_enable();
	sdn_host_set_bt_cb(eqc_recv);
	if (sdn_host_enable(SDN_INTF_BT)) {
		BT_LOGE("EQC:bt controller open fail\n\r");
		hci_set_mp(false);
		return;
	}

	if (false == osif_sem_create(&eqc_recv_sem, 0, 1)) {
		BT_LOGE("EQC:eqc_recv_sem create fail!\n\r");
		goto failed;
	}

	rtk_bt_post_enable();

	g_bt_controller_open = true;
	return;

failed:
	sdn_host_disable(SDN_INTF_BT);
	g_bt_controller_open = false;
	hci_set_mp(false);
	return;
}

void hci_eqc_close(void)
{
	osif_sem_delete(eqc_recv_sem);
	eqc_recv_sem = NULL;

	sdn_host_disable(SDN_INTF_BT);
	g_bt_controller_open = false;
	hci_set_mp(false);
}

/*command status*/
bool hci_if_eqc_send_status(uint16_t opcode, uint8_t *buf, uint16_t len)
{
	buf[0] = (uint8_t)(opcode >> 0);
	buf[1] = (uint8_t)(opcode >> 8);

	hci_eqc_buf = buf;
	if (sdn_host_send(SDN_INTF_BT, HCI_CMD, buf, len)) {
		return false;
	}

	osif_sem_take(eqc_recv_sem, BT_TIMEOUT_FOREVER);
	hci_eqc_buf = NULL;

	/* Check Resp: OpCode and Status */
	if (buf[4] != (uint8_t)(opcode >> 0) || buf[5] != (uint8_t)(opcode >> 8) || buf[2] != 0x00) {
		BT_LOGE("check command status[0x%x] fail\n\r", buf[2]);
		return false;
	}

	return true;
}

/* command complete */
bool hci_if_eqc_send_comple(uint16_t opcode, uint8_t *buf, uint16_t len)
{
	buf[0] = (uint8_t)(opcode >> 0);
	buf[1] = (uint8_t)(opcode >> 8);

	hci_eqc_buf = buf;
	if (sdn_host_send(SDN_INTF_BT, HCI_CMD, buf, len)) {
		return false;
	}

	osif_sem_take(eqc_recv_sem, BT_TIMEOUT_FOREVER);
	hci_eqc_buf = NULL;

	/* Check Resp: OpCode and Status */
	if (buf[3] != (uint8_t)(opcode >> 0) || buf[4] != (uint8_t)(opcode >> 8) || buf[5] != 0x00) {
		BT_LOGE("check command complete[0x%x] fail\n\r", buf[5]);
		return false;
	}

	return true;
}