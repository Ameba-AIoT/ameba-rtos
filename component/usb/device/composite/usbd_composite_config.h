/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBD_COMPOSITE_CONFIG_H
#define USBD_COMPOSITE_CONFIG_H

/* Includes ------------------------------------------------------------------*/

#include "usbd.h"

/* Exported defines ----------------------------------------------------------*/

#define USBD_COMP_VID                     USB_VID
#define USBD_COMP_PID                     USB_PID

#define USBD_COMP_LANGID                  0x0409U
#define USBD_COMP_SELF_POWERED            1U
#define USBD_COMP_REMOTE_WAKEUP_EN        1U

#define USBD_COMP_MFG_STRING              "Realtek"
#define USBD_COMP_PROD_STRING             "Realtek Composite Device"
#define USBD_COMP_SN_STRING               "1234567890"


#if defined(CONFIG_USBD_COMPOSITE_CDC_ACM_HID)
/* Interfaces */
#define USBD_COMP_CDC_COM_ITF             0x00
#define USBD_COMP_CDC_DAT_ITF             0x01
#define USBD_COMP_HID_ITF                 0x02
/* Endpoints */
#if defined (CONFIG_AMEBAGREEN2)
#define USBD_COMP_CDC_BULK_IN_EP          0x82U
#define USBD_COMP_CDC_BULK_OUT_EP         0x02U
#define USBD_COMP_CDC_INTR_IN_EP          0x83U
#define USBD_COMP_HID_INTR_OUT_EP         0x05U
#define USBD_COMP_HID_INTR_IN_EP          0x84U
#elif defined (CONFIG_AMEBAL2)
#define USBD_COMP_CDC_BULK_IN_EP          0x81U
#define USBD_COMP_CDC_BULK_OUT_EP         0x01U
#define USBD_COMP_CDC_INTR_IN_EP          0x82U
#define USBD_COMP_HID_INTR_OUT_EP         0x02U
#define USBD_COMP_HID_INTR_IN_EP          0x83U
#else
#define USBD_COMP_CDC_BULK_IN_EP          0x81U
#define USBD_COMP_CDC_BULK_OUT_EP         0x02U
#define USBD_COMP_CDC_INTR_IN_EP          0x83U
#define USBD_COMP_HID_INTR_OUT_EP         0x04U
#define USBD_COMP_HID_INTR_IN_EP          0x85U
#endif
/* String indices */
#define USBD_IDX_CDC_ITF_STR              ((USBD_IDX_SERIAL_STR) + 1)
#define USBD_IDX_HID_ITF_STR              ((USBD_IDX_CDC_ITF_STR) + 1)
/* Strings */
#define USBD_COMP_CDC_HS_ITF_STRING       "Realtek HS CDC ACM Device"
#define USBD_COMP_CDC_FS_ITF_STRING       "Realtek FS CDC ACM Device"
#define USBD_COMP_HID_HS_ITF_STRING       "Realtek HS HID Device"
#define USBD_COMP_HID_FS_ITF_STRING       "Realtek FS HID Device"

#elif defined(CONFIG_USBD_COMPOSITE_CDC_ACM_UAC)  //acm+uac
/* Interfaces */
#define USBD_COMP_UAC_AC_HEADSET               0x00
#define USBD_COMP_UAC_AS_HEADSET_HEADPHONES    0x01
#define USBD_COMP_CDC_COM_ITF                  0x02
#define USBD_COMP_CDC_DAT_ITF                  0x03
/* Endpoints */
#if defined (CONFIG_AMEBAGREEN2)
#define USBD_COMP_CDC_BULK_IN_EP               0x82U
#define USBD_COMP_CDC_BULK_OUT_EP              0x02U
#define USBD_COMP_CDC_INTR_IN_EP               0x83U
#define USBD_COMP_HID_INTR_OUT_EP              0x05U
#define USBD_COMP_HID_INTR_IN_EP               0x84U
#else
#define USBD_COMP_CDC_BULK_IN_EP               0x81U
#define USBD_COMP_CDC_BULK_OUT_EP              0x02U
#define USBD_COMP_CDC_INTR_IN_EP               0x83U
#define USBD_COMP_UAC_ISOC_OUT_EP              0x04U
#define USBD_COMP_UAC_ISOC_IN_EP               0x85U
#endif

/* String indices */
#define USBD_IDX_CDC_ITF_STR                   ((USBD_IDX_SERIAL_STR) + 1)
#define USBD_IDX_UAC_ITF_STR                   ((USBD_IDX_CDC_ITF_STR) + 1)
/* Strings */
#define USBD_COMP_CDC_HS_ITF_STRING            "Realtek HS CDC ACM Device"
#define USBD_COMP_CDC_FS_ITF_STRING            "Realtek FS CDC ACM Device"
#define USBD_COMP_UAC_HS_ITF_STRING            "Realtek HS UAC Device"
#define USBD_COMP_UAC_FS_ITF_STRING            "Realtek FS UAC Device"

