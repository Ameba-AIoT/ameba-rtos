/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_H
#define USBH_H

/* Includes ------------------------------------------------------------------*/

#include "usb_os.h"
#include "usb_ch9.h"

/* Exported defines ----------------------------------------------------------*/

/* This define is used to trace transfer performance */
#define USBH_TP_TRACE_DEBUG                 1U

#define USBH_MAX_CLASS_NUM                  1
#define USBH_MAX_HUB_PORT                   32
#define USBH_EVENT_OWNER                    0x00

/* USB Host interrupt enable flag*/
#define USBH_SOF_INTR                       (BIT0) /**< Start of (micro)Frame GINTSTS.Sof. For host class driver to handle timing issues in SOF callback of @ref usbh_class_driver_t */

/**
 * @brief USBH_DEV_ID_MATCH_XX used in @ref usbh_dev_id_t to specify which fields to match when searching for a device or interface.
 * @{
 */
#define USBH_DEV_ID_MATCH_VID               (BIT0)
#define USBH_DEV_ID_MATCH_PID               (BIT1)
#define USBH_DEV_ID_MATCH_DEV_CLASS         (BIT2)
#define USBH_DEV_ID_MATCH_DEV_SUBCLASS      (BIT3)
#define USBH_DEV_ID_MATCH_DEV_PROTOCOL      (BIT4)
#define USBH_DEV_ID_MATCH_ITF_CLASS         (BIT5)
#define USBH_DEV_ID_MATCH_ITF_SUBCLASS      (BIT6)
#define USBH_DEV_ID_MATCH_ITF_PROTOCOL      (BIT7)
#define USBH_DEV_ID_MATCH_DEVICE            (USBH_DEV_ID_MATCH_VID | USBH_DEV_ID_MATCH_PID)
#define USBH_DEV_ID_MATCH_DEV_INFO          (USBH_DEV_ID_MATCH_DEV_CLASS | USBH_DEV_ID_MATCH_DEV_SUBCLASS | USBH_DEV_ID_MATCH_DEV_PROTOCOL)
#define USBH_DEV_ID_MATCH_ITF_INFO          (USBH_DEV_ID_MATCH_ITF_CLASS | USBH_DEV_ID_MATCH_ITF_SUBCLASS | USBH_DEV_ID_MATCH_ITF_PROTOCOL)
/** @} */

/* Exported macros -----------------------------------------------------------*/

/**
 * @brief Notifies the host core that the class state has changed.
 * @details This is a convenience macro for @ref usbh_notify_composite_class_state_change with owner set to `USBH_EVENT_OWNER`.
 * @param host Pointer to the @ref usb_host_t.
 * @param pipe_num The pipe number associated with the change.
 */
#define usbh_notify_class_state_change(host, pipe_num) usbh_notify_composite_class_state_change(host, pipe_num, USBH_EVENT_OWNER)

/* Exported types ------------------------------------------------------------*/

/**
 * @brief Defines the transfer states for an endpoint's pipe.
 */
typedef enum {
	USBH_EP_XFER_IDLE = 0U,       /**< The endpoint is idle and ready for a new transfer. */
	USBH_EP_XFER_WAIT_SOF,        /**< For periodic transfers, waiting for the correct SOF interval. Check interval in SOF callback. */
	USBH_EP_XFER_START,           /**< A new transfer is starting. */
	USBH_EP_XFER_BUSY,            /**< The endpoint is currently busy with a transfer. */
	USBH_EP_XFER_ERROR,           /**< An error occurred during the last transfer. */
	USBH_EP_XFER_STATE_MAX,       /**< Sentinel value for state validation. */
} usbh_ep_xfer_state_t;

/**
 * @brief Defines the connection states of the USB host.
 */
typedef enum {
	USBH_STATE_IDLE = 0U,         /**< The host is idle. */
	USBH_STATE_DETACHED,          /**< A device has been detached. */
	USBH_STATE_ATTACH,            /**< A device has been attached and is being enumerated. */
	USBH_STATE_SETUP,             /**< The device is in the setup/configured phase. */
	USBH_STATE_MAX,               /**< Sentinel value for state validation. */
} usbh_state_t;

