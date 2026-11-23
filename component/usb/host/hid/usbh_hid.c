/*
 * Copyright (c) 2025 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbh_hid.h"

/* Private defines -----------------------------------------------------------*/

#define USBH_HID_CTRL_BUF_LEN        USBH_HID_REPORT_MAX_LEN /**< EP0 buffer of the class requests, one EP0 packet */
#define USBH_HID_TRIGGER_MAX_CNT     50U     /**< SOF ticks after which an idle INTR IN pipe is driven again */
#define USBH_HID_THREAD_PRIORITY     3U      /**< Report thread priority */
#define USBH_HID_THREAD_STACK_SIZE   768U    /**< Report thread stack size */
#define USBH_HID_RING_NODE_CNT       10U     /**< Reports queued for the report thread, all interfaces together */
#define USBH_HID_RING_NODE_LEN       (USBH_HID_REPORT_MAX_LEN + 1U) /**< Interface index, then the report */

/* Per interface setup steps */
#define USBH_HID_SETUP_GET_REPORT_DESC  0U
#define USBH_HID_SETUP_SET_IDLE         1U
#define USBH_HID_SETUP_NOTIFY           2U
#define USBH_HID_SETUP_SET_PROTOCOL     3U
#define USBH_HID_SETUP_DONE             4U

/* Application request on EP0 */
#define USBH_HID_REQ_IDLE            0U      /**< No request */
#define USBH_HID_REQ_PENDING         1U      /**< Posted by the application, not taken by the host task yet */
#define USBH_HID_REQ_BUSY            2U      /**< Issued by the host task */

/* Private types -------------------------------------------------------------*/

/**
 * @brief HID Report Item, Ref HID 1.11 6.2.2.2.
 */
typedef struct {
	u32 data;                   /**< Item Data */
	u8 type;                    /**< Item Type (Main, Global, Local) */
	u8 tag;                     /**< Item Tag */
	u8 size;                    /**< Item Size */
} usbh_hid_item_t;

/**
 * @brief One HID interface of the attached device.
 */
typedef struct {
	usbh_pipe_t pipe_in;                /**< Interrupt IN pipe */
	usbh_pipe_t pipe_out;               /**< Interrupt OUT pipe, pipe_num 0 if absent */
	usbh_ep_desc_t ep_desc_in;          /**< Interrupt IN Endpoint Descriptor */
	usbh_ep_desc_t ep_desc_out;         /**< Interrupt OUT Endpoint Descriptor, optional */
	u8 *in_buf;                         /**< INTR IN buffer, the DMA window of one packet */
	u8 *report_desc;                    /**< Report descriptor, kept until detach */
	usbh_hid_itf_info_t info;           /**< What the application is told in setup() */
	usbh_dev_hid_desc_t hid_desc;       /**< HID Descriptor */
	u8 setup_step;                      /**< USBH_HID_SETUP_xxx */
	u8 halt_pending;                    /**< INTR IN STALLed, CLEAR_FEATURE(ENDPOINT_HALT) to be sent */
	u8 halt_retry_cnt;                  /**< Failed CLEAR_FEATURE(ENDPOINT_HALT) attempts */
	u8 report_cut_logged;               /**< A report longer than USBH_HID_REPORT_MAX_LEN was reported for this attach */
} usbh_hid_itf_t;

/**
 * @brief HID host class handle.
 */
typedef struct {
	usb_host_t *host;                         /**< USB host handle, set at attach */
	const usbh_hid_usr_cb_t *cb;              /**< User callbacks */

	/* Interfaces of the attached device */
	usbh_hid_itf_t itf[USBH_HID_MAX_ITF_NUM]; /**< HID interfaces, itf_cnt of them in use */
	u8 itf_cnt;                               /**< HID interfaces in use */
	u8 setup_idx;                             /**< Interface setup() is working on */
	__IO u8 ready;                            /**< 1 once setup() is done: reports flow and the class requests are accepted */

	/* Application class request on EP0, served by the host task */
	usb_os_lock_t req_lock;                   /**< One application request at a time */
	usb_os_sema_t req_done_sema;              /**< Given by the host task when the request ends */
	u8 *ctrl_buf;                             /**< EP0 data stage of the class requests */
	usbh_setup_req_t req_setup;               /**< Request to issue */
	u32 req_rx_len;                           /**< Bytes received by a D2H request */
	int req_status;                           /**< Result of the request */
	__IO u8 req_state;                        /**< USBH_HID_REQ_xxx */

	/* Report thread */
	rtos_task_t report_task;                  /**< Report thread handle */
	usb_os_sema_t report_sema;                /**< Wakes the report thread when a report is queued */
	usb_ringbuf_manager_t report_msg;         /**< Queued reports, interface index first */
	u8 rx_node[USBH_HID_RING_NODE_LEN];       /**< Host task staging of one node */
	u8 report_buf[USBH_HID_RING_NODE_LEN];    /**< Report thread copy of one node, kept off its stack */
	__IO u8 report_task_alive;                /**< Report thread is running */
	__IO u8 report_task_exit;                 /**< Asks the report thread to leave */
} usbh_hid_t;

/**
 * @brief Report descriptor walk state, Ref HID 1.11 6.2.2.
 */
typedef struct {
	s32 logical_min;            /**< Current Logical Minimum */
	s32 logical_max;            /**< Current Logical Maximum */
	u32 usage_min;              /**< Pending Usage Minimum */
	u32 found_idx;              /**< Index of the requested usage among the pending usages */
	u32 usage_cnt;              /**< Usages declared for the pending Main item */
	u32 in_bit_offset;          /**< Bits used by the Input fields of the current report */
	u32 out_bit_offset;         /**< Bits used by the Output fields of the current report */
	u16 usage_page;             /**< Current Usage Page */
	u16 report_size;            /**< Current Report Size (bits) */
	u16 report_count;           /**< Current Report Count */
	u8 report_id;               /**< Current Report ID */
	u8 usage_min_valid;         /**< A Usage Minimum is pending a Usage Maximum */
	u8 found;                   /**< The requested usage is among the pending usages */
} usbh_hid_walk_t;

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static const u8 *usbh_hid_fetch_item(const u8 *start, const u8 *end, usbh_hid_item_t *item);
static void usbh_hid_walk_local(usbh_hid_walk_t *walk, const usbh_hid_item_t *item, u16 usage_page, u16 usage);
static int usbh_hid_parse_interface(usbh_hid_itf_t *itf, const usbh_itf_data_t *itf_data);
static void usbh_hid_add_interface(const usbh_itf_data_t *itf_data, u8 *ignored);
static int usbh_hid_collect_interfaces(usb_host_t *host);
static int usbh_hid_open_interface(usb_host_t *host, usbh_hid_itf_t *itf);
static void usbh_hid_close_interface(usb_host_t *host, usbh_hid_itf_t *itf);
static int usbh_hid_setup_interface(usb_host_t *host, usbh_hid_itf_t *itf, u8 idx);
static void usbh_hid_in_process(usb_host_t *host, u8 idx);
static void usbh_hid_out_process(usb_host_t *host, usbh_hid_itf_t *itf);
static void usbh_hid_process_halt(usb_host_t *host, usbh_hid_itf_t *itf);
static void usbh_hid_process_request(usb_host_t *host);
static int usbh_hid_attach(usb_host_t *host);
static void usbh_hid_detach(usb_host_t *host);
static int usbh_hid_setup(usb_host_t *host);
static void usbh_hid_process(usb_host_t *host, usbh_drv_msg_t *msg);
static void usbh_hid_sof(usb_host_t *host);
static int usbh_hid_ctrl_sync(u8 idx, u8 bm_request_type, u8 request, u16 value, const u8 *tx, u8 *rx, u32 *len);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "HID";

/* USB HID device identification: every HID interface, the application tells them apart */
static const usbh_dev_id_t hid_devs[] = {
	{
		.mMatchFlags = USBH_DEV_ID_MATCH_ITF_CLASS,
		.bInterfaceClass = USBH_CLASS_HID,
	},
	{
	},
};

