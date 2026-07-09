/*
 * Copyright (c) 2025 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbh_composite_cdc_acm.h"
#include "usbh.h"

/* Private defines -----------------------------------------------------------*/
#define USBH_CDC_ACM_DEBUG                     0
#define USBH_BULK_OUT_MAX_TIMEOUT_TICK         8000 //sof
#define USBH_BULK_IN_MAX_TIMEOUT_TICK          100  //sof
#define USBH_INTR_MAX_TIMEOUT_TICK             1000
#define USBH_CDC_ACM_NOTIFY_BUF_SIZE           256
#define USBH_CDC_ACM_LOOPBACK_BUF_SIZE         1024

/* CDC ACM state */
typedef enum {
	CDC_ACM_STATE_IDLE = 0U,
	CDC_ACM_STATE_TRANSFER,
	CDC_ACM_STATE_ERROR,
} usbh_composite_cdc_acm_state_t;

typedef enum {
	CDC_ACM_STATE_CTRL_IDLE  = 0U,
	CDC_ACM_STATE_SET_LINE_CODING,
	CDC_ACM_STATE_GET_LINE_CODING,
	CDC_ACM_STATE_CTRL_ALT_SETTING,
	CDC_ACM_STATE_CTRL_SETTING_MAX,
} usbh_composite_cdc_acm_ctrl_state_t;

/* Private types -------------------------------------------------------------*/
static u8 usbh_composite_cdc_acm_tx_buf[USBH_CDC_ACM_LOOPBACK_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));
static u8 usbh_composite_cdc_acm_notify_rx_buf[USBH_CDC_ACM_NOTIFY_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static int usbh_composite_cdc_acm_attach(usb_host_t *host);
static int usbh_composite_cdc_acm_detach(usb_host_t *host);
static int usbh_composite_cdc_acm_process(usb_host_t *host, usbh_event_t *event);
static int usbh_composite_cdc_acm_sof(usb_host_t *host);
static int usbh_composite_cdc_acm_setup(usb_host_t *host);
static int usbh_composite_cdc_acm_get_line_coding(usb_host_t *host, usb_cdc_line_coding_t *line_coding);
static int usbh_composite_cdc_acm_set_line_coding(usb_host_t *host, usb_cdc_line_coding_t *line_coding);
static int usbh_composite_cdc_acm_process_tx(usb_host_t *host);
static int usbh_composite_cdc_acm_process_rx(usb_host_t *host);
static int usbh_composite_cdc_acm_process_intr_rx(usb_host_t *host);

/* Private variables ---------------------------------------------------------*/
static const char *const TAG = "ACM";

/* USB Class Driver */
const usbh_class_driver_t usbh_composite_cdc_acm_driver = {
	.attach = usbh_composite_cdc_acm_attach,
	.detach = usbh_composite_cdc_acm_detach,
	.setup = usbh_composite_cdc_acm_setup,
	.process = usbh_composite_cdc_acm_process,
	.sof = usbh_composite_cdc_acm_sof,
};

static usbh_composite_cdc_acm_host_t usbh_composite_cdc_acm_host;

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Dump endpoint descriptor information for debugging purposes.
 *
 *         Logs the pipe number and endpoint interval for the INTR input,
 *         BULK output, and BULK input pipes of the CDC ACM device.
 *         This function is a no-op when USBH_CDC_ACM_DEBUG is disabled.
 *
 * @note   Debug-only; guarded by @ref USBH_CDC_ACM_DEBUG.
 */
static void usbh_composite_cdc_acm_dump_desc(void)
{
#if USBH_CDC_ACM_DEBUG
	usbh_composite_cdc_acm_host_t *cdc = &usbh_composite_cdc_acm_host;
	usbh_pipe_t *pipe_info;

	RTK_LOGS(TAG, RTK_LOG_INFO, "--------------------ACM Dump Descriptor Start------------------\n");

	// Dump CDC control pipe info
	pipe_info = &(cdc->intr_in);
	RTK_LOGS(TAG, RTK_LOG_INFO, "INTR pipe(%d)interval(%d)\n", pipe_info->pipe_num, pipe_info->ep_interval);

	// Dump ACM data pipe info
	pipe_info = &(cdc->bulk_out);
	RTK_LOGS(TAG, RTK_LOG_INFO, "BULK out:pipe(%d)interval(%d)\n", pipe_info->pipe_num, pipe_info->ep_interval);

	pipe_info = &(cdc->bulk_in);
	RTK_LOGS(TAG, RTK_LOG_INFO, "BULK in:pipe(%d)interval(%d)\n", pipe_info->pipe_num, pipe_info->ep_interval);

	RTK_LOGS(TAG, RTK_LOG_INFO, "---------------------Dump End-----------------------------\n");
#endif
}

/**
  * @brief  check USB device enum status
  * @retval return HAL_OK if enum success, else return HAL_BUSY
  */
static int usbh_composite_cdc_acm_check_enum_status(void)
{
	usbh_composite_cdc_acm_host_t *cdc = &usbh_composite_cdc_acm_host;

	if ((cdc->driver != NULL) && (cdc->driver->host != NULL) && (cdc->driver->host->connect_state >= USBH_STATE_SETUP)) {
		return HAL_OK;
	}

	return HAL_BUSY;
}


/**
  * @brief  Start to receive data
  * @param  buf: Data buffer
  * @param  len: Data length
  * @retval Status
  */
static int usbh_composite_cdc_acm_receive(u8 *buf, u32 len)
{
	int ret = HAL_BUSY;
	usbh_composite_cdc_acm_host_t *cdc = &usbh_composite_cdc_acm_host;
	usb_host_t *host = (cdc->driver != NULL) ? cdc->driver->host : NULL;
	usbh_pipe_t *pipe = &cdc->bulk_in;

	if (usbh_composite_cdc_acm_check_enum_status() != HAL_OK) {
		return ret;
	}

	if ((cdc->state == CDC_ACM_STATE_IDLE) || (cdc->state == CDC_ACM_STATE_TRANSFER)) {
		if (pipe->xfer_state == USBH_EP_XFER_IDLE || pipe->xfer_state == USBH_EP_XFER_ERROR) {
			pipe->xfer_buf = buf;
			pipe->xfer_len = len;

			if ((pipe->xfer_len > 0) && (pipe->xfer_len % pipe->ep_mps) == 0) { //ZLP
				pipe->trx_zlp = 1;
			} else {
				pipe->trx_zlp = 0;
			}

			cdc->state = CDC_ACM_STATE_TRANSFER;
			pipe->xfer_state = USBH_EP_XFER_START;
			usbh_notify_class_state_change(host, pipe->pipe_num);
			ret = HAL_OK;
		}
	}

	return ret;
}

/**
  * @brief  Start to receive notify data
  * @param  buf: Data buffer
  * @param  len: Data length
  * @retval Status
  */
static int usbh_composite_cdc_acm_notify_receive(u8 *buf, u32 len)
{
	int ret = HAL_BUSY;
	usbh_composite_cdc_acm_host_t *cdc = &usbh_composite_cdc_acm_host;
	usb_host_t *host = (cdc->driver != NULL) ? cdc->driver->host : NULL;
	usbh_pipe_t *pipe = &cdc->intr_in;

	if (usbh_composite_cdc_acm_check_enum_status() != HAL_OK) {
		return ret;
	}

	if ((cdc->state == CDC_ACM_STATE_IDLE) || (cdc->state == CDC_ACM_STATE_TRANSFER)) {
		pipe->xfer_buf = buf;
		pipe->xfer_len = pipe->ep_mps;

		/* The user buf len < MPS, update the xfer length */
		if (len < pipe->ep_mps) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Pls inc inbuf len %d-%d\n", len, pipe->ep_mps);
			pipe->xfer_len = len;
		}

		if ((pipe->xfer_len > 0) && (pipe->xfer_len % pipe->ep_mps) == 0) { //ZLP
			pipe->trx_zlp = 1;
		} else {
			pipe->trx_zlp = 0;
		}

		cdc->state = CDC_ACM_STATE_TRANSFER;
		pipe->xfer_state = USBH_EP_XFER_START;
		usbh_notify_class_state_change(host, pipe->pipe_num);
		ret = HAL_OK;
	}

	return ret;
}

