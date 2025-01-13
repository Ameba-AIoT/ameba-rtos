/******************************************************************************
 *
 * Copyright(c) 2020 - 2021 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/
#include "inic_dev.h"

#include "lwip/sys.h"
#include "lwip/etharp.h"
#include "lwip/prot/tcp.h"
#include "lwip/prot/dns.h"
#include "lwip/udp.h"
#include "lwip_netconf.h"

#define PORT_TO_BOTH	0x0
#define PORT_TO_UP	0x1
#define PORT_TO_HOST	0x2
#define PORT_TO_UNKNOWN 0xFF

#define DEV_PORT_NUM 2
#define IP_ICMP_REQUEST 0x08
#ifndef DHCP_SERVER_PORT
#define DHCP_SERVER_PORT 67
#endif

struct bridge_pkt_attrib {
	u16_t protocol;
	u16_t src_port;
	u16_t dst_port;
	u8_t src_ip[4];
	u8_t dst_ip[4];
	u8_t flags;
	u8_t type;
	struct eth_addr src_mac;
	struct eth_addr dst_mac;
	u8_t port_idx;
};

struct device_port {
	u16_t port_num;
	u8_t  used;
};

struct device_port dev_port_array[DEV_PORT_NUM] = { {0, 0}};
struct sk_buff *skb_transit = NULL;
extern struct netif xnetif[NET_IF_NUM]; /* network interface structure */
extern void inic_send_api_ret_value(u32 api_id, u8 *pbuf, u32 len);

u8(*inic_sdio_bridge_pkt_redirect_custom_ptr)(struct sk_buff *skb, struct bridge_pkt_attrib *pattrib);

void inic_event_bridge_get_dev_mac(u32 api_id, u32 *param_buf)
{
	(void) param_buf;
	struct _rtw_mac_t dev_mac = {0};
	wifi_get_mac_address(STA_WLAN_INDEX, &dev_mac, 0);
	inic_send_api_ret_value(api_id, dev_mac.octet, 6);
}

void inic_event_bridge_DHCP(u32 api_id, u32 *param_buf)
{
	(void) param_buf;
	int ret = -1;
	LwIP_netif_set_link_up(0);
	if (LwIP_DHCP(0, DHCP_START) == DHCP_ADDRESS_ASSIGNED) {
		ret = 0;
	}
	inic_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}

void inic_event_bridge_get_ip(u32 api_id, u32 *param_buf)
{
	(void) param_buf;
	u32 ip_ret = 0;
	u8 *ip;
	ip = LwIP_GetIP(0);
	memcpy((u8 *)&ip_ret, ip, 4);
	inic_send_api_ret_value(api_id, (u8 *)&ip_ret, sizeof(ip_ret));
}

void inic_event_bridge_get_scan_res(u32 api_id, u32 *param_buf)
{
	(void)param_buf;
	char *scan_buf = NULL;
	unsigned int scanned_AP_num = 0;
	int ret = 0;

	scanned_AP_num = param_buf[0];
	if (scanned_AP_num == 0) {/* scanned no AP*/
		goto error_exit;
	}

	scan_buf = (char *)rtos_mem_zmalloc(scanned_AP_num * sizeof(struct rtw_scan_result));
	if (scan_buf == NULL) {
		goto error_exit;
	}
	if (wifi_get_scan_records(&scanned_AP_num, scan_buf) < 0) {
		rtos_mem_free((void *)scan_buf);
		goto error_exit;
	} else {
		inic_send_api_ret_value(api_id, (u8 *)scan_buf, scanned_AP_num * sizeof(struct rtw_scan_result));
		rtos_mem_free(scan_buf);
		return;
	}

error_exit:
	inic_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));

}


void bridge_packet_attrib_parse(struct sk_buff *skb, struct bridge_pkt_attrib *pattrib)
{
	u16_t protocol, src_port = 0, dst_port = 0;
	u8_t type = 0;
	u8_t flags = 0;
	struct ip_hdr  *iph = NULL;
	struct etharp_hdr *arph = NULL;
	struct tcp_hdr *tcph = NULL;
	struct udp_hdr *udph = NULL;
	u8_t *src_ip = NULL, *dst_ip = NULL;
	struct eth_addr *src_addr, *dst_addr;

	dst_addr = (struct eth_addr *)(skb->data);
	src_addr = (struct eth_addr *)(skb->data + ETH_ALEN);
	protocol = *((unsigned short *)(skb->data + 2 * ETH_ALEN));

	if (protocol == lwip_htons(ETH_P_IP)) {
		/* update src ip/mac mapping */
		iph = (struct ip_hdr *)(skb->data + ETH_HLEN);
		src_ip = (u8_t *) & (iph->src.addr);
		dst_ip = (u8_t *) & (iph->dest.addr);
		type = iph->_proto;

		switch (iph->_proto) {
		case IP_PROTO_TCP://TCP
			tcph = (struct tcp_hdr *)(skb->data + ETH_HLEN + sizeof(struct ip_hdr));
			if (tcph != NULL) {
				src_port = PP_NTOHS(tcph->src);
				dst_port = PP_NTOHS(tcph->dest);
				flags = TCPH_FLAGS(tcph);
			}
			break;
		case IP_PROTO_UDP://UDP
			udph = (struct udp_hdr *)(skb->data + ETH_HLEN + sizeof(struct ip_hdr));
			if (udph != NULL) {
				src_port = PP_NTOHS(udph->src);
				dst_port = PP_NTOHS(udph->dest);
			}
			break;
		default: //ICMP
			src_port = 0;
			dst_port = 0;
		}
	} else if (protocol == lwip_htons(ETH_P_ARP)) {
		arph = (struct etharp_hdr *)(skb->data + ETH_HLEN);
		src_ip = (u8 *) & (arph->sipaddr);
		dst_ip = (u8 *) & (arph->dipaddr);
	}

	pattrib->protocol = protocol;
	pattrib->src_port = src_port;
	pattrib->dst_port = dst_port;
	pattrib->type = type;
	pattrib->flags = flags;
	memcpy(&pattrib->src_mac, src_addr, ETH_ALEN);
	memcpy(&pattrib->dst_mac, dst_addr, ETH_ALEN);
	if (src_ip != NULL) {
		memcpy(pattrib->src_ip, src_ip, 4);
	}
	if (dst_ip != NULL) {
		memcpy(pattrib->dst_ip, dst_ip, 4);
	}

}

