/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBD_UVCD_H
#define USBD_UVCD_H

/* Includes ------------------------------------------------------------------*/
#include "dlist.h"
#include "usbd_video.h"
#include "usb_ringbuf.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported defines ----------------------------------------------------------*/
#define USBD_UVC_INTF_CONTROL           0
#define USBD_UVC_INTF_STREAMING         1

#define USBD_UVC_EVENT_FIRST            0
#define USBD_UVC_EVENT_CONNECT          1
#define USBD_UVC_EVENT_DISCONNECT       2
#define USBD_UVC_EVENT_STREAMON         3
#define USBD_UVC_EVENT_STREAMOFF        4
#define USBD_UVC_EVENT_SETUP            5
#define USBD_UVC_EVENT_DATA             6
#define USBD_UVC_EVENT_LAST             7

#define USBD_UVC_CMD_THREAD_PRIORITY    2

#define USBD_UVC_NUM_REQUESTS           2
#define USBD_UVC_MAX_REQUEST_SIZE       64
#define USBD_UVC_MAX_EVENTS             4

#define USBD_TUNING_MODE                0
#if USBD_TUNING_MODE
#define USBD_TUNING_W                   (USBD_UVC_FRAME_WIDTH*2)
#define USBD_TUNING_H                   (USBD_UVC_FRAME_HEIGHT*3/2)
#else
#define USBD_TUNING_W                   USBD_UVC_FRAME_WIDTH
#define USBD_TUNING_H                   USBD_UVC_FRAME_HEIGHT
#endif


#define USBD_UVC_WEBCAM_VENDOR_ID       USB_VID
#define USBD_UVC_WEBCAM_PRODUCT_ID      USB_PID
#define USBD_UVC_WEBCAM_DEVICE_BCD      0x0010
#define USBD_UVC_LANGID_STRING          0x0409U

#define USBD_UVC_MFG_STRING             "Realtek"
#define USBD_UVC_MFG_HS_STRING          "USB UVC CLASS"
#define USBD_UVC_MFG_FS_STRING          "USB UVC CLASS"
#define USBD_UVC_SN_STRING              "1234567890"

#define USBD_UVC_MAKE_SUBCMD(data,dir,subcmd) ((u8)(data)<<7|(u8)(dir)<<6|subcmd)

#define USBD_UVC_STRING_CONTROL_IDX     0
#define USBD_UVC_STRING_STREAMING_IDX   1

#define USBD_UVC_INTF_VIDEO_CONTROL     0
#define USBD_UVC_INTF_VIDEO_STREAMING   1

#define USBD_UVC_STATUS_MAX_PACKET_SIZE 64	/* 16 bytes status */

/* Platform-specific ISOC IN endpoint address and payload buffer size */
#if defined(CONFIG_AMEBAGREEN2)
#define USBD_UVC_ISO_IN_EP                   0x82U      /* EP2 IN */
#define USBD_UVC_IN_BUF_SIZE                 3072U
#elif defined(CONFIG_AMEBAPRO3)
#define USBD_UVC_ISO_IN_EP                   0x83U      /* EP3 IN */
#define USBD_UVC_IN_BUF_SIZE                 3072U      /* HS high-bandwidth: 3 x 1024 per microframe */
#else
#define USBD_UVC_ISO_IN_EP                   0x83U
#define USBD_UVC_IN_BUF_SIZE                 1024U
#endif

#define USBD_UVC_IN_SLOT_CNT                 10U        /* Payload ring depth; absorbs microframe-level jitter between producer task and ISR consumer */
#define USBD_UVC_PAYLOAD_HEADER_LEN          12U        /* UVC payload header: 2 (BFH) + 4 (PTS) + 6 (SCR: STC + SOF) */
#define USBD_UVC_STALL_SOF_MAX               2U         /* SOF watchdog: an ISOC IN xfer must finish within 1 microframe; >= this many SOFs while armed => incompISOIN, recover */
#define USBD_UVC_DUMP_INTERVAL_MS            2000U      /* Stats dump thread period in milliseconds */

