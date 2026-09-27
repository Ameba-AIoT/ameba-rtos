/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */

#include <platform_autoconf.h>
#include "usbd_whc.h"
#include "os_wrapper.h"

/* Private defines -----------------------------------------------------------*/

// This configuration is used to enable a thread to check hotplug event
// and reset USB stack to avoid memory leak, only for example.
// while test suspend/resume, hotplug should be disabled
#define WHC_HOTPLUG                                          1

#define USBD_WHC_BULK_BUF_SIZE                               64U

// USB speed
#define WHC_USB_SPEED                                        USB_SPEED_FULL

// Thread priorities
#define WHC_INIT_THREAD_PRIORITY                             5
#define WHC_HOTPLUG_THREAD_PRIORITY                          8
#define WHC_XFER_THREAD_PRIORITY                             6
#define WHC_RESET_THREAD_PRIORITY                            6

// Thread stack sizes
#define WHC_INIT_THREAD_STACK_SIZE                           1024U
#define WHC_HOTPLUG_THREAD_STACK_SIZE                        1024U
#define WHC_XFER_THREAD_STACK_SIZE                           1024U
#define WHC_RESET_THREAD_STACK_SIZE                          1024U

// Vendor requests
#define USBD_WHC_VENDOR_REQ_FW_DOWNLOAD    0xF0U
#define USBD_WHC_VENDOR_QUERY_CMD          0x01U
#define USBD_WHC_VENDOR_QUERY_ACK          0x81U
#define USBD_WHC_VENDOR_RESET_CMD          0x06U
#define USBD_WHC_VENDOR_RESET_ACK          0x86U

#define USBD_WHC_FW_TYPE_RAM               0xF1U

/* Private types -------------------------------------------------------------*/

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
} usbd_whc_query_packet_t;

typedef struct {
	u8 *buf;
	u16 buf_len;
} usbd_whc_app_ep_t;

typedef struct {
	usbd_whc_app_ep_t in_ep[USB_MAX_ENDPOINTS];
	usbd_whc_app_ep_t out_ep[USB_MAX_ENDPOINTS];
} usbd_whc_app_t;

#define	USBD_WHC_QUERY_PACKET_SIZE			(sizeof(usbd_whc_query_packet_t))

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int whc_cb_init(void);
static int whc_cb_deinit(void);
static int whc_cb_setup(usb_setup_req_t *req, u8 *buf);
static int whc_cb_set_config(void);
static int whc_cb_clear_config(void);
static int whc_cb_received(usbd_whc_ep_t *out_ep, u32 len);
static void whc_cb_transmitted(usbd_whc_ep_t *in_ep, u8 status);
static void whc_cb_status_changed(u8 old_status, u8 status);

/* Private variables ---------------------------------------------------------*/
static const char *const TAG = "WHC";

static const usbd_config_t whc_cfg = {
	.speed = WHC_USB_SPEED,
	.isr_priority = INT_PRI_MIDDLE,
};

static const usbd_whc_cb_t whc_cb = {
	.init = whc_cb_init,
	.deinit = whc_cb_deinit,
	.setup = whc_cb_setup,
	.set_config = whc_cb_set_config,
	.clear_config = whc_cb_clear_config,
	.received = whc_cb_received,
	.transmitted = whc_cb_transmitted,
	.status_changed = whc_cb_status_changed,
};

/* WHC Device */
static usbd_whc_app_t usbd_whc_app;
static rtos_sema_t whc_wifi_bulk_in_sema;
static rtos_sema_t reset_sema;

#if WHC_HOTPLUG
static u8 whc_attach_status;
static rtos_sema_t whc_attach_status_changed_sema;
#endif

/* Worker thread handles, needed by the hotplug thread to clean them up when the
   stack can not be recovered. */
static rtos_task_t whc_wifi_bulk_in_task;
static rtos_task_t whc_reset_task;

