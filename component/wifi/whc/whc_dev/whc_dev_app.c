
#include "whc_dev.h"
#include "lwip/sys.h"
#include "lwip_netconf.h"
#include "os_wrapper.h"

#define BRIDGE_WIFI_TEST 0xffa5a5a5

#define BRIDGE_WIFI_TEST_GET_MAC_ADDR 0x1
#define BRIDGE_WIFI_TEST_GET_IP       0x2
#define BRIDGE_WIFI_TEST_SET_READY    0x3
#define BRIDGE_WIFI_TEST_SET_UNREADY    0x4
#define BRIDGE_WIFI_TEST_SET_TICKPS_CMD    0x5

#define BRIDGE_WIFI_TEST_BUF_SIZE     16

#define WHC_BRIDGE_WIFI_USER_TASK_STACK_SIZE		4096
#define CONFIG_WHC_BRIDGE_WIFI_USER_TASK_PRIO 		3

rtos_sema_t whc_bridge_user_rx_sema;
u8 *whc_bridge_rx_msg = NULL;
u16 rx_msg_size;

/* here in sdio rx done callback */
__weak void whc_bridge_dev_pkt_rx_to_user(u8 *rxbuf, u16 size)
{
	while (whc_bridge_rx_msg) {
		/* waiting last msg done */
		rtos_time_delay_ms(1);
	}

	whc_bridge_rx_msg = rxbuf;
	rx_msg_size = size;
	rtos_sema_give(whc_bridge_user_rx_sema);
}

__weak void whc_bridge_dev_pkt_rx_to_user_task(void)
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
			}
			whc_bridge_rx_msg = NULL;
		}
	}
}

__weak void whc_bridge_dev_init_user_task(void)
{
	/* initialize the semaphores */
	rtos_sema_create(&whc_bridge_user_rx_sema, 0, 0xFFFFFFFF);

	/* Initialize the event task */
	if (RTK_SUCCESS != rtos_task_create(NULL, (const char *const)"whc_bridge_dev_pkt_rx_to_user_task", (rtos_task_function_t)whc_bridge_dev_pkt_rx_to_user_task,
										NULL,
										WHC_BRIDGE_WIFI_USER_TASK_STACK_SIZE, CONFIG_WHC_BRIDGE_WIFI_USER_TASK_PRIO)) {
		RTK_LOGE(TAG_WLAN_INIC, "Create whc_bridge_dev_pkt_rx_to_user_task Err!!\n");
	}
}