/**
  * @brief  Open BULK in/out pipes from a CDC data interface descriptor.
  */
static void usbh_composite_cdc_acm_open_data_eps(usb_host_t *host, usbh_composite_cdc_acm_host_t *cdc, usbh_itf_desc_t *itf_desc)
{
	usbh_pipe_t *bulk_out = &cdc->bulk_out;
	usbh_pipe_t *bulk_in = &cdc->bulk_in;

	for (u8 i = 0; i < itf_desc->bNumEndpoints; i++) {
		usbh_ep_desc_t *ep_desc = &itf_desc->ep_desc_array[i];
		if ((ep_desc->bEndpointAddress & USB_REQ_DIR_MASK) == USB_D2H) {
			usbh_open_pipe(host, bulk_in, ep_desc);
			bulk_in->max_timeout_tick = USBH_BULK_IN_MAX_TIMEOUT_TICK;
		} else {
			usbh_open_pipe(host, bulk_out, ep_desc);
			bulk_out->max_timeout_tick = USBH_BULK_OUT_MAX_TIMEOUT_TICK;
		}
	}
}

#if USBH_COMPOSITE_4G_DONGLE_SUPPORT
/**
  * @brief  Look up the dongle parameter entry that matches the device VID/PID.
  */
static const usbh_composite_cdc_acm_param_t *usbh_composite_cdc_acm_find_param(const usbh_composite_cdc_acm_param_t *list, u16 vid, u16 pid)
{
	while ((list != NULL) && (list->vid != 0)) {
		if ((list->vid == vid) && (list->pid == pid)) {
			return list;
		}
		list++;
	}
	return NULL;
}

