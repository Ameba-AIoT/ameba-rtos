#ifndef __WHC_HOST_APP_H__
#define __WHC_HOST_APP_H__

void whc_host_deliver_rxbuf_to_user(u8 *payload, u32 len);
void whc_host_send_cmd_data_to_dev(u8 *buf, u32 len);
void whc_host_update_network_info(u8 wlan_idx);

#endif /* __WHC_HOST_APP_H__ */
