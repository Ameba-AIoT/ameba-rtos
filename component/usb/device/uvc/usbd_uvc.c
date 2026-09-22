/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/
#include "usbd.h"
#include "usbd_uvc.h"
#ifdef CONFIG_USBD_COMPOSITE
#include "usbd_composite.h"
#endif
#include "usbd_video.h"
#include "usb_video.h"
//#include "os_wrapper.h"

/* Private defines -----------------------------------------------------------*/

/* Task configuration */
#define USBD_UVC_CMD_TASK_STACK_SIZE         1024U
#define USBD_UVC_CMD_TASK_PRIO               5U
#define USBD_UVC_FRAME_TASK_STACK_SIZE       1024U
#define USBD_UVC_FRAME_TASK_PRIO             5U
#if USBD_UVC_DEBUG
#define USBD_UVC_DUMP_TASK_STACK_SIZE        512U
#define USBD_UVC_DUMP_TASK_PRIO              6U
#endif

/* Queue depths */
#define USBD_UVC_CMD_QUEUE_DEPTH             8U
#define USBD_UVC_COMPLETE_QUEUE_DEPTH        10U

/* SOF counter mask (11-bit USB SOF counter) */
#define USBD_UVC_SOF_COUNT_MASK              0x07FFU

/* bmRequestType value for class, interface, host-to-device */
#define USBD_UVC_BMREQTYPE_CLASS_INTF_OUT    (USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE)

/* Producer ring-full wait (ms) */
#define USBD_UVC_RING_WAIT_MS                2U

/* Frame-get loop limits */
#define USBD_UVC_FRAME_WAIT_ITER             100000U
#define USBD_UVC_FRAME_STOP_ITER             30U

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/
#ifndef min
#define min(x, y) ((x) < (y) ? (x) : (y))
#endif

/* Private function prototypes -----------------------------------------------*/
static u16 usbd_uvc_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf, u16 buf_len);
static int usbd_uvc_set_config(usb_dev_t *dev, u8 config);
static void usbd_uvc_clear_config(usb_dev_t *dev, u8 config);
static int usbd_uvc_setup(usb_dev_t *dev, usb_setup_req_t *req);
static int usbd_uvc_handle_ep0_data_out(usb_dev_t *dev);
static int usbd_uvc_handle_ep0_data_in(usb_dev_t *dev, u8 status);
static int usbd_uvc_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status);
static int usbd_uvc_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u32 len);
static void usbd_uvc_handle_sof(usb_dev_t *dev);
static u8 usbd_uvc_set_interface(usb_dev_t *dev, u8 interface, u8 alt);
static void usbd_uvc_patch_desc(u8 *desc, u16 len);
#ifdef CONFIG_USBD_COMPOSITE
static void usbd_uvc_set_interface_base(u8 base);
#endif
static void usbd_uvc_video_try_arm(usb_dev_t *dev);
static usbd_uvc_buffer_t *usbd_uvc_video_in_stream_queue(usbd_uvc_dev_t *uvc_ctx);
static void usbd_uvc_get_frame_handler(void *parm);
void usbd_uvc_video_put_out_stream_queue(usbd_uvc_buffer_t *payload);

/* Private variables ---------------------------------------------------------*/

static usbd_uvc_dev_t usbd_uvc_dev;
/* Pre-allocated UVC format info; avoids dynamic allocation (MISRA 21.3). */
static usbd_uvc_format_t s_uvc_format;

static const char *const TAG = "UVC";

static const u8 usbd_uvc_dev_desc[USB_LEN_DEV_DESC] USB_DMA_ALIGNED = {
	USB_LEN_DEV_DESC,                               /* bLength */
	USB_DESC_TYPE_DEVICE,                           /* bDescriptorType */
	0x00,                                           /* bcdUSB */
	0x02,
	0xef,                                           /* bDeviceClass */
	0x02,                                           /* bDeviceSubClass */
	0x01,                                           /* bDeviceProtocol */
	USB_MAX_EP0_SIZE,                               /* bMaxPacketSize */
	USB_LOW_BYTE(USBD_UVC_WEBCAM_VENDOR_ID),                      /* idVendor */
	USB_HIGH_BYTE(USBD_UVC_WEBCAM_VENDOR_ID),
	USB_LOW_BYTE(USBD_UVC_WEBCAM_PRODUCT_ID),                      /* idProduct */
	USB_HIGH_BYTE(USBD_UVC_WEBCAM_PRODUCT_ID),
	USB_LOW_BYTE(USBD_UVC_WEBCAM_DEVICE_BCD),                    /* bcdDevice */
	USB_HIGH_BYTE(USBD_UVC_WEBCAM_DEVICE_BCD),
	USBD_IDX_MFC_STR,                               /* iManufacturer */
	USBD_IDX_PRODUCT_STR,                           /* iProduct */
	USBD_IDX_SERIAL_STR,                            /* iSerialNumber */
	0x01                                            /* bNumConfigurations */
};

/* USB Standard String Descriptor 0 */
static const u8 usbd_uvc_lang_id_desc[USB_LEN_LANGID_STR_DESC] USB_DMA_ALIGNED = {
	USB_LEN_LANGID_STR_DESC,                        /* bLength */
	USB_DESC_TYPE_STRING,                           /* bDescriptorType */
	USB_LOW_BYTE(USBD_UVC_LANGID_STRING),           /* wLANGID */
	USB_HIGH_BYTE(USBD_UVC_LANGID_STRING),
};  /* usbd_uvc_lang_id_desc */

#ifndef CONFIG_USB_FS
/* USB Standard Device Qualifier Descriptor */
static const u8 usbd_uvc_device_qualifier_desc[USB_LEN_DEV_QUALIFIER_DESC] USB_DMA_ALIGNED = {
	USB_LEN_DEV_QUALIFIER_DESC,                     /* bLength */
	USB_DESC_TYPE_DEVICE_QUALIFIER,                 /* bDescriptorType */
	0x00,                                           /* bcdUSB */
	0x02,
	0x00,                                           /* bDeviceClass */
	0x00,                                           /* bDeviceSubClass */
	0x00,                                           /* bDeviceProtocol */
	0x40,                                           /* bMaxPacketSize */
	0x01,                                           /* bNumConfigurations */
	0x00,                                           /* Reserved */
};  /* usbd_uvc_device_qualifier_desc */
#endif

/* UVC Class Driver */
static const usbd_class_driver_t usbd_uvc_driver = {
	.get_descriptor = usbd_uvc_get_descriptor,
	.set_config = usbd_uvc_set_config,
	.clear_config = usbd_uvc_clear_config,
	.setup = usbd_uvc_setup,
	.ep0_data_out = usbd_uvc_handle_ep0_data_out,
	.ep0_data_in = usbd_uvc_handle_ep0_data_in,
	.ep_data_in = usbd_uvc_handle_ep_data_in,
	.ep_data_out = usbd_uvc_handle_ep_data_out,
	.sof = usbd_uvc_handle_sof,
#ifdef CONFIG_USBD_COMPOSITE
	.set_interface_base = usbd_uvc_set_interface_base,
#endif
};

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initialize UVC command queue
  * @retval None
  */
void usbd_uvc_cmd_queue_init(void)
{
	usbd_uvc_dev_t *cdev = &usbd_uvc_dev;
	if (rtos_queue_create(&cdev->uvc_cmd_queue, USBD_UVC_CMD_QUEUE_DEPTH, sizeof(usbd_uvc_req_data_t)) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Queue create failed\n");
		return;
	}
}
/**
  * @brief  UVC command handler task
  *         Process UVC control and event messages from queue
  * @param  parm Task parameter (unused)
  * @retval None
  */
