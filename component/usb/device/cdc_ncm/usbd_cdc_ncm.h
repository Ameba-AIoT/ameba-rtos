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

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup USB_Device_API USB Device API
 *  @{
 */

/* Exported defines ----------------------------------------------------------*/

/** @addtogroup USB_Device_Constants USB Device Constants
 * @{
 */
/** @addtogroup Device_CDC_NCM_Constants Device CDC NCM Constants
 * @{
 */

#define USBD_CDC_NCM_MAC_STR_LEN        (6)      /**< Length of the MAC address in bytes. */

/* Defaults applied when the matching @ref usbd_cdc_ncm_config_t field is left 0. */
#define USBD_CDC_NCM_DEFAULT_INTR_IN_INTERVAL         8U     /**< Default INTR IN bInterval. */
#define USBD_CDC_NCM_DEFAULT_NTB_IN_SIZE              4096U  /**< Default dwNtbInMaxSize. */
#define USBD_CDC_NCM_DEFAULT_NTB_OUT_SIZE             4096U  /**< Default dwNtbOutMaxSize. */
#define USBD_CDC_NCM_DEFAULT_NTB_OUT_MAX_DATAGRAMS    1U     /**< Default wNtbOutMaxDatagrams. */
#define USBD_CDC_NCM_DEFAULT_BULK_TX_TIMEOUT_MS       20U    /**< Default blocking-transmit timeout in ms. */
#define USBD_CDC_NCM_DEFAULT_RX_TASK_STACK_SIZE       1024U  /**< Default RX delivery task stack size in bytes. */
#define USBD_CDC_NCM_DEFAULT_RX_TASK_PRIORITY         6U     /**< Default RX delivery task priority. */

#ifdef CONFIG_USBD_CDC_NCM_TX_AGGREGATION
/* Aggregation build: one slot is being filled while a second is in flight and a third queued,
 * and 2 datagrams per NTB halve the per-frame NTB overhead at ~3 KB per slot buffer. */
#define USBD_CDC_NCM_DEFAULT_TX_NTB_DEPTH             4U     /**< Default TX NTB ring slot count. */
#define USBD_CDC_NCM_DEFAULT_NTB_IN_MAX_DATAGRAMS     2U     /**< Default datagrams aggregated per IN NTB. */
#define USBD_CDC_NCM_DEFAULT_TX_RAW_QUEUE_DEPTH       5U     /**< Default raw frame queue depth (lwIP -> TX task); the head
                                                                   frame stays queued until it is copied into an NTB. */
#define USBD_CDC_NCM_DEFAULT_TX_TASK_STACK_SIZE       1024U  /**< Default TX aggregation task stack size in bytes. */
#define USBD_CDC_NCM_DEFAULT_TX_TASK_PRIORITY         5U     /**< Default TX aggregation task priority. */
#define USBD_CDC_NCM_DEFAULT_TX_SEMA_TIMEOUT_MS       1000U  /**< Default TX task safety-net wake-up period in ms; the task
                                                                   is woken by every enqueue, this only bounds a lost signal. */
#else
/* Non-aggregation: the lwIP task builds one-frame NTBs itself, so a depth of 3 (one in flight,
 * one queued) is enough and only one datagram per NTB is ever emitted. */
#define USBD_CDC_NCM_DEFAULT_TX_NTB_DEPTH             3U     /**< Default TX NTB ring slot count. */
#define USBD_CDC_NCM_DEFAULT_NTB_IN_MAX_DATAGRAMS     1U     /**< Datagrams per IN NTB; always 1 without aggregation. */
#endif

/* Minimum TX NTB ring depth: the SPSC ring keeps one empty gap between the write and read
 * index, so a depth of 2 is the smallest that leaves one usable slot. */
#define USBD_CDC_NCM_MIN_TX_NTB_DEPTH                 2U

/* Set to 1 to enable the periodic state-trace thread (default off).
 * When enabled, a low-priority thread prints link/endpoint/TX-ring-buffer state,
 * per-path completion counters (including an RX thread heartbeat) and NTB/aggregation
 * stats once per USBD_CDC_NCM_TRACE_INTERVAL_MS so stalls can be diagnosed
 * without a debugger.  Zero-cost when disabled: every dbg_* counter field and
 * all instrumentation is compiled out. */
#define USBD_CDC_NCM_STATE_TRACE_ENABLE 0

/* Number of ping-pong RX buffers used to decouple USB OUT from upper-layer RX. */
#define USBD_CDC_NCM_RX_BUF_NUM         2U

/** @} End of Device_CDC_NCM_Constants group*/
/** @} End of USB_Device_Constants group*/

/* Exported types ------------------------------------------------------------*/

/** @addtogroup USB_Device_Types USB Device Types
 * @{
 */
/** @addtogroup Device_CDC_NCM_Types Device CDC NCM Types
 * @{
 */

/**
 * @brief Configuration for the CDC NCM class.
 * @details Carries every CDC NCM option: the endpoint allocation, which the top-level
 *          application owns, plus the MAC address, the advertised NTB parameters and the
 *          TX/RX resource sizing. Used by both standalone and composite modes. Device-global
 *          parameters (VID/PID, strings, power attributes) live in @ref usbd_dev_info_t
 *          instead, and application-level options such as hotplug or the network configuration
 *          belong in the example's own configuration structure.
 * @note  The structure is stored by pointer, not copied, so it shall remain valid until
 *        @ref usbd_cdc_ncm_deinit.
 */
typedef struct {
	/**
	 * @brief MAC address reported to the host, 6 bytes, NULL for the class default.
	 * @details Copied into the class at init and formatted into the iMACAddress string,
	 *          so the buffer itself needs not outlive the init call.
	 */
	const u8 *mac;
	const char *itf_str;        /**< iInterface string of the communication interface, NULL for none. */
	/**
	 * @brief dwNtbInMaxSize advertised in GET_NTB_PARAMETERS, 0 uses USBD_CDC_NCM_DEFAULT_NTB_IN_SIZE.
	 * @details Upper bound on what SET_NTB_INPUT_SIZE may negotiate, so it shall be at least
	 *          USB_CDC_NCM_MIN_NTB_INPUT_SIZE. It may exceed what one TX slot buffer holds:
	 *          a device may always emit NTBs smaller than the maximum it advertises, and the
	 *          class clamps every NTB to the slot capacity that `ntb_in_max_datagrams` sets.
	 */
	u32 ntb_in_max_size;
	/**
	 * @brief dwNtbOutMaxSize advertised in GET_NTB_PARAMETERS, 0 uses USBD_CDC_NCM_DEFAULT_NTB_OUT_SIZE.
	 * @details Also sizes the two ping-pong RX buffers, because the device shall be able to
	 *          receive a whole max-size OUT NTB in one transfer.
	 */
	u32 ntb_out_max_size;
	u16 bulk_tx_timeout_ms;     /**< Blocking-transmit timeout in ms, 0 uses USBD_CDC_NCM_DEFAULT_BULK_TX_TIMEOUT_MS. */
	u16 rx_task_stack_size;     /**< RX delivery task stack size in bytes, 0 uses USBD_CDC_NCM_DEFAULT_RX_TASK_STACK_SIZE. */
	u16 tx_task_stack_size;     /**< TX aggregation task stack size in bytes, 0 uses USBD_CDC_NCM_DEFAULT_TX_TASK_STACK_SIZE.
	                                 Ignored unless `CONFIG_USBD_CDC_NCM_TX_AGGREGATION`. */
	u16 tx_sema_timeout_ms;     /**< TX aggregation task safety-net wake-up period in ms, 0 uses USBD_CDC_NCM_DEFAULT_TX_SEMA_TIMEOUT_MS.
	                                 Ignored unless `CONFIG_USBD_CDC_NCM_TX_AGGREGATION`. */
	u8  bulk_in_addr;           /**< BULK IN endpoint address (e.g. 0x81). */
	u8  bulk_out_addr;          /**< BULK OUT endpoint address (e.g. 0x01). */
	u8  intr_in_addr;           /**< INTERRUPT IN endpoint address (e.g. 0x83). */
	u8  intr_in_interval_hs;    /**< INTR IN bInterval at High Speed, 0 uses USBD_CDC_NCM_DEFAULT_INTR_IN_INTERVAL. */
	u8  intr_in_interval_fs;    /**< INTR IN bInterval at Full Speed, 0 uses USBD_CDC_NCM_DEFAULT_INTR_IN_INTERVAL. */
	/**
	 * @brief Number of TX NTB ring slots, 0 uses @ref USBD_CDC_NCM_DEFAULT_TX_NTB_DEPTH.
	 * @details The SPSC ring keeps one empty gap between the write and the read index, so the
	 *          usable depth is one less than this and the minimum accepted value is
	 *          @ref USBD_CDC_NCM_MIN_TX_NTB_DEPTH. Each slot owns one NTB buffer, whose size
	 *          `ntb_in_max_datagrams` determines.
	 */
	u8  tx_ntb_depth;
	/**
	 * @brief Ethernet frames aggregated into one device-to-host (IN) NTB, 0 uses
	 *        @ref USBD_CDC_NCM_DEFAULT_NTB_IN_MAX_DATAGRAMS.
	 * @details "IN" is the USB direction (Ref NCM 1.0: IN = device-to-host), so this is purely a
	 *          property of what the device emits; it is not the `ntb_out_max_datagrams` limit
	 *          advertised for the opposite direction.
	 *
	 *          It sizes both the NDP16 directory a slot reserves, 8 + (value + 1) * 4 bytes, and
	 *          the slot buffer itself, which holds that directory plus `value` max-size frames.
	 *          A higher value lowers the per-frame NTB overhead and the number of USB transfers
	 *          a small-packet burst costs, at a linearly growing heap cost: ~3 KB per slot at 2,
	 *          ~6 KB at 4. Ignored unless `CONFIG_USBD_CDC_NCM_TX_AGGREGATION`: without it the
	 *          lwIP task builds one-frame NTBs and the class forces 1.
	 */
	u8  ntb_in_max_datagrams;
	/**
	 * @brief wNtbOutMaxDatagrams advertised in GET_NTB_PARAMETERS, 0 uses
	 *        @ref USBD_CDC_NCM_DEFAULT_NTB_OUT_MAX_DATAGRAMS.
	 * @details How many datagrams the host may pack into one host-to-device (OUT) NTB
	 *          (Ref NCM 1.0 Table 6-3). Purely advertised: the RX parser walks every NDP entry
	 *          and bounds-checks each datagram, so the real limit is `ntb_out_max_size`.
	 */
	u8  ntb_out_max_datagrams;
	u8  tx_raw_queue_depth;     /**< Raw frame queue depth (lwIP -> TX task), 0 uses USBD_CDC_NCM_DEFAULT_TX_RAW_QUEUE_DEPTH.
	                                 Ignored unless `CONFIG_USBD_CDC_NCM_TX_AGGREGATION`. */
	u8  rx_task_priority;       /**< RX delivery task priority, 0 uses USBD_CDC_NCM_DEFAULT_RX_TASK_PRIORITY. */
	u8  tx_task_priority;       /**< TX aggregation task priority, 0 uses USBD_CDC_NCM_DEFAULT_TX_TASK_PRIORITY.
	                                 Ignored unless `CONFIG_USBD_CDC_NCM_TX_AGGREGATION`. */
} usbd_cdc_ncm_config_t;

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
	u8 *buf;                /**< DMA-aligned NTB scratch buffer (size usbd_cdc_ncm_dev_t::ntb_tx_buf_size). */
	u32 data_offset;        /**< Total NTB length (bytes) once finalised; DMA transfer length. */
	u16 sequence;           /**< NTH16 wSequence value for this NTB. */
	u8 frame_count;         /**< Datagrams aggregated into this NTB so far. */
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
	/**
	 * @brief Called during CDC NCM class driver initialization to set up application resources.
	 * @return 0 on success, non-zero on failure.
	 */
	int (*init)(void);

	/**
	 * @brief Called when the CDC NCM device is de-initialized for resource cleanup.
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
	 * @param[in] buf: Pointer to the received data buffer.
	 * @param[in] len: Length of the received data in bytes.
	 * @return 0 on success, non-zero on failure.
	 */
	int (*received)(u8 *buf, u32 len);

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
	const usbd_cdc_ncm_config_t *cfg;    /**< Pointer to the class configuration (set by init). */
	const usbd_cdc_ncm_cb_t *cb;         /**< User-defined callback structure. */
	usb_dev_t *dev;                 /**< USB device instance. */

	/* Interface and link state */
	__IO u8 data_alt_setting;       /**< Alternate setting currently selected for the DATA interface:
	                                     0 = default setting, no endpoints; 1 = BULK IN/OUT initialised.
	                                     Ref NCM 1.0 3.1 / USB 2.0 9.4.10: alt 0 has bNumEndpoints = 0, so the
	                                     BULK pipes exist only in alt 1.  Written only by
	                                     usbd_cdc_ncm_data_alt_start()/_stop(), which keep it in lockstep with
	                                     the real endpoint state.  It therefore serves both purposes at once:
	                                     the value GET_INTERFACE reports, and the "BULK endpoints are alive"
	                                     predicate guarding every TX/RX submission.
	                                     Deliberately distinct from connect_status, which is the link state
	                                     reported to the host and also depends on the uplink state.
	                                     The communication interface has alt 0 only, so it needs no field -
	                                     GET_INTERFACE answers it with a literal 0.
	                                     ISR writes (setup/clear_config/status_changed); the lwIP and TX
	                                     aggregation tasks read it, hence volatile. */
	__IO u8 connect_status;              /**< Link state last reported to the host by NETWORK_CONNECTION: 1 only
	                                          while the data interface is in alt 1 and the uplink is up.
	                                          Written only in USB ISR context (setup/SOF/clear_config/
	                                          status_changed); tasks only read it.  Must be volatile. */
	__IO u8 uplink_status;               /**< Upper-layer (uplink) link state set by usbd_cdc_ncm_set_link_status(),
	                                          1=up, 0=down.  Written only by that task-context API, only read by
	                                          the USB ISR, so it needs no lock.  Starts at 0 on every init. */
	__IO u8 notify_state;                /**< Active notification type.  Written only in USB ISR context. */
	__IO u8 notify_retry;                /**< 1 when a notification send failed or is queued;
	                                          SOF handler (ISR) retries it.  Must be volatile. */
	u8 ctrl_req_pending;            /**< 1 if ctrl_req is waiting for its EP0 OUT data stage. */
	u8 from_composite;              /**< Flag indicating if part of a composite device. */
	u8 if_base;                     /**< First interface number of this class; 0 in standalone mode
	                                     unless the composite framework rebases it via
	                                     set_interface_base(). */

	/* TX NTB ring.
	 * An NTB slot is always finalised and published before the producer returns:
	 * the non-aggregation path builds one-frame NTBs in the lwIP task, and the
	 * aggregation TX task stops appending as soon as the raw ring runs empty.
	 * No slot is therefore ever left partially filled, so the TX path needs no
	 * flush timeout or fill-in-progress state. */
	ncm_tx_ntb_t *tx_slot;          /**< SPSC TX NTB ring, tx_ntb_depth slots allocated at init. */
	usb_os_sema_t tx_buf_free_sema; /**< ISR -> producer: given each XFRC when a slot is freed. */
	__IO u8 tx_wd;                  /**< Producer write index (TX task / lwIP task only advances this). */
	__IO u8 tx_rd;                  /**< Consumer read index (USB ISR only advances this). */
	__IO u8 tx_inflight;            /**< 1 = slot[tx_rd] is currently being DMA'd; only USB ISR touches it. */
	u8 tx_ntb_depth;                /**< Resolved TX NTB ring slot count (tx_slot[] length). */
	__IO u8 is_closing;             /**< Set by deinit: transmit() rejects new callers before the TX
	                                     resources are released.  Written under the USB critical section. */
	__IO u8 xfer_users;             /**< Number of tasks inside usbd_cdc_ncm_transmit(); deinit waits for
	                                     it to drop to 0.  Updated under the USB critical section. */

	/* TX NTB layout, derived once at init from usbd_cdc_ncm_config_t::ntb_in_max_datagrams.
	 * Held as fields rather than recomputed so the aggregation hot path does no arithmetic,
	 * and so the slot buffers and the offsets written into them can never disagree. */
	u32 agg_data_offset;            /**< Offset of the first datagram in a slot, i.e. NTH16 + NDP16
	                                     directory rounded up to the datagram alignment. */
	u32 ntb_tx_buf_size;            /**< Allocated size of one slot buffer: agg_data_offset plus
	                                     ntb_in_max_datagrams aligned max-size frames. */
	u16 sequence;                   /**< NTB sequence number, incremented per transmitted NTB. */
	u8 ntb_in_max_datagrams;        /**< Resolved datagrams per IN NTB; always 1 without aggregation. */

#ifdef CONFIG_USBD_CDC_NCM_TX_AGGREGATION
	/* Aggregation path: dedicated TX task reads raw frames from tx_raw_rb,
	 * aggregates them into NTB slots, and advances tx_wd.
	 * lwIP only calls usb_ringbuf_add_tail() and returns immediately.
	 * Without this option the lwIP task builds one-frame NTBs itself, so none
	 * of the fields below exist: no extra task, no ringbuf, minimal latency. */
	usb_ringbuf_manager_t tx_raw_rb;    /**< Raw ethernet frame ring buffer (lwIP -> TX task). */
	usb_os_sema_t tx_raw_sema;          /**< lwIP -> TX task: given when a frame is enqueued. */
	usb_os_sema_t tx_raw_free_sema;     /**< TX task -> lwIP: given when a raw frame slot is freed. */
	usb_os_task_t tx_task;              /**< TX aggregation task handle. */
	__IO u8 tx_task_running;            /**< TX aggregation task loop guard; cleared to 0 to request exit. */
#endif

	/* RX ping-pong */
	usb_os_sema_t rx_data_ready_sema; /**< ISR -> thread: signals a buffer holds data. */
	usb_os_task_t rx_task;          /**< RX delivery thread handle. */
	u8 *rx_buf[USBD_CDC_NCM_RX_BUF_NUM];  /**< Ping-pong RX buffers (ISR and RxThread). */
	u8 *rx_msg_buf;                 /**< Buffer pointer handed to the RX thread for current data. */
	u32 rx_msg_len;                 /**< Data length handed to the RX thread. */
	__IO u32 rx_pending_len;        /**< Deferred data length waiting in rx_buf[rx_xfer_idx]. */
	u32 rx_buf_size;                /**< Allocated size of one RX ping-pong buffer: a whole max-size
	                                     OUT NTB, rounded up to a whole number of HS bulk packets. */
	u8 rx_xfer_idx;                 /**< Index of the buffer currently armed for USB OUT (0 or 1). */
	__IO u8 rx_buf_free;            /**< thread->ISR: 1=buffer available, 0=held by RX thread. */
	__IO u8 rx_thread_running;      /**< RX thread loop guard; cleared to 0 to request exit. */

	/* NTB parameters.
	 * dwNtbInMaxSize / dwNtbOutMaxSize are 32-bit fields in GET_NTB_PARAMETERS
	 * (Ref NCM 1.0 Table 6-3), so they must be stored as u32.  A u16 would also
	 * silently truncate the value carried in the SET_NTB_INPUT_SIZE data stage. */
	u32 ntb_in_max_size;            /**< Max NTB the device may send to the host (IN),
	                                     negotiated via SET_NTB_INPUT_SIZE.  Also caps the TX aggregation length. */
	u32 ntb_out_max_size;           /**< Max NTB the host may send to the device (OUT). */
	/**
	 * @brief dwNtbInMaxSize the device advertises, i.e. the resolved
	 *        usbd_cdc_ncm_config_t::ntb_in_max_size.
	 * @details Ref NCM 1.0 6.2.1: GET_NTB_PARAMETERS reports the device's fixed capability,
	 *          not the value last negotiated by SET_NTB_INPUT_SIZE, so this is kept separate
	 *          from ntb_in_max_size and also serves as the upper bound that request accepts.
	 */
	u32 ntb_in_max_size_cap;
	u8 ntb_format;                  /**< NTB format: 0=NTB16, currently only NTB16 supported. */
	u8 crc_mode;                    /**< CRC mode: 0=none, currently only no-CRC supported. */

	/* Endpoints and EP0 */
	usbd_ep_t ep_bulk_in;           /**< BULK IN endpoint. */
	usbd_ep_t ep_bulk_out;          /**< BULK OUT endpoint. */
	usbd_ep_t ep_intr_in;           /**< INTERRUPT IN endpoint. */
	usb_setup_req_t ctrl_req;       /**< Saved control request for EP0 OUT data phase. */

	/* MAC address and strings */
	u8 mac[USBD_CDC_NCM_MAC_STR_LEN];    /**< Device MAC address (6 bytes). */
	/**
	 * @brief MAC address as the 12 uppercase hex characters the iMACAddress string reports.
	 * @details Registered with the core by pointer, so it has to outlive enumeration rather
	 *          than being formatted into a local buffer on each request.
	 */
	char mac_str[(USBD_CDC_NCM_MAC_STR_LEN * 2U) + 1U];
	u8 mac_str_idx;                 /**< iMACAddress string index from usbd_add_string(), 0 for none. */
	u8 itf_str_idx;                 /**< Communication interface iInterface string index from usbd_add_string(), 0 for none. */

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
} usbd_cdc_ncm_dev_t;