/* ISOC IN endpoint configuration */
#define USBD_UVC_ISOC_EP_MPS                 1024U      /* Per-transaction MPS */
#define USBD_UVC_ISOC_EP_BINTERVAL           1U         /* bInterval for ISOC IN EP */
/* Transactions per microframe the endpoint descriptor must declare.
   Round UP: the payload occupies ceil(IN_BUF_SIZE / MPS) transactions, and the host
   only issues that many IN tokens per microframe. Truncating division here (the
   previous form) under-declares whenever IN_BUF_SIZE is not an exact multiple of MPS
   (e.g. 1100 -> declares 1 token while usbd_hal_ep_start_transfer() programs
   PKTCNT/MC = 2): the trailing transaction never gets a token, DIEPCTL.EPENA is never
   cleared, no XFRC arrives, and the SOF watchdog drops every payload. It also
   unsigned-underflows for IN_BUF_SIZE < MPS, corrupting wMaxPacketSize outright.
   USB 2.0 5.9.2 caps HS high-bandwidth at 3 transactions/microframe, so any
   IN_BUF_SIZE above 3 * MPS is not representable in wMaxPacketSize bits[12:11]. */
#define USBD_UVC_ISOC_EP_TRANS_CNT           (((USBD_UVC_IN_BUF_SIZE) + (USBD_UVC_ISOC_EP_MPS) - 1U) / (USBD_UVC_ISOC_EP_MPS))
/* USB HS ISOC wMaxPacketSize: bits[12:11] = extra transactions, bits[10:0] = MPS */
#define USBD_UVC_ISOC_EP_MPKT                ((((USBD_UVC_ISOC_EP_TRANS_CNT) - 1U) << 11U) | USBD_UVC_ISOC_EP_MPS)

/* UVC streaming control defaults */
#define USBD_UVC_PROBE_BMHINT                0x0003U    /* bmHint: keep frame interval and key frame rate constant */
#define USBD_UVC_YUY2_BPP                    2U         /* YUY2 bytes per pixel */
#define USBD_UVC_NV12_SIZE_NUM               3U         /* NV12 frame size: (W * H * 3) / 2 */
#define USBD_UVC_NV12_SIZE_DEN               2U
#define USBD_UVC_FRAME_INTERVAL_UNIT         10000000U  /* UVC frame interval base: 100 ns units per second */
#define USBD_UVC_GET_LEN_RESP_SIZE           2U         /* GET_LEN response length (wLength field, 2 bytes) */
#define USBD_UVC_GET_INFO_CAPS               0x03U      /* GET_INFO response: GET and SET supported */
#define USBD_UVC_FRAME_INTERVAL_CNT          4U         /* Number of frame intervals in usbd_uvc_frame_info_t.intervals[] */
#define USBD_UVC_BMREQTYPE_DIR_IN            0x80U      /* bmRequestType direction bit: device-to-host */
#define USBD_UVC_CLOCK_FREQUENCY             48000000U  /* Source clock frequency in Hz (48 MHz) */

/* Periodic stats dump thread. Set to 1 to enable; keeps the dump task and its
	struct fields out of production builds entirely. */
#define USBD_UVC_DEBUG                           0

/* Vendor Extension Unit (XU): disabled by default. The XU descriptor is only
	legal if the firmware actually backs every control it declares (bNumControls /
	bmControls) with GET/SET handlers; otherwise a strict host (e.g. Windows) fails
	enumeration. Enable only together with a real XU control implementation. */
#if defined(CONFIG_AMEBAGREEN2)
#define USBD_UVC_USE_EXTENSION_UNIT              0
#else
#define USBD_UVC_USE_EXTENSION_UNIT              1
#endif
#if USBD_UVC_USE_EXTENSION_UNIT
#define USBD_UVC_XU_DESC_LEN                     28U    /* bLength of the XU descriptor */
#define USBD_UVC_OT_SOURCE_ID                    0x03U  /* Output Terminal sourced from XU (unit id 3) */
#else
#define USBD_UVC_XU_DESC_LEN                     0U
#define USBD_UVC_OT_SOURCE_ID                    0x02U  /* Output Terminal sourced from PU (unit id 2) */
#endif

