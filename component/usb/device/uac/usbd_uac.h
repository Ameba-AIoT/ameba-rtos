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

/** @addtogroup USB_Device_API USB Device API
 *  @{
 */

/* Exported defines ----------------------------------------------------------*/

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
#define USBD_UAC_PLAY_FILLER_THRESHOLD   3U

/**
 * @brief 2^(n) shorthand used across UAC MPS/ring/interval scaling.
 */
#define USBD_UAC_POW2(n)            (1U << (n))

#if USBD_UAC_DEBUG && (USBD_TP_TRACE_DEBUG == 0)
#error "Please set USBD_TP_TRACE_DEBUG in usbd.h"
#endif

/* Defaults applied when the matching @ref usbd_uac_config_t field is left 0. */
#define USBD_UAC_DEFAULT_PLAY_BUF_COUNT    20U    /**< Default playback ring buffer slot count. */
#define USBD_UAC_DEFAULT_RECORD_BUF_COUNT  20U    /**< Default record ring buffer slot count. */
#define USBD_UAC_RING_NODE_COUNT_MAX       255U   /**< Ring buffer node limit, the ring manager counts nodes in a u8. */
#ifdef CONFIG_SUPPORT_USB_FS_ONLY
#define USBD_UAC_BUFFER_COUNT_MAX          255U   /**< Largest play_buf_count / record_buf_count: full speed uses 1 node per slot. */
#else
#define USBD_UAC_BUFFER_COUNT_MAX          31U    /**< Largest UAC 2.0 play_buf_count / record_buf_count: high speed uses 8 nodes per slot. */
#endif
/* Volume range reported to the host. UAC encodes volume as a signed fixed-point dB value, so the
 * range wraps around zero: 0xFF42 is the minimum and 0x00BE the maximum. Both shall stay inside
 * what the class volume mapping table covers. */
#define USBD_UAC_DEFAULT_VOLUME_MIN  0xFF42U /**< Default minimum volume. */
#define USBD_UAC_DEFAULT_VOLUME_MAX  0x00BEU /**< Default maximum volume. */
#define USBD_UAC_DEFAULT_VOLUME      0x001FU /**< Default initial volume. */

/* Uac Endpoint parameters */
/* Audio channel count */
#define USBD_UAC_CHANNEL_COUNT_1    1U /**< Audio 1 channel count. */
#define USBD_UAC_CHANNEL_COUNT_2    2U /**< Audio 2 channel count. */
#define USBD_UAC_CHANNEL_COUNT_4    4U /**< Audio 4 channel count. */
#define USBD_UAC_CHANNEL_COUNT_6    6U /**< Audio 6 channel count. */
#define USBD_UAC_CHANNEL_COUNT_8    8U /**< Audio 8 channel count. */

/* Audio byte width */
#define USBD_UAC_BYTE_WIDTH_2       2U /**< Audio 2 bytes width. */
#define USBD_UAC_BYTE_WIDTH_3       3U /**< Audio 3 bytes width. */
/* amebasmart itself cannot play 32-bit audio data */
#define USBD_UAC_BYTE_WIDTH_4       4U /**< Audio 4 bytes width. */

/* Sampling frequency */
#define USBD_UAC_SAMPLING_FREQ_8K   8000U   /**< Audio 8000 sample frequency. */
#define USBD_UAC_SAMPLING_FREQ_16K  16000U  /**< Audio 16000 sample frequency. */
#define USBD_UAC_SAMPLING_FREQ_32K  32000U  /**< Audio 32000 sample frequency. */
#define USBD_UAC_SAMPLING_FREQ_44K  44100U  /**< Audio 44100 sample frequency. */
#define USBD_UAC_SAMPLING_FREQ_48K  48000U  /**< Audio 48000 sample frequency. */
#define USBD_UAC_SAMPLING_FREQ_96K  96000U  /**< Audio 96000 sample frequency. */
#define USBD_UAC_SAMPLING_FREQ_192K 192000U /**< Audio 192000 sample frequency. */