#if WHC_HOTPLUG
/* Raised by the hotplug thread when the stack can not be recovered: the worker
   threads leave their loops so the semaphores can be freed safely. */
static volatile u8 whc_stack_fatal;
#endif

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Free the objects shared by the example threads
  * @note   Only called by the last running thread, after the USB stack is fully
  *         deinited, so that no ISR callback can touch these objects any more
  * @retval None
  */
static void whc_free_resource(void)
{
	rtos_sema_delete(whc_wifi_bulk_in_sema);
	whc_wifi_bulk_in_sema = NULL;
	rtos_sema_delete(reset_sema);
	reset_sema = NULL;
#if WHC_HOTPLUG
	rtos_sema_delete(whc_attach_status_changed_sema);
	whc_attach_status_changed_sema = NULL;
#endif
}

#if WHC_HOTPLUG
/**
  * @brief  Ask the worker threads to leave their loops, then wait for them to
  *         delete themselves (each one clears its own handle as its last action)
  * @note   Must run before whc_free_resource(): those threads block on the
  *         semaphores it frees. A thread that misses its polling window within
  *         the grace period is force-deleted as a last resort.
  * @retval None
  */
static void whc_stop_workers(void)
{
	int wait_cnt;

	whc_stack_fatal = 1U;

	rtos_sema_give(whc_wifi_bulk_in_sema);
	rtos_sema_give(reset_sema);

	for (wait_cnt = 0; wait_cnt < 50; wait_cnt++) { /* max wait 1s */
		if ((whc_wifi_bulk_in_task == NULL) && (whc_reset_task == NULL)) {
			return;
		}
		rtos_time_delay_ms(20);
	}

	RTK_LOGS(TAG, RTK_LOG_WARN, "Force delete worker thread\n");
	if (whc_wifi_bulk_in_task != NULL) {
		rtos_task_delete(whc_wifi_bulk_in_task);
		whc_wifi_bulk_in_task = NULL;
	}
	if (whc_reset_task != NULL) {
		rtos_task_delete(whc_reset_task);
		whc_reset_task = NULL;
	}
}
#endif // WHC_HOTPLUG

static int whc_setup_handle_query(usb_setup_req_t *req, u8 *buf)
{
	int ret = HAL_ERR_PARA;
	usbd_whc_query_packet_t *pkt;

	/* The ACK packet is built in place, reject a request which cannot carry it */
	if (buf == NULL) {
		return HAL_ERR_PARA;
	}

	if (req->wIndex == USBD_WHC_VENDOR_QUERY_CMD) {
		pkt = (usbd_whc_query_packet_t *)buf;
		pkt->data_len = 0;
		pkt->data_offset = USBD_WHC_QUERY_PACKET_SIZE;
		pkt->pkt_type = USBD_WHC_VENDOR_QUERY_ACK;
		pkt->xfer_status = HAL_OK;
		pkt->rl_version = (u8)(EFUSE_GetChipVersion() & 0xFF);
		pkt->dev_mode = USBD_WHC_FW_TYPE_RAM;
		ret = HAL_OK;
	} else if (req->wIndex == USBD_WHC_VENDOR_RESET_CMD) {
		pkt = (usbd_whc_query_packet_t *)buf;
		pkt->data_len = 0;
		pkt->data_offset = USBD_WHC_QUERY_PACKET_SIZE;
		pkt->pkt_type = USBD_WHC_VENDOR_RESET_ACK;
		pkt->xfer_status = HAL_OK;
		pkt->rl_version = (u8)(EFUSE_GetChipVersion() & 0xFF);
		pkt->dev_mode = USBD_WHC_FW_TYPE_RAM;
		rtos_sema_give(reset_sema);
		ret = HAL_OK;
	}

	return ret;
}

/**
  * @brief  Handle the whc class control requests
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  cmd: Command code
  * @param  buf: Buffer containing command data (request parameters)
  * @param  len: Number of data to be sent (in bytes)
  * @param  value: Value for the command code
  * @retval Status
  */