/* Exported macros -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/**
	* @brief UVC endpoint configuration structure.
	* @details Used to configure endpoint addresses at runtime.
	*/
typedef struct {
	u32 iso_in_xfer_size;  /**< ISOC IN transfer buffer size (0 = use default). */
	u8 iso_in_addr;    /**< ISOC IN endpoint address for video streaming */
} usbd_uvc_ep_cfg_t;

/**
	* @brief UVC frame information structure.
	* @details
	* Describes a single video frame size and its supported frame intervals.
	*/
typedef struct {
	u32 width;
	u32 height;
	u32 intervals[4];
} usbd_uvc_frame_info_t;

/**
	* @brief UVC video format information.
	* @details
	* Defines a video format and its supported frame resolutions.
	*/
typedef struct {
	u32 fcc;
	usbd_uvc_frame_info_t *frames;
} usbd_uvc_format_info_t;

/**
	* @brief UVC vendor-specific command structure.
	* @details
	* Used for vendor-defined control or data transfer commands over USB.
	*/
typedef struct {
	u8 cmd;
	u8 subcmd;
	u16 address;
	u16 length;
	u16 reserved;
	u8 buffer[8];
} __PACKED usbd_uvc_vendorcmd_t;

/**
	* @brief UVC Processing Unit control data.
	* @details
	* Stores control values returned by standard UVC GET requests
	* (e.g. brightness, contrast, saturation).
	*/
typedef struct {
	int cur;/* 0X81 */
	int min;/* 0X82 */
	int max;/* 0X83 */
	int res;/* 0X84 */
	int len;/* 0X85 */
	int info;/* 0X86 */
	int def;/* 0X87 */
} usbd_uvc_process_unit_t;

/**
	* @brief ISP USB device command data structure.
	* @details
	* Used for communication between USB and ISP firmware logic.
	*/
typedef struct {
	u8 buf[0x40];   /* The maximum buffer size — 64B, largest member */
	u32 offset;     /* to recode the index for read and write */
	u16 cmdcode;
	u16 param;
	u16 addr;
	u8 index;
	u8 length;
} usbd_uvc_isp_usbd_cmd_data_t;

/**
	* @brief UVC request payload data.
	* @details
	* Used to store data associated with a UVC control request.
	*/
typedef struct {
	u8 data[64];    /* 64B payload — largest member */
	int length;
} usbd_uvc_request_data_t;

/**
	* @brief USB UVC video buffer structure.
	* @details
	* Represents a single video buffer used for streaming.
	*/
typedef struct {
	struct list_head buffer_list;
	const u8 *mem;
	u32 bytesused;
	void *exbuf;
	int index;
} usbd_uvc_buffer_t;

/**
	* @brief UVC request data container.
	* @details
	* Wraps USB setup requests and UVC-specific request payloads.
	*/
typedef struct {
	u32 type;
	union {
		usb_setup_req_t req;
		usbd_uvc_request_data_t uvc_data;
		u8 buf[64];
	};
	struct list_head list;
} usbd_uvc_req_data_t;

/**
	* @brief UVC video streaming context.
	* @details
	* Holds all runtime information related to UVC video streaming,
	* including format, buffers, payload handling, and synchronization.
	*/
