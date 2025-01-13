// #include <rtw_adapter.h>
#include "inic_dev.h"

#define TAG_WLAN_PROXY  "PROXY"

u8 *rtw_get_adapter_mac_addr(u8 iface_type);
struct H2C_WAROFFLOAD_PARM WAROffloadParm = {0};
struct MDNS_PARM MDNS = {0};
struct IPV4_PARM IPv4Parm = {0};
struct IPV6_PARM IPv6Parm = {0};

struct IPV4_OFFLOAD_SERVICE_TABLE  Ipv4OffloadService[] = {
	{0x01, 0x2, 0, 0x2, {0x0, 0x0, 0x0, 0x0}, 0x800, 0x0, Offload_Service_ICMPv4}
	,
	{0x11, 0x0, 0, 0x3, {0x0, 0x0, 0x0, 0x0}, 0x89, 0x89, Offload_Service_NetBios}
	,
	{0x11, 0x6, 0, 0x1, {0x0, 0x0, 0x0, 0x0}, 0xA1, 0xA1, Offload_Service_Snmpv4}
	,
	{0x11, 0x1, 4, 0x3, {0xE0, 0x0, 0x0, 0xFB}, 0x14E9, 0x14E9, Offload_Service_mDnsv4}
	,
	{0x11, 0x1, 4, 0x1, {0xEF, 0xFF, 0xFF, 0xFA}, 0x76C, 0x76C, Offload_Service_SSDPv4}
	,
	{0x11, 0x1, 4, 0x1, {0xEF, 0xFF, 0xFF, 0xFA}, 0xE76, 0xE76, Offload_Service_WSDv4}
	,
	{0x11, 0x1, 4, 0x1, {0xEF, 0xFF, 0xFF, 0xFD}, 0x1AB, 0x1AB, Offload_Service_SLPv4}
	,
	{0x11, 0x1, 4, 0x1, {0xE0, 0x00, 0x00, 0xFC}, 0x14EB, 0x14EB, Offload_Service_LLMNRv4}
};