/* Stream formats selected in menuconfig (CONFIG USB > UAC). The descriptors declare them at build
 * time and the ISOC rings are sized for them once, so they are plain integers usable in #if. */

/* Playback: one alt setting per channel count from 2 up to this value */
#if defined(CONFIG_USBD_UAC_PLAY_CH_8)
#define USBD_UAC_PLAY_MAX_CHANNEL_COUNT  USBD_UAC_CHANNEL_COUNT_8 /**< Playback max channel count. */
#elif defined(CONFIG_USBD_UAC_PLAY_CH_6)
#define USBD_UAC_PLAY_MAX_CHANNEL_COUNT  USBD_UAC_CHANNEL_COUNT_6 /**< Playback max channel count. */
#elif defined(CONFIG_USBD_UAC_PLAY_CH_4)
#define USBD_UAC_PLAY_MAX_CHANNEL_COUNT  USBD_UAC_CHANNEL_COUNT_4 /**< Playback max channel count. */
#else
#define USBD_UAC_PLAY_MAX_CHANNEL_COUNT  USBD_UAC_CHANNEL_COUNT_2 /**< Playback max channel count. */
#endif

/* Playback byte width, shared by every alt setting */
#if defined(CONFIG_USBD_UAC_PLAY_BIT_32)
#define USBD_UAC_PLAY_BYTE_WIDTH         USBD_UAC_BYTE_WIDTH_4 /**< Playback byte width. */
#elif defined(CONFIG_USBD_UAC_PLAY_BIT_24)
#define USBD_UAC_PLAY_BYTE_WIDTH         USBD_UAC_BYTE_WIDTH_3 /**< Playback byte width. */
#else
#define USBD_UAC_PLAY_BYTE_WIDTH         USBD_UAC_BYTE_WIDTH_2 /**< Playback byte width. */
#endif

/* Playback highest sampling frequency, UAC 2.0 only: UAC 1.0 declares 44.1 and 48 kHz */
#if defined(CONFIG_USBD_UAC2) && defined(CONFIG_USBD_UAC_PLAY_FREQ_192K)
#define USBD_UAC_PLAY_MAX_SAMPLING_FREQ  USBD_UAC_SAMPLING_FREQ_192K /**< Playback max sampling frequency. */
#elif defined(CONFIG_USBD_UAC2) && defined(CONFIG_USBD_UAC_PLAY_FREQ_96K)
#define USBD_UAC_PLAY_MAX_SAMPLING_FREQ  USBD_UAC_SAMPLING_FREQ_96K /**< Playback max sampling frequency. */
#else
#define USBD_UAC_PLAY_MAX_SAMPLING_FREQ  USBD_UAC_SAMPLING_FREQ_48K /**< Playback max sampling frequency. */
#endif

#ifdef CONFIG_USBD_UAC_RECORD
/* Record (microphone): the single format the mic reports */
#if defined(CONFIG_USBD_UAC_RECORD_CH_1)
#define USBD_UAC_RECORD_CHANNEL_COUNT    USBD_UAC_CHANNEL_COUNT_1 /**< Record channel count. */
#else
#define USBD_UAC_RECORD_CHANNEL_COUNT    USBD_UAC_CHANNEL_COUNT_2 /**< Record channel count. */
#endif

#if defined(CONFIG_USBD_UAC_RECORD_BIT_32)
#define USBD_UAC_RECORD_BYTE_WIDTH       USBD_UAC_BYTE_WIDTH_4 /**< Record byte width. */
#elif defined(CONFIG_USBD_UAC_RECORD_BIT_24)
#define USBD_UAC_RECORD_BYTE_WIDTH       USBD_UAC_BYTE_WIDTH_3 /**< Record byte width. */
#else
#define USBD_UAC_RECORD_BYTE_WIDTH       USBD_UAC_BYTE_WIDTH_2 /**< Record byte width. */
#endif