typedef struct {
	usbd_uvc_buffer_t uvc_buffer;
	/* ISOC IN payload ring (producer: get_frame task, consumer: ISR).
		Each node holds one microframe payload (USBD_UVC_PAYLOAD_HEADER_LEN + data = USBD_UVC_IN_BUF_SIZE). */
	usb_ringbuf_manager_t in_rb;     /**< Payload ring; TX DMA reads node->buf in place (zero-copy). */
	struct list_head input_queue;    /* 8B */
	struct list_head output_queue;   /* 8B */
	int stream_index;
	u32 fcc;
	u32 width;
	u32 height;
	u32 imagesize;
	u32 frm_type;
	u32 req_size;
	u32 payload_size;
	u32 max_payload_size;
	u32 buf_used;
	usb_os_sema_t in_rb_space_sema;  /**< Given by ISR when a node is consumed; producer waits on it when ring is full. */
	u32 cur_pts;                     /**< PTS (48MHz units) captured at frame start; constant for the whole frame. */
	/* Diagnostics counters — updated in ISR; read only by the dump thread */
	__IO u32 tx_payloads;  /**< Total microframe payloads transmitted (XFRC). */
	__IO u32 tx_frames;    /**< Total UVC frames fully queued into ring by producer. */
	__IO u32 incomp_cnt;   /**< incompISOIN recoveries: SOF watchdog triggered. */
	__IO u32 underrun_cnt; /**< Ring underrun: try_arm found empty ring. */
	usb_os_lock_t input_lock;
	usb_os_lock_t output_lock;
	usb_os_sema_t output_queue_sema;
	usb_os_sema_t output_frame_sema;
	usb_os_queue_t complete_bf_req;
	void *complete_bf_task;
	__IO u16 sof_count;              /**< Software SOF counter (11-bit) maintained in .sof; used as SCR SOF token. */
	__IO u8 armed;                   /**< 1 = an ISOC IN xfer is in flight (set/cleared only in ISR). */
	__IO u8 stall_sof;               /**< SOF count since arm; watchdog for incompISOIN recovery. */
	u8 fid;
	u8 format;
	u8 end_flag;
	u8 bpp;
} usbd_uvc_video_t;

/**
	* @brief UVC device context structure.
	* @details
	* Represents the complete UVC device state, including control,
	* streaming, USB endpoints, and synchronization primitives.
	*/
typedef struct {
	usbd_uvc_video_t video;
	usbd_uvc_streaming_control_t probe;
	usbd_uvc_streaming_control_t commit;
	usbd_uvc_req_data_t req_data;
	u8 result[64];             /* 64B response buffer */
	struct list_head bod_list; /* 8B: two list pointers */
	u32 event_setup_out : 1;
	int control;               /* control selector */
	int command_interface;     /* 0x00 = control, 0x01 = streaming */
	int command_entity;        /* 0x02 = process unit, 0x03 = extension unit */
	/**
		* @brief Called when UVC streaming parameters are committed/changed by the host.
		* @note   This function is called within an interrupt service routine (ISR) context;
		*         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
		* @param[in] uvc_format_ptr: Pointer to the active UVC format descriptor.
		*/
	void (*change_parm_cb)(void *);
	usb_dev_t *dev;
	usb_os_sema_t uvc_cmd_wakeup_sema;
#if USBD_UVC_DEBUG
	rtos_task_t dump_task;
#endif
	usb_os_lock_t bod_mutex;
	usb_os_lock_t lock;
	usbd_ep_t ep_isoc_in;
	rtos_queue_t uvc_cmd_queue;
	usbd_uvc_format_t *uvc_format_ptr;
	const usbd_uvc_ep_cfg_t *ep_cfg;
	u8 *uvc_in_buf;
	u16 event_length;
	u16 interface_number;
	u8 uvc_cmd_blocked;
#if USBD_UVC_DEBUG
	u8 dump_task_exit;
	u8 dump_task_alive;
#endif
	u8 running;
	u8 init_done;        /* 0: not initialized, 1: fully initialized */
	u8 cmd_task_alive;   /* 1 while usbd_uvc_cmd_handler runs; deinit joins on it */
	u8 frame_task_alive; /* 1 while usbd_uvc_get_frame_handler runs; deinit joins on it */
	u8 frame_done;
	u8 config;
	u8 ctrl_req;
	u8 ctrl_data_len;
	u8 ctrl_req_pending;    /**< 1 if ctrl_req is waiting for its EP0 OUT data stage. */
	u8 from_composite;      /**< Flag indicating if part of a composite device. */
	u8 if_base;             /**< First interface number of this class; 0 in standalone mode unless
	                             the composite framework rebases it via set_interface_base(). */
} usbd_uvc_dev_t;

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/


