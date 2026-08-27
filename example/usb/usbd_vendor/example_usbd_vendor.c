/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */

#include <platform_autoconf.h>
#include "usbd_vendor.h"

#include "os_wrapper.h"

/* Private defines -----------------------------------------------------------*/

// Endpoint address
#if defined (CONFIG_AMEBAGREEN2)
#define VENDOR_BULK_IN_EP                         0x86U
#define VENDOR_BULK_OUT_EP                        0x03U
#define VENDOR_ISOC_IN_EP                         0x82U
#define VENDOR_ISOC_OUT_EP                        0x02U
#define VENDOR_INTR_IN_EP                         0x84U
#define VENDOR_INTR_OUT_EP                        0x05U
#elif defined (CONFIG_AMEBAL2)
#define VENDOR_BULK_IN_EP                         0x84U
#define VENDOR_BULK_OUT_EP                        0x04U
#define VENDOR_ISOC_IN_EP                         0x81U
#define VENDOR_ISOC_OUT_EP                        0x01U
#define VENDOR_INTR_IN_EP                         0x82U
#define VENDOR_INTR_OUT_EP                        0x02U
#else
#define VENDOR_BULK_IN_EP                         0x81U
#define VENDOR_BULK_OUT_EP                        0x02U
#define VENDOR_ISOC_IN_EP                         0x83U
#define VENDOR_ISOC_OUT_EP                        0x04U
#define VENDOR_INTR_IN_EP                         0x85U
#define VENDOR_INTR_OUT_EP                        0x05U
#endif

// This configuration is used to enable a thread to check hotplug event
// and reset USB stack to avoid memory leak, only for example.
// while test suspend/resume, hotplug should be disabled
#define VENDOR_HOTPLUG                          1

// USB speed
#ifdef CONFIG_SUPPORT_USB_FS_ONLY
#define VENDOR_USB_SPEED                        USB_SPEED_FULL
#else
#define VENDOR_USB_SPEED                        USB_SPEED_HIGH
#endif

// Loopback ISOC data in an async thread
#define VENDOR_ISOC_ASYNC_XFER                  0

// Loopback INTR data in an async thread
#define VENDOR_INTR_ASYNC_XFER                  0

// Loopback BULK data in an async thread
#define VENDOR_BULK_ASYNC_XFER                  0

// Thread priorities
#define VENDOR_INIT_THREAD_PRIORITY             5
#define VENDOR_HOTPLUG_THREAD_PRIORITY          8
#define VENDOR_XFER_THREAD_PRIORITY             6

// Thread stack sizes
#define VENDOR_INIT_THREAD_STACK_SIZE           1024U
#define VENDOR_HOTPLUG_THREAD_STACK_SIZE        1024U
#define VENDOR_XFER_THREAD_STACK_SIZE           1024U

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/

static int vendor_cb_init(void);
static int vendor_cb_deinit(void);
static int vendor_cb_setup(usb_setup_req_t *req, u8 *buf);
static int vendor_cb_set_config(void);
static int vendor_cb_bulk_received(u8 *buf, u32 len);
static int vendor_cb_intr_received(u8 *buf, u32 len);
static int vendor_cb_isoc_received(u8 *buf, u32 len);
static void vendor_cb_status_changed(u8 old_status, u8 status);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "VND";

static const usbd_config_t vendor_cfg = {
	.speed = VENDOR_USB_SPEED,
	.isr_priority = INT_PRI_MIDDLE,
#if defined(CONFIG_AMEBASMART) || defined(CONFIG_AMEBAD) || defined(CONFIG_AMEBADPLUS)
	.nptx_max_epmis_cnt = 100U,
	.intr_use_ptx_fifo = 0U,
#elif defined (CONFIG_AMEBAGREEN2)
	/*DFIFO total 1024 DWORD, resv 12 DWORD for DMA addr and EP0 fixed 32 DWORD*/
	.rx_fifo_depth = 292U,
	.ptx_fifo_depth = {16U, 256U, 32U, 256U, 128U, },
#elif defined (CONFIG_AMEBAL2)
	/*DFIFO total 1024 DWORD, resv 11 DWORD for DMA addr and EP0 fixed 32 DWORD*/
	.rx_fifo_depth = 405U,
	.ptx_fifo_depth = {256U, 256U, 32U, 32U},
#elif defined (CONFIG_AMEBAPRO3)
	/*DFIFO total 2232 DWORD, resv 8 DWORD for DMA addr and EP0 fixed 256 DWORD*/
	.rx_fifo_depth = 1424U,
	.ptx_fifo_depth = {256U, 256U, 32U},
#endif
};

