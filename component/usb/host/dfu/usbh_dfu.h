/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_DFU_H
#define USBH_DFU_H

/* Includes ------------------------------------------------------------------*/

#include "usbh.h"
#include "usb_dfu.h"

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
/** @addtogroup Host_DFU_Constants Host DFU Constants
 * @{
 */

/**
 * @brief Default maximum transfer size for a single DFU block transfer (bytes).
 * @details Applied when @ref usbh_dfu_config_t::xfer_size is left 0.
 */
#define USBH_DFU_DEFAULT_XFER_SIZE          1024U

/* Protocol defines come from common/usb_dfu.h — use USB_DFU_* names directly */

/*
 * Run-Time / DFU Mode dual-descriptor support (DFU 1.1 §5 Reconfiguration Phase).
 *
 *   1 — Host matches both Run-Time (Protocol=0x01) and DFU (Protocol=0x02) devices.
 *       When a Run-Time device connects, the host issues DFU_DETACH and waits for the
 *       device to re-enumerate in DFU mode before calling cb->attached().
 *   0 — Legacy behaviour: host only matches DFU mode devices (Protocol=0x02).
 */

/* Maximum consecutive write-block retries (CLRSTATUS path) before aborting */
#define USBH_DFU_MAX_RETRY                  3U
/* Upper bound (ms) applied to the device-reported bwPollTimeout. DFU 1.1 6.1.2 makes
 * bwPollTimeout a 24-bit hint (up to ~4.6 h), so this clamps a device reporting a bogus
 * or corrupt value; the wait itself no longer blocks the host task, as it is served in
 * USBH_DFU_POLL_SLICE_MS slices. Waiting less than requested is recoverable: a device
 * still busy answers dfuDNBUSY again and the host simply polls once more. */
#define USBH_DFU_MAX_POLL_TIMEOUT           5000U
/* Longest single sleep (ms) the DFU state machine may hold the shared USB host task for.
 * process() runs on usbh_hcd_main_task, which also drives enumeration, detach handling
 * and every other class driver, so a device-reported poll timeout is served in slices of
 * this size across successive process() calls instead of in one blocking sleep. The size
 * trades the delay seen by the other classes against one host task wakeup per slice. */
#define USBH_DFU_POLL_SLICE_MS              10U
/* Floor applied to bwPollTimeout. DFU 1.1 6.1.2 lets a device report 0; honouring that
 * literally would turn the DNBUSY poll into a back-to-back GETSTATUS loop. */
#define USBH_DFU_POLL_MIN_MS                1U
/* Bounds on how long the host waits for ONE block to finish programming. DFU 1.1 sets no
 * upper limit on device programming time, so timing out a device that never leaves
 * dfuDNBUSY is the host's responsibility. Two bounds, whichever trips first:
 *   - poll count, mirroring USBH_DFU_MAX_MANIFEST_RETRY, catches a device that keeps
 *     answering with bwPollTimeout = 0;
 *   - wall-clock budget, which the count alone cannot bound because each poll waits up
 *     to USBH_DFU_MAX_POLL_TIMEOUT (100 x 5 s would be over 8 min). */
#define USBH_DFU_MAX_DNLOAD_POLL            100U
#define USBH_DFU_DNLOAD_POLL_BUDGET_MS      10000U
/* Maximum GETSTATUS polls while device reports dfuMANIFEST; each poll waits at least
 * bwPollTimeout ms, so the worst case is this count times USBH_DFU_MAX_POLL_TIMEOUT.
 * Unlike the per-block DNBUSY poll above this has no wall-clock budget: manifestation
 * happens once at the end of a download, and the MANIFEST_SYNC branch resets the count
 * only on observed progress. */
#define USBH_DFU_MAX_MANIFEST_RETRY         100U

/** @} End of Host_DFU_Constants group */
/** @} End of USB_Host_Constants group */

/* Exported types ------------------------------------------------------------*/

/** @addtogroup USB_Host_Types USB Host Types
 * @{
 */
/** @addtogroup Host_DFU_Types Host DFU Types
 * @{
 */

/**
 * @brief DFU host-side state machine states.
 */
typedef enum {
	USBH_DFU_STATE_IDLE = 0,           /**< Idle; waiting for a download or upload request. */
	USBH_DFU_STATE_GET_STATUS,         /**< Issuing DFU_GETSTATUS to query device state. */
	USBH_DFU_STATE_CLR_STATUS,         /**< Issuing DFU_CLRSTATUS to clear an error. */
	USBH_DFU_STATE_DNLOAD_BLOCK,       /**< Fetching a block from the application and sending DFU_DNLOAD. */
	USBH_DFU_STATE_DNLOAD_POLL,        /**< Waiting the device-reported poll timeout before retrying. */
	USBH_DFU_STATE_DNLOAD_FINAL,       /**< Sending the zero-length DFU_DNLOAD to signal end-of-firmware. */
	USBH_DFU_STATE_MANIFEST_POLL,      /**< Polling via GETSTATUS until the device finishes manifesting. */
	USBH_DFU_STATE_UPLOAD_BLOCK,       /**< Issuing DFU_UPLOAD to receive one block from the device. */
	USBH_DFU_STATE_DONE,               /**< Transfer complete; about to invoke the completion callback. */
	USBH_DFU_STATE_ERROR,              /**< Error occurred; about to invoke the error callback. */
	USBH_DFU_STATE_RECONFIGURE,        /**< Run-Time device: sending DFU_DETACH to trigger re-enumeration. */
	USBH_DFU_STATE_ABORT,              /**< Issuing DFU_ABORT to cancel an in-progress transfer. */
} usbh_dfu_state_t;

