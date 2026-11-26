/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBD_RNDIS_H
#define USBD_RNDIS_H

/* Includes ------------------------------------------------------------------*/

#include "platform_autoconf.h"
#include "usbd.h"
#include "usb_rndis.h"
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
/** @addtogroup Device_RNDIS_Constants Device RNDIS Constants
 * @{
 */

/* Defaults applied when the matching @ref usbd_rndis_config_t field is left 0. */
#define USBD_RNDIS_DEFAULT_INTR_IN_INTERVAL_HS        4U     /**< Default INTR IN bInterval at High Speed, 1 ms. */
#define USBD_RNDIS_DEFAULT_INTR_IN_INTERVAL_FS        1U     /**< Default INTR IN bInterval at Full Speed, 1 ms. */
#define USBD_RNDIS_DEFAULT_BULK_TX_QUEUE_DEPTH        5U     /**< Default TX ring buffer slot count. */
#define USBD_RNDIS_DEFAULT_BULK_TX_TIMEOUT_MS         10U    /**< Default blocking-transmit timeout in ms. */
#define USBD_RNDIS_DEFAULT_RX_TASK_STACK_SIZE         1280U  /**< Default RX delivery task stack size in bytes. */
#define USBD_RNDIS_DEFAULT_RX_TASK_PRIORITY           6U     /**< Default RX delivery task priority. */

/* Interface numbers */
#define USBD_RNDIS_COMM_ITF_NUM                 0x00U  /**< Communication interface */
#define USBD_RNDIS_DATA_ITF_NUM                 0x01U  /**< Data interface */

/* Number of ping-pong RX buffers used to decouple USB OUT from upper-layer RX. */
#define USBD_RNDIS_RX_BUF_NUM                         2U

/* Capacity of the encapsulated response buffer: the largest response is the QUERY_CMPLT of
 * OID_GEN_SUPPORTED_LIST, 24 bytes of header and 26 OIDs. */
#define USBD_RNDIS_RESP_BUF_SIZE                      128U

/** @} End of Device_RNDIS_Constants group */
/** @} End of USB_Device_Constants group */

/* Exported types ------------------------------------------------------------*/

/** @addtogroup USB_Device_Types USB Device Types
 * @{
 */
/** @addtogroup Device_RNDIS_Types Device RNDIS Types
 * @{
 */

/**
 * @brief Configuration for the RNDIS class.
 * @details Carries every RNDIS option: the endpoint allocation, which the top-level application
 *          owns, plus the MAC address, the vendor description and the TX/RX resource sizing. Used
 *          by both standalone and composite modes. Device-global parameters (VID/PID, strings,
 *          power attributes) live in @ref usbd_dev_info_t instead.
 * @note  The structure is stored by pointer, not copied, so it shall remain valid until
 *        @ref usbd_rndis_deinit.
 */
typedef struct {
	/**
	 * @brief MAC address reported to the host by OID_802_3_PERMANENT_ADDRESS, 6 bytes, NULL for
	 *        the class default.
	 * @details Copied into the class at init, so the buffer itself needs not outlive the init call.
	 */
	const u8 *mac;
	const char *itf_str;        /**< iInterface string of the communication interface, NULL for none. */
	const char *vendor_desc;    /**< OID_GEN_VENDOR_DESCRIPTION, NULL for the class default; kept by pointer. */
	u16 bulk_tx_timeout_ms;     /**< Blocking-transmit timeout in ms, 0 uses @ref USBD_RNDIS_DEFAULT_BULK_TX_TIMEOUT_MS. */
	u16 rx_task_stack_size;     /**< RX delivery task stack size in bytes, 0 uses @ref USBD_RNDIS_DEFAULT_RX_TASK_STACK_SIZE. */
	u8  bulk_in_addr;           /**< BULK IN endpoint address (e.g. 0x81). */
	u8  bulk_out_addr;          /**< BULK OUT endpoint address (e.g. 0x02). */
	u8  intr_in_addr;           /**< INTERRUPT IN endpoint address (e.g. 0x83). */
	u8  intr_in_interval_hs;    /**< INTR IN bInterval at High Speed, 0 uses @ref USBD_RNDIS_DEFAULT_INTR_IN_INTERVAL_HS. */
	u8  intr_in_interval_fs;    /**< INTR IN bInterval at Full Speed, 0 uses @ref USBD_RNDIS_DEFAULT_INTR_IN_INTERVAL_FS. */
	/**
	 * @brief Number of Ethernet frames the TX ring buffer can hold, 0 uses
	 *        @ref USBD_RNDIS_DEFAULT_BULK_TX_QUEUE_DEPTH.
	 * @details Each slot costs one max-size frame of heap.
	 */
	u8  bulk_tx_queue_depth;
	u8  rx_task_priority;       /**< RX delivery task priority, 0 uses @ref USBD_RNDIS_DEFAULT_RX_TASK_PRIORITY. */
} usbd_rndis_config_t;