/* USB Class Driver */
static const usbh_class_driver_t usbh_hid_driver = {
	.id_table = hid_devs,
	.attach = usbh_hid_attach,
	.detach = usbh_hid_detach,
	.setup = usbh_hid_setup,
	.process = usbh_hid_process,
	.sof = usbh_hid_sof,
};

static usbh_hid_t usbh_hid;

/* Exported variables --------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Decode one short item, Ref HID 1.11 6.2.2.2
  * @param  start: First byte of the item
  * @param  end: End of the descriptor
  * @param  item: Decoded item
  * @retval First byte of the next item, NULL at the end or for a truncated item
  */
static const u8 *usbh_hid_fetch_item(const u8 *start, const u8 *end, usbh_hid_item_t *item)
{
	u8 b;

	if (start >= end) {
		return NULL;
	}

	b = *start++;
	item->type = (u8)((b >> 2) & 0x3U);
	item->tag = (u8)((b >> 4) & 0xFU);
	item->size = (u8)(b & 0x3U);
	if (item->size == 3U) {
		item->size = 4U;
	}

	/* Item data is little endian. Cast to u32 before the shift: a u8 operand would be promoted to
	 * signed int, and shifting the top byte of a 4-byte item left by 24 would overflow the sign bit. */
	item->data = 0U;
	for (u32 i = 0U; i < (u32)item->size; i++) {
		if (start >= end) {
			return NULL;
		}
		item->data |= ((u32)(*start)) << (8U * i);
		start++;
	}

	return start;
}

/**
  * @brief  Track the usages declared for the next Main item, Ref HID 1.11 6.2.2.8
  * @note   Local items apply to the next Main item only, which tells whether the field is Variable or
  *         Array, so only the index of the requested usage among the declared ones is kept here.
  * @param  walk: Walk state
  * @param  item: Local item
  * @param  usage_page: Requested Usage Page
  * @param  usage: Requested Usage ID
  * @retval None
  */
static void usbh_hid_walk_local(usbh_hid_walk_t *walk, const usbh_hid_item_t *item, u16 usage_page, u16 usage)
{
	u32 u = item->data;
	u16 page = walk->usage_page;

	/* A 4-byte usage carries its own Usage Page in the upper half, Ref HID 1.11 6.2.2.8 */
	if (item->size == 4U) {
		page = (u16)(u >> 16);
		u &= 0xFFFFU;
	}

	switch (item->tag) {
	case USBH_HID_LOCAL_ITEM_TAG_USAGE:
		if ((walk->found == 0U) && (page == usage_page) && (u == usage)) {
			walk->found = 1U;
			walk->found_idx = walk->usage_cnt;
		}
		walk->usage_cnt++;
		break;

	case USBH_HID_LOCAL_ITEM_TAG_USAGE_MIN:
		walk->usage_min = u;
		walk->usage_min_valid = 1U;
		break;

	case USBH_HID_LOCAL_ITEM_TAG_USAGE_MAX:
		/* Usage Minimum/Maximum declare an inclusive range, the same as listing each usage in order */
		if ((walk->usage_min_valid != 0U) && (u >= walk->usage_min)) {
			if ((walk->found == 0U) && (page == usage_page) && ((u32)usage >= walk->usage_min) && ((u32)usage <= u)) {
				walk->found = 1U;
				walk->found_idx = walk->usage_cnt + ((u32)usage - walk->usage_min);
			}
			walk->usage_cnt += (u - walk->usage_min) + 1U;
		}
		walk->usage_min_valid = 0U;
		break;

	default:
		break;
	}
}

/**
  * @brief  Parse one HID interface: its HID descriptor and Interrupt endpoints
  * @note   Only alt setting 0 is parsed: SET_CONFIGURATION selects it (Ref USB 2.0 9.1.1.5) and HID
  *         defines no other (Ref HID 1.11 4.1). bLength is checked before each descriptor is read.
  * @param  itf: Interface to fill
  * @param  itf_data: Interface descriptors
  * @retval RTK_SUCCESS, -RTK_ERR_BADARG for an interface without Interrupt IN endpoint
  */
static int usbh_hid_parse_interface(usbh_hid_itf_t *itf, const usbh_itf_data_t *itf_data)
{
	const usbh_itf_desc_t *itf_desc = &(itf_data->itf_desc_array[0]);
	const u8 *desc = itf_data->raw_data;
	const usbh_ep_desc_t *ep_desc;
	u32 offset = 0U;
	u8 len;

	itf->info.itf_num = itf_desc->bInterfaceNumber;
	itf->info.subclass = itf_desc->bInterfaceSubClass;
	itf->info.protocol = itf_desc->bInterfaceProtocol;

	while ((offset + sizeof(usbh_desc_header_t)) <= (u32)itf_data->raw_data_len) {
		len = desc[offset];
		if ((len < sizeof(usbh_desc_header_t)) || ((offset + len) > (u32)itf_data->raw_data_len)) {
			break;
		}

		switch (desc[offset + 1U]) {
		case USB_DESC_TYPE_INTERFACE:
			/* Another alt setting or interface: its endpoints are not active */
			if ((offset != 0U) && ((((const usbh_itf_desc_t *)(const void *)&desc[offset])->bInterfaceNumber != itf->info.itf_num) ||
								   (((const usbh_itf_desc_t *)(const void *)&desc[offset])->bAlternateSetting != 0U))) {
				offset = itf_data->raw_data_len;
				continue;
			}
			break;

		case USBH_HID_DESC:
			/* At least 9 bytes, the fields read, Ref HID 1.11 6.2.1 */
			if (len >= USBH_HID_DESC_LEN) {
				usb_os_memcpy((void *) & (itf->hid_desc), (const void *)&desc[offset], sizeof(usbh_dev_hid_desc_t));
			} else {
				RTK_LOGS(TAG, RTK_LOG_WARN, "Short HID desc %d\n", len);
			}
			break;

		case USB_DESC_TYPE_ENDPOINT:
			/* HID uses Interrupt endpoints only, one per direction, Ref HID 1.11 4.4 */
			if (len < USB_LEN_EP_DESC) {
				break;
			}
			ep_desc = (const usbh_ep_desc_t *)(const void *)&desc[offset];
			if ((ep_desc->bmAttributes & USB_EP_XFER_TYPE_MASK) != USB_CH_EP_TYPE_INTR) {
				RTK_LOGS(TAG, RTK_LOG_WARN, "Skip EP%02x type %d\n", ep_desc->bEndpointAddress, ep_desc->bmAttributes);
			} else if (USB_EP_IS_IN(ep_desc->bEndpointAddress)) {
				if (itf->ep_desc_in.bEndpointAddress == 0U) {
					usb_os_memcpy((void *) & (itf->ep_desc_in), (const void *)ep_desc, sizeof(usbh_ep_desc_t));
				}
			} else {
				if (itf->ep_desc_out.bEndpointAddress == 0U) {
					usb_os_memcpy((void *) & (itf->ep_desc_out), (const void *)ep_desc, sizeof(usbh_ep_desc_t));
				}
			}
			break;

		default:
			break;
		}

		offset += len;
	}

	/* The Interrupt IN endpoint is mandatory, Ref HID 1.11 4.4 */
	if (itf->ep_desc_in.bEndpointAddress == 0U) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "Itf %d no INTR IN\n", itf->info.itf_num);
		return -RTK_ERR_BADARG;
	}

	return RTK_SUCCESS;
}

/**
  * @brief  Add one HID interface unless it is collected already
  * @param  itf_data: Interface descriptors
  * @param  ignored: Counts the interfaces beyond USBH_HID_MAX_ITF_NUM
  * @retval None
  */
