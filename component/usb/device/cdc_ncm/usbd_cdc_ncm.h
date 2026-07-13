/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBD_CDC_NCM_H
#define USBD_CDC_NCM_H

/* Includes ------------------------------------------------------------------*/
#include "platform_autoconf.h"
#include "usbd.h"
#include "usb_cdc_ncm.h"
#include "usb_os.h"
#include "usb_ringbuf.h"

/* Exported defines ----------------------------------------------------------*/

/** @addtogroup USB_Device_API USB Device API
 *  @{
 */
/** @addtogroup USB_Device_Constants USB Device Constants
 * @{
 */
/** @addtogroup Device_CDC_NCM_Constants Device CDC NCM Constants
 * @{
 */

#define USBD_CDC_NCM_MAC_STR_LEN                           (6)      /**< Length of the MAC address in bytes. */

/* Set to 1 to enable the periodic state-trace thread (default off).
 * When enabled, a low-priority thread prints link/endpoint/TX-ring-buffer state,
 * per-path completion counters (including an RX thread heartbeat) and NTB/aggregation
 * stats once per USBD_CDC_NCM_TRACE_INTERVAL_MS so stalls can be diagnosed
 * without a debugger.  Zero-cost when disabled: every dbg_* counter field and
 * all instrumentation is compiled out. */
#define USBD_CDC_NCM_STATE_TRACE_ENABLE               0

/* Defines endpoint addresses for BULK and INTERRUPT transfers. */
#if defined (CONFIG_AMEBAGREEN2)
#define USBD_CDC_NCM_BULK_IN_EP                       0x82U
#define USBD_CDC_NCM_BULK_OUT_EP                      0x02U
#else
#define USBD_CDC_NCM_BULK_IN_EP                       0x81U
#define USBD_CDC_NCM_BULK_OUT_EP                      0x02U
#endif
#define USBD_CDC_NCM_INTR_IN_EP                       0x83U

/* Interface numbers */
#define USBD_CDC_NCM_COMM_INTERFACE_NUM               0x00U  /**< Communication interface */
#define USBD_CDC_NCM_DATA_INTERFACE_NUM               0x01U  /**< Data interface */

/* Number of ping-pong RX buffers used to decouple USB OUT from upper-layer RX. */
#define USBD_CDC_NCM_RX_BUF_NUM                       2U

/* Number of TX NTB ring slots (SPSC ring).
 * The ring keeps one empty gap between write and read index for lock-free
 * operation, so usable depth = DEPTH - 1.
 * Non-aggregation: DEPTH=3 (2 usable: one in-flight + one queued).
 * Aggregation:     DEPTH=4 (3 usable: one being filled + one in-flight + one queued). */
#ifndef USBD_CDC_NCM_TX_DEPTH
#ifdef CONFIG_USBD_CDC_NCM_TX_AGGREGATION
#define USBD_CDC_NCM_TX_DEPTH                      4U
#else
#define USBD_CDC_NCM_TX_DEPTH                      3U
#endif
#endif

#ifdef CONFIG_USBD_CDC_NCM_TX_AGGREGATION
/* Aggregation TX task ring buffer depth (raw ethernet frames queued from lwIP).
 * 4 nodes = enough to absorb one full NTB worth of frames (MAX_DATAGRAMS=2)
 * plus 2 slots of burst headroom, without wasting memory. */
#ifndef USBD_CDC_NCM_TX_RB_DEPTH
#define USBD_CDC_NCM_TX_RB_DEPTH                      4U
#endif
#endif

/** @} End of Device_CDC_NCM_Constants group*/
/** @} End of USB_Device_Constants group*/

/* Exported macros -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/** @addtogroup USB_Device_Types USB Device Types
 * @{
 */
/** @addtogroup Device_CDC_NCM_Types Device CDC NCM Types
 * @{
 */

/**
 * @brief USB CDC NCM Device Private Data Structure.
 */
typedef struct {
	const u8 *mac_value;     /**< Pointer to the MAC address buffer; typically points to a 6-byte physical address. */
} usbd_cdc_ncm_priv_data_t;

/**
 * @brief One TX NTB buffer slot in the SPSC ring.
 *
 * @details Each slot owns its own DMA-aligned NTB scratch buffer.  The single
 *          producer (lwIP task) fills slot[tx_wd] with NTH16+NDP16+frame and
 *          advances tx_wd; the single consumer (USB ISR: SOF and XFRC run
 *          serially in interrupt context, never concurrently) sends slot[tx_rd]
 *          and advances tx_rd after XFRC.  Because producer touches only the
 *          tx_wd slot and consumer touches only the tx_rd slot, and the ring
 *          keeps at least one empty gap between them, no lock is needed --
 *          tx_wd / tx_rd index isolation provides the mutual exclusion.
 */