void usbd_uvc_cmd_handler(void *parm)
{
	usbd_uvc_dev_t *cdev = &usbd_uvc_dev;
	usbd_uvc_req_data_t req_data;
	(void)parm;

	while (cdev->init_done != 0U) {
		if (rtos_queue_receive(cdev->uvc_cmd_queue, &req_data, RTOS_MAX_DELAY) == RTK_SUCCESS) {
			/* deinit() clears init_done then sends a dummy item to unblock this
			   receive; bail out before processing it so the task self-deletes. */
			if (cdev->init_done == 0U) {
				break;
			}
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "Receive type=%d len=%d\n", req_data.type, req_data.uvc_data.length);
			usbd_uvc_events_process(cdev, &req_data);
		}
	}
	cdev->cmd_task_alive = 0U;
	rtos_task_delete(NULL);
}

/* --- Payload ring helpers (single-producer task / single-consumer ISR) -----
   The shared usb_ringbuf copies data in/out; here we access node->buf directly
   so the producer fills a node in place (task context) and the TX DMA reads the
   same node in place (ISR context, zero-copy). Producer owns 'tail', consumer
   owns 'head'; both are updated with a memory barrier. */
/**
 * @brief  Return the tail node for writing without advancing the tail index.
 *         Returns NULL if the ring is full.
 * @note   Producer (task) context only.
 */
static usb_ringbuf_t *usbd_uvc_rb_write_node(usb_ringbuf_manager_t *rb)
{
	if (usb_ringbuf_is_full(rb)) {
		return NULL;
	}
	return &rb->list_node[rb->tail];
}

/**
 * @brief  Commit a completed write: set buf_len, mark valid, advance tail.
 * @note   Memory barrier ensures ISR sees data before data_valid flag.
 */
static void usbd_uvc_rb_commit_write(usb_ringbuf_manager_t *rb, u16 len)
{
	usb_ringbuf_t *node = &rb->list_node[rb->tail];
	node->buf_len = len;
	node->data_valid = 1U;
	__sync_synchronize();
	rb->tail = (rb->tail + 1U) % rb->capacity;
}

/**
 * @brief  Return the head node for reading without advancing the head index.
 *         Returns NULL if the ring is empty.
 * @note   Consumer (ISR) context only.
 */
static usb_ringbuf_t *usbd_uvc_rb_read_node(usb_ringbuf_manager_t *rb)
{
	if (usb_ringbuf_is_empty(rb)) {
		return NULL;
	}
	return &rb->list_node[rb->head];
}

/**
 * @brief  Release the head node after DMA transmission: clear flags, advance head.
 * @note   ISR context only.
 */
static void usbd_uvc_rb_pop_read(usb_ringbuf_manager_t *rb)
{
	usb_ringbuf_t *node = &rb->list_node[rb->head];
	node->buf_len = 0;
	node->data_valid = 0U;
	__sync_synchronize();
	rb->head = (rb->head + 1U) % rb->capacity;
}

/**
  * @brief  Encode a 12-byte UVC payload header into a ring node (UVC 1.5 2.4.3.3)
  * @param  video        UVC video context
  * @param  data         Destination (start of the ring node buffer)
  * @param  frame_start  1 if this is the first payload of the frame
  * @param  eof          1 if this payload carries the end of the frame
  * @retval Header length in bytes (always USBD_UVC_PAYLOAD_HEADER_LEN)
  * @note   PTS is captured once at frame start (constant for the frame);
  *         SCR carries a fresh STC plus the software SOF counter each payload.
  *         PTS/STC use the 48 MHz clock advertised in probe/commit dwClockFrequency.
  */
static int
usbd_uvc_video_encode_header(usbd_uvc_video_t *video, u8 *data, u8 frame_start, u8 eof)
{
	u32 stc = (u32)(usb_os_get_timestamp_us() * 48ULL);
	int pos = 2;

	if (frame_start != 0) {
		video->cur_pts = stc;
	}

	data[1] = USBD_UVC_STREAM_EOH | (u8)video->fid;

	/* PTS: presentation time of the frame (same value for every payload of the frame) */
	data[1] |= USBD_UVC_STREAM_PTS;
	data[pos + 0] = (u8)(video->cur_pts & 0xFFU);
	data[pos + 1] = (u8)((video->cur_pts >> 8) & 0xFFU);
	data[pos + 2] = (u8)((video->cur_pts >> 16) & 0xFFU);
	data[pos + 3] = (u8)((video->cur_pts >> 24) & 0xFFU);
	pos += 4;

	/* SCR: source clock (STC 4B + SOF token 2B) sampled at payload build time */
	data[1] |= USBD_UVC_STREAM_SCR;
	data[pos + 0] = (u8)(stc & 0xFFU);
	data[pos + 1] = (u8)((stc >> 8) & 0xFFU);
	data[pos + 2] = (u8)((stc >> 16) & 0xFFU);
	data[pos + 3] = (u8)((stc >> 24) & 0xFFU);
	data[pos + 4] = (u8)(video->sof_count & 0xFFU);
	data[pos + 5] = (u8)((video->sof_count >> 8) & 0x07U);
	pos += 6;

	data[0] = (u8)pos;

	if (eof != 0) {
		data[1] |= USBD_UVC_STREAM_EOF;
	}

	return pos;
}

/**
  * @brief  Encode one full video frame into the payload ring (producer, task ctx)
  *         Slices the frame into microframe-sized payloads (12B header + data),
  *         blocking on the ring-space semaphore while the ring is full. All memcpy
  *         happens here (task context), never in the ISR.
  * @param  dev  USB device instance
  * @param  mem  Frame data
  * @param  len  Frame length in bytes
  * @retval None
  */
static void
usbd_uvc_video_produce_frame(usb_dev_t *dev, const u8 *mem, u32 len)
{
	usbd_uvc_dev_t *cdev = &usbd_uvc_dev;
	usbd_uvc_video_t *video = &cdev->video;
	usb_ringbuf_manager_t *rb = &video->in_rb;
	usb_ringbuf_t *node = NULL;
	const u32 data_cap = USBD_UVC_IN_BUF_SIZE - USBD_UVC_PAYLOAD_HEADER_LEN;
	u32 sent = 0U;
	u32 data_len = 0U;
	int hdr = 0;
	u8 frame_start = 1U;
	u8 eof = 0U;

	UNUSED(dev);

	if ((mem == NULL) || (len == 0U)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Produce_frame: invalid frame mem=%p len=%u\n", mem, len);
		return;
	}

	while (sent < len) {
		/* Wait for a usb_os_mfree ring node */
		while (usb_ringbuf_is_full(rb)) {
			if (cdev->running == 0U) {
				return;
			}
			usb_os_sema_take(video->in_rb_space_sema, USBD_UVC_RING_WAIT_MS);
		}
		if (cdev->running == 0U) {
			return;
		}

		node = usbd_uvc_rb_write_node(rb);
		if (node == NULL) {
			continue;
		}

		data_len = min(data_cap, len - sent);
		eof = (u8)((sent + data_len) >= len);

		hdr = usbd_uvc_video_encode_header(video, node->buf, frame_start, eof);
		usb_os_memcpy((void *)(node->buf + hdr), (const void *)(mem + sent), (u32)data_len);
		sent += data_len;
		frame_start = 0;

		usbd_uvc_rb_commit_write(rb, (u16)(hdr + data_len));
	}

	/* Toggle FID for the next frame (constant across all payloads of this frame) */
	video->fid ^= USBD_UVC_STREAM_FID;
}

/**
  * @brief  Arm the next queued payload on the ISOC IN endpoint (consumer, ISR ctx)
  *         Zero-copy: DMA reads the ring node buffer in place. Only ever called
  *         from ISR context (XFRC / SOF), so 'armed' needs no extra locking.
  * @param  dev  USB device instance
  * @retval None
  */
