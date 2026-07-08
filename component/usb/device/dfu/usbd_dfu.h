/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBD_DFU_H
#define USBD_DFU_H

/* Includes ------------------------------------------------------------------*/

#include "usbd.h"
#include "usb_dfu.h"

/* Exported defines ----------------------------------------------------------*/

/* ---------- Configuration defines (device-specific) -----------------------*/

/* Maximum firmware block transfer size per DNLOAD/UPLOAD request */
#define USBD_DFU_XFER_SIZE                  1024U

/* DFU detach timeout (ms) */
#define USBD_DFU_DETACH_TIMEOUT             1000U

/* Device/Product IDs */
#define USBD_DFU_VID                        USB_VID               /**< Vendor ID. */
#define USBD_DFU_PID                        USB_PID               /**< Product ID. */

/* String descriptor content */
#define USBD_DFU_LANGID_STRING              0x0409U
#define USBD_DFU_MFR_STRING                 "Realtek"
#define USBD_DFU_PRODUCT_STRING             "Realtek DFU"
#define USBD_DFU_IFACE_STRING               "DFU Interface"

/* DFU configuration descriptor total length: Config(9) + Interface(9) + DFU Func(9) */
#define USBD_DFU_CONFIG_DESC_SIZE           27U

/* Async write task configuration */
#define USBD_DFU_WRITE_TASK_STACK           768U
#define USBD_DFU_WRITE_TASK_PRIORITY        3U
#define USBD_DFU_WRITE_POLL_MS_DEFAULT      0U    /* ms reported in bwPollTimeout; set via usbd_dfu_set_write_poll_ms() */

/* Async manifest task configuration */
#define USBD_DFU_MANIFEST_TASK_STACK        768U
#define USBD_DFU_MANIFEST_TASK_PRIORITY     3U
#define USBD_DFU_MANIFEST_POLL_MS_DEFAULT   100U  /* ms reported as bwPollTimeout during MANIFEST */

/*
 * Run-Time / DFU Mode dual-descriptor support (DFU 1.1 §4.1/§4.2 + §5 Reconfiguration).
 * The device first enumerates in Run-Time mode (bInterfaceProtocol=0x01, appIDLE).
 * When the host issues DFU_DETACH, the device (bitWillDetach=1) performs a bus
 * detach/attach and re-enumerates in DFU mode (bInterfaceProtocol=0x02, dfuIDLE).
 */

/* Byte offset of bInterfaceProtocol inside the config blob: Config(9) + 7 */
#define USBD_DFU_CFG_IF_PROTOCOL_OFFSET     16U
/* Reconfiguration task that performs the off-ISR bus detach/attach */
#define USBD_DFU_REENUM_TASK_STACK          768U
#define USBD_DFU_REENUM_TASK_PRIORITY       3U
/* Delay (ms) after DFU_DETACH so its control status stage completes before detach */
#define USBD_DFU_DETACH_STATUS_DELAY_MS     5U

/*
 * DFU bitWillDetach behaviour (DFU 1.1 §4.2.4, bmAttributes bit 3):
 *   1 — Device generates its own bus detach/attach sequence after DFU_DETACH.
 *       The host must NOT issue a USB Reset; reconf_task toggles the D+ pull-up.
 *       bmAttributes bit 3 (WILL_DETACH) is set in the DFU Functional Descriptor.
 *   0 (default) — Device waits for the host to issue a USB Reset within
 *       wDetachTimeOut (USBD_DFU_DETACH_TIMEOUT) ms after DFU_DETACH.  If the
 *       reset arrives the device re-enumerates in DFU mode; if the timeout expires
 *       without a reset the device reverts to appIDLE (Run-Time mode).
 *       bmAttributes bit 3 is NOT set.
 */
#ifndef CONFIG_USBD_DFU_WILL_DETACH
#define CONFIG_USBD_DFU_WILL_DETACH        1
#endif

/*
 * DFU bitManifestationTolerant behaviour (DFU 1.1 §4.2, bmAttributes bit 2):
 *   1 (default) — Device returns to dfuIDLE immediately after manifest.
 *                 Host can continue further operations (e.g., UPLOAD verify).
 *   0 — Device enters dfuMANIFEST-WAIT-RESET after manifest.
 *       Host must issue a USB bus reset to activate the new firmware.
 *       bmAttributes bit 2 is NOT set.
 */
#ifndef CONFIG_USBD_DFU_MANIFESTATION_TOL
#define CONFIG_USBD_DFU_MANIFESTATION_TOL  1
#endif

/*
 * DFU bitCanUpload (bmAttributes bit 1): enables firmware upload (device→host).
 *   1 (default) — DFU_UPLOAD accepted; cb->read must be provided.
 *   0 — All DFU_UPLOAD stall → dfuERROR.
 */
#ifndef CONFIG_USBD_DFU_CAN_UPLOAD
#define CONFIG_USBD_DFU_CAN_UPLOAD         1
#endif

