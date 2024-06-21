/**
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------ */

#include <platform_autoconf.h>
#include "usbd.h"
#include "usbd_vendor.h"
#include "os_wrapper.h"

/* Private defines -----------------------------------------------------------*/
static const char *TAG = "VND";
// This configuration is used to enable a thread to check hotplug event
// and reset USB stack to avoid memory leak, only for example.
#define CONFIG_USBD_VENDOR_HOTPLUG						1

// USB speed
#ifdef CONFIG_USB_FS
#define CONFIG_USBD_VENDOR_SPEED						USB_SPEED_FULL
#else
#define CONFIG_USBD_VENDOR_SPEED						USB_SPEED_HIGH
#endif

// Loopback ISOC data in an async thread
#if CONFIG_USBD_VENDOR_ISOC_TEST
#define CONFIG_USBD_VENDOR_ISOC_ASYNC_XFER				1
#else
#define CONFIG_USBD_VENDOR_ISOC_ASYNC_XFER				0
#endif

#if CONFIG_USBD_VENDOR_INTR_TEST
// Loopback INTR data in an async thread
#define CONFIG_USBD_VENDOR_INTR_ASYNC_XFER				0
// INTR IN only
#define CONFIG_USBD_VENDOR_INTR_IN_ONLY					0
#else
// Loopback INTR data in an async thread
#define CONFIG_USBD_VENDOR_INTR_ASYNC_XFER				0
// INTR IN only
#define CONFIG_USBD_VENDOR_INTR_IN_ONLY					0
#endif

// Loopback BULK data in an async thread
#if CONFIG_USBD_VENDOR_BULK_TEST
#define CONFIG_USBD_VENDOR_BULK_ASYNC_XFER				1
#else
#define CONFIG_USBD_VENDOR_BULK_ASYNC_XFER				0
#endif

// Thread priorities
#define CONFIG_USBD_VENDOR_INIT_THREAD_PRIORITY			5U
#define CONFIG_USBD_VENDOR_ISR_THREAD_PRIORITY			7U
#define CONFIG_USBD_VENDOR_HOTPLUG_THREAD_PRIORITY		8U // Should be higher than CONFIG_USBD_VENDOR_ISR_THREAD_PRIORITY
#define CONFIG_USBD_VENDOR_XFER_THREAD_PRIORITY			6U // Should be lower than CONFIG_USBD_VENDOR_ISR_THREAD_PRIORITY

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/

static int vendor_cb_init(void);
static int vendor_cb_deinit(void);
static int vendor_cb_setup(usb_setup_req_t *req, u8 *buf);
static int vendor_cb_set_config(void);
#if CONFIG_USBD_VENDOR_BULK_TEST
static int vendor_cb_bulk_received(u8 *buf, u32 len);
#endif
#if CONFIG_USBD_VENDOR_INTR_TEST
static int vendor_cb_intr_received(u8 *buf, u32 len);
#if CONFIG_USBD_VENDOR_INTR_IN_ONLY
static void vendor_cb_intr_transmitted(u8 status);
#endif
#endif
#if CONFIG_USBD_VENDOR_ISOC_TEST
static int vendor_cb_isoc_received(u8 *buf, u32 len);
#endif
static void vendor_cb_status_changed(u8 status);

/* Private variables ---------------------------------------------------------*/

static usbd_config_t vendor_cfg = {
	.speed = CONFIG_USBD_VENDOR_SPEED,
	.dma_enable = 1U,
	.ptx_fifo_first = 1U,
	.isr_priority = CONFIG_USBD_VENDOR_ISR_THREAD_PRIORITY,
	.ext_intr_en =  USBD_EOPF_INTR | USBD_ICII_INTR,
	.intr_use_ptx_fifo = 0U,
};

static usbd_vendor_cb_t vendor_cb = {
	.init = vendor_cb_init,
	.deinit = vendor_cb_deinit,
	.setup = vendor_cb_setup,
	.set_config = vendor_cb_set_config,
#if CONFIG_USBD_VENDOR_BULK_TEST
	.bulk_received = vendor_cb_bulk_received,
#endif
#if CONFIG_USBD_VENDOR_ISOC_TEST
	.isoc_received = vendor_cb_isoc_received,
#endif
#if CONFIG_USBD_VENDOR_INTR_TEST
	.intr_received = vendor_cb_intr_received,
#if CONFIG_USBD_VENDOR_INTR_IN_ONLY
	.isoc_transmitted = vendor_cb_intr_transmitted,
#endif
#endif
	.status_changed = vendor_cb_status_changed,
};

#if CONFIG_USBD_VENDOR_ISOC_ASYNC_XFER
static u8 *vendor_isoc_tx_buf = NULL;
static u32 vendor_isoc_tx_len = 0;
static rtos_sema_t vendor_isoc_async_xfer_sema;
#endif