/**
  * @brief  Find an interface descriptor by interface number and alternate setting,
  *         walking the same-class chain returned by usbh_get_interface_descriptor.
  *
  *         All alternate settings of one interface number share the same
  *         bInterfaceNumber but differ in bAlternateSetting, so the alt index
  *         must be iterated within each itf_data rather than checked only on
  *         itf_desc_array[0].
  */
static usbh_itf_desc_t *usbh_composite_cdc_acm_find_itf(usbh_itf_data_t *itf_data, u8 itf_num, u8 alt_setting)
{
	usbh_itf_desc_t *itf_desc;
	u8 alt_idx;

	while (itf_data != NULL) {
		if ((itf_data->itf_desc_array != NULL) && (itf_data->alt_setting_cnt > 0) &&
			(itf_data->itf_desc_array[0].bInterfaceNumber == itf_num)) {
			for (alt_idx = 0; alt_idx < itf_data->alt_setting_cnt; alt_idx++) {
				itf_desc = &itf_data->itf_desc_array[alt_idx];
				if (itf_desc->bAlternateSetting == alt_setting) {
					return itf_desc;
				}
			}
		}
		itf_data = itf_data->next;
	}
	return NULL;
}

/**
  * @brief  Open all endpoints (BULK in/out + INTR in) carried by a Quectel-style
  *         vendor-specific interface.
  */
static void usbh_composite_cdc_acm_open_quectel_eps(usb_host_t *host, usbh_composite_cdc_acm_host_t *cdc, usbh_itf_desc_t *itf_desc)
{
	usbh_pipe_t *bulk_out = &cdc->bulk_out;
	usbh_pipe_t *bulk_in = &cdc->bulk_in;
	usbh_pipe_t *intr_in = &cdc->intr_in;

	for (u8 i = 0; i < itf_desc->bNumEndpoints; i++) {
		usbh_ep_desc_t *ep_desc = &itf_desc->ep_desc_array[i];

		if (ep_desc->bmAttributes == USB_CH_EP_TYPE_BULK) {
			if (USB_EP_IS_IN(ep_desc->bEndpointAddress)) {
				usbh_open_pipe(host, bulk_in, ep_desc);
				bulk_in->max_timeout_tick = USBH_BULK_IN_MAX_TIMEOUT_TICK;
			} else {
				usbh_open_pipe(host, bulk_out, ep_desc);
				bulk_out->max_timeout_tick = USBH_BULK_OUT_MAX_TIMEOUT_TICK;
			}
		} else if (ep_desc->bmAttributes == USB_CH_EP_TYPE_INTR) {
			usbh_open_pipe(host, intr_in, ep_desc);
			intr_in->max_timeout_tick = USBH_INTR_MAX_TIMEOUT_TICK;
		} else {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Unknown xfer type(%d)\n", ep_desc->bmAttributes);
		}
	}
}

/**
  * @brief  Attach handler for the standard CDC-ACM topology:
  *         comm interface carries the INTR endpoint, the next data interface
  *         carries BULK in/out.
  */
static int usbh_composite_cdc_acm_attach_acm(usb_host_t *host, usbh_composite_cdc_acm_host_t *cdc)
{
	usbh_pipe_t *intr_in = &cdc->intr_in;
	usbh_itf_data_t *itf_data;
	usbh_itf_desc_t *itf_desc;
	usbh_ep_desc_t *ep_desc;
	usbh_dev_id_t dev_id_info = {0,};
	u8 comm_itf_num = cdc->param_item->at_line_idx;

	/* Communication interface (one INTR endpoint) */
	dev_id_info.bInterfaceClass = USB_CDC_COMM_INTERFACE_CLASS_CODE;
	dev_id_info.bInterfaceSubClass = USB_CDC_SUBCLASS_ACM;
	dev_id_info.bInterfaceProtocol = USB_CDC_CTRL_PROTOCOL_COMMON_AT_COMMAND;
	dev_id_info.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO;

	itf_data = usbh_get_interface_descriptor(host, &dev_id_info);
	if (itf_data == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Get comm itf fail\n");
		return HAL_ERR_UNKNOWN;
	}

	itf_desc = usbh_composite_cdc_acm_find_itf(itf_data, comm_itf_num, 0);
	if (itf_desc == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "No comm itf %d\n", comm_itf_num);
		return HAL_ERR_UNKNOWN;
	}
	if (itf_desc->bNumEndpoints != 1) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "EP err\n");
		return HAL_ERR_UNKNOWN;
	}

	ep_desc = &itf_desc->ep_desc_array[0];
	if ((ep_desc->bEndpointAddress & USB_REQ_DIR_MASK) == USB_D2H) {
		usbh_open_pipe(host, intr_in, ep_desc);
		intr_in->max_timeout_tick = USBH_INTR_MAX_TIMEOUT_TICK;
	}

	/* Data interface (BULK in/out) lives at comm_itf_num + 1 */
	dev_id_info.bInterfaceClass = USB_CDC_DATA_INTERFACE_CLASS_CODE;
	dev_id_info.bInterfaceSubClass = USB_CDC_SUBCLASS_RESERVED;
	dev_id_info.bInterfaceProtocol = USB_CDC_CTRL_PROTOCOL_NO_CLASS_SPECIFIC;
	dev_id_info.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO;

	itf_data = usbh_get_interface_descriptor(host, &dev_id_info);
	if (itf_data == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Get data itf fail\n");
		return HAL_ERR_UNKNOWN;
	}

	itf_desc = usbh_composite_cdc_acm_find_itf(itf_data, comm_itf_num + 1, 1);
	if (itf_desc == NULL) {
		itf_desc = usbh_composite_cdc_acm_find_itf(itf_data, comm_itf_num + 1, 0);
	}
	if (itf_desc == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "No data itf with BULK EP\n");
		return HAL_ERR_UNKNOWN;
	}
	cdc->data_itf_num = itf_desc->bInterfaceNumber;
	cdc->data_itf_alt = itf_desc->bAlternateSetting;

	usbh_composite_cdc_acm_open_data_eps(host, cdc, itf_desc);
	return HAL_OK;
}