/**
 * @brief Defines the states of a USB Request Block (URB).
 */
typedef enum {
	USBH_URB_IDLE = 0U,           /**< The URB is idle, no transfer is active. */
	USBH_URB_DONE,                /**< The URB has completed successfully. */
	USBH_URB_BUSY,                /**< The URB is currently being processed. */
	USBH_URB_ERROR,               /**< The URB completed with an error. */
	USBH_URB_STALL                /**< The URB resulted in a STALL condition. */
} usbh_urb_state_t;

/**
 * @brief Defines message types for user process callbacks of @ref usbh_user_cb_t.
 */
typedef enum {
	USBH_MSG_USER_SET_CONFIG = 0U,/**< Message to request user to set the configuration. */
	USBH_MSG_CONNECTED,           /**< Message indicating a device has been successfully connected and configured. */
	USBH_MSG_DISCONNECTED,        /**< Message indicating a device has been disconnected. */
	USBH_MSG_PROBE_FAIL,          /**< Message indicating that device probing failed due to mismatched device properties. */
	USBH_MSG_ATTACH_FAIL,         /**< Message indicating device attachment failed. */
	USBH_MSG_ERROR,               /**< Message indicating a general error occurred. */
} usbh_msg_t;

/**
 * @brief  Defines the source for the USB host tick counter.
 */
typedef enum {
	USBH_SW_TICK = 0U,            /**< Use a software-based timer (e.g., system timestamp) as the tick source.
	                                 It does not require responding to additional interrupts, resulting in lower CPU usage.
	                                 However, there may be deviations from SOF counting. Suitable for applications with low CPU
	                                 usage requirements or when timing accuracy does not need to match SOF counting precision.*/
	USBH_SOF_TICK,                /**< Use the SOF (Start-of-Frame) interrupt as the tick source. Since it needs to periodically respond to SOF interrupts,
	                                 CPU usage is higher, but timing accuracy matches SOF counting precision. */
	USBH_TICK_ERROR,
} usbh_tick_source_t;

// Forward declarations to resolve circular dependencies.
struct _usbh_itf_data_t;
struct _usb_host_t;

/**
 * @brief Generic USB descriptor header.
 * @details All standard USB descriptors start with these two fields.
 */
typedef struct {
	u8 bLength;                   /**< Size of this descriptor in bytes. */
	u8 bDescriptorType;           /**< Descriptor type. */
} __PACKED usbh_desc_header_t;

/**
 * @brief USB standard device descriptor structure.
 */
typedef struct {
	u8 bLength;                   /**< Size of this descriptor in bytes. */
	u8 bDescriptorType;           /**< DEVICE Descriptor Type. */
	u16 bcdUSB;                   /**< USB specification version number (e.g., 0x0200 for USB 2.0). */
	u8 bDeviceClass;              /**< Class code (assigned by the USB-IF). */
	u8 bDeviceSubClass;           /**< Subclass code (assigned by the USB-IF). */
	u8 bDeviceProtocol;           /**< Protocol code (assigned by the USB-IF). If equal to 0, each interface specifies its own class
                                     code if equal to 0xFF, the class code is vendor specified.
                                     Otherwise field is valid class code.*/
	u8 bMaxPacketSize;            /**< Maximum packet size for endpoint zero (only 8, 16, 32, or 64 are valid). */
	u16 idVendor;                 /**< Vendor ID (assigned by the USB-IF). */
	u16 idProduct;                /**< Product ID (assigned by the manufacturer). */
	u16 bcdDevice;                /**< Device release number in BCD format. */
	u8 iManufacturer;             /**< Index of string descriptor describing manufacturer. */
	u8 iProduct;                  /**< Index of string descriptor describing product. */
	u8 iSerialNumber;             /**< Index of string descriptor for the serial number. */
	u8 bNumConfigurations;        /**< Number of possible configurations. */
} __PACKED usbh_dev_desc_t;

