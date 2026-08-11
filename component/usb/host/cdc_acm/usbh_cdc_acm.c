/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbh_cdc_acm.h"

/* Private defines -----------------------------------------------------------*/

#define USB_BULK_OUT_MAX_TIMEOUT_TICK          8000 //sof
#define USB_BULK_IN_MAX_TIMEOUT_TICK           100  //sof
#define USB_INTR_MAX_TIMEOUT_TICK              1000
#define USB_INTR_RETRY_MAX_CNT                 10
/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int usbh_cdc_acm_attach(usb_host_t *host);
static int usbh_cdc_acm_detach(usb_host_t *host);
static int usbh_cdc_acm_process(usb_host_t *host, usbh_event_t *event);
#if CONFIG_USBH_CDC_ACM_NOTIFY
static int usbh_cdc_acm_sof(usb_host_t *host);
static int usbh_cdc_acm_completed(usb_host_t *host, u8 pipe_num);
static void usbh_cdc_acm_process_intr_rx(usb_host_t *host);
#endif
static int usbh_cdc_acm_setup(usb_host_t *host);
static int usbh_cdc_acm_process_get_line_coding(usb_host_t *host, usb_cdc_line_coding_t *linecoding);
static int usbh_cdc_acm_process_set_line_coding(usb_host_t *host, usb_cdc_line_coding_t *linecoding);
static int usbh_cdc_acm_process_set_control_line_state(usb_host_t *host);
static int usbh_cdc_acm_process_send_break(usb_host_t *host);
static void usbh_cdc_acm_process_tx(usb_host_t *host);
static void usbh_cdc_acm_process_rx(usb_host_t *host);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "ACMH";

#if CONFIG_USBH_CDC_ACM_4G_DONGLE

#define USBH_CDC_ACM_BULK_OUT_MAX_TIMEOUT_TICK      USB_BULK_OUT_MAX_TIMEOUT_TICK
#define USBH_CDC_ACM_BULK_IN_MAX_TIMEOUT_TICK       USB_BULK_IN_MAX_TIMEOUT_TICK
#define USBH_CDC_ACM_INTR_MAX_TIMEOUT_TICK          USB_INTR_MAX_TIMEOUT_TICK

/* 4G-dongle control-setting sub-states (SET_INTERFACE + line coding). */
typedef enum {
	USBH_CDC_ACM_CTRL_IDLE = 0U,
	USBH_CDC_ACM_CTRL_ALT_SETTING,
	USBH_CDC_ACM_CTRL_SET_LINE_CODING,
	USBH_CDC_ACM_CTRL_GET_LINE_CODING,
} usbh_cdc_acm_ctrl_state_t;
#endif

/* USB CDC ACM device identification */
static const usbh_dev_id_t cdc_acm_devs[] = {
	{
		/* Match the CDC communication interface down to the ACM subclass so a
		 * composite device (e.g. ACM + ECM, both class 0x02) does not have its
		 * ECM comm interface wrongly claimed by this driver. The ACM data
		 * interface (class 0x0A) carries no distinguishing class, so it is not
		 * matched by any id_table; attach() reaches it by CDC topology and
		 * opens its endpoints directly. */
		.mMatchFlags = USBH_DEV_ID_MATCH_ITF_CLASS | USBH_DEV_ID_MATCH_ITF_SUBCLASS,
		.bInterfaceClass = USB_CDC_CLASS_CODE,
		.bInterfaceSubClass = USB_CDC_SUBCLASS_ACM,
	},
	{
	},
};

/* USB Host CDC ACM class driver */
static const usbh_class_driver_t usbh_cdc_acm_driver = {
	.id_table = cdc_acm_devs,
	.attach = usbh_cdc_acm_attach,
	.detach = usbh_cdc_acm_detach,
	.setup = usbh_cdc_acm_setup,
	.process = usbh_cdc_acm_process,
#if CONFIG_USBH_CDC_ACM_NOTIFY
	.sof = usbh_cdc_acm_sof,
	.completed = usbh_cdc_acm_completed,
#endif
};

static usbh_cdc_acm_host_t usbh_cdc_acm_host;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Dump the CDC ACM pipe (endpoint) information for debugging.
  *         Only active when USBH_CDC_ACM_DEBUG is enabled; compiles to an
  *         empty function otherwise so callers pay no runtime cost.
  * @retval void
  */
static void usbh_cdc_acm_dump_desc(void)
{
#if defined(USBH_CDC_ACM_DEBUG) && USBH_CDC_ACM_DEBUG
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usbh_pipe_t *pipe_info;

	RTK_LOGS(TAG, RTK_LOG_INFO, "--------------------ACM Dump Descriptor Start------------------\n");

	/* Dump CDC control(INTR) pipe info */
	pipe_info = &(cdc->intr_in);
	RTK_LOGS(TAG, RTK_LOG_INFO, "INTR pipe(%d)interval(%d)\n", pipe_info->pipe_num, pipe_info->ep_interval);

	/* Dump ACM data pipe info */
	pipe_info = &(cdc->bulk_out);
	RTK_LOGS(TAG, RTK_LOG_INFO, "BULK out:pipe(%d)interval(%d)\n", pipe_info->pipe_num, pipe_info->ep_interval);

	pipe_info = &(cdc->bulk_in);
	RTK_LOGS(TAG, RTK_LOG_INFO, "BULK in:pipe(%d)interval(%d)\n", pipe_info->pipe_num, pipe_info->ep_interval);

	RTK_LOGS(TAG, RTK_LOG_INFO, "---------------------Dump End-----------------------------\n");
#endif
}

#if CONFIG_USBH_CDC_ACM_4G_DONGLE

/**
  * @brief  Look up the 4G-dongle parameter entry that matches the device VID/PID.
  */
static usbh_cdc_acm_param_t *usbh_cdc_acm_find_param(usbh_cdc_acm_param_t *list, u16 vid, u16 pid)
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
  */
static usbh_itf_desc_t *usbh_cdc_acm_find_itf(usbh_itf_data_t *itf_data, u8 itf_num, u8 alt_setting)
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
  * @brief  Open BULK in/out pipes from a CDC data interface descriptor.
  */