/**
  * @brief  Attach handler for Quectel dongles: a single vendor-specific
  *         interface carries INTR + BULK in + BULK out.
  */
static int usbh_composite_cdc_acm_attach_quectel(usb_host_t *host, usbh_composite_cdc_acm_host_t *cdc)
{
	usbh_itf_data_t *itf_data;
	usbh_itf_desc_t *itf_desc;
	usbh_dev_id_t dev_id_info = {0,};

	dev_id_info.bInterfaceClass = 0xFF;
	dev_id_info.bInterfaceSubClass = 0x00;
	dev_id_info.bInterfaceProtocol = 0x00;
	dev_id_info.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO;

	itf_data = usbh_get_interface_descriptor(host, &dev_id_info);
	if (itf_data == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Get comm itf fail\n");
		return HAL_ERR_UNKNOWN;
	}

	itf_desc = usbh_composite_cdc_acm_find_itf(itf_data, cdc->param_item->at_line_idx, 0);
	if (itf_desc == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "No itf %d\n", cdc->param_item->at_line_idx);
		return HAL_ERR_UNKNOWN;
	}
	if (itf_desc->bNumEndpoints != 3) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "EP count(%d) err\n", itf_desc->bNumEndpoints);
		return HAL_ERR_UNKNOWN;
	}
	cdc->data_itf_num = itf_desc->bInterfaceNumber;
	cdc->data_itf_alt = itf_desc->bAlternateSetting;

	usbh_composite_cdc_acm_open_quectel_eps(host, cdc, itf_desc);
	return HAL_OK;
}
#endif

/**
  * @brief  Attach callback.
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_composite_cdc_acm_attach(usb_host_t *host)
{
	usbh_composite_cdc_acm_host_t *cdc = &usbh_composite_cdc_acm_host;

#if USBH_COMPOSITE_4G_DONGLE_SUPPORT
	u16 vid = host->dev_desc->idVendor;
	u16 pid = host->dev_desc->idProduct;
	int ret;

	cdc->param_item = usbh_composite_cdc_acm_find_param(cdc->priv_param, vid, pid);
	if (cdc->param_item == NULL) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "Find cfg err for VID=0x%04x PID=0x%04x\n", vid, pid);
		return HAL_ERR_UNKNOWN;
	}

	if (vid == USBH_COMPOSITE_QUECTEL_DONGLE_VID) {
		ret = usbh_composite_cdc_acm_attach_quectel(host, cdc);
	} else {
		ret = usbh_composite_cdc_acm_attach_acm(host, cdc);
	}
	if (ret != HAL_OK) {
		return ret;
	}
#else
	usbh_pipe_t *intr_in = &cdc->intr_in;
	usbh_itf_data_t *itf_data;
	usbh_itf_desc_t *itf_desc;
	usbh_ep_desc_t *ep_desc;
	usbh_dev_id_t dev_id_info = {0,};
	u8 comm_if_num;

	dev_id_info.bInterfaceClass = USB_CDC_COMM_INTERFACE_CLASS_CODE;
	dev_id_info.bInterfaceSubClass = USB_CDC_SUBCLASS_ACM;
	dev_id_info.bInterfaceProtocol = USB_CDC_CTRL_PROTOCOL_COMMON_AT_COMMAND;
	dev_id_info.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO;

	itf_data = usbh_get_interface_descriptor(host, &dev_id_info);
	if (itf_data == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Get comm itf fail\n");
		return HAL_ERR_UNKNOWN;
	}

	itf_desc = itf_data->itf_desc_array;
	comm_if_num = itf_desc->bInterfaceNumber;

	/* The comm interface may legally carry 0 endpoints (no notify INTR EP) */
	if (itf_desc->bNumEndpoints >= 1) {
		ep_desc = &itf_desc->ep_desc_array[0];
		if ((ep_desc->bEndpointAddress & USB_REQ_DIR_MASK) == USB_D2H) {
			usbh_open_pipe(host, intr_in, ep_desc);
			intr_in->max_timeout_tick = USBH_INTR_MAX_TIMEOUT_TICK;
		}
	}

	dev_id_info.bInterfaceClass = USB_CDC_DATA_INTERFACE_CLASS_CODE;
	dev_id_info.bInterfaceSubClass = USB_CDC_SUBCLASS_RESERVED;
	dev_id_info.bInterfaceProtocol = USB_CDC_CTRL_PROTOCOL_NO_CLASS_SPECIFIC;
	dev_id_info.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO;

	itf_data = usbh_get_interface_descriptor(host, &dev_id_info);
	if (itf_data == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Get data itf fail\n");
		return HAL_ERR_UNKNOWN;
	}

	itf_desc = usbh_composite_cdc_acm_find_itf(itf_data, comm_if_num + 1, 1);
	if (itf_desc == NULL) {
		itf_desc = usbh_composite_cdc_acm_find_itf(itf_data, comm_if_num + 1, 0);
	}
	if (itf_desc == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "No data itf found(%d)\n", comm_if_num + 1);
		return HAL_ERR_UNKNOWN;
	}
	cdc->data_itf_num = itf_desc->bInterfaceNumber;
	cdc->data_itf_alt = itf_desc->bAlternateSetting;

	usbh_composite_cdc_acm_open_data_eps(host, cdc, itf_desc);
