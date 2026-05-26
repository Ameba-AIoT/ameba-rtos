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

#define USBH_UAC_TERM_MAX_CNT           8U      /**< Maximum number of Audio Terminals supported */
#define USBH_UAC_FU_MAX_CNT             4U      /**< Maximum number of Feature Units supported */
#define USBH_UAC_MAX_CHANNEL            8U      /**< Maximum number of audio channels supported (e.g., 8 for 7.1 audio) */

#define USBH_UAC_ALT_SETTING_MAX        10      /**< Maximum number of Alternate Settings per interface */
#define USBH_UAC_FREQ_FORMAT_MAX        6       /**< Maximum number of discrete sampling frequencies per format */

#define USBH_UAC_ISOC_OUT_DIR           0U      /**< Isochronous OUT endpoint (Playback) */
#define USBH_UAC_ISOC_IN_DIR            1U      /**< Isochronous IN endpoint (Recording) */

/**
 * @brief UAC Mode Configuration
 * Supports three modes:
 *   - USBH_UAC_MODE_PLAYBACK_ONLY: Speaker/Playback only
 *   - USBH_UAC_MODE_RECORD_ONLY: Microphone/Recording only
 *   - USBH_UAC_MODE_PLAYBACK_AND_RECORD: Both Playback and Recording
 */
typedef enum {
	USBH_UAC_MODE_NONE                  = 0U,   /**< No UAC interface detected */
	USBH_UAC_MODE_PLAYBACK_ONLY         = 1U,   /**< Playback (Speaker) only, no recording */
	USBH_UAC_MODE_RECORD_ONLY           = 2U,   /**< Recording (Microphone) only, no playback */
	USBH_UAC_MODE_PLAYBACK_AND_RECORD   = 3U    /**< Both Playback and Recording supported */
} usbh_uac_mode_t;

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
	u16 channel_support[USBH_UAC_MAX_CHANNEL]; /**< Bitmap of supported fu_controls for each logical channel (index 0 = channel 1) */
	u16 master_support;    /**< Bitmap of supported fu_controls for the Master Channel (Channel 0) */
	u16 sink_type;         /**< Terminal Type of the sink connected to this Feature Unit (used to classify OUT/IN direction) */
	u8 unit_id;            /**< ID of this Feature Unit */
	u8 source_id;          /**< ID of the source entity connected to this Feature Unit */
	u8 sink_id;            /**< ID of the sink Terminal connected to this Feature Unit */
	u8 num_channels;       /**< Number of logical audio channels */
	u8 control_size;       /**< Size in bytes of each bmaControls field in the descriptor (1 or 2) */
} usbh_uac_vol_ctrl_info;

/**
  * @brief  Audio Control (AC) Interface Aggregated Information.
  *         Contains the topology of all Terminals and Feature Units within the AC interface.
  */
typedef struct {
	usbh_uac_vol_ctrl_info fu_controls[USBH_UAC_FU_MAX_CNT];  /**< Parsed Feature Units, indexed 0..volume_ctrl_count-1 */
	usbh_uac_term_info terminals[USBH_UAC_TERM_MAX_CNT];   /**< Parsed Terminals, indexed 0..terminal_count-1 */

	u8 volume_ctrl_count; /**< Number of valid entries in fu_controls[] */
	u8 terminal_count;    /**< Number of valid entries in fu_controls[] */

	u8 in_best_idx;      /**< Index into fu_controls[] for the best IN/Record Feature Unit; (u8)-1 if none */
	u8 out_best_idx;     /**< Index into fu_controls[] for the best OUT/Playback Feature Unit; (u8)-1 if none */

	u8 ac_itf_idx;        /**< Interface number of the Audio Control interface (used as wIndex in control requests) */
} usbh_uac_ac_itf_info_t;

/**
  * @brief  Audio Buffer and Transmission Control Block.
  *         Manages ring buffers, synchronization, and isochronous packet sizing.
  */
