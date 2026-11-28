/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usb_os.h"
#include "usbh_rndis.h"

/* Private defines -----------------------------------------------------------*/

/* RX window: one REMOTE_NDIS_PACKET_MSG with the largest frame plus the 1-byte pad a device may
 * append instead of a ZLP (Ref RNDIS USB 3.1), in whole HS packets. Reported to the device as the
 * MaxTransferSize of INITIALIZE, so it never sends a longer transfer. */
#define USBH_RNDIS_RX_BUF_SIZE                 ((USB_RNDIS_MAX_PACKET_MSG_SIZE + 1U + USB_BULK_HS_MAX_MPS - 1U) / USB_BULK_HS_MAX_MPS * USB_BULK_HS_MAX_MPS)

/* TX buffer: one REMOTE_NDIS_PACKET_MSG with the largest frame */
#define USBH_RNDIS_TX_BUF_SIZE                 USB_RNDIS_MAX_PACKET_MSG_SIZE

/* GET_ENCAPSULATED_RESPONSE polls of one command before the device is given up, 1 ms apart */
#define USBH_RNDIS_RESP_RETRY_MAX_CNT          200U

/* Pipe timeouts in ticks, a tick being a (micro)frame, the same budgets as CDC ECM */
#define USBH_RNDIS_BULK_OUT_IDLE_MAX_CNT       (8U * 50U)
#define USBH_RNDIS_BULK_OUT_BUSY_MAX_CNT       (8U * 10U)
#define USBH_RNDIS_BULK_IN_IDLE_MAX_CNT        (8U * 10U)
#define USBH_RNDIS_BULK_IN_NAK_CHECK_MAX_CNT   (8U * 5U)
#define USBH_RNDIS_INTR_BUSY_MAX_CNT           (8U * 5U)

/* CLEAR_FEATURE(ENDPOINT_HALT) attempts before the device is re-enumerated */
#define USBH_RNDIS_HALT_RETRY_MAX_CNT          3U

/* Private types -------------------------------------------------------------*/

/* RNDIS class state */
typedef enum {
	USBH_RNDIS_STATE_IDLE = 0U,
	USBH_RNDIS_STATE_XFER,
	USBH_RNDIS_STATE_ERROR,
} usbh_rndis_state_t;

/* Setup sequence: every command is a SEND_ENCAPSULATED_COMMAND followed by
 * GET_ENCAPSULATED_RESPONSE polls, so an even step sends and the odd one after it reads */
typedef enum {
	USBH_RNDIS_SETUP_SEND_INIT = 0U,
	USBH_RNDIS_SETUP_GET_INIT,
	USBH_RNDIS_SETUP_SEND_QUERY_MAC,
	USBH_RNDIS_SETUP_GET_QUERY_MAC,
	USBH_RNDIS_SETUP_SEND_SET_FILTER,
	USBH_RNDIS_SETUP_GET_SET_FILTER,
	USBH_RNDIS_SETUP_SEND_QUERY_MEDIA,
	USBH_RNDIS_SETUP_GET_QUERY_MEDIA,
	USBH_RNDIS_SETUP_DONE,
} usbh_rndis_setup_step_t;

/* Control step in the transfer state */
typedef enum {
	USBH_RNDIS_CTRL_IDLE = 0U,
	USBH_RNDIS_CTRL_GET_RESP,          /* Read the message RESPONSE_AVAILABLE announced */
	USBH_RNDIS_CTRL_SEND_KEEPALIVE,    /* Answer a device keepalive */
} usbh_rndis_ctrl_step_t;

/* Private macros ------------------------------------------------------------*/

#define USBH_RNDIS_FREE_MEM(x)  do { if ((x) != NULL) { usb_os_mfree((void *)(x)); (x) = NULL; } } while (0)

/* Private function prototypes -----------------------------------------------*/

static int usbh_rndis_attach(usb_host_t *host);
static void usbh_rndis_detach(usb_host_t *host);
static void usbh_rndis_process(usb_host_t *host, usbh_drv_msg_t *msg);
static int usbh_rndis_setup(usb_host_t *host);
static void usbh_rndis_sof(usb_host_t *host);
static void usbh_rndis_deinit_all_pipe(void);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "RNDISH";

/* Interface triplets of an RNDIS communication interface, Ref usb_rndis.h */
static const usbh_dev_id_t usbh_rndis_devs[] = {
	{
		.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO,
		.bInterfaceClass = USB_RNDIS_WIRELESS_CLASS_CODE,
		.bInterfaceSubClass = USB_RNDIS_WIRELESS_SUBCLASS,
		.bInterfaceProtocol = USB_RNDIS_WIRELESS_PROTOCOL,
	},
	{
		.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO,
		.bInterfaceClass = USB_RNDIS_MISC_CLASS_CODE,
		.bInterfaceSubClass = USB_RNDIS_MISC_SUBCLASS,
		.bInterfaceProtocol = USB_RNDIS_MISC_PROTOCOL,
	},
	{
		/* Linux gadgets and many phones: CDC ACM with the vendor protocol */
		.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO,
		.bInterfaceClass = USB_CDC_CLASS_CODE,
		.bInterfaceSubClass = USB_RNDIS_CDC_SUBCLASS_ACM,
		.bInterfaceProtocol = USB_RNDIS_CDC_PROTOCOL_VENDOR,
	},
	{
	},
};

/* USB Host RNDIS class driver */
static const usbh_class_driver_t usbh_rndis_driver = {
	.id_table = usbh_rndis_devs,
	.attach  = usbh_rndis_attach,
	.detach  = usbh_rndis_detach,
	.setup   = usbh_rndis_setup,
	.process = usbh_rndis_process,
	.sof = usbh_rndis_sof,
};

static usbh_rndis_host_t usbh_rndis_host;

/* Exported variables --------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Find the class-specific descriptor of one subtype in the communication interface
  * @note   raw_data comes from the device: every descriptor is bounds-checked before it is read.
  * @param  itf_data: Communication interface data
  * @param  subtype: bDescriptorSubtype to look for
  * @param  min_len: Smallest bLength the caller reads
  * @retval The descriptor, NULL if absent or truncated
  */
static const u8 *usbh_rndis_find_func_desc(const usbh_itf_t *itf_data, u8 subtype, u8 min_len)
{
	const u8 *end;
	const u8 *desc;

	if (itf_data->raw_data == NULL) {
		return NULL;
	}

	end = itf_data->raw_data + itf_data->raw_data_len;
	desc = usbh_desc_find(itf_data->raw_data, end, USB_CDC_CS_INTERFACE, subtype);
	while ((desc != NULL) && (desc[0] < min_len)) {
		desc = usbh_desc_find(usbh_desc_next(desc, end), end, USB_CDC_CS_INTERFACE, subtype);
	}

	return desc;
}

/**
  * @brief  Check that an interface setting carries exactly one BULK IN and one BULK OUT endpoint
  * @param  itf_desc: Interface descriptor (one alternate setting)
  * @param  bulk_in: Set to the BULK IN endpoint
  * @param  bulk_out: Set to the BULK OUT endpoint
  * @retval RTK_SUCCESS when both are present and no other endpoint is, -RTK_ERR_BADARG otherwise
  */
static int usbh_rndis_check_data_eps(usbh_itf_desc_t *itf_desc, usbh_ep_desc_t **bulk_in, usbh_ep_desc_t **bulk_out)
{
	usbh_ep_desc_t *ep;
	u8 i;

	*bulk_in = NULL;
	*bulk_out = NULL;

	if (itf_desc->bNumEndpoints != 2U) {
		return -RTK_ERR_BADARG;
	}

	for (i = 0U; i < itf_desc->bNumEndpoints; i++) {
		ep = &itf_desc->ep_desc_array[i];
		if ((ep->bmAttributes & USB_EP_XFER_TYPE_MASK) != USB_CH_EP_TYPE_BULK) {
			return -RTK_ERR_BADARG;
		}
		if (USB_EP_IS_IN(ep->bEndpointAddress)) {
			*bulk_in = ep;
		} else {
			*bulk_out = ep;
		}
	}

	return ((*bulk_in != NULL) && (*bulk_out != NULL)) ? RTK_SUCCESS : -RTK_ERR_BADARG;
}