#endif

	usbh_composite_cdc_acm_dump_desc();
	cdc->state = CDC_ACM_STATE_IDLE;
	cdc->sub_status = CDC_ACM_STATE_CTRL_ALT_SETTING;

	if ((cdc->cb != NULL) && (cdc->cb->attach != NULL)) {
		cdc->cb->attach();
	}

	return HAL_OK;
}

/**
  * @brief  Detach callback.
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_composite_cdc_acm_detach(usb_host_t *host)
{
	usbh_composite_cdc_acm_host_t *cdc = &usbh_composite_cdc_acm_host;
	usbh_pipe_t *bulk_out = &cdc->bulk_out;
	usbh_pipe_t *bulk_in = &cdc->bulk_in;
	usbh_pipe_t *intr_in = &cdc->intr_in;

	if ((cdc->cb != NULL) && (cdc->cb->detach != NULL)) {
		cdc->cb->detach();
	}

	if (intr_in->pipe_num) {
		usbh_close_pipe(host, intr_in);
	}

	if (bulk_in->pipe_num) {
		usbh_close_pipe(host, bulk_in);
	}

	if (bulk_out->pipe_num) {
		usbh_close_pipe(host, bulk_out);
	}

	return HAL_OK;
}

/**
  * @brief  Standard control requests handling callback
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_composite_cdc_acm_setup(usb_host_t *host)
{
	usbh_composite_cdc_acm_host_t *cdc = &usbh_composite_cdc_acm_host;

	if ((cdc->cb != NULL) && (cdc->cb->setup != NULL)) {
		cdc->cb->setup();
	}

	cdc->state = CDC_ACM_STATE_TRANSFER;
	usbh_notify_class_state_change(host, 0);

	return HAL_OK;
}

/**
* @brief  State machine handling callback
* @param  host:Host handle
* @retval Status
*/
static int usbh_composite_cdc_acm_process(usb_host_t *host, usbh_event_t *event)
{
	int status = HAL_BUSY;
	u8 req_status = HAL_OK;
	usbh_composite_cdc_acm_host_t *cdc = &usbh_composite_cdc_acm_host;

	switch (cdc->state) {

	case CDC_ACM_STATE_IDLE:
		/* Idle: no event consumed, leave status = HAL_BUSY so a composite
		   peer class (e.g. ECM) still gets a chance to handle the event. */
		break;

	case CDC_ACM_STATE_TRANSFER:
		if (event) {
			/* Only claim the event (HAL_OK) when it targets one of our pipes,
			   otherwise keep HAL_BUSY to pass it on to the next class. */
			if (event->pipe_num == cdc->bulk_out.pipe_num) {
				usbh_composite_cdc_acm_process_tx(host);
				status = HAL_OK;
			} else if (event->pipe_num == cdc->bulk_in.pipe_num) {
				usbh_composite_cdc_acm_process_rx(host);
				status = HAL_OK;
			} else if (event->pipe_num == cdc->intr_in.pipe_num) {
				usbh_composite_cdc_acm_process_intr_rx(host);
				status = HAL_OK;
			}
		}
		break;

	case CDC_ACM_STATE_ERROR:
		req_status = usbh_ctrl_clear_feature(host, 0x00U);
		if (req_status == HAL_OK) {
			cdc->state = CDC_ACM_STATE_IDLE;
		}
		break;

	default:
		break;

	}

	return status;
}

/**
  * @brief  SOF callback for class-specific timing process.
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param[in] host: USB host handle.
  * @return 0 on success, non-zero on failure.
  */
