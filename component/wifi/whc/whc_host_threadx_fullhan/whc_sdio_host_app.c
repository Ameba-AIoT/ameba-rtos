#include "rtw_whc_common.h"

#ifdef CONFIG_WHC_WIFI_API_PATH
struct event_priv_t event_priv;
#endif

int scan_ap_cnt;
char *whc_host_scan_result_list;
extern struct whc_sdio whc_sdio_priv;

#define SCAN_MAX_NUM	64

/* for test demo */
#define WHC_WIFI_TEST 0xffa5a5a5

#define WHC_WIFI_TEST_GET_MAC_ADDR   0x1
#define WHC_WIFI_TEST_GET_IP         0x2
#define WHC_WIFI_TEST_SET_READY      0x3
#define WHC_WIFI_TEST_SET_UNREADY    0x4
#define WHC_WIFI_TEST_SET_TICKPS_CMD 0x5
#define WHC_WIFI_TEST_CONNECT        0x6
#define WHC_WIFI_TEST_SCAN           0x7
#define WHC_WIFI_TEST_DHCP           0x8
#define WHC_WIFI_TEST_WIFION         0x9
#define WHC_WIFI_TEST_SCAN_RESULT    0xA
#define WHC_WIFI_TEST_DISCONN        0x10
#define WHC_WIFI_TEST_SOFTAP         0x11
#define WHC_WIFI_TEST_CONN_STATUS    0x12

/* for rtos host only */
#define WHC_WIFI_TEST_SET_HOST_RTOS  0xFF

//#define rtos_mem_zmalloc(n)		rt_malloc(n)
#define rtos_mem_free(ptr)		rt_free(ptr)

int whc_host_wifi_get_scanresult(WIFI_SCAN_RESULT_t *p_scan_result, int ap_cnt)
{
	if (whc_host_scan_result_list) {
		memcpy((char *)p_scan_result, (char *)whc_host_scan_result_list, ap_cnt * sizeof(WIFI_SCAN_RESULT_t));
		rt_free(whc_host_scan_result_list);
		whc_host_scan_result_list = NULL;
		return 0;
	} else {
		printf("error %s \r\n", __func__);
		return 1;
	}
}

void whc_host_scan_result(uint8_t *buf)
{
	uint32_t idx = buf[1];
	uint8_t *ptr = (buf + 4);
	uint8_t len;
	WIFI_SCAN_RESULT_t *ap_info;

	if (idx == 0) {
		if (whc_sdio_priv.host_send_block_sema && whc_sdio_priv.ret) {
			memcpy(whc_sdio_priv.ret, (void *)&scan_ap_cnt, whc_sdio_priv.ret_len);
			rtos_sema_give(whc_sdio_priv.host_send_block_sema);
		}
		return;
	} else {
		if (scan_ap_cnt >= SCAN_MAX_NUM) {
			return;
		}

		ap_info = (WIFI_SCAN_RESULT_t *)(whc_host_scan_result_list + scan_ap_cnt * sizeof(WIFI_SCAN_RESULT_t));
		//bssid
		memcpy(ap_info->bssid, ptr, 6);
		ptr += 6;

		//rssi
		memcpy(&(ap_info->signal_level), ptr, 4);
		ptr += 4;

		//chn
		memcpy(&(ap_info->channel), ptr, 4);
		ptr += 4;

		//security
		memcpy(&(ap_info->security), ptr, 4);
		ptr += 4;

		//ssid
		len = *ptr;
		ptr += 1;
		if (len > WIFI_SSID_MAX) {
			printf("error len for ssid %d:%d \r\n", WIFI_SSID_MAX, len);
			len = WIFI_SSID_MAX;
		}
		memcpy(ap_info->ssid, ptr, len);
		scan_ap_cnt ++;
	}

}

void whc_host_set_connect_status(u8 *buf)
{
	//g_wifi_status = *buf;
	int status = (int)(*buf);

	if (whc_sdio_priv.host_send_block_sema && whc_sdio_priv.ret) {
		memcpy(whc_sdio_priv.ret, (void *)&status, whc_sdio_priv.ret_len);
		rtos_sema_give(whc_sdio_priv.host_send_block_sema);
	}
}

