/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */

#include <platform_autoconf.h>
#include "usbd.h"
#include "usbd_inic.h"
#include "os_wrapper.h"

/* Private defines -----------------------------------------------------------*/

// This configuration is used to enable a thread to check hotplug event
// and reset USB stack to avoid memory leak, only for example.
#define CONFIG_USBD_INIC_HOTPLUG					1

#define USBD_INIC_BULK_BUF_SIZE							64U

// Thread priorities
#define CONFIG_USBD_INIC_INIT_THREAD_PRIORITY		5
#define CONFIG_USBD_INIC_HOTPLUG_THREAD_PRIORITY	8
#define CONFIG_USBD_INIC_XFER_THREAD_PRIORITY		6
#define CONFIG_USBD_INIC_RESET_THREAD_PRIORITY		6

// Vendor requests
#define USBD_INIC_VENDOR_REQ_FW_DOWNLOAD			0xF0U
#define USBD_INIC_VENDOR_QUERY_CMD					0x01U
#define USBD_INIC_VENDOR_QUERY_ACK					0x81U
#define USBD_INIC_VENDOR_RESET_CMD					0x06U
#define USBD_INIC_VENDOR_RESET_ACK					0x86U

#define USBD_INIC_FW_TYPE_ROM						0xF0U
#define USBD_INIC_FW_TYPE_RAM						0xF1U

/* Private types -------------------------------------------------------------*/
static const char *const TAG = "INIC";
typedef struct {
	// DWORD 0
	u32	data_len: 16;		// Data payload length
	u32	data_offset: 8;		// Data payload offset i.e. header length
	u32	data_checksum: 8;	// Checksum of the data payload

	// DWORD 1
	u32	pkt_type: 8;		// Packet type
	u32	xfer_status: 8;		// Xfer status
	u32	rl_version: 8;		// RL Version
	u32	dev_mode: 8;		// Device mode

	// DWORD 2
	u32	mem_addr;			// Memory address

	// DWORD 3
	u32	mem_size;			// Memory size

	// DWORD 4
	union {
		u32	d32;
		u16	d16[2];
		u8	d8[4];
	} value;				// Target value

	// DWORD 5
	u32	reserved;
} usbd_inic_query_packet_t;

typedef struct {
	u8 *buf;
	u16 buf_len;
} usbd_inic_app_ep_t;

typedef struct {
	usbd_inic_app_ep_t in_ep[USB_MAX_ENDPOINTS];
	usbd_inic_app_ep_t out_ep[USB_MAX_ENDPOINTS];
} usbd_inic_app_t;

#define	USBD_INIC_QUERY_PACKET_SIZE			(sizeof(usbd_inic_query_packet_t))

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int inic_cb_init(void);
static int inic_cb_deinit(void);
static int inic_cb_setup(usb_setup_req_t *req, u8 *buf);
static int inic_cb_set_config(void);
static int inic_cb_clear_config(void);
static int inic_cb_received(usbd_inic_ep_t *out_ep, u16 len);
static void inic_cb_transmitted(usbd_inic_ep_t *in_ep, u8 status);
static void inic_cb_status_changed(u8 old_status, u8 status);
static void inic_cb_suspend(void);
static void inic_cb_resume(void);

/* Private variables ---------------------------------------------------------*/

static usbd_config_t inic_cfg = {
	.speed = USB_SPEED_FULL,
	.dma_enable = 1U,
	.isr_priority = INT_PRI_MIDDLE,
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
static rtos_sema_t inic_wifi_bulk_in_sema;
static rtos_sema_t reset_sema;

#if CONFIG_USBD_INIC_HOTPLUG
static u8 inic_attach_status;
static rtos_sema_t inic_attach_status_changed_sema;
#endif

/* Private functions ---------------------------------------------------------*/

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
		rtos_sema_give(reset_sema);
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
	case USBD_INIC_VENDOR_REQ_FW_DOWNLOAD:
		ret = inic_setup_handle_query(req, buf);
		break;
	default:
		break;
	}

	return ret;
}

static void inic_wifi_deinit(void)
{
	usbd_inic_app_t *iapp = &usbd_inic_app;
	usbd_inic_app_ep_t *ep;

	ep = &iapp->in_ep[USB_EP_NUM(USBD_WHC_WIFI_EP3_BULK_IN)];
	if (ep->buf != NULL) {
		usb_os_mfree(ep->buf);
		ep->buf = NULL;
	}

	ep = &iapp->out_ep[USB_EP_NUM(USBD_WHC_WIFI_EP4_BULK_OUT)];
	if (ep->buf != NULL) {
		usb_os_mfree(ep->buf);
		ep->buf = NULL;
	}

	ep = &iapp->out_ep[USB_EP_NUM(USBD_WHC_WIFI_EP2_BULK_OUT)];
	if (ep->buf != NULL) {
		usb_os_mfree(ep->buf);
		ep->buf = NULL;
	}
}