/** @} End of Device_CDC_NCM_Types group*/
/** @} End of USB_Device_Types group*/

/* Exported macros -----------------------------------------------------------*/

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
 * @param[in] cfg: Pointer to the class configuration. See @ref usbd_cdc_ncm_config_t.
 * @return 0 on success, non-zero on failure.
 */
int usbd_cdc_ncm_init(const usbd_cdc_ncm_cb_t *cb, const usbd_cdc_ncm_config_t *cfg);

#ifdef CONFIG_USBD_COMPOSITE
/**
 * @brief Initializes class driver as part of a composite device.
 * @param[in] cb: Pointer to the user-defined callback structure.
 * @param[in] cfg: Pointer to the class configuration. See @ref usbd_cdc_ncm_config_t.
 * @return 0 on success, non-zero on failure.
 */
int usbd_composite_cdc_ncm_init(const usbd_cdc_ncm_cb_t *cb, const usbd_cdc_ncm_config_t *cfg);
#endif

/**
 * @brief De-initializes the CDC NCM class driver.
 * @note  Call it in task context and never from a task that calls usbd_cdc_ncm_transmit(): it
 *        waits, without a bound, for every task inside usbd_cdc_ncm_transmit() to leave before the
 *        TX resources are released, so such a caller would wait for itself.
 * @note  Composite mode: call it after @ref usbd_composite_deinit. Before that the class is
 *        still reachable from the USB ISR, so the call returns at once and releases nothing.
 * @return None. This is a teardown path: apart from the composite case above, the class is always
 *         unregistered and every resource it owns released, nothing for the caller to recover from.
 */
