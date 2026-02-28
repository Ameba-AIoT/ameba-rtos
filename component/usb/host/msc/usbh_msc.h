/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_MSC_H
#define USBH_MSC_H

/* Includes ------------------------------------------------------------------*/

#include "usbh.h"
#include "usbh_msc_disk.h"
#include "usb_msc.h"

/* Exported defines ----------------------------------------------------------*/

/** @addtogroup USB_Host_API USB Host API
 *  @{
 */
/** @addtogroup USB_Host_Constants USB Host Constants
 * @{
 */
/** @addtogroup Host_MSC_Constants Host MSC Constants
 * @{
 */
/* MSC Class Codes */
#define MSC_CLASS_CODE                            0x08U       /**< MSC interface class code. */

/* MSC Protocol */
#define USBH_MSC_BOT                              0x50U       /**< MSC `Bulk-Only Transport` interface protocol code. */
#define USBH_MSC_TRANSPARENT                      0x06U       /**< MSC `SCSI Transparent Command Set` interface sub-class code. */

#define USBH_BOT_DATA_LENGTH                      64U         /**< Generic data buffer length for small transfers. */

#define USBH_MSC_MAX_LUN                          1U          /**< Maximum number of logical units (LUNs) supported. */
/** @} End of Host_MSC_Constants group*/
/** @} End of USB_Host_Constants group*/

/* Exported types ------------------------------------------------------------*/

/**
 * @brief BOT command state enumeration.
 */
typedef enum {
	BOT_CMD_IDLE = 0,           /**< BOT command layer is idle. */
	BOT_CMD_SEND,               /**< BOT is sending a command. */
	BOT_CMD_BUSY,               /**< BOT is busy processing a command. */
} usbh_bot_cmd_state_t;

/**
 * @brief BOT state machine enumeration for data transfer phases.
 */
typedef enum {
	BOT_SEND_CBW = 1,           /**< Sending Command Block Wrapper (CBW). */
	BOT_DATA_IN,                /**< Data-In phase (Device to Host). */
	BOT_DATA_OUT,               /**< Data-Out phase (Host to Device). */
	BOT_RECEIVE_CSW,            /**< Receiving Command Status Wrapper (CSW). */
	BOT_ERROR_IN,               /**< Error occurred during Data-In phase. */
	BOT_ERROR_OUT,              /**< Error occurred during Data-Out phase. */
	BOT_UNRECOVERED_ERROR       /**< Unrecoverable BOT protocol error. */
} usbh_bot_state_t;

/**
 * @brief MSC state machine enumeration.
 */
typedef enum {
	MSC_INIT = 0,               /**< Initialization state. */
	MSC_IDLE,                   /**< Idle state, waiting for user commands. */
	MSC_TEST_UNIT_READY,        /**< Processing TEST UNIT READY command. */
	MSC_READ_CAPACITY10,        /**< Processing READ CAPACITY(10) command. */
	MSC_READ_INQUIRY,           /**< Processing INQUIRY command. */
	MSC_REQUEST_SENSE,          /**< Processing REQUEST SENSE command. */
	MSC_READ,                   /**< Processing READ command. */
	MSC_WRITE,                  /**< Processing WRITE command. */
	MSC_UNRECOVERED_ERROR,      /**< Unrecoverable error state. */
	MSC_PERIODIC_CHECK,         /**< Periodic check state for device status. */
} usbh_msc_state_t;

/**
 * @brief MSC status enumeration.
 */
typedef enum {
	MSC_OK,                     /**< Command successful. */
	MSC_NOT_READY,              /**< Device or unit is not ready. */
	MSC_ERROR,                  /**< A general error occurred. */
} usbh_msc_error_t;

/**
 * @brief MSC control request state machine enumeration.
 */
typedef enum {
	MSC_REQ_IDLE = 0,           /**< No control request active. */
	MSC_REQ_RESET,              /**< Processing a Bulk-Only Mass Storage Reset request. */
	MSC_REQ_GET_MAX_LUN,        /**< Processing a Get Max LUN request. */
	MSC_REQ_ERROR,              /**< An error occurred during a control request. */
} usbh_msc_req_state_t;

/**
 * @brief BOT protocol handler structure.
 * @details Manages the state and buffers for a single BOT transaction.
 */
typedef struct {
	usbh_bot_state_t state;         /**< Current state of the BOT state machine. */
	usbh_bot_cmd_state_t cmd_state; /**< Current state of the command processing. */
	usb_msc_bot_cbw_t *cbw;         /**< Pointer to the Command Block Wrapper. */
	usb_msc_bot_csw_t *csw;         /**< Pointer to the Command Status Wrapper. */
	u32 origin_tx_pbuf_len;         /**< Original length of the user's transmit buffer. */
	u32 origin_rx_pbuf_len;         /**< Original length of the user's receive buffer. */
	u8 *origin_tx_pbuf;             /**< Pointer to the original user transmit buffer. */
	u8 *origin_rx_pbuf;             /**< Pointer to the original user receive buffer. */
	u8 *pbuf;                       /**< Internal pointer to the current data buffer for transfer. */
	u8 *data;                       /**< A general-purpose data buffer. */
} usbh_bot_handle_t;

/**
 * @brief SCSI capacity data structure.
 * @details Holds the result from a READ CAPACITY command.
 */
typedef struct {
	u32 block_nbr;                  /**< Total number of logical blocks on the medium. */
	u16 block_size;                 /**< Size of each logical block in bytes. */
} usbh_scsi_capacity_t;