static int usbh_cdc_acm_open_data_eps(usb_host_t *host, usbh_cdc_acm_host_t *cdc, usbh_itf_desc_t *itf_desc)
{
	usbh_pipe_t *bulk_out = &cdc->bulk_out;
	usbh_pipe_t *bulk_in = &cdc->bulk_in;
	int ret = HAL_OK;

	for (u8 i = 0; i < itf_desc->bNumEndpoints; i++) {
		usbh_ep_desc_t *ep_desc = &itf_desc->ep_desc_array[i];
		if ((ep_desc->bEndpointAddress & USB_REQ_DIR_MASK) == USB_D2H) {
			ret = usbh_open_pipe(host, bulk_in, ep_desc, &usbh_cdc_acm_driver);
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Open bulk in pipe fail\n");
				return ret;
			}
			bulk_in->max_timeout_tick = USBH_CDC_ACM_BULK_IN_MAX_TIMEOUT_TICK;
		} else {
			ret = usbh_open_pipe(host, bulk_out, ep_desc, &usbh_cdc_acm_driver);
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Open bulk out pipe fail\n");
				return ret;
			}
			bulk_out->max_timeout_tick = USBH_CDC_ACM_BULK_OUT_MAX_TIMEOUT_TICK;
		}
	}
	return ret;
}

/**
  * @brief  Open all endpoints (BULK in/out + INTR in) carried by a Quectel-style
  *         vendor-specific interface.
  */
static void usbh_cdc_acm_open_quectel_eps(usb_host_t *host, usbh_cdc_acm_host_t *cdc, usbh_itf_desc_t *itf_desc)
{
	usbh_pipe_t *bulk_out = &cdc->bulk_out;
	usbh_pipe_t *bulk_in = &cdc->bulk_in;
#if CONFIG_USBH_CDC_ACM_NOTIFY
	usbh_pipe_t *intr_in = &cdc->intr_in;
#endif

	for (u8 i = 0; i < itf_desc->bNumEndpoints; i++) {
		usbh_ep_desc_t *ep_desc = &itf_desc->ep_desc_array[i];

		if ((ep_desc->bmAttributes & USB_EP_XFER_TYPE_MASK) == USB_CH_EP_TYPE_BULK) {
			if (USB_EP_IS_IN(ep_desc->bEndpointAddress)) {
				usbh_open_pipe(host, bulk_in, ep_desc, &usbh_cdc_acm_driver);
				bulk_in->max_timeout_tick = USBH_CDC_ACM_BULK_IN_MAX_TIMEOUT_TICK;
			} else {
				usbh_open_pipe(host, bulk_out, ep_desc, &usbh_cdc_acm_driver);
				bulk_out->max_timeout_tick = USBH_CDC_ACM_BULK_OUT_MAX_TIMEOUT_TICK;
			}
		} else if ((ep_desc->bmAttributes & USB_EP_XFER_TYPE_MASK) == USB_CH_EP_TYPE_INTR) {
#if CONFIG_USBH_CDC_ACM_NOTIFY
			/* Only open INTR IN when notification support is enabled; otherwise
			 * the pipe would be opened but never polled or closed (leaked),
			 * because notify_process() / SOF / detach() paths are compiled out. */
			usbh_open_pipe(host, intr_in, ep_desc, &usbh_cdc_acm_driver);
			intr_in->max_timeout_tick = USBH_CDC_ACM_INTR_MAX_TIMEOUT_TICK;
#endif
		} else {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Unknown xfer type(%d)\n", ep_desc->bmAttributes);
		}
	}
}

/**
  * @brief  Attach handler for the standard CDC-ACM 4G topology: comm interface
  *         (param at_line_idx) carries INTR, data interface (comm+1) carries BULK.
  */
static int usbh_cdc_acm_attach_acm_4g(usb_host_t *host, usbh_cdc_acm_host_t *cdc)
{
#if CONFIG_USBH_CDC_ACM_NOTIFY
	usbh_pipe_t *intr_in = &cdc->intr_in;
	usbh_ep_desc_t *ep_desc;
#endif
	usbh_itf_data_t *itf_data;
	usbh_itf_desc_t *itf_desc;

	usbh_dev_id_t dev_id_info = {0,};
	u8 comm_itf_num = cdc->param_item->at_line_idx;

	dev_id_info.bInterfaceClass = USB_CDC_COMM_INTERFACE_CLASS_CODE;
	dev_id_info.bInterfaceSubClass = USB_CDC_SUBCLASS_ACM;
	dev_id_info.bInterfaceProtocol = USB_CDC_CTRL_PROTOCOL_COMMON_AT_COMMAND;
	dev_id_info.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO;

	itf_data = usbh_get_interface_descriptor(host, &dev_id_info);
	if (itf_data == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Get comm itf fail\n");
		return HAL_ERR_UNKNOWN;
	}

	itf_desc = usbh_cdc_acm_find_itf(itf_data, comm_itf_num, 0);
	if ((itf_desc == NULL) || (itf_desc->bNumEndpoints != 1)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "No comm itf %d\n", comm_itf_num);
		return HAL_ERR_UNKNOWN;
	}

	/* Communication interface: already claimed by the core's Phase 1 match
	 * (id_table matches class 0x02 + subclass ACM). */
#if CONFIG_USBH_CDC_ACM_NOTIFY
	/* Open INTR IN only when notification support is enabled; otherwise
	 * the pipe would be opened but never polled or closed (leaked),
	 * because notify_process() / SOF / detach() paths are compiled out. */
	ep_desc = &itf_desc->ep_desc_array[0];
	if ((ep_desc->bEndpointAddress & USB_REQ_DIR_MASK) == USB_D2H) {
		usbh_open_pipe(host, intr_in, ep_desc, &usbh_cdc_acm_driver);
		intr_in->max_timeout_tick = USBH_CDC_ACM_INTR_MAX_TIMEOUT_TICK;
	}
#endif

	dev_id_info.bInterfaceClass = USB_CDC_DATA_INTERFACE_CLASS_CODE;
	dev_id_info.bInterfaceSubClass = USB_CDC_SUBCLASS_RESERVED;
	dev_id_info.bInterfaceProtocol = USB_CDC_CTRL_PROTOCOL_NO_CLASS_SPECIFIC;
	dev_id_info.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO;

	itf_data = usbh_get_interface_descriptor(host, &dev_id_info);
	if (itf_data == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Get data itf fail\n");
		return HAL_ERR_UNKNOWN;
	}

	itf_desc = usbh_cdc_acm_find_itf(itf_data, comm_itf_num + 1, 1);
	if (itf_desc == NULL) {
		itf_desc = usbh_cdc_acm_find_itf(itf_data, comm_itf_num + 1, 0);
	}
	if (itf_desc == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "No data itf with BULK EP\n");
		return HAL_ERR_UNKNOWN;
	}
	cdc->data_itf_num = itf_desc->bInterfaceNumber;
	cdc->data_itf_alt = itf_desc->bAlternateSetting;
	cdc->comm_itf_num = comm_itf_num;

	if (usbh_cdc_acm_open_data_eps(host, cdc, itf_desc) != HAL_OK) {
		return HAL_ERR_HW;
	}
	return HAL_OK;
}

