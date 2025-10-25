#ifndef __BT_INIC_H__
#define __BT_INIC_H__

#include "basic_types.h"

uint8_t bt_inic_usb_hci_cmd_hdl(uint8_t *buf, uint16_t len);
uint8_t bt_inic_usb_acl_hdl(uint8_t *buf, uint16_t len);
void bt_inic_usb_suspend_cb(void);
void bt_inic_usb_resume_cb(void);
void bt_inic_status_change_cb(uint8_t status);

void bt_inic_sdio_init(void);

void bt_inic_h4_init(void);
void bt_inic_h4_deinit(void);

void bt_inic_h5_init(void);
void bt_inic_h5_deinit(void);

void bt_inic_spi_init(void);
void bt_inic_spi_deinit(void);

#if ((defined(CONFIG_WHC_INTF_SPI) && CONFIG_WHC_INTF_SPI) && (defined(CONFIG_BT_INIC_SHARE) && CONFIG_BT_INIC_SHARE)) || (defined(CONFIG_BT_INICD_SPI) && CONFIG_BT_INICD_SPI)
#define bt_inic_init bt_inic_spi_init
#elif ((defined(CONFIG_WHC_INTF_SDIO) && CONFIG_WHC_INTF_SDIO) && (defined(CONFIG_BT_INIC_SHARE) && CONFIG_BT_INIC_SHARE)) || (defined(CONFIG_BT_INICD_SDIO) && CONFIG_BT_INICD_SDIO)
#define bt_inic_init bt_inic_sdio_init
#elif ((defined(CONFIG_BT_INIC_H4) && CONFIG_BT_INIC_H4) || (defined(CONFIG_BT_INICD_H4) && CONFIG_BT_INICD_H4))
#define bt_inic_init bt_inic_h4_init
#elif ((defined(CONFIG_BT_INIC_H5) && CONFIG_BT_INIC_H5) || (defined(CONFIG_BT_INICD_H5) && CONFIG_BT_INICD_H5))
#define bt_inic_init bt_inic_h5_init
#endif
#endif /* __BT_INIC_H__ */