#if CONFIG_USBD_VENDOR_INTR_ASYNC_XFER
static u8 *vendor_intr_tx_buf = NULL;
static u32 vendor_intr_tx_len = 0;
static rtos_sema_t vendor_intr_async_xfer_sema;
#endif

#if CONFIG_USBD_VENDOR_INTR_IN_ONLY
static u8 vendor_intr_tx_num = 0;
static u8 vendor_intr_in_buf[USBD_VENDOR_INTR_IN_BUF_SIZE];
#endif

#if CONFIG_USBD_VENDOR_BULK_ASYNC_XFER
static u8 *vendor_bulk_tx_buf = NULL;
static u32 vendor_bulk_tx_len = 0;
static rtos_sema_t vendor_bulk_async_xfer_sema;
#endif

#if CONFIG_USBD_VENDOR_HOTPLUG
static u8 vendor_attach_status;
static rtos_sema_t vendor_attach_status_changed_sema;
#endif

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Handle the vendor class control requests
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
#if CONFIG_USBD_VENDOR_INTR_ASYNC_XFER
	vendor_intr_tx_buf = NULL;
	vendor_intr_tx_len = 0;
	rtos_sema_create(&vendor_intr_async_xfer_sema, 0U, 1U);
#endif
#if CONFIG_USBD_VENDOR_ISOC_ASYNC_XFER
	vendor_isoc_tx_buf = NULL;
	vendor_isoc_tx_len = 0;
	rtos_sema_create(&vendor_isoc_async_xfer_sema, 0U, 1U);
#endif
#if CONFIG_USBD_VENDOR_BULK_ASYNC_XFER
	vendor_bulk_tx_buf = NULL;
	vendor_bulk_tx_len = 0;
	rtos_sema_create(&vendor_bulk_async_xfer_sema, 0U, 1U);
#endif
	return HAL_OK;
}

/**
  * @brief  DeInitializes vendor application layer
  * @param  None
  * @retval Status
  */
static int vendor_cb_deinit(void)
{
#if CONFIG_USBD_VENDOR_INTR_ASYNC_XFER
	rtos_sema_delete(vendor_intr_async_xfer_sema);
#endif
#if CONFIG_USBD_VENDOR_ISOC_ASYNC_XFER
	rtos_sema_delete(vendor_isoc_async_xfer_sema);
#endif
#if CONFIG_USBD_VENDOR_BULK_ASYNC_XFER
	rtos_sema_delete(vendor_bulk_async_xfer_sema);
#endif
	return HAL_OK;
}

/**
  * @brief  Set config callback
  * @param  None
  * @retval Status
  */
static int vendor_cb_set_config(void)
{
#if CONFIG_USBD_VENDOR_INTR_IN_ONLY
	vendor_intr_tx_num = 0;
	memset((void *)vendor_intr_in_buf, 0, USBD_VENDOR_INTR_IN_BUF_SIZE);
	usbd_vendor_transmit_intr_data(vendor_intr_in_buf, USBD_VENDOR_INTR_IN_BUF_SIZE);
#endif
	return HAL_OK;
}

#if CONFIG_USBD_VENDOR_INTR_TEST

/**
  * @brief  Data received over USB INTR OUT endpoint
  * @param  buf: RX buffer
  * @param  len: RX data length (in bytes)
  * @retval Status
  */
static int vendor_cb_intr_received(u8 *buf, u32 len)
{
#if CONFIG_USBD_VENDOR_INTR_ASYNC_XFER
	vendor_intr_tx_buf = buf;
	vendor_intr_tx_len = len;
	rtos_sema_give(vendor_intr_async_xfer_sema);
#else
	usbd_vendor_transmit_intr_data(buf, len);
#endif // CONFIG_USBD_VENDOR_INTR_ASYNC_XFER
	return usbd_vendor_receive_intr_data();
}

#if CONFIG_USBD_VENDOR_INTR_IN_ONLY
/**
  * @brief  Data transmitted over USB INTR IN endpoint
  * @param  state: Transmitted status
  * @retval Status
  */
static void vendor_cb_intr_transmitted(u8 status)
{
	UNUSED(status);
	memset((void *)vendor_intr_in_buf, ++vendor_intr_tx_num, USBD_VENDOR_INTR_IN_BUF_SIZE);
	usbd_vendor_transmit_intr_data(vendor_intr_in_buf, USBD_VENDOR_INTR_IN_BUF_SIZE);
}
#endif // CONFIG_USBD_VENDOR_INTR_IN_ONLY

#if CONFIG_USBD_VENDOR_INTR_ASYNC_XFER
static void vendor_intr_xfer_thread(void *param)
{
	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(vendor_intr_async_xfer_sema, RTOS_SEMA_MAX_COUNT) == SUCCESS) {
			if ((vendor_intr_tx_buf != NULL) && (vendor_intr_tx_len != 0)) {
				usbd_vendor_transmit_intr_data(vendor_intr_tx_buf, vendor_intr_tx_len);
			}
		}
	}
}
#endif // CONFIG_USBD_VENDOR_INTR_ASYNC_XFER

