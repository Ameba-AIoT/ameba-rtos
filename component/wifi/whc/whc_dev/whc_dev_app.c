
#include "whc_dev.h"
#include "lwip/sys.h"
#include "lwip_netconf.h"
#include "os_wrapper.h"

#define BRIDGE_WIFI_TEST 0xffa5a5a5
#define BRIDGE_ATCMD_TEST 0xeea5a5a5

#define BRIDGE_WIFI_TEST_GET_MAC_ADDR 0x1
#define BRIDGE_WIFI_TEST_GET_IP       0x2
#define BRIDGE_WIFI_TEST_SET_READY    0x3
#define BRIDGE_WIFI_TEST_SET_UNREADY    0x4
#define BRIDGE_WIFI_TEST_SET_TICKPS_CMD    0x5

#define BRIDGE_WIFI_TEST_BUF_SIZE     16

#define WHC_BRIDGE_WIFI_USER_TASK_STACK_SIZE		4096
#define CONFIG_WHC_BRIDGE_WIFI_USER_TASK_PRIO 		3

rtos_sema_t whc_bridge_user_rx_sema;
rtos_sema_t whc_bridge_atcmd_rx_sema;
u8 *whc_bridge_rx_msg = NULL;
/* spi add header before msg, different from others */
/* real addr needed for mem free */
u8 *whc_bridge_rx_msg_free_addr = NULL;

u16 rx_msg_size;

extern volatile UART_LOG_CTL shell_ctl;
extern UART_LOG_BUF shell_rxbuf;

typedef void (*at_write)(char *buf, int len);
extern at_write out_buffer;

/* here in sdio rx done callback */
__weak void whc_dev_pkt_rx_to_user(u8 *rxbuf, u8 *real_buf, u16 size)
{
	while (whc_bridge_rx_msg) {
		/* waiting last msg done */
		rtos_time_delay_ms(1);
	}

	whc_bridge_rx_msg = rxbuf;
	whc_bridge_rx_msg_free_addr = real_buf;
	rx_msg_size = size;
	rtos_sema_give(whc_bridge_user_rx_sema);
}

__weak void whc_dev_pkt_rx_to_user_task(void)
{
	u8 *ptr = NULL;
	u32 event = 0;
	u8 *ip, *buf, idx = 0;

	while (1) {
		rtos_sema_take(whc_bridge_user_rx_sema, RTOS_MAX_TIMEOUT);
		if (whc_bridge_rx_msg) {
			ptr = whc_bridge_rx_msg;
			event = *(u32 *)whc_bridge_rx_msg;
			ptr += 4;

			if (event == BRIDGE_WIFI_TEST) {
				buf = rtos_mem_malloc(BRIDGE_WIFI_TEST_BUF_SIZE);
				if (!buf) {
					RTK_LOGE(TAG_WLAN_INIC, "%s, can't alloc buffer!!\n", __func__);
					return;
				}
				if (*ptr == BRIDGE_WIFI_TEST_GET_MAC_ADDR) {
					struct rtw_mac dev_mac = {0};
					idx = *(ptr + 1);
					if (!wifi_is_running(idx)) {
						RTK_LOGE(TAG_WLAN_INIC, "%s, port %d is not running!\n", __func__, idx);
						rtos_mem_free(buf);
						return;
					}
					wifi_get_mac_address(idx, &dev_mac, 0);
					ptr = buf;
					*(u32 *)ptr = BRIDGE_WIFI_TEST;
					ptr += 4;
					*ptr = BRIDGE_WIFI_TEST_GET_MAC_ADDR;
					ptr += 1;
					memcpy(ptr, dev_mac.octet, 6);
					//6+4+1=11
					whc_bridge_dev_api_send_to_host(buf, BRIDGE_WIFI_TEST_BUF_SIZE);
				} else if (*ptr == BRIDGE_WIFI_TEST_GET_IP) {
					idx = *(ptr + 1);
					if (!wifi_is_running(idx)) {
						RTK_LOGE(TAG_WLAN_INIC, "%s, port %d is not running!\n", __func__, idx);
						rtos_mem_free(buf);
						return;
					}
					ip = LwIP_GetIP(idx);
					ptr = buf;
					*(u32 *)ptr = BRIDGE_WIFI_TEST;
					ptr += 4;
					*ptr = BRIDGE_WIFI_TEST_GET_IP;
					ptr += 1;
					memcpy(ptr, ip, 4);

					whc_bridge_dev_api_send_to_host(buf, BRIDGE_WIFI_TEST_BUF_SIZE);
				} else if (*ptr == BRIDGE_WIFI_TEST_SET_READY) {
					whc_bridge_dev_api_set_host_state(1);
				} else if (*ptr == BRIDGE_WIFI_TEST_SET_UNREADY) {
					whc_bridge_dev_api_set_host_state(0);
				} else if (*ptr == BRIDGE_WIFI_TEST_SET_TICKPS_CMD) {
					u8 subtype = *(ptr + 1);
					whc_bridge_dev_api_set_tickps_cmd(subtype);
				}
				rtos_mem_free(buf);
			} else if (event == BRIDGE_ATCMD_TEST) {
				rtos_sema_give(whc_bridge_atcmd_rx_sema);
			}
			rtos_mem_free(whc_bridge_rx_msg_free_addr);
			whc_bridge_rx_msg = NULL;
			whc_bridge_rx_msg_free_addr = NULL;
		}
	}
}

void whc_bridge_atcmd_task(void)
{
	PUART_LOG_BUF pShellRxBuf = &shell_rxbuf;
	u32 i = 0;
	while (1) {
		pShellRxBuf->BufCount = 0;
		i = 0;

		rtos_sema_take(whc_bridge_atcmd_rx_sema, 0xFFFFFFFF);
		memcpy(pShellRxBuf->UARTLogBuf, whc_bridge_rx_msg + 4, rx_msg_size - 4);
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

void whc_bridge_dev_api_send_to_host_wrap(char *buf, int len)
{
	u8 *message = rtos_mem_malloc(len + 6);
	u8 *ptr = message;
	*(u32 *)ptr = BRIDGE_ATCMD_TEST;
	ptr += 4;
	*(u16 *)ptr  = (u16)len;
	ptr += 2;
	memcpy(ptr, buf, len);
	whc_bridge_dev_api_send_to_host((u8 *)message, (u32)len + 6);
	rtos_mem_free(message);
}

__weak void whc_dev_init_cmd_path_task(void)
{
	/* initialize the semaphores */
	rtos_sema_create(&whc_bridge_user_rx_sema, 0, 0xFFFFFFFF);
	rtos_sema_create(&whc_bridge_atcmd_rx_sema, 0, 0xFFFFFFFF);

	out_buffer = whc_bridge_dev_api_send_to_host_wrap;

	/* Initialize the event task */
	if (RTK_SUCCESS != rtos_task_create(NULL, (const char *const)"whc_dev_pkt_rx_to_user_task", (rtos_task_function_t)whc_dev_pkt_rx_to_user_task,
										NULL,
										WHC_BRIDGE_WIFI_USER_TASK_STACK_SIZE, CONFIG_WHC_BRIDGE_WIFI_USER_TASK_PRIO)) {
		RTK_LOGE(TAG_WLAN_INIC, "Create whc_dev_pkt_rx_to_user_task Err!!\n");
	}

	if (rtos_task_create(NULL, ((const char *)"whc_bridge_atcmd_task"), (rtos_task_t)whc_bridge_atcmd_task, NULL, 1024, 5) != RTK_SUCCESS) {
		RTK_LOGE(TAG_WLAN_INIC, "Create whc_bridge_atcmd_task Err!!\n");
	}
}