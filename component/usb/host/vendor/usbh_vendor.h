/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_VENDOR_H
#define USBH_VENDOR_H

/* Includes ------------------------------------------------------------------*/

#include "usbh.h"

/* Exported defines ----------------------------------------------------------*/

/** @addtogroup USB_Host_API USB Host API
 *  @{
 */
/** @addtogroup USB_Host_Constants USB Host Constants
 * @{
 */
/** @addtogroup Host_Vendor_Constants Host Vendor Constants
 * @{
 */
#define VENDOR_CLASS_CODE            0xFFU  /**< Vendor Specific Class Code */
#define VENDOR_SUBCLASS_CODE         0x00U  /**< Vendor Specific SubClass Code */
#define VENDOR_PROTOCOL              0x00U  /**< Vendor Specific Protocol Code */

/* Endpoint Mask definitions for internal testing */
#define USBH_VENDOR_MASK_ALL_EPS     0x3FU   /**< Mask for All Endpoints */
#define USBH_VENDOR_MASK_BULK_IN     (BIT0)  /**< Mask for Bulk IN transfer */
#define USBH_VENDOR_MASK_BULK_OUT    (BIT1)  /**< Mask for Bulk OUT transfer */
#define USBH_VENDOR_MASK_INTR_IN     (BIT2)  /**< Mask for Interrupt IN transfer */
#define USBH_VENDOR_MASK_INTR_OUT    (BIT3)  /**< Mask for Interrupt OUT transfer */
#define USBH_VENDOR_MASK_ISOC_IN     (BIT4)  /**< Mask for Isochronous IN transfer */
#define USBH_VENDOR_MASK_ISOC_OUT    (BIT5)  /**< Mask for Isochronous OUT transfer */
/** @} End of Host_Vendor_Constants group*/
/** @} End of USB_Host_Constants group*/

/* Exported types ------------------------------------------------------------*/

/**
 * @brief  Align data length to 4-byte boundary (Round up).
 * @note   Used for FIFO read operations where 32-bit access alignment is required.
 * @param  x: Original data length (bytes).
 * @retval Aligned length.
 */
#define USB_FIFO_READ_SIZE_ALIGNED4B(x)     ((((u32)x + 3U) / 4U) * 4U )
#define USB_CTRL_BUF_LENGTH                 (18)    /**< Buffer length for control transfer */

/** @addtogroup USB_Host_Types USB Host Types
 * @{
 */
/** @addtogroup Host_Vendor_Types Host Vendor Types
 * @{
 */
/**
 * @brief  States for Vendor State Machine.
 */
typedef enum {
	VENDOR_STATE_IDLE = 0U, /**< Idle state */
	VENDOR_STATE_XFER,      /**< Data transfer start state */
	VENDOR_STATE_BUSY,      /**< Data transfer busy state */
	VENDOR_STATE_ERROR,     /**< Error state */
} usbh_vendor_state_t;

/**
 * @brief Vendor user callback interface structure.
 */
typedef struct {
	/**
	 * @brief Called when the host driver initialization.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* init)(void);

	/**
	 * @brief Called when the host driver de-initialization.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* deinit)(void);

	/**
	 * @brief Called when device attached, used to report device connection status.
	 * @return 0 on success, non-zero on failure.
	 */
	int (*attach)(void);

	/**
	 * @brief CCalled when device detached, used to report device disconnection status.
	 * @return 0 on success, non-zero on failure.
	 */
	int (*detach)(void);

	/**
	 * @brief Called when device setup done, used to indicate that device is ready for data transfer.
	 * @return 0 on success, non-zero on failure.
	 */
	int (*setup)(void);

	/**
	 * @brief Called when IN transfer is completed, used for application to handle the received IN data.
	 * @param  ep_type: Endpoint type (BULK/INTR/ISOC).
	 * @param  buf: Pointer to the received data buffer.
	 * @param  len: Length of received data in bytes.
	 * @param[in] status: The status of the transfer(0 for success)..
	 * @return 0 on success, non-zero on failure.
	 */
	int (*receive)(u8 ep_type, u8 *buf, u32 len, int status);

	/**
	 * @brief Called when OUT transfer is completed, used to report OUT transfer completion status.
	 * @param ep_type: Endpoint type(BULK/INTR/ISOC)..
	 * @return 0 on success, non-zero on failure.
	 */
	int (*transmit)(u8 ep_type);
} usbh_vendor_cb_t;

/**
 * @brief  Vendor transfer control structure
 */
typedef struct {
	usbh_pipe_t pipe;          /**< USB Host pipe handle */
	u8 *xfer_bk_buf;           /**< Backup pointer to the original user buffer */
	u8 *test_buf;              /**< Buffer for verification/testing */
	u32 test_mask;             /**< Bitmask to identify the active transfer */
	u32 xfer_max_len;          /**< Max length of a single transfer */
	u32 cur_frame;             /**< Current frame number (for ISOC synchronization) */
	u8 xfer_cnt;               /**< Current transfer count (for test loops) */
	u8 xfer_max_cnt;           /**< Target transfer count (for test loops) */
} usbh_vendor_xfer_t;

