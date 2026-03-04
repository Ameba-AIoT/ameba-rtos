/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBD_MSC_H
#define USBD_MSC_H

/* Includes ------------------------------------------------------------------*/

#include "usbd.h"
#include "usb_msc.h"

/* Exported defines ----------------------------------------------------------*/

/** @addtogroup USB_Device_API USB Device API
 *  @{
 */
/** @addtogroup USB_Device_Constants USB Device Constants
 * @{
 */
/** @addtogroup Device_MSC_Constants Device MSC Constants
 * @{
 */
#define USBD_MSC_TX_THREAD_PRIORITY                 5U                 /**< TX thread priority */
#define USBD_MSC_RX_THREAD_PRIORITY                 5U                 /**< RX thread priority */

/* Defines configuration constants like VID/PID, USB strings, and power settings. */
#define USBD_MSC_VID                                USB_VID            /**< Vendor ID. */
#define USBD_MSC_PID                                USB_PID            /**< Product ID. */
#define USBD_MSC_SELF_POWERED                       1U                 /**< Set to 1 if device is self-powered, 0 for bus-powered. */
#define USBD_MSC_CONFIG_DESC_SIZE                   32U                /**< Size of the MSC configuration descriptor. */
#define USBD_MSC_LANGID_STRING                      0x0409U            /**< Language ID string (0x0409 for U.S. English). */
#define USBD_MSC_MFG_STRING                         "Realtek"          /**< Manufacturer string. */
#define USBD_MSC_PROD_HS_STRING                     "Realtek MSC (HS)" /**< Product string for High-Speed. */
#define USBD_MSC_PROD_FS_STRING                     "Realtek MSC (FS)" /**< Product string for Full-Speed. */
#define USBD_MSC_SN_STRING                          "1234567890"       /**< Serial number string. */

/* Defines MSC-specific endpoint parameters. */
#if defined (CONFIG_AMEBAGREEN2)
#define USBD_MSC_BULK_IN_EP                         0x82U              /**< Endpoint for BULK IN. */
#define USBD_MSC_BULK_OUT_EP                        0x02U              /**< Endpoint for BULK OUT. */
#else
#define USBD_MSC_BULK_IN_EP                         0x81U              /**< Endpoint for BULK IN. */
#define USBD_MSC_BULK_OUT_EP                        0x02U              /**< Endpoint for BULK OUT. */
#endif

#define USBD_MSC_HS_MAX_PACKET_SIZE                 512U               /**< High-Speed BULK IN & OUT maximum packet size. */
#define USBD_MSC_FS_MAX_PACKET_SIZE                 64U                /**< Full-Speed BULK IN & OUT maximum packet size. */

#define USBD_MSC_CTRL_BUF_SIZE                      512U               /**< Control transfer buffer size */

/* MSC configurations */
#define USBD_MSC_FIX_CV_TEST_ISSUE                  0                  /* Enable for CV test */

/* RAM disk configurations */
#ifdef CONFIG_USBD_MSC_RAM_DISK
#define USBD_MSC_RAM_DISK_SIZE                      (USBD_MSC_BUFLEN * 8) /**< Total size of the RAM disk. Should be > 64KB to support ATTO benchmark test. */
#define USBD_MSC_RAM_DISK_SECTORS                   (USBD_MSC_RAM_DISK_SIZE >> USBD_MSC_BLK_BITS) /**< Total number of sectors in RAM disk. */
#endif

/* Defines storage-related parameters like block size and buffer length. */
#define USBD_MSC_BLK_BITS                           9                        /**< Number of bits per block (log2(512)). */
#define USBD_MSC_BLK_SIZE                           (1 << USBD_MSC_BLK_BITS) /**< Block size in bytes (512). */
#define USBD_MSC_BUFLEN                             (16 * 1024)              /**< Default size of the internal data buffer. */

/* BOT state */
#define USBD_MSC_IDLE                               0U          /**< Idle state */
#define USBD_MSC_DATA_OUT                           1U          /**< Data Out state */
#define USBD_MSC_DATA_IN                            2U          /**< Data In state */
#define USBD_MSC_LAST_DATA_IN                       3U          /**< Last Data In state */
#define USBD_MSC_SEND_DATA                          4U          /**< Send Immediate data */
#define USBD_MSC_NO_DATA                            5U          /**< No data Stage */

/* BOT status */
#define USBD_MSC_STATUS_NORMAL                      0U          /**< Normal working status */
#define USBD_MSC_STATUS_RECOVERY                    1U          /**< Get MSC Reset request for recovery */
#define USBD_MSC_STATUS_ERROR                       2U          /**< Error status */

#define USBD_MSC_SENSE_LIST_DEPTH                   4U          /**< Depth of the SCSI sense data list. */

/** @} End of Device_MSC_Constants group*/
/** @} End of USB_Device_Constants group*/

/* Exported types ------------------------------------------------------------*/


/**
 * @brief Disk operation functions structure.
 * @details This structure holds pointers to the low-level disk I/O functions,
 *          abstracting the physical storage medium.
 */
