/*
 * Copyright (c) 2025 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBD_UAC_H
#define USBD_UAC_H

/* Includes ------------------------------------------------------------------*/

#include "usbd.h"
#include "usb_ringbuf.h"

#ifdef __cplusplus
extern "C" {
#endif

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

#define USBD_UAC_DEBUG              0    /**< Enable/Disable UAC debug feature. */

/**
 * @brief SOF-to-data desync threshold before the OUT ISR helper starts appending
 *        ZLP filler nodes. Prevents readers from stalling when the host silences
 *        the ISOC stream (e.g. switching tracks).
 */
#define USBD_UAC_ISOC_RX_APPEND_ZLP_DIFF_MAX 3U

/**
 * @brief 2^(n) shorthand used across UAC MPS/ring/interval scaling.
 */
#define USBD_UAC_POW2(n)            (1U << (n))

#if USBD_UAC_DEBUG && (USBD_TP_TRACE_DEBUG == 0)
#error "Please set USBD_TP_TRACE_DEBUG in usbd.h"
#endif

/* Defines basic device parameters like VID, PID, and string descriptors. */
#define USBD_UAC_VID                USB_VID /**< Vendor ID. */
#define USBD_UAC_PID                USB_PID /**< Product ID. */

/* Uac Endpoint parameters */
/* Audio channel count */
#define USBD_UAC_CH_CNT_2           2U /**< Audio 2 channel count. */
#define USBD_UAC_CH_CNT_4           4U /**< Audio 4 channel count. */
#define USBD_UAC_CH_CNT_6           6U /**< Audio 6 channel count. */
#define USBD_UAC_CH_CNT_8           8U /**< Audio 8 channel count. */

/* Audio byte width */
#define USBD_UAC_BYTE_WIDTH_2       2U /**< Audio 2 bytes width. */
#define USBD_UAC_BYTE_WIDTH_3       3U /**< Audio 3 bytes width. */
/* amebasmart itself cannot play 32-bit audio data */
#define USBD_UAC_BYTE_WIDTH_4       4U /**< Audio 4 bytes width. */

/* Default channel cnt */
#define USBD_UAC_DEFAULT_CH_CNT     USBD_UAC_CH_CNT_8 /**< Default audio channel count. */

/* Default byte width */
#define USBD_UAC_DEFAULT_BYTE_WIDTH USBD_UAC_BYTE_WIDTH_2 /**< Default audio byte width. */

/* Microphone (record) default channel cnt */
#define USBD_UAC_IN_DEFAULT_CH_CNT      USBD_UAC_CH_CNT_2 /**< Default microphone channel count (stereo, aligned with n_push). */

/* Microphone (record) default byte width */
#define USBD_UAC_IN_DEFAULT_BYTE_WIDTH  USBD_UAC_BYTE_WIDTH_2 /**< Default microphone byte width. */

/* Microphone (record) default sampling frequency */
#define USBD_UAC_IN_DEFAULT_SAMPLING_FREQ   USBD_UAC_SAMPLING_FREQ_16K /**< Default microphone sampling frequency (16 kHz, aligned with n_push). */

/* Sampling frequency */
#define USBD_UAC_SAMPLING_FREQ_16K  16000U  /**< Audio 16000 sample frequency. */
#define USBD_UAC_SAMPLING_FREQ_44K  44100U  /**< Audio 44100 sample frequency. */
#define USBD_UAC_SAMPLING_FREQ_48K  48000U  /**< Audio 48000 sample frequency. */
#define USBD_UAC_SAMPLING_FREQ_96K  96000U  /**< Audio 96000 sample frequency. */
#define USBD_UAC_SAMPLING_FREQ_192K 192000U /**< Audio 192000 sample frequency. */

/** @} End of Device_UAC_Constants group */
/** @} End of USB_Device_Constants group */

/* Exported types ------------------------------------------------------------*/

/** @addtogroup USB_Device_Types USB Device Types
 * @{
 */
/** @addtogroup Device_UAC_Types Device UAC Types
 * @{
 */

/* Endpoint address configuration for the UAC driver */
typedef struct {
	u8 isoc_in_addr;   /**< ISOC IN endpoint address */
	u8 isoc_out_addr;  /**< ISOC OUT endpoint address */
} usbd_uac_ep_cfg_t;

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
	int (*init)(void);

	/**
	 * @brief Called when the UAC device is de-initialized for resource cleanup.
	 * @return 0 on success, non-zero on failure.
	 */
	int (*deinit)(void);

	/**
	 * @brief Called to handle class-specific SETUP requests.
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 * @param[in] req: Pointer to the setup request packet.
	 * @param[out] buf: Pointer to a buffer for data stage of control transfers.
	 * @return 0 on success, non-zero on failure.
	 */
	int (*setup)(usb_setup_req_t *req, u8 *buf);

	/**
	 * @brief Called when the UAC class driver initialization for application resource setup.
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 * @return 0 on success, non-zero on failure.
	 */
	int (*set_config)(void);

	/**
	 * @brief Called when USB attach status changes for application to support hot-plug events.
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 * @param[in] old_status: The previous attach status.
	 * @param[in] status: The new attach status.
	 */
	void (*status_changed)(u8 old_status, u8 status);

	/**
	 * @brief Called when the UAC class driver initialization for application resource setup.
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 * @param[in] mute: Mute value, 0 unmute, 1 mute
	 */
	void (*mute_changed)(u8 mute);

	/**
	 * @brief Called when the UAC class driver initialization for application resource setup.
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 * @param[in] volume: Volume value, from 0~100
	 */
	void (*volume_changed)(u8 volume);

	/**
	 * @brief Called when the audio format changed in the UAC class driver.
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 * @param[in] sampling_freq: New sample frequency.
	 * @param[in] ch_cnt: New channel count. such as 2,4,6,8...
	 * @param[in] byte_width: New byte width, such as 1,2,3,4.
	 */
	void (*format_changed)(u32 sampling_freq, u8 ch_cnt, u8 byte_width);

	/**
	 * @brief Called when the SOF interrupt occurs in the UAC class driver.
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 */

	void (*sof)(void);
} usbd_uac_cb_t;