/**
  * @brief  Locate the RNDIS function of the configuration
  * @note   Read only, shared by attach() and usbh_rndis_check_config(). Ref RNDIS USB 2.1: the
  *         communication interface carries at most one INTR IN endpoint, the data interface named
  *         by the Union (the next interface without one) carries the BULK pair.
  * @param  host: Host handle
  * @param  ctrl: Set to the communication interface data
  * @param  data: Set to the data interface setting with the BULK pair
  * @retval RTK_SUCCESS when the whole function is found, -RTK_ERR_BADARG otherwise
  */
static int usbh_rndis_find_function(usb_host_t *host, usbh_itf_t **ctrl, usbh_itf_desc_t **data)
{
	usbh_dev_id_t dev_id = {0,};
	usbh_itf_t *itf_data = NULL;
	usbh_itf_desc_t *itf_desc;
	usbh_ep_desc_t *bulk_in;
	usbh_ep_desc_t *bulk_out;
	const u8 *union_desc;
	u8 data_itf_num;
	u8 i;

	*data = NULL;

	for (i = 0U; (usbh_rndis_devs[i].mMatchFlags != 0U) && (itf_data == NULL); i++) {
		usb_os_memcpy((void *)&dev_id, (const void *)&usbh_rndis_devs[i], sizeof(dev_id));
		itf_data = usbh_itf_find(host, &dev_id);
	}
	if ((itf_data == NULL) || (itf_data->itf_desc_array == NULL)) {
		return -RTK_ERR_BADARG;
	}

	itf_desc = &itf_data->itf_desc_array[0];
	if ((itf_desc->bNumEndpoints > 1U) || ((itf_desc->bNumEndpoints == 1U) &&
										   (((itf_desc->ep_desc_array[0].bmAttributes & USB_EP_XFER_TYPE_MASK) != USB_CH_EP_TYPE_INTR) ||
											!USB_EP_IS_IN(itf_desc->ep_desc_array[0].bEndpointAddress)))) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Bad ctrl EP\n");
		return -RTK_ERR_BADARG;
	}
	*ctrl = itf_data;

	union_desc = usbh_rndis_find_func_desc(itf_data, USB_CDC_FUNC_DESC_UNION, USB_CDC_UNION_FUNC_DESC_MIN_SIZE);
	data_itf_num = (union_desc != NULL) ? union_desc[4] : (u8)(itf_desc->bInterfaceNumber + 1U);

	usb_os_memset((void *)&dev_id, 0, sizeof(dev_id));
	dev_id.bInterfaceClass = USB_CDC_DATA_INTERFACE_CLASS_CODE;
	dev_id.mMatchFlags = USBH_DEV_ID_MATCH_ITF_CLASS;
	itf_data = usbh_itf_find(host, &dev_id);
	while ((itf_data != NULL) && (itf_data->itf_desc_array != NULL) && (itf_data->itf_desc_array[0].bInterfaceNumber != data_itf_num)) {
		itf_data = itf_data->next;
	}
	if ((itf_data == NULL) || (itf_data->itf_desc_array == NULL)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "No data if %d\n", data_itf_num);
		return -RTK_ERR_BADARG;
	}

	for (i = 0U; i < itf_data->alt_setting_cnt; i++) {
		if (usbh_rndis_check_data_eps(&itf_data->itf_desc_array[i], &bulk_in, &bulk_out) == RTK_SUCCESS) {
			*data = &itf_data->itf_desc_array[i];
			return RTK_SUCCESS;
		}
	}

	RTK_LOGS(TAG, RTK_LOG_ERROR, "No bulk pair\n");
	return -RTK_ERR_BADARG;
}

/**
  * @brief  Use a random locally administered unicast MAC for a device that reports none
  * @retval None
  */
static void usbh_rndis_set_random_mac(void)
{
	usbh_rndis_host_t *cdc = &usbh_rndis_host;
	u32 ts;

	if (TRNG_get_random_bytes(cdc->mac, USB_RNDIS_MAC_ADDR_LEN) != RTK_SUCCESS) {
		ts = usb_os_get_timestamp_ms();
		usb_os_memcpy((void *)&cdc->mac[2], (const void *)&ts, sizeof(ts));
	}
	cdc->mac[0] = (u8)((cdc->mac[0] & 0xFEU) | 0x02U);
	cdc->mac_valid = 1U;
	RTK_LOGS(TAG, RTK_LOG_WARN, "Random MAC\n");
}

/**
  * @brief  Send the encapsulated command held in ctrl_buf
  * @param  host: Host handle
  * @param  len: Message length
  * @retval usbh_ctrl_request() status
  */
static int usbh_rndis_send_command(usb_host_t *host, u16 len)
{
	usbh_rndis_host_t *cdc = &usbh_rndis_host;
	usbh_setup_req_t setup;

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = USB_RNDIS_SEND_ENCAPSULATED_COMMAND;
	setup.req.wValue = 0U;
	setup.req.wIndex = cdc->ctrl_itf_id;
	setup.req.wLength = len;

	return usbh_ctrl_request(host, &setup, cdc->ctrl_buf);
}

/**
  * @brief  Read one encapsulated response into ctrl_buf
  * @param  host: Host handle
  * @retval usbh_ctrl_request() status
  */
static int usbh_rndis_get_response(usb_host_t *host)
{
	usbh_rndis_host_t *cdc = &usbh_rndis_host;
	usbh_setup_req_t setup;

	setup.req.bmRequestType = USB_D2H | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = USB_RNDIS_GET_ENCAPSULATED_RESPONSE;
	setup.req.wValue = 0U;
	setup.req.wIndex = cdc->ctrl_itf_id;
	setup.req.wLength = USBH_RNDIS_CTRL_BUF_SIZE;

	return usbh_ctrl_request(host, &setup, cdc->ctrl_buf);
}

/**
  * @brief  Build the command of a setup step in ctrl_buf
  * @note   A pure function of the step and request_id: the core may call setup() again with the
  *         request in flight, which then has to carry the same bytes.
  * @param  step: Even setup step
  * @retval Message length
  */
static u16 usbh_rndis_build_command(u8 step)
{
	usbh_rndis_host_t *cdc = &usbh_rndis_host;
	usb_rndis_initialize_msg_t *init = (usb_rndis_initialize_msg_t *)(void *)cdc->ctrl_buf;
	usb_rndis_query_msg_t *query = (usb_rndis_query_msg_t *)(void *)cdc->ctrl_buf;
	u16 len;

	usb_os_memset((void *)cdc->ctrl_buf, 0, USBH_RNDIS_CTRL_BUF_SIZE);

	if (step == USBH_RNDIS_SETUP_SEND_INIT) {
		init->MessageType = USB_RNDIS_MSG_INITIALIZE;
		init->MessageLength = sizeof(usb_rndis_initialize_msg_t);
		init->RequestId = cdc->request_id;
		init->MajorVersion = USB_RNDIS_MAJOR_VERSION;
		init->MinorVersion = USB_RNDIS_MINOR_VERSION;
		init->MaxTransferSize = USBH_RNDIS_RX_BUF_SIZE;
		return (u16)sizeof(usb_rndis_initialize_msg_t);
	}

	query->RequestId = cdc->request_id;
	len = (u16)sizeof(usb_rndis_query_msg_t);
	if (step == USBH_RNDIS_SETUP_SEND_SET_FILTER) {
		query->MessageType = USB_RNDIS_MSG_SET;
		query->Oid = USB_RNDIS_OID_GEN_CURRENT_PACKET_FILTER;
		query->InformationBufferLength = sizeof(cdc->packet_filter);
		query->InformationBufferOffset = sizeof(usb_rndis_query_msg_t) - USB_RNDIS_INFO_BUF_OFFSET_BASE;
		usb_os_memcpy((void *)(cdc->ctrl_buf + len), (const void *)&cdc->packet_filter, sizeof(cdc->packet_filter));
		len += (u16)sizeof(cdc->packet_filter);
	} else {
		query->MessageType = USB_RNDIS_MSG_QUERY;
		query->Oid = (step == USBH_RNDIS_SETUP_SEND_QUERY_MAC) ? USB_RNDIS_OID_802_3_PERMANENT_ADDRESS : USB_RNDIS_OID_GEN_MEDIA_CONNECT_STATUS;
	}
	query->MessageLength = len;

	return len;
}