u8 bridge_rcvpkt_redirect(struct sk_buff *skb, struct bridge_pkt_attrib *pattrib)
{
	u8 icmp_type = 0;
	u8 i = 0;
	u8 type = PORT_TO_UNKNOWN;

	if (inic_sdio_bridge_pkt_redirect_custom_ptr) {
		type = inic_sdio_bridge_pkt_redirect_custom_ptr(skb, pattrib);
		if (type != PORT_TO_UNKNOWN) {
			return type;
		}
	}

	if (pattrib->protocol == lwip_htons(ETH_P_ARP)) {
		return PORT_TO_BOTH;
	}

	if (pattrib->type == IP_PROTO_ICMP) {
		icmp_type = *(u8 *)(skb->data + ETH_HLEN + sizeof(struct ip_hdr));
		if (icmp_type == IP_ICMP_REQUEST) {
			return PORT_TO_UP;
		} else {
			return PORT_TO_BOTH;
		}
	}

	if (pattrib->src_port == 0 || pattrib->dst_port == 0) {
		return PORT_TO_BOTH;
	}

	if (pattrib->type == IP_PROTO_TCP) {
		for (i = 0; i < DEV_PORT_NUM; ++i) { //check if pkt should go to ameba
			if (dev_port_array[i].used == 1 && dev_port_array[i].port_num == pattrib->dst_port) {
				return PORT_TO_UP;
			}
		}
		return PORT_TO_HOST; //not for amebad, go to host
	} else if (pattrib->type == IP_PROTO_UDP) {
		if (pattrib->src_port == DNS_SERVER_PORT) {
			return PORT_TO_BOTH;
		} else if (pattrib->src_port == DHCP_SERVER_PORT) {
			return PORT_TO_UP;
		} else {
			return PORT_TO_HOST;
		}
	}

	return PORT_TO_HOST;
}

u8 inic_dev_recv_bridge_process(u8 *idx, struct sk_buff **skb_send)
{
	struct bridge_pkt_attrib *pattrib;
	u8 direction = 0;
	struct sk_buff *skb_backup;
	struct sk_buff *skb;

	/* case1: pkt for softap, no need bridge, just indicate to lwip */
	if (*idx == 1) {
		skb = wifi_if_get_recv_skb(*idx);
		if (skb) {
			LwIP_ethernetif_recv(*idx, skb->len);
			dev_kfree_skb_any(skb);
		}
		return 0;
	}

	/* case2: host->dev->host pkt, when host send arp req, dev just reply with arp rsp to host */
	if (*idx == 2) {
		*idx = 0;
		*skb_send = skb_transit;
		return 1; //need send to host
	}

	/* case3: normal netdev0 pkt*/
	skb = wifi_if_get_recv_skb(*idx);
	pattrib = (struct bridge_pkt_attrib *)rtos_mem_zmalloc(sizeof(struct bridge_pkt_attrib));
	bridge_packet_attrib_parse(skb, pattrib);


	if (pattrib->protocol == lwip_htons(ETH_P_IPV6)) {
		dev_kfree_skb_any(skb);
		rtos_mem_free(pattrib);
		return 0;
	}

	pattrib->port_idx = *idx;//BRIDGE_TODO: check if needed

	direction = bridge_rcvpkt_redirect(skb, pattrib);

	switch (direction) {
	case PORT_TO_BOTH:
		skb_backup = skb_copy(skb, GFP_ATOMIC, SKB_WLAN_TX_EXTRA_LEN); 	//skb will be corrupt when copy to pbuf, thus use a new skb
		rltk_wlan_info[0].skb = (void *)skb_backup;
		LwIP_ethernetif_recv(*idx, skb_backup->len); //BRIDGE_TODO: indicate to host will happen after lwip processed, do we need accelerate
		dev_kfree_skb_any(skb_backup);
		break;
	case PORT_TO_UP:
		LwIP_ethernetif_recv(*idx, skb->len);
		dev_kfree_skb_any(skb);
		break;
	case PORT_TO_HOST:
		break;
	default:
		break;
	}

	rtos_mem_free(pattrib);
	if (direction == PORT_TO_HOST || direction == PORT_TO_BOTH) {
		*skb_send = skb;
		return 1;
	} else {
		return 0;
	}
}