extern usbd_uvc_streaming_control_t usbd_uvc_probe;
extern usbd_uvc_streaming_control_t usbd_uvc_commit;
extern usbd_uvc_format_info_t uvcd_formats[];
extern const u32 uvcd_nformats;
extern u8 usbd_uvc_descriptors[];
extern int usbd_uvc_descriptors_size;

/**
	* @brief  Dispatch a UVC event (SETUP / DATA / STREAMON / STREAMOFF) to the event handlers.
	* @param[in]  dev        UVC device context.
	* @param[in]  uvc_event  Event container (type + setup request or data payload).
	*/
void usbd_uvc_events_process(usbd_uvc_dev_t *dev, usbd_uvc_req_data_t *uvc_event);

/**
	* @brief  Return a pointer to the internal UVC device context.
	* @retval Pointer to the singleton usbd_uvc_dev_t; never NULL after usbd_uvc_init().
	* @note   Caller must not free the returned pointer.
	*/
usbd_uvc_dev_t *get_private_usbd_uvcd(void);

/**
	* @brief  Return a completed video buffer to the output (free) queue.
	* @param[in]  payload  Buffer to return; must have been obtained from usbd_uvc_video_out_stream_queue().
	*/
void usbd_uvc_video_put_out_stream_queue(usbd_uvc_buffer_t *payload);

/**
	* @brief  Submit a filled video buffer to the input (ready-to-send) queue.
	* @param[in]  payload  Buffer containing a complete video frame; mem and bytesused must be set.
	*/
void usbd_uvc_video_put_in_stream_queue(usbd_uvc_buffer_t *payload);

/**
	* @brief  Dequeue one buffer from the output (free) queue.
	* @retval Pointer to a free usbd_uvc_buffer_t, or NULL if the queue is empty.
	*/
usbd_uvc_buffer_t *usbd_uvc_video_out_stream_queue(void);

/**
	* @brief  Block until the current video frame has been fully queued into the ring.
	* @note   Must not be called from ISR context.
	*/
void usbd_uvc_wait_frame_down(void);

/**
	* @brief  Query whether UVC streaming is currently active.
	* @retval 1 if streaming is running, 0 otherwise.
	*/
int usbd_uvc_get_status(void);

/**
	* @brief  Initialize the UVC device class and start its worker tasks.
	* @retval HAL_OK on success, negative error code on failure.
	*/
int usbd_uvc_init(const usbd_uvc_ep_cfg_t *ep_cfg);

#ifdef CONFIG_USBD_COMPOSITE
int usbd_composite_uvc_init(const usbd_uvc_ep_cfg_t *ep_cfg);
#endif

/**
	* @brief  Initialize UVC processing-unit and extension-unit control parameters.
	* @retval HAL_OK on success.
	*/
int usbd_uvc_parameter_init(void);

/**
	* @brief  Tear down the UVC device class: stop tasks, delete OS objects, free ring.
	*/
void usbd_uvc_deinit(void);

/**
	* @brief  Register a callback invoked when the host commits new streaming parameters.
	* @param[in]  cb  Function pointer cast to int; signature: void (*)(void *uvc_format_ptr).
	* @note   The callback is invoked in ISR context — must not block.
	*/
void usbd_uvc_set_change_parm_cb(int cb);

/**
	* @brief  Return a pointer to the active UVC format descriptor.
	* @retval Pointer to the internal usbd_uvc_format_t; valid after usbd_uvc_init().
	* @note   Do not free the returned pointer.
	*/
usbd_uvc_format_t *usbd_uvc_get_format(void);

#ifdef __cplusplus
}
#endif

#endif /* USBD_UVC_H */
