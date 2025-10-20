/*
 * Copyright (c) 2025 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_COMPOSITE_CONFIG_H
#define USBH_COMPOSITE_CONFIG_H

/* Includes ------------------------------------------------------------------*/

#include "usbh.h"

// #define  CONFIG_USBH_COMPOSITE_HID_UAC 1

/* Exported defines ----------------------------------------------------------*/
#if defined(CONFIG_USBH_COMPOSITE_HID_UAC)

#define USBH_COMPOSITE_HID_UAC_DEBUG              1

#else
#error "No composite class configured"
#endif

/* Exported types ------------------------------------------------------------*/
typedef struct {
	u8 bLength;
	u8 bDescriptorType;
} usb_descriptor_header_t;

typedef enum {
	USBH_COMPOSITE_IDLE = 0U,
	USBH_COMPOSITE_DETACHED,
	USBH_COMPOSITE_ATTACH,
	USBH_COMPOSITE_SETUP,
	USBH_COMPOSITE_MAX,
} usbh_compsoite_state_t;

typedef struct {
	u8 *buf;
	__IO u16 buf_len;     /* buf valid len */
} usb_ringbuf_t;

typedef struct {
	usb_ringbuf_t *list_node;
	u8 *buf;

	__IO u16 head;  //read idx
	__IO u16 tail;  //write idx

	__IO u16 written;  //part write data length

	u16 item_cnt;
	u16 item_size;
} usb_ringbuf_manager_t;

typedef struct {
	void (*status_changed)(u8 old_status, u8 status);
	int (* set_config)(void);
} usbh_composite_cb_t;

typedef struct {
#if defined(CONFIG_USBH_COMPOSITE_HID_UAC)
	usbh_class_driver_t *hid;
	usbh_class_driver_t *uac;
#endif

	usbh_composite_cb_t *cb;
	usb_host_t *host;
	__IO usbh_compsoite_state_t state;
} usbh_composite_host_t;

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
u32 usb_ringbuf_get_count(usb_ringbuf_manager_t *handle);
int usb_ringbuf_is_empty(usb_ringbuf_manager_t *handle);
int usb_ringbuf_is_full(usb_ringbuf_manager_t *handle);

int usb_ringbuf_add_tail(usb_ringbuf_manager_t *handle, u8 *buf, u32 size);
u32 usb_ringbuf_remove_head(usb_ringbuf_manager_t *handle, u8 *buf, u32 size);

usb_ringbuf_t *usb_ringbuf_get_head(usb_ringbuf_manager_t *handle);
int usb_ringbuf_release_head(usb_ringbuf_manager_t *handle);

int usb_ringbuf_write_partial(usb_ringbuf_manager_t *handle, u8 *data, u32 len);
int usb_ringbuf_finish_write(usb_ringbuf_manager_t *handle);

int usb_ringbuf_manager_init(usb_ringbuf_manager_t *handle, u16 cnt, u16 size, u8 cache_align);
int usb_ringbuf_manager_deinit(usb_ringbuf_manager_t *handle);

#endif // USBH_COMPOSITE_CONFIG_H

