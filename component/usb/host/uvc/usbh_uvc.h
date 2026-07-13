/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_UVC_H
#define USBH_UVC_H

/* Includes ------------------------------------------------------------------*/

#include "usbh.h"
#include "usbh_uvc_desc.h"
#include "usbh_uvc_intf.h"

/* Exported defines ----------------------------------------------------------*/

#define USBH_UVC_IN_TERMINAL_MAX_NUM            5
#define USBH_UVC_OUT_TERMINAL_MAX_NUM           2
#define USBH_UVC_CAMERA_TERMINAL_MAX_NUM        2
#define USBH_UVC_PROCESSING_UNIT_MAX_NUM        2
#define USBH_UVC_EXTENSION_UNIT_MAX_NUM         2
#define USBH_UVC_ENCODING_UNIT_MAX_NUM          2
#define USBH_UVC_SELECTOR_UNIT_MAX_NUM          2

#define USBH_UVC_REQUEST_BUF_LEN                48 /**< Length of the UVC control request buffer. */

#define USBH_UVC_FRAME_INTERVAL_UNIT_NS              (10000000U) /**< UVC frame interval base unit: 100ns (10^7 * 100ns = 1s). */
#define USBH_UVC_PAYLOAD_HEADER_MIN_LEN              (2U)   /**< Minimum valid UVC payload header length in bytes. */



#define USBH_UVC_BCD_UVC_1_1                   (0x0110U)  /**< UVC 1.1 release number. */
#define USBH_UVC_BCD_UVC_1_5                   (0x0150U)  /**< UVC 1.5 release number. */

#define USBH_UVC_PROBE_SIZE_UVC10              (26U)      /**< Probe/Commit control size for UVC 1.0. */
#define USBH_UVC_PROBE_SIZE_UVC11              (34U)      /**< Probe/Commit control size for UVC 1.1. */
#define USBH_UVC_PROBE_SIZE_UVC15              (48U)      /**< Probe/Commit control size for UVC 1.5. */
/* Exported types ------------------------------------------------------------*/
typedef enum {
	UVC_STATE_IDLE = 0,
	UVC_STATE_CTRL,
	UVC_STATE_TRANSFER,
	UVC_STATE_STOP,
	UVC_STATE_ERROR
} usbh_uvc_state_t;

typedef enum {
	STREAM_STATE_CTRL_IDLE = 0,
	STREAM_STATE_SET_PARA,        // Set Param: Probe - Commit - Set alt
	STREAM_STATE_RESET_ALT,       // Set Interface/0
	STREAM_STATE_PROBE_NEGOTIATE, // Set Video (Probe1)
	STREAM_STATE_PROBE_UPDATE,    // Get Video (Probe2)
	STREAM_STATE_PROBE_FINAL,     // Set Video (Probe3)
	STREAM_STATE_COMMIT,          // Set Video (Commit)
	STREAM_STATE_FIND_ALT,        // Find Interface/Altsetting for transfer
	STREAM_STATE_SET_ALT,         // Set Interface/Altsetting for transfer
	STREAM_STATE_ERROR
} usbh_stream_state_t;

typedef struct {
	u32 dwMinBitRate;
	u32 dwMaxBitRate;
	u32 dwMaxVideoFrameBufferSize;   /* for mjpeg & uncompressed payload type descriptor */
	u32 dwDefaultFrameInterval;
	u32 *dwFrameInterval;
	u32 dwBytesPerLine;   /* for frame based payload frame type descriptor */
	u16 wWidth;
	u16 wHeight;
	u8  bFrameIndex;
	u8  bmCapabilities;
	u8  bFrameIntervalType;
} __PACKED usbh_uvc_vs_frame_t;

typedef struct {
	char name[32];
	usbh_uvc_vs_frame_t *frame;
	u32 nframes;
	u32 fcc;
	u32 flags;
	u8 type;
	u8 index;
	u8 bpp;
	u8 colorspace;
} usbh_uvc_vs_format_t;

typedef struct {
	usbh_uvc_vc_header_desc_t *vcheader;
	void *p;
	u8 bInterfaceNumber;
	u8 entity_num;
} usbh_uvc_vc_t;

typedef struct {
	void *p;
	usbh_ep_desc_t *endpoint;
} usbh_uvc_alt_t;

typedef struct {
	usbh_uvc_alt_t altsetting[USBH_UVC_VS_ALTS_MAX_NUM];
	usbh_uvc_vs_input_header_desc_t *InputHeader;
	usbh_uvc_vs_format_t *format;
	u32 nformat;
	void *p;
	u8 bInterfaceNumber;
	u8 alt_num;
	u8 has_h264;
} usbh_uvc_vs_t;

typedef struct {
	usbh_uvc_vs_t vs_intf[USBH_UVC_VS_DESC_MAX_NUM];
	usbh_uvc_vc_t vc_intf;
	u8 vs_num;
} usbh_uvc_cfg_t;

