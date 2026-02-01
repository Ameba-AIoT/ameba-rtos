/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBD_COMPOSITE_CDC_ACM_H
#define USBD_COMPOSITE_CDC_ACM_H

/* Includes ------------------------------------------------------------------*/

#include "usbd.h"
#include "usbd_composite_config.h"
#include "usb_cdc_acm.h"

/* Exported defines ----------------------------------------------------------*/

/**
 * @brief Configuration macro to enable or disable the CDC ACM notification endpoint.
 * @details Set to 1 to enable the Interrupt IN endpoint for sending notifications
 *          like SERIAL_STATE. Set to 0 to disable.
 */
#define CONFIG_COMP_CDC_ACM_NOTIFY                       1     /**< Enable/Disable notification feature. */

#define COMP_CDC_ACM_HS_INTR_IN_INTERVAL                 8U  /**< High-speed Interrupt IN endpoint polling interval. */
#define COMP_CDC_ACM_FS_INTR_IN_INTERVAL                 8U  /**< Full-speed Interrupt IN endpoint polling interval. */

/* Exported types ------------------------------------------------------------*/

/**
 * @brief User callback structure for CDC ACM events.
 * @details This structure allows the application layer to handle key CDC ACM events.
 */
typedef struct {
	int(* init)(void);                             /**< Called during class driver initialization for application resource setup. */
	int(* deinit)(void);                           /**< Called during class driver deinitialization for resource cleanup. */
	int(* setup)(usb_setup_req_t *req, u8 *buf);   /**< Called during control transfer SETUP/DATA phases to handle application-specific control requests. */
	int(* received)(u8 *buf, u32 len);             /**< Called when new data is received on the Bulk OUT endpoint. */
	void(* transmitted)(u8 status);                /**< Called after data transmission on the Bulk IN endpoint is complete. */
} usbd_composite_cdc_acm_usr_cb_t;

#if CONFIG_COMP_CDC_ACM_NOTIFY
/**
 * @brief CDC ACM Notification Structure.
 * @details This structure defines the format for sending notifications (like SERIAL_STATE)
 *          on the Interrupt IN endpoint. This is only available if CONFIG_COMP_CDC_ACM_NOTIFY is enabled.
 */
typedef struct {
	u8 bmRequestType;     /**< D7: 0=Host-to-Device, 1=Device-to-Host. D6-5: 1=Class. D4-0: 0=Device, 1=Interface, 2=Endpoint. */
	u8 bNotificationType; /**< Notification code, e.g., USB_CDC_NOTIFY_SERIAL_STATE. */
	u16 wValue;           /**< Typically 0. */
	u16 wIndex;           /**< The interface number. */
	u16 wLength;          /**< Size of the notification data payload. */
	u8 buf[USB_CDC_ACM_INTR_IN_DATA_SIZE]; /**< Notification data payload. */
} __PACKED usbd_composite_cdc_acm_ntf_t;
#endif

/**
 * @brief CDC ACM device context structure.
 * @details This main structure holds the state and configuration for the CDC ACM function.
 */
typedef struct {
	usbd_composite_dev_t *cdev;           /**< Pointer to the parent composite device structure. */
	usbd_composite_cdc_acm_usr_cb_t *cb;  /**< Pointer to the user-registered callback structure. */
	usbd_ep_t ep_bulk_in;                 /**< Bulk IN endpoint handler. */
	usbd_ep_t ep_bulk_out;                /**< Bulk OUT endpoint handler. */
#if CONFIG_COMP_CDC_ACM_NOTIFY
	usbd_ep_t ep_intr_in;                 /**< Interrupt IN endpoint handler (for notifications). */
#endif
} usbd_composite_cdc_acm_dev_t;

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

extern const usbd_class_driver_t usbd_composite_cdc_acm_driver;

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Initializes the CDC ACM composite function.
 * @param[in] cdev: Pointer to the composite device context.
 * @param[in] bulk_out_xfer_size: The desired transfer size for the Bulk OUT buffer.
 * @param[in] bulk_in_xfer_size: The desired transfer size for the Bulk IN buffer.
 * @param[in] cb: Pointer to the user callback structure.
 * @return 0 on success, a negative error code on failure.
 */
int usbd_composite_cdc_acm_init(usbd_composite_dev_t *cdev, u16 bulk_out_xfer_size, u16 bulk_in_xfer_size, usbd_composite_cdc_acm_usr_cb_t *cb);

/**
 * @brief De-initializes the CDC ACM composite function.
 * @return 0 on success, a negative error code on failure.
 */
int usbd_composite_cdc_acm_deinit(void);

/**
 * @brief Transmits data over the CDC ACM Bulk IN endpoint.
 * @param[in] buf: Pointer to the data buffer to be transmitted.
 * @param[in] len: Length of the data in bytes.
 * @return 0 on success, a negative error code on failure.
 */
int usbd_composite_cdc_acm_transmit(u8 *buf, u32 len);

#if CONFIG_COMP_CDC_ACM_NOTIFY
/**
 * @brief Sends a SERIAL_STATE notification to the host over the CDC ACM Interrupt IN endpoint..
 * @param[in] serial_state: A bitmap of the serial line states (see @ref COMP_CDC_ACM_CTRL_DCD, etc.).
 * @return 0 on success, a negative error code on failure.
 */
int usbd_composite_cdc_acm_notify_serial_state(u16 serial_state);
#endif

#endif  /* USBD_COMPOSITE_CDC_ACM_H */