typedef struct {
	u32 data_offset;        /**< Total NTB length (bytes) once finalised; DMA transfer length. */
	u8 frame_count;         /**< Datagrams aggregated into this NTB so far. */
	u16 sequence;           /**< NTH16 wSequence value for this NTB. */
	u8 *buf;                /**< DMA-aligned NTB scratch buffer (size USBD_CDC_NCM_NTB_TX_BUF_SIZE). */
} ncm_tx_ntb_t;

/**
 * @brief Structure containing callback functions for the CDC NCM class.
 * @details The user application should provide an instance of this structure
 *          to handle class-specific events.
 */
typedef struct {
	/**
	 * @brief Pointer to the USB CDC NCM private data structure.
	 */
	const usbd_cdc_ncm_priv_data_t *priv;

	/**
	 * @brief Called during CDC NCM class driver initialization to set up application resources.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* init)(void);

	/**
	 * @brief Called when the CDC NCM device is de-initialized for resource cleanup.
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
} usbd_cdc_ncm_cb_t;

/**
 * @brief Structure representing the CDC NCM device instance.
 */
typedef struct {
	usbd_ep_t ep_bulk_in;           /**< BULK IN endpoint. */
	usbd_ep_t ep_bulk_out;          /**< BULK OUT endpoint. */
	usbd_ep_t ep_intr_in;           /**< INTERRUPT IN endpoint. */

	usb_setup_req_t ctrl_req;       /**< Saved control request for EP0 OUT data phase. */

	ncm_tx_ntb_t tx_slot[USBD_CDC_NCM_TX_DEPTH]; /**< SPSC TX NTB ring slots. */
	usb_os_sema_t tx_buf_free_sema; /**< ISR -> producer: given each XFRC when a slot is freed. */
	__IO u8 tx_wd;                  /**< Producer write index (TX task / lwIP task only advances this). */
	__IO u8 tx_rd;                  /**< Consumer read index (USB ISR only advances this). */
	__IO u8 tx_inflight;            /**< 1 = slot[tx_rd] is currently being DMA'd; only USB ISR touches it. */

#ifdef CONFIG_USBD_CDC_NCM_TX_AGGREGATION
	/* Aggregation path: dedicated TX task reads raw frames from tx_raw_rb,
	 * aggregates them into NTB slots, and advances tx_wd.
	 * lwIP only calls usb_ringbuf_add_tail() and returns immediately. */
	usb_ringbuf_manager_t tx_raw_rb;    /**< Raw ethernet frame ring buffer (lwIP -> TX task). */
	usb_os_sema_t tx_raw_sema;          /**< lwIP -> TX task: given when a frame is enqueued. */
	usb_os_task_t tx_task;              /**< TX aggregation task handle. */
	__IO u8 tx_task_running;            /**< TX task loop guard; cleared to 0 to request exit. */
#else
	/* Non-aggregation path: lwIP task directly builds one-frame NTBs and
	 * advances tx_wd.  No extra task, no ringbuf, minimal latency. */
	__IO u8 tx_filling_busy;        /**< 1 = producer is mid-append on slot[tx_wd]; SOF backs off. */
	__IO u8 tx_wd_tick;             /**< SOF ticks since first frame appended to slot[tx_wd]. */
#endif

	usb_os_sema_t rx_data_ready_sema; /**< ISR -> thread: signals a buffer holds data. */
	usb_os_task_t rx_task;          /**< RX delivery thread handle. */

	u8 *rx_buf[USBD_CDC_NCM_RX_BUF_NUM];  /**< Ping-pong RX buffers (ISR and RxThread). */
	usb_dev_t *dev;                 /**< USB device instance. */
	const usbd_cdc_ncm_cb_t *cb;         /**< User-defined callback structure. */
	u8 *rx_msg_buf;                 /**< Buffer pointer handed to the RX thread for current data. */
	u32 rx_msg_len;                 /**< Data length handed to the RX thread. */
	__IO u32 rx_pending_len;       /**< Deferred data length waiting in rx_buf[rx_xfer_idx]. */

#if USBD_CDC_NCM_STATE_TRACE_ENABLE
	usb_os_task_t trace_task;       /**< State-trace thread handle; NULL when not running. */

	/* Debug counters - incremented in the relevant hot-paths and read by the
	 * trace thread.  u32 wraps gracefully; a frozen value flags a wedged path. */
	u32 dbg_tx_cnt;                 /**< usbd_cdc_ncm_transmit() call count. */
	u32 dbg_bulk_in_done_cnt;       /**< BULK IN completion ISR count. */
	u32 dbg_bulk_out_done_cnt;      /**< BULK OUT completion ISR count. */
	u32 dbg_intr_in_done_cnt;       /**< INTR IN completion ISR count. */
	u32 dbg_rx_deliver_cnt;         /**< NTBs delivered to upper-layer callback. */
	u32 dbg_rx_loop_cnt;            /**< RX thread heartbeat; frozen => thread wedged. */
	u32 dbg_tx_bytes;               /**< Bytes successfully submitted to BULK IN (speed calc). */
	u32 dbg_rx_bytes;               /**< Bytes delivered to upper layer via received() (speed calc). */

	u8 trace_task_running;          /**< State-trace thread loop guard; cleared to request exit. */
#endif

	u16 ntb_in_max_size;            /**< Max NTB input size (host->device), negotiated via SET_NTB_INPUT_SIZE. */
	u16 ntb_out_max_size;           /**< Max NTB output size (device->host). */
	u16 sequence;                   /**< NTB sequence number, incremented per transmitted NTB. */

	u8 mac[USBD_CDC_NCM_MAC_STR_LEN];    /**< Device MAC address (6 bytes). */

	__IO u8 connect_status;              /**< Current network connection state (1=connected, 0=disconnected).
	                                          ISR writes (status_changed/clear_config/setup);
	                                          task reads (transmit/set_link_status).  Must be volatile. */
	__IO u8 notify_state;                /**< Active notification type.
	                                          ISR writes (send_notification/SOF/setup) and
	                                          task writes (set_link_status).  Must be volatile. */
	u8 alt_setting;                 /**< Currently selected data alternate setting. */
	__IO u8 notify_retry;                /**< 1 when a notification send failed or is queued;
	                                          SOF handler (ISR) retries it.  Must be volatile. */
	u8 rx_xfer_idx;                 /**< Index of the buffer currently armed for USB OUT (0 or 1). */
	__IO u8 rx_buf_free;            /**< thread->ISR: 1=buffer available, 0=held by RX thread. */
	__IO u8 rx_thread_running;      /**< RX thread loop guard; cleared to 0 to request exit. */
	u8 ntb_format;                  /**< NTB format: 0=NTB16, currently only NTB16 supported. */
	u8 crc_mode;                    /**< CRC mode: 0=none, currently only no-CRC supported. */
} usbd_cdc_ncm_dev_t;

/** @} End of Device_CDC_NCM_Types group*/
/** @} End of USB_Device_Types group*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @addtogroup USB_Device_Functions USB Device Functions
 * @{
 */
/** @addtogroup Device_CDC_NCM_Functions Device CDC NCM Functions
 * @{
 */

/**
 * @brief Initializes class driver with application callback handler.
 * @param[in] cb: Pointer to the user-defined callback structure.
 * @return 0 on success, non-zero on failure.
 */
int usbd_cdc_ncm_init(const usbd_cdc_ncm_cb_t *cb);

/**
 * @brief De-initializes the CDC NCM class driver.
 * @return 0 on success, non-zero on failure.
 */
int usbd_cdc_ncm_deinit(void);

/**
 * @brief Transmits an ethernet frame to the host wrapped in an NTB16.
 * @param[in] buf: Pointer to the ethernet frame buffer to be transmitted.
 * @param[in] len: Length of the ethernet frame in bytes.
 * @param[in] block: Blocking mode flag (e.g., 0 for non-blocking, else for blocking).
 * @return 0 on success, non-zero on failure.
 */
int usbd_cdc_ncm_transmit(u8 *buf, u32 len, u8 block);

/**
 * @brief  Gets the current network link state (uplink status).
 * @return 1: Network link is up (connected to uplink).
 *         0: Network link is down (disconnected from uplink).
 */
int usbd_cdc_ncm_get_link_status(void);

/**
 * @brief  Reports the upper-layer network link state to the host (edge-triggered).
 * @details A NETWORK_CONNECTION notification is queued only when the state
 *          actually changes; calling repeatedly with the same value is a no-op.
 *          Call from the layer that owns the real uplink (e.g. Wi-Fi / cellular)
 *          on link up/down events.
 * @param[in] link_up: Non-zero when the link is up, zero when it is down.
 * @return 0 on success.
 */
int usbd_cdc_ncm_set_link_status(u8 link_up);

/** @} End of Device_CDC_NCM_Functions group */
/** @} End of USB_Device_Functions group */
/** @} End of USB_Device_API group */

#endif  /* USBD_CDC_NCM_H */