/**
  * @brief  Attach handler for Quectel dongles: a single vendor-specific
  *         interface carries INTR + BULK in + BULK out.
  */
static int usbh_cdc_acm_attach_quectel(usb_host_t *host, usbh_cdc_acm_host_t *cdc)
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
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Get vendor itf fail\n");
		return HAL_ERR_UNKNOWN;
	}

	itf_desc = usbh_cdc_acm_find_itf(itf_data, cdc->param_item->at_line_idx, 0);
	if ((itf_desc == NULL) || (itf_desc->bNumEndpoints != 3)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "No itf %d or EP count err\n", cdc->param_item->at_line_idx);
		return HAL_ERR_UNKNOWN;
	}
	cdc->data_itf_num = itf_desc->bInterfaceNumber;
	cdc->data_itf_alt = itf_desc->bAlternateSetting;
	cdc->comm_itf_num = itf_desc->bInterfaceNumber;

	usbh_cdc_acm_open_quectel_eps(host, cdc, itf_desc);
	return HAL_OK;
}

/**
  * @brief  Run the 4G-dongle control-setting sequence: SET_INTERFACE (data alt),
  *         SET_LINE_CODING, GET_LINE_CODING. Driven from setup() until HAL_OK.
  */
int usbh_cdc_acm_ctrl_setting(usb_host_t *host)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	int ret_state = HAL_ERR_UNKNOWN;
	int state = HAL_OK;

	switch (cdc->sub_status) {
	case USBH_CDC_ACM_CTRL_IDLE:
		ret_state = HAL_OK;
		break;

	case USBH_CDC_ACM_CTRL_ALT_SETTING:
		state = usbh_ctrl_set_interface(host, cdc->data_itf_num, cdc->data_itf_alt);
		if (state == HAL_OK) {
			cdc->sub_status = USBH_CDC_ACM_CTRL_SET_LINE_CODING;
		} else if (state != HAL_BUSY) {
			cdc->sub_status = USBH_CDC_ACM_CTRL_IDLE;
		}
		usbh_notify(host, 0, &usbh_cdc_acm_driver);
		break;

	case USBH_CDC_ACM_CTRL_SET_LINE_CODING:
		state = usbh_cdc_acm_process_set_line_coding(host, cdc->user_line_coding);
		if (state == HAL_OK) {
			cdc->sub_status = USBH_CDC_ACM_CTRL_GET_LINE_CODING;
		} else if (state != HAL_BUSY) {
			cdc->sub_status = USBH_CDC_ACM_CTRL_IDLE;
		}
		usbh_notify(host, 0, &usbh_cdc_acm_driver);
		break;

	case USBH_CDC_ACM_CTRL_GET_LINE_CODING:
		state = usbh_cdc_acm_process_get_line_coding(host, cdc->line_coding);
		if (state == HAL_OK) {
			cdc->sub_status = USBH_CDC_ACM_CTRL_IDLE;
			if ((cdc->cb != NULL) && (cdc->cb->line_coding_changed != NULL)) {
				cdc->cb->line_coding_changed(cdc->line_coding);
			}
			ret_state = HAL_OK;
		} else if (state != HAL_BUSY) {
			cdc->sub_status = USBH_CDC_ACM_CTRL_IDLE;
		}
		usbh_notify(host, 0, &usbh_cdc_acm_driver);
		break;

	default:
		ret_state = HAL_OK;
		break;
	}

	return ret_state;
}
#endif /* CONFIG_USBH_CDC_ACM_4G_DONGLE */

/**
  * @brief  Attach callback.
  * @param  host: Host handle
  * @retval HAL_OK on success, others on failure.
  */
static int usbh_cdc_acm_attach(usb_host_t *host)
{
	int status = HAL_ERR_UNKNOWN;
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;

#if CONFIG_USBH_CDC_ACM_4G_DONGLE
	/* 4G LTE dongle path: recognise the device by VID/PID and use the
	 * vendor-specific topology (Quectel single interface, or param-driven ACM). */
	u16 vid = host->dev_desc->idVendor;
	u16 pid = host->dev_desc->idProduct;
	int ret;

	cdc->param_item = usbh_cdc_acm_find_param(cdc->priv_param, vid, pid);
	if (cdc->param_item == NULL) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "No 4G cfg for VID=0x%04x PID=0x%04x\n", vid, pid);
		return HAL_ERR_PARA;
	}

	if (vid == USBH_CDC_ACM_QUECTEL_DONGLE_VID) {
		ret = usbh_cdc_acm_attach_quectel(host, cdc);
	} else {
		ret = usbh_cdc_acm_attach_acm_4g(host, cdc);
	}
	if (ret != HAL_OK) {
		return ret;
	}
	cdc->sub_status = USBH_CDC_ACM_CTRL_ALT_SETTING;
#else
	usbh_itf_data_t *itf_data;
	usbh_itf_desc_t *comm_itf_desc;
	usbh_itf_desc_t *data_itf_desc;
	usbh_ep_desc_t *ep_desc;
	usbh_pipe_t *bulk_out = &cdc->bulk_out;
	usbh_pipe_t *bulk_in = &cdc->bulk_in;
	usbh_dev_id_t dev_id = {0,};
	u8 comm_itf_num;
#if CONFIG_USBH_CDC_ACM_NOTIFY
	usbh_pipe_t *intr_in = &cdc->intr_in;
#endif

	/* Standard CDC-ACM topology: a communication interface (optional INTR
	 * endpoint) immediately followed by a data interface (BULK in/out).
	 *
	 * Match the communication interface by class+subclass only — the ACM
	 * control protocol (bInterfaceProtocol) varies across spec-compliant
	 * devices (0x00 none / 0x01 AT-V.250 / 0x02-0x06 / 0xFF vendor) and does
	 * not identify the interface (PSTN §6.3), so matching a fixed protocol
	 * would reject valid ACM devices.
	 *
	 * Limitation: usbh_get_interface_descriptor() returns the FIRST matching
	 * comm interface. On a composite device that exposes multiple ACM
	 * functions (e.g. AT + PPP), only the first one is used; the rest are
	 * left claimed by the core (iface_owner set in Phase 1) but never
	 * opened. Use the 4G-dongle path (CONFIG_USBH_CDC_ACM_4G_DONGLE) with a
	 * priv_param.at_line_idx to select a specific ACM comm interface. */
	dev_id.bInterfaceClass = USB_CDC_COMM_INTERFACE_CLASS_CODE;
	dev_id.bInterfaceSubClass = USB_CDC_SUBCLASS_ACM;
	dev_id.mMatchFlags = USBH_DEV_ID_MATCH_ITF_CLASS | USBH_DEV_ID_MATCH_ITF_SUBCLASS;
	itf_data = usbh_get_interface_descriptor(host, &dev_id);
	if (itf_data == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Get comm itf fail\n");
		return HAL_ERR_PARA;
	}
	comm_itf_desc = itf_data->itf_desc_array;
	comm_itf_num = comm_itf_desc->bInterfaceNumber;
	cdc->comm_itf_num = comm_itf_num;

	/* Communication interface: already claimed by the core's Phase 1 match
	 * (id_table matches class 0x02 + subclass ACM), no need to re-claim. */