/**
 * @brief USB standard Binary Device Object Store (BOS) descriptor structure.
 */
typedef struct {
	u8 bLength;                   /**< Size of this descriptor in bytes. */
	u8 bDescriptorType;           /**< BOS Descriptor Type. */
	u16 wTotalLength;             /**< Total length of the BOS descriptor and all its sub-descriptors. */
	u8 bNumDeviceCaps;            /**< The number of device capability descriptors in the BOS. */
} usbh_dev_bos_t;

/**
 * @brief USB standard endpoint descriptor structure.
 */
typedef struct {
	u8 bLength;                   /**< Size of this descriptor in bytes. */
	u8 bDescriptorType;           /**< ENDPOINT Descriptor Type. */
	u8 bEndpointAddress;          /**< The address of the endpoint on the USB device. */
	u8 bmAttributes;              /**< Endpoint attributes (transfer type, etc.). */
	u16 wMaxPacketSize;           /**< Maximum packet size this endpoint is capable of sending or receiving. */
	u8 bInterval;                 /**< Interval for polling endpoint for data transfers. */
} __PACKED usbh_ep_desc_t;

/**
 * @brief USB standard interface descriptor structure.
 */
typedef struct {
	u8 bLength;                    /**< Size of this descriptor in bytes. */
	u8 bDescriptorType;            /**< INTERFACE Descriptor Type. */
	u8 bInterfaceNumber;           /**< Number of this interface. */
	u8 bAlternateSetting;          /**< Value used to select this alternate setting. */
	u8 bNumEndpoints;              /**< Number of endpoints used by this interface (excluding endpoint zero). */
	u8 bInterfaceClass;            /**< Class code (assigned by the USB-IF). */
	u8 bInterfaceSubClass;         /**< Subclass code (assigned by the USB-IF). */
	u8 bInterfaceProtocol;         /**< Protocol code (assigned by the USB-IF). */
	u8 iInterface;                 /**< Index of string descriptor describing this interface. */
	usbh_ep_desc_t *ep_desc_array; /**< Endpoint descriptors */
} __PACKED usbh_itf_desc_t;

/**
 * @brief A structure to hold information about all alternate settings for a single interface number.
 * @details This forms a linked list where each node represents a unique interface number.
 *          Inside each node, it points to another linked list (`itf_desc_array`) of all alternate settings for that interface.
 */
typedef struct _usbh_itf_data_t {
	struct _usbh_itf_data_t *next;       /**< Pointer to the next info structure for a different interface number but has the same class/subclass/protocol. */
	usbh_itf_desc_t *itf_desc_array;     /**< Pointer to a linked list of all alternate settings for this interface number (bAlternateSetting 0, 1, 2...). */
	u8 *raw_data;                        /**< Pointer to the start of the raw interface descriptor data for class-specific parsing. */
	u32 raw_data_len;                    /**< The total length of the entire interface descriptor in bytes. */
	u8 alt_setting_cnt;                  /**< Count of alternate settings for this interface number. */
} usbh_itf_data_t;

/**
 * @brief USB configuration descriptor.
 */
typedef struct {
	u8 bLength;                          /**< Size of this descriptor in bytes. */
	u8 bDescriptorType;                  /**< CONFIGURATION Descriptor Type. */
	u16 wTotalLength;                    /**< Total length of data returned for this configuration. */
	u8 bNumInterfaces;                   /**< The number of interfaces supported by this configuration. */
	u8 bConfigurationValue;              /**< Value to use as an argument to select this configuration. */
	u8 iConfiguration;                   /**< Index of string descriptor describing this configuration. */
	u8 bmAttributes;                     /**< Configuration characteristics (D7:Bus Powered, D6:Self Powered, D5:Remote Wakeup, D4..0 Reserved (0)). */
	u8 bMaxPower;                        /**< Maximum power consumption of the device from the bus in 2mA units. */
	usbh_itf_data_t *itf_data_array;     /**< Save all interfaces info in this struct, total count is defined by bNumInterfaces,
                                             each interface is indentified by bInterfaceNumber: 0/1/2... */
} usbh_cfg_desc_t;