static void
usbd_uvc_video_try_arm(usb_dev_t *dev)
{
	usbd_uvc_dev_t *cdev = &usbd_uvc_dev;
	usbd_uvc_video_t *video = &cdev->video;
	usbd_ep_t *ep_isoc_in = &cdev->ep_isoc_in;
	usb_ringbuf_t *node;

	if ((cdev->running == 0U) || (video->armed != 0U)) {
		return;
	}

	node = usbd_uvc_rb_read_node(&video->in_rb);
	if (node == NULL) {
		if (video->underrun_cnt == 0U) {
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "Try_arm: ring empty (first underrun)\n");
		}
		video->underrun_cnt++;
		return; /* underrun: nothing to send this microframe */
	}

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "Try_arm: tx len=%u underruns=%u\n",
			 (unsigned)node->buf_len, (unsigned)video->underrun_cnt);
	ep_isoc_in->xfer_buf = node->buf;
	ep_isoc_in->xfer_len = node->buf_len;
	video->armed = 1U;
	video->stall_sof = 0U;
	usbd_ep_transmit(dev, ep_isoc_in);
}

/**
  * @brief  usb_os_mfree UVC buffer and return it to output queue
  * @param  video UVC video context
  * @retval None
  */
void usbd_uvc_free_uvcd_list_buffer(usbd_uvc_video_t *video)
{
	if (list_empty(&video->output_queue)) {
		usb_os_lock(video->output_lock);
		list_add_tail(&video->uvc_buffer.buffer_list, &video->output_queue);
		usb_os_unlock(video->output_lock);
		usb_os_sema_give(video->output_queue_sema);
		usb_os_sema_give(video->output_frame_sema);
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Free the buffer\n");
	} else {
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "No buffer to free\n");
	}
}
/**
  * @brief  Put buffer back to output queue
  *         Used when frame transmission is completed
  * @param  payload UVC buffer
  * @retval None
  */
void usbd_uvc_video_put_out_stream_queue(usbd_uvc_buffer_t *payload)
{
	usbd_uvc_dev_t *uvc_ctx = &usbd_uvc_dev;
	usbd_uvc_video_t *video = &uvc_ctx->video;
	usb_os_lock(video->output_lock);
	/* Guard against relinking a node already on a list (would corrupt it).
	   A node is safe to link when:
	     - null-initialized (zero-init global, never touched): next == NULL
	     - properly detached via list_del_init:               next == &node
	   list_empty() alone is NOT sufficient: it returns false for zero-init
	   nodes (next==NULL != &node), which silently drops app-side buffers. */
	if (payload->buffer_list.next == NULL || list_empty(&payload->buffer_list)) {
		list_add_tail(&payload->buffer_list, &video->output_queue);
	} else {
		RTK_LOGS(TAG, RTK_LOG_WARN, "Put_out: skip linked buf %p next=%p\n",
				 (void *)payload, (void *)payload->buffer_list.next);
	}
	usb_os_unlock(video->output_lock);
}
/**
  * @brief  Put buffer into input queue
  *         Buffer contains new video frame data
  * @param  payload UVC buffer
  * @retval None
  */
void usbd_uvc_video_put_in_stream_queue(usbd_uvc_buffer_t *payload)
{
	usbd_uvc_dev_t *uvc_ctx = &usbd_uvc_dev;
	usbd_uvc_video_t *video = &uvc_ctx->video;
	usb_os_lock(video->input_lock);
	/* Same null-init guard as put_out_stream_queue. */
	if (payload->buffer_list.next == NULL || list_empty(&payload->buffer_list)) {
		list_add_tail(&payload->buffer_list, &video->input_queue);
	} else {
		RTK_LOGS(TAG, RTK_LOG_WARN, "Put_in: skip linked buf %p next=%p\n",
				 (void *)payload, (void *)payload->buffer_list.next);
	}
	usb_os_unlock(video->input_lock);
	/* Signal get_frame_handler that a new frame is available.
	   Without this, if get_frame_handler's FRAME_WAIT_ITER spin expires before
	   we call put_in_stream_queue (a race at stream start or after stop/restart),
	   it falls back to sema_take(output_queue_sema) and nobody gives it —
	   permanent deadlock with uvcd_handle blocked in the out_stream_queue poll loop. */
	usb_os_sema_give(video->output_queue_sema);
}
/**
  * @brief  Get buffer from output queue
  * @retval Pointer to UVC buffer or NULL if empty
  */
usbd_uvc_buffer_t *usbd_uvc_video_out_stream_queue(void)
{
	usbd_uvc_dev_t *uvc_ctx = &usbd_uvc_dev;
	usbd_uvc_video_t *video = &uvc_ctx->video;
	usbd_uvc_buffer_t *payload = NULL;
	if (!list_empty(&video->output_queue)) {
		usb_os_lock(video->output_lock);
		payload = list_first_entry(&video->output_queue, usbd_uvc_buffer_t, buffer_list);
		video->uvc_buffer = *payload;
		list_del_init(&payload->buffer_list);
		/* video->uvc_buffer is a snapshot copy: its embedded buffer_list still
		   holds payload's old link pointers. Re-init it so the later
		   put_out_stream_queue(&video->uvc_buffer) in wait_frame_down() sees a
		   properly detached node instead of a stale/aliased one. */
		INIT_LIST_HEAD(&video->uvc_buffer.buffer_list);
		usb_os_unlock(video->output_lock);
	}
	return payload;
}

/**
  * @brief  Get one video buffer from input queue
  * @param  uvc_ctx UVC device context
  * @retval Pointer to UVC buffer or NULL if empty
  */
static usbd_uvc_buffer_t *usbd_uvc_video_in_stream_queue(usbd_uvc_dev_t *uvc_ctx)
{
	usbd_uvc_video_t *video = &uvc_ctx->video;
	usbd_uvc_buffer_t *payload = NULL;
	if (!list_empty(&video->input_queue)) {
		usb_os_lock(video->input_lock);
		payload = list_first_entry(&video->input_queue, usbd_uvc_buffer_t, buffer_list);
		video->uvc_buffer = *payload;
		list_del_init(&payload->buffer_list);
		/* See usbd_uvc_video_out_stream_queue(): re-init the copy's buffer_list. */
		INIT_LIST_HEAD(&video->uvc_buffer.buffer_list);
		usb_os_unlock(video->input_lock);
	}
	return payload;
}
/**
  * @brief  Wait until current UVC video frame transmission is completed
  *         Block the caller until the output frame semaphore is released,
  *         indicating that one video frame has been fully sent.
  * @retval None
  */
void usbd_uvc_wait_frame_down(void)
{
	usbd_uvc_dev_t *uvc_ctx = &usbd_uvc_dev;
	usbd_uvc_video_t *video = &uvc_ctx->video;
	usb_os_sema_take(video->output_frame_sema, RTOS_MAX_DELAY);
	usbd_uvc_video_put_out_stream_queue(&video->uvc_buffer);
	usb_os_sema_give(video->output_queue_sema);
}
/**
  * @brief  Get private UVC device context
  *         Return internal UVC device structure pointer
  * @retval Pointer to UVC device context
  */
usbd_uvc_dev_t *get_private_usbd_uvcd(void)
{
	return &usbd_uvc_dev;
}
/**
  * @brief  Get UVC running status
  * @retval 1 if UVC streaming is running, otherwise 0
  */
int usbd_uvc_get_status(void)
{
	return usbd_uvc_dev.running;
}
/**
  * @brief  Handle EP0 OUT data stage (Control OUT)
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `usb_os_malloc`, `rtos_sema_take`) are not permitted.
  *         Receive class-specific control data from host
  * @param  dev USB device instance
  * @retval HAL status
  */