/* Audio control structure */
typedef struct {
	rtos_sema_t uac_isoc_sema;   /**< ISOC sema. */
	usb_ringbuf_manager_t buf_list; /**< Ring buffer manager for ISOC packets. */

	/*
	 * used to append ZLP while xfer error/timeout,
	 * sof_idx counts SOF frames while xfer_continue is on;
	 * data_idx is bumped by every real xfer done and by every appended ZLP.
	 * When sof_idx - data_idx >= USBD_UAC_ISOC_RX_APPEND_ZLP_DIFF_MAX we
	 * synthesise a filler node so consumers do not stall.
	 */
	__IO u32 sof_idx;
	__IO u32 data_idx;
	__IO u32 xfer_cnt;         /**< Xfer valid count */
	u16 last_xfer_len;         /**< Last xfer length, used to size appended ZLP */

	u16 isoc_mps;              /**< ISOC maximum packet size. */
	u8 buf_list_cnt;           /**< Ring buffer node count. */
	u8 binterval;              /**< bInterval used for MPS/ring sizing. */

	u8 uac_sema_valid;         /**< ISOC sema created. */
	__IO u8 read_wait_sema;    /**< The sema is waiting. */
	__IO u8 next_xfer;         /**< Audio transfer continue flag. */
#if USBD_UAC_DEBUG
	__IO u32 append_zlp_cnt;   /**< Zero-length filler node inserted from SOF path. */
	__IO u32 overwrite_cnt;    /**< OUT ISR forced head-drop due to full ring. */
	__IO u32 append_overwrite_cnt;/**< SOF append forced head-drop due to full ring. */
	__IO u32 timeout_cnt;      /**< Inter-packet gap exceeded expected step. */
	u32 last_xfer_tick;        /**< Timestamp (us) of previous xfer, for gap check. */
#endif
} usbd_uac_buf_ctrl_t;

/** @} End of Device_UAC_Types group */
/** @} End of USB_Device_Types group */

/**
 * @brief Structure representing the UAC device instance.
 */
