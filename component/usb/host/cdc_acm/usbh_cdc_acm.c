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

/* Line coding assumed when the device does not report its own: 115200 8N1. */
#define USBH_CDC_ACM_DEFAULT_DTE_RATE               115200U
#define USBH_CDC_ACM_DEFAULT_DATA_BITS              8U

/* SET_CONTROL_LINE_STATE wValue: D0 = DTR, D1 = RTS, D15..D2 reserved (Ref PSTN 1.2 6.3.12) */
#define USBH_CDC_ACM_CTRL_LINE_STATE_MASK           0x0003U

/* Private types -------------------------------------------------------------*/

#if CONFIG_USBH_CDC_ACM_4G_DONGLE
/* 4G-dongle control-setting sub-states (SET_INTERFACE + line coding). */
typedef enum {
	USBH_CDC_ACM_CTRL_IDLE = 0U,
	USBH_CDC_ACM_CTRL_ALT_SETTING,
	USBH_CDC_ACM_CTRL_SET_LINE_CODING,
	USBH_CDC_ACM_CTRL_GET_LINE_CODING,
} usbh_cdc_acm_ctrl_state_t;
#endif

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int usbh_cdc_acm_attach(usb_host_t *host);
static void usbh_cdc_acm_detach(usb_host_t *host);
static void usbh_cdc_acm_process(usb_host_t *host, usbh_drv_msg_t *msg);
static void usbh_cdc_acm_sof(usb_host_t *host);
#ifdef CONFIG_USBH_CDC_ACM_NOTIFY
static void usbh_cdc_acm_completed(usb_host_t *host, u8 pipe_num);
static void usbh_cdc_acm_process_intr_rx(usb_host_t *host);
#endif
static int usbh_cdc_acm_setup(usb_host_t *host);
static int usbh_cdc_acm_process_get_line_coding(usb_host_t *host, usb_cdc_acm_line_coding_t *linecoding);
static int usbh_cdc_acm_process_set_line_coding(usb_host_t *host, usb_cdc_acm_line_coding_t *linecoding);
static int usbh_cdc_acm_process_set_control_line_state(usb_host_t *host);
static int usbh_cdc_acm_process_send_break(usb_host_t *host);
static void usbh_cdc_acm_process_tx(usb_host_t *host);
static void usbh_cdc_acm_process_rx(usb_host_t *host);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "ACMH";

#if CONFIG_USBH_CDC_ACM_4G_DONGLE

#define USBH_CDC_ACM_INTR_MAX_TIMEOUT_TICK          USB_INTR_MAX_TIMEOUT_TICK
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
#if CONFIG_USBH_CDC_ACM_4G_DONGLE
	{
		/* Some 4G dongles (e.g. Quectel EG91) expose their AT command
		 * interface as a vendor-specific interface (bInterfaceClass=0xFF,
		 * bInterfaceSubClass=0x00, bInterfaceProtocol=0x00) rather than
		 * using the standard CDC-ACM descriptor (class 0x02 / subclass 0x02).
		 * This entry ensures Phase-1 claims that interface so attach() can
		 * locate and open its bulk endpoints via the vendor-specific path
		 * (usbh_cdc_acm_attach_quectel). Bound to the Quectel VID, the only
		 * vendor taking that path: a vendor-specific interface of any other
		 * device belongs to another driver (e.g. usbh_vendor), and claiming it
		 * here would keep it from that driver after attach() rejects it. */
		.mMatchFlags = USBH_DEV_ID_MATCH_VID | USBH_DEV_ID_MATCH_ITF_CLASS | USBH_DEV_ID_MATCH_ITF_SUBCLASS,
		.idVendor = USBH_CDC_ACM_QUECTEL_DONGLE_VID,
		.bInterfaceClass = 0xFF,
		.bInterfaceSubClass = 0x00,
	},
#endif
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
	.sof = usbh_cdc_acm_sof,
#ifdef CONFIG_USBH_CDC_ACM_NOTIFY
	.completed = usbh_cdc_acm_completed,
#endif
};

static usbh_cdc_acm_host_t usbh_cdc_acm_host;

/* Exported variables --------------------------------------------------------*/

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

/**
  * @brief  Validate a line coding before it is sent in SET_LINE_CODING
  * @note   Ref PSTN 1.2 Table 17: bDataBits is one of 5/6/7/8/16, bCharFormat selects
  *         1 / 1.5 / 2 stop bits, and bParityType is none/odd/even/mark/space. A value
  *         outside those sets would be sent verbatim, so it is rejected by the caller.
  * @param  data_bits: bDataBits
  * @param  char_format: bCharFormat
  * @param  parity_type: bParityType
  * @retval HAL_OK when the line coding is legal, HAL_ERR_PARA otherwise
  */
static int usbh_cdc_acm_check_line_coding(u8 data_bits, u8 char_format, u8 parity_type)
{
	if ((data_bits != 5U) && (data_bits != 6U) && (data_bits != 7U) &&
		(data_bits != 8U) && (data_bits != 16U)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid data bits %d\n", data_bits);
		return HAL_ERR_PARA;
	}

	if (char_format > LINE_CODING_CHAR_FORMAT_2_STOP_BITS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid char format %d\n", char_format);
		return HAL_ERR_PARA;
	}

	if (parity_type > LINE_CODING_PARITY_SPACE) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid parity %d\n", parity_type);
		return HAL_ERR_PARA;
	}

	return HAL_OK;
}

/**
  * @brief  Load the default line coding, 115200 8N1, for a device that does not report its own
  * @retval None
  */
static void usbh_cdc_acm_set_default_line_coding(void)
{
	usb_cdc_acm_line_coding_t *lc = usbh_cdc_acm_host.line_coding;

	lc->b.dwDteRate = USBH_CDC_ACM_DEFAULT_DTE_RATE;
	lc->b.bCharFormat = LINE_CODING_CHAR_FORMAT_1_STOP_BITS;
	lc->b.bParityType = LINE_CODING_PARITY_NO;
	lc->b.bDataBits = USBH_CDC_ACM_DEFAULT_DATA_BITS;
}

/**
  * @brief  Close every pipe of the class that is open
  * @note   Guarded with pipe_num: an unopened pipe has pipe_num 0, and closing it would halt the control pipe.
  * @param  host: Host handle
  * @retval None
  */
static void usbh_cdc_acm_close_pipes(usb_host_t *host)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;

	if (cdc->intr_in.pipe_num != 0U) {
		usbh_close_pipe(host, &cdc->intr_in);
	}
	if (cdc->bulk_in.pipe_num != 0U) {
		usbh_close_pipe(host, &cdc->bulk_in);
	}
	if (cdc->bulk_out.pipe_num != 0U) {
		usbh_close_pipe(host, &cdc->bulk_out);
	}
}

/**
  * @brief  Open one BULK endpoint of the data interface
  * @note   Only a BULK endpoint carries ACM data (Ref CDC 1.2 3.3.2), and each direction is opened once: a second
  *         endpoint of the same direction would overwrite the pipe of the first and leak its channel.
  * @param  host: Host handle
  * @param  ep_desc: Endpoint descriptor
  * @retval HAL_OK when opened or skipped as not BULK, HAL_ERR_PARA on a duplicate direction or an open failure
  */
