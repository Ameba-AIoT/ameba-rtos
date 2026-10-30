/*
 * Copyright (c) 2025 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_HID_H
#define USBH_HID_H

/* Includes ------------------------------------------------------------------*/

#include "usbh.h"
#include "usb_ch9.h"
#include "usb_ringbuf.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup USB_Host_API USB Host API
 *  @{
 */

/* Exported defines ----------------------------------------------------------*/

/* HID debug switch. Set to 1 for debug. */
#define USBH_HID_DEBUG                         0

/** @addtogroup USB_Host_Constants USB Host Constants
 * @{
 */
/** @addtogroup Host_HID_Constants Host HID Constants
 * @{
 */

/* Audio Class Codes (Note: HID is 0x03, defined here for reference) */
#define USBH_CLASS_HID                         0x03U   /**< USB HID Class Code */

/* Descriptor Types */
#define USBH_HID_DESC                          0x21    /**< HID Descriptor Type */
#define USBH_HID_REPORT_TYPE                   0x22    /**< HID Report Descriptor Type */

/* HID Item Types */
#define USBH_HID_ITEM_TYPE_MAIN                0U      /**< Main Item Type */
#define USBH_HID_ITEM_TYPE_GLOBAL              1U      /**< Global Item Type */
#define USBH_HID_ITEM_TYPE_LOCAL               2U      /**< Local Item Type */
/* HID Main Item Tags */
#define USBH_HID_MAIN_ITEM_TAG_INPUT           0x8  /**< Input Tag */
#define USBH_HID_MAIN_ITEM_TAG_OUTPUT          0x9  /**< Output Tag */
#define USBH_HID_MAIN_ITEM_TAG_COLLECTION      0xA  /**< Collection Tag */
#define USBH_HID_MAIN_ITEM_TAG_FEATURE         0xB  /**< Feature Tag */
#define USBH_HID_MAIN_ITEM_TAG_END_COLLECTION  0xC  /**< End Collection Tag */

/* HID Main Item data bits, Ref HID 1.11 6.2.2.5 */
#define USBH_HID_ITEM_DATA_CONSTANT            0x1U /**< 0: Data, 1: Constant(padding) */
#define USBH_HID_ITEM_DATA_VARIABLE            0x2U /**< 0: Array, 1: Variable */

/* HID Global Item Tags */
#define USBH_HID_GLOBAL_ITEM_TAG_USAGE_PAGE    0x0  /**< Usage Page Tag */
#define USBH_HID_GLOBAL_ITEM_TAG_LOGICAL_MINIMUM 0x1 /**< Logical Minimum Tag */
#define USBH_HID_GLOBAL_ITEM_TAG_LOGICAL_MAXIMUM 0x2 /**< Logical Maximum Tag */
#define USBH_HID_GLOBAL_ITEM_TAG_REPORT_SIZE   0x7  /**< Report Size Tag */
#define USBH_HID_GLOBAL_ITEM_TAG_REPORT_ID     0x8  /**< Report ID Tag */
#define USBH_HID_GLOBAL_ITEM_TAG_REPORT_COUNT  0x9  /**< Report Count Tag */

/* HID Local Item Tags */
#define USBH_HID_LOCAL_ITEM_TAG_USAGE          0x0  /**< Usage Tag */
#define USBH_HID_LOCAL_ITEM_TAG_USAGE_MIN      0x1  /**< Usage Minimum Tag */
#define USBH_HID_LOCAL_ITEM_TAG_USAGE_MAX      0x2  /**< Usage Maximum Tag */

/* Usage Consumer Pages */
#define USBH_HID_UP_CONSUMER                   0x0C /**< Consumer Usage Page */

/* Consumer Page Volume Control Usages */
#define USBH_HID_CONSUMER_VOLUME_UP            0xE9 /**< Volume Increment */
#define USBH_HID_CONSUMER_VOLUME_DOWN          0xEA /**< Volume Decrement */
#define USBH_HID_CONSUMER_MUTE                 0xE2 /**< Mute */
#define USBH_HID_CONSUMER_PLAY_PAUSE           0xCD /**< Play/Pause */
#define USBH_HID_CONSUMER_STOP                 0xB7 /**< Stop */

/* Report descriptor parser limits */
#define USBH_HID_BIT_NONE                      0xFFFFU /**< Control is not declared in the report descriptor */
#define USBH_HID_TRACK_VOLUME_UP               0U   /**< Tracked usage slot: Volume Up */
#define USBH_HID_TRACK_VOLUME_DOWN             1U   /**< Tracked usage slot: Volume Down */
#define USBH_HID_TRACK_MUTE                    2U   /**< Tracked usage slot: Mute */
#define USBH_HID_TRACK_PLAY_PAUSE              3U   /**< Tracked usage slot: Play/Pause */
#define USBH_HID_TRACK_STOP                    4U   /**< Tracked usage slot: Stop */
#define USBH_HID_TRACK_USAGE_CNT               5U   /**< Tracked usages: volume up/down/mute, play-pause, stop */
#define USBH_HID_ARRAY_ELEM_CNT                4U   /**< Max usage codes decoded from one Array field */
#define USBH_HID_REPORT_MAX_LEN                64U  /**< Largest FS/LS INTR IN report kept per ring node (Ref USB 2.0 5.7.3) */
#define USBH_HID_DESC_LEN                      9U   /**< HID descriptor length with one class descriptor, Ref HID 1.11 6.2.1 */
#define USBH_HID_HALT_RETRY_MAX_CNT            3U   /**< CLEAR_FEATURE(ENDPOINT_HALT) attempts before the device is re-enumerated */

/** @} End of Host_HID_Constants group */
/** @} End of USB_Host_Constants group */

/* Exported types ------------------------------------------------------------*/

/** @addtogroup USB_Host_Types USB Host Types
 * @{
 */
/** @addtogroup Host_HID_Types Host HID Types
 * @{
 */

/**
 * @brief USB Standard HID Descriptor structure.
 */
typedef struct {
	u8 bLength;                 /**< Size of this descriptor in bytes, fixed is 9 */
	u8 bDescriptorType;         /**< Descriptor Type (USBH_HID_DESC) */
	u16 bcdHID;                 /**< HID Class Specification release number (BCD: 0x0100) */
	u8 bCountryCode;            /**< Hardware target country */
	u8 bNumDescriptors;         /**< Number of class descriptors to follow, usually is 1 */
	u8 bReportDescriptorType;   /**< Report descriptor type (USBH_HID_REPORT_DESC) */
	u16 wDescriptorLength;      /**< Total length of Report descriptor */
} __PACKED usbh_dev_hid_desc_t;

/**
 * @brief HID Report Descriptor State Machine states.
 */
typedef enum {
	USBH_HID_REPORT_IDLE,       /**< Idle state */
	USBH_HID_REPORT_SET_ALT,    /**< Setting Interface Alternate Setting */
	USBH_HID_REPORT_GET_DESC,   /**< Getting Report Descriptor */
	USBH_HID_REPORT_MAX,        /**< Max state guard */
} usbh_hid_get_report_desc_t;

/**
 * @brief HID Event Types.
 * Abstracted events generated from HID reports.
 */
typedef enum {
	VOLUME_EVENT_NONE,                  /**< No event */
	VOLUME_EVENT_CONSUMER_UP,           /**< Volume Up pressed */
	VOLUME_EVENT_CONSUMER_DOWN,         /**< Volume Down pressed */
	VOLUME_EVENT_CONSUMER_MUTE,         /**< Mute pressed */
	VOLUME_EVENT_CONSUMER_PLAY_PAUSE,   /**< Play/Pause pressed */
	VOLUME_EVENT_CONSUMER_STOP,         /**< Stop pressed */
} usbh_hid_event_type_t;

/**
 * @brief HID Event Structure.
 * @note  Reported once per INTR IN report, also for a report repeating the previous one. Only presses are
 *        decoded: a tracked control set in the report gives its type with is_press 1, a report with no tracked
 *        control set, a key release included, gives VOLUME_EVENT_NONE with is_press 0.
 */
typedef struct {
	usbh_hid_event_type_t type; /**< Event type, VOLUME_EVENT_NONE when no tracked control is set */
	u8 is_press;                          /**< 1 if a tracked control is pressed, 0 for VOLUME_EVENT_NONE */
} usbh_hid_event_t;

/**
 * @brief HID Report Item structure.
 * Used during parsing of the HID Report Descriptor.
 */
typedef struct {
	u32 data;                   /**< Item Data */
	u8 type;                    /**< Item Type (Main, Global, Local) */
	u8 tag;                     /**< Item Tag */
	u8 size;                    /**< Item Size */
} usbh_hid_item_t;

/**
 * @brief HID Control Capabilities structure.
 * Stores supported features found during parsing (Volume, Media).
 * @note  A control declared by a Variable field of Report Size 1 is located by
 *        its absolute bit position (bitmap form). A control declared by an Array
 *        field is not a bit at all: the field carries usage codes, so it is
 *        described by consumer_array instead. Ref HID 1.11 6.2.2.5.
 */
typedef struct {
	/* Volume controls, bitmap form */
	struct {
		u16 up_bit;             /**< Absolute bit of Volume Up, USBH_HID_BIT_NONE if not declared */
		u16 down_bit;           /**< Absolute bit of Volume Down, USBH_HID_BIT_NONE if not declared */
		u16 mute_bit;           /**< Absolute bit of Mute, USBH_HID_BIT_NONE if not declared */
		u8 report_id;           /**< Report ID for volume */
		bool supported;         /**< Is volume control declared as a bitmap */
	} volume;

	/* Media controls, bitmap form */
	struct {
		u16 play_pause_bit;     /**< Absolute bit of Play/Pause, USBH_HID_BIT_NONE if not declared */
		u16 stop_bit;           /**< Absolute bit of Stop, USBH_HID_BIT_NONE if not declared */
		u8 report_id;           /**< Report ID for media */
		bool supported;         /**< Is media control declared as a bitmap */
	} media;

	/* Consumer controls declared by an Array field carrying usage codes */
	struct {
		u16 bit_offset;         /**< Absolute bit of the first array element */
		u8 report_id;           /**< Report ID holding the array field */
		u8 elem_bits;           /**< Bits per element, i.e. Report Size */
		u8 elem_cnt;            /**< Elements to decode, clamped to USBH_HID_ARRAY_ELEM_CNT */
		bool supported;         /**< Is a consumer array field present */
	} consumer_array;

	u8 report_id_count;         /**< Number of report IDs found */
	u8 report_id_zero;          /**< The descriptor declares the reserved Report ID 0, so its reports can not be framed */
} usbh_hid_ctrl_caps_t;

/**
 * @brief HID Parser State structure.
 * Maintains the context while parsing the HID Report Descriptor.
 * @note  Input, Output and Feature fields live in separate reports, so each one
 *        needs its own bit cursor. Ref HID 1.11 5.6 / 8.
 */
typedef struct {
	usbh_hid_ctrl_caps_t *device_info; /**< Pointer to capabilities struct to populate */
	int logical_min;            /**< Current Logical Minimum */
	int logical_max;            /**< Current Logical Maximum */
	u32 in_bit_offset;          /**< Bits consumed by previous Input fields of the current report */
	u32 out_bit_offset;         /**< Bits consumed by previous Output fields of the current report */
	u32 feat_bit_offset;        /**< Bits consumed by previous Feature fields of the current report */
	u32 usage_cnt;              /**< Usages declared so far for the pending Main item */
	u32 usage_min;              /**< Pending Usage Minimum */
	u16 track_ord[USBH_HID_TRACK_USAGE_CNT]; /**< Ordinal of each tracked usage in the pending field */
	u16 usage_page;             /**< Current Usage Page */
	u16 report_size;            /**< Current Report Size (bits) */
	u16 report_count;           /**< Current Report Count */
	u8 track_mask;              /**< Bitmask of tracked usages declared in the pending field */
	u8 report_id;               /**< Current Report ID */
	u8 usage_min_valid;         /**< A Usage Minimum is pending a Usage Maximum */
	u8 collection_depth;        /**< Current depth of Collection nesting */
} usbh_hid_parse_state;

/**
 * @brief User Callback Structure for HID Host Driver.
 */
typedef struct {
	int (*init)(void);          /**< Called on initialization */
	void (*deinit)(void);      /**< Called on de-initialization */
	void (*attached)(void);    /**< Called when device is attached */
	void (*detached)(void);    /**< Called when device is detached */
	void (*setup)(void);       /**< Called during setup phase */
	void (*report)(usbh_hid_event_t *event); /**< Called for every INTR IN report received, see usbh_hid_event_t */
} usbh_hid_usr_cb_t;

typedef struct {
	usb_host_t *host;                       /**< USB host handle (stored at attach) */
	const usbh_hid_usr_cb_t *cb;              /**< User callback pointers */

	/* Class and interface state */
	__IO u8 hid_ctrl;                         /**< HID control state */
	u8 report_desc_status;                  /**< Status of report descriptor retrieval */
	u8 itf_idx;                               /**< Interface Index */
	u8 itf_alt_idx;                           /**< Interface Alternate Setting Index */
	u8 alt_setting_count;                     /**< Number of alternate settings on this HID interface */
	u8 halt_pending;                          /**< INTR IN STALLed, CLEAR_FEATURE(ENDPOINT_HALT) to be sent */
	u8 halt_retry_cnt;                        /**< Failed CLEAR_FEATURE(ENDPOINT_HALT) attempts */

	/* Descriptors */
	usbh_dev_hid_desc_t hid_desc;             /**< HID Descriptor */
	usbh_ep_desc_t ep_desc_in;                /**< Interrupt IN Endpoint Descriptor */
	usbh_ep_desc_t ep_desc_out;               /**< Interrupt OUT Endpoint Descriptor, optional */
	u8 *report_desc;                          /**< Alias pointer into hid_ctrl_buf (zero-copy). Valid only until setup() ends:
	                                               hid_ctrl_buf then becomes the INTR IN buffer and is overwritten by reports. */
	usbh_hid_ctrl_caps_t vol_caps;            /**< Volume capabilities detected; written with hid_ctrl under the USB
	                                               critical section, read by the parse thread through caps_snap */

	/* Interrupt pipes */
	usbh_pipe_t pipe_in;                      /**< Interrupt IN pipe handle */
	usbh_pipe_t pipe_out;                     /**< Interrupt OUT pipe handle (optional) */
	u8 *hid_ctrl_buf;                         /**< General-purpose control transfer buffer (512 B); backs report_desc during
	                                               setup(), then serves as the INTR IN transfer buffer. */

	/* Report parse thread */
	rtos_task_t msg_parse_task;               /**< Task handle for parsing messages */
	usb_os_sema_t report_sema;                /**< Wakes the parse thread when a report is queued */
	usb_ringbuf_manager_t report_msg;         /**< Ring buffer for report data */
	usbh_hid_event_t report_event;            /**< HID report event */
	__IO u8 parse_task_alive;                 /**< Flag to indicate parse task is alive */
	__IO u8 parse_task_exit;                  /**< Flag to indicate signal parse task is exit */
	u8 report_buf[USBH_HID_REPORT_MAX_LEN];   /**< Parse thread copy of one report, kept off its 768 B stack */
	usbh_hid_ctrl_caps_t caps_snap;           /**< Parse thread copy of vol_caps for one report, taken under the lock */
	usbh_hid_ctrl_caps_t caps_build;          /**< Main task scratch the report descriptor is parsed into before publish */

#if USBH_HID_DEBUG
	__IO u32 event_cnt;                       /**< Debug: Event counter */
	usbh_hid_event_t last_event;              /**< Debug: Last event recorded */
#endif
} usbh_hid_t;

/** @} End of Host_HID_Types group */
/** @} End of USB_Host_Types group */

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @addtogroup USB_Host_Functions USB Host Functions
 * @{
 */
/** @addtogroup Host_HID_Functions Host HID Functions
 * @{
 */

/**
 * @brief  Initialize the HID host class.
 * @param  cb: Pointer to user callback structure.
 * @return 0 on success, non-zero on failure.
 */
int usbh_hid_init(const usbh_hid_usr_cb_t *cb);

/**
 * @brief  De-Initialize the HID host class.
 */
void usbh_hid_deinit(void);

/**
 * @brief  Send an output report to the HID device via the Interrupt OUT pipe.
 * @param  buf: Pointer to the report data buffer. The controller reads it by DMA, so it must
 *         start on a cache line boundary (declare it with USB_DMA_ALIGNED, or take it from
 *         the heap) and stay valid and unchanged until the transfer completes.
 * @param  len: Length of the report data in bytes.
 * @return 0 (RTK_SUCCESS) on success, -RTK_ERR_BADARG if no OUT pipe is available or buf is NULL or
 *         not cache line aligned, -RTK_ERR_BUSY if a transfer is already in progress.
 * @note   Only available when the HID interface exposes an Interrupt OUT endpoint.
 */
int usbh_hid_send_report(u8 *buf, u32 len);

/** @} End of Host_HID_Functions group */
/** @} End of USB_Host_Functions group */

/** @} End of USB_Host_API group */

#ifdef __cplusplus
}
#endif

#endif /* USBH_HID_H */