/**
 * @brief USB standard hub descriptor structure. (USB 2.0 Spec, 11.23.2.1).
 */
typedef struct {
	u8 bLength;                          /**< Size of this descriptor in bytes. */
	u8 bDescriptorType;                  /**< HUB Descriptor Type. */
	u8 bNbrPorts;                        /**< Number of downstream ports that this hub supports. */
	u16 wHubCharacteristics;             /**< Hub characteristics (power switching, over-current protection, etc.).
                                             D1...D0: Logical Power Switching Mode
                                             D2: Identifies a Compound Device
                                             D4...D3: Over-current Protection Mode
                                             D6...D5: TT Think TIme
                                             D7: Port Indicators Supported
                                             D15...D8: Reserved*/
	u8 bPwrOn2PwrGood;                   /**< Time (in 2ms intervals) from when power is turned on to when power is good. */
	u8 bHubContrCurrent;                 /**< Maximum current requirements of the hub controller in mA. */
	/**< add 1 bit for hub status change; round to bytes */
	u8 DeviceRemovable[(USBH_MAX_HUB_PORT + 1 + 7) / 8];    /**< Indicates if a port has a removable device attached.
                                                               Bit 0: Reserved for future use.
                                                               Bit 1: Port 1
                                                               Bit n: Port n (implementation-dependent, up to a maximum of 255 ports). */
	u8 PortPwrCtrlMask[(USBH_MAX_HUB_PORT + 1 + 7) / 8];    /**< Port power control mask (obsolete).
                                                                This field exists for reasons of compatibility with software written for 1.0 compliant devices.
                                                                All bits in this field should be set to 1B. This field has one bit for each port on the hub with additional pad bits,
                                                                if necessary, to make the number of bits in the field an integer multiple of 8.*/
} usbh_hub_desc_t;

/**
 * @brief Union for a standard USB setup request packet.
 */
typedef union {
	u32 d32[2];                        /**< 64-bit access to the setup packet data. */
	usb_setup_req_t req;               /**< Access as a structured setup request. */
} usbh_setup_req_t;

/**
 * @brief Packed structure for a USB event message.
 */
typedef struct {
	u16 tick;                          /**< Tick count when the event occurred. */
	u8 pipe_num;                       /**< Pipe number associated with the event. */
	u8 type: 4;                        /**< Message type, corresponds to `usbh_event_type_t`. */
	u8 owner: 4;                       /**< Owner of the message (e.g., core, class). */
} __PACKED usbh_event_msg_t;

/**
 * @brief Union for a USB event.
 */
typedef union {
	u32 d32;                          /**< 32-bit access to the event data. */
	usbh_event_msg_t msg;             /**< Access as a structured event message. */
} usbh_event_t;

/**
 * @brief USB host user configuration structure.
 */
typedef struct {
	u32 ext_intr_enable;              /**< Allows class drivers to enable optional interrupts,
	                                     (e.g., @ref USBH_SOF_INTR is used for host class driver to handle timing issues in SOF callback). */
	u16 rx_fifo_depth;                /**< RxFIFO depth in dwords (Dedicated FIFO only).
	                                     Min value 16 and max value restricted by SoC hardware. */
	u16 nptx_fifo_depth;              /**< Non-periodic TxFIFO depth in dwords (Dedicated FIFO only).
	                                     Min value 16 and max value restricted by SoC hardware. */
	u16 ptx_fifo_depth;               /**< Periodic TxFIFO depth in dwords (Dedicated FIFO only).
	                                     Min value 16 and max value restricted by SoC hardware. */
	u8 isr_priority;                  /**< USB ISR priority. */
	u8 isr_in_critical;               /**< Flag to process USB ISR within a critical section (0: Disable, 1: Enable). */
	u8 main_task_priority;            /**< USB main task priority, the main task processes the USB host messages. */
	/**
	 * @brief USB speed mode. See @ref usb_speed_type_t.
	 * - `USB_SPEED_HIGH`: USB 2.0 High-Speed mode(for HS-capable SoCs).
	 * - `USB_SPEED_HIGH_IN_FULL`: USB 2.0 PHY operating in Full-Speed mode(for HS-capable SoCs with low bandwidth applcations like UAC).
	 * - `USB_SPEED_FULL`: USB 1.1 Full-Speed mode(for FS-only SoCs).
	 */
	u8 speed;
	u8 tick_source;                   /**< Tick source for getting the usb host tick of USB host core driver, see @ref usbh_tick_source_t.
	                                     Which is used to trigger periodic transfers based on the endpoint interval and to detect transfer timeouts.*/
	u8 xfer_retry_max_cnt;            /**< Maximum number of retries for a failed transfer. */
	u8 hub_support;                   /**< Support 1-level HUB (0: Disable, 1: Enable). */
#if USBH_TP_TRACE_DEBUG
	u8 tp_trace;                      /**< Enable/disable tracing for transfer performance. */
#endif
} usbh_config_t;

