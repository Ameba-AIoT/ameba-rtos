/*
 * Copyright (c) 2025 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/
#include "usbh_composite_cdc_acm_ecm.h"

/* Private defines -----------------------------------------------------------*/
#define USBH_COMPOSITE_MAIN_THREAD_PRIORITY         5
#define USBH_COMPOSITE_ISR_PRIORITY                 INT_PRI_MIDDLE

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static int usbh_composite_acm_ecm_attach(usb_host_t *host);
static int usbh_composite_acm_ecm_detach(usb_host_t *host);
static int usbh_composite_acm_ecm_process(usb_host_t *host, usbh_event_t *event);
static int usbh_composite_acm_ecm_setup(usb_host_t *host);
static int usbh_composite_acm_ecm_sof(usb_host_t *host);
static int usbh_composite_acm_ecm_completed(usb_host_t *host, u8 pipe_num);

static int usbh_composite_acm_ecm_user_process(usb_host_t *host, u8 id);
static int usbh_composite_acm_ecm_user_device_check(usb_host_t *host, u8 cfg_max);

/* Private variables ---------------------------------------------------------*/
static const char *const TAG = "COMP";

static usbh_config_t usbh_composite_acm_ecm_cfg = {
	.speed = USB_SPEED_HIGH,
	.isr_priority = USBH_COMPOSITE_ISR_PRIORITY,
	.main_task_stack_size = 1792U,
	.main_task_priority = USBH_COMPOSITE_MAIN_THREAD_PRIORITY,
	.tick_source = USBH_SOF_TICK,
#if defined (CONFIG_AMEBAGREEN2)
	/*FIFO total depth is 1024, reserve 12 for DMA addr*/
	.rx_fifo_depth = 500,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#elif defined (CONFIG_AMEBAL2)
	/*FIFO total depth is 1024 DWORD, reserve 11 DWORD for DMA addr*/
	.rx_fifo_depth = 501,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#endif
};

static usbh_user_cb_t usbh_composite_acm_ecm_usr_cb = {
	.process = usbh_composite_acm_ecm_user_process,
	.validate = usbh_composite_acm_ecm_user_device_check,
};

static const usbh_dev_id_t usbh_composite_acm_ecm_devs[] = {
	{
		.mMatchFlags = USBH_DEV_ID_MATCH_ITF_CLASS,
		.bInterfaceClass = USB_CDC_CLASS_CODE,
	},
	{
	},
};

/* USB Class Driver */
static usbh_class_driver_t usbh_composite_driver = {
	.id_table = usbh_composite_acm_ecm_devs,
	.attach = usbh_composite_acm_ecm_attach,
	.detach = usbh_composite_acm_ecm_detach,
	.setup = usbh_composite_acm_ecm_setup,
	.process = usbh_composite_acm_ecm_process,
	.sof = usbh_composite_acm_ecm_sof,
	.completed = usbh_composite_acm_ecm_completed,
};

static usbh_composite_host_t usbh_composite_host;

static int usbh_composite_deinit_ecm_class(void)
{
	usbh_composite_host_t *chost = &usbh_composite_host;

	if (chost->ecm) {
		usbh_composite_cdc_ecm_deinit();
		chost->ecm = NULL;
	}

	return HAL_OK;
}

static int usbh_composite_deinit_acm_class(void)
{
	usbh_composite_host_t *chost = &usbh_composite_host;

	if (chost->acm) {
		usbh_composite_cdc_acm_deinit();
		chost->acm = NULL;
	}

	return HAL_OK;
}

static int usbh_composite_acm_ecm_user_process(usb_host_t *host, u8 id)
{
	UNUSED(host);
	switch (id) {
	case USBH_MSG_USER_SET_CONFIG:
		usbh_composite_cdc_ecm_choose_config(host);
		break;
	case USBH_MSG_DISCONNECTED:
		/* usbh_cdc_ecm_host_user.cdc_ecm_is_ready = 0; */
		break;

	case USBH_MSG_CONNECTED:
		break;

	default:
		break;
	}

	return HAL_OK;
}

static int usbh_composite_acm_ecm_user_device_check(usb_host_t *host, u8 cfg_max)
{
	return usbh_composite_cdc_ecm_check_config_desc(host, cfg_max);
}