void usbd_cdc_ncm_deinit(void);

/**
 * @brief Transmits an ethernet frame to the host wrapped in an NTB16.
 * @param[in] buf: Pointer to the ethernet frame buffer to be transmitted.
 * @param[in] len: Length of the ethernet frame in bytes.
 * @param[in] block: When non-zero, block for a bounded time waiting for TX room, then
 *                   drop the frame and return HAL_BUSY.  When zero, drop immediately.
 * @return 0 on success, non-zero on failure.
 * @note   Task context only, a call in interrupt context is rejected. It may run concurrently
 *         with usbd_cdc_ncm_deinit(), which waits for it to leave before releasing resources.
 */
int usbd_cdc_ncm_transmit(u8 *buf, u32 len, u8 block);

/**
 * @brief  Gets the link state last reported to the host.
 * @return 1: Link is up, i.e. the host activated the data interface and the uplink is up.
 *         0: Link is down.
 */
int usbd_cdc_ncm_get_link_status(void);

/**
 * @brief  Reports the upper-layer network link state to the host (edge-triggered).
 * @details The host is notified with NETWORK_CONNECTION only when the reported state
 *          actually changes; calling repeatedly with the same value is a no-op.
 *          Call from the layer that owns the real uplink (e.g. Wi-Fi / cellular)
 *          on link up/down events.  The uplink starts down at every
 *          @ref usbd_cdc_ncm_init, so call this after each init once the uplink is up.
 * @param[in] link_up: Non-zero when the link is up, zero when it is down.
 * @return 0 on success.
 */
int usbd_cdc_ncm_set_link_status(u8 link_up);

/** @} End of Device_CDC_NCM_Functions group */
/** @} End of USB_Device_Functions group */

/** @} End of USB_Device_API group */

#ifdef __cplusplus
}
#endif

#endif /* USBD_CDC_NCM_H */
