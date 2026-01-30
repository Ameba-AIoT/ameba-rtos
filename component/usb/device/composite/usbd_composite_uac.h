/*
 * Copyright (c) 2025 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBD_COMPOSITE_UAC_H
#define USBD_COMPOSITE_UAC_H

/* Includes ------------------------------------------------------------------*/

#include "usbd.h"
#include "usbd_composite_config.h"

/* Exported defines ----------------------------------------------------------*/

/** @addtogroup USB_Device_API USB Device API
 *  @{
 */
/** @addtogroup USB_Device_Constants USB Device Constants
 * @{
 */
/** @addtogroup Device_Composite_UAC_Constants Device Composite UAC Constants
 * @{
 */
#define USBD_COMPOSITE_UAC_DEBUG                    0           /**< Enable/disable UAC debug features. */

#if USBD_COMPOSITE_UAC_DEBUG && (USBD_TP_TRACE_DEBUG == 0)
#error "Please set USBD_TP_TRACE_DEBUG in usbd.h"
#endif

#define USBD_UAC_VID                                USB_VID  /**< Vendor ID. */
#define USBD_UAC_PID                                USB_PID  /**< Product ID. */

/* Channel cnt */
#define USBD_UAC_CH_CNT_2                           2U /**< 2 Channels. */
#define USBD_UAC_CH_CNT_4                           4U /**< 4 Channels. */
#define USBD_UAC_CH_CNT_6                           6U /**< 6 Channels. */
#define USBD_UAC_CH_CNT_8                           8U /**< 8 Channels. */
//#define USBD_UAC_CH_CNT_16                        16U  /* to do: 16 ch support */
#define USBD_UAC_DEFAULT_CH_CNT                     USBD_UAC_CH_CNT_8 /**< Default channel count. */

/* Byte/Bit Widths */
#define USBD_UAC_BYTE_WIDTH_2                       2U /**< 2 bytes per audio sample. */
#define USBD_UAC_BYTE_WIDTH_3                       3U /**< 3 bytes per audio sample. */
#define USBD_UAC_BYTE_WIDTH_4                       4U /**< 4 bytes per audio sample. Amebasmart itself cannot play 32-bit audio data */
#define USBD_UAC_DEFAULT_BYTE_WIDTH                 USBD_UAC_BYTE_WIDTH_2  /**< Default byte width. */
#define USBD_UAC_BIT_WIDTH(byte_width)              (8U * (byte_width))    /**< Calculate bit width from byte width. */

/* Sampling Frequencies */
#define USBD_UAC_SAMPLING_FREQ_MAX_COUNT            1U      /**< Max count of supported sampling frequencies. */
#define USBD_UAC_SAMPLING_FREQ_44K                  44100U  /**< 44.1 kHz sampling frequency. */
#define USBD_UAC_SAMPLING_FREQ_48K                  48000U  /**< 48 kHz sampling frequency. */
#define USBD_UAC_SAMPLING_FREQ_96K                  96000U  /**< 96 kHz sampling frequency. */
#define USBD_UAC_SAMPLING_FREQ_192K                 192000U /**< 192 kHz sampling frequency. */

/*
* while the host does not send the audio data(for example: switch to next song), the usb should append zero packet to the buffer
*
*/
/**
 * @brief Maximum number of SOF frames to wait before appending a zero-length packet.
 * @details When the host stops sending audio data (e.g., switching songs), the device
 *          appends a zero-length packet to the buffer to ensure smooth transitions.
 */
#define USBD_UAC_ISOC_RX_APPEND_ZLP_DIFF_MAX        3



/** @} End of Device_Composite_UAC_Constants group*/
/** @} End of USB_Device_Constants group*/

/* Exported types ------------------------------------------------------------*/
/** @addtogroup USB_Device_Types USB Device Types
 * @{
 */
/** @addtogroup Device_Composite_UAC_Types Device Composite UAC Types
 * @{
 */
/* Audio configuration parameters: application uses this structure to configure audio parameters to the UAC driver. */
typedef struct {
	u32 sampling_freq;         /**< Audio sample frequency param. */
	u8 byte_width;             /**< Audio byte width param. */
	u8 ch_cnt;                 /**< Audio channel count param. */
	u8 enable;                 /**< Audio enable. */
} usbd_audio_cfg_t;

