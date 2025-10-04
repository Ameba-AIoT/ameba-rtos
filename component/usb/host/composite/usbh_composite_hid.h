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
#include "usbh_composite_config.h"

/* Exported defines ----------------------------------------------------------*/
#define USBH_COMPOSITE_HID_THREAD_PRIORITY         3U
#define USBH_COMPOSITE_HID_MST_COUNT               10U
#define USBH_COMPOSITE_HID_MSG_LENGTH              16U

/* Audio Class Codes */
#define USBH_CLASS_HID               0x03U

#define USBH_HID_DESC                0x21
#define USBH_HID_REPORT_TYPE         0x22

// HID Type
#define USBH_HID_ITEM_TYPE_MAIN          0U
#define USBH_HID_ITEM_TYPE_GLOBAL        1U
#define USBH_HID_ITEM_TYPE_LOCAL         2U

#define USBH_HID_MAIN_ITEM_TAG_INPUT               0x8
#define USBH_HID_MAIN_ITEM_TAG_COLLECTION          0xA
#define USBH_HID_MAIN_ITEM_TAG_END_COLLECTION      0xC

#define USBH_HID_GLOBAL_ITEM_TAG_USAGE_PAGE        0x0
#define USBH_HID_GLOBAL_ITEM_TAG_LOGICAL_MINIMUM   0x1
#define USBH_HID_GLOBAL_ITEM_TAG_LOGICAL_MAXIMUM   0x2
#define USBH_HID_GLOBAL_ITEM_TAG_REPORT_SIZE       0x7
#define USBH_HID_GLOBAL_ITEM_TAG_REPORT_ID         0x8
#define USBH_HID_GLOBAL_ITEM_TAG_REPORT_COUNT      0x9

#define USBH_HID_LOCAL_ITEM_TAG_USAGE              0x0

// Usage consumter Pages
#define USBH_HID_UP_CONSUMER                       0x0C

//Consumer Page Volume Control
#define USBH_HID_CONSUMER_VOLUME_UP                0xE9
#define USBH_HID_CONSUMER_VOLUME_DOWN              0xEA
#define USBH_HID_CONSUMER_MUTE                     0xE2
#define USBH_HID_CONSUMER_PLAY_PAUSE               0xCD
#define USBH_HID_CONSUMER_STOP                     0xB7

/* Exported types ------------------------------------------------------------*/
typedef struct {
	u8 *buf;
	__IO u16 buf_len;     /* buf valid len */
} usb_ringbuf_t;

typedef struct {
	usb_ringbuf_t *list_node;
	u8 *buf;

	__IO u32 head;
	__IO u32 tail;

	u16 item_cnt;
	u16 item_size;
} usb_ringbuf_manager_t;

/* USB HID descriptor */
typedef struct {
	u8  bLength;                  /* 9 */
	u8  bDescriptorType;          /* USBH_HID_DESC */
	u16 bcdHID;                   /* 0x0100 */
	u8  bCountryCode;             /* includes Unit and Terminal desc. */
	u8  bNumDescriptors;          /* sub descriptor count,usually is 1 */
	u8  bReportDescriptorType;    /* report type, USBH_HID_REPORT_DESC */
	u16 wDescriptorLength;        /* descriptor length */
} __attribute__((packed)) usbh_dev_hid_desc_t ;

typedef enum {
	USBH_HID_REPORT_IDLE,
	USBH_HID_REPORT_SET_ALT,
	USBH_HID_REPORT_GET_DESC,
	USBH_HID_REPORT_MAX,
} usbh_composite_hid_get_report_desc_t;

typedef enum {
	VOLUME_EVENT_NONE,

	//consumer volume ctrl
	VOLUME_EVENT_CONSUMER_UP,
	VOLUME_EVENT_CONSUMER_DOWN,
	VOLUME_EVENT_CONSUMER_MUTE,
	VOLUME_EVENT_CONSUMER_PLAY_PAUSE,
	VOLUME_EVENT_CONSUMER_STOP,
} usbh_composite_hid_event_type_t;

typedef struct {
	usbh_composite_hid_event_type_t type;
	u8 is_press;
} usbh_composite_hid_event_t;

// HID Item
typedef struct {
	u8 type;
	u8 tag;
	u8 size;
	u32 data;
} usbh_composite_hid_item_t;

typedef struct {
	u8 report_id_count;

	//volume
	struct {
		u8 report_id;
		u8 up_bit;
		u8 down_bit;
		u8 mute_bit;
		bool supported;
	} volume;

	// media
	struct {
		u8 report_id;
		u8 play_pause_bit;
		u8 stop_bit;
		bool supported;
	} media;
} usbh_composite_hid_ctrl_caps_t;

typedef struct {
	u32 usage_page;
	int logical_min;
	int logical_max;
	u32 report_size;
	u32 report_count;
	u32 report_id;

	// current reportId usage index
	int usage_stack_ptr;

	// collecttion depth
	int collection_depth;

	//report info
	usbh_composite_hid_ctrl_caps_t *device_info;
} usbh_composite_hid_parse_state;

typedef struct {
	int(* init)(void);
	int(* deinit)(void);
	int(* attach)(void);
	int(* detach)(void);
	int(* setup)(void);
	int(* report)(usbh_composite_hid_event_t *event);
} usbh_composite_hid_usr_cb_t;

/* States for transfer */
typedef enum {
	USBH_HID_XFER_IDLE = 0U,
	USBH_HID_XFER,
	USBH_HID_XFER_BUSY,
	USBH_HID_XFER_MAX,
} usbh_hid_ep_xfer_state_t;

typedef struct {
	u8 *buf;
	u32 len;
	u32 interval;
	__IO u32 tick;
	u16 packet_size;
	u8 pipe;
	u8 ep_addr;
	__IO usbh_hid_ep_xfer_state_t xfer_state;
} usbh_hid_ep_cfg_t;

typedef struct {
	usbh_composite_hid_ctrl_caps_t vol_caps;
	usb_ringbuf_manager_t report_msg;
	usbh_dev_hid_desc_t hid_desc;
	usbh_hid_ep_cfg_t ep_info;
	usbh_composite_hid_event_t report_event;
	u8 *report_desc;
	u8 *hid_ctrl_buf;

	usbh_composite_host_t *driver;

#if USBH_COMPOSITE_HID_UAC_DEBUG
	usbh_composite_hid_event_t last_event;
	__IO u32 event_cnt;
#endif

	rtos_task_t msg_parse_task;

	__IO u8 parse_task_alive;
	__IO u8 parse_task_exit;

	u8 report_desc_status;
	u8 itf_idx;
	u8 alt_set_idx;

	u8 next_xfer;
	usbh_composite_hid_usr_cb_t *cb;
} usbh_composite_hid_t;

/* Exported variables --------------------------------------------------------*/
extern const usbh_class_driver_t usbh_composite_hid_driver;

/* Exported functions --------------------------------------------------------*/
int usbh_composite_hid_init(usbh_composite_host_t *chost, usbh_composite_hid_usr_cb_t *cb);
int usbh_composite_hid_deinit(void);
int usbh_composite_hid_handle_report_desc(usb_host_t *host);

#endif // USBG_HID_H