#if defined(CONFIG_USBD_UAC_RECORD_FREQ_8K)
#define USBD_UAC_RECORD_SAMPLING_FREQ    USBD_UAC_SAMPLING_FREQ_8K /**< Record sampling frequency. */
#elif defined(CONFIG_USBD_UAC_RECORD_FREQ_32K)
#define USBD_UAC_RECORD_SAMPLING_FREQ    USBD_UAC_SAMPLING_FREQ_32K /**< Record sampling frequency. */
#elif defined(CONFIG_USBD_UAC_RECORD_FREQ_44P1K)
#define USBD_UAC_RECORD_SAMPLING_FREQ    USBD_UAC_SAMPLING_FREQ_44K /**< Record sampling frequency. */
#elif defined(CONFIG_USBD_UAC_RECORD_FREQ_48K)
#define USBD_UAC_RECORD_SAMPLING_FREQ    USBD_UAC_SAMPLING_FREQ_48K /**< Record sampling frequency. */
#elif defined(CONFIG_USBD_UAC_RECORD_FREQ_96K)
#define USBD_UAC_RECORD_SAMPLING_FREQ    USBD_UAC_SAMPLING_FREQ_96K /**< Record sampling frequency. */
#elif defined(CONFIG_USBD_UAC_RECORD_FREQ_192K)
#define USBD_UAC_RECORD_SAMPLING_FREQ    USBD_UAC_SAMPLING_FREQ_192K /**< Record sampling frequency. */
#else
#define USBD_UAC_RECORD_SAMPLING_FREQ    USBD_UAC_SAMPLING_FREQ_16K /**< Record sampling frequency. */
#endif
#endif /* CONFIG_USBD_UAC_RECORD */

/** @} End of Device_UAC_Constants group */
/** @} End of USB_Device_Constants group */

/* Exported types ------------------------------------------------------------*/

/** @addtogroup USB_Device_Types USB Device Types
 * @{
 */
/** @addtogroup Device_UAC_Types Device UAC Types
 * @{
 */

/**
 * @brief Configuration for the UAC class.
 * @details Carries the endpoint allocation, which the top-level application owns, the ISOC buffer
 *          sizing and the class-control ranges. Used by both standalone and composite modes.
 *          Device-global parameters (VID/PID, strings, power attributes) live in @ref usbd_dev_info_t instead.
 * @note  The structure is stored by pointer, not copied, so it shall remain valid until
 *        @ref usbd_uac_deinit.
 * @note  The stream formats are baked into the `const` descriptor tables at build time from the
 *        menuconfig formats: the playback ones the host selects from, refer to `USBD_UAC_PLAY_MAX_CHANNEL_COUNT`,
 *        `USBD_UAC_PLAY_BYTE_WIDTH` and `USBD_UAC_PLAY_MAX_SAMPLING_FREQ`, and with CONFIG_USBD_UAC_RECORD the
 *        single record one, refer to `USBD_UAC_RECORD_CHANNEL_COUNT`, `USBD_UAC_RECORD_BYTE_WIDTH` and
 *        `USBD_UAC_RECORD_SAMPLING_FREQ`. The host reports the playback format it selected through
 *        @ref usbd_uac_cb_t::format_changed.
 */
