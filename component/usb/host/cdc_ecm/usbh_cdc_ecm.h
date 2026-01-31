/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_CDC_ECM_H
#define USBH_CDC_ECM_H

/* Includes ------------------------------------------------------------------*/

#include "usbh.h"

/*
        Example Hierarchy:
          LWIP (Network Stack)
            |
       ECM Class Driver
            |
       USB Core Driver
*/

/* Exported defines ----------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/
/** @addtogroup USB_Host_API USB Host API
 *  @{
 */
/** @addtogroup USB_Host_Constants USB Host Constants
 * @{
 */
/** @addtogroup Host_CDC_ECM_Constants Host CDC ECM Constants
 * @{
 */

#define USBH_CDC_ECM_STATE_TRACE_ENABLE                         0        /**< Enable or disable CDC ECM state trace logging. */

#define USB_DEFAULT_VID                                         USB_VID  /**< Default Vendor ID. */

#define CDC_ECM_MAC_STR_LEN                                     (6)      /**< Length of the MAC address in bytes. */
#define CDC_ECM_CTRL_REG_BUF_LEN                                (4)      /**< Length of the ECM dongle control register buffer. */
#define CDC_ECM_MUTICAST_FILTER_STR_LEN                         (20)     /**< Length of the ECM multicast filter control buffer. */

/** @} End of Host_CDC_ECM_Constants group*/
/** @} End of USB_Host_Constants group*/

/* Exported types ------------------------------------------------------------*/

/** @addtogroup USB_Host_Types USB Host Types
 * @{
 */
/** @addtogroup Host_CDC_ECM_Types Host CDC ECM Types
 * @{
 */

/**
 * @brief USB CDC ECM Host Private Data Structure.
 */
typedef struct {
	u16 *led_array;    /**< Pointer to the LED status array; each u16 element represents the state or brightness of an LED. */
	u8 *mac_value;     /**< Pointer to the MAC address buffer; typically points to a 6-byte physical address. */
	u8 led_cnt;        /**< LED count; indicates the number of valid elements in the led_array. */
} usbh_cdc_ecm_priv_data_t;

/* CDC ECM user callback interface */
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
	 *        Used to report the device connection status to the application.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* attach)(void);

	/**
	 * @brief Callback invoked when a device is detached.
	 *        Used to report the device disconnection status to the application.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* detach)(void);

	/**
	 * @brief Callback invoked when the device setup phase is complete.
	 *        Indicates that the device is ready for data transfer.
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
} usbh_cdc_ecm_state_cb_t;

typedef struct {
	usbh_ep_desc_t ep_desc;                   /**< Endpoint descriptor. */
	usbh_pipe_t pipe;                         /**< USB Host pipe handle. */
	u8 *buf;                                  /**< Malloc buffer for RX data. */
	u32 buf_len;                              /**< Length of the RX buffer. */
	u32 busy_tick;                            /**< Timestamp for busy detection. */
#if USBH_CDC_ECM_STATE_TRACE_ENABLE
	u32 trigger_cnt;                          /**< Debug trigger counter. */
#endif
	u8 valid;                                 /**< Validity flag for this pipe info. */
} usbh_cdc_ecm_pipe_info_t;

/** @} End of Host_CDC_ECM_Types group*/
/** @} End of USB_Host_Types group*/

/**
 * @brief Structure representing the CDC ECM host instance.
 */
