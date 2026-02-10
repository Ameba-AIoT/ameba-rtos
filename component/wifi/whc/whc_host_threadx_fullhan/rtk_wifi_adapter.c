#include "apl_api.h"
#include "sys_log.h"
#include "ap_net.h"
#include "ap_mpi.h"
#include "ap_gpio.h"
#include "ap.h"
#include "ap_common.h"
#include "ap_misc.h"
#include "ap_upg.h"

#include "apl_cloud.h"
#include "noodles.h"
#include "mcu_upgrade.h"

#include "whc_sdio_host.h"
#include "fh_sdio.h"
/********************* rtk wifi api ****************************/
#include <lwip/netif.h>
#include <lwip/etharp.h>
#include <netif/ethernetif.h>

WIFI_CFG_t      g_wifi_cur_cfg;
struct rt_mutex g_wifi_scan_lock;
int             g_wifi_is_connect;
int             g_wifi_ready;
WIFI_MODE_t     g_wifi_mode;
FN_WIFI_API_SET_SSID_CALLBACK g_wifi_api_set_ssid_callback = NULL;
unsigned int g_sdio_irq_num;
extern struct netif xnetif[NET_IF_NUM]; /* network interface structure */

/*********************************************************************************************/
void whc_sdio_host_rx_handler(uint8_t *buf)
{
	uint8_t *ptr = buf + SIZE_RX_DESC;
	struct whc_msg_info *msg_info = (struct whc_msg_info *)ptr;
	char *data = (char *)(ptr + sizeof(struct whc_msg_info) + msg_info->pad_len);
	struct pbuf *temp_buf = 0;
	struct pbuf *p_buf;

retry:
	p_buf = pbuf_alloc(PBUF_RAW, msg_info->data_len, PBUF_POOL);

	if (p_buf == NULL) {
		printf("%s: nobuf\n", __func__);
		rt_thread_mdelay(1);
		goto retry;
		//just send rsp when pbuf alloc fail
		//return;
	}

	/* cpoy data from skb(ipc data) to pbuf(ether net data) */
	temp_buf = p_buf;
	while (temp_buf) {
		/* If tot_len > PBUF_POOL_BUFSIZE_ALIGNED, the skb will be
		 * divided into several pbufs. Therefore, there is a while to
		 * use to assigne data to pbufs.
		 */

		memcpy(temp_buf->payload, data, temp_buf->len);
		data = data + temp_buf->len;
		temp_buf = temp_buf->next;
	}

	if (p_buf != NULL) {
		LwIP_ethernetif_recv_inic(msg_info->wlan_idx, p_buf);
	}

	rt_free(buf);
}


int wifi_api_start_rtk(WIFI_MODE_t mode)
{
	int ret = 0;
	struct netif *p_netif = NULL;

	whc_host_init();

	if (mode == WIFI_MODE_STA) {
		g_wifi_mode = mode;
		p_netif = netif_find(wifi_get_ifname());
	} else if (mode == WIFI_MODE_AP) {
		// start ap mode
		g_wifi_mode = mode;
		p_netif = netif_find(wifi_get_ifname_ap());
	}

	if (p_netif) {
		nif_set_up(p_netif);
	}
	return ret;
}

extern int sdio_init(unsigned int which, unsigned int wkmod, unsigned int *dma_desc, HSDC *phandle);
extern struct whc_sdio whc_sdio_priv;
int rtk_init_sdio(int workmomde)
{
	return sdio_init(WIFI_SDIO, workmomde, NULL, &whc_sdio_priv.func);

}

/*********************rtk api wrapper****************************/

void sdio_gpio_int_hdl(void)
{
	struct whc_sdio *priv = &whc_sdio_priv;
#ifdef CONFIG_SDIO_TX_ENABLE_AVAL_INT
	sdio_disable_data1_irq();
#endif

	rt_sem_release((rt_sem_t)priv->host_irq); /* ignore failure since there is nothing that can be done about it in a ISR */

}

