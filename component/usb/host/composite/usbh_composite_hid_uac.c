/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/
#include "usbh_composite_hid_uac.h"
#include "usb_uac1.h"

/* Private defines -----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static int usbh_composite_hid_uac_attach(usb_host_t *host);
static int usbh_composite_hid_uac_detach(usb_host_t *host);
static int usbh_composite_hid_uac_process(usb_host_t *host, usbh_event_t *event);
static int usbh_composite_hid_uac_setup(usb_host_t *host);
static int usbh_composite_hid_uac_sof(usb_host_t *host);
static int usbh_composite_hid_uac_completed(usb_host_t *host, u8 pipe_num);

/* Private variables ---------------------------------------------------------*/
static const char *const TAG = "COMP";

static const usbh_dev_id_t composite_devs[] = {
	{
		/* Match UAC by class + subclass only, NOT bInterfaceProtocol: we also want
		   to attach to UAC 2.0 devices (protocol 0x20, which can run at Full Speed)
		   so usbh_composite_uac_parse_interface_desc() can reject them with a clear
		   "UAC 2.0 not supported" message. Matching on protocol==0x00 here would
		   make UAC2 devices match no driver and report a generic probe fail. */
		.mMatchFlags = USBH_DEV_ID_MATCH_ITF_CLASS | USBH_DEV_ID_MATCH_ITF_SUBCLASS,
		.bInterfaceClass = USB_UAC1_CLASS_CODE,
		.bInterfaceSubClass = USB_UAC1_SUBCLASS_AUDIOSTREAMING,
	},
	{
	},
};

/* USB Class driver */
static usbh_class_driver_t usbh_composite_driver = {
	.id_table = composite_devs,
	.attach = usbh_composite_hid_uac_attach,
	.detach = usbh_composite_hid_uac_detach,
	.setup = usbh_composite_hid_uac_setup,
	.process = usbh_composite_hid_uac_process,
	.sof = usbh_composite_hid_uac_sof,
	.completed = usbh_composite_hid_uac_completed,
};

static usbh_composite_host_t usbh_composite_host;

static int usbh_composite_deinit_uac_class(void)
{
	usbh_composite_host_t *chost = &usbh_composite_host;

	if (chost->uac) {
		usbh_composite_uac_deinit();
		chost->uac = NULL;
	}

	return HAL_OK;
}

static int usbh_composite_deinit_hid_class(void)
{
	usbh_composite_host_t *chost = &usbh_composite_host;

	if (chost->hid) {
		usbh_composite_hid_deinit();
		chost->hid = NULL;
	}

	return HAL_OK;
}

/**
  * @brief  Attach callback.
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_composite_hid_uac_attach(usb_host_t *host)
{
	int ret;
	usbh_composite_host_t *chost = &usbh_composite_host;

	chost->host = host;

	if ((chost->hid != NULL) && (chost->hid->attach != NULL)) {
		ret = chost->hid->attach(host);
		if (ret != HAL_OK) {
			usbh_composite_deinit_hid_class();
			RTK_LOGS(TAG, RTK_LOG_WARN, "Can not support hid\n");
		}
	}

	if ((chost->uac != NULL) && (chost->uac->attach != NULL)) {
		ret = chost->uac->attach(host);
		if (ret != HAL_OK) {
			usbh_composite_deinit_uac_class();
			RTK_LOGS(TAG, RTK_LOG_WARN, "Can not support uac\n");
		}
	}

	return HAL_OK;
}

/**
  * @brief  Detach callback.
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_composite_hid_uac_detach(usb_host_t *host)
{
	usbh_composite_host_t *chost = &usbh_composite_host;

	if ((chost->hid != NULL) && (chost->hid->detach != NULL)) {
		chost->hid->detach(host);
	}

	if ((chost->uac != NULL) && (chost->uac->detach != NULL)) {
		chost->uac->detach(host);
	}

	return HAL_OK;
}

/**
  * @brief  Standard control requests handling callback
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_composite_hid_uac_setup(usb_host_t *host)
{
	usbh_composite_host_t *chost = &usbh_composite_host;
	int ret = HAL_OK;

	if (chost->uac != NULL) {
		ret = usbh_composite_uac_get_volume_info(host);
		if (ret != HAL_OK) {
			return ret;
		}
	}

	if (chost->hid != NULL) {  /* maybe not support hid while do attach check */
		ret = usbh_composite_hid_handle_report_desc(host);
		if (ret != HAL_OK) {
			return ret;
		}
	}

	if ((chost->hid != NULL) && (chost->hid->setup != NULL)) {
		chost->hid->setup(host);
	}

	if ((chost->uac != NULL) && (chost->uac->setup != NULL)) {
		chost->uac->setup(host);
	}

	return HAL_OK;
}

