/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBD_COMPOSITE_MSC_H
#define USBD_COMPOSITE_MSC_H

/* Includes ------------------------------------------------------------------*/

#include "usbd.h"
#include "usbd_composite_config.h"
#include "usb_msc.h"

/* Exported defines ----------------------------------------------------------*/

/** @addtogroup USB_Device_API USB Device API
 *  @{
 */
/** @addtogroup USB_Device_Constants USB Device Constants
 * @{
 */
/** @addtogroup Device_Composite_MSC_Constants Device Composite MSC Constants
 * @{
 */
#define COMP_MSC_TX_THREAD_PRIORITY             5U     /**< TX thread priority */
#define COMP_MSC_RX_THREAD_PRIORITY             5U     /**< RX thread priority */

/* MSC Endpoint parameters */
#define COMP_MSC_HS_MAX_PACKET_SIZE				512U   /**< High speed BULK IN & OUT packet size */
#define COMP_MSC_FS_MAX_PACKET_SIZE				64U    /**< Full speed BULK IN & OUT packet size */

/* MSC configurations */
#define COMP_MSC_FIX_CV_TEST_ISSUE				0      /**< Enable for CV test */

#define COMP_MSC_BLK_BITS						9      /**< Number of bits per block (log2(512)). */
#define COMP_MSC_BLK_SIZE						(1 << COMP_MSC_BLK_BITS)/**< Block size in bytes (512). */
#define COMP_MSC_BUFLEN							(16*1024)/**< Default size of the internal data buffer. */

/* RAM disk configurations */
#ifdef CONFIG_USBD_COMPOSITE_MSC_RAM_DISK
#define COMP_MSC_RAM_DISK_SIZE					(COMP_MSC_BUFLEN * 8) /**< Total size of the RAM disk. Should be > 64KB to support ATTO benchmark test. */
#define COMP_MSC_RAM_DISK_SECTORS				(COMP_MSC_RAM_DISK_SIZE >> COMP_MSC_BLK_BITS) /**< Total number of sectors in RAM disk. */
#endif

/* CBW/CSW configurations */
#define COMP_MSC_MAX_DATA						256U

/* BOT State */
#define COMP_MSC_IDLE							0U             /* Idle state */
#define COMP_MSC_DATA_OUT						1U             /* Data Out state */
#define COMP_MSC_DATA_IN						2U             /* Data In state */
#define COMP_MSC_LAST_DATA_IN					3U             /* Last Data In Last */
#define COMP_MSC_SEND_DATA						4U             /* Send Immediate data */
#define COMP_MSC_NO_DATA						5U             /* No data Stage */

/* BOT Status */
#define COMP_MSC_STATUS_NORMAL					0U             /**< Normal working status */
#define COMP_MSC_STATUS_RECOVERY				1U             /**< Get MSC Reset request for recovery */
#define COMP_MSC_STATUS_ERROR					2U             /**< Error status */

/* Sense */
#define COMP_MSC_SENSE_LIST_DEPTH               4U             /**< Depth of the SCSI sense data list. */
/** @} End of Device_Composite_MSC_Constants group*/
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
	int(*disk_getcapacity)(u32 *sectors);
	/**
	 * @brief Reads one or more sectors from the disk.
	 * @param[in] sector: The starting sector number to read from.
	 * @param[out] buffer: Pointer to the buffer to store the read data.
	 * @param[in] count: The number of sectors to read.
	 * @return 0 on success, non-zero on failure.
	 */
	int(*disk_read)(u32 sector, u8 *buffer, u32 count);
	/**
	 * @brief Writes one or more sectors to the disk.
	 * @param[in] sector: The starting sector number to write to.
	 * @param[in] buffer: Pointer to the buffer containing the data to write.
	 * @param[in] count: The number of sectors to write.
	 * @return 0 on success, non-zero on failure.
	 */
	int(*disk_write)(u32 sector, const u8 *buffer, u32 count);
} usbd_composite_msc_disk_ops_t;

typedef struct {
	usbd_ep_t ep_bulk_in;
	usbd_ep_t ep_bulk_out;
	usb_dev_t *dev;
	usbd_composite_dev_t *cdev;
	usb_msc_bot_cbw_t *cbw;
	usb_msc_bot_csw_t *csw;
	usbd_composite_msc_disk_ops_t disk_ops;
	usb_msc_scsi_sense_data_t scsi_sense_data[COMP_MSC_SENSE_LIST_DEPTH];
	rtos_task_t rx_task;
	rtos_sema_t rx_sema;
	rtos_task_t tx_task;
	rtos_sema_t tx_sema;
	u32 num_sectors;
	u32 lba; // logic block address
	u32 blkbits; /* bits of logical block size of bound block device */
	u32 blksize;
	u32 blklen;
	u32 data_length;
	u16 rx_data_length;
	u8 tx_status;
	u8 *data;
	u8 ro;
	u8 bot_state;
	u8 bot_status;
	u8 scsi_sense_head;
	u8 scsi_sense_tail;
	u8 is_open : 1;
	u8 phase_error : 1;
} usbd_composite_msc_dev_t;

extern const usbd_class_driver_t usbd_composite_msc_driver;

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Initializes the MSC device class driver.
 * @param[in] cdev: Pointer to the composite device structure.
 * @return 0 on success, non-zero on failure.
 */
int usbd_composite_msc_init(usbd_composite_dev_t *cdev);

/**
 * @brief De-initializes the MSC device class driver.
 */
void usbd_composite_msc_deinit(void);

/** @addtogroup USB_Device_Functions USB Device Functions
 * @{
 */
/** @addtogroup Device_Composite_MSC_Functions Device Composite MSC Functions
 * @{
 */
/**
 * @brief Initializes the underlying storage disk.
 * @return 0 on success, non-zero on failure.
 */
int usbd_composite_msc_disk_init(void);

/**
 * @brief De-initializes the underlying storage disk.
 * @return 0 on success, non-zero on failure.
 */
int usbd_composite_msc_disk_deinit(void);
/** @} End of Device_Composite_MSC_Functions group */
/** @} End of USB_Device_Functions group */
/** @} End of USB_Device_API group */

#endif // USBD_COMPOSITE_MSC_H