typedef struct {
	u8 muticast_filter[CDC_ECM_MUTICAST_FILTER_STR_LEN]; /**< Buffer for multicast filer control */
	u8 mac[CDC_ECM_MAC_STR_LEN];                     /**< Buffer for saving MAC string */
	u8 mac_ctrl_lock[CDC_ECM_CTRL_REG_BUF_LEN];      /**< Buffer for RTL8152 MAC change control */
	u8 flow_ctrl[CDC_ECM_CTRL_REG_BUF_LEN];          /**< Buffer for RTL8152 flow control */
	u8 rcr[CDC_ECM_CTRL_REG_BUF_LEN];                /**< Buffer for RTL8156 RCR register */

	usbh_cdc_ecm_pipe_info_t intr_rx;                /* Intr IN Endpoint Info */
	usbh_cdc_ecm_pipe_info_t bulk_tx;                /* Bulk OUT Endpoint Info */
	usbh_cdc_ecm_pipe_info_t bulk_rx;                /* Bulk IN Endpoint Info */

	usb_os_sema_t           bulk_tx_sema;            /* Semaphore for BULK TX synchronization */

	usbh_cdc_ecm_state_cb_t  *cb;                    /* User callback structure */
	usb_host_t *host;                                /* USB Host core handle */
	u16 intr_check_tick;                             /* ECM Intr check tick, used to reduce the cpu load */
	u16 sub_status;                                  /* ECM sub-status, see @ref usbh_cdc_ecm_at_set_state_t */
	u16 *led_array;                                  /* Pointer to LED array */
	u8  *dongle_ctrl_buf;                            /* Buffer for control transfers (cache aligned) */
	u32 eth_statistic_count;                         /* Feature selector parameter: Statistic count */
	u16 feature_selector;                            /* Feature selector parameter */
	u16 packet_filter;                               /* Packet filter configuration */
	u16 muticast_filter_len;                         /* Length of multicast filter parameters */
	u16 vid;                                         /* Vendor ID */
	u16 pid;                                         /* Product ID */
	u8 data_itf_id;                                  /* Data Interface number */
	u8 data_alt_set;                                 /* Alternate Setting value for Data Interface */
	u8 iMACAddressStringId;                          /* Index of the MAC address string descriptor */
	u8 state;                                        /* Internal state machine status, see @ref usbh_cdc_ecm_state_t */
	u8 next_xfer;                                    /* Flag to trigger the next transfer event */
	u8 led_cnt;                                      /* Number of LEDs */
	u8 mac_valid;                                    /* Flag indicating if MAC address is valid */
	u8 mac_src_type;                                 /* ECM dongle MAC source type, see @ref usbh_cdc_ecm_dongle_mac_type_t */
	volatile u8             bulk_tx_block;           /* Flag indicating BULK TX is blocked/busy */
	volatile u8             eth_hw_connect;          /* Ethernet physical link status: 0=Disconnect, 1=Connect */
} usbh_cdc_ecm_host_t;

/* Exported functions --------------------------------------------------------*/

/** @addtogroup USB_Host_API USB Host API
 *  @{
 */

/** @addtogroup USB_Host_Functions USB Host Functions
 * @{
 */
/** @addtogroup Host_CDC_ECM_Functions Host CDC ECM Functions
 * @{
 */

/**
 * @brief  Initializes the CDC ECM host class driver.
 *         Registers the application callback handler and user private parameters.
 * @param[in] cb: Pointer to the user-defined callback structure.
 * @param[in] priv: Pointer to the user-defined private parameter structure.
 * @return 0 (HAL_OK) on success, non-zero on failure.
 */
int usbh_cdc_ecm_init(usbh_cdc_ecm_state_cb_t *cb, usbh_cdc_ecm_priv_data_t *priv);

/**
 * @brief  De-initializes the CDC ECM host class driver and releases resources.
 * @return 0 (HAL_OK) on success, non-zero on failure.
 */
int usbh_cdc_ecm_deinit(void);

/**
 * @brief  Selects the appropriate configuration index for the device.
 * @param[in] host: USB Host Handle.
 * @return 0 (HAL_OK) on success, non-zero on failure.
 */
int usbh_cdc_ecm_choose_config(usb_host_t *host);

/**
 * @brief  Validates the configuration descriptor of the connected device.
 * @param[in] host: USB Host Handle.
 * @return 0 (HAL_OK) on success, non-zero on failure.
 */
int usbh_cdc_ecm_check_config_desc(usb_host_t *host);

/**
 * @brief  Checks if the CDC ECM interface is fully configured and ready for data transfer.
 * @return 1: Driver is ready (Enumerated & Configured).
 *         0: Driver is not ready.
 */
u8 usbh_cdc_ecm_usb_is_ready(void);

/**
 * @brief  Transmits an Ethernet packet to the device via the Bulk OUT endpoint.
 * @param[in] buf: Pointer to the data buffer (Ethernet frame) to be transmitted.
 * @param[in] len: Length of the data in bytes.
 * @return 0 (HAL_OK) on success, non-zero on failure.
 */
int usbh_cdc_ecm_send_data(u8 *buf, u32 len);

/**
 * @brief  Gets the Ethernet physical link status.
 * @return 1: Link Up (Cable connected).
 *         0: Link Down (Cable disconnected or device not ready).
 */
int usbh_cdc_ecm_get_connect_status(void);

/**
 * @brief  Gets the MAC address of the connected device.
 * @return Pointer to the 6-byte MAC address array on success, NULL on failure.
 */
const u8 *usbh_cdc_ecm_process_mac_str(void);

/** @} End of Host_CDC_ECM_Functions group */
/** @} End of USB_Host_Functions group */
/** @} End of USB_Host_API group */

#endif  /* USBD_CDC_ECM_H */
