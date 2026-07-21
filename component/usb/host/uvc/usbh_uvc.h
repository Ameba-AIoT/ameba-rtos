/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_UVC_H
#define USBH_UVC_H

/* Includes ------------------------------------------------------------------*/

#include "usbh.h"
#include "dlist.h"
#include "usbh_uvc_desc.h"

/* Exported defines ----------------------------------------------------------*/

/** @addtogroup USB_Host_API USB Host API
 *  @{
 */
/** @addtogroup USB_Host_Constants USB Host Constants
 * @{
 */
/** @addtogroup Host_UVC_Constants Host UVC Constants
 * @{
 */

#ifdef CONFIG_SUPPORT_USBH_UVC_HW_DEC
#define USBH_UVC_USE_HW                               1U      /**< Enable usage of UVC hardware decoder. */
#else
#define USBH_UVC_USE_HW                               0U      /**< Disable usage of UVC hardware decoder. */
#endif

#if USBH_UVC_USE_HW
#include "usbh_hw_uvc.h"
#endif

/* Video format selectors for usbh_uvc_s_ctx_t.fmt_type.
 * MJPEG/YUV/H264 values coincide with the UVC VS Format bDescriptorSubtype.
 * H.264 and H.265 both arrive as frame-based (subtype 0x10) and are told
 * apart by the driver via guidFormat, so H265 uses a driver-private selector
 * value (there is no dedicated UVC subtype for it). */
#define USBH_UVC_FORMAT_MJPEG                         0x6U    /**< Motion-JPEG format. */
#define USBH_UVC_FORMAT_YUV                           0x4U    /**< Uncompressed YUV format. */
#define USBH_UVC_FORMAT_H264                          0x10U   /**< H.264 (frame-based) format. */
#define USBH_UVC_FORMAT_H265                          0x30U   /**< H.265/HEVC (frame-based) format (driver-private selector). */

/* Configuration */
/*
 * Number of VideoStreaming (VS) streams the driver instantiates. This macro
 * bounds two things at once:
 *   1. How many streams usbh_uvc_init()/usbh_uvc_deinit() allocate/release.
 *      init/deinit take NO stream_index: they prepare/free ALL streams in one call.
 *   2. The valid range of the stream_index argument on the data-flow APIs
 *      (set_param/start/get_frame/put_frame/stop): 0 .. USBH_UVC_VS_DESC_MAX_NUM-1.
 * Single-stream camera: keep this at 1 -> the only valid stream_index is 0.
 * Dual-stream  camera : set this to 2 -> stream_index may be 0 or 1.
 * Maximum supported value is 2.
 */
#define USBH_UVC_VS_DESC_MAX_NUM                      1U

/* Maximum number of VideoStreaming alternate settings supported.
 * The actual number of alternate settings depends on the capabilities
 * of the connected UVC camera.
 */
#define USBH_UVC_VS_ALTS_MAX_NUM                      12U

/* USB Vedio Frame Configuration */
#define USBH_UVC_GET_FRAME_TIMEOUT                    1000U   /**< Timeout for getting a frame in ms. */
#define USBH_UVC_VIDEO_FRAME_NUMS                     3U      /**< Number of video frame buffers. Fixed to 3 if using HW decoder. */

/* Max retries for SET_INTERFACE(intf, alt) when STATUS IN is NAKed. */
#define USBH_UVC_SET_ALT_RETRY_MAX                    15U      /**< Max retries for SET_ALT (SET_INTERFACE) failures. */

/* Max retries for clear_feature in UVC_STATE_ERROR before forcing IDLE. */
#define USBH_UVC_ERROR_CLEAR_RETRY_MAX                3U      /**< Max retries for clear_feature before forcing IDLE. */

#if (USBH_UVC_USE_HW == 0)
/* USB Request Block (URB) Configuration
 * SW path uses a lock-free SPSC ring of single-packet URBs: each URB slot holds
 * exactly one mps (cur_setting.pipe.xfer_len) isoc packet. Depth = USBH_UVC_URB_NUMS
 * (usable depth is NUMS-1; one slot is always the in-flight tail). The ISR (producer)
 * gives urb_ready_sema only once the ready-count reaches USBH_UVC_URB_WATERMARK; the
 * combine thread (consumer) then batch-drains the whole ring. Both are tunable. */
#define USBH_UVC_URB_NUMS                             12U     /**< Ring depth: number of single-packet URB slots (usable = NUMS-1). */
#define USBH_UVC_URB_WATERMARK                        6U      /**< ISR wakes combine thread when ready URB count >= this. Must be < USBH_UVC_URB_NUMS. */
#define USBH_UVC_URB_SIZE                             3072U   /**< Legacy: not used by the SPSC single-packet ring. */
#define USBH_UVC_URB_PACKET_NUM                       1U      /**< Payload packets per URB slot; also the packet_info[] entry count. */