static int usbh_composite_cdc_acm_sof(usb_host_t *host)
{
	usbh_composite_cdc_acm_host_t *cdc = &usbh_composite_cdc_acm_host;
	usbh_pipe_t *pipe = &(cdc->intr_in);

	/*
		If cur_frame - last_frame_num >= interval, trigger a transfer ASAP.
		Skip when the INTR pipe was never opened (pipe_num == 0, e.g. a comm
		interface with 0 endpoints) to avoid a divide-by-zero on ep_mps and
		dispatching to an invalid pipe.
	*/
	if ((pipe->pipe_num != 0) &&
		(usbh_get_elapsed_frame_cnt(host, pipe->frame_num) >= pipe->ep_interval)) {
		usbh_composite_cdc_acm_notify_receive(usbh_composite_cdc_acm_notify_rx_buf, USBH_CDC_ACM_NOTIFY_BUF_SIZE);
	}

	pipe = &(cdc->bulk_in);
	if ((pipe->xfer_state == USBH_EP_XFER_IDLE) || (pipe->xfer_state == USBH_EP_XFER_ERROR)) {
		usbh_composite_cdc_acm_receive(usbh_composite_cdc_acm_tx_buf, USBH_CDC_ACM_LOOPBACK_BUF_SIZE);
	}

	return HAL_OK;
}

/**
  * @brief  Get line coding
  * @param  host: Host handle
  * @param  line_coding: Line coding data pointer
  * @retval Status
  */
static int usbh_composite_cdc_acm_get_line_coding(usb_host_t *host, usb_cdc_line_coding_t *line_coding)
{
	usbh_setup_req_t setup;
	u16 windex = 0;
#if USBH_COMPOSITE_4G_DONGLE_SUPPORT
	usbh_composite_cdc_acm_host_t *cdc = &usbh_composite_cdc_acm_host;
	if (cdc->param_item) {
		windex = cdc->param_item->at_line_idx;
	}
#endif

	setup.req.bmRequestType = USB_D2H | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = USB_CDC_ACM_GET_LINE_CODING;
	setup.req.wValue = 0U;
	setup.req.wIndex = windex;
	setup.req.wLength = USB_CDC_ACM_LINE_CODING_SIZE;

	return usbh_ctrl_request(host, &setup, line_coding->d8);
}

/**
  * @brief  Set line coding
  * @param  host: Host handle
  * @param  line_coding: Line coding data pointer
  * @retval Status
  */
static int usbh_composite_cdc_acm_set_line_coding(usb_host_t *host, usb_cdc_line_coding_t *line_coding)
{
	usbh_setup_req_t setup;
	u16 windex = 0;
#if USBH_COMPOSITE_4G_DONGLE_SUPPORT
	usbh_composite_cdc_acm_host_t *cdc = &usbh_composite_cdc_acm_host;
	if (cdc->param_item) {
		windex = cdc->param_item->at_line_idx;
	}
#endif

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = USB_CDC_ACM_SET_LINE_CODING;
	setup.req.wValue = 0U;
	setup.req.wIndex = windex;
	setup.req.wLength = USB_CDC_ACM_LINE_CODING_SIZE;

	return usbh_ctrl_request(host, &setup, line_coding->d8);
}

/**
  * @brief  TX handling
  * @param  host: Host handle
  * @retval Status
*/
static int usbh_composite_cdc_acm_process_tx(usb_host_t *host)
{
	usbh_composite_cdc_acm_host_t *cdc = &usbh_composite_cdc_acm_host;
	usbh_pipe_t *bulk_out = &cdc->bulk_out;

	int status = usbh_transfer_process(host, bulk_out);

	if ((status == HAL_OK) && (bulk_out->xfer_state == USBH_EP_XFER_IDLE)) {
		if ((cdc->cb != NULL) && (cdc->cb->bulk_send != NULL)) {
			cdc->cb->bulk_send(status);
		}
	} else if (bulk_out->xfer_state == USBH_EP_XFER_START) {
		usbh_notify_class_state_change(host, bulk_out->pipe_num);
	} else if (bulk_out->xfer_state == USBH_EP_XFER_ERROR) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "BULK TX fail %d\n", usbh_get_urb_state(host, bulk_out));
		if ((cdc->cb != NULL) && (cdc->cb->bulk_send != NULL)) {
			cdc->cb->bulk_send(status);
		}
	}

	return HAL_OK;
}

/**
  * @brief  RX handling
  * @param  host: Host handle
  * @retval None
*/
static int usbh_composite_cdc_acm_process_rx(usb_host_t *host)
{
	usbh_composite_cdc_acm_host_t *cdc = &usbh_composite_cdc_acm_host;
	usbh_pipe_t *bulk_in = &cdc->bulk_in;

	int status = usbh_transfer_process(host, bulk_in);
	u32 rx_len = usbh_get_last_transfer_size(host, bulk_in);
	if ((status == HAL_OK) && (bulk_in->xfer_state == USBH_EP_XFER_IDLE)) {
		if ((cdc->cb != NULL) && (cdc->cb->bulk_received != NULL)) {
			cdc->cb->bulk_received(bulk_in->xfer_buf, rx_len);
		}
	} else if (bulk_in->xfer_state == USBH_EP_XFER_START) {
		usbh_notify_class_state_change(host, bulk_in->pipe_num);
	} else if (bulk_in->xfer_state == USBH_EP_XFER_ERROR) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "BULK RX fail %d\n", usbh_get_urb_state(host, bulk_in));
		if ((cdc->cb != NULL) && (cdc->cb->bulk_received != NULL)) {
			cdc->cb->bulk_received(bulk_in->xfer_buf, rx_len);
		}
	}

	return HAL_OK;
}

