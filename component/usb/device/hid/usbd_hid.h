/*
 * Copyright (c) 2025 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBD_HID_H
#define USBD_HID_H

/* Includes ------------------------------------------------------------------*/

#include "usbd.h"

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
/** @addtogroup Device_HID_Constants Device HID Constants
 * @{
 */

/* Defaults applied when the matching @ref usbd_hid_config_t field is left 0. */
#define USBD_HID_INTR_IN_BUF_SIZE            512U    /**< Default INTR IN transfer buffer size. */
#define USBD_HID_INTR_OUT_BUF_SIZE           64U     /**< Default INTR OUT transfer buffer size, one max packet: holds the keyboard LED report or one vendor Output report. */

/* Default INTR bInterval. Full Speed counts frames (1 ms each), High Speed counts
 * 2^(value-1) microframes, so the same polling rate needs different numbers: FS 10 -> 10 ms for
 * mouse/keyboard/headset, HS 4 -> 8 microframes -> 1 ms. The vendor variant polls every frame. */
#ifdef CONFIG_USBD_HID_VENDOR
#define USBD_HID_DEFAULT_FS_INTR_INTERVAL    1U      /**< Default INTR bInterval at Full Speed. */
#else
#define USBD_HID_DEFAULT_FS_INTR_INTERVAL    10U     /**< Default INTR bInterval at Full Speed. */
#endif
#define USBD_HID_DEFAULT_HS_INTR_INTERVAL    4U      /**< Default INTR bInterval at High Speed. */

/* Endpoint address */
#define USBD_HID_HS_INT_MAX_PACKET_SIZE      64 /**< High speed INTR maximum packet size */
#define USBD_HID_FS_INT_MAX_PACKET_SIZE      64 /**< Full speed INTR maximum packet size */

#define USBD_HID_DESC_SIZE                   9       /**< HID descriptor size. */
#define USBD_HID_DESC_ITEM_LENGTH_OFFSET     7       /**< Offset of report desc wItemLength in the HID descriptor. */
/* cfg header (9) + if (9) + hid header (7) = 25 */
#define USBD_HID_CFG_DESC_ITEM_LENGTH_OFFSET 25      /**< Offset of report desc wItemLength in the config descriptor. */

#ifdef CONFIG_USBD_HID_VENDOR
/* Vendor variant: one interface on the Vendor Defined usage page 0xFF00, so the host OS leaves it
 * to applications such as hidapi based HID tools. Ref HID 1.11 5.6 and 8.1: the report descriptor
 * declares a Report ID, so every report on the interface starts with it, and every report has the
 * fixed size declared, Report ID included. A customer protocol adds its own Report IDs and lengths
 * to the report descriptor, usbd_hid_send_data() and the received callback carry whole reports. */
#define USBD_HID_VENDOR_IN_REPORT_ID         0x01U   /**< Report ID of the vendor Input report (device to host). */
#define USBD_HID_VENDOR_OUT_REPORT_ID        0x02U   /**< Report ID of the vendor Output report (host to device). */
#define USBD_HID_VENDOR_REPORT_PAYLOAD_LEN   63U     /**< Payload length of each vendor report, Report ID excluded. */
#define USBD_HID_VENDOR_REPORT_LEN           ((USBD_HID_VENDOR_REPORT_PAYLOAD_LEN) + 1U) /**< Whole vendor report length, Report ID included: one FS packet. */
#endif

#ifdef CONFIG_USBD_HID_HEADSET
/* Headset variant: the button interface of a USB Type-C headset, Consumer Control page, Ref
 * Android USB headset specification. The Input report is one byte without Report ID, each bit
 * below is 1 while the button is held: send the pressed bits, then 0 on release. A headset that
 * lacks a button just never sets its bit. */
