/*
 * Copyright (c) 2025 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_COMPOSITE_HID_H
#define USBH_COMPOSITE_HID_H

/* Includes ------------------------------------------------------------------*/
#include "usbh.h"
#include "usb_ch9.h"
#include "usb_ringbuf.h"
#include "usbh_composite_config.h"

/* Exported defines ----------------------------------------------------------*/

/** @addtogroup USB_Host_API USB Host API
 *  @{
 */
/** @addtogroup USB_Host_Constants USB Host Constants
 * @{
 */
/** @addtogroup Host_Composite_HID_Constants Host Composite HID Constants
 * @{
 */
#define USBH_COMPOSITE_HID_THREAD_PRIORITY     3U      /**< HID processing thread priority */
#define USBH_COMPOSITE_HID_MST_COUNT           10U     /**< Maximum support touch count (if applicable) */
#define USBH_COMPOSITE_HID_MSG_LENGTH          16U     /**< Message queue length */

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
#define USBH_HID_MAIN_ITEM_TAG_COLLECTION      0xA  /**< Collection Tag */
#define USBH_HID_MAIN_ITEM_TAG_END_COLLECTION  0xC  /**< End Collection Tag */

/* HID Global Item Tags */
#define USBH_HID_GLOBAL_ITEM_TAG_USAGE_PAGE    0x0  /**< Usage Page Tag */
#define USBH_HID_GLOBAL_ITEM_TAG_LOGICAL_MINIMUM 0x1 /**< Logical Minimum Tag */
#define USBH_HID_GLOBAL_ITEM_TAG_LOGICAL_MAXIMUM 0x2 /**< Logical Maximum Tag */
#define USBH_HID_GLOBAL_ITEM_TAG_REPORT_SIZE   0x7  /**< Report Size Tag */
#define USBH_HID_GLOBAL_ITEM_TAG_REPORT_ID     0x8  /**< Report ID Tag */
#define USBH_HID_GLOBAL_ITEM_TAG_REPORT_COUNT  0x9  /**< Report Count Tag */

/* HID Local Item Tags */
#define USBH_HID_LOCAL_ITEM_TAG_USAGE          0x0  /**< Usage Tag */

/* Usage Consumer Pages */
#define USBH_HID_UP_CONSUMER                   0x0C /**< Consumer Usage Page */

/* Consumer Page Volume Control Usages */
#define USBH_HID_CONSUMER_VOLUME_UP            0xE9 /**< Volume Increment */
#define USBH_HID_CONSUMER_VOLUME_DOWN          0xEA /**< Volume Decrement */
#define USBH_HID_CONSUMER_MUTE                 0xE2 /**< Mute */
#define USBH_HID_CONSUMER_PLAY_PAUSE           0xCD /**< Play/Pause */
#define USBH_HID_CONSUMER_STOP                 0xB7 /**< Stop */
/** @} End of Host_Composite_HID_Constants group*/
/** @} End of USB_Host_Constants group*/

/* Exported types ------------------------------------------------------------*/
/** @addtogroup USB_Host_Types USB Host Types
 * @{
 */
/** @addtogroup Host_Composite_HID_Types Host Composite HID Types
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
} usbh_composite_hid_get_report_desc_t;

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
} usbh_composite_hid_event_type_t;

/**
 * @brief HID Event Structure.
 */
typedef struct {
	usbh_composite_hid_event_type_t type; /**< Event type */
	u8 is_press;                          /**< 1 if pressed, 0 if released */
} usbh_composite_hid_event_t;

/**
 * @brief HID Report Item structure.
 * Used during parsing of the HID Report Descriptor.
 */
typedef struct {
	u8 type;                    /**< Item Type (Main, Global, Local) */
	u8 tag;                     /**< Item Tag */
	u8 size;                    /**< Item Size */
	u32 data;                   /**< Item Data */
} usbh_composite_hid_item_t;


/**
 * @brief HID Control Capabilities structure.
 * Stores supported features found during parsing (Volume, Media).
 */
typedef struct {
	u8 report_id_count;         /**< Number of report IDs found */

	/* Volume controls */
	struct {
		u8 report_id;           /**< Report ID for volume */
		u8 up_bit;              /**< Bit position/usage for Volume Up */
		u8 down_bit;            /**< Bit position/usage for Volume Down */
		u8 mute_bit;            /**< Bit position/usage for Mute */
		bool supported;         /**< Is volume control supported */
	} volume;

	/* Media controls */
	struct {
		u8 report_id;           /**< Report ID for media */
		u8 play_pause_bit;      /**< Bit position/usage for Play/Pause */
		u8 stop_bit;            /**< Bit position/usage for Stop */
		bool supported;         /**< Is media control supported */
	} media;
} usbh_composite_hid_ctrl_caps_t;

