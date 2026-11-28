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
#include "usb_hid.h"
#include "usb_ringbuf.h"

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
/** @addtogroup Host_HID_Constants Host HID Constants
 * @{
 */

#define USBH_CLASS_HID                         USB_HID_CLASS_CODE   /**< USB HID Class Code */
#define USBH_HID_SUBCLASS_BOOT                 0x01U   /**< Boot Interface Subclass, Ref HID 1.11 4.2 */

#define USBH_HID_DESC                          0x21U   /**< HID Descriptor Type */
#define USBH_HID_REPORT_TYPE                   0x22U   /**< HID Report Descriptor Type */

/* HID class requests, Ref HID 1.11 7.2 */
#define USBH_HID_GET_REPORT                    0x01U   /**< GET_REPORT */
#define USBH_HID_GET_IDLE                      0x02U   /**< GET_IDLE */
#define USBH_HID_GET_PROTOCOL                  0x03U   /**< GET_PROTOCOL */
#define USBH_HID_SET_REPORT                    0x09U   /**< SET_REPORT */
#define USBH_HID_SET_IDLE                      0x0AU   /**< SET_IDLE */
#define USBH_HID_SET_PROTOCOL                  0x0BU   /**< SET_PROTOCOL */

/* Report types of GET_REPORT / SET_REPORT, Ref HID 1.11 7.2.1 */
#define USBH_HID_REPORT_INPUT                  0x01U   /**< Input report */
#define USBH_HID_REPORT_OUTPUT                 0x02U   /**< Output report */
#define USBH_HID_REPORT_FEATURE                0x03U   /**< Feature report */

/* Protocols of SET_PROTOCOL, Ref HID 1.11 7.2.6 */
#define USBH_HID_PROTOCOL_BOOT                 0x00U   /**< Boot protocol */
#define USBH_HID_PROTOCOL_REPORT               0x01U   /**< Report protocol, the default after SET_CONFIGURATION */

/* HID Item Types, Ref HID 1.11 6.2.2.2 */
#define USBH_HID_ITEM_TYPE_MAIN                0U      /**< Main Item Type */
#define USBH_HID_ITEM_TYPE_GLOBAL              1U      /**< Global Item Type */
#define USBH_HID_ITEM_TYPE_LOCAL               2U      /**< Local Item Type */
/* HID Main Item Tags */
#define USBH_HID_MAIN_ITEM_TAG_INPUT           0x8U    /**< Input Tag */
#define USBH_HID_MAIN_ITEM_TAG_OUTPUT          0x9U    /**< Output Tag */
#define USBH_HID_MAIN_ITEM_TAG_COLLECTION      0xAU    /**< Collection Tag */
#define USBH_HID_MAIN_ITEM_TAG_FEATURE         0xBU    /**< Feature Tag */
#define USBH_HID_MAIN_ITEM_TAG_END_COLLECTION  0xCU    /**< End Collection Tag */

/* HID Main Item data bits, Ref HID 1.11 6.2.2.5 */
#define USBH_HID_ITEM_DATA_CONSTANT            0x1U    /**< 0: Data, 1: Constant(padding) */
#define USBH_HID_ITEM_DATA_VARIABLE            0x2U    /**< 0: Array, 1: Variable */
#define USBH_HID_ITEM_DATA_RELATIVE            0x4U    /**< 0: Absolute, 1: Relative */

/* HID Global Item Tags */
#define USBH_HID_GLOBAL_ITEM_TAG_USAGE_PAGE    0x0U    /**< Usage Page Tag */
#define USBH_HID_GLOBAL_ITEM_TAG_LOGICAL_MINIMUM 0x1U  /**< Logical Minimum Tag */
#define USBH_HID_GLOBAL_ITEM_TAG_LOGICAL_MAXIMUM 0x2U  /**< Logical Maximum Tag */
#define USBH_HID_GLOBAL_ITEM_TAG_REPORT_SIZE   0x7U    /**< Report Size Tag */
#define USBH_HID_GLOBAL_ITEM_TAG_REPORT_ID     0x8U    /**< Report ID Tag */
#define USBH_HID_GLOBAL_ITEM_TAG_REPORT_COUNT  0x9U    /**< Report Count Tag */

/* HID Local Item Tags */
#define USBH_HID_LOCAL_ITEM_TAG_USAGE          0x0U    /**< Usage Tag */
#define USBH_HID_LOCAL_ITEM_TAG_USAGE_MIN      0x1U    /**< Usage Minimum Tag */
#define USBH_HID_LOCAL_ITEM_TAG_USAGE_MAX      0x2U    /**< Usage Maximum Tag */

