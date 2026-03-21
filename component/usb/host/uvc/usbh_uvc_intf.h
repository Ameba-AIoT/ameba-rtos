/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_UVC_INTF_H
#define USBH_UVC_INTF_H

/* Includes ------------------------------------------------------------------*/

#include "dlist.h"

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
#define USBH_UVC_USE_HW                               1      /**< Enable usage of UVC hardware decoder. */
#else
#define USBH_UVC_USE_HW                               0      /**< Disable usage of UVC hardware decoder. */
#endif

#if USBH_UVC_USE_HW
#include "usbh_hw_uvc.h"
#endif

#define USBH_UVC_GET_FRAME_TIMEOUT                    2000   /**< Timeout for getting a frame in ms. */

/* Supported Video Formats */
#define USBH_UVC_FORMAT_MJPEG                         0x6    /**< Motion-JPEG format. */
#define USBH_UVC_FORMAT_YUV                           0x4    /**< Uncompressed YUV format. */
#define USBH_UVC_FORMAT_H264                          0x10   /**< H.264 format. */

/* USB Request Block (URB) Configuration */
#define USBH_UVC_URB_NUMS                             4      /**< Number of URBs used for streaming. */
#define USBH_UVC_URB_SIZE                             ((3072 + 32) * 8)   /**< Size of each URB buffer in bytes. */

#define USBH_UVC_VIDEO_FRAME_NUMS                     3      /**< Number of video frame buffers. Fixed to 3 if using HW decoder. */

/* Task Configuration */
#define USBH_UVC_COMBINE_TASK_STACK                    (512 * 4)   /**< Stack size for the combine task in bytes. */
#define USBH_UVC_COMBINE_TASK_PRIORITY                 5           /**< Priority of the combine task. */

#define UBSH_UVC_REQUEST_BUF_LEN                       64     /**< Length of the UVC control request buffer. */

/* Debug / Error Handling */
#define USBH_UVC_DEBUG                                 0

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
	STREAMING_OFF = 0,    /**< Streaming is currently stopped. */
	STREAMING_ON          /**< Streaming is active. */
} usbh_uvc_streaming_state_t;

/**
 * @brief UVC Stream Data Transfer State.
 */
typedef enum {
	STREAM_STATE_IDLE = 1, /**< Idle state, no data transfer. */
	STREAM_DATA_IN,        /**< Data IN transfer state. */
} usbh_uvc_stream_data_state_t;

/**
 * @brief UVC Frame Buffer State.
 */
typedef enum  {
	UVC_FRAME_INIT = 0,    /**< Frame buffer initialized / empty. */
	UVC_FRAME_FLYING,      /**< Frame is being filled (memcpying/receiving). */
	UVC_FRAME_READY,       /**< Frame is complete and ready to commit to application. */
	UVC_FRAME_INUSE        /**< Frame is currently held by the application. */
} usbh_uvc_frame_state_t;

/**
 * @brief UVC Stream Context / Parameters.
 */
typedef struct  {
	int fmt_type;          /**< Video format type (e.g., USBH_UVC_FORMAT_MJPEG). */
	int width;             /**< Video frame width in pixels. */
	int height;            /**< Video frame height in pixels. */
	int frame_rate;        /**< Video frame rate (fps). */
	u32 frame_buf_size;    /**< Size of a single video frame buffer in bytes. */
} usbh_uvc_s_ctx_t;

/**
 * @brief UVC Global Context.
 */
typedef struct  {
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
	 * @brief Callback invoked when parameters need to be set/negotiated.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* setparam)(void);
} usbh_uvc_cb_t;

/**
 * @brief UVC Video Frame Structure.
 *        Represents a single video frame node in the queue.
 */
typedef struct {
	struct list_head list;      /**< Linked list node for queue management. */
	u8 *buf;                    /**< Pointer to the data buffer containing the frame. */
	u32 index;                  /**< Index of the frame in the pool. */
	u32 byteused;               /**< Actual number of bytes used in the buffer (frame size). */
	u32 err;                    /**< Error flags associated with this frame. */
	u32 timestamp;              /**< Presentation timestamp of the frame. */
	usbh_uvc_frame_state_t state; /**< Current state of this frame buffer. */
#if USBH_UVC_DEBUG
	u32 get_frame_ts;           /**< Timestamp recorded when consumer gets this frame. */
	u32 submit_frame_ts;        /**< Timestamp recorded when driver submits this frame. */
#endif
} usbh_uvc_frame_t;

/** @} End of Host_UVC_Types group */
/** @} End of USB_Host_Types group */

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

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
 *         Registers the application callback handler and global context.
 * @param[in] cfg: Pointer to the global UVC context configuration.
 * @param[in] cb: Pointer to the user-defined callback structure.
 * @return 0 on success, non-zero on failure.
 */
int usbh_uvc_init(usbh_uvc_ctx_t *cfg, usbh_uvc_cb_t *cb);

/**
 * @brief  De-initializes the UVC host class driver and releases resources.
 */
void usbh_uvc_deinit(void);

/**
 * @brief  Starts video streaming on the specified interface.
 * @param[in] para: Pointer to the stream parameters (resolution, format, etc.).
 * @param[in] itf_num: Interface number to start streaming on.
 * @return 0 on success, non-zero on failure.
 */
int usbh_uvc_stream_on(usbh_uvc_s_ctx_t *para, u32 itf_num);

/**
 * @brief  Stops video streaming on the specified interface.
 * @param[in] itf_num: Interface number to stop streaming on.
 * @return 0 on success, non-zero on failure.
 */
int usbh_uvc_stream_off(u32 itf_num);

/**
 * @brief  Gets the current streaming state of an interface.
 * @param[in] itf_num: Interface number to check.
 * @return Current streaming state (e.g., STREAMING_ON or STREAMING_OFF).
 */
int usbh_uvc_stream_state(u32 itf_num);

/**
 * @brief  Sets video streaming parameters for the interface.
 *         Performs negotiation (PROBE/COMMIT) with the device.
 * @param[in] para: Pointer to the desired stream parameters.
 * @param[in] itf_num: Interface number.
 * @return 0 on success, non-zero on failure.
 */
int usbh_uvc_set_param(usbh_uvc_s_ctx_t *para, u32 itf_num);

/**
 * @brief  Retrieves a ready video frame from the driver's output queue.
 * @param[in] itf_num: Interface number.
 * @return Pointer to a valid `usbh_uvc_frame_t` if available, otherwise NULL.
 */
usbh_uvc_frame_t *usbh_uvc_get_frame(u32 itf_num);

/**
 * @brief  Returns a processed frame back to the driver's free pool.
 *         Must be called after the application finishes using the frame.
 * @param[in] frame: Pointer to the frame to be released.
 * @param[in] itf_num: Interface number.
 */
void usbh_uvc_put_frame(usbh_uvc_frame_t *frame, u32 itf_num);
#ifdef __cplusplus
}
#endif

/** @} End of Host_UVC_Functions group */
/** @} End of USB_Host_Functions group */
/** @} End of USB_Host_API group */

#endif /* USBH_UVC_INTF_H */
