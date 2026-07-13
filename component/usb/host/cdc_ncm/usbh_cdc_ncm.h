/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_CDC_NCM_H
#define USBH_CDC_NCM_H

/* Includes ------------------------------------------------------------------*/

#include "usbh.h"
#include "usb_cdc_ncm.h"

/*
        Example Hierarchy:
          LWIP (Network Stack)
            |
       NCM Class Driver
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
/** @addtogroup Host_CDC_NCM_Constants Host CDC NCM Constants
 * @{
 */

#define USBH_CDC_NCM_STATE_TRACE_ENABLE                    0        /**< Enable or disable NCM state trace logging. */

#define CDC_NCM_MAC_STR_LEN                                6U             /**< Length of the MAC address in bytes. */
#define CDC_NCM_CTRL_REG_BUF_LEN                           4U             /**< Length of the NCM dongle control register buffer. */

/* TX datagram aggregation dimensions (used by CONFIG_USBH_CDC_NCM_TX_AGGREGATION).
 * These define the layout of the aggregation bookkeeping members below, so they
 * live here with the type. The matching DMA buffer SIZE stays tunable in the .c. */
#define USBH_CDC_NCM_TX_AGG_MAX_DATAGRAMS                  16U      /**< Max frames coalesced per NTB. */
#define USBH_CDC_NCM_TX_AGG_NUM_BUF                        2U       /**< Ping-pong buffer count (fixed at 2). */

/** @} End of Host_CDC_NCM_Constants group*/
/** @} End of USB_Host_Constants group*/

/* Exported types ------------------------------------------------------------*/

/** @addtogroup USB_Host_Types USB Host Types
 * @{
 */
/** @addtogroup Host_CDC_NCM_Types Host CDC NCM Types
 * @{
 */

/**
 * @brief USB CDC NCM Host Private Data Structure.
 */
typedef struct {
	const u16 *led_array;    /**< Pointer to the LED status array */
	const u8 *mac_value;     /**< Pointer to the MAC address buffer */
	u8 led_cnt;        /**< LED count */
} usbh_cdc_ncm_priv_data_t;

/* NCM user callback interface */
typedef struct {
	/**
	 * @brief Callback invoked during NCM class driver initialization.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* init)(void);

	/**
	 * @brief Callback invoked during NCM class driver de-initialization.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* deinit)(void);

	/**
	 * @brief Callback invoked when a device is attached.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* attach)(void);

	/**
	 * @brief Callback invoked when a device is detached.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* detach)(void);

	/**
	 * @brief Callback invoked when the device setup phase is complete.
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
} usbh_cdc_ncm_state_cb_t;

typedef struct {
	usbh_pipe_t pipe;                         /**< USB Host pipe handle. */
	usbh_ep_desc_t ep_desc;                   /**< Endpoint descriptor. */
#if USBH_CDC_NCM_STATE_TRACE_ENABLE
	u32 trigger_cnt;                          /**< Debug trigger counter. */
#endif
	u8 valid;                                 /**< Validity flag for this pipe info. */
} usbh_cdc_ncm_pipe_info_t;

/** @} End of Host_CDC_NCM_Types group*/
/** @} End of USB_Host_Types group*/

/**
 * @brief Structure representing the CDC NCM host instance.
 */