static int usbh_cdc_acm_open_bulk_ep(usb_host_t *host, usbh_ep_desc_t *ep_desc)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usbh_pipe_t *pipe;
	u32 timeout;

	if ((ep_desc->bmAttributes & USB_EP_XFER_TYPE_MASK) != USB_CH_EP_TYPE_BULK) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "Skip EP%02x type %d\n", ep_desc->bEndpointAddress, ep_desc->bmAttributes);
		return HAL_OK;
	}

	if (USB_EP_IS_IN(ep_desc->bEndpointAddress)) {
		pipe = &cdc->bulk_in;
		timeout = USB_BULK_IN_MAX_TIMEOUT_TICK;
	} else {
		pipe = &cdc->bulk_out;
		timeout = USB_BULK_OUT_MAX_TIMEOUT_TICK;
	}

	if (pipe->pipe_num != 0U) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Dup EP%02x\n", ep_desc->bEndpointAddress);
		return HAL_ERR_PARA;
	}

	if (usbh_open_pipe(host, pipe, ep_desc, &usbh_cdc_acm_driver) != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Open EP%02x fail\n", ep_desc->bEndpointAddress);
		return HAL_ERR_PARA;
	}
	pipe->max_timeout_tick = timeout;

	return HAL_OK;
}

/**
  * @brief  Check that both BULK pipes are open
  * @note   ACM data needs a BULK IN and a BULK OUT (Ref CDC 1.2 3.3.2). A path that opened neither, or only
  *         one, would leave pipe_num 0, which is EP0, and ep_mps 0 to the transmit/receive length math.
  * @retval HAL_OK when both are open, HAL_ERR_PARA otherwise
  */
static int usbh_cdc_acm_check_bulk_pipes(void)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;

	if ((cdc->bulk_in.pipe_num == 0U) || (cdc->bulk_out.pipe_num == 0U)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Missing bulk pipe\n");
		return HAL_ERR_PARA;
	}

	return HAL_OK;
}

/**
  * @brief  Atomically claim an idle data pipe and arm it for one transfer
  * @note   The check and the claim must be one step: two callers, possibly on different cores, could both see the
  *         pipe idle and overwrite each other's buffer. The busy flag, not xfer_state, is what keeps the pipe owned
  *         until the completion is reported, because the core already sets xfer_state back to IDLE before the class
  *         has read the result.
  * @param  pipe: Data pipe
  * @param  busy: Busy flag of that pipe
  * @param  buf: Transfer buffer
  * @param  len: Transfer length
  * @param  buf_len: Capacity of buf, refer to usbh_pipe_t xfer_buf_len
  * @retval HAL_OK when armed, HAL_ERR_UNKNOWN when the pipe is not open or still in use
  */
static int usbh_cdc_acm_claim_pipe(usbh_pipe_t *pipe, u8 *busy, u8 *buf, u32 len, u32 buf_len)
{
	int ret = HAL_ERR_UNKNOWN;

	usb_os_enter_critical();
	/* An unopened pipe has pipe_num 0, which is EP0, and ep_mps 0 */
	if ((pipe->pipe_num != 0U) && (*busy == 0U) && (pipe->xfer_state == USBH_EP_XFER_IDLE)) {
		*busy = 1U;
		pipe->xfer_buf = buf;
		pipe->xfer_len = len;
		pipe->xfer_buf_len = buf_len;
		/* A BULK transfer of N*MPS bytes ends with a ZLP (Ref USB 2.0 5.8.3) */
		if ((pipe->ep_type == USB_CH_EP_TYPE_BULK) && (len > 0U) && ((len % pipe->ep_mps) == 0U)) {
			pipe->trx_zlp = 1U;
		} else {
			pipe->trx_zlp = 0U;
		}
		pipe->xfer_state = USBH_EP_XFER_START;
		ret = HAL_OK;
	}
	usb_os_exit_critical();

	return ret;
}

/**
  * @brief  Release a data pipe or EP0 claimed by an API caller
  * @note   Taken under the critical section so the stores that finished the transfer are visible to the other core
  *         before it can claim again.
  * @param  flag: Busy flag or EP0 state to clear
  * @retval None
  */
static void usbh_cdc_acm_release(u8 *flag)
{
	usb_os_enter_critical();
	*flag = 0U;
	usb_os_exit_critical();
}

/**
  * @brief  Publish line_coding as filled, to usbh_cdc_acm_get_line_coding()
  * @note   Taken under the critical section so the line coding stored before is visible to the other core first.
  * @retval None
  */
static void usbh_cdc_acm_set_line_coding_valid(void)
{
	usb_os_enter_critical();
	usbh_cdc_acm_host.line_coding_valid = 1U;
	usb_os_exit_critical();
}

/**
  * @brief  Atomically claim EP0 for one class request
  * @param  state: Request state to enter, @ref usbh_cdc_acm_state_t
  * @param  value: wValue of SET_CONTROL_LINE_STATE / SEND_BREAK, ignored otherwise
  * @param  lc: Line coding of SET_LINE_CODING, NULL otherwise
  * @retval HAL_OK when claimed, HAL_BUSY while a previous request is still in flight
  */
static int usbh_cdc_acm_claim_ctrl(u8 state, u16 value, const usb_cdc_acm_line_coding_t *lc)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	int ret = HAL_BUSY;

	usb_os_enter_critical();
	if (cdc->state == USBH_CDC_ACM_STATE_IDLE) {
		if (state == USBH_CDC_ACM_STATE_SET_CONTROL_LINE_STATE) {
			cdc->ctrl_line_state = value;
		} else if (state == USBH_CDC_ACM_STATE_SEND_BREAK) {
			cdc->break_duration = value;
		} else if (lc != NULL) {
			cdc->user_line_coding->b.dwDteRate = lc->b.dwDteRate;
			cdc->user_line_coding->b.bCharFormat = lc->b.bCharFormat;
			cdc->user_line_coding->b.bParityType = lc->b.bParityType;
			cdc->user_line_coding->b.bDataBits = lc->b.bDataBits;
		} else {
			/* No other request is started by the API */
		}
		cdc->state = state;
		ret = HAL_OK;
	}
	usb_os_exit_critical();

	return ret;
}

#if !CONFIG_USBH_CDC_ACM_4G_DONGLE
/**
  * @brief  Find the data interface bound to a comm interface by its Union Functional Descriptor
  * @note   Ref CDC 1.2 5.2.3.2: bControlInterface names the comm interface and bSubordinateInterface0 its data
  *         interface. The bytes come from the device, so every descriptor is bounds-checked before it is read.
  * @param  itf_data: Comm interface data, raw_data covers its CS descriptors
  * @param  comm_itf_num: bInterfaceNumber of that comm interface
  * @retval bInterfaceNumber of the data interface, 0xFF when no matching Union is present
  */