/**
 * @brief USB pipe structure for managing data transfers to/from an endpoint.
 */
typedef struct {
	u8 *xfer_buf;                     /**< Pointer to the transfer buffer. */
	u32 xfer_len;                     /**< Total length of the data to transfer. */
	__IO u32 frame_num;               /**< Frame number for the transfer (from HFNUM register, max 0x3FFF). */
	__IO u32 tick;                    /**< Host tick count at the start of the transfer (based on SOF or timestamp). */
	u16 max_timeout_tick;             /**< Maximum wait timeout in ticks for this transfer. */
	u16 ep_interval;                  /**< Endpoint polling interval in ticks. */

	/* Endpoint information get from the endpoint descriptor */
	u16 ep_mps;                       /**< Endpoint Maximum Packet Size (0-64KB). */
	u8 ep_trans;                      /**< Number of transactions per microframe (1-3, for HS isochronous). */
	u8 ep_is_in;                      /**< Endpoint direction: 1 for IN, 0 for OUT. */
	u8 ep_addr;                       /**< Endpoint address (including direction bit). */
	u8 ep_type;                       /**< Endpoint type, `USB_CH_EP_TYPE_XXX` (Control, Bulk, Interrupt, Isochronous). */

	u8 pipe_num;                      /**< Host pipe/channel number assigned to this endpoint. */
	__IO u8 xfer_state;               /**< Current transfer state. See @ref usbh_ep_xfer_state_t. */
	__IO u8 trx_zlp;                  /**< Flag to indicate if a Zero-Length Packet is required, only for BULK xfer with xfer_len is N*mps. */
	__IO u8 retry_cnt;                /**< Current retry count for the transfer. */
} usbh_pipe_t;

/**
 * @brief Structure for defining device/interface properties to match against.
 * @details This is used by class drivers to identify supported devices.
 */
typedef struct {
	/* Bitmask of flags from `USBH_DEV_ID_MATCH_XXX` specifying which fields are valid. */
	u16 mMatchFlags;

	/* Used for product specific matches. */
	u16 idVendor;
	u16 idProduct;

	/* Used for device class matches. */
	u8 bDeviceClass;
	u8 bDeviceSubClass;
	u8 bDeviceProtocol;

	/* Used for interface class matches. */
	u8 bInterfaceClass;
	u8 bInterfaceSubClass;
	u8 bInterfaceProtocol;
} usbh_dev_id_t;

/**
 * @brief Structure defining the interface for a USB class driver.
 */
