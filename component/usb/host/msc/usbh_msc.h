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
/** @addtogroup Host_MSC_Constants Host MSC Constants
 * @{
 */

/* MSC Class Codes */
#define MSC_CLASS_CODE                            0x08U       /**< MSC interface class code. */

/* MSC Protocol */
#define USBH_MSC_BOT                              0x50U       /**< MSC `Bulk-Only Transport` interface protocol code. */
#define USBH_MSC_TRANSPARENT                      0x06U       /**< MSC `SCSI Transparent Command Set` interface sub-class code. */

/* A Bulk-In transfer reserves whole packets, so a buffer receiving fewer bytes than the MPS
 * still has to hold one: size the small ones for the HS bulk MPS, refer to usb_get_dma_len(). */
#define USBH_BOT_DATA_LENGTH                      512U        /**< Generic data buffer length for small transfers, one HS bulk packet. */
#define USBH_BOT_CSW_BUF_LENGTH                   512U        /**< CSW buffer length, one HS bulk packet. */

#define USBH_MSC_MAX_LUN                          1U          /**< Maximum number of logical units (LUNs) supported. */

#define USBH_MSC_MIN_BLOCK_SIZE                   512U        /**< Minimum accepted logical block size reported by READ CAPACITY(10). */
#define USBH_MSC_MAX_BLOCK_SIZE                   4096U       /**< Maximum accepted logical block size reported by READ CAPACITY(10). */

#define USBH_MSC_DEFAULT_BUF_SIZE                 (16U * 1024U) /**< Default size of the READ(10)/WRITE(10) bounce buffer. */
/* One READ(10)/WRITE(10) chunk is one channel transfer: at full speed HCTSIZn.PktCnt (10 bits)
 * caps it at 1023 packets of 64 bytes, and the buffer holds a whole number of the largest blocks. */
#define USBH_MSC_MAX_BUF_SIZE                     (60U * 1024U) /**< Maximum size of the READ(10)/WRITE(10) bounce buffer. */

/** @} End of Host_MSC_Constants group */
/** @} End of USB_Host_Constants group */

/* Exported types ------------------------------------------------------------*/

/** @addtogroup USB_Host_Types USB Host Types
 * @{
 */
/** @addtogroup Host_MSC_Types Host MSC Types
 * @{
 */

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
	MSC_ABORT,                  /**< A timed-out READ/WRITE is being abandoned through BOT Reset Recovery. */
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
	usb_msc_bot_cbw_t *cbw;         /**< Pointer to the Command Block Wrapper. */
	usb_msc_bot_csw_t *csw;         /**< Pointer to the Command Status Wrapper. */
	usb_os_sema_t cmd_done_sema;    /**< Given once by the host task when the in-flight READ/WRITE ends, waking its caller. */
	u32 rx_data_len;                /**< Bytes actually sent by the device in the Data-In phase (BOT §6.7 case 5). */
	u32 cmd_address;                /**< LBA of the in-flight READ/WRITE, re-used to re-issue the same CBW (BOT §6.3.3). */
	u32 cmd_block_count;            /**< Block count of the in-flight READ/WRITE, re-used on the retry path. */
	u32 cmd_done_count;             /**< Blocks of the in-flight READ/WRITE already moved; the next chunk starts here. */
	u8 *cmd_rx_buf;                 /**< User buffer of the in-flight READ, receiving the data. */
	const u8 *cmd_tx_buf;           /**< User buffer of the in-flight WRITE, holding the data to send. */
	u8 *pbuf;                       /**< Data buffer of the current command: data, or bounce_buf for READ(10)/WRITE(10). */
	u8 *data;                       /**< A general-purpose data buffer. */
	u8 *bounce_buf;                 /**< READ(10)/WRITE(10) bounce buffer, one chunk of a READ/WRITE. */
	u32 bounce_len;                 /**< Size of bounce_buf, the resolved usbh_msc_config_t::buf_size. */
	u32 tag_counter;                /**< Per-CBW tag; incremented before each CBW send (BOT §6.3.1). */
	u8 state;                       /**< Current state of the BOT state machine, @ref usbh_bot_state_t. */
	u8 reset_recovery;              /**< Set during BOT Reset Recovery, so the endpoint-halt clearing ends in BOT_SEND_CBW. */
	u8 csw_retried;                 /**< 1 once the CSW of the current command was re-read after a Bulk-In clear (BOT 6.7.2). */
	u8 cmd_state;                   /**< Progress of the BOT command layer (send / busy), @ref usbh_bot_cmd_state_t. Not a result. */
	u8 cmd_result;                  /**< Final HAL status of the last READ/WRITE (HAL_OK, HAL_TIMEOUT, ...), valid once
	                                     cmd_done_sema is given. Unlike cmd_state, it is an outcome, not a progress state. */
} usbh_bot_handle_t;

