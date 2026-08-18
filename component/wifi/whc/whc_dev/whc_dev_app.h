#ifndef _WHC_DEV_APP_H_
#define _WHC_DEV_APP_H_

#define WHC_WIFI_TEST_BUF_SIZE     32

#define WHC_WHC_CMD_USER_TASK_STACK_SIZE		4096
#define CONFIG_WHC_WHC_CMD_USER_TASK_PRIO 		3

/* In WHC_DEV mode at_printf routes AT responses back to the host via the AT_RESP channel. */
#if !(!defined (CONFIG_WHC_INTF_IPC) && defined (CONFIG_WHC_DEV))
#define at_printf(fmt, args...)    RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, fmt, ##args)
#endif

struct whc_cmd_path_priv {
	rtos_sema_t whc_user_rx_sema;
	rtos_mutex_t whc_user_blocksend_mutex;
	rtos_sema_t whc_user_blocksend_sema;
	u8 *whc_rx_msg;

	u8 *ret;
	u32 ret_len;
};

extern struct whc_cmd_path_priv whc_cmdpath_data;

int whc_dev_get_lwip_info(u32 type, unsigned char *input, int index);

#endif