/**
  * @brief  Take an INDICATE_STATUS message out of ctrl_buf
  * @param  len: Bytes received
  * @retval None
  */
static void usbh_rndis_handle_status(u32 len)
{
	usbh_rndis_host_t *cdc = &usbh_rndis_host;
	const usb_rndis_indicate_status_msg_t *ind = (const usb_rndis_indicate_status_msg_t *)(const void *)cdc->ctrl_buf;

	if (len < sizeof(usb_rndis_indicate_status_msg_t)) {
		return;
	}
	if (ind->Status == USB_RNDIS_STATUS_MEDIA_CONNECT) {
		cdc->link_up = 1U;
		RTK_LOGS(TAG, RTK_LOG_INFO, "Media connect\n");
	} else if (ind->Status == USB_RNDIS_STATUS_MEDIA_DISCONNECT) {
		cdc->link_up = 0U;
		RTK_LOGS(TAG, RTK_LOG_INFO, "Media disconnect\n");
	} else {
		/* Other status indications are informational */
	}
}

/**
  * @brief  Check the response in ctrl_buf against the command of a setup step and apply it
  * @param  host: Host handle
  * @param  step: Odd setup step
  * @retval RTK_SUCCESS when the step is done, -RTK_ERR_BUSY to poll again, other errors terminal
  */
static int usbh_rndis_parse_response(usb_host_t *host, u8 step)
{
	usbh_rndis_host_t *cdc = &usbh_rndis_host;
	const usb_rndis_query_cmplt_t *cmplt = (const usb_rndis_query_cmplt_t *)(const void *)cdc->ctrl_buf;
	const usb_rndis_initialize_cmplt_t *init;
	u32 len = usbh_ctrl_get_xfer_len(host);
	u32 expect = (step == USBH_RNDIS_SETUP_GET_INIT) ? USB_RNDIS_MSG_INITIALIZE_CMPLT :
				 ((step == USBH_RNDIS_SETUP_GET_SET_FILTER) ? USB_RNDIS_MSG_SET_CMPLT : USB_RNDIS_MSG_QUERY_CMPLT);
	const u8 *mac;
	u32 info_off;
	u32 val;

	if (len > USBH_RNDIS_CTRL_BUF_SIZE) {
		len = USBH_RNDIS_CTRL_BUF_SIZE;
	}

	/* Ref RNDIS USB 2.2.2: a single 0x00 byte means no response yet */
	if (len < sizeof(usb_rndis_status_cmplt_t)) {
		return -RTK_ERR_BUSY;
	}
	if (cmplt->MessageType == USB_RNDIS_MSG_INDICATE_STATUS) {
		usbh_rndis_handle_status(len);
		return -RTK_ERR_BUSY;
	}
	if ((cmplt->MessageType != expect) || (cmplt->RequestId != cdc->request_id) || (cmplt->MessageLength > len)) {
		/* A stale or foreign message: read the next one */
		return -RTK_ERR_BUSY;
	}

	switch (step) {
	case USBH_RNDIS_SETUP_GET_INIT:
		init = (const usb_rndis_initialize_cmplt_t *)(const void *)cdc->ctrl_buf;
		if ((len < sizeof(usb_rndis_initialize_cmplt_t)) || (init->Status != USB_RNDIS_STATUS_SUCCESS) ||
			(init->Medium != USB_RNDIS_MEDIUM_802_3)) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Init fail %08x\n", init->Status);
			return -RTK_ERR_IO;
		}
		cdc->dev_max_xfer = init->MaxTransferSize;
		RTK_LOGS(TAG, RTK_LOG_INFO, "Init ok, ver %d.%d max %d\n", init->MajorVersion, init->MinorVersion, init->MaxTransferSize);
		break;

	case USBH_RNDIS_SETUP_GET_SET_FILTER:
		if (cmplt->Status != USB_RNDIS_STATUS_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Set filter fail %08x\n", cmplt->Status);
			return -RTK_ERR_IO;
		}
		break;

	default:
		/* QUERY_CMPLT: the information buffer shall lie inside the message */
		info_off = cmplt->InformationBufferOffset + USB_RNDIS_INFO_BUF_OFFSET_BASE;
		if ((cmplt->Status != USB_RNDIS_STATUS_SUCCESS) || (info_off > cmplt->MessageLength) ||
			(cmplt->InformationBufferLength > (cmplt->MessageLength - info_off))) {
			val = 0U;
		} else {
			val = cmplt->InformationBufferLength;
		}
		if (step == USBH_RNDIS_SETUP_GET_QUERY_MAC) {
			mac = cdc->ctrl_buf + info_off;
			/* An all-zero address is no address, as the CDC ECM class treats it */
			if ((val >= USB_RNDIS_MAC_ADDR_LEN) && ((mac[0] | mac[1] | mac[2] | mac[3] | mac[4] | mac[5]) != 0U)) {
				usb_os_memcpy((void *)cdc->mac, (const void *)mac, USB_RNDIS_MAC_ADDR_LEN);
				cdc->mac_valid = 1U;
				RTK_LOGS(TAG, RTK_LOG_INFO, "MAC[%02x %02x %02x %02x %02x %02x]\n", cdc->mac[0], cdc->mac[1], cdc->mac[2], cdc->mac[3],
						 cdc->mac[4], cdc->mac[5]);
			} else {
				usbh_rndis_set_random_mac();
			}
		} else {
			/* No answer reads as connected, as the Linux rndis_host assumes */
			if (val >= sizeof(val)) {
				usb_os_memcpy((void *)&val, (const void *)(cdc->ctrl_buf + info_off), sizeof(val));
				cdc->link_up = (val == USB_RNDIS_MEDIA_STATE_CONNECTED) ? 1U : 0U;
			} else {
				cdc->link_up = 1U;
			}
		}
		break;
	}

	return RTK_SUCCESS;
}

/**
  * @brief  Check whether the device is set up and data may flow
  * @note   Also called from the SOF ISR: the host pointer is read once.
  * @retval true if ready, false otherwise
  */
static bool usbh_rndis_data_ready(void)
{
	usbh_rndis_host_t *cdc = &usbh_rndis_host;
	usb_host_t *host = cdc->host;

	return ((host != NULL) && (usbh_dev_is_ready(host)) && (cdc->state == USBH_RNDIS_STATE_XFER));
}

/**
  * @brief  Take the endpoints of the attached device
  * @param  host: Host handle
  * @retval RTK_SUCCESS on success, -RTK_ERR_BADARG when the device is no usable RNDIS function
  */
