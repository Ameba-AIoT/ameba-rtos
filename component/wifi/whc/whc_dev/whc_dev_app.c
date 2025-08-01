
#include "whc_dev.h"
#include "lwip/sys.h"
#include "lwip_netconf.h"
#include "os_wrapper.h"

#define WHC_CMD_TEST 0xffa5a5a5
#define WHC_ATCMD_TEST 0xeea5a5a5

#define WHC_CMD_TEST_GET_MAC_ADDR   0x1
#define WHC_CMD_TEST_GET_IP         0x2
#define WHC_CMD_TEST_SET_READY      0x3
#define WHC_CMD_TEST_SET_UNREADY    0x4
#define WHC_CMD_TEST_SET_TICKPS_CMD 0x5
#define WHC_CMD_TEST_CONNECT        0x6
#define WHC_CMD_TEST_SCAN           0x7
#define WHC_CMD_TEST_DHCP           0x8
#define WHC_CMD_TEST_WIFION         0x9

#define WHC_CMD_TEST_BUF_SIZE     16

#define WHC_WHC_CMD_USER_TASK_STACK_SIZE		4096
#define CONFIG_WHC_WHC_CMD_USER_TASK_PRIO 		3

rtos_sema_t whc_user_rx_sema;
u8 *whc_rx_msg = NULL;
/* spi add header before msg, different from others */
/* real addr needed for mem free */
u8 *whc_rx_msg_free_addr = NULL;

u16 rx_msg_size;
static struct rtw_network_info wifi = {0};

#if (defined CONFIG_WHC_HOST || defined CONFIG_WHC_NONE) && defined(CONFIG_SUPPORT_ATCMD)
rtos_sema_t whc_atcmd_rx_sema;

extern volatile UART_LOG_CTL shell_ctl;
extern UART_LOG_BUF shell_rxbuf;

#ifdef CONFIG_ATCMD_HOST_CONTROL
typedef void (*at_write)(char *buf, int len);
extern at_write out_buffer;
#endif
#endif

#define at_printf(fmt, args...)    RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, fmt, ##args)

__weak int whc_dev_ip_in_table_indicate(u8 gate, u8 ip)
{
	(void)gate;
	(void)ip;

	//return 1 to forward all pkt now.
	return 1;
	//todo
}

void whc_dev_cmd_scan(void)
{
	struct rtw_scan_param *scan_param;
	scan_param = rtos_mem_zmalloc(sizeof(struct rtw_scan_param));
	struct rtw_scan_result *scanned_AP_info;
	int ret;
	u32 scanned_AP_num, i;
	struct rtw_scan_result *scanned_AP_list = NULL;

	if (scan_param == NULL) {
		RTK_LOGE(TAG_WLAN_INIC, "%s mem fail!\n", __func__);
		return;
	}
	ret = wifi_scan_networks(scan_param, 1);

	if (ret < RTK_SUCCESS) {
		RTK_LOGE(TAG_WLAN_INIC, " wifi_scan_networks ERROR!\n");
		goto end;
	}

	/* get scan results and log them */
	scanned_AP_num = ret;

	if (scanned_AP_num != 0) {
		scanned_AP_list = (struct rtw_scan_result *)rtos_mem_zmalloc(scanned_AP_num * sizeof(struct rtw_scan_result));
		if (scanned_AP_list == NULL) {
			RTK_LOGE(TAG_WLAN_INIC, "%s mem fail!\n", __func__);
			goto end;
		}

		if (wifi_get_scan_records(&scanned_AP_num, scanned_AP_list) < 0) {
			RTK_LOGE(TAG_WLAN_INIC, "Get result failed\r\n");
			goto end;
		}

		for (i = 0; i < scanned_AP_num; i++) {
			at_printf("%2d, ", (i + 1));

			scanned_AP_info = &scanned_AP_list[i];
			scanned_AP_info->ssid.val[scanned_AP_info->ssid.len] = 0; /* Ensure the SSID is null terminated */

			//print_scan_result(scanned_AP_info);
		}
	}

end:
	if (scan_param) {
		rtos_mem_free(scan_param);
	}

	if (scanned_AP_list) {
		rtos_mem_free(scanned_AP_list);
	}
}