static const usbd_vendor_ep_cfg_t vendor_ep = {
	.bulk_in_xfer_size = USBD_VENDOR_HS_BULK_MPS,
	.bulk_out_xfer_size = USBD_VENDOR_HS_BULK_MPS,
	.isoc_in_xfer_size = USBD_VENDOR_HS_ISOC_MPS,
	.isoc_out_xfer_size = USBD_VENDOR_HS_ISOC_MPS,
	.intr_in_xfer_size = USBD_VENDOR_HS_INTR_MPS,
	.intr_out_xfer_size = USBD_VENDOR_HS_INTR_MPS,
	.bulk_in_addr  = VENDOR_BULK_IN_EP,
	.bulk_out_addr = VENDOR_BULK_OUT_EP,
	.isoc_in_addr  = VENDOR_ISOC_IN_EP,
	.isoc_out_addr = VENDOR_ISOC_OUT_EP,
	.intr_in_addr  = VENDOR_INTR_IN_EP,
	.intr_out_addr = VENDOR_INTR_OUT_EP,
};

static const usbd_vendor_cb_t vendor_cb = {
	.init = vendor_cb_init,
	.deinit = vendor_cb_deinit,
	.setup = vendor_cb_setup,
	.set_config = vendor_cb_set_config,
	.bulk_received = vendor_cb_bulk_received,
	.isoc_received = vendor_cb_isoc_received,
	.intr_received = vendor_cb_intr_received,
	.status_changed = vendor_cb_status_changed,
};

#if VENDOR_ISOC_ASYNC_XFER
static u8 *vendor_isoc_tx_buf = NULL;
static u32 vendor_isoc_tx_len = 0;
static rtos_sema_t vendor_isoc_async_xfer_sema;
#endif

#if VENDOR_INTR_ASYNC_XFER
static u8 *vendor_intr_tx_buf = NULL;
static u32 vendor_intr_tx_len = 0;
static rtos_sema_t vendor_intr_async_xfer_sema;
#endif

#if VENDOR_BULK_ASYNC_XFER
static u8 *vendor_bulk_tx_buf = NULL;
static u32 vendor_bulk_tx_len = 0;
static rtos_sema_t vendor_bulk_async_xfer_sema;
#endif

#if VENDOR_HOTPLUG
static u8 vendor_attach_status;
static rtos_sema_t vendor_attach_status_changed_sema;
#endif

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Handle the vendor class control requests
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  cmd: Command code
  * @param  buf: Buffer containing command data (request parameters)
  * @param  len: Number of data to be sent (in bytes)
  * @param  value: Value for the command code
  * @retval Status
  */
static int vendor_cb_setup(usb_setup_req_t *req, u8 *buf)
{
	UNUSED(buf);

	if ((req->bmRequestType & USB_REQ_DIR_MASK) == USB_D2H) {
		// TBD
	} else {
		// TBD
	}

	return HAL_OK;
}

/**
  * @brief  Initializes vendor application layer
  * @param  None
  * @retval Status
  */
static int vendor_cb_init(void)
{
	return HAL_OK;
}

/**
  * @brief  DeInitializes vendor application layer
  * @param  None
  * @retval Status
  */
static int vendor_cb_deinit(void)
{
	return HAL_OK;
}

/**
  * @brief  Set config callback
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  None
  * @retval Status
  */
static int vendor_cb_set_config(void)
{
	return HAL_OK;
}

/**
  * @brief  Data received over USB INTR OUT endpoint
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  buf: RX buffer
  * @param  len: RX data length (in bytes)
  * @retval Status
  */
static int vendor_cb_intr_received(u8 *buf, u32 len)
{
#if VENDOR_INTR_ASYNC_XFER
	vendor_intr_tx_buf = buf;
	vendor_intr_tx_len = len;
	rtos_sema_give(vendor_intr_async_xfer_sema);
#else
	return usbd_vendor_transmit_intr_data(buf, len);
#endif // VENDOR_INTR_ASYNC_XFER
}

#if VENDOR_INTR_ASYNC_XFER
static void example_usbd_vendor_intr_xfer_thread(void *param)
{
	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(vendor_intr_async_xfer_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			if ((vendor_intr_tx_buf != NULL) && (vendor_intr_tx_len != 0)) {
				usbd_vendor_transmit_intr_data(vendor_intr_tx_buf, vendor_intr_tx_len);
			}
		}
	}
	rtos_task_delete(NULL);
}
#endif // VENDOR_INTR_ASYNC_XFER


