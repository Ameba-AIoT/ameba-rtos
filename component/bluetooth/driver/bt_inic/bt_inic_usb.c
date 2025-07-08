// #include "usbd.h"
#include "usbd_inic.h"
#include "osif.h"
#include "bt_inic.h"
#include "hci_if_inic.h"
#include "bt_debug.h"

#define BT_INIC_USB_STATUS_ACTIVE                       0
#define BT_INIC_USB_STATUS_SUSPEND                      1
#define BT_INIC_USB_STATUS_DISABLED                     2

#define BT_INIC_USB_WAKE_HOST_MAX_WAIT_CNT              10
#define BT_INIC_USB_WAKE_HOST_UNIT                      10

#define DEV_DMA_ALIGN                   4

struct bt_inic_usb_priv {
	void *rx_queue;
	u8 rx_req_flag;
	void *evt_tx_lock;
	void *evt_tx_done_sema;
	void *acl_tx_lock;
	void *acl_tx_done_sema;
};

enum {
	USB_CLOSED = 0,  /* BT host uart is closed. */
	USB_RUNNING,     /* BT host uart is running. */
	USB_CLOSING,     /* BT host uart is closing. */
};

static struct bt_inic_usb_priv usb_priv = {0};
static u8 bt_inic_usb_status = BT_INIC_USB_STATUS_ACTIVE;

void bt_inic_usb_tx_data(u8 type, u8 *data, u32 len)
{
	u8 i = 0;
	if (bt_inic_usb_status == BT_INIC_USB_STATUS_DISABLED) {
		return;
	}

	if (bt_inic_usb_status == BT_INIC_USB_STATUS_SUSPEND) {
		usbd_wake_host();
		for (i = 0; i < BT_INIC_USB_WAKE_HOST_MAX_WAIT_CNT; i++) {
			osif_delay(BT_INIC_USB_WAKE_HOST_UNIT);
			if (bt_inic_usb_status == BT_INIC_USB_STATUS_ACTIVE) {
				break;
			}
		}
	}

	if (i == BT_INIC_USB_WAKE_HOST_MAX_WAIT_CNT) {
		return;
	}

	switch (type) {
	case HCI_EVT:
		BT_DUMPA("HOST RX EVT: ", data, len);
		osif_mutex_take(usb_priv.evt_tx_lock, BT_TIMEOUT_FOREVER);
		osif_sem_take(usb_priv.evt_tx_done_sema, BT_TIMEOUT_FOREVER);

		usbd_inic_transmit_data(USBD_INIT_BT_EP1_INTR_IN, data, len, NULL);
		osif_mutex_give(usb_priv.evt_tx_lock);
		break;
	case HCI_ACL:
		BT_DUMPA("HOST RX ACL: ", data, len);
		osif_mutex_take(usb_priv.acl_tx_lock, BT_TIMEOUT_FOREVER);
		osif_sem_take(usb_priv.acl_tx_done_sema, BT_TIMEOUT_FOREVER);

		usbd_inic_transmit_data(USBD_INIC_BT_EP2_BULK_IN, data, len, NULL);
		osif_mutex_give(usb_priv.acl_tx_lock);
		break;
	default:
		break;
	}

}

void bt_inic_send_to_host(u8 type, u8 *pdata, u32 len)
{
	u8 *buf = NULL;
	u8 *data;

	buf = osif_mem_alloc(RAM_TYPE_DATA_ON, len + DEV_DMA_ALIGN);
	if (!(buf)) {
		return;
	}
	data = (u8 *)N_BYTE_ALIGMENT((u32)buf, DEV_DMA_ALIGN);
	memcpy(data, pdata, len);//BLE Packet data

	bt_inic_usb_tx_data(type, data, len);
	return;
}

u8 bt_inic_usb_hci_cmd_hdl(u8 *buf, u16 len)
{
	BT_DUMPA("HOST TX CMD: ", buf, len);
	u8 *rx_cmd;

	rx_cmd = (u8 *)osif_mem_alloc(RAM_TYPE_DATA_ON, len + 3);
	if (!rx_cmd) {
		BT_LOGE("Alloc memory failed\n");
		return 0;
	}

	rx_cmd[0] = len & 0xff; /* Packet Length */
	rx_cmd[1] = (len >> 8) & 0xff;
	rx_cmd[2] = HCI_CMD;

	memcpy(rx_cmd + 3, buf, len);
	osif_msg_send(usb_priv.rx_queue, &rx_cmd, BT_TIMEOUT_NONE);
	return 1;
}