/**
 * @brief Structure containing callback functions for the RNDIS class.
 */
typedef struct {
	/**
	 * @brief Called during RNDIS class driver initialization to set up application resources.
	 * @return 0 on success, non-zero on failure.
	 */
	int (*init)(void);

	/**
	 * @brief Called when the RNDIS device is de-initialized for resource cleanup.
	 * @return 0 on success, non-zero on failure.
	 */
	int (*deinit)(void);

	/**
	 * @brief Called when one Ethernet frame is received from the host.
	 * @note  Runs in the class RX task. buf is valid only until the callback returns.
	 * @param[in] buf: Pointer to the Ethernet frame.
	 * @param[in] len: Frame length in bytes.
	 * @return 0 on success, non-zero on failure.
	 */
	int (*received)(u8 *buf, u32 len);

	/**
	 * @brief Called when USB attach status changes for application to support hot-plug events.
	 * @note  This function is called within an interrupt service routine (ISR) context;
	 *        time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 * @param[in] old_status: The previous attach status.
	 * @param[in] status: The new attach status.
	 */
	void (*status_changed)(u8 old_status, u8 status);
} usbd_rndis_cb_t;

/**
 * @brief Structure representing the RNDIS device instance.
 */
typedef struct {
	const usbd_rndis_config_t *cfg;         /**< Pointer to the class configuration (set by init). */
	const usbd_rndis_cb_t *cb;              /**< User-defined callback structure. */
	usb_dev_t *dev;                         /**< USB device instance. */

	/* RNDIS and link state */
	__IO u8 rndis_state;                    /**< RNDIS state, see usbd_rndis_state_t. Written in USB ISR context only;
	                                             the DATA_INITIALIZED value is the "data may flow" predicate of TX/RX. */
	__IO u8 connect_status;                 /**< Media state last reported to the host: 1 only while data may flow
	                                             and the uplink is up. Written in USB ISR context only. */
	__IO u8 uplink_status;                  /**< Upper-layer link state set by usbd_rndis_set_link_status(), only
	                                             read by the USB ISR. Starts at 0 on every init. */
	__IO u8 is_configured;                  /**< 1 between set_config() and clear_config(): every endpoint is up. */
	__IO u8 notify_pending;                 /**< 1 while a RESPONSE_AVAILABLE notification still has to be sent. */
	u8 ctrl_req_pending;                    /**< 1 if a SEND_ENCAPSULATED_COMMAND waits for its EP0 OUT data stage. */
	u8 from_composite;                      /**< Flag indicating if part of a composite device. */
	u8 itf_base;                             /**< First interface number of this class, rebased by the composite framework. */

	/* Encapsulated response */
	u32 packet_filter;                      /**< OID_GEN_CURRENT_PACKET_FILTER set by the host. */
	u16 ctrl_req_len;                       /**< wLength of the pending SEND_ENCAPSULATED_COMMAND. */
	u16 resp_len;                           /**< Length of the queued command response in resp_buf, 0 for none. */
	__IO u8 status_pending;                 /**< 1 while an INDICATE_STATUS message is still to be read by the host. */
	u8 status_connect;                      /**< Media state the pending INDICATE_STATUS reports. */

	/* Statistics reported through the OID_GEN_xxx counters */
	u32 xmit_ok;                            /**< Frames sent to the host. */
	u32 rcv_ok;                             /**< Frames delivered to the upper layer. */
	u32 xmit_err;                           /**< Frames dropped on the TX path. */
	u32 rcv_err;                            /**< Malformed RNDIS packet messages received. */

	/* TX */
	u8 *bulk_tx_dma_buf;                    /**< DMA source for BULK IN: the PACKET_MSG header is written in front of
	                                             the frame taken from the ring buffer head. */
	usb_os_sema_t bulk_tx_slot_sema;        /**< ISR -> task: given each time a ring buffer slot is freed. */
	usb_ringbuf_manager_t bulk_tx_rb;       /**< TX ring buffer of raw Ethernet frames. */
	__IO u8 is_closing;                     /**< Set by deinit: transmit() rejects new callers. Written under the USB critical section. */
	__IO u8 xfer_users;                     /**< Number of tasks inside usbd_rndis_transmit(); deinit waits for 0. */

	/* RX ping-pong */
	usb_os_sema_t rx_data_ready_sema;       /**< ISR -> thread: signals a buffer holds a transfer. */
	usb_os_task_t rx_task;                  /**< RX delivery thread handle. */
	u8 *rx_buf[USBD_RNDIS_RX_BUF_NUM];      /**< Ping-pong RX buffers. */
	u8 *rx_msg_buf;                         /**< Buffer handed to the RX thread. */
	u32 rx_msg_len;                         /**< Transfer length handed to the RX thread. */
	__IO u32 rx_pending_len;                /**< Deferred transfer length waiting in rx_buf[rx_xfer_idx]. */
	u8 rx_xfer_idx;                         /**< Index of the buffer currently armed for USB OUT. */
	__IO u8 rx_buf_free;                    /**< thread -> ISR: 1 = buffer available, 0 = held by the RX thread. */
	__IO u8 rx_thread_running;              /**< RX thread loop guard; cleared to 0 to request exit. */

	/* Endpoints and EP0 */
	usbd_ep_t ep_bulk_in;                   /**< BULK IN endpoint. */
	usbd_ep_t ep_bulk_out;                  /**< BULK OUT endpoint. */
	usbd_ep_t ep_intr_in;                   /**< INTERRUPT IN endpoint. */

	/* MAC address and strings */
	u8 mac[USB_RNDIS_MAC_ADDR_LEN];         /**< MAC address reported to the host. */
	u8 itf_str_idx;                         /**< Communication interface iInterface string index, 0 for none. */

	u8 resp_buf[USBD_RNDIS_RESP_BUF_SIZE];  /**< Queued command response, read by GET_ENCAPSULATED_RESPONSE. */
} usbd_rndis_dev_t;