static u8 usbh_cdc_acm_get_union_data_itf(const usbh_itf_data_t *itf_data, u8 comm_itf_num)
{
	const u8 *desc = itf_data->raw_data;
	u32 total = 0U;
	u8 data_itf_num = 0xFFU;
	u8 len;

	while ((desc != NULL) && ((total + USB_LEN_DESC_HEADER) <= itf_data->raw_data_len)) {
		len = desc[0];
		if ((len < USB_LEN_DESC_HEADER) || ((total + len) > itf_data->raw_data_len)) {
			/* bLength is the only way to the next descriptor, so the rest can not be framed */
			break;
		}
		if ((desc[1] == USB_CDC_CS_INTERFACE) && (len >= USB_CDC_UNION_FUNC_DESC_MIN_SIZE) &&
			(desc[2] == USB_CDC_FUNC_DESC_UNION) && (desc[3] == comm_itf_num)) {
			data_itf_num = desc[4];
			break;
		}
		desc += len;
		total += len;
	}

	return data_itf_num;
}
#endif

#if CONFIG_USBH_CDC_ACM_4G_DONGLE

/**
  * @brief  Look up the 4G-dongle parameter entry that matches the device VID/PID.
  */
static const usbh_cdc_acm_param_t *usbh_cdc_acm_find_param(const usbh_cdc_acm_param_t *list, u16 vid, u16 pid)
{
	const usbh_cdc_acm_param_t *item = list;

	while ((item != NULL) && (item->vid != 0U)) {
		if ((item->vid == vid) && (item->pid == pid)) {
			return item;
		}
		item++;
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
  * @brief  Open the BULK in/out pipes of a CDC data interface descriptor
  * @param  host: Host handle
  * @param  itf_desc: Data interface descriptor
  * @retval HAL_OK when both BULK pipes are open, HAL_ERR_PARA otherwise
  */
static int usbh_cdc_acm_open_data_eps(usb_host_t *host, usbh_itf_desc_t *itf_desc)
{
	u8 i;

	for (i = 0U; i < itf_desc->bNumEndpoints; i++) {
		if (usbh_cdc_acm_open_bulk_ep(host, &itf_desc->ep_desc_array[i]) != HAL_OK) {
			return HAL_ERR_PARA;
		}
	}

	return usbh_cdc_acm_check_bulk_pipes();
}

/**
  * @brief  Open all endpoints (BULK in/out + INTR in) carried by a Quectel-style
  *         vendor-specific interface.
  * @param  host: Host handle
  * @param  cdc: Class handle
  * @param  itf_desc: Vendor interface descriptor
  * @retval HAL_OK when both BULK pipes are open, HAL_ERR_PARA otherwise
  */
static int usbh_cdc_acm_open_quectel_eps(usb_host_t *host, usbh_cdc_acm_host_t *cdc, usbh_itf_desc_t *itf_desc)
{
	usbh_ep_desc_t *ep_desc;
	u8 i;

	for (i = 0U; i < itf_desc->bNumEndpoints; i++) {
		ep_desc = &itf_desc->ep_desc_array[i];

		if ((ep_desc->bmAttributes & USB_EP_XFER_TYPE_MASK) == USB_CH_EP_TYPE_INTR) {
#ifdef CONFIG_USBH_CDC_ACM_NOTIFY
			/* Only open INTR IN when notification support is enabled; otherwise
			 * the pipe would be opened but never polled or closed (leaked),
			 * because notify_process() / SOF / detach() paths are compiled out. */
			if (USB_EP_IS_IN(ep_desc->bEndpointAddress) && (cdc->intr_in.pipe_num == 0U)) {
				if (usbh_open_pipe(host, &cdc->intr_in, ep_desc, &usbh_cdc_acm_driver) != HAL_OK) {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "Open intr in pipe fail\n");
					return HAL_ERR_PARA;
				}
				cdc->intr_in.max_timeout_tick = USBH_CDC_ACM_INTR_MAX_TIMEOUT_TICK;
			}
#else
			UNUSED(cdc);
#endif
		} else if (usbh_cdc_acm_open_bulk_ep(host, ep_desc) != HAL_OK) {
			return HAL_ERR_PARA;
		} else {
			/* BULK opened, or any other type skipped */
		}
	}

	return usbh_cdc_acm_check_bulk_pipes();
}

/**
  * @brief  Attach handler for the standard CDC-ACM 4G topology: comm interface
  *         (param at_line_idx) carries INTR, data interface (comm+1) carries BULK.
  */
static int usbh_cdc_acm_attach_acm_4g(usb_host_t *host, usbh_cdc_acm_host_t *cdc)
{
#ifdef CONFIG_USBH_CDC_ACM_NOTIFY
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
#ifdef CONFIG_USBH_CDC_ACM_NOTIFY
	/* Open INTR IN only when notification support is enabled; otherwise
	 * the pipe would be opened but never polled or closed (leaked),
	 * because notify_process() / SOF / detach() paths are compiled out. */
	ep_desc = &itf_desc->ep_desc_array[0];
	if (USB_EP_IS_IN(ep_desc->bEndpointAddress) && ((ep_desc->bmAttributes & USB_EP_XFER_TYPE_MASK) == USB_CH_EP_TYPE_INTR)) {
		if (usbh_open_pipe(host, intr_in, ep_desc, &usbh_cdc_acm_driver) != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Open intr in pipe fail\n");
			return HAL_ERR_PARA;
		}
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

	return usbh_cdc_acm_open_data_eps(host, itf_desc);
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

	return usbh_cdc_acm_open_quectel_eps(host, cdc, itf_desc);
}

/**
  * @brief  Run the 4G-dongle control-setting sequence: SET_INTERFACE (data alt),
  *         SET_LINE_CODING, GET_LINE_CODING. Driven from setup() until HAL_OK.
  * @note   A refused SET_INTERFACE of a non-zero alternate setting is terminal: the BULK endpoints live in that
  *         setting and stay inactive without it. Alternate setting 0 is the one already selected by
  *         SET_CONFIGURATION, and an interface with no other setting may STALL the request (Ref USB 2.0
  *         9.4.10), so it is no error. A refused SET/GET_LINE_CODING is optional for an AT port and leaves the
  *         default line coding, as in usbh_cdc_acm_setup().
  * @param  host: Host handle
  * @retval HAL_OK when done, HAL_BUSY while running, HAL_ERR_PARA when the data interface can not be selected
  */
static int usbh_cdc_acm_ctrl_setting(usb_host_t *host)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	/* HAL_BUSY = sequence still running. Every state below either advances
	 * sub_status or leaves the control transfer in flight, so "not finished"
	 * must be reported as BUSY: the core treats any other non-OK status as a
	 * terminal failure and drops the class. */
	int ret_state = HAL_BUSY;
	int state = HAL_OK;

	switch (cdc->sub_status) {
	case USBH_CDC_ACM_CTRL_ALT_SETTING:
		state = usbh_ctrl_set_interface(host, cdc->data_itf_num, cdc->data_itf_alt);
		if ((state == HAL_OK) || ((state != HAL_BUSY) && (cdc->data_itf_alt == 0U))) {
			cdc->sub_status = USBH_CDC_ACM_CTRL_SET_LINE_CODING;
		} else if (state != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Set itf %d alt %d err %d\n", cdc->data_itf_num, cdc->data_itf_alt, state);
			cdc->sub_status = USBH_CDC_ACM_CTRL_IDLE;
			ret_state = HAL_ERR_PARA;
		} else {
			/* Control transfer still in flight */
		}
		break;

	case USBH_CDC_ACM_CTRL_SET_LINE_CODING:
		state = usbh_cdc_acm_process_set_line_coding(host, cdc->user_line_coding);
		if (state == HAL_OK) {
			cdc->sub_status = USBH_CDC_ACM_CTRL_GET_LINE_CODING;
		} else if (state != HAL_BUSY) {
			/* GET_LINE_CODING may still report what the device runs with */
			RTK_LOGS(TAG, RTK_LOG_WARN, "Set line coding err %d\n", state);
			cdc->sub_status = USBH_CDC_ACM_CTRL_GET_LINE_CODING;
		} else {
			/* Control transfer still in flight */
		}
		break;

	case USBH_CDC_ACM_CTRL_GET_LINE_CODING:
		state = usbh_cdc_acm_process_get_line_coding(host, cdc->line_coding);
		if (state != HAL_BUSY) {
			if (state != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_WARN, "Get line coding err %d, use default\n", state);
				usbh_cdc_acm_set_default_line_coding();
			} else if ((cdc->cb != NULL) && (cdc->cb->line_coding_changed != NULL)) {
				cdc->cb->line_coding_changed(cdc->line_coding);
			} else {
				/* No user to tell */
			}
			cdc->sub_status = USBH_CDC_ACM_CTRL_IDLE;
			ret_state = HAL_OK;
		}
		break;

	case USBH_CDC_ACM_CTRL_IDLE:
	default:
		ret_state = HAL_OK;
		break;
	}

	if (ret_state == HAL_BUSY) {
		usbh_notify(host, 0U, &usbh_cdc_acm_driver);
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

	cdc->param_item = usbh_cdc_acm_find_param(cdc->cfg->dongle_param, vid, pid);
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
		/* Roll back any pipe the 4G attach already opened. Otherwise a partial attach
		 * leaks them until physical unplug, since the core only calls detach() for
		 * attached slots. */
		usbh_cdc_acm_close_pipes(host);
		return ret;
	}
	cdc->sub_status = USBH_CDC_ACM_CTRL_ALT_SETTING;
	UNUSED(status);
#else
	usbh_itf_data_t *itf_data;
	usbh_itf_desc_t *comm_itf_desc;
	usbh_itf_desc_t *data_itf_desc;
	usbh_dev_id_t dev_id = {0,};
	usbh_itf_data_t *node;
	u8 comm_itf_num;
	u8 union_itf_num;
	u8 i;
#ifdef CONFIG_USBH_CDC_ACM_NOTIFY
	usbh_ep_desc_t *ep_desc;
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
	 * dongle_param.at_line_idx to select a specific ACM comm interface. */
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
	union_itf_num = usbh_cdc_acm_get_union_data_itf(itf_data, comm_itf_num);

	/* Communication interface: already claimed by the core's Phase 1 match
	 * (id_table matches class 0x02 + subclass ACM), no need to re-claim. */

#ifdef CONFIG_USBH_CDC_ACM_NOTIFY
	/* The comm interface may legally carry 0 endpoints (no notify INTR EP). */
	if (comm_itf_desc->bNumEndpoints >= 1U) {
		ep_desc = &comm_itf_desc->ep_desc_array[0];
		if (USB_EP_IS_IN(ep_desc->bEndpointAddress) &&
			((ep_desc->bmAttributes & USB_EP_XFER_TYPE_MASK) == USB_CH_EP_TYPE_INTR)) {
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
		status = HAL_ERR_PARA;
		goto open_fail;
	}

	/* In a composite device several CDC_DATA interfaces may exist (e.g. ACM
	 * data + ECM data), and taking a foreign one would open another function's
	 * endpoints. The Union Functional Descriptor names this function's data
	 * interface exactly (Ref CDC 1.2 5.2.3.2), so it wins. Without one, use the
	 * standard layout (comm_itf_num + 1), and only as a last resort a data
	 * interface that is the sole one on the device, where no other function can
	 * own it. Anything else is ambiguous and fails the attach. */
	if (union_itf_num == 0xFFU) {
		union_itf_num = (u8)(comm_itf_num + 1U);
	}
	data_itf_desc = NULL;
	for (node = itf_data; node != NULL; node = node->next) {
		if ((node->itf_desc_array != NULL) && (node->itf_desc_array[0].bInterfaceNumber == union_itf_num)) {
			data_itf_desc = node->itf_desc_array;
			break;
		}
	}
	if ((data_itf_desc == NULL) && (itf_data->next == NULL)) {
		data_itf_desc = itf_data->itf_desc_array;
	}
	if (data_itf_desc == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "No data itf %d\n", union_itf_num);
		status = HAL_ERR_PARA;
		goto open_fail;
	}

	/* Data endpoints of the default setting (Ref CDC 1.2 3.3.2): only BULK ones, one per direction */
	for (i = 0U; i < data_itf_desc->bNumEndpoints; i++) {
		if (usbh_cdc_acm_open_bulk_ep(host, &data_itf_desc->ep_desc_array[i]) != HAL_OK) {
			status = HAL_ERR_PARA;
			goto open_fail;
		}
	}

	/* Both bulk pipes are mandatory for CDC ACM data transfer */
	status = usbh_cdc_acm_check_bulk_pipes();
	if (status != HAL_OK) {
		goto open_fail;
	}
#endif

	/* Common tail: shared by the 4G-dongle and the standard CDC-ACM paths. */
	cdc->host = host;
	cdc->state = USBH_CDC_ACM_STATE_IDLE;
	cdc->bulk_out_busy = 0U;
	cdc->bulk_in_busy = 0U;
	cdc->intr_in_busy = 0U;
	cdc->line_coding_valid = 0U;

#ifdef CONFIG_USBH_CDC_ACM_NOTIFY
	/* A legal MPS below the 10-byte SERIAL_STATE splits it over two packets (Ref USB 2.0 5.7.3), but a notify
	   transfer receives one packet, so such a notification is reported as notify(NULL, 0).
	   FIXME: when ep_mps < USB_CDC_ACM_INTR_IN_PACKET_SIZE, reassemble the notification in the class: store each
	   packet, re-arm the pipe for the next one, and deliver once the header plus wLength bytes, or a short packet,
	   have arrived. */
	if ((cdc->intr_in.pipe_num != 0U) && (cdc->intr_in.ep_mps < USB_CDC_ACM_INTR_IN_PACKET_SIZE)) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "INTR MPS %d < %d, SERIAL_STATE lost\n", cdc->intr_in.ep_mps, USB_CDC_ACM_INTR_IN_PACKET_SIZE);
	}
