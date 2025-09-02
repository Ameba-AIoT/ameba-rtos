#ifndef __WHC_SPI_HOST_TRX_H__
#define __WHC_SPI_HOST_TRX_H__
/* -------------------------------- Includes -------------------------------- */
/* external head files */
#include "rtw_wifi_common.h"
#include "rtw_skbuff.h"
/* -------------------------------- Defines --------------------------------- */
#define whc_host_send           whc_spi_host_send

#define whc_host_send_to_dev           whc_spi_host_send_to_dev
/* -------------------------------- Macros ---------------------------------- */
/* ------------------------------- Data Types ------------------------------- */

/* ---------------------------- Global Variables ---------------------------- */

/* -------------------------- Function declaration -------------------------- */
int whc_spi_host_send(int idx, struct eth_drv_sg *sg_list, int sg_len, int total_len, struct skb_raw_para *raw_para, u8 is_special_pkt);
void whc_spi_host_send_to_dev(u8 *buf, u32 len);


#endif /* __INIC_IPC_HOST_TRX_H__ */


