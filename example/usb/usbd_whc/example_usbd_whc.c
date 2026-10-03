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
#define WHC_HOTPLUG                        1

#define WHC_BT_INTR_IN_BUF_SIZE        16U   /* BT INTR IN buffer size */
#define WHC_BT_BULK_IN_BUF_SIZE        512U  /* BT BULK IN buffer size */
#define WHC_BT_BULK_OUT_BUF_SIZE       512U  /* BT BULK OUT buffer size */
#define WHC_WIFI_BULK_IN_BUF_SIZE      512U  /* WiFi BULK IN buffer size */
#define WHC_WIFI_BULK_OUT_1_BUF_SIZE     512U  /* WiFi BULK OUT 1 buffer size */
#define WHC_WIFI_BULK_OUT_2_BUF_SIZE     512U  /* WiFi BULK OUT 2 buffer size */
#define WHC_WIFI_BULK_OUT_3_BUF_SIZE     512U  /* WiFi BULK OUT 3 buffer size */
#ifdef CONFIG_WHC_ETH
#define WHC_ETH_BULK_OUT_BUF_SIZE      512U  /* Ethernet BULK OUT buffer size */
#define WHC_ETH_BULK_IN_BUF_SIZE       512U  /* Ethernet BULK IN buffer size */
#endif

// Thread priorities
#define WHC_INIT_THREAD_PRIORITY           5
#define WHC_HOTPLUG_THREAD_PRIORITY        8
#define WHC_XFER_THREAD_PRIORITY           6

// Thread stack sizes
#define WHC_INIT_THREAD_STACK_SIZE         1024U
#define WHC_HOTPLUG_THREAD_STACK_SIZE      768U
#define WHC_XFER_THREAD_STACK_SIZE         700U

/* Private types -------------------------------------------------------------*/

typedef struct {
	u8 *buf;
	u16 buf_len;
} usbd_whc_app_ep_t;

typedef struct {
	usbd_whc_app_ep_t in_ep[USB_MAX_ENDPOINTS];
	usbd_whc_app_ep_t out_ep[USB_MAX_ENDPOINTS];
} usbd_whc_app_t;

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
	.isr_priority = INT_PRI_MIDDLE,
#if defined(CONFIG_AMEBAGREEN2)
#ifdef CONFIG_WHC_ETH
	.rx_fifo_depth = 324U,
	.ptx_fifo_depth = {16U, 256U, 0U, 256U, 128U, },
#else
	.rx_fifo_depth = 420U,
	.ptx_fifo_depth = {16U, 256U, 32U, 256U, 0U, },
#endif
#elif defined(CONFIG_RLE1509)
#ifdef CONFIG_WHC_ETH
	.rx_fifo_depth = 288U,
	.ptx_fifo_depth = {16U, 256U, 0U, 256U, 128U, },
#else
	.rx_fifo_depth = 384U,
	.ptx_fifo_depth = {16U, 256U, 32U, 256U, 0U, },
#endif
#endif
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
static rtos_sema_t whc_bt_bulk_in_sema;
#ifdef CONFIG_WHC_ETH
static rtos_sema_t whc_eth_bulk_in_sema;
#endif

#if WHC_HOTPLUG
static u8 whc_attach_status;
static rtos_sema_t whc_attach_status_changed_sema;
/* Raised by the hotplug thread when the stack can not be recovered: the bulk IN
   threads leave their loops so the semaphores can be freed safely. */
static volatile u8 whc_stack_fatal;
#endif

/* Bulk IN thread handles. Each worker clears its own handle as its last action,
   so the hotplug thread can tell when it is gone. */
static rtos_task_t whc_wifi_bulk_in_task;
static rtos_task_t whc_bt_bulk_in_task;
#ifdef CONFIG_WHC_ETH
static rtos_task_t whc_eth_bulk_in_task;
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
#ifdef CONFIG_WHC_ETH
	rtos_sema_delete(whc_eth_bulk_in_sema);
	whc_eth_bulk_in_sema = NULL;
#endif
	rtos_sema_delete(whc_wifi_bulk_in_sema);
	whc_wifi_bulk_in_sema = NULL;
	rtos_sema_delete(whc_bt_bulk_in_sema);
	whc_bt_bulk_in_sema = NULL;
