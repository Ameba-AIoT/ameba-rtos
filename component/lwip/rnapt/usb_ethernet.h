/**
 * @file usb_ethernet.h
 * @brief USB Ethernet Network Management for R-NAPT
 */

#ifndef __USB_ETHERNET_H
#define __USB_ETHERNET_H

#include "platform_autoconf.h"

#if defined(CONFIG_LWIP_USB_ETHERNET)

#ifdef __cplusplus
extern "C" {
#endif

void usb_eth_init(void);
void usb_eth_register_link_cb(void (*cb)(int link_up));

#ifdef __cplusplus
}
#endif

#endif /* CONFIG_LWIP_USB_ETHERNET */

#endif /* __USB_ETHERNET_H */