void whc_host_pkt_rx_to_user(u8 *payload)
{
	//(void)len;
	u32 event = *(u32 *)payload;
	u8 *ptr = payload;
	u8 id;
	u32 ipaddr, netmask, gw;
	uint8_t idx = 0;
	if (event == WHC_WIFI_TEST) {
		ptr += 4;
		id = *ptr;
		ptr += 1;
		switch (id) {
		case WHC_WIFI_TEST_SCAN_RESULT:
			whc_host_scan_result(payload + 4);
			break;
		case WHC_WIFI_TEST_GET_MAC_ADDR:
			printf("!!!!!!!!!!!!!get mac addr\r\n");
			idx = *ptr;
			ptr += 1;
			LwIP_wlan_set_netif_info(idx, NULL, ptr);
			break;
		case WHC_WIFI_TEST_CONN_STATUS:
			whc_host_set_connect_status(payload + 4);
			break;
		case WHC_WIFI_TEST_GET_IP:
			ipaddr = CONCAT_TO_UINT32(ptr[0], ptr[1], ptr[2], ptr[3]);
			netmask = CONCAT_TO_UINT32(255, 255, 255, 0);
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

	whc_sdio_host_send_to_dev(buf, buf_len);
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

	whc_sdio_host_send_to_dev(buf, buf_len);
}

void whc_host_set_rdy(uint8_t state)
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
	*ptr = state;
	buf_len += 1;

	whc_sdio_host_send_to_dev(buf, buf_len);
}

void whc_host_wifi_on(void)
{
	uint8_t buf[12] = {0};
	uint8_t *ptr = buf;
	uint32_t buf_len = 0;
	*(uint32_t *)ptr = WHC_WIFI_TEST;
	ptr += 4;
	buf_len += 4;

	*ptr = WHC_WIFI_TEST_WIFION;
	ptr += 1;
	buf_len += 1;

	whc_sdio_host_send_to_dev(buf, buf_len);
}

void whc_host_get_connet_status(int status)
{
	uint8_t buf[12] = {0};
	uint8_t *ptr = buf;
	uint32_t buf_len = 0;

	//rtos_sema_create(&(whc_host_get_status), 0, 1);

	*(uint32_t *)ptr = WHC_WIFI_TEST;
	ptr += 4;
	buf_len += 4;

	*ptr = WHC_WIFI_TEST_CONN_STATUS;
	ptr += 1;
	buf_len += 1;

	whc_sdio_host_send_to_dev_block(buf, buf_len, (uint8_t *)&status, sizeof(int));
}

void whc_host_dhcp(void)
{
	uint8_t buf[12] = {0};
	uint8_t *ptr = buf;
	uint32_t buf_len = 0;
	*(uint32_t *)ptr = WHC_WIFI_TEST;
	ptr += 4;
	buf_len += 4;

	*ptr = WHC_WIFI_TEST_DHCP;
	ptr += 1;
	buf_len += 1;

	whc_sdio_host_send_to_dev(buf, buf_len);
}

void whc_host_set_host(void)
{
	uint8_t buf[12] = {0};
	uint8_t *ptr = buf;
	uint32_t buf_len = 0;
	*(uint32_t *)ptr = WHC_WIFI_TEST;
	ptr += 4;
	buf_len += 4;

	*ptr = WHC_WIFI_TEST_SET_HOST_RTOS;
	ptr += 1;
	buf_len += 1;

	whc_sdio_host_send_to_dev(buf, buf_len);
}

void whc_host_wifi_scan(int *ap_cnt)
{
	uint8_t buf[12] = {0};
	uint8_t *ptr = buf;
	uint32_t buf_len = 0;
	scan_ap_cnt = 0;

	if (whc_host_scan_result_list) {
		printf("error %s, scan report shoule be empty\r\n", __func__);
		rt_free(whc_host_scan_result_list);
	}

	whc_host_scan_result_list = rt_malloc(SCAN_MAX_NUM * sizeof(WIFI_SCAN_RESULT_t));
	memset(whc_host_scan_result_list, 0, SCAN_MAX_NUM * sizeof(WIFI_SCAN_RESULT_t));

	*(uint32_t *)ptr = WHC_WIFI_TEST;
	ptr += 4;
	buf_len += 4;

	*ptr = WHC_WIFI_TEST_SCAN;
	ptr += 1;
	buf_len += 1;

	whc_sdio_host_send_to_dev_block(buf, buf_len, (uint8_t *)ap_cnt, sizeof(int));
}

void whc_host_wifi_connect(char *ssid, char *pwd)
{
	uint8_t *buf;
	uint8_t *ptr;
	uint32_t buf_len = 0, len = 0;

	buf = rt_malloc(128);
	ptr = buf;

	*(uint32_t *)ptr = WHC_WIFI_TEST;
	ptr += 4;

	*ptr = WHC_WIFI_TEST_CONNECT;
	ptr += 1;

	len = strlen(ssid);
	*ptr = len;
	ptr += 1;

	memcpy(ptr, ssid, len);
	ptr += len;

	if (pwd && (strlen(pwd) > 0)) {
		len = strlen(pwd);
		*ptr = len;
		ptr += 1;

		memcpy(ptr, pwd, len);
		ptr += len;

	} else {
		*ptr = 0;
		ptr += 1;
	}

	buf_len = ptr - buf;

	whc_sdio_host_send_to_dev(buf, buf_len);

	rtos_mem_free(buf);
}

void whc_host_wifi_disconnect(void)
{
	uint8_t buf[12] = {0};
	uint8_t *ptr = buf;
	uint32_t buf_len = 0;

	*(uint32_t *)ptr = WHC_WIFI_TEST;
	ptr += 4;
	buf_len += 4;

	*ptr = WHC_WIFI_TEST_DISCONN;
	ptr += 1;
	buf_len += 1;

	whc_sdio_host_send_to_dev(buf, buf_len);
}

/**
* @brief  enable/disable ap mode
* @param  state: 0: disable, 1: enable
* @return none.
*/
void whc_host_wifi_stop_ap(void)
{
	uint8_t buf[12] = {0};
	uint8_t *ptr = buf;
	uint32_t buf_len = 0;

	*(uint32_t *)ptr = WHC_WIFI_TEST;
	ptr += 4;
	buf_len += 4;

	*ptr = WHC_WIFI_TEST_SOFTAP;
	ptr += 1;
	buf_len += 1;

	*ptr = 0;
	ptr += 1;
	buf_len += 1;

	whc_sdio_host_send_to_dev(buf, buf_len);
}


int whc_host_wifi_enable_ap(unsigned char *ssid, char *psk, int chn, unsigned int ip)
{
	uint8_t *buf = rt_malloc(128);
	uint8_t *ptr = buf;
	uint32_t buf_len = 0, len = 0;

	*(uint32_t *)ptr = WHC_WIFI_TEST;
	ptr += 4;

	*ptr = WHC_WIFI_TEST_SOFTAP;
	ptr += 1;

	// means enable
	*ptr = 1;
	ptr += 1;

	len = strlen((char *)ssid);
	*ptr = len;
	ptr += 1;

	memcpy(ptr, ssid, len);
	ptr += len;

	memcpy(ptr, (u8 *)&chn, 4);
	ptr += 4;

	memcpy(ptr, (u8 *)&ip, 4);
	ptr += 4;

	if (psk && (strlen(psk) > 0)) {
		len = strlen(psk);
		memcpy(ptr, psk, len);
		ptr += len;
	} else {
		// 0 means open ap
		*ptr = 0;
		ptr += 1;
	}

	buf_len = ptr - buf;

	whc_sdio_host_send_to_dev(buf, buf_len);

	return 0;
}

/**
* @brief  send buf to dev
* @param  buf: data buf to be sent.
* @param  len: real buf address, to be freed after sent.
* @return none.
*/
void whc_sdio_host_send_to_dev(uint8_t *buf, uint32_t len)
{
	uint8_t *txbuf = NULL;
	uint32_t txsize = len + SIZE_TX_DESC;

	/* construct struct whc_buf_info & whc_buf_info_t */
	txbuf = rt_malloc(txsize);

	if (txbuf == NULL) {
		printf("%s mem fail \r\n", __func__);
		if (txbuf) {
			rt_free(txbuf);
		}
		return;
	}

	/* copy data */
	memcpy(txbuf + SIZE_TX_DESC, buf, len);

	/* send ret_msg + ret_val(buf, len) */
	rtw_sdio_send_data(txbuf, txsize, NULL);
}

void whc_sdio_host_send_to_dev_block(uint8_t *buf, uint32_t len, uint8_t *ret, uint32_t ret_len)
{
	uint8_t *txbuf = NULL;
	uint32_t txsize = len + SIZE_TX_DESC;
	int val;
	/* construct struct whc_buf_info & whc_buf_info_t */
	txbuf = rt_malloc(txsize);

	if (txbuf == NULL) {
		printf("%s mem fail \r\n", __func__);
		if (txbuf) {
			rt_free(txbuf);
		}
		return;
	}

	/* copy data */
	memcpy(txbuf + SIZE_TX_DESC, buf, len);

	/* send ret_msg + ret_val(buf, len) */
	rtw_sdio_send_data(txbuf, txsize, NULL);

	if (ret != NULL) {
		whc_sdio_priv.ret = ret;
		whc_sdio_priv.ret_len = ret_len;

		val = rtos_sema_take(whc_sdio_priv.host_send_block_sema, 20000);
		if (val) {
			printf("!!wait return val time out %s %d \r\n", __func__, val);
		}
		/* clear */
		whc_sdio_priv.ret = NULL;
	}
}

