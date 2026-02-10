/*
 * Copyright (c) 2025 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_COMPOSITE_UAC1_H
#define USBH_COMPOSITE_UAC1_H

/* Includes ------------------------------------------------------------------*/
#include "usbh.h"
#include "usb_ch9.h"
#include "usb_ringbuf.h"
#include "usbh_composite_config.h"
#include "os_wrapper.h"

/* Exported defines ----------------------------------------------------------*/

#define USBH_UAC_TERM_MAX_CNT          8U        /**< Maximum number of Audio Terminals supported */
#define USBH_UAC_FU_MAX_CNT            8U        /**< Maximum number of Feature Units supported */
#define USBH_UAC_MAX_CHANNEL           8U        /**< Maximum number of audio channels supported (e.g., 8 for 7.1 audio) */

#define USBH_UAC_ALT_SETTING_MAX       10        /**< Maximum number of Alternate Settings per interface */
#define USBH_UAC_FREQ_FORMAT_MAX       6         /**< Maximum number of discrete sampling frequencies per format */

/* Exported types ------------------------------------------------------------*/

/* uac1.0 Audio Control   */
/**
  * @brief  UAC 1.0 Audio Terminal Information structure.
  *         Describes an Input Terminal or Output Terminal in the audio function topology.
  */
typedef struct {
	u16 terminal_type; /**< Terminal Type (e.g., 0x0101 USB Streaming, 0x0301 Speaker) */
	u8 terminal_id;    /**< ID of this Terminal */
	u8 source_id;      /**< ID of the entity to which this Terminal is connected */
	u8 is_input;       /**< Direction flag (1: Input Terminal, 0: Output Terminal) */
} usbh_uac_term_info;

/**
  * @brief  UAC 1.0 Feature Unit (Volume Control) Information structure.
  */
typedef struct {
	u16 channel_support[USBH_UAC_MAX_CHANNEL]; /**< Bitmap of supported controls for each logical channel */
	u16 master_support;    /**< Bitmap of supported controls for the Master Channel (Channel 0) */
	u16 sink_type;         /**< Type of the sink terminal connected to this unit */
	u8 unit_id;            /**< ID of this Feature Unit */
	u8 source_id;          /**< ID of the source entity connected to this unit */
	u8 sink_id;            /**< ID of the sink entity connected to this unit */
	u8 num_channels;       /**< Number of logical audio channels */
	u8 control_size;       /**< Size of the control field in bytes (usually 1 or 2) */
} usbh_uac_vol_ctrl_info;

/**
  * @brief  Audio Control (AC) Interface Aggregated Information.
  *         Contains the topology of all Terminals and Units within the AC interface.
  */
typedef struct {
	/* terminals array */
	usbh_uac_vol_ctrl_info controls[USBH_UAC_FU_MAX_CNT];  /**< Array of Feature Units found */
	usbh_uac_term_info terminals[USBH_UAC_TERM_MAX_CNT];   /**< Array of Terminals found */

	u32 volume_ctrl_count; /**< Count of valid Feature Units */
	u32 terminal_count;    /**< Count of valid Terminals */
	u8 best_match_idx;     /**< Index of the best matching Feature Unit (for main volume control) */
	u8 ac_itf_idx;         /**< Interface index of the Audio Control interface */
} usbh_uac_ac_itf_info_t;

/**
  * @brief  Audio Buffer and Transmission Control Block.
  *         Manages ring buffers, synchronization, and isochronous packet sizing.
  */
typedef struct {
	usb_ringbuf_manager_t buf_manager; /**< Ring buffer manager instance */
	rtos_sema_t isoc_sema;             /**< Semaphore for isochronous transfer synchronization */

	/* Sampling Frequency and Packet Size Calculations */
	u32 sample_freq;       /**< Current sampling frequency (Hz, e.g., 48000) */
	u32 sample_rem;        /**< Remainder when dividing frequency by packets/sec (for fractional sample rates) */
	u32 sample_accum;      /**< Accumulator for fractional sample rate calculation */
	u32 last_sample_accum; /**< Previous accumulator value */
	u32 pkt_per_second;    /**< Number of USB packets per second (e.g., 1000 for Full Speed) */

	u16 frame_cnt;         /**< Number of frames configured for the buffer */

	__IO u16 written;        /**< Length of data already written to the current packet buffer */
	__IO u16 mps;            /**< Endpoint Maximum Packet Size */
	__IO u8 sema_valid;      /**< Flag indicating if the semaphore is valid */
	__IO u8 write_wait_sema; /**< Flag indicating if a write operation is waiting for the semaphore */
	__IO u8 next_xfer;       /**< Flag to trigger the next transfer */
} usbh_uac_buf_ctrl_t;