static int usbh_rndis_parse_itf_desc(usb_host_t *host)
{
	usbh_rndis_host_t *cdc = &usbh_rndis_host;
	usbh_itf_t *ctrl_itf;
	usbh_itf_desc_t *data_itf_desc;
	usbh_ep_desc_t *bulk_in;
	usbh_ep_desc_t *bulk_out;

	if (usbh_rndis_find_function(host, &ctrl_itf, &data_itf_desc) != RTK_SUCCESS) {
		return -RTK_ERR_BADARG;
	}

	cdc->ctrl_itf_id = ctrl_itf->itf_desc_array[0].bInterfaceNumber;
	/* The BULK pair may live in a non-zero alternate setting, which setup() selects first */
	cdc->data_itf_id = data_itf_desc->bInterfaceNumber;
	cdc->data_alt_set = data_itf_desc->bAlternateSetting;
	usb_os_memset((void *)&cdc->intr_rx.ep_desc, 0, sizeof(usbh_ep_desc_t));
	if (ctrl_itf->itf_desc_array[0].bNumEndpoints != 0U) {
		usb_os_memcpy((void *)&cdc->intr_rx.ep_desc, (const void *)&ctrl_itf->itf_desc_array[0].ep_desc_array[0], sizeof(usbh_ep_desc_t));
	}

	if (usbh_rndis_check_data_eps(data_itf_desc, &bulk_in, &bulk_out) != RTK_SUCCESS) {
		return -RTK_ERR_BADARG;
	}
	usb_os_memcpy((void *)&cdc->bulk_rx.ep_desc, (const void *)bulk_in, sizeof(usbh_ep_desc_t));
	usb_os_memcpy((void *)&cdc->bulk_tx.ep_desc, (const void *)bulk_out, sizeof(usbh_ep_desc_t));

	RTK_LOGS(TAG, RTK_LOG_INFO, "Ctrl if %d data if %d alt %d\n", cdc->ctrl_itf_id, cdc->data_itf_id, cdc->data_alt_set);

	return RTK_SUCCESS;
}

/**
  * @brief  Usb Attach callback function
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_rndis_attach(usb_host_t *host)
{
	usbh_rndis_host_t *cdc = &usbh_rndis_host;
	usbh_pipe_t *pipe;
	int status = -RTK_ERR_BADARG;

	/* deinit_all_pipe() needs the host to close what a failed attach opened */
	cdc->host = host;

	if (usbh_rndis_parse_itf_desc(host) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Parse desc fail\n");
		goto attach_fail;
	}

	/* The notification endpoint is optional, Ref RNDIS USB 2.1: without it the class polls */
	if (cdc->intr_rx.ep_desc.bLength != 0U) {
		pipe = &cdc->intr_rx.pipe;
		if (usbh_pipe_open(host, pipe, &cdc->intr_rx.ep_desc, &usbh_rndis_driver) != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Open intr pipe fail\n");
			goto attach_fail;
		}
		pipe->xfer_buf_len = usb_get_dma_len(pipe->ep_mps, pipe->ep_mps);
		pipe->xfer_buf = (u8 *)usb_os_malloc(pipe->xfer_buf_len);
		if (pipe->xfer_buf == NULL) {
			status = -RTK_ERR_NOMEM;
			goto attach_fail;
		}
		pipe->xfer_len = pipe->ep_mps;
		pipe->max_timeout_tick = USBH_RNDIS_INTR_BUSY_MAX_CNT;
		pipe->xfer_state = USBH_PIPE_XFER_IDLE;
	}

	pipe = &cdc->bulk_tx.pipe;
	if (usbh_pipe_open(host, pipe, &cdc->bulk_tx.ep_desc, &usbh_rndis_driver) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Open bulk out pipe fail\n");
		goto attach_fail;
	}
	pipe->max_timeout_tick = USBH_RNDIS_BULK_OUT_BUSY_MAX_CNT;
	pipe->xfer_state = USBH_PIPE_XFER_IDLE;

	pipe = &cdc->bulk_rx.pipe;
	if (usbh_pipe_open(host, pipe, &cdc->bulk_rx.ep_desc, &usbh_rndis_driver) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Open bulk in pipe fail\n");
		goto attach_fail;
	}
	pipe->xfer_buf = (u8 *)usb_os_malloc(USBH_RNDIS_RX_BUF_SIZE);
	if (pipe->xfer_buf == NULL) {
		status = -RTK_ERR_NOMEM;
		goto attach_fail;
	}
	pipe->xfer_len = usb_get_max_payload_len(USBH_RNDIS_RX_BUF_SIZE, pipe->ep_mps);
	pipe->xfer_buf_len = USBH_RNDIS_RX_BUF_SIZE;
	pipe->max_timeout_tick = USBH_RNDIS_BULK_IN_IDLE_MAX_CNT;
	pipe->xfer_state = USBH_PIPE_XFER_IDLE;

	cdc->state = USBH_RNDIS_STATE_IDLE;
	cdc->setup_step = USBH_RNDIS_SETUP_SEND_INIT;
	cdc->ctrl_step = USBH_RNDIS_CTRL_IDLE;
	cdc->cmd_len = 0U;
	cdc->resp_avail = 0U;
	cdc->halt_retry_cnt = 0U;
	cdc->link_up = 0U;
	cdc->mac_valid = 0U;
	cdc->dev_max_xfer = 0U;

	if ((cdc->cb != NULL) && (cdc->cb->attached != NULL)) {
		cdc->cb->attached();
	}

	return RTK_SUCCESS;

attach_fail:
	/* The core calls no detach() for a failed attach: undo here */
	usbh_rndis_deinit_all_pipe();
	cdc->host = NULL;
	return status;
}

/**
  * @brief  Usb Detach callback function
  * @param  host: Host handle
  * @retval None
  */
static void usbh_rndis_detach(usb_host_t *host)
{
	usbh_rndis_host_t *cdc = &usbh_rndis_host;

	UNUSED(host);

	cdc->state = USBH_RNDIS_STATE_IDLE;
	cdc->link_up = 0U;
	cdc->mac_valid = 0U;

	usbh_rndis_deinit_all_pipe();
	cdc->host = NULL;

	/* Fail a blocked sender: the closed pipe never completes its TX */
	cdc->bulk_tx_err = 1U;
	usb_os_sema_give(cdc->bulk_tx_sema);

	if ((cdc->cb != NULL) && (cdc->cb->detached != NULL)) {
		cdc->cb->detached();
	}
}

/**
  * @brief  Usb Setup callback function: data alternate setting, INITIALIZE, MAC query, packet filter and media query
  * @note   Non-blocking, one control transfer per call. A command is built once per step, so a
  *         call while it is in flight resubmits nothing.
  * @param  host: Host handle
  * @retval RTK_SUCCESS when done, -RTK_ERR_BUSY in progress, other errors terminal
  */
