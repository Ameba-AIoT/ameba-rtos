#ifndef SDN_COEX_INTF_H
#define SDN_COEX_INTF_H

#ifndef BT_LE_BUSY_CONN_INTERVAL
#define BT_LE_BUSY_CONN_INTERVAL    0x10    //20ms, units: 1.25ms
#endif

#define COEX_SDN_MSG_NUM    8
#define COEX_SDN_BUF_SIZE   9

#define MSG_DIR_B2W         (1)
#define MSG_DIR_W2B         (2)


void sdn_coex_b2w_send_to_task(uint8_t plen, void *param);
void sdn_coex_w2b_send_to_task(uint8_t plen, void *param);
void sdn_coex_msg_parse(struct sdn_intf_data_msg *pmsg);
#endif