static int whc_cb_setup(usb_setup_req_t *req, u8 *buf)
{
	int ret = HAL_ERR_PARA;
	switch (req->bRequest) {
	case USBD_WHC_VENDOR_REQ_FW_DOWNLOAD:
		ret = whc_setup_handle_query(req, buf);
		break;
	default:
		break;
	}

	return ret;
}

static void whc_wifi_deinit(void)
{
	usbd_whc_app_t *iapp = &usbd_whc_app;
	usbd_whc_app_ep_t *ep;

	ep = &iapp->in_ep[USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_IN)];
	usb_os_mfree((void *)ep->buf);
	ep->buf = NULL;

	ep = &iapp->out_ep[USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_OUT_2)];
	usb_os_mfree((void *)ep->buf);
	ep->buf = NULL;

	ep = &iapp->out_ep[USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_OUT_1)];
	usb_os_mfree((void *)ep->buf);
	ep->buf = NULL;
}

static int whc_wifi_init(void)
{
	int ret = HAL_OK;
	usbd_whc_app_t *iapp = &usbd_whc_app;
	usbd_whc_app_ep_t *ep;
	u8 ep_num;

	ep_num = USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_IN);
	ep = &iapp->in_ep[ep_num];
	ep->buf_len = USBD_WHC_BULK_BUF_SIZE;
	ep->buf = (u8 *)usb_os_malloc(ep->buf_len);
	if (ep->buf == NULL) {
		ret = HAL_ERR_MEM;
		goto wifi_init_exit;
	}

	ep_num = USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_OUT_2);
	ep = &iapp->out_ep[ep_num];
	ep->buf_len = USBD_WHC_BULK_BUF_SIZE;
	ep->buf = (u8 *)usb_os_malloc(ep->buf_len);
	if (ep->buf == NULL) {
		ret = HAL_ERR_MEM;
		goto wifi_init_clean_ep3_bulk_in_buf_exit;
	}

	ep_num = USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_OUT_1);
	ep = &iapp->out_ep[ep_num];
	ep->buf_len = USBD_WHC_BULK_BUF_SIZE;
	ep->buf = (u8 *)usb_os_malloc(ep->buf_len);
	if (ep->buf == NULL) {
		ret = HAL_ERR_MEM;
		goto wifi_init_clean_ep4_bulk_out_buf_exit;
	}

	return HAL_OK;

wifi_init_clean_ep4_bulk_out_buf_exit:
	ep = &iapp->out_ep[USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_OUT_2)];
	usb_os_mfree((void *)ep->buf);
	ep->buf = NULL;

wifi_init_clean_ep3_bulk_in_buf_exit:
	ep = &iapp->in_ep[USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_IN)];
	usb_os_mfree((void *)ep->buf);
	ep->buf = NULL;

wifi_init_exit:
	return ret;
}

/**
  * @brief  Initializes whc application layer
  * @param  None
  * @retval Status
  */
static int whc_cb_init(void)
{
	/* The semaphores shared with the worker threads are owned by the example
	   bootstrap thread, not by the class: they must outlive a hotplug re-init,
	   otherwise a worker would block on a deleted handle. */
	return whc_wifi_init();
}

/**
  * @brief  DeInitializes whc application layer
  * @param  None
  * @retval Status
  */
static int whc_cb_deinit(void)
{
	whc_wifi_deinit();

	return HAL_OK;
}

/**
  * @brief  Set config callback
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  None
  * @retval Status
  */
static int whc_cb_set_config(void)
{
	usbd_whc_app_t *iapp = &usbd_whc_app;
	usbd_whc_app_ep_t *ep;

	// Prepare to RX
	ep = &iapp->out_ep[USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_OUT_2)];
	usbd_whc_receive_data(USBD_WHC_WIFI_EP_BULK_OUT_2, ep->buf, ep->buf_len, NULL);
	ep = &iapp->out_ep[USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_OUT_1)];
	usbd_whc_receive_data(USBD_WHC_WIFI_EP_BULK_OUT_1, ep->buf, ep->buf_len, NULL);

	return HAL_OK;
}