static void usbh_hid_add_interface(const usbh_itf_data_t *itf_data, u8 *ignored)
{
	usbh_hid_t *hid = &usbh_hid;
	usbh_hid_itf_t *itf;
	u8 itf_num = itf_data->itf_desc_array[0].bInterfaceNumber;

	for (u8 i = 0U; i < hid->itf_cnt; i++) {
		if (hid->itf[i].info.itf_num == itf_num) {
			return;
		}
	}

	if (hid->itf_cnt >= USBH_HID_MAX_ITF_NUM) {
		(*ignored)++;
		return;
	}

	itf = &(hid->itf[hid->itf_cnt]);
	if (usbh_hid_parse_interface(itf, itf_data) == RTK_SUCCESS) {
		hid->itf_cnt++;
	} else {
		usb_os_memset((void *)itf, 0, sizeof(usbh_hid_itf_t));
		RTK_LOGS(TAG, RTK_LOG_WARN, "Skip itf %d\n", itf_num);
	}
}

/**
  * @brief  Find the HID interfaces of the active configuration
  * @note   The core hands every HID interface of the device to this class. Interfaces of one
  *         subclass/protocol are chained, so each chain HID 1.11 4.2/4.3 defines is walked; a
  *         non-standard encoding, e.g. subclass 0xFF, is reached by a match on the class alone.
  * @param  host: Host handle
  * @retval RTK_SUCCESS if at least one is usable, -RTK_ERR_BADARG otherwise
  */
static int usbh_hid_collect_interfaces(usb_host_t *host)
{
	usbh_hid_t *hid = &usbh_hid;
	usbh_dev_id_t dev_id = {0,};
	usbh_itf_data_t *itf_data;
	u8 ignored = 0U;

	dev_id.bInterfaceClass = USBH_CLASS_HID;
	dev_id.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO;

	/* Boot keyboard, boot mouse and non-boot interfaces form one chain each */
	for (u8 sub = 0U; sub <= USBH_HID_SUBCLASS_BOOT; sub++) {
		for (u8 proto = 0U; proto <= USB_HID_PROTOCOL_MOUSE; proto++) {
			dev_id.bInterfaceSubClass = sub;
			dev_id.bInterfaceProtocol = proto;
			for (itf_data = usbh_get_interface_descriptor(host, &dev_id); itf_data != NULL; itf_data = itf_data->next) {
				usbh_hid_add_interface(itf_data, &ignored);
			}
		}
	}

	/* Room left: the first HID interface whatever its subclass and protocol, and the ones chained to it.
	 * Not once full, it could only count again an interface the walk above already ignored. */
	if (hid->itf_cnt < USBH_HID_MAX_ITF_NUM) {
		dev_id.mMatchFlags = USBH_DEV_ID_MATCH_ITF_CLASS;
		for (itf_data = usbh_get_interface_descriptor(host, &dev_id); itf_data != NULL; itf_data = itf_data->next) {
			usbh_hid_add_interface(itf_data, &ignored);
		}
	}

	if (ignored != 0U) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "%d HID itf over %d ignored\n", ignored, USBH_HID_MAX_ITF_NUM);
	}

	return (hid->itf_cnt != 0U) ? RTK_SUCCESS : -RTK_ERR_BADARG;
}

/**
  * @brief  Open the pipes of one interface
  * @param  host: Host handle
  * @param  itf: Interface
  * @retval RTK_SUCCESS, or the error with nothing left open
  */
static int usbh_hid_open_interface(usb_host_t *host, usbh_hid_itf_t *itf)
{
	u16 mps = itf->ep_desc_in.wMaxPacketSize & USB_EP_MPS_SIZE_MASK;

	if (mps == 0U) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Itf %d IN MPS 0\n", itf->info.itf_num);
		return -RTK_ERR_BADARG;
	}

	/* Every IN transaction lands in in_buf and the controller writes a whole packet, so in_buf
	 * covers the DMA window of one packet, Ref usb_get_dma_len() */
	itf->in_buf = (u8 *)usb_os_malloc(usb_get_dma_len(mps, mps));
	if (itf->in_buf == NULL) {
		return -RTK_ERR_NOMEM;
	}

	if (usbh_open_pipe(host, &(itf->pipe_in), &(itf->ep_desc_in), &usbh_hid_driver) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Open itf %d IN fail\n", itf->info.itf_num);
		usbh_hid_close_interface(host, itf);
		return -RTK_ERR_BADARG;
	}
	itf->pipe_in.xfer_buf = itf->in_buf;
	itf->pipe_in.xfer_buf_len = usb_get_dma_len(mps, mps);

	if (itf->ep_desc_out.bEndpointAddress != 0U) {
		if (usbh_open_pipe(host, &(itf->pipe_out), &(itf->ep_desc_out), &usbh_hid_driver) != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Open itf %d OUT fail\n", itf->info.itf_num);
			usbh_hid_close_interface(host, itf);
			return -RTK_ERR_BADARG;
		}
		itf->info.has_out_ep = 1U;
	}

	return RTK_SUCCESS;
}

/**
  * @brief  Close the pipes of one interface and free its buffers
  * @note   Host task only, or once the class is unregistered. Safe on a half opened interface.
  * @param  host: Host handle
  * @param  itf: Interface
  * @retval None
  */
static void usbh_hid_close_interface(usb_host_t *host, usbh_hid_itf_t *itf)
{
	/* Closed before the buffers go: the controller may still write in_buf */
	if (itf->pipe_in.pipe_num != 0U) {
		usbh_close_pipe(host, &(itf->pipe_in));
	}
	if (itf->pipe_out.pipe_num != 0U) {
		usbh_close_pipe(host, &(itf->pipe_out));
	}
	usb_os_mfree((void *)itf->in_buf);
	usb_os_mfree((void *)itf->report_desc);
	usb_os_memset((void *)itf, 0, sizeof(usbh_hid_itf_t));
}

/**
  * @brief  One step of the setup of an interface: GET_DESCRIPTOR(Report), SET_IDLE(0), the user setup(),
  *         then SET_PROTOCOL(Boot) if it asks for it
  * @note   No request is fatal: a refused Report descriptor leaves the application with the raw reports,
  *         SET_IDLE is optional for any device but a boot keyboard (Ref HID 1.11 7.2.4), and a refused
  *         SET_PROTOCOL leaves the Report protocol.
  * @param  host: Host handle
  * @param  itf: Interface
  * @param  idx: Interface index
  * @retval RTK_SUCCESS once done, -RTK_ERR_BUSY while a request is in flight
  */