/**
 * @brief SCSI capacity data structure.
 * @details Holds the result from a READ CAPACITY command.
 */
typedef struct {
	u32 block_nbr;                  /**< Total number of logical blocks on the medium. */
	u32 block_size;                 /**< Size of each logical block in bytes. */
} usbh_scsi_capacity_t;

/**
 * @brief SCSI standard INQUIRY data structure.
 * @details Fields decoded from the standard INQUIRY response (SPC-3 6.4.2). The ID strings
 *          are space-padded ASCII and are not NUL-terminated.
 */
typedef struct {
	u8 vendor_id[8];                /**< T10 VENDOR IDENTIFICATION, response bytes 8..15. */
	u8 product_id[16];              /**< PRODUCT IDENTIFICATION, response bytes 16..31. */
	u8 revision_id[4];              /**< PRODUCT REVISION LEVEL, response bytes 32..35. */
	u8 peripheral_qualifier;        /**< PERIPHERAL QUALIFIER, bits 7..5 of response byte 0. */
	u8 device_type;                 /**< PERIPHERAL DEVICE TYPE, bits 4..0 of response byte 0 (0x00 = direct access block device). */
	u8 removable_media;             /**< 1 if the RMB bit (bit 7 of response byte 1) marks the medium removable, else 0. */
} usbh_scsi_inquiry_t;

/**
 * @brief Logical unit (LUN) structure.
 * @details Holds what the driver learned about one logical unit during enumeration, and the
 *          per-unit state of the command currently addressed to it.
 */
typedef struct {
	usbh_scsi_capacity_t capacity;      /**< Geometry reported by READ CAPACITY(10). */
	usb_msc_scsi_sense_data_t sense;    /**< Sense data of the last failed command, from REQUEST SENSE. */
	usbh_scsi_inquiry_t inquiry;        /**< Identity reported by INQUIRY. */
	u8 state;                           /**< Current state of the per-unit state machine, @ref usbh_msc_state_t. */
	u8 error;                           /**< Stores the last occurred error, @ref usbh_msc_error_t. */
	u8 prev_ready;                      /**< 1 if the previous TEST UNIT READY passed, used to detect a ready-state change. */
	u8 state_changed;                   /**< 1 if the last TEST UNIT READY changed the ready state, else 0. */
	u8 retired;                         /**< 1 once a timed-out init or a failed Reset Recovery left the unit unusable; READ/WRITE are
	                                         refused until re-attach. Unlike error, a single failed command never sets it. */
} usbh_msc_lun_t;

/**
 * @brief MSC user callback structure.
 * @details This structure allows the user application to register callbacks for
 *          key MSC notifications like device attachment and detachment.
 */
typedef struct {
	/**
	 * @brief Called when device attached, used to report device connection status.
	 */
	void (*attached)(void);
	/**
	 * @brief Called when device detached, used to report device disconnection status.
	 */
	void (*detached)(void);
	/**
	 * @brief Called when device setup done, used to indicate that device is ready for bulk transfer.
	 */
	void (*setup)(void);
} usbh_msc_cb_t;

/**
 * @brief Configuration for the MSC host class.
 * @note  Read by usbh_msc_init() only, so it need not outlive that call.
 */
typedef struct {
	/**
	 * @brief Size of the READ(10)/WRITE(10) bounce buffer, 0 uses @ref USBH_MSC_DEFAULT_BUF_SIZE.
	 * @details Allocated once by usbh_msc_init(). A READ/WRITE larger than it is moved in chunks of
	 *          this size, one READ(10)/WRITE(10) each, so it trades throughput against heap. Shall be a
	 *          multiple of @ref USBH_MSC_MAX_BLOCK_SIZE not above @ref USBH_MSC_MAX_BUF_SIZE.
	 */
	u32 buf_size;
} usbh_msc_config_t;

/**
 * @brief MSC host structure.
 * @details This structure holds all the state and context for the MSC host driver.
 */