typedef struct {
	usbh_pipe_t pipe;
	usbh_uvc_alt_t *altsetting;
	usbh_uvc_vs_t *cur_vs_intf;
	u8 bAlternateSetting;
	u8 bInterfaceNumber;
	u8 valid;
} usbh_uvc_setting_t;

#if USBH_UVC_USE_HW == 0
typedef struct {
	u32 offset;
	u32 length;
} usbh_uvc_packet_desc_t;

typedef struct {
	u8 *addr;
	u8 index;
	u8 packet_num;
	u8 packet_length;
	usbh_uvc_packet_desc_t packet_info[];
} usbh_uvc_urb_t;
#endif

#if USBH_UVC_USE_HW
typedef struct {
	u8 err_ch0_buf0_ov;
	u8 err_ch0_buf1_ov;
	u8 err_ch0_buf0_h;
	u8 err_ch0_buf1_h;
	u8 err_ch1_buf0_ov;
	u8 err_ch1_buf1_ov;
	u8 err_ch1_buf0_h;
	u8 err_ch1_buf1_h;
} usbh_uvc_err_t;
#endif

typedef struct {
	usbh_uvc_setting_t cur_setting;                /* Current interface setting (Alt setting) */
	usbh_uvc_frame_t frame_buffer[USBH_UVC_VIDEO_FRAME_NUMS];/* Frame object pool (Metadata) */
	usb_os_task_t combine_task;                     /* Handle for the SW decoding thread */
	usbh_uvc_stream_control_t stream_ctrl;         /* UVC Probe/Commit control data */

	usbh_uvc_vs_t *vs_intf;                 /* Pointer to Video Streaming Interface */
	usbh_uvc_vs_format_t *def_format;       /* Default format descriptor */
	usbh_uvc_vs_format_t *cur_format;       /* Current active format descriptor */
	usbh_uvc_vs_frame_t *cur_frame;         /* Current active frame resolution descriptor */

	u32 frame_buffer_size;                  /* Size of one frame buffer */
	u8 *frame_buf;                          /* Raw memory block allocated for all frames */
	usb_os_sema_t frame_sema;               /* Semaphore to notify App: "Frame Ready" */

#if USBH_UVC_USE_HW
	usbh_hw_uvc_dec_t *uvc_dec;                    /* Handle for UVC hardware combiner */
#else
	/*
	* Data Structure Hierarchy for (USBH_UVC_USE_HW == 0)
	*
	* +-----------------------------------------------------------------------+
	* |                          Video Frame (Image)                          |
	* | (Composed of data extracted from multiple sequentially processed URBs)|
	* +-----------------------------------------------------------------------+
	*            ^                    ^                    ^
	*            | Append             | Append             | Append
	* +--------------------+  +--------------------+  +--------------------+
	* |       URB 1        |  |       URB 2        |  |   URB N (End)      |
	* | (Batch of Packets) |  | (Batch of Packets) |  | (Batch of Packets) |
	* +--------------------+  +--------------------+  +--------------------+
	*      ^     ^     ^
	*      |     |     |
	*   +-----+-----+-----+
	*   | Pkt | Pkt | Pkt | ... (USB Bus Data: Header + Payload)
	*   +-----+-----+-----+
	*
	* Summary:
	* 1. One ISOC IN transaction fill one packet.
	* 2. Multiple Packets fill one URB.
	* 3. Multiple URBs fill one Frame.
	* Frames form the stream, which is delivered to the App on a per-frame basis.
	*/
	struct list_head frame_empty;           /* List of free frames ready to be filled */
	struct list_head frame_chain;           /* List of completed frames ready for App */
	usb_os_lock_t frame_mutex;              /* Mutex protecting frame lists */
	usbh_uvc_frame_t *cur_frame_buf;        /* Pointer to the frame currently being assembled */

	usb_os_sema_t urb_ready_sema;           /* Queue: ready for give */
	usb_os_queue_t urb_wait_queue;          /* Queue: waiting for combine */
	usb_os_queue_t urb_giveback_queue;      /* Queue: reclaim after URB combined */
	usbh_uvc_urb_t *urb[USBH_UVC_URB_NUMS]; /* URB object pool */
	u8 *urb_buffer;                         /* Raw memory block for URB */

#if USBH_UVC_DEBUG
	u32 rx_frame_cnt;                       /* Counters of valid frames successfully pushed to App queue */
	u32 err_frame_cnt;                      /* Counters of frames dropped due to UVC payloadheader error bit */
	u32 drop_frame_cnt;                     /* Counters of ready frames forcibly discarded (App is too slow) */
	u32 dec_no_buf_cnt;                     /* Counters of drops due to no buffer available at combine start */
	u32 foi_no_buf_cnt;                     /* Counters of drops due to no buffer available at FID toggle */
	u32 eof_no_buf_cnt;                     /* Counters of drops due to no buffer available at EOF */
	u32 next_no_buf_cnt;                    /* Counters of total failed attempts to acquire a free buffer */
#endif

	u16 urb_buffer_size;                    // Size of one URB buffer
	u8 cur_urb;                             // Index of the URB currently being filled by combine_urb
	u8 cur_packet;                          // Index of the packet within the current URB
	u8 last_fid;                            // Last Frame ID (toggled bit in UVC payloadheader)
	__IO u8 complete_flag;                  // Flag for ISR Gate: 1=Allow pushing to queue, 0=Drop (protects queue access)
	__IO u8 complete_on;                    // Flag for Thread Loop: 1=Run, 0=Exit (controls combine task lifecycle)
	__IO u8 next_xfer;                      // Flag for next xfer: 0 (Stop), 1 (Start).
#endif// USBH_UVC_USE_HW == 0
	__IO u8 stream_state;                   // Stream state, @ref usbh_uvc_streaming_state_t
	__IO u8 state;                          // State for state machine, @ref usbh_stream_state_t
	__IO u8 get_valid;                      // Flag indicating get_frame is in use. Set to 1 when entering get_frame, cleared to 0 when exiting.
	__IO u8 get_exit;                       // Flag to signal get_frame to exit safely. Set to 1 before detaching to ensure get_frame has exited.
	u8 stream_idx;                          // Stream Index (0 or 1 for dual stream support)
	u8 set_alt;                             // Flag used in ctrl process machine: 0 (Unset), 1 (Set).
	u8 set_alt_retry;                       // Retry counter for SET_ALT (SET_INTERFACE) failures.
} usbh_uvc_stream_t;

