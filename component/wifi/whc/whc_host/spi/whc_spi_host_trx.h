#ifndef __INIC_SPI_HOST_TRX_H__
#define __INIC_SPI_HOST_TRX_H__
/* -------------------------------- Includes -------------------------------- */
/* external head files */
#include "rtw_wifi_common.h"

/* -------------------------------- Defines --------------------------------- */
/* -------------------------------- Macros ---------------------------------- */
/* ------------------------------- Data Types ------------------------------- */

/* ---------------------------- Global Variables ---------------------------- */

/* -------------------------- Function declaration -------------------------- */
int inic_host_send(int idx, struct eth_drv_sg *sg_list, int sg_len, int total_len, struct skb_raw_para *raw_para, u8 is_special_pkt);


#endif /* __INIC_IPC_HOST_TRX_H__ */