typedef struct {
	usb_ringbuf_manager_t buf_manager; /**< Ring buffer manager for isochronous data */
	rtos_sema_t isoc_sema;             /**< Semaphore signaled when a ring buffer slot becomes available */

	/* Sampling Frequency and Packet Size Calculations */
	u32 sample_freq;       /**< Current sampling frequency in Hz (e.g., 48000) */
	u32 sample_rem;        /**< Remainder of (sample_freq % pkt_per_second), used for fractional packet sizing */
	u32 sample_accum;      /**< Accumulated remainder across frames; when >= pkt_per_second, one extra sample is added */
	u32 last_sample_accum; /**< Saved accumulator value after the last committed packet, restored on ring-buffer full */
	u16 packet_rate;       /**< Number of USB packets per second (max: 8000 for HS ISOC) */

	__IO u16 written;        /**< Bytes accumulated in ringbuf_partial_write_buf for the current packet under construction */
	__IO u16 mps;            /**< Endpoint Maximum Packet Size in bytes */
	__IO u8 sema_valid;      /**< 1 when isoc_sema has been created and is safe to use */
	__IO u8 wait_sema;       /**< 1 while a write/read thread is blocked on isoc_sema (used for safe teardown) */
	__IO u8 next_xfer;       /**< 1 to allow isochronous transfers; cleared by stop_play/stop_capture */

#if USBH_COMPOSITE_HID_UAC_DEBUG
	__IO u32 last_xfer_len;      /**< Length of the last completed IN transfer (used to detect ZLP) */
	u32 xfer_start_cnt;          /**< Isochronous transfer start counter */
	u32 xfer_done_cnt;           /**< Isochronous transfer done counter */
	u32 xfer_buf_empty_cnt;      /**< Ring buffer underrun counter (OUT: no data to send) */
	u32 xfer_buf_err_cnt;        /**< Transfer error counter */
	u32 xfer_interval_cnt;       /**< SOF interval mismatch counter */
#endif

	u8 frame_cnt;         /**< Ring buffer depth in frames; set from cb->isoc_out/in_frm_cnt before ep_buf_ctrl_init */
} usbh_uac_buf_ctrl_t;

/**
  * @brief  Audio Volume Status Information.
  */
typedef struct {
	s16 volume;    /**< Current volume in device units (1/256 dB steps; 0x8000 = silence) */
	s16 vol_min;   /**< Minimum volume supported by the device */
	s16 vol_max;   /**< Maximum volume supported by the device */
	u8  mute;      /**< Current mute state (1: Muted, 0: Unmuted) */
} usbh_uac_volume_info_t;

/**
  * @brief  Audio Format Configuration Descriptor Information.
  */
typedef struct {
	u32 freq[USBH_UAC_FREQ_FORMAT_MAX]; /**< Supported discrete sampling frequencies in Hz */
	u8 bit_width; /**< Bit resolution per sample (e.g., 16, 24, 32) */
	u8 channels;  /**< Number of audio channels */
	u8 freq_cnt;  /**< Number of valid entries in freq[] */
} usbh_uac_format_cfg_t;

/**
  * @brief  Audio Streaming (AS) Interface Alternate Setting Information.
  */
typedef struct {
	usbh_uac_format_cfg_t format_info; /**< Audio format details for this alternate setting */
	usbh_ep_desc_t ep_desc;            /**< Isochronous endpoint descriptor for this alternate setting */
	u8 alt_setting;                    /**< USB Alternate Setting number as reported in the descriptor */
} usbh_uac_as_itf_alt_info_t;

/**
  * @brief  Generic Audio Format Structure (Used for API interaction).
  */
typedef struct {
	u32 sampling_freq;     /**< Sampling frequency in Hz */
	u8 bit_width;          /**< Bit depth per sample */
	u8 ch_cnt;             /**< Channel count */
} usbh_audio_fmt_t;