/* Usage Pages, Ref HID Usage Tables 1.4 */
#define USBH_HID_UP_GENERIC_DESKTOP            0x01U   /**< Generic Desktop Page */
#define USBH_HID_UP_KEYBOARD                   0x07U   /**< Keyboard/Keypad Page */
#define USBH_HID_UP_LED                        0x08U   /**< LED Page */
#define USBH_HID_UP_BUTTON                     0x09U   /**< Button Page */
#define USBH_HID_UP_CONSUMER                   0x0CU   /**< Consumer Page */
#define USBH_HID_UP_VENDOR_MIN                 0xFF00U /**< First Vendor Defined Page */

/* Limits */
#define USBH_HID_MAX_ITF_NUM                   2U      /**< HID interfaces served per device, e.g. a keyboard plus its media keys */
#define USBH_HID_REPORT_MAX_LEN                64U     /**< Largest INTR IN report delivered, Ref USB 2.0 5.7.3 FS/LS maximum */
#define USBH_HID_DESC_LEN                      9U      /**< HID descriptor length with one class descriptor, Ref HID 1.11 6.2.1 */
#define USBH_HID_HALT_RETRY_MAX_CNT            3U      /**< CLEAR_FEATURE(ENDPOINT_HALT) attempts before the device is re-enumerated */
#define USBH_HID_CTRL_TIMEOUT_MS               6000U   /**< Bound of a class request issued by the application, beyond the core control timeout */

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
 * @brief One HID interface of the attached device, as handed to the application.
 */
typedef struct {
	const u8 *report_desc;      /**< Report descriptor, NULL if the device refused it; valid until detach */
	u16 report_desc_len;        /**< Length of report_desc in bytes */
	u8 itf_num;                 /**< bInterfaceNumber */
	u8 subclass;                /**< bInterfaceSubClass, USBH_HID_SUBCLASS_BOOT for a boot device */
	u8 protocol;                /**< bInterfaceProtocol, USB_HID_PROTOCOL_KEYBOARD / USB_HID_PROTOCOL_MOUSE for a boot device */
	u8 has_out_ep;              /**< 1 if the interface has an Interrupt OUT endpoint */
} usbh_hid_itf_info_t;

/**
 * @brief Location of one field in a report, found by usbh_hid_find_field().
 * @note  Ref HID 1.11 6.2.2.5/6.2.2.7. A Variable field holds one value per usage: the value of the
 *        requested usage is value usage_idx. An Array field holds report_count usage codes: the
 *        requested usage is active while one of them reads logical_min + usage_idx.
 */
typedef struct {
	s32 logical_min;            /**< Logical Minimum */
	s32 logical_max;            /**< Logical Maximum */
	u16 bit_offset;             /**< First bit of the field, counted after the Report ID byte */
	u16 report_size;            /**< Bits per value */
	u16 report_count;           /**< Values in the field */
	u16 usage_idx;              /**< Index of the requested usage among the usages the field declares */
	u8 report_id;               /**< Report ID of the report holding the field, 0 if the device uses none */
	u8 flags;                   /**< Main item data, USBH_HID_ITEM_DATA_xxx */
} usbh_hid_field_t;

/**
 * @brief User Callback Structure for HID Host Driver.
 * @note  attach, detach and setup run in the host task: they shall not block nor call the blocking
 *        class requests. report runs in the class report thread; it shall not call usbh_hid_deinit().
 */
typedef struct {
	int (*init)(void);          /**< Called on initialization */
	void (*deinit)(void);       /**< Called on de-initialization */
	void (*attached)(void);     /**< Called when a device with HID interfaces is attached */
	void (*detached)(void);     /**< Called when the device is detached */
	/** Called once per HID interface before its reports flow, idx being its index for the class API. Returns the
	    protocol to run: USBH_HID_PROTOCOL_REPORT keeps the default, USBH_HID_PROTOCOL_BOOT makes the class send
	    SET_PROTOCOL(Boot) to a boot interface, Ref HID 1.11 7.2.6. NULL keeps the default. */
	u8(*setup)(u8 idx, const usbh_hid_itf_info_t *info);
	/** Called for every INTR IN report: buf starts with the Report ID if the device uses one, valid during the call only */
	void (*report)(u8 idx, const u8 *buf, u32 len);
} usbh_hid_cb_t;

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
int usbh_hid_init(const usbh_hid_cb_t *cb);

/**
 * @brief  De-Initialize the HID host class.
 * @note   Task context only, not from a class callback.
 */
void usbh_hid_deinit(void);

/**
 * @brief  Send an output report to the device via the Interrupt OUT pipe of an interface.
 * @param  idx: Interface index as passed to usbh_hid_cb_t::setup.
 * @param  buf: Report, Report ID first if the device uses one. The controller reads it by DMA, so it
 *         must start on a cache line boundary (USB_DMA_ALIGNED, or from the heap) and stay unchanged
 *         until the transfer completes.
 * @param  len: Length of the report in bytes.
 * @return RTK_SUCCESS, -RTK_ERR_BADARG if the interface has no OUT pipe or buf is NULL or not cache
 *         line aligned, -RTK_ERR_BUSY if a transfer is already in progress.
 */