typedef struct {
	const usbh_dev_id_t *id_table;                        /**< A table of device IDs that this driver supports. */
	/**
	* @brief Called after `SET_CONFIGURATION` request when a supported device is attached.
	* @param[in] host: USB host.
	* @return 0 on success, non-zero on failure.
	*/
	int(*attach)(struct _usb_host_t *host);

	/**
	* @brief Called when a supported device is detached.
	* @param[in] host: USB host.
	* @return 0 on success, non-zero on failure.
	*/
	int(*detach)(struct _usb_host_t *host);

	/**
	* @brief Called after attach to handle class-specific standard control requests.
	* @param[in] host: USB host.
	* @return 0 on success, non-zero on failure.
	*/
	int(*setup)(struct _usb_host_t *host);

	/**
	* @brief Main processing loop for the class driver after class setup to process class-specific transfers.
	* @param[in] host: USB host.
	* @param[in] msg: @ref usbh_msg_t.
	* @return 0 on success, non-zero on failure.
	*/
	int(*process)(struct _usb_host_t *host, u32 msg);

	/**
	* @brief Called at each Start-of-Frame (SOF) interrupt for class-specific timing process.
	* @param[in] host: USB host.
	* @return 0 on success, non-zero on failure.
	*/
	int(*sof)(struct _usb_host_t *host);

	/**
	* @brief Called when a transfer on a specific pipe completes.
	* @param[in] host: USB host.
	* @param[in] pipe: Pipe number.
	* @return 0 on success, non-zero on failure.
	*/
	int(*completed)(struct _usb_host_t *host, u8 pipe);
} usbh_class_driver_t;

/**
 * @brief Structure defining user-level callbacks.
 */
typedef struct {
	/**
	* @brief Callback to handle class-independent events in the application.
	* @param[in] host: USB host.
	* @param[in] msg: @ref usbh_msg_t.
	* @return 0 on success, non-zero on failure.
	*/
	int(*process)(struct _usb_host_t *host, u8 msg);

	/**
	 * @brief Callback to validate a device when multiple devices are connected to a hub.
	 * @param[in] host: USB host.
	 * @param[in] cfg_max: bNumConfigurations.
	 * @note
	 *    The class can get all the config descriptor and check the interface class.
	 *	   if find the device, return HAL_OK, the enum process will use this device as the target device,
	 *	   else return !HAL_OK, the enum process will discard this device, and switch to check next hub port.
	 * @return 0 on success, non-zero on failure.
	 */
	int(*validate)(struct _usb_host_t *host, u8 cfg_max);
} usbh_user_cb_t;

/**
 * @brief USB host.
 */
typedef struct _usb_host_t {
#if USBH_TP_TRACE_DEBUG
	/**
	 * @name ISR Time Tracing
	 * @{
	 * @details These fields are used for debugging time-sensitive transfers (like isochronous for audio applications)
	 *          by measuring the execution time of the interrupt handler.
	 *          Enabled by `tp_trace` in `usbh_config_t`.
	 * 	     - if the isr_process_time is relatively large, check whether the callback of the class has taken a long time.
	 * 	     - if the isr_enter_period is relatively large, check whether there is an operation to mask interrupts in the class.
	 */
	__IO u64 isr_enter_time;            /**< [Debug] Record the timestamp of interrupt handler entry. */
	__IO u64 isr_enter_period;          /**< [Debug] The interval between two consecutive interrupt processing. */
	__IO u64 isr_enter_period_max;      /**< [Debug] The maximum value of the interval between two consecutive interrupt processing. */
	__IO u64 isr_process_time;          /**< [Debug] Time consumption for a complete interrupt handler processing. */
	__IO u64 isr_process_time_max;      /**< [Debug] The maximum time recorded for a complete interrupt handler processing */
	/** @} */
#endif
	const usbh_dev_id_t *dev_id;        /**< Pointer to the active device ID. */
	usbh_dev_desc_t *dev_desc;          /**< Pointer to the device's descriptor. */
	usbh_user_cb_t *cb;                 /**< Pointer to the user-provided callbacks. */
	void *core;                         /**< Pointer for USB host core. */

	u8 dev_addr;                        /**< The address of the attached device. */
	u8 dev_speed;                       /**< The speed of the attached device. */
	u8 connect_state;                   /**< The current connection state. See @ref usbh_state_t. */
} usb_host_t;

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/*
 * API for application
 */
/**
 * @brief Initialize USB host core driver with user configuration and callback.
 * @param[in] cfg: USB user configuration.
 * @param[in] cb: USB user callback.
 * @return 0 on success, non-zero on failure.
 */
int usbh_init(usbh_config_t *cfg, usbh_user_cb_t *cb);

