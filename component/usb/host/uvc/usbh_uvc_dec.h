/**
  ******************************************************************************
  * The header file for uvc decoder
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#ifndef USBH_UVC_DEC_H
#define USBH_UVC_DEC_H

#define USBH_MAX_BUF_NUM		3


typedef struct {
	u32 buf_start_addr;
	u32 buf_size;
	u32 is_used;
} usbh_uvc_dec_buf;

typedef struct {
	u32 ch;
	usbh_uvc_dec_buf uvc_dec_buf[USBH_MAX_BUF_NUM];
	u32 frame_done_num;
	u32 frame_done_size;
	u32 free_buf_num;
	u32 ch_buf_num;
	u32 ep_num;
	u32 pipe_num;
	u32 binterval;
	u32 dev_addr;
	void *priv;

	/* cmd reg related */
	u32 ep_size;
	u32 mps;
	rtos_sema_t dec_sema;
} usbh_uvc_dec;

usbh_uvc_dec *usbh_uvc_dec_alloc_channel(void);
void usbh_uvc_dec_free_channel(usbh_uvc_dec *uvc_dec);
void usbh_uvc_dec_init(usbh_uvc_dec *uvc_dec);
void usbh_uvc_dec_start(usbh_uvc_dec *uvc_dec);
void usbh_uvc_dec_stop(usbh_uvc_dec *uvc_dec);
void usbh_uvc_dec_dump_reg(void);


#endif