static int usbd_uvc_handle_ep0_data_out(usb_dev_t *dev)
{
	usbd_uvc_req_data_t req_data;
	usbd_uvc_dev_t *cdev = &usbd_uvc_dev;
	usbd_ep_t *ep0_out = &dev->ep0_out;
	int ret = HAL_OK;

	/* No pending request means this data stage does not belong to UVC, the composite dispatcher
	   already routed it by active_func. Ref USB 2.0 8.5.3.1: a non-zero return makes the core
	   stall the status stage, so do not report a failure the host cannot act on. */
	if (cdev->ctrl_req_pending == 0U) {
		return HAL_OK;
	}

	/* Consume the pending request: a single data stage belongs to exactly one setup packet, so
	   the saved request must not be replayed by a later EP0 OUT event. */
	cdev->ctrl_req_pending = 0U;

	req_data.type = USBD_UVC_EVENT_DATA;
	DCache_Invalidate((u32)ep0_out->xfer_buf, cdev->ctrl_data_len);
	usb_os_memcpy((void *)req_data.uvc_data.data, (const void *)ep0_out->xfer_buf, (u32)cdev->ctrl_data_len);
	req_data.uvc_data.length = cdev->ctrl_data_len;
	usbd_uvc_events_process(cdev, &req_data);

	return ret;
}
/**
  * @brief  Handle EP0 IN data stage
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `usb_os_malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev USB device instance
  * @param  status Transfer status
  * @retval HAL status
  */
static int usbd_uvc_handle_ep0_data_in(usb_dev_t *dev, u8 status)
{
	(void)dev;
	(void)status;
	return HAL_OK;
}
/**
  * @brief  Handle isochronous IN endpoint transfer complete
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `usb_os_malloc`, `rtos_sema_take`) are not permitted.
  *         Trigger next video packet transmission
  * @param  dev     USB device instance
  * @param  ep_addr Endpoint address
  * @param  status  Transfer status
  * @retval HAL status
  */
static int usbd_uvc_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status)
{
	usbd_uvc_dev_t *cdev = &usbd_uvc_dev;
	usbd_uvc_video_t *video = &cdev->video;

	UNUSED(ep_addr);
	UNUSED(status);

	if (cdev->init_done == 0U) {
		return HAL_OK;
	}

	/* The armed payload finished transmitting this microframe: consume it,
	   usb_os_mfree the ring slot for the producer, then chain the next payload. */
	video->armed = 0U;
	video->tx_payloads++;

	if (usb_ringbuf_is_empty(&video->in_rb) == 0) {
		usbd_uvc_rb_pop_read(&video->in_rb);
		usb_os_sema_give(video->in_rb_space_sema);
	}

	if (cdev->running == 0U) {
		return HAL_OK;
	}

	usbd_uvc_video_try_arm(dev);

	return HAL_OK;
}

/**
  * @brief  Handle SOF interrupt for UVC ISOC IN streaming
  * @note   ISR context; no blocking calls. Maintains the software SOF counter
  *         (used as the SCR SOF token), watchdogs a stalled transfer caused by
  *         an incomplete ISOC IN (core clears EPENA without an XFRC callback),
  *         and kicks arming so streaming self-starts / recovers each microframe.
  * @param  dev  USB device instance
  * @retval NONE
  */
static void usbd_uvc_handle_sof(usb_dev_t *dev)
{
	usbd_uvc_dev_t *cdev = &usbd_uvc_dev;
	usbd_uvc_video_t *video = &cdev->video;

	video->sof_count = (video->sof_count + 1U) & USBD_UVC_SOF_COUNT_MASK;

	if ((cdev->init_done == 0U) || (cdev->running == 0U)) {
		return;
	}

	/* Watchdog: an ISOC IN xfer must complete within its microframe. If several
	   SOFs pass while still armed, the xfer was dropped by an incompISOIN (which
	   the core handles by clearing EPENA, with no XFRC callback to us). Drop the
	   stuck payload and recover on the next arm. */
	if (video->armed != 0U) {
		video->stall_sof++;
		if (video->stall_sof >= USBD_UVC_STALL_SOF_MAX) {
			video->armed = 0U;
			video->incomp_cnt++;
			if (!usb_ringbuf_is_empty(&video->in_rb)) {
				usbd_uvc_rb_pop_read(&video->in_rb);
				usb_os_sema_give(video->in_rb_space_sema);
			}
		}
	}

	if (video->armed == 0U) {
		usbd_uvc_video_try_arm(dev);
	}
}
/**
  * @brief  Handle USB data OUT endpoint callback
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `usb_os_malloc`, `rtos_sema_take`) are not permitted.
  *         This function is called when data is received from host
  *         on a non-control OUT endpoint.
  *         (Currently not used for UVC video streaming)
  * @param  dev     USB device instance
  * @param  ep_addr Endpoint address
  * @param  len     Length of received data
  * @retval HAL status
  */
static int usbd_uvc_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u32 len)
{
	int ret = HAL_OK;
	(void)ep_addr;
	(void)len;
	(void)dev;
	return ret;
}
/**
  * @brief  Rebase the class-specific interface cross-references of a config descriptor block
  * @note   The VC Header baInterfaceNr[] entries name this function's own streaming
  *         interfaces and are rebased with the current interface base, which is 0 unless
  *         the composite framework rebased it; the framework itself only rebases the
  *         standard Interface and IAD descriptors. Each entry keeps its own class-local
  *         interface number, hence the base is added rather than assigned.
  *         Ref UVC 1.5 3.7.2 Tbl 3-3: bInCollection at offset 11, baInterfaceNr[j] at 12+j.
  * @param  desc  Pointer to the descriptor block (after the configuration descriptor header)
  * @param  len   Length of the descriptor block
  * @retval None
  */
static void usbd_uvc_patch_desc(u8 *desc, u16 len)
{
	usbd_uvc_dev_t *cdev = &usbd_uvc_dev;
	u16 i;

	if (cdev->if_base == 0U) {
		return;
	}

	for (i = 0; i < len;) {
		u8 dlen = desc[i];
		u8 dtype = desc[i + 1];

		if (dlen == 0U) {
			break;
		}

		if ((dtype == USB_DESC_TYPE_CS_INTERFACE) && (dlen >= 13U) && (desc[i + 2] == USBD_UVC_VC_HEADER)) {
			u8 n = desc[i + 11];
			u8 j;

			if ((u16)(12U + n) > (u16)dlen) { /* malformed bInCollection: do not run off the descriptor */
				n = (u8)(dlen - 12U);
			}
			for (j = 0; j < n; j++) {
				desc[i + 12U + j] = (u8)(desc[i + 12U + j] + cdev->if_base);
			}
		}
		i += dlen;
	}
}

#ifdef CONFIG_USBD_COMPOSITE
/**
  * @brief  Store the first interface number assigned to this class by the composite framework
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `usb_os_malloc`, `rtos_sema_take`) are not permitted.
  * @param  base  First interface number of this class
  * @retval None
  */
static void usbd_uvc_set_interface_base(u8 base)
{
	usbd_uvc_dev.if_base = base;
}
#endif

/**
  * @brief  Get USB descriptor callback for UVC device
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `usb_os_malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev   USB device instance
  * @param  req   USB setup request
  * @param  buf   Buffer to store descriptor data
  * @retval Length of descriptor returned
  */
