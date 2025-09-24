#include <platform_autoconf.h>
#include "usbd.h"
#include "usbd_inic.h"
#include "osif.h"
#include "bt_inic.h"
#include "bt_debug.h"

#if !(defined(CONFIG_WHC_INTF_USB) && CONFIG_WHC_INTF_USB)
// This configuration is used to enable a thread to check hotplug event
// and reset USB stack to avoid memory leak, only for example.
#define CONFIG_USBD_INIC_HOTPLUG                        0

// Thread priorities
#define CONFIG_USBD_INIC_INIT_THREAD_PRIORITY       5
#define CONFIG_USBD_INIC_HOTPLUG_THREAD_PRIORITY    8
#define CONFIG_USBD_INIC_XFER_THREAD_PRIORITY       6
/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int inic_cb_init(void);
static int inic_cb_deinit(void);
static int inic_cb_setup(usb_setup_req_t *req, u8 *buf);
static int inic_cb_set_config(void);
static int inic_cb_clear_config(void);
static int inic_cb_received(usbd_inic_ep_t *ep, u16 len);
static void inic_cb_transmitted(usbd_inic_ep_t *ep, u8 status);
static void inic_cb_status_changed(u8 old_status, u8 status);
static void inic_cb_suspend(void);
static void inic_cb_resume(void);

/* Private variables ---------------------------------------------------------*/

static usbd_config_t inic_cfg = {
	.speed = USB_SPEED_HIGH,
	.isr_priority = INT_PRI_MIDDLE,
#if defined(CONFIG_AMEBAGREEN2)
	.rx_fifo_depth = 292U,
	.ptx_fifo_depth = {16U, 256U, 32U, 256U, 128U, },
#endif
};

static usbd_inic_cb_t inic_cb = {
	.init = inic_cb_init,
	.deinit = inic_cb_deinit,
	.setup = inic_cb_setup,
	.set_config = inic_cb_set_config,
	.clear_config = inic_cb_clear_config,
	.received = inic_cb_received,
	.transmitted = inic_cb_transmitted,
	.status_changed = inic_cb_status_changed,
	.suspend = inic_cb_suspend,
	.resume = inic_cb_resume,
};

#if CONFIG_USBD_INIC_HOTPLUG
static u8 inic_attach_status;
static void *inic_attach_status_changed_sema;
#endif

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Handle the inic class control requests
  * @param  cmd: Command code
  * @param  buf: Buffer containing command data (request parameters)
  * @param  len: Number of data to be sent (in bytes)
  * @param  value: Value for the command code
  * @retval Status
  */
static int inic_cb_setup(usb_setup_req_t *req, u8 *buf)
{
	int ret = HAL_ERR_PARA;
	switch (req->bRequest) {
	case USBD_INIC_VENDOR_REQ_BT_HCI_CMD:
		ret = bt_inic_usb_hci_cmd_hdl(buf, req->wLength);
		break;
	default:
		break;
	}

	return ret;
}

/**
  * @brief  Initializes inic application layer
  * @param  None
  * @retval Status
  */
static int inic_cb_init(void)
{
	int ret = HAL_OK;

	if (usbd_inic_is_bt_en()) {
		ret = bt_inic_usb_init();
	}

	return ret;
}


/**
  * @brief  DeInitializes inic application layer
  * @param  None
  * @retval Status
  */
static int inic_cb_deinit(void)
{
	if (usbd_inic_is_bt_en()) {
		bt_inic_usb_deinit();
	}
	return HAL_OK;
}

/**
  * @brief  Set config callback
  * @param  None
  * @retval Status
  */
static int inic_cb_set_config(void)
{
	bt_inic_set_config();

	return HAL_OK;
}


/**
  * @brief  Clear config callback
  * @param  None
  * @retval Status
  */
static int inic_cb_clear_config(void)
{
	return HAL_OK;
}

/**
  * @brief  Data received over USB BULK OUT endpoint
  * @param  buf: RX buffer
  * @param  len: RX data length (in bytes)
  * @retval Status
  */
static int inic_cb_received(usbd_inic_ep_t *out_ep, u16 len)
{
	usbd_ep_t *ep = &out_ep->ep;

	bt_inic_usb_hci_acl_hdl(ep->xfer_buf, len);
	return HAL_OK;
}