typedef struct {
	usbd_audio_cfg_t in;       /**< Audio configuration parameters for record. */
	usbd_audio_cfg_t out;      /**< Audio configuration parameters for playback. */
	void *audio_ctx;           /**< Audio context params. */
	/**
	 * @brief Called when the UAC class driver initialization for application resource setup.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* init)(void);

	/**
	 * @brief Called when the UAC device is de-initialized for resource cleanup.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* deinit)(void);

	/**
	 * @brief Called during control transfer SETUP/DATA phases to handle class-specific SETUP requests.
	 * @param[in] req: Pointer to the setup request packet.
	 * @param[out] buf: Pointer to a buffer for data stage of control transfers.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* setup)(usb_setup_req_t *req, u8 *buf);

	/**
	 * @brief Notifies application layer when UAC driver becomes operational.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* set_config)(void);

	/**
	 * @brief Called when USB attach status changes for application to support hot-plug events.
	 * @param[in] old_status: The previous attach status.
	 * @param[in] status: The new attach status.
	 */
	int(* status_changed)(u8 old_status, u8 status);

	/**
	 * @brief Handles mute setting updates from USB host.
	 * @param[in] mute: Mute value, 0 unmute, 1 mute
	 */
	int(* mute_changed)(u8 mute);

	/**
	 * @brief Adjusts playback volume according to host-side volume changes.
	 * @param[in] volume: Volume value, from 0~100
	 */
	int(* volume_changed)(u8 volume);

	/**
	 * @brief Called when the audio parameters(sample rate/channels) are modified by host.
	 * @param[in] sampling_freq: New sample frequency.
	 * @param[in] ch_cnt: New channel count. such as 2,4,6,8...
	 * @param[in] byte_width: New byte width, such as 1,2,3,4.
	 */
	int(* format_changed)(u32 sampling_freq, u8 ch_cnt, u8 byte_width);

	/**
	 * @brief Called upon SOF interrupt for clock synchronization.
	 */
	int(* sof)(void);
} usbd_composite_uac_usr_cb_t;
/** @} End of Device_Composite_UAC_Types group*/
/** @} End of USB_Device_Types group*/

struct _usbd_composite_uac_buf_t;
typedef struct _usbd_composite_uac_buf_t {
	struct _usbd_composite_uac_buf_t *next; /**< Next buf node. */
	u8 *buf_raw;                            /**< Pointer to the buffer. */
	__IO u16 buf_valid_len;                 /**< Valid buffer length. */
	__IO u8 is_zero_pkt;                    /**< The buf is zero packet */
} usbd_composite_uac_buf_t;

typedef struct {
	usbd_composite_uac_buf_t *head;
	usbd_composite_uac_buf_t *tail;
	u16 count;
} usbd_composite_uac_buf_list_t;

typedef struct {
	usbd_audio_cfg_t audio_config;  /* Audio config */
	rtos_sema_t uac_isoc_sema;      /**< ISOC sema. */
	usbd_composite_uac_buf_list_t empty_list;
	usbd_composite_uac_buf_list_t data_list;

	usbd_composite_uac_buf_t *buf_list_node;
	usbd_composite_uac_buf_t *p_cur_buf_node;
	u8 *isoc_buf;                       /**< Pointer to audio total buffer. */

	u64 sof_idx;
	u64 data_idx;

	__IO u16 isoc_mps;            /**< ISOC maximum packet size. */
	__IO u8 uac_sema_valid : 1;   /**< ISOC sema created. */
	__IO u8 read_wait_sema : 1;   /**< The sema is waiting. */
	__IO u8 next_xfer : 1;        /**< Audio transfer continue flag. */
} usbd_composite_uac_buf_ctrl_t;