static int usbh_hid_setup_interface(usb_host_t *host, usbh_hid_itf_t *itf, u8 idx)
{
	usbh_hid_t *hid = &usbh_hid;
	u8 protocol = USBH_HID_PROTOCOL_REPORT;
	usbh_setup_req_t setup;
	u16 len = itf->hid_desc.wDescriptorLength;
	u32 rx_len;
	int ret;

	switch (itf->setup_step) {
	case USBH_HID_SETUP_GET_REPORT_DESC:
		/* The first class descriptor is the Report descriptor, Ref HID 1.11 6.2.1 */
		if ((itf->hid_desc.bNumDescriptors == 0U) || (itf->hid_desc.bReportDescriptorType != USBH_HID_REPORT_TYPE) || (len == 0U)) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "Itf %d no report desc\n", itf->info.itf_num);
			itf->setup_step = USBH_HID_SETUP_SET_IDLE;
			return -RTK_ERR_BUSY;
		}
		/* wDescriptorLength may reach 65535: the buffer covers the whole EP0 DMA window */
		if (itf->report_desc == NULL) {
			itf->report_desc = (u8 *)usb_os_malloc(usb_get_dma_len(len, USB_MAX_EP0_SIZE));
			if (itf->report_desc == NULL) {
				itf->setup_step = USBH_HID_SETUP_SET_IDLE;
				return -RTK_ERR_BUSY;
			}
		}
		setup.req.bmRequestType = USB_D2H | USB_REQ_TYPE_STANDARD | USB_REQ_RECIPIENT_INTERFACE;
		setup.req.bRequest = USB_REQ_GET_DESCRIPTOR;
		setup.req.wValue = (u16)((u16)USBH_HID_REPORT_TYPE << 8);
		setup.req.wIndex = itf->info.itf_num;
		setup.req.wLength = len;
		ret = usbh_ctrl_request(host, &setup, itf->report_desc);
		if (ret == RTK_SUCCESS) {
			/* A short reply leaves stale bytes past what was received */
			rx_len = usbh_get_last_ctrl_in_size(host);
			itf->info.report_desc = itf->report_desc;
			itf->info.report_desc_len = (u16)((rx_len < len) ? rx_len : len);
			itf->setup_step = USBH_HID_SETUP_SET_IDLE;
		} else if (ret != -RTK_ERR_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "Itf %d report desc err %d\n", itf->info.itf_num, ret);
			usb_os_mfree((void *)itf->report_desc);
			itf->report_desc = NULL;
			itf->setup_step = USBH_HID_SETUP_SET_IDLE;
		} else {
			/* In flight */
		}
		return -RTK_ERR_BUSY;

	case USBH_HID_SETUP_SET_IDLE:
		/* Duration 0: a report only when the data changes, for every Report ID, Ref HID 1.11 7.2.4 */
		setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
		setup.req.bRequest = USBH_HID_SET_IDLE;
		setup.req.wValue = 0U;
		setup.req.wIndex = itf->info.itf_num;
		setup.req.wLength = 0U;
		ret = usbh_ctrl_request(host, &setup, NULL);
		if (ret == -RTK_ERR_BUSY) {
			return ret;
		}
		if (ret != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "Itf %d SET_IDLE err %d\n", itf->info.itf_num, ret);
		}
		itf->setup_step = USBH_HID_SETUP_NOTIFY;
		return -RTK_ERR_BUSY;

	case USBH_HID_SETUP_NOTIFY:
		if ((hid->cb != NULL) && (hid->cb->setup != NULL)) {
			protocol = hid->cb->setup(idx, &(itf->info));
		}
		/* Only a boot interface knows the Boot protocol, Ref HID 1.11 7.2.6 */
		if ((protocol == USBH_HID_PROTOCOL_BOOT) && (itf->info.subclass == USBH_HID_SUBCLASS_BOOT)) {
			itf->setup_step = USBH_HID_SETUP_SET_PROTOCOL;
			return -RTK_ERR_BUSY;
		}
		itf->setup_step = USBH_HID_SETUP_DONE;
		return RTK_SUCCESS;

	case USBH_HID_SETUP_SET_PROTOCOL:
		setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
		setup.req.bRequest = USBH_HID_SET_PROTOCOL;
		setup.req.wValue = USBH_HID_PROTOCOL_BOOT;
		setup.req.wIndex = itf->info.itf_num;
		setup.req.wLength = 0U;
		ret = usbh_ctrl_request(host, &setup, NULL);
		if (ret == -RTK_ERR_BUSY) {
			return ret;
		}
		if (ret != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "Itf %d boot protocol err %d\n", itf->info.itf_num, ret);
		}
		itf->setup_step = USBH_HID_SETUP_DONE;
		return RTK_SUCCESS;

	default:
		return RTK_SUCCESS;
	}
}

/**
  * @brief  Drive the INTR IN pipe of one interface
  * @param  host: Host handle
  * @param  idx: Interface index
  * @retval None
  */
static void usbh_hid_in_process(usb_host_t *host, u8 idx)
{
	usbh_hid_t *hid = &usbh_hid;
	usbh_hid_itf_t *itf = &(hid->itf[idx]);
	usbh_pipe_t *pipe = &(itf->pipe_in);
	usbh_urb_state_t urb_state;
	u32 len;
	int ret;

	switch (pipe->xfer_state) {
	case USBH_EP_XFER_START:
		if (usbh_get_elapsed_ticks(host, pipe->tick) > pipe->ep_interval) {
			pipe->tick = usbh_get_tick(host);
			pipe->xfer_len = pipe->ep_mps;
			/* -RTK_ERR_BUSY: the previous poll completed before its channel could be halted, its
			 * report follows as URB_DONE and is read in BUSY, as usbh_transfer_process() does */
			ret = usbh_transfer_data(host, pipe);
			if ((ret == RTK_SUCCESS) || (ret == -RTK_ERR_BUSY)) {
				pipe->xfer_state = USBH_EP_XFER_BUSY;
			}
		}
		break;

	case USBH_EP_XFER_BUSY:
		urb_state = usbh_get_urb_state(host, pipe);
		if (urb_state == USBH_URB_DONE) {
			len = usbh_get_last_transfer_size(host, pipe);
			if (len > USBH_HID_REPORT_MAX_LEN) {
				/* Once per attach, the device would repeat it on every poll */
				if (itf->report_cut_logged == 0U) {
					itf->report_cut_logged = 1U;
					RTK_LOGS(TAG, RTK_LOG_WARN, "Itf %d report %d cut to %d\n", itf->info.itf_num, len, USBH_HID_REPORT_MAX_LEN);
				}
				len = USBH_HID_REPORT_MAX_LEN;
			}
			/* A node holds the interface index, then the report. The producer is this host task only. */
			hid->rx_node[0] = idx;
			usb_os_memcpy((void *) & (hid->rx_node[1]), (const void *)pipe->xfer_buf, len);
			if (usb_ringbuf_add_tail(&(hid->report_msg), hid->rx_node, len + 1U, 1U) != RTK_SUCCESS) {
				RTK_LOGS(TAG, RTK_LOG_WARN, "Report drop\n");
			}
			usb_os_sema_give(hid->report_sema);
			pipe->xfer_state = USBH_EP_XFER_START;
		} else if (urb_state == USBH_URB_STALL) {
			/* A halted endpoint answers every IN with STALL until the host clears it, Ref USB 2.0
			 * 9.4.5: park the pipe until the halt is cleared */
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Itf %d INTR IN stall\n", itf->info.itf_num);
			pipe->xfer_state = USBH_EP_XFER_IDLE;
			itf->halt_pending = 1U;
			usbh_notify(host, 0U, &usbh_hid_driver);
		} else if (urb_state == USBH_URB_ERROR) {
			pipe->xfer_state = USBH_EP_XFER_START;
		} else if (usbh_get_elapsed_ticks(host, pipe->tick) > ((urb_state == USBH_URB_BUSY) ? pipe->ep_interval : (10U * pipe->ep_interval))) {
			/* NAKed or silent for a while: poll again */
			pipe->xfer_state = USBH_EP_XFER_START;
		} else {
			/* In flight */
		}
		break;

	default:
		break;
	}
}

/**
  * @brief  Drive the INTR OUT pipe of one interface
  * @param  host: Host handle
  * @param  itf: Interface
  * @retval None
  */
static void usbh_hid_out_process(usb_host_t *host, usbh_hid_itf_t *itf)
{
	usbh_pipe_t *pipe = &(itf->pipe_out);

	if (usbh_transfer_process(host, pipe) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Itf %d OUT fail %d\n", itf->info.itf_num, usbh_get_urb_state(host, pipe));
	}

	if (pipe->xfer_state == USBH_EP_XFER_START) {
		usbh_notify(host, pipe->pipe_num, &usbh_hid_driver);
	} else if (pipe->xfer_state == USBH_EP_XFER_ERROR) {
		pipe->xfer_state = USBH_EP_XFER_IDLE;
	} else {
		/* In flight or done */
	}
}

/**
  * @brief  Clear the halted INTR IN endpoint of an interface, re-enumerate the device if that keeps failing
  * @note   Host task only. CLEAR_FEATURE(ENDPOINT_HALT) resets the device data toggle to DATA0, Ref USB 2.0
  *         9.4.5, matching the host toggle the STALL interrupt already reset.
  * @param  host: Host handle
  * @param  itf: Interface
  * @retval None
  */
