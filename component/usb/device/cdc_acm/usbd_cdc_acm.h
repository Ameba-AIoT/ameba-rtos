/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBD_CDC_ACM_H
#define USBD_CDC_ACM_H

/* Includes ------------------------------------------------------------------*/
#include "platform_autoconf.h"
#include "usbd.h"
#include "usb_cdc_acm.h"

/* Exported defines ----------------------------------------------------------*/

#ifdef CONFIG_ATCMD_HOST_CONTROL
#define CONFIG_CDC_ACM_NOTIFY                     1     /**< Enable/Disable notification feature. */
#else
#define CONFIG_CDC_ACM_NOTIFY                     1     /**< Enable/Disable notification feature. */
#endif

#define CONFIG_CDC_ACM_NOTIFY_LOOP_TEST           0     /**< Enable notification loopback test mode. */

#define CONFIG_CDC_ACM_BULK_TX_SKIP_MEMCPY        1     /**< Skip memcpy BULK IN DATA from application in class */

/** @} */

/**
 * @brief Defines basic device parameters like VID, PID, and string descriptors.
 * @{
 */
#define USBD_CDC_ACM_VID                              USB_VID               /**< Vendor ID. */
#define USBD_CDC_ACM_PID                              USB_PID               /**< Product ID. */
#define USBD_CDC_ACM_SELF_POWERED                     1U                    /**< Device is self-powered. */
#define USBD_CDC_ACM_REMOTE_WAKEUP_EN                 1U                    /**< Remote wakeup is enabled. */
#define USBD_CDC_ACM_LANGID_STRING                    0x0409U               /**< Language ID for string descriptors (0x0409 = English */
#define USBD_CDC_ACM_MFG_STRING                       "Realtek"             /**< Manufacturer string. */
#define USBD_CDC_ACM_PROD_HS_STRING                   "Realtek CDC ACM (HS)"/**< Product string for High-Speed mode. */
#define USBD_CDC_ACM_PROD_FS_STRING                   "Realtek CDC ACM (FS)"/**< Product string for Full-Speed mode. */
#define USBD_CDC_ACM_SN_STRING                        "1234567890"          /**< Serial number string. */
/** @} */

/**
 * @brief Defines endpoint addresses for BULK and INTERRUPT transfers.
 * @{
 */
#if defined (CONFIG_AMEBAGREEN2)
#define USBD_CDC_ACM_BULK_IN_EP                       0x82U
#define USBD_CDC_ACM_BULK_OUT_EP                      0x02U
#else
#define USBD_CDC_ACM_BULK_IN_EP                       0x81U
#define USBD_CDC_ACM_BULK_OUT_EP                      0x02U
#endif
#define USBD_CDC_ACM_INTR_IN_EP                       0x83U
/** @} */

#define USBD_CDC_ACM_HS_INTR_IN_INTERVAL              8U     /**< High speed INTR IN interval */
#define USBD_CDC_ACM_FS_INTR_IN_INTERVAL              8U     /**< Full speed INTR IN interval */

#define USBD_CDC_ACM_CTRL_BUF_SIZE                    512U   /**< Control transfer buffer size. */

/* Exported macros -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

#if CONFIG_CDC_ACM_NOTIFY
/**
 * @brief Structure for CDC ACM notifications sent to the host.
 * @details This is a packed structure used for sending notifications like SERIAL_STATE
 *          over the INTERRUPT IN endpoint.
 */
typedef struct {
	u8 bmRequestType;                  /**< D7: Data transfer direction (1=Device-to-Host), D6-5: Type (1=Class), D4-0: Recipient (1=Interface). */
	u8 bNotificationType;              /**< Notification code, e.g., `USB_CDC_ACM_NOTIFY_SERIAL_STATE`. */
	u16 wValue;                        /**< Varies by notification. For SERIAL_STATE, it's 0. */
	u16 wIndex;                        /**< Interface number. */
	u16 wLength;                       /**< Size of the notification data payload. */
	u8 buf[USB_CDC_ACM_INTR_IN_DATA_SIZE]; /**< Notification data payload. */
} __PACKED usbd_cdc_acm_ntf_t;
#endif

/**
 * @brief Structure containing callback functions for the CDC ACM class.
 * @details The user application should provide an instance of this structure
 *          to handle class-specific events.
 */
typedef struct {
	/**
	 * @brief Called when the CDC ACM class driver initialization for application resource setup.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* init)(void);

	/**
	 * @brief Called when the CDC ACM device is de-initialized for resource cleanup.
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
	 * @brief Called when new data is received from the host on the BULK OUT endpoint.
	 * @param[in] buf: Pointer to the received data buffer.
	 * @param[in] len: Length of the received data in bytes.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* received)(u8 *buf, u32 len);

	/**
	 * @brief Called when a data transmission to the host on the BULK IN endpoint is complete.
	 * @param[in] status: The status of the transmission.
	 */
	void(* transmitted)(u8 status);

	/**
	 * @brief Called when USB attach status changes for application to support hot-plug events.
	 * @param[in] old_status: The previous attach status.
	 * @param[in] status: The new attach status.
	 */
	void (*status_changed)(u8 old_status, u8 status);
} usbd_cdc_acm_cb_t;

/**
 * @brief Structure representing the CDC ACM device instance.
 */
typedef struct {
	usb_setup_req_t ctrl_req;   /**< Stores the current control request. */
	usbd_ep_t ep_bulk_in;       /**< BULK IN endpoint structure. */
	usbd_ep_t ep_bulk_out;      /**< BULK OUT endpoint structure. */
	usbd_ep_t ep_intr_in;       /**< INTERRUPT IN endpoint structure. */
	usb_dev_t *dev;             /**< Pointer to the USB device instance. */
	usbd_cdc_acm_cb_t *cb;      /**< Pointer to the user-defined callback structure. */
#if CONFIG_CDC_ACM_NOTIFY
	u16 intr_notify_idx;        /**< Index for managing interrupt notifications. */
#endif
} usbd_cdc_acm_dev_t;

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Initializes class driver with application callback handler.
 * @param[in] bulk_out_xfer_size: BULK OUT xfer buffer malloc length.
 * @param[in] bulk_in_xfer_size: BULK IN xfer buffer malloc length.
 * @param[in] cb: Pointer to the user-defined callback structure.
 * @return 0 on success, non-zero on failure.
 */
int usbd_cdc_acm_init(u32 bulk_out_xfer_size, u32 bulk_in_xfer_size, usbd_cdc_acm_cb_t *cb);

/**
 * @brief De-initializes the CDC ACM class driver.
 * @return 0 on success, non-zero on failure.
 */
int usbd_cdc_acm_deinit(void);

/**
 * @brief Transmits data to the host over the BULK IN endpoint.
 * @param[in] buf: Pointer to the data buffer to be transmitted.
 * @param[in] len: Length of the data in bytes.
 * @return 0 on success, non-zero on failure.
 */
int usbd_cdc_acm_transmit(u8 *buf, u32 len);

#if CONFIG_CDC_ACM_NOTIFY
/**
 * @brief Sets new line coding properties over the INTR IN endpoint.
 * @param[in] serial_state: New line coding properties.
 * @return 0 on success, non-zero on failure.
 */
int usbd_cdc_acm_notify_serial_state(u16 serial_state);
#endif
#endif  /* USBD_CDC_ACM_H */