static int usbh_rndis_setup(usb_host_t *host)
{
	usbh_rndis_host_t *cdc = &usbh_rndis_host;
	int status;

	/* A BULK pair outside the default setting only works once selected (Ref USB 2.0 9.4.10).
	   Alt 0 is already in effect after SET_CONFIGURATION, so no request is sent for it */
	if (cdc->data_alt_set != 0U) {
		status = usbh_itf_set_alt(host, cdc->data_itf_id, cdc->data_alt_set);
		if (status == RTK_SUCCESS) {
			cdc->data_alt_set = 0U;
		} else if (status != -RTK_ERR_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Set if %d alt %d err %d\n", cdc->data_itf_id, cdc->data_alt_set, status);
			return -RTK_ERR_IO;
		} else {
			/* In flight */
		}
		return -RTK_ERR_BUSY;
	}

	if (cdc->setup_step >= USBH_RNDIS_SETUP_DONE) {
		return RTK_SUCCESS;
	}

	if ((cdc->setup_step & 0x1U) == 0U) {
		if (cdc->cmd_len == 0U) {
			cdc->request_id++;
			cdc->cmd_len = usbh_rndis_build_command(cdc->setup_step);
		}
		status = usbh_rndis_send_command(host, cdc->cmd_len);
		if (status == RTK_SUCCESS) {
			cdc->cmd_len = 0U;
			cdc->resp_retry = USBH_RNDIS_RESP_RETRY_MAX_CNT;
			cdc->setup_step++;
		} else if (status != -RTK_ERR_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Send cmd %d err %d\n", cdc->setup_step, status);
			return -RTK_ERR_IO;
		} else {
			/* In flight */
		}
		return -RTK_ERR_BUSY;
	}

	status = usbh_rndis_get_response(host);
	if (status == -RTK_ERR_BUSY) {
		return status;
	}
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Get resp %d err %d\n", cdc->setup_step, status);
		return -RTK_ERR_IO;
	}

	status = usbh_rndis_parse_response(host, cdc->setup_step);
	if (status == -RTK_ERR_BUSY) {
		if (--cdc->resp_retry == 0U) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Resp %d timeout\n", cdc->setup_step);
			return -RTK_ERR_TIMEOUT;
		}
		/* The device is still working on the command: poll again a little later */
		usb_os_sleep_ms(1U);
		return -RTK_ERR_BUSY;
	}
	if (status != RTK_SUCCESS) {
		return status;
	}

	cdc->setup_step++;
	if (cdc->setup_step < USBH_RNDIS_SETUP_DONE) {
		return -RTK_ERR_BUSY;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Setup done, link %d\n", cdc->link_up);
	cdc->state = USBH_RNDIS_STATE_XFER;
	if ((cdc->cb != NULL) && (cdc->cb->setup != NULL)) {
		cdc->cb->setup();
	}

	return RTK_SUCCESS;
}

/**
  * @brief  Start the recovery of a halted endpoint
  * @note   Re-submitting to a halted endpoint only repeats the STALL (Ref USB 2.0 8.4.5), the
  *         error state keeps the SOF from re-arming any pipe until it is cleared.
  * @param  host: Host handle
  * @param  pipe: Halted pipe
  * @retval None
  */
static void usbh_rndis_halt(usb_host_t *host, const usbh_pipe_t *pipe)
{
	usbh_rndis_host_t *cdc = &usbh_rndis_host;

	if (cdc->state == USBH_RNDIS_STATE_ERROR) {
		return;
	}

	cdc->halt_ep_addr = pipe->ep_addr;
	cdc->state = USBH_RNDIS_STATE_ERROR;
	usbh_notify(host, 0U, &usbh_rndis_driver);
}

/**
  * @brief  Clear the halted endpoint, re-enumerate the device if that keeps failing
  * @param  host: Host handle
  * @retval None
  */
static void usbh_rndis_process_halt(usb_host_t *host)
{
	usbh_rndis_host_t *cdc = &usbh_rndis_host;
	int ret;

	if (cdc->halt_retry_cnt >= USBH_RNDIS_HALT_RETRY_MAX_CNT) {
		return;
	}

	ret = usbh_pipe_clear_halt(host, cdc->halt_ep_addr);
	if (ret == RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "EP%02x halt cleared\n", cdc->halt_ep_addr);
		cdc->halt_retry_cnt = 0U;
		cdc->state = USBH_RNDIS_STATE_XFER;
		/* This state swallowed any pipe message that arrived meanwhile: re-drive the pipes */
		usbh_notify(host, cdc->bulk_rx.pipe.pipe_num, &usbh_rndis_driver);
		usbh_notify(host, cdc->bulk_tx.pipe.pipe_num, &usbh_rndis_driver);
	} else if (ret != -RTK_ERR_BUSY) {
		cdc->halt_retry_cnt++;
		if (cdc->halt_retry_cnt >= USBH_RNDIS_HALT_RETRY_MAX_CNT) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "EP%02x clear halt fail, re-enum\n", cdc->halt_ep_addr);
			if (usbh_dev_reset() != RTK_SUCCESS) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Re-enum refused\n");
			}
		} else {
			usbh_notify(host, 0U, &usbh_rndis_driver);
		}
	} else {
		/* In flight: the EP0 completion re-drives process() */
	}
}

/**
  * @brief  Release a blocked sender once its BULK OUT transfer ended
  * @param  state: RTK_SUCCESS, or the error the transfer ended with
  * @retval None
  */
static void usbh_rndis_tx_done(int state)
{
	usbh_rndis_host_t *cdc = &usbh_rndis_host;

	if (cdc->bulk_tx_block != 0U) {
		cdc->bulk_tx_err = (state == RTK_SUCCESS) ? 0U : 1U;
		usb_os_sema_give(cdc->bulk_tx_sema);
	}
}

/**
  * @brief  Bulk Out handling function
  * @param  host: Host handle
  * @retval None
  */
static void usbh_rndis_process_bulk_out(usb_host_t *host)
{
	usbh_pipe_t *pipe = &usbh_rndis_host.bulk_tx.pipe;
	int status;

	status = usbh_xfer_process(host, pipe);
	if ((status == RTK_SUCCESS) && (pipe->xfer_state == USBH_PIPE_XFER_IDLE)) {
		usbh_rndis_tx_done(RTK_SUCCESS);
	} else if (pipe->xfer_state == USBH_PIPE_XFER_START) {
		usbh_notify(host, pipe->pipe_num, &usbh_rndis_driver);
	} else if (pipe->xfer_state == USBH_PIPE_XFER_ERROR) {
		if (usbh_get_urb_state(host, pipe) == USBH_URB_STALL) {
			usbh_rndis_halt(host, pipe);
		}
		/* Drop the frame (Ethernet is best effort) and release the sender */
		pipe->xfer_state = USBH_PIPE_XFER_IDLE;
		usbh_rndis_tx_done(-RTK_ERR_IO);
	} else {
		/* Idle or still in flight */
	}
}

/**
  * @brief  Deliver the frames of one received transfer
  * @note   Ref RNDIS 1.0: one or more REMOTE_NDIS_PACKET_MSG, possibly followed by a 1-byte pad.
  *         Every message is bounds-checked before its frame is read.
  * @param  buf: Received transfer
  * @param  len: Transfer length
  * @retval None
  */
static void usbh_rndis_deliver(u8 *buf, u32 len)
{
	usbh_rndis_host_t *cdc = &usbh_rndis_host;
	const usb_rndis_packet_msg_t *pkt;
	u32 off = 0U;
	u32 msg_len;
	u32 data_off;
	u32 data_len;

	while ((len - off) >= USB_RNDIS_PACKET_MSG_HDR_SIZE) {
		pkt = (const usb_rndis_packet_msg_t *)(const void *)(buf + off);
		msg_len = pkt->MessageLength;
		data_off = pkt->DataOffset + 8U;
		data_len = pkt->DataLength;
		if ((pkt->MessageType != USB_RNDIS_MSG_PACKET) || (msg_len < USB_RNDIS_PACKET_MSG_HDR_SIZE) || (msg_len > (len - off)) ||
			(data_off < USB_RNDIS_PACKET_MSG_HDR_SIZE) || (pkt->DataOffset > msg_len) || (data_off > msg_len) || (data_len > (msg_len - data_off)) ||
			(data_len == 0U)) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "Bad pkt msg\n");
			return;
		}
		if ((cdc->cb != NULL) && (cdc->cb->received != NULL)) {
			cdc->cb->received(buf + off + data_off, data_len);
		}
		off += msg_len;
	}
}

/**
  * @brief  Bulk In handling function
  * @param  host: Host handle
  * @retval None
  */