/**
 * @brief User-supplied callbacks for the DFU host class driver.
 */
typedef struct {
	/**
	 * @brief Called during usbh_dfu_init() for any application-side initialization.
	 * @return HAL_OK on success, non-zero on failure.
	 */
	int (*init)(void);

	/**
	 * @brief Called during usbh_dfu_deinit() for application-side cleanup.
	 */
	void (*deinit)(void);

	/**
	 * @brief Called when a DFU device is attached and enumerated.
	 * @details Typically used to give a semaphore so the application thread can proceed.
	 */
	void (*attached)(void);

	/**
	 * @brief Called when the DFU device is detached.
	 * @details Typically used to give a semaphore so the application thread can react.
	 */
	void (*detached)(void);

	/**
	 * @brief Called once per DFU_DNLOAD block to obtain firmware data.
	 * @param[in]  block_num: Zero-based block index, the wBlockNum sent to the device. It wraps
	 *                        from 65535 to 0 (DFU 1.1 6.1.1), so an image of more than 65536
	 *                        blocks needs its own block count to locate the data.
	 * @param[out] buf:       Buffer to fill (at most @p max_len bytes).
	 * @param[in]  max_len:   Maximum bytes the driver can accept (wTransferSize from device).
	 * @return Number of bytes written to @p buf; 0 signals end-of-firmware.
	 */
	int (*get_block)(u16 block_num, u8 *buf, u32 max_len);

	/**
	 * @brief Called when the download operation completes (or fails).
	 * @param[in] status: HAL_OK on success, HAL_ERR_UNKNOWN on failure.
	 */
	void (*download_done)(int status);

	/**
	 * @brief Called after each DFU_UPLOAD block is received from the device.
	 * @param[in] block_num: Zero-based block index, wraps from 65535 to 0 as in get_block.
	 * @param[in] buf:       Buffer containing the received data.
	 * @param[in] len:       Number of bytes in @p buf.
	 * @return HAL_OK to continue uploading; any other value aborts the upload.
	 */
	int (*recv_block)(u16 block_num, u8 *buf, u32 len);

	/**
	 * @brief Called when the upload operation completes (or fails).
	 * @param[in] status: HAL_OK on success, HAL_ERR_UNKNOWN on failure.
	 */
	void (*upload_done)(int status);
} usbh_dfu_cb_t;

/**
 * @brief Configuration for the DFU host class.
 * @details Carries every DFU host option. Host-global parameters (speed, FIFO depths, main
 *          task stack and priority, ISR priority) live in @ref usbh_config_t instead.
 * @note  The structure is stored by pointer, not copied, so it shall remain valid until
 *        @ref usbh_dfu_deinit.
 */
typedef struct {
	/**
	 * @brief Largest DFU block this host will transfer, 0 uses @ref USBH_DFU_DEFAULT_XFER_SIZE.
	 * @details Sizes the internal DMA transfer buffer and therefore caps what the host can
	 *          accept: a device advertising a larger wTransferSize in its DFU Functional
	 *          Descriptor is clamped down to this value. Raising it lets a device use bigger
	 *          blocks at the cost of heap.
	 */
	u32 xfer_size;
} usbh_dfu_config_t;

/**
 * @brief Internal DFU host instance context.
 * @details Allocated statically; one instance shared across attach/detach cycles.
 */