void wifi_gpio_init(void)
{
	int irq, ret;
	g_sdio_irq_num = 14;

	irq = gpio_to_irq(g_sdio_irq_num);
	gpio_request(g_sdio_irq_num);
	gpio_direction_input(g_sdio_irq_num);

	ret = gpio_set_irq_type(g_sdio_irq_num, IRQ_TYPE_EDGE_RISING);
	if (ret) {
		printf("gpio set irq error %d \r\n", ret);
		return ret;
	}

	rt_hw_interrupt_install(irq, sdio_gpio_int_hdl, NULL, "wifi_gpio_int");
	rt_hw_interrupt_umask(irq);
	gpio_irq_enable(irq);
	gpio_release(g_sdio_irq_num);

#ifdef SDIO_DEBUG
	uint8_t debug_gpio = 11; //GPIO1_3
	gpio_request(debug_gpio);
	gpio_direction_output(debug_gpio, 0);
	gpio_set_value(11, 0);
	gpio_release(debug_gpio);
#endif
}

int wifi_sdio_init(void)
{
#ifndef WIFI_SDIO_BITWIDTH
	int bitwidth = 1;
#else
	int bitwidth = WIFI_SDIO_BITWIDTH;
#endif
	int workmode = SDC_WKMOD_50M_HI_SPEED | bitwidth;

	/* init host sdio */
	rtk_init_sdio(workmode);

	wifi_gpio_init();

	int ret = sdio_high_speed_mode(whc_sdio_priv.func, bitwidth, 50000000);
	if (ret) {
		printf("sdio_high_speed_mode ret %d\n", ret);
	}
	return 0;
}

/*************************************************************/
int wifi_api_init(int param)
{
	if (g_wifi_ready) {
		return 0;
	}

	wifi_sdio_init();

	g_wifi_is_connect = 0;
	g_wifi_mode = WIFI_MODE_UNKNOWN;
	memset(&g_wifi_cur_cfg, 0, sizeof(g_wifi_cur_cfg));

	rt_mutex_init(&g_wifi_scan_lock, "scan", RT_IPC_FLAG_FIFO);

	g_wifi_ready = 1;
	return 0;
}

void wifi_api_exit(void)
{
#ifdef todo
#endif
	printf("%s todo \r\n", __func__);
	return;
}

int wifi_api_disconnect(void)
{
	int ret = 0;
	if (g_wifi_ready == 0) {
		dbg_log_e("wifi not ready\n");
		return -1;
	}

	if (g_wifi_is_connect == 0) {
		return 0;
	}

	if (g_wifi_mode == WIFI_MODE_STA) {
		ret = whc_host_wifi_disconnect(0);
	} else if (g_wifi_mode == WIFI_MODE_AP) {
		dhcpd_stop(wifi_get_ifname_ap());
		dnsd_stop();
	}

	memset(&g_wifi_cur_cfg, 0, sizeof(WIFI_CFG_t));

	g_wifi_is_connect = 0;

	return ret;
}

WIFI_MODE_t wifi_api_get_cur_mode(void)
{
	return g_wifi_mode;
}

int _wifi_api_clear_if(WIFI_MODE_t mode)
{
	struct netif *p_netif = NULL;
	int i;

	if (mode != g_wifi_mode) {
		return 0;
	}
	if (g_wifi_mode == WIFI_MODE_STA) {
		p_netif = netif_find(wifi_get_ifname());
	} else if (g_wifi_mode == WIFI_MODE_AP) {
		p_netif = netif_find(wifi_get_ifname_ap());
	}
	if (p_netif) {
		ip4_addr_t ip4_addr;
		ip4_addr_set_zero(&ip4_addr);

		netif_set_ipaddr(p_netif, &ip4_addr);
		netif_set_netmask(p_netif, &ip4_addr);
		netif_set_gw(p_netif, &ip4_addr);

		for (i = 0; i < 2; i++) {
			ip_addr_t dns_addr;
#if (FH_RTT_SDK_VER >= 300)
			ip_addr_set_ip4_u32_val(dns_addr, 0);
#else
			dns_addr.addr = 0;
#endif
			dns_setserver(i, &dns_addr);
		}
	}

	return 0;
}