/**
  * @brief  Data received over USB ISOC OUT endpoint
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  buf: RX buffer
  * @param  len: RX data length (in bytes)
  * @retval Status
  */
static int vendor_cb_isoc_received(u8 *buf, u32 len)
{
#if VENDOR_ISOC_ASYNC_XFER
	vendor_isoc_tx_buf = buf;
	vendor_isoc_tx_len = len;
	rtos_sema_give(vendor_isoc_async_xfer_sema);
#else
	return usbd_vendor_transmit_isoc_data(buf, len);
#endif // VENDOR_ISOC_ASYNC_XFER
}

#if VENDOR_ISOC_ASYNC_XFER
static void example_usbd_vendor_isoc_xfer_thread(void *param)
{
	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(vendor_isoc_async_xfer_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			if ((vendor_isoc_tx_buf != NULL) && (vendor_isoc_tx_len != 0)) {
				usbd_vendor_transmit_isoc_data(vendor_isoc_tx_buf, vendor_isoc_tx_len);
			}
		}
	}
	rtos_task_delete(NULL);
}
#endif // VENDOR_ISOC_ASYNC_XFER

/**
  * @brief  Data received over USB BULK OUT endpoint
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  buf: RX buffer
  * @param  len: RX data length (in bytes)
  * @retval Status
  */
static int vendor_cb_bulk_received(u8 *buf, u32 len)
{
#if VENDOR_BULK_ASYNC_XFER
	vendor_bulk_tx_buf = buf;
	vendor_bulk_tx_len = len;
	rtos_sema_give(vendor_bulk_async_xfer_sema);
#else
	return usbd_vendor_transmit_bulk_data(buf, len);
#endif // VENDOR_BULK_ASYNC_XFER
}

#if VENDOR_BULK_ASYNC_XFER
static void example_usbd_vendor_bulk_xfer_thread(void *param)
{
	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(vendor_bulk_async_xfer_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			if ((vendor_bulk_tx_buf != NULL) && (vendor_bulk_tx_len != 0)) {
				usbd_vendor_transmit_bulk_data(vendor_bulk_tx_buf, vendor_bulk_tx_len);
			}
		}
	}
	rtos_task_delete(NULL);
}
#endif // VENDOR_BULK_ASYNC_XFER

/**
  * @brief  Handle vendor class attach status change notifications from the USB stack
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  old_status: Previous attach status
  * @param  status: New attach status
  * @retval None
  */
static void vendor_cb_status_changed(u8 old_status, u8 status)
{
	UNUSED(old_status);

#if VENDOR_HOTPLUG
	vendor_attach_status = status;
	rtos_sema_give(vendor_attach_status_changed_sema);
#else
	UNUSED(status);
#endif
}

#if VENDOR_HOTPLUG
static void example_usbd_vendor_hotplug_thread(void *param)
{
	int ret = 0;

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(vendor_attach_status_changed_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			if (vendor_attach_status == USBD_ATTACH_STATUS_DETACHED) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "DETACHED\n");
				usbd_vendor_deinit();
				ret = usbd_deinit();
				if (ret != 0) {
					break;
				}
				RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());
				ret = usbd_init(&vendor_cfg);
				if (ret != 0) {
					break;
				}
				ret = usbd_vendor_init(&vendor_cb, &vendor_ep);
				if (ret != 0) {
					usbd_deinit();
					break;
				}
			} else if (vendor_attach_status == USBD_ATTACH_STATUS_ATTACHED) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "ATTACHED\n");
			} else {
				RTK_LOGS(TAG, RTK_LOG_INFO, "INIT\n");
			}
		}
	}
	RTK_LOGS(TAG, RTK_LOG_ERROR, "Hotplug thread fail\n");
	rtos_task_delete(NULL);
}
#endif // VENDOR_HOTPLUG

