#ifndef _WHC_SPI_DEV_H_
#define _WHC_SPI_DEV_H_

#define whc_dev_init            whc_spi_dev_init
#define whc_dev_api_send_data   whc_spi_dev_send_cmd_data
#define _whc_dev_api_bus_is_idle       whc_spi_dev_bus_is_idle

void whc_spi_dev_init(void);
u8 whc_spi_dev_bus_is_idle(void);

#endif

