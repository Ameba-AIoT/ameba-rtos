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

/* Exported defines ----------------------------------------------------------*/

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

/* MSC Request Codes */
#define COMP_MSC_REQUEST_RESET					0xFF            /**< Bulk-Only Mass Storage Reset request. */
#define COMP_MSC_REQUEST_GET_MAX_LUN			0xFE            /**< Get Max LUN request. */

/* CBW/CSW configurations */
#define COMP_MSC_CB_WRAP_LEN					31U            /**< Standard CBW length */
#define COMP_MSC_CB_SIGN						0x43425355U    /**< Standard dCBWSignature, spells out USBC */
#define COMP_MSC_MAX_DATA						256U
#define COMP_MSC_CS_WRAP_LEN					13U            /**< Standard CSW length */
#define COMP_MSC_CS_SIGN						0x53425355U    /**< Standard dCSWSignature, spells out 'USBS' */

/* CSW Status Definitions */
#define COMP_MSC_CSW_CMD_PASSED					0x00U          /**< The `pass` status of the command execution */
#define COMP_MSC_CSW_CMD_FAILED					0x01U          /**< The `fail` status of the command execution */
#define COMP_MSC_CSW_PHASE_ERROR				0x02U          /**< The `phase error` status of the command execution */

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

/* Command Block Wrapper */
typedef struct {
	u32 dCBWSignature;			/* CBW ID: 'USBC' */
	u32 dCBWTag;				/* A Command Block Tag sent by the host, unique per command id */
	u32 dCBWDataTransferLength;	/* The number of bytes of data that the host expects to transfer */
	u8 bmCBWFlags;				/* Data transfer direction */
	u8 bCBWLUN;					/* The device Logical Unit Number to which the command block is being sent */
	u8 bCBWCBLength;			/* The valid length of the CBWCB in bytes */
	u8 CBWCB[16];				/* The command block to be executed by the device */
} usbd_composite_msc_cbw_t;

/* Command Status Wrapper */
typedef struct {
	u32 dCSWSignature;			/* CSW ID: 'USBS' */
	u32 dCSWTag;				/* The device shall set this field to the value received in the dCBWTag of the associated CBW */
	u32 dCSWDataResidue;		/* The amount of data not transferred, shall <= dCBWDataTransferLength */
	u8 bCSWStatus;				/* Execute command status */
} usbd_composite_msc_csw_t;

/* SCSI Sense data */
typedef struct {
	char skey;				/* Sense key */
	union {
		struct _asc {
			char asc;		/* additional sense code */
			char ascq;		/* additional sense code qualifier */
		} b;
		u8 asc;				/* additional sense code */
	} w;
} usbd_composite_msc_scsi_sense_data_t;

typedef struct {
	usbd_ep_t ep_bulk_in;
	usbd_ep_t ep_bulk_out;
	usb_dev_t *dev;
	usbd_composite_dev_t *cdev;
	usbd_composite_msc_cbw_t *cbw;
	usbd_composite_msc_csw_t *csw;
	usbd_composite_msc_disk_ops_t disk_ops;
	usbd_composite_msc_scsi_sense_data_t scsi_sense_data[COMP_MSC_SENSE_LIST_DEPTH];
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

#endif // USBD_COMPOSITE_MSC_H