#if CONFIG_USBH_CDC_ACM_NOTIFY
	/* The comm interface may legally carry 0 endpoints (no notify INTR EP). */
	if (comm_itf_desc->bNumEndpoints >= 1) {
		ep_desc = &comm_itf_desc->ep_desc_array[0];
		if ((ep_desc->bEndpointAddress & USB_REQ_DIR_MASK) == USB_D2H) {
			if (usbh_open_pipe(host, intr_in, ep_desc, &usbh_cdc_acm_driver) == HAL_OK) {
				intr_in->max_timeout_tick = USB_INTR_MAX_TIMEOUT_TICK;
			} else {
				RTK_LOGS(TAG, RTK_LOG_WARN, "Open intr pipe fail\n");
			}
		}
	}
#endif

	/* Get Data Interface */
	dev_id.bInterfaceClass = USB_CDC_DATA_INTERFACE_CLASS_CODE;
	dev_id.bInterfaceSubClass = USB_CDC_SUBCLASS_RESERVED;
	dev_id.bInterfaceProtocol = USB_CDC_CTRL_PROTOCOL_NO_CLASS_SPECIFIC;
	dev_id.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO;
	itf_data = usbh_get_interface_descriptor(host, &dev_id);
	if (itf_data == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Get data itf fail\n");
		goto open_fail;
	}

	/* In a composite device several CDC_DATA interfaces may exist (e.g. ACM
	 * data + ECM data). Select the one that belongs to this ACM function:
	 * the standard CDC topology places the ACM data interface immediately
	 * after its communication interface (comm_itf_num + 1). Walk the
	 * same-class chain to find it; fall back to the first node otherwise. */
	data_itf_desc = NULL;
	{
		usbh_itf_data_t *node = itf_data;
		while (node != NULL) {
			if ((node->itf_desc_array != NULL) &&
				(node->itf_desc_array[0].bInterfaceNumber == (comm_itf_num + 1))) {
				data_itf_desc = node->itf_desc_array;
				break;
			}
			node = node->next;
		}
	}
	if (data_itf_desc == NULL) {
		data_itf_desc = itf_data->itf_desc_array;
	}

	/* Set data in/out endpoints */
	for (int i = 0; i < data_itf_desc->bNumEndpoints && i < 2; i++) {
		ep_desc = &data_itf_desc->ep_desc_array[i];
		if ((ep_desc->bEndpointAddress & USB_REQ_DIR_MASK) == USB_D2H) {
			if (usbh_open_pipe(host, bulk_in, ep_desc, &usbh_cdc_acm_driver) != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Open bulk in pipe fail\n");
				goto open_fail;
			}
			bulk_in->max_timeout_tick = USB_BULK_IN_MAX_TIMEOUT_TICK;
		} else {
			if (usbh_open_pipe(host, bulk_out, ep_desc, &usbh_cdc_acm_driver) != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Open bulk out pipe fail\n");
				goto open_fail;
			}
			bulk_out->max_timeout_tick = USB_BULK_OUT_MAX_TIMEOUT_TICK;
		}
	}

	/* Both bulk pipes are mandatory for CDC ACM data transfer */
	if ((bulk_in->pipe_num == 0) || (bulk_out->pipe_num == 0)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Missing bulk pipe\n");
		goto open_fail;
	}
#endif

	/* Common tail: shared by the 4G-dongle and the standard CDC-ACM paths. */
	cdc->host = host;
	cdc->state = USBH_CDC_ACM_STATE_IDLE;

	usbh_cdc_acm_dump_desc();

	if ((cdc->cb != NULL) && (cdc->cb->attach != NULL)) {
		cdc->cb->attach();
	}

	return HAL_OK;

#if !CONFIG_USBH_CDC_ACM_4G_DONGLE
open_fail:
	/* Roll back any pipe already opened (guard with pipe_num: only close open pipes) */
#if CONFIG_USBH_CDC_ACM_NOTIFY
	if (intr_in->pipe_num) {
		usbh_close_pipe(host, intr_in);
	}
#endif
	if (bulk_in->pipe_num) {
		usbh_close_pipe(host, bulk_in);
	}
	if (bulk_out->pipe_num) {
		usbh_close_pipe(host, bulk_out);
	}
	return status;
#else
	UNUSED(status);
#endif
}

/**
  * @brief  Detach callback.
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_acm_detach(usb_host_t *host)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usbh_pipe_t *bulk_out = &cdc->bulk_out;
	usbh_pipe_t *bulk_in = &cdc->bulk_in;
#if CONFIG_USBH_CDC_ACM_NOTIFY
	usbh_pipe_t *intr_in = &cdc->intr_in;
#endif

	if ((cdc->cb != NULL) && (cdc->cb->detach != NULL)) {
		cdc->cb->detach();
	}
#if CONFIG_USBH_CDC_ACM_NOTIFY
	if (intr_in->pipe_num) {
		usbh_close_pipe(host, intr_in);
	}
#endif

	if (bulk_in->pipe_num) {
		usbh_close_pipe(host, bulk_in);
	}

	if (bulk_out->pipe_num) {
		usbh_close_pipe(host, bulk_out);
	}

	/* Device gone: drop the stale host handle and reset state so post-detach
	   transmit/receive calls are rejected by their host/connect_state checks. */
	cdc->host = NULL;
	cdc->state = USBH_CDC_ACM_STATE_IDLE;
	cdc->comm_itf_num = 0U;
	cdc->ctrl_line_state = 0U;
	cdc->break_duration = 0U;
#if CONFIG_USBH_CDC_ACM_4G_DONGLE
	cdc->param_item = NULL;
	cdc->sub_status = 0U;
#endif

	return HAL_OK;
}

/**
  * @brief  Standard control requests handling callback
  * @param  host: Host handle
  * @retval HAL_OK on success, others on failure.
  */