typedef struct {
	usbh_cdc_ncm_pipe_info_t intr_rx;                /**< Intr IN Endpoint Info */
	usbh_cdc_ncm_pipe_info_t bulk_tx;                /**< Bulk OUT Endpoint Info */
	usbh_cdc_ncm_pipe_info_t bulk_rx;                /**< Bulk IN Endpoint Info */

	usb_cdc_ncm_ntb_parameters_t ntb_params;         /**< NTB parameters from device */

	usb_os_sema_t bulk_tx_sema;                      /**< Semaphore for BULK TX synchronization */
	const usbh_cdc_ncm_state_cb_t *cb;                     /**< User callback structure */
	usb_host_t *host;                                /**< USB Host core handle */
	u8 *dongle_ctrl_buf;                             /**< Buffer for control transfers (cache aligned) */
#if !defined(CONFIG_USBH_CDC_NCM_TX_AGGREGATION)
	u8 *tx_ntb_buf;                                  /**< DMA-safe TX NTB buffer (non-aggregation path), allocated in init */
#else
	u8 *tx_agg_buf[USBH_CDC_NCM_TX_AGG_NUM_BUF];    /**< DMA-safe TX aggregation ping-pong buffers, allocated in init */
#endif
	u32 ntb_in_max_size;                             /**< Max NTB input size (host-to-device) */
	u32 ntb_out_max_size;                            /**< Max NTB output size (device-to-host) */

#if defined(CONFIG_USBH_CDC_NCM_TX_AGGREGATION)
	/* ====================================================================
	 * TX datagram aggregation bookkeeping
	 * --------------------------------------------------------------------
	 * The DMA payload itself lives in two file-static, cache-line-aligned
	 * buffers in the .c (g_ncm_tx_agg_buf[2][SIZE]). The members below are
	 * pure metadata: they record what is currently in each buffer, and which
	 * one is being filled vs. transmitted. All of them are guarded by
	 * tx_agg_lock (producer task vs. TX-complete callback).
	 *
	 * --- Per-buffer NTB layout (b = 0 or 1) ----------------------------
	 *
	 *   g_ncm_tx_agg_buf[b]:
	 *
	 *   off=0                                   tx_agg_write_off[b]
	 *    |                                            |
	 *    v                                            v
	 *   +-------+----+--------+----+--------+- ... --+--------+----------+
	 *   |NTH16  |gap|datagram|gap |datagram|        |datagram| NDP16    |
	 *   |(12 B) |   |  #0    |    |  #1    |        |  #N-1  | (header  |
	 *   |       |   |        |    |        |        |        |  + table)|
	 *   +-------+----+--------+----+--------+- ... --+--------+----------+
	 *               ^
	 *               |
	 *      tx_agg_data_pos_idx[b][0]  ...  tx_agg_data_pos_idx[b][N-1]
	 *      tx_agg_pkt_len[b][0]       ...  tx_agg_pkt_len[b][N-1]
	 *
	 *   tx_agg_pkt_cnt[b]   = N    (number of datagrams currently appended)
	 *   tx_agg_write_off[b] = high-water mark; the next datagram lands here
	 *                        (after divisor alignment), and on finalize it
	 *                        also marks where the closing NDP16 is placed.
	 *   NTH16 / NDP16 are written by agg_finalize() right before launch;
	 *   agg_append() only deposits payload + records data_pos_idx[]/pkt_len[].
	 *
	 * --- Ping-pong & TX flow -------------------------------------------
	 *
	 *           tx_agg_fill_idx ------------> A    [ fill side ]
	 *                                        ---
	 *                                         B    [ may be in flight ]
	 *
	 *           tx_agg_xfer_busy = 0  -> wire idle
	 *           tx_agg_xfer_busy = 1  -> a buffer is on USB right now
	 *
	 *   Lwip: agg_send(eth_frame)
	 *      lock
	 *      append into buf[fill_idx]               (low-latency path)
	 *      if (xfer_busy == 0)
	 *          finalize buf[fill_idx]
	 *          submit -> xfer_busy = 1
	 *          fill_idx ^= 1; reset_buf(fill_idx)  (swap to the empty one)
	 *      else
	 *          (keep coalescing into buf[fill_idx])
	 *      unlock
	 *
	 *   tx-done: cb_bulk_send()
	 *      lock
	 *      xfer_busy = 0
	 *      if (buf[fill_idx] non-empty)
	 *          finalize + submit -> xfer_busy = 1; swap fill_idx
	 *      unlock
	 *
	 *   Result: idle link  -> 1 frame per NTB, single-frame latency.
	 *           busy link  -> as many frames as fit get coalesced into the
	 *                         buffer being filled while the other one is on
	 *                         the wire (true pipeline).
	 * ==================================================================== */
	usb_os_lock_t tx_agg_lock;                                                              /**< Producer vs. TX-complete lock. */
	u16 tx_agg_write_off[USBH_CDC_NCM_TX_AGG_NUM_BUF];                                      /**< High-water byte offset per buffer (next free slot / NDP placement). */
	u16 tx_agg_pkt_cnt[USBH_CDC_NCM_TX_AGG_NUM_BUF];                                        /**< Number of datagrams currently appended in each buffer. */
	u16 tx_agg_data_pos_idx[USBH_CDC_NCM_TX_AGG_NUM_BUF][USBH_CDC_NCM_TX_AGG_MAX_DATAGRAMS];/**< Byte offset of each datagram inside its buffer. */
	u16 tx_agg_pkt_len[USBH_CDC_NCM_TX_AGG_NUM_BUF][USBH_CDC_NCM_TX_AGG_MAX_DATAGRAMS];     /**< Byte length of each datagram. */
	u8 tx_agg_fill_idx;                                                                     /**< Index (0/1) of the buffer currently being filled. */
	__IO u8 tx_agg_xfer_busy;                                                               /**< 1 while a buffer is on the wire (USB OUT in progress). */
#endif

#if USBH_CDC_NCM_STATE_TRACE_ENABLE
	rtos_task_t trace_task;                          /**< Debug state-trace task handle. */
	u8 trace_task_init_flag;                         /**< Trace task created flag. */
#endif

	u16 ntb_sequence;                                /**< NTB sequence counter */
	u16 intr_check_tick;                             /**< NCM Intr check tick */
	u16 sub_status;                                  /**< NCM sub-status */
	u16 packet_filter;                               /**< Packet filter configuration */
	u16 vid;                                         /**< Vendor ID */
	u16 pid;                                         /**< Product ID */

	u8 mac[CDC_NCM_MAC_STR_LEN];                     /**< Buffer for saving MAC string */
	u8 mac_ctrl_lock[CDC_NCM_CTRL_REG_BUF_LEN];      /**< Buffer for MAC lock register readback */
	u8 rcr[CDC_NCM_CTRL_REG_BUF_LEN];                /**< RCR (Receive Configuration Register) value */

	u8 ctrl_itf_id;                                  /**< Communication (control) Interface number */
	u8 data_itf_id;                                  /**< Data Interface number */
	u8 data_alt_set;                                 /**< Alternate Setting value for Data Interface */
	u8 iMACAddressStringId;                          /**< Index of the MAC address string descriptor */
	u8 state;                                        /**< Internal state machine status */
	u8 mac_valid;                                    /**< Flag indicating if MAC address is valid */
	u8 mac_src_type;                                 /**< NCM dongle MAC source type */
	__IO u8 bulk_tx_block;                           /**< Flag indicating BULK TX is blocked/busy */
	__IO u8 eth_hw_connect;                          /**< Ethernet physical link status: 0=Disconnect, 1=Connect */
} usbh_cdc_ncm_host_t;