static u16 usbd_uvc_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf, u16 buf_len)
{
	usbd_uvc_dev_t *cdev = &usbd_uvc_dev;
	usb_speed_type_t speed = dev->dev_speed;
	u16 len = 0;
	const u8 *desc = NULL;
	u8 type = USB_HIGH_BYTE(req->wValue);
	u8 is_cfg = 0;
	u8 attr = 0x80U;

	if (cdev->from_composite == 0U) {
#ifdef CONFIG_USBD_SELF_POWERED
		attr |= USB_CFG_DESC_OFFSET_ATTR_BIT_SELF_POWERED;
#endif
#ifdef CONFIG_USBD_REMOTE_WAKEUP_EN
		attr |= USB_CFG_DESC_OFFSET_ATTR_BIT_REMOTE_WAKEUP;
#endif
	}

	switch (type) {

	case USB_DESC_TYPE_DEVICE:
		RTK_LOGS(TAG, RTK_LOG_INFO, "Get descriptor USB_DESC_TYPE_DEVICE\n");
		desc = usbd_uvc_dev_desc;
		len = sizeof(usbd_uvc_dev_desc);
		break;

	case USB_DESC_TYPE_CONFIGURATION:
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Get descriptor USB_DESC_TYPE_CONFIGURATION\n");

		desc = usbd_uvc_descriptors;
		len  = usbd_uvc_descriptors_size;
		is_cfg = 1;

		RTK_LOGS(TAG, RTK_LOG_INFO, "desc_self %p len %d\r\n", desc, len);
		break;

#ifndef CONFIG_USB_FS
	case USB_DESC_TYPE_DEVICE_QUALIFIER:
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Get descriptor USB_DESC_TYPE_DEVICE_QUALIFIER\n");

		desc = usbd_uvc_device_qualifier_desc;
		len = sizeof(usbd_uvc_device_qualifier_desc);
		break;

	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Get descriptor USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION\n");

		desc = usbd_uvc_descriptors;
		len  = usbd_uvc_descriptors_size;
		is_cfg = 1;

		RTK_LOGS(TAG, RTK_LOG_INFO, "Use the array for uvc descriptors\r\n");
		break;
#endif

	case USB_DESC_TYPE_STRING:
		switch (USB_LOW_BYTE(req->wValue)) {
		case USBD_IDX_LANGID_STR:
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "Get descriptor USBD_IDX_LANGID_STR\n");

			desc = usbd_uvc_lang_id_desc;
			len = sizeof(usbd_uvc_lang_id_desc);
			break;
		case USBD_IDX_MFC_STR:
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "Get descriptor USBD_IDX_MFC_STR\n");

			len = usbd_get_str_descriptor(USBD_UVC_MFG_STRING, buf, buf_len);
			break;
		case USBD_IDX_PRODUCT_STR:
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "Get descriptor USBD_IDX_PRODUCT_STR\n");

			if (speed == USB_SPEED_HIGH) {
				len = usbd_get_str_descriptor(USBD_UVC_MFG_HS_STRING, buf, buf_len);
			} else {
				len = usbd_get_str_descriptor(USBD_UVC_MFG_FS_STRING, buf, buf_len);
			}
			break;
		case USBD_IDX_SERIAL_STR:
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "Get descriptor USBD_IDX_SERIAL_STR\n");

			len = usbd_get_str_descriptor(USBD_UVC_SN_STRING, buf, buf_len);
			break;
		default:
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Get descriptor failed, invalid string index %d\n", USB_LOW_BYTE(req->wValue));

			break;
		}
		break;

	default:
		break;
	}

	if (desc != NULL) {
		/* Truncation is not allowed: a short descriptor is illegal, so stall instead */
		if (len > buf_len) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Desc %d OVSZ %d > %d\n", type, len, buf_len);
			return 0;
		}

		usb_os_memcpy((void *)buf, (const void *)desc, len);
	}

	if (is_cfg != 0) {
		if (cdev->from_composite == 0U) {
			buf[USB_CFG_DESC_OFFSET_ATTR] = attr;
		}
		/* Patch the copy in buf, never the source template: get_descriptor() is invoked
		 * repeatedly and the source must stay pristine. */
		usbd_uvc_patch_desc(buf + USB_LEN_CFG_DESC, (u16)(len - USB_LEN_CFG_DESC));
	}

	return len;
}

/**
  * @brief  Set UVC device configuration
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `usb_os_malloc`, `rtos_sema_take`) are not permitted.
  *         Initialize isochronous IN endpoint for video streaming
  * @param  dev     USB device instance
  * @param  config  Configuration index
  * @retval HAL status
  */
static int usbd_uvc_set_config(usb_dev_t *dev, u8 config)
{
	usbd_uvc_dev_t *cdev = &usbd_uvc_dev;
	usbd_ep_t *ep_isoc_in = &cdev->ep_isoc_in;
	usb_ep_info_t *info = &ep_isoc_in->info;
	int ret = HAL_OK;

	/* Only the bConfigurationValue advertised in the config descriptor is valid */
	if (config != 1U) {
		return HAL_ERR_PARA;
	}

	cdev->dev = dev;

	if (cdev->from_composite == 0U) {
#ifdef CONFIG_USBD_SELF_POWERED
		dev->self_powered = 1;
#else
		dev->self_powered = 0;
#endif
#ifdef CONFIG_USBD_REMOTE_WAKEUP_EN
		dev->remote_wakeup_en = 1;
#else
		dev->remote_wakeup_en = 0;
#endif
	}

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "Set config %d\n", config);
	/* Init ISOC IN EP */
	info->mps = USBD_UVC_ISOC_EP_MPS;
	info->binterval = USBD_UVC_ISOC_EP_BINTERVAL;
	usbd_ep_init(dev, ep_isoc_in);
	return ret;
}

/**
  * @brief  Clear UVC device configuration
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `usb_os_malloc`, `rtos_sema_take`) are not permitted.
  *         De-initialize isochronous endpoint
  * @param  dev     USB device instance
  * @param  config  Configuration index
  * @retval None
  */
static void usbd_uvc_clear_config(usb_dev_t *dev, u8 config)
{
	usbd_uvc_dev_t *cdev = &usbd_uvc_dev;
	usbd_ep_t *ep_bulk_in = &cdev->ep_isoc_in;
	usbd_ep_deinit(dev, ep_bulk_in);
	RTK_LOGS(TAG, RTK_LOG_DEBUG, "Clear config %d\n", config);
}

/**
  * @brief  Handle USB setup requests for UVC class and standard requests
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `usb_os_malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev  USB device instance
  * @param  req  USB setup request
  * @retval HAL status
  */
