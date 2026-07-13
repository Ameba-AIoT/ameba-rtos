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
#include "usb_ringbuf.h"

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

#define USBD_CDC_ECM_MAC_STR_LEN                           (6)      /**< Length of the MAC address in bytes. */

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

/* Number of ping-pong RX buffers used to decouple USB OUT from upper-layer RX. */
#define USBD_CDC_ECM_RX_BUF_NUM                       2U

/* Set to 1 to enable the periodic state-trace thread (default off).
 * When enabled, a low-priority thread prints link/endpoint/TX-ring-buffer state,
 * per-path completion counters (including an RX thread heartbeat) and TX/RX
 * throughput once per USBD_CDC_ECM_TRACE_INTERVAL_MS so stalls can be diagnosed
 * without a debugger.  Zero-cost when disabled: every dbg_* counter field and
 * all instrumentation is compiled out. */
#define USBD_CDC_ECM_STATE_TRACE_ENABLE               0

/** @} End of Device_CDC_ECM_Constants group */
/** @} End of USB_Device_Constants group */

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
	const u8 *mac_value;     /**< Pointer to the MAC address buffer; typically points to a 6-byte physical address. */
} usbd_cdc_ecm_priv_data_t;

/**
 * @brief Structure containing callback functions for the CDC ECM class.
 * @details The user application should provide an instance of this structure
 *          to handle class-specific events.
 */
typedef struct {
	/**
	 * @brief Pointer to the USB CDC ECM private data structure.
	 */
	const usbd_cdc_ecm_priv_data_t *priv;

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
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
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
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 * @param[in] old_status: The previous attach status.
	 * @param[in] status: The new attach status.
	 */
	void (*status_changed)(u8 old_status, u8 status);
} usbd_cdc_ecm_cb_t;

/**
 * @brief Structure representing the CDC ECM device instance.
 */
typedef struct {
	u8 *rx_buf[USBD_CDC_ECM_RX_BUF_NUM];  /**< Ping-pong RX buffers (ISR and RxThread). */

	usb_setup_req_t ctrl_req;   /**< Saved control request for EP0 OUT data phase. */
	usbd_ep_t ep_bulk_in;       /**< BULK IN endpoint. */
	usbd_ep_t ep_bulk_out;      /**< BULK OUT endpoint. */
	usbd_ep_t ep_intr_in;       /**< INTERRUPT IN endpoint. */

	usb_ringbuf_manager_t bulk_tx_rb;   /**< TX ring buffer: queues up to USBD_CDC_ECM_BULK_TX_RB_SIZE frames. */
	u8 *bulk_tx_dma_buf;                /**< DMA source for BULK IN. A frame is copied here from the ring buffer
	                                         head (usb_ringbuf_remove_head) before each transfer, so the DMA never
	                                         reads ring buffer node memory directly and we touch no ring buffer
	                                         internals. Size = USBD_CDC_ECM_BULK_BUF_MAX_SIZE, cache-line aligned. */
	usb_os_sema_t bulk_tx_slot_sema;    /**< ISR -> tcpip: given each time a ring buffer slot is freed (head advance). */
	usb_os_task_t rx_task;              /**< RX delivery thread handle. */
	usb_os_sema_t rx_data_ready_sema;   /**< ISR -> thread: signals a buffer holds a frame. */

	usb_dev_t *dev;        /**< USB device instance. */
	const usbd_cdc_ecm_cb_t *cb; /**< User-defined callback structure. */
	u8 *rx_msg_buf;        /**< Buffer pointer handed to the RX thread for current frame. */

	u32 rx_msg_len;             /**< Frame length handed to the RX thread. */
	__IO u32 rx_pending_len;    /**< Deferred frame length waiting in rx_buf[rx_xfer_idx]. */

#if USBD_CDC_ECM_STATE_TRACE_ENABLE
	usb_os_task_t trace_task;   /**< State-trace thread handle; NULL when not running. */

	/* Debug counters - incremented in the relevant hot-paths and read by the
	 * trace thread.  u32 wraps gracefully; a frozen value flags a wedged path. */
	u32 dbg_tx_cnt;             /**< usbd_cdc_ecm_transmit() call count. */
	u32 dbg_bulk_in_done_cnt;   /**< BULK IN completion ISR count. */
	u32 dbg_bulk_out_done_cnt;  /**< BULK OUT completion ISR count. */
	u32 dbg_intr_in_done_cnt;   /**< INTR IN completion ISR count. */
	u32 dbg_rx_deliver_cnt;     /**< Frames delivered to upper-layer callback. */
	u32 dbg_rx_loop_cnt;        /**< RX thread heartbeat; frozen => thread wedged. */
	u32 dbg_tx_bytes;           /**< Bytes successfully submitted to BULK IN (speed calc). */
	u32 dbg_rx_bytes;           /**< Bytes delivered to upper layer via received() (speed calc). */

	u8 trace_task_running;      /**< State-trace thread loop guard; cleared to request exit. */
#endif

	u8 mac[USBD_CDC_ECM_MAC_STR_LEN];          /**< Device MAC address (6 bytes). */

	u8 connect_status;          /**< Current network connection state (1=connected, 0=disconnected). */
	u8 notify_state;            /**< Active notification type, see usbd_cdc_ecm_notify_state. */
	u8 alt_setting;             /**< Currently selected data alternate setting. */
	u8 mac_valid;               /**< 1 when mac[] holds a valid address. */
	u8 mac_src_type;            /**< MAC source, see usbd_cdc_ecm_dongle_mac_type_t. */
	u8 rx_xfer_idx;             /**< Index of the buffer currently armed for USB OUT (0 or 1). */
	u8 notify_retry;            /**< 1 when a notification is pending (queued or send failed); SOF handler (re)sends it. */
	__IO u8 rx_buf_free;        /**< thread->ISR: 1=buffer available, 0=held by RX thread. */
	__IO u8 rx_thread_running;  /**< RX thread loop guard; cleared to 0 to request exit. */
} usbd_cdc_ecm_dev_t;

/** @} End of Device_CDC_ECM_Types group */
/** @} End of USB_Device_Types group */

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
 * @param[in] cb: Pointer to the user-defined callback structure.
 * @return 0 on success, non-zero on failure.
 */
int usbd_cdc_ecm_init(const usbd_cdc_ecm_cb_t *cb);

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
 * @return 1: Device is connected to the host.
 *         0: Device is disconnected.
 */
int usbd_cdc_ecm_get_connect_status(void);

/**
 * @brief  Reports the upper-layer network link state to the host (edge-triggered).
 * @details A NETWORK_CONNECTION notification is queued only when the state
 *          actually changes; calling repeatedly with the same value is a no-op.
 *          Call from the layer that owns the real uplink (e.g. Wi-Fi / cellular)
 *          on link up/down events.
 * @param[in] link_up: Non-zero when the link is up, zero when it is down.
 * @return 0 on success.
 */
int usbd_cdc_ecm_set_link_status(u8 link_up);

/**
 * @brief  Gets the MAC address of the device.
 * @return Pointer to the 6-byte MAC address array on success, NULL on failure.
 */
const u8 *usbd_cdc_ecm_get_mac_str(void);

/** @} End of Device_CDC_ECM_Functions group */
/** @} End of USB_Device_Functions group */
/** @} End of USB_Device_API group */

#endif  /* USBD_CDC_ECM_H */