static void usbh_hid_process_halt(usb_host_t *host, usbh_hid_itf_t *itf)
{
	int ret;

	/* Re-enumeration already requested: wait for the detach it brings */
	if (itf->halt_retry_cnt >= USBH_HID_HALT_RETRY_MAX_CNT) {
		return;
	}

	ret = usbh_ctrl_clear_feature(host, itf->pipe_in.ep_addr);
	if (ret == RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "EP%02x halt cleared\n", itf->pipe_in.ep_addr);
		itf->halt_pending = 0U;
		itf->halt_retry_cnt = 0U;
		itf->pipe_in.xfer_state = USBH_EP_XFER_START;
		usbh_notify(host, itf->pipe_in.pipe_num, &usbh_hid_driver);
	} else if (ret != -RTK_ERR_BUSY) {
		itf->halt_retry_cnt++;
		if (itf->halt_retry_cnt >= USBH_HID_HALT_RETRY_MAX_CNT) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "EP%02x clear halt fail, re-enum\n", itf->pipe_in.ep_addr);
			ret = usbh_reenumerate();
			if (ret != RTK_SUCCESS) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Re-enum err %d\n", ret);
			}
		} else {
			usbh_notify(host, 0U, &usbh_hid_driver);
		}
	} else {
		/* In flight: the EP0 completion re-drives process() */
	}
}

/**
  * @brief  Issue the application class request, see usbh_hid_ctrl_sync()
  * @note   Host task only
  * @param  host: Host handle
  * @retval None
  */
static void usbh_hid_process_request(usb_host_t *host)
{
	usbh_hid_t *hid = &usbh_hid;
	int ret;

	if (hid->req_state == USBH_HID_REQ_PENDING) {
		hid->req_state = USBH_HID_REQ_BUSY;
	}

	ret = usbh_ctrl_request(host, &(hid->req_setup), hid->ctrl_buf);
	if (ret == -RTK_ERR_BUSY) {
		/* In flight, or EP0 owned by another class: the core drives this class again */
		return;
	}

	hid->req_rx_len = ((hid->req_setup.req.bmRequestType & USB_D2H) != 0U) ? usbh_get_last_ctrl_in_size(host) : 0U;
	hid->req_status = ret;
	hid->req_state = USBH_HID_REQ_IDLE;
	usb_os_sema_give(hid->req_done_sema);
}

/**
  * @brief  Attach callback
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_hid_attach(usb_host_t *host)
{
	usbh_hid_t *hid = &usbh_hid;
	int ret;

	hid->host = host;
	hid->setup_idx = 0U;
	/* The core runs no detach() after a failed attach(), so start from empty interfaces */
	hid->itf_cnt = 0U;
	usb_os_memset((void *)hid->itf, 0, sizeof(hid->itf));

	ret = usbh_hid_collect_interfaces(host);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "No usable HID itf\n");
		return ret;
	}

	for (u8 i = 0U; i < hid->itf_cnt; i++) {
		ret = usbh_hid_open_interface(host, &(hid->itf[i]));
		if (ret != RTK_SUCCESS) {
			/* Unwind the interfaces already opened */
			while (i > 0U) {
				i--;
				usbh_hid_close_interface(host, &(hid->itf[i]));
			}
			hid->itf_cnt = 0U;
			return ret;
		}
	}

	if ((hid->cb != NULL) && (hid->cb->attached != NULL)) {
		hid->cb->attached();
	}

	return RTK_SUCCESS;
}

/**
  * @brief  Detach callback
  * @param  host: Host handle
  * @retval None
  */
static void usbh_hid_detach(usb_host_t *host)
{
	usbh_hid_t *hid = &usbh_hid;

	/* Close the gate first: no report is queued and no request accepted from here on */
	usb_os_enter_critical();
	hid->ready = 0U;
	usb_os_exit_critical();

	/* A request waiting for the host task ends now: the device is gone */
	if (hid->req_state != USBH_HID_REQ_IDLE) {
		hid->req_status = RTK_FAIL;
		hid->req_state = USBH_HID_REQ_IDLE;
		usb_os_sema_give(hid->req_done_sema);
	}

	/* The report thread keeps consuming on another core under SMP: drop under the lock it dequeues
	 * with, so it never writes back a head index from before the reset */
	usb_os_enter_critical();
	usb_ringbuf_reset(&(hid->report_msg));
	usb_os_exit_critical();

	/* The Report descriptors handed to setup() go with the interfaces, see usbh_hid_itf_info_t */
	for (u8 i = 0U; i < hid->itf_cnt; i++) {
		usbh_hid_close_interface(host, &(hid->itf[i]));
	}
	hid->itf_cnt = 0U;

	if ((hid->cb != NULL) && (hid->cb->detached != NULL)) {
		hid->cb->detached();
	}
}

/**
  * @brief  Setup callback: set up every interface in turn, one control request per call
  * @param  host: Host handle
  * @retval RTK_SUCCESS once every interface is set up, -RTK_ERR_BUSY while in progress
  */
static int usbh_hid_setup(usb_host_t *host)
{
	usbh_hid_t *hid = &usbh_hid;
	usbh_hid_itf_t *itf;

	while (hid->setup_idx < hid->itf_cnt) {
		itf = &(hid->itf[hid->setup_idx]);
		if (usbh_hid_setup_interface(host, itf, hid->setup_idx) == -RTK_ERR_BUSY) {
			usbh_notify(host, 0U, &usbh_hid_driver);
			return -RTK_ERR_BUSY;
		}
		itf->pipe_in.xfer_state = USBH_EP_XFER_START;
		hid->setup_idx++;
	}

	usb_os_enter_critical();
	hid->ready = 1U;
	usb_os_exit_critical();

	return RTK_SUCCESS;
}

/**
  * @brief  State machine handling callback
  * @param  host: Host handle
  * @param  msg: USB host driver message
  * @retval None
  */
static void usbh_hid_process(usb_host_t *host, usbh_drv_msg_t *msg)
{
	usbh_hid_t *hid = &usbh_hid;
	usbh_hid_itf_t *itf;
	u8 halt = 0U;

	if (msg == NULL) {
		return;
	}

	/* One EP0 user at a time: an application request already issued runs to its end, then a halt
	 * recovery, then a new application request */
	if (hid->req_state == USBH_HID_REQ_BUSY) {
		usbh_hid_process_request(host);
	} else {
		for (u8 i = 0U; i < hid->itf_cnt; i++) {
			if (hid->itf[i].halt_pending != 0U) {
				usbh_hid_process_halt(host, &(hid->itf[i]));
				halt = 1U;
				break;
			}
		}
		if ((halt == 0U) && (hid->req_state == USBH_HID_REQ_PENDING)) {
			usbh_hid_process_request(host);
		}
	}

	if (msg->pipe_num == 0U) {
		return;
	}

	for (u8 i = 0U; i < hid->itf_cnt; i++) {
		itf = &(hid->itf[i]);
		if (msg->pipe_num == itf->pipe_in.pipe_num) {
			if (itf->halt_pending == 0U) {
				usbh_hid_in_process(host, i);
			}
			break;
		}
		if (msg->pipe_num == itf->pipe_out.pipe_num) {
			usbh_hid_out_process(host, itf);
			break;
		}
	}
}

/**
  * @brief  SOF callback: drive every INTR IN pipe that has been idle for a while
  * @note   ISR context, read only
  * @param  host: Host handle
  * @retval None
  */
static void usbh_hid_sof(usb_host_t *host)
{
	usbh_hid_t *hid = &usbh_hid;
	usbh_pipe_t *pipe;

	if (hid->ready == 0U) {
		return;
	}

	for (u8 i = 0U; i < hid->itf_cnt; i++) {
		pipe = &(hid->itf[i].pipe_in);
		if ((pipe->pipe_num != 0U) && (hid->itf[i].halt_pending == 0U) &&
			(usbh_get_elapsed_ticks(host, pipe->tick) > USBH_HID_TRIGGER_MAX_CNT)) {
			usbh_notify(host, pipe->pipe_num, &usbh_hid_driver);
		}
	}
}