/**
 * @brief HID Parser State structure.
 * Maintains the context while parsing the HID Report Descriptor.
 */
typedef struct {
	u32 usage_page;             /**< Current Usage Page */
	int logical_min;            /**< Current Logical Minimum */
	int logical_max;            /**< Current Logical Maximum */
	u32 report_size;            /**< Current Report Size (bits) */
	u32 report_count;           /**< Current Report Count */
	u32 report_id;              /**< Current Report ID */

	int usage_stack_ptr;        /**< Stack pointer for Push/Pop items */
	int collection_depth;       /**< Current depth of Collection nesting */

	usbh_composite_hid_ctrl_caps_t *device_info; /**< Pointer to capabilities struct to populate */
} usbh_composite_hid_parse_state;

/**
 * @brief User Callback Structure for HID Composite Driver.
 */
typedef struct {
	int(* init)(void);          /**< Called on initialization */
	int(* deinit)(void);        /**< Called on de-initialization */
	int(* attach)(void);        /**< Called when device is attached */
	int(* detach)(void);        /**< Called when device is detached */
	int(* setup)(void);         /**< Called during setup phase */
	int(* report)(usbh_composite_hid_event_t *event); /**< Called when a HID event occurs */
} usbh_composite_hid_usr_cb_t;

typedef struct {
	usbh_composite_hid_ctrl_caps_t vol_caps; /**< Volume capabilities detected */
	usbh_composite_hid_event_t report_event;  /**< HID report event */
	usb_ringbuf_manager_t report_msg;         /**< Ring buffer for report data */
	rtos_task_t msg_parse_task;               /**< Task handle for parsing messages */
	usbh_dev_hid_desc_t hid_desc;             /**< HID Descriptor */
	usbh_ep_desc_t ep_desc;                   /**< Endpoint Descriptor */
	usbh_pipe_t pipe;                         /**< USB Pipe handle */
#if USBH_COMPOSITE_HID_UAC_DEBUG
	usbh_composite_hid_event_t last_event;    /**< Debug: Last event recorded */
	__IO u32 event_cnt;                       /**< Debug: Event counter */
#endif
	usbh_composite_host_t *driver;            /**< Parent composite driver handle */
	usbh_composite_hid_usr_cb_t *cb;          /**< User callback pointers */
	u8 *report_desc;                          /**< Raw Report Descriptor buffer */
	u8 *hid_ctrl_buf;                         /**< Buffer for control transfers */
	u8 report_desc_status;                    /**< Status of report descriptor retrieval */
	u8 itf_idx;                               /**< Interface Index */
	u8 itf_alt_idx;                           /**< Interface Alternate Setting Index */
	u8 next_xfer;                             /**< Flag to trigger the next transfer */
	__IO u8 parse_task_alive;                 /**< Flag to indicate parse task is alive */
	__IO u8 parse_task_exit;                  /**< Flag to indicate signal parse task is exit */
	__IO u8 hid_ctrl;                         /**< HID control state */
} usbh_composite_hid_t;
/** @} End of Host_Composite_HID_Types group*/
/** @} End of USB_Host_Types group*/

/* Exported variables --------------------------------------------------------*/
extern const usbh_class_driver_t usbh_composite_hid_driver;

/* Exported functions --------------------------------------------------------*/
/** @addtogroup USB_Host_Functions USB Host Functions
 * @{
 */
/** @addtogroup Host_Composite_HID_Functions Host Composite HID Functions
 * @{
 */
/**
 * @brief  Initialize the Composite HID class.
 * @param  chost: Pointer to the composite host structure.
 * @param  cb: Pointer to user callback structure.
 * @return 0 on success, non-zero on failure.
 */
int usbh_composite_hid_init(usbh_composite_host_t *chost, usbh_composite_hid_usr_cb_t *cb);

/**
 * @brief  De-Initialize the Composite HID class.
 * @return 0 on success, non-zero on failure.
 */
int usbh_composite_hid_deinit(void);

/**
 * @brief  Handle HID Report Descriptor retrieval and parsing.
 * @param  host: Pointer to the USB host structure.
 * @return 0 on success, non-zero on failure.
 */
int usbh_composite_hid_handle_report_desc(usb_host_t *host);
/** @} End of Host_Composite_HID_Functions group */
/** @} End of USB_Host_Functions group */
/** @} End of USB_Host_API group */
#endif // USBH_COMPOSITE_HID_H