typedef struct {
	const char *itf_str;     /**< iInterface string of the audio control interface, NULL for none. */
	u16 volume_min;          /**< Minimum volume reported to the host, 0 uses @ref USBD_UAC_DEFAULT_VOLUME_MIN.
	                              Keep the range within the defaults: a SET_CUR outside them is acknowledged
	                              and ignored, the current volume is kept. */
	u16 volume_max;          /**< Maximum volume reported to the host, 0 uses @ref USBD_UAC_DEFAULT_VOLUME_MAX. */
	u16 volume_default;      /**< Initial volume, 0 uses @ref USBD_UAC_DEFAULT_VOLUME. */
	u8 mute_default;         /**< Initial mute state (0: unmuted, 1: muted). */
	u8 play_ep_addr;         /**< Playback ISOC OUT endpoint address. */
	u8 record_ep_addr;       /**< Record ISOC IN endpoint address, CONFIG_USBD_UAC_RECORD only. */
	u8 play_buf_count;       /**< Playback ring buffer slot count in ms, 0 uses @ref USBD_UAC_DEFAULT_PLAY_BUF_COUNT.
	                              UAC 2.0: at most @ref USBD_UAC_BUFFER_COUNT_MAX, init fails otherwise. */
	u8 record_buf_count;     /**< Record ring buffer slot count in ms, 0 uses @ref USBD_UAC_DEFAULT_RECORD_BUF_COUNT.
	                              CONFIG_USBD_UAC_RECORD only. UAC 2.0: at most @ref USBD_UAC_BUFFER_COUNT_MAX, init fails otherwise. */
} usbd_uac_config_t;

typedef struct {

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
	 * @param[in] req: Pointer to the setup request packet. For a device-to-host request with a
	 *            data stage, buf holds at least wLength bytes and wLength never exceeds the
	 *            host's: write at most wLength bytes.
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
	 * @param[in] channel_count: New channel count. such as 2,4,6,8...
	 * @param[in] byte_width: New byte width, such as 1,2,3,4.
	 */
	void (*format_changed)(u32 sampling_freq, u8 channel_count, u8 byte_width);

	/**
	 * @brief Called when the SOF interrupt occurs in the UAC class driver.
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 */

	void (*sof)(void);
} usbd_uac_cb_t;

/* One ISOC stream, playback (ISOC OUT) or record (ISOC IN): its ring of packets, the DMA buffer of
   the packet in flight and the gates between the ISR and the task APIs */
typedef struct {
	u8 *dma_buf;                 /**< DMA buffer of one packet, allocated by usbd_uac_init(). */
	rtos_sema_t sema;            /**< Wakes the reader blocked in usbd_uac_read(). */

	/*
	 * Underrun filler: sof_count counts SOF frames while streaming is on, data_count is bumped
	 * by every real packet and by every appended filler. When sof_count - data_count >=
	 * USBD_UAC_PLAY_FILLER_THRESHOLD a filler node is synthesised so consumers do not stall.
	 */
	__IO u32 sof_count;
	__IO u32 data_count;
	__IO u32 packet_count;       /**< Real packets received. */

#if USBD_UAC_DEBUG
	__IO u32 filler_count;           /**< Filler nodes inserted from the SOF path. */
	__IO u32 overwrite_count;        /**< OUT ISR forced head-drop due to full ring. */
	__IO u32 filler_overwrite_count; /**< SOF filler forced head-drop due to full ring. */
	__IO u32 timeout_count;          /**< Inter-packet gap exceeded expected step. */
	u32 last_packet_time_us;         /**< Timestamp of the previous packet, for gap check. */
#endif

	usb_ringbuf_manager_t ring;  /**< Ring of ISOC packets. */

	u16 last_packet_len;         /**< Last packet length, used to size an appended filler. */
	u16 max_packet_size;         /**< Ring node size, the MPS of the stream. */

	u8 ring_node_count;          /**< Ring node count. */
	u8 binterval;                /**< bInterval the ring is paced at. */
	u8 sema_valid;               /**< sema created. */
	__IO u8 read_wait_sema;      /**< The reader waits on sema. */
	__IO u8 streaming;           /**< Stream started, refer to usbd_uac_start_play() / start_record(). */
	__IO u8 is_closing;          /**< Set by the stream teardown: usbd_uac_read() (playback) or
	                                  usbd_uac_transmit_data() (record) rejects new callers.
	                                  Written under the USB critical section. */
	__IO u8 user_count;          /**< Number of tasks inside those APIs on this stream; the teardown
	                                  waits for it to drop to 0. Updated under the USB critical section. */
} usbd_uac_stream_t;

/* Record packetizer: splits the PCM stream the application submits into one packet per bInterval
   of the nominal size, refer to usbd_uac_transmit_data() */