#if WHC_HOTPLUG
	rtos_sema_delete(whc_attach_status_changed_sema);
	whc_attach_status_changed_sema = NULL;
#endif
}

#if WHC_HOTPLUG
/**
  * @brief  Ask the bulk IN threads to leave their loops, then wait for them to
  *         delete themselves (each one clears its own handle as its last action)
  * @note   Must run before whc_free_resource(): those threads block on the
  *         semaphores it frees. A thread that misses its polling window within
  *         the grace period is force-deleted as a last resort.
  * @retval None
  */
static void whc_stop_xfer_threads(void)
{
	int wait_cnt;

	whc_stack_fatal = 1U;

	rtos_sema_give(whc_wifi_bulk_in_sema);
	rtos_sema_give(whc_bt_bulk_in_sema);
#ifdef CONFIG_WHC_ETH
	rtos_sema_give(whc_eth_bulk_in_sema);
#endif

	for (wait_cnt = 0; wait_cnt < 50; wait_cnt++) { /* max wait 1s */
		if ((whc_wifi_bulk_in_task == NULL) && (whc_bt_bulk_in_task == NULL)
#ifdef CONFIG_WHC_ETH
			&& (whc_eth_bulk_in_task == NULL)
#endif
		   ) {
			return;
		}
		rtos_time_delay_ms(20);
	}

	RTK_LOGS(TAG, RTK_LOG_WARN, "Force delete xfer thread\n");
	if (whc_wifi_bulk_in_task != NULL) {
		rtos_task_delete(whc_wifi_bulk_in_task);
		whc_wifi_bulk_in_task = NULL;
	}
	if (whc_bt_bulk_in_task != NULL) {
		rtos_task_delete(whc_bt_bulk_in_task);
		whc_bt_bulk_in_task = NULL;
	}
#ifdef CONFIG_WHC_ETH
	if (whc_eth_bulk_in_task != NULL) {
		rtos_task_delete(whc_eth_bulk_in_task);
		whc_eth_bulk_in_task = NULL;
	}
#endif
}
#endif // WHC_HOTPLUG

static void whc_wifi_deinit(void)
{
	usbd_whc_app_t *iapp = &usbd_whc_app;
	usbd_whc_app_ep_t *ep;

	ep = &iapp->in_ep[USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_IN)];
	usb_os_mfree(ep->buf);
	ep->buf = NULL;

	ep = &iapp->out_ep[USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_OUT_1)];
	usb_os_mfree(ep->buf);
	ep->buf = NULL;

	ep = &iapp->out_ep[USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_OUT_2)];
	usb_os_mfree(ep->buf);
	ep->buf = NULL;

	ep = &iapp->out_ep[USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_OUT_3)];
	usb_os_mfree(ep->buf);
	ep->buf = NULL;
}

#ifdef CONFIG_WHC_ETH
static void whc_eth_deinit(void)
{
	usbd_whc_app_t *iapp = &usbd_whc_app;
	usbd_whc_app_ep_t *ep;

	ep = &iapp->in_ep[USB_EP_NUM(USBD_WHC_ETH_EP_BULK_IN)];
	usb_os_mfree(ep->buf);
	ep->buf = NULL;

	ep = &iapp->out_ep[USB_EP_NUM(USBD_WHC_ETH_EP_BULK_OUT)];
	usb_os_mfree(ep->buf);
	ep->buf = NULL;
}
#endif

static void whc_bt_deinit(void)
{
	usbd_whc_app_t *iapp = &usbd_whc_app;
	usbd_whc_app_ep_t *ep;

	ep = &iapp->in_ep[USB_EP_NUM(USBD_WHC_BT_EP_INTR_IN)];
	usb_os_mfree(ep->buf);
	ep->buf = NULL;

	ep = &iapp->in_ep[USB_EP_NUM(USBD_WHC_BT_EP_BULK_IN)];
	usb_os_mfree(ep->buf);
	ep->buf = NULL;

	ep = &iapp->out_ep[USB_EP_NUM(USBD_WHC_BT_EP_BULK_OUT)];
	usb_os_mfree(ep->buf);
	ep->buf = NULL;
}

/**
  * @brief  Handle the WHC class control (setup) requests
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  req: USB setup request
  * @param  buf: Buffer containing the setup data payload
  * @retval Status
  */
