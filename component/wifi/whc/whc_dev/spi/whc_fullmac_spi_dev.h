#ifndef _FULLMAC_SPI_DEV_H_
#define _FULLMAC_SPI_DEV_H_

#define whc_dev_init                whc_fullmac_spi_dev_init
#define whc_dev_api_send_data       whc_spi_dev_send_cmd_data

void whc_fullmac_spi_dev_init(void);

#endif