typedef struct {
	usbd_composite_uac_buf_ctrl_t uac_isoc_out;  /**< ISOC OUT control structure. */
	usbd_composite_uac_buf_ctrl_t uac_isoc_in;   /**< ISOC IN control structure. */
	usbd_ep_t ep_isoc_in;                        /**< ISOC IN endpoint structure. */
	usbd_ep_t ep_isoc_out;                       /**< ISOC OUT endpoint structure. */
	usb_setup_req_t ctrl_req;                    /**< Stores the current control request. */
	usbd_composite_dev_t *cdev;                  /**< Pointer to the USB composite device instance. */
	usbd_composite_uac_usr_cb_t *cb;             /**< Pointer to the USB composite device user-defined callback structure. */

	__IO u32 isoc_rx_valid_cnt;
	__IO u32 isoc_rx_len;

#if USBD_COMPOSITE_UAC_DEBUG
	rtos_task_t uac_dump_task;
	u32 isoc_rx_last_tick;
	u32 isoc_timeout_max_step;
	u32 isoc_timeout_max_value;

	u32 isoc_zlp_cnt;
	u32 isoc_rx_cnt;
	u32 isoc_rx_zlp_cnt;
	u32 copy_data_len;
	__IO u32 isoc_timeout_cnt;
	__IO u32 isoc_overwrite_cnt;
	__IO u32 uac_dump_task_alive;
#endif

	u16 cur_volume;        /**< Current Audio volume . */
	u8 cur_clk_valid;      /**< Current Clock valid flag. */
	u8 alt_setting;        /**< Current choose alternate setting. */
	u8 cur_mute;           /**< Current Audio mute value. */
} usbd_composite_uac_device_t;

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

extern const usbd_class_driver_t usbd_composite_uac_driver;

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Initializes the UAC composite function.
 * @param[in] cdev: Pointer to the composite device context.
 * @param[in] cb: Pointer to the user callback structure passed by the upper layer.
 * @return 0 on success, non-zero on failure.
 */
int usbd_composite_uac_init(usbd_composite_dev_t *cdev, usbd_composite_uac_usr_cb_t *cb);

/**
 * @brief De-initializes the UAC composite function.
 * @return 0 on success, non-zero on failure.
 */
int usbd_composite_uac_deinit(void);

/** @addtogroup USB_Device_Functions USB Device Functions
 * @{
 */
/** @addtogroup Device_Composite_UAC_Functions Device Composite UAC Functions
 * @{
 */
/**
 * @brief Sets the audio configuration for a UAC class.
 * @param[in] uac_cfg: Pointer to the audio configuration to apply.
 * @param[in] is_record: 1 for the record (IN) stream, 0 for the play (OUT) stream.
 * @param[in] flag: (Unused) A flag for future extensions.
 * @return 0 on success, non-zero on failure.
 */
u8 usbd_composite_uac_config(const usbd_audio_cfg_t *uac_cfg, u8 is_record, u32 flag);

/**
 * @brief Starts the audio playback stream, receive audio data to ring buffer.
 * @return 0 on success, non-zero on failure.
 */
u32 usbd_composite_uac_start_play(void);

/**
 * @brief Stops the audio playback stream.
 */
void usbd_composite_uac_stop_play(void);

/**
  * @brief  Read the data from the ring buffer for playing
  * @param  buffer: write buffer handle used to save the data
  * @param  size: allow to write buffer size
  * @param  time_out_ms if 0 means no wait,
  *                     other value will set time_out_ms to read buffer,if read data for more than time_out_ms, it will return.
  * @param  zero_pkt_flag: zero packet, each bit means a zero packet
  * @return return the read data length in bytes
  */
u32 usbd_composite_uac_read(u8 *buffer, u32 size, u32 time_out_ms, u32 *zero_pkt_flag);

/**
 * @brief Gets the number of available audio frames count ready for reading.
 * @return The number of queued audio frames.
 */
u32  usbd_composite_uac_get_read_frame_cnt(void);

/**
 * @brief Gets the time duration of available audio frames.
 * @return return the time duration for the queued audio frames duration in us
 */
u32  usbd_composite_uac_get_read_frame_time_in_us(void);

// for USB IN tx, TODO
// u32  usbd_uac_write(void, void *buffer, u32 size, u8 blocking);
// u32  usbd_uac_get_write_frame_cnt(void);

/** @} End of Device_Composite_UAC_Functions group */
/** @} End of USB_Device_Functions group */
/** @} End of USB_Device_API group */

#endif // USBD_COMPOSITE_UAC_H