#define USBD_HID_HEADSET_KEY_PLAY_PAUSE      0x01U   /**< Play/Pause (0xCD), Android button A, also hook / answer call. */
#define USBD_HID_HEADSET_KEY_VOLUME_UP       0x02U   /**< Volume Increment (0xE9), Android button B. */
#define USBD_HID_HEADSET_KEY_VOLUME_DOWN     0x04U   /**< Volume Decrement (0xEA), Android button C. */
#define USBD_HID_HEADSET_KEY_VOICE_COMMAND   0x08U   /**< Voice Command (0xCF), Android button D, launches the voice assistant. */
#define USBD_HID_HEADSET_KEY_MUTE            0x10U   /**< Mute (0xE2), toggles the host mute. */
#define USBD_HID_HEADSET_REPORT_LEN          1U      /**< Headset Input report length. */
#endif

/* Mouse button definition  */
#define USBD_HID_MOUSE_BUTTON_LEFT           0x01    /**< Mouse Left button. 0: release, 1: press. */
#define USBD_HID_MOUSE_BUTTON_RIGHT          0x02    /**< Mouse Right button. 0: release, 1: press. */
#define USBD_HID_MOUSE_BUTTON_MIDDLE         0x04    /**< Mouse Wheel button. 0: release, 1: press. */
#define USBD_HID_MOUSE_BUTTON_RESERVED       0xf8    /**< Mouse Reserved. all bit should set to 1. */

/** @} End of Device_HID_Constants group */
/** @} End of USB_Device_Constants group */

/* Exported types ------------------------------------------------------------*/

/** @addtogroup USB_Device_Types USB Device Types
 * @{
 */
/** @addtogroup Device_HID_Types Device HID Types
 * @{
 */

/**
 * @brief HID endpoint configuration structure.
 * @details Allows the user to override default endpoint addresses and buffer sizes.
 *          Keyboard variant needs INTR OUT (LED report), vendor variant needs INTR OUT
 *          (vendor Output report). Mouse and headset variants use INTR IN only.
 */
typedef struct {
	u32 intr_in_xfer_size;      /**< INTR IN transfer buffer size, 0 uses @ref USBD_HID_INTR_IN_BUF_SIZE. */
	u32 intr_out_xfer_size;     /**< INTR OUT transfer buffer size, 0 uses @ref USBD_HID_INTR_OUT_BUF_SIZE.
	                                 Ignored unless the keyboard or vendor variant is built. */
	const char *itf_str;        /**< iInterface string of the HID interface, NULL for none. */
	u8  intr_in_addr;           /**< INTR IN endpoint address. */
	/**
	 * @brief INTR OUT endpoint address.
	 * @details Ignored unless `CONFIG_USBD_HID_KEYBOARD` (LED report) or
	 *          `CONFIG_USBD_HID_VENDOR` (vendor Output report) is built.
	 */
	u8  intr_out_addr;
	u8  intr_interval_hs;       /**< INTR bInterval at High Speed, 0 uses @ref USBD_HID_DEFAULT_HS_INTR_INTERVAL. */
	u8  intr_interval_fs;       /**< INTR bInterval at Full Speed, 0 uses @ref USBD_HID_DEFAULT_FS_INTR_INTERVAL. */
} usbd_hid_config_t;

/**
 * @brief Structure containing callback functions for the HID class.
 * @details The user application should provide an instance of this structure
 *          to handle class-specific events.
 */
typedef struct {
	/**
	 * @brief Called when the HID class driver initialization for application resource setup.
	 */
	void (*init)(void);

	/**
	 * @brief Called when the HID device is de-initialized for resource cleanup.
	 */
	void (*deinit)(void);

	/**
	 * @brief Called to handle class-specific SETUP requests.
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 */
	void (*setup)(void);

	/**
	 * @brief Called when a data transmission to the host on the BULK IN endpoint is complete.
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 * @param[in] status The status of the transmission.
	 */
	void (*transmitted)(int status);

#if defined(CONFIG_USBD_HID_KEYBOARD) || defined(CONFIG_USBD_HID_VENDOR)
	/**
	 * @brief Called when new data is received from the host on the INTR OUT endpoint (or via SET_REPORT on EP0).
	 *        Keyboard: LED-state report; Vendor: the whole Output report, Report ID first. The buffer
	 *        is re-armed for the next report once this returns, copy what is needed.
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 * @param[in] buf Pointer to the received data buffer.
	 * @param[in] len Length of the received data in bytes.
	 */
	void (*received)(u8 *buf, u32 len);
#endif

	/**
	 * @brief Called when USB attach status changes for application to support hot-plug events.
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 * @param[in] old_status The previous attach status.
	 * @param[in] status The new attach status.
	 */
	void (*status_changed)(u8 old_status, u8 status);
} usbd_hid_usr_cb_t;

