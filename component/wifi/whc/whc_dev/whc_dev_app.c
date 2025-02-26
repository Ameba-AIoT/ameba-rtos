
#include "whc_dev.h"
#include "lwip/sys.h"
#include "lwip_netconf.h"

#define BRIDGE_WIFI_TEST 0xffa5a5a5

#define BRIDGE_WIFI_TEST_GET_MAC_ADDR 0x1
#define BRIDGE_WIFI_TEST_GET_IP       0x2
#define BRIDGE_WIFI_TEST_SET_READY    0x3

#define BRIDGE_WIFI_TEST_BUF_SIZE     16

__weak void whc_bridge_dev_pkt_rx_to_user(u8 *rxbuf, u16 size)
{
	(void)size;
	u8 *ptr = rxbuf;
	u32 event = *(u32 *)rxbuf;
	u8 *ip, *buf;

	ptr += 4;

	if (event == BRIDGE_WIFI_TEST) {
		buf = rtos_mem_malloc(BRIDGE_WIFI_TEST_BUF_SIZE);
		if (!buf) {
			RTK_LOGE(TAG_WLAN_INIC, "%s, can't alloc buffer!!\n", __func__);
			return;
		}
		if (*ptr == BRIDGE_WIFI_TEST_GET_MAC_ADDR) {
			struct _rtw_mac_t dev_mac = {0};
			wifi_get_mac_address(STA_WLAN_INDEX, &dev_mac, 0);
			ptr = buf;
			*(u32 *)ptr = BRIDGE_WIFI_TEST;
			ptr += 4;
			*ptr = BRIDGE_WIFI_TEST_GET_MAC_ADDR;
			ptr += 1;
			memcpy(ptr, dev_mac.octet, 6);
			//6+4+1=11
			whc_bridge_dev_api_send_to_host(buf, BRIDGE_WIFI_TEST_BUF_SIZE);
		} else if (*ptr == BRIDGE_WIFI_TEST_GET_IP) {
			ip = LwIP_GetIP(0);
			ptr = buf;
			*(u32 *)ptr = BRIDGE_WIFI_TEST;
			ptr += 4;
			*ptr = BRIDGE_WIFI_TEST_GET_IP;
			ptr += 1;
			memcpy(ptr, ip, 4);

			whc_bridge_dev_api_send_to_host(buf, BRIDGE_WIFI_TEST_BUF_SIZE);
		} else if (*ptr == BRIDGE_WIFI_TEST_SET_READY) {
			whc_bridge_dev_api_set_host_state(1);
		}

		rtos_mem_free(buf);
	}

	rtos_mem_free(rxbuf);
}

