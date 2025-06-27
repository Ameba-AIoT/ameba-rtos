#ifndef _WHC_BRIDGE_SDIO_DEV_H_
#define _WHC_BRIDGE_SDIO_DEV_H_

#define whc_dev_init_lite           whc_bridge_sdio_dev_init
#define whc_dev_api_send_data       whc_sdio_dev_send_data

void whc_bridge_sdio_dev_send_to_host(u8 *data, u32 len, u8 need_free);
void whc_bridge_sdio_dev_init(void);

#endif