/**
  * @brief  Clear config callback
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  None
  * @retval Status
  */
static int whc_cb_clear_config(void)
{
	return HAL_OK;
}

/**
  * @brief  Data received over USB BULK OUT endpoint
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  buf: RX buffer
  * @param  len: RX data length (in bytes)
  * @retval Status
  */
static int whc_cb_received(usbd_whc_ep_t *out_ep, u32 len)
{
	usbd_whc_app_t *iapp = &usbd_whc_app;
	usbd_whc_app_ep_t *ep_in;
	usbd_ep_t *ep = &out_ep->ep;
	u8 ep_num;

	switch (ep->info.addr) {
	case USBD_WHC_WIFI_EP_BULK_OUT_2:
		// Loopback with WiFi BULK IN
		ep_num = USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_IN);
		ep_in = &iapp->in_ep[ep_num];
		usb_os_memcpy((void *)ep_in->buf, (const void *)ep->xfer_buf, len);
		ep_in->buf_len = len;
		rtos_sema_give(whc_wifi_bulk_in_sema);
		break;
	default:
		break;
	}

	return HAL_OK;
}

/**
  * @brief  Notify completion of a WHC IN endpoint transfer
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  in_ep: IN endpoint that completed the transfer
  * @param  status: Transfer completion status
  * @retval None
  */
static void whc_cb_transmitted(usbd_whc_ep_t *in_ep, u8 status)
{
	usbd_ep_t *ep = &in_ep->ep;
	(void)status;
	switch (ep->info.addr) {
	case USBD_WHC_WIFI_EP_BULK_IN:
		// TBD
		break;
	default:
		break;
	}
}

static void example_usbd_whc_wifi_bulk_in_thread(void *param)
{
	UNUSED(param);
	usbd_whc_app_t *iapp = &usbd_whc_app;
	usbd_whc_app_ep_t *ep;
	u8 ep_num;

	ep_num = USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_IN);
	ep = &iapp->in_ep[ep_num];

	for (;;) {
		if (rtos_sema_take(whc_wifi_bulk_in_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
#if WHC_HOTPLUG
			if (whc_stack_fatal != 0U) {
				break;
			}
#endif
			if ((ep->buf != NULL) && (ep->buf_len != 0)) {
				usbd_whc_transmit_data(USBD_WHC_WIFI_EP_BULK_IN, ep->buf, ep->buf_len, NULL);
			}
		}
	}
	whc_wifi_bulk_in_task = NULL;
	rtos_task_delete(NULL);
}

/**
  * @brief  Handle WHC attach status change notifications from the USB stack
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  old_status: Previous attach status
  * @param  status: New attach status
  * @retval None
  */
static void whc_cb_status_changed(u8 old_status, u8 status)
{
	UNUSED(old_status);

#if WHC_HOTPLUG
	whc_attach_status = status;
	rtos_sema_give(whc_attach_status_changed_sema);
#else
	UNUSED(status);
#endif
}

static void example_usbd_whc_reset_thread(void *param)
{
	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(reset_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
#if WHC_HOTPLUG
			if (whc_stack_fatal != 0U) {
				break;
			}
#endif
			rtos_time_delay_ms(500); // Wait reset request done
			System_Reset();
		}
	}
	whc_reset_task = NULL;
	rtos_task_delete(NULL);
}