/**
  * @brief  Report thread: hand every queued report to the application
  * @param  param: Unused
  * @retval None
  */
static void usbh_hid_report_thread(void *param)
{
	usbh_hid_t *hid = &usbh_hid;
	const usbh_hid_usr_cb_t *cb;
	u32 read_len;

	UNUSED(param);

	while (hid->report_task_exit == 0U) {
		/* Serialized with the reset in usbh_hid_detach(), a short copy inside the lock */
		usb_os_enter_critical();
		read_len = usb_ringbuf_remove_head(&(hid->report_msg), hid->report_buf, USBH_HID_RING_NODE_LEN, NULL);
		usb_os_exit_critical();
		if (read_len > 1U) {
			/* Read once: deinit may clear it on another core */
			cb = hid->cb;
			if ((cb != NULL) && (cb->report != NULL) && (hid->ready != 0U)) {
				cb->report(hid->report_buf[0], &(hid->report_buf[1]), read_len - 1U);
			}
		} else if (read_len == 0U) {
			/* Ring drained: the ring is checked again before every wait, a binary sema is enough */
			usb_os_sema_take(hid->report_sema, USB_OS_SEMA_TIMEOUT);
		} else {
			/* Empty report: nothing to tell */
		}
	}

	hid->report_task_alive = 0U;
	rtos_task_delete(NULL);
}

/**
  * @brief  Issue one class request on an interface from the application and wait for it
  * @note   Task context only. The host task issues it, so it is serialized with the class's own EP0 use
  *         and arbitrated against the other classes of the device.
  * @param  idx: Interface index
  * @param  bm_request_type: bmRequestType
  * @param  request: bRequest
  * @param  value: wValue
  * @param  tx: H2D data stage, NULL for none or a D2H request
  * @param  rx: D2H data stage destination, NULL for a H2D request
  * @param  len: In: wLength, NULL without data stage. Out: bytes received by a D2H request.
  * @retval RTK_SUCCESS, -RTK_ERR_BADARG for a STALL or a bad argument, -RTK_ERR_TIMEOUT, or the transfer error
  */
static int usbh_hid_ctrl_sync(u8 idx, u8 bm_request_type, u8 request, u16 value, const u8 *tx, u8 *rx, u32 *len)
{
	usbh_hid_t *hid = &usbh_hid;
	u32 wlen = (len != NULL) ? *len : 0U;
	u32 rx_len;
	int ret;

	if ((usb_os_is_in_interrupt() != 0U) || (wlen > USBH_HID_CTRL_BUF_LEN) || (hid->req_lock == NULL) ||
		((wlen != 0U) && ((((bm_request_type & USB_D2H) != 0U) ? (const u8 *)rx : tx) == NULL))) {
		return -RTK_ERR_BADARG;
	}

	if (usb_os_lock(hid->req_lock) != RTK_SUCCESS) {
		return -RTK_ERR_BUSY;
	}

	usb_os_enter_critical();
	if ((hid->ready == 0U) || (idx >= hid->itf_cnt) || (hid->host == NULL)) {
		usb_os_exit_critical();
		usb_os_unlock(hid->req_lock);
		return -RTK_ERR_BADARG;
	}
	/* A request that timed out may still be in flight in the host task, on req_setup and ctrl_buf */
	if (hid->req_state != USBH_HID_REQ_IDLE) {
		usb_os_exit_critical();
		usb_os_unlock(hid->req_lock);
		return -RTK_ERR_BUSY;
	}
	hid->req_setup.req.bmRequestType = bm_request_type;
	hid->req_setup.req.bRequest = request;
	hid->req_setup.req.wValue = value;
	hid->req_setup.req.wIndex = hid->itf[idx].info.itf_num;
	hid->req_setup.req.wLength = (u16)wlen;
	hid->req_rx_len = 0U;
	hid->req_status = -RTK_ERR_BUSY;
	if ((tx != NULL) && (wlen != 0U)) {
		usb_os_memcpy((void *)hid->ctrl_buf, (const void *)tx, wlen);
	}
	/* Drop a give left over by a detach that raced the end of the previous request */
	usb_os_sema_take(hid->req_done_sema, 0U);
	hid->req_state = USBH_HID_REQ_PENDING;
	usb_os_exit_critical();

	usbh_notify(hid->host, 0U, &usbh_hid_driver);

	if (usb_os_sema_take(hid->req_done_sema, USBH_HID_CTRL_TIMEOUT_MS) == RTK_SUCCESS) {
		ret = hid->req_status;
		/* Copied before the lock goes: the next request reuses ctrl_buf */
		if ((ret == RTK_SUCCESS) && (rx != NULL) && (len != NULL)) {
			rx_len = (hid->req_rx_len < wlen) ? hid->req_rx_len : wlen;
			usb_os_memcpy((void *)rx, (const void *)hid->ctrl_buf, rx_len);
			*len = rx_len;
		}
	} else {
		/* Longer than the core control timeout, so only a host task that stopped serving gets here */
		ret = -RTK_ERR_TIMEOUT;
		usb_os_enter_critical();
		if (hid->req_state == USBH_HID_REQ_PENDING) {
			hid->req_state = USBH_HID_REQ_IDLE;
		}
		usb_os_exit_critical();
	}

	usb_os_unlock(hid->req_lock);

	return ret;
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Init hid class
  * @param  cb: User callback
  * @retval Status
  */
int usbh_hid_init(const usbh_hid_usr_cb_t *cb)
{
	usbh_hid_t *hid = &usbh_hid;
	int ret;

	if (cb == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid user CB\n");
		return -RTK_ERR_BADARG;
	}

	/* ctrl_buf is only allocated by a successful init and freed by deinit: a second init would leak it,
	   the ring and the report task, and register the driver twice */
	if (hid->ctrl_buf != NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Already init\n");
		return -RTK_ERR_BADARG;
	}

	usb_os_memset((void *)hid, 0x00, sizeof(usbh_hid_t));

	/* A D2H request reserves a whole EP0 packet */
	hid->ctrl_buf = (u8 *)usb_os_malloc(usb_get_dma_len(USBH_HID_CTRL_BUF_LEN, USB_MAX_EP0_SIZE));
	if (hid->ctrl_buf == NULL) {
		return -RTK_ERR_NOMEM;
	}

	if (cb->init != NULL) {
		ret = cb->init();
		if (ret != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "CB init fail\n");
			goto cb_fail;
		}
	}

	ret = usb_ringbuf_manager_init(&(hid->report_msg), USBH_HID_RING_NODE_CNT, USBH_HID_RING_NODE_LEN, 0);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Ringbuffer init fail\n");
		goto ring_fail;
	}

	ret = usb_os_sema_create(&(hid->report_sema));
	if (ret != RTK_SUCCESS) {
		goto report_sema_fail;
	}

	ret = usb_os_sema_create(&(hid->req_done_sema));
	if (ret != RTK_SUCCESS) {
		goto req_sema_fail;
	}

	ret = usb_os_lock_create(&(hid->req_lock));
	if (ret != RTK_SUCCESS) {
		goto lock_fail;
	}

	/* Set before task creation: the new task may not run at once, and an exit asked for before would be
	   overwritten by its own startup */
	hid->report_task_alive = 1U;
	hid->report_task_exit = 0U;
	if (rtos_task_create(&(hid->report_task), "usbh_hid_report_thread", usbh_hid_report_thread,
						 NULL, USBH_HID_THREAD_STACK_SIZE, USBH_HID_THREAD_PRIORITY) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create report thread fail\n");
		hid->report_task_alive = 0U;
		ret = RTK_FAIL;
		goto task_fail;
	}

	hid->cb = cb;

	ret = usbh_register_class(&usbh_hid_driver);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Register class fail %d\n", ret);
		hid->cb = NULL;
		hid->report_task_exit = 1U;
		usb_os_sema_give(hid->report_sema);
		while (hid->report_task_alive != 0U) {
			rtos_time_delay_ms(1U);
		}
		goto task_fail;
	}

	return RTK_SUCCESS;

	/* Undo in reverse order, the user resources taken by cb->init() last */