/* Exported types ------------------------------------------------------------*/

/**
 * @brief DFU status codes (DFU 1.1 spec Table 3-2)
 */
typedef enum {
	USBD_DFU_STATUS_OK                    = 0x00U,
	USBD_DFU_STATUS_ERR_TARGET            = 0x01U,
	USBD_DFU_STATUS_ERR_FILE              = 0x02U,
	USBD_DFU_STATUS_ERR_WRITE             = 0x03U,
	USBD_DFU_STATUS_ERR_ERASE             = 0x04U,
	USBD_DFU_STATUS_ERR_CHECK_ERASED      = 0x05U,
	USBD_DFU_STATUS_ERR_PROG              = 0x06U,
	USBD_DFU_STATUS_ERR_VERIFY            = 0x07U,
	USBD_DFU_STATUS_ERR_ADDRESS           = 0x08U,
	USBD_DFU_STATUS_ERR_NOTDONE           = 0x09U,
	USBD_DFU_STATUS_ERR_FIRMWARE          = 0x0AU,
	USBD_DFU_STATUS_ERR_VENDOR            = 0x0BU,
	USBD_DFU_STATUS_ERR_USBR              = 0x0CU,
	USBD_DFU_STATUS_ERR_POR               = 0x0DU,
	USBD_DFU_STATUS_ERR_UNKNOWN           = 0x0EU,
	USBD_DFU_STATUS_ERR_STALLEDPKT        = 0x0FU,
} usbd_dfu_status_t;

/**
 * @brief DFU GETSTATUS 6-byte response packet
 */
typedef struct {
	u8 bStatus;
	u8 bwPollTimeout[3];    /* little-endian poll timeout in ms */
	u8 bState;
	u8 iString;
} __PACKED usbd_dfu_status_pkt_t;

/**
 * @brief User application callbacks for the DFU device class driver.
 * @note  write() and manifest() are called from dedicated task contexts and
 *        may block (e.g., flash erase/program, OTA activation).  All other
 *        callbacks — setup, read, detach, status_changed — are called from
 *        ISR context and must not block.
 */
typedef struct {
	/**
	 * @brief  Allocate resources, called once before USB enumeration begins.
	 * @retval 0 on success, non-zero on failure.
	 */
	int (*init)(void);

	/**
	 * @brief  Release resources, called on deinitialization.
	 * @retval 0 on success, non-zero on failure.
	 */
	int (*deinit)(void);

	/**
	 * @brief  Write one downloaded firmware block.
	 * @note   Called from a dedicated task context — blocking operations
	 *         such as flash erase/program are permitted.
	 * @param  block_num: wBlockNum from the DFU_DNLOAD request.
	 * @param  buf:       Data buffer (USBD_DFU_XFER_SIZE bytes max).
	 * @param  len:       Actual byte count in this block.
	 * @retval 0 on success.  Non-zero triggers dfuERROR with ERR_WRITE.
	 */
	int (*write)(u16 block_num, u8 *buf, u32 len);

	/**
	 * @brief  Read one firmware block for host upload (synchronous).
	 * @param  block_num: wBlockNum from the DFU_UPLOAD request.
	 * @param  buf:       Output buffer.
	 * @param  max_len:   Maximum bytes to return (equals wLength).
	 * @retval Bytes placed in buf (< max_len signals end-of-image), or <0 on error.
	 */
	int (*read)(u16 block_num, u8 *buf, u32 max_len);

	/**
	 * @brief  Activate newly downloaded firmware.
	 *         Called after all blocks received (zero-length DFU_DNLOAD).
	 *         Device may trigger a reset inside this callback.
	 */
	void (*manifest)(void);

	/**
	 * @brief  DFU_DETACH received while in application runtime mode.
	 */
	void (*detach)(void);

	/**
	 * @brief  Called from reconf_task when the device needs to re-enumerate in DFU
	 *         mode (CONFIG_USBD_DFU_WILL_DETACH=1 only).  dfu->mode is already
	 *         USBD_DFU_PROTOCOL_DFU when this fires.  The application must respond
	 *         from a separate task: call usbd_dfu_deinit(), usbd_deinit(),
	 *         usbd_init(), then usbd_dfu_init() to force re-enumeration.
	 *         May be NULL if re-enumeration is handled another way.
	 */
	void (*reconf)(void);

	/**
	 * @brief  Return the expected write time (ms) for the block about to be written.
	 *         Called from ISR — must not block.  May be NULL; driver falls back to
	 *         the static write_poll_ms value set via usbd_dfu_set_write_poll_ms().
	 * @param  block_num: wBlockNum of the incoming DNLOAD block.
	 * @retval Expected milliseconds for this specific block.
	 */
	u32(*get_write_poll_ms)(u16 block_num);

	/**
	 * @brief  USB cable attach/detach status changed.
	 * @param  old_status: Previous @ref usbd_attach_status_t value.
	 * @param  status:     Current  @ref usbd_attach_status_t value.
	 */
	void (*status_changed)(u8 old_status, u8 status);
} usbd_dfu_cb_t;