/**
  * @brief  Audio Streaming (AS) Interface Aggregated Information.
  *         Manages all Alternate Settings and formats for a specific AS interface.
  */
typedef struct {
	usbh_uac_as_itf_alt_info_t interface_array[USBH_UAC_ALT_SETTING_MAX]; /**< Valid alternate settings (Alt 0 skipped; index 0 = Alt 1) */
	usbh_pipe_t pipe;                     /**< USB pipe for this AS interface */
	usbh_audio_fmt_t *fmt_array;          /**< Flattened array of all supported audio formats (malloc'd in get_audio_format_info) */
	u16 packet_size_small;                /**< Bytes per packet when the sample accumulator does not overflow */
	u16 packet_size_large;                /**< Bytes per packet when the sample accumulator overflows (one extra sample) */

	u8 as_itf_num;                        /**< USB interface number of this AS interface */

	u8 alt_setting_cnt;                   /**< Number of valid entries in interface_array[] */
	u8 fmt_array_cnt;                     /**< Number of entries in fmt_array[] */

	u8 choose_alt_idx;                    /**< Index into interface_array[] for the currently active alternate setting */
	u8 choose_freq_idx;                   /**< Index into format_info.freq[] for the currently active sampling frequency */
} usbh_uac_as_itf_info_t;

/** @addtogroup USB_Host_API USB Host API
 *  @{
 */
/** @addtogroup USB_Host_Types USB Host Types
 * @{
 */
/** @addtogroup Host_Composite_UAC_Types Host Composite UAC Types
 * @{
 */
/**
  * @brief  UAC User Callback Interface.
  */
typedef struct {
	int (*init)(void);                                /**< Called once when the UAC driver is initialized */
	int (*deinit)(void);                              /**< Called once when the UAC driver is de-initialized */
	int (*attach)(void);                              /**< Called when a UAC device is enumerated and pipes are opened */
	int (*detach)(void);                              /**< Called when a UAC device is removed */
	int (*setup)(void);                               /**< Called on each USB Setup-stage transaction */
	int (*isoc_transmitted)(usbh_urb_state_t state);  /**< Reserved: registered by application but not currently invoked by the driver */
	int (*isoc_received)(u8 *buf, u32 len);           /**< Reserved: registered by application but not currently invoked by the driver */

	u8 isoc_in_frm_cnt;                               /**< Ring buffer depth in frames for IN/Record path (passed to ep_buf_ctrl_init) */
	u8 isoc_out_frm_cnt;                              /**< Ring buffer depth in frames for OUT/Playback path (passed to ep_buf_ctrl_init) */
} usbh_composite_uac_usr_cb_t;

typedef struct {
	usbh_uac_as_itf_info_t *as_itf;     /**< Pointer to the AS interface for this direction; NULL if not supported */

	usbh_uac_volume_info_t volume_info[1 + USBH_UAC_MAX_CHANNEL]; /**< Volume info per channel: [0]=master, [1..N]=per-channel */

	usbh_uac_buf_ctrl_t buf_ctrl;        /**< Isochronous buffer and packet-size control */
	u8 *xfer_buf;                          /**< DMA-safe transfer buffer passed to usbh_transfer_data */

	u8 pending_vol_valid;                /**< 1 when a set_volume call arrived while ctrl was busy */
	u8 pending_vol_value;                /**< Volume percentage (0-100) to apply when ctrl becomes idle */
	u8 pending_mute_valid;               /**< 1 when a set_mute call arrived while ctrl was busy */
	u8 pending_mute_value;               /**< Mute state (0 or 1) to apply when ctrl becomes idle */
} usbh_uac_channel_t;

/**
  * @brief  Main UAC Host Class Driver Handle.
  */
