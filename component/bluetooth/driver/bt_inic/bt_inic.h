#ifndef __BT_INIC_H__
#define __BT_INIC_H__

#include "basic_types.h"

void bt_usbd_inic_init(void);

int bt_inic_usb_init(void);
void bt_inic_usb_deinit(void);
uint8_t bt_inic_usb_hci_cmd_hdl(uint8_t *buf, uint16_t len);
uint8_t bt_inic_usb_hci_acl_hdl(uint8_t *buf, uint16_t len);
void bt_inic_usb_suspend_cb(void);
void bt_inic_usb_evt_txdone_cb(u8 *buf);
void bt_inic_usb_acl_txdone_cb(u8 *buf);
void bt_inic_usb_resume_cb(void);
void bt_inic_status_change_cb(uint8_t status);
void bt_inic_set_config(void);

void bt_inic_sdio_init(void);

void bt_inic_h4_init(void);
void bt_inic_h4_deinit(void);

void bt_inic_h5_init(void);
void bt_inic_h5_deinit(void);

void bt_inic_spi_init(void);
void bt_inic_spi_deinit(void);

#if defined(CONFIG_BT_INIC_SPI) && CONFIG_BT_INIC_SPI
#define bt_inic_init bt_inic_spi_init
#elif defined(CONFIG_BT_INIC_SDIO) && CONFIG_BT_INIC_SDIO
#define bt_inic_init bt_inic_sdio_init
#elif defined(CONFIG_BT_INIC_H4) && CONFIG_BT_INIC_H4
#define bt_inic_init bt_inic_h4_init
#elif defined(CONFIG_BT_INIC_H5) && CONFIG_BT_INIC_H5
#define bt_inic_init bt_inic_h5_init
#elif defined(CONFIG_BT_INIC_USB) && CONFIG_BT_INIC_USB
#define bt_inic_init bt_usbd_inic_init
#endif
#endif /* __BT_INIC_H__ */