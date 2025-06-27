#ifndef _BRIDGE_SPI_DEV_H_
#define _BRIDGE_SPI_DEV_H_

#define whc_dev_init_lite      whc_bridge_spi_dev_init
#define whc_dev_api_send_data   whc_spi_dev_send_cmd_data

void whc_bridge_spi_dev_init(void);

#endif

