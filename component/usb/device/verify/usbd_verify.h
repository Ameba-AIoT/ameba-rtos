/**
  ******************************************************************************
  * The header file for usbd_verify.c
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#ifndef USBD_VERIFY_COMMON_H
#define USBD_VERIFY_COMMON_H

/* Includes ------------------------------------------------------------------*/
#include "usbd.h"

/* Exported defines ----------------------------------------------------------*/
extern u8 usbd_verify_cfg_desc[];
extern u8 usbd_verify_if_desc[];
extern u8 usbd_verify_ep_desc[];

/* Verify Device parameters */
#define USBD_VENDOR_CLASS_CODE					0xFFU
#define USBD_VENDOR_SUBCLASS_CODE				0x00U
#define USBD_VENDOR_PROTOCOL					0x00U
#define USBD_VENDOR_CONFIG_DESC_SIZE			83U
#define USBD_VENDOR_SELF_POWERED				1U
#define USBD_VENDOR_REMOTE_WAKEUP_EN			1U

#define USBD_VERIFY_CFG_LEN_OFFSET				2U
#define USBD_VERIFY_IF_EPNUM_OFFSET				4U
#define USBD_VERIFY_EP_ADDR_OFFSET				2U
#define USBD_VERIFY_EP_TYPE_OFFSET				3U
#define USBD_VERIFY_EP_MPS_OFFSET				4U
#define USBD_VERIFY_EP_INTERVAL_OFFSET			6U

#define USBD_VERIFY_CLASS_CODE					0xFFU
#define USBD_VERIFY_SUBCLASS_CODE				0x00U
#define USBD_VERIFY_PROTOCOL					0x00U

/*
	D7: Reserved, set 1 	: 0x01
	D6: Self Powered	: 0x01 (yes)
	D5: Remote Wakeup	: 0x01 (yes)
	D4..0: Reserved, set 0	: 0x00
*/
#define CONFIG_ATTRIBUTE						0xE0

//Default PID/VID
#define USBD_VERIFY_DEFAULT_VID					USB_VID
#define USBD_VERIFY_DEFAULT_PID					USB_PID

#define USBD_VENDOR_LANGID_STRING				0x0409U
#define USBD_VERIFY_MFG_STRING					"Realtek"
#define USBD_VERIFY_FS_STRING					"Realtek Verify(FS)"
#define USBD_VERIFY_HS_STRING					"Realtek Verify(HS)"
#define USBD_VERIFY_SN_STRING					"01234567890"

#define USBD_VERIFY_CTRL_BUF_SIZE				512U
#define USBD_VERIFY_MATCH_EP_ADDR				(0xFF)
/*
	Configuration + Interface + ep*Endpoint
*/
#define USBD_EP_DESCRIPTION_SIZE(ep)			(USB_LEN_CFG_DESC + USB_LEN_IF_DESC + ep * USB_LEN_EP_DESC)

/* Exported types ------------------------------------------------------------*/

/* States for Endpoint */
typedef enum {
	VERIFY_TRANSFER_STATE_IDLE = 0,
	VERIFY_TRANSFER_STATE_TX,
	VERIFY_TRANSFER_STATE_TX_BUSY,
} usbd_verify_state_t;

typedef struct {
	u16          mps;	//buf length, packet MPS
	u16          trans_len;	//transfor data length
	u8           ep_addr;
	u8           match_addr;
	u8           ep_type;
	u8           ep_interval;
} usbd_verify_ep_basic_t;

typedef struct {
	usbd_verify_ep_basic_t ep_infor;
	__IO u32     done_count ; //transfor packet success count
	__IO u32     send_count ; //transfor packet count
	__IO u32     drop_count ; //transfor packet drop count
	u8           *buf;
	__IO u8      state;
	u8           in_start;
	u8           ep_init : 1;
	u8           zlp : 1;
} usbd_verify_ep_t;


typedef struct {
	int(* init)(void);
	int(* deinit)(void);
	u8 *(* get_config_desc)(u16 *strlen);

	int(* set_config)(usb_dev_t *dev);
	int(* clear_config)(usb_dev_t *dev);
	int(* setup)(u8 cmd, u8 *buf, u16 len, u16 value);

	int(*ep0_data_in)(usb_dev_t *dev, u8 status);
	int(*ep0_data_out)(usb_dev_t *dev);
	int(*ep_data_in)(usb_dev_t *dev, u8 ep_num, u8 status);
	int(*ep_data_out)(usb_dev_t *dev, u8 ep_num, u16 len);
} usbd_verify_cb_t;

typedef struct {
	usbd_verify_ep_t     ctrl_ep0;
	usb_dev_t            *dev;
	usbd_verify_cb_t     *cb;
	u8                   ctrl_req;
	u8                   alt_setting;
	u8                   enable_dump : 1;
} usbd_verify_device_t;

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
int usbd_verify_init(usbd_verify_cb_t *cb);
int usbd_verify_deinit(void);

int usbd_verify_ep_init(usbd_verify_ep_t *ep, u8 type, u8 epaddr, u8 intervalue, u16 mps, u16 trans_size, u8 matchep);
int usbd_verify_ep_deinit(usbd_verify_ep_t *ep);

int usbd_verify_transmit_zlp(u8 addr);
int usbd_verify_transmit_data(usbd_verify_ep_t *ep);
int usbd_verify_receive_data(usbd_verify_ep_t *ep);

int usbd_verify_transmit_ctrl_data(u8 *buf, u16 len);

int usbd_verify_ep_enable(usb_dev_t *dev, usbd_verify_ep_t *ep);
int usbd_verify_ep_disable(usb_dev_t *dev, usbd_verify_ep_t *ep);

/* dump apis */
void usbd_verify_dump_ep(usbd_verify_ep_t *ep);
void usbd_verify_dump_buf(u8 *type, u8 *buf, u32 length, u32 count);
char *usbd_verify_get_xfer_type_text(u8 ep_type);
void usbd_verify_enable_dump_debug(u8 flag);

#endif  /* USBD_VERIFY_COMMON_H */