/* INQUIRY data */
typedef struct {
	u8 PeripheralQualifier;
	u8 DeviceType;
	u8 RemovableMedia;
	u8 vendor_id[8];
	u8 product_id[16];
	u8 revision_id[4];
} usbh_scsi_inquiry_t;

/* Structure for LUN */
typedef struct {
	usbh_scsi_capacity_t capacity;
	usb_msc_scsi_sense_data_t sense;
	usbh_scsi_inquiry_t inquiry;
	usbh_msc_state_t state;
	usbh_msc_error_t error;
	HAL_Status prev_ready_state;
	u8 state_changed;
} usbh_msc_lun_t;

/** @addtogroup USB_Host_Types USB Host Types
 * @{
 */
/** @addtogroup Host_MSC_Types Host MSC Types
 * @{
 */
/**
 * @brief MSC user callback structure.
 * @details This structure allows the user application to register callbacks for
 *          key MSC events like device attachment and detachment.
 */
typedef struct {
	/**
	 * @brief Called when device attached, used to report device connection status.
	 * @return 0 on success, non-zero on failure.
	 */
	int (*attach)(void);
	/**
	 * @brief Called when device detached, used to report device disconnection status.
	 * @return 0 on success, non-zero on failure.
	 */
	int (*detach)(void);
	/**
	 * @brief Called when device setup done, used to indicate that device is ready for bulk transfer.
	 * @return 0 on success, non-zero on failure.
	 */
	int (*setup)(void);
} usbh_msc_cb_t;
/** @} End of Host_MSC_Types group*/
/** @} End of USB_Host_Types group*/

/**
 * @brief MSC host structure.
 * @details This structure holds all the state and context for the MSC host driver.
 */
typedef struct {
	usbh_pipe_t bulk_in;                   /**< Pipe handle for the Bulk IN endpoint. */
	usbh_pipe_t bulk_out;                  /**< Pipe handle for the Bulk OUT endpoint. */
	usbh_bot_handle_t hbot;                /**< Handle for the BOT protocol layer. */
	usbh_msc_lun_t unit[USBH_MSC_MAX_LUN]; /**< Array to hold information for each LUN. */
	usbh_msc_state_t state;                /**< Current state of the main MSC state machine. */
	usbh_msc_error_t error;                /**< Stores the last occurred error. */
	usbh_msc_req_state_t req_state;        /**< Current state of the MSC control request state machine. */
	usbh_msc_cb_t *cb;                     /**< Pointer to the user-registered callback structure. */
	usb_host_t *host;                      /**< Pointer to the parent USB host structure. */
	u32 *max_lun_buf;                      /**< Buffer to hold the result of GET MAX LUN request. */
	u32 max_lun;                           /**< The maximum logical unit number supported by the device. */
	u32 tick;                              /**< A tick counter for timeouts. */
	u16 current_lun;                       /**< The currently active logical unit number. */
} usbh_msc_host_t;

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @addtogroup USB_Host_Functions USB Host Functions
 * @{
 */
/** @addtogroup Host_MSC_Functions Host MSC Functions
 * @{
 */
/**
 * @brief Initializes the MSC class driver with application callback handler.
 * @param[in] cb: Pointer to a user callback structure.
 * @return 0 on success, non-zero on failure.
 */
int usbh_msc_init(usbh_msc_cb_t *cb);

/**
 * @brief De-initializes the MSC class driver.
 * @return 0 on success, non-zero on failure.
 */
int usbh_msc_deinit(void);
/** @} End of Host_MSC_Functions group */
/** @} End of USB_Host_Functions group */
/** @} End of USB_Host_API group */

/**
 * @brief Main processing function for the MSC BOT state machine.
 * @param[in] host: Pointer to the parent USB host structure.
 * @param[in] lun: The logical unit number to process.
 * @return 0 on success, non-zero on failure.
 */
int usbh_msc_bot_process(usb_host_t *host, u8 lun);

/**
 * @brief Checks if the MSC device is ready for I/O operations.
 * @return 1 if ready, 0 otherwise.
 */
int usbh_msc_is_ready(void);

/**
 * @brief Gets the maximum number of logical units (LUNs) supported by the device.
 * @return The maximum LUN number.
 */
u32 usbh_msc_get_max_lun(void);

/**
 * @brief Checks if a specific logical unit (LUN) is ready.
 * @param[in] lun: The LUN to check.
 * @return 1 if the unit is ready, 0 otherwise.
 */
int usbh_msc_unit_is_ready(u8 lun);

/**
 * @brief Retrieves information about a specific LUN, like capacity and block size.
 * @param[in] lun: The LUN to query.
 * @param[out] info: Pointer to a `usbh_msc_lun_t` structure to store the information.
 * @return 0 on success, non-zero on failure.
 */
int usbh_msc_get_lun_info(u8 lun, usbh_msc_lun_t *info);

/**
 * @brief Reads data from the MSC device.
 * @param[in] lun: The logical unit number to read from.
 * @param[in] address: The starting logical block address.
 * @param[out] pbuf: Pointer to the buffer where data will be stored.
 * @param[in] length: The number of logical blocks to read.
 * @return 0 on success, non-zero on failure.
 */
int usbh_msc_read(u8 lun, u32 address, u8 *pbuf, u32 length);

/**
 * @brief Writes data to the MSC device.
 * @param[in] lun: The logical unit number to write to.
 * @param[in] address: The starting logical block address.
 * @param[in] pbuf: Pointer to the buffer containing the data to write.
 * @param[in] length: The number of logical blocks to write.
 * @return 0 on success, non-zero on failure.
 */
int usbh_msc_write(u8 lun, u32 address, u8 *pbuf, u32 length);

#endif  /* USBH_MSC_H */