task_fail:
	usb_os_lock_delete(hid->req_lock);
	hid->req_lock = NULL;
lock_fail:
	usb_os_sema_delete(hid->req_done_sema);
	hid->req_done_sema = NULL;
req_sema_fail:
	usb_os_sema_delete(hid->report_sema);
	hid->report_sema = NULL;
report_sema_fail:
	/* Fails only for a NULL handle, never the case for this static instance */
	usb_ringbuf_manager_deinit(&(hid->report_msg));
ring_fail:
	if (cb->deinit != NULL) {
		cb->deinit();
	}
cb_fail:
	usb_os_mfree((void *)hid->ctrl_buf);
	hid->ctrl_buf = NULL;
	return ret;
}

/**
  * @brief  Deinit hid class
  */
void usbh_hid_deinit(void)
{
	usbh_hid_t *hid = &usbh_hid;

	/* First, before anything is freed: on RTK_SUCCESS no attach/setup/process/detach of this class runs
	 * any more, an EP0 request it left in flight is halted and a sof() still running on another core is
	 * waited out. Anything else means nothing was released. */
	if (usbh_unregister_class(&usbh_hid_driver) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Deinit refused\n");
		return;
	}

	usb_os_enter_critical();
	hid->ready = 0U;
	usb_os_exit_critical();

	/* An application request still waiting ends: the host task will not serve it any more */
	if (hid->req_state != USBH_HID_REQ_IDLE) {
		hid->req_status = RTK_FAIL;
		hid->req_state = USBH_HID_REQ_IDLE;
		usb_os_sema_give(hid->req_done_sema);
	}

	/* Stop the report thread before cb->deinit(): it calls cb->report(), so the user resources it reaches
	   shall outlive it */
	if (hid->report_task_alive != 0U) {
		hid->report_task_exit = 1U;
		usb_os_sema_give(hid->report_sema);
		while (hid->report_task_alive != 0U) {
			rtos_time_delay_ms(1U);
		}
	}

	if ((hid->cb != NULL) && (hid->cb->deinit != NULL)) {
		hid->cb->deinit();
	}

	/* No detach follows a deinit while attached: close what attach opened */
	for (u8 i = 0U; i < hid->itf_cnt; i++) {
		usbh_hid_close_interface(hid->host, &(hid->itf[i]));
	}
	hid->itf_cnt = 0U;

	/* A caller may still be inside usbh_hid_ctrl_sync(): take the lock, so it has left, before it goes */
	if (usb_os_lock(hid->req_lock) == RTK_SUCCESS) {
		usb_os_unlock(hid->req_lock);
	}
	usb_os_lock_delete(hid->req_lock);
	hid->req_lock = NULL;
	usb_os_sema_delete(hid->req_done_sema);
	hid->req_done_sema = NULL;
	usb_os_sema_delete(hid->report_sema);
	hid->report_sema = NULL;

	usb_os_mfree((void *)hid->ctrl_buf);
	hid->ctrl_buf = NULL;
	hid->cb = NULL;
	hid->host = NULL;

	/* Fails only for a NULL handle, never the case for this static instance */
	usb_ringbuf_manager_deinit(&(hid->report_msg));
}

/**
  * @brief  Send an output report via the Interrupt OUT pipe of an interface
  * @param  idx: Interface index
  * @param  buf: Report, cache line aligned (DMA source)
  * @param  len: Length of the report in bytes
  * @retval RTK_SUCCESS, -RTK_ERR_BADARG if no OUT pipe or buf is NULL / unaligned, -RTK_ERR_BUSY if in progress
  */
int usbh_hid_send_report(u8 idx, u8 *buf, u32 len)
{
	usbh_hid_t *hid = &usbh_hid;
	usbh_pipe_t *pipe;
	usb_host_t *host;

	/* The OUT data is read by DMA: an unaligned buffer would only fail later in the HAL */
	if ((buf == NULL) || (len == 0U) || (USB_IS_MEM_DMA_ALIGNED(buf) == 0)) {
		return -RTK_ERR_BADARG;
	}

	/* Check and claim in one step: two callers, possibly on two cores, could both see the pipe idle,
	 * and the host task writes xfer_state as well */
	usb_os_enter_critical();
	if ((hid->ready == 0U) || (idx >= hid->itf_cnt) || (hid->itf[idx].pipe_out.pipe_num == 0U)) {
		usb_os_exit_critical();
		return -RTK_ERR_BADARG;
	}
	pipe = &(hid->itf[idx].pipe_out);
	if (pipe->xfer_state != USBH_EP_XFER_IDLE) {
		usb_os_exit_critical();
		return -RTK_ERR_BUSY;
	}
	pipe->xfer_buf = buf;
	pipe->xfer_len = len;
	pipe->xfer_state = USBH_EP_XFER_START;
	host = hid->host;
	usb_os_exit_critical();

	usbh_notify(host, pipe->pipe_num, &usbh_hid_driver);

	return RTK_SUCCESS;
}

/**
  * @brief  SET_PROTOCOL, Ref HID 1.11 7.2.6
  * @param  idx: Interface index
  * @param  protocol: USBH_HID_PROTOCOL_BOOT or USBH_HID_PROTOCOL_REPORT
  * @retval Status
  */
int usbh_hid_set_protocol(u8 idx, u8 protocol)
{
	if (protocol > USBH_HID_PROTOCOL_REPORT) {
		return -RTK_ERR_BADARG;
	}

	return usbh_hid_ctrl_sync(idx, USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE, USBH_HID_SET_PROTOCOL, protocol, NULL, NULL, NULL);
}

/**
  * @brief  SET_REPORT over EP0, Ref HID 1.11 7.2.2
  * @param  idx: Interface index
  * @param  type: USBH_HID_REPORT_OUTPUT or USBH_HID_REPORT_FEATURE
  * @param  id: Report ID
  * @param  buf: Report
  * @param  len: Report length
  * @retval Status
  */
int usbh_hid_set_report(u8 idx, u8 type, u8 id, const u8 *buf, u32 len)
{
	u32 wlen = len;

	if ((buf == NULL) || (len == 0U) || ((type != USBH_HID_REPORT_OUTPUT) && (type != USBH_HID_REPORT_FEATURE))) {
		return -RTK_ERR_BADARG;
	}

	return usbh_hid_ctrl_sync(idx, USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE, USBH_HID_SET_REPORT,
							  (u16)(((u16)type << 8) | id), buf, NULL, &wlen);
}

/**
  * @brief  GET_REPORT over EP0, Ref HID 1.11 7.2.1
  * @param  idx: Interface index
  * @param  type: USBH_HID_REPORT_INPUT or USBH_HID_REPORT_FEATURE
  * @param  id: Report ID
  * @param  buf: Destination
  * @param  len: In: capacity of buf; Out: bytes received
  * @retval Status
  */
int usbh_hid_get_report(u8 idx, u8 type, u8 id, u8 *buf, u32 *len)
{
	if ((buf == NULL) || (len == NULL) || (*len == 0U) || ((type != USBH_HID_REPORT_INPUT) && (type != USBH_HID_REPORT_FEATURE))) {
		return -RTK_ERR_BADARG;
	}

	return usbh_hid_ctrl_sync(idx, USB_D2H | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE, USBH_HID_GET_REPORT,
							  (u16)(((u16)type << 8) | id), NULL, buf, len);
}

