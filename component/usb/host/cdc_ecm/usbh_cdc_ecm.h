/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_CDC_ECM_H
#define USBH_CDC_ECM_H

/* Includes ------------------------------------------------------------------*/

#include "usbh.h"
#include "usb_cdc_ecm.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
        Example Hierarchy:
          LWIP (Network Stack)
            |
       ECM Class Driver
            |
       USB Core Driver
*/

/** @addtogroup USB_Host_API USB Host API
 *  @{
 */

/* Exported defines ----------------------------------------------------------*/

/** @addtogroup USB_Host_Constants USB Host Constants
 * @{
 */
/** @addtogroup Host_CDC_ECM_Constants Host CDC ECM Constants
 * @{
 */

#define USBH_CDC_ECM_STATE_TRACE_ENABLE                              (0)      /**< Enable or disable CDC ECM state trace logging. */

#define USBH_CDC_ECM_MAC_STR_LEN                                     (6)      /**< Length of the MAC address in bytes. */
#define USBH_CDC_ECM_CTRL_REG_BUF_LEN                                (4)      /**< Length of the ECM dongle control register buffer. */
#define USBH_CDC_ECM_MULTICAST_FILTER_STR_LEN                        (20)     /**< Length of the ECM multicast filter control buffer. */

/** @} End of Host_CDC_ECM_Constants group */
/** @} End of USB_Host_Constants group */

/* Exported types ------------------------------------------------------------*/

/** @addtogroup USB_Host_Types USB Host Types
 * @{
 */
/** @addtogroup Host_CDC_ECM_Types Host CDC ECM Types
 * @{
 */

/**
 * @brief Configuration for the CDC ECM host class.
 * @details Carries every CDC ECM host option: the MAC address to program into a Realtek
 *          dongle and the dongle LED pattern. Host-global parameters (speed, FIFO depths,
 *          main task stack and priority, ISR priority) live in @ref usbh_config_t instead.
 * @note  The structure is stored by pointer, but both arrays are copied at init, so the
 *        arrays themselves need not outlive the init call.
 */
typedef struct {
	const u16 *led_array;    /**< LED state array, NULL for none; each u16 element is one LED state. */
	const u8 *mac_value;     /**< MAC address to program, 6 bytes, NULL to keep the dongle default. */
	u8 led_cnt;              /**< Number of valid elements in `led_array`; 0 disables LED setup. */
} usbh_cdc_ecm_config_t;

/* CDC ECM user callback interface */
typedef struct {
	/**
	 * @brief Callback invoked during CDC ECM class driver initialization.
	 *        Used for application-specific resource setup.
	 * @return 0 on success, non-zero on failure.
	*/
	int (*init)(void);

	/**
	 * @brief Callback invoked during CDC ECM class driver de-initialization.
	 *        Used for application-specific resource cleanup.
	 */
	void (*deinit)(void);

	/**
	 * @brief Callback invoked when a device is attached.
	 *        Used to report the device connection status to the application.
	 */
	void (*attached)(void);

	/**
	 * @brief Callback invoked when a device is detached.
	 *        Used to report the device disconnection status to the application.
	 */
	void (*detached)(void);

	/**
	 * @brief Callback invoked when the device setup phase is complete.
	 *        Indicates that the device is ready for data transfer.
	 */
	void (*setup)(void);

	/**
	* @brief Callback invoked when a bulk IN data transfer (Receive) is complete.
	* @param[in] buf: Pointer to the buffer containing received data.
	* @param[in] len: Length of the valid received data in bytes.
	*/
	void (*received)(u8 *buf, u32 len);
} usbh_cdc_ecm_state_cb_t;

typedef struct {
	usbh_pipe_t pipe;                         /**< USB Host pipe handle. */
#if USBH_CDC_ECM_STATE_TRACE_ENABLE
	u32 trigger_cnt;                          /**< Debug trigger counter. */
#endif
	usbh_ep_desc_t ep_desc;                   /**< Endpoint descriptor (byte-packed). */
	u8 valid;                                 /**< Validity flag for this pipe info. */
} usbh_cdc_ecm_pipe_info_t;

/** @} End of Host_CDC_ECM_Types group */
/** @} End of USB_Host_Types group */

/**
 * @brief Structure representing the CDC ECM host instance.
 */