typedef struct {
	usbh_uac_ac_itf_info_t ac_isoc_desc;  /**< Audio Control interface topology (Feature Units + Terminals) */

	usbh_uac_channel_t isoc_out;          /**< OUT channel state (Playback/Speaker) */
	usbh_uac_channel_t isoc_in;           /**< IN channel state (Record/Microphone) */

	rtos_mutex_t alt_set_mutex;           /**< Serializes concurrent usbh_composite_uac_set_alt_setting calls */
	rtos_sema_t  ctrl_done_sema;          /**< Signaled by ctrl_setting when SET_ALT + SET_FREQ sequence completes */

	usbh_composite_uac_usr_cb_t *cb;      /**< User callback structure registered via usbh_composite_uac_init */
	usbh_composite_host_t *driver;        /**< Pointer to the parent USB Host Composite handle */

	u8 *audio_ctrl_buf;                   /**< Shared buffer for all USB control transfer payloads */
	u8 *ringbuf_partial_write_buf;        /**< Staging buffer that accumulates partial writes until a full packet is ready */

#if USBH_COMPOSITE_HID_UAC_DEBUG
	rtos_task_t dump_status_task;         /**< Handle for the periodic status-dump debug task */
	u32 sof_cnt;                          /**< SOF interrupt counter, incremented in cb_sof */
	u8 dump_status_task_alive;            /**< 1 while the dump task is running */
	u8 dump_status_task_exit;             /**< Set to 0 to signal the dump task to exit */
#endif

	__IO u8 xfer_state;        /**< Isochronous data transfer state (UAC_STATE_IDLE / UAC_STATE_TRANSFER) */
	__IO u8 ctrl_state;        /**< USB control transfer state machine state (UAC_STATE_CTRL_IDLE, SET_ALT, SET_FREQ, ...) */
	__IO u8 init_state;        /**< Initialization sequence state (UAC_INIT_OUT_GET_FU -> UAC_INIT_IN_GET_FU -> UAC_INIT_OUT_SET_ITF -> UAC_INIT_DONE) */

	u8 ch_idx;                 /**< Channel index driving the current iterative SET_VOLUME/SET_MUTE loop (0 = master) */
	u8 mute_value;             /**< Mute state (1: Mute, 0: Unmute) dispatched to process_set_ch_mute */
	u8 volume_value;           /**< Volume value dispatched to process_set_ch_volume */

	u8 cur_dir;                    /**< Direction being operated on by the current ctrl_state sequence */
	u8 ctrl_waiting;               /**< 1 when set_alt_setting is blocked on ctrl_done_sema */
} usbh_composite_uac_t;
/** @} End of Host_Composite_UAC_Types group*/
/** @} End of USB_Host_Types group*/

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/
extern const usbh_class_driver_t usbh_composite_uac_driver;  /**< Composite uac driver handle */

/* Exported functions --------------------------------------------------------*/

/** @addtogroup USB_Host_Functions USB Host Functions
 * @{
 */
/** @addtogroup Host_Composite_UAC_Functions Host Composite UAC Functions
 * @{
 */
/**
  * @brief  Initialize the UAC Class Driver.
  * @param  chost: Pointer to the composite host handle.
  * @param  cb: Pointer to the user callback structure.
  * @return 0 on success, non-zero on failure.
  */
int usbh_composite_uac_init(usbh_composite_host_t *chost, usbh_composite_uac_usr_cb_t *cb);

/**
  * @brief  De-initialize the UAC Class Driver and release resources.
  * @return 0 on success, non-zero on failure.
  */
int usbh_composite_uac_deinit(void);

/**
  * @brief  Get the current UAC mode (Playback only, Record only, or Both).
  * @return usbh_uac_mode_t indicating the current mode:
  *         - USBH_UAC_MODE_NONE: No UAC interface detected
  *         - USBH_UAC_MODE_PLAYBACK_ONLY: Playback only (Speaker)
  *         - USBH_UAC_MODE_RECORD_ONLY: Record only (Microphone)
  *         - USBH_UAC_MODE_PLAYBACK_AND_RECORD: Both Playback and Record
  */
usbh_uac_mode_t usbh_composite_uac_get_mode(void);