/**
  * @brief  Audio Volume Status Information.
  */
typedef struct {
	s16 volume;    /**< Current volume value (resolution depends on device, usually 1/256 dB) */
	s16 vol_min;   /**< Minimum volume supported by the device */
	s16 vol_max;   /**< Maximum volume supported by the device */
	u8  mute;      /**< Current mute state (1: Muted, 0: Unmuted) */
} usbh_uac_volume_info_t;

/**
  * @brief  UAC User Callback Interface.
  */
typedef struct {
	int(* init)(void);      /**< Callback when UAC driver is initialized */
	int(* deinit)(void);    /**< Callback when UAC driver is de-initialized */
	int(* attach)(void);    /**< Callback when a UAC device is attached */
	int(* detach)(void);    /**< Callback when a UAC device is detached */
	int(* setup)(void);     /**< Callback during the Setup stage of a control transfer */
	int(* isoc_transmitted)(usbh_urb_state_t state); /**< Callback when isochronous OUT (Play) transfer completes */
	int(* isoc_received)(u8 *buf, u32 len);          /**< Callback when isochronous IN (Record) data is received */
} usbh_composite_uac_usr_cb_t;

/**
  * @brief  Audio Format Configuration Descriptor Information.
  */
typedef struct {
	u8 freq[USBH_UAC_FREQ_FORMAT_MAX][3]; /**< List of supported sampling frequencies (3-byte format) */
	u8 bit_width; /**< Bit resolution (e.g., 16, 24, 32 bits) */
	u8 channels;  /**< Number of channels */
	u8 freq_cnt;  /**< Count of supported sampling frequencies */
} usbh_uac_format_cfg_t;

/**
  * @brief  Audio Streaming (AS) Interface Alternate Setting Information.
  */
typedef struct {
	usbh_uac_format_cfg_t format_info; /**< Audio format details for this setting */
	usbh_ep_desc_t ep_desc;            /**< Associated endpoint descriptor */
	u8 alt_setting;                    /**< USB Alternate Setting number */
} usbh_uac_as_itf_alt_info_t;

/**
  * @brief  Generic Audio Format Structure (Used for API interaction).
  */
typedef struct {
	u32 sampling_freq;     /**< Sampling frequency in Hz */
	u8 bit_width;          /**< Bit depth */
	u8 ch_cnt;             /**< Channel count */
} usbh_audio_fmt_t;

/**
  * @brief  Audio Streaming (AS) Interface Aggregated Information.
  *         Manages all Alternate Settings and formats for a specific AS interface.
  */
typedef struct {
	usbh_uac_as_itf_alt_info_t itf_info_array[USBH_UAC_ALT_SETTING_MAX]; /**< Stores valid alternate settings (skipping Alt 0 which usually has 0 bandwidth) */
	usbh_pipe_t pipe;                     /**< USB Pipe information associated with this interface */
	usbh_audio_fmt_t *fmt_array;          /**< Array of all parsed audio formats */
	u16 packet_size_small;                /**< Size of a small packet in samples (for non-integer freq ratios) */
	u16 packet_size_large;                /**< Size of a large packet in samples */

	u8 as_itf_num;                        /**< Interface number of this AS interface */
	u8 alt_setting_cnt;                   /**< Count of Alternate Settings available */
	u8 fmt_array_cnt;                     /**< Count of items in fmt_array */
	u8 choose_alt_idx;                    /**< Index of the currently selected setting in the internal array */
	u8 choose_freq_idx;                   /**< Index of the currently selected frequency */
} usbh_uac_as_itf_info_t;

/**
  * @brief  Main UAC Host Class Driver Handle.
  */
