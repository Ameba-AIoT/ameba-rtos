/*
 * Copyright (c) 2025 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBD_HID_H
#define USBD_HID_H

/* Includes ------------------------------------------------------------------*/

#include "usbd.h"

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
/** @addtogroup Device_HID_Constants Device HID Constants
 * @{
 */

/* Defaults applied when the matching @ref usbd_hid_config_t field is left 0. */
#define USBD_HID_INTR_IN_BUF_SIZE            512U    /**< Default INTR IN transfer buffer size. */
#define USBD_HID_INTR_OUT_BUF_SIZE           64U     /**< Default INTR OUT transfer buffer size, one max packet: the keyboard LED report is a single byte. */

/* Default INTR bInterval. Full Speed counts frames (1 ms each), High Speed counts
 * 2^(value-1) microframes, so the same polling rate needs different numbers: FS 10 -> 10 ms for
 * mouse/keyboard, HS 4 -> 8 microframes -> 1 ms. The vendor variant polls every frame. */
#ifdef CONFIG_USBD_HID_VENDOR
#define USBD_HID_DEFAULT_FS_INTR_INTERVAL    1U      /**< Default INTR bInterval at Full Speed. */
#else
#define USBD_HID_DEFAULT_FS_INTR_INTERVAL    10U     /**< Default INTR bInterval at Full Speed. */
#endif
#define USBD_HID_DEFAULT_HS_INTR_INTERVAL    4U      /**< Default INTR bInterval at High Speed. */

/* Endpoint address */
#define USBD_HID_HS_INT_MAX_PACKET_SIZE      64 /**< High speed INTR maximum packet size */
#define USBD_HID_FS_INT_MAX_PACKET_SIZE      64 /**< Full speed INTR maximum packet size */

#define USBD_HID_DESC_SIZE                   9       /**< HID descriptor size. */
#define USBD_HID_DESC_ITEM_LENGTH_OFFSET     7       /**< Offset of report desc wItemLength in the HID descriptor. */
#ifdef CONFIG_USBD_HID_VENDOR
/* cfg header (9) + IAD (8) + priv-if (9) + priv-hid header (7) = 33 */
#define USBD_HID_CFG_DESC_ITEM_LENGTH_OFFSET 33      /**< Offset of report desc wItemLength in the config descriptor. */
#else
#define USBD_HID_CFG_DESC_ITEM_LENGTH_OFFSET 25      /**< Offset of report desc wItemLength in the config descriptor. */
#endif

#ifdef CONFIG_USBD_HID_VENDOR
/* Vendor HID variant: 2 interfaces (Priv raw + Vendor),
 * 3 endpoints (Priv INTR IN + Priv INTR OUT + Vendor INTR IN). */
#define USBD_HID_MAX_BUF_SIZE                64U     /**< Vendor INTR EP max packet size (LS/FS-compatible). */
/* Ref HID 1.11 5.6 and 8.1: hid_priv_report_desc declares this Report ID, so every priv IN/OUT report
 * carries it as the first byte: usbd_hid_send_data() prepends it and usbd_hid_read() strips it, dropping
 * an OUT report with any other leading byte. A non-standard host that sends OUT reports without the
 * Report ID, or rejects IN reports carrying it, does not match this descriptor: adjust the descriptor
 * and both functions to the actual application needs. */
#define USBD_HID_REPORT_ID                   0x1FU   /**< Priv (vendor) Report ID for IN/OUT. */
#define USBD_HID_VOLUME_REPORT_ID            ((USBD_HID_REPORT_ID) + 1) /**< Vendor Report ID (Volume/Power). */
#define USBD_HID_TX_PKT_SIZE                 ((USBD_HID_MAX_BUF_SIZE) - 4) /**< IN payload size before Report ID / trailer. */
#define USBD_HID_RX_PKT_SIZE                 ((USBD_HID_MAX_BUF_SIZE) - 1) /**< OUT payload size (Report ID prepended). */
#define USBD_HID_BUF_MAX_CNT                 10U     /**< OUT ring buffer slot count. */
/* wItemLength offset for the Vendor HID descriptor in the config descriptor:
 *   cfg header (9) + IAD (8) + priv-if (9) + priv-hid (9) + priv-EP IN (7) + priv-EP OUT (7)
 *   + vendor-if (9) + vendor-hid header (7) = 65 */
