/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBD_UAC_PRIV_H
#define USBD_UAC_PRIV_H

/* Includes ------------------------------------------------------------------*/

#include "usbd_uac.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup USB_Device_API USB Device API
 *  @{
 */

/* Exported defines ----------------------------------------------------------*/

/** @addtogroup USB_Device_Constants USB Device Constants
 *  @{
 */
/** @defgroup Device_UAC_Priv_Constants Device UAC Private Constants
 *  @brief Constants shared between the UAC common part and the UAC 1.0 / 2.0 drivers.
 *  @{
 */

#define USBD_UAC_VOLUME_ERR_VAL      255U   /**< usbd_uac_volume_linear_interpolation() result for a volume out of the table. */
#define USBD_UAC_HS_ISOC_MPS         1024U  /**< High speed ISOC IN & OUT maximum packet size. */
#define USBD_UAC_FS_ISOC_MPS         1023U  /**< Full speed ISOC IN & OUT maximum packet size. */
#define USBD_UAC_ONE_KHZ             1000U  /**< Full speed frames, and milliseconds, per second. */

#ifdef CONFIG_USBD_UAC_RECORD
/* The mic AS interface descriptor only defines alt setting 0 (zero-bandwidth) and 1 (streaming) */
#define USBD_UAC_AS_MIC_ALT_SETTING_MAX  0x01U
/* Spatial location of the mic cluster, ref UAC 1.0 3.7.2.3 / UAC 2.0 4.1: a mono cluster is the
 * Front Center channel, a stereo one Front Left and Front Right */
#define USBD_UAC_RECORD_CHANNEL_CONFIG   ((USBD_UAC_RECORD_CHANNEL_COUNT == USBD_UAC_CHANNEL_COUNT_1) ? 0x0004U : 0x0003U)
/* Full-Speed record packet: one ms of audio plus one frame, the same for UAC 1.0 and UAC 2.0 */
#define USBD_UAC_RECORD_FS_MPS \
    (USBD_UAC_RECORD_CHANNEL_COUNT * USBD_UAC_RECORD_BYTE_WIDTH * ((USBD_UAC_RECORD_SAMPLING_FREQ / USBD_UAC_ONE_KHZ) + 1U))
/* Ref USB 2.0 5.6.3: a Full-Speed ISOC packet is at most 1023 bytes. A High-Speed capable device shall
 * also work at Full Speed, and a packet that fits Full Speed fits High Speed too */
#if USBD_UAC_RECORD_FS_MPS > USBD_UAC_FS_ISOC_MPS
#error "UAC: the record format exceeds one Full-Speed ISOC packet (1023B per ms). Lower the record rate, channels or bit width"
#endif
#endif

/** @} End of Device_UAC_Priv_Constants group */
/** @} End of USB_Device_Constants group */

/* Exported types ------------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/** @addtogroup USB_Device_Variables USB Device Variables
 *  @{
 */
/** @defgroup Device_UAC_Priv_Variables Device UAC Private Variables
 *  @brief Variables shared between the UAC common part and the UAC 1.0 / 2.0 drivers.
 *  @{
 */

/** @brief The singleton UAC device, one UAC version is built per image. */
extern usbd_uac_dev_t usbd_uac_dev;

/** @brief The class driver of the UAC version built, defined by usbd_uac1.c / usbd_uac2.c. */
extern const usbd_class_driver_t usbd_uac_driver;


/** @} End of Device_UAC_Priv_Variables group */
/** @} End of USB_Device_Variables group */

/* Exported functions --------------------------------------------------------*/

/** @addtogroup USB_Device_Functions USB Device Functions
 *  @{
 */
/** @defgroup Device_UAC_Priv_Functions Device UAC Private Functions
 *  @brief Implemented by usbd_uac_common.c, except the version hooks implemented by usbd_uac1.c / usbd_uac2.c.
 *  @{
 */

/* Version hooks, implemented by usbd_uac1.c / usbd_uac2.c ------------------*/

/**
  * @brief  Version hook: validate the class configuration and set up the version specific state
  * @note   Called by usbd_uac_init() / usbd_composite_uac_init() after the common checks.
  * @param  cfg: Class configuration
  * @retval Status
  */
int usbd_uac_version_init(const usbd_uac_config_t *cfg);

/**
  * @brief  Version hook: arm the ISOC OUT endpoint and start the playback stream
  * @retval Status
  */
int usbd_uac_play_ep_arm(void);

/**
  * @brief  Version hook: ISOC packets per second, the packet period being one bInterval
  * @param  speed: USB connection speed
  * @retval Packets per second
  */
u16 usbd_uac_get_packets_per_second(u8 speed);

/**
  * @brief  Version hook: ISOC bInterval of the streaming endpoints at a speed
  * @param  speed: USB connection speed
  * @retval bInterval
  */
u8 usbd_uac_get_binterval(u8 speed);

#ifdef CONFIG_USBD_UAC_RECORD
/**
  * @brief  Version hook: max packet size of the record stream, its nominal packet plus one frame
  * @param  speed: USB connection speed
  * @retval Max packet size
  */
u16 usbd_uac_get_record_max_packet_size(u8 speed);
#endif

/**
  * @brief  Version hook: size of a playback ring node, the MPS of the widest declared alt setting
  * @param  speed: USB connection speed
  * @retval Node size in bytes
  */
u16 usbd_uac_get_play_max_packet_size(u8 speed);

/**
  * @brief  Version hook: check a playback sampling frequency against the declared ones
  * @param  freq: Sampling frequency in Hz
  * @param  speed: USB connection speed
  * @retval true if declared at that speed
  */
bool usbd_uac_is_valid_sample_rate(u32 freq, u8 speed);

/* Common functions, implemented by usbd_uac_common.c -----------------------*/

/**
  * @brief  Push a node to a ring buffer, dropping the oldest one if it is full
  * @param  stream: Buffer control structure of the ring
  * @param  buf: Node payload, NULL for a filler node
  * @param  len: Node length
  * @param  valid: 1 for real data, 0 for a filler node
  * @retval 1 if the oldest node was dropped, 0 otherwise
  */
u8 usbd_uac_ring_push(usbd_uac_stream_t *stream, const u8 *buf, u32 len, u8 valid);

/**
  * @brief  Append a filler node to the OUT ring buffer when the host stops sending data
  * @note   ISR context (SOF path).
  * @param  stream: Playback stream
  */
void usbd_uac_append_filler(usbd_uac_stream_t *stream);

/**
  * @brief  Release a ring buffer, waiting for every task still using it
  * @note   Task context only.
  * @param  stream: Stream to release
  */
void usbd_uac_stream_deinit(usbd_uac_stream_t *stream);

/**
  * @brief  ISOC OUT data received, class driver ep_data_out callback
  * @note   ISR context.
  * @param  dev: USB device instance
  * @param  ep_addr: Endpoint address
  * @param  len: Received length
  * @retval Status
  */
int usbd_uac_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u32 len);

