/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_COMPOSITE_CDC_ECM_H
#define USBH_COMPOSITE_CDC_ECM_H

/* Includes ------------------------------------------------------------------*/

#include "usbh.h"
#include "usbh_composite_config.h"

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
/** @addtogroup Host_Composite_Constants Host CDC ECM Constants
 * @{
 */

/* CDC ECM data report interface */
typedef void (*usb_report_data)(u8 *buf, u32 len);

#define ECM_STATE_DEBUG_ENABLE                                  0      /**< Enable or disable CDC ECM state trace logging. */

/* Macro defines -----------------------------------------------------------*/


#define CDC_ECM_MAC_STR_LEN                                     (6)    /**< Length of the MAC address in bytes. */
#define CDC_ECM_MAC_CTRL_REG_LEN                                (4)    /**< Length of the ECM dongle control register buffer. */
#define CDC_ECM_MUTICAST_FILTER_STR_LEN                         (20)   /**< Length of the ECM multicast filter control buffer. */

/* CDC Class&Subclass Codes */
#define CDC_IF_CDC_CTRL_CODE                                    0x02U /**< CDC interface control code. */
#define CDC_IF_CDC_CTRL_SUB_CLASS_ECM_CODE                      0x06U /**< CDC interface control sub class code. */
#define CDC_IF_CDC_DATA_CODE                                    0x0AU /**< CDC interface data code. */
#define CDC_IF_CDC_DATA_SUB_CLASS_DATA_CODE                     0x00U /**< CDC interface data sub class code. */

/** @} End of Host_Composite_Constants group*/
/** @} End of USB_Host_Constants group*/

/* Exported types ------------------------------------------------------------*/

/** @addtogroup USB_Host_Types USB Host Types
 * @{
 */
/** @addtogroup Host_Composite_Types Host CDC ECM Types
 * @{
 */

/**
 * @brief USB CDC ACM ECM Composite Host Private Data Structure.
 */
typedef struct {
	u16 *led_array;    /**< Pointer to the LED status array; each u16 element represents the state or brightness of an LED. */
	u8 *mac_value;     /**< Pointer to the MAC address buffer; typically points to a 6-byte physical address. */
	u8 led_cnt;        /**< LED count; indicates the number of valid elements in the led_array. */
} usbh_composite_cdc_ecm_priv_data_t;

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

	usbh_composite_cdc_ecm_priv_data_t *priv;
} usbh_composite_cdc_ecm_usr_cb_t;

typedef struct {
	usbh_ep_desc_t ep_desc;                   /**< Endpoint descriptor. */
	usbh_pipe_t pipe;                         /**< USB Host pipe handle. */
	u8 *buf;                                  /**< Malloc buffer for RX data. */
	u32 buf_len;                              /**< Length of the RX buffer. */
	u32 busy_tick;                            /**< Timestamp for busy detection. */
#if ECM_STATE_DEBUG_ENABLE
	u32 trigger_cnt;                          /**< Debug trigger counter. */
#endif
	u8 valid;                                 /**< Validity flag for this pipe info. */
} usbh_composite_cdc_ecm_pipe_info_t;

/** @} End of Host_Composite_Types group*/
/** @} End of USB_Host_Types group*/

/**
 * @brief Structure representing the CDC ECM host instance.
 */
