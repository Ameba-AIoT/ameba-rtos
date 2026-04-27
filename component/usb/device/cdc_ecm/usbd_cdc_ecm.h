/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBD_CDC_ECM_H
#define USBD_CDC_ECM_H

/* Includes ------------------------------------------------------------------*/
#include "platform_autoconf.h"
#include "usbd.h"
#include "usb_cdc_ecm.h"

/* Exported defines ----------------------------------------------------------*/

/** @addtogroup USB_Device_API USB Device API
 *  @{
 */
/** @addtogroup USB_Device_Constants USB Device Constants
 * @{
 */
/** @addtogroup Device_CDC_ECM_Constants Device CDC ECM Constants
 * @{
 */

#define CDC_ECM_MAC_STR_LEN                           (6)      /**< Length of the MAC address in bytes. */

/* Defines endpoint addresses for BULK and INTERRUPT transfers. */
#if defined (CONFIG_AMEBAGREEN2)
#define USBD_CDC_ECM_BULK_IN_EP                       0x82U
#define USBD_CDC_ECM_BULK_OUT_EP                      0x02U
#else
#define USBD_CDC_ECM_BULK_IN_EP                       0x81U
#define USBD_CDC_ECM_BULK_OUT_EP                      0x02U
#endif
#define USBD_CDC_ECM_INTR_IN_EP                       0x83U

/* Interface numbers */
#define USBD_CDC_ECM_COMM_INTERFACE_NUM               0x00U  /**< Communication interface */
#define USBD_CDC_ECM_DATA_INTERFACE_NUM               0x01U  /**< Data interface */

/** @} End of Device_CDC_ECM_Constants group*/
/** @} End of USB_Device_Constants group*/

/* Exported macros -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/** @addtogroup USB_Device_Types USB Device Types
 * @{
 */
/** @addtogroup Device_CDC_ECM_Types Device CDC ECM Types
 * @{
 */

/**
 * @brief USB CDC ECM Device Private Data Structure.
 */
typedef struct {
	u8 *mac_value;     /**< Pointer to the MAC address buffer; typically points to a 6-byte physical address. */
} usbd_cdc_ecm_priv_data_t;

/**
 * @brief Structure containing callback functions for the CDC ECM class.
 * @details The user application should provide an instance of this structure
 *          to handle class-specific events.
 */
typedef struct {
	/**
	 * @brief Pointer to the USB CDC ECM private data structue
	 */
	usbd_cdc_ecm_priv_data_t *priv;

	/**
	 * @brief Called when the CDC ECM class driver initialization for application resource setup.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* init)(void);

	/**
	 * @brief Called when the CDC ECM device is de-initialized for resource cleanup.
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
	 * @brief Called when USB attach status changes for application to support hot-plug events.
	 * @param[in] old_status: The previous attach status.
	 * @param[in] status: The new attach status.
	 */
	void (*status_changed)(u8 old_status, u8 status);
} usbd_cdc_ecm_cb_t;

/**
 * @brief Structure representing the CDC ECM device instance.
 */
typedef struct {
	u8 mac[CDC_ECM_MAC_STR_LEN];/**< Buffer for saving MAC address */
	usb_setup_req_t ctrl_req;   /**< Stores the current control request. */
	usbd_ep_t ep_bulk_in;       /**< BULK IN endpoint structure. */
	usbd_ep_t ep_bulk_out;      /**< BULK OUT endpoint structure. */
	usbd_ep_t ep_intr_in;       /**< INTERRUPT IN endpoint structure. */
	usb_dev_t *dev;             /**< Pointer to the USB device instance. */
	usbd_cdc_ecm_cb_t *cb;      /**< Pointer to the user-defined callback structure. */
	usb_os_sema_t bulk_tx_sema; /* Semaphore for BULK TX synchronization */

	u8 connect_status;          /**< NetworkConnection. */
	u8 notify_state;            /**< Notify Type. */
	u8 interface_id;            /**< Choose interface number. */
	__IO u8 bulk_tx_block;      /* Flag indicating BULK TX is blocked/busy */
	u8 mac_valid;               /* Flag indicating if MAC address is valid */
	u8 mac_src_type;            /* ECM dongle MAC source type, see @ref usbd_cdc_ecm_dongle_mac_type_t */
} usbd_cdc_ecm_dev_t;

/** @} End of Device_CDC_ECM_Types group*/
/** @} End of USB_Device_Types group*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @addtogroup USB_Device_Functions USB Device Functions
 * @{
 */
/** @addtogroup Device_CDC_ECM_Functions Device CDC ECM Functions
 * @{
 */

/**
 * @brief Initializes class driver with application callback handler.
 * @param[in] bulk_out_xfer_size: BULK OUT xfer buffer malloc length.
 * @param[in] bulk_in_xfer_size: BULK IN xfer buffer malloc length.
 * @param[in] cb: Pointer to the user-defined callback structure.
 * @return 0 on success, non-zero on failure.
 */
int usbd_cdc_ecm_init(usbd_cdc_ecm_cb_t *cb);

/**
 * @brief De-initializes the CDC ECM class driver.
 * @return 0 on success, non-zero on failure.
 */
int usbd_cdc_ecm_deinit(void);

/**
 * @brief Transmits data to the host over the BULK IN endpoint.
 * @param[in] buf: Pointer to the data buffer to be transmitted.
 * @param[in] len: Length of the data in bytes.
 * @param[in] block: Blocking mode flag (e.g., 0 for non-blocking, else for blocking).
 * @return 0 on success, non-zero on failure.
 */
int usbd_cdc_ecm_transmit(u8 *buf, u32 len, u8 block);

/**
 * @brief  Gets the device connect status.
 * @return 1: Device connect to the host
 *         0:
 */
int usbd_cdc_ecm_get_connect_status(void);

/**
 * @brief  Gets the MAC address of the device.
 * @return Pointer to the 6-byte MAC address array on success, NULL on failure.
 */
const u8 *usbd_cdc_ecm_get_mac_str(void);

/** @} End of Device_CDC_ECM_Functions group */
/** @} End of USB_Device_Functions group */
/** @} End of USB_Device_API group */

#endif  /* USBD_CDC_ECM_H */
