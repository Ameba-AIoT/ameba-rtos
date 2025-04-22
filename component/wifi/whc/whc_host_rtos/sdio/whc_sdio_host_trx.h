#ifndef __WHC_SDIO_HOST_TRX_H__
#define __WHC_SDIO_HOST_TRX_H__
/* -------------------------------- Includes -------------------------------- */
/* external head files */
#include "rtw_wifi_common.h"
#include "rtw_skbuff.h"
/* -------------------------------- Defines --------------------------------- */
#define whc_host_send           whc_sdio_host_send
#define whc_bridge_host_send_to_dev           whc_bridge_sdio_host_send_to_dev

/* -------------------------------- Macros ---------------------------------- */
/* ------------------------------- Data Types ------------------------------- */

/* ---------------------------- Global Variables ---------------------------- */

/* -------------------------- Function declaration -------------------------- */
int whc_sdio_host_send(int idx, struct eth_drv_sg *sg_list, int sg_len, int total_len, struct skb_raw_para *raw_para, u8 is_special_pkt);
void whc_bridge_sdio_host_send_to_dev(u8 *buf, u32 len);

#endif /* __WHC_SDIO_HOST_TRX_H__ */