#endif

	usbh_cdc_acm_dump_desc();

	if ((cdc->cb != NULL) && (cdc->cb->attached != NULL)) {
		cdc->cb->attached();
	}

	return HAL_OK;

#if !CONFIG_USBH_CDC_ACM_4G_DONGLE
open_fail:
	/* Roll back any pipe already opened */
	usbh_cdc_acm_close_pipes(host);
	return status;
#endif
}

/**
  * @brief  Detach callback.
  * @param  host: Host handle
  * @retval None
  */
static void usbh_cdc_acm_detach(usb_host_t *host)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;

	if ((cdc->cb != NULL) && (cdc->cb->detached != NULL)) {
		cdc->cb->detached();
	}

	usbh_cdc_acm_close_pipes(host);

	/* Device gone: drop the stale host handle and reset state so post-detach
	   transmit/receive calls are rejected by their host/connect_state checks. */
	cdc->host = NULL;
	cdc->state = USBH_CDC_ACM_STATE_IDLE;
	cdc->bulk_out_busy = 0U;
	cdc->bulk_in_busy = 0U;
	cdc->intr_in_busy = 0U;
	cdc->line_coding_valid = 0U;
	cdc->comm_itf_num = 0U;
	cdc->ctrl_line_state = 0U;
	cdc->break_duration = 0U;