/* Exported functions --------------------------------------------------------*/

/** @addtogroup USB_Host_Functions USB Host Functions
 * @{
 */
/** @addtogroup Host_CDC_NCM_Functions Host CDC NCM Functions
 * @{
 */

/**
 * @brief  Initializes the CDC NCM host class driver.
 * @param[in] cb: Pointer to the user-defined callback structure.
 * @param[in] priv: Pointer to the user-defined private parameter structure.
 * @return 0 (HAL_OK) on success, non-zero on failure.
 */
int usbh_cdc_ncm_init(const usbh_cdc_ncm_state_cb_t *cb, const usbh_cdc_ncm_priv_data_t *priv);

/**
 * @brief  De-initializes the CDC NCM host class driver and releases resources.
 * @return 0 (HAL_OK) on success, non-zero on failure.
 */
int usbh_cdc_ncm_deinit(void);

/**
 * @brief  Selects the appropriate configuration index for the device.
 * @param[in] host: USB Host Handle.
 * @return 0 (HAL_OK) on success, non-zero on failure.
 */
int usbh_cdc_ncm_choose_config(usb_host_t *host);

/**
 * @brief  Validates the device configuration descriptor for CDC NCM support.
 * @note   Searches for both the NCM Communication Interface and the NCM Data
 *         Interface.  Both must be present for the device to be accepted as
 *         a valid CDC NCM device.
 * @param[in] host: USB Host Handle.
 * @return HAL_OK if both NCM interfaces are found, HAL_ERR_PARA otherwise.
 */
int usbh_cdc_ncm_check_config_desc(usb_host_t *host);

/**
 * @brief  Checks if the CDC NCM device is enumerated and configured on the USB bus.
 * @return 1: USB configured (ready for control/bulk transfer).
 *         0: Device not enumerated or not yet configured.
 */
u8 usbh_cdc_ncm_is_configured(void);

/**
 * @brief  Transmits an Ethernet packet to the device via the Bulk OUT endpoint.
 * @param[in] buf: Pointer to the data buffer (Ethernet frame) to be transmitted.
 * @param[in] len: Length of the data in bytes.
 * @param[in] block: Blocking mode flag (e.g., 0 for non-blocking, else for blocking).
 * @return 0 (HAL_OK) on success, non-zero on failure.
 */
int usbh_cdc_ncm_transmit(u8 *buf, u32 len, u8 block);

/**
 * @brief  Checks if the Ethernet link is up (USB enumerated + link active).
 * @return 1: Device enumerated and Ethernet link is up.
 *         0: Device not enumerated or Ethernet link is down.
 */
u8 usbh_cdc_ncm_get_link_status(void);

/**
 * @brief  Gets the MAC address of the connected device.
 * @return Pointer to the 6-byte MAC address array on success, NULL on failure.
 */
const u8 *usbh_cdc_ncm_get_mac_str(void);

/** @} End of Host_CDC_NCM_Functions group */
/** @} End of USB_Host_Functions group */
/** @} End of USB_Host_API group */

#endif  /* USBH_CDC_NCM_H */