#define USBD_HID_CFG_VENDOR_DESC_ITEM_LENGTH_OFFSET  65
#endif

/* Mouse button definition  */
#define USBD_HID_MOUSE_BUTTON_LEFT           0x01    /**< Mouse Left button. 0: release, 1: press. */
#define USBD_HID_MOUSE_BUTTON_RIGHT          0x02    /**< Mouse Right button. 0: release, 1: press. */
#define USBD_HID_MOUSE_BUTTON_MIDDLE         0x04    /**< Mouse Wheel button. 0: release, 1: press. */
#define USBD_HID_MOUSE_BUTTON_RESERVED       0xf8    /**< Mouse Reserved. all bit should set to 1. */

/** @} End of Device_HID_Constants group */
/** @} End of USB_Device_Constants group */

/* Exported types ------------------------------------------------------------*/

/** @addtogroup USB_Device_Types USB Device Types
 * @{
 */
/** @addtogroup Device_HID_Types Device HID Types
 * @{
 */

/**
 * @brief HID endpoint configuration structure.
 * @details Allows the user to override default endpoint addresses and buffer sizes.
 *          Keyboard variant needs INTR OUT (LED report).
 *          Vendor variant needs INTR OUT (raw OUT report) and a second INTR IN
 *          (Vendor: Volume/Power).
 */
typedef struct {
	u32 intr_in_xfer_size;      /**< INTR IN transfer buffer size, 0 uses @ref USBD_HID_INTR_IN_BUF_SIZE. */
	u32 intr_out_xfer_size;     /**< INTR OUT transfer buffer size, 0 uses @ref USBD_HID_INTR_OUT_BUF_SIZE.
	                                 Ignored unless the keyboard variant is built. */
	const char *itf_str;        /**< iInterface string of the HID interface, NULL for none. */
	u8  intr_in_addr;           /**< INTR IN endpoint address. */
	/**
	 * @brief INTR OUT endpoint address.
	 * @details Ignored unless `CONFIG_USBD_HID_KEYBOARD` (LED report) or
	 *          `CONFIG_USBD_HID_VENDOR` (raw OUT report) is built.
	 */
	u8  intr_out_addr;
	/**
	 * @brief Vendor INTR IN endpoint address.
	 * @details Ignored unless `CONFIG_USBD_HID_VENDOR` is built.
	 */
	u8  vendor_intr_in_addr;
	u8  intr_interval_hs;       /**< INTR bInterval at High Speed, 0 uses @ref USBD_HID_DEFAULT_HS_INTR_INTERVAL. */
	u8  intr_interval_fs;       /**< INTR bInterval at Full Speed, 0 uses @ref USBD_HID_DEFAULT_FS_INTR_INTERVAL. */
} usbd_hid_config_t;

/**
 * @brief Structure containing callback functions for the HID class.
 * @details The user application should provide an instance of this structure
 *          to handle class-specific events.
 */
