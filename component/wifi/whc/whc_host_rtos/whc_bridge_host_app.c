#include "whc_host.h"
#include "os_wrapper.h"
#include "atcmd_service.h"

__weak void whc_bridge_host_pkt_rx_to_user(u8 *payload, u32 len)
{
	(void)len;
	u32 event = *(u32 *)payload;
	u8 *ptr = payload;
	u8 id;
	u32 ipaddr, netmask, gw;

	if (event == WHC_WIFI_TEST) {
		ptr += 4;
		id = *ptr;
		ptr += 1;
		switch (id) {
		case WHC_WIFI_TEST_GET_MAC_ADDR:
			LwIP_wlan_set_netif_info(STA_WLAN_INDEX, NULL, ptr);
			break;

		case WHC_WIFI_TEST_GET_IP:
			ipaddr = CONCAT_TO_UINT32(ptr[0], ptr[1], ptr[2], ptr[3]);
			netmask = CONCAT_TO_UINT32(NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
			gw = CONCAT_TO_UINT32(ptr[0], ptr[1], ptr[2], 1);
			LwIP_SetIP(STA_WLAN_INDEX, ipaddr, netmask, gw);
			LwIP_netif_set_link_up(0);
			break;
		default:
			break;
		}


	}
}

void whc_host_get_mac_addr(uint8_t idx)
{
	uint8_t buf[12] = {0};
	uint8_t *ptr = buf;
	uint32_t buf_len = 0;

	*(uint32_t *)ptr = WHC_WIFI_TEST;
	ptr += 4;
	buf_len += 4;
	*ptr = WHC_WIFI_TEST_GET_MAC_ADDR;
	ptr += 1;
	buf_len += 1;
	*ptr = idx;
	ptr += 1;
	buf_len += 1;

	whc_bridge_host_send_to_dev(buf, buf_len);
}

void whc_host_get_ip(uint8_t idx)
{
	uint8_t buf[12] = {0};
	uint8_t *ptr = buf;
	uint32_t buf_len = 0;

	*(uint32_t *)ptr = WHC_WIFI_TEST;
	ptr += 4;
	buf_len += 4;
	*ptr = WHC_WIFI_TEST_GET_IP;
	ptr += 1;
	buf_len += 1;
	*ptr = idx;
	ptr += 1;
	buf_len += 1;

	whc_bridge_host_send_to_dev(buf, buf_len);
}

void whc_bridge_host_set_rdy(void)
{
	uint8_t buf[12] = {0};
	uint8_t *ptr = buf;
	uint32_t buf_len = 0;

	*(uint32_t *)ptr = WHC_WIFI_TEST;
	ptr += 4;
	buf_len += 4;
	*ptr = WHC_WIFI_TEST_SET_READY;
	ptr += 1;
	buf_len += 1;

	whc_bridge_host_send_to_dev(buf, buf_len);
}

u32 cmd_bridge_test(u16 argc, u8  *argv[])
{
	UNUSED(argc);

	if (_strcmp((const char *)argv[0], "getmac") == 0) {
		whc_host_get_mac_addr(0);
	}

	if (_strcmp((const char *)argv[0], "getip") == 0) {
		whc_host_get_ip(0);
	}

	if (_strcmp((const char *)argv[0], "setrdy") == 0) {
		LwIP_netif_set_up(0);
		whc_bridge_host_set_rdy();
	}
	return 0;
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE   spi_bridge_test_cmd_table[] = {
	{
		(const u8 *)"BRIDGE", 8, cmd_bridge_test, (const u8 *)"\tBRIDGE \n"
		"\t\t BRIDGE Test \n"
	},
};