int usbh_hid_send_report(u8 idx, u8 *buf, u32 len);

/**
 * @brief  SET_PROTOCOL, Ref HID 1.11 7.2.6. Only a boot interface accepts it.
 * @param  idx: Interface index.
 * @param  protocol: USBH_HID_PROTOCOL_BOOT or USBH_HID_PROTOCOL_REPORT.
 * @return RTK_SUCCESS, -RTK_ERR_BADARG if refused by the device, or the transfer error.
 * @note   Task context only, blocks up to USBH_HID_CTRL_TIMEOUT_MS. Not from a class callback.
 */
int usbh_hid_set_protocol(u8 idx, u8 protocol);

/**
 * @brief  SET_REPORT over EP0, Ref HID 1.11 7.2.2, e.g. the LED report of a keyboard without OUT endpoint.
 * @param  idx: Interface index.
 * @param  type: USBH_HID_REPORT_OUTPUT or USBH_HID_REPORT_FEATURE.
 * @param  id: Report ID, 0 if the device uses none.
 * @param  buf: Report, Report ID first if id is not 0.
 * @param  len: Length of the report in bytes, at most USBH_HID_REPORT_MAX_LEN.
 * @return RTK_SUCCESS, -RTK_ERR_BADARG if refused by the device, or the transfer error.
 * @note   Task context only, blocks up to USBH_HID_CTRL_TIMEOUT_MS. Not from a class callback.
 */
int usbh_hid_set_report(u8 idx, u8 type, u8 id, const u8 *buf, u32 len);

/**
 * @brief  GET_REPORT over EP0, Ref HID 1.11 7.2.1.
 * @param  idx: Interface index.
 * @param  type: USBH_HID_REPORT_INPUT or USBH_HID_REPORT_FEATURE.
 * @param  id: Report ID, 0 if the device uses none.
 * @param  buf: Destination, Report ID first if id is not 0.
 * @param  len: In: capacity of buf, at most USBH_HID_REPORT_MAX_LEN. Out: bytes received.
 * @return RTK_SUCCESS, -RTK_ERR_BADARG if refused by the device, or the transfer error.
 * @note   Task context only, blocks up to USBH_HID_CTRL_TIMEOUT_MS. Not from a class callback.
 */
int usbh_hid_get_report(u8 idx, u8 type, u8 id, u8 *buf, u32 *len);

/**
 * @brief  Find the Input or Output field declaring a usage in a report descriptor.
 * @param  desc: Report descriptor, see usbh_hid_itf_info_t.
 * @param  len: Length of desc in bytes.
 * @param  main_tag: USBH_HID_MAIN_ITEM_TAG_INPUT or USBH_HID_MAIN_ITEM_TAG_OUTPUT.
 * @param  usage_page: Usage Page of the usage.
 * @param  usage: Usage ID.
 * @param  field: Filled with the first field declaring the usage.
 * @return RTK_SUCCESS if found, -RTK_ERR_BADARG otherwise.
 */
int usbh_hid_find_field(const u8 *desc, u32 len, u8 main_tag, u16 usage_page, u16 usage, usbh_hid_field_t *field);

/**
 * @brief  Read one value of a field from a report.
 * @param  report: Report as passed to usbh_hid_cb_t::report, Report ID first if the device uses one.
 * @param  len: Length of the report in bytes.
 * @param  field: Field found by usbh_hid_find_field().
 * @param  idx: Value index in the field, 0..report_count-1.
 * @param  value: Filled with the value, sign extended if logical_min is negative.
 * @return RTK_SUCCESS, -RTK_ERR_BADARG if the report has another Report ID or is too short.
 */
int usbh_hid_get_field_value(const u8 *report, u32 len, const usbh_hid_field_t *field, u16 idx, s32 *value);

/**
 * @brief  Whether the usage a field was found for is active in a report, e.g. a key held.
 * @param  report: Report as passed to usbh_hid_cb_t::report.
 * @param  len: Length of the report in bytes.
 * @param  field: Field found by usbh_hid_find_field() for the usage.
 * @return true if active: a non-zero Variable value, or an Array code naming the usage. false if not, or if the
 *         report has another Report ID.
 */
bool usbh_hid_is_usage_active(const u8 *report, u32 len, const usbh_hid_field_t *field);

/** @} End of Host_HID_Functions group */
/** @} End of USB_Host_Functions group */

/** @} End of USB_Host_API group */

#ifdef __cplusplus
}
#endif

#endif /* USBH_HID_H */