typedef struct {
	const usbh_msc_cb_t *cb;               /**< Pointer to the user-registered callback structure. */
	usb_host_t *host;                      /**< Pointer to the parent USB host structure. */

	/* Class state */
	u8 state;                              /**< Current state of the main MSC state machine, @ref usbh_msc_state_t. */
	u8 req_state;                          /**< Current state of the MSC control request state machine, @ref usbh_msc_req_state_t. */
	u8 current_lun;                        /**< The currently active logical unit number. */
	u8 max_lun;                            /**< Number of LUNs in use: GET MAX LUN + 1, clamped to USBH_MSC_MAX_LUN. */
	u8 itf_num;                            /**< MSC interface number; used as wIndex in class requests (BOT §3.1). */
	u8 init_diag_done;                     /**< 1 once the stuck LUN init diagnostic has been printed for this attach. */
	u8 init_recovery;                      /**< Recovery steps a stuck LUN init has used this attach: 1 after BOT Reset Recovery. */

	/* BOT transport and pipes */
	usbh_bot_handle_t hbot;                /**< Handle for the BOT protocol layer. */
	usbh_pipe_t bulk_in;                   /**< Pipe handle for the Bulk IN endpoint. */
	usbh_pipe_t bulk_out;                  /**< Pipe handle for the Bulk OUT endpoint. */

	/* LUN init timing */
	u8 *max_lun_buf;                       /**< Buffer to hold the result of GET MAX LUN request. */
	u32 sof_tick;                          /**< Host tick of the last SOF wakeup of the BOT FSM, see usbh_msc_sof(). */
	u32 lun_ms;                            /**< OS time in ms when the current LUN init step began, base of its budget. */
	u32 init_ms;                           /**< OS time in ms at attach, diagnostic base for a stuck LUN init. */

	usbh_msc_lun_t unit[USBH_MSC_MAX_LUN]; /**< Array to hold information for each LUN. */
} usbh_msc_host_t;

/** @} End of Host_MSC_Types group */
/** @} End of USB_Host_Types group */

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
 * @param[in] cfg: Pointer to the class configuration. See @ref usbh_msc_config_t.
 * @return HAL_OK on success, HAL_ERR_PARA if cb/cfg is invalid, another HAL error code otherwise.
 */
int usbh_msc_init(const usbh_msc_cb_t *cb, const usbh_msc_config_t *cfg);

/**
 * @brief De-initializes the MSC class driver.
 */
void usbh_msc_deinit(void);

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
 * @brief Gets the number of logical units (LUNs) in use.
 * @return The LUN count, i.e. the GET MAX LUN result + 1 clamped to @ref USBH_MSC_MAX_LUN, so valid
 *         LUN indexes are 0 to count - 1. 0 if no device is attached, 0xFFFFFFFF while the class is
 *         still bringing the LUNs up or a command is in flight.
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
 * @note  Blocks the calling thread until the command ends; the BOT transfer itself runs in the
 *        host task. Hence it must not be called from any MSC or host callback (they run in that
 *        task), not even indirectly through FatFs, e.g. a file operation in the callback set by
 *        fatfs_set_hotplug_usr_cb(). Callers on several threads, as well as usbh_msc_deinit(),
 *        must be serialized by the application.
 * @param[in] lun: The logical unit number to read from.
 * @param[in] address: The starting logical block address.
 * @param[out] pbuf: Pointer to the buffer where data will be stored.
 * @param[in] length: The number of logical blocks to read.
 * @return HAL_OK on success, HAL_TIMEOUT if the command was abandoned, another HAL error code otherwise.
 */
int usbh_msc_read(u8 lun, u32 address, u8 *pbuf, u32 length);

/**
 * @brief Writes data to the MSC device.
 * @note  Same blocking and serialization rules as usbh_msc_read().
 * @param[in] lun: The logical unit number to write to.
 * @param[in] address: The starting logical block address.
 * @param[in] pbuf: Pointer to the buffer containing the data to write.
 * @param[in] length: The number of logical blocks to write.
 * @return HAL_OK on success, HAL_TIMEOUT if the command was abandoned, another HAL error code otherwise.
 */
int usbh_msc_write(u8 lun, u32 address, const u8 *pbuf, u32 length);

/** @} End of Host_MSC_Functions group */
/** @} End of USB_Host_Functions group */

/** @} End of USB_Host_API group */

#ifdef __cplusplus
}
#endif

#endif /* USBH_MSC_H */