#if CONFIG_USBH_CDC_ACM_4G_DONGLE
	cdc->param_item = NULL;
	cdc->sub_status = 0U;
#endif
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
			usbh_cdc_acm_set_line_coding_valid();
			if ((cdc->cb != NULL) && (cdc->cb->setup != NULL)) {
				cdc->cb->setup();
			}
		}
		return status;
	}
#endif

	/*Issue the get line coding request*/
	status = usbh_cdc_acm_process_get_line_coding(host, cdc->line_coding);
	if (status == HAL_BUSY) {
		/* Control transfer still running; the core re-invokes setup(). */
		return status;
	}

	if (status != HAL_OK) {
		/* GET_LINE_CODING is only required of a device that actually manages a
		 * serial line (CDC PSTN 1.2 6.3.11); ACM-like interfaces commonly STALL
		 * it. Retrying cannot make an unsupported request supported, so keep the
		 * defaults already in line_coding and report the class ready - the same
		 * "optional request refused means use a default" handling that MSC
		 * applies to GET_MAX_LUN. */
		RTK_LOGS(TAG, RTK_LOG_WARN, "Get line coding err %d, use default\n", status);
		usbh_cdc_acm_set_default_line_coding();
	}

	usbh_cdc_acm_set_line_coding_valid();

	if ((cdc->cb != NULL) && (cdc->cb->setup != NULL)) {
		cdc->cb->setup();
	}

	return HAL_OK;
}

/**
  * @brief  Check whether a BULK pipe has been busy longer than its timeout
  * @note   ISR context, read only. A NAKed BULK channel masks its NAK interrupt after the first one, so
  *         nothing else wakes the host task to run the timeout and NAK retry of usbh_transfer_process().
  *         Only a NAKed transfer (URB_BUSY) is re-driven: one that has seen no NAK is still moving data and is
  *         reported by its own completion.
  * @param  host: Host handle
  * @param  pipe: BULK pipe
  * @retval 1 when the owner shall be re-driven, 0 otherwise
  */
static u8 usbh_cdc_acm_bulk_due(usb_host_t *host, usbh_pipe_t *pipe)
{
	if ((pipe->pipe_num != 0U) && (pipe->xfer_state == USBH_EP_XFER_BUSY) &&
		(usbh_get_elapsed_ticks(host, pipe->tick) > pipe->max_timeout_tick) &&
		(usbh_get_urb_state(host, pipe) == USBH_URB_BUSY)) {
		return 1U;
	}

	return 0U;
}

#ifdef CONFIG_USBH_CDC_ACM_NOTIFY
/**
  * @brief  Check whether the polling interval of an unanswered INTR IN transfer has elapsed
  * @note   Read only, so the SOF ISR and the host task may both call it.
  * @param  host: Host handle
  * @param  pipe: INTR IN pipe
  * @retval 1 when the transfer shall be re-armed, 0 otherwise
  */
static u8 usbh_cdc_acm_intr_due(usb_host_t *host, usbh_pipe_t *pipe)
{
	usbh_urb_state_t urb_state;

	if ((pipe->pipe_num == 0U) || (pipe->xfer_state != USBH_EP_XFER_BUSY)) {
		return 0U;
	}

	/* A DONE transfer is left to process_intr_rx() */
	urb_state = usbh_get_urb_state(host, pipe);
	if ((urb_state != USBH_URB_BUSY) && (urb_state != USBH_URB_IDLE)) {
		return 0U;
	}

	return ((usbh_get_elapsed_frame_cnt(host, pipe->frame_num) + 1U) >= pipe->ep_interval) ? 1U : 0U;
}
#endif

/**
  * @brief  SOF callback for CDC ACM. Runs in ISR context every microframe;
  *         time-consuming operations (e.g. `malloc`, `rtos_sema_take`) are not permitted.
  * @note   Wakeup only: the pipes are written by the host task alone, which may run on another core, so this
  *         reads their state and notifies the owner. The INTR IN is re-armed once its polling interval has
  *         elapsed, a BULK pipe re-driven once it has been busy past its timeout.
  * @param  host: Host handle
  * @retval None
  */
static void usbh_cdc_acm_sof(usb_host_t *host)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;

#ifdef CONFIG_USBH_CDC_ACM_NOTIFY
	if (usbh_cdc_acm_intr_due(host, &cdc->intr_in) != 0U) {
		usbh_notify(host, cdc->intr_in.pipe_num, &usbh_cdc_acm_driver);
	}
#endif
	if (usbh_cdc_acm_bulk_due(host, &cdc->bulk_in) != 0U) {
		usbh_notify(host, cdc->bulk_in.pipe_num, &usbh_cdc_acm_driver);
	}
	if (usbh_cdc_acm_bulk_due(host, &cdc->bulk_out) != 0U) {
		usbh_notify(host, cdc->bulk_out.pipe_num, &usbh_cdc_acm_driver);
	}
}

#ifdef CONFIG_USBH_CDC_ACM_NOTIFY
/**
  * @brief  Transfer completion callback: handle the end of an interrupt IN transfer,
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param[in] host: Pointer to the USB host handle.
  * @param[in] pipe_num: Pipe number of the completed transfer.
  * @retval None
  */
static void usbh_cdc_acm_completed(usb_host_t *host, u8 pipe_num)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usbh_pipe_t *intr_in = &cdc->intr_in;

	/* The core routes a completion to the pipe's owning driver only, so the
	 * pipe check is a defensive guard against a stale/closed INTR pipe. */
	if ((intr_in->pipe_num != 0U) && (pipe_num == intr_in->pipe_num)) {
		if (intr_in->xfer_state == USBH_EP_XFER_BUSY) {
			usbh_notify(host, intr_in->pipe_num, &usbh_cdc_acm_driver);
		}
	}
}
#endif