typedef struct {
	const usbd_uac_ep_cfg_t *ep_cfg;   /**< Pointer to endpoint address configuration. */
	const usbd_uac_cb_t *cb;           /**< Pointer to the user-defined callback structure. */
	usb_dev_t *dev;                    /**< Pointer to the USB device instance. */

	usbd_uac_buf_ctrl_t uac_isoc_out;  /**< ISOC OUT control structure. */
	usbd_uac_buf_ctrl_t uac_isoc_in;   /**< ISOC IN control structure. */
	usbd_ep_t ep_isoc_in;              /**< ISOC IN endpoint structure. */
	usbd_ep_t ep_isoc_out;             /**< ISOC OUT endpoint structure. */
	usb_setup_req_t ctrl_req;          /**< Stores the current control request. */

	u32 cur_sampling_freq;             /**< Current Audio sample freqnency. */
	u16 cur_volume;                    /**< Current Audio volume . */
	u8 cur_byte_width;                 /**< Current Audio byte width. */
	u8 cur_clk_valid;                  /**< Current Clock valid flag. */
	u8 alt_setting;                    /**< Current choose alternate setting. */
	u8 alt_setting_in;                 /**< Current choose alternate setting for the microphone AS interface. */
	u8 cur_ch_cnt;                     /**< Current Audio channel count. */
	u8 cur_mute;                       /**< Current Audio mute value. */
	u16 cur_volume_in;                 /**< Current microphone (record) volume. */
	u8 cur_mute_in;                    /**< Current microphone (record) mute value. */

#if USBD_UAC_DEBUG
	u32 copy_data_len;                 /**< Audio xfer total data length. */
	__IO u8  isoc_dump_thread;         /**< Audio dump thread running flag. */
#endif
	u8 from_composite;			/**< Flag indicating if part of a composite device. */
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
 * @brief Initializes class driver as a standalone device.
 * @param[in] cb: Pointer to the user-defined callback structure.
 * @param[in] ep_cfg: Pointer to endpoint address configuration, or NULL for defaults.
 * @return 0 on success, non-zero on failure.
 */
int usbd_uac_init(const usbd_uac_cb_t *cb, const usbd_uac_ep_cfg_t *ep_cfg);

#ifdef CONFIG_USBD_COMPOSITE
/**
 * @brief Initializes class driver as part of a composite device.
 * @param[in] cb: Pointer to the user-defined callback structure.
 * @param[in] ep_cfg: Pointer to endpoint address configuration, or NULL for defaults.
 * @return 0 on success, non-zero on failure.
 */
int usbd_composite_uac_init(const usbd_uac_cb_t *cb, const usbd_uac_ep_cfg_t *ep_cfg);
#endif

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
int usbd_uac_config(const usbd_audio_cfg_t *uac_cfg, u8 is_record, u32 flag);

/**
  * @brief  USB audio start play
  * @retval 0 on success, non-zero on failure.
  */
int usbd_uac_start_play(void);

/**
  * @brief  USB audio stop play
  */
void usbd_uac_stop_play(void);

/**
  * @brief  USB audio start record
  * @retval 0 on success, non-zero on failure.
  */
int usbd_uac_start_record(void);

/**
  * @brief  USB audio stop record
  */
void usbd_uac_stop_record(void);

/**
  * @brief  Push one ISOC IN packet of PCM data into the record TX ring buffer.
  * @param[in] buf: pointer to the packet payload, no larger than the ISOC IN max packet size.
  * @param[in] len: length of the packet in bytes.
  * @retval 0 on success, non-zero on failure.
  */
int usbd_uac_transmit_data(u8 *buf, u32 len);

/**
  * @brief  Read the data from the ring_buffer
  * @param[in]  buffer: write buffer handle used to save the data
  * @param[in]  size: allow to write buffer size
  * @param[in]  time_out_ms: if 0 means no wait,
  *                     other value will set time_out_ms to read buffer,if read data for more than time_out_ms, it will return.
  * @param[out] zero_pkt_flag: bitmap of zero-length filler packets returned in this read. Bit N is set if
  *                            the N-th packet copied out was a ZLP inserted by the SOF underrun handler.
  *                            May be NULL if the caller does not care.
  * @retval return the read data length in bytes
  */
u32 usbd_uac_read(u8 *buffer, u32 size, u32 time_out_ms, u32 *zero_pkt_flag);

/**
  * @brief  Get the read frame packet count
  * @retval return avail packet count
  */
u32  usbd_uac_get_read_frame_cnt(void);

/**
  * @brief  Get the time duration of currently queued audio frames in microseconds.
  * @retval frame_cnt * bInterval_us
  */
u32  usbd_uac_get_read_frame_time_in_us(void);

/** @} End of Device_UAC_Functions group */
/** @} End of USB_Device_Functions group */
/** @} End of USB_Device_API group */

#ifdef __cplusplus
}
#endif

#endif  /* USBD_UAC_H */
