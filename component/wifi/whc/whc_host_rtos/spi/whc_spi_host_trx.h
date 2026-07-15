#ifndef __WHC_SPI_HOST_TRX_H__
#define __WHC_SPI_HOST_TRX_H__

#include "rtw_wifi_common.h"
#include "rtw_skbuff.h"

#define whc_host_send           whc_spi_host_send_pkt

int whc_spi_host_send_pkt(int idx, struct eth_drv_sg *sg_list, int sg_len, int total_len, struct skb_raw_para *raw_para, u8 is_special_pkt);

#endif /* __INIC_IPC_HOST_TRX_H__ */