static void example_usbd_vendor_thread(void *param)
{
	int ret = 0;
#if VENDOR_HOTPLUG
	rtos_task_t check_status_task;
#endif
#if VENDOR_INTR_ASYNC_XFER
	rtos_task_t intr_async_xfer_task;
#endif
#if VENDOR_ISOC_ASYNC_XFER
	rtos_task_t isoc_async_xfer_task;
#endif
#if VENDOR_BULK_ASYNC_XFER
	rtos_task_t bulk_async_xfer_task;
#endif

	UNUSED(param);

#if VENDOR_HOTPLUG
	rtos_sema_create(&vendor_attach_status_changed_sema, 0U, 1U);
#endif

#if VENDOR_INTR_ASYNC_XFER
	vendor_intr_tx_buf = NULL;
	vendor_intr_tx_len = 0;
	rtos_sema_create(&vendor_intr_async_xfer_sema, 0U, 1U);
#endif
#if VENDOR_ISOC_ASYNC_XFER
	vendor_isoc_tx_buf = NULL;
	vendor_isoc_tx_len = 0;
	rtos_sema_create(&vendor_isoc_async_xfer_sema, 0U, 1U);
#endif
#if VENDOR_BULK_ASYNC_XFER
	vendor_bulk_tx_buf = NULL;
	vendor_bulk_tx_len = 0;
	rtos_sema_create(&vendor_bulk_async_xfer_sema, 0U, 1U);
#endif

	ret = usbd_init(&vendor_cfg);
	if (ret != HAL_OK) {
		goto exit;
	}

	ret = usbd_vendor_init(&vendor_cb, &vendor_ep);
	if (ret != HAL_OK) {
		goto clear_usb_driver_exit;
	}

#if VENDOR_HOTPLUG
	ret = rtos_task_create(&check_status_task, "usbd_vendor_hotplug_thread",
						   example_usbd_vendor_hotplug_thread, NULL,
						   VENDOR_HOTPLUG_THREAD_STACK_SIZE, VENDOR_HOTPLUG_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		goto clear_usb_class_exit;
	}
#endif // VENDOR_HOTPLUG
#if VENDOR_INTR_ASYNC_XFER
	// The priority of transfer thread shall be lower than USB isr priority
	ret = rtos_task_create(&intr_async_xfer_task, "usbd_vendor_intr_xfer_thread",
						   example_usbd_vendor_intr_xfer_thread, NULL,
						   VENDOR_XFER_THREAD_STACK_SIZE, VENDOR_XFER_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		goto clear_check_status_task;
	}
#endif // VENDOR_INTR_ASYNC_XFER
#if VENDOR_ISOC_ASYNC_XFER
	// The priority of transfer thread shall be lower than USB isr priority
	ret = rtos_task_create(&isoc_async_xfer_task, "usbd_vendor_isoc_xfer_thread",
						   example_usbd_vendor_isoc_xfer_thread, NULL,
						   VENDOR_XFER_THREAD_STACK_SIZE, VENDOR_XFER_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		goto clear_intr_async_task;
	}
#endif // VENDOR_ISOC_ASYNC_XFER
#if VENDOR_BULK_ASYNC_XFER
	// The priority of transfer thread shall be lower than USB isr priority
	ret = rtos_task_create(&bulk_async_xfer_task, "usbd_vendor_bulk_xfer_thread",
						   example_usbd_vendor_bulk_xfer_thread, NULL,
						   VENDOR_XFER_THREAD_STACK_SIZE, VENDOR_XFER_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		goto clear_isoc_async_task;
	}
#endif // VENDOR_BULK_ASYNC_XFER

	rtos_time_delay_ms(100);

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD vendor demo start\n");

	rtos_task_delete(NULL);

	return;

#if VENDOR_BULK_ASYNC_XFER
clear_isoc_async_task:
#endif

#if VENDOR_ISOC_ASYNC_XFER
	rtos_task_delete(isoc_async_xfer_task);
clear_intr_async_task:
#endif

#if VENDOR_INTR_ASYNC_XFER
	rtos_task_delete(intr_async_xfer_task);
clear_check_status_task:
#endif

#if VENDOR_HOTPLUG
	rtos_task_delete(check_status_task);

clear_usb_class_exit:
#endif
	usbd_vendor_deinit();

clear_usb_driver_exit:
	usbd_deinit();

exit:
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD vendor demo stop\n");
#if VENDOR_HOTPLUG
	rtos_sema_delete(vendor_attach_status_changed_sema);
#endif
#if VENDOR_INTR_ASYNC_XFER
	rtos_sema_delete(vendor_intr_async_xfer_sema);
#endif
#if VENDOR_ISOC_ASYNC_XFER
	rtos_sema_delete(vendor_isoc_async_xfer_sema);
#endif
#if VENDOR_BULK_ASYNC_XFER
	rtos_sema_delete(vendor_bulk_async_xfer_sema);
#endif
	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

void example_usbd_vendor(void)
{
	int ret;
	rtos_task_t task;

	ret = rtos_task_create(&task, "usbd_vendor_thread", example_usbd_vendor_thread, NULL,
						   VENDOR_INIT_THREAD_STACK_SIZE, VENDOR_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create USBD vendor thread fail\n");
	}
}