static int whc_cb_setup(usb_setup_req_t *req, u8 *buf)
{
	UNUSED(req);
	UNUSED(buf);

	return HAL_OK;
}

static int whc_wifi_init(void)
{
	int ret = HAL_OK;
	usbd_whc_app_t *iapp = &usbd_whc_app;
	usbd_whc_app_ep_t *ep;
	u8 ep_num;

	ep_num = USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_IN);
	ep = &iapp->in_ep[ep_num];
	ep->buf_len = WHC_WIFI_BULK_IN_BUF_SIZE;
	ep->buf = (u8 *)usb_os_malloc(ep->buf_len);
	if (ep->buf == NULL) {
		ret = HAL_ERR_MEM;
		goto wifi_init_exit;
	}

	ep_num = USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_OUT_1);
	ep = &iapp->out_ep[ep_num];
	ep->buf_len = WHC_WIFI_BULK_OUT_1_BUF_SIZE;
	ep->buf = (u8 *)usb_os_malloc(ep->buf_len);
	if (ep->buf == NULL) {
		ret = HAL_ERR_MEM;
		goto wifi_init_clean_ep4_bulk_in_buf_exit;
	}

	ep_num = USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_OUT_2);
	ep = &iapp->out_ep[ep_num];
	ep->buf_len = WHC_WIFI_BULK_OUT_2_BUF_SIZE;
	ep->buf = (u8 *)usb_os_malloc(ep->buf_len);
	if (ep->buf == NULL) {
		ret = HAL_ERR_MEM;
		goto wifi_init_clean_ep5_bulk_out_buf_exit;
	}

	ep_num = USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_OUT_3);
	ep = &iapp->out_ep[ep_num];
	ep->buf_len = WHC_WIFI_BULK_OUT_3_BUF_SIZE;
	ep->buf = (u8 *)usb_os_malloc(ep->buf_len);
	if (ep->buf == NULL) {
		ret = HAL_ERR_MEM;
		goto wifi_init_clean_ep6_bulk_out_buf_exit;
	}

	return HAL_OK;

wifi_init_clean_ep6_bulk_out_buf_exit:
	ep = &iapp->out_ep[USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_OUT_2)];
	usb_os_mfree(ep->buf);
	ep->buf = NULL;

wifi_init_clean_ep5_bulk_out_buf_exit:
	ep = &iapp->out_ep[USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_OUT_1)];
	usb_os_mfree(ep->buf);
	ep->buf = NULL;

wifi_init_clean_ep4_bulk_in_buf_exit:
	ep = &iapp->in_ep[USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_IN)];
	usb_os_mfree(ep->buf);
	ep->buf = NULL;

wifi_init_exit:
	return ret;
}

#ifdef CONFIG_WHC_ETH
static int whc_eth_init(void)
{
	int ret = HAL_OK;
	usbd_whc_app_t *iapp = &usbd_whc_app;
	usbd_whc_app_ep_t *ep;
	u8 ep_num;

	ep_num = USB_EP_NUM(USBD_WHC_ETH_EP_BULK_IN);
	ep = &iapp->in_ep[ep_num];
	ep->buf_len = WHC_ETH_BULK_IN_BUF_SIZE;
	ep->buf = (u8 *)usb_os_malloc(ep->buf_len);
	if (ep->buf == NULL) {
		ret = HAL_ERR_MEM;
		goto eth_init_exit;
	}

	ep_num = USB_EP_NUM(USBD_WHC_ETH_EP_BULK_OUT);
	ep = &iapp->out_ep[ep_num];
	ep->buf_len = WHC_ETH_BULK_OUT_BUF_SIZE;
	ep->buf = (u8 *)usb_os_malloc(ep->buf_len);
	if (ep->buf == NULL) {
		ret = HAL_ERR_MEM;
		goto eth_init_clean_ep6_bulk_in_buf_exit;
	}

	return HAL_OK;

eth_init_clean_ep6_bulk_in_buf_exit:
	ep = &iapp->in_ep[USB_EP_NUM(USBD_WHC_ETH_EP_BULK_IN)];
	usb_os_mfree(ep->buf);
	ep->buf = NULL;

eth_init_exit:
	return ret;
}
#endif

