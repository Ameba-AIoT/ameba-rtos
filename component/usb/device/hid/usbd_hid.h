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

/* Exported defines ----------------------------------------------------------*/

/** @addtogroup USB_Device_API USB Device API
 *  @{
 */
/** @addtogroup USB_Device_Constants USB Device Constants
 * @{
 */
/** @addtogroup Device_HID_Constants Device HID Constants
 * @{
 */

/* HID PID/VID */
#define USBD_HID_VID                         USB_VID  /**< Vendor ID. */
#define USBD_HID_PID                         USB_PID  /**< Product ID. */

/* HID strings */
#define USBD_HID_LANGID_STRING               0x0409U             /**< Language ID for string descriptors (0x0409 = English) */
#define USBD_HID_MFG_STRING                  "Realtek"           /**< Manufacturer string. */
#define USBD_HID_PROD_HS_STRING              "Realtek HID (HS)"  /**< Product string for High-Speed mode. */
#define USBD_HID_PROD_FS_STRING              "Realtek HID (FS)"  /**< Product string for Full-Speed mode. */
#define USBD_HID_SN_STRING                   "1234567890"        /**< Serial number string. */

#define USBD_HID_INTR_IN_BUF_SIZE            512U    /**< INTR IN maximum buffer size */
#define USBD_HID_INTR_OUT_BUF_SIZE           1024U   /**< INTR OUT maximum buffer size */

/* Endpoint address */
#define USBD_HID_HS_INT_MAX_PACKET_SIZE      64 /**< High speed INTR maximum packet size */
#define USBD_HID_FS_INT_MAX_PACKET_SIZE      64 /**< Full speed INTR maximum packet size */

#define USBD_HID_DESC_SIZE                   9       /**< HID descriptor size. */
#define USBD_HID_DESC_ITEM_LENGTH_OFFSET     7       /**< Offset of report desc wItemLength in the HID descriptor. */
#define USBD_HID_CFG_DESC_ITEM_LENGTH_OFFSET 25      /**< Offset of report desc wItemLength in the config descriptor. */

#ifdef CONFIG_USBD_HID_BIDIR
/* Bidirectional HID variant: 2 interfaces (Vendor raw + Consumer Control),
 * 3 endpoints (Priv INTR IN + Priv INTR OUT + Consumer INTR IN). */
#define USBD_HID_MAX_BUF_SIZE                64U     /**< Bidir INTR EP max packet size (LS/FS-compatible). */
#define USBD_HID_REPORT_ID                   0x1FU   /**< Priv (vendor) Report ID for IN/OUT. */
#define USBD_HID_VOLUME_REPORT_ID            ((USBD_HID_REPORT_ID) + 1) /**< Consumer control Report ID (Volume/Power). */
#define USBD_HID_TX_PKT_SIZE                 ((USBD_HID_MAX_BUF_SIZE) - 4) /**< IN payload size before Report ID / trailer. */
#define USBD_HID_RX_PKT_SIZE                 ((USBD_HID_MAX_BUF_SIZE) - 1) /**< OUT payload size (Report ID prepended). */
#define USBD_HID_BUF_MAX_CNT                 10U     /**< OUT ring buffer slot count. */
/* wItemLength offset for the Vendor (Consumer Control) HID descriptor in the config descriptor:
 *   cfg header (9) + priv-if (9) + priv-hid (9) + priv-EP IN (7) + priv-EP OUT (7)
 *   + vend-if (9) + vend-hid header (7) = 57 */
#define USBD_HID_CFG_VEND_DESC_ITEM_LENGTH_OFFSET  57
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
 *          Bidir variant needs INTR OUT (raw OUT report) and a second INTR IN
 *          (Consumer Control: Volume/Power).
 */
typedef struct {
	u32 intr_in_xfer_size;      /**< INTR IN transfer buffer size (0 = use default). */
	u8  intr_in_addr;
#if defined(CONFIG_USBD_HID_KEYBOARD) || defined(CONFIG_USBD_HID_BIDIR)
	u8  intr_out_addr;
#endif
#ifdef CONFIG_USBD_HID_BIDIR
	u8  consumer_intr_in_addr;  /**< Vendor (Consumer Control) INTR IN endpoint address. */
#endif
} usbd_hid_ep_cfg_t;

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

#if defined(CONFIG_USBD_HID_KEYBOARD) || defined(CONFIG_USBD_HID_BIDIR)
	/**
	 * @brief Called when new data is received from the host on the INTR OUT endpoint (or via SET_REPORT on EP0).
	 *        Keyboard: LED-state report; Bidir: raw vendor OUT report (also queued into ring buffer for
	 *        blocking @ref usbd_hid_read).
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 * @param[in] buf Pointer to the received data buffer.
	 * @param[in] len Length of the received data in bytes.
	 */
	void (*received)(u8 *buf, u32 len);
#endif

#ifdef CONFIG_USBD_HID_BIDIR
	/**
	 * @brief Called upon SOF interrupt (GINTSTS.Sof). Used by the bidir variant to
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

#ifdef CONFIG_USBD_HID_BIDIR
/**
 * @brief One slot of the bidir HID OUT ring buffer.
 */