/**
* @brief  State machine handling callback
* @param  host:Host handle
* @param  msg: USB host driver message
* @retval None
*/
static void usbh_cdc_acm_process(usb_host_t *host, usbh_drv_msg_t *msg)
{
	int req_status = HAL_OK;
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;

	/* A data pipe message is served whatever EP0 is doing: the pipes are independent of
	   the control pipe (Ref USB 2.0 5.3.2), and dropping one here would leave that pipe
	   busy forever. Pipe 0 is EP0, which carries the class request state machine below. */
	if ((msg != NULL) && (msg->pipe_num != 0U)) {
		if (msg->pipe_num == cdc->bulk_out.pipe_num) {
			usbh_cdc_acm_process_tx(host);
		} else if (msg->pipe_num == cdc->bulk_in.pipe_num) {
			usbh_cdc_acm_process_rx(host);
#ifdef CONFIG_USBH_CDC_ACM_NOTIFY
		} else if (msg->pipe_num == cdc->intr_in.pipe_num) {
			usbh_cdc_acm_process_intr_rx(host);
#endif
		} else {
			/* Not one of this class's pipes */
		}
		return;
	}

	switch (cdc->state) {

	case USBH_CDC_ACM_STATE_IDLE:
		break;

	case USBH_CDC_ACM_STATE_SET_CONTROL_LINE_STATE:
		req_status = usbh_cdc_acm_process_set_control_line_state(host);
		if (req_status == HAL_OK) {
			usbh_cdc_acm_release(&cdc->state);
		} else if (req_status != HAL_BUSY) {
			cdc->state = USBH_CDC_ACM_STATE_ERROR;
		}
		usbh_notify(host, 0, &usbh_cdc_acm_driver);
		break;

	case USBH_CDC_ACM_STATE_SEND_BREAK:
		req_status = usbh_cdc_acm_process_send_break(host);
		if (req_status == HAL_OK) {
			usbh_cdc_acm_release(&cdc->state);
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
			/* Report what the device now runs with, which it may have adjusted from the request (Ref PSTN 1.2
			   6.3.11), so the user always learns the line coding actually in effect */
			usbh_cdc_acm_release(&cdc->state);
			if ((cdc->cb != NULL) && (cdc->cb->line_coding_changed != NULL)) {
				cdc->cb->line_coding_changed(cdc->line_coding);
			}
		} else if (req_status != HAL_BUSY) {
			cdc->state = USBH_CDC_ACM_STATE_ERROR;
		}
		usbh_notify(host, 0, &usbh_cdc_acm_driver);
		break;

	case USBH_CDC_ACM_STATE_ERROR:
		/* EP0 needs no recovery request: a control pipe clears a protocol STALL by accepting
		 * the next SETUP (USB 2.0 §8.5.3.4) and has no ENDPOINT_HALT feature to clear
		 * (§9.4.5). The failed request above already released EP0 in the host core, so
		 * returning to IDLE is the whole recovery and always completes in one pass. */
		RTK_LOGS(TAG, RTK_LOG_ERROR, "ACM ctrl err\n");
		usbh_cdc_acm_release(&cdc->state);
		break;

	default:
		break;

	}
}

/**
  * @brief  Get line coding
  * @param  host: Host handle
  * @param  linecoding: Line coding data pointer
  * @retval Status
  */
static int usbh_cdc_acm_process_get_line_coding(usb_host_t *host, usb_cdc_acm_line_coding_t *linecoding)
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
static int usbh_cdc_acm_process_set_line_coding(usb_host_t *host, usb_cdc_acm_line_coding_t *linecoding)
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
	u8 done = 0U;

	if ((status == HAL_OK) && (bulk_out->xfer_state == USBH_EP_XFER_IDLE)) {
		done = 1U;
	} else if (bulk_out->xfer_state == USBH_EP_XFER_START) {
		usbh_notify(host, bulk_out->pipe_num, &usbh_cdc_acm_driver);
	} else if (bulk_out->xfer_state == USBH_EP_XFER_ERROR) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "BULK TX fail: %d\n", usbh_get_urb_state(host, bulk_out));
		bulk_out->xfer_state = USBH_EP_XFER_IDLE;
		done = 1U;
	} else {
		/* Still in flight */
	}

	if (done != 0U) {
		/* Release before the callback so the user can submit the next transfer from it */
		usbh_cdc_acm_release(&cdc->bulk_out_busy);
		if ((cdc->cb != NULL) && (cdc->cb->transmitted != NULL)) {
			cdc->cb->transmitted((u8)status);
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
	/* Latched before the pipe can turn idle: once released, another caller may re-arm xfer_buf */
	u8 *buf = bulk_in->xfer_buf;
	u32 len;
	int status;

	status = usbh_transfer_process(host, bulk_in);

	if ((status == HAL_OK) && (bulk_in->xfer_state == USBH_EP_XFER_IDLE)) {
		len = usbh_get_last_transfer_size(host, bulk_in);
		usbh_cdc_acm_release(&cdc->bulk_in_busy);
		if ((cdc->cb != NULL) && (cdc->cb->received != NULL)) {
			cdc->cb->received(buf, len, (u8)status);
		}
	} else if (bulk_in->xfer_state == USBH_EP_XFER_START) {
		/* xfer_len 0 means N*MPS bytes landed and the trailing ZLP is being fetched: report the
		   data now, the pipe stays owned until the ZLP completes */
		len = usbh_get_last_transfer_size(host, bulk_in);
		if ((bulk_in->xfer_len == 0U) && (cdc->cb != NULL) && (cdc->cb->received != NULL)) {
			cdc->cb->received(buf, len, (u8)status);
		}
		usbh_notify(host, bulk_in->pipe_num, &usbh_cdc_acm_driver);
	} else if (bulk_in->xfer_state == USBH_EP_XFER_ERROR) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "BULK RX fail: %d\n", usbh_get_urb_state(host, bulk_in));
		bulk_in->xfer_state = USBH_EP_XFER_IDLE;
		usbh_cdc_acm_release(&cdc->bulk_in_busy);
		if ((cdc->cb != NULL) && (cdc->cb->received != NULL)) {
			cdc->cb->received(NULL, 0U, (u8)status);
		}
	} else {
		/* Still in flight */
	}
}

#ifdef CONFIG_USBH_CDC_ACM_NOTIFY
/**
  * @brief  Deliver one received notification to the user
  * @note   The callback carries no notification code, so only SERIAL_STATE, the one notification an ACM
  *         device sends with data (Ref PSTN 1.2 6.5.4), hands its payload over. Every other code, and a
  *         packet too short to hold the 8-byte header (Ref CDC 1.2 6.3), is reported as notify(NULL, 0)
  *         so the user still re-arms the pipe but never parses a payload of another layout.
  * @param  buf: Received data
  * @param  len: Received length
  * @param  status: Transfer status
  * @retval None
  */
static void usbh_cdc_acm_report_notify(u8 *buf, u32 len, u8 status)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	u32 data_len;

	if ((len >= USB_CDC_NOTIFY_HDR_LEN) && (buf[0] == (USB_D2H | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE)) &&
		(buf[1] == USB_CDC_ACM_NOTIFY_SERIAL_STATE)) {
		/* SERIAL_STATE always carries the 2-byte UART state bitmap, any other wLength is malformed */
		data_len = (u32)buf[6] | ((u32)buf[7] << 8U);
		if ((data_len == USB_CDC_ACM_INTR_IN_DATA_SIZE) && (len >= (USB_CDC_NOTIFY_HDR_LEN + data_len))) {
			cdc->cb->notify(&buf[USB_CDC_NOTIFY_HDR_LEN], data_len, status);
			return;
		}
	}

	cdc->cb->notify(NULL, 0U, status);
}

/**
  * @brief  Intr in function
  * @param  host: Host handle
  * @retval None
  */