typedef struct {
	u8                            muticast_filter[CDC_ECM_MUTICAST_FILTER_STR_LEN]; /**< Buffer for multicast filer control */
	u8                            mac[CDC_ECM_MAC_STR_LEN];                     /**< Buffer for saving MAC string */
	u8                            mac_ctrl_lock[CDC_ECM_MAC_CTRL_REG_LEN];      /**< Buffer for RTL8152 MAC change control */
	u8                            flow_ctrl[CDC_ECM_MAC_CTRL_REG_LEN];          /**< Buffer for RTL8152 flow control */
	u8                            rcr[CDC_ECM_MAC_CTRL_REG_LEN];                /**< Buffer for RTL8156 RCR register */

	usbh_composite_cdc_ecm_pipe_info_t      ecm_ctrl_ep;            /* Control Endpoint Info */
	usbh_composite_cdc_ecm_pipe_info_t      ecm_tx_ep;              /* Bulk OUT Endpoint Info */
	usbh_composite_cdc_ecm_pipe_info_t      ecm_rx_ep;              /* Bulk IN Endpoint Info */

	usb_os_sema_t                           cdc_ecm_tx_sema;    /**<  Semaphore for USB TX synchronization */
	usb_report_data                         report_data;        /**< usb rx callback function */
	usbh_composite_cdc_ecm_usr_cb_t         *cb;                /**< User callback structure */
	usbh_composite_host_t                   *driver;            /**< Composite driver handle */
	u16                                     sub_status;         /**< ECM sub-status, see @ref usbh_cdc_ecm_at_set_state_t */
	u16                                     *led_array;         /**< Pointer to LED array */
	u8                                      *dongle_ctrl_buf;   /**< Buffer for control transfers (cache aligned)*/

	u32 eth_statistic_count;                         /**< Feature selector parameter: Statistic count */
	u16 feature_selector;                            /**< Feature selector parameter */
	u16 packet_filter;                               /**< Packet filter configuration */
	u16 muticast_filter_len;                         /**< Length of multicast filter parameters */
	u16 vid;                                         /**< Vendor ID */
	u16 pid;                                         /**< Product ID */

	volatile u8                   cdc_ecm_tx_block;  /**< Ecm tx status */
	volatile u8                   usb_setup;         /**< Ecm setup status */
	volatile u8                   eth_hw_connect;    /**< Ecm ethernet connect status:0 disconnect,1 connect */
	volatile u8                   ready_to_xfer;     /**< Dongle need some prepare before transfer ethernet packets */

	u8                            if_num;            /**< Interface number */
	u8                            alt_set;           /**< Value used to select alternative setting */
	u8                            mac_str_id;        /**< MAC string id get from descriptor */
	u8                            state;             /**< Process status : usbh_composite_cdc_ecm_state_t*/
	u8                            next_xfer;         /**< Send next events */
	u8                            led_cnt;           /**< Led count */
	u8                            mac_valid;         /**< Has get a valid MAC */
	u8                            mac_src_type;      /**< ecm dongle mac source type : usbh_composite_cdc_ecm_dongle_mac_type_t */
} usbh_composite_cdc_ecm_host_t;

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/
extern const usbh_class_driver_t usbh_composite_cdc_ecm_driver;  /**< Point to composite cdc ecm driver handle */

/* Exported functions --------------------------------------------------------*/

/** @addtogroup USB_Host_API USB Host API
 *  @{
 */

/** @addtogroup USB_Host_Functions USB Host Functions
 * @{
 */
/** @addtogroup Host_Composite_Functions Host CDC ECM Functions
 * @{
 */
/**
 * @brief  Initializes the CDC ECM composite host class driver.
 *         Registers the application callback handler and user private parameters.
 * @param[in] chost: Pointer to the driver.
 * @param[in] cb: Pointer to the user-defined callback structure.
 * @return 0 (HAL_OK) on success, non-zero on failure.
 */
int usbh_composite_cdc_ecm_init(usbh_composite_host_t *chost, usbh_composite_cdc_ecm_usr_cb_t *cb);

/**
 * @brief  De-initializes the CDC ECM host class driver and releases resources.
 * @return 0 (HAL_OK) on success, non-zero on failure.
 */
int usbh_composite_cdc_ecm_deinit(void);

/**
 * @brief Perform some necessary control transfers.
 * @param[in] host: USB Host Handle.
 * @return 0 (HAL_OK) on success, non-zero on failure.
 */
int usbh_composite_cdc_ecm_ctrl_setting(usb_host_t *host);

/**
 * @brief  Selects the appropriate configuration index.
 * @param[in] host: USB Host Handle.
 * @return 0 (HAL_OK) on success, non-zero on failure.
 */
int usbh_composite_cdc_ecm_choose_config(usb_host_t *host);

/**
 * @brief  Validates the configuration descriptor of the connected device.
 * @param[in] host: USB Host Handle.
 * @return 0 (HAL_OK) on success, non-zero on failure.
 */
int usbh_composite_cdc_ecm_check_config_desc(usb_host_t *host, u8 cfg_max);

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
int usbh_composite_cdc_ecm_get_connect_status(void);

/**
 * @brief  Gets the MAC address of the connected device.
 * @return Pointer to the 6-byte MAC address array on success, NULL on failure.
 */
const u8 *usbh_composite_cdc_ecm_process_mac_str(void);


/**
 * @brief  Gets the MAC address of the connected device.
 * @return Pointer to the 6-byte MAC address array on success, NULL on failure.
 */
u8 usbh_composite_cdc_ecm_hex_to_char(u8 hex_num);


/**
 * @brief Notify that ecm prepare has been done
 * @return Pointer to the 6-byte MAC address array on success, NULL on failure.
 */
u8 usbh_composite_cdc_ecm_prepare_done(void);

/** @} End of Host_Composite_Functions group */
/** @} End of USB_Host_Functions group */
/** @} End of USB_Host_API group */

#endif  /* USBD_CDC_ECM_H */