typedef struct {
	usb_host_t          *host;            /**< Pointer to the USB host handle (set on attach). */
	const usbh_dfu_cb_t *cb;              /**< Pointer to the user-supplied callback table. */
	const usbh_dfu_config_t *cfg;         /**< Pointer to the class configuration (set by init). */

	/* DFU state */
	usbh_dfu_state_t     state;           /**< Current host-side DFU state machine state. */
	u8                   dev_state;       /**< Latest device-side bState from GETSTATUS. */
	u8                   dev_status;      /**< Latest device-side bStatus from GETSTATUS. */
	u8                   itf_num;         /**< Matched DFU interface number (set on attach; used as wIndex for all DFU class requests). */
	u8                   is_download;     /**< 1 = download operation in progress; 0 = upload. */
	u8                   is_runtime;      /**< 1 = device attached in Run-Time mode (Protocol=0x01). */
	u8                   reconf_pending;  /**< 1 = attached in Run-Time mode and cb->attached() has not
	                                           been delivered; suppresses cb->detached() until the
	                                           device re-enumerates in DFU mode. */
	u8                   xfer_pending;    /**< 1 while a DNLOAD block ctrl transfer is in flight. */
	usb_dfu_func_desc_t  func_desc;       /**< Parsed DFU Functional Descriptor fields (byte-packed). */

	/* Block transfer */
	u8                  *xfer_buf;        /**< DMA-aligned transfer buffer (xfer_size bytes). */
	u32                  xfer_size;       /**< Allocated size of xfer_buf, i.e. the resolved
	                                           usbh_dfu_config_t::xfer_size. */
	u16                  block_num;       /**< Current block number for DNLOAD or UPLOAD. */
	u16                  last_good_block; /**< Block last confirmed written (dfuDNLOAD-IDLE).
	                                           Initialised to 0xFFFF (no block confirmed yet);
	                                           CLR_STATUS resumes from last_good_block + 1. */
	u16                  xfer_len;        /**< Byte count of the block currently being downloaded;
	                                           saved before the ctrl transfer starts so the DMA
	                                           buffer is not re-filled while the transfer is live. */
	u8                   upload_phase;    /**< Sub-phase inside UPLOAD_BLOCK:
	                                           0 = DFU_UPLOAD ctrl transfer in progress;
	                                           1 = follow-up DFU_GETSTATUS in progress. */
	u8                   retry_cnt;       /**< Consecutive write failures at the same block;
	                                           reset to 0 on each confirmed write. */

	/* bwPollTimeout handling */
	u32                  poll_timeout;    /**< Poll timeout in ms from bwPollTimeout field. */
	u32                  poll_remain;     /**< Length (ms) of the current bwPollTimeout wait, counted
	                                           from poll_start_ms; 0 = no wait pending. */
	u32                  poll_start_ms;   /**< Timestamp (ms) at which the current bwPollTimeout
	                                           wait was armed. */
	u32                  dnload_poll_ms;  /**< Timestamp (ms) at which the current block's DNBUSY
	                                           poll started, bounding it against the wall-clock
	                                           budget; 0 = not started yet. */
	u16                  dnload_poll_cnt; /**< GETSTATUS polls spent on the current block while the
	                                           device reports dfuDNBUSY/dfuDNLOAD-SYNC. */
	u8                   manifest_retry_cnt; /**< GETSTATUS polls while device reports dfuMANIFEST;
	                                              independent of write retry_cnt. */
} usbh_dfu_host_t;

/** @} End of Host_DFU_Types group */
/** @} End of USB_Host_Types group */

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @addtogroup USB_Host_Functions USB Host Functions
 * @{
 */
/** @addtogroup Host_DFU_Functions Host DFU Functions
 * @{
 */

/**
 * @brief Initializes the DFU host class driver.
 * @param[in] cb: Pointer to the user-supplied callback structure.
 * @param[in] cfg: Pointer to the class configuration. See @ref usbh_dfu_config_t.
 * @return HAL_OK on success, HAL_ERR_PARA if cb/cfg is invalid, HAL_ERR_MEM on allocation failure.
 */
int usbh_dfu_init(const usbh_dfu_cb_t *cb, const usbh_dfu_config_t *cfg);

/**
 * @brief De-initializes the DFU host class driver and frees all resources.
 */
void usbh_dfu_deinit(void);

/**
 * @brief Starts a DFU firmware download (host → device).
 * @details Transitions the state machine to USBH_DFU_STATE_GET_STATUS and
 *          triggers the first process() call.  The driver will call
 *          cb->get_block() repeatedly until it returns 0 (EOF), then sends
 *          the final zero-length DFU_DNLOAD and polls for manifestation.
 * @return HAL_OK if the operation was accepted, HAL_ERR_UNKNOWN if no device
 *         is connected.
 */
int usbh_dfu_download(void);

/**
 * @brief Starts a DFU firmware upload (device → host).
 * @details Transitions the state machine to USBH_DFU_STATE_UPLOAD_BLOCK and
 *          triggers the first process() call.  The driver calls cb->recv_block()
 *          for every block received.  The upload ends when the device returns a
 *          short (or zero-length) block.
 * @return HAL_OK if the operation was accepted, HAL_ERR_UNKNOWN if no device
 *         is connected.
 */
int usbh_dfu_upload(void);

/**
 * @brief Sends DFU_ABORT to the device and resets the host state to IDLE.
 * @return HAL_OK on success, HAL_ERR_UNKNOWN if no device is connected.
 */
int usbh_dfu_abort(void);

/**
 * @brief Get the DFU device's bmAttributes from the functional descriptor.
 * @return bmAttributes byte, or 0 if no device is connected.
 */
u8 usbh_dfu_get_attr(void);

/** @} End of Host_DFU_Functions group */
/** @} End of USB_Host_Functions group */

/** @} End of USB_Host_API group */

#ifdef __cplusplus
}
#endif

#endif /* USBH_DFU_H */