/* Task Configuration */
#define USBH_UVC_COMBINE_TASK_STACK                    768U   /**< Stack size for the combine task in bytes. */
#define USBH_UVC_COMBINE_TASK_PRIORITY                 5U      /**< Priority of the combine task. */
#endif

/* Debug / Error Handling */
#define USBH_UVC_DEBUG                                 0U

/** @} End of Host_UVC_Constants group */
/** @} End of USB_Host_Constants group */

/* Exported types ------------------------------------------------------------*/

/** @addtogroup USB_Host_Types USB Host Types
 * @{
 */
/** @addtogroup Host_UVC_Types Host UVC Types
 * @{
 */

/**
 * @brief UVC Streaming State.
 */
typedef enum {
	UVC_STREAM_OFF = 0U,   /**< No streaming resources: before stream_on / after stream_off. */
	UVC_STREAM_READY,     /**< Resources ready, idle: after stream_on / after stream_stop. */
	UVC_STREAM_ACTIVE,    /**< Actively transferring: after stream_start. */
} usbh_uvc_streaming_state_t;

/**
 * @brief UVC Frame Buffer State.
 */
typedef enum  {
	UVC_FRAME_INIT = 0U,    /**< Frame buffer initialized / empty. */
	UVC_FRAME_FLYING,      /**< Frame is being filled (memcpying/receiving). */
	UVC_FRAME_READY,       /**< Frame is complete and ready to commit to application. */
	UVC_FRAME_INUSE        /**< Frame is currently held by the application. */
} usbh_uvc_frame_state_t;

/**
 * @brief UVC Stream Context / Parameters.
 */
typedef struct  {
	u32 frame_buf_size;    /**< Size of a single video frame buffer in bytes. */
	u16 width;             /**< Video frame width in pixels. */
	u16 height;            /**< Video frame height in pixels. */
	u8 frame_rate;        /**< Video frame rate (fps). */
	u8 fmt_type;          /**< Video format type (e.g., USBH_UVC_FORMAT_MJPEG). */
} usbh_uvc_s_ctx_t;

/**
 * @brief UVC Global Context.
 */
typedef struct  {
	u32 frame_buf_size;    /**< Size of a single video frame buffer in bytes (applied to every stream at init). */
#if USBH_UVC_USE_HW
	u8 hw_isr_pri;         /**< UVC hardware ISR priority. */
#endif
} usbh_uvc_ctx_t;

/**
 * @brief UVC Host User Callback Interface.
 */
typedef struct {
	/**
	 * @brief Callback invoked during UVC driver initialization.
	 *        Used for application-specific resource setup.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* init)(void);

	/**
	 * @brief Callback invoked during UVC driver de-initialization.
	 *        Used for application-specific resource cleanup.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* deinit)(void);

	/**
	 * @brief Callback invoked when a UVC device is attached.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* attach)(void);

	/**
	 * @brief Callback invoked when a UVC device is detached.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* detach)(void);

	/**
	 * @brief Callback invoked when the device setup phase is complete.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* setup)(void);

	/**
	 * @brief Callback invoked when the UVC parameter-setting sequence
	 *        (Probe/Commit/SET_INTERFACE) completes; status is HAL_OK
	 *        on success or HAL_ERR_HW on failure.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* set_param)(int status);  // status: HAL_OK on success, HAL_ERR_HW on failure
} usbh_uvc_cb_t;

/**
 * @brief UVC Video Frame Structure.
 *        Represents a single video frame node in the queue.
 */
typedef struct {
	struct list_head list;      /**< Linked list node for queue management. */
	u8 *buf;                    /**< Pointer to the data buffer containing the frame. */
	u32 byteused;               /**< Actual number of bytes used in the buffer (frame size). */
	u32 timestamp;              /**< Presentation timestamp of the frame. */
#if USBH_UVC_DEBUG
	u32 get_frame_ts;           /**< Timestamp recorded when consumer gets this frame. */
	u32 submit_frame_ts;        /**< Timestamp recorded when driver submits this frame. */
#endif
	u8 err;                    /**< Error flags associated with this frame. */
	u8 state;             /**< Current state of this frame buffer, @ref usbh_uvc_frame_state_t. */
} usbh_uvc_frame_t;

/** @} End of Host_UVC_Types group */
/** @} End of USB_Host_Types group */

/*
 * The types below are internal to the UVC host driver (shared across the
 * class / parse / stream modules). They are not part of the application-facing
 * API but live in this single public header so it stays the one include every
 * UVC translation unit needs, mirroring the sibling class convention.
 */

/**
 * @brief UVC host controller state (state machine driver, @ref usbh_uvc_host_t).
 */
typedef enum {
	UVC_STATE_IDLE = 0,
	UVC_STATE_CTRL,
	UVC_STATE_TRANSFER,
	UVC_STATE_STOP,
	UVC_STATE_ERROR
} usbh_uvc_state_t;

/**
 * @brief UVC per-stream control state machine (Probe/Commit/SET_INTERFACE).
 */
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

/* --- Descriptor-parse module types (usbh_uvc_parse.c) --- */

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
	u32 frame_num;
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
	u32 format_num;
	void *p;
	u8 bInterfaceNumber;
	u8 alt_num;
} usbh_uvc_vs_t;