/**
  * @brief  USB attach status change, class driver status_changed callback
  * @note   ISR context.
  * @param  dev: USB device instance
  * @param  old_status: Previous attach status
  * @param  status: New attach status
  */
void usbd_uac_status_changed(usb_dev_t *dev, u8 old_status, u8 status);

#ifdef CONFIG_USBD_UAC_RECORD
/**
  * @brief  Open the ISOC IN endpoint on entering the mic streaming setting
  * @note   ISR context (SET_INTERFACE).
  * @param  dev: USB device instance
  * @retval Status
  */
int usbd_uac_isoc_in_ep_open(usb_dev_t *dev);

/**
  * @brief  Submit the next record packet if the IN endpoint is idle
  * @note   ISR or task context.
  * @param  dev: USB device instance
  */
void usbd_uac_isoc_in_transmit_next(usb_dev_t *dev);
#endif

#ifdef CONFIG_USBD_UAC_PLAY_FEEDBACK
/**
  * @brief  Version hook: bInterval of the playback feedback endpoint, one feedback per ms
  * @param  speed: USB connection speed
  * @retval bInterval
  */
u8 usbd_uac_get_play_feedback_binterval(u8 speed);

/**
  * @brief  Open the feedback ISOC IN endpoint on entering a playback streaming setting
  * @note   ISR context (SET_INTERFACE).
  * @param  dev: USB device instance
  * @retval Status
  */
int usbd_uac_play_feedback_open(usb_dev_t *dev);

/**
  * @brief  Close the feedback ISOC IN endpoint
  * @note   ISR context, or under the USB critical section.
  * @param  dev: USB device instance
  */
void usbd_uac_play_feedback_close(usb_dev_t *dev);

/**
  * @brief  Submit the next feedback packet if the feedback endpoint is idle
  * @note   ISR context (SOF of every ms).
  * @param  dev: USB device instance
  */
void usbd_uac_play_feedback_transmit(usb_dev_t *dev);
#endif

#if defined(CONFIG_USBD_UAC_RECORD) || defined(CONFIG_USBD_UAC_PLAY_FEEDBACK)
/**
  * @brief  ISOC IN transfer done, class driver ep_data_in callback
  * @note   ISR context.
  * @param  dev: USB device instance
  * @param  ep_addr: Endpoint address
  * @param  status: Transfer status
  * @retval Status
  */
int usbd_uac_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, int status);
#endif

/**
  * @brief  Convert a 16-bit little-endian control word to a signed value
  * @param  buf: The two received bytes
  * @retval Signed volume
  */
s16 usbd_uac_le16_to_s16(const u8 *buf);

/**
  * @brief  Map a volume of the UAC driver to the volume level the host intends, 0 to 100
  * @param  target_y: Volume of the UAC driver, in 1/256 dB
  * @retval Volume level, or USBD_UAC_VOLUME_ERR_VAL out of the mapping table
  */
u8 usbd_uac_volume_linear_interpolation(s16 target_y);

/**
  * @brief  Check the number of an application supplied endpoint address
  * @param  ep_addr: Endpoint address, direction bit included
  * @retval 1 for a non-control endpoint the controller has, else 0
  */
u8 usbd_uac_is_valid_ep(u8 ep_addr);

/**
  * @brief  Accept the data stage of an H2D class or vendor request
  * @param  dev: USB device instance
  * @param  req: USB request
  * @param  expected_length: Size of the parameter block of the addressed control
  * @retval Status
  */
int usbd_uac_receive_control_data(usb_dev_t *dev, const usb_setup_req_t *req, u16 expected_length);

/**
  * @brief  Wake the reader blocked in usbd_uac_read() once the OUT ring holds a node
  * @note   ISR context.
  * @param  stream: Playback stream
  */
void usbd_uac_wake_reader(usbd_uac_stream_t *stream);

/** @} End of Device_UAC_Priv_Functions group */
/** @} End of USB_Device_Functions group */

/** @} End of USB_Device_API group */

#ifdef __cplusplus
}
#endif

#endif /* USBD_UAC_PRIV_H */
