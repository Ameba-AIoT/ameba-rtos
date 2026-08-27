#ifndef __WHC_HOST_APP_H__
#define __WHC_HOST_APP_H__

#include "os_wrapper.h"

#define WHC_HOST_CMD_USER_TASK_STACK_SIZE   1200
#define CONFIG_WHC_HOST_CMD_USER_TASK_PRIO  3

struct whc_host_cmd_path_priv {
	rtos_sema_t whc_user_rx_sema;
	u8 *whc_rx_buf;       /* original buffer (to be freed after processing) */
};

extern struct whc_host_cmd_path_priv whc_host_cmdpath_data;

void whc_host_deliver_rxbuf_to_user(u8 *buf, u32 buf_size);
void whc_host_send_cmd_data_to_dev(u8 *buf, u32 len);
void whc_host_update_network_info(u8 wlan_idx);
void whc_host_cmd_path_init(void);

#endif /* __WHC_HOST_APP_H__ */