/** @} End of Device_RNDIS_Types group */
/** @} End of USB_Device_Types group */

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @addtogroup USB_Device_Functions USB Device Functions
 * @{
 */
/** @addtogroup Device_RNDIS_Functions Device RNDIS Functions
 * @{
 */

/**
 * @brief Initializes class driver with application callback handler.
 * @param[in] cb: Pointer to the user-defined callback structure.
 * @param[in] cfg: Pointer to the class configuration. See @ref usbd_rndis_config_t.
 * @return 0 on success, non-zero on failure.
 */
int usbd_rndis_init(const usbd_rndis_cb_t *cb, const usbd_rndis_config_t *cfg);

#ifdef CONFIG_USBD_COMPOSITE
/**
 * @brief Initializes class driver as part of a composite device.
 * @param[in] cb: Pointer to the user-defined callback structure.
 * @param[in] cfg: Pointer to the class configuration. See @ref usbd_rndis_config_t.
 * @return 0 on success, non-zero on failure.
 */
int usbd_composite_rndis_init(const usbd_rndis_cb_t *cb, const usbd_rndis_config_t *cfg);
#endif

/**
 * @brief De-initializes the RNDIS class driver.
 * @note  Call it in task context and never from a task that calls usbd_rndis_transmit(): it waits,
 *        without a bound, for every task inside usbd_rndis_transmit() to leave before the TX
 *        resources are released.
 * @note  Composite mode: call it after @ref usbd_composite_deinit. Before that the class is still
 *        reachable from the USB ISR, so the call returns at once and releases nothing.
 */
void usbd_rndis_deinit(void);

/**
 * @brief Transmits one Ethernet frame to the host over the BULK IN endpoint.
 * @param[in] buf: Ethernet frame, no alignment required: it is copied.
 * @param[in] len: Frame length in bytes, at most USB_RNDIS_MAX_FRAME_SIZE (no FCS).
 * @param[in] block: Blocking mode flag (0 for non-blocking, else block up to
 *                   @ref usbd_rndis_config_t::bulk_tx_timeout_ms for a free slot).
 * @return 0 on success, non-zero on failure.
 * @note   The frame is only queued; the USB ISR (SOF and BULK IN completion) starts the transfer,
 *         so @ref USBD_SOF_INTR must be set in usbd_config_t::ext_intr_enable. Task context only.
 *         Frames are rejected until the host has set a non-zero packet filter.
 */
int usbd_rndis_transmit(const u8 *buf, u32 len, u8 block);

/**
 * @brief  Gets the media state last reported to the host.
 * @return true: Link is up, i.e. the host enabled the data path and the uplink is up. false: Link is down.
 */
bool usbd_rndis_get_link_status(void);

/**
 * @brief  Reports the upper-layer network link state to the host (edge-triggered).
 * @details The host is told with INDICATE_STATUS (MEDIA_CONNECT / MEDIA_DISCONNECT) only when
 *          the reported state actually changes. The uplink starts down at every
 *          @ref usbd_rndis_init, so call this after each init once the uplink is up.
 * @param[in] link_up: Non-zero when the link is up, zero when it is down.
 * @return 0 on success.
 */
int usbd_rndis_set_link_status(u8 link_up);

/** @} End of Device_RNDIS_Functions group */
/** @} End of USB_Device_Functions group */

/** @} End of USB_Device_API group */

#ifdef __cplusplus
}
#endif

#endif /* USBD_RNDIS_H */