/**
 * @brief Deinitialize USB host core driver.
 * @return 0 on success, non-zero on failure.
 */
int usbh_deinit(void);

/*
 * API for class
 */
/**
 * @brief Register a host class driver, called in class initialization function.
 * @param[in] driver: USB class driver.
 * @return 0 on success, non-zero on failure.
 */
int usbh_register_class(usbh_class_driver_t *driver);

/**
 * @brief Un-register a class, called in class de-initialization function.
 * @param[in] driver: USB class driver.
 * @return 0 on success, non-zero on failure.
 */
int usbh_unregister_class(usbh_class_driver_t *driver);

/* Pipe operations */
/**
 * @brief  Open a pipe.
 * @param[in] host: Host Handle.
 * @param[in] pipe: Pipe struct handle.
 * @param[in] ep_desc: Endpoint descriptor handle.
 * @return 0 on success, non-zero on failure.
 */
int usbh_open_pipe(usb_host_t *host, usbh_pipe_t *pipe, usbh_ep_desc_t *ep_desc);

/**
 * @brief  Close a pipe.
 * @param[in] host: Host Handle.
 * @param[in] pipe: Pipe struct handle.
 * @return 0 on success, non-zero on failure.
 */
int usbh_close_pipe(usb_host_t *host, usbh_pipe_t *pipe);

/* Config operations, choose the config index while bNumConfigurations > 1 */
/**
 * @brief  Get the config idx by devicd id information.
 * @param[in] host: Host Handle.
 * @param[in] id: Device id information.
 * @return config index
 */
u8 usbh_get_configuration(usb_host_t *host, usbh_dev_id_t *id);
/**
 * @brief  Set the config index for bNumConfigurations>1 in device descriptor.
 * @param[in] host: Host Handle.
 * @param[in] cfg: Config index.
 * @return 0 on success, non-zero on failure.
 */
int usbh_set_configuration(usb_host_t *host, u8 cfg);

/* Descriptor operations */
/**
 * @brief  Get the interface descriptor by devicd id information.
 * @param[in] host: Host Handle.
 * @param[in] id: Device id information.
 * @return interface descriptor handler.
 * @note NULL means interface not found.
 */
usbh_itf_data_t *usbh_get_interface_descriptor(usb_host_t *host, usbh_dev_id_t *id);

/**
 * @brief  Find next standard descriptor that matches the descriptor type.
 * @param[in] buf: buffer to find.
 * @param[in] len: buffer length.
 * @param[in] desc_type: Descriptor type.
 * @return Pointer of next standard descriptor.
 */
u8 *usbh_find_next_descriptor(u8 *buf, u32 *len, u8 desc_type);

/**
 * @brief  Get the current time ticks.
 * @param[in] host: Host Handle.
 * @return  tick count.
 */
u32 usbh_get_tick(usb_host_t *host);

/**
 * @brief  Get the elapse ticks.
 * @param[in] host: Host Handle.
 * @param[in] start_tick: Start tick count.
 * @return  Tick count.
 */
u32 usbh_get_elapsed_ticks(usb_host_t *host, u32 start_tick);

/**
 * @brief  Get current frame number.
 * @param[in] host: Host Handle.
 * @return Current frame number.
 */
u32 usbh_get_current_frame_number(usb_host_t *host);

/**
 * @brief  Get the elapse frame number.
 * @param[in] host: Host Handle.
 * @param[in] start_frame: Start frame number.
 * @return Tick count.
 */
u32 usbh_get_elapsed_frame_cnt(usb_host_t *host, u32 start_frame);

/**
 * @brief  Get the URB state.
 * @param[in] host: Host handle.
 * @param[in] pipe: Pipe struct handle.
 * @return URB state.
 */
usbh_urb_state_t usbh_get_urb_state(usb_host_t *host, usbh_pipe_t *pipe);

/**
 * @brief  Notify composite class state change.
 * @param[in] host: Host Handle.
 * @param[in] pipe_num: Pipe number.
 * @param[in] owner: Notify owner class.
 * @return 0 on success, non-zero on failure.
 */