#if WHC_HOTPLUG
static void example_usbd_whc_hotplug_thread(void *param)
{
	int ret = 0;

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(whc_attach_status_changed_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			if (whc_attach_status == USBD_ATTACH_STATUS_DETACHED) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "DETACHED\n");
				usbd_whc_deinit();
				usbd_deinit();
				RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());
				ret = usbd_init(&whc_cfg);
				if (ret != 0) {
					break;
				}
				ret = usbd_whc_init(&whc_cb);
				if (ret != 0) {
					usbd_deinit();
					break;
				}
			} else if (whc_attach_status == USBD_ATTACH_STATUS_ATTACHED) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "ATTACHED\n");
			} else {
				RTK_LOGS(TAG, RTK_LOG_INFO, "INIT\n");
			}
		}
	}
	RTK_LOGS(TAG, RTK_LOG_ERROR, "Hotplug thread fail\n");

	/* The stack is fully deinited here, no more ISR callback: stop the workers
	   blocked on the semaphores, then free them as the last thread standing. */
	whc_stop_workers();
	whc_free_resource();
	rtos_task_delete(NULL);
}
#endif // WHC_HOTPLUG

static void example_usbd_whc_thread(void *param)
{
	int ret = 0;
#if WHC_HOTPLUG
	rtos_task_t hotplug_task;
#endif

	UNUSED(param);

#if WHC_HOTPLUG
	ret = rtos_sema_create(&whc_attach_status_changed_sema, 0, 1);
	if (ret != RTK_SUCCESS) {
		goto exit;
	}
#endif

	/* Owned here rather than by whc_cb_init(), so that they survive the deinit and
	   re-init the hotplug thread does on every cable cycle. */
	ret = rtos_sema_create(&whc_wifi_bulk_in_sema, 0, 1);
	if (ret != RTK_SUCCESS) {
		goto exit;
	}

	ret = rtos_sema_create(&reset_sema, 0, 1);
	if (ret != RTK_SUCCESS) {
		goto exit;
	}

	ret = usbd_init(&whc_cfg);
	if (ret != HAL_OK) {
		goto exit;
	}

	ret = usbd_whc_init(&whc_cb);
	if (ret != HAL_OK) {
		goto clear_usb_driver_exit;
	}

	ret = rtos_task_create(&whc_wifi_bulk_in_task, "usbd_whc_wifi_bulk_in_thread",
						   example_usbd_whc_wifi_bulk_in_thread, NULL,
						   WHC_XFER_THREAD_STACK_SIZE, WHC_XFER_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		goto clear_class_exit;
	}

	ret = rtos_task_create(&whc_reset_task, "usbd_whc_reset_thread",
						   example_usbd_whc_reset_thread, NULL,
						   WHC_RESET_THREAD_STACK_SIZE, WHC_RESET_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		goto clear_wifi_bulk_in_task;
	}

#if WHC_HOTPLUG
	ret = rtos_task_create(&hotplug_task, "usbd_whc_hotplug_thread",
						   example_usbd_whc_hotplug_thread, NULL,
						   WHC_HOTPLUG_THREAD_STACK_SIZE, WHC_HOTPLUG_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		goto clear_reset_task;
	}
#endif // WHC_HOTPLUG

	rtos_time_delay_ms(100);

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD WHC dplus demo start\n");

	rtos_task_delete(NULL);

	return;

#if WHC_HOTPLUG
clear_reset_task:
	rtos_task_delete(whc_reset_task);
	whc_reset_task = NULL;
#endif

clear_wifi_bulk_in_task:
	rtos_task_delete(whc_wifi_bulk_in_task);
	whc_wifi_bulk_in_task = NULL;

clear_class_exit:
	usbd_whc_deinit();

clear_usb_driver_exit:
	usbd_deinit();

exit:
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD WHC dplus demo stop\n");
	whc_free_resource();

	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

void example_usbd_whc_dplus(void)
{
	int ret;
	rtos_task_t task;

	ret = rtos_task_create(&task, "usbd_whc_thread", example_usbd_whc_thread, NULL,
						   WHC_INIT_THREAD_STACK_SIZE, WHC_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create USBD WHC thread fail\n");
	}
}