static int usbh_cdc_acm_setup(usb_host_t *host)
{
	int status = HAL_ERR_UNKNOWN;
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;

#if CONFIG_USBH_CDC_ACM_4G_DONGLE
	/* 4G dongle: run SET_INTERFACE + line-coding sequence before signalling ready. */
	if (cdc->param_item != NULL) {
		status = usbh_cdc_acm_ctrl_setting(host);
		if (status == HAL_OK) {
			if ((cdc->cb != NULL) && (cdc->cb->setup != NULL)) {
				cdc->cb->setup();
			}
		}
		return status;
	}
#endif

	/*Issue the get line coding request*/
	status = usbh_cdc_acm_process_get_line_coding(host, cdc->line_coding);
	if (status == HAL_OK) {
		if ((cdc->cb != NULL) && (cdc->cb->setup != NULL)) {
			cdc->cb->setup();
		}
	}
	return status;
}

#if CONFIG_USBH_CDC_ACM_NOTIFY
/**
  * @brief  SOF callback for CDC ACM. Runs in ISR context every microframe;
  *         time-consuming operations (e.g. `malloc`, `rtos_sema_take`) are not permitted.
  * @param  host: Host handle
  * @retval Status
  * @note   INTR IN watchdog + retry: when a notification transfer is in flight
  *         and its polling interval has elapsed, re-arm it (up to
  *         USB_INTR_RETRY_MAX_CNT retries, then mark error). Only relevant when
  *         CONFIG_USBH_CDC_ACM_NOTIFY=1 (this whole callback is compiled out
  *         otherwise). BULK IN reception is application-initiated: the app calls
  *         usbh_cdc_acm_receive() in a loop (re-arming buf/len each time), so
  *         the SOF does not drive bulk IN.
  */
static int usbh_cdc_acm_sof(usb_host_t *host)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usbh_pipe_t *intr_in = &cdc->intr_in;
	usbh_urb_state_t urb_state;
	u32 elapsed;

	/* INTR IN: watchdog + retry on timeout (only when a transfer is in flight). */
	if (intr_in->pipe_num != 0) {
		elapsed = usbh_get_elapsed_frame_cnt(host, intr_in->frame_num);

		if (intr_in->xfer_state == USBH_EP_XFER_BUSY) {
			urb_state = usbh_get_urb_state(host, intr_in);
			if ((urb_state == USBH_URB_BUSY) || (urb_state == USBH_URB_IDLE)) {
				if (elapsed + 1 >= intr_in->ep_interval) {
					intr_in->retry_cnt++;
					if (intr_in->retry_cnt <= USB_INTR_RETRY_MAX_CNT) {
						intr_in->frame_num = usbh_get_current_frame_number(host);
						intr_in->xfer_state = USBH_EP_XFER_START;
						usbh_notify(host, intr_in->pipe_num, &usbh_cdc_acm_driver);
					} else {
						/* Retry count exceeded, set error */
						intr_in->retry_cnt = 0;
						intr_in->xfer_state = USBH_EP_XFER_ERROR;
						usbh_notify(host, intr_in->pipe_num, &usbh_cdc_acm_driver);
					}
				}
				/* If interval not elapsed, keep BUSY state */
			}
			/* If URB is DONE, wait for process_intr_rx to handle complete */
		}
	}

	return HAL_OK;
}

/**
  * @brief  Transfer completion callback: handle the end of an interrupt IN transfer,
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param[in] host: Pointer to the USB host handle.
  * @param[in] pipe_num: Pipe number of the completed transfer.
  * @return 0 on success, non-zero on failure.
  */
static int usbh_cdc_acm_completed(usb_host_t *host, u8 pipe_num)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usbh_pipe_t *intr_in = &cdc->intr_in;

	/* Only consume the completion if it targets a pipe this driver owns.
	 * Returning HAL_BUSY for a foreign pipe lets the core offer the event to
	 * the next attached class driver (composite support). */
	if (intr_in->pipe_num && (pipe_num == intr_in->pipe_num)) {
		if (intr_in->xfer_state == USBH_EP_XFER_BUSY) {
			usbh_notify(host, intr_in->pipe_num, &usbh_cdc_acm_driver);
		}
		return HAL_OK;
	}

	return HAL_BUSY;
}
#endif

/**
* @brief  State machine handling callback
* @param  host:Host handle
* @param  event: USB host event
* @retval Status
*/
static int usbh_cdc_acm_process(usb_host_t *host, usbh_event_t *event)
{
	int status = HAL_BUSY;
	u8 req_status = HAL_OK;
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;

	switch (cdc->state) {

	case USBH_CDC_ACM_STATE_IDLE:
		status = HAL_OK;
		break;

	case USBH_CDC_ACM_STATE_SET_CONTROL_LINE_STATE:
		req_status = usbh_cdc_acm_process_set_control_line_state(host);
		if (req_status == HAL_OK) {
			cdc->state = USBH_CDC_ACM_STATE_IDLE;
		} else if (req_status != HAL_BUSY) {
			cdc->state = USBH_CDC_ACM_STATE_ERROR;
		}
		usbh_notify(host, 0, &usbh_cdc_acm_driver);
		break;

	case USBH_CDC_ACM_STATE_SEND_BREAK:
		req_status = usbh_cdc_acm_process_send_break(host);
		if (req_status == HAL_OK) {
			cdc->state = USBH_CDC_ACM_STATE_IDLE;
		} else if (req_status != HAL_BUSY) {
			cdc->state = USBH_CDC_ACM_STATE_ERROR;
		}
		usbh_notify(host, 0, &usbh_cdc_acm_driver);
		break;

	case USBH_CDC_ACM_STATE_SET_LINE_CODING:
		req_status = usbh_cdc_acm_process_set_line_coding(host, cdc->user_line_coding);
		if (req_status == HAL_OK) {
			cdc->state = USBH_CDC_ACM_STATE_GET_LINE_CODING;
		} else if (req_status != HAL_BUSY) {
			cdc->state = USBH_CDC_ACM_STATE_ERROR;
		}
		usbh_notify(host, 0, &usbh_cdc_acm_driver);
		break;

	case USBH_CDC_ACM_STATE_GET_LINE_CODING:
		req_status = usbh_cdc_acm_process_get_line_coding(host, cdc->line_coding);
		if (req_status == HAL_OK) {
			usb_cdc_line_coding_t *lc = cdc->line_coding;
			usb_cdc_line_coding_t *ulc = cdc->user_line_coding;
			cdc->state = USBH_CDC_ACM_STATE_IDLE;
			if ((lc->b.bCharFormat == ulc->b.bCharFormat) &&
				(lc->b.bDataBits == ulc->b.bDataBits) &&
				(lc->b.bParityType == ulc->b.bParityType) &&
				(lc->b.dwDteRate == ulc->b.dwDteRate)) {
				if ((cdc->cb != NULL) && (cdc->cb->line_coding_changed != NULL)) {
					cdc->cb->line_coding_changed(cdc->line_coding);
				}
			}
		} else if (req_status != HAL_BUSY) {
			cdc->state = USBH_CDC_ACM_STATE_ERROR;
		}
		usbh_notify(host, 0, &usbh_cdc_acm_driver);
		break;

	case USBH_CDC_ACM_STATE_TRANSFER:
		if (event) {
			if (event->pipe_num == cdc->bulk_out.pipe_num) {
				usbh_cdc_acm_process_tx(host);
			} else if (event->pipe_num == cdc->bulk_in.pipe_num) {
				usbh_cdc_acm_process_rx(host);
#if CONFIG_USBH_CDC_ACM_NOTIFY
			} else if (event->pipe_num == cdc->intr_in.pipe_num) {
				usbh_cdc_acm_process_intr_rx(host);
#endif
			}
		}
		break;

	case USBH_CDC_ACM_STATE_ERROR:
		req_status = usbh_ctrl_clear_feature(host, 0x00U);
		if (req_status == HAL_OK) {
			cdc->state = USBH_CDC_ACM_STATE_IDLE;
		}
		break;

	default:
		break;

	}

	return status;
}