static int usbd_uvc_setup(usb_dev_t *dev, usb_setup_req_t *req)
{
	usbd_uvc_req_data_t req_data = { 0U };
	usbd_uvc_dev_t *cdev = &usbd_uvc_dev;
	usbd_ep_t *ep0_in = &dev->ep0_in;
	usbd_ep_t *ep0_out = &dev->ep0_out;
	int ret = HAL_OK;
	RTK_LOGS(TAG, RTK_LOG_DEBUG, "Setup bmRequestType=0x%08x bRequest=0x%08x wLength=0x%08x wValue=0x%08x wIndex=0x%08x\n",
			 req->bmRequestType,
			 req->bRequest,
			 req->wLength,
			 req->wValue,
			 req->wIndex
			);

	switch (req->bmRequestType & USB_REQ_TYPE_MASK) {
	case USB_REQ_TYPE_STANDARD:
		switch (req->bRequest) {
		case USB_REQ_SET_INTERFACE:
			if (dev->dev_state != USBD_STATE_CONFIGURED) {
				ret = HAL_ERR_PARA;
			} else if ((req->wIndex != USBD_UVC_INTF_CONTROL) && (req->wIndex != USBD_UVC_INTF_STREAMING)) {
				/* Ref USB 2.0 Table 9-10: the whole wIndex is the interface number, and a
				   foreign interface must be rejected so composite dispatch is not broken */
				ret = HAL_ERR_PARA;
			} else if (usbd_uvc_set_interface(dev, (u8)req->wIndex, USB_LOW_BYTE(req->wValue)) != 0U) {
				ret = HAL_ERR_PARA;
			} else {
				/* SET_INTERFACE accepted */
			}
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "USB_REQ_SET_INTERFACE\n");
			break;
		case USB_REQ_GET_INTERFACE:
			if (dev->dev_state != USBD_STATE_CONFIGURED) {
				ret = HAL_ERR_PARA;
			} else if (req->wIndex == USBD_UVC_INTF_CONTROL) {
				/* Ref USB 2.0 9.4.4: the VC interface has one alternate setting only */
				ep0_in->xfer_buf[0] = 0U;
				ep0_in->xfer_len = 1U;
				usbd_ep_transmit(dev, ep0_in);
			} else if (req->wIndex == USBD_UVC_INTF_STREAMING) {
				/* Report the alternate setting actually in use, else the host may believe the
				   stream is stopped while the ISOC IN endpoint is still active */
				ep0_in->xfer_buf[0] = (cdev->running != 0U) ? 1U : 0U;
				ep0_in->xfer_len = 1U;
				usbd_ep_transmit(dev, ep0_in);
			} else {
				ret = HAL_ERR_PARA;
			}
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "USB_REQ_GET_INTERFACE\n");
			break;
		case USB_REQ_GET_STATUS:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				ep0_in->xfer_buf[0] = 0U;
				ep0_in->xfer_buf[1] = 0U;
				ep0_in->xfer_len = 2U;
				usbd_ep_transmit(dev, ep0_in);
			} else {
				ret = HAL_ERR_PARA;
			}
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "USB_REQ_GET_STATUS\n");
			break;
		}
		break;
	case USB_REQ_TYPE_CLASS :
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "USB_REQ_TYPE_CLASS\n");
		/* Ref UVC 1.5 Table 4-1: the low byte of wIndex is the interface number and the high
		   byte is the entity ID. Only the VC and VS interfaces of this function are handled;
		   anything else must be stalled instead of leaving the data stage unanswered. */
		if (((req->bmRequestType & USB_REQ_RECIPIENT_MASK) != USB_REQ_RECIPIENT_INTERFACE)
			|| ((USB_LOW_BYTE(req->wIndex) != USBD_UVC_INTF_CONTROL) && (USB_LOW_BYTE(req->wIndex) != USBD_UVC_INTF_STREAMING))) {
			ret = HAL_ERR_PARA;
			break;
		}
		req_data.type = USBD_UVC_EVENT_SETUP;
		usb_os_memcpy((void *)&req_data.req, (const void *)req, (u32)sizeof(req_data.req));
		if ((req->bmRequestType == USBD_UVC_BMREQTYPE_CLASS_INTF_OUT) && (req->wLength > 0U)) {
			/* Ref USB 2.0 8.5.3: an H2D class request with a data stage cannot be dispatched
			   until the payload arrives. ctrl_data_len bounds the copy done by
			   usbd_uvc_handle_ep0_data_out(), whose destination holds 64 bytes, so a larger
			   wLength is rejected here instead of overflowing it. */
			if (req->wLength > sizeof(req_data.uvc_data.data)) {
				ret = HAL_ERR_PARA;
				break;
			}
			cdev->ctrl_req = req->bRequest;
			cdev->ctrl_data_len = (u8)req->wLength;
			cdev->ctrl_req_pending = 1U;
			ep0_out->xfer_len = req->wLength;
			if (usbd_ep_receive(dev, ep0_out) != HAL_OK) {
				/* The data stage never started, so no EP0 OUT completion will arrive to
				   consume the pending request. Drop it, else the next unrelated request's
				   data stage would be delivered as this one's payload. */
				cdev->ctrl_req_pending = 0U;
				ret = HAL_ERR_HW;
				break;
			}
		} else {
			//usbd_uvc_events_process(cdev, &req_data);
			//rtos_queue_send(cdev->uvc_cmd_queue, &req_data, 0);
		}
		usbd_uvc_events_process(cdev, &req_data);
		break;
	default:
		ret = HAL_ERR_HW;
		break;
	}
	return ret;
}
/**
  * @brief  Handle SET_INTERFACE request for UVC streaming
  *         - Alt 1: Start video streaming
  *         - Alt 0: Stop video streaming and release buffers
  * @param  dev       USB device instance
  * @param  interface Interface number
  * @param  alt       Alternate setting value
  * @retval Status
  */
static u8 usbd_uvc_set_interface(usb_dev_t *dev, u8 interface, u8 alt)
{
	usbd_uvc_dev_t *cdev = &usbd_uvc_dev;
	usbd_uvc_video_t *video = &cdev->video;
	usbd_ep_t *ep_isoc_in = &cdev->ep_isoc_in;
	usb_ep_info_t *info = &ep_isoc_in->info;
	u8 output_q_empty;

	if (cdev->init_done == 0U) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Set_interface: UVC not initialized! init_done=%d\n", cdev->init_done);
		return 1U;
	}

	/* Ref USB 2.0 9.4.10: the VC interface has alt 0 only, the VS interface has alt 0 and 1.
	   Any other combination is a request error. */
	if (((interface == USBD_UVC_INTF_CONTROL) && (alt != 0U)) || (alt > 1U)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Set_interface: bad if %d alt %d\n", interface, alt);
		return 1U;
	}

	if (interface == 1 && alt == 1) {
		if (cdev->running == 0U) {
			cdev->frame_done = 0U;
			info->mps = USBD_UVC_ISOC_EP_MPS;
			info->binterval = USBD_UVC_ISOC_EP_BINTERVAL;
			usbd_ep_init(dev, ep_isoc_in);
			/* Start with an empty payload ring; SOF/XFRC will arm once the producer fills it. */
			usb_ringbuf_reset(&video->in_rb);
			video->armed = 0U;
			video->stall_sof = 0U;
			video->buf_used = 0U;
			video->fid = 0U;
			RTK_LOGS(TAG, RTK_LOG_INFO, "Start to send the frame\n");
			cdev->running = 1U;
			usb_os_sema_give(video->output_queue_sema);
			if (cdev->change_parm_cb != NULL) {
				cdev->change_parm_cb(cdev->uvc_format_ptr);
			}
		}
	} else if (interface == 1 && alt == 0) {
		if (cdev->running != 0U) {
			cdev->running = 0U;
			usbd_ep_deinit(dev, ep_isoc_in);

			output_q_empty = (u8)list_empty(&video->output_queue);

			if (output_q_empty != 0U) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "Output queue empty\n");
			} else {
				RTK_LOGS(TAG, RTK_LOG_INFO, "Output queue full\n");
			}

			if (list_empty(&video->input_queue)) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "Input queue empty\n");
			} else {
				RTK_LOGS(TAG, RTK_LOG_INFO, "Input queue full\n");
			}
			/* Stop the payload ring and wake the producer if it is blocked on ring space. */
			video->armed = 0U;
			video->stall_sof = 0U;
			usb_ringbuf_reset(&video->in_rb);
			usb_os_sema_give(video->in_rb_space_sema);

			/* Do NOT call usbd_uvc_free_uvcd_list_buffer() here: this runs in ISR
			   context, but that helper takes output_lock via rtos_mutex_take(),
			   which is invalid from an ISR and corrupts the mutex, wedging
			   usbd_uvc_video_out_stream_queue() forever after a stop/restart.
			   The buffer already flows back via usbd_uvc_wait_frame_down() in
			   task context regardless of cdev->running. */
			video->uvc_buffer.bytesused = 0U; /* reset for next frame */
			video->buf_used = 0U; /* reset buffer offset */

			/* Only rescue output_frame_sema if the buffer is actually missing
			   from output_queue (i.e. a producer may be blocked in
			   usbd_uvc_wait_frame_down() waiting for it). Giving it
			   unconditionally, when the buffer is already sitting idle in
			   output_queue, hands out a spurious credit that the next cycle's
			   wait_frame_down() consumes prematurely - it returns before its
			   own frame is actually queued, which desyncs the single-buffer
			   handoff and wedges the pipeline on the following stream restart. */
			if (output_q_empty != 0U) {
				usb_os_sema_give(video->output_frame_sema);
			}
			cdev->frame_done = 0U;
		}
		cdev->running = 0U;
	}
	RTK_LOGS(TAG, RTK_LOG_INFO, "Interface %d alt %d\n", interface, alt);
	return 0U;
}

#if USBD_UVC_DEBUG
/**
  * @brief  Periodic stats dump (called from dump thread every USBD_UVC_DUMP_INTERVAL_MS)
  * @retval None
  */