static int inic_wifi_init(void)
{
	int ret = HAL_OK;
	usbd_inic_app_t *iapp = &usbd_inic_app;
	usbd_inic_app_ep_t *ep;
	u8 ep_num;

	ep_num = USB_EP_NUM(USBD_WHC_WIFI_EP3_BULK_IN);
	ep = &iapp->in_ep[ep_num];
	ep->buf_len = USBD_INIC_BULK_BUF_SIZE;
	ep->buf = (u8 *)usb_os_malloc(ep->buf_len);
	if (ep->buf == NULL) {
		ret = HAL_ERR_MEM;
		goto wifi_init_exit;
	}

	ep_num = USB_EP_NUM(USBD_WHC_WIFI_EP4_BULK_OUT);
	ep = &iapp->out_ep[ep_num];
	ep->buf_len = USBD_INIC_BULK_BUF_SIZE;
	ep->buf = (u8 *)usb_os_malloc(ep->buf_len);
	if (ep->buf == NULL) {
		ret = HAL_ERR_MEM;
		goto wifi_init_clean_ep3_bulk_in_buf_exit;
	}

	ep_num = USB_EP_NUM(USBD_WHC_WIFI_EP2_BULK_OUT);
	ep = &iapp->out_ep[ep_num];
	ep->buf_len = USBD_INIC_BULK_BUF_SIZE;
	ep->buf = (u8 *)usb_os_malloc(ep->buf_len);
	if (ep->buf == NULL) {
		ret = HAL_ERR_MEM;
		goto wifi_init_clean_ep4_bulk_out_buf_exit;
	}

	return HAL_OK;

wifi_init_clean_ep4_bulk_out_buf_exit:
	ep = &iapp->out_ep[USB_EP_NUM(USBD_WHC_WIFI_EP4_BULK_OUT)];
	usb_os_mfree(ep->buf);
	ep->buf = NULL;

wifi_init_clean_ep3_bulk_in_buf_exit:
	ep = &iapp->in_ep[USB_EP_NUM(USBD_WHC_WIFI_EP3_BULK_IN)];
	usb_os_mfree(ep->buf);
	ep->buf = NULL;

wifi_init_exit:
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

	ret = inic_wifi_init();
	if (ret != HAL_OK) {
		goto init_exit;
	}

	rtos_sema_create(&inic_wifi_bulk_in_sema, 0, 1);
	rtos_sema_create(&reset_sema, 0, 1);

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
	rtos_sema_delete(inic_wifi_bulk_in_sema);
	rtos_sema_delete(reset_sema);

	inic_wifi_deinit();

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
	ep = &iapp->out_ep[USB_EP_NUM(USBD_WHC_WIFI_EP4_BULK_OUT)];
	usbd_inic_receive_data(USBD_WHC_WIFI_EP4_BULK_OUT, ep->buf, ep->buf_len, NULL);
	ep = &iapp->out_ep[USB_EP_NUM(USBD_WHC_WIFI_EP2_BULK_OUT)];
	usbd_inic_receive_data(USBD_WHC_WIFI_EP2_BULK_OUT, ep->buf, ep->buf_len, NULL);

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
	usbd_inic_app_t *iapp = &usbd_inic_app;
	usbd_inic_app_ep_t *ep_in;
	usbd_ep_t *ep = &out_ep->ep;
	u8 ep_num;

	switch (ep->addr) {
	case USBD_WHC_WIFI_EP4_BULK_OUT:
		// Loopback with EP3
		ep_num = USB_EP_NUM(USBD_WHC_WIFI_EP3_BULK_IN);
		ep_in = &iapp->in_ep[ep_num];
		usb_os_memcpy((void *)ep_in->buf, (void *)ep->xfer_buf, len);
		ep_in->buf_len = len;
		rtos_sema_give(inic_wifi_bulk_in_sema);
		break;
	default:
		break;
	}

	return HAL_OK;
}

static void inic_cb_transmitted(usbd_inic_ep_t *in_ep, u8 status)
{
	usbd_ep_t *ep = &in_ep->ep;
	(void)status;
	switch (ep->addr) {
	case USBD_WHC_WIFI_EP3_BULK_IN:
		// TBD
		break;
	default:
		break;
	}
}