static void usbh_rndis_process_bulk_in(usb_host_t *host)
{
	usbh_pipe_t *pipe = &usbh_rndis_host.bulk_rx.pipe;
	int status = usbh_xfer_process(host, pipe);

	if ((status == RTK_SUCCESS) && (pipe->xfer_state == USBH_PIPE_XFER_IDLE)) {
		usbh_rndis_deliver(pipe->xfer_buf, usbh_pipe_get_xfer_len(host, pipe));
		pipe->xfer_state = USBH_PIPE_XFER_START;
		usbh_notify(host, pipe->pipe_num, &usbh_rndis_driver);
	} else if (pipe->xfer_state == USBH_PIPE_XFER_START) {
		/* A retry reads 0 and delivers nothing; a non-zero length is data that landed just as
		   the retry fired, see the CDC ECM class */
		usbh_rndis_deliver(pipe->xfer_buf, usbh_pipe_get_xfer_len(host, pipe));
		usbh_notify(host, pipe->pipe_num, &usbh_rndis_driver);
	} else if (pipe->xfer_state == USBH_PIPE_XFER_ERROR) {
		if (usbh_get_urb_state(host, pipe) == USBH_URB_STALL) {
			/* Re-armed by the SOF once the halt is cleared */
			pipe->xfer_state = USBH_PIPE_XFER_IDLE;
			usbh_rndis_halt(host, pipe);
		} else {
			pipe->xfer_state = USBH_PIPE_XFER_START;
			usbh_notify(host, pipe->pipe_num, &usbh_rndis_driver);
		}
	} else {
		/* Still in flight */
	}
}

/**
  * @brief  Intr In handling function: RESPONSE_AVAILABLE
  * @note   A completed IN shows as latched URB_DONE while xfer_state is BUSY, consumed here
  *         rather than through usbh_xfer_process(), as the CDC ECM class does.
  * @param  host: Host handle
  * @retval None
  */
static void usbh_rndis_process_intr_in(usb_host_t *host)
{
	usbh_rndis_host_t *cdc = &usbh_rndis_host;
	usbh_pipe_t *pipe = &cdc->intr_rx.pipe;
	usbh_urb_state_t urb_state = usbh_get_urb_state(host, pipe);
	int status;

	if ((pipe->xfer_state == USBH_PIPE_XFER_BUSY) && (urb_state == USBH_URB_DONE)) {
		pipe->retry_cnt = 0U;
		pipe->xfer_state = USBH_PIPE_XFER_IDLE;
		/* Ref RNDIS USB 2.2.3: the first 32-bit word is RESPONSE_AVAILABLE */
		if ((usbh_pipe_get_xfer_len(host, pipe) >= 4U) && (pipe->xfer_buf[0] == (u8)USB_RNDIS_NOTIFY_RESPONSE_AVAILABLE)) {
			cdc->resp_avail = 1U;
			usbh_notify(host, 0U, &usbh_rndis_driver);
		}
		return;
	}

	if ((pipe->xfer_state == USBH_PIPE_XFER_BUSY) && (urb_state == USBH_URB_STALL)) {
		pipe->xfer_state = USBH_PIPE_XFER_IDLE;
		usbh_rndis_halt(host, pipe);
		return;
	}

	/* Never re-submit a BUSY INTR channel, only arm on START */
	if (pipe->xfer_state == USBH_PIPE_XFER_START) {
		status = usbh_xfer_process(host, pipe);
		if ((status != RTK_SUCCESS) || (pipe->xfer_state == USBH_PIPE_XFER_ERROR)) {
			pipe->xfer_state = USBH_PIPE_XFER_IDLE;
		}
	}
}

/**
  * @brief  Drive the control step of the transfer state: read an announced message, answer a keepalive
  * @param  host: Host handle
  * @retval None
  */
static void usbh_rndis_process_ctrl(usb_host_t *host)
{
	usbh_rndis_host_t *cdc = &usbh_rndis_host;
	usb_rndis_request_msg_t *req = (usb_rndis_request_msg_t *)(void *)cdc->ctrl_buf;
	usb_rndis_status_cmplt_t *cmplt = (usb_rndis_status_cmplt_t *)(void *)cdc->ctrl_buf;
	u32 request_id;
	u32 len;
	int status;

	if ((cdc->ctrl_step == USBH_RNDIS_CTRL_IDLE) && (cdc->resp_avail != 0U)) {
		cdc->resp_avail = 0U;
		cdc->ctrl_step = USBH_RNDIS_CTRL_GET_RESP;
	}

	switch (cdc->ctrl_step) {
	case USBH_RNDIS_CTRL_GET_RESP:
		status = usbh_rndis_get_response(host);
		if (status == -RTK_ERR_BUSY) {
			return;
		}
		cdc->ctrl_step = USBH_RNDIS_CTRL_IDLE;
		if (status != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "Get resp err %d\n", status);
			return;
		}
		len = usbh_ctrl_get_xfer_len(host);
		if (len < sizeof(usb_rndis_msg_hdr_t)) {
			return;
		}
		if (req->MessageType == USB_RNDIS_MSG_INDICATE_STATUS) {
			usbh_rndis_handle_status(len);
		} else if ((req->MessageType == USB_RNDIS_MSG_KEEPALIVE) && (len >= sizeof(usb_rndis_request_msg_t))) {
			/* Ref RNDIS 1.0: a device keepalive is answered with KEEPALIVE_CMPLT */
			request_id = req->RequestId;
			usb_os_memset((void *)cdc->ctrl_buf, 0, sizeof(usb_rndis_status_cmplt_t));
			cmplt->MessageType = USB_RNDIS_MSG_KEEPALIVE_CMPLT;
			cmplt->MessageLength = sizeof(usb_rndis_status_cmplt_t);
			cmplt->RequestId = request_id;
			cmplt->Status = USB_RNDIS_STATUS_SUCCESS;
			cdc->ctrl_step = USBH_RNDIS_CTRL_SEND_KEEPALIVE;
			usbh_notify(host, 0U, &usbh_rndis_driver);
		} else {
			/* A late completion nobody waits for */
		}
		break;

	case USBH_RNDIS_CTRL_SEND_KEEPALIVE:
		status = usbh_rndis_send_command(host, (u16)sizeof(usb_rndis_status_cmplt_t));
		if (status != -RTK_ERR_BUSY) {
			cdc->ctrl_step = USBH_RNDIS_CTRL_IDLE;
			if (cdc->resp_avail != 0U) {
				usbh_notify(host, 0U, &usbh_rndis_driver);
			}
		}
		break;

	default:
		break;
	}
}

/**
  * @brief  Usb State Machine handling callback
  * @param  host: Host handle
  * @param  msg: Driver message
  * @retval None
  */
static void usbh_rndis_process(usb_host_t *host, usbh_drv_msg_t *msg)
{
	usbh_rndis_host_t *cdc = &usbh_rndis_host;
	u8 pipe_num;

	switch (cdc->state) {
	case USBH_RNDIS_STATE_XFER:
		if (msg == NULL) {
			break;
		}
		pipe_num = msg->pipe_num;
		if ((pipe_num != 0U) && (pipe_num == cdc->bulk_tx.pipe.pipe_num)) {
			usbh_rndis_process_bulk_out(host);
		} else if ((pipe_num != 0U) && (pipe_num == cdc->bulk_rx.pipe.pipe_num)) {
			usbh_rndis_process_bulk_in(host);
		} else if ((pipe_num != 0U) && (pipe_num == cdc->intr_rx.pipe.pipe_num)) {
			usbh_rndis_process_intr_in(host);
		} else {
			usbh_rndis_process_ctrl(host);
		}
		break;

	case USBH_RNDIS_STATE_ERROR:
		usbh_rndis_process_halt(host);
		break;

	case USBH_RNDIS_STATE_IDLE:
	default:
		usb_os_sleep_ms(1U);
		break;
	}
}

/**
  * @brief  Tell whether an armed pipe is due to be driven again
  * @note   ISR context. Same budgets as the CDC ECM class: an idle URB past the idle budget, a busy
  *         one past the NAK budget, or a finished one.
  * @param  host: Host handle
  * @param  pipe: Pipe
  * @param  idle_max: Idle budget in ticks
  * @param  busy_max: Busy budget in ticks
  * @retval 1 if due, 0 otherwise
  */