static void usbd_uvc_dump(void)
{
	const usbd_uvc_dev_t *cdev = &usbd_uvc_dev;
	const usbd_uvc_video_t *video = &cdev->video;

	/* Guard: ring buffer and TX counters are only valid during active streaming */
	if ((cdev->init_done == 0U) || (cdev->running == 0U)) {
		return;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO,
			 "run=%u arm=%u sof=%u rb(h=%u,t=%u,cap=%u) "
			 "tx_pl=%u tx_fr=%u incomp=%u urun=%u\n",
			 (u32)cdev->running, (u32)video->armed, (u32)video->sof_count,
			 video->in_rb.head, video->in_rb.tail, video->in_rb.capacity,
			 video->tx_payloads, video->tx_frames,
			 video->incomp_cnt, video->underrun_cnt);
}

/**
  * @brief  Stats dump task (periodic); prints ring buffer state and TX/error counters.
  * @param  param Task parameter (unused)
  * @retval None
  */
static void usbd_uvc_dump_thread(void *param)
{
	usbd_uvc_dev_t *cdev = &usbd_uvc_dev;

	UNUSED(param);

	cdev->dump_task_alive = 1U;
	while (cdev->dump_task_exit != 1U) {
		usbd_uvc_dump();
		if (cdev->dump_task_exit == 1U) {
			break;
		}
		rtos_time_delay_ms(USBD_UVC_DUMP_INTERVAL_MS);
	}
	cdev->dump_task_alive = 0U;
	rtos_task_delete(NULL);
}
#endif /* USBD_UVC_DEBUG */

/**
  * @brief  UVC video transmit handler task
  *         This task retrieves prepared video buffers from the input queue
  *         and sends them to host via isochronous IN endpoint.
  * @param  parm Task parameter (unused)
  * @retval None
  */
static void usbd_uvc_get_frame_handler(void *parm)
{
	usbd_uvc_dev_t *cdev = &usbd_uvc_dev;
	usbd_uvc_video_t *video = &cdev->video;
	usbd_uvc_buffer_t *payload = NULL;
	u32 i = 0U;

	(void)parm;

	RTK_LOGS(TAG, RTK_LOG_INFO, "Get frame handler started\n");
	while (cdev->init_done != 0U) {

		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Get_frame: wait sema, running=%d\n", cdev->running);
		usb_os_sema_take(video->output_queue_sema, RTOS_MAX_DELAY);
		/* deinit() gives this sema after clearing init_done to wake us for shutdown. */
		if (cdev->init_done == 0U) {
			break;
		}
		if (usbd_uvc_get_status() == 0) {
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "Get_frame: not running, continue\n");
			continue;
		}

		payload = NULL;
		for (i = 0U; i < USBD_UVC_FRAME_WAIT_ITER; i++) {
			payload = usbd_uvc_video_in_stream_queue(cdev);
			if (payload != NULL) {
				/* usbd_uvc_video_in_stream_queue() already refreshed video->uvc_buffer;
				   re-copying *payload here would clobber its just-fixed-up buffer_list. */
				break;
			}
			rtos_time_delay_ms(1);
			if ((cdev->running == 0U) && (i > USBD_UVC_FRAME_STOP_ITER)) {
				break;
			}
		}

		if (payload == NULL) {
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "Get_frame: no payload after %u iters (out_q empty?)\n",
					 (unsigned)USBD_UVC_FRAME_WAIT_ITER);
			continue;
		}

		/* Encode the whole frame into the payload ring (all usb_os_memcpy in task ctx).
		   The ISR (XFRC / SOF) drains the ring and drives the ISOC IN endpoint. */
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Get_frame: produce sz=%u frames=%u\n",
				 (unsigned)video->uvc_buffer.bytesused, (unsigned)video->tx_frames);
		usbd_uvc_video_produce_frame(cdev->dev, video->uvc_buffer.mem, video->uvc_buffer.bytesused);
		video->tx_frames++;

		/* Frame fully queued into the ring; release it so the app can supply the next.
		   Guard: if set_interface(alt=0) fired while produce_frame was running, the
		   stop handler already gave output_frame_sema to unblock wait_frame_down.
		   Giving it again here would leave a spurious count; the next stream's
		   wait_frame_down() would consume it prematurely, call put_out_stream_queue
		   while the buffer is still in input_queue (list guard → no-op), and then
		   out_stream_queue polls on an empty queue forever — deadlock. */
		if (cdev->running != 0U) {
			usb_os_sema_give(video->output_frame_sema);
		}
	}
	cdev->frame_task_alive = 0U;
	rtos_task_delete(NULL);
}

/**
  * @brief  Init of UVC Extension Handle
  */
__weak void usbd_ext_init(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "Initialization of extension unit handle\n");
}

static void usbd_uvc_patch_ep_addresses(u8 old_addr, u8 new_addr)
{
	int i;
	for (i = 0; i < usbd_uvc_descriptors_size; i++) {
		if (usbd_uvc_descriptors[i] == old_addr) {
			usbd_uvc_descriptors[i] = new_addr;
		}
	}
}

/**
  * @brief  Initialize USB UVC device and related resources
  *         - Initialize USB device stack
  *         - Register UVC class driver
  *         - Create queues, semaphores, and tasks
  *         - Prepare video streaming environment
  * @param  ep_cfg: Endpoint configuration
  * @retval HAL_OK if success, otherwise error code
  */
static int usbd_uvc_private_init(const usbd_uvc_ep_cfg_t *ep_cfg)
{
	usbd_uvc_dev_t *dev = &usbd_uvc_dev;
	usbd_uvc_video_t *video = &dev->video;
	usb_ep_info_t *info = &dev->ep_isoc_in.info;
	int ret = HAL_OK;

	usbd_uvc_parameter_init();
	usbd_ext_init();
	usb_os_memset(dev, 0U, (u32)sizeof(usbd_uvc_dev_t));
	/* Standalone default; the composite framework rebases it via set_interface_base() */
	dev->if_base = 0;
	dev->probe = usbd_uvc_probe;
	dev->commit = usbd_uvc_commit;

	if (ep_cfg == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid EP cfg\n");
		return HAL_ERR_PARA;
	}

	dev->ep_cfg = ep_cfg;

	usbd_uvc_patch_ep_addresses(USBD_UVC_ISO_IN_EP, dev->ep_cfg->iso_in_addr);

	usbd_uvc_cmd_queue_init();

	INIT_LIST_HEAD(&video->input_queue);
	INIT_LIST_HEAD(&video->output_queue);
	INIT_LIST_HEAD(&video->uvc_buffer.buffer_list);
	usb_os_lock_create(&video->input_lock);
	usb_os_lock_create(&video->output_lock);

	INIT_LIST_HEAD(&dev->bod_list);
	usb_os_lock_create(&dev->bod_mutex);

	usb_os_lock_create(&dev->lock);

	usb_os_sema_create(&dev->uvc_cmd_wakeup_sema);
	usb_os_sema_create(&video->output_queue_sema);
	usb_os_sema_create(&video->output_frame_sema);
	usb_os_sema_create(&video->in_rb_space_sema);

	/* Payload ring: USBD_UVC_IN_SLOT_CNT nodes of one microframe payload each,
	   cache-line aligned so the TX DMA can read each node in place. */
	if (usb_ringbuf_manager_init(&video->in_rb, USBD_UVC_IN_SLOT_CNT, USBD_UVC_IN_BUF_SIZE, 1) != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "UVC payload ring alloc fail\n");
		ret = -1;
		goto exit;
	}
	video->armed = 0U;
	video->stall_sof = 0U;
	video->sof_count = 0U;

	usb_os_queue_create(&video->complete_bf_req, USBD_UVC_COMPLETE_QUEUE_DEPTH, sizeof(int));

	info->addr = dev->ep_cfg->iso_in_addr;
	info->type = USB_CH_EP_TYPE_ISOC;
	info->binterval = USBD_UVC_ISOC_EP_BINTERVAL;

	usb_os_memset(&s_uvc_format, 0U, (u32)sizeof(s_uvc_format));
	dev->uvc_format_ptr = &s_uvc_format;
	usb_os_sema_create(&dev->uvc_format_ptr->uvcd_change_sema);

	/* Mark as initialized before creating tasks so tasks can run properly */
	dev->init_done = 1U;

	/* Set the alive flag before creating the task (not inside it) so a very
	   early deinit() cannot race past the join while the task is still starting. */
	dev->cmd_task_alive = 1U;
	ret = rtos_task_create(NULL, "usbd_uvc_cmd_handler", usbd_uvc_cmd_handler, NULL, USBD_UVC_CMD_TASK_STACK_SIZE, USBD_UVC_CMD_TASK_PRIO);
	if (ret != RTK_SUCCESS) {
		dev->cmd_task_alive = 0U;
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create USBD UVC CMD thread fail\n");
		ret = -1;
		goto exit;
	}

	dev->frame_task_alive = 1U;
	ret = rtos_task_create(NULL, "usbd_uvc_get_frame_handler", usbd_uvc_get_frame_handler, NULL, USBD_UVC_FRAME_TASK_STACK_SIZE, USBD_UVC_FRAME_TASK_PRIO);
	if (ret != RTK_SUCCESS) {
		dev->frame_task_alive = 0U;
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create USBD UVC GET FRAME thread fail\n");
		ret = -1;
		goto exit;
	}

	dev->uvc_in_buf = usb_os_malloc(dev->ep_cfg->iso_in_xfer_size ? dev->ep_cfg->iso_in_xfer_size : USBD_UVC_IN_BUF_SIZE);