typedef struct {
	u8 *buf;            /**< One packet being assembled, max_packet_size bytes. */
	u32 rate_rem;       /**< sampling_freq % packets_per_second. */
	u32 acc;            /**< Fractional frame accumulator, in 1/packets_per_second frames. */
	u16 packets_per_second; /**< Refer to usbd_uac_get_packets_per_second(). */
	u16 base_len;       /**< Nominal packet length in bytes, without the fractional frame. */
	u16 fill;           /**< Bytes already in buf. */
	u16 target;         /**< Length of the packet being assembled. */
	u8 frame_len;       /**< Bytes per audio frame: channel_count * byte_width. */
} usbd_uac_record_packetizer_t;

/** @} End of Device_UAC_Types group */
/** @} End of USB_Device_Types group */

/**
 * @brief Structure representing the UAC device instance.
 */
typedef struct {
	const usbd_uac_config_t *cfg;      /**< Pointer to the class configuration (set by init). */
	const usbd_uac_cb_t *cb;           /**< Pointer to the user-defined callback structure. */
	usb_dev_t *dev;                    /**< Pointer to the USB device instance. */

	/* Class and EP0 state */
	u8 ctrl_req_pending;               /**< 1 if ctrl_req is waiting for its EP0 OUT data stage. */
	u8 from_composite;                 /**< Flag indicating if part of a composite device. */
	u8 if_base;                        /**< First interface number of this class; 0 in standalone mode
	                                        unless the composite framework rebases it via
	                                        set_interface_base(). */
	u8 interface_string_index;         /**< Audio control interface iInterface string index from
	                                        usbd_add_string(), 0 for none. */
	__IO u8 is_configured;             /**< 1 from a successful set_config() to clear_config(), written under
	                                        the USB critical section. The task APIs arm an EP only while it is
	                                        set, tested under the same lock. */
	u8 sof_count;                      /**< SOFs since the last packet period, UAC 1.0 at High Speed only. */
	usb_setup_req_t ctrl_req;          /**< Stores the current control request. */

	/* Playback (ISOC OUT) */
	u32 play_sampling_freq;            /**< Current playback (headphone) sample frequency. */
	u16 play_volume;                   /**< Current playback volume. */
	u8 play_byte_width;                /**< Current playback byte width. */
	u8 play_channel_count;             /**< Current playback channel count. */
	u8 play_alt_setting;               /**< Current alternate setting of the headphone AS interface. */
	u8 play_mute;                      /**< Current playback mute value. */
	usbd_uac_stream_t play_stream;     /**< Playback stream. */
	usbd_ep_t play_ep;                 /**< ISOC OUT endpoint. */

	/* Record (ISOC IN) */
	u32 record_sampling_freq;          /**< Current record (microphone) sample frequency. */
	u16 record_volume;                 /**< Current record volume. */
	u8 record_alt_setting;             /**< Current alternate setting of the microphone AS interface. */
	u8 record_mute;                    /**< Current record mute value. */
#ifdef CONFIG_USBD_UAC_RECORD
	usbd_uac_record_packetizer_t record_packetizer; /**< Cuts the record PCM into packets. */
#endif
	usbd_uac_stream_t record_stream;   /**< Record stream. */
	usbd_ep_t record_ep;               /**< ISOC IN endpoint. */

#if USBD_UAC_DEBUG
	u32 read_data_len;                 /**< Total bytes read by usbd_uac_read(). */
	__IO u8 dump_thread_running;       /**< Audio dump thread running flag. */
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
 * @brief Initializes class driver as a standalone device.
 * @param[in] cb: Pointer to the user-defined callback structure.
 * @param[in] cfg: Pointer to endpoint address configuration, or NULL for defaults.
 * @return 0 on success, non-zero on failure.
 */
int usbd_uac_init(const usbd_uac_cb_t *cb, const usbd_uac_config_t *cfg);

#ifdef CONFIG_USBD_COMPOSITE
/**
 * @brief Initializes class driver as part of a composite device.
 * @param[in] cb: Pointer to the user-defined callback structure.
 * @param[in] cfg: Pointer to endpoint address configuration, or NULL for defaults.
 * @return 0 on success, non-zero on failure.
 */
int usbd_composite_uac_init(const usbd_uac_cb_t *cb, const usbd_uac_config_t *cfg);
#endif

/**
  * @brief  DeInitialize UAC device
  * @note   Call it in task context, without holding the USB critical section, and never from a
  *         task that calls usbd_uac_read() or usbd_uac_transmit_data(): it waits, without a bound,
  *         for every task inside them to leave before the ring buffers are released, so such a
  *         task calling it would wait for itself. Stop them first (usbd_uac_stop_play() /
  *         usbd_uac_stop_record() or an application lock held across the loop), as the examples
  *         do. Never delete a task while it is inside usbd_uac_read() or usbd_uac_transmit_data():
  *         it would never leave, and this call would wait forever, reporting it once per second.
  *         Same rule for usbd_uac_start_play() and usbd_uac_start_record().
  * @note  Composite mode: call it after @ref usbd_composite_deinit. Before that the class is
  *        still reachable from the USB ISR, so the call returns at once and releases nothing.
  * @return None. This is a teardown path: apart from the composite case above, the class is always
  *         unregistered and every resource it owns released, nothing for the caller to recover from.
  */
void usbd_uac_deinit(void);

/**
  * @brief  Start the playback (ISOC OUT) stream
  * @details Sizes the playback ring for the connection speed and arms the ISOC OUT endpoint. The ring
  *          fits every format the descriptors declare, so it is allocated by the first call after
  *          enumeration and only reallocated when the speed changed: a format switch requested by
  *          the host never reallocates it. Call it again after each @ref usbd_uac_cb_t::format_changed.
  * @note   Task context only, same rules as @ref usbd_uac_deinit.
  * @retval 0 on success, -RTK_ERR_IO before the host configured the device, else non-zero on failure.
  */
int usbd_uac_start_play(void);

/**
  * @brief  USB audio stop play
  */
void usbd_uac_stop_play(void);

/**
  * @brief  Start the record (ISOC IN) stream
  * @details Sizes the record ring and packetizer for the connection speed, then lets
  *          @ref usbd_uac_transmit_data queue data. The ring is only reallocated when the speed changed.
  * @note   Task context only, same rules as @ref usbd_uac_deinit.
  * @retval 0 on success, -RTK_ERR_IO before the host configured the device or without
  *         CONFIG_USBD_UAC_RECORD, else non-zero on failure.
  */
int usbd_uac_start_record(void);

/**
  * @brief  USB audio stop record
  */
void usbd_uac_stop_record(void);

/**
  * @brief  Queue record (ISOC IN) PCM data for transmission to the host.
  * @details The stream is cut into one packet per bInterval of the nominal size for the record
  *          format: per ms for UAC 1.0, per microframe for UAC 2.0 at High Speed. The class adds
  *          one audio frame now and then when the rate is not a multiple of the packet rate
  *          (e.g. 44.1kHz), ref UAC 2.0 2.3.1.1. Any length is accepted, typically 1ms of audio per
  *          call: nothing is truncated, and a tail shorter than a packet is kept for the next call.
  *          When the ring is full the oldest packet is dropped. Without CONFIG_USBD_UAC_RECORD it
  *          returns -RTK_ERR_IO.
  * @note   Task context only, from a single producer task, same rules as @ref usbd_uac_deinit.
  *         Pass whole audio frames (channel_count * byte_width bytes) so that packets stay frame aligned.
  * @param[in] buf: pointer to the PCM data.
  * @param[in] len: length of the data in bytes.
  * @retval 0 on success, non-zero on failure.
  */
int usbd_uac_transmit_data(const u8 *buf, u32 len);

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

#endif /* USBD_UAC_H */