static void inic_cb_transmitted(usbd_inic_ep_t *in_ep, u8 status)
{
	(void)status;
	usbd_ep_t *ep = &in_ep->ep;

	switch (ep->addr) {
	case USBD_INIT_BT_EP1_INTR_IN:
		// TBD
		bt_inic_usb_evt_txdone_cb(ep->xfer_buf);
		break;
	case USBD_INIC_BT_EP2_BULK_IN:
		bt_inic_usb_acl_txdone_cb(ep->xfer_buf);
		// TBD
		break;
	default:
		break;
	}
}

static void inic_cb_status_changed(u8 old_status, u8 status)
{
	BT_LOGA("Status change: %d -> %d \n", old_status, status);
#if CONFIG_USBD_INIC_HOTPLUG
	inic_attach_status = status;
	osif_sem_give(inic_attach_status_changed_sema);
#endif
	bt_inic_status_change_cb(status);
}

static void inic_cb_suspend(void)
{
	// TODO: Mark suspend status, stop USB transfer
	bt_inic_usb_suspend_cb();
}

static void inic_cb_resume(void)
{
	// TODO: Unmark suspend status, re-start USB transfer
	bt_inic_usb_resume_cb();
}

#if CONFIG_USBD_INIC_HOTPLUG
static void inic_hotplug_thread(void *param)
{
	int ret = 0;

	UNUSED(param);


	while (osif_sem_take(inic_attach_status_changed_sema, BT_TIMEOUT_FOREVER)) {
		if (inic_attach_status == USBD_ATTACH_STATUS_DETACHED) {
			BT_LOGA("DETACHED\n");
			usbd_inic_deinit();
			ret = usbd_deinit();
			if (ret != 0) {
				break;
			}
			BT_LOGA("Free heap: 0x%x\n", osif_mem_peek(RAM_TYPE_DATA_ON));

			ret = usbd_init(&inic_cfg);
			if (ret != 0) {
				break;
			}
			ret = usbd_inic_init(&inic_cb);
			if (ret != 0) {
				usbd_deinit();
				break;
			}
		} else if (inic_attach_status == USBD_ATTACH_STATUS_ATTACHED) {
			BT_LOGA("ATTACHED\n");
		} else {
			BT_LOGA("INIT\n");
		}
	}

	BT_LOGE("Hotplug thread fail\n");
	osif_task_delete(NULL);
}
#endif // CONFIG_USBD_INIC_HOTPLUG

static void bt_usbd_inic_thread(void *param)
{
	UNUSED(param);
	int ret = 0;
#if CONFIG_USBD_INIC_HOTPLUG
	void *hotplug_task = NULL;
#endif

#if CONFIG_USBD_INIC_HOTPLUG
	osif_sem_create(&inic_attach_status_changed_sema, 0, 1);
#endif

	ret = usbd_init(&inic_cfg);
	if (ret != HAL_OK) {
		goto exit;
	}

	ret = usbd_inic_init(&inic_cb);
	if (ret != HAL_OK) {
		goto clear_usb_driver_exit;
	}

#if CONFIG_USBD_INIC_HOTPLUG
	if (!osif_task_create(&hotplug_task, "inic_hotplug_thread", inic_hotplug_thread, NULL, 1024, CONFIG_USBD_INIC_HOTPLUG_THREAD_PRIORITY)) {
		BT_LOGE("Create inic_hotplug_thread Err!!\n");
		goto clear_class_exit;
	}
#endif // CONFIG_USBD_INIC_HOTPLUG

	osif_delay(100);

	BT_LOGA("BT USBD INIC demo start\n");

	osif_task_delete(NULL);

	return;

#if CONFIG_USBD_INIC_HOTPLUG
clear_class_exit:
	usbd_inic_deinit();
#endif

clear_usb_driver_exit:
	usbd_deinit();

exit:
	BT_LOGA("BT USBD INIC demo stop\n");
#if CONFIG_USBD_INIC_HOTPLUG
	osif_sem_delete(inic_attach_status_changed_sema);
#endif

	osif_task_delete(NULL);
}
#endif

/* Exported functions --------------------------------------------------------*/

void bt_usbd_inic_init(void)
{
#if defined(CONFIG_WHC_INTF_USB) && CONFIG_WHC_INTF_USB
	BT_LOGA("BT USBD INIC demo start\n");
#else
	if (!osif_task_create(NULL, "bt_usbd_inic_thread", bt_usbd_inic_thread, NULL, 1024 * 4, CONFIG_USBD_INIC_INIT_THREAD_PRIORITY)) {
		BT_LOGE("Create bt_usbd_inic_thread Err!!\n");
		return;
	}
#endif
	return;
}