int wifi_api_stop(WIFI_MODE_t mode)
{
	int ret = 0;
	struct netif *p_netif = NULL;

	dbg_log_i(" stop %d\n", mode);
	if (g_wifi_ready == 0) {
		dbg_log_e("wifi not ready\n");
		return -1;
	}

	if (WIFI_MODE_UNKNOWN == mode) {
		mode = g_wifi_mode;
	} else {
		if (mode != WIFI_MODE_STA && mode != WIFI_MODE_AP) {
			dbg_log_i(" stop mode %d invalid\n", mode);
			return -1;
		}
	}

	if (mode != g_wifi_mode) {
		dbg_log_i(" stop mode %d != g_wifi_mode %d\n", mode, g_wifi_mode);
		return 0;
	}

	if (mode == WIFI_MODE_STA) {
#ifdef todo
//sta mode stop?
#endif
		p_netif = netif_find(wifi_get_ifname());
	} else if (mode == WIFI_MODE_AP) {
		dhcpd_stop(wifi_get_ifname_ap());
		dnsd_stop();

		ret = whc_host_wifi_stop_ap();
		p_netif = netif_find(wifi_get_ifname_ap());
	}

	if (p_netif) {
		nif_set_down(p_netif);
	}
	_wifi_api_clear_if(mode);
	g_wifi_mode = WIFI_MODE_UNKNOWN;

	return ret;
}
//todo
int wifi_api_get_driver_status(void)
{
	return 0;
}

#ifndef WIFI_AP_SSID_PREFIX
#define WIFI_AP_SSID_PREFIX         "YG-AP"
#endif