typedef struct {
	const usbh_cdc_ecm_state_cb_t  *cb;              /**< User callback structure */
	usb_host_t *host;                                /**< USB Host core handle */
	u8  *dongle_ctrl_buf;                            /**< Buffer for control transfers (cache aligned) */
	u16 *led_array;                                  /**< Pointer to LED array */

	/* Class state */
	u16 sub_status;                                  /**< ECM sub-status, see usbh_cdc_ecm_at_set_state_t */
	u16 intr_check_tick;                             /**< ECM Intr check tick, used to reduce the cpu load */
	u8 state;                                        /**< Internal state machine status, see usbh_cdc_ecm_state_t */
	__IO u8 eth_hw_connect;                          /**< Ethernet physical link status: 0=Disconnect, 1=Connect */
	u8 ready_to_xfer;                                /**< Upper-layer prepare-done gate: set via usbh_cdc_ecm_prepare_done() once the dongle (e.g. 4G AT config) is ready; SOF withholds data transfer until this is set. */
	u8 halt_ep_addr;                                 /**< Address of the halted endpoint being cleared in CDC_ECM_STATE_ERROR */
	u8 halt_retry_cnt;                               /**< Failed CLEAR_FEATURE(ENDPOINT_HALT) attempts for halt_ep_addr */

	/* Device identity and interfaces */
	u16 vid;                                         /**< Vendor ID */
	u16 pid;                                         /**< Product ID */
	u8 data_itf_id;                                  /**< Data Interface number */
	u8 data_alt_set;                                 /**< Alternate Setting value for Data Interface */
	u8 union_data_itf_id;                            /**< Data Interface number from Union Functional Descriptor, 0xFF if not found */

	/* Pipes */
	usbh_cdc_ecm_pipe_info_t intr_rx;                /**< Intr IN Endpoint Info */
	usbh_cdc_ecm_pipe_info_t bulk_tx;                /**< Bulk OUT Endpoint Info */
	usbh_cdc_ecm_pipe_info_t bulk_rx;                /**< Bulk IN Endpoint Info */

	/* BULK TX */
	usb_os_sema_t bulk_tx_sema;                      /**< Semaphore for BULK TX synchronization */
	__IO u8 bulk_tx_block;                           /**< Flag indicating BULK TX is blocked/busy */
	__IO u8 bulk_tx_err;                             /**< Set when the BULK TX a blocked sender waits for failed */
	__IO u8 tx_open;                                 /**< Set by init once the TX resources exist, cleared first by deinit */
	__IO u8 tx_users;                                /**< Tasks inside the TX API; deinit waits for 0 before freeing TX resources */

	/* Ethernet filters and statistics */
	u32 eth_statistic_count;                         /**< Feature selector parameter: Statistic count */
	u16 feature_selector;                            /**< Feature selector parameter */
	u16 packet_filter;                               /**< Packet filter configuration */
	u16 multicast_filter_len;                         /**< Length of multicast filter parameters */
	u8 multicast_filter[USBH_CDC_ECM_MULTICAST_FILTER_STR_LEN]; /**< Buffer for multicast filer control */

	/* MAC address, dongle registers and LED */
	u8 mac[USBH_CDC_ECM_MAC_STR_LEN];                /**< Buffer for saving MAC string */
	u8 mac_valid;                                    /**< Flag indicating if MAC address is valid */
	u8 mac_src_type;                                 /**< ECM dongle MAC source type, see usbh_cdc_ecm_dongle_mac_type_t */
	u8 iMACAddressStringId;                          /**< Index of the MAC address string descriptor */
	u8 mac_ctrl_lock[USBH_CDC_ECM_CTRL_REG_BUF_LEN]; /**< Buffer for RTL8152 MAC change control */
	u8 flow_ctrl[USBH_CDC_ECM_CTRL_REG_BUF_LEN];     /**< Buffer for RTL8152 flow control */
	u8 rcr[USBH_CDC_ECM_CTRL_REG_BUF_LEN];           /**< Buffer for RTL8156 RCR register */
	u8 led_cnt;                                      /**< Number of LEDs */
} usbh_cdc_ecm_host_t;

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @addtogroup USB_Host_Functions USB Host Functions
 * @{
 */
/** @addtogroup Host_CDC_ECM_Functions Host CDC ECM Functions
 * @{
 */

/**
 * @brief  Initializes the CDC ECM host class driver.
 * @param[in] cb: Pointer to the user-defined callback structure.
 * @param[in] cfg: Pointer to the class configuration, NULL to take every default.
 *                 See @ref usbh_cdc_ecm_config_t.
 * @return 0 (HAL_OK) on success, non-zero on failure.
 */
int usbh_cdc_ecm_init(const usbh_cdc_ecm_state_cb_t *cb, const usbh_cdc_ecm_config_t *cfg);

/**
 * @brief  De-initializes the CDC ECM host class driver and releases resources.
 */
void usbh_cdc_ecm_deinit(void);

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
 * @brief  Signal that the upper layer has finished all pre-transfer preparation
 *         (e.g. 4G dongle AT configuration) and Ethernet data transfer may start.
 *         Until this is called, SOF withholds bulk/intr transfer scheduling.
 */
void usbh_cdc_ecm_prepare_done(void);

/**
 * @brief  Transmits an Ethernet packet to the device via the Bulk OUT endpoint.
 * @param[in] buf: Pointer to the data buffer (Ethernet frame) to be transmitted.
 * @param[in] len: Length of the data in bytes.
 * @param[in] block: Blocking mode flag (e.g., 0 for non-blocking, else for blocking).
 * @note   Task context only, one sender at a time. Safe against a concurrent deinit of the class:
 *         returns HAL_ERR_HW while the class is not initialized or is being de-initialized.
 * @return 0 (HAL_OK) on success, non-zero on failure.
 */
int usbh_cdc_ecm_send_data(u8 *buf, u32 len, u8 block);

/**
 * @brief  Gets the current network link state (uplink status).
 * @return 1: Network link is up (connected to uplink).
 *         0: Network link is down (disconnected, or USB not yet enumerated).
 */
int usbh_cdc_ecm_get_link_status(void);

/**
 * @brief  Gets the MAC address of the connected device.
 * @return Pointer to the 6-byte MAC address array on success, NULL on failure.
 */
const u8 *usbh_cdc_ecm_process_mac_str(void);

/**
 * @brief  Gets the Vendor ID of the attached CDC ECM device.
 * @return VID on success, 0 if no device is attached.
 */
u16 usbh_cdc_ecm_get_device_vid(void);

/**
 * @brief  Gets the Product ID of the attached CDC ECM device.
 * @return PID on success, 0 if no device is attached.
 */
u16 usbh_cdc_ecm_get_device_pid(void);

/** @} End of Host_CDC_ECM_Functions group */
/** @} End of USB_Host_Functions group */

/** @} End of USB_Host_API group */

#ifdef __cplusplus
}
#endif

#endif /* USBH_CDC_ECM_H */
