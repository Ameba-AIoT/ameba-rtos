/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_RNDIS_H
#define USBH_RNDIS_H

/* Includes ------------------------------------------------------------------*/

#include "usbh.h"
#include "usb_rndis.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
        Example Hierarchy:
          LWIP (Network Stack)
            |
       RNDIS Class Driver
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
/** @addtogroup Host_RNDIS_Constants Host RNDIS Constants
 * @{
 */

/* Default OID_GEN_CURRENT_PACKET_FILTER, used when @ref usbh_rndis_config_t::packet_filter is 0 */
#define USBH_RNDIS_DEFAULT_PACKET_FILTER   (USB_RNDIS_PACKET_TYPE_DIRECTED | USB_RNDIS_PACKET_TYPE_BROADCAST | USB_RNDIS_PACKET_TYPE_ALL_MULTICAST)

/* Capacity of the encapsulated message buffer: the largest response the class reads */
#define USBH_RNDIS_CTRL_BUF_SIZE           128U

/** @} End of Host_RNDIS_Constants group */
/** @} End of USB_Host_Constants group */

/* Exported types ------------------------------------------------------------*/

/** @addtogroup USB_Host_Types USB Host Types
 * @{
 */
/** @addtogroup Host_RNDIS_Types Host RNDIS Types
 * @{
 */

/**
 * @brief Configuration for the RNDIS host class.
 * @details Host-global parameters (speed, FIFO depths, main task stack and priority) live in
 *          @ref usbh_config_t instead.
 * @note  The structure is copied at init.
 */
typedef struct {
	u32 packet_filter;       /**< OID_GEN_CURRENT_PACKET_FILTER programmed into the device, 0 uses
	                              @ref USBH_RNDIS_DEFAULT_PACKET_FILTER. */
} usbh_rndis_config_t;

/* RNDIS user callback interface */
typedef struct {
	/**
	 * @brief Callback invoked during RNDIS class driver initialization.
	 * @return 0 on success, non-zero on failure.
	 */
	int (*init)(void);

	/**
	 * @brief Callback invoked during RNDIS class driver de-initialization.
	 */
	void (*deinit)(void);

	/**
	 * @brief Callback invoked when a device is attached.
	 */
	void (*attached)(void);

	/**
	 * @brief Callback invoked when a device is detached.
	 */
	void (*detached)(void);

	/**
	 * @brief Callback invoked once the RNDIS device is initialized and data may flow.
	 */
	void (*setup)(void);

	/**
	 * @brief Callback invoked for each received Ethernet frame.
	 * @note  Runs in the USB host task. buf is valid only until the callback returns.
	 * @param[in] buf: Ethernet frame.
	 * @param[in] len: Frame length in bytes.
	 */
	void (*received)(u8 *buf, u32 len);
} usbh_rndis_cb_t;

/**
 * @brief Pipe of the RNDIS host instance.
 */
typedef struct {
	usbh_pipe_t pipe;                         /**< USB Host pipe handle. */
	usbh_ep_desc_t ep_desc;                   /**< Endpoint descriptor (byte-packed). */
} usbh_rndis_pipe_info_t;

/**
 * @brief Structure representing the RNDIS host instance.
 */