#elif defined(CONFIG_USBD_COMPOSITE_CDC_ACM_MSC) //acm + msc
/* Interfaces */
#define USBD_COMP_CDC_COM_ITF                  0x00
#define USBD_COMP_CDC_DAT_ITF                  0x01
#define USBD_COMP_MSC_ITF                      0x02
/* Endpoints */
#if defined (CONFIG_AMEBAGREEN2)
#define USBD_COMP_CDC_BULK_IN_EP               0x82U
#define USBD_COMP_CDC_BULK_OUT_EP              0x02U
#define USBD_COMP_CDC_INTR_IN_EP               0x83U
#define USBD_COMP_MSC_BULK_OUT_EP              0x05U
#define USBD_COMP_MSC_BULK_IN_EP               0x84U
#else
#define USBD_COMP_CDC_BULK_IN_EP               0x81U
#define USBD_COMP_CDC_BULK_OUT_EP              0x02U
#define USBD_COMP_CDC_INTR_IN_EP               0x83U
#define USBD_COMP_MSC_BULK_OUT_EP              0x04U
#define USBD_COMP_MSC_BULK_IN_EP               0x85U
#endif
/* String indices */
#define USBD_IDX_CDC_ITF_STR                   ((USBD_IDX_SERIAL_STR) + 1)
#define USBD_IDX_MSC_ITF_STR                   ((USBD_IDX_CDC_ITF_STR) + 1)
/* Strings */
#define USBD_COMP_CDC_HS_ITF_STRING            "Realtek HS CDC ACM Device"
#define USBD_COMP_CDC_FS_ITF_STRING            "Realtek FS CDC ACM Device"
#define USBD_COMP_MSC_HS_ITF_STRING            "Realtek HS MSC Device"
#define USBD_COMP_MSC_FS_ITF_STRING            "Realtek FS MSC Device"

#elif defined(CONFIG_USBD_COMPOSITE_HID_UAC)  //hid+uac

#define USBD_COMP_UAC_PID                      (USBD_COMP_PID)

/* Interfaces */
#define USBD_COMP_UAC_AC_HEADSET               0x00
#define USBD_COMP_UAC_AS_HEADSET_HEADPHONES    0x01
#define USBD_COMP_HID_ITF                      0x02
#define USBD_COMP_HID_VEND_ITF                 0x03

/* Endpoints */
#define USBD_COMP_HID_INTR_IN_EP               0x81U
#define USBD_COMP_HID_INTR_OUT_EP              0x02U
#define USBD_COMP_HID_CONSUMER_INTR_IN_EP      0x85U

#define USBD_COMP_UAC_ISOC_IN_EP               0x83U
#define USBD_COMP_UAC_ISOC_OUT_EP              0x04U

/* String indices */
#define USBD_IDX_UAC_ITF_STR                   ((USBD_IDX_SERIAL_STR) + 1)
#define USBD_IDX_HID_ITF_STR                   ((USBD_IDX_SERIAL_STR) + 2)
#define USBD_IDX_HID_VEND_ITF_STR              ((USBD_IDX_SERIAL_STR) + 3)

/* Strings */
#define USBD_COMP_HID_HS_ITF_STRING            "Realtek HS HID Priv Device"
#define USBD_COMP_HID_FS_ITF_STRING            "Realtek FS HID Priv Device"
#define USBD_COMP_HID_CONSUMER_HS_ITF_STRING   "Realtek HS HID Consumer Device"
#define USBD_COMP_HID_CONSUMER_FS_ITF_STRING   "Realtek FS HID Consumer Device"
#define USBD_COMP_UAC_HS_ITF_STRING            "Realtek HS UAC Device"
#define USBD_COMP_UAC_FS_ITF_STRING            "Realtek FS UAC Device"

#define USBD_COMP_HID_MAX_BUF_SIZE             64U

#define USBD_COMP_HID_REPORT_ID                0x1F
#define USBD_COMP_HID_VOLUME_REPORT_ID         ((USBD_COMP_HID_REPORT_ID) + 1)
#define USBD_COMP_HID_TX_PKT_SIZE              ((USBD_COMP_HID_MAX_BUF_SIZE)-4)
#define USBD_COMP_HID_RX_PKT_SIZE              ((USBD_COMP_HID_MAX_BUF_SIZE)-1)

#else
#error "No composite class configured"
#endif

/* Exported types ------------------------------------------------------------*/

typedef struct {
	void (*status_changed)(u8 old_status, u8 status);
	int (* set_config)(void);
} usbd_composite_cb_t;

typedef struct {
	usb_setup_req_t ctrl_req;

#if defined(CONFIG_USBD_COMPOSITE_CDC_ACM_HID)
	usbd_class_driver_t *cdc;
	usbd_class_driver_t *hid;
#elif defined(CONFIG_USBD_COMPOSITE_CDC_ACM_UAC)
	usbd_class_driver_t *cdc;
	usbd_class_driver_t *uac;
#elif defined(CONFIG_USBD_COMPOSITE_CDC_ACM_MSC)
	usbd_class_driver_t *cdc;
	usbd_class_driver_t *msc;
#elif defined(CONFIG_USBD_COMPOSITE_HID_UAC)
	usbd_class_driver_t *hid;  /* used for hid private command */
	usbd_class_driver_t *uac;
#endif

	usbd_composite_cb_t *cb;
	usb_dev_t *dev;
} usbd_composite_dev_t;

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

#endif // USBD_COMPOSITE_CONFIG_H