/**
  * @brief  Intr in function
  * @param  host: Host handle
  * @retval Status
*/
static int usbh_composite_cdc_acm_process_intr_rx(usb_host_t *host)
{
	usbh_composite_cdc_acm_host_t *cdc = &usbh_composite_cdc_acm_host;
	usbh_pipe_t *intr_in = &cdc->intr_in;

	int status = usbh_transfer_process(host, intr_in);
	u32 rx_len = usbh_get_last_transfer_size(host, intr_in);
	if ((status == HAL_OK) && (intr_in->xfer_state == USBH_EP_XFER_IDLE)) {
		if ((cdc->cb != NULL) && (cdc->cb->intr_received != NULL)) {
			cdc->cb->intr_received(intr_in->xfer_buf, rx_len);
		}
	} else if ((intr_in->xfer_state == USBH_EP_XFER_START)) {
		usbh_notify_class_state_change(host, intr_in->pipe_num);
	} else if (intr_in->xfer_state == USBH_EP_XFER_ERROR) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "INTR RX fail %d\n", usbh_get_urb_state(host, intr_in));
		if ((cdc->cb != NULL) && (cdc->cb->intr_received != NULL)) {
			cdc->cb->intr_received(intr_in->xfer_buf, rx_len);
		}
	}

	return HAL_OK;
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Init CDC ACM class
  * @param  cb: User callback
  * @retval Status
  */
int usbh_composite_cdc_acm_init(usbh_composite_host_t *driver, const usbh_composite_cdc_acm_usr_cb_t *cb)
{
	int ret = HAL_OK;
	usbh_composite_cdc_acm_host_t *cdc = &usbh_composite_cdc_acm_host;

	if (cb == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid user CB\n");
		return HAL_ERR_PARA;
	}

	usb_os_memset(cdc, 0x00, sizeof(usbh_composite_cdc_acm_host_t));

	cdc->priv_param = cb->priv;
	cdc->driver = driver;

#if USBH_COMPOSITE_4G_DONGLE_SUPPORT
	if (NULL == cdc->priv_param) {
		return HAL_ERR_PARA;
	}
#endif

	if (cb->init != NULL) {
		ret = cb->init();
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "User init err %d\n", ret);
			return ret;
		}
	}

	cdc->line_coding = (usb_cdc_line_coding_t *)usb_os_malloc(sizeof(usb_cdc_line_coding_t));
	if (cdc->line_coding == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Malloc line code fail\n");
		return HAL_ERR_MEM;
	}
	cdc->user_line_coding = (usb_cdc_line_coding_t *)usb_os_malloc(sizeof(usb_cdc_line_coding_t));
	if (cdc->user_line_coding == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Malloc user line code fail\n");
		usb_os_mfree(cdc->line_coding);
		cdc->line_coding = NULL;
		return HAL_ERR_MEM;
	}

	/* set acm line coding */
	usb_cdc_line_coding_t *ulc = cdc->user_line_coding;
	ulc->b.dwDteRate = 115200;
	ulc->b.bCharFormat = LINE_CODING_CHAR_FORMAT_1_STOP_BITS;
	ulc->b.bParityType = LINE_CODING_PARITY_NO;
	ulc->b.bDataBits = 8;

	cdc->cb = cb;

	return ret;
}

/**
  * @brief  Deinit CDC ACM class
  * @retval Status
  */
int usbh_composite_cdc_acm_deinit(void)
{
	int ret = HAL_OK;
	usbh_composite_cdc_acm_host_t *cdc = &usbh_composite_cdc_acm_host;
	usb_host_t *host = (cdc->driver != NULL) ? cdc->driver->host : NULL;

	if ((cdc->cb != NULL) && (cdc->cb->deinit != NULL)) {
		cdc->cb->deinit();
	}

	if ((host != NULL) && (host->connect_state == USBH_STATE_SETUP)) {
		cdc->state = CDC_ACM_STATE_IDLE;

		usbh_close_pipe(host, &cdc->intr_in);
		usbh_close_pipe(host, &cdc->bulk_in);
		usbh_close_pipe(host, &cdc->bulk_out);
	}

	if (cdc->line_coding != NULL) {
		usb_os_mfree(cdc->line_coding);
		cdc->line_coding = NULL;
	}
	if (cdc->user_line_coding != NULL) {
		usb_os_mfree(cdc->user_line_coding);
		cdc->user_line_coding = NULL;
	}

	return ret;
}