static int whc_bt_init(void)
{
	int ret = HAL_OK;
	usbd_whc_app_t *iapp = &usbd_whc_app;
	usbd_whc_app_ep_t *ep;
	u8 ep_num;

	ep_num = USB_EP_NUM(USBD_WHC_BT_EP_INTR_IN);
	ep = &iapp->in_ep[ep_num];
	ep->buf_len = WHC_BT_INTR_IN_BUF_SIZE;
	ep->buf = (u8 *)usb_os_malloc(ep->buf_len);
	if (ep->buf == NULL) {
		ret = HAL_ERR_MEM;
		goto bt_init_exit;
	}

	ep_num = USB_EP_NUM(USBD_WHC_BT_EP_BULK_IN);
	ep = &iapp->in_ep[ep_num];
	ep->buf_len = WHC_BT_BULK_IN_BUF_SIZE;
	ep->buf = (u8 *)usb_os_malloc(ep->buf_len);
	if (ep->buf == NULL) {
		ret = HAL_ERR_MEM;
		goto bt_init_clean_ep1_intr_in_buf_exit;
	}

	ep_num = USB_EP_NUM(USBD_WHC_BT_EP_BULK_OUT);
	ep = &iapp->out_ep[ep_num];
	ep->buf_len = WHC_BT_BULK_OUT_BUF_SIZE;
	ep->buf = (u8 *)usb_os_malloc(ep->buf_len);
	if (ep->buf == NULL) {
		ret = HAL_ERR_MEM;
		goto bt_init_clean_ep2_bulk_in_buf_exit;
	}

	return HAL_OK;

bt_init_clean_ep2_bulk_in_buf_exit:
	ep = &iapp->in_ep[USB_EP_NUM(USBD_WHC_BT_EP_BULK_IN)];
	usb_os_mfree(ep->buf);
	ep->buf = NULL;

bt_init_clean_ep1_intr_in_buf_exit:
	ep = &iapp->in_ep[USB_EP_NUM(USBD_WHC_BT_EP_INTR_IN)];
	usb_os_mfree(ep->buf);
	ep->buf = NULL;

bt_init_exit:
	return ret;
}

/**
  * @brief  Initializes whc application layer
  * @param  None
  * @retval Status
  */
static int whc_cb_init(void)
{
	int ret = HAL_OK;

	ret = whc_wifi_init();
	if (ret != HAL_OK) {
		goto init_exit;
	}

	if (usbd_whc_is_bt_en()) {
		ret = whc_bt_init();
		if (ret != HAL_OK) {
			goto init_deinit_wifi_exit;
		}
	}

#ifdef CONFIG_WHC_ETH
	ret = whc_eth_init();
	if (ret != HAL_OK) {
		goto init_deinit_bt_exit;
	}
#endif

	return HAL_OK;

#ifdef CONFIG_WHC_ETH
init_deinit_bt_exit:
	if (usbd_whc_is_bt_en()) {
		whc_bt_deinit();
	}
#endif

init_deinit_wifi_exit:
	whc_wifi_deinit();
init_exit:
	return ret;
}

/**
  * @brief  DeInitializes whc application layer
  * @param  None
  * @retval Status
  */
static int whc_cb_deinit(void)
{
#ifdef CONFIG_WHC_ETH
	whc_eth_deinit();
#endif

	whc_wifi_deinit();

	if (usbd_whc_is_bt_en()) {
		whc_bt_deinit();
	}
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
	ep = &iapp->out_ep[USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_OUT_1)];
	usbd_whc_receive_data(USBD_WHC_WIFI_EP_BULK_OUT_1, ep->buf, ep->buf_len, NULL);
	ep = &iapp->out_ep[USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_OUT_2)];
	usbd_whc_receive_data(USBD_WHC_WIFI_EP_BULK_OUT_2, ep->buf, ep->buf_len, NULL);
	ep = &iapp->out_ep[USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_OUT_3)];
	usbd_whc_receive_data(USBD_WHC_WIFI_EP_BULK_OUT_3, ep->buf, ep->buf_len, NULL);

	if (usbd_whc_is_bt_en()) {
		ep = &iapp->out_ep[USB_EP_NUM(USBD_WHC_BT_EP_BULK_OUT)];
		usbd_whc_receive_data(USBD_WHC_BT_EP_BULK_OUT, ep->buf, ep->buf_len, NULL);
	}