/* Interface string descriptor index (beyond the standard 0x00–0x03) */
#define USBD_DFU_IFACE_STRING_IDX           0x04U

/**
 * @brief Internal DFU device instance (opaque to application).
 */
typedef struct {
	usb_dev_t         *dev;
	usbd_dfu_cb_t     *cb;
	u8                *xfer_buf;        /* DNLOAD/UPLOAD block buffer (USBD_DFU_XFER_SIZE) */
	u8                *ep0_default_buf;     /* Saved original ep0_in.xfer_buf (shared ctrl_buf) */
	u32                ep0_default_buf_len;  /* Saved original ep0_out.xfer_buf_len (EP0 IN/OUT share capacity) */
	usb_dfu_state_t    state;
	usbd_dfu_status_t  status;
	u16                block_num;       /* wBlockNum from the current DNLOAD/UPLOAD request */
	u16                block_len;       /* wLength saved from DNLOAD setup for ep0_data_out */
#if CONFIG_USBD_DFU_CAN_UPLOAD
	u8                 upload_last;     /* 1 = last UPLOAD block was transmitted */
#endif
	u8                 alt_setting;
	/* Async write support */
	usb_os_sema_t      write_sema;      /* ISR signals write_task when a block is ready */
	usb_os_task_t      write_task;      /* Dedicated task that calls cb->write() */
	u32                write_poll_ms;   /* bwPollTimeout sent to host while write is in progress */
	u8                 write_stop;      /* Set to 1 on deinit to terminate write_task */
	__IO u8            write_done;      /* set by write_task when block is programmed; read by GETSTATUS */
	/* Async manifest support */
	usb_os_sema_t      manifest_sema;   /* ISR signals manifest_task when MANIFEST state entered */
	usb_os_task_t      manifest_task;   /* Calls cb->manifest() in task context */
	rtos_timer_t       manifest_timer;  /* "Status poll timeout" one-shot timer */
	u32                manifest_poll_ms;/* bwPollTimeout reported during MANIFEST state */
	u8                 manifest_stop;   /* Set to 1 on deinit to terminate manifest_task */
	/* Manifest completion flag (set by manifest_task, read by GETSTATUS/timer in MANIFEST states) */
	__IO u8            manifest_done;
	/* Run-Time / DFU mode reconfiguration support */
	u8                 mode;            /* Current descriptor set: USBD_DFU_PROTOCOL_RUNTIME or _DFU */
	u8                 reconf_pending;  /* 1 = DFU_DETACH seen, bus re-enumeration requested */
	u8                 reconf_stop;     /* Set to 1 on deinit to terminate reconf_task */
	usb_os_sema_t      reconf_sema;     /* ISR signals reconf_task to swap descriptor sets */
	usb_os_task_t      reconf_task;     /* Performs the bus detach/attach off ISR context */
#if !CONFIG_USBD_DFU_WILL_DETACH
	rtos_timer_t       detach_timer;    /* One-shot timer: fires if host doesn't Reset within wDetachTimeOut */
#endif
} usbd_dfu_dev_t;

/* Exported functions --------------------------------------------------------*/

/**
 * @brief  Initialize and register the DFU device class driver.
 * @param  cb: Pointer to user callback structure.
 * @retval 0 on success, non-zero on failure.
 */
int usbd_dfu_init(usbd_dfu_cb_t *cb);


/**
 * @brief  Unregister the DFU device class driver and release resources.
 * @retval 0 on success, non-zero on failure.
 */
int usbd_dfu_deinit(void);

/**
 * @brief  Set the poll timeout reported to the host in bwPollTimeout while
 *         the device is busy writing a firmware block (dfuDNBUSY state).
 *         Call this before usbd_dfu_init() or at any time before the first
 *         DNLOAD block arrives.
 * @param  poll_ms: Expected worst-case write time in milliseconds
 *                  (e.g., flash sector erase + program time).
 *                  0 means the host polls immediately (default).
 */
void usbd_dfu_set_write_poll_ms(u32 poll_ms);

/**
 * @brief  Set the poll timeout (ms) reported to the host in bwPollTimeout
 *         during the MANIFEST state — the "Status poll timeout" period.
 *         After this interval the device transitions out of MANIFEST.
 * @param  poll_ms: Expected manifestation time in milliseconds.
 *                  Default is 100 ms.
 */
void usbd_dfu_set_manifest_poll_ms(u32 poll_ms);

/**
 * @brief  Read the current DFU device state.
 *
 * @retval Current @ref usb_dfu_state_t value (e.g. USB_DFU_STATE_DFU_IDLE).
 */
u8 usbd_dfu_get_state(void);

#endif /* USBD_DFU_H */