/* here in sdio rx done callback */
__weak void whc_dev_pkt_rx_to_user(u8 *rxbuf, u8 *real_buf, u16 size)
{
	while (whc_rx_msg) {
		/* waiting last msg done */
		rtos_time_delay_ms(1);
	}

	whc_rx_msg = rxbuf;
	whc_rx_msg_free_addr = real_buf;
	rx_msg_size = size;
	rtos_sema_give(whc_user_rx_sema);
}

__weak void whc_dev_pkt_rx_to_user_task(void)
{
	u8 *ptr = NULL;
	u32 event = 0;
	u8 *ip, *buf, idx = 0;
	(void)ip;
	u8 len;
	char *password = NULL;
	int ret;
	(void)ret;

	while (1) {
		rtos_sema_take(whc_user_rx_sema, RTOS_MAX_TIMEOUT);
		if (whc_rx_msg) {
			ptr = whc_rx_msg;
			event = *(u32 *)whc_rx_msg;
			ptr += 4;

			if (event == WHC_CMD_TEST) {
				buf = rtos_mem_malloc(WHC_CMD_TEST_BUF_SIZE);
				if (!buf) {
					RTK_LOGE(TAG_WLAN_INIC, "%s, can't alloc buffer!!\n", __func__);
					return;
				}
				if (*ptr == WHC_CMD_TEST_GET_MAC_ADDR) {
					struct rtw_mac dev_mac = {0};
					idx = *(ptr + 1);
					if (!wifi_is_running(idx)) {
						RTK_LOGE(TAG_WLAN_INIC, "%s, port %d is not running!\n", __func__, idx);
						rtos_mem_free(buf);
						return;
					}
					wifi_get_mac_address(idx, &dev_mac, 0);
					ptr = buf;
					*(u32 *)ptr = WHC_CMD_TEST;
					ptr += 4;
					*ptr = WHC_CMD_TEST_GET_MAC_ADDR;
					ptr += 1;
					memcpy(ptr, dev_mac.octet, 6);
					//6+4+1=11
					whc_dev_api_send_to_host(buf, WHC_CMD_TEST_BUF_SIZE);
				} else if (*ptr == WHC_CMD_TEST_SCAN) {
					whc_dev_cmd_scan();
#ifdef CONFIG_LWIP_LAYER
				} else if (*ptr == WHC_CMD_TEST_DHCP) {
					LwIP_netif_set_link_up(STA_WLAN_INDEX);
					/* Start DHCPClient */
					LwIP_DHCP(STA_WLAN_INDEX, DHCP_START);
#endif
				} else if (*ptr == WHC_CMD_TEST_CONNECT) {
					memset(&wifi, 0, sizeof(struct rtw_network_info));
					ptr += 1;
					len = *ptr;
					wifi.ssid.len = len;
					memcpy((char *)wifi.ssid.val, ptr + 1, len);

					ptr += len + 1;
					len = *ptr;
					if (len > 0) {
						password = rtos_mem_zmalloc(129);
						memcpy((char *)password, ptr + 1, len);
						wifi.password = (u8 *)password;
						wifi.password_len = len;
					}
					ret = wifi_connect(&wifi, 1);

					if (password) {
						rtos_mem_free(password);
					}
#ifdef CONFIG_LWIP_LAYER
					if (ret == RTK_SUCCESS) {
						/* Start DHCPClient */
						LwIP_DHCP(0, DHCP_START);
					} else {
						RTK_LOGE(TAG_WLAN_INIC, "connect fail !\n");
					}
#endif

#ifdef CONFIG_LWIP_LAYER
				} else if (*ptr == WHC_CMD_TEST_GET_IP) {
					idx = *(ptr + 1);
					if (!wifi_is_running(idx)) {
						RTK_LOGE(TAG_WLAN_INIC, "%s, port %d is not running!\n", __func__, idx);
						rtos_mem_free(buf);
						return;
					}
					ip = LwIP_GetIP(idx);
					ptr = buf;
					*(u32 *)ptr = WHC_CMD_TEST;
					ptr += 4;
					*ptr = WHC_CMD_TEST_GET_IP;
					ptr += 1;
					memcpy(ptr, ip, 4);

					whc_dev_api_send_to_host(buf, WHC_CMD_TEST_BUF_SIZE);
#endif
#ifdef CONFIG_WHC_DUAL_TCPIP
				} else if (*ptr == WHC_CMD_TEST_SET_READY) {
					whc_dev_api_set_host_state(1);
				} else if (*ptr == WHC_CMD_TEST_SET_UNREADY) {
					whc_dev_api_set_host_state(0);
#endif
				} else if (*ptr == WHC_CMD_TEST_SET_TICKPS_CMD) {
					u8 subtype = *(ptr + 1);
					whc_dev_api_set_tickps_cmd(subtype);
				} else if (*ptr == WHC_CMD_TEST_WIFION) {
#ifdef CONFIG_WHC_DUAL_TCPIP
					whc_dev_api_set_host_state(WHC_HOST_READY);
#endif
					wifi_on(RTW_MODE_STA);
				}
				rtos_mem_free(buf);
#if (defined CONFIG_WHC_HOST || defined CONFIG_WHC_NONE) && defined(CONFIG_SUPPORT_ATCMD)
			} else if (event == WHC_ATCMD_TEST) {
				rtos_sema_give(whc_atcmd_rx_sema);
#endif
			}
			rtos_mem_free(whc_rx_msg_free_addr);
			whc_rx_msg = NULL;
			whc_rx_msg_free_addr = NULL;
		}
	}
}