u8 bt_inic_usb_hci_acl_hdl(u8 *buf, u16 len)
{
	BT_DUMPA("HOST TX ACL: ", buf, len);
	u8 *rx_acl;

	rx_acl = (u8 *)osif_mem_alloc(RAM_TYPE_DATA_ON, len + 3);
	if (!rx_acl) {
		BT_LOGE("Alloc memory failed\n");
		return 0;
	}

	rx_acl[0] = len & 0xff; /* Packet Length */
	rx_acl[1] = (len >> 8) & 0xff;
	rx_acl[2] = HCI_ACL;

	memcpy(rx_acl + 3, buf, len);
	osif_msg_send(usb_priv.rx_queue, &rx_acl, BT_TIMEOUT_NONE);
	return 1;
}

void bt_inic_usb_suspend_cb(void)
{
	bt_inic_usb_status = BT_INIC_USB_STATUS_SUSPEND;
}

void bt_inic_usb_resume_cb(void)
{
	bt_inic_usb_status = BT_INIC_USB_STATUS_ACTIVE;
}

void bt_inic_status_change_cb(u8 status)
{
	if (status == USBD_ATTACH_STATUS_ATTACHED) {
		bt_inic_usb_status = BT_INIC_USB_STATUS_ACTIVE;
	} else if (status == USBD_ATTACH_STATUS_DETACHED) {
		bt_inic_usb_status = BT_INIC_USB_STATUS_DISABLED;
	}
}

static void _usb_rx_thread(void *pData)
{
	(void)pData;
	uint8_t *buf;
	uint16_t pkt_len;
	while (osif_msg_recv(usb_priv.rx_queue, &buf, BT_TIMEOUT_FOREVER)) {
		if (usb_priv.rx_req_flag != USB_RUNNING) {
			break;
		}

		if (buf) {
			pkt_len = *((u16 *)buf);
			bt_inic_recv_from_host(*(buf + 2), buf + 3, pkt_len);
			osif_mem_free(buf);
		}
	}
	usb_priv.rx_req_flag = USB_CLOSED;
	osif_task_delete(NULL);
}

static bool _usb_rx_queue_init(void)
{
	osif_msg_queue_create(&usb_priv.rx_queue, 32, sizeof(u8 *));

	usb_priv.rx_req_flag = USB_RUNNING;
	if (!osif_task_create(NULL, "USB_BT_RX_THREAD", _usb_rx_thread, (void *)(&usb_priv), 1024 * 4, 7)) {
		BT_LOGE("Create USB_RX_THREAD Err!!\n");
		return FALSE;
	}

	return TRUE;
}

static void _usb_rx_queue_deinit(void)
{
	uint8_t *buf = NULL;

	usb_priv.rx_req_flag = USB_CLOSING;
	osif_msg_send(usb_priv.rx_queue, &buf, BT_TIMEOUT_FOREVER);

	while (usb_priv.rx_req_flag != USB_CLOSED) {
		osif_delay(5);
	}

	osif_msg_queue_delete(usb_priv.rx_queue);
	usb_priv.rx_queue = NULL;
}

void bt_inic_usb_evt_txdone_cb(u8 *buf)
{
	osif_mem_free(buf);
	osif_sem_give(usb_priv.evt_tx_done_sema);
}

void bt_inic_usb_acl_txdone_cb(u8 *buf)
{
	osif_mem_free(buf);
	osif_sem_give(usb_priv.acl_tx_done_sema);
}

void bt_inic_usb_init(void)
{
	_usb_rx_queue_init();
	osif_mutex_create(&usb_priv.evt_tx_lock);
	osif_mutex_create(&usb_priv.acl_tx_lock);
	osif_sem_create(&usb_priv.evt_tx_done_sema, 0, 1);
	osif_sem_create(&usb_priv.acl_tx_done_sema, 0, 0xFFFFFFFF);
	osif_sem_give(usb_priv.evt_tx_done_sema);
	osif_sem_give(usb_priv.acl_tx_done_sema);
	BT_LOGA("USB device init done!\n");
}

void bt_inic_usb_deinit(void)
{
	_usb_rx_queue_deinit();
	osif_sem_delete(usb_priv.evt_tx_done_sema);
	usb_priv.evt_tx_done_sema = NULL;
	osif_sem_delete(usb_priv.acl_tx_done_sema);
	usb_priv.acl_tx_done_sema = NULL;
	osif_mutex_delete(usb_priv.evt_tx_lock);
	usb_priv.evt_tx_lock = NULL;
	osif_mutex_delete(usb_priv.acl_tx_lock);
	usb_priv.acl_tx_lock = NULL;
	BT_LOGA("USB device deinit done!\n");
}