typedef struct {
	usbh_uvc_vs_t vs_intf[USBH_UVC_VS_DESC_MAX_NUM];
	usbh_uvc_vc_t vc_intf;
	u8 vs_num;
} usbh_uvc_cfg_t;

/* --- Streaming module types (usbh_uvc_stream.c) --- */

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
	usbh_uvc_packet_desc_t packet_info[USBH_UVC_URB_PACKET_NUM];
	u8 *addr;
#if USBH_UVC_DEBUG
	u32 get_us;
	u32 submit_us;
	u32 process_us;
	u32 regive_us;
	__IO u8 owner;
	__IO u8 valid_data;
#endif
	u8 index;
	u8 packet_num;
	u8 packet_length;
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
	u32 reuse_cnt;
#endif

	u16 urb_buffer_size;                    // Size of one URB buffer
	u8 cur_urb;                             // Index of the URB currently being filled by combine_urb
	u8 cur_packet;                          // Index of the packet within the current URB
	u8 last_fid;                            // Last Frame ID (toggled bit in UVC payloadheader)
	__IO u8 urb_head;                      // SPSC ring READ cursor (combine thread owns): next filled slot to consume
	__IO u8 complete_flag;                  // Flag for ISR Gate: 1=Allow pushing to queue, 0=Drop (protects queue access)
	__IO u8 complete_on;                    // Flag for Thread Loop: 1=Run, 0=Exit (controls combine task lifecycle)
	__IO u8 is_resource_safe;               // Flag for uvc resource: 0 (UnSafe), 1 (Safe).
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

/**
 * @brief UVC Host driver context (internal root object).
 */
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
	u32 max_hold_frame_ms;
	/* SW Dump Task control */
	rtos_task_t sw_dump_task;
	u32 max_combine_cost_us;                // Max combine_urb() cost per URB (us)
	u32 max_memcpy_cost_us;                 // Max single payload memcpy cost inside combine_urb (us)
	u32 max_publish_cost_us;                // Max next_frame_buffer (publish + rotate) cost (us)
	u32 max_batch_drain_us;                 // Max time to drain the whole ready ring in one wake (us)
	u32 max_batch_cnt;                      // Max URBs drained in one wake (for watermark tuning) (us)
#endif

#if USBH_UVC_USE_HW
	/**
	 * @brief Called when the HW UVC engine reports an error condition.
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 * @param[in] err: Bitmask of HW UVC error flags (USBH_HW_UVC_CHx_BUFx_OVERSIZE / _HEADER).
	 */
	void (*hw_error)(u32 err);
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

/* Exported functions --------------------------------------------------------*/

/** @addtogroup USB_Host_Functions USB Host Functions
 * @{
 */
/** @addtogroup Host_UVC_Functions Host UVC Functions
 * @{
 */