#if USBD_UVC_DEBUG
	dev->dump_task_exit = 0U;
	dev->dump_task_alive = 0U;
	ret = rtos_task_create(&dev->dump_task, "usbd_uvc_dump", usbd_uvc_dump_thread, NULL, USBD_UVC_DUMP_TASK_STACK_SIZE, USBD_UVC_DUMP_TASK_PRIO);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create USBD UVC dump thread fail\n");
		ret = -1;
		goto exit;
	}
#endif

exit:
	return ret;
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Initialize UVC device as a standalone device (from_composite = 0)
  * @retval Status
  */
int usbd_uvc_init(const usbd_uvc_ep_cfg_t *ep_cfg)
{
	usbd_uvc_dev_t *dev = &usbd_uvc_dev;
	int ret;

	dev->from_composite = 0;
	ret = usbd_uvc_private_init(ep_cfg);
	if (ret == HAL_OK) {
		usbd_register_class(&usbd_uvc_driver);
	}
	return ret;
}

#ifdef CONFIG_USBD_COMPOSITE
/**
  * @brief  Initialize UVC device as part of a composite device (from_composite = 1)
  * @retval Status
  */
int usbd_composite_uvc_init(const usbd_uvc_ep_cfg_t *ep_cfg)
{
	usbd_uvc_dev_t *dev = &usbd_uvc_dev;
	int ret;

	dev->from_composite = 1;
	ret = usbd_uvc_private_init(ep_cfg);
	if (ret == HAL_OK) {
		ret = usbd_composite_register_driver(&usbd_uvc_driver);
	}
	return ret;
}
#endif

/**
  * @brief  De-Initialize UVC device
  *         - Unregister UVC class
  *         - Delete tasks, queues, semaphores, and locks
  *         - Reset internal buffer lists
  * @retval None
  */
void usbd_uvc_deinit(void)
{
	/* Drop any request still waiting for its data stage, so a re-init cannot inherit it and
	   dispatch a stale payload. The unregister below stops all further class callbacks. */
	usbd_uvc_dev.ctrl_req_pending = 0U;

#ifdef CONFIG_USBD_COMPOSITE
	if (usbd_uvc_dev.from_composite != 0U) {
		usbd_composite_unregister_driver(&usbd_uvc_driver);
	} else
#endif
	{
		usbd_uvc_dev_t *cdev = &usbd_uvc_dev;
		usbd_uvc_video_t *video = &cdev->video;
		usbd_uvc_req_data_t wakeup = { 0U };
		u8 wait_cnt = 0U;

		usbd_unregister_class();
		cdev->init_done = 0U;
		cdev->running = 0U;

		/* Wake the worker tasks so they observe init_done == 0 and self-delete
		BEFORE we usb_os_mfree the queue/semaphores they block on (delete-under-blocked-
		receiver would corrupt the object / crash). Order: wake -> join -> usb_os_mfree. */
		if (cdev->uvc_cmd_queue != NULL) {
			/* cmd_handler is parked in rtos_queue_receive(RTOS_MAX_DELAY). */
			rtos_queue_send(cdev->uvc_cmd_queue, &wakeup, 0);
		}
		/* get_frame_handler is parked on output_queue_sema; the producer may also be
		parked on in_rb_space_sema inside usbd_uvc_video_produce_frame(). */
		usb_os_sema_give(video->output_queue_sema);
		usb_os_sema_give(video->in_rb_space_sema);

		/* Join: wait (bounded) for both tasks to clear their alive flags. */
		while (((cdev->cmd_task_alive != 0U) || (cdev->frame_task_alive != 0U)) && (wait_cnt < 100U)) {
			rtos_time_delay_ms(10U);
			wait_cnt++;
		}

#if USBD_UVC_DEBUG
		cdev->dump_task_exit = 1U;
		wait_cnt = 0U;
		do {
			rtos_time_delay_ms(10U);
			wait_cnt++;
		} while ((cdev->dump_task_alive != 0U) && (wait_cnt < 100U));
#endif

		usb_os_lock_delete(cdev->lock);
		usb_os_sema_delete(cdev->uvc_cmd_wakeup_sema);
		usb_os_sema_delete(video->output_queue_sema);
		usb_os_sema_delete(video->output_frame_sema);
		usb_os_sema_delete(video->in_rb_space_sema);
		usb_os_queue_delete(video->complete_bf_req);
		/* uvc_cmd_queue was created via rtos_queue_create() and was previously
		leaked on deinit; usb_os_mfree it (matching API) now that no task waits on it. */
		if (cdev->uvc_cmd_queue != NULL) {
			rtos_queue_delete(cdev->uvc_cmd_queue);
			cdev->uvc_cmd_queue = NULL;
		}
		usb_os_lock_delete(cdev->bod_mutex);
		usb_os_lock_delete(video->input_lock);
		usb_os_lock_delete(video->output_lock);
		usb_ringbuf_manager_deinit(&video->in_rb);
		INIT_LIST_HEAD(&video->input_queue);
		INIT_LIST_HEAD(&video->output_queue);
		INIT_LIST_HEAD(&cdev->bod_list);

		if (usbd_uvc_dev.uvc_in_buf) {
			usb_os_mfree(usbd_uvc_dev.uvc_in_buf);
			usbd_uvc_dev.uvc_in_buf = NULL;
		}
	}
}
/**
  * @brief  Register UVC parameter change callback
  *         - Set callback function for UVC parameter updates
  *         - Triggered when format, resolution, or FPS changes
  * @param  cb: Callback function pointer (void (*)(void *))
  * @retval None
  * @note
  *         - Callback should be non-blocking
  *         - May be called from control or streaming context
  */
void usbd_uvc_set_change_parm_cb(int cb)
{
	usbd_uvc_dev.change_parm_cb = (void (*)(void *))cb;
}
/**
  * @brief  Get current UVC format configuration
  *         - Return pointer to internal UVC format structure
  *         - Contains resolution, format, FPS, and ISP settings
  * @retval Pointer to usbd_uvc_format_t
  * @note
  *         - Do not usb_os_mfree returned pointer
  *         - Ensure UVC is initialized before calling
  *         - Shared resource, may require synchronization
  */
usbd_uvc_format_t *usbd_uvc_get_format(void)
{
	return usbd_uvc_dev.uvc_format_ptr;
}