typedef struct {
	/**
	 * @brief Called when the HID class driver initialization for application resource setup.
	 */
	void (*init)(void);

	/**
	 * @brief Called when the HID device is de-initialized for resource cleanup.
	 */
	void (*deinit)(void);

	/**
	 * @brief Called to handle class-specific SETUP requests.
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 */
	void (*setup)(void);

	/**
	 * @brief Called when a data transmission to the host on the BULK IN endpoint is complete.
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 * @param[in] status The status of the transmission.
	 */
	void (*transmitted)(u8 status);

#if defined(CONFIG_USBD_HID_KEYBOARD) || defined(CONFIG_USBD_HID_VENDOR)
	/**
	 * @brief Called when new data is received from the host on the INTR OUT endpoint (or via SET_REPORT on EP0).
	 *        Keyboard: LED-state report; Vendor: raw OUT report (also queued into ring buffer for
	 *        blocking usbd_hid_read).
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 * @param[in] buf Pointer to the received data buffer.
	 * @param[in] len Length of the received data in bytes.
	 */
	void (*received)(u8 *buf, u32 len);
#endif

#ifdef CONFIG_USBD_HID_VENDOR
	/**
	 * @brief Called upon SOF interrupt (GINTSTS.Sof). Used by the vendor variant to
	 *        re-arm the INTR OUT endpoint when the OUT ring buffer has free slots.
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 */
	void (*sof)(void);
#endif

	/**
	 * @brief Called when USB attach status changes for application to support hot-plug events.
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 * @param[in] old_status The previous attach status.
	 * @param[in] status The new attach status.
	 */
	void (*status_changed)(u8 old_status, u8 status);
} usbd_hid_usr_cb_t;

/** @} End of Device_HID_Types group */
/** @} End of USB_Device_Types group */

#ifdef CONFIG_USBD_HID_VENDOR
/**
 * @brief One slot of the vendor HID OUT ring buffer.
 */
typedef struct {
	u8 *buf_raw;
	__IO u16 buf_valid_len;
} usbd_hid_buf_t;

/**
 * @brief Vendor HID OUT ring buffer control block.
 */
typedef struct {
	usb_os_sema_t rx_sema;            /**< Signal from ISR when a new OUT frame arrives. */
	usbd_hid_buf_t *buf_array;        /**< Slot array (size = buf_array_cnt). */
	u8 *hid_buf;                      /**< Backing DMA buffer for all slots (contiguous). */

	u16 buf_array_cnt;                /**< Number of slots (USBD_HID_BUF_MAX_CNT). */
	u16 hid_mps;                      /**< Max packet size per slot (USBD_HID_MAX_BUF_SIZE). */

	__IO u8 hid_sema_valid;           /**< 1 if @ref rx_sema was created. */
	__IO u8 read_wait_sema;           /**< 1 while a reader is blocked on rx_sema. */
	__IO u8 read_idx;                 /**< Next slot to be read out. */
	__IO u8 write_idx;                /**< Next slot the ISR will fill. */
	__IO u8 is_intr_out_busy;         /**< 1 if an INTR OUT request is currently armed. */
} usbd_hid_buf_ctrl_t;
#endif /* CONFIG_USBD_HID_VENDOR */

/**
 * @brief Structure representing the HID device instance.
 */
