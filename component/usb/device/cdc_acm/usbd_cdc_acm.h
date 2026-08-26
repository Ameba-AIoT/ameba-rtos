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
/** @addtogroup Device_CDC_ACM_Constants Device CDC ACM Constants
 * @{
 */
#ifdef CONFIG_ATCMD_HOST_CONTROL
#define USBD_CDC_ACM_NOTIFY                  1     /**< Enable/Disable notification feature. */
#else
#define USBD_CDC_ACM_NOTIFY                  1     /**< Enable/Disable notification feature. */
#endif

#if USBD_CDC_ACM_NOTIFY
#define USBD_CDC_ACM_NOTIFY_LOOP_TEST        0     /**< Enable notification loopback test mode. */
#endif

#define USBD_CDC_ACM_BULK_TX_SKIP_MEMCPY     1     /**< Skip memcpy BULK IN DATA from application in class */

/* Defines basic device parameters like VID, PID, and string descriptors. */
#define USBD_CDC_ACM_VID                     USB_VID               /**< Vendor ID. */
#define USBD_CDC_ACM_PID                     USB_PID               /**< Product ID. */
#define USBD_CDC_ACM_LANGID_STRING           0x0409U               /**< Language ID for string descriptors (0x0409 = English) */
#define USBD_CDC_ACM_MFG_STRING              "Realtek"             /**< Manufacturer string. */
#define USBD_CDC_ACM_PROD_HS_STRING          "Realtek CDC ACM (HS)"/**< Product string for High-Speed mode. */
#define USBD_CDC_ACM_PROD_FS_STRING          "Realtek CDC ACM (FS)"/**< Product string for Full-Speed mode. */
#define USBD_CDC_ACM_SN_STRING               "1234567890"          /**< Serial number string. */

/* Defines endpoint addresses for BULK and INTERRUPT transfers. */
#define USBD_CDC_ACM_HS_INTR_IN_INTERVAL     8U     /**< High speed INTR IN interval */
#define USBD_CDC_ACM_FS_INTR_IN_INTERVAL     8U     /**< Full speed INTR IN interval */

/** @} End of Device_CDC_ACM_Constants group */
/** @} End of USB_Device_Constants group */

/* Exported macros -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/** @addtogroup USB_Device_Types USB Device Types
 * @{
 */
/** @addtogroup Device_CDC_ACM_Types Device CDC ACM Types
 * @{
 */

/**
 * @brief EP configuration for CDC ACM.
 * @details Specifies endpoint addresses for BULK IN/OUT and INTR IN endpoints.
 *          Used by both standalone and composite modes. Assigned at example layer.
 */
typedef struct {
	u32 bulk_in_xfer_size;      /**< BULK IN transfer buffer size */
	u32 bulk_out_xfer_size;     /**< BULK OUT transfer buffer size */
	u8  bulk_in_addr;           /**< BULK IN endpoint address (e.g. 0x81) */
	u8  bulk_out_addr;          /**< BULK OUT endpoint address (e.g. 0x01) */
	u8  intr_in_addr;           /**< INTERRUPT IN endpoint address (e.g. 0x83) */
} usbd_cdc_acm_ep_cfg_t;

#if USBD_CDC_ACM_NOTIFY
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
	int (*init)(void);

	/**
	 * @brief Called when the CDC ACM device is de-initialized for resource cleanup.
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
	 * @brief Called when new data is received from the host on the BULK OUT endpoint.
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 * @param[in] buf: Pointer to the received data buffer.
	 * @param[in] len: Length of the received data in bytes.
	 * @return 0 on success, non-zero on failure.
	 */
	int (*received)(u8 *buf, u32 len);

	/**
	 * @brief Called when a data transmission to the host on the BULK IN endpoint is complete.
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 * @param[in] status: The status of the transmission.
	 */
	void (*transmitted)(u8 status);

	/**
	 * @brief Called when USB attach status changes for application to support hot-plug events.
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 * @param[in] old_status: The previous attach status.
	 * @param[in] status: The new attach status.
	 */
	void (*status_changed)(u8 old_status, u8 status);

	/**
	 * @brief Called when the USB device resumes from suspend (wakeup).
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 * @details Indicates that the USB bus is active again and the upper layer can resume normal TRX.
	 */
	void (*wakeup)(void);
} usbd_cdc_acm_cb_t;

/**
 * @brief Structure representing the CDC ACM device instance.
 */
typedef struct {
	const usbd_cdc_acm_ep_cfg_t *ep_cfg;  /**< Pointer to the EP configuration (set by init). */
	const usbd_cdc_acm_cb_t *cb;      /**< Pointer to the user-defined callback structure. */
	usb_dev_t *dev;             /**< Pointer to the USB device instance. */
	usb_setup_req_t ctrl_req;   /**< Stores the current control request. */
	usbd_ep_t ep_bulk_in;       /**< BULK IN endpoint structure. */
	usbd_ep_t ep_bulk_out;      /**< BULK OUT endpoint structure. */
	usbd_ep_t ep_intr_in;       /**< INTERRUPT IN endpoint structure. */
#if defined(USBD_CDC_ACM_NOTIFY_LOOP_TEST) && (USBD_CDC_ACM_NOTIFY_LOOP_TEST == 1)
	u16 intr_notify_idx;        /**< Index for managing interrupt notifications. */
#endif
	u8 from_composite;          /**< Flag indicating if part of a composite device. */
} usbd_cdc_acm_dev_t;

/** @} End of Device_CDC_ACM_Types group */
/** @} End of USB_Device_Types group */

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @addtogroup USB_Device_Functions USB Device Functions
 * @{
 */
/** @addtogroup Device_CDC_ACM_Functions Device CDC ACM Functions
 * @{
 */

/**
 * @brief Initializes class driver as a standalone device.
 * @param[in] cb: Pointer to the user-defined callback structure.
 * @param[in] ep_cfg: Pointer to EP configuration (endpoint addresses and buffer sizes).
 * @return 0 on success, non-zero on failure.
 */
int usbd_cdc_acm_init(const usbd_cdc_acm_cb_t *cb, const usbd_cdc_acm_ep_cfg_t *ep_cfg);

#ifdef CONFIG_USBD_COMPOSITE
/**
 * @brief Initializes class driver as part of a composite device.
 * @param[in] cb: Pointer to the user-defined callback structure.
 * @param[in] ep_cfg: Pointer to EP configuration (endpoint addresses and buffer sizes).
 * @return 0 on success, non-zero on failure.
 */
int usbd_composite_cdc_acm_init(const usbd_cdc_acm_cb_t *cb, const usbd_cdc_acm_ep_cfg_t *ep_cfg);
#endif

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

#if USBD_CDC_ACM_NOTIFY

/**
 * @brief Sets new line coding properties over the INTR IN endpoint.
 * @param[in] serial_state: New line coding properties.
 * @return 0 on success, non-zero on failure.
 */
int usbd_cdc_acm_notify_serial_state(u16 serial_state);

#endif

/** @} End of Device_CDC_ACM_Functions group */
/** @} End of USB_Device_Functions group */
/** @} End of USB_Device_API group */

#ifdef __cplusplus
}
#endif

#endif  /* USBD_CDC_ACM_H */