typedef struct {
	usbh_uvc_stream_t stream[USBH_UVC_VS_DESC_MAX_NUM];
	usbh_uvc_cfg_t uvc_desc;
	usbh_setup_req_t setup_req;
	struct list_head entity_list;
	usb_host_t *host;
	const usbh_uvc_cb_t *cb;
	u8 *request_buf;

#if (USBH_UVC_USE_HW == 0) && USBH_UVC_DEBUG
	u32 sof_cnt;
	u32 isoc_rx_start_cnt;
	u32 isoc_rx_done_cnt;
	u32 isoc_rx_process_cnt;
	u32 isoc_com_start_rx;
	u32 isoc_sof_start_rx;
	u32 isoc_in_empty_cnt;
	u32 isoc_xfer_interval_cnt;
	u32 max_hold_frame_ts;
	/* SW Dump Task control */
	rtos_task_t sw_dump_task;
#endif

#if USBH_UVC_USE_HW
	/**
	 * @brief Called when the HW UVC engine reports an error condition.
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 * @param[in] err: HW UVC error status, see @ref usbh_hw_uvc_err_status_t.
	 */
	void (*hw_error)(usbh_hw_uvc_err_status_t err);
	usbh_uvc_err_t hw_stats;
	usbh_uvc_err_t hw_stats_shadow;
#if USBH_UVC_DEBUG
	/* HW Dump Task control */
	rtos_task_t hw_dump_task;
	usb_os_sema_t hw_dump_sema;
	__IO u8 hw_dump_task_alive;
	__IO u8 hw_dump_task_exit;
#endif
	u8 hw_isr_pri;
	__IO u8 hw_irq_ref_cnt;
#endif

#if (USBH_UVC_USE_HW == 0) && USBH_UVC_DEBUG
	__IO u8 sw_dump_task_alive;
	__IO u8 sw_dump_task_exit;
#endif

	__IO u8 state; // @ref usbh_uvc_state_t
	__IO u8 stream_ctrl_idx; // record stream idx for ctrl process
	u8 err_retry_cnt; // Retry counter for clear_feature in UVC_STATE_ERROR
} usbh_uvc_host_t;

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/*class module*/
int usbh_uvc_class_init(void);
void usbh_uvc_class_deinit(void);

/*stream module*/
int usbh_uvc_set_video(usbh_uvc_stream_t *stream, int probe);
int usbh_uvc_get_video(usbh_uvc_stream_t *stream, int probe, u16 request);
int usbh_uvc_video_init(usbh_uvc_stream_t *stream);
int usbh_uvc_stream_init(usbh_uvc_stream_t *stream);
void usbh_uvc_stream_deinit(usbh_uvc_stream_t *stream);
int usbh_uvc_parse_cfgdesc(usb_host_t *host);
int usbh_uvc_stream_stop(usbh_uvc_stream_t *stream);
int usbh_uvc_desc_init(void);
void usbh_uvc_desc_deinit(void);
u16 usbh_uvc_get_ctrl_len_by_version(u16 bcdUVC);

#if (USBH_UVC_USE_HW == 0)
void usbh_uvc_process_sof(usb_host_t *host);
int usbh_uvc_process_completed(usb_host_t *host, u8 pipe_num);
usbh_uvc_urb_t *usbh_uvc_urb_complete(usbh_uvc_stream_t *stream, usbh_uvc_urb_t *urb);
#if USBH_UVC_DEBUG
void usbh_uvc_sw_status_dump_thread(void *param);
#endif
#endif

#if USBH_UVC_USE_HW && USBH_UVC_DEBUG
void usbh_uvc_hw_status_dump_thread(void *param);
#endif
#endif