/**
  * @brief  Check if UAC supports playback (speaker output).
  * @return 1 if playback is supported, 0 otherwise.
  */
u8 usbh_composite_uac_support_playback(void);

/**
  * @brief  Check if UAC supports recording (microphone input).
  * @return 1 if recording is supported, 0 otherwise.
  */
u8 usbh_composite_uac_support_record(void);

/**
  * @brief  Get the volume information .
  *         Loop to find volume mute state, volume max min and cur value.
  * @param  host: Pointer to the USB core handle.
  * @return 0 on success, non-zero on failure.
  */
int usbh_composite_uac_get_volume_info(usb_host_t *host);

/**
  * @brief  Configure the UAC Audio Format.
  *         Finds the matching Alternate Setting and sends the SET_INTERFACE request.
  * @param  dir: Direction (0: Playback/OUT, 1: Record/IN).
  * @param  channels: Number of channels.
  * @param  bit_width: Bit width.
  * @param  sampling_freq: Sampling frequency in Hz.
  * @return 0 on success, non-zero on failure.
  */
int usbh_composite_uac_set_alt_setting(u8 dir, u8 channels, u8 bit_width, u32 sampling_freq);

/**
  * @brief  Get the list of audio formats supported by the connected device.
  * @param  dir: Direction (0: Playback/OUT, 1: Record/IN).
  * @param  fmt_cnt: Pointer to store the number of formats found.
  * @return Pointer to the array of supported formats.
  */
const usbh_audio_fmt_t *usbh_composite_uac_get_alt_setting(u8 dir, u8 *fmt_cnt);

/**
  * @brief  Calculate the frame size (in bytes) based on current configuration.
  * @param  dir: Direction.
  * @return Frame size in bytes.
  */
u32 usbh_composite_uac_get_frame_size(u8 dir);

/**
  * @brief  Set the volume level percentage.
  * @param  volume: Volume level (0-100).
  * @param  dir: Direction.
  * @return 0 on success, non-zero on failure.
  */
int usbh_composite_uac_set_volume(u8 volume, u8 dir);

/**
  * @brief  Set the mute state.
  * @param  mute: 1 to mute, 0 to unmute.
  * @param  dir: Direction.
  * @return 0 on success, non-zero on failure.
  */
int usbh_composite_uac_set_mute(u8 mute, u8 dir);

/**
  * @brief  Start the audio playback process.
  *         Enables SOF handling and prepares for data transmission.
  */
void usbh_composite_uac_start_play(void);

/**
  * @brief  Stop the audio playback process.
  */
void usbh_composite_uac_stop_play(void);

/**
  * @brief  Write audio data to the UAC driver ring buffer.
  * @param  buffer: Pointer to the PCM data buffer.
  * @param  size: Size of data to write (in bytes).
  * @param  timeout_ms: Timeout in milliseconds to wait if buffer is full.
  * @return Actual number of bytes written.
  */
u32 usbh_composite_uac_write(u8 *buffer, u32 size, u32 timeout_ms);

/**
  * @brief  Start the audio capture process (microphone recording).
  *         Enables SOF handling and prepares for data reception.
  */
void usbh_composite_uac_start_capture(void);

/**
  * @brief  Stop the audio capture process.
  */
void usbh_composite_uac_stop_capture(void);

/**
  * @brief  Read audio data from UAC IN (microphone)
  * @param  buffer: Pointer to the data buffer to store read data
  * @param  size: Size of data to read (in bytes)
  * @param  time_out_ms: Timeout in milliseconds to wait if no data available
  * @retval Actual number of bytes read
  */
u32 usbh_composite_uac_read(u8 *buffer, u32 size, u32 time_out_ms);

/** @} End of Host_Composite_UAC_Functions group */
/** @} End of USB_Host_Functions group */
/** @} End of USB_Host_API group */

#endif  /* USBH_COMPOSITE_UAC1_H */