/**
  * @brief  Attach callback.
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_composite_acm_ecm_attach(usb_host_t *host)
{
	int ret;
	usbh_composite_host_t *chost = &usbh_composite_host;

	chost->host = host;

	if ((chost->acm != NULL) && (chost->acm->attach != NULL)) {
		ret = chost->acm->attach(host);
		if (ret != HAL_OK) {
			usbh_composite_deinit_acm_class();
			RTK_LOGS(TAG, RTK_LOG_WARN, "No acm\n");
		}
	}

	if ((chost->ecm != NULL) && (chost->ecm->attach != NULL)) {
		ret = chost->ecm->attach(host);
		if (ret != HAL_OK) {
			usbh_composite_deinit_ecm_class();
			RTK_LOGS(TAG, RTK_LOG_WARN, "No ecm\n");
		}
	}

	return HAL_OK;
}

/**
  * @brief  Detach callback.
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_composite_acm_ecm_detach(usb_host_t *host)
{
	usbh_composite_host_t *chost = &usbh_composite_host;
	UNUSED(host);

	if ((chost->acm != NULL) && (chost->acm->detach != NULL)) {
		chost->acm->detach(host);
	}

	if ((chost->ecm != NULL) && (chost->ecm->detach != NULL)) {
		chost->ecm->detach(host);
	}

	if ((chost->cb != NULL) && (chost->cb->detach != NULL)) {
		chost->cb->detach();
	}

	return HAL_OK;
}

/**
  * @brief  Standard control requests handling callback
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_composite_acm_ecm_setup(usb_host_t *host)
{
	usbh_composite_host_t *chost = &usbh_composite_host;
	int status = HAL_ERR_UNKNOWN;

	if (chost->acm != NULL) {
		status = usbh_composite_cdc_acm_ctrl_setting(host);
		if (status != HAL_OK) {
			return status;
		}
	}

	if (chost->ecm != NULL) {
		status = usbh_composite_cdc_ecm_ctrl_setting(host);
		if (status != HAL_OK) {
			return status;
		}
	}

	if ((chost->acm != NULL) && (chost->acm->setup != NULL)) {
		chost->acm->setup(host);
	}

	if ((chost->ecm != NULL) && (chost->ecm->setup != NULL)) {
		chost->ecm->setup(host);
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
static int usbh_composite_acm_ecm_sof(usb_host_t *host)
{
	usbh_composite_host_t *chost = &usbh_composite_host;

	if ((chost->acm != NULL) && (chost->acm->sof != NULL)) {
		chost->acm->sof(host);
	}

	if ((chost->ecm != NULL) && (chost->ecm->sof != NULL)) {
		chost->ecm->sof(host);
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
static int usbh_composite_acm_ecm_completed(usb_host_t *host, u8 pipe_num)
{
	usbh_composite_host_t *chost = &usbh_composite_host;
	int ret = HAL_BUSY;

	if ((chost->acm != NULL) && (chost->acm->completed != NULL)) {
		ret = chost->acm->completed(host, pipe_num);
	}

	if ((ret != HAL_OK) && (chost->ecm != NULL) && (chost->ecm->completed != NULL)) {
		ret = chost->ecm->completed(host, pipe_num);
	}

	return ret;
}

/**
  * @brief  State machine handling callback
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_composite_acm_ecm_process(usb_host_t *host, usbh_event_t *event)
{
	usbh_composite_host_t *chost = &usbh_composite_host;
	int ret = HAL_BUSY;

	/*
	 * If the process has handled the msg, it returns HAL_OK, else returns HAL_BUSY.
	 */
	if ((chost->acm != NULL) && (chost->acm->process != NULL)) {
		ret = chost->acm->process(host, event);
	}

	if ((ret != HAL_OK) && (chost->ecm != NULL) && (chost->ecm->process != NULL)) {
		ret = chost->ecm->process(host, event);
	}

	return ret;
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Init ecm class
  * @param  cb: User callback
  * @retval Status
  */
int usbh_composite_acm_ecm_init(usbh_composite_cdc_acm_usr_cb_t *acm_cb, usbh_composite_cdc_ecm_usr_cb_t *ecm_cb, usbh_composite_cb_t *cb)
{
	int ret;
	usbh_composite_host_t *chost = &usbh_composite_host;
	usb_os_memset(chost, 0x00, sizeof(usbh_composite_host_t));

	if ((acm_cb == NULL) || (ecm_cb == NULL)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "NULL CB\n");
		return HAL_ERR_PARA;
	}

	ret = usbh_init(&usbh_composite_acm_ecm_cfg, &usbh_composite_acm_ecm_usr_cb);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init fail\n");
		return HAL_ERR_UNKNOWN;
	}

	ret = usbh_composite_cdc_acm_init(chost, acm_cb);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Acm init fail\n");
		usbh_deinit();
		return ret;
	}
	chost->acm = (usbh_class_driver_t *)&usbh_composite_cdc_acm_driver;

	ret = usbh_composite_cdc_ecm_init(chost, ecm_cb);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Ecm init fail\n");
		usbh_composite_cdc_acm_deinit();
		usbh_deinit();
		return ret;
	}
	chost->ecm = (usbh_class_driver_t *)&usbh_composite_cdc_ecm_driver;

	chost->cb = cb;

	usbh_register_class(&usbh_composite_driver);

	return HAL_OK;
}

/**
  * @brief  Deinit ecm class
  * @retval Status
  */
int usbh_composite_acm_ecm_deinit(void)
{
	usbh_composite_host_t *chost = &usbh_composite_host;

	usbh_unregister_class(&usbh_composite_driver);

	usbh_composite_deinit_ecm_class();
	usbh_composite_deinit_acm_class();

	usbh_deinit();

	chost->cb = NULL;

	return HAL_OK;
}

u16 usbh_composite_acm_ecm_get_device_vid(void)
{
	usbh_composite_host_t *chost = &usbh_composite_host;
	usb_host_t *host = chost->host;

	if ((host != NULL) && (host->connect_state >= USBH_STATE_SETUP)) {
		return host->dev_desc->idVendor;
	}

	return 0;
}

u16 usbh_composite_acm_ecm_get_device_pid(void)
{
	usbh_composite_host_t *chost = &usbh_composite_host;
	usb_host_t *host = chost->host;

	if ((host != NULL) && (host->connect_state >= USBH_STATE_SETUP)) {
		return host->dev_desc->idProduct;
	}

	return 0;
}

/**
  * @brief  usb enum success
  * @retval status
  */
u8 usbh_composite_acm_ecm_is_ready(void)
{
	usbh_composite_host_t *chost = &usbh_composite_host;
	usb_host_t *host = chost->host;

	if ((host != NULL) && (host->connect_state >= USBH_STATE_SETUP)) {
		return HAL_OK;
	}

	return HAL_BUSY;
}