typedef struct {
	usbh_uac_volume_info_t volume_info[1 + USBH_UAC_MAX_CHANNEL]; /**< Volume info array (Index 0 is Master) */
	usbh_uac_buf_ctrl_t isoc_out;          /**< Isochronous OUT (Playback) buffer control */
	usbh_uac_ac_itf_info_t  ac_isoc_in;    /**< Audio Control Interface info (Topology) */
	usbh_uac_as_itf_info_t *as_isoc_out;   /**< Pointer to Audio Streaming Output interface info */
	usbh_uac_as_itf_info_t *as_isoc_in;    /**< Pointer to Audio Streaming Input interface info */
	usbh_composite_uac_usr_cb_t *cb;       /**< User callback structure */
	usbh_composite_host_t *driver;         /**< Pointer to the USB Host Composite handle */
	u8 *audio_ctrl_buf;                    /**< Buffer for CTRL transfers */
	u8 *isoc_tx_buf;                      /**< Buffer for ISOC out transfers. */
	u8 *ringbuf_partial_write_buf;        /**< Buffer for saving partial data packets. */

#if USBH_COMPOSITE_HID_UAC_DEBUG
	/* Debugging and Statistics */
	rtos_task_t dump_status_task;
	__IO u32 sof_cnt;                 /**< SOF interrupt counter */
	__IO u32 isoc_tx_start_cnt;       /**< Isochronous TX start counter */
	__IO u32 isoc_tx_done_cnt;        /**< Isochronous TX completion counter */

	__IO u32 isoc_xfer_buf_empty_cnt; /**< Buffer underrun counter */
	__IO u32 isoc_xfer_buf_err_cnt;   /**< Transfer error counter */
	__IO u32 isoc_xfer_interval_cnt;  /**< Interval mismatch counter */

	__IO u8 dump_status_task_alive;
	__IO u8 dump_status_task_exit;
#endif

	__IO u8 xfer_state;        /**< Current data transfer state (IDLE/BUSY/WAIT_SOF) */
	__IO u8 ctrl_state;        /**< Current control transfer state */
	u16 volume_db;             /**< Current volume in dB representation */
	u8 ch_idx;                 /**< Current channel index being operated on */
	u8 mute_value;             /**< Global mute state (1: Mute, 0: Unmute) */
	u8 next_xfer;              /**< Flag to trigger next transfer */
	u8 cur_volume;             /**< Current volume percentage (0 ~ 100) */
} usbh_composite_uac_t;

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/
extern const usbh_class_driver_t usbh_composite_uac_driver;  /**< Composite uac driver handle */

/* Exported functions --------------------------------------------------------*/
/**
  * @brief  Initialize the UAC Class Driver.
  * @param  chost: Pointer to the composite host handle.
  * @param  cb: Pointer to the user callback structure.
  * @param  frame_cnt: Configuration for ring buffer size (frame count).
  * @retval Status (0: Success, <0: Failure)
  */
int usbh_composite_uac_init(usbh_composite_host_t *chost, usbh_composite_uac_usr_cb_t *cb, int frame_cnt);

/**
  * @brief  De-initialize the UAC Class Driver and release resources.
  * @retval Status
  */
int usbh_composite_uac_deinit(void);

/**
  * @brief  Get the volume information .
  *         Loop to find volume mute state, volume max min and cur value.
  * @param  host: Pointer to the USB core handle.
  * @retval Status
  */
int usbh_composite_uac_get_volume_infor(usb_host_t *host);


/**
  * @brief  Configure the UAC Audio Format.
  *         Finds the matching Alternate Setting and sends the SET_INTERFACE request.
  * @param  dir: Direction (0: Playback/OUT, 1: Record/IN).
  * @param  channels: Number of channels.
  * @param  bit_width: Bit width.
  * @param  sampling_freq: Sampling frequency in Hz.
  * @retval Status
  */
int usbh_composite_uac_set_alt_setting(u8 dir, u8 channels, u8 bit_width, u32 sampling_freq);

/**
  * @brief  Get the list of audio formats supported by the connected device.
  * @param  dir: Direction (0: Playback/OUT, 1: Record/IN).
  * @param  fmt_cnt: Pointer to store the number of formats found.
  * @retval Pointer to the array of supported formats.
  */
const usbh_audio_fmt_t *usbh_composite_uac_get_alt_setting(u8 dir, u8 *fmt_cnt);

/**
  * @brief  Calculate the frame size (in bytes) based on current configuration.
  * @param  dir: Direction.
  * @retval Frame size in bytes.
  */
u32 usbh_composite_uac_get_frame_size(u8 dir);

/**
  * @brief  Set the volume level percentage.
  * @param  volume: Volume level (0-100).
  * @retval Status
  */
int usbh_composite_uac_set_volume(u8 volume);

/**
  * @brief  Set the mute state.
  * @param  mute: 1 to mute, 0 to unmute.
  * @retval Status
  */
int usbh_composite_uac_set_mute(u8 mute);

/**
  * @brief  Write audio data to the UAC driver ring buffer.
  * @param  buffer: Pointer to the PCM data buffer.
  * @param  size: Size of data to write (in bytes).
  * @param  timeout_ms: Timeout in milliseconds to wait if buffer is full.
  * @retval Actual number of bytes written.
  */
u32 usbh_composite_uac_write(u8 *buffer, u32 size, u32 timeout_ms);

/**
  * @brief  Start the audio playback process.
  *         Enables SOF handling and prepares for data transmission.
  */
void usbh_composite_uac_start_play(void);

/**
  * @brief  Stop the audio playback process.
  */
void usbh_composite_uac_stop_play(void);
#endif  /* USBH_COMPOSITE_UAC1_H */