static void inic_wifi_bulk_in_thread(void *param)
{
	UNUSED(param);
	usbd_inic_app_t *iapp = &usbd_inic_app;
	usbd_inic_app_ep_t *ep;
	u8 ep_num;

	ep_num = USB_EP_NUM(USBD_WHC_WIFI_EP3_BULK_IN);
	ep = &iapp->in_ep[ep_num];

	for (;;) {
		if (rtos_sema_take(inic_wifi_bulk_in_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			if ((ep->buf != NULL) && (ep->buf_len != 0)) {
				usbd_inic_transmit_data(USBD_WHC_WIFI_EP3_BULK_IN, ep->buf, ep->buf_len, NULL);
			}
		}
	}
}

static void inic_cb_status_changed(u8 old_status, u8 status)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "Status change: %d -> %d \n", old_status, status);
#if CONFIG_USBD_INIC_HOTPLUG
	inic_attach_status = status;
	rtos_sema_give(inic_attach_status_changed_sema);
#endif
}

static void inic_cb_suspend(void)
{
	// TODO: Mark suspend status, stop USB transfer
}

static void inic_cb_resume(void)
{
	// TODO: Unmark suspend status, re-start USB transfer
}

static void inic_reset_thread(void *param)
{
	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(reset_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			rtos_time_delay_ms(500); // Wait reset request done
			System_Reset();
		}
	}
}

#if CONFIG_USBD_INIC_HOTPLUG
static void inic_hotplug_thread(void *param)
{
	int ret = 0;

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(inic_attach_status_changed_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			if (inic_attach_status == USBD_ATTACH_STATUS_DETACHED) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "DETACHED\n");
				usbd_inic_deinit();
				ret = usbd_deinit();
				if (ret != 0) {
					break;
				}
				RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());
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
				RTK_LOGS(TAG, RTK_LOG_INFO, "ATTACHED\n");
			} else {
				RTK_LOGS(TAG, RTK_LOG_INFO, "INIT\n");
			}
		}
	}
	RTK_LOGS(TAG, RTK_LOG_ERROR, "Hotplug thread fail\n");
	rtos_task_delete(NULL);
}
#endif // CONFIG_USBD_INIC_HOTPLUG

static void example_usbd_inic_thread(void *param)
{
	int ret = 0;
#if CONFIG_USBD_INIC_HOTPLUG
	rtos_task_t hotplug_task;
#endif
	rtos_task_t wifi_bulk_in_task;
	rtos_task_t reset_task;

	UNUSED(param);

#if CONFIG_USBD_INIC_HOTPLUG
	rtos_sema_create(&inic_attach_status_changed_sema, 0, 1);
#endif

	ret = usbd_init(&inic_cfg);
	if (ret != HAL_OK) {
		goto exit;
	}

	ret = usbd_inic_init(&inic_cb);
	if (ret != HAL_OK) {
		goto clear_usb_driver_exit;
	}

	ret = rtos_task_create(&wifi_bulk_in_task, "inic_wifi_bulk_in_thread", inic_wifi_bulk_in_thread, NULL, 1024, CONFIG_USBD_INIC_XFER_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		goto clear_class_exit;
	}

	ret = rtos_task_create(&reset_task, "inic_reset_thread", inic_reset_thread, NULL, 1024, CONFIG_USBD_INIC_RESET_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		goto clear_wifi_bulk_in_task;
	}

#if CONFIG_USBD_INIC_HOTPLUG
	ret = rtos_task_create(&hotplug_task, "inic_hotplug_thread", inic_hotplug_thread, NULL, 1024, CONFIG_USBD_INIC_HOTPLUG_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		goto clear_reset_task;
	}
#endif // CONFIG_USBD_INIC_HOTPLUG

	rtos_time_delay_ms(100);

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD INIC dplus demo start\n");

	rtos_task_delete(NULL);

	return;

#if CONFIG_USBD_INIC_HOTPLUG
clear_reset_task:
	rtos_task_delete(reset_task);
#endif

clear_wifi_bulk_in_task:
	rtos_task_delete(wifi_bulk_in_task);

clear_class_exit:
	usbd_inic_deinit();

clear_usb_driver_exit:
	usbd_deinit();

exit:
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD INIC dplus demo stop\n");
#if CONFIG_USBD_INIC_HOTPLUG
	rtos_sema_delete(inic_attach_status_changed_sema);
#endif

	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

void example_usbd_inic_dplus(void)
{
	int status;
	rtos_task_t task;

	status = rtos_task_create(&task, "example_usbd_inic_thread", example_usbd_inic_thread, NULL, 1024, CONFIG_USBD_INIC_INIT_THREAD_PRIORITY);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create USBD INIC thread fail\n");
	}
}