#if (defined CONFIG_WHC_HOST || defined CONFIG_WHC_NONE) && defined(CONFIG_SUPPORT_ATCMD)
void whc_atcmd_task(void)
{
	PUART_LOG_BUF pShellRxBuf = &shell_rxbuf;
	u32 i = 0;
	while (1) {
		pShellRxBuf->BufCount = 0;
		i = 0;

		rtos_sema_take(whc_atcmd_rx_sema, 0xFFFFFFFF);
		memcpy(pShellRxBuf->UARTLogBuf, whc_rx_msg + 4, rx_msg_size - 4);
		pShellRxBuf->BufCount = rx_msg_size - 4;

recv_again:
		if (shell_cmd_chk(pShellRxBuf->UARTLogBuf[i++], (UART_LOG_CTL *)&shell_ctl, ENABLE) == 2) {
			if (shell_ctl.pTmpLogBuf != NULL) {
				shell_ctl.ExecuteCmd = TRUE;

				if (shell_ctl.shell_task_rdy) {
					shell_ctl.GiveSema();
				}
			} else {
				shell_array_init((u8 *)shell_ctl.pTmpLogBuf->UARTLogBuf, UART_LOG_CMD_BUFLEN, '\0');
			}
		}

		/* recv all data one time */
		if ((pShellRxBuf->BufCount != i) && (pShellRxBuf->BufCount != 0)) {
			goto recv_again;
		}
	}
}

void whc_dev_api_send_to_host_wrap(char *buf, int len)
{
	u8 *message = rtos_mem_malloc(len + 6);
	u8 *ptr = message;
	*(u32 *)ptr = WHC_ATCMD_TEST;
	ptr += 4;
	*(u16 *)ptr  = (u16)len;
	ptr += 2;
	memcpy(ptr, buf, len);
	whc_dev_api_send_to_host((u8 *)message, (u32)len + 6);
	rtos_mem_free(message);
}
#endif

__weak void whc_dev_init_cmd_path_task(void)
{
	/* initialize the semaphores */
	rtos_sema_create(&whc_user_rx_sema, 0, 0xFFFFFFFF);

	/* Initialize the event task */
	if (RTK_SUCCESS != rtos_task_create(NULL, (const char *const)"whc_dev_pkt_rx_to_user_task", (rtos_task_function_t)whc_dev_pkt_rx_to_user_task,
										NULL,
										WHC_WHC_CMD_USER_TASK_STACK_SIZE, CONFIG_WHC_WHC_CMD_USER_TASK_PRIO)) {
		RTK_LOGE(TAG_WLAN_INIC, "Create whc_dev_pkt_rx_to_user_task Err!!\n");
	}

#if (defined CONFIG_WHC_HOST || defined CONFIG_WHC_NONE) && defined(CONFIG_SUPPORT_ATCMD)
	rtos_sema_create(&whc_atcmd_rx_sema, 0, 0xFFFFFFFF);

#ifdef CONFIG_ATCMD_HOST_CONTROL
	out_buffer = whc_dev_api_send_to_host_wrap;
#endif

	if (rtos_task_create(NULL, ((const char *)"whc_atcmd_task"), (rtos_task_t)whc_atcmd_task, NULL, 1024, 5) != RTK_SUCCESS) {
		RTK_LOGE(TAG_WLAN_INIC, "Create whc_atcmd_task Err!!\n");
	}
#endif
}