/**
  * @brief  Find the Input or Output field declaring a usage in a report descriptor
  * @param  desc: Report descriptor
  * @param  len: Length of desc
  * @param  main_tag: USBH_HID_MAIN_ITEM_TAG_INPUT or USBH_HID_MAIN_ITEM_TAG_OUTPUT
  * @param  usage_page: Usage Page
  * @param  usage: Usage ID
  * @param  field: Filled with the field
  * @retval RTK_SUCCESS if found, -RTK_ERR_BADARG otherwise
  */
int usbh_hid_find_field(const u8 *desc, u32 len, u8 main_tag, u16 usage_page, u16 usage, usbh_hid_field_t *field)
{
	usbh_hid_walk_t walk;
	usbh_hid_item_t item;
	const u8 *end;
	const u8 *ptr;
	u32 *bit_offset;
	u32 field_bits;
	u8 hit;

	if ((desc == NULL) || (field == NULL) || ((main_tag != USBH_HID_MAIN_ITEM_TAG_INPUT) && (main_tag != USBH_HID_MAIN_ITEM_TAG_OUTPUT))) {
		return -RTK_ERR_BADARG;
	}

	usb_os_memset((void *)&walk, 0, sizeof(walk));
	end = desc + len;
	ptr = usbh_hid_fetch_item(desc, end, &item);

	while (ptr != NULL) {
		switch (item.type) {
		case USBH_HID_ITEM_TYPE_GLOBAL:
			switch (item.tag) {
			case USBH_HID_GLOBAL_ITEM_TAG_USAGE_PAGE:
				walk.usage_page = (u16)item.data;
				break;
			case USBH_HID_GLOBAL_ITEM_TAG_LOGICAL_MINIMUM:
				/* Signed in the item size, Ref HID 1.11 6.2.2.7 */
				walk.logical_min = (item.size == 1U) ? (s32)(s8)item.data : ((item.size == 2U) ? (s32)(s16)item.data : (s32)item.data);
				break;
			case USBH_HID_GLOBAL_ITEM_TAG_LOGICAL_MAXIMUM:
				walk.logical_max = (item.size == 1U) ? (s32)(s8)item.data : ((item.size == 2U) ? (s32)(s16)item.data : (s32)item.data);
				break;
			case USBH_HID_GLOBAL_ITEM_TAG_REPORT_SIZE:
				walk.report_size = (u16)item.data;
				break;
			case USBH_HID_GLOBAL_ITEM_TAG_REPORT_COUNT:
				walk.report_count = (u16)item.data;
				break;
			case USBH_HID_GLOBAL_ITEM_TAG_REPORT_ID:
				/* Each Report ID starts a new report, Ref HID 1.11 5.6 */
				walk.report_id = (u8)item.data;
				walk.in_bit_offset = 0U;
				walk.out_bit_offset = 0U;
				break;
			default:
				break;
			}
			break;

		case USBH_HID_ITEM_TYPE_LOCAL:
			usbh_hid_walk_local(&walk, &item, usage_page, usage);
			break;

		case USBH_HID_ITEM_TYPE_MAIN:
			field_bits = (u32)walk.report_size * (u32)walk.report_count;
			if ((item.tag == USBH_HID_MAIN_ITEM_TAG_INPUT) || (item.tag == USBH_HID_MAIN_ITEM_TAG_OUTPUT)) {
				bit_offset = (item.tag == USBH_HID_MAIN_ITEM_TAG_INPUT) ? &walk.in_bit_offset : &walk.out_bit_offset;
				hit = 0U;
				if ((item.tag == main_tag) && ((item.data & USBH_HID_ITEM_DATA_CONSTANT) == 0U) && (walk.report_size != 0U)) {
					if ((item.data & USBH_HID_ITEM_DATA_VARIABLE) != 0U) {
						/* One value per usage, the last usage repeated for the rest, Ref HID 1.11 6.2.2.8 */
						hit = ((walk.found != 0U) && (walk.found_idx < walk.report_count)) ? 1U : 0U;
					} else {
						/* Usage codes: code logical_min + n stands for the n-th usage, Ref HID 1.11 6.2.2.5 */
						hit = ((walk.found != 0U) && (walk.found_idx <= 0xFFFFU)) ? 1U : 0U;
					}
				}
				if ((hit != 0U) && ((*bit_offset + field_bits) <= 0xFFFFU)) {
					field->report_id = walk.report_id;
					field->bit_offset = (u16)(*bit_offset);
					field->report_size = walk.report_size;
					field->report_count = walk.report_count;
					field->logical_min = walk.logical_min;
					field->logical_max = walk.logical_max;
					field->usage_idx = (u16)walk.found_idx;
					field->flags = (u8)item.data;
					return RTK_SUCCESS;
				}
				*bit_offset += field_bits;
			}
			/* Local items apply to one Main item only, Ref HID 1.11 6.2.2.8 */
			walk.usage_cnt = 0U;
			walk.found = 0U;
			walk.usage_min_valid = 0U;
			break;

		default:
			break;
		}
		ptr = usbh_hid_fetch_item(ptr, end, &item);
	}

	return -RTK_ERR_BADARG;
}

/**
  * @brief  Read one value of a field from a report
  * @param  report: Report, Report ID first if the device uses one
  * @param  len: Length of the report
  * @param  field: Field found by usbh_hid_find_field()
  * @param  idx: Value index in the field
  * @param  value: Value read
  * @retval RTK_SUCCESS, -RTK_ERR_BADARG if the report does not hold the field
  */
int usbh_hid_get_field_value(const u8 *report, u32 len, const usbh_hid_field_t *field, u16 idx, s32 *value)
{
	const u8 *data = report;
	u32 data_len = len;
	u32 bit;
	u32 val = 0U;

	if ((report == NULL) || (field == NULL) || (value == NULL) || (idx >= field->report_count) ||
		(field->report_size == 0U) || (field->report_size > 32U)) {
		return -RTK_ERR_BADARG;
	}

	if (field->report_id != 0U) {
		if ((len == 0U) || (report[0] != field->report_id)) {
			return -RTK_ERR_BADARG;
		}
		data++;
		data_len--;
	}

	/* Report data is packed least significant bit first, Ref HID 1.11 8 */
	bit = (u32)field->bit_offset + ((u32)idx * field->report_size);
	if ((bit + field->report_size) > (data_len * 8U)) {
		return -RTK_ERR_BADARG;
	}
	for (u32 i = 0U; i < (u32)field->report_size; i++) {
		if (((data[(bit + i) / 8U] >> ((bit + i) % 8U)) & 0x01U) != 0U) {
			val |= (1UL << i);
		}
	}

	/* A negative Logical Minimum makes the field two's complement, Ref HID 1.11 6.2.2.7 */
	if ((field->logical_min < 0) && (field->report_size < 32U) && ((val & (1UL << (field->report_size - 1U))) != 0U)) {
		val |= ~((1UL << field->report_size) - 1U);
	}
	*value = (s32)val;

	return RTK_SUCCESS;
}

/**
  * @brief  Whether the usage a field was found for is active in a report
  * @param  report: Report
  * @param  len: Length of the report
  * @param  field: Field found by usbh_hid_find_field()
  * @retval 1 if active, 0 otherwise
  */
u8 usbh_hid_usage_active(const u8 *report, u32 len, const usbh_hid_field_t *field)
{
	s32 val;

	if (field == NULL) {
		return 0U;
	}

	if ((field->flags & USBH_HID_ITEM_DATA_VARIABLE) != 0U) {
		/* One value per usage */
		return ((usbh_hid_get_field_value(report, len, field, field->usage_idx, &val) == RTK_SUCCESS) && (val != 0)) ? 1U : 0U;
	}

	/* Usage codes: code logical_min + usage_idx names the usage, Ref HID 1.11 6.2.2.5 */
	for (u16 i = 0U; i < field->report_count; i++) {
		if ((usbh_hid_get_field_value(report, len, field, i, &val) == RTK_SUCCESS) && (val == (field->logical_min + (s32)field->usage_idx))) {
			return 1U;
		}
	}

	return 0U;
}