static u8 usbh_rndis_pipe_due(usb_host_t *host, usbh_pipe_t *pipe, u32 idle_max, u32 busy_max)
{
	usbh_urb_state_t urb_state = usbh_get_urb_state(host, pipe);
	u32 elapsed = usbh_tick_elapsed(host, pipe->tick);

	if (((urb_state == USBH_URB_IDLE) && (elapsed >= idle_max)) || ((urb_state == USBH_URB_BUSY) && (elapsed >= busy_max))) {
		return 1U;
	}

	return ((urb_state == USBH_URB_ERROR) || (urb_state == USBH_URB_STALL) || (urb_state == USBH_URB_DONE)) ? 1U : 0U;
}

/**
  * @brief  SOF callback: keep the INTR IN and BULK IN pipes armed and re-drive a pending BULK OUT
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  host: USB host handle
  * @retval None
  */
static void usbh_rndis_sof(usb_host_t *host)
{
	usbh_rndis_host_t *cdc = &usbh_rndis_host;
	usbh_urb_state_t urb_state;
	usbh_pipe_t *pipe;
	u32 interval;

	if (!usbh_rndis_data_ready()) {
		return;
	}

	pipe = &cdc->intr_rx.pipe;
	if (pipe->pipe_num != 0U) {
		/* Widened past the bInterval=1 case, else the re-arm fires on an INTR IN still in flight */
		interval = (u32)pipe->ep_interval + USBH_RNDIS_INTR_BUSY_MAX_CNT;
		urb_state = usbh_get_urb_state(host, pipe);
		if (pipe->xfer_state == USBH_PIPE_XFER_IDLE) {
			if (usbh_tick_elapsed(host, pipe->tick) >= pipe->ep_interval) {
				pipe->xfer_state = USBH_PIPE_XFER_START;
				usbh_notify(host, pipe->pipe_num, &usbh_rndis_driver);
			}
		} else if ((pipe->xfer_state == USBH_PIPE_XFER_BUSY) && (urb_state == USBH_URB_STALL)) {
			/* Reported only: re-submitting to a halted endpoint repeats the STALL */
			usbh_notify(host, pipe->pipe_num, &usbh_rndis_driver);
		} else if ((pipe->xfer_state == USBH_PIPE_XFER_BUSY) && (urb_state != USBH_URB_DONE) &&
				   ((urb_state == USBH_URB_ERROR) || (usbh_tick_elapsed(host, pipe->tick) >= interval))) {
			/* NAKed past its window: poll again. A DONE is consumed by process() */
			pipe->xfer_state = USBH_PIPE_XFER_START;
			usbh_notify(host, pipe->pipe_num, &usbh_rndis_driver);
		} else {
			/* In flight */
		}
	}

	pipe = &cdc->bulk_rx.pipe;
	if (pipe->pipe_num != 0U) {
		if ((pipe->xfer_state == USBH_PIPE_XFER_IDLE) ||
			(usbh_rndis_pipe_due(host, pipe, USBH_RNDIS_BULK_IN_IDLE_MAX_CNT, USBH_RNDIS_BULK_IN_NAK_CHECK_MAX_CNT) != 0U)) {
			if (pipe->xfer_state == USBH_PIPE_XFER_IDLE) {
				pipe->xfer_state = USBH_PIPE_XFER_START;
			}
			usbh_notify(host, pipe->pipe_num, &usbh_rndis_driver);
		}
	}

	pipe = &cdc->bulk_tx.pipe;
	if ((pipe->pipe_num != 0U) && (pipe->xfer_state != USBH_PIPE_XFER_IDLE) &&
		(usbh_rndis_pipe_due(host, pipe, USBH_RNDIS_BULK_OUT_IDLE_MAX_CNT, USBH_RNDIS_BULK_OUT_BUSY_MAX_CNT) != 0U)) {
		usbh_notify(host, pipe->pipe_num, &usbh_rndis_driver);
	}
}

/**
  * @brief  Close every pipe opened by attach() and free its buffer
  * @retval None
  */
static void usbh_rndis_deinit_all_pipe(void)
{
	usbh_rndis_host_t *cdc = &usbh_rndis_host;
	usb_host_t *host = cdc->host;

	if (host == NULL) {
		return;
	}

	if (cdc->intr_rx.pipe.pipe_num != 0U) {
		usbh_pipe_close(host, &cdc->intr_rx.pipe);
	}
	USBH_RNDIS_FREE_MEM(cdc->intr_rx.pipe.xfer_buf);

	if (cdc->bulk_rx.pipe.pipe_num != 0U) {
		usbh_pipe_close(host, &cdc->bulk_rx.pipe);
	}
	USBH_RNDIS_FREE_MEM(cdc->bulk_rx.pipe.xfer_buf);

	if (cdc->bulk_tx.pipe.pipe_num != 0U) {
		usbh_pipe_close(host, &cdc->bulk_tx.pipe);
	}
}

/**
  * @brief  Wait for every task inside the TX API to leave
  * @note   No upper bound: freeing under a starved caller would be a use after free.
  * @retval None
  */
static void usbh_rndis_tx_wait_exit(void)
{
	usbh_rndis_host_t *cdc = &usbh_rndis_host;
	u32 wait_ms = 0U;

	while ((cdc->bulk_tx_block != 0U) || (cdc->tx_users != 0U)) {
		usb_os_sema_give(cdc->bulk_tx_sema);
		usb_os_sleep_ms(1U);
		wait_ms++;
		if ((wait_ms >= USBH_TEARDOWN_WAIT_FIRST_LOG_MS) && (((wait_ms - USBH_TEARDOWN_WAIT_FIRST_LOG_MS) % USBH_TEARDOWN_WAIT_LOG_MS) == 0U)) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Wait TX exit %dms\n", wait_ms);
		}
	}
}

/**
  * @brief  Send one frame, called by usbh_rndis_transmit() inside the TX gate
  * @param  buf: Ethernet frame
  * @param  len: Frame length
  * @param  block: Wait until the transfer completes
  * @retval Status
  */
static int usbh_rndis_tx_frame(const u8 *buf, u32 len, u8 block)
{
	usbh_rndis_host_t *cdc = &usbh_rndis_host;
	usbh_pipe_t *pipe = &cdc->bulk_tx.pipe;
	usb_rndis_packet_msg_t *pkt = (usb_rndis_packet_msg_t *)(void *)cdc->tx_buf;
	u32 msg_len = USB_RNDIS_PACKET_MSG_HDR_SIZE + len;
	u32 retry_cnt = 0U;
	int ret = -RTK_ERR_BUSY;

	if ((cdc->dev_max_xfer != 0U) && (msg_len > cdc->dev_max_xfer)) {
		return -RTK_ERR_BADARG;
	}

	/* One sender at a time and tx_buf is single: wait for the previous transfer to retire */
	while (pipe->xfer_state != USBH_PIPE_XFER_IDLE) {
		if ((block == 0U) || (++retry_cnt > 100U) || (!usbh_rndis_data_ready())) {
			return -RTK_ERR_BUSY;
		}
		usb_os_sleep_ms(1U);
	}

	usb_os_memset((void *)pkt, 0, USB_RNDIS_PACKET_MSG_HDR_SIZE);
	pkt->MessageType = USB_RNDIS_MSG_PACKET;
	pkt->MessageLength = msg_len;
	pkt->DataOffset = USB_RNDIS_PACKET_DATA_OFFSET;
	pkt->DataLength = len;
	usb_os_memcpy((void *)(cdc->tx_buf + USB_RNDIS_PACKET_MSG_HDR_SIZE), (const void *)buf, len);

	if (block != 0U) {
		cdc->bulk_tx_err = 0U;
		/* Drop a give left over from a TX nobody waited for */
		usb_os_sema_take(cdc->bulk_tx_sema, 0U);
		cdc->bulk_tx_block = 1U;
	}

	/* Published before START: the SOF on another core notifies the host task once it sees START */
	usb_os_enter_critical();
	if ((usbh_rndis_data_ready()) && (pipe->pipe_num != 0U) && (pipe->ep_mps != 0U) && (pipe->xfer_state == USBH_PIPE_XFER_IDLE)) {
		pipe->xfer_buf = cdc->tx_buf;
		pipe->xfer_len = msg_len;
		/* Ref RNDIS USB 3.1: a transfer of N*MPS bytes is ended by a ZLP */
		pipe->trx_zlp = ((msg_len % pipe->ep_mps) == 0U) ? 1U : 0U;
		pipe->xfer_state = USBH_PIPE_XFER_START;
		ret = RTK_SUCCESS;
	}
	usb_os_exit_critical();

	if (ret == RTK_SUCCESS) {
		usbh_notify(cdc->host, pipe->pipe_num, &usbh_rndis_driver);
		if (block != 0U) {
			/* No timeout: every submitted TX ends in tx_done() or detach, and returning earlier
			   would let the next call overwrite tx_buf under the DMA. Deinit ends it too. */
			while ((usb_os_sema_take(cdc->bulk_tx_sema, USB_OS_SEMA_TIMEOUT) != RTK_SUCCESS) && (cdc->tx_open != 0U)) {
				usb_os_sleep_ms(1U);
			}
			if (cdc->bulk_tx_err != 0U) {
				ret = -RTK_ERR_IO;
			}
		}
	}

	cdc->bulk_tx_block = 0U;

	return ret;
}