void _wifi_api_gen_default_ssid(unsigned char *p_ssid)
{
#if (WIFI_GEN_AP_SSID == 0)  // use mac addr as ap ssid
	FH_UINT8 mac[6];

	memset(mac, 0, sizeof(mac));
	struct netif *p_netif = netif_find(wifi_get_ifname_ap());
	if (!p_netif) {
		p_netif = netif_find("e0");
	}
	if (p_netif) {
		memcpy(mac, p_netif->hwaddr, 6);
	}

	sprintf((char *)p_ssid, "%s-%02X%02X%02X%02X%02X%02X", WIFI_AP_SSID_PREFIX,
			mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
#elif (WIFI_GEN_AP_SSID == 1)  // use serial number as ap ssid
	extern const char *ut_uboot_env_get(const char *p_key, char *p_val, int len);

	char sn[128];
	ut_uboot_env_get(ENV_KEY_SN, sn, sizeof(sn));
	sprintf((char *)p_ssid, "%s%s", WIFI_AP_SSID_PREFIX, sn);
#endif
	dbg_log_i("wifi_api_gen_default_ssid(): ssid = %s\n", p_ssid);
}

int _wifi_api_set_default_config(WIFI_CFG_t *p_wifi_cfg)
{
	memset(p_wifi_cfg, 0, sizeof(WIFI_CFG_t));
	_wifi_api_gen_default_ssid(p_wifi_cfg->ssid);
	p_wifi_cfg->channel = WIFI_AP_DEFAULT_CHANL;
	strncpy((char *)p_wifi_cfg->password, WIFI_AP_DEFAULT_PASSWORD, sizeof(p_wifi_cfg->password) - 1);

	p_wifi_cfg->dhcp_flag = 1;
	p_wifi_cfg->ipaddr_flag = 1;
	p_wifi_cfg->netmask_flag = 1;
	p_wifi_cfg->gateway_flag = 1;
	p_wifi_cfg->dns_flag = 1;

	p_wifi_cfg->ipaddr = AP_MODE_IP_ADDR;
	p_wifi_cfg->netmask = AP_MODE_NETMASK_ADDR;
	p_wifi_cfg->gateway = AP_MODE_GATEWAY_ADDR;

	return 0;
}

int wifi_api_start(WIFI_MODE_t mode)
{
	int ret = 0;
	struct netif *p_netif = NULL;

	dbg_log_i(" start %d %x\n", mode, __builtin_return_address(0));
	if (g_wifi_ready == 0) {
		dbg_log_e("wifi not ready\n");
		return -1;
	}

	if (mode != WIFI_MODE_STA && mode != WIFI_MODE_AP) {
		dbg_log_i(" start mode %d invalid, Now just support station and AP mode\n", mode);
		return -1;
	}

	if (mode == g_wifi_mode) {
		dbg_log_i(" start mode %d == g_wifi_mode %d\n", mode, g_wifi_mode);
		return 0;
	}

	if (g_wifi_mode != WIFI_MODE_UNKNOWN) {
		wifi_api_stop(g_wifi_mode);
	}


	if (mode == WIFI_MODE_STA) {
		// start station mode
#ifdef todo
// start sta, no need in rtk
#endif
		wifi_api_start_rtk(WIFI_MODE_STA);
		ret = 0;
		if (ret != 0) {
			rt_kprintf("station mode(0) ret %d\n", ret);
			g_wifi_mode = WIFI_MODE_UNKNOWN;
			return ret;
		}
		rt_kprintf("sta mode 0 end\n");
		g_wifi_mode = mode;
		p_netif = netif_find(wifi_get_ifname());
	} else if (mode == WIFI_MODE_AP) {
		// start ap mode
#ifdef todo
//start ap no need in rtk
#endif
		wifi_api_start_rtk(WIFI_MODE_AP);

		ret = 0;
		if (ret) {
			rt_kprintf("softap ret %d\n", ret);
			g_wifi_mode = WIFI_MODE_UNKNOWN;
			return ret;
		}
		rt_kprintf("w_start 1 end\n");
		_wifi_api_set_default_config(&g_wifi_cur_cfg);
		if (g_wifi_api_set_ssid_callback) {
			g_wifi_api_set_ssid_callback(g_wifi_cur_cfg.ssid, g_wifi_cur_cfg.password, &g_wifi_cur_cfg.channel);
		}
		g_wifi_mode = mode;
		p_netif = netif_find(wifi_get_ifname_ap());
	}

	if (p_netif) {
		nif_set_up(p_netif);
	}

	whc_host_get_mac_addr(mode);

	return ret;
}

int _wifi_api_connect_with_sta(WIFI_CFG_t *p_wifi_cfg)
{

	whc_host_wifi_connect(p_wifi_cfg->ssid, p_wifi_cfg->password);
	struct netif *p_netif = netif_find(wifi_get_ifname());
	if (p_netif) {
		nif_set_up(p_netif);
		nif_set_link_up(p_netif);

		if (p_wifi_cfg->dhcp_flag) {
#ifdef USE_WIFI_DHCP
			//default dhcp in ameba
#else
			printf("!!!!!!!!!never should be here \r\n");
			nif_dhcp_start(p_netif);
#endif
		} else if (p_wifi_cfg->ipaddr_flag) {
#ifdef USE_WIFI_DHCP
#ifdef todo
//stop dhcp??
#endif
#else
			nif_dhcp_stop(p_netif);
#endif

			ip_addr_t ipaddr;
			ip_addr_set_ip4_u32(&ipaddr, p_wifi_cfg->ipaddr);
			netif_set_ipaddr(p_netif, (ip4_addr_t *)&ipaddr);

			if (p_wifi_cfg->netmask_flag) {
				ip_addr_set_ip4_u32(&ipaddr, p_wifi_cfg->netmask);
				netif_set_netmask(p_netif, (ip4_addr_t *)&ipaddr);
			}
			if (p_wifi_cfg->gateway_flag) {
				ip_addr_set_ip4_u32(&ipaddr, p_wifi_cfg->gateway);
				netif_set_gw(p_netif, (ip4_addr_t *)&ipaddr);
			}

			if (p_wifi_cfg->dns_flag) {
				int i;

				for (i = 0; i < 2; i++) {
					ip_addr_set_ip4_u32(&ipaddr, p_wifi_cfg->dns[i]);
					dns_setserver(i, &ipaddr);
				}
			}
		}
	} else {
		dbg_log_e("Not found interface w0\n");
		return -1;
	}

	g_wifi_is_connect = 1;

	return 0;
}

int _wifi_api_connect_with_ap(WIFI_CFG_t *p_wifi_cfg)
{
	int ret;

	dbg_log_i("ap: %s,%s,%d,%x\n", p_wifi_cfg->ssid, p_wifi_cfg->password, p_wifi_cfg->channel, p_wifi_cfg->ipaddr);

	ret = whc_host_wifi_enable_ap((unsigned char *)p_wifi_cfg->ssid, (unsigned char *)p_wifi_cfg->password, p_wifi_cfg->channel, p_wifi_cfg->ipaddr);
	if (ret) {
		dbg_log_e("w_ap_on_ext() ret %d\n", ret);
		return ret;
	}
	dbg_log_t();

	struct netif *p_netif = netif_find(wifi_get_ifname_ap());
	if (p_netif) {
		nif_set_up(p_netif);
		nif_set_link_up(p_netif);

		if (p_wifi_cfg->ipaddr_flag) {
			ip_addr_t ipaddr;
			ip_addr_set_ip4_u32(&ipaddr, p_wifi_cfg->ipaddr);
			netif_set_ipaddr(p_netif, (ip4_addr_t *)&ipaddr);

			if (p_wifi_cfg->netmask_flag) {
				ip_addr_set_ip4_u32(&ipaddr, p_wifi_cfg->netmask);
				netif_set_netmask(p_netif, (ip4_addr_t *)&ipaddr);
			}
			if (p_wifi_cfg->gateway_flag) {
				ip_addr_set_ip4_u32(&ipaddr, p_wifi_cfg->gateway);
				netif_set_gw(p_netif, (ip4_addr_t *)&ipaddr);
			}
		}
	} else {
		dbg_log_e("Not found interface w0\n");
		return -1;
	}

	g_wifi_is_connect = 1;

	return 0;
}


int wifi_api_connect(WIFI_CFG_t *p_wifi_cfg)
{
	int ret = 0;

	if (g_wifi_ready == 0) {
		dbg_log_e("wifi not ready\n");
		return -1;
	}

	if (!p_wifi_cfg) {
		p_wifi_cfg = &g_wifi_cur_cfg;
	}

	if (g_wifi_mode == WIFI_MODE_AP) {
		if (g_wifi_api_set_ssid_callback) {
			g_wifi_api_set_ssid_callback(p_wifi_cfg->ssid, p_wifi_cfg->password, &p_wifi_cfg->channel);
		}
	}

	if (p_wifi_cfg->ssid[0] == 0) {
		dbg_log_e("No SSID\n");
		return -1;
	}

	if (p_wifi_cfg->password[0] == 0) {
		p_wifi_cfg->security = 0;
	}

	if (g_wifi_mode == WIFI_MODE_STA) {
		if (!netif_find(wifi_get_ifname())) {
			dbg_log_e("Not found interface %s\n", wifi_get_ifname());
			return -1;
		}
		// start station mode
		ret = _wifi_api_connect_with_sta(p_wifi_cfg);
	} else if (g_wifi_mode == WIFI_MODE_AP) {
		if (!netif_find(wifi_get_ifname_ap())) {
			dbg_log_e("Not found interface %s\n", wifi_get_ifname_ap());
			return -1;
		}
		// start ap mode
		ret = _wifi_api_connect_with_ap(p_wifi_cfg);
		whc_host_get_mac_addr(g_wifi_mode);
	} else {
		dbg_log_e("Current not int station or ap mode\n");
		return -1;
	}
	if (ret == 0 && p_wifi_cfg != &g_wifi_cur_cfg) {
		memcpy(&g_wifi_cur_cfg, p_wifi_cfg, sizeof(WIFI_CFG_t));
	}

	return ret;
}


int wifi_api_set_cfg(WIFI_CFG_t *p_wifi_cfg)
{
	if (!p_wifi_cfg) {
		return -1;
	}

	memcpy(&g_wifi_cur_cfg, p_wifi_cfg, sizeof(WIFI_CFG_t));

	return 0;
}

WIFI_CFG_t *wifi_api_get_cfg(void)
{
	return &g_wifi_cur_cfg;
}

int wifi_api_get_status(WIFI_STATUS_t *p_status)
{
	return 0;
}

const char *wifi_get_ifname(void)
{
	if (g_wifi_mode == WIFI_MODE_AP) {
		return wifi_get_ifname_ap();
	}
	return wifi_get_ifname_sta();
}

const char *wifi_get_ifname_sta(void)
{
	return WIFI_IFNAME_STA;
}

const char *wifi_get_ifname_ap(void)
{
	return WIFI_IFNAME_AP;
}

int wifi_api_is_connect(void)
{
	int ret = 0;
	if (!g_wifi_is_connect) {
		return 0;
	}

	if (g_wifi_mode == WIFI_MODE_AP) {
		return 1;
	}

	if (g_wifi_mode == WIFI_MODE_STA) {
		whc_host_get_connet_status(&ret);
	}

	return ret;
}

int wifi_api_ap_get_sta_num(void)
{
	int num;
	whc_sdio_host_get_stanum(&num);
	return num;
}

int wifi_api_scan(WIFI_SCAN_RESULT_t *p_scan_result, int max_num)
{
	if (g_wifi_ready == 0) {
		dbg_log_e("wifi not ready\n");
		return 0;
	}

	int ap_cnt = 0;

	/* need?? */
	int ret = rt_mutex_take(&g_wifi_scan_lock, 20000);

	if (ret) {
		printf("last scan still ongoing, check status \r\n");
		return -1;
	}

	whc_host_wifi_scan(&ap_cnt);

	if (ap_cnt == 0) {
		// no ap found
		goto DONE;
	} else {

		ap_cnt = max_num > ap_cnt ? ap_cnt : max_num;

		whc_host_wifi_get_scanresult(p_scan_result, ap_cnt);
	}

DONE:
	rt_mutex_release(&g_wifi_scan_lock);

	return ap_cnt;
}

int wifi_api_ready(void)
{
	return g_wifi_ready;
}

// get ip addr for sta mode
void wifi_api_dhcp(void)
{
	whc_host_get_ip(0);
}

int wifi_conn_test(char *ssid, char *pass)
{
	WIFI_CFG_t con_wifi_cfg;
	memset(&con_wifi_cfg, 0, sizeof(WIFI_CFG_t));
	wifi_api_start(WIFI_MODE_STA);

	if (ssid) {
		strcpy(con_wifi_cfg.ssid, ssid);
	}

	if (pass) {
		strcpy(con_wifi_cfg.password, pass);
	}

	wifi_api_connect(&con_wifi_cfg);
	return 0;
}

int wifi_ap_start(char *ssid, char *pass, uint8_t ch, char *ip)
{
	WIFI_CFG_t con_wifi_cfg;
	memset(&con_wifi_cfg, 0, sizeof(WIFI_CFG_t));
	wifi_api_start(WIFI_MODE_AP);

	if (ssid) {
		strcpy(con_wifi_cfg.ssid, ssid);
		printf("cmd wifi_connect ssid %s\n", ssid);
	}

	if (pass) {
		strcpy(con_wifi_cfg.password, pass);
	}

	if (ch != 0) {
		con_wifi_cfg.channel = ch;

	}
	if (ip) {
		con_wifi_cfg.ipaddr = ipaddr_addr(ip);
		con_wifi_cfg.ipaddr_flag = 1;
		con_wifi_cfg.gateway = con_wifi_cfg.ipaddr;
		con_wifi_cfg.gateway_flag = 1;
		con_wifi_cfg.netmask = 0xFFFFFF;
		con_wifi_cfg.netmask_flag = 1;
	}

	wifi_api_connect(&con_wifi_cfg);
	return 0;
}