#endif // CONFIG_USBD_VENDOR_INTR_TEST

#if CONFIG_USBD_VENDOR_ISOC_TEST
/**
  * @brief  Data received over USB ISOC OUT endpoint
  * @param  buf: RX buffer
  * @param  len: RX data length (in bytes)
  * @retval Status
  */
static int vendor_cb_isoc_received(u8 *buf, u32 len)
{
#if CONFIG_USBD_VENDOR_ISOC_ASYNC_XFER
	vendor_isoc_tx_buf = buf;
	vendor_isoc_tx_len = len;
	rtos_sema_give(vendor_isoc_async_xfer_sema);
#else
	usbd_vendor_transmit_isoc_data(buf, len);
#endif // CONFIG_USBD_VENDOR_ISOC_ASYNC_XFER
	return usbd_vendor_receive_isoc_data();
}

#if CONFIG_USBD_VENDOR_ISOC_ASYNC_XFER
static void vendor_isoc_xfer_thread(void *param)
{
	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(vendor_isoc_async_xfer_sema, RTOS_SEMA_MAX_COUNT) == SUCCESS) {
			if ((vendor_isoc_tx_buf != NULL) && (vendor_isoc_tx_len != 0)) {
				usbd_vendor_transmit_isoc_data(vendor_isoc_tx_buf, vendor_isoc_tx_len);
			}
		}
	}
}
#endif // CONFIG_USBD_VENDOR_ISOC_ASYNC_XFER

#endif // CONFIG_USBD_VENDOR_ISOC_TEST

#if CONFIG_USBD_VENDOR_BULK_TEST

/**
  * @brief  Data received over USB BULK OUT endpoint
  * @param  buf: RX buffer
  * @param  len: RX data length (in bytes)
  * @retval Status
  */
static int vendor_cb_bulk_received(u8 *buf, u32 len)
{
#if CONFIG_USBD_VENDOR_BULK_ASYNC_XFER
	vendor_bulk_tx_buf = buf;
	vendor_bulk_tx_len = len;
	rtos_sema_give(vendor_bulk_async_xfer_sema);
#else
	usbd_vendor_transmit_bulk_data(buf, len);
#endif // CONFIG_USBD_VENDOR_BULK_ASYNC_XFER
	return usbd_vendor_receive_bulk_data();
}

#if CONFIG_USBD_VENDOR_BULK_ASYNC_XFER
static void vendor_bulk_xfer_thread(void *param)
{
	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(vendor_bulk_async_xfer_sema, RTOS_SEMA_MAX_COUNT) == SUCCESS) {
			if ((vendor_bulk_tx_buf != NULL) && (vendor_bulk_tx_len != 0)) {
				usbd_vendor_transmit_bulk_data(vendor_bulk_tx_buf, vendor_bulk_tx_len);
			}
		}
	}
}
#endif // CONFIG_USBD_VENDOR_BULK_ASYNC_XFER

#endif // CONFIG_USBD_VENDOR_BULK_TEST

static void vendor_cb_status_changed(u8 status)
{
	RTK_LOGS(TAG, "[VND] Status change: %d\n", status);
#if CONFIG_USBD_VENDOR_HOTPLUG
	vendor_attach_status = status;
	rtos_sema_give(vendor_attach_status_changed_sema);
#endif
}

#if CONFIG_USBD_VENDOR_HOTPLUG
static void vendor_hotplug_thread(void *param)
{
	int ret = 0;

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(vendor_attach_status_changed_sema, RTOS_SEMA_MAX_COUNT) == SUCCESS) {
			if (vendor_attach_status == USBD_ATTACH_STATUS_DETACHED) {
				RTK_LOGS(TAG, "[VND] DETACHED\n");
				usbd_vendor_deinit();
				ret = usbd_deinit();
				if (ret != 0) {
					break;
				}
				RTK_LOGS(TAG, "[VND] Free heap: 0x%x\n", rtos_mem_get_free_heap_size());
				ret = usbd_init(&vendor_cfg);
				if (ret != 0) {
					break;
				}
				ret = usbd_vendor_init(&vendor_cb);
				if (ret != 0) {
					usbd_deinit();
					break;
				}
			} else if (vendor_attach_status == USBD_ATTACH_STATUS_ATTACHED) {
				RTK_LOGS(TAG, "[VND] ATTACHED\n");
			} else {
				RTK_LOGS(TAG, "[VND] INIT\n");
			}
		}
	}
	RTK_LOGS(TAG, "[VND] Hotplug thread fail\n");
	rtos_task_delete(NULL);
}
#endif // CONFIG_USBD_VENDOR_HOTPLUG

