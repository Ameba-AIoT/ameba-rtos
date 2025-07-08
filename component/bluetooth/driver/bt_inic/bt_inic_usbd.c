#include <platform_autoconf.h>
#include "usbd.h"
#include "usbd_inic.h"
#include "osif.h"
#include "bt_inic.h"
#include "bt_debug.h"

// This configuration is used to enable a thread to check hotplug event
// and reset USB stack to avoid memory leak, only for example.
#define CONFIG_USBD_INIC_HOTPLUG                        0

#define USBD_INIC_BT_EP1_INTR_IN_BUF_SIZE               256U   /* BT EP1 INTR IN buffer size */
#define USBD_INIC_BT_EP2_BULK_IN_BUF_SIZE               512U  /* BT EP2 BULK IN buffer size */
#define USBD_INIC_BT_EP2_BULK_OUT_BUF_SIZE              512U  /* BT EP2 BULK OUT buffer size */

// Thread priorities
#define CONFIG_USBD_INIC_INIT_THREAD_PRIORITY       5
#define CONFIG_USBD_INIC_HOTPLUG_THREAD_PRIORITY    8
#define CONFIG_USBD_INIC_XFER_THREAD_PRIORITY       6
#define CONFIG_USBD_INIC_RESET_THREAD_PRIORITY      6

// Vendor requests
#define USBD_INIC_VENDOR_REQ_BT_HCI_CMD             0x00U
#define USBD_INIC_VENDOR_REQ_FW_DOWNLOAD            0xF0U
#define USBD_INIC_VENDOR_QUERY_CMD                  0x01U
#define USBD_INIC_VENDOR_QUERY_ACK                  0x81U
#define USBD_INIC_VENDOR_RESET_CMD                  0x06U
#define USBD_INIC_VENDOR_RESET_ACK                  0x86U

#define USBD_INIC_FW_TYPE_ROM                       0xF0U
#define USBD_INIC_FW_TYPE_RAM                       0xF1U

typedef struct {
	// DWORD 0
	u32 data_len: 16;       // Data payload length
	u32 data_offset: 8;     // Data payload offset i.e. header length
	u32 data_checksum: 8;   // Checksum of the data payload

	// DWORD 1
	u32 pkt_type: 8;        // Packet type
	u32 xfer_status: 8;     // Xfer status
	u32 rl_version: 8;      // RL Version
	u32 dev_mode: 8;        // Device mode

	// DWORD 2
	u32 mem_addr;           // Memory address

	// DWORD 3
	u32 mem_size;           // Memory size

	// DWORD 4
	union {
		u32 d32;
		u16 d16[2];
		u8  d8[4];
	} value;                // Target value

	// DWORD 5
	u32 reserved;
} usbd_inic_query_packet_t;

typedef struct {
	u8 *buf;
	u16 buf_len;
} usbd_inic_app_ep_t;

typedef struct {
	usbd_inic_app_ep_t in_ep[USB_MAX_ENDPOINTS];
	usbd_inic_app_ep_t out_ep[USB_MAX_ENDPOINTS];
} usbd_inic_app_t;

#define USBD_INIC_QUERY_PACKET_SIZE         (sizeof(usbd_inic_query_packet_t))

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
	.dma_enable = 1U,
	.isr_priority = INT_PRI_MIDDLE,
#if defined (CONFIG_AMEBAGREEN2)
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

/* INIC Device */
static usbd_inic_app_t usbd_inic_app;
static void *reset_sema;

#if CONFIG_USBD_INIC_HOTPLUG
static u8 inic_attach_status;
static void *inic_attach_status_changed_sema;
#endif

/* Private functions ---------------------------------------------------------*/

static u8 inic_setup_handle_hci_cmd(usb_setup_req_t *req, u8 *buf)
{
	// Handle BT HCI commands
	bt_inic_usb_hci_cmd_hdl(buf, req->wLength);

	return HAL_OK;
}

