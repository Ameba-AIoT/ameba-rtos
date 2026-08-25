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

/* Exported defines ----------------------------------------------------------*/

/** @addtogroup USB_Device_API USB Device API
 *  @{
 */
/** @addtogroup USB_Device_Constants USB Device Constants
 * @{
 */
/** @addtogroup Device_HID_Constants Device HID Constants
 * @{
 */

/* HID PID/VID */
#define USBD_HID_VID                         USB_VID  /**< Vendor ID. */
#define USBD_HID_PID                         USB_PID  /**< Product ID. */

/* HID strings */
#define USBD_HID_LANGID_STRING               0x0409U             /**< Language ID for string descriptors (0x0409 = English) */
#define USBD_HID_MFG_STRING                  "Realtek"           /**< Manufacturer string. */
#define USBD_HID_PROD_HS_STRING              "Realtek HID (HS)"  /**< Product string for High-Speed mode. */
#define USBD_HID_PROD_FS_STRING              "Realtek HID (FS)"  /**< Product string for Full-Speed mode. */
#define USBD_HID_SN_STRING                   "1234567890"        /**< Serial number string. */

#define USBD_HID_INTR_IN_BUF_SIZE            512U    /**< INTR IN maximum buffer size */
#define USBD_HID_INTR_OUT_BUF_SIZE           1024U   /**< INTR OUT maximum buffer size */

/* Endpoint address */
#define USBD_HID_HS_INT_MAX_PACKET_SIZE      64 /**< High speed INTR maximum packet size */
#define USBD_HID_FS_INT_MAX_PACKET_SIZE      64 /**< Full speed INTR maximum packet size */

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
 */
typedef struct {
	u32 intr_in_xfer_size;      /**< INTR IN transfer buffer size (0 = use default). */
	u8  intr_in_addr;
#ifdef CONFIG_USBD_HID_KEYBOARD
	u8  intr_out_addr;
#endif
} usbd_hid_ep_cfg_t;

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
	void (*transmitted)(u8 status);

#ifdef CONFIG_USBD_HID_KEYBOARD
	/**
	 * @brief Called when new data is received from the host on the INTR IN endpoint, used for keyboard.
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
	const usbd_hid_ep_cfg_t *ep_cfg;    /**< Pointer to the EP configuration. */
	const usbd_hid_usr_cb_t *cb;        /**< Pointer to the user-defined callback structure. */
	usb_dev_t *dev;               /**< Pointer to the USB device instance. */
	usb_setup_req_t ctrl_req;     /**< Stores the current control request. */
	usbd_ep_t ep_intr_in;         /**< INTR IN endpoint structure. */
#ifdef CONFIG_USBD_HID_KEYBOARD
	usbd_ep_t ep_intr_out;        /**< INTR OUT endpoint structure. */
#endif
	u8 protocol;                  /**< Store the device protocol value. */
	u8 idle_rate;                 /**< Store the device idle rate. */
	u8 from_composite;			/**< Flag indicating if part of a composite device. */
} usbd_hid_t;

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
 * @param[in] ep_cfg: Pointer to EP configuration (endpoint addresses and buffer sizes).
 * @return 0 on success, non-zero on failure.
 */
int usbd_hid_init(const usbd_hid_usr_cb_t *cb, const usbd_hid_ep_cfg_t *ep_cfg);

#ifdef CONFIG_USBD_COMPOSITE
/**
 * @brief Initializes class driver as part of a composite device.
 * @param[in] cb: Pointer to the user-defined callback structure.
 * @param[in] ep_cfg: Pointer to EP configuration (endpoint addresses and buffer sizes).
 * @return 0 on success, non-zero on failure.
 */
int usbd_composite_hid_init(const usbd_hid_usr_cb_t *cb, const usbd_hid_ep_cfg_t *ep_cfg);
#endif

/**
  * @brief  DeInitialize HID device
  * @return 0 on success, non-zero on failure.
  */
int usbd_hid_deinit(void);

/**
 * @brief Transmits data to the host over the INTR IN endpoint.
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