#ifdef __cplusplus
extern "C"
{
#endif
/**
 * @brief  Initializes the UVC host class driver.
 *         Registers the application callback handler and global context, and
 *         allocates all per-stream runtime resources (frame buffers, frame lists,
 *         semaphores, mutexes and worker threads / HW channels) for every stream,
 *         moving them to the READY state. The per-stream frame buffer size is taken
 *         from cfg->frame_buf_size. This does NOT begin data transfer.
 * @note   The public API has a single data-flow axis:
 *           usbh_uvc_start() <-> usbh_uvc_stop()
 *         Lifecycle order: usbh_uvc_init() (resources ready here) ->
 *         usbh_uvc_set_param() -> usbh_uvc_start() (transfer begins here) ->
 *         usbh_uvc_get_frame() / usbh_uvc_put_frame() -> usbh_uvc_stop().
 *         Resources are released by usbh_uvc_deinit().
 * @note   usbh_uvc_init()/usbh_uvc_deinit() take no stream_index: they prepare/release
 *         ALL streams at once (count = USBH_UVC_VS_DESC_MAX_NUM). A specific stream
 *         is then selected by the stream_index argument on the data-flow APIs. For a
 *         single-stream camera (USBH_UVC_VS_DESC_MAX_NUM = 1) stream_index must be 0;
 *         a dual-stream camera (= 2) accepts stream_index 0 or 1.
 * @param[in] cfg: Pointer to the global UVC context configuration (incl. frame_buf_size).
 * @param[in] cb: Pointer to the user-defined callback structure.
 * @return 0 on success, non-zero on failure.
 */
int usbh_uvc_init(const usbh_uvc_ctx_t *cfg, const usbh_uvc_cb_t *cb);

/**
 * @brief  Sets video streaming parameters for the interface.
 *         Performs negotiation (PROBE/COMMIT) with the device.
 * @param[in] para: Pointer to the desired stream parameters.
 * @param[in] stream_index: VideoStreaming stream index, range 0 .. USBH_UVC_VS_DESC_MAX_NUM-1
 *                     (must be 0 for a single-stream camera).
 * @return 0 on success, non-zero on failure.
 */
int usbh_uvc_set_param(usbh_uvc_s_ctx_t *para, u8 stream_index);

/**
 * @brief  Start video streaming and kick off isochronous data transfer.
 *         Data-flow-axis entry; the stream resources are ready after usbh_uvc_init()
 *         and must be negotiated (usbh_uvc_set_param) first. Pair with usbh_uvc_stop().
 * @param[in] stream_index: VideoStreaming stream index, range 0 .. USBH_UVC_VS_DESC_MAX_NUM-1
 *                     (must be 0 for a single-stream camera).
 * @return 0 on success, non-zero on failure.
 */
int usbh_uvc_start(u8 stream_index);

/**
 * @brief  Retrieves a ready video frame from the driver's output queue.
 * @param[in] stream_index: VideoStreaming stream index, range 0 .. USBH_UVC_VS_DESC_MAX_NUM-1
 *                     (must be 0 for a single-stream camera).
 * @return Pointer to a valid `usbh_uvc_frame_t` if available, otherwise NULL.
 */
usbh_uvc_frame_t *usbh_uvc_get_frame(u8 stream_index);

/**
 * @brief  Returns a processed frame back to the driver's free pool.
 *         Must be called after the application finishes using the frame.
 * @param[in] frame    Pointer to the frame to be released. Must be obtained via usbh_uvc_get_frame().
 * @param[in] stream_index  VideoStreaming stream index, range 0 .. USBH_UVC_VS_DESC_MAX_NUM-1
 *                     (must be 0 for a single-stream camera).
 * @return 0 on success; negative value on failure.
 */
int usbh_uvc_put_frame(usbh_uvc_frame_t *frame, u8 stream_index);

/**
 * @brief  Stop video streaming and return to the ready state (resources retained).
 *         Data-flow-axis exit; the stream stays ready and can be re-started with
 *         usbh_uvc_start() without re-init. Pair with usbh_uvc_start().
 * @param[in] stream_index: VideoStreaming stream index, range 0 .. USBH_UVC_VS_DESC_MAX_NUM-1
 *                     (must be 0 for a single-stream camera).
 * @return 0 on success, non-zero on failure.
 */
int usbh_uvc_stop(u8 stream_index);

/**
 * @brief  De-initializes the UVC host class driver and releases resources.
 *         Releases everything allocated by usbh_uvc_init() (frame buffers, frame
 *         lists, semaphores, mutexes and worker threads / HW channels) for every
 *         stream and wakes up any thread blocked in usbh_uvc_get_frame().
 */
void usbh_uvc_deinit(void);

#if USBH_UVC_DEBUG
/**
 * @brief  Dumps UVC device information for debugging purposes.
 *         Prints descriptors, interfaces, supported formats/frames, and current stream settings to the log.
 * @return None.
 */
void usbh_uvc_dump_dev_info(void);

#if (USBH_UVC_USE_HW == 0)
/**
 * @brief  Clear class-layer frame and drop statistics via RTK_LOGS.
 *         Call from the application at the start of each measurement round.
 * @param[in] stream_index: VideoStreaming stream index, range 0 .. USBH_UVC_VS_DESC_MAX_NUM-1
 *                     (must be 0 for a single-stream camera).
 */
void usbh_uvc_clear_stats(u8 stream_index);

/**
 * @brief  Print class-layer frame and drop statistics via RTK_LOGS.
 *         Call from the application at the end of each measurement round.
 * @param[in] stream_index: VideoStreaming stream index, range 0 .. USBH_UVC_VS_DESC_MAX_NUM-1
 *                     (must be 0 for a single-stream camera).
 */
void usbh_uvc_print_stats(u8 stream_index);
#endif
#endif
#ifdef __cplusplus
}
#endif

/** @} End of Host_UVC_Functions group */
/** @} End of USB_Host_Functions group */
/** @} End of USB_Host_API group */

#endif /* USBH_UVC_H */
