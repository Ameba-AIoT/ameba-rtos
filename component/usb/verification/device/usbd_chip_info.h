/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __AMEBA_USBD_VERIFY_CHIP_INFO_H
#define __AMEBA_USBD_VERIFY_CHIP_INFO_H

/* VERIFY Endpoint parameters */
#if defined (CONFIG_AMEBAGREEN2)
#define VERIFY_BULK_IN_EP                          0x86U  /* EP6 for BULK IN */
#define VERIFY_BULK_OUT_EP                         0x03U  /* EP3 for BULK OUT */
#define VERIFY_ISOC_IN_EP                          0x82U  /* EP2 for ISOC IN */
#define VERIFY_ISOC_OUT_EP                         0x02U  /* EP2 for ISOC OUT */
#define VERIFY_INTR_IN_EP                          0x84U  /* EP4 for INTR IN */
#define VERIFY_INTR_OUT_EP                         0x05U  /* EP5 for INTR OUT */
#elif defined (CONFIG_AMEBASMARTPLUS) || defined (CONFIG_AMEBAL2)
#define VERIFY_BULK_IN_EP                          0x84U  /* EP4 for BULK IN */
#define VERIFY_BULK_OUT_EP                         0x04U  /* EP4 for BULK OUT */
#define VERIFY_ISOC_IN_EP                          0x81U  /* EP1 for ISOC IN */
#define VERIFY_ISOC_OUT_EP                         0x01U  /* EP1 for ISOC OUT */
#define VERIFY_INTR_IN_EP                          0x82U  /* EP2 for INTR IN */
#define VERIFY_INTR_OUT_EP                         0x02U  /* EP2 for INTR OUT */
#else
#define VERIFY_BULK_IN_EP                          0x81U  /* EP1 for BULK IN */
#define VERIFY_BULK_OUT_EP                         0x02U  /* EP2 for BULK OUT */
#define VERIFY_ISOC_IN_EP                          0x83U  /* EP3 for ISOC IN */
#define VERIFY_ISOC_OUT_EP                         0x04U  /* EP4 for ISOC OUT */
#define VERIFY_INTR_IN_EP                          0x85U  /* EP5 for INTR IN */
#define VERIFY_INTR_OUT_EP                         0x05U  /* EP5 for INTR OUT */
#endif

#define VERIFY_HS_BULK_MAX_PACKET_SIZE             512U   /* High speed BULK IN & OUT packet size */
#define VERIFY_FS_BULK_MAX_PACKET_SIZE             64U    /* Full speed BULK IN & OUT packet size */
#define VERIFY_BULK_BUF_SIZE                       2048U   /* BULK IN & OUT buffer size */

#define VERIFY_HS_ISOC_MAX_PACKET_SIZE             1024U   /* High speed ISOC IN & OUT packet size */
#define VERIFY_FS_ISOC_MAX_PACKET_SIZE             1023U    /* Full speed ISOC IN & OUT packet size */
#define VERIFY_HS_ISOC_BUF_SIZE             	   1024U   /* High speed BULK IN & OUT buffer size */
#define VERIFY_FS_ISOC_BUF_SIZE             	   1023U   /* Full speed BULK IN & OUT buffer size */

#define VERIFY_HS_INTR_MAX_PACKET_SIZE             1024U   /* High speed INTR IN & OUT packet size */
#define VERIFY_FS_INTR_MAX_PACKET_SIZE             64U    /* Full speed INTR IN & OUT packet size */
#define VERIFY_HS_INTR_BUF_SIZE                    1024U   /* High speed INTR IN & OUT buffer size */
#define VERIFY_FS_INTR_BUF_SIZE                    64U   /* Full speed INTR IN & OUT buffer size */

/*    Verify Usb Device Ep Config Start           */
static usbd_verify_ep_basic_t usbd_ep_hs[] = {
	{VERIFY_HS_BULK_MAX_PACKET_SIZE,	VERIFY_BULK_BUF_SIZE, VERIFY_BULK_OUT_EP, VERIFY_BULK_IN_EP, USB_CH_EP_TYPE_BULK, 0U},
	{VERIFY_HS_BULK_MAX_PACKET_SIZE,	VERIFY_BULK_BUF_SIZE, VERIFY_BULK_IN_EP, VERIFY_BULK_OUT_EP, USB_CH_EP_TYPE_BULK, 0U},
	{VERIFY_HS_ISOC_MAX_PACKET_SIZE,	VERIFY_HS_ISOC_BUF_SIZE, VERIFY_ISOC_OUT_EP, VERIFY_ISOC_IN_EP, USB_CH_EP_TYPE_ISOC, 1U},
	{VERIFY_HS_ISOC_MAX_PACKET_SIZE,	VERIFY_HS_ISOC_BUF_SIZE, VERIFY_ISOC_IN_EP, VERIFY_ISOC_OUT_EP, USB_CH_EP_TYPE_ISOC, 1U},
	{VERIFY_HS_INTR_MAX_PACKET_SIZE,	VERIFY_HS_INTR_BUF_SIZE, VERIFY_INTR_OUT_EP, VERIFY_INTR_IN_EP, USB_CH_EP_TYPE_INTR, 1U},
	{VERIFY_HS_INTR_MAX_PACKET_SIZE,	VERIFY_HS_INTR_BUF_SIZE, VERIFY_INTR_IN_EP, VERIFY_INTR_OUT_EP, USB_CH_EP_TYPE_INTR, 1U},
	{0, 0, 0, 0, 0, 0},
};
static usbd_verify_ep_basic_t usbd_ep_fs[] = {
	{VERIFY_FS_BULK_MAX_PACKET_SIZE,	VERIFY_BULK_BUF_SIZE, VERIFY_BULK_OUT_EP, VERIFY_BULK_IN_EP, USB_CH_EP_TYPE_BULK, 0U},
	{VERIFY_FS_BULK_MAX_PACKET_SIZE,	VERIFY_BULK_BUF_SIZE, VERIFY_BULK_IN_EP, VERIFY_BULK_OUT_EP, USB_CH_EP_TYPE_BULK, 0U},
	{VERIFY_FS_ISOC_MAX_PACKET_SIZE,	VERIFY_FS_ISOC_BUF_SIZE, VERIFY_ISOC_OUT_EP, VERIFY_ISOC_IN_EP, USB_CH_EP_TYPE_ISOC, 1U},
	{VERIFY_FS_ISOC_MAX_PACKET_SIZE,	VERIFY_FS_ISOC_BUF_SIZE, VERIFY_ISOC_IN_EP, VERIFY_ISOC_OUT_EP, USB_CH_EP_TYPE_ISOC, 1U},
	{VERIFY_FS_INTR_MAX_PACKET_SIZE,	VERIFY_FS_INTR_BUF_SIZE, VERIFY_INTR_OUT_EP, VERIFY_INTR_IN_EP, USB_CH_EP_TYPE_INTR, 1U},
	{VERIFY_FS_INTR_MAX_PACKET_SIZE,	VERIFY_FS_INTR_BUF_SIZE, VERIFY_INTR_IN_EP, VERIFY_INTR_OUT_EP, USB_CH_EP_TYPE_INTR, 1U},

	{0, 0, 0, 0, 0, 0},
};
/*    Verify Usb Device Ep Config Finish           */


#endif	//__AMEBA_USBD_VERIFY_CHIP_INFO_H