struct IPV6_OFFLOAD_SERVICE_TABLE  Ipv6OffloadService[] = {
	{0x3A, 0x2, 0, 0x2, {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, 0x8000, 0x0, Offload_Service_ICMPv6}
	,
	{0x3A, 0x4, 0, 0x2, {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, 0x8700, 0x0, Offload_Service_ICMPNS}
	,
	{0x11, 0x6, 0, 0x1, {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, 0xA1, 0xA1, Offload_Service_Snmpv6}
	,
	{0x11, 0x1, 16, 0x3, {0xFF02, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFB}, 0x14E9, 0x14E9, Offload_Service_mDnsv6}
	,
	{0x11, 0x1, 16, 0x1, {0xFF02, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xC}, 0x76C, 0x76C, Offload_Service_SSDPv6}
	,
	{0x11, 0x1, 16, 0x1, {0xFF05, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xC}, 0x76C, 0x76C, Offload_Service_SSDPv6}
	,
	{0x11, 0x1, 16, 0x1, {0xFF02, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xC}, 0xE76, 0xE76, Offload_Service_WSDv6}
	,
	{0x11, 0x1, 16, 0x1, {0xFF02, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x123}, 0x1AB, 0x1AB, Offload_Service_SLPv6}
	,
	{0x11, 0x1, 16, 0x1, {0xFF05, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x123}, 0x1AB, 0x1AB, Offload_Service_SLPv6}
	,
	{0x11, 0x1, 16, 0x1, {0xFF02, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x116}, 0x1AB, 0x1AB, Offload_Service_SLPv6}
	,
	{0x11, 0x1, 16, 0x1, {0xFF05, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x116}, 0x1AB, 0x1AB, Offload_Service_SLPv6}
	,
	{0x11, 0x1, 14, 0x1, {0xFF02, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x0}, 0x1AB, 0x1AB, Offload_Service_SLPv6}
	,
	{0x11, 0x1, 14, 0x1, {0xFF05, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x0}, 0x1AB, 0x1AB, Offload_Service_SLPv6}
	,
	{0x11, 0x1, 16, 0x1, {0xFF02, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x3}, 0x14EB, 0x14EB, Offload_Service_LLMNRv6}
};

u8 ParseProtocol(int idx, struct sk_buff *skb)
{
	u8  service = Offload_Service_Unknown;
	u8  protocol = 0;
	u8  ipv4_broadcast_ip[4] = {0xff, 0xff, 0xff, 0xff};
	bool   isUcIpMatch = FALSE;
	u8  ipIndex = 0;
	u8 *ptr = skb->data;

	(void)idx;

	do {
		u8  type[2] = {0}, index = 0;
		u8  ipv4_dst_ip[4] = {0};
		u16  ipv6_dst_ip[8] = {0};
		u16  ipv6_local_ip[8];
		u16 src_port = 0, dst_port = 0;

		/* Get Ether Type */
		memcpy(&type, &((struct ethhdr *)ptr)->h_proto,  2);
		memset(ipv6_local_ip, 0, sizeof(ipv6_local_ip));

		/* IP header */
		ptr += ETH_HLEN;

		if (type[0] == 0x08 && type[1] == 0x00) { // IPv4
			/* Get Protocal in IPv4 Header */
			protocol = *(ptr + IPv4_PROTOCOL_OFFSET);

			/* Get Dest IP Address in IPv4 Header */
			for (index = 0; index < 4; index++) {
				ipv4_dst_ip[index] = *(ptr + IPv4_DEST_IP_OFFSET + index);
			}

			/* Get Port Number in UDP Header */
			src_port = (u16)((*(ptr + IP_HEADER + UDP_SRC_PORT_OFFSET) << 8) |
							 (*(ptr + IP_HEADER + UDP_SRC_PORT_OFFSET + 1)));
			dst_port = (u16)((*(ptr + IP_HEADER + UDP_DES_PORT_OFFSET) << 8) |
							 (*(ptr + IP_HEADER + UDP_DES_PORT_OFFSET + 1)));

			//RT_TRACE(DBGMSG_WOWLAN, DBG_LOUD, ("IPv4 :: protocol[%bx], ipv4_dst_ip[%bx:%bx:%bx:%bx], src_port[%x], dst_port[%x]\n",
			//    protocol, ipv4_dst_ip[0], ipv4_dst_ip[1], ipv4_dst_ip[2], ipv4_dst_ip[3], src_port, dst_port));

			for (index = 0; index < sizeof(Ipv4OffloadService) / sizeof(struct IPV4_OFFLOAD_SERVICE_TABLE); index++) {
				if (protocol == Ipv4OffloadService[index].u1Prot
					&& (Ipv4OffloadService[index].u1PortMask & 0x2 ? (src_port == Ipv4OffloadService[index].u2SrcPort) : TRUE)
					&& (Ipv4OffloadService[index].u1PortMask & 0x1 ? (dst_port == Ipv4OffloadService[index].u2DstPort) : TRUE)) {

					if ((Ipv4OffloadService[index].u1IpCmpType & COMPARE_UNICAST_IP)) {
						for (ipIndex = 0; ipIndex < 4; ipIndex++) {
							if (memcmp(ipv4_dst_ip, IPv4Parm.IP, 4) == 0) {
								isUcIpMatch = TRUE;
								break;
							}
						}
					}

					if (((Ipv4OffloadService[index].u1IpCmpType & COMPARE_SPECIFIED_IP) &&
						 (memcmp(ipv4_dst_ip, Ipv4OffloadService[index].au1DstIp, Ipv4OffloadService[index].u1IpMatchSize) ? FALSE : TRUE)) ||
						((Ipv4OffloadService[index].u1IpCmpType & COMPARE_BROADCAST_IP) &&
						 (memcmp(ipv4_dst_ip, ipv4_broadcast_ip, 4) ? FALSE : TRUE)) ||
						(Ipv4OffloadService[index].u1IpCmpType == COMPARE_NONE) || isUcIpMatch) {
						service = Ipv4OffloadService[index].Service;
						RTK_LOGD(TAG_WLAN_PROXY, "IPv4 :: Table Found Service[%x]\n", service);
						break;
					}

				}
			}
		} else if (type[0] == 0x88 && type[1] == 0xD9) { //LLTD
			service = Offload_Service_LLTD;
		} else if (type[0] == 0x08 && type[1] == 0x06) {
			protocol = 0xff;   // No need to parse
		} else if (type[0] == 0x86 && type[1] == 0xdd) { // IPv6
			/* Get Protocal in IPv6 Header */
			protocol = *(ptr + IPv6_NEXT_HEADER_OFFSET);

			/* Get Dest IP Address in IPv6 Header */
			for (index = 0; index < 8; index++) {
				ipv6_dst_ip[index] = (u16)(*(ptr + IPv6_DES_ADDRESS_OFFSET + index * 2) << 8 |
										   *(ptr + IPv6_DES_ADDRESS_OFFSET + index * 2 + 1));
				ipv6_local_ip[index] = (u16)(IPv6Parm.IP[index * 2] << 8 | IPv6Parm.IP[index * 2 + 1]);
			}

			/* Get Port Number in UDP Header */
			src_port = (u16)((*(ptr + IPv6_HEADER + UDP_SRC_PORT_OFFSET) << 8) |
							 (*(ptr + IPv6_HEADER + UDP_SRC_PORT_OFFSET + 1)));
			dst_port = (u16)((*(ptr + IPv6_HEADER + UDP_DES_PORT_OFFSET) << 8) |
							 (*(ptr + IPv6_HEADER + UDP_DES_PORT_OFFSET + 1)));

			//RT_TRACE(DBGMSG_WOWLAN, DBG_LOUD, ("IPv6 :: protocol[%bx], ipv6_dst_ip[%x:%x:%x:%x:%x:%x:%x:%x], src_port[%x], dst_port[%x]\n",
			//    protocol,
			//    ipv6_dst_ip[0], ipv6_dst_ip[1], ipv6_dst_ip[2], ipv6_dst_ip[3],
			//    ipv6_dst_ip[4], ipv6_dst_ip[5], ipv6_dst_ip[6], ipv6_dst_ip[7],
			//    src_port, dst_port));

			for (index = 0; index < sizeof(Ipv6OffloadService) / sizeof(struct IPV6_OFFLOAD_SERVICE_TABLE); index++) {
				if (protocol == Ipv6OffloadService[index].u1Prot
					&& (Ipv6OffloadService[index].u1PortMask & 0x2 ? (src_port == Ipv6OffloadService[index].u2SrcPort) : TRUE)
					&& (Ipv6OffloadService[index].u1PortMask & 0x1 ? (dst_port == Ipv6OffloadService[index].u2DstPort) : TRUE)) {

					if ((Ipv6OffloadService[index].u1IpCmpType & COMPARE_UNICAST_IP)) {
						if (memcmp(ipv6_dst_ip, ipv6_local_ip, 16) == 0) {
							isUcIpMatch = TRUE;
							break;
						}
					}

					if (((Ipv6OffloadService[index].u1IpCmpType & COMPARE_SPECIFIED_IP) &&
						 (memcmp(ipv6_dst_ip, Ipv6OffloadService[index].au2DstIp, Ipv6OffloadService[index].u1IpMatchSize) ? FALSE : TRUE)) ||
						((Ipv6OffloadService[index].u1IpCmpType & COMPARE_BROADCAST_IP) &&
						 (((ipv6_dst_ip[0] & 0xFF00) >> 8) == 0xFF)) ||
						(Ipv6OffloadService[index].u1IpCmpType == COMPARE_NONE) || isUcIpMatch) {
						if (Ipv6OffloadService[index].u1IpMatchSize == 16 || Ipv6OffloadService[index].u1IpMatchSize == 0) {
							service = Ipv6OffloadService[index].Service;
							RTK_LOGD(TAG_WLAN_PROXY, "IPv6 :: Table Found Service[%x]\n", service);
							break;
						} else if (Ipv6OffloadService[index].u1IpMatchSize == 14 &&
								   (0x1000 <= ipv6_dst_ip[7]) &&
								   (ipv6_dst_ip[7] <= 0x13FF)) {
							service = Ipv6OffloadService[index].Service;
							RTK_LOGD(TAG_WLAN_PROXY, "IPv6 :: Table Found Service[%x]\n", service);
							break;
						}
					}

				}
			}
		} else if (type[0] == 0x88 && type[1] == 0x8e) { // 802.1x
		}
	} while (FALSE);

	return service;
}

u32 getSumValue(u8 *buf, u16 pktLen)
{
	u32 sum = 0;
	u32 word16 = 0;
	u16 i;

	// make 16 bit words out of every two adjacent 8 bit words and
	// calculate the sum of all 16 vit words
	for (i = 0; i < pktLen; i = i + 2) {
		if (pktLen & 0x1) {
			if (i == (pktLen - 1)) {
				word16 = ((((u16)buf[i]) << 8) & 0xFF00) + ((u16)0x00);
			} else {
				word16 = ((((u16)buf[i]) << 8) & 0xFF00) + (((u16)buf[i + 1]) & 0xFF);
			}
		} else {
			word16 = ((((u16)buf[i]) << 8) & 0xFF00) + (((u16)buf[i + 1]) & 0xFF);
		}
		sum = sum + word16;
	}
	return sum;
}

u16 getCheckSum(u8 protocol, u16 pktLen, u8 *src, u8 *dest, u32 input)
{
	u32 sum = 0;
	u32 word16;
	u8  i;

	sum = input;

	switch (protocol) {
	case TCP_CHECK_SUM:
	case UDP_CHECK_SUM:
		// add the UDP pseudo header which contains the IP source and destinationn addresses
		for (i = 0; i < 4; i = i + 2) {
			if (src != NULL) {
				word16 = ((((u16)src[i]) << 8) & 0xFF00) + ((u16)(src[i + 1]) & 0xFF);
				//word16 =((u16)(*(src_addr+i)<<8)&0xFF00)
				//   +((u16)(*(src_addr+i+1))&0xFF);
				sum = sum + word16;
			}
		}
		for (i = 0; i < 4; i = i + 2) {
			word16 = ((((u16)dest[i]) << 8) & 0xFF00) + (((u16)dest[i + 1]) & 0xFF);
			//word16 =((u16)(*(dest_addr+i)<<8)&0xFF00)
			//   +((u16)(*(dest_addr+i+1))&0xFF);
			sum = sum + word16;
		}
		// the protocol number and the length of the UDP packet

		sum = sum + protocol + pktLen;
		break;
	default:
		break;
	}


	// keep only the last 16 bits of the 32 bit calculated sum and add the carries
	while (sum >> 16) {
		sum = (sum & 0xFFFF) + (sum >> 16);
	}
	// Take the one's complement of sum
	sum = ~sum;

	return ((u16) sum);
}

void updateCheckSumVer2(u8 mode, u8 *buf)
{
	// offset = IP_header
	// calculate ip header check_sum and fill into TxPktBuf
	u16 checkSumValue;
	u16 headerLen;
	u16 Length;
	u32 sum = 0;
	u8 *pCheckSum;

	switch (mode) {
	case IP_CHECK_SUM:

		pCheckSum = buf + IP_CHECKSUM_OFFSET;
		// first clear up check_sum value
		memset(pCheckSum, 0, 2);
		// calculate IP check_sum and fill into inputBuf
		sum = getSumValue(buf, IP_HEADER);
		checkSumValue = getCheckSum(IP_CHECK_SUM, IP_HEADER, 0, 0, sum);

		break;

	case UDP_CHECK_SUM:

		buf += IP_HEADER;
		pCheckSum = buf + UDP_CHECK_SUM_OFFSET;
		// first clear up check_sum value
		memset(pCheckSum, 0, 2);
		// calculate UDP check_sum and fill into inputBuf
		Length = (((u16)buf[UDP_LENGTH_OFFSET]) << 8) + ((u16)(buf[UDP_LENGTH_OFFSET + 1]));

		sum = getSumValue(buf, Length); //sum of UDP header and payload

		buf -= IP_HEADER;
		checkSumValue = getCheckSum(UDP_CHECK_SUM, Length, buf + IP_SRC_ADDR_OFFSET, buf + IP_DES_ADDR_OFFSET, sum); //Add pseudo header

		break;

	case ICMPV4_CHECK_SUM:

		pCheckSum = buf + IP_HEADER + ICMPV4_CHECKSUM_OFFSET;
		// first clear up check_sum value
		memset(pCheckSum, 0, 2);
		headerLen = *buf & 0x0F;

		Length = ((((u16)buf[IP_LENGTH]) & 0x00FF) << 8)
				 + ((u16)(buf[IP_LENGTH + 1]) & 0x00FF)
				 - (headerLen * 4);

		sum = getSumValue((buf + IP_HEADER), Length);
		sum = (sum >> 16) + (sum & 0x0000FFFF);
		sum = (sum >> 16) + sum;
		checkSumValue = ~sum;

		break;

	case ICMPV6_CHECK_SUM:

		pCheckSum = buf + IPv6_HEADER + ICMPV6_CHECKSUM_OFFSET;
		// first clear up check_sum value
		memset(pCheckSum, 0, 2);
		sum = getSumValue(buf, IPv6_HEADER + ICMPV6_HEADER);
		sum = (sum >> 16) + (sum & 0x0000FFFF);
		sum = (sum >> 16) + sum;
		checkSumValue = ~sum;

		break;

	case ICMPV6_CHECK_SUM_v2:

		pCheckSum = buf + IPv6_HEADER + ICMPV6_CHECKSUM_OFFSET;
		// first clear up check_sum value
		memset(pCheckSum, 0, 2);

		Length = ((((u16)(buf[IPv6_ALEN * 2 + 2])) & 0xFF) << 8)
				 + ((u16)(buf[IPv6_ALEN * 2 + 3]) & 0xFF);

		sum = getSumValue(buf, IPv6_HEADER + Length);
		sum = (sum >> 16) + (sum & 0x0000FFFF);
		sum = (sum >> 16) + sum;
		checkSumValue = ~sum;

		break;

	case UDPV6_CHECK_SUM:

		pCheckSum = buf + IPv6_HEADER + UDP_CHECK_SUM_OFFSET;
		// first clear up check_sum value
		memset(pCheckSum, 0, 2);

		Length = ((((u16)(buf[IPv6_ALEN * 2 + 2])) & 0xFF) << 8)
				 + ((u16)(buf[IPv6_ALEN * 2 + 3]) & 0xFF);

		sum = getSumValue(buf, IPv6_HEADER + Length);
		sum = (sum >> 16) + (sum & 0x0000FFFF);
		sum = (sum >> 16) + sum;
		checkSumValue = ~sum;

		break;

	default:
		pCheckSum = buf + IP_CHECKSUM_OFFSET;
		// first clear up check_sum value
		memset(pCheckSum, 0, 2);
		// calculate IP check_sum and fill into inputBuf

		sum = getSumValue(buf, IP_HEADER);
		checkSumValue = getCheckSum(IP_CHECK_SUM, IP_HEADER, 0, 0, sum);

		break;
	}

	//update checksum value
	//MIPS: little endian, TCP/IP: big endian
	*pCheckSum = (u8)(checkSumValue >> 8);
	*(pCheckSum + 1) = (u8)(checkSumValue & 0xff);
}

void GetMDNSInfo(u8 *inbuf, u32 len)
{
	u8 i = 0;
	u8 *ptr;
	struct MDNS_PARM *pMDNS = &MDNS;

	if (pMDNS->mdns_para_buf) {
		rtos_mem_free(pMDNS->mdns_para_buf);
		pMDNS->mdns_para_buf = NULL;
	}

	pMDNS->mdns_para_buf = rtos_mem_malloc(len);
	if (!pMDNS->mdns_para_buf) {
		return;
	}

	memcpy(pMDNS->mdns_para_buf, inbuf, len);
	ptr = pMDNS->mdns_para_buf;

	/* 2.1. service name and its relative parameter */
	ptr += MDNS_TLV_TAG_SIZE;
	pMDNS->num_support_service = *ptr;
	ptr += 1;
	RTK_LOGA(TAG_WLAN_PROXY, "GetMDNSInfo() ==> num_support_service = %x\n", pMDNS->num_support_service);

	for (i = 0; i < pMDNS->num_support_service; i++) {
		struct MDNS_SUPPORT_SERVICE *pssrv = &pMDNS->service_list[i];
		memset(pssrv, 0x00, sizeof(struct MDNS_SUPPORT_SERVICE));

		pssrv->service_name.len = *ptr;
		ptr += 1;
		pssrv->service_name.str = ptr;
		ptr += pssrv->service_name.len;
		RTK_LOGA(TAG_WLAN_PROXY, "==> #len(ssrv[%x].sname) = %x\n", i, pssrv->service_name.len);

		pssrv->type_srv_rsp.len = *ptr;
		ptr += 1;
		pssrv->type_srv_rsp.str = ptr;
		ptr += pssrv->type_srv_rsp.len;
		pssrv->type_srv_target_name.len = pssrv->type_srv_rsp.len - 18;
		pssrv->type_srv_target_name.str = pssrv->type_srv_rsp.str + 16;
		RTK_LOGA(TAG_WLAN_PROXY, "==> #len(ssrv[%x].srv rsp) = %x\n", i, pssrv->type_srv_rsp.len);

		pssrv->type_txt_rsp_idx = *ptr;
		ptr += 1;
	}

	/* 2.2. machine name */
	ptr += MDNS_TLV_TAG_SIZE;
	pMDNS->num_machine_name = *ptr;
	ptr += 1;
	RTK_LOGA(TAG_WLAN_PROXY, "GetMDNSInfo() ==> num_machine_name = %x\n", pMDNS->num_machine_name);

	for (i = 0; i < pMDNS->num_machine_name; i++) {
		struct TXFF_STRING *pmnane = &pMDNS->machine_name_list[i];
		memset(pmnane, 0x00, sizeof(struct TXFF_STRING));

		pmnane->len = *ptr;
		ptr += 1;
		pmnane->str = ptr;
		ptr += pmnane->len;
		RTK_LOGA(TAG_WLAN_PROXY, "==> #len(mnane[%x]) = %x\n", i, pmnane->len);
	}

	/* 2.3. a rsp */
	ptr += MDNS_TLV_TAG_SIZE;
	pMDNS->type_a_rsp.len = *ptr;
	ptr += 1;
	pMDNS->type_a_rsp.str = ptr;
	ptr += pMDNS->type_a_rsp.len;

	/* 2.4. aaaa rsp */
	ptr += MDNS_TLV_TAG_SIZE;
	pMDNS->type_aaaa_rsp.len = *ptr;
	ptr += 1;
	pMDNS->type_aaaa_rsp.str = ptr;
	ptr += pMDNS->type_aaaa_rsp.len;

	/* 2.5. ptr rsp */
	ptr += MDNS_TLV_TAG_SIZE;
	pMDNS->type_ptr_rsp.len = *ptr;
	ptr += 1;
	pMDNS->type_ptr_rsp.str = ptr;
	ptr += pMDNS->type_ptr_rsp.len;

	/* 2.6. txt in ptr rsp */
	ptr += MDNS_TLV_TAG_SIZE;
	pMDNS->txt_in_type_ptr_rsp.len = *ptr;
	ptr += 1;
	pMDNS->txt_in_type_ptr_rsp.str = ptr;
	ptr += pMDNS->txt_in_type_ptr_rsp.len;

	/* 2.7. txt rsp */
	ptr += MDNS_TLV_TAG_SIZE;
	pMDNS->num_type_txt_rsp = *ptr;
	ptr += 1;
	RTK_LOGA(TAG_WLAN_PROXY, "GetMDNSInfo()==> num_type_txt_rsp = %x\n", pMDNS->num_type_txt_rsp);

	for (i = 0; i < pMDNS->num_type_txt_rsp; i++) {
		struct TXFF_STRING *ptxt_rsp = &pMDNS->type_txt_rsp_list[i];
		memset(ptxt_rsp, 0x00, sizeof(struct TXFF_STRING));

		memcpy(&ptxt_rsp->len, ptr, 2);
		ptr += 2;
		ptxt_rsp->str = ptr;
		ptr += ptxt_rsp->len;
		RTK_LOGA(TAG_WLAN_PROXY, "GetMDNSInfo() ==> #len(txt_rsp[%x]) = %x\n", i, ptxt_rsp->len);
	}
}

int rtw_war_offload_ctrl(u8 *param_buf)
{
	rtk_log_level_set(TAG_WLAN_PROXY, RTK_LOG_DEBUG);

	memcpy(&WAROffloadParm, param_buf, sizeof(struct H2C_WAROFFLOAD_PARM));

	if (WAROffloadParm.offload_en) {
		IPv4Parm.identifier = 1;
	}

	RTK_LOGD(TAG_WLAN_PROXY, "offload_en:%d, offload_ctrl:%x\n", WAROffloadParm.offload_en, WAROffloadParm.offload_ctrl);

	return 0;
}

#if defined(CONFIG_OFFLOAD_MDNS_V4)
bool OnMDNSv4(int idx, struct sk_buff *skb)
{
	u8        SenderMAC[6] = {0};
	u8        SenderIP[IPv4_ALEN]  = {0};
	u8        i, j, num_ptr = 0;
	struct MDNS_PARM *pMDNS = &MDNS;
	struct MDNS_RX_DATA mdns_rx;
	bool   isMDNS = FALSE;
	u8 *ptr = skb->data;

	RTK_LOGD(TAG_WLAN_PROXY, "OnMDNSv4\n");
	memset(&mdns_rx, 0x00, sizeof(struct MDNS_RX_DATA));

	do {
		memcpy(SenderMAC, ((struct ethhdr *)ptr)->h_source,  ETH_ALEN);
		memcpy(SenderIP, ptr + ETH_HLEN + IP_SRC_ADDR_OFFSET, 4);

		/* MDNS header */
		ptr += ETH_HLEN + IP_HEADER + UDP_HEADER;

		memcpy(mdns_rx.transactionId, ptr, 2);

		mdns_rx.num_question = *(ptr + MDNS_HEADER_QUE_OFFSET);
		mdns_rx.num_add = *(ptr + MDNS_HEADER_ADD_OFFSET);

		RTK_LOGD(TAG_WLAN_PROXY, "\nOnMDNSv4(): RX -- #question(%x), #add(%x)\n", mdns_rx.num_question, mdns_rx.num_add);

		/* MDNS content : query section */
		ptr += MDNS_HEADER ;

		for (i = 0; i < mdns_rx.num_question; i++, isMDNS = FALSE) {
			struct MDNS_RECORD *precord = &mdns_rx.ans_records[mdns_rx.num_ans] ;
			memset(precord, 0x00, sizeof(struct MDNS_RECORD));
			precord->first_match_node = -1;
			precord->matched_machine_name_id = -1;
			precord->matched_service_name_id = -1;
			precord->matched_target_name_id = -1;
			u8 is_machine_name_checked = FALSE;
			u8 node_idx = 0, is_message_compression_record = FALSE;

			/*  1. parsing all nodes in name content (3) */
			do {
				/* 1.1 is message compression ? */
				if (mdns_rx.is_c0_match && node_idx == 0 && (0xc0 == *ptr) && (0x0c == *(ptr + 1))) {
					/* RTK_LOGD(TAG_WLAN_PROXY, "OnMDNSv4(): match 0xc00c! (%x)\n", i); */
					precord->first_match_node = mdns_rx.ans_records[0].first_match_node;
					precord->matched_machine_name_id = mdns_rx.ans_records[0].matched_machine_name_id;
					precord->matched_service_name_id = mdns_rx.ans_records[0].matched_service_name_id;
					precord->matched_target_name_id = mdns_rx.ans_records[0].matched_target_name_id;
					is_message_compression_record = TRUE;
					ptr += 2;
					break;
				}

				if (-1 == precord->matched_service_name_id) {
					/* 1.2 match machine name */
					if (FALSE == is_machine_name_checked) {
						for (j = 0; j < pMDNS->num_machine_name; j++) {
							if (!memcmp((void *)ptr, (void *)pMDNS->machine_name_list[j].str, pMDNS->machine_name_list[j].len)) {
								precord->matched_machine_name_id = j;
								precord->first_match_node = node_idx;
								if (i == 0) {
									mdns_rx.is_c0_match = TRUE;
								}
								RTK_LOGD(TAG_WLAN_PROXY, "OnMDNSv4(): match mn idx(%x), len(%x)\n", precord->matched_machine_name_id,
										 pMDNS->machine_name_list[j].len);
								break;
							}
						}

						is_machine_name_checked = TRUE;
					}

					/* 1.3 match service name or _sub */
					if (node_idx == 1 && (!memcmp((void *)(ptr + 1), "_sub", 4))) {
						precord->is_machine_name_sub_exist = TRUE;
					} else {
						for (j = 0; j < pMDNS->num_support_service; j++) {
							if (!memcmp((void *)ptr, (void *)pMDNS->service_list[j].service_name.str, pMDNS->service_list[j].service_name.len)) {
								precord->matched_service_name_id = j;
								precord->first_match_node = node_idx;
								if (i == 0) {
									mdns_rx.is_c0_match = TRUE;
								}
								RTK_LOGD(TAG_WLAN_PROXY, "OnMDNSv4(): match sn idx(%x), len(%x)\n", precord->matched_service_name_id,
										 pMDNS->service_list[j].service_name.len);
								break;
							}
						}
					}
				}

				ptr += (*ptr + 1); /* len + #(len) */
				node_idx ++;
			} while (*ptr != 0x00);

			/* 1.4 check for target name */
			if (-1 == precord->matched_service_name_id) {
				u8 *offset_of_target_name_in_mdns_req = ptr - 6; /* len + "local" */

				if (!memcmp((void *)(offset_of_target_name_in_mdns_req + 1), "local", 5)) {
					for (j = 0; j < pMDNS->num_support_service; j++) {
						offset_of_target_name_in_mdns_req  = ptr - (6 + pMDNS->service_list[j].type_srv_target_name.len) ;

						if (!memcmp((void *)(offset_of_target_name_in_mdns_req), (void *)pMDNS->service_list[j].type_srv_target_name.str,
									pMDNS->service_list[j].type_srv_target_name.len)) {
							precord->matched_target_name_id = j;
							if (i == 0) {
								mdns_rx.is_c0_match = TRUE;
							}
							break;
						}
					}
				}
			}

			/*  1.5 for delimiter */
			if (!is_message_compression_record) {
				ptr += 1;
			}

			/* 2. parsing type/qu of matched record */
			if ((-1 != precord->matched_service_name_id) || (-1 != precord->matched_target_name_id)) {

				memcpy(precord->type, ptr, 2);
				ptr += 2;

				/* check req. type and machine name condition */
				if (IS_SUPPORT_DNS_TYPE(precord->type, DNS_TYPE_A) && IS_TARGET_NAME_MATCHED(precord)) {
					isMDNS = TRUE;
				} else if (IS_SUPPORT_DNS_TYPE(precord->type, DNS_TYPE_AAAA) && IS_TARGET_NAME_MATCHED(precord)) {
					isMDNS = TRUE;
				} else if (IS_SUPPORT_DNS_TYPE(precord->type, DNS_TYPE_PTR) &&
						   (IS_NO_MACHINE_NAME(precord) || IS_MACHINE_NAME_MATCHED(precord))) {
					isMDNS = TRUE;
					num_ptr++;
				} else if (IS_SUPPORT_DNS_TYPE(precord->type, DNS_TYPE_SRV) && IS_MACHINE_NAME_MATCHED(precord)) {
					isMDNS = TRUE;
				} else if (IS_SUPPORT_DNS_TYPE(precord->type, DNS_TYPE_TXT) && IS_MACHINE_NAME_MATCHED(precord)) {
					isMDNS = TRUE;
				}

				if (isMDNS) {
					precord->is_multicast = (0x00 == *ptr) ? TRUE : FALSE;
					mdns_rx.num_ans++;

					RTK_LOGD(TAG_WLAN_PROXY, "OnMDNSv4(): add an answer, num_ans[%x] => type(%x), #mn(%x), #sn(%x)\n",
							 mdns_rx.num_ans, precord->type[1],
							 precord->matched_machine_name_id, precord->matched_service_name_id);
				}
				ptr += 2;
			} else {
				memset(precord, 0x00, sizeof(struct MDNS_RECORD));
				RTK_LOGD(TAG_WLAN_PROXY, "OnMDNSv4(): no match for record[%x]\n", i);
				ptr += 4;
			}
		}

		/* MDNS content : additional section */
		if ((1 == mdns_rx.num_add) && (1 == num_ptr)) {
			if ((0x00 == *(ptr + 1)) && (0x29 == *(ptr + 2))) {
				mdns_rx.num_add = 3;
			}
		} else {
			mdns_rx.num_add = 0;
		}

		/* End of parsing; Do the corresponsive actions (Issue rsp or/and.wakeup) */
		if (0 == mdns_rx.num_ans) {
			RTK_LOGD(TAG_WLAN_PROXY, "no ans, return\n");
			break;
		}

		if (WAROffloadParm.offload_ctrl & WAR_MDNS_V4_RSP_EN)
			for (i = 0; i < mdns_rx.num_ans; i++) {
				issue_MDNS_v4_rsp(idx, SenderIP, SenderMAC, &mdns_rx, i);
			}

		if (WAROffloadParm.offload_ctrl & WAR_MDNS_V4_WAKEUP_EN) {
			// TODO
		}
	} while (FALSE);

	return mdns_rx.num_ans;
}

void issue_MDNS_v4_rsp(int idx, u8 *TargetIP, u8 *TargetMAC, struct MDNS_RX_DATA *mdns_rx_data, u8  issue_no)
{
	u16 TotalLen = 0;
	u8  multicast_mac_addr[] = {0x01, 0x00, 0x5e, 0x00, 0x00, 0xfb};
	u8  multicast_ip_addr[] = {0xe0, 0x00, 0x00, 0xfb};
	u8  machine_name_sub[] = {0x04, 0x5f, 0x73, 0x75, 0x62}; /* 04+"_sub" */
	u8  domain_local[] = {0x05, 0x6c, 0x6f, 0x63, 0x61, 0x6c, 0x00}; /* 05+"local"+delimiter (7) */
	struct MDNS_PARM *pMDNS = &MDNS;
	u8 iface_type = idx;
	struct sk_buff *skb = NULL;
	u8 *ptr, *udp_hdr, *ip_hdr;

	RTK_LOGD(TAG_WLAN_PROXY, "==> issue_MDNS_v4_rsp \n");

	rtos_critical_enter();
	if (rtw_is_netdev_enable(idx)) {
		wifi_if_tx_inc(idx);
	} else {
		RTK_LOGE(TAG_WLAN_DRV, "[%s] netif is DOWN\n", __FUNCTION__);
		rtos_critical_exit();
		return;
	}
	rtos_critical_exit();

	skb = wifi_if_alloc_skb(WLAN_MAX_ETHFRM_LEN);
	if (skb == NULL) {
		goto exit;
	}
	ptr = skb->data;

	/* fill Ethernet Header */
	if (mdns_rx_data->ans_records[issue_no].is_multicast) {
		memcpy(ptr, multicast_mac_addr, ETH_ALEN);
	} else {
		memcpy(ptr, TargetMAC, ETH_ALEN);
	}
	ptr += ETH_ALEN;

	memcpy(ptr, rtw_get_adapter_mac_addr(iface_type), ETH_ALEN);
	ptr += ETH_ALEN;

	ptr[0] = 0x08;
	ptr[1] = 0x00;
	ptr += ETH_TYPE_LEN;

	/* fill IPv4 Header */
	IPv4Parm.identifier++; // identifier
	ip_hdr = ptr;

	SET_IPHDR_VERSION(ptr, 0x45);
	SET_IPHDR_DSCP(ptr, 0);
	SET_IPHDR_IDENTIFIER(ptr, _htons(IPv4Parm.identifier));
	SET_IPHDR_FLAGS(ptr, 0x40);
	SET_IPHDR_FRAG_OFFSET(ptr, 0);
	SET_IPHDR_TTL(ptr, 0x40);
	SET_IPHDR_PROTOCOL(ptr, 0x11);  // ICMP-UDP
	SET_IPHDR_SRC_IP_ADDR(ptr, IPv4Parm.IP);

	if (mdns_rx_data->ans_records[issue_no].is_multicast) {
		SET_IPHDR_DST_IP_ADDR(ptr, multicast_ip_addr);
	} else {
		SET_IPHDR_DST_IP_ADDR(ptr, TargetIP);
	}
	ptr += IP_HEADER;

	/* fill UDP Header */
	udp_hdr = ptr;
	SET_UDP_SRC_PORT(ptr, _htons(0x14e9)); // MDNS port: 5353
	SET_UDP_DST_PORT(ptr, _htons(0x14e9));
	ptr += UDP_HEADER;

	/* mDNS header : Update */
	/* TotalLen ==> calculate for the length from mdns header to the end.   */
	memset(ptr, 0, MDNS_HEADER);

	ptr[0] = mdns_rx_data->transactionId[0];
	ptr[1] = mdns_rx_data->transactionId[1];
	SET_MDNS_HDR_FLAG(ptr, 0x84);
	ptr[MDNS_NUM_ANS] = 0x01;
	ptr[MDNS_NUM_ADD] = mdns_rx_data->num_add;

	TotalLen += MDNS_HEADER;

	/* mDNS answer RRs */
	u8 mid = mdns_rx_data->ans_records[issue_no].matched_machine_name_id;
	u8 sid = mdns_rx_data->ans_records[issue_no].matched_service_name_id;
	u8 tid = mdns_rx_data->ans_records[issue_no].matched_target_name_id;
	u8 txt_idx = 0xff;

	if (IS_SUPPORT_DNS_TYPE(mdns_rx_data->ans_records[issue_no].type, DNS_TYPE_A) ||
		IS_SUPPORT_DNS_TYPE(mdns_rx_data->ans_records[issue_no].type, DNS_TYPE_AAAA)) {
		/* fill DNS Host name  (such as, KM1.local ) */
		memcpy(ptr + TotalLen, pMDNS->service_list[tid].type_srv_target_name.str, pMDNS->service_list[tid].type_srv_target_name.len);
		TotalLen += pMDNS->service_list[tid].type_srv_target_name.len;

		memcpy(ptr + TotalLen, domain_local, domain_local[0] + 2);
		TotalLen += (domain_local[0] + 2);
	} else {
		/*  fill machine name  (such as, _universal._sub ) */
		if (mid != 0xff) {
			memcpy(ptr + TotalLen, pMDNS->machine_name_list[mid].str, pMDNS->machine_name_list[mid].len);
			TotalLen += pMDNS->machine_name_list[mid].len;

			if (mdns_rx_data->ans_records[issue_no].is_machine_name_sub_exist) {
				memcpy(ptr + TotalLen, machine_name_sub, machine_name_sub[0] + 1);
				TotalLen += (machine_name_sub[0] + 1);
			}
		}

		/* fill service name   (such as, _ipp._tcp.local) */
		memcpy(ptr + TotalLen, pMDNS->service_list[sid].service_name.str, pMDNS->service_list[sid].service_name.len);
		TotalLen += pMDNS->service_list[sid].service_name.len;
	}

	/* fill rsp depend on different type */
	if (IS_SUPPORT_DNS_TYPE(mdns_rx_data->ans_records[issue_no].type, DNS_TYPE_A)) {
		memcpy(ptr + TotalLen, pMDNS->type_a_rsp.str, pMDNS->type_a_rsp.len);
		TotalLen += pMDNS->type_a_rsp.len;

	} else if (IS_SUPPORT_DNS_TYPE(mdns_rx_data->ans_records[issue_no].type, DNS_TYPE_AAAA)) {
		memcpy(ptr + TotalLen, pMDNS->type_aaaa_rsp.str, pMDNS->type_aaaa_rsp.len);
		TotalLen += pMDNS->type_aaaa_rsp.len;

	} else if (IS_SUPPORT_DNS_TYPE(mdns_rx_data->ans_records[issue_no].type, DNS_TYPE_SRV)) {
		memcpy(ptr + TotalLen, pMDNS->service_list[sid].type_srv_rsp.str, pMDNS->service_list[sid].type_srv_rsp.len);
		TotalLen += pMDNS->service_list[sid].type_srv_rsp.len;

		/* update offset (the last byte in target) */
		if (mid != 0xff) {
			if (mdns_rx_data->ans_records[issue_no].is_machine_name_sub_exist) {
				ptr[TotalLen - 1] = MDNS_HEADER + pMDNS->machine_name_list[mid].len + 5 + pMDNS->service_list[sid].service_name.len - 7;
			} else {
				ptr[TotalLen - 1] = MDNS_HEADER + pMDNS->machine_name_list[mid].len + pMDNS->service_list[sid].service_name.len - 7;
			}
			/* 5=1+ len(_sub); 7=1+len(local)+delimter */
		}
	} else if (IS_SUPPORT_DNS_TYPE(mdns_rx_data->ans_records[issue_no].type, DNS_TYPE_TXT)) {
		txt_idx = pMDNS->service_list[sid].type_txt_rsp_idx;

		memcpy(ptr + TotalLen, pMDNS->type_txt_rsp_list[txt_idx].str, pMDNS->type_txt_rsp_list[txt_idx].len);
		TotalLen += pMDNS->type_txt_rsp_list[txt_idx].len;

	} else if (IS_SUPPORT_DNS_TYPE(mdns_rx_data->ans_records[issue_no].type, DNS_TYPE_PTR)) {

		memcpy(ptr + TotalLen, pMDNS->type_ptr_rsp.str, pMDNS->type_ptr_rsp.len);
		TotalLen += pMDNS->type_ptr_rsp.len;

		/*  update offset (the last byte in target) */
		if (mid != 0xff) {
			if (mdns_rx_data->ans_records[issue_no].is_machine_name_sub_exist) {
				ptr[TotalLen - 1] = MDNS_HEADER + pMDNS->machine_name_list[mid].len + 5;
			} else {
				ptr[TotalLen - 1] = MDNS_HEADER + pMDNS->machine_name_list[mid].len;
			}
		} else {
			ptr[TotalLen - 1] = MDNS_HEADER;
		}
	}

	/* mDNS additional RRs */
	if (3 == mdns_rx_data->num_add) {
		u8 mid = mdns_rx_data->ans_records[0].matched_machine_name_id;
		u8 sid = mdns_rx_data->ans_records[0].matched_service_name_id;
		/* offset in mdns rsp */
		u8 name_offset  = MDNS_HEADER + pMDNS->machine_name_list[mid].len + pMDNS->service_list[sid].service_name.len + 10;
		u8 target_postfix_offset = MDNS_HEADER + pMDNS->machine_name_list[mid].len + pMDNS->service_list[sid].service_name.len -  7;

		RTK_LOGD(TAG_WLAN_PROXY, "==> issue_MDNS_v4_rsp : additional RR! \n");

		if (mdns_rx_data->ans_records[issue_no].is_machine_name_sub_exist) {
			name_offset += 5;
			target_postfix_offset += 5;
		}

		/* 1. Type - SRV : message comprresion */
		ptr[TotalLen] = 0xc0;
		ptr[TotalLen + 1] = name_offset;
		TotalLen += 2;

		/*      Type - SRV : rsp  */
		memcpy(ptr + TotalLen, pMDNS->service_list[sid].type_srv_rsp.str, pMDNS->service_list[sid].type_srv_rsp.len);
		TotalLen += pMDNS->service_list[sid].type_srv_rsp.len;

		/*      Type - SRV : rsp, update offset (the last two byte in target) */
		ptr[TotalLen - 2] = 0xc0;
		ptr[TotalLen - 1] = target_postfix_offset;// local

		/* 2. Type - TXT : message comprresion */
		ptr[TotalLen] = 0xc0;
		ptr[TotalLen + 1] = name_offset;// domain name
		TotalLen += 2;

		/* 2. Type - TXT : rsp */
		memcpy(ptr + TotalLen, pMDNS->type_txt_rsp_list[pMDNS->service_list[sid].type_txt_rsp_idx].str,
			   pMDNS->type_txt_rsp_list[pMDNS->service_list[sid].type_txt_rsp_idx].len);
		TotalLen += pMDNS->type_txt_rsp_list[pMDNS->service_list[sid].type_txt_rsp_idx].len;

		/* 3. Type - A : DNS-host name  (len + target name) */
		memcpy(ptr + TotalLen, pMDNS->service_list[sid].type_srv_target_name.str, pMDNS->service_list[sid].type_srv_target_name.len);
		TotalLen += pMDNS->service_list[sid].type_srv_target_name.len;

		/* 3. Type - A : message comprresion  (len + target name + local ) */
		ptr[TotalLen] = 0xc0;
		ptr[TotalLen + 1] = target_postfix_offset;// domain name
		TotalLen += 2;

		/* 3. Type - A : rsp */
		memcpy(ptr + TotalLen, pMDNS->type_a_rsp.str, pMDNS->type_a_rsp.len);
		TotalLen += pMDNS->type_a_rsp.len;
	}
	ptr += TotalLen;

	/* Update length info (3) */

	/*   1. Update len. and checksum in UDP */
	TotalLen += UDP_HEADER;

	SET_UDP_LEN(udp_hdr, _htons(TotalLen));
	updateCheckSumVer2(UDP_CHECK_SUM, ip_hdr);

	/*   2. Update len. and checksum in IPv4 */
	TotalLen += IP_HEADER;

	SET_IPHDR_TOTAL_LEN(ip_hdr, _htons(TotalLen));
	updateCheckSumVer2(IP_CHECK_SUM, ip_hdr);

	/*	3. Update skb len*/
	skb_put(skb, ptr - skb->data);

	/* Send !! */
	wifi_if_send_skb(idx, skb);

	RTK_LOGD(TAG_WLAN_PROXY, "==> issue_MDNS_v4_rsp : no.%x - TotalLen = %x\n", issue_no, TotalLen);

exit:
	rtos_critical_enter();
	wifi_if_tx_dec(idx);
	rtos_critical_exit();
	return;
}
#endif /* CONFIG_OFFLOAD_MDNS_V4 */

#if defined(CONFIG_OFFLOAD_MDNS_V6)

bool OnMDNSv6(int idx, struct sk_buff *skb)
{
	u8        SenderMAC[6] = {0};
	u8        SenderIP[IPv6_ALEN]  = {0};
	u8        i, j, num_ptr = 0;
	struct MDNS_PARM *pMDNS = &MDNS;
	struct MDNS_RX_DATA mdns_rx;
	bool   isMDNS = FALSE;
	u8 *ptr = skb->data;

	RTK_LOGD(TAG_WLAN_PROXY, "OnMDNSv6\n");
	memset(&mdns_rx, 0x00, sizeof(struct MDNS_RX_DATA));

	do {
		memcpy(SenderMAC, ((struct ethhdr *)ptr)->h_source,  ETH_ALEN);
		memcpy(SenderIP, ptr + ETH_HLEN + IPv6_SRC_ADDRESS_OFFSET, IPv6_ALEN);

		/* MDNS header */
		ptr += ETH_HLEN + IPv6_HEADER + UDP_HEADER;

		memcpy(mdns_rx.transactionId, ptr, 2);

		mdns_rx.num_question = *(ptr + MDNS_HEADER_QUE_OFFSET);
		mdns_rx.num_add = *(ptr + MDNS_HEADER_ADD_OFFSET);

		RTK_LOGD(TAG_WLAN_PROXY, "\nOnMDNSv6(): RX -- #question(%x), #add(%x)\n", mdns_rx.num_question, mdns_rx.num_add);

		/* MDNS content : query section */
		ptr += MDNS_HEADER ;

		for (i = 0; i < mdns_rx.num_question; i++, isMDNS = FALSE) {
			struct MDNS_RECORD *precord = &mdns_rx.ans_records[mdns_rx.num_ans] ;
			memset(precord, 0x00, sizeof(struct MDNS_RECORD));
			precord->first_match_node = -1;
			precord->matched_machine_name_id = -1;
			precord->matched_service_name_id = -1;
			precord->matched_target_name_id = -1;
			u8 is_machine_name_checked = FALSE;
			u8 node_idx = 0, is_message_compression_record = FALSE;

			/*  1. parsing all nodes in name content (3) */
			do {
				/* 1.1 is message compression ? */
				if (mdns_rx.is_c0_match && node_idx == 0 && (0xc0 == *ptr) && (0x0c == *(ptr + 1))) {
					/* RTK_LOGD(TAG_WLAN_PROXY, "OnMDNSv6(): match 0xc00c! (%x)\n", i); */
					precord->first_match_node = mdns_rx.ans_records[0].first_match_node;
					precord->matched_machine_name_id = mdns_rx.ans_records[0].matched_machine_name_id;
					precord->matched_service_name_id = mdns_rx.ans_records[0].matched_service_name_id;
					precord->matched_target_name_id = mdns_rx.ans_records[0].matched_target_name_id;
					is_message_compression_record = TRUE;
					ptr += 2;
					break;
				}

				if (-1 == precord->matched_service_name_id) {
					/* 1.2 match machine name */
					if (FALSE == is_machine_name_checked) {
						for (j = 0; j < pMDNS->num_machine_name; j++) {
							if (!memcmp((void *)ptr, (void *)pMDNS->machine_name_list[j].str, pMDNS->machine_name_list[j].len)) {
								precord->matched_machine_name_id = j;
								precord->first_match_node = node_idx;
								if (i == 0) {
									mdns_rx.is_c0_match = TRUE;
								}
								RTK_LOGD(TAG_WLAN_PROXY, "OnMDNSv6(): match mn idx(%x), len(%x)\n", precord->matched_machine_name_id,
										 pMDNS->machine_name_list[j].len);
								break;
							}
						}

						is_machine_name_checked = TRUE;
					}

					/* 1.3 match service name or _sub */
					if (node_idx == 1 && (!memcmp((void *)(ptr + 1), "_sub", 4))) {
						precord->is_machine_name_sub_exist = TRUE;
					} else {
						for (j = 0; j < pMDNS->num_support_service; j++) {
							if (!memcmp((void *)ptr, (void *)pMDNS->service_list[j].service_name.str, 	pMDNS->service_list[j].service_name.len)) {
								precord->matched_service_name_id = j;
								precord->first_match_node = node_idx;
								if (i == 0) {
									mdns_rx.is_c0_match = TRUE;
								}
								RTK_LOGD(TAG_WLAN_PROXY, "OnMDNSv6(): match sn idx(%x), len(%x)\n", precord->matched_service_name_id,
										 pMDNS->service_list[j].service_name.len);
								break;
							}
						}
					}
				}

				ptr += (*ptr + 1); /* len + #(len) */
				node_idx ++;
			} while (*ptr != 0x00);

			/* 1.4 check for target name */
			if (-1 == precord->matched_service_name_id) {
				u8 *offset_of_target_name_in_mdns_req = ptr - 6; /* len + "local" */

				if (!memcmp((void *)(offset_of_target_name_in_mdns_req + 1), "local", 5)) {
					for (j = 0; j < pMDNS->num_support_service; j++) {
						offset_of_target_name_in_mdns_req  = ptr - (6 + pMDNS->service_list[j].type_srv_target_name.len) ;

						if (!memcmp((void *)(offset_of_target_name_in_mdns_req), (void *)pMDNS->service_list[j].type_srv_target_name.str,
									pMDNS->service_list[j].type_srv_target_name.len)) {
							precord->matched_target_name_id = j;
							if (i == 0) {
								mdns_rx.is_c0_match = TRUE;
							}
							break;
						}
					}
				}
			}

			/*  1.5 for delimiter */
			if (!is_message_compression_record) {
				ptr += 1;
			}

			/* 2. parsing type/qu of matched record */
			if ((-1 != precord->matched_service_name_id) || (-1 != precord->matched_target_name_id)) {

				memcpy(precord->type, ptr, 2);
				ptr += 2;

				/* check req. type and machine name condition */
				if (IS_SUPPORT_DNS_TYPE(precord->type, DNS_TYPE_A) && IS_TARGET_NAME_MATCHED(precord)) {
					isMDNS = TRUE;
				} else if (IS_SUPPORT_DNS_TYPE(precord->type, DNS_TYPE_AAAA) && IS_TARGET_NAME_MATCHED(precord)) {
					isMDNS = TRUE;
				} else if (IS_SUPPORT_DNS_TYPE(precord->type, DNS_TYPE_PTR) &&
						   (IS_NO_MACHINE_NAME(precord) || IS_MACHINE_NAME_MATCHED(precord))) {
					isMDNS = TRUE;
					num_ptr++;
				} else if (IS_SUPPORT_DNS_TYPE(precord->type, DNS_TYPE_SRV) && IS_MACHINE_NAME_MATCHED(precord)) {
					isMDNS = TRUE;
				} else if (IS_SUPPORT_DNS_TYPE(precord->type, DNS_TYPE_TXT) && IS_MACHINE_NAME_MATCHED(precord)) {
					isMDNS = TRUE;
				}

				if (isMDNS) {
					precord->is_multicast = (0x00 == *ptr) ? TRUE : FALSE;
					mdns_rx.num_ans++;

					RTK_LOGD(TAG_WLAN_PROXY, "OnMDNSv6(): add an answer, num_ans[%x] => type(%x), #mn(%x), #sn(%x)\n",
							 mdns_rx.num_ans, precord->type[1],
							 precord->matched_machine_name_id, precord->matched_service_name_id);
				}
				ptr += 2;
			} else {
				memset(precord, 0x00, sizeof(struct MDNS_RECORD));
				RTK_LOGD(TAG_WLAN_PROXY, "OnMDNSv6(): no match for record[%x]\n", i);
				ptr += 4;
			}
		}

		/* MDNS content : additional section */
		if ((1 == mdns_rx.num_add) && (1 == num_ptr)) {
			if ((0x00 == *(ptr + 1)) && (0x29 == *(ptr + 2))) {
				mdns_rx.num_add = 3;
			}
		} else {
			mdns_rx.num_add = 0;
		}

		/* End of parsing; Do the corresponsive actions (Issue rsp or/and.wakeup) */
		if (0 == mdns_rx.num_ans) {
			break;
		}

		if (WAROffloadParm.offload_ctrl & WAR_MDNS_V6_RSP_EN)
			for (i = 0; i < mdns_rx.num_ans; i++) {
				issue_MDNS_v6_rsp(idx, SenderIP, SenderMAC, &mdns_rx, i);
			}

		if (WAROffloadParm.offload_ctrl & WAR_MDNS_V6_WAKEUP_EN) {
			// TODO
		}
	} while (FALSE);

	return mdns_rx.num_ans;
}

void issue_MDNS_v6_rsp(int idx, u8 *TargetIP, u8 *TargetMAC, struct MDNS_RX_DATA *mdns_rx_data, u8  issue_no)
{
	u16 TotalLen = 0;
	u8  multicast_mac_addr[] = {0x33, 0x33, 0x00, 0x00, 0x00, 0xfb};
	u8  multicast_ip_addr[] = {0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfb};
	u8  machine_name_sub[] = {0x04, 0x5f, 0x73, 0x75, 0x62}; /* 04+"_sub" */
	u8  domain_local[] = {0x05, 0x6c, 0x6f, 0x63, 0x61, 0x6c, 0x00}; /* 05+"local"+delimiter (7) */
	struct MDNS_PARM *pMDNS = &MDNS;
	u8 iface_type = idx;
	struct sk_buff *skb = NULL;
	u8 *ptr, *udp_hdr, *ip_hdr;

	RTK_LOGD(TAG_WLAN_PROXY, "==> issue_MDNS_v6_rsp \n");

	rtos_critical_enter();
	if (rtw_is_netdev_enable(idx)) {
		wifi_if_tx_inc(idx);
	} else {
		RTK_LOGE(TAG_WLAN_DRV, "[%s] netif is DOWN\n", __FUNCTION__);
		rtos_critical_exit();
		return;
	}
	rtos_critical_exit();

	skb = wifi_if_alloc_skb(WLAN_MAX_ETHFRM_LEN);
	if (skb == NULL) {
		goto exit;
	}
	ptr = skb->data;

	/* fill Ethernet Header */
	if (mdns_rx_data->ans_records[issue_no].is_multicast) {
		memcpy(ptr, multicast_mac_addr, ETH_ALEN);
	} else {
		memcpy(ptr, TargetMAC, ETH_ALEN);
	}
	ptr += ETH_ALEN;

	memcpy(ptr, rtw_get_adapter_mac_addr(iface_type), ETH_ALEN);
	ptr += ETH_ALEN;

	ptr[0] = 0x86;
	ptr[1] = 0xDD;
	ptr += ETH_TYPE_LEN;

	/* fill IPv6 Header later */
	ip_hdr = ptr;
	ptr += IPv6_HEADER;

	/* fill UDP Header */
	udp_hdr = ptr;
	SET_UDP_SRC_PORT(ptr, _htons(0x14e9)); // MDNS port: 5353
	SET_UDP_DST_PORT(ptr, _htons(0x14e9));
	ptr += UDP_HEADER;

	/* mDNS header : Update */
	/* TotalLen ==> calculate for the length from mdns header to the end.   */
	memset(ptr, 0, MDNS_HEADER);

	ptr[0] = mdns_rx_data->transactionId[0];
	ptr[1] = mdns_rx_data->transactionId[1];
	SET_MDNS_HDR_FLAG(ptr, 0x84);
	ptr[MDNS_NUM_ANS] = 0x01;
	ptr[MDNS_NUM_ADD] = mdns_rx_data->num_add;

	TotalLen += MDNS_HEADER;

	/* mDNS answer RRs */
	u8 mid = mdns_rx_data->ans_records[issue_no].matched_machine_name_id;
	u8 sid = mdns_rx_data->ans_records[issue_no].matched_service_name_id;
	u8 tid = mdns_rx_data->ans_records[issue_no].matched_target_name_id;
	u8 txt_idx = 0xff;

	if (IS_SUPPORT_DNS_TYPE(mdns_rx_data->ans_records[issue_no].type, DNS_TYPE_A) ||
		IS_SUPPORT_DNS_TYPE(mdns_rx_data->ans_records[issue_no].type, DNS_TYPE_AAAA)) {
		/* fill DNS Host name  (such as, KM1.local ) */
		memcpy(ptr + TotalLen, pMDNS->service_list[tid].type_srv_target_name.str, pMDNS->service_list[tid].type_srv_target_name.len);
		TotalLen += pMDNS->service_list[tid].type_srv_target_name.len;

		memcpy(ptr + TotalLen, domain_local, domain_local[0] + 2);
		TotalLen += (domain_local[0] + 2);
	} else {
		/*  fill machine name  (such as, _universal._sub ) */
		if (mid != 0xff) {
			memcpy(ptr + TotalLen, pMDNS->machine_name_list[mid].str, pMDNS->machine_name_list[mid].len);
			TotalLen += pMDNS->machine_name_list[mid].len;

			if (mdns_rx_data->ans_records[issue_no].is_machine_name_sub_exist) {
				memcpy(ptr + TotalLen, machine_name_sub, machine_name_sub[0] + 1);
				TotalLen += (machine_name_sub[0] + 1);
			}
		}

		/* fill service name   (such as, _ipp._tcp.local) */
		memcpy(ptr + TotalLen, pMDNS->service_list[sid].service_name.str, pMDNS->service_list[sid].service_name.len);
		TotalLen += pMDNS->service_list[sid].service_name.len;
	}

	/* fill rsp depend on different type */
	if (IS_SUPPORT_DNS_TYPE(mdns_rx_data->ans_records[issue_no].type, DNS_TYPE_A)) {
		memcpy(ptr + TotalLen, pMDNS->type_a_rsp.str, pMDNS->type_a_rsp.len);
		TotalLen += pMDNS->type_a_rsp.len;

	} else if (IS_SUPPORT_DNS_TYPE(mdns_rx_data->ans_records[issue_no].type, DNS_TYPE_AAAA)) {
		memcpy(ptr + TotalLen, pMDNS->type_aaaa_rsp.str, pMDNS->type_aaaa_rsp.len);
		TotalLen += pMDNS->type_aaaa_rsp.len;

	} else if (IS_SUPPORT_DNS_TYPE(mdns_rx_data->ans_records[issue_no].type, DNS_TYPE_SRV)) {
		memcpy(ptr + TotalLen, pMDNS->service_list[sid].type_srv_rsp.str, pMDNS->service_list[sid].type_srv_rsp.len);
		TotalLen += pMDNS->service_list[sid].type_srv_rsp.len;

		/* update offset (the last byte in target) */
		if (mid != 0xff) {
			if (mdns_rx_data->ans_records[issue_no].is_machine_name_sub_exist) {
				ptr[TotalLen - 1] = MDNS_HEADER + pMDNS->machine_name_list[mid].len + 5 + pMDNS->service_list[sid].service_name.len - 7;
			} else {
				ptr[TotalLen - 1] = MDNS_HEADER + pMDNS->machine_name_list[mid].len + pMDNS->service_list[sid].service_name.len - 7;
			}
			/* 5=1+ len(_sub); 7=1+len(local)+delimter */
		}
	} else if (IS_SUPPORT_DNS_TYPE(mdns_rx_data->ans_records[issue_no].type, DNS_TYPE_TXT)) {
		txt_idx = pMDNS->service_list[sid].type_txt_rsp_idx;

		memcpy(ptr + TotalLen, pMDNS->type_txt_rsp_list[txt_idx].str, pMDNS->type_txt_rsp_list[txt_idx].len);
		TotalLen += pMDNS->type_txt_rsp_list[txt_idx].len;

	} else if (IS_SUPPORT_DNS_TYPE(mdns_rx_data->ans_records[issue_no].type, DNS_TYPE_PTR)) {

		memcpy(ptr + TotalLen, pMDNS->type_ptr_rsp.str, pMDNS->type_ptr_rsp.len);
		TotalLen += pMDNS->type_ptr_rsp.len;

		/*  update offset (the last byte in target) */
		if (mid != 0xff) {
			if (mdns_rx_data->ans_records[issue_no].is_machine_name_sub_exist) {
				ptr[TotalLen - 1] = MDNS_HEADER + pMDNS->machine_name_list[mid].len + 5;
			} else {
				ptr[TotalLen - 1] = MDNS_HEADER + pMDNS->machine_name_list[mid].len;
			}
		} else {
			ptr[TotalLen - 1] = MDNS_HEADER;
		}
	}

	/* mDNS additional RRs */
	if (3 == mdns_rx_data->num_add) {
		u8 mid = mdns_rx_data->ans_records[0].matched_machine_name_id;
		u8 sid = mdns_rx_data->ans_records[0].matched_service_name_id;
		/* offset in mdns rsp */
		u8 name_offset  = MDNS_HEADER + pMDNS->machine_name_list[mid].len + pMDNS->service_list[sid].service_name.len + 10;
		u8 target_postfix_offset = MDNS_HEADER + pMDNS->machine_name_list[mid].len + pMDNS->service_list[sid].service_name.len -  7;

		RTK_LOGD(TAG_WLAN_PROXY, "==> issue_MDNS_v6_rsp : additional RR! \n");

		if (mdns_rx_data->ans_records[issue_no].is_machine_name_sub_exist) {
			name_offset += 5;
			target_postfix_offset += 5;
		}

		/* 1. Type - SRV : message comprresion */
		ptr[TotalLen] = 0xc0;
		ptr[TotalLen + 1] = name_offset;
		TotalLen += 2;

		/*      Type - SRV : rsp  */
		memcpy(ptr + TotalLen, pMDNS->service_list[sid].type_srv_rsp.str, pMDNS->service_list[sid].type_srv_rsp.len);
		TotalLen += pMDNS->service_list[sid].type_srv_rsp.len;

		/*      Type - SRV : rsp, update offset (the last two byte in target) */
		ptr[TotalLen - 2] = 0xc0;
		ptr[TotalLen - 1] = target_postfix_offset;// local

		/* 2. Type - TXT : message comprresion */
		ptr[TotalLen] = 0xc0;
		ptr[TotalLen + 1] = name_offset;// domain name
		TotalLen += 2;

		/* 2. Type - TXT : rsp */
		memcpy(ptr + TotalLen, pMDNS->type_txt_rsp_list[pMDNS->service_list[sid].type_txt_rsp_idx].str,
			   pMDNS->type_txt_rsp_list[pMDNS->service_list[sid].type_txt_rsp_idx].len);
		TotalLen += pMDNS->type_txt_rsp_list[pMDNS->service_list[sid].type_txt_rsp_idx].len;

		/* 3. Type - AAAA : DNS-host name  (len + target name) */
		memcpy(ptr + TotalLen, pMDNS->service_list[sid].type_srv_target_name.str, pMDNS->service_list[sid].type_srv_target_name.len);
		TotalLen += pMDNS->service_list[sid].type_srv_target_name.len;

		/* 3. Type - AAAA : message comprresion  (len + target name + local ) */
		ptr[TotalLen] = 0xc0;
		ptr[TotalLen + 1] = target_postfix_offset;// domain name
		TotalLen += 2;

		/* 3. Type - AAAA : rsp */
		memcpy(ptr + TotalLen, pMDNS->type_aaaa_rsp.str, pMDNS->type_aaaa_rsp.len);
		TotalLen += pMDNS->type_aaaa_rsp.len;
	}
	ptr += TotalLen;

	/* Update length info (3) */

	/*   1. Update len. and checksum in UDP */
	TotalLen += UDP_HEADER;
	SET_UDP_LEN(udp_hdr, _htons(TotalLen));

	/* IP header length = 8(UDP_LEN) + UDP length */
	SET_IPHDRV6_PAYLOAD_LENGTH(ip_hdr, _htons(TotalLen));
	SET_IPHDRV6_NEXT_HEADER(ip_hdr, 0x11);
	SET_IPHDRV6_HOP_LIMIT(ip_hdr, 0xFF);
	TotalLen += IPv6_HEADER;

	/* Fill pseudo-header IPv6 source & destination address */
	memcpy(ip_hdr, IPv6Parm.IP, IPv6_ALEN);
	if (mdns_rx_data->ans_records[issue_no].is_multicast) {
		memcpy(ip_hdr + IPv6_ALEN, multicast_ip_addr, IPv6_ALEN);
	} else {
		memcpy(ip_hdr + IPv6_ALEN, TargetIP, IPv6_ALEN);
	}

	ip_hdr[IPv6_ALEN * 2] = 0x00;
	ip_hdr[IPv6_ALEN * 2 + 1] = 0x00;
	ip_hdr[IPv6_ALEN * 2 + 2] = (u8)((TotalLen - IPv6_HEADER) >> 8);
	ip_hdr[IPv6_ALEN * 2 + 3] = (u8)((TotalLen - IPv6_HEADER) & 0xFF);
	ip_hdr[IPv6_ALEN * 2 + 4] = 0x00;
	ip_hdr[IPv6_ALEN * 2 + 5] = 0x00;
	ip_hdr[IPv6_ALEN * 2 + 6] = 0x00;
	ip_hdr[IPv6_ALEN * 2 + 7] = 0x11;

	/* UDP Header Checksum */
	updateCheckSumVer2(UDPV6_CHECK_SUM, ip_hdr);

	/* Recover IPv6 header */
	ip_hdr[0] = 0x60;
	ip_hdr[1] = 0x00;
	ip_hdr[2] = 0x00;
	ip_hdr[3] = 0x00;

	/* Fill Source & Destination IP of IP Header */
	memcpy(ip_hdr + IPv6_SRC_ADDRESS_OFFSET, IPv6Parm.IP, IPv6_ALEN);
	if (mdns_rx_data->ans_records[issue_no].is_multicast) {
		memcpy(ip_hdr + IPv6_DES_ADDRESS_OFFSET, multicast_ip_addr, IPv6_ALEN);
	} else {
		memcpy(ip_hdr + IPv6_DES_ADDRESS_OFFSET, TargetIP, IPv6_ALEN);
	}

	SET_IPHDRV6_PAYLOAD_LENGTH(ip_hdr, _htons((TotalLen - IPv6_HEADER)));
	SET_IPHDRV6_NEXT_HEADER(ip_hdr, 0x11);
	SET_IPHDRV6_HOP_LIMIT(ip_hdr, 0xFF);

	/*	3. Update skb len*/
	skb_put(skb,  ptr - skb->data);

	/* Send !! */
	wifi_if_send_skb(idx, skb);

	RTK_LOGD(TAG_WLAN_PROXY, "==> issue_MDNS_v6_rsp : no.%x - TotalLen = %x\n", issue_no, TotalLen);

exit:
	rtos_critical_enter();
	wifi_if_tx_dec(idx);
	rtos_critical_exit();
	return;
}

#endif  /* CONFIG_OFFLOAD_MSDN_V6 */