static void example_usbd_vendor_thread(void *param)
{
	int ret = 0;
#if CONFIG_USBD_VENDOR_HOTPLUG
	rtos_task_t check_status_task;
#endif
#if CONFIG_USBD_VENDOR_INTR_ASYNC_XFER
	rtos_task_t intr_async_xfer_task;
#endif
#if CONFIG_USBD_VENDOR_ISOC_ASYNC_XFER
	rtos_task_t isoc_async_xfer_task;
#endif
#if CONFIG_USBD_VENDOR_BULK_ASYNC_XFER
	rtos_task_t bulk_async_xfer_task;
#endif

	UNUSED(param);

#if CONFIG_USBD_VENDOR_HOTPLUG
	rtos_sema_create(&vendor_attach_status_changed_sema, 0U, 1U);
#endif

	ret = usbd_init(&vendor_cfg);
	if (ret != HAL_OK) {
		goto exit;
	}

	ret = usbd_vendor_init(&vendor_cb);
	if (ret != HAL_OK) {
		goto clear_usb_driver_exit;
	}

#if CONFIG_USBD_VENDOR_HOTPLUG
	ret = rtos_task_create(&check_status_task, "vendor_hotplug_thread", vendor_hotplug_thread, NULL, 1024U, CONFIG_USBD_VENDOR_HOTPLUG_THREAD_PRIORITY);
	if (ret != SUCCESS) {
		goto clear_usb_class_exit;
	}
#endif // CONFIG_USBD_VENDOR_HOTPLUG
#if CONFIG_USBD_VENDOR_INTR_ASYNC_XFER
	// The priority of transfer thread shall be lower than USB isr priority
	ret = rtos_task_create(&intr_async_xfer_task, "vendor_intr_xfer_thread", vendor_intr_xfer_thread, NULL, 1024U, CONFIG_USBD_VENDOR_XFER_THREAD_PRIORITY);
	if (ret != SUCCESS) {
		goto clear_check_status_task;
	}
#endif // CONFIG_USBD_VENDOR_INTR_ASYNC_XFER
#if CONFIG_USBD_VENDOR_ISOC_ASYNC_XFER
	// The priority of transfer thread shall be lower than USB isr priority
	ret = rtos_task_create(&isoc_async_xfer_task, "vendor_isoc_xfer_thread", vendor_isoc_xfer_thread, NULL, 1024U, CONFIG_USBD_VENDOR_XFER_THREAD_PRIORITY);
	if (ret != SUCCESS) {
		goto clear_intr_async_task;
	}
#endif // CONFIG_USBD_VENDOR_ISOC_ASYNC_XFER
#if CONFIG_USBD_VENDOR_BULK_ASYNC_XFER
	// The priority of transfer thread shall be lower than USB isr priority
	ret = rtos_task_create(&bulk_async_xfer_task, "vendor_bulk_xfer_thread", vendor_bulk_xfer_thread, NULL, 1024U, CONFIG_USBD_VENDOR_XFER_THREAD_PRIORITY);
	if (ret != SUCCESS) {
		goto clear_isoc_async_task;
	}
#endif // CONFIG_USBD_VENDOR_BULK_ASYNC_XFER

	rtos_time_delay_ms(100);

	RTK_LOGS(TAG, "[VND] USBD vendor demo start\n");

	rtos_task_delete(NULL);

	return;

#if CONFIG_USBD_VENDOR_BULK_ASYNC_XFER
clear_isoc_async_task:
#endif

#if CONFIG_USBD_VENDOR_ISOC_ASYNC_XFER
	rtos_task_delete(isoc_async_xfer_task);
clear_intr_async_task:
#endif

#if CONFIG_USBD_VENDOR_INTR_ASYNC_XFER
	rtos_task_delete(intr_async_xfer_task);
clear_check_status_task:
#endif

#if CONFIG_USBD_VENDOR_HOTPLUG
	rtos_task_delete(check_status_task);

clear_usb_class_exit:
#endif
	usbd_vendor_deinit();

clear_usb_driver_exit:
	usbd_deinit();

exit:
	RTK_LOGS(TAG, "[VND] USBD vendor demo stop\n");
#if CONFIG_USBD_VENDOR_HOTPLUG
	rtos_sema_delete(vendor_attach_status_changed_sema);
#endif
	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

void example_usbd_vendor(void)
{
	int status;
	rtos_task_t task;

	status = rtos_task_create(&task, "example_usbd_vendor_thread", example_usbd_vendor_thread, NULL, 1024U, CONFIG_USBD_VENDOR_INIT_THREAD_PRIORITY);
	if (status != SUCCESS) {
		RTK_LOGS(TAG, "[VND] Create USBD vendor thread fail\n");
	}
}