static int inic_setup_handle_query(usb_setup_req_t *req, u8 *buf)
{
	int ret = HAL_ERR_PARA;
	usbd_inic_query_packet_t *pkt;

	if (req->wIndex == USBD_INIC_VENDOR_QUERY_CMD) {
		pkt = (usbd_inic_query_packet_t *)buf;
		pkt->data_len = 0;
		pkt->data_offset = USBD_INIC_QUERY_PACKET_SIZE;
		pkt->pkt_type = USBD_INIC_VENDOR_QUERY_ACK;
		pkt->xfer_status = HAL_OK;
		pkt->rl_version = (u8)(SYSCFG_RLVersion() & 0xFF);
		pkt->dev_mode = USBD_INIC_FW_TYPE_RAM;
		ret = HAL_OK;
	} else if (req->wIndex == USBD_INIC_VENDOR_RESET_CMD) {
		pkt = (usbd_inic_query_packet_t *)buf;
		pkt->data_len = 0;
		pkt->data_offset = USBD_INIC_QUERY_PACKET_SIZE;
		pkt->pkt_type = USBD_INIC_VENDOR_RESET_ACK;
		pkt->xfer_status = HAL_OK;
		pkt->rl_version = (u8)(SYSCFG_RLVersion() & 0xFF);
		pkt->dev_mode = USBD_INIC_FW_TYPE_RAM;
		osif_sem_give(reset_sema);
		ret = HAL_OK;
	}

	return ret;
}

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
		ret = inic_setup_handle_hci_cmd(req, buf);
		break;
	case USBD_INIC_VENDOR_REQ_FW_DOWNLOAD:
		ret = inic_setup_handle_query(req, buf);
		break;
	default:
		break;
	}

	return ret;
}

static void inic_bt_deinit(void)
{
	usbd_inic_app_t *iapp = &usbd_inic_app;
	usbd_inic_app_ep_t *ep;

	ep = &iapp->in_ep[USB_EP_NUM(USBD_INIT_BT_EP1_INTR_IN)];
	if (ep->buf != NULL) {
		usb_os_mfree(ep->buf);
		ep->buf = NULL;
	}

	ep = &iapp->in_ep[USB_EP_NUM(USBD_INIC_BT_EP2_BULK_IN)];
	if (ep->buf != NULL) {
		usb_os_mfree(ep->buf);
		ep->buf = NULL;
	}

	ep = &iapp->out_ep[USB_EP_NUM(USBD_INIC_BT_EP2_BULK_OUT)];
	if (ep->buf != NULL) {
		usb_os_mfree(ep->buf);
		ep->buf = NULL;
	}
}

static int inic_bt_init(void)
{
	int ret = HAL_OK;
	usbd_inic_app_t *iapp = &usbd_inic_app;
	usbd_inic_app_ep_t *ep;
	u8 ep_num;

	ep_num = USB_EP_NUM(USBD_INIT_BT_EP1_INTR_IN);
	ep = &iapp->in_ep[ep_num];
	ep->buf_len = USBD_INIC_BT_EP1_INTR_IN_BUF_SIZE;
	ep->buf = (u8 *)usb_os_malloc(ep->buf_len);
	if (ep->buf == NULL) {
		ret = HAL_ERR_MEM;
		goto bt_init_exit;
	}

	ep_num = USB_EP_NUM(USBD_INIC_BT_EP2_BULK_IN);
	ep = &iapp->in_ep[ep_num];
	ep->buf_len = USBD_INIC_BT_EP2_BULK_IN_BUF_SIZE;
	ep->buf = (u8 *)usb_os_malloc(ep->buf_len);
	if (ep->buf == NULL) {
		ret = HAL_ERR_MEM;
		goto bt_init_clean_ep1_intr_in_buf_exit;
	}

	ep_num = USB_EP_NUM(USBD_INIC_BT_EP2_BULK_OUT);
	ep = &iapp->out_ep[ep_num];
	ep->buf_len = USBD_INIC_BT_EP2_BULK_OUT_BUF_SIZE;
	ep->buf = (u8 *)usb_os_malloc(ep->buf_len);
	if (ep->buf == NULL) {
		ret = HAL_ERR_MEM;
		goto bt_init_clean_ep2_bulk_in_buf_exit;
	}

	return HAL_OK;

bt_init_clean_ep2_bulk_in_buf_exit:
	ep = &iapp->in_ep[USB_EP_NUM(USBD_INIC_BT_EP2_BULK_IN)];
	usb_os_mfree(ep->buf);
	ep->buf = NULL;

bt_init_clean_ep1_intr_in_buf_exit:
	ep = &iapp->in_ep[USB_EP_NUM(USBD_INIT_BT_EP1_INTR_IN)];
	usb_os_mfree(ep->buf);
	ep->buf = NULL;

bt_init_exit:
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
		ret = inic_bt_init();
		if (ret != HAL_OK) {
			goto init_exit;
		}
		bt_inic_usb_init();
	}
	osif_sem_create(&reset_sema, 0, 1);

	return HAL_OK;