/**
 * @brief  Vendor Host Class Structure
 */
typedef struct {
	usbh_vendor_xfer_t bulk_in_xfer;    /**< Bulk IN transfer handle */
	usbh_vendor_xfer_t bulk_out_xfer;   /**< Bulk OUT transfer handle */
	usbh_vendor_xfer_t intr_in_xfer;    /**< Interrupt IN transfer handle */
	usbh_vendor_xfer_t intr_out_xfer;   /**< Interrupt OUT transfer handle */
	usbh_vendor_xfer_t isoc_in_xfer;    /**< Isochronous IN transfer handle */
	usbh_vendor_xfer_t isoc_out_xfer;   /**< Isochronous OUT transfer handle */
	usbh_vendor_cb_t *cb;               /**< Pointer to user callback structure */
	usb_host_t *host;                   /**< Pointer to USB Host core handle */
	usbh_vendor_state_t state;          /**< Current class state machine state */
	u8 *ctrl_buf;                       /**< Buffer for control transfer */
	u32 ep_mask;                        /**< Active endpoint mask */
} usbh_vendor_host_t;
/** @} End of Host_Vendor_Types group*/
/** @} End of USB_Host_Types group*/

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @addtogroup USB_Host_Functions USB Host Functions
 * @{
 */
/** @addtogroup Host_Vendor_Functions Host Vendor Functions
 * @{
 */
/**
 * @brief  Initialize the Vendor Class driver.
 * @param  cb: Pointer to the application callback structure.
 * @return 0 on success, non-zero on failure.
 */
int usbh_vendor_init(usbh_vendor_cb_t *cb);

/**
 * @brief  De-Initialize the Vendor Class driver.
 * @return 0 on success, non-zero on failure.
 */
int usbh_vendor_deinit(void);

/**
 * @brief  Start a BULK transmission.
 * @param  buf: Pointer to the data buffer to be transmitted.
 * @param  len: Length of the data in bytes.
 * @param  test_cnt: Count of transfer for test. (usually 1 for normal use).
 * @retval Status.
 */
int usbh_vendor_bulk_transmit(u8 *buf, u32 len, u32 test_cnt);

/**
 * @brief  Start a BULK reception.
 * @param  buf: Pointer to the buffer where received data will be stored.
 * @param  len: Expected length of data in bytes.
 * @param  test_cnt: Count of transfer for test.(usually 1 for normal use).
 * @return 0 on success, non-zero on failure.
 */
int usbh_vendor_bulk_receive(u8 *buf, u32 len, u32 test_cnt);

/**
 * @brief  Start an INTERRUPT transmission.
 * @param  buf: Pointer to the data buffer to be transmitted.
 * @param  len: Length of the data in bytes.
 * @param  test_cnt: Count of transfer for test.(usually 1 for normal use).
 * @return 0 on success, non-zero on failure.
 */
int usbh_vendor_intr_transmit(u8 *buf, u32 len, u32 test_cnt);

/**
 * @brief  Start an INTERRUPT reception.
 * @param  buf: Pointer to the buffer where received data will be stored.
 * @param  len: Expected length of data in bytes.
 * @param  test_cnt: Count of transfer for test.(usually 1 for normal use).
 * @return 0 on success, non-zero on failure.
 */
int usbh_vendor_intr_receive(u8 *buf, u32 len, u32 test_cnt);

/**
 * @brief  Start an ISOCHRONOUS transmission.
 * @param  buf: Pointer to the data buffer to be transmitted.
 * @param  len: Length of the data in bytes.
 * @param  test_cnt: Count of transfer for test.(usually 1 for normal use).
 * @return 0 on success, non-zero on failure.
 */
int usbh_vendor_isoc_transmit(u8 *buf, u32 len, u32 test_cnt);

/**
 * @brief  Start an ISOCHRONOUS reception.
 * @param  buf: Pointer to the buffer where received data will be stored.
 * @param  len: Expected length of data in bytes.
 * @param  test_cnt: Count of transfer for test.(usually 1 for normal use).
 * @return 0 on success, non-zero on failure.
 */
int usbh_vendor_isoc_receive(u8 *buf, u32 len, u32 test_cnt);

/**
 * @brief  Get the Max Packet Size (MPS) of the BULK IN endpoint.
 * @retval Max packet size in bytes.
 */
u16 usbh_vendor_get_bulk_ep_mps(void);

/**
 * @brief  Get the Max Packet Size (MPS) of the INTERRUPT IN endpoint.
 * @retval Max packet size in bytes.
 */
u16 usbh_vendor_get_intr_ep_mps(void);
/** @} End of Host_Vendor_Functions group */
/** @} End of USB_Host_Functions group */
/** @} End of USB_Host_API group */

#endif  /* USBH_VENDOR_H */
