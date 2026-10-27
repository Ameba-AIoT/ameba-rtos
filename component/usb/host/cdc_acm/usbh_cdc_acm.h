/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_CDC_ACM_H
#define USBH_CDC_ACM_H

/* Includes ------------------------------------------------------------------*/

#include "usbh.h"
#include "usb_cdc_acm.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup USB_Host_API USB Host API
 *  @{
 */

/* Exported defines ----------------------------------------------------------*/

/** @addtogroup USB_Host_Constants USB Host Constants
 * @{
 */
/** @addtogroup Host_CDC_ACM_Constants Host CDC ACM Constants
 * @{
 */

#define USBH_CDC_ACM_DEBUG                          0   /**< Enable/Disable debug dump (pipe/descriptor info). Set to 1 for debug. */

/* Defaults applied when the matching @ref usbh_cdc_acm_config_t field is left 0. */
#define USBH_CDC_ACM_DEFAULT_DTE_RATE               115200U /**< Default dwDteRate. */
#define USBH_CDC_ACM_DEFAULT_DATA_BITS              8U      /**< Default bDataBits. */

/**
 * @brief Enable optional 4G LTE dongle support (Quectel / Fibocom, ECM mode).
 *        When enabled, the driver accepts a VID/PID parameter table via
 *        usbh_cdc_acm_config_t::dongle_param and handles vendor-specific ACM topologies
 *        (param-driven AT-command interface index, single vendor interface
 *        carrying INTR+BULK+BULK, SET_INTERFACE alternate setting). A plain
 *        CDC ACM device (dongle_param == NULL) works exactly as before.
 */
#ifndef CONFIG_USBH_CDC_ACM_4G_DONGLE
#define CONFIG_USBH_CDC_ACM_4G_DONGLE               0
#endif

/* Quectel CAT1 dongle (ECM mode) VID/PID */
#define USBH_CDC_ACM_QUECTEL_DONGLE_VID             (0x2C7C)
#define USBH_CDC_ACM_QUECTEL_DONGLE_EG915_PID       (0x0901)
#define USBH_CDC_ACM_QUECTEL_DONGLE_EG91_PID        (0x0191)
/* Fibocom LE271 CAT1 dongle (ECM mode) VID/PID */
#define USBH_CDC_ACM_FIBOCOM_DONGLE_LE271_VID       (0x2CB7)
#define USBH_CDC_ACM_FIBOCOM_DONGLE_LE271_PID       (0x0D01)
/* SIMCom SIM767X CAT1 dongle (ECM mode) VID/PID */
#define USBH_CDC_ACM_SIMCOM_DONGLE_SIM767X_VID      (0x05C6)
#define USBH_CDC_ACM_SIMCOM_DONGLE_SIM767X_PID      (0x9330)

/** @} End of Host_CDC_ACM_Constants group */
/** @} End of USB_Host_Constants group */

/* Exported types ------------------------------------------------------------*/

/** @addtogroup USB_Host_Types USB Host Types
 * @{
 */
/** @addtogroup Host_CDC_ACM_Types Host CDC ACM Types
 * @{
 */

/**
 * @brief CDC ACM EP0 class request state machine.
 * @details Tracks the control pipe only. The BULK and INTR pipes are independent of EP0
 *          (Ref USB 2.0 5.3.2), so a data transfer has no state here and never waits for,
 *          or is blocked by, a class request.
 */
typedef enum {
	USBH_CDC_ACM_STATE_IDLE = 0U,                /**< State IDLE: EP0 free for a new class request. */
	USBH_CDC_ACM_STATE_SET_LINE_CODING,          /**< State SET_LINE_CODING: Configuring line coding. */
	USBH_CDC_ACM_STATE_GET_LINE_CODING,          /**< State GET_LINE_CODING: Retrieving line coding. */
	USBH_CDC_ACM_STATE_SET_CONTROL_LINE_STATE,   /**< State SET_CONTROL_LINE_STATE: Setting control line state. */
	USBH_CDC_ACM_STATE_SEND_BREAK,               /**< State SEND_BREAK: Sending break signal (PSTN §6.3.13). */
	USBH_CDC_ACM_STATE_ERROR,                    /**< State ERROR: Error occurred. */
} usbh_cdc_acm_state_t;

/**
 * @brief 4G dongle VID/PID parameter entry.
 * @details A NULL-terminated array (vid==0 sentinel) supplied through
 *          @ref usbh_cdc_acm_config_t::dongle_param lets the driver recognise specific
 *          LTE dongles and locate their AT-command interface.
 *          Only consulted when `CONFIG_USBH_CDC_ACM_4G_DONGLE` is enabled.
 */
typedef struct {
	u16 vid;               /**< Dongle Vendor ID. */
	u16 pid;               /**< Dongle Product ID. */
	u8 at_line_idx;        /**< Dongle AT-command interface index (used as SET/GET_LINE_CODING wIndex and comm interface number). */
} usbh_cdc_acm_param_t;

/**
 * @brief Configuration for the CDC ACM host class.
 * @details Carries every CDC ACM host option: the initial line coding and control line state
 *          the driver applies to the attached device, plus the optional 4G-dongle
 *          identification table. Host-global parameters (speed,
 *          FIFO depths, main task stack and priority, ISR priority) live in
 *          @ref usbh_config_t instead, and application-level options such as hotplug or
 *          worker task priorities belong in the application's own configuration.
 * @note  The structure is stored by pointer, not copied, so it shall remain valid until
 *        @ref usbh_cdc_acm_deinit.
 */
typedef struct {
	/**
	 * @brief Optional 4G-dongle VID/PID table, NULL-terminated (vid == 0 sentinel).
	 * @details NULL for a plain CDC ACM device. Ignored unless
	 *          `CONFIG_USBH_CDC_ACM_4G_DONGLE` is enabled.
	 */
	const usbh_cdc_acm_param_t *dongle_param;
	u32 dte_rate;           /**< dwDteRate, 0 uses @ref USBH_CDC_ACM_DEFAULT_DTE_RATE (Ref PSTN 1.2 6.3.11). */
	u8  data_bits;          /**< bDataBits, 0 uses @ref USBH_CDC_ACM_DEFAULT_DATA_BITS. */
	/**
	 * @brief bCharFormat, one of the `LINE_CODING_CHAR_FORMAT_*` values.
	 * @details 0 is the valid encoding for 1 stop bit, so it is used as given rather than
	 *          being treated as "take the default".
	 */
	u8  char_format;
	/**
	 * @brief bParityType, one of the `LINE_CODING_PARITY_*` values.
	 * @details 0 is the valid encoding for no parity, so it is used as given.
	 */
	u8  parity_type;
	/**
	 * @brief Initial SET_CONTROL_LINE_STATE bitmap: D0 = DTR, D1 = RTS (Ref PSTN 1.2 6.3.12).
	 * @details 0 is a valid state (both lines deasserted), so it is used as given.
	 */
	u8  ctrl_line_state;
} usbh_cdc_acm_config_t;

/**
 * @brief Structure containing callback functions for the CDC ACM host class.
 * @details The user application should provide an instance of this structure
 *          to handle class-specific messages.
 */
typedef struct {
	/**
	 * @brief Called when the CDC ACM host driver initialization.
	 * @return 0 on success, non-zero on failure.
	 */
	int (*init)(void);

	/**
	 * @brief Called when the CDC ACM host driver de-initialization.
	 */
	void (*deinit)(void);

	/**
	 * @brief Called when a CDC ACM device is attached.
	 */
	void (*attached)(void);

	/**
	 * @brief Called when a CDC ACM device is detached.
	 */
	void (*detached)(void);

	/**
	 * @brief Called to handle class-specific SETUP requests completion.
	 */
	void (*setup)(void);

	/**
	 * @brief Called when interrupt data is received from the device (e.g. Serial State).
	 * @param[in] buf: Pointer to the received data buffer.
	 * @param[in] len: Length of the received data in bytes.
	 * @param[in] status: The status of the transfer.
	 */
	void (*notify)(u8 *buf, u32 len, u8 status);

	/**
	 * @brief Called when data is received from the device on the BULK IN pipe.
	 * @param[in] buf: Pointer to the received data buffer.
	 * @param[in] len: Length of the received data in bytes.
	 * @param[in] status: The status of the transfer.
	 */
	void (*received)(u8 *buf, u32 len, u8 status);

	/**
	 * @brief Called when a data transmission to the device on the BULK OUT pipe is complete.
	 * @param[in] status: The status of the transmission.
	 */
	void (*transmitted)(u8 status);

	/**
	 * @brief Called when the line coding parameters have changed.
	 * @param[in] line_coding: Pointer to the new line coding structure.
	 */
	void (*line_coding_changed)(usb_cdc_acm_line_coding_t *line_coding);
} usbh_cdc_acm_cb_t;

/**
 * @brief Structure representing the CDC ACM host instance.
 */
typedef struct {
	usb_host_t *host;                           /**< Pointer to the USB host instance. */
	const usbh_cdc_acm_cb_t *cb;                /**< Pointer to the user-defined callback structure. */
	const usbh_cdc_acm_config_t *cfg;           /**< Pointer to the class configuration (set by init). */

	/* Class state */
	u8 state;                                   /**< EP0 class request state, @ref usbh_cdc_acm_state_t. */
	u8 comm_itf_num;                            /**< bInterfaceNumber of Communication Interface, used as wIndex in class requests (SET_CONTROL_LINE_STATE / SEND_BREAK). */
	u8 bulk_in_busy;                            /**< BULK IN submitted and its completion not yet reported. */
	u8 bulk_out_busy;                           /**< BULK OUT submitted and its completion not yet reported. */
	u8 intr_in_busy;                            /**< INTR IN submitted and its completion not yet reported. */

	/* Pipes */
	usbh_pipe_t bulk_in;                        /**< BULK IN pipe structure. */
	usbh_pipe_t bulk_out;                       /**< BULK OUT pipe structure. */
	usbh_pipe_t intr_in;                        /**< INTERRUPT IN pipe structure. */

	/* Line coding and control signals.
	   Both line codings are the EP0 DMA buffer of GET/SET_LINE_CODING, so each must start on a cache line and own
	   it alone (the RX invalidate would discard CPU writes to a neighbour). Heap blocks from usb_os_malloc() meet
	   both; do not embed them in this structure. */
	usb_cdc_acm_line_coding_t *line_coding;         /**< Current line coding of the device. */
	usb_cdc_acm_line_coding_t *user_line_coding;    /**< User requested line coding. */
	u16 ctrl_line_state;                       /**< Control Signal Bitmap sent in SET_CONTROL_LINE_STATE wValue: D0=DTR, D1=RTS (PSTN §6.3.12). */
	u16 break_duration;                         /**< Duration for SEND_BREAK in milliseconds; 0xFFFF = continuous break. */

#if CONFIG_USBH_CDC_ACM_4G_DONGLE
	const usbh_cdc_acm_param_t *param_item;     /**< Matched 4G dongle param entry for the attached device. */
	u8 data_itf_num;                          /**< Data interface number for SET_INTERFACE. */
	u8 data_itf_alt;                            /**< Data interface alternate setting carrying BULK endpoints. */
	u8 sub_status;                              /**< 4G control-setting sub-state. */
#endif
} usbh_cdc_acm_host_t;

/** @} End of Host_CDC_ACM_Types group */
/** @} End of USB_Host_Types group */

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @addtogroup USB_Host_Functions USB Host Functions
 * @{
 */
/** @addtogroup Host_CDC_ACM_Functions Host CDC ACM Functions
 * @{
 */

/**
 * @brief Initializes the CDC ACM host class driver.
 * @param[in] cb: Pointer to the user-defined callback structure.
 * @param[in] cfg: Pointer to the class configuration. See @ref usbh_cdc_acm_config_t.
 * @return 0 on success, non-zero on failure.
 */
int usbh_cdc_acm_init(const usbh_cdc_acm_cb_t *cb, const usbh_cdc_acm_config_t *cfg);

/**
 * @brief De-initializes the CDC ACM host class driver.
 */
void usbh_cdc_acm_deinit(void);

/**
 * @brief Sets the line coding parameters for the device.
 * @param[in] lc: Pointer to the line coding structure.
 * @return 0 on success, HAL_BUSY while a previous class request is still in flight, other non-zero on failure.
 */
int usbh_cdc_acm_set_line_coding(usb_cdc_acm_line_coding_t *lc);

/**
 * @brief Gets the current line coding parameters from the device.
 * @param[out] lc: Pointer to the structure where the line coding will be stored.
 * @return 0 on success, HAL_BUSY while SET/GET_LINE_CODING is still in flight, other non-zero on failure.
 */
int usbh_cdc_acm_get_line_coding(usb_cdc_acm_line_coding_t *lc);

/**
 * @brief Sets the control line state (PSTN §6.3.12 SET_CONTROL_LINE_STATE).
 * @param[in] bitmap: Control Signal Bitmap written to wValue. D0=DTR, D1=RTS; all other bits reserved and shall be zero.
 * @return 0 on success, HAL_BUSY while a previous class request is still in flight, other non-zero on failure.
 */
int usbh_cdc_acm_set_control_line_state(u16 bitmap);

/**
 * @brief Sends a break signal to the device (PSTN §6.3.13).
 * @param[in] duration_ms: Break duration in milliseconds. 0xFFFF = continuous break; 0x0000 = end break.
 * @return 0 on success, HAL_BUSY while a previous class request is still in flight, other non-zero on failure.
 */
int usbh_cdc_acm_send_break(u16 duration_ms);

/**
 * @brief Transmits data to the device over the BULK OUT pipe.
 * @note  The pipe stays busy until the transmitted callback, and buf must stay valid until then.
 *        buf is accessed by DMA and must be cache line aligned (e.g. declared with USB_DMA_ALIGNED).
 * @param[in] buf: Pointer to the data buffer to be transmitted.
 * @param[in] len: Length of the data in bytes.
 * @return 0 on success, non-zero on failure.
 */
int usbh_cdc_acm_transmit(u8 *buf, u32 len);

/**
 * @brief Prepares to receive data from the device over the BULK IN pipe.
 * @note  The pipe stays busy until the received callback, and buf must stay valid until then.
 *        buf is accessed by DMA and must be cache line aligned (e.g. declared with USB_DMA_ALIGNED).
 *        The controller receives whole packets, so the request is cut down to the packets that
 *        fit in len: use a multiple of the BULK IN MPS to have it used in full.
 * @param[in] buf: Pointer to the buffer where received data will be stored.
 * @param[in] len: Length of the buffer in bytes, at least one BULK IN MPS.
 * @return 0 on success, non-zero on failure.
 */
int usbh_cdc_acm_receive(u8 *buf, u32 len);

#ifdef CONFIG_USBH_CDC_ACM_NOTIFY
/**
 * @brief Prepares to receive notification data (e.g. Serial State) over the INTERRUPT IN pipe.
 * @note  Requires `CONFIG_USBH_CDC_ACM_NOTIFY`, otherwise the class owns no INTR IN pipe
 *        and this function is not built.
 *        buf is accessed by DMA and must be cache line aligned (e.g. declared with USB_DMA_ALIGNED).
 * @param[in] buf: Pointer to the buffer where notification data will be stored.
 * @param[in] len: Length of the buffer in bytes, at least the INTR IN MPS: the controller may fill a whole packet.
 * @return 0 on success, non-zero on failure.
 */
int usbh_cdc_acm_notify_receive(u8 *buf, u32 len);
#endif

/**
 * @brief Gets the Maximum Packet Size (MPS) of the BULK endpoint.
 * @return The MPS in bytes.
 */
u16 usbh_cdc_acm_get_bulk_ep_mps(void);

/** @} End of Host_CDC_ACM_Functions group */
/** @} End of USB_Host_Functions group */

/** @} End of USB_Host_API group */

#ifdef __cplusplus
}
#endif

#endif /* USBH_CDC_ACM_H */
