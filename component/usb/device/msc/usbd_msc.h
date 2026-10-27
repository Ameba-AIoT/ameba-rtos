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
#include "usb_scsi.h"

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
/** @addtogroup Device_MSC_Constants Device MSC Constants
 * @{
 */

/* Defaults applied when the matching @ref usbd_msc_config_t field is left 0. */
#define USBD_MSC_DEFAULT_TX_TASK_PRIORITY   5U      /**< Default TX task priority. */
#define USBD_MSC_DEFAULT_RX_TASK_PRIORITY   5U      /**< Default RX task priority. */
#define USBD_MSC_DEFAULT_TRX_TASK_STACK_SIZE 1024U  /**< Default TX/RX task stack size in bytes. */
#define USBD_MSC_DEFAULT_BUF_SIZE      (16U * 1024U)  /**< Default size of the internal data buffer. */

/* Lengths of the SCSI INQUIRY identification fields (Ref SPC Table 59). A configured string
 * shorter than the field is space-padded; a longer one is an error. */
#define USBD_MSC_INQUIRY_VENDOR_LEN    8U           /**< Vendor identification field length. */
#define USBD_MSC_INQUIRY_PRODUCT_LEN   16U          /**< Product identification field length. */
#define USBD_MSC_INQUIRY_REVISION_LEN  4U           /**< Product revision level field length. */

/* Defines MSC-specific endpoint parameters. */
#define USBD_MSC_HS_MAX_PACKET_SIZE    512U               /**< High-Speed BULK IN & OUT maximum packet size. */
#define USBD_MSC_FS_MAX_PACKET_SIZE    64U                /**< Full-Speed BULK IN & OUT maximum packet size. */

/* MSC configurations */
#define USBD_MSC_FIX_CV_TEST_ISSUE     0                  /* Enable for CV test */

/* Defines storage-related parameters like block size and buffer length. */
#define USBD_MSC_BLK_BITS              9U                        /**< Number of bits per block (log2(512)). */
#define USBD_MSC_BLK_SIZE              (1U << USBD_MSC_BLK_BITS) /**< Block size in bytes (512). */
/**
  * @brief Size of the CBW buffer.
  * @note  The BULK OUT DMA window is always a multiple of the endpoint max packet size, so the
  *        buffer used to receive the 31-byte CBW shall be able to hold a full max packet.
  */
#define USBD_MSC_CBW_BUF_LEN           USBD_MSC_HS_MAX_PACKET_SIZE

/* BOT state */
#define USBD_MSC_IDLE                  0U          /**< Idle state */
#define USBD_MSC_DATA_OUT              1U          /**< Data Out state */
#define USBD_MSC_DATA_IN               2U          /**< Data In state */
#define USBD_MSC_LAST_DATA_IN          3U          /**< Last Data In state */
#define USBD_MSC_SEND_DATA             4U          /**< Send Immediate data */

/** @} End of Device_MSC_Constants group */
/** @} End of USB_Device_Constants group */

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
	 * @note   This function is called within an interrupt service routine (ISR) context;
	 *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
	 * @param[in] old_status: The previous USB device status.
	 * @param[in] status: The new USB device status.
	 */
	void (*status_changed)(u8 old_status, u8 status);
} usbd_msc_cb_t;

/** @} End of Device_MSC_Types group */
/** @} End of USB_Device_Types group */

/**
 * @brief MSC endpoint configuration structure.
 * @details This structure holds the endpoint addresses for MSC bulk IN and bulk OUT endpoints.
 */
typedef struct {
	/**
	 * @brief Size of the internal data buffer, 0 uses @ref USBD_MSC_DEFAULT_BUF_SIZE.
	 * @details Bounds how much of a READ/WRITE the class moves per SCSI round trip, so it
	 *          trades throughput against heap. Shall be a non-zero multiple of @ref USBD_MSC_BLK_SIZE.
	 */
	u32 buf_size;
	const char *itf_str;                        /**< iInterface string of the MSC interface, NULL for none. */
	/**
	 * @brief SCSI INQUIRY vendor identification, NULL for the class default.
	 * @details Space-padded to @ref USBD_MSC_INQUIRY_VENDOR_LEN; longer is rejected at init.
	 */
	const char *inquiry_vendor;
	/**
	 * @brief SCSI INQUIRY product identification, NULL for the class default.
	 * @details Space-padded to @ref USBD_MSC_INQUIRY_PRODUCT_LEN; longer is rejected at init.
	 */
	const char *inquiry_product;
	/**
	 * @brief SCSI INQUIRY product revision level, NULL for the class default.
	 * @details Space-padded to @ref USBD_MSC_INQUIRY_REVISION_LEN; longer is rejected at init.
	 */
	const char *inquiry_revision;
	u16 trx_task_stack_size;                    /**< TX/RX task stack size in bytes, 0 uses @ref USBD_MSC_DEFAULT_TRX_TASK_STACK_SIZE. */
	u8  bulk_in_addr;                           /**< Bulk IN endpoint address */
	u8  bulk_out_addr;                          /**< Bulk OUT endpoint address */
	u8  tx_task_priority;                       /**< TX task priority, 0 uses @ref USBD_MSC_DEFAULT_TX_TASK_PRIORITY. */
	u8  rx_task_priority;                       /**< RX task priority, 0 uses @ref USBD_MSC_DEFAULT_RX_TASK_PRIORITY. */
} usbd_msc_config_t;