/**
  * @brief  Get line coding
  * @param  host: Host handle
  * @param  linecoding: Line coding data pointer
  * @retval Status
  */
static int usbh_cdc_acm_process_get_line_coding(usb_host_t *host, usb_cdc_line_coding_t *linecoding)
{
	usbh_setup_req_t setup;
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	u16 windex = cdc->comm_itf_num;

#if CONFIG_USBH_CDC_ACM_4G_DONGLE
	if (cdc->param_item != NULL) {
		windex = cdc->param_item->at_line_idx;
	}
#endif

	setup.req.bmRequestType = USB_D2H | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = USB_CDC_ACM_GET_LINE_CODING;
	setup.req.wValue = 0U;
	setup.req.wIndex = windex;
	setup.req.wLength = USB_CDC_ACM_LINE_CODING_SIZE;

	return usbh_ctrl_request(host, &setup, linecoding->d8);
}

/**
  * @brief  Set line coding
  * @param  host: Host handle
  * @param  linecoding: Line coding data pointer
  * @retval Status
  */
static int usbh_cdc_acm_process_set_line_coding(usb_host_t *host, usb_cdc_line_coding_t *linecoding)
{
	usbh_setup_req_t setup;
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	u16 windex = cdc->comm_itf_num;

#if CONFIG_USBH_CDC_ACM_4G_DONGLE
	if (cdc->param_item != NULL) {
		windex = cdc->param_item->at_line_idx;
	}
#endif

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = USB_CDC_ACM_SET_LINE_CODING;
	setup.req.wValue = 0U;
	setup.req.wIndex = windex;
	setup.req.wLength = USB_CDC_ACM_LINE_CODING_SIZE;

	return usbh_ctrl_request(host, &setup, linecoding->d8);
}

/**
  * @brief  Set control line
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_acm_process_set_control_line_state(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = USB_CDC_ACM_SET_CONTROL_LINE_STATE;
	setup.req.wValue = cdc->ctrl_line_state;
	setup.req.wIndex = cdc->comm_itf_num;
	setup.req.wLength = 0U;
	return usbh_ctrl_request(host, &setup, NULL);
}

/**
  * @brief  Issue the CDC SEND_BREAK class request to the device.
  * @param  host: Host handle
  * @retval Status from usbh_ctrl_request
  */
static int usbh_cdc_acm_process_send_break(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = USB_CDC_ACM_SEND_BREAK;
	setup.req.wValue = cdc->break_duration;
	setup.req.wIndex = cdc->comm_itf_num;
	setup.req.wLength = 0U;
	return usbh_ctrl_request(host, &setup, NULL);
}

/**
  * @brief  TX handling
  * @param  host: Host handle
  * @retval None
*/
static void usbh_cdc_acm_process_tx(usb_host_t *host)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usbh_pipe_t *bulk_out = &cdc->bulk_out;

	int status = usbh_transfer_process(host, bulk_out);

	if ((status == HAL_OK) && (bulk_out->xfer_state == USBH_EP_XFER_IDLE)) {
		if ((cdc->cb != NULL) && (cdc->cb->transmit != NULL)) {
			cdc->cb->transmit(status);
		}
	} else if (bulk_out->xfer_state == USBH_EP_XFER_START) {
		usbh_notify(host, bulk_out->pipe_num, &usbh_cdc_acm_driver);
	} else if (bulk_out->xfer_state == USBH_EP_XFER_ERROR) {
		bulk_out->xfer_state = USBH_EP_XFER_IDLE;
		RTK_LOGS(TAG, RTK_LOG_ERROR, "BULK TX fail: %d\n", usbh_get_urb_state(host, bulk_out));
		if ((cdc->cb != NULL) && (cdc->cb->transmit != NULL)) {
			cdc->cb->transmit(status);
		}
	}
}

/**
  * @brief  RX handling
  * @param  host: Host handle
  * @retval None
*/
static void usbh_cdc_acm_process_rx(usb_host_t *host)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usbh_pipe_t *bulk_in = &cdc->bulk_in;
	u32 len;
	int status;

	status = usbh_transfer_process(host, bulk_in);

	if ((status == HAL_OK) && (bulk_in->xfer_state == USBH_EP_XFER_IDLE)) {
		len = usbh_get_last_transfer_size(host, bulk_in);
		if ((cdc->cb != NULL) && (cdc->cb->receive != NULL)) {
			cdc->cb->receive(bulk_in->xfer_buf, len, status);
		}
	} else if (bulk_in->xfer_state == USBH_EP_XFER_START) {
		len = usbh_get_last_transfer_size(host, bulk_in);
		if ((bulk_in->xfer_len == 0) && (cdc->cb != NULL) && (cdc->cb->receive != NULL)) {
			cdc->cb->receive(bulk_in->xfer_buf, len, status);
		}
		usbh_notify(host, bulk_in->pipe_num, &usbh_cdc_acm_driver);
	} else if (bulk_in->xfer_state == USBH_EP_XFER_ERROR) {
		bulk_in->xfer_state = USBH_EP_XFER_IDLE;
		RTK_LOGS(TAG, RTK_LOG_ERROR, "BULK RX fail: %d\n", usbh_get_urb_state(host, bulk_in));
		if ((cdc->cb != NULL) && (cdc->cb->receive != NULL)) {
			cdc->cb->receive(NULL, 0, status);
		}
	}
}