/**
  * @brief  SOF callback for class-specific timing process.
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param[in] host: USB host handle.
  * @return 0 on success, non-zero on failure.
  */
static int usbh_composite_hid_uac_sof(usb_host_t *host)
{
	usbh_composite_host_t *chost = &usbh_composite_host;

	if ((chost->hid != NULL) && (chost->hid->sof != NULL)) {
		chost->hid->sof(host);
	}

	if ((chost->uac != NULL) && (chost->uac->sof != NULL)) {
		chost->uac->sof(host);
	}

	return HAL_OK;
}

/**
  * @brief  Transfer completion callback.
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param[in] host: USB host handle.
  * @param[in] pipe_num: Pipe number of the completed transfer.
  * @return 0 on success, non-zero on failure.
  */
static int usbh_composite_hid_uac_completed(usb_host_t *host, u8 pipe_num)
{
	usbh_composite_host_t *chost = &usbh_composite_host;
	int ret = HAL_BUSY;

	if ((chost->hid != NULL) && (chost->hid->completed != NULL)) {
		ret = chost->hid->completed(host, pipe_num);
	}

	if ((ret != HAL_OK) && (chost->uac != NULL) && (chost->uac->completed != NULL)) {
		ret = chost->uac->completed(host, pipe_num);
	}

	return ret;
}

/**
  * @brief  State machine handling callback
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_composite_hid_uac_process(usb_host_t *host, usbh_event_t *event)
{
	usbh_composite_host_t *chost = &usbh_composite_host;
	int ret = HAL_BUSY;

	/*
	 * If the process has handled the msg, it returns HAL_OK, else returns HAL_BUSY.
	 */
	if ((chost->hid != NULL) && (chost->hid->process != NULL)) {
		ret = chost->hid->process(host, event);
	}

	if ((ret != HAL_OK) && (chost->uac != NULL) && (chost->uac->process != NULL)) {
		ret = chost->uac->process(host, event);
	}

	return ret;
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Init composite class
  * @param  cb: User callback
  * @retval Status
  */
int usbh_composite_init(usbh_composite_hid_usr_cb_t *hid_cb, usbh_composite_uac_usr_cb_t *uac_cb)
{
	int ret;
	usbh_composite_host_t *chost = &usbh_composite_host;

	if ((hid_cb == NULL) || (uac_cb == NULL)) {
		ret = HAL_ERR_PARA;
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid user CB\n");
		return ret;
	}

	ret = usbh_composite_hid_init(chost, hid_cb);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init HID itf fail: %d\n", ret);
		return ret;
	}
	chost->hid = (usbh_class_driver_t *)&usbh_composite_hid_driver;

	ret = usbh_composite_uac_init(chost, uac_cb);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init UAC itf fail: %d\n", ret);
		usbh_composite_hid_deinit();
		return ret;
	}
	chost->uac = (usbh_class_driver_t *)&usbh_composite_uac_driver;
	usbh_register_class(&usbh_composite_driver);

	return HAL_OK;
}

/**
  * @brief  Deinit composite class
  * @retval Status
  */
int usbh_composite_deinit(void)
{
	usbh_composite_host_t *chost = &usbh_composite_host;

	usbh_unregister_class(&usbh_composite_driver);

	usbh_composite_deinit_uac_class();
	usbh_composite_deinit_hid_class();

	chost->cb = NULL;

	return HAL_OK;
}