/**
 * @brief Main structure for the MSC device class.
 * @details This structure holds all the state and context for the active MSC class instance.
 * @note The original snippet was incomplete.
 */
typedef struct {
	const usbd_msc_config_t *cfg;                   /**< Pointer to the class configuration (set by init). */
	const usbd_msc_cb_t *cb;                        /**< Pointer to the user callback structure. */
	usb_dev_t *dev;                                 /**< Pointer to the USB device structure. */

	/* BOT state.
	 * The flags below are written from both the USB ISR and the MSC threads, so each one is a whole byte: bit-fields
	 * sharing a byte would be updated by read-modify-write and could lose a concurrent write on SMP. */
	u8 bot_state;                                   /**< BOT state. */
	u8 bot_status;                                  /**< BOT status. */
	u8 phase_error;                                 /**< SCSI check status. */
	u8 csw_pending;                                 /**< Failed Data-In command whose CSW waits for the Bulk-In halt to be cleared. */
	u8 is_open;                                     /**< MSC is ready for data transfer. */
	u8 wedged;                                      /**< Bulk endpoints halted on an invalid CBW until Reset Recovery. */
	u8 bot_reset_pending;                           /**< Set after BOT Reset; triggers one-time OUT EP reinit in send_csw. */
	u8 ro;                                          /**< Flag for media is write-protected. */

	/* BOT transport */
	usb_msc_bot_cbw_t *cbw;                         /**< Pointer to the Command Block Wrapper. */
	usb_msc_bot_csw_t *csw;                         /**< Pointer to the Command Status Wrapper. */
	u8 *data;                                       /**< Pointer to the data buffer for transfers. */
	u32 buf_size;                                   /**< Allocated size of `data`, i.e. the resolved
	                                                     usbd_msc_config_t::buf_size. */
	u32 data_length;                                /**< Transfer data length. */
	u32 blklen;                                     /**< The data length requested by the host. */
	u16 rx_data_length;                             /**< BULK Received data length. */
	u8 tx_status;                                   /**< BULK Transmitted status. */

	/* Disk and SCSI */
	usbd_msc_disk_ops_t disk_ops;                   /**< Structure with disk operation function pointers. */
	u32 num_sectors;                                /**< Total number of sectors on the disk. */
	u32 lba;                                        /**< Current logical block address for operations. */
	usb_msc_scsi_sense_data_t scsi_sense;           /**< Sense data of the most recent failed command, reported by REQUEST SENSE. */
	u8 from_composite;                              /**< Flag indicating if part of a composite device. */
	u8 itf_str_idx;                                 /**< iInterface string index from usbd_add_string(), 0 for none. */
	/**
	 * @brief SCSI standard INQUIRY response, built at init from the configured identification strings.
	 * @details Held per instance rather than as a const table because the vendor/product/revision
	 *          fields are configurable; answered verbatim by the INQUIRY handler.
	 */
	u8 inquiry_data[INQUIRY_DATA_LEN];

	/* RX / TX threads */
	rtos_task_t rx_task;                           /**< RTOS task handle for data reception. */
	rtos_sema_t rx_sema;                            /**< RTOS semaphore to signal data reception. */
	rtos_task_t tx_task;                            /**< RTOS task handle for data transmission. */
	rtos_sema_t tx_sema;                            /**< RTOS semaphore to signal data transmission. */

	/* Endpoints */
	usbd_ep_t ep_bulk_in;                           /**< Structure for the Bulk IN endpoint. */
	usbd_ep_t ep_bulk_out;                          /**< Structure for the Bulk OUT endpoint. */
} usbd_msc_dev_t;

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @addtogroup USB_Device_Functions USB Device Functions
 * @{
 */
/** @addtogroup Device_MSC_Functions Device MSC Functions
 * @{
 */

/**
 * @brief Initializes the MSC device class driver (standalone).
 * @param[in] cb: Pointer to the user callback structure.
 * @param[in] cfg: Pointer to the class configuration. See @ref usbd_msc_config_t.
 * @return 0 on success, non-zero on failure.
 */
int usbd_msc_init(const usbd_msc_cb_t *cb, const usbd_msc_config_t *cfg);

#ifdef CONFIG_USBD_COMPOSITE
/**
 * @brief Initializes MSC as part of a composite device.
 * @param[in] cb: Pointer to the user callback structure.
 * @param[in] cfg: Pointer to the class configuration. See @ref usbd_msc_config_t.
 * @return 0 on success, non-zero on failure.
 */
int usbd_composite_msc_init(const usbd_msc_cb_t *cb, const usbd_msc_config_t *cfg);
#endif

/**
 * @brief De-initializes the MSC device class driver.
 * @note  Composite mode: call it after @ref usbd_composite_deinit. Before that the class is
 *        still reachable from the USB ISR, so the call returns at once and releases nothing.
 */
void usbd_msc_deinit(void);

/**
 * @brief Initializes the underlying storage disk.
 * @return 0 on success, non-zero on failure.
 */
int usbd_msc_disk_init(void);

/**
 * @brief De-initializes the underlying storage disk.
 * @return None. This is a teardown path: a backend that cannot unmount cleanly is reported through
 *         the log, and the caller has no way to recover from it anyway.
 */
void usbd_msc_disk_deinit(void);

/** @} End of Device_MSC_Functions group */
/** @} End of USB_Device_Functions group */

/** @} End of USB_Device_API group */

#ifdef __cplusplus
}
#endif

#endif /* USBD_MSC_H */