typedef struct {
	const usbh_rndis_cb_t *cb;         /**< User callback structure */
	usb_host_t *host;                        /**< USB Host core handle */
	u8 *ctrl_buf;                            /**< Encapsulated message buffer (DMA aligned) */
	u8 *tx_buf;                              /**< BULK OUT buffer, the PACKET_MSG header and the frame (DMA aligned) */
	u32 packet_filter;                       /**< Packet filter programmed at setup */
	u32 request_id;                          /**< RequestId of the last command */
	u32 dev_max_xfer;                    /**< MaxTransferSize reported by the device */
	u16 cmd_len;                             /**< Length of the command built in ctrl_buf for the current setup step, 0 if none */

	/* Class state */
	u8 state;                                /**< Class state, see usbh_rndis_state_t */
	u8 setup_step;                           /**< Setup sequence step, see usbh_rndis_setup_step_t */
	u8 ctrl_step;                            /**< Control step in the transfer state, see usbh_rndis_ctrl_step_t */
	u8 resp_retry;                           /**< GET_ENCAPSULATED_RESPONSE polls left for the current command */
	__IO u8 link_up;                         /**< Media state reported by the device */
	__IO u8 resp_avail;                      /**< RESPONSE_AVAILABLE received on INTR IN, not read yet */
	u8 ctrl_itf_id;                          /**< Communication interface number, wIndex of the class requests */
	u8 data_itf_id;                          /**< Data interface number */
	u8 data_alt_set;                         /**< Data alternate setting carrying the BULK pair; non-zero until setup selects it */
	u8 halt_ep_addr;                         /**< Address of the halted endpoint being cleared */
	u8 halt_retry_cnt;                       /**< Failed CLEAR_FEATURE(ENDPOINT_HALT) attempts */
	u8 mac_valid;                            /**< mac holds the device MAC */
	u8 mac[USB_RNDIS_MAC_ADDR_LEN];          /**< MAC address read by OID_802_3_PERMANENT_ADDRESS */

	/* Pipes */
	usbh_rndis_pipe_info_t intr_rx;          /**< INTR IN pipe, optional */
	usbh_rndis_pipe_info_t bulk_tx;          /**< BULK OUT pipe */
	usbh_rndis_pipe_info_t bulk_rx;          /**< BULK IN pipe */

	/* BULK TX */
	usb_os_sema_t bulk_tx_sema;              /**< Given when the BULK OUT transfer ends */
	__IO u8 bulk_tx_block;                   /**< A sender waits on bulk_tx_sema */
	__IO u8 bulk_tx_err;                     /**< The BULK OUT transfer failed */
	__IO u8 tx_open;                         /**< Set by init once the TX resources exist, cleared first by deinit */
	__IO u8 tx_users;                        /**< Tasks inside the TX API; deinit waits for 0 */
} usbh_rndis_host_t;

/** @} End of Host_RNDIS_Types group */
/** @} End of USB_Host_Types group */

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @addtogroup USB_Host_Functions USB Host Functions
 * @{
 */
/** @addtogroup Host_RNDIS_Functions Host RNDIS Functions
 * @{
 */

/**
 * @brief  Initializes the RNDIS host class driver.
 * @param[in] cb: Pointer to the user-defined callback structure.
 * @param[in] cfg: Pointer to the class configuration, NULL to take every default.
 * @return 0 (RTK_SUCCESS) on success, non-zero on failure.
 */
int usbh_rndis_init(const usbh_rndis_cb_t *cb, const usbh_rndis_config_t *cfg);

/**
 * @brief  De-initializes the RNDIS host class driver and releases resources.
 * @note   Task context only, not the host task. Closes the TX gate first, then waits without an
 *         upper bound for every task inside usbh_rndis_transmit() to leave.
 */
void usbh_rndis_deinit(void);

/**
 * @brief  Selects the configuration carrying the RNDIS function.
 * @param[in] host: USB Host Handle.
 * @return 0 (RTK_SUCCESS) on success, non-zero on failure.
 */
int usbh_rndis_choose_config(usb_host_t *host);

/**
 * @brief  Validates the configuration descriptor of the connected device.
 * @param[in] host: USB Host Handle.
 * @return 0 (RTK_SUCCESS) when an RNDIS function is found, non-zero otherwise.
 */
int usbh_rndis_check_config(usb_host_t *host);

/**
 * @brief  Checks whether the RNDIS device is initialized and data may flow.
 * @return true if ready, false otherwise.
 */
bool usbh_rndis_is_ready(void);

/**
 * @brief  Transmits one Ethernet frame to the device as a REMOTE_NDIS_PACKET_MSG.
 * @param[in] buf: Ethernet frame, no alignment required: it is copied.
 * @param[in] len: Frame length in bytes, 1 to USB_RNDIS_MAX_FRAME_SIZE.
 * @param[in] block: 0 to return once the frame is submitted, else wait until the transfer completes.
 * @note   Task context only, one sender at a time. A frame is copied into the single TX buffer,
 *         so a non-blocking call fails with -RTK_ERR_BUSY while the previous one is in flight.
 * @return 0 on success, -RTK_ERR_BADARG for ISR context or a bad frame, -RTK_ERR_IO while the
 *         class is not ready or being de-initialized, other non-zero values on failure.
 */
int usbh_rndis_transmit(const u8 *buf, u32 len, u8 block);

/**
 * @brief  Gets the media state reported by the device.
 * @return true if the link is up, false if it is down or the class is not ready.
 */
bool usbh_rndis_get_link_status(void);

/**
 * @brief  Gets the MAC address of the device.
 * @return Pointer to the 6-byte MAC address, NULL if not known yet.
 */
const u8 *usbh_rndis_get_mac(void);

/** @} End of Host_RNDIS_Functions group */
/** @} End of USB_Host_Functions group */

/** @} End of USB_Host_API group */

#ifdef __cplusplus
}
#endif

#endif /* USBH_RNDIS_H */