init_exit:
	return ret;
}

/**
  * @brief  DeInitializes inic application layer
  * @param  None
  * @retval Status
  */
static int inic_cb_deinit(void)
{
	osif_sem_delete(reset_sema);

	if (usbd_inic_is_bt_en()) {
		bt_inic_usb_deinit();
		inic_bt_deinit();
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
	usbd_inic_app_t *iapp = &usbd_inic_app;
	usbd_inic_app_ep_t *ep;

	// Prepare to RX

	if (usbd_inic_is_bt_en()) {
		ep = &iapp->out_ep[USB_EP_NUM(USBD_INIC_BT_EP2_BULK_OUT)];
		usbd_inic_receive_data(USBD_INIC_BT_EP2_BULK_OUT, ep->buf, ep->buf_len, NULL);
	}
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

static void inic_reset_thread(void *param)
{
	UNUSED(param);
	while (osif_sem_take(reset_sema, BT_TIMEOUT_FOREVER)) {
		osif_delay(500); // Wait reset request done
		System_Reset();
	}
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
	void *reset_task = NULL;

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

	if (!osif_task_create(&reset_task, "inic_reset_thread", inic_reset_thread, NULL, 1024, CONFIG_USBD_INIC_RESET_THREAD_PRIORITY)) {
		BT_LOGE("Create inic_reset_thread Err!!\n");
		goto clear_class_exit;
	}

#if CONFIG_USBD_INIC_HOTPLUG
	if (!osif_task_create(&hotplug_task, "inic_hotplug_thread", inic_hotplug_thread, NULL, 1024, CONFIG_USBD_INIC_HOTPLUG_THREAD_PRIORITY)) {
		BT_LOGE("Create inic_hotplug_thread Err!!\n");
		goto clear_reset_task;
	}
#endif // CONFIG_USBD_INIC_HOTPLUG

	osif_delay(100);

	BT_LOGA("BT USBD INIC demo start\n");

	osif_task_delete(NULL);

	return;

#if CONFIG_USBD_INIC_HOTPLUG
clear_reset_task:
	osif_task_delete(reset_task);
#endif

clear_class_exit:
	usbd_inic_deinit();

clear_usb_driver_exit:
	usbd_deinit();

exit:
	BT_LOGA("BT USBD INIC demo stop\n");
#if CONFIG_USBD_INIC_HOTPLUG
	osif_sem_delete(inic_attach_status_changed_sema);
#endif

	osif_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

void bt_usbd_inic_init(void)
{
	if (!osif_task_create(NULL, "bt_usbd_inic_thread", bt_usbd_inic_thread, NULL, 1024 * 4, CONFIG_USBD_INIC_INIT_THREAD_PRIORITY)) {
		BT_LOGE("Create bt_usbd_inic_thread Err!!\n");
		return;
	}
	return;
}