int usbh_notify_composite_class_state_change(usb_host_t *host, u8 pipe_num, u8 owner);

/* Transfer operations */
/**
 * @brief  Send `SET_INTERFACE` standard request to device.
 * @param[in] host: Host Handle.
 * @param[in] itf_num: Interface number.
 * @param[in] alt_setting: Interface alternate setting number.
 * @return 0 on success, non-zero on failure.
 */
int usbh_ctrl_set_interface(usb_host_t *host, u8 itf_num, u8 alt_setting);

/**
 * @brief  Send `SET_FEATURE` standard request to device (remote wakeup feature,..)
 * @param[in] host: Host handler.
 * @param[in] value: Feature value.
 * @return 0 on success, non-zero on failure.
 */
int usbh_ctrl_set_feature(usb_host_t *host, u8 value);

/**
 * @brief  Send `CLEAR_FEATURE` standard request to device.
 * @param[in] host: Host Handle.
 * @param[in] ep_num: Endpoint number.
 * @return 0 on success, non-zero on failure.
 */
int usbh_ctrl_clear_feature(usb_host_t *host, u8 ep_num);

/**
 * @brief  Send control request to device.
 * @param[in] host: Host Handle.
 * @param[in] req: Control request.
 * @param[in] buf: Data buffer.
 * @return 0 on success, non-zero on failure.
 */
int usbh_ctrl_request(usb_host_t *host, usbh_setup_req_t *req, u8 *buf);

/**
 * @brief  Start one data transfer.
 * @param[in] host: Host Handle.
 * @param[in] pipe: Pipe struct handle.
 * @return 0 on success, non-zero on failure.
 */
int usbh_transfer_data(usb_host_t *host, usbh_pipe_t *pipe);

/**
 * @brief  Get the last transfer data size of specific pipe.
 * @param[in] host: Host Handle.
 * @param[in] pipe: Pipe struct handle.
 * @return None
 */
u32 usbh_get_last_transfer_size(usb_host_t *host, usbh_pipe_t *pipe);

/**
 * @brief  Start one transfer and handle result.
 * @param[in] host: Host Handle.
 * @param[in] pipe: Pipe struct handle.
 * @return 0 on success, non-zero on failure.
 */
int usbh_transfer_process(usb_host_t *host, usbh_pipe_t *pipe);

/**
 * @brief  Enable NAK mask for the pipe.
 * @param[in] host: Host Handle.
 * @param[in] pipe_num: Pipe number.
 * @return 0 on success, non-zero on failure.
 */
int usbh_enable_nak_interrupt(usb_host_t *host, u8 pipe_num);

/**
 * @brief  Check NAK time out.
 * @param[in] host: Host Handle.
 * @param[in] pipe_num: Pipe number.
 * @param[in] tick_cnt: Timout tick count.
 * @return 0 on success, non-zero on failure.
*/
int usbh_check_nak_timeout(usb_host_t *host, u8 pipe_num, u32 tick_cnt);

/**
 * @brief  Increase pipe busy count.
 * @param[in] host: Host Handle.
 * @param[in] pipe_num: Pipe number.
 * @param[in] step: Increase step.
 * @return 0 on success, non-zero on failure.
 */
int usbh_increase_busy_cnt(usb_host_t *host, u8 pipe_num, u8 step);

/**
 * @brief  Prepare for retransfer.
 * @param[in] host: Host Handle.
 * @param[in] pipe_num: Pipe number.
 * @return 0 on success, non-zero on failure.
 */
int usbh_prepare_retransfer(usb_host_t *host, u8 pipe_num);

/* Usbh CTS test operations. */
/**
 * @brief  USB Host enter suspend.
 * @return None
 */
void usbh_suspend(void);

/**
 * @brief  USB Host exit suspend.
 * @return None
 */
void usbh_resume(void);

/**
 * @brief  USB Host Port Test Control.
 * @param[in] mode: Test mode.
 * @return 0 on success, non-zero on failure.
 */
int usbh_select_test_mode(u8 mode);

#endif /* USBH_H */