/* Exported functions --------------------------------------------------------*/

int usbh_rndis_init(const usbh_rndis_cb_t *cb, const usbh_rndis_config_t *cfg)
{
	usbh_rndis_host_t *cdc = &usbh_rndis_host;
	int ret;

	if (cb == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid user CB\n");
		return -RTK_ERR_BADARG;
	}

	/* ctrl_buf only exists between a successful init and deinit */
	if (cdc->ctrl_buf != NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Already init\n");
		return -RTK_ERR_BADARG;
	}

	usb_os_memset((void *)cdc, 0x00, sizeof(usbh_rndis_host_t));
	cdc->packet_filter = ((cfg != NULL) && (cfg->packet_filter != 0U)) ? cfg->packet_filter : USBH_RNDIS_DEFAULT_PACKET_FILTER;

	if (usb_os_sema_create(&cdc->bulk_tx_sema) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "TX sema create fail\n");
		return -RTK_ERR_NOMEM;
	}

	ret = -RTK_ERR_NOMEM;
	/* A D2H request reserves whole EP0 packets: the size is a multiple of 64 */
	cdc->ctrl_buf = (u8 *)usb_os_malloc(USBH_RNDIS_CTRL_BUF_SIZE);
	cdc->tx_buf = (u8 *)usb_os_malloc(USBH_RNDIS_TX_BUF_SIZE);
	if ((cdc->ctrl_buf == NULL) || (cdc->tx_buf == NULL)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Alloc buf fail\n");
		goto init_fail;
	}

	if (cb->init != NULL) {
		ret = cb->init();
		if (ret != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "CB init fail\n");
			goto init_fail;
		}
	}

	cdc->cb = cb;

	ret = usbh_class_register(&usbh_rndis_driver);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Register class fail %d\n", ret);
		cdc->cb = NULL;
		if (cb->deinit != NULL) {
			cb->deinit();
		}
		goto init_fail;
	}

	/* Every TX resource exists from here on: let the TX API in */
	usb_os_enter_critical();
	cdc->tx_open = 1U;
	usb_os_exit_critical();

	return RTK_SUCCESS;

init_fail:
	USBH_RNDIS_FREE_MEM(cdc->tx_buf);
	USBH_RNDIS_FREE_MEM(cdc->ctrl_buf);
	usb_os_sema_delete(cdc->bulk_tx_sema);
	cdc->bulk_tx_sema = NULL;

	return ret;
}

void usbh_rndis_deinit(void)
{
	usbh_rndis_host_t *cdc = &usbh_rndis_host;
	const usbh_rndis_cb_t *cb;

	/* Close the TX gate first: frees nothing */
	usb_os_enter_critical();
	cdc->tx_open = 0U;
	usb_os_exit_critical();

	/* On RTK_SUCCESS no callback of this class runs any more */
	if (usbh_class_unregister(&usbh_rndis_driver) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Deinit refused\n");
		return;
	}

	cdc->state = USBH_RNDIS_STATE_IDLE;
	cdc->link_up = 0U;

	/* Halt the channels before a blocked sender is released */
	usbh_rndis_deinit_all_pipe();
	cdc->bulk_tx_err = 1U;
	usbh_rndis_tx_wait_exit();
	usb_os_sema_delete(cdc->bulk_tx_sema);
	cdc->bulk_tx_sema = NULL;
	cdc->host = NULL;

	USBH_RNDIS_FREE_MEM(cdc->tx_buf);
	USBH_RNDIS_FREE_MEM(cdc->ctrl_buf);

	cb = cdc->cb;
	cdc->cb = NULL;
	if ((cb != NULL) && (cb->deinit != NULL)) {
		cb->deinit();
	}
}

int usbh_rndis_choose_config(usb_host_t *host)
{
	usbh_dev_id_t dev_id;
	u8 cfg_idx = 0U;
	u8 i;

	/* The first configuration carrying any of the RNDIS triplets, e.g. not the CDC ECM one of a
	   Linux gadget offering both */
	for (i = 0U; usbh_rndis_devs[i].mMatchFlags != 0U; i++) {
		usb_os_memcpy((void *)&dev_id, (const void *)&usbh_rndis_devs[i], sizeof(dev_id));
		if (usbh_itf_find(host, &dev_id) != NULL) {
			cfg_idx = usbh_config_find(host, &dev_id);
			break;
		}
	}

	return usbh_config_select(host, cfg_idx);
}

int usbh_rndis_check_config(usb_host_t *host)
{
	usbh_itf_t *ctrl_itf;
	usbh_itf_desc_t *data_itf_desc;

	return usbh_rndis_find_function(host, &ctrl_itf, &data_itf_desc);
}

bool usbh_rndis_is_ready(void)
{
	return usbh_rndis_data_ready();
}

int usbh_rndis_transmit(const u8 *buf, u32 len, u8 block)
{
	usbh_rndis_host_t *cdc = &usbh_rndis_host;
	int ret;

	if (usb_os_is_in_interrupt()) {
		return -RTK_ERR_BADARG;
	}

	if ((buf == NULL) || (len == 0U) || (len > USB_RNDIS_MAX_FRAME_SIZE)) {
		return -RTK_ERR_BADARG;
	}

	/* Counted under the same lock deinit closes the gate with */
	usb_os_enter_critical();
	if (cdc->tx_open == 0U) {
		usb_os_exit_critical();
		return -RTK_ERR_IO;
	}
	cdc->tx_users++;
	usb_os_exit_critical();

	ret = (usbh_rndis_data_ready()) ? usbh_rndis_tx_frame(buf, len, block) : -RTK_ERR_IO;

	usb_os_enter_critical();
	cdc->tx_users--;
	usb_os_exit_critical();

	return ret;
}

bool usbh_rndis_get_link_status(void)
{
	return (usbh_rndis_data_ready() && (usbh_rndis_host.link_up != 0U));
}

const u8 *usbh_rndis_get_mac(void)
{
	usbh_rndis_host_t *cdc = &usbh_rndis_host;

	return (cdc->mac_valid != 0U) ? cdc->mac : NULL;
}

__attribute__((weak))
int usb_ethernet_transmit(u8 *buf, u32 len, u8 block)
{
	return usbh_rndis_transmit(buf, len, block);
}