/**
  * @brief  Intr in function
  * @param  host: Host handle
  */
#if CONFIG_USBH_CDC_ACM_NOTIFY
static void usbh_cdc_acm_process_intr_rx(usb_host_t *host)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usbh_pipe_t *intr_in = &cdc->intr_in;
	u32 len;

	int status = usbh_transfer_process(host, intr_in);

	if ((status == HAL_OK) && (intr_in->xfer_state == USBH_EP_XFER_IDLE)) {
		len = usbh_get_last_transfer_size(host, intr_in);
		if ((cdc->cb != NULL) && (cdc->cb->notify != NULL)) {
			if (len >= 8U) {
				/* Parse CDC notification header: byte1=bNotificationCode, bytes6-7=wLength */
				u8 notif_code = intr_in->xfer_buf[1];
				u16 data_len = (u16)intr_in->xfer_buf[6] | ((u16)intr_in->xfer_buf[7] << 8);
				u8 *payload = intr_in->xfer_buf + 8U;
				switch (notif_code) {
				case USB_CDC_ACM_NOTIFY_SERIAL_STATE:
					if (len >= (8U + (u32)data_len)) {
						cdc->cb->notify(payload, (u32)data_len, status);
					} else {
						cdc->cb->notify(NULL, 0U, status);
					}
					break;
				case USB_CDC_ACM_NOTIFY_RESPONSE_AVAILABLE:
					cdc->cb->notify(NULL, 0U, status);
					break;
				default:
					cdc->cb->notify(payload, (u32)data_len, status);
					break;
				}
			} else {
				cdc->cb->notify(intr_in->xfer_buf, len, status);
			}
		}
	} else if (intr_in->xfer_state == USBH_EP_XFER_START) {
		usbh_notify(host, intr_in->pipe_num, &usbh_cdc_acm_driver);
	} else if (intr_in->xfer_state == USBH_EP_XFER_ERROR) {
		intr_in->xfer_state = USBH_EP_XFER_IDLE;
		RTK_LOGS(TAG, RTK_LOG_ERROR, "INTR RX fail: %d\n", usbh_get_urb_state(host, intr_in));
		if ((cdc->cb != NULL) && (cdc->cb->notify != NULL)) {
			cdc->cb->notify(NULL, 0, status);
		}
	}
}
#endif

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Init CDC ACM class
  * @param  cb: User callback
  * @retval Status
  */
int usbh_cdc_acm_init(const usbh_cdc_acm_cb_t *cb)
{
	int ret = HAL_OK;
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;

	if (cb == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid user CB\n");
		return HAL_ERR_PARA;
	}

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

	ret = usbh_register_class(&usbh_cdc_acm_driver);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Register class fail %d\n", ret);
		usb_os_mfree(cdc->line_coding);
		cdc->line_coding = NULL;
		usb_os_mfree(cdc->user_line_coding);
		cdc->user_line_coding = NULL;
		return ret;
	}

	cdc->ctrl_line_state = 0x01U;
	cdc->cb = cb;

#if CONFIG_USBH_CDC_ACM_4G_DONGLE
	cdc->priv_param = cb->priv;
	cdc->param_item = NULL;
#endif

	return ret;
}

/**
  * @brief  Deinit CDC ACM class
  * @retval Status
  */
int usbh_cdc_acm_deinit(void)
{
	int ret = HAL_OK;
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usb_host_t *host = cdc->host;

	usbh_unregister_class(&usbh_cdc_acm_driver);

	if ((cdc->cb != NULL) && (cdc->cb->deinit != NULL)) {
		cdc->cb->deinit();
	}

	if ((host != NULL) && (host->connect_state == USBH_STATE_SETUP)) {
		cdc->state = USBH_CDC_ACM_STATE_IDLE;
#if CONFIG_USBH_CDC_ACM_NOTIFY
		/* Guard with pipe_num: an unopened pipe has pipe_num == 0, and
		   usbh_close_pipe() would otherwise halt/free the control pipe (pipe 0). */
		if (cdc->intr_in.pipe_num) {
			usbh_close_pipe(host, &cdc->intr_in);
		}
#endif
		if (cdc->bulk_in.pipe_num) {
			usbh_close_pipe(host, &cdc->bulk_in);
		}
		if (cdc->bulk_out.pipe_num) {
			usbh_close_pipe(host, &cdc->bulk_out);
		}
	}

	if (cdc->line_coding != NULL) {
		usb_os_mfree(cdc->line_coding);
		cdc->line_coding = NULL;
	}
	if (cdc->user_line_coding != NULL) {
		usb_os_mfree(cdc->user_line_coding);
		cdc->user_line_coding = NULL;
	}

	/* Cover the deinit-while-connected path (no detach ran): drop the host
	   handle so post-deinit transmit/receive calls are rejected. The detach
	   path already cleared it. */
	cdc->host = NULL;

	return ret;
}

/**
* @brief  Set the control line status (PSTN §6.3.12)
* @param  bitmap: Control Signal Bitmap: D0=DTR, D1=RTS
* @retval HAL_OK on success, others on failure.
*/
int usbh_cdc_acm_set_control_line_state(u16 bitmap)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usb_host_t *host = cdc->host;
	int ret = HAL_ERR_UNKNOWN;

	if (host == NULL) {
		return HAL_ERR_PARA;
	}

	if (host->connect_state == USBH_STATE_SETUP) {
		cdc->ctrl_line_state = bitmap;
		cdc->state = USBH_CDC_ACM_STATE_SET_CONTROL_LINE_STATE;
		usbh_notify(host, 0, &usbh_cdc_acm_driver);
		ret = HAL_OK;
	}

	return ret;
}

/**
* @brief  Send break signal (PSTN §6.3.13)
* @param  duration_ms: Break duration in ms; 0xFFFF = continuous; 0x0000 = end break
* @retval HAL_OK on success, others on failure.
*/
int usbh_cdc_acm_send_break(u16 duration_ms)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usb_host_t *host = cdc->host;
	int ret = HAL_ERR_UNKNOWN;

	if (host == NULL) {
		return HAL_ERR_PARA;
	}

	if (host->connect_state == USBH_STATE_SETUP) {
		cdc->break_duration = duration_ms;
		cdc->state = USBH_CDC_ACM_STATE_SEND_BREAK;
		usbh_notify(host, 0, &usbh_cdc_acm_driver);
		ret = HAL_OK;
	}

	return ret;
}

