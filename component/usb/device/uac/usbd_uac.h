/*
 * Copyright (c) 2025 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBD_UAC_H
#define USBD_UAC_H

/* Includes ------------------------------------------------------------------*/

#include "usbd.h"

/* Exported defines ----------------------------------------------------------*/

/** @addtogroup USB_Device_API USB Device API
 *  @{
 */
/** @addtogroup USB_Device_Constants USB Device Constants
 * @{
 */
/** @addtogroup Device_UAC_Constants Device UAC Constants
 * @{
 */

#define USBD_UAC_DEBUG                              0    /**< Enable/Disable UAC debug feature. */

#if USBD_UAC_DEBUG && (USBD_TP_TRACE_DEBUG == 0)
#error "Please set USBD_TP_TRACE_DEBUG in usbd.h"
#endif

/* Defines basic device parameters like VID, PID, and string descriptors. */
#define USBD_UAC_VID                                USB_VID /**< Vendor ID. */
#define USBD_UAC_PID                                USB_PID /**< Product ID. */

/* Uac Endpoint parameters */
#if defined (CONFIG_AMEBAGREEN2)
#define USBD_UAC_ISOC_IN_EP                         0x84U   /**< EP for ISOC IN. */
#define USBD_UAC_ISOC_OUT_EP                        0x02U   /**< EP for ISOC OUT. */
#else
#define USBD_UAC_ISOC_IN_EP                         0x81U   /**< EP for ISOC IN. */
#define USBD_UAC_ISOC_OUT_EP                        0x02U   /**< EP for ISOC OUT. */
#endif

/* Audio channel count */
#define USBD_UAC_CH_CNT_2                           2U /**< Audio 2 channel count. */
#define USBD_UAC_CH_CNT_4                           4U /**< Audio 4 channel count. */
#define USBD_UAC_CH_CNT_6                           6U /**< Audio 6 channel count. */
#define USBD_UAC_CH_CNT_8                           8U /**< Audio 8 channel count. */

/* Audio byte width */
#define USBD_UAC_BYTE_WIDTH_2                       2U /**< Audio 2 bytes width. */
#define USBD_UAC_BYTE_WIDTH_3                       3U /**< Audio 3 bytes width. */
/* amebasmart itself cannot play 32-bit audio data */
#define USBD_UAC_BYTE_WIDTH_4                       4U /**< Audio 4 bytes width. */

/* Default channel cnt */
#define USBD_UAC_DEFAULT_CH_CNT                     USBD_UAC_CH_CNT_8 /**< Default audio channel count. */

/* Default byte width */
#define USBD_UAC_DEFAULT_BYTE_WIDTH                 USBD_UAC_BYTE_WIDTH_2 /**< Default audio byte width. */

/* Sampling frequency */
#define USBD_UAC_SAMPLING_FREQ_44K                  44100U  /**< Audio 44100 sample frequency. */
#define USBD_UAC_SAMPLING_FREQ_48K                  48000U  /**< Audio 48000 sample frequency. */
#define USBD_UAC_SAMPLING_FREQ_96K                  96000U  /**< Audio 96000 sample frequency. */
#define USBD_UAC_SAMPLING_FREQ_192K                 192000U /**< Audio 192000 sample frequency. */

/** @} End of Device_UAC_Constants group*/
/** @} End of USB_Device_Constants group*/

/* Exported types ------------------------------------------------------------*/

/** @addtogroup USB_Device_Types USB Device Types
 * @{
 */
/** @addtogroup Device_UAC_Types Device UAC Types
 * @{
 */

/* Audio configuration parameters: The application uses this structure to configure audio parameters to the UAC driver. */
typedef struct {
	u32 sampling_freq;   /**< Audio sample frequency param. */
	u8 byte_width;       /**< Audio byte width param. */
	u8 ch_cnt;           /**< Audio channel count param. */
	u8 enable;           /**< Audio enable. */
} usbd_audio_cfg_t;

typedef struct {
	usbd_audio_cfg_t in;     /**< Audio configuration parameters for record. */
	usbd_audio_cfg_t out;    /**< Audio configuration parameters for playback. */
	void *audio_ctx;         /**< Audio context params. */

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
	 * @brief Called to handle class-specific SETUP requests.
	 * @param[in] req: Pointer to the setup request packet.
	 * @param[out] buf: Pointer to a buffer for data stage of control transfers.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* setup)(usb_setup_req_t *req, u8 *buf);

	/**
	 * @brief Called when the UAC class driver initialization for application resource setup.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* set_config)(void);

	/**
	 * @brief Called when USB attach status changes for application to support hot-plug events.
	 * @param[in] old_status: The previous attach status.
	 * @param[in] status: The new attach status.
	 */
	void(* status_changed)(u8 old_status, u8 status);

	/**
	 * @brief Called when the UAC class driver initialization for application resource setup.
	 * @param[in] mute: Mute value, 0 unmute, 1 mute
	 */
	void(* mute_changed)(u8 mute);

	/**
	 * @brief Called when the UAC class driver initialization for application resource setup.
	 * @param[in] volume: Volume value, from 0~100
	 */
	void(* volume_changed)(u8 volume);

	/**
	 * @brief Called when the audio format changed in the UAC class driver.
	 * @param[in] sampling_freq: New sample frequency.
	 * @param[in] ch_cnt: New channel count. such as 2,4,6,8...
	 * @param[in] byte_width: New byte width, such as 1,2,3,4.
	 */
	void(* format_changed)(u32 sampling_freq, u8 ch_cnt, u8 byte_width);

	/**
	 * @brief Called when the SOF interrupt occurs in the UAC class driver.
	 */

	void(* sof)(void);
} usbd_uac_cb_t;