typedef struct {
	const usbd_hid_config_t *cfg;       /**< Pointer to the class configuration (set by init). */
	const usbd_hid_usr_cb_t *cb;        /**< Pointer to the user-defined callback structure. */
	usb_dev_t *dev;               /**< Pointer to the USB device instance. */
	usbd_ep_t ep_intr_in;         /**< INTR IN endpoint structure (Priv/data IN). */
#if defined(CONFIG_USBD_HID_KEYBOARD) || defined(CONFIG_USBD_HID_VENDOR)
	usbd_ep_t ep_intr_out;        /**< INTR OUT endpoint structure. */
#endif
#ifdef CONFIG_USBD_HID_VENDOR
	usbd_ep_t ep_vendor_intr_in;      /**< Vendor (Volume/Power) INTR IN. */
	usbd_hid_buf_ctrl_t rx_ctrl;      /**< OUT ring buffer control block. */
#endif
	usb_setup_req_t ctrl_req;     /**< Stores the current control request (byte-packed). */
	u8 protocol;                  /**< Store the device protocol value. */
	u8 idle_rate;                 /**< Store the device idle rate. */
	u8 ctrl_req_pending;          /**< 1 if ctrl_req is waiting for its EP0 OUT data stage. */
	u8 from_composite;			/**< Flag indicating if part of a composite device. */
	u8 itf_str_idx;				/**< iInterface string index from usbd_add_string(), 0 for none. */
} usbd_hid_t;

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @addtogroup USB_Device_Functions USB Device Functions
 * @{
 */
/** @addtogroup Device_HID_Functions Device HID Functions
 * @{
 */

/**
 * @brief Initializes class driver as a standalone device.
 * @param[in] cb: Pointer to the user-defined callback structure.
 * @param[in] cfg: Pointer to the class configuration. See @ref usbd_hid_config_t.
 * @return 0 on success, non-zero on failure.
 */
int usbd_hid_init(const usbd_hid_usr_cb_t *cb, const usbd_hid_config_t *cfg);

#ifdef CONFIG_USBD_COMPOSITE
/**
 * @brief Initializes class driver as part of a composite device.
 * @param[in] cb: Pointer to the user-defined callback structure.
 * @param[in] cfg: Pointer to the class configuration. See @ref usbd_hid_config_t.
 * @return 0 on success, non-zero on failure.
 */
int usbd_composite_hid_init(const usbd_hid_usr_cb_t *cb, const usbd_hid_config_t *cfg);
#endif

/**
  * @brief  DeInitialize HID device
  * @note  Call it in task context, once no other task is inside usbd_hid_send_data(),
  *        usbd_hid_volume_ctrl() or usbd_hid_power_ctrl(): the TX buffers they write to are
  *        released here.
  * @note  Composite mode: call it after @ref usbd_composite_deinit. Before that the class is
  *        still reachable from the USB ISR, so the call returns at once and releases nothing.
  * @return None. This is a teardown path: apart from the composite case above, the class is always
  *         unregistered and every resource it owns released, nothing for the caller to recover from.
  */
void usbd_hid_deinit(void);

/**
 * @brief Transmits data to the host over the INTR IN endpoint.
 * @param[in] buf: Pointer to the data buffer to be transmitted.
 * @param[in] len: Length of the data in bytes.
 * @return 0 on success, non-zero on failure.
 */
int usbd_hid_send_data(const u8 *buf, u32 len);

#ifdef CONFIG_USBD_HID_VENDOR
/**
 * @brief  Vendor variant: send a Vendor Volume Up/Down code.
 * @param[in] vol_up: 1 = Volume Up, 0 = Volume Down.
 * @return 0 on success, non-zero on failure.
 */
int usbd_hid_volume_ctrl(u8 vol_up);

/**
 * @brief  Vendor variant: send a Vendor Power press.
 * @return 0 on success, non-zero on failure.
 */
int usbd_hid_power_ctrl(void);

/**
 * @brief  Vendor variant: read one or more queued raw OUT reports from the ring buffer.
 * @param[in]  buffer      Destination buffer for payload bytes (leading Report ID stripped).
 * @param[in]  size        Destination buffer capacity.
 * @param[in]  time_out_ms 0 = non-blocking; else block up to this many ms waiting for data.
 * @return Number of bytes actually copied into @p buffer. 0 also when the queued report does not
 *         carry @ref USBD_HID_REPORT_ID, such a report is dropped.
 */
u32 usbd_hid_read(u8 *buffer, u32 size, u32 time_out_ms);

/**
 * @brief  Vendor variant: number of raw OUT frames currently queued.
 * @return Queued frame count.
 */
u32 usbd_hid_get_read_buf_cnt(void);

/**
 * @brief  Vendor variant: query whether the RX ring buffer is full.
 * @return 1 if full, 0 otherwise.
 */
u32 usbd_hid_ring_buf_is_full(void);
#endif /* CONFIG_USBD_HID_VENDOR */

/** @} End of Device_HID_Functions group */
/** @} End of USB_Device_Functions group */

/** @} End of USB_Device_API group */

#ifdef __cplusplus
}
#endif

#endif /* USBD_HID_H */
