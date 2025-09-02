#ifndef _WHC_SDIO_DEV_H_
#define _WHC_SDIO_DEV_H_

#define whc_dev_init                whc_sdio_dev_init
#define whc_dev_api_send_data       whc_sdio_dev_send_data
#define _whc_dev_api_bus_is_idle       whc_sdio_dev_bus_is_idle

void whc_sdio_dev_init(void);
u8 whc_sdio_dev_bus_is_idle(void);

#endif