/**
* @brief  Set the line coding parameters (PSTN §6.3.10 SET_LINE_CODING)
* @param  line_coding: Line coding data to apply
* @retval HAL_OK on success, others on failure.
*/
int usbh_cdc_acm_set_line_coding(usb_cdc_line_coding_t *line_coding)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usb_host_t *host = cdc->host;
	int ret = HAL_ERR_UNKNOWN;

	if ((host == NULL) || (line_coding == NULL)) {
		return HAL_ERR_PARA;
	}

	if (host->connect_state == USBH_STATE_SETUP) {
		usb_cdc_line_coding_t *ulc = cdc->user_line_coding;
		usb_cdc_line_coding_t *lc = line_coding;
		cdc->state = USBH_CDC_ACM_STATE_SET_LINE_CODING;
		ulc->b.dwDteRate = lc->b.dwDteRate;
		ulc->b.bCharFormat = lc->b.bCharFormat;
		ulc->b.bParityType = lc->b.bParityType;
		ulc->b.bDataBits = lc->b.bDataBits;
		usbh_notify(host, 0, &usbh_cdc_acm_driver);
		ret = HAL_OK;
	}

	return ret;
}

/**
* @brief  Get the current line coding parameters (PSTN §6.3.11 GET_LINE_CODING)
* @param  line_coding: Pointer to store the retrieved line coding data
* @retval HAL_OK on success, others on failure.
*/
int usbh_cdc_acm_get_line_coding(usb_cdc_line_coding_t *line_coding)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usb_host_t *host = cdc->host;
	int ret = HAL_ERR_UNKNOWN;

	if ((host == NULL) || (line_coding == NULL)) {
		return HAL_ERR_PARA;
	}

	if ((host->connect_state == USBH_STATE_SETUP) || (host->connect_state == USBH_STATE_ATTACH)) {
		usb_cdc_line_coding_t *lc = cdc->line_coding;
		usb_cdc_line_coding_t *ulc = line_coding;
		ulc->b.dwDteRate = lc->b.dwDteRate;
		ulc->b.bCharFormat = lc->b.bCharFormat;
		ulc->b.bParityType = lc->b.bParityType;
		ulc->b.bDataBits = lc->b.bDataBits;
		ret = HAL_OK;
	}

	return ret;
}

/**
  * @brief  Start to transfer data via BULK OUT pipe
  * @param  buf: Data buffer
  * @param  len: Data length
  * @retval HAL_OK on success, others on failure.
  */
int usbh_cdc_acm_transmit(u8 *buf, u32 len)
{
	int ret = HAL_ERR_UNKNOWN;
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usb_host_t *host = cdc->host;
	usbh_pipe_t *pipe = &cdc->bulk_out;

	if ((host == NULL) || (buf == NULL)) {
		return HAL_ERR_PARA;
	}

	if ((host->connect_state == USBH_STATE_SETUP) &&
		((cdc->state == USBH_CDC_ACM_STATE_IDLE) || (cdc->state == USBH_CDC_ACM_STATE_TRANSFER))) {
		if (pipe->xfer_state == USBH_EP_XFER_IDLE) {
			pipe->xfer_buf = buf;
			pipe->xfer_len = len;

			if ((pipe->xfer_len > 0) && (pipe->xfer_len % pipe->ep_mps) == 0) { //ZLP
				pipe->trx_zlp = 1;
			} else {
				pipe->trx_zlp = 0;
			}

			cdc->state = USBH_CDC_ACM_STATE_TRANSFER;
			pipe->xfer_state = USBH_EP_XFER_START;
			usbh_notify(host, pipe->pipe_num, &usbh_cdc_acm_driver);
			ret = HAL_OK;
		}
	}

	return ret;
}

/**
  * @brief  Start to receive data via BULK IN pipe
  * @param  buf: Data buffer
  * @param  len: Data length
  * @retval HAL_OK on success, others on failure.
  */
int usbh_cdc_acm_receive(u8 *buf, u32 len)
{
	int ret = HAL_ERR_UNKNOWN;
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usb_host_t *host = cdc->host;
	usbh_pipe_t *pipe = &cdc->bulk_in;

	if ((host == NULL) || (buf == NULL)) {
		return HAL_ERR_PARA;
	}

	if ((host->connect_state == USBH_STATE_SETUP) &&
		((cdc->state == USBH_CDC_ACM_STATE_IDLE) || (cdc->state == USBH_CDC_ACM_STATE_TRANSFER))) {
		if (pipe->xfer_state == USBH_EP_XFER_IDLE) {
			pipe->xfer_buf = buf;
			pipe->xfer_len = len;

			if ((pipe->xfer_len > 0) && (pipe->xfer_len % pipe->ep_mps) == 0) { //ZLP
				pipe->trx_zlp = 1;
			} else {
				pipe->trx_zlp = 0;
			}

			cdc->state = USBH_CDC_ACM_STATE_TRANSFER;
			pipe->xfer_state = USBH_EP_XFER_START;
			usbh_notify(host, pipe->pipe_num, &usbh_cdc_acm_driver);
			ret = HAL_OK;
		}
	}

	return ret;
}

#if CONFIG_USBH_CDC_ACM_NOTIFY
/**
  * @brief  Start to receive notify data via INTERRUPT IN pipe
  * @param  buf: Data buffer
  * @param  len: Data length
  * @retval HAL_OK on success, others on failure.
  */
int usbh_cdc_acm_notify_receive(u8 *buf, u32 len)
{
	int ret = HAL_ERR_UNKNOWN;
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usb_host_t *host = cdc->host;
	usbh_pipe_t *pipe = &cdc->intr_in;

	if ((host == NULL) || (buf == NULL)) {
		return HAL_ERR_PARA;
	}

	if ((host->connect_state == USBH_STATE_SETUP) &&
		((cdc->state == USBH_CDC_ACM_STATE_IDLE) || (cdc->state == USBH_CDC_ACM_STATE_TRANSFER))) {
		if (pipe->xfer_state == USBH_EP_XFER_IDLE) {
			pipe->xfer_buf = buf;
			pipe->xfer_len = pipe->ep_mps;

			/* The user buf len < MPS, update the xfer length */
			if (len < pipe->ep_mps) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "Pls inc inbuf len %u-%u\n", len, pipe->ep_mps);
				pipe->xfer_len = len;
			}

			cdc->state = USBH_CDC_ACM_STATE_TRANSFER;
			pipe->xfer_state = USBH_EP_XFER_START;
			usbh_notify(host, pipe->pipe_num, &usbh_cdc_acm_driver);
			ret = HAL_OK;
		}
	}

	return ret;
}
#endif

/**
  * @brief  Get the BULK IN max packet size
  * @retval Max packet size
  */
u16 usbh_cdc_acm_get_bulk_ep_mps(void)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	return cdc->bulk_in.ep_mps;
}
