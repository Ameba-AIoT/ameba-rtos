/******************************************************************************
 *
 * Copyright(c) 2020 - 2021 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/
#include "whc_dev.h"
#include "whc_dev_bridge.h"
#include "whc_dev_api.h"

#include "lwip/sys.h"
#include "lwip/etharp.h"
#include "lwip/prot/tcp.h"
#include "lwip/prot/dns.h"
#include "lwip/udp.h"
#include "lwip_netconf.h"

struct sk_buff *skb_transit = NULL;

extern struct list_head whc_filter_head;

u8(*whc_dev_pkt_redir_cusptr)(struct sk_buff *skb, struct bridge_pkt_attrib *pattrib);

void whc_dev_pktfilter_init(void)
{
	rtw_init_listhead(&whc_filter_head);
}

void whc_dev_packet_attrib_parse(struct sk_buff *skb, struct bridge_pkt_attrib *pattrib)
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

	if (protocol == lwip_htons(ETHTYPE_IP)) {
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
	} else if (protocol == lwip_htons(ETHTYPE_ARP)) {
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

static bool whc_dev_match_filter(struct bridge_pkt_attrib *pattrib, struct whc_dev_pkt_filter *filter)
{
	/* consider concurrent mode */
	if ((filter->mask & MASK_IDX) && pattrib->port_idx != filter->index) {
		return false;
	}

	if ((filter->mask & MASK_SRC_IP) &&
		memcmp(pattrib->src_ip, filter->src_ip, sizeof(filter->src_ip)) != 0) {
		return false;
	}

	if ((filter->mask & MASK_DST_IP) &&
		memcmp(pattrib->dst_ip, filter->dst_ip, sizeof(filter->dst_ip)) != 0) {
		return false;
	}

	if ((filter->mask & MASK_SRC_PORT) && pattrib->src_port != filter->src_port) {
		return false;
	}

	if ((filter->mask & MASK_DST_PORT) && pattrib->dst_port != filter->dst_port) {
		return false;
	}

	if ((filter->mask & MASK_TYPE) && pattrib->type != filter->type) {
		return false;
	}

	return true;
}

u8_t whc_dev_rcvpkt_filter(struct bridge_pkt_attrib *pattrib)
{
	struct list_head *plist, *phead;
	struct PktFilterNode *target;

	phead = &whc_filter_head;
	if (list_empty(phead)) {
		return whc_dev_api_get_default_direction();
	}

	plist = get_next(phead);

	while ((rtw_end_of_queue_search(phead, plist)) == FALSE) {
		target = LIST_CONTAINOR(plist, struct PktFilterNode, list);

		if (target && whc_dev_match_filter(pattrib, &target->filter)) {
			return target->filter.direction;
		}
		plist = get_next(plist);
	}

	return whc_dev_api_get_default_direction();
}

u8 whc_dev_rcvpkt_redirect(struct sk_buff *skb, struct bridge_pkt_attrib *pattrib)
{
	u8 icmp_type = 0;
	u8 type = PORT_TO_UNKNOWN;

	if (whc_dev_pkt_redir_cusptr) {
		type = whc_dev_pkt_redir_cusptr(skb, pattrib);
		if (type != PORT_TO_UNKNOWN) {
			return type;
		}
	}

#ifdef CONFIG_WHC_BRIDGE
	/* all pkt to upper before host rdy to avoid txbd full */
	if (!(whc_dev_api_get_host_rdy())) {
		return PORT_TO_UP;
	}
#endif

	if (pattrib->protocol == lwip_htons(ETHTYPE_ARP)) {
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

	if (pattrib->type == IP_PROTO_UDP) {
		if (pattrib->src_port == DNS_SERVER_PORT) {
			return PORT_TO_BOTH;
		} else if ((pattrib->src_port == DHCP_SERVER_PORT) && (pattrib->port_idx == STA_WLAN_INDEX)) {
			return PORT_TO_UP;
		} else if ((pattrib->src_port == DHCP_CLIENT_PORT) && (pattrib->port_idx == SOFTAP_WLAN_INDEX)) {
			return PORT_TO_UP;
		}
	}

	type = whc_dev_rcvpkt_filter(pattrib);

	return type;
}

u8 whc_dev_recv_pkt_process(u8 *idx, struct sk_buff **skb_send)
{
	struct bridge_pkt_attrib *pattrib;
	u8 direction = 0;
	struct sk_buff *skb_backup;
	struct sk_buff *skb;

	/* case1: host->dev->host pkt, when host send arp req, dev just reply with arp rsp to host */
	if (*idx == 2) {
		*idx = 0;
		*skb_send = skb_transit;
		return 1; //need send to host
	}

	/* case2: normal netdev0 pkt*/
	skb = wifi_if_get_recv_skb(*idx);
	pattrib = (struct bridge_pkt_attrib *)rtos_mem_zmalloc(sizeof(struct bridge_pkt_attrib));
	whc_dev_packet_attrib_parse(skb, pattrib);


	if (pattrib->protocol == lwip_htons(ETHTYPE_IPV6)) {
		dev_kfree_skb_any(skb);
		rtos_mem_free(pattrib);
		return 0;
	}

	pattrib->port_idx = *idx;//BRIDGE_TODO: check if needed

	direction = whc_dev_rcvpkt_redirect(skb, pattrib);

	switch (direction) {
	case PORT_TO_BOTH:
		skb_backup = skb_copy(skb, GFP_ATOMIC, SKB_WLAN_TX_EXTRA_LEN); 	//skb will be corrupt when copy to pbuf, thus use a new skb
		if (skb_backup) {
			rltk_wlan_info[*idx].skb = (void *)skb_backup;
			LwIP_ethernetif_recv(*idx, skb_backup->len); //BRIDGE_TODO: indicate to host will happen after lwip processed, do we need accelerate
			dev_kfree_skb_any(skb_backup);
		} else {
			RTK_LOGD(TAG_WLAN_DRV, "both, alloc skb for lwip failed\n");
		}
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
