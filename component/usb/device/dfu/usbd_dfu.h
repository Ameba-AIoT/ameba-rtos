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

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup USB_Device_API USB Device API
 *  @{
 */

/* Exported defines ----------------------------------------------------------*/

/** @addtogroup USB_Device_Constants USB Device Constants
 *  @{
 */
/** @defgroup Device_DFU_Constants Device DFU Constants
 *  @brief Descriptor layout, request codes and flash parameters of the DFU class.
 *  @{
 */

/* ---------- Configuration defines (device-specific) -----------------------*/

/* Defaults applied when the matching @ref usbd_dfu_config_t field is left 0/NULL. */

/** Default maximum firmware block transfer size per DNLOAD/UPLOAD request. */
#define USBD_DFU_DEFAULT_XFER_SIZE        1024U
/** Default DFU detach timeout (ms), reported as wDetachTimeOut. */
#define USBD_DFU_DEFAULT_DETACH_TIMEOUT   1000U
/** Default iInterface string. */
#define USBD_DFU_DEFAULT_IFACE_STRING     "DFU Interface"

/* Async write task defaults. frame data is staged in the class buffer, so the stack only needs
 * call frames plus RTOS overhead. */
#define USBD_DFU_DEFAULT_WRITE_TASK_STACK       768U
#define USBD_DFU_DEFAULT_WRITE_TASK_PRIORITY    3U
/** Default ms reported in bwPollTimeout during DNLOAD; 0 lets the host poll immediately. */
#define USBD_DFU_DEFAULT_WRITE_POLL_MS          0U

/* Async manifest task defaults. */
#define USBD_DFU_DEFAULT_MANIFEST_TASK_STACK    768U
#define USBD_DFU_DEFAULT_MANIFEST_TASK_PRIORITY 3U
/** Default ms reported as bwPollTimeout during MANIFEST. */
#define USBD_DFU_DEFAULT_MANIFEST_POLL_MS       100U

/*
 * Run-Time / DFU Mode dual-descriptor support (DFU 1.1 §4.1/§4.2 + §5 Reconfiguration).
 * The device first enumerates in Run-Time mode (bInterfaceProtocol=0x01, appIDLE).
 * When the host issues DFU_DETACH, the device (bitWillDetach=1) performs a bus
 * detach/attach and re-enumerates in DFU mode (bInterfaceProtocol=0x02, dfuIDLE).
 */

/* Byte offset of bInterfaceProtocol inside the config blob: Config(9) + 7 */
#define USBD_DFU_CFG_IF_PROTOCOL_OFFSET   16U
/* Byte offset of iInterface inside the config blob: Config(9) + 8 */
#define USBD_DFU_CFG_IF_ISTR_OFFSET       17U
/* Byte offsets inside the config blob of the DFU Functional Descriptor fields patched at
 * runtime: the descriptor starts after Config(9) + Interface(9), and wDetachTimeOut sits at
 * its offset 3, wTransferSize at its offset 5. */
#define USBD_DFU_CFG_DETACH_TIMEOUT_OFFSET 21U
#define USBD_DFU_CFG_XFER_SIZE_OFFSET      23U
/* Reconfiguration task that performs the off-ISR bus detach/attach */
#define USBD_DFU_DEFAULT_REENUM_TASK_STACK      768U
#define USBD_DFU_DEFAULT_REENUM_TASK_PRIORITY   3U
/** Default delay (ms) after DFU_DETACH so its control status stage completes before detach. */
#define USBD_DFU_DEFAULT_DETACH_STATUS_DELAY_MS 5U

/*
 * Feature switches CONFIG_USBD_DFU_WILL_DETACH, CONFIG_USBD_DFU_MANIFESTATION_TOLERANT and
 * CONFIG_USBD_DFU_CAN_UPLOAD come from Kconfig, refer to its help for the behaviour. They select
 * which code and which usbd_dfu_dev_t members are compiled, so set them in the application prj.conf.
 */

/** @} End of Device_DFU_Constants group */
/** @} End of USB_Device_Constants group */

/* Exported types ------------------------------------------------------------*/

/** @addtogroup USB_Device_Types USB Device Types
 *  @{
 */
/** @defgroup Device_DFU_Types Device DFU Types
 *  @brief Descriptor, state and device context types of the DFU class.
 *  @{
 */

/**
 * @brief Configuration for the DFU class.
 * @details Carries every DFU option. This class is EP0-only (DFU 1.1: no class endpoints), so
 *          there is no endpoint allocation to pass in; what remains is the advertised transfer
 *          size and timeouts plus the sizing of the three asynchronous tasks. Device-global
 *          parameters (VID/PID, strings, power attributes) live in @ref usbd_dev_info_t instead.
 *          The bmAttributes feature bits are compile-time Kconfig options, not fields here,
 *          because they select which code and which device members exist at all.
 * @note  The structure is stored by pointer, not copied, so it shall remain valid until
 *        @ref usbd_dfu_deinit.
 */
typedef struct {
	/**
	 * @brief Maximum firmware block per DNLOAD/UPLOAD, 0 uses @ref USBD_DFU_DEFAULT_XFER_SIZE.
	 * @details Advertised as wTransferSize, so the host honours it, and it also sizes the class
	 *          buffer that temporarily replaces the core's EP0 buffer. It therefore has to be
	 *          DMA-alignable and within what the EP0 path can carry; init validates the
	 *          allocation and rejects a size it cannot serve.
	 */
	u32 xfer_size;
	/**
	 * @brief Detach timeout in ms, 0 uses @ref USBD_DFU_DEFAULT_DETACH_TIMEOUT.
	 * @details Reported as wDetachTimeOut and, when `CONFIG_USBD_DFU_WILL_DETACH` is not set,
	 *          used as the real detach-timer period, so one value drives both and the device
	 *          cannot promise the host a window it does not honour.
	 */
	u32 detach_timeout_ms;
	u32 write_poll_ms;          /**< bwPollTimeout during DNLOAD, 0 uses @ref USBD_DFU_DEFAULT_WRITE_POLL_MS.
	                                 Overridable later via @ref usbd_dfu_set_write_poll_ms. */
	u32 manifest_poll_ms;       /**< bwPollTimeout during MANIFEST, 0 uses @ref USBD_DFU_DEFAULT_MANIFEST_POLL_MS.
	                                 Overridable later via @ref usbd_dfu_set_manifest_poll_ms. */
	const char *itf_str;        /**< iInterface string, NULL uses @ref USBD_DFU_DEFAULT_IFACE_STRING. */
	u16 write_task_stack_size;  /**< Write task stack in bytes, 0 uses @ref USBD_DFU_DEFAULT_WRITE_TASK_STACK. */
	u16 manifest_task_stack_size; /**< Manifest task stack in bytes, 0 uses @ref USBD_DFU_DEFAULT_MANIFEST_TASK_STACK. */
	u16 reenum_task_stack_size; /**< Reconfiguration task stack in bytes, 0 uses @ref USBD_DFU_DEFAULT_REENUM_TASK_STACK. */
	u16 detach_status_delay_ms; /**< Delay after DFU_DETACH before the bus detach, 0 uses
	                                 @ref USBD_DFU_DEFAULT_DETACH_STATUS_DELAY_MS. */
	u8  write_task_priority;    /**< Write task priority, 0 uses @ref USBD_DFU_DEFAULT_WRITE_TASK_PRIORITY. */
	u8  manifest_task_priority; /**< Manifest task priority, 0 uses @ref USBD_DFU_DEFAULT_MANIFEST_TASK_PRIORITY. */
	u8  reenum_task_priority;   /**< Reconfiguration task priority, 0 uses @ref USBD_DFU_DEFAULT_REENUM_TASK_PRIORITY. */
} usbd_dfu_config_t;

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
	 * @param  buf:       Data buffer (usbd_dfu_config_t::xfer_size bytes max).
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
	 *         mode (CONFIG_USBD_DFU_WILL_DETACH=y only).  dfu->mode is already
	 *         USBD_DFU_PROTOCOL_DFU when this fires.  The application must respond
	 *         from a separate task: call usbd_dfu_deinit(), usbd_deinit(),
	 *         usbd_init(), then usbd_dfu_init() to force re-enumeration.
	 *         usbd_dfu_deinit() waits for reconf_task to exit, so calling it from
	 *         this callback would never return.
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

/**
 * @brief Internal DFU device instance (opaque to application).
 */
typedef struct {
	usb_dev_t         *dev;
	const usbd_dfu_cb_t *cb;
	const usbd_dfu_config_t *cfg;       /**< Pointer to the class configuration (set by init). */

	/* DFU state */
	usb_dfu_state_t    state;
	usbd_dfu_status_t  status;
	u8                 mode;            /* Run-Time/DFU mode reconf: current descriptor set, USBD_DFU_PROTOCOL_RUNTIME or _DFU */
	u8                 alt_setting;
	u8                 from_composite;  /**< Flag indicating if part of a composite device. */
	u8                 itf_str_idx;     /**< iInterface string index from usbd_add_string(), 0 for none. */

	/* Async worker handshakes: ISR/task completion flags and deinit stop requests */
	__IO u8            write_state;     /* Async write: block state (0 idle, 1 busy, 2 ok, 3 failed), set by write_task, consumed by GETSTATUS */
	__IO u8            manifest_done;   /* Async manifest: set by manifest_task, read by GETSTATUS/timer in MANIFEST states */
	u8                 reconf_pending;  /* Run-Time/DFU mode reconf: 1 = DFU_DETACH seen, bus re-enumeration requested */
	u8                 write_stop;      /* Async write: set to 1 on deinit to terminate write_task */
	u8                 manifest_stop;   /* Async manifest: set to 1 on deinit to terminate manifest_task and mute its timer */
	u8                 reconf_stop;     /* Run-Time/DFU mode reconf: set to 1 on deinit to terminate reconf_task */

	/* DNLOAD/UPLOAD block transfer */
	u8                *xfer_buf;        /* DNLOAD/UPLOAD block buffer (xfer_size bytes) */
	u32                xfer_size;              /**< Allocated size of xfer_buf, i.e. the resolved
	                                                usbd_dfu_config_t::xfer_size. */
	u8                *ep0_default_buf; /* Saved original ep0_in.xfer_buf (shared ctrl_buf) */
	u32                ep0_default_buf_len;    /* Saved original ep0_out.xfer_buf_len (EP0 IN/OUT share capacity) */
	u16                block_num;              /* wBlockNum from the current DNLOAD/UPLOAD request */
	u16                block_len;              /* wLength saved from DNLOAD setup for ep0_data_out */
	u8                 ep0_in_owned;    /* 1 = the in-flight EP0 IN data stage was armed by DFU */
#ifdef CONFIG_USBD_DFU_CAN_UPLOAD
	u8                 upload_last;     /* 1 = last UPLOAD block was transmitted */
#endif

	/* Async write */
	usb_os_sema_t      write_sema;      /* Async write: ISR signals write_task when a block is ready */
	usb_os_task_t      write_task;      /* Async write: dedicated task that calls cb->write() */
	u32                write_poll_ms;          /* Async write: bwPollTimeout sent to host while write is in progress */

	/* Async manifest */
	usb_os_sema_t      manifest_sema;   /* Async manifest: ISR signals manifest_task when MANIFEST state entered */
	usb_os_task_t      manifest_task;   /* Async manifest: calls cb->manifest() in task context */
	rtos_timer_t       manifest_timer;  /* Async manifest: "status poll timeout" one-shot timer */
	u32                manifest_poll_ms;       /* Async manifest: bwPollTimeout reported during MANIFEST state */

	/* Run-Time/DFU mode reconf and detach */
	usb_os_sema_t      reconf_sema;     /* Run-Time/DFU mode reconf: ISR signals reconf_task to swap descriptor sets */
	usb_os_task_t      reconf_task;     /* Run-Time/DFU mode reconf: performs the bus detach/attach off ISR context */
	u32                detach_timeout_ms;      /**< Resolved usbd_dfu_config_t::detach_timeout_ms; drives both
	                                                the advertised wDetachTimeOut and the detach timer. */
	u16                detach_status_delay_ms; /**< Resolved usbd_dfu_config_t::detach_status_delay_ms. */
#ifndef CONFIG_USBD_DFU_WILL_DETACH
	rtos_timer_t       detach_timer;    /* One-shot timer: fires if host doesn't Reset within wDetachTimeOut */
	u16                detach_wait_ms;  /* Detach timer period: min(DFU_DETACH wValue, wDetachTimeOut) */
#endif
} usbd_dfu_dev_t;

/** @} End of Device_DFU_Types group */
/** @} End of USB_Device_Types group */

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @addtogroup USB_Device_Functions USB Device Functions
 *  @{
 */
/** @defgroup Device_DFU_Functions Device DFU Functions
 *  @brief Standalone and composite initialization, teardown and state query of the DFU class.
 *  @{
 */

/**
 * @brief  Initialize and register the DFU device class driver (standalone).
 * @param  cb: Pointer to user callback structure.
 * @param  cfg: Pointer to the DFU configuration, carrying the flash parameters.
 * @retval 0 on success, non-zero on failure.
 */
int usbd_dfu_init(const usbd_dfu_cb_t *cb, const usbd_dfu_config_t *cfg);

#ifdef CONFIG_USBD_COMPOSITE
/**
 * @brief  Initialize DFU as part of a composite device (driver registration
 *         is handled by composite).
 * @param  cb: Pointer to user callback structure.
 * @param  cfg: Pointer to the DFU configuration, carrying the flash parameters.
 * @retval 0 on success, non-zero on failure.
 */
int usbd_composite_dfu_init(const usbd_dfu_cb_t *cb, const usbd_dfu_config_t *cfg);
#endif

/**
 * @brief  Unregister the DFU device class driver and release resources.
 * @note  Composite mode: call it after @ref usbd_composite_deinit. Before that the class is
 *        still reachable from the USB ISR, so the call returns at once and releases nothing.
 * @retval None. This is a teardown path: apart from the composite case above, the class is always
 *         unregistered and every resource it owns released, nothing for the caller to recover from.
 */
void usbd_dfu_deinit(void);

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

/** @} End of Device_DFU_Functions group */
/** @} End of USB_Device_Functions group */

/** @} End of USB_Device_API group */

#ifdef __cplusplus
}
#endif

#endif /* USBD_DFU_H */