static void usbh_cdc_acm_process_intr_rx(usb_host_t *host)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usbh_pipe_t *intr_in = &cdc->intr_in;
	/* Latched before the pipe can turn idle, see usbh_cdc_acm_process_rx() */
	u8 *buf = intr_in->xfer_buf;
	u32 len;
	int status;

	/* An unanswered poll is re-armed here rather than in the SOF ISR, so the pipe has a single writer. A device
	   with nothing to report NAKs every poll, after USB_INTR_RETRY_MAX_CNT of them the user gets notify(NULL, 0)
	   and decides whether to re-arm. */
	if (usbh_cdc_acm_intr_due(host, intr_in) != 0U) {
		intr_in->retry_cnt++;
		if (intr_in->retry_cnt <= USB_INTR_RETRY_MAX_CNT) {
			intr_in->xfer_state = USBH_EP_XFER_START;
		} else {
			intr_in->retry_cnt = 0U;
			intr_in->xfer_state = USBH_EP_XFER_ERROR;
		}
	}

	status = usbh_transfer_process(host, intr_in);

	if ((status == HAL_OK) && (intr_in->xfer_state == USBH_EP_XFER_IDLE)) {
		len = usbh_get_last_transfer_size(host, intr_in);
		usbh_cdc_acm_release(&cdc->intr_in_busy);
		if ((cdc->cb != NULL) && (cdc->cb->notify != NULL)) {
			usbh_cdc_acm_report_notify(buf, len, (u8)status);
		}
	} else if (intr_in->xfer_state == USBH_EP_XFER_START) {
		usbh_notify(host, intr_in->pipe_num, &usbh_cdc_acm_driver);
	} else if (intr_in->xfer_state == USBH_EP_XFER_ERROR) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "INTR RX fail: %d\n", usbh_get_urb_state(host, intr_in));
		intr_in->xfer_state = USBH_EP_XFER_IDLE;
		usbh_cdc_acm_release(&cdc->intr_in_busy);
		if ((cdc->cb != NULL) && (cdc->cb->notify != NULL)) {
			cdc->cb->notify(NULL, 0U, (u8)status);
		}
	} else {
		/* Still in flight */
	}
}
#endif

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Init CDC ACM class
  * @param  cb: User callback
  * @param  cfg: Class configuration
  * @retval Status
  */
int usbh_cdc_acm_init(const usbh_cdc_acm_cb_t *cb, const usbh_cdc_acm_config_t *cfg)
{
	int ret = HAL_OK;
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;

	if (cb == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid user CB\n");
		return HAL_ERR_PARA;
	}

	if (cfg == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid cfg\n");
		return HAL_ERR_PARA;
	}

	/* line_coding is only allocated by a successful init and freed by deinit: a second init would leak it and
	   register the driver twice */
	if (cdc->line_coding != NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Already init\n");
		return HAL_ERR_PARA;
	}

	/* The line coding is what the device is actually programmed with, so an illegal
	   combination has to be rejected here rather than being sent on the wire
	   (Ref PSTN 1.2 Table 17). */
	if (usbh_cdc_acm_check_line_coding((cfg->data_bits != 0U) ? cfg->data_bits : USBH_CDC_ACM_DEFAULT_DATA_BITS,
									   cfg->char_format, cfg->parity_type) != HAL_OK) {
		return HAL_ERR_PARA;
	}

	if ((cfg->ctrl_line_state & ~USBH_CDC_ACM_CTRL_LINE_STATE_MASK) != 0U) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid ctrl line state %02x\n", cfg->ctrl_line_state);
		return HAL_ERR_PARA;
	}

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "Config:\n");
	RTK_LOGS(TAG, RTK_LOG_DEBUG, "* dte_rate: %d\n", cfg->dte_rate);
	RTK_LOGS(TAG, RTK_LOG_DEBUG, "* data_bits: %d\n", cfg->data_bits);
	RTK_LOGS(TAG, RTK_LOG_DEBUG, "* char_format: %d\n", cfg->char_format);
	RTK_LOGS(TAG, RTK_LOG_DEBUG, "* parity_type: %d\n", cfg->parity_type);
	RTK_LOGS(TAG, RTK_LOG_DEBUG, "* ctrl_line_state: %02x\n", cfg->ctrl_line_state);

	/* Published before anything that reads it: attach() resolves the 4G dongle table
	   through cdc->cfg. */
	cdc->cfg = cfg;

	if (cb->init != NULL) {
		ret = cb->init();
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "User init err %d\n", ret);
			return ret;
		}
	}

	/* GET_LINE_CODING returns 7 bytes, but its data stage reserves a whole EP0 packet */
	cdc->line_coding = (usb_cdc_acm_line_coding_t *)usb_os_malloc(USB_MAX_EP0_SIZE);
	if (cdc->line_coding == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Malloc line code fail\n");
		ret = HAL_ERR_MEM;
		goto init_fail;
	}
	cdc->user_line_coding = (usb_cdc_acm_line_coding_t *)usb_os_malloc(sizeof(usb_cdc_acm_line_coding_t));
	if (cdc->user_line_coding == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Malloc user line code fail\n");
		ret = HAL_ERR_MEM;
		goto init_fail;
	}

	/* init user_line_coding from the configuration; char_format/parity_type are used as
	   given because 0 is a valid encoding for both. */
	cdc->user_line_coding->b.dwDteRate = (cfg->dte_rate != 0U) ? cfg->dte_rate : USBH_CDC_ACM_DEFAULT_DTE_RATE;
	cdc->user_line_coding->b.bCharFormat = cfg->char_format;
	cdc->user_line_coding->b.bParityType = cfg->parity_type;
	cdc->user_line_coding->b.bDataBits = (cfg->data_bits != 0U) ? cfg->data_bits : USBH_CDC_ACM_DEFAULT_DATA_BITS;

	cdc->ctrl_line_state = cfg->ctrl_line_state;
	cdc->cb = cb;

#if CONFIG_USBH_CDC_ACM_4G_DONGLE
	cdc->param_item = NULL;
#endif

	/* Last: once registered, the host task may attach and read everything set above */
	ret = usbh_register_class(&usbh_cdc_acm_driver);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Register class fail %d\n", ret);
		goto init_fail;
	}

	return HAL_OK;

init_fail:
	/* Undo in reverse order, user resources last as they were taken first */
	usb_os_mfree((void *)cdc->user_line_coding);
	cdc->user_line_coding = NULL;
	usb_os_mfree((void *)cdc->line_coding);
	cdc->line_coding = NULL;
	cdc->cb = NULL;
	cdc->cfg = NULL;
	if (cb->deinit != NULL) {
		cb->deinit();
	}
	return ret;
}

/**
  * @brief  Deinit CDC ACM class
  */