/* Audio data buffer structure */
typedef struct {
	u8 *buf_raw;             /**< Pointer to the buffer. */
	__IO u16 buf_valid_len;  /**< Valid buffer length. */
} usbd_uac_buf_t;

/* Audio control structure */
typedef struct {
	rtos_sema_t uac_isoc_sema; /**< ISOC sema. */
	usbd_uac_buf_t *buf_array; /**< Pointer to audio buffer array. */
	u8 *isoc_buf;              /**< Pointer to audio total buffer. */

	u16 isoc_mps;              /**< ISOC maximum packet size. */
	u16 buf_array_cnt;         /**< Valid audio buffer array count. */

	u8 uac_sema_valid;         /**< ISOC sema created. */
	__IO u8 read_wait_sema;    /**< The sema is waiting. */
	__IO u8 isoc_read_idx;     /**< Next read position. */
	__IO u8 isoc_write_idx;    /**< Next write position. */
	__IO u8 next_xfer;         /**< Audio transfer continue flag. */
} usbd_uac_buf_ctrl_t;

/** @} End of Device_UAC_Types group*/
/** @} End of USB_Device_Types group*/

/**
 * @brief Structure representing the UAC device instance.
 */
typedef struct {
	usbd_uac_buf_ctrl_t uac_isoc_out;  /**< ISOC OUT control structure. */
	usbd_uac_buf_ctrl_t uac_isoc_in;   /**< ISOC IN control structure. */
	usbd_ep_t ep_isoc_in;              /**< ISOC IN endpoint structure. */
	usbd_ep_t ep_isoc_out;             /**< ISOC OUT endpoint structure. */
	usb_setup_req_t ctrl_req;          /**< Stores the current control request. */
	usb_dev_t *dev;                    /**< Pointer to the USB device instance. */
	usbd_uac_cb_t *cb;                 /**< Pointer to the user-defined callback structure. */

	u32 cur_sampling_freq;             /**< Current Audio sample freqnency. */
	u16 cur_volume;                    /**< Current Audio volume . */
	u8 cur_byte_width;                 /**< Current Audio byte width. */
	u8 cur_clk_valid;                  /**< Current Clock valid flag. */
	u8 alt_setting;                    /**< Current choose alternate setting. */
	u8 cur_ch_cnt;                     /**< Current Audio channel count. */
	u8 cur_mute;                       /**< Current Audio mute value. */

#if USBD_UAC_DEBUG
	u32 copy_data_len;                 /**< Audio xfer total data length. */
	__IO u32 isoc_rx_cnt;              /**< Audio xfer rx count. */
	__IO u32 isoc_timeout_cnt;         /**< Audio timeout count. */
	__IO u32 isoc_overwrite_cnt;       /**< Audio over write count. */
	__IO u8  isoc_dump_thread;         /**< Audio dump thread running flag. */
#endif
} usbd_uac_dev_t;

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @addtogroup USB_Device_Functions USB Device Functions
 * @{
 */
/** @addtogroup Device_UAC_Functions Device UAC Functions
 * @{
 */
/**
 * @brief Initializes class driver with application callback handler.
 * @param[in] cb: Pointer to the user-defined callback structure.
 * @return 0 on success, non-zero on failure.
 */
int usbd_uac_init(usbd_uac_cb_t *cb);

/**
  * @brief  DeInitialize UAC device
  * @return 0 on success, non-zero on failure.
  */
int usbd_uac_deinit(void);

/**
  * @brief  Set audio configuration to the UAC class
  * @param[in] uac_cfg: Audio configure params
  * @param[in] is_record: 1 for recording, 0 for playback
  * @param[in] flag: Unused parameter
  * @retval 0 on success, non-zero on failure.
  */
u8  usbd_uac_config(const usbd_audio_cfg_t *uac_cfg, u8 is_record, u32 flag);

/**
  * @brief  USB audio start play
  * @retval 0 on success, non-zero on failure.
  */
u32  usbd_uac_start_play(void);

/**
  * @brief  USB audio stop play
  */
void usbd_uac_stop_play(void);

/**
  * @brief  Read the data from the ring_buffer
  * @param[in] buffer: write buffer handle used to save the data
  * @param[in] size: allow to write buffer size
  * @param[in] time_out_ms: if 0 means no wait,
  *                     other value will set time_out_ms to read buffer,if read data for more than time_out_ms, it will return.
  * @retval return the read data length in bytes
  */
u32 usbd_uac_read(u8 *buffer, u32 size, u32 time_out_ms);

/**
  * @brief  Get the read frame packet count
  * @retval return avail packet count
  */
u32  usbd_uac_get_read_frame_cnt(void);

/** @} End of Device_UAC_Functions group */
/** @} End of USB_Device_Functions group */
/** @} End of USB_Device_API group */

#endif  /* USBD_UAC_H */