/** @} End of Device_HID_Types group */
/** @} End of USB_Device_Types group */

/**
 * @brief Structure representing the HID device instance.
 */
typedef struct {
	const usbd_hid_config_t *cfg;       /**< Pointer to the class configuration (set by init). */
	const usbd_hid_usr_cb_t *cb;        /**< Pointer to the user-defined callback structure. */
	usb_dev_t *dev;               /**< Pointer to the USB device instance. */
	usbd_ep_t ep_intr_in;         /**< INTR IN endpoint structure. */
#if defined(CONFIG_USBD_HID_KEYBOARD) || defined(CONFIG_USBD_HID_VENDOR)
	usbd_ep_t ep_intr_out;        /**< INTR OUT endpoint structure. */
#endif
	usb_setup_req_t ctrl_req;     /**< Stores the current control request (byte-packed). */
	u8 protocol;                  /**< Store the device protocol value. */
	u8 idle_rate;                 /**< Store the device idle rate. */
	u8 ctrl_req_pending;          /**< 1 if ctrl_req is waiting for its EP0 OUT data stage. */
	u8 from_composite;			/**< Flag indicating if part of a composite device. */
	u8 itf_str_idx;				/**< iInterface string index from usbd_add_string(), 0 for none. */
} usbd_hid_t;

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @addtogroup USB_Device_Functions USB Device Functions
 * @{
 */
/** @addtogroup Device_HID_Functions Device HID Functions
 * @{
 */

/**
 * @brief Initializes class driver as a standalone device.
 * @param[in] cb: Pointer to the user-defined callback structure.
 * @param[in] cfg: Pointer to the class configuration. See @ref usbd_hid_config_t.
 * @return 0 on success, non-zero on failure.
 */
int usbd_hid_init(const usbd_hid_usr_cb_t *cb, const usbd_hid_config_t *cfg);

#ifdef CONFIG_USBD_COMPOSITE
/**
 * @brief Initializes class driver as part of a composite device.
 * @param[in] cb: Pointer to the user-defined callback structure.
 * @param[in] cfg: Pointer to the class configuration. See @ref usbd_hid_config_t.
 * @return 0 on success, non-zero on failure.
 */
int usbd_composite_hid_init(const usbd_hid_usr_cb_t *cb, const usbd_hid_config_t *cfg);
#endif

/**
  * @brief  DeInitialize HID device
  * @note  Call it in task context, once no other task is inside usbd_hid_send_data(): the TX
  *        buffer it writes to is released here.
  * @note  Composite mode: call it after @ref usbd_composite_deinit. Before that the class is
  *        still reachable from the USB ISR, so the call returns at once and releases nothing.
  * @return None. This is a teardown path: apart from the composite case above, the class is always
  *         unregistered and every resource it owns released, nothing for the caller to recover from.
  */
void usbd_hid_deinit(void);

/**
 * @brief Transmits data to the host over the INTR IN endpoint.
 * @details The data is sent as given, so it shall be one whole Input report: the vendor variant
 *          puts @ref USBD_HID_VENDOR_IN_REPORT_ID first and sends @ref USBD_HID_VENDOR_REPORT_LEN
 *          bytes, the headset variant sends the @ref USBD_HID_HEADSET_REPORT_LEN byte key bitmap.
 * @param[in] buf: Pointer to the data buffer to be transmitted.
 * @param[in] len: Length of the data in bytes.
 * @return 0 on success, non-zero on failure.
 */
int usbd_hid_send_data(const u8 *buf, u32 len);

/** @} End of Device_HID_Functions group */
/** @} End of USB_Device_Functions group */

/** @} End of USB_Device_API group */

#ifdef __cplusplus
}
#endif

#endif /* USBD_HID_H */