#ifdef CONFIG_WHC_ETH
	ep = &iapp->out_ep[USB_EP_NUM(USBD_WHC_ETH_EP_BULK_OUT)];
	usbd_whc_receive_data(USBD_WHC_ETH_EP_BULK_OUT, ep->buf, ep->buf_len, NULL);
#endif

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
	case USBD_WHC_BT_EP_BULK_OUT:
		// Loopback with BT BULK IN
		ep_num = USB_EP_NUM(USBD_WHC_BT_EP_BULK_IN);
		ep_in = &iapp->in_ep[ep_num];
		usb_os_memcpy((void *)ep_in->buf, (const void *)ep->xfer_buf, len);
		ep_in->buf_len = len;

		rtos_sema_give(whc_bt_bulk_in_sema);
		break;
	case USBD_WHC_WIFI_EP_BULK_OUT_1:
		// Loopback with WiFi BULK IN
		ep_num = USB_EP_NUM(USBD_WHC_WIFI_EP_BULK_IN);
		ep_in = &iapp->in_ep[ep_num];
		usb_os_memcpy((void *)ep_in->buf, (const void *)ep->xfer_buf, len);
		ep_in->buf_len = len;

		rtos_sema_give(whc_wifi_bulk_in_sema);

		/* Framework auto re-arm excludes this EP; re-arm the next BULK OUT receive */
		usbd_whc_receive_data(USBD_WHC_WIFI_EP_BULK_OUT_1, ep->xfer_buf, ep->xfer_len, NULL);
		break;
	case USBD_WHC_WIFI_EP_BULK_OUT_2:
		// TBD
		break;
	case USBD_WHC_WIFI_EP_BULK_OUT_3:
		// TBD
		break;
#ifdef CONFIG_WHC_ETH
	case USBD_WHC_ETH_EP_BULK_OUT:
		// Loopback with Ethernet BULK IN
		ep_num = USB_EP_NUM(USBD_WHC_ETH_EP_BULK_IN);
		ep_in = &iapp->in_ep[ep_num];
		usb_os_memcpy((void *)ep_in->buf, (const void *)ep->xfer_buf, len);
		ep_in->buf_len = len;

		rtos_sema_give(whc_eth_bulk_in_sema);

		/* Framework auto re-arm excludes this EP; re-arm the next BULK OUT receive */
		usbd_whc_receive_data(USBD_WHC_ETH_EP_BULK_OUT, ep->xfer_buf, ep->xfer_len, NULL);
		break;
#endif
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
	case USBD_WHC_BT_EP_INTR_IN:
		// TBD
		break;
	case USBD_WHC_BT_EP_BULK_IN:
		// TBD
		break;
	case USBD_WHC_WIFI_EP_BULK_IN:
		// TBD
		break;
#ifdef CONFIG_WHC_ETH
	case USBD_WHC_ETH_EP_BULK_IN:
		// TBD
		break;
#endif
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

