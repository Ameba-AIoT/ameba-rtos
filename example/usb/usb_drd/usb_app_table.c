/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include <platform_autoconf.h>
#include "usb_app_mgr.h"

#ifdef CONFIG_USBD_CDC_ACM
#include "example_usbd_cdc_acm.h"
#endif
/* The 4G dongle option turns the ACM host class into a VID/PID matched dongle driver, which
   usbh_cdc_acm can not use; with it on, the ECM Ethernet host is the 4G composite instead of
   usbh_cdc_ecm. Same derivation as in CMakeLists.txt. */
#if defined(CONFIG_USBH_CDC_ACM) && defined(CONFIG_USBH_CDC_ACM_4G_DONGLE)
#define USB_APP_HOST_4G
#endif
#if defined(CONFIG_USBH_CDC_ACM) && !defined(USB_APP_HOST_4G)
#define USB_APP_HOST_ACM
#include "example_usbh_cdc_acm.h"
#endif
#ifdef CONFIG_USBD_VENDOR
#include "example_usbd_vendor.h"
#endif
#ifdef CONFIG_USBH_VENDOR
#include "example_usbh_vendor.h"
#endif
#ifdef CONFIG_USBD_DFU
#include "example_usbd_dfu.h"
#endif
#ifdef CONFIG_USBH_DFU
#include "example_usbh_dfu.h"
#endif
#ifdef CONFIG_USBD_MSC
#include "example_usbd_msc.h"
#endif
#ifdef CONFIG_USBH_MSC
#include "example_usbh_msc.h"
#endif
#ifdef CONFIG_USBH_HID
#include "example_usbh_hid.h"
#endif
#ifdef CONFIG_USBD_HID
#include "example_usbd_hid.h"
#endif
#ifdef CONFIG_USBD_UVC
#include "example_usbd_uvc.h"
#endif
#if defined(CONFIG_USBD_COMPOSITE) && defined(CONFIG_USBD_CDC_ACM) && defined(CONFIG_USBD_HID)
#define USB_APP_COMP_ACM_HID
#include "example_usbd_composite_cdc_acm_hid.h"
#endif
#if defined(CONFIG_USBD_COMPOSITE) && defined(CONFIG_USBD_CDC_ACM) && defined(CONFIG_USBD_MSC)
#define USB_APP_COMP_ACM_MSC
#include "example_usbd_composite_cdc_acm_msc.h"
#endif
#ifdef CONFIG_USBD_CDC_ECM
#include "example_usbd_cdc_ecm.h"
#endif
#ifdef CONFIG_USBD_CDC_NCM
#include "example_usbd_cdc_ncm.h"
#endif
#if defined(CONFIG_USBH_CDC_ECM) && defined(CONFIG_USBH_ECM_ETHERNET) && !defined(USB_APP_HOST_4G)
#define USB_APP_HOST_ECM
#include "example_usbh_cdc_ecm.h"
#endif
#if defined(CONFIG_USBH_CDC_ECM) && defined(CONFIG_USBH_ECM_ETHERNET) && defined(USB_APP_HOST_4G)
#define USB_APP_HOST_ACM_ECM
#include "example_usbh_composite_cdc_acm_ecm.h"
#endif
#if defined(CONFIG_USBH_CDC_ECM) && defined(CONFIG_USBH_ECM_ETHERNET_BRIDGE)
#define USB_APP_HOST_BRIDGE
#include "example_usbh_wifi_bridge.h"
#endif
#ifdef CONFIG_USBH_CDC_NCM
#include "example_usbh_cdc_ncm.h"
#endif
#ifdef CONFIG_USBD_UAC
#include "example_usbd_uac.h"
#endif
#if defined(CONFIG_USBD_COMPOSITE) && defined(CONFIG_USBD_CDC_ACM) && defined(CONFIG_USBD_UAC)
#define USB_APP_COMP_ACM_UAC
#include "example_usbd_composite_cdc_acm_uac.h"
#endif
#if defined(CONFIG_USBD_COMPOSITE) && defined(CONFIG_USBD_HID_HEADSET) && defined(CONFIG_USBD_UAC)
#define USB_APP_COMP_HID_UAC
#include "example_usbd_composite_hid_uac.h"
#endif
#ifdef CONFIG_USBH_UAC
#include "example_usbh_uac.h"
#endif
#if defined(CONFIG_USBH_HID) && defined(CONFIG_USBH_UAC)
#define USB_APP_HOST_HID_UAC
#include "example_usbh_composite_hid_uac.h"
#endif
#ifdef CONFIG_USBH_UVC
#include "example_usbh_uvc.h"
#endif
#if defined(CONFIG_USBH_UVC) && defined(CONFIG_USBH_UAC)
#define USB_APP_HOST_UVC_UAC
#include "example_usbh_composite_uvc_uac.h"
#endif

/* Private defines -----------------------------------------------------------*/

/* The lwIP USB netif has one TX hook, usb_ethernet_transmit(), defined by the network app itself:
   an image carries at most one of the CDC ECM / NCM device and host apps. The USBH_CDC_ECM class
   builds exactly one of usbh_cdc_ecm, usbh_composite_cdc_acm_ecm (4G dongle) and usbh_wifi_bridge. */