void usbh_cdc_acm_deinit(void)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usb_host_t *host = cdc->host;

	/* First, before anything is freed: on HAL_OK no attach/setup/process/detach of this
	 * class runs any more, an EP0 request it left in flight is halted and a sof() still
	 * running on another core is waited out. Anything else means nothing was released. */
	if (usbh_unregister_class(&usbh_cdc_acm_driver) != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Deinit refused\n");
		return;
	}

	if ((cdc->cb != NULL) && (cdc->cb->deinit != NULL)) {
		cdc->cb->deinit();
	}

	if (host != NULL) {
		cdc->state = USBH_CDC_ACM_STATE_IDLE;
		cdc->bulk_out_busy = 0U;
		cdc->bulk_in_busy = 0U;
		cdc->intr_in_busy = 0U;
		cdc->line_coding_valid = 0U;
		usbh_cdc_acm_close_pipes(host);
	}

	usb_os_mfree((void *)cdc->line_coding);
	cdc->line_coding = NULL;
	usb_os_mfree((void *)cdc->user_line_coding);
	cdc->user_line_coding = NULL;

	/* Cover the deinit-while-connected path (no detach ran): drop the host
	   handle so post-deinit transmit/receive calls are rejected. The detach
	   path already cleared it. */
	cdc->host = NULL;
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

	if ((host == NULL) || ((bitmap & ~USBH_CDC_ACM_CTRL_LINE_STATE_MASK) != 0U)) {
		return HAL_ERR_PARA;
	}

	if (host->connect_state == USBH_STATE_SETUP) {
		ret = usbh_cdc_acm_claim_ctrl(USBH_CDC_ACM_STATE_SET_CONTROL_LINE_STATE, bitmap, NULL);
		if (ret == HAL_OK) {
			usbh_notify(host, 0U, &usbh_cdc_acm_driver);
		}
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
		ret = usbh_cdc_acm_claim_ctrl(USBH_CDC_ACM_STATE_SEND_BREAK, duration_ms, NULL);
		if (ret == HAL_OK) {
			usbh_notify(host, 0U, &usbh_cdc_acm_driver);
		}
	}

	return ret;
}

/**
* @brief  Set the line coding parameters (PSTN §6.3.10 SET_LINE_CODING)
* @param  line_coding: Line coding data to apply
* @retval HAL_OK on success, others on failure.
*/
int usbh_cdc_acm_set_line_coding(usb_cdc_acm_line_coding_t *line_coding)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usb_host_t *host = cdc->host;
	int ret = HAL_ERR_UNKNOWN;

	if ((host == NULL) || (line_coding == NULL)) {
		return HAL_ERR_PARA;
	}

	/* Rejected here as in init, rather than sent and STALLed after this call already returned */
	if (usbh_cdc_acm_check_line_coding(line_coding->b.bDataBits, line_coding->b.bCharFormat, line_coding->b.bParityType) != HAL_OK) {
		return HAL_ERR_PARA;
	}

	if (host->connect_state == USBH_STATE_SETUP) {
		ret = usbh_cdc_acm_claim_ctrl(USBH_CDC_ACM_STATE_SET_LINE_CODING, 0U, line_coding);
		if (ret == HAL_OK) {
			usbh_notify(host, 0U, &usbh_cdc_acm_driver);
		}
	}

	return ret;
}

/**
* @brief  Get the current line coding parameters (PSTN §6.3.11 GET_LINE_CODING)
* @param  line_coding: Pointer to store the retrieved line coding data
* @retval HAL_OK on success, others on failure.
*/
int usbh_cdc_acm_get_line_coding(usb_cdc_acm_line_coding_t *line_coding)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usb_host_t *host = cdc->host;
	int ret = HAL_ERR_UNKNOWN;

	if ((host == NULL) || (line_coding == NULL)) {
		return HAL_ERR_PARA;
	}

	if ((host->connect_state == USBH_STATE_SETUP) || (host->connect_state == USBH_STATE_ATTACH)) {
		usb_cdc_acm_line_coding_t *lc = cdc->line_coding;
		usb_cdc_acm_line_coding_t *ulc = line_coding;
		/* line_coding is the DMA target of SET/GET_LINE_CODING: a copy taken while that
		   pair runs could mix old and new bytes. Until setup() has filled it, during ATTACH, it holds nothing. */
		usb_os_enter_critical();
		if ((cdc->line_coding_valid == 0U) || (cdc->state == USBH_CDC_ACM_STATE_SET_LINE_CODING) ||
			(cdc->state == USBH_CDC_ACM_STATE_GET_LINE_CODING)) {
			ret = HAL_BUSY;
		} else {
			ulc->b.dwDteRate = lc->b.dwDteRate;
			ulc->b.bCharFormat = lc->b.bCharFormat;
			ulc->b.bParityType = lc->b.bParityType;
			ulc->b.bDataBits = lc->b.bDataBits;
			ret = HAL_OK;
		}
		usb_os_exit_critical();
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

	/* The HAL would only fail the DMA later as an opaque transfer error */
	if (!USB_IS_MEM_DMA_ALIGNED(buf)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "TX buf unaligned\n");
		return HAL_ERR_PARA;
	}

	if (host->connect_state == USBH_STATE_SETUP) {
		ret = usbh_cdc_acm_claim_pipe(pipe, &cdc->bulk_out_busy, buf, len, len);
		if (ret == HAL_OK) {
			usbh_notify(host, pipe->pipe_num, &usbh_cdc_acm_driver);
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
	u32 xfer_len;

	if ((host == NULL) || (buf == NULL)) {
		return HAL_ERR_PARA;
	}

	if (!USB_IS_MEM_DMA_ALIGNED(buf)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "RX buf unaligned\n");
		return HAL_ERR_PARA;
	}

	if (host->connect_state == USBH_STATE_SETUP) {
		/* The controller receives whole packets, each adjusted to the DWORD boundary, so ask for
		   no more than the packets that fit in buf; the device keeps the rest for the next one */
		xfer_len = usb_get_max_payload_len(len, pipe->ep_mps);
		if (xfer_len == 0U) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "RX buf %u < mps %u\n", len, pipe->ep_mps);
			return HAL_ERR_PARA;
		}
		ret = usbh_cdc_acm_claim_pipe(pipe, &cdc->bulk_in_busy, buf, xfer_len, len);
		if (ret == HAL_OK) {
			usbh_notify(host, pipe->pipe_num, &usbh_cdc_acm_driver);
		}
	}

	return ret;
}

#ifdef CONFIG_USBH_CDC_ACM_NOTIFY
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
	u32 xfer_len;

	if ((host == NULL) || (buf == NULL)) {
		return HAL_ERR_PARA;
	}

	if (!USB_IS_MEM_DMA_ALIGNED(buf)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Notify buf unaligned\n");
		return HAL_ERR_PARA;
	}

	/* An unopened pipe has pipe_num == 0; arming it would target the control pipe. */
	if (pipe->pipe_num == 0U) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "No intr in pipe\n");
		return HAL_ERR_PARA;
	}

	if (host->connect_state == USBH_STATE_SETUP) {
		/* One notification fits in one MPS packet, and the controller may fill all of it */
		xfer_len = pipe->ep_mps;
		if (len < usb_get_dma_len(xfer_len, pipe->ep_mps)) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Notify buf %u < mps %u\n", len, xfer_len);
			return HAL_ERR_PARA;
		}
		ret = usbh_cdc_acm_claim_pipe(pipe, &cdc->intr_in_busy, buf, xfer_len, len);
		if (ret == HAL_OK) {
			usbh_notify(host, pipe->pipe_num, &usbh_cdc_acm_driver);
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