typedef struct {
	u8 *buf_raw;
	__IO u16 buf_valid_len;
} usbd_hid_buf_t;

/**
 * @brief Bidir HID OUT ring buffer control block.
 */
typedef struct {
	rtos_sema_t rx_sema;              /**< Signal from ISR when a new OUT frame arrives. */
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
#endif /* CONFIG_USBD_HID_BIDIR */

/**
 * @brief Structure representing the HID device instance.
 */
typedef struct {
	const usbd_hid_ep_cfg_t *ep_cfg;    /**< Pointer to the EP configuration. */
	const usbd_hid_usr_cb_t *cb;        /**< Pointer to the user-defined callback structure. */
	usb_dev_t *dev;               /**< Pointer to the USB device instance. */
	usb_setup_req_t ctrl_req;     /**< Stores the current control request. */
	usbd_ep_t ep_intr_in;         /**< INTR IN endpoint structure (Priv/data IN). */
#if defined(CONFIG_USBD_HID_KEYBOARD) || defined(CONFIG_USBD_HID_BIDIR)
	usbd_ep_t ep_intr_out;        /**< INTR OUT endpoint structure. */
#endif
#ifdef CONFIG_USBD_HID_BIDIR
	usbd_ep_t ep_consumer_intr_in;    /**< Consumer Control (Volume/Power) INTR IN. */
	usbd_hid_buf_ctrl_t rx_ctrl;      /**< OUT ring buffer control block. */
	/*  Windows HID Tool prepends the Report ID to raw IN transfers so the
	 *  transfer length equals ReportSize; the TV/consumer paths only report
	 *  the user data (length may be < ReportSize). Detected at RX by reading
	 *  the leading byte and consulted at TX to decide whether to prepend. */
	__IO u8 report_id_append : 1;
#endif
	u8 protocol;                  /**< Store the device protocol value. */
	u8 idle_rate;                 /**< Store the device idle rate. */
	u8 from_composite;			/**< Flag indicating if part of a composite device. */
} usbd_hid_t;

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
 * @param[in] ep_cfg: Pointer to EP configuration (endpoint addresses and buffer sizes).
 * @return 0 on success, non-zero on failure.
 */
int usbd_hid_init(const usbd_hid_usr_cb_t *cb, const usbd_hid_ep_cfg_t *ep_cfg);

#ifdef CONFIG_USBD_COMPOSITE
/**
 * @brief Initializes class driver as part of a composite device.
 * @param[in] cb: Pointer to the user-defined callback structure.
 * @param[in] ep_cfg: Pointer to EP configuration (endpoint addresses and buffer sizes).
 * @return 0 on success, non-zero on failure.
 */
int usbd_composite_hid_init(const usbd_hid_usr_cb_t *cb, const usbd_hid_ep_cfg_t *ep_cfg);
#endif

/**
  * @brief  DeInitialize HID device
  * @return 0 on success, non-zero on failure.
  */
int usbd_hid_deinit(void);

/**
 * @brief Transmits data to the host over the INTR IN endpoint.
 * @param[in] buf: Pointer to the data buffer to be transmitted.
 * @param[in] len: Length of the data in bytes.
 * @return 0 on success, non-zero on failure.
 */
int usbd_hid_send_data(const u8 *buf, u32 len);

#ifdef CONFIG_USBD_HID_BIDIR
/**
 * @brief  Bidir variant: send a Consumer Control Volume Up/Down code.
 * @param[in] vol_up: 1 = Volume Up, 0 = Volume Down.
 * @return 0 on success, non-zero on failure.
 */
int usbd_hid_volume_ctrl(u8 vol_up);

/**
 * @brief  Bidir variant: send a Consumer Control Power press.
 * @return 0 on success, non-zero on failure.
 */
int usbd_hid_power_ctrl(void);

/**
 * @brief  Bidir variant: read one or more queued raw OUT reports from the ring buffer.
 * @param[in]  buffer      Destination buffer for payload bytes (Report ID stripped when appended).
 * @param[in]  size        Destination buffer capacity.
 * @param[in]  time_out_ms 0 = non-blocking; else block up to this many ms waiting for data.
 * @return Number of bytes actually copied into @p buffer.
 */
u32 usbd_hid_read(u8 *buffer, u32 size, u32 time_out_ms);

/**
 * @brief  Bidir variant: number of raw OUT frames currently queued.
 * @return Queued frame count.
 */
u32 usbd_hid_get_read_buf_cnt(void);

/**
 * @brief  Bidir variant: query whether the RX ring buffer is full.
 * @return 1 if full, 0 otherwise.
 */
u32 usbd_hid_ring_buf_is_full(void);
#endif /* CONFIG_USBD_HID_BIDIR */

/** @} End of Device_HID_Functions group */
/** @} End of USB_Device_Functions group */
/** @} End of USB_Device_API group */

#ifdef __cplusplus
}
#endif

#endif /* USBD_HID_H */