#if (defined(CONFIG_USBD_CDC_ECM) + defined(CONFIG_USBD_CDC_NCM) + defined(USB_APP_HOST_ECM) + defined(CONFIG_USBH_CDC_NCM) + \
	 defined(USB_APP_HOST_ACM_ECM) + defined(USB_APP_HOST_BRIDGE)) > 1
#error "usb_drd: enable at most one network app (USBD_CDC_ECM / USBD_CDC_NCM / USBH_CDC_ECM / USBH_CDC_NCM) in prj.conf"
#endif

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* An app registers its own AT command, named after it (AT+USB<D|H><APP>), so no two apps share one */
#define USB_APP(n, r)   {.name = #n, .start = example_##n##_start, .stop = example_##n##_stop, .role = (r)}

/* Private function prototypes -----------------------------------------------*/


/* Private variables ---------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* The apps built into this image: an entry is present when the class Kconfig of the app is on
   (see prj.conf), which is also what pulls its example source into the build */
const usb_app_t usb_app_table[] = {
#ifdef CONFIG_USBD_CDC_ACM
	USB_APP(usbd_cdc_acm, USB_APP_ROLE_DEVICE),
#endif
#ifdef CONFIG_USBD_VENDOR
	USB_APP(usbd_vendor, USB_APP_ROLE_DEVICE),
#endif
#ifdef CONFIG_USBD_DFU
	USB_APP(usbd_dfu, USB_APP_ROLE_DEVICE),
#endif
#ifdef CONFIG_USBD_MSC
	USB_APP(usbd_msc, USB_APP_ROLE_DEVICE),
#endif
#ifdef USB_APP_HOST_ACM
	USB_APP(usbh_cdc_acm, USB_APP_ROLE_HOST),
#endif
#ifdef CONFIG_USBH_VENDOR
	USB_APP(usbh_vendor, USB_APP_ROLE_HOST),
#endif
#ifdef CONFIG_USBH_DFU
	USB_APP(usbh_dfu, USB_APP_ROLE_HOST),
#endif
#ifdef CONFIG_USBD_HID
	USB_APP(usbd_hid, USB_APP_ROLE_DEVICE),
#endif
#ifdef CONFIG_USBD_UVC
	USB_APP(usbd_uvc, USB_APP_ROLE_DEVICE),
#endif
#ifdef USB_APP_COMP_ACM_HID
	USB_APP(usbd_composite_cdc_acm_hid, USB_APP_ROLE_DEVICE),
#endif
#ifdef USB_APP_COMP_ACM_MSC
	USB_APP(usbd_composite_cdc_acm_msc, USB_APP_ROLE_DEVICE),
#endif
#ifdef CONFIG_USBD_CDC_ECM
	USB_APP(usbd_cdc_ecm, USB_APP_ROLE_DEVICE),
#endif
#ifdef CONFIG_USBD_CDC_NCM
	USB_APP(usbd_cdc_ncm, USB_APP_ROLE_DEVICE),
#endif
#ifdef CONFIG_USBH_MSC
	USB_APP(usbh_msc, USB_APP_ROLE_HOST),
#endif
#ifdef USB_APP_HOST_ECM
	USB_APP(usbh_cdc_ecm, USB_APP_ROLE_HOST),
#endif
#ifdef CONFIG_USBH_CDC_NCM
	USB_APP(usbh_cdc_ncm, USB_APP_ROLE_HOST),
#endif
#ifdef CONFIG_USBH_HID
	USB_APP(usbh_hid, USB_APP_ROLE_HOST),
#endif
#ifdef CONFIG_USBD_UAC
	USB_APP(usbd_uac, USB_APP_ROLE_DEVICE),
#endif
#ifdef USB_APP_COMP_ACM_UAC
	USB_APP(usbd_composite_cdc_acm_uac, USB_APP_ROLE_DEVICE),
#endif
#ifdef USB_APP_COMP_HID_UAC
	USB_APP(usbd_composite_hid_uac, USB_APP_ROLE_DEVICE),
#endif
#ifdef CONFIG_USBH_UAC
	USB_APP(usbh_uac, USB_APP_ROLE_HOST),
#endif
#ifdef USB_APP_HOST_HID_UAC
	USB_APP(usbh_composite_hid_uac, USB_APP_ROLE_HOST),
#endif
#ifdef CONFIG_USBH_UVC
	USB_APP(usbh_uvc, USB_APP_ROLE_HOST),
#endif
#ifdef USB_APP_HOST_UVC_UAC
	USB_APP(usbh_composite_uvc_uac, USB_APP_ROLE_HOST),
#endif
#ifdef USB_APP_HOST_ACM_ECM
	USB_APP(usbh_composite_cdc_acm_ecm, USB_APP_ROLE_HOST),
#endif
#ifdef USB_APP_HOST_BRIDGE
	USB_APP(usbh_wifi_bridge, USB_APP_ROLE_HOST),
#endif
	{.name = NULL},
};

const u32 usb_app_table_num = (sizeof(usb_app_table) / sizeof(usb_app_table[0])) - 1U;

/* Private functions ---------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