static void example_usbd_whc_bt_bulk_in_thread(void *param)
{
	UNUSED(param);
	usbd_whc_app_t *iapp = &usbd_whc_app;
	usbd_whc_app_ep_t *ep;
	u8 ep_num;

	ep_num = USB_EP_NUM(USBD_WHC_BT_EP_BULK_IN);
	ep = &iapp->in_ep[ep_num];

	for (;;) {
		if (rtos_sema_take(whc_bt_bulk_in_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
#if WHC_HOTPLUG
			if (whc_stack_fatal != 0U) {
				break;
			}
#endif
			if ((ep->buf != NULL) && (ep->buf_len != 0)) {
				usbd_whc_transmit_data(USBD_WHC_BT_EP_BULK_IN, ep->buf, ep->buf_len, NULL);
			}
		}
	}
	whc_bt_bulk_in_task = NULL;
	rtos_task_delete(NULL);
}

#ifdef CONFIG_WHC_ETH
static void example_usbd_whc_eth_bulk_in_thread(void *param)
{
	UNUSED(param);
	usbd_whc_app_t *iapp = &usbd_whc_app;
	usbd_whc_app_ep_t *ep;
	u8 ep_num;

	ep_num = USB_EP_NUM(USBD_WHC_ETH_EP_BULK_IN);
	ep = &iapp->in_ep[ep_num];

	for (;;) {
		if (rtos_sema_take(whc_eth_bulk_in_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
#if WHC_HOTPLUG
			if (whc_stack_fatal != 0U) {
				break;
			}
#endif
			if ((ep->buf != NULL) && (ep->buf_len != 0)) {
				usbd_whc_transmit_data(USBD_WHC_ETH_EP_BULK_IN, ep->buf, ep->buf_len, NULL);
			}
		}
	}
	whc_eth_bulk_in_task = NULL;
	rtos_task_delete(NULL);
}
#endif

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

	/* The stack is fully deinited here, no more ISR callback: stop the bulk IN
	   threads blocked on the semaphores, then free them as the last thread standing. */
	whc_stop_xfer_threads();
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
	ret = rtos_sema_create(&whc_bt_bulk_in_sema, 0, 1);
	if (ret != RTK_SUCCESS) {
		goto exit;
	}

	ret = rtos_sema_create(&whc_wifi_bulk_in_sema, 0, 1);
	if (ret != RTK_SUCCESS) {
		goto exit;
	}
#ifdef CONFIG_WHC_ETH
	ret = rtos_sema_create(&whc_eth_bulk_in_sema, 0, 1);
	if (ret != RTK_SUCCESS) {
		goto exit;
	}
#endif

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

	ret = rtos_task_create(&whc_bt_bulk_in_task, "usbd_whc_bt_bulk_in_thread",
						   example_usbd_whc_bt_bulk_in_thread, NULL,
						   WHC_XFER_THREAD_STACK_SIZE, WHC_XFER_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		goto clear_wifi_bulk_in_task;
	}

#ifdef CONFIG_WHC_ETH
	ret = rtos_task_create(&whc_eth_bulk_in_task, "usbd_whc_eth_bulk_in_thread",
						   example_usbd_whc_eth_bulk_in_thread, NULL,
						   WHC_XFER_THREAD_STACK_SIZE, WHC_XFER_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		goto clear_bt_bulk_in_task;
	}
#endif

#if WHC_HOTPLUG
	ret = rtos_task_create(&hotplug_task, "usbd_whc_hotplug_thread",
						   example_usbd_whc_hotplug_thread, NULL,
						   WHC_HOTPLUG_THREAD_STACK_SIZE, WHC_HOTPLUG_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
#ifdef CONFIG_WHC_ETH
		goto clear_eth_bulk_in_task;
#else
		goto clear_bt_bulk_in_task;
#endif
	}
#if defined(CONFIG_SMP)
	/* C-2: the USB OTG ISR is delivered on CPU0 (GIC ITARGETSR pins every SPI to core 0).
	   Pinning the hotplug/deinit thread to CPU0 puts it on the same core as the ISR,
	   so deinit's local interrupt disable is meaningful again under SMP. The wifi/bt/eth
	   bulk-in data threads stay unaffined: whc_stop_xfer_threads() gates them out of a
	   pending teardown, so they do not need to share the ISR's core. */
	rtos_task_set_affinity(hotplug_task, 0);
#endif
#endif // WHC_HOTPLUG

	rtos_time_delay_ms(100);

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD WHC demo start\n");

	rtos_task_delete(NULL);

	return;

#ifdef CONFIG_WHC_ETH
clear_eth_bulk_in_task:
	rtos_task_delete(whc_eth_bulk_in_task);
	whc_eth_bulk_in_task = NULL;
#endif

clear_bt_bulk_in_task:
	rtos_task_delete(whc_bt_bulk_in_task);
	whc_bt_bulk_in_task = NULL;

clear_wifi_bulk_in_task:
	rtos_task_delete(whc_wifi_bulk_in_task);
	whc_wifi_bulk_in_task = NULL;

clear_class_exit:
	usbd_whc_deinit();

clear_usb_driver_exit:
	usbd_deinit();

exit:
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD WHC demo stop\n");
	whc_free_resource();

	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

void example_usbd_whc(void)
{
	int ret;
	rtos_task_t task;

	ret = rtos_task_create(&task, "usbd_whc_thread", example_usbd_whc_thread, NULL,
						   WHC_INIT_THREAD_STACK_SIZE, WHC_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create USBD WHC thread fail\n");
	}
}
