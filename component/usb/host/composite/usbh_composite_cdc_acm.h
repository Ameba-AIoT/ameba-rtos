/*
 * Copyright (c) 2025 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_COMPOSITE_CDC_ACM_H
#define USBH_COMPOSITE_CDC_ACM_H

/* Includes ------------------------------------------------------------------*/

#include "usbh.h"
#include "usbh_composite_config.h"
#include "usb_cdc_acm.h"

/* Exported defines ----------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/
/** @addtogroup USB_Host_API USB Host API
 *  @{
 */
/** @addtogroup USB_Host_Constants USB Host Constants
 * @{
 */
/** @addtogroup Host_Composite_Constants Host CDC ACM Constants
 * @{
 */

/** @} End of Host_Composite_Constants group*/
/** @} End of USB_Host_Constants group*/

/* Exported types ------------------------------------------------------------*/

/* CDC ACM user callback interface */
typedef struct {
	u16 vid;               /**< Dongle Vendor ID */
	u16 pid;               /**< Dongle Product ID */
	u8 at_line_idx;        /**< Dongle at cmd interface idx*/
} usbh_composite_cdc_acm_param_t;

/* ---------------------------CDC ACM Config End -------------------------------*/
typedef struct {
	/**
	 * @brief Callback invoked during CDC ECM class driver initialization.
	 *        Used for application-specific resource setup.
	 * @return 0 on success, non-zero on failure.
	*/
	int(* init)(void);

	/**
	 * @brief Callback invoked during CDC ECM class driver de-initialization.
	 *        Used for application-specific resource cleanup.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* deinit)(void);

	/**
	 * @brief Callback invoked when a device is attached.
	 *        Used to report the connection status to the application.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* attach)(void);

	/**
	 * @brief Callback invoked when a device is detached.
	 *        Used to report the disconnection status to the application.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* detach)(void);

	/**
	 * @brief Callback invoked when the host setup phase is complete.
	 *        Indicates that the host is ready for data transfer.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* setup)(void);

	/**
	* @brief Callback invoked when a bulk IN data transfer (Receive) is complete.
	* @param[in] buf: Pointer to the buffer containing received data.
	* @param[in] len: Length of the valid received data in bytes.
	* @return 0 on success, non-zero on failure.
	*/
	int(* bulk_received)(u8 *buf, u32 len);

	/**
	* @brief Callback invoked when a bulk IN data transfer (Receive) is complete.
	* @param[in] buf: Pointer to the buffer containing received data.
	* @param[in] len: Length of the valid received data in bytes.
	* @return 0 on success, non-zero on failure.
	*/
	int(* bulk_send)(usbh_urb_state_t state);

	/**
	* @brief Callback invoked when a intr IN data transfer (Receive) is complete.
	* @param[in] buf: Pointer to the buffer containing received data.
	* @param[in] len: Length of the valid received data in bytes.
	* @return 0 on success, non-zero on failure.
	*/
	int(* intr_received)(u8 *buf, u32 len);

	/**
	* @brief Callback invoked when the CDC ACM line coding format changed.
	* @param[in] buf: Pointer to the line coding structure.
	* @return 0 on success, non-zero on failure.
	*/
	int(* line_coding_changed)(usb_cdc_line_coding_t *line_coding);

	usbh_composite_cdc_acm_param_t *priv;
} usbh_composite_cdc_acm_usr_cb_t;

/* CDC ACM host */
typedef struct {
	usbh_pipe_t bulk_in;    /**< Bulk IN Endpoint Info. */
	usbh_pipe_t bulk_out;   /**< Bulk OUT Endpoint Info. */
	usbh_pipe_t intr_in;    /**< INTR In Endpoint Info. */

	usb_cdc_line_coding_t    *line_coding;      /**< Pointer to line coding  */
	usb_cdc_line_coding_t    *user_line_coding; /**< Pointer to user line coding */
	usbh_composite_cdc_acm_param_t          *priv_param;       /**< Pointer to private params */
	usbh_composite_cdc_acm_param_t          *param_item;       /**< Pointer to choose param */
	usbh_composite_cdc_acm_usr_cb_t         *cb;               /**< User callback structure */
	usbh_composite_host_t                   *driver;           /**< Composite driver handle */

	u8                                      state;             /**< Process status, ref usbh_composite_cdc_acm_state_t */
	u8                                      sub_status;        /**< ACM sub-status, ref usbh_composite_cdc_acm_ctrl_state_t */
} usbh_composite_cdc_acm_host_t;

/* Exported variables --------------------------------------------------------*/
extern const usbh_class_driver_t usbh_composite_cdc_acm_driver;/**< Point to composite CDC ACM driver handle */

/* Exported functions --------------------------------------------------------*/

/** @addtogroup USB_Host_API USB Host API
 *  @{
 */

/** @addtogroup USB_Host_Functions USB Host Functions
 * @{
 */
/** @addtogroup Host_Composite_Functions Host CDC ACM Functions
 * @{
 */
/**
 * @brief  Initializes the CDC ACM composite host class driver.
 *         Registers the application callback handler and user private parameters.
 * @param[in] chost: Pointer to the driver.
 * @param[in] cb: Pointer to the user-defined callback structure.
 * @return 0 (HAL_OK) on success, non-zero on failure.
 */
int usbh_composite_cdc_acm_init(usbh_composite_host_t *host, usbh_composite_cdc_acm_usr_cb_t *cb);

/**
 * @brief  De-initializes the CDC ACM host class driver and releases resources.
 * @return 0 (HAL_OK) on success, non-zero on failure.
 */
int usbh_composite_cdc_acm_deinit(void);

/**
 * @brief Perform some necessary control transfers.
 * @param[in] host: USB Host Handle.
 * @return 0 (HAL_OK) on success, non-zero on failure.
 */
int usbh_composite_cdc_acm_ctrl_setting(usb_host_t *host);

/**
 * @brief  Transmits an Ethernet packet to the device via the Bulk OUT endpoint.
 * @param[in] buf: Pointer to the data buffer (Ethernet frame) to be transmitted.
 * @param[in] len: Length of the data in bytes.
 * @return 0 (HAL_OK) on success, non-zero on failure.
 */
int usbh_composite_cdc_acm_transmit(u8 *buf, u32 len);

/** @} End of Host_Composite_Functions group */
/** @} End of USB_Host_Functions group */
/** @} End of USB_Host_API group */
#endif  /* USBH_CDC_ECM_APPX_H */