typedef struct {
	/**
	 * @brief Gets the capacity of the disk.
	 * @param[out] sectors: Pointer to a variable to store the total number of sectors.
	 * @return 0 on success, non-zero on failure.
	 */
	int (*disk_getcapacity)(u32 *sectors);

	/**
	 * @brief Reads one or more sectors from the disk.
	 * @param[in] sector: The starting sector number to read from.
	 * @param[out] buffer: Pointer to the buffer to store the read data.
	 * @param[in] count: The number of sectors to read.
	 * @return 0 on success, non-zero on failure.
	 */
	int (*disk_read)(u32 sector, u8 *buffer, u32 count);

	/**
	 * @brief Writes one or more sectors to the disk.
	 * @param[in] sector: The starting sector number to write to.
	 * @param[in] buffer: Pointer to the buffer containing the data to write.
	 * @param[in] count: The number of sectors to write.
	 * @return 0 on success, non-zero on failure.
	 */
	int (*disk_write)(u32 sector, const u8 *buffer, u32 count);
} usbd_msc_disk_ops_t;

/** @addtogroup USB_Device_Types USB Device Types
 * @{
 */
/** @addtogroup Device_MSC_Types Device MSC Types
 * @{
 */
/**
 * @brief MSC user callback structure.
 * @details This structure holds pointers to user-defined callback functions
 *          that are invoked on various MSC events.
 */
typedef struct {
	/**
	 * @brief Called when the USB device status changes for application to support USB hot-plug events.
	 * @param[in] old_status: The previous USB device status.
	 * @param[in] status: The new USB device status.
	 */
	void (*status_changed)(u8 old_status, u8 status);
} usbd_msc_cb_t;
/** @} End of Device_MSC_Types group*/
/** @} End of USB_Device_Types group*/

/**
 * @brief Main structure for the MSC device class.
 * @details This structure holds all the state and context for the active MSC class instance.
 * @note The original snippet was incomplete.
 */
typedef struct {
	usbd_ep_t ep_bulk_in;                           /**< Structure for the Bulk IN endpoint. */
	usbd_ep_t ep_bulk_out;                          /**< Structure for the Bulk OUT endpoint. */
	usb_msc_bot_cbw_t *cbw;                            /**< Pointer to the Command Block Wrapper. */
	usb_msc_bot_csw_t *csw;                            /**< Pointer to the Command Status Wrapper. */
	usbd_msc_disk_ops_t disk_ops;                   /**< Structure with disk operation function pointers. */
	usbd_msc_cb_t *cb;                              /**< Pointer to the user callback structure. */
	usb_dev_t *dev;                                 /**< Pointer to the USB device structure. */
	rtos_task_t rx_task;                            /**< RTOS task handle for data reception. */
	rtos_sema_t rx_sema;                            /**< RTOS semaphore to signal data reception. */
	rtos_task_t tx_task;                            /**< RTOS task handle for data transmission. */
	rtos_sema_t tx_sema;                            /**< RTOS semaphore to signal data transmission. */
	usb_msc_scsi_sense_data_t scsi_sense_data[USBD_MSC_SENSE_LIST_DEPTH]; /**< Array to store SCSI sense data. */
	u8 *ctrl_buf;                                   /**< Pointer to the control transfer buffer. */
	u8 *data;                                       /**< Pointer to the data buffer for transfers. */
	u32 data_length;                                /**< Transfer data length. */
	u32 num_sectors;                                /**< Total number of sectors on the disk. */
	u32 lba;                                        /**< Current logical block address for operations. */
	u32 blkbits;                                    /**< Number of bits per block. */
	u32 blksize;                                    /**< Block size in bytes. */
	u32 blklen;                                     /**< The data length requested by the host. */
	u16 rx_data_length;                             /**< BULK Received data length. */
	u8 tx_status;                                   /**< BULK Transmitted status. */
	u8 ro;                                          /**< Flag for media is write-protected. */
	u8 bot_state;                                   /**< BOT state. */
	u8 bot_status;                                  /**< BOT status. */
	u8 scsi_sense_head;                             /**< Head index of SCSI sense data list. */
	u8 scsi_sense_tail;                             /**< Tail index of SCSI sense data list. */
	u8 is_open : 1;                                 /**< MSC is ready for data transfer. */
	u8 phase_error : 1;                             /**< SCSI check status. */
} usbd_msc_dev_t;

/* Exported functions --------------------------------------------------------*/

/** @addtogroup USB_Device_Functions USB Device Functions
 * @{
 */
/** @addtogroup Device_MSC_Functions Device MSC Functions
 * @{
 */
/**
 * @brief Initializes the MSC device class driver with application callback handler.
 * @param[in] cb: Pointer to the user callback structure.
 * @return 0 on success, non-zero on failure.
 */
int usbd_msc_init(usbd_msc_cb_t *cb);

/**
 * @brief De-initializes the MSC device class driver.
 */
void usbd_msc_deinit(void);

/**
 * @brief Initializes the underlying storage disk.
 * @return 0 on success, non-zero on failure.
 */
int usbd_msc_disk_init(void);

/**
 * @brief De-initializes the underlying storage disk.
 * @return 0 on success, non-zero on failure.
 */
int usbd_msc_disk_deinit(void);
/** @} End of Device_MSC_Functions group */
/** @} End of USB_Device_Functions group */
/** @} End of USB_Device_API group */

#endif // USBD_MSC_H