/**
 * @brief  Run the CDC ACM control request state machine.
 *
 *         This function drives the control sub-state machine for configuring
 *         the CDC ACM communication parameters. The sequence is:
 *
 *         State transitions:
 *          - CDC_ACM_STATE_CTRL_ALT_SETTING  -> CDC_ACM_STATE_SET_LINE_CODING (on success)
 *          - CDC_ACM_STATE_SET_LINE_CODING   -> CDC_ACM_STATE_GET_LINE_CODING (on success)
 *          - CDC_ACM_STATE_GET_LINE_CODING   -> CDC_ACM_STATE_CTRL_IDLE      (on success)
 *          - Any error returns to CDC_ACM_STATE_CTRL_IDLE.
 *
 * @param[in] host  Pointer to the USB host instance.
 *
 * @retval HAL_OK           The control setting sequence has completed.
 * @retval HAL_BUSY         A control request is still in progress.
 * @retval HAL_ERR_UNKNOWN  An unexpected error occurred.
 */
int usbh_composite_cdc_acm_ctrl_setting(usb_host_t *host)
{
	usbh_composite_cdc_acm_host_t *cdc = &usbh_composite_cdc_acm_host;
	u8 ret_state = HAL_ERR_UNKNOWN;
	u8 state = HAL_OK;

	switch (cdc->sub_status) {
	case CDC_ACM_STATE_CTRL_IDLE:
		ret_state = HAL_OK;
		break;

	case CDC_ACM_STATE_CTRL_ALT_SETTING:
		state = usbh_ctrl_set_interface(host, cdc->data_itf_num, cdc->data_itf_alt);
		if (state == HAL_OK) {
			cdc->sub_status = CDC_ACM_STATE_SET_LINE_CODING;
		} else if (state != HAL_BUSY) {
			cdc->sub_status = CDC_ACM_STATE_CTRL_IDLE;
		}
		usbh_notify_class_state_change(host, 0);
		break;

	case CDC_ACM_STATE_SET_LINE_CODING:
		state = usbh_composite_cdc_acm_set_line_coding(host, cdc->user_line_coding);
		if (state == HAL_OK) {
			cdc->sub_status = CDC_ACM_STATE_GET_LINE_CODING;
		} else if (state != HAL_BUSY) {
			cdc->sub_status = CDC_ACM_STATE_CTRL_IDLE;
		}
		usbh_notify_class_state_change(host, 0);
		break;

	case CDC_ACM_STATE_GET_LINE_CODING:
		state = usbh_composite_cdc_acm_get_line_coding(host, cdc->line_coding);
		if (state == HAL_OK) {
			cdc->sub_status = CDC_ACM_STATE_CTRL_IDLE;
			usb_cdc_line_coding_t *lc = cdc->line_coding;
			usb_cdc_line_coding_t *ulc = cdc->user_line_coding;
			if ((lc->b.bCharFormat == ulc->b.bCharFormat) &&
				(lc->b.bDataBits == ulc->b.bDataBits) &&
				(lc->b.bParityType == ulc->b.bParityType) &&
				(lc->b.dwDteRate == ulc->b.dwDteRate)) {
				if ((cdc->cb != NULL) && (cdc->cb->line_coding_changed != NULL)) {
					cdc->cb->line_coding_changed(cdc->line_coding);
				}
			}
			ret_state = HAL_OK;
		} else if (state != HAL_BUSY) {
			cdc->sub_status = CDC_ACM_STATE_CTRL_IDLE;
		}
		usbh_notify_class_state_change(host, 0);
		break;
	default:
		ret_state = HAL_OK;
		break;
	}

	return ret_state;
}

/**
  * @brief  Start to transfer data
  * @param  buf: Data buffer
  * @param  len: Data length
  * @retval Status
  */
int usbh_composite_cdc_acm_transmit(u8 *buf, u32 len)
{
	int ret = HAL_BUSY;
	usbh_composite_cdc_acm_host_t *cdc = &usbh_composite_cdc_acm_host;
	usbh_pipe_t *pipe = &cdc->bulk_out;
	usb_host_t *host;

	if (usbh_composite_cdc_acm_check_enum_status() != HAL_OK) {
		return ret;
	}

	host = cdc->driver->host;
	if ((cdc->state == CDC_ACM_STATE_IDLE) || (cdc->state == CDC_ACM_STATE_TRANSFER)) {
		if (pipe->xfer_state == USBH_EP_XFER_IDLE) {
			pipe->xfer_buf = buf;
			pipe->xfer_len = len;

			if ((pipe->xfer_len > 0) && (pipe->xfer_len % pipe->ep_mps) == 0) { //ZLP
				pipe->trx_zlp = 1;
			} else {
				pipe->trx_zlp = 0;
			}

			cdc->state = CDC_ACM_STATE_TRANSFER;
			pipe->xfer_state = USBH_EP_XFER_START;
			usbh_notify_class_state_change(host, pipe->pipe_num);
			ret = HAL_OK;
		}
	